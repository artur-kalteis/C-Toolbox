
//Erstellt von Artur Kalteis
//am 18.5.2017
//artur.kalteis@gmx.net
//C-Programm für den IgH Ethercat Master unter Linux.
//IgH Version 1.5.2(2013-02-12)

//(Flag_info) to search the missing Code which is necessary to update by the Topology-Manager (TM.sh)

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include <json-c/json.h>
#include <stdbool.h>

//addet
#include <time.h>
#include <sched.h>
#include <sys/mman.h>
#include <sys/resource.h> // f�r den Timer
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h> //for Shared-Memory
#include <sys/shm.h> //for Shared-Memory

#include "ecrt.h"

/************************************Header Created by Artur Kalteis****************************************/

#include "../include/slaves.h"          // mit dem Topology-Manager ausgelesene Topologie
#include "../include/pdo.h"             // Vom IgH-EtherCAT-Master ausgelesene PDO's
#include "../include/domain_reg.h"      // erst nachdem alle Slaves includet sind
#include "../include/read_config.h"     // erst nachdem alle Slaves includet sind
#include "../include/slave_obj.h"
#include "../include/master_obj.h"

#define datasets_X 8 // maximale Datensatz Anzahl
unsigned long measurement_timestamp[slave_X][datasets_X]; //every slave gets 6 timestamps for the beginning, later it need to be dynamic

#include "../include/write_influx.h"

int bit_array[16]={1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768}; // für das bitweise beschreiben eines 16Bit oder 8Bit Values

//Arango Parser Handling.
//Attribut namen müssen noch angepasst werden
char collection[17][32]={"app","cmd","sts","desc","external_sender","file","fct","geo","internal_receiver","internal_sender","msg","par","state","spec","external_receiver","temp","user"};

// enumeration variables for the handling in the aragodb-parser
enum attribut_list{e_application=0,e_commando,e_status,e_description,e_mesg,e_parameter,e_state};
enum attribut_list handler=5;

enum attribut_obj{e_master=0,e_slave}; // Beschreibt was eingelesen wird
enum attribut_obj obj_handler=1;


Master_Topology *m_;

Slave *s_;
Dataset *d_;
Output_Data_Array *o_d_a;
Output_CMD_Array *o_c_a;
Output_Status_Array *o_s_a;

Beckhoff_Slave_Array *b_s_a_i; // Beckhoff Slave analog in
Beckhoff_Slave_Array *b_s_a_o; // Beckhoff Slave analog out
Data_Array *d_a;
Status_Array *s_a;

 
//modecheck=1 includes printf in the cycle task
//modecheck=0 no printf in the cycle task
int modeCheck=0;  

//Variablen fürs Forken
int array_counter=0; 
int pid_array[10]; // die PID's der Kinder werden hier zwischengespeichert
int  cyclic_counter=0;
pid_t pid,pid_parent;  // für das Forken wichtig
int fork_result=0;

int x=0; // beim durchlaufen der fork for-schleife

int slave_x=0; 
#include "../include/read_arango_parser.h"
void fork_task()
{
	switch (pid=fork())
	{
		case -1:
		printf ("FORK-ERROR\n");
		break;
		case 0:
		//printf ("Child\n");
		fork_result=1;
		break;
		default:
		//printf ("Parant\n");
		fork_result=2;
		break;
	
	}	

}





//################# Preempt##############
#define NSEC_PER_SEC    (1000000000L)

#define MY_PRIORITY (1)
#define MAX_SAFE_STACK (12*1024)

void stack_prefault(void) {

        unsigned char dummy[MAX_SAFE_STACK];

        memset(dummy, 0, MAX_SAFE_STACK);
        return;
}


int status=1; // Status der Statemachine des Masters

void signal_handler(int signum) {
    switch (signum) {
	
        case SIGHUP:
        status=1;
        printf("preop-----------------\n");
		break;		
        case SIGCONT:
        status=2;
        printf("op--------------------\n");
		break;
        case SIGINT:
        status=3;
        printf("kill-----------------\n");
		break;		
	
    }
}



// EtherCAT Strukturen
static ec_master_t *master = NULL;
static ec_master_state_t master_state = {}; // ausgabe fertig

static ec_domain_t *domain1 = NULL;
static ec_domain_state_t domain1_state = {}; // ausgabe fertig

static ec_slave_config_t *sc_dig_in = NULL;
static ec_slave_config_t *sc_dig_out = NULL;

static ec_slave_config_t *sc_ana_out = NULL; // Analog neu
static ec_slave_config_t *sc_ana_in = NULL; // Analog neu

static ec_slave_config_state_t sc_dig_in_state = {}; // ausgabe fertig

static ec_slave_info_t slave_inf={};  // ausgabe da ,muss aber nicht mit ecrt_master_get_slave verbunden werden
static ec_master_link_state_t link_state={}; // ausgabe fertig
static ec_master_info_t master_info={}; // ausgabe da
static ec_sync_info_t sync_info={};// testweise

// Timer
struct timespec start;
struct timespec end;
struct timespec endtask;
struct timespec result;	
struct timespec sleeptime;

//Frequenz ins ns

struct timespec cycletime;

// process data
static uint8_t *domain1_pd = NULL;



/****************************************************************************/
// Funktion durläuft das übergebene Master-Objekt und schreibt die Lese- und Schreibbefehle in den EtherCAT-Frame
void process_master(Master_Topology *m) 
{

        //hilfsvariablen die beim auslesen eines Datensatzes benötigt werden
        int act_status=0; //zum zwischenspeichern eines Dataset-Status
        int bit_buffer=0; //zum zwischenspeichern der bits
        int data_counter=0; //zum zwischenspeichern der dataset-> data->counter
        

    int i,i2,i3,i4,send_cmd_value,aib_status;  // send_cmd_value contains all Bool cmd vor all ethercat-variables
    send_cmd_value=0;
    

    for(i=0; i< m->slave_count; i++) // Alle Slaves werden nacheinander abgearbeitet
    {
       
    if(slave_serilized[i]==0)// Beckhoff Slave erkannt
    {

    
            for(i2=0; i2< m->slave_objekt[i].beckoff_slave_ana_in_count; i2++) // I/O's des jeweiligen Slaves werden nacheinander abgearbeitet
            {
        
                 
               /* m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].value=EC_READ_S32(domain1_pd + ana_in[i][i2]);
               
                if(modeCheck==1)
                {  
                    printf("Input Value%i= %i \n",i2,m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].value);
                }

                */
                
               /* m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].value=EC_READ_S16(domain1_pd + ana_in[i][i2]);
                if(modeCheck==1)
                {  
                    printf("Input Value%i= %i \n",i2,m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].value);
                }*/

                
   
                if(!strcmp(m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].unit,"ui16"))
                {                            
                    m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].u_value=EC_READ_U16(domain1_pd + ana_in[i][i2]);
                }

                else if(!strcmp(m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].unit,"fl32"))
                {

                   // printf("Eingelesen %i ",EC_READ_S32(domain1_pd + ana_in[i][i2]));

                    *((int32_t*)&m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].f_value)=EC_READ_S32(domain1_pd + ana_in[i][i2]);

                   // printf("Reading from float = %f \n",m_->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].f_value);   

                }
         
                else if(!strcmp(m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].unit,"in16"))
                {
                    m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].value=EC_READ_S16(domain1_pd + ana_in[i][i2]);
                }

                else if(!strcmp(m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].unit,"ui32"))
                {
                    m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].value=EC_READ_U32(domain1_pd + ana_in[i][i2]);
                }



            }

            for(i2=0; i2< m->slave_objekt[i].beckoff_slave_ana_out_count; i2++) // I/O's des jeweiligen Slaves werden nacheinander abgearbeitet
            {

               /* EC_WRITE_S32(domain1_pd + ana_out[i][i2],m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].value);
                if(modeCheck==1)
                {  
                printf("Output Value%i= %i \n",i2,m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].value);
                }
                
                */

               // EC_WRITE_S32(domain1_pd + ana_out[i][i2],m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].value);

                
                if(!strcmp(m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].unit,"ui16"))
                {                            
                    EC_WRITE_U16(domain1_pd + ana_out[i][i2],m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].u_value);
                }

                else if(!strcmp(m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].unit,"fl32"))
                {
                    EC_WRITE_S32(domain1_pd + ana_out[i][i2],m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].value);

                    //printf("Writing into the Frame= %i \n",m_->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].value);

                }
         
                else if(!strcmp(m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].unit,"in16"))
                {
                    EC_WRITE_S32(domain1_pd + ana_out[i][i2],m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].value);
                }

                else if(!strcmp(m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].unit,"ui32"))
                {
                    EC_WRITE_S32(domain1_pd + ana_out[i][i2],m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].value);
                }
                




            }    
           

              
    }
    else
    { // Slave Serilized here


        send_cmd_value=0;

        if(!strcmp(slave_name[i],"AB1001"))
        {

            for(i2=0;i2<m_->slave_objekt[i].slave_output_cmd_count;i2++)
            {

            if(m_->slave_objekt[i].output_cmd->output_cmd_array[i2].value!=0)
            {
                send_cmd_value=send_cmd_value+bit_array[i2]; // Der jeweilige Bit wird gesetzt

            }
            
            }

            if(modeCheck==1)
            {
            printf("Send Slave%i Variable%i cmd_Value =%i  \n",i,m->slave_objekt[i].slave_cmd_output,send_cmd_value);
            }    
            EC_WRITE_U8(domain1_pd + ana_out[i][m->slave_objekt[i].slave_cmd_output],send_cmd_value);

            

            //READING STATUS EVERY CYCLLE
            // But storing the values only if check status =1;
            aib_status=0;
            bit_buffer=0;
            
            aib_status=EC_READ_U16(domain1_pd + ana_in[i][26]); // allways the Variable 7 from the AIB-Slave
            
            for(i3=0; i3<m->slave_objekt[i].slave_output_status_count ;i3++)
            {
                bit_buffer=aib_status >> i3;
                m->slave_objekt[i].output_status->output_status_array[i3].value=bit_buffer & 1;
             //   printf("slave %i  bit:%i Value%i  \n",i,i3,m->slave_objekt[i].output_status->output_status_array[i3].value);
            }
            
           // printf("---------------------------\n");

        } 

            
        send_cmd_value=0;
            
        //go through every dataset of this slave
        for(i2=0; i2< m->slave_objekt[i].slave_dataset_count; i2++) // I/O's des jeweiligen Slaves werden nacheinander abgearbeitet
        {
           



            send_cmd_value=send_cmd_value+bit_array[m->slave_objekt[i].d_set[i2].dataset_cmd_var_read];
             
            //read Status
            act_status=EC_READ_U8(domain1_pd + ana_in[i][m->slave_objekt[i].d_set[i2].dataset_status_var_input]);    


            bit_buffer=0;
            
            for(i3=0; i3<8 ;i3++)
            {

                bit_buffer=act_status >> i3;
                m->slave_objekt[i].d_set[i2].dataset_act_8bit_array[i3]=bit_buffer & 1;
  
            }

            if(m->slave_objekt[i].d_set[i2].dataset_act_8bit_array[7]==1) // Interrupt
            {

             

                if(modeCheck==1)
                {                        
                printf(" Slave %i Dataset %iInterrupt  \n",i,i2);
                }
                
                            
                            
                           
                                if(m->slave_objekt[i].d_set[i2].dataset_data_array_count>200) // bei >5 sind es meist harmonische
                                {
                                    EC_WRITE_U16(domain1_pd + ana_out[i][m->slave_objekt[i].d_set[i2].dataset_addr_var_out],m_->slave_objekt[i].d_set[i2].dataset_addr_send); // Write current address (adress is seperat for every dataset)
                                }
                                else
                                {
                                    EC_WRITE_U8(domain1_pd + ana_out[i][m->slave_objekt[i].d_set[i2].dataset_addr_var_out],m_->slave_objekt[i].d_set[i2].dataset_addr_send); // Write current address (adress is seperat for every dataset)
                                }
 
                             

                            m->slave_objekt[i].d_set[i2].dataset_act_addr=EC_READ_U8(domain1_pd + ana_in[i][m->slave_objekt[i].d_set[i2].dataset_addr_var_input]);

                            if(m->slave_objekt[i].d_set[i2].dataset_act_addr==0)
                            {
                                m_->slave_objekt[i].d_set[i2].dataset_act_check=0; 
                            }   
                            
                            if(m_->slave_objekt[i].d_set[i2].dataset_act_check==0) // dann addres erhöhen
                            {    
                                m_->slave_objekt[i].d_set[i2].dataset_addr_send++;
                            }   

                            if((m_->slave_objekt[i].d_set[i2].dataset_act_check==0) && (m->slave_objekt[i].d_set[i2].dataset_act_addr!=m->slave_objekt[i].d_set[i2].dataset_act_last_addr))
                            {

                                
                                    
                                if(m_->slave_objekt[i].d_set[i2].dataset_act_addr  <= m->slave_objekt[i].d_set[i2].dataset_data_array_count)   
                                {     
                                    if(modeCheck==1)
                                    {                        
                                    printf("reading data from slave%i  address %i \n",i, m_->slave_objekt[i].d_set[i2].dataset_act_addr);
                                    }
                                   

                                        if(!strcmp(m->slave_objekt[i].d_set[i2].data->d_array[m->slave_objekt[i].d_set[i2].dataset_act_addr].unit,"ui16"))
                                        {                            
                                            m->slave_objekt[i].d_set[i2].data->d_array[m->slave_objekt[i].d_set[i2].dataset_act_addr].u_value=EC_READ_U16(domain1_pd + ana_in[i][m->slave_objekt[i].d_set[i2].dataset_data_var_input]);
                                        }

                                        else if(!strcmp(m->slave_objekt[i].d_set[i2].data->d_array[m->slave_objekt[i].d_set[i2].dataset_act_addr].unit,"fl32"))
                                        {
                                            m->slave_objekt[i].d_set[i2].data->d_array[m->slave_objekt[i].d_set[i2].dataset_act_addr].f_value=EC_READ_S32(domain1_pd + ana_in[i][m->slave_objekt[i].d_set[i2].dataset_data_var_input]);                                 
                                        }
                                 
                                        else if(!strcmp(m->slave_objekt[i].d_set[i2].data->d_array[m->slave_objekt[i].d_set[i2].dataset_act_addr].unit,"in16"))
                                        {
                                            m->slave_objekt[i].d_set[i2].data->d_array[m->slave_objekt[i].d_set[i2].dataset_act_addr].value=EC_READ_S16(domain1_pd + ana_in[i][m->slave_objekt[i].d_set[i2].dataset_data_var_input]);
                                        }


                                        
                                    m_->slave_objekt[i].d_set[i2].dataset_counter++;
                                    
                    
                                }
    
                                if(m_->slave_objekt[i].d_set[i2].dataset_counter== m->slave_objekt[i].d_set[i2].dataset_data_array_count)
                                {
    
                                    if(modeCheck==1)
                                    {    
                                    printf("--------reading done---------------\n");
                                    }
                                    m_->slave_objekt[i].d_set[i2].dataset_counter=0; 
                                    //old  m_->slave_objekt[i].d_set[i2].dataset_act_addr=-1;  // zum initialisieren
                                    m_->slave_objekt[i].d_set[i2].dataset_act_addr=0;  // zum initialisieren
                                    m_->slave_objekt[i].d_set[i2].dataset_addr_send=0;  // zum initialisieren
                                    m_->slave_objekt[i].d_set[i2].dataset_act_check=1;
                                    
    
                                    measurement_timestamp[i][i2]=EC_READ_U64(domain1_pd + ana_in[i][m->slave_objekt[i].d_set[i2].dataset_time_var_input]); 
    
                                    send_cmd_value=send_cmd_value+bit_array[m->slave_objekt[i].d_set[i2].dataset_cmd_var_interrupt]; // reset everything
                                    
                                 //   printf("############################################\n");
                                    for(i4=0; i4<m->slave_objekt[i].d_set[i2].dataset_data_array_count ;i4++)
                                    {
                                        


                                        
                                        //*((int32_t*)&measurement_buffer[i][i2][i4])=m->slave_objekt[i].d_set[i2].data->d_array[i4].value;  
                                        //*((int32_t*)&m->slave_objekt[i].d_set[i2].data->d_array[i4].buffer_f)=m->slave_objekt[i].d_set[i2].data->d_array[i4].value;   

                                      //  printf("slave %i Dataset%i value=%i \n",i,i2,m->slave_objekt[i].d_set[i2].data->d_array[i4].u_value);

                                        if(!strcmp(m->slave_objekt[i].d_set[i2].data->d_array[i4].unit,"ui16"))
                                        {                            
                                            m->slave_objekt[i].d_set[i2].data->d_array[i4].buffer_ui=m->slave_objekt[i].d_set[i2].data->d_array[i4].u_value;

                                          //  printf(" Slave %i Dataset %i Value %u \n",i,i2, m->slave_objekt[i].d_set[i2].data->d_array[i4].buffer_ui);

                                        }

                                        else if(!strcmp(m->slave_objekt[i].d_set[i2].data->d_array[i4].unit,"fl32"))
                                        {
                                            *((int32_t*)&m->slave_objekt[i].d_set[i2].data->d_array[i4].buffer_f)=m->slave_objekt[i].d_set[i2].data->d_array[i4].f_value;  
                                           // printf(" Slave %i Dataset %i Value %f \n",i,i2,m->slave_objekt[i].d_set[i2].data->d_array[i4].buffer_f);                               
                                        }
                                 
                                        else if(!strcmp(m->slave_objekt[i].d_set[i2].data->d_array[i4].unit,"in16"))
                                        {
                                            m->slave_objekt[i].d_set[i2].data->d_array[i4].buffer_i=m->slave_objekt[i].d_set[i2].data->d_array[i4].value;
                                           // printf(" Slave %i Dataset %i Value %i \n",i,i2,m->slave_objekt[i].d_set[i2].data->d_array[i4].buffer_i);
                                        }

                                   

                                        if(modeCheck==1)
                                        {      
                                        printf("name %s \n",m->slave_objekt[i].d_set[i2].data->d_array[i4].name);  
                                        printf("value %f \n",m->slave_objekt[i].d_set[i2].data->d_array[i4].value);   
                                        }

                                        //Reset all Current Measurements
                                        m->slave_objekt[i].d_set[i2].data->d_array[i4].value=0;
                                        m->slave_objekt[i].d_set[i2].data->d_array[i4].u_value=0;
                                        m->slave_objekt[i].d_set[i2].data->d_array[i4].f_value=0;
                    
                                    }
                                    
                                    
                                   
                            
    
                                } 
                            }
                             
                            if(modeCheck==1)
                            {      
                            printf("Slave %i  act_Address %s \n",i,m->slave_objekt[i].d_set[i2].data->d_array[i4].name);  
                         
                            }

                            m->slave_objekt[i].d_set[i2].dataset_act_last_addr=m->slave_objekt[i].d_set[i2].dataset_act_addr;
                              
            }
            else
            {
                if(modeCheck==1)
                {                        

                //    send_cmd_value=send_cmd_value+bit_array[m->slave_objekt[i].d_set[i2].dataset_cmd_var_interrupt]; // reset everything
                printf("No Interrupt \n");
                }
                
              //  send_cmd_value=send_cmd_value+bit_array[m->slave_objekt[i].d_set[i2].dataset_cmd_var_interrupt]; // reset everything

            }   

      
            
              
        
        } //end of for(i2=0; i2< m->slave_objekt[i].slave_dataset_count; i2++)

        // at the end the cmd will be send
        if(modeCheck==1)
        {
        printf("Send Slave%i cmd_Value %i \n",i,send_cmd_value);
        }
        EC_WRITE_U16(domain1_pd + ana_out[i][m->slave_objekt[i].d_set[0].dataset_cmd_var_out],send_cmd_value);  // d_set[0] da alle sets mit der selben variable angesteuert werdern
        //EC_WRITE_U16(domain1_pd + ana_out[i][m->slave_objekt[i].d_set[0].dataset_cmd_var_out],bit_array[0]); 


    } //end of else

    } // end of go trough all slaves
    
    
    
} // end of function


/*
void test_write(Master_Topology *m)
{


    int i,i2,i3;


    for(i=0; i< m->slave_count; i++) // Alle Slaves werden nacheinander abgearbeitet
    {
       
    if(slave_serilized[i]==0)// Beckhoff Slave erkannt
    {
    }

    else
    {
        for(i2=0; i2< m->slave_objekt[i].slave_dataset_count; i2++) // I/O's des jeweiligen Slaves werden nacheinander abgearbeitet
        {

            


            for(i3=0; i3< m->slave_objekt[i].d_set[i2].dataset_data_array_count; i3++) // I/O's des jeweiligen Slaves werden nacheinander abgearbeitet
            {

                
            
            if(!strcmp(m->slave_objekt[i].d_set[i2].data->d_array[i3].unit,"ui16"))
            {                            
                printf("writing data.........ui..........\n");
                m->slave_objekt[i].d_set[i2].data->d_array[i3].buffer_ui=-33;    
            }

            else if(!strcmp(m->slave_objekt[i].d_set[i2].data->d_array[i3].unit,"fl32"))
            {
                printf("writing data.........f..........\n");
                m->slave_objekt[i].d_set[i2].data->d_array[i3].buffer_f=22.11;                                 
            }
     
            else if(!strcmp(m->slave_objekt[i].d_set[i2].data->d_array[i3].unit,"in16"))
            {
                
                printf("writing data.........i..........\n");
                m->slave_objekt[i].d_set[i2].data->d_array[i3].buffer_i=444;
            }

            }
        }    
    }

    }


}*/



void cyclic_task()
{
    cyclic_counter++; // für das forken werden die Zyklen gezählt


	clock_gettime(CLOCK_REALTIME  ,&start);


   //receive process data
   ecrt_master_receive(master); //empfangen von datagramen
   ecrt_domain_process(domain1);//
	

    process_master(m_);


    // send process data
    ecrt_domain_queue(domain1);
    ecrt_master_send(master);
	
	
////////////////////Messung der Etehrcat Funktionen
			
			clock_gettime(CLOCK_REALTIME  ,&endtask);		

				
				
			if((endtask.tv_sec < start.tv_sec)||(endtask.tv_sec==start.tv_sec)&&(endtask.tv_nsec<=start.tv_nsec))	
			{
				 result.tv_sec=  result.tv_nsec =0;
			}
				
			result.tv_sec= endtask.tv_sec  - start.tv_sec;
			result.tv_nsec= endtask.tv_nsec  - start.tv_nsec;	
							
			if(result.tv_nsec<0)
			{
			
			result.tv_sec--;
			
			result.tv_nsec= NSEC_PER_SEC + result.tv_nsec;
			
			
            }	
            
            if((cycletime.tv_nsec -result.tv_nsec)<=0)
			{
				
				printf("Zeit wurde ueberschritten \n");
			
			}    
            
  
				
			sleeptime.tv_sec=0;
			sleeptime.tv_nsec=cycletime.tv_nsec -result.tv_nsec - 5000; //5000 f�r die Verzugszeit des ganzen tasks
		
			
			clock_nanosleep(CLOCK_REALTIME , 0, &sleeptime, NULL);
		


///////////////////messung des ganzen Tasks
		
		
		    clock_gettime(CLOCK_REALTIME  ,&end);		
								
			if((end.tv_sec< start.tv_sec)||(end.tv_sec==start.tv_sec)&&(end.tv_nsec<=start.tv_nsec))	
			{
				 result.tv_sec=  result.tv_nsec =0;
			}
				
			result.tv_sec= end.tv_sec  - start.tv_sec;
			result.tv_nsec= end.tv_nsec  - start.tv_nsec;	
							
			if(result.tv_nsec<0)
			{
			
			result.tv_sec--;
			
			result.tv_nsec= NSEC_PER_SEC + result.tv_nsec;
			
			
			}			
            
            //printf("Zyklusdauer %9ld \n",result.tv_nsec);
			
          
           
	
}


//int main(int argc, char* argv[])
int main(int argc, char* argv[])
{
   



        //////////////////////////////////////////////Preempt///////////////////////////////////
	
	struct sched_param param;
    param.sched_priority = MY_PRIORITY;		//Priorität wird gesetzt
       if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
               perror("sched_setscheduler failed");
               exit(-1);
       }
       
       else{printf("Realtime-Betrieb aktiviert\n");}

       // Lock memory 

       if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
               perror("mlockall failed");
               exit(-2);
       }

       // Pre-fault our stack 

       stack_prefault();




    read_config();

        char login[64];
        sprintf(login,"%s:%s",arangodb_user,arangodb_password); 

         // printf("Hier-------------->%s\n",login);

          char URL[256];
          char server[64];
          
          
          sprintf(server, "http://%s/_db/",arangodb_ip);
          char collection_a[64]="/_api/cursor/";
  
          
          sprintf(URL,"");
          strcat(URL,server); 
          strcat(URL,arangodb_database); 
          strcat(URL,collection_a); 

          //printf("Hier-------------->%s\n",URL);



    ec_slave_config_t *sc;


    m_=(Master_Topology*) malloc(sizeof(Master_Topology));

    s_=(Slave*) malloc(sizeof(Slave));
    d_=(Dataset*) malloc(sizeof(Dataset));

    m_->slave_count=slave_count;
    init_Master_Topology(m_,m_->slave_count);

    
    for(slave_x=0;slave_x< slave_count;slave_x++)
    {

        s_->slave_dataset_count=slave_datasets[slave_x]; // Anzahl der Datasets wird dan das Slaveobjekt übergeben 




          
        init_Slave(s_,s_->slave_dataset_count);

       if(slave_serilized[slave_x]==1)//
       {

        printf("----------------------------parameter-------------------------------\n");
        handler=5;
        read_arango_curl(collection[11],slave_id[slave_x],URL,login);  // 11= Parameter , 12=state

        
        }

        if(!strcmp(slave_name[slave_x],"EL6692")) // nur für die Bridge notwendig
        {
            
            
            printf("----------------------------parameter-------------------------------\n");
            handler=5;
            read_arango_curl(collection[11],slave_id[slave_x],URL,login);  // 11= Parameter , 12=state


        }    

        printf("---------------------------state-------------------------------\n");
        handler=6;
        read_arango_curl(collection[12],slave_id[slave_x],URL,login);  // 11= Parameter , 12=state

        if(!strcmp(slave_name[slave_x],"AB1001"))
        {                    
           
        
        printf("---------------------------CMD-------------------------------\n");         
            
        handler=1;
        read_arango_curl(collection[1],slave_id[slave_x],URL,login);  // cmd    

        printf("---------------------------status-------------------------------\n");          

        handler=2;
        read_arango_curl(collection[2],slave_id[slave_x],URL,login);  // Status

       
        }
        else
        {
            s_->slave_output_status_count=0;
            s_->slave_output_cmd_count=0;

        }

       


        printf("---------------------------save slave-------------------------------\n");      
          
        insert_Master_Topology(m_,s_);


      

      
    }

    int i_for=0;
    int i2_for=0;

    printf("-------------MAIN-input-Beckhoff-Slave-%i \n",m_->slave_objekt[i_for].beckoff_slave_ana_in_count);
    for(i2_for=0; i2_for< m_->slave_objekt[i_for].beckoff_slave_ana_in_count; i2_for++)
    {
        printf("name %s\n", m_->slave_objekt[i_for].beckhoff_data_in->beckhoff_s_array[i2_for].name);
        printf("unit %s\n", m_->slave_objekt[i_for].beckhoff_data_in->beckhoff_s_array[i2_for].unit);
       // printf("%i\n", m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].value);        
          
    }
   


output_master_obj(m_);
initialize_influx(m_); // to initialize with default values

  
    /***************************Shared-Memory*****************************/

    
    //Schlüssel und Memory wird reserviert, auch wenn kein Memory für den jeweiligen Slave benötigt wird
    int me_id[slave_count];
    key_t key[slave_count];
    int *shm[slave_count]; // ist ein Zeigerarray d.h. shm[REGISTER_MESS][Array-Position]
    int keystart=9876; // ab hier startet der KEY
   
    int ix1=0; // shared Memory
    int ix2=0; // shared Memory
    
    
    for(ix1=0;ix1<slave_count;ix1++)
    {
       
    //(Flag_info) für JM muss eine eigene seperate Bedinung eingeführt werden    
    if(slave_serilized[ix1]==0)// Beckhoff Slave erkannt
    {   
        
        


    key[ix1]=keystart+ix1;
    
    
    me_id[ix1] = shmget(key[ix1], m_->slave_objekt[ix1].beckoff_slave_ana_out_count*sizeof(int),IPC_CREAT | 0666); 
    
    if(me_id[ix1] == -1)
    {
        perror("Failed to shmget basic_mess master\n");
        exit(1);
    
    }
    
    shm[ix1]= shmat(me_id[ix1], NULL , 0);
    
    
    if(shm[ix1]==(int *)-1)
    {
        perror("Failed to shmat \n");
        exit(1);
    
    }
     

    } // end of if(slave_serilized[i]==0) 
    else
    {
        if(!strcmp(slave_name[ix1],"AB1001"))
        {
                printf("Slave %i Shared Memory Block ---------------------------------------------------\n",ix1);

            key[ix1]=keystart+ix1;
            
            
            me_id[ix1] = shmget(key[ix1], m_->slave_objekt[ix1].slave_output_cmd_count*sizeof(int),IPC_CREAT | 0666); 
            
            if(me_id[ix1] == -1)
            {
                perror("Failed to shmget basic_mess \n");
                exit(1);
            
            }
            
            shm[ix1]= shmat(me_id[ix1], NULL , 0);
            
            
            if(shm[ix1]==(int *)-1)
            {
                perror("Failed to shmat \n");
                exit(1);
            
            }

            
        }
        

    }



    
    }
    
    
    

    ///////////////////////////Prepare for first start
    int a,b;

    for(a=0;a< slave_count;a++)
    {
        for(b=0; b< m_->slave_objekt[a].slave_dataset_count; b++)
        {

            m_->slave_objekt[a].d_set[b].dataset_act_addr=0;  // zum initialisieren
            m_->slave_objekt[a].d_set[b].dataset_addr_send=0; 
            m_->slave_objekt[a].d_set[b].dataset_act_last_addr=0;
           // m_->slave_objekt[a].d_set[b].dataset_act_check=0; // zum initialisieren

        }

    }





	
/////////////////////////////////////Signalstruktur////////////////////////////////////

 struct sigaction sa; 	//Signal Struktur

	
	sa.sa_handler = signal_handler; // der sigaction Struktur wird ein hander hinzugef�gt
    sigemptyset(&sa.sa_mask); 
    sa.sa_flags = 0; // flags sind Signal Optionen die eingestellt werden k�nnen
	
    
	if (sigaction(SIGINT, &sa, 0)) { // 
        fprintf(stderr, "Failed to install SIGINT to signal handler!\n");
        return -1;
    }
	
	
	 if (sigaction(SIGHUP, &sa, 0)) { // pausiert den Prozess
        fprintf(stderr, "Failed to install SIGSTOP to signal handler!\n");
        return -1;
    }
	
	
	if (sigaction(SIGCONT, &sa, 0)) { // fortf�hren des Prozesses
        fprintf(stderr, "Failed to install SIGCOUNT to signal handler!\n");
        return -1;
    }
    else
    {
        printf("Signal wurde geladen \n");

    }




    while (1) {
		   
	switch (status)
	 {
        case 1:
		//do
		
		while(status==1)
		{
		printf("Master PREOP.....\n");
		sleep(1);
		}
        break;
			
		case 2:

            printf("Master wird gestartet \n");

master = ecrt_request_master(0); // bei einen wiederholten Aufruf entsteht ein Fehler
    if (!master)
       {  printf("master ERROR \n");

return -1; 
}
else
{
printf("Master called \n");
}

domain1 = ecrt_master_create_domain(master);
    if (!domain1)
        
{
printf("Domain ERROR \n");
return -1;
}
else
{
printf("Domain called \n");
}

FLAG_XY
		
    if (ecrt_domain_reg_pdo_entry_list(domain1, domain1_regs)) {
        fprintf(stderr, "PDO entry registration failed!\n");
        return -1;
    }


 printf("Master aktiv\n");
	


 	

    if (ecrt_master_activate(master))
        return -1;

    if (!(domain1_pd = ecrt_domain_data(domain1))) {
        return -1;
    }
 /////////////////////Ethercat Ende///////////////////////////////////
        
		printf("Master OP........\n");
		while(status==2)
		{
            
            
            if(cyclic_counter==2000)
            {
                cyclic_counter=0;
        
                if(array_counter==10)
                {
        
                    printf("---------------------Alle PID's werden gelöscht---------------\n");
                    //printf("free all childs \n");	
                    for(x=0;x<10;x++)
                    {
                        
                        waitpid(pid_array[x], NULL, 0);
                    }
                    
                    array_counter=0;
                }	
                
                    
                fork_task();
                
        
                switch (fork_result)
                {
                    case 1:
                    
                    printf("---------------------master-Child erstellt--------------------\n");


                      

                    write_influx_init(m_); 
                   

                     return 0;
        
                    break;
                    case 2:
                    //printf("master-Parent erstellt\n");
                    pid_array[array_counter]=pid; // PID wird im Array zwischengespeichert
                    //printf("kind-pid ====================%i\n",pid_array[array_counter]);
                    
                    array_counter++;
                    //pid_parent=getpid();
                    //printf ("Parant still Running\n");
                    //printf ("Parant-PID=%i \n",pid_parent);
                    //printf ("Parant-PID=%i \n",array_counter);

                     //nach dem form müssen alle status resetet werden
                        for(ix1=0;ix1<slave_count;ix1++)
                        {
                        if(m_->slave_objekt[ix1].check_status==1) 
                        {
                            m_->slave_objekt[ix1].check_status=0;

                        }
                        }
        
                    
                    break;
        
        
        
                }	
                
               

            }
                
        //die frequenz wird vor jedem Zyklus neu ausgelesen
        




            for(ix1=0;ix1<slave_count;ix1++)
            {
            
            //(Flag_info) für JM muss eine eigene seperate Bedinung eingeführt werden    
            if(slave_serilized[ix1]==0)// Beckhoff Slave erkannt
            {   

                for(ix2=0;ix2<m_->slave_objekt[ix1].beckoff_slave_ana_out_count;ix2++)
                {
                
                //shm[slave_counter][output_counter]  
                m_->slave_objekt[ix1].beckhoff_data_out->beckhoff_s_array[ix2].value=shm[ix1][ix2];   


                //*((int32_t*)&m_->slave_objekt[ix1].beckhoff_data_out->beckhoff_s_array[ix2].f_value)=m_->slave_objekt[ix1].beckhoff_data_out->beckhoff_s_array[ix2].value;

               // printf("Reading from Shared Memory= %i    floating %f \n",shm[ix1][ix2],m_->slave_objekt[ix1].beckhoff_data_in->beckhoff_s_array[ix2].f_value);
                
                }

            

            }
            else
            {
                if(!strcmp(slave_name[ix1],"AB1001"))
                {

                    for(ix2=0;ix2<m_->slave_objekt[ix1].slave_output_cmd_count;ix2++)
                    {

                        if(m_->slave_objekt[ix1].output_cmd->output_cmd_array[ix2].value!=shm[ix1][ix2])
                        {
                          m_->slave_objekt[ix1].check_status=1;

                          printf("CHANGE HEREE-------------------------\n");
                        } 

                       

                    m_->slave_objekt[ix1].output_cmd->output_cmd_array[ix2].value=shm[ix1][ix2];   
                    
                    }

                }    
            }


            }



		cycletime.tv_sec=0;
		cycletime.tv_nsec=500000;
		//printf("Periodendauer %9ld",cycletime.tv_nsec);
		cyclic_task();
		
		
		}
        
      


ecrt_master_deactivate(master);		
ecrt_release_master(master);	
        break;
		
		
		case 3:
		printf("Stop...\n");

 
        free(m_);

    
        free(s_);
    
        free(d_);

        free(o_d_a);

        free(o_c_a);

        free(o_s_a);

        free(b_s_a_i);
        
        free(b_s_a_o);
        
        free(d_a);
        
        free(s_a);
        

			
		return 0;
		break;
     
		
		default:
        printf("ERROR\n");
        break;


        //L�schen der Shared-Memory Bl�cke
		 for(ix1=0;ix1<slave_count;ix1++)
         {
         shmdt(shm[ix1]); //Zum loslosen des Speichers vom Prozess  (speicher wird dabei nicht gel�scht)
         
         shmctl(me_id[ix1],IPC_RMID,NULL); //L�scht den Speicher
         }   
	
	

            }
        }


    return 0;
}


