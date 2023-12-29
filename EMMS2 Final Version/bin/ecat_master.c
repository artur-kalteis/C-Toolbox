
//Erstellt von Artur Kalteis
//am 18.5.2017
//artur.kalteis@gmx.net
//C-Programm f�r den IgH Ethercat Master unter Linux.
//IgH Version 1.5.2(2013-02-12)
//Das Programm zeigt den Aufbau f�r die Ansteuerung des JMM01 Messboards mit Shared-Memory

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


/****************************************************************************/

#include "ecrt.h"

#include "../include/slaves.h" // mit dem Watchdog ausgelesene Topologie
#include "../include/pdo.h"
#include "../include/domain_reg.h" // erst nachdem alle Slaves includet sind
#include "../include/read_config.h" // erst nachdem alle Slaves includet sind
#include "../include/slave_obj.h"

#include "../include/master_obj.h"
#define slave_X 1 // Slave Anzahl
#define datasets_X 8 // maximale Datensatz Anzahl
#define datasets_size 256 // maximale Datensatz Anzahl

unsigned long measurement_timestamp[slave_X][datasets_X]; //every slave gets 6 timestamps for the beginning, later it need to be dynamic

int measurement_buffer[slave_X][datasets_X][datasets_size];

#include "../include/write_influx.h"



int bit_array[16]={1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768}; // für das bitweise beschreiben eines 16Bit Values

//Arango Parser Handling
char collection[17][32]={"application","commando","status","description","external_sender","file","function","geography","internal_receiver","internal_sender","message","parameter","state","specification","external_receiver","temporary","user"};

enum attribut_list{e_application=0,e_commando,e_status,e_description,e_mesg,e_parameter,e_state};
enum attribut_list handler=5;

enum attribut_obj{e_master=0,e_slave}; // Beschreibt was eingelesen wird
enum attribut_obj obj_handler=1;


//Master/slave/dataset Objekts
//struct cmd_master_obj *cmd;
//struct sts_master_obj *sts;

Master_Topology *m_;
//Master_Topology *m_buffer; // für das zwischenspeichern der WERTE
Slave *s_;
//Slave *s_buffer;
Dataset *d_;
Output_Data_Array *o_d_a;
Output_CMD_Array *o_c_a;
Beckhoff_Slave_Array *b_s_a; // if Beckhoff Slave
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
#define MAX_SAFE_STACK (8*1024)

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
        printf("preop------------dooo1--------\n");
		break;		
        case SIGCONT:
        status=2;
        printf("op--------------dooo2------\n");
		break;
        case SIGINT:
        status=3;
        printf("kill------------dooo3--------\n");
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





/*****************************************************************************/
void check_domain1_state(void)
{
//	printf("domain1 state check\n");

    ec_domain_state_t ds; // leere struktur erstellt

    ecrt_domain_state(domain1, &ds);	//speichert die domain state in die gegebene domain state struktur

    if (ds.working_counter != domain1_state.working_counter) //
 //       printf("Domain1: WC %u.\n", ds.working_counter);
//	else{printf("domain ist gleich, keine aenderung \n");}

    if (ds.wc_state != domain1_state.wc_state)
 //       printf("Domain1: State %u.\n", ds.wc_state);
//	else{printf("domain ist gleich, keine aenderung \n");}


    domain1_state = ds;
}

/*****************************************************************************//*********notwendig f�r lesen vom Master Status****************/
// erst nach dem check kann man die werte ausgeben lassen
void check_master_state(void)
{
	//printf("master state check \n");
    ec_master_state_t ms;

    ecrt_master_state(master, &ms);

    if (ms.slaves_responding != master_state.slaves_responding)
 //       printf("%u slave(s).\n", ms.slaves_responding);
//	else{printf("master ist gleich, keine aenderung \n");}
    if (ms.al_states != master_state.al_states)
 //       printf("AL states: 0x%02X.\n", ms.al_states);
//	else{printf("master ist gleich, keine aenderung \n");}
    if (ms.link_up != master_state.link_up)
  //      printf("Link is %s.\n", ms.link_up ? "up" : "down");
//	else{printf("master ist gleich, keine aenderung \n");}

    master_state = ms;
}


/*****************************************************************************/

void check_slave_config_states_in(void)
{
//	printf("slave config state check\n");
    ec_slave_config_state_t s;

	//ecrt_slave_config_state(sc_dig_in, &s); // f�r die analog eing�nge
    ecrt_slave_config_state(sc_dig_in, &s);

    if (s.al_state != sc_dig_in_state.al_state)
//        printf("DigIn: State 0x%02X.\n", s.al_state);
//	else{printf("slave ist gleich, keine aenderung \n");}

    if (s.online != sc_dig_in_state.online)
 //      printf("DigIn: %s.\n", s.online ? "online" : "offline");
//	else{printf("slave ist gleich, keine aenderung \n");}

    if (s.operational != sc_dig_in_state.operational)
 //       printf("DigIn: %soperational.\n",
   //             s.operational ? "" : "Not ");
//	else{printf("slave ist gleich, keine aenderung \n");}


    sc_dig_in_state = s;
}


/****************************************************************************/

void process_master(Master_Topology *m)
{

        //hilfsvariablen die beim auslesen eines Datensatzes benötigt werden
        int act_status=0; //zum zwischenspeichern eines Dataset-Status
        int bit_buffer=0; //zum zwischenspeichern der bits
        int data_counter=0; //zum zwischenspeichern der dataset-> data->counter
      

    int i,i2,i3,i4,send_cmd_value;  // send_cmd_value contains all Bool cmd vor all ethercat-variables
    send_cmd_value=0;
    

    for(i=0; i< m->slave_count; i++) // Alle Slaves werden nacheinander abgearbeitet
    {
        // Slave not  Serilized here
    if(slave_serilized[i]==0)// Beckhoff Slave erkannt
    {


        for(i2=0; i2< m->slave_objekt[i].beckhoff_slave_count; i2++) // I/O's des jeweiligen Slaves werden nacheinander abgearbeitet
        {
        

                if(ana_in_count[i]>0) //Analog Output erkannt
                {
                    m->slave_objekt[i].beckhoff_data->beckhoff_s_array[i2].value=EC_READ_U16(domain1_pd + ana_in[i][i2]);
                    printf("Input Value%i= %i \n",i2,m->slave_objekt[i].beckhoff_data->beckhoff_s_array[i2].value);
                }
                if(ana_out_count[i]>0)//Analog Input erkannt
                {
                    EC_WRITE_U16(domain1_pd + ana_out[i][i2],m->slave_objekt[i].beckhoff_data->beckhoff_s_array[i2].value);
                    printf("wrinting data\n");
                }
                /*
                if(digital_output)//Digital Output erkannt
                {
    
                }
                if(digital_input)//Digital Input erkannt
                {
                    
                }*/
        }   

            
      
              
    }
    else
    { // Slave Serilized here
            
            /*
            Write Output data of this slave
            */
 
            
        //go through every dataset of this slave
        for(i2=0; i2< m->slave_objekt[i].slave_dataset_count; i2++) // I/O's des jeweiligen Slaves werden nacheinander abgearbeitet
        {
           // printf("DATASET NUMBER %i \n",i2);
           // if( m->slave_objekt[i].d_set[i2].dataset_act_check==0) // this check is =1 if this dataset is already read out
           // {
  //EC_WRITE_U16(domain1_pd + ana_out[i][m->slave_objekt[i].d_set[i2].dataset_cmd_var_out],bit_array[m->slave_objekt[i].d_set[i2].dataset_cmd_var_interrupt]);
            //sending address 
           // printf("-------------------------------------------------------------------------------\n");  

             //dient nur als zwischenbuffer
             data_counter=m->slave_objekt[i].d_set[i2].dataset_data_array_count;


            if(data_counter>50)
            {
            
                EC_WRITE_U16(domain1_pd + ana_out[i][m->slave_objekt[i].d_set[i2].dataset_addr_var_out],m_->slave_objekt[i].d_set[i2].dataset_addr_send); // Write current address (adress is seperat for every dataset)
            }
           else
           {
                EC_WRITE_U8(domain1_pd + ana_out[i][m->slave_objekt[i].d_set[i2].dataset_addr_var_out],m_->slave_objekt[i].d_set[i2].dataset_addr_send); // Write current address (adress is seperat for every dataset)
           }

            if(modeCheck==1)
            {
                printf("CHECK STATUS = %i \n",m->slave_objekt[i].d_set[i2].dataset_act_check);
            }
           

           send_cmd_value=send_cmd_value+bit_array[m->slave_objekt[i].d_set[i2].dataset_cmd_var_read];
            // send output data with two bits

             // Write current address
            //EC_WRITE_U16(domain1_pd + ana_out[i][m->slave_objekt[i].d_set[i2].dataset_cmd_var_out],bit_array[m->slave_objekt[i].d_set[i2].dataset_cmd_var_read]+bit_array[m->slave_objekt[i].d_set[i2].dataset_cmd_var_interrupt]); // Write current address

     
            //EC_WRITE_U16(domain1_pd + ana_out[i][m->slave_objekt[i].d_set[i2].dataset_cmd_var_out],bit_array[m->slave_objekt[i].d_set[i2].dataset_cmd_var_interrupt]);
        
            
            //read Status
            act_status=EC_READ_U8(domain1_pd + ana_in[i][m->slave_objekt[i].d_set[i2].dataset_status_var_input]);    
            
            for(i3=0; i3<8 ;i3++)
            {

                bit_buffer=act_status >> i3;
                m->slave_objekt[i].d_set[i2].dataset_act_8bit_array[i3]=bit_buffer & 1;

                //if(i2==0)
               // {
                  //  printf("Array %i  = %i \n",i3,m->slave_objekt[i].d_set[i2].dataset_act_8bit_array[i3]);    

               // }
     
               // printf("Array %i  = %i \n",i3,m->slave_objekt[i].d_set[i2].dataset_act_8bit_array[i3]);    
            }
            
            // Read Out all information
             if((m->slave_objekt[i].d_set[i2].dataset_act_check==0) && (m->slave_objekt[i].d_set[i2].dataset_act_8bit_array[7]==0)) // this check is =1 if this dataset is already read out and no interrupt
              {

               
                measurement_timestamp[i][i2]=EC_READ_U64(domain1_pd + ana_in[i][m->slave_objekt[i].d_set[i2].dataset_time_var_input]); 
                
            
                //read Address
                m->slave_objekt[i].d_set[i2].dataset_act_addr=EC_READ_U8(domain1_pd + ana_in[i][m->slave_objekt[i].d_set[i2].dataset_addr_var_input]);  

             //   printf("Empfangene Adresse  = %i \n", m->slave_objekt[i].d_set[i2].dataset_act_addr);    

             if(modeCheck==1)
             {
                printf("----------------------Readout Address %i ----------------------\n",m_->slave_objekt[i].d_set[i2].dataset_act_addr);
             }   
                if(m_->slave_objekt[i].d_set[i2].dataset_act_addr  <= data_counter)  // +2 to consider the past cycles    
                {                           
    
                    if(m->slave_objekt[i].d_set[i2].dataset_act_addr!=m->slave_objekt[i].d_set[i2].dataset_act_last_addr) 
                    {
        
                        m->slave_objekt[i].d_set[i2].dataset_act_last_addr=m->slave_objekt[i].d_set[i2].dataset_act_addr; // last_address=act_address
        
                        m->slave_objekt[i].d_set[i2].data->d_array[m->slave_objekt[i].d_set[i2].dataset_act_addr].value=EC_READ_U16(domain1_pd + ana_in[i][m->slave_objekt[i].d_set[i2].dataset_data_var_input]);
        
                    } 
    
                }
                // reset wird gesetzt sobald die maximale anzahl der Adresse(ausgelesene Adresse) des jeweiligen Slaves erreicht wurde
               /* if(m_->slave_objekt[i].d_set[i2].dataset_act_addr  == data_counter)
                {


                    if(m->slave_objekt[i].d_set[i2].dataset_act_addr!=m->slave_objekt[i].d_set[i2].dataset_act_last_addr) 
                    {
        
                      //  printf("Messwert wird gespeichert mit der Adresse %i \n", m->slave_objekt[i].d_set[i2].dataset_act_addr);
                        m->slave_objekt[i].d_set[i2].dataset_act_last_addr=m->slave_objekt[i].d_set[i2].dataset_act_addr; // last_address=act_address

        
                        m->slave_objekt[i].d_set[i2].data->d_array[m->slave_objekt[i].d_set[i2].dataset_act_addr].value=EC_READ_U16(domain1_pd + ana_in[i][m->slave_objekt[i].d_set[i2].dataset_data_var_input]);
        
                    } 

                   // EC_WRITE_U16(domain1_pd + ana_out[i][m->slave_objekt[i].d_set[i2].dataset_cmd_var_out],bit_array[m->slave_objekt[i].d_set[i2].dataset_cmd_var_interrupt]);
                    //m_->slave_objekt[i].d_set[i2].dataset_act_check=1;

                    printf("----------------------Check----------------------\n");
                  //  sleep(10);
                }
                */

                m_->slave_objekt[i].d_set[i2].dataset_addr_send++;    

                if( m_->slave_objekt[i].d_set[i2].dataset_addr_send > (data_counter+2))
                {
                    if(modeCheck==1)
                    {
                    printf("----------------------Address to high----------------------\n");
                    }
                    m_->slave_objekt[i].d_set[i2].dataset_act_check=1; // readout af all variables is done !
                    m_->slave_objekt[i].d_set[i2].dataset_act_addr=0;  // zum initialisieren
                    m_->slave_objekt[i].d_set[i2].dataset_addr_send=0;  // zum initialisieren
                }
               
                
              }


              // Interrupt + clear all Values
              if(m->slave_objekt[i].d_set[i2].dataset_act_8bit_array[7]==1) 
              {

                if(modeCheck==1)
                {
                printf("----------------------Interrupt  NOW----------------------\n");
                }
                send_cmd_value=send_cmd_value+bit_array[m->slave_objekt[i].d_set[i2].dataset_cmd_var_interrupt]; // reset everything


                if(m_->slave_objekt[i].d_set[i2].dataset_act_check==1) // ensure that this is called only once
                {

                    //kopieren der Master struktur in den Buffer
                    //Allgemeine Funktion 
                    //struct mystruct a,b;
                    //memcpy((void*)&a,(void*)&b,sizeof(struct mystruct));
                   // m_buffer->slave_objekt[i].d_set[i2]=m->slave_objekt[i].d_set[i2];
                    //memcpy((void *)&RTCclk, (void *)&RTCclkBuffert, sizeof RTCclk);
                   // memcpy((void*)&m_buffer,(void*)&m,sizeof(Master_Topology));

                for(i4=0; i4<m->slave_objekt[i].d_set[i2].dataset_data_array_count ;i4++)
                {
                    if(modeCheck==1)
                    {      
                    printf("name %s \n",m->slave_objekt[i].d_set[i2].data->d_array[i4].name);  
                    // ??? überhabe value für value ?
                    printf("value %i \n",m->slave_objekt[i].d_set[i2].data->d_array[i4].value);   
                    }
                    measurement_buffer[i][i2][i4]=m->slave_objekt[i].d_set[i2].data->d_array[i4].value;   
                m->slave_objekt[i].d_set[i2].data->d_array[i4].value=0;

                }   


                /*for(i4=0; i4<m->slave_objekt[i].d_set[i2].dataset_data_array_count ;i4++)
                {
                          
                    printf("name %s \n",m->slave_objekt[i].d_set[i2].data->d_array[i4].name);  
                    // ??? überhabe value für value ?
                    printf("value %i \n",measurement_buffer[i][i2][i4]);   
                    
               

                }*/

               // printf("--------------------------------Timestamp %ld-------------------------------",measurement_timestamp[i][i2]);
                }


                m_->slave_objekt[i].d_set[i2].dataset_act_addr=0;  // zum initialisieren
                m_->slave_objekt[i].d_set[i2].dataset_addr_send=0;  // zum initialisieren
                m_->slave_objekt[i].d_set[i2].dataset_act_check=0; // zum initialisieren
              } 
            
                //Readout done
                //values should be keept as long as possible
                if((m->slave_objekt[i].d_set[i2].dataset_act_8bit_array[7]==0) && (m_->slave_objekt[i].d_set[i2].dataset_act_check==1))// this check is =1 if this dataset is already read out and no interrupt
                {
                    if(modeCheck==1)
                    {     
                   printf("----------------------Readout Done!!!-%i----------------------\n",i2);
                    }   
              //    m_->slave_objekt[i].d_set[i2].dataset_act_addr=0;  // zum initialisieren
             //  m_->slave_objekt[i].d_set[i2].dataset_addr_send=0;  // zum initialisieren
                  

                  //send_cmd_value=send_cmd_value+bit_array[m->slave_objekt[i].d_set[i2].dataset_cmd_var_interrupt];
                  //EC_WRITE_U16(domain1_pd + ana_out[i][m->slave_objekt[i].d_set[i2].dataset_cmd_var_out],bit_array[m->slave_objekt[i].d_set[i2].dataset_cmd_var_interrupt]);
                 
                  
                 }
            
              
        
        } //end of for(i2=0; i2< m->slave_objekt[i].slave_dataset_count; i2++)

        // at the end the cmd will be send
       // printf("Send Value %i \n",send_cmd_value);
        EC_WRITE_U16(domain1_pd + ana_out[i][m->slave_objekt[i].d_set[0].dataset_cmd_var_out],send_cmd_value); 
        //EC_WRITE_U16(domain1_pd + ana_out[i][m->slave_objekt[i].d_set[0].dataset_cmd_var_out],bit_array[0]); 


    } //end of else

    } // end of go trough all slaves
    
    
    
} // end of function



void cyclic_task()
{
    cyclic_counter++; // für das forken werden die Zyklen gezählt


	clock_gettime(CLOCK_REALTIME  ,&start);


   //  receive process data
   ecrt_master_receive(master); //empfangen von datagramen
   ecrt_domain_process(domain1);//
	

    //check process data state (optional)
    check_domain1_state();
    // check for master state (optional)
    check_master_state();
	

       
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
            
            printf("Zyklusdauer %9ld \n",result.tv_nsec);
			
          
           
	
}


//int main(int argc, char* argv[])
int main(int argc, char* argv[])
{
   
        //////////////////////////////////////////////Preempt///////////////////////////////////
	
	struct sched_param param;
    param.sched_priority = MY_PRIORITY;		//Priorit�t wird gesetzt
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
          char uebergabe[64];
          
          sprintf(server, "http://%s/_db/",arangodb_ip);
          char collection_a[64]="/_api/document/";
  
          
          sprintf(URL,"");
          strcat(URL,server); 
          strcat(URL,arangodb_database); 
          strcat(URL,collection_a); 

          //printf("Hier-------------->%s\n",URL);



    ec_slave_config_t *sc;



    m_=(Master_Topology*) malloc(sizeof(Master_Topology));
   // m_buffer=(Master_Topology*) malloc(sizeof(Master_Topology));
    s_=(Slave*) malloc(sizeof(Slave));
   // s_buffer=(Slave*) malloc(sizeof(Slave));
    d_=(Dataset*) malloc(sizeof(Dataset));

    //cmd=(struct cmd_master_obj*) malloc(sizeof(struct cmd_master_obj));
    //sts=(struct sts_master_obj*) malloc(sizeof(struct sts_master_obj));


    //m_-> slave_datasets
    m_->slave_count=slave_count;
    init_Master_Topology(m_,m_->slave_count);
   // m_buffer->slave_count=slave_count;
   // init_Master_Topology(m_buffer,m_->slave_count);

    
    for(slave_x=0;slave_x< slave_count;slave_x++)
    {

        s_->slave_dataset_count=slave_datasets[slave_x]; // Anzahl der Datasets wird dan das Slaveobjekt übergeben 
       // s_buffer->slave_dataset_count=slave_datasets[slave_x]; // Anzahl der Datasets wird dan das Slaveobjekt übergeben 
        
        init_Slave(s_,s_->slave_dataset_count);
       // init_Slave(s_buffer,s_buffer->slave_dataset_count);
        printf("----------------------------parameter-------------------------------\n");
        handler=5;
        read_arango_curl(collection[11],slave_id[slave_x],URL,login);  // 11= Parameter , 12=state
        printf("---------------------------state-------------------------------\n");
        handler=6;
        read_arango_curl(collection[12],slave_id[slave_x],URL,login);  // 11= Parameter , 12=state
    
     //   s_buffer=s_;    
        insert_Master_Topology(m_,s_);
       // insert_Master_Topology(m_buffer,s_buffer);
       // free_Slave(s_);
   
    }

   
    //printf("---------------------------------------------------------- \n");

    output_master_obj(m_);


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

	//PAR_shm[0][2]=status;

   
		   
		   //system("clear");
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

if (!(sc_ana_in= ecrt_master_slave_config(master,SlavePos1, JM1001 ))) {return -1;} if(ecrt_slave_config_pdos(sc_ana_in, EC_END, slave_0_syncs)) {return -1;}
		
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
            

            if(cyclic_counter==1000)
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
        
                    
                    break;
        
        
        
                }	
                
                
            }

		//die frequenz wird vor jedem Zyklus neu ausgelesen
		cycletime.tv_sec=0;
		cycletime.tv_nsec=450000;
		//printf("Periodendauer %9ld",cycletime.tv_nsec);
		cyclic_task();
		//printf("HHHHHHHHHHHHHllllllllllloooooooooooooooo---------\n");
		
		}
		
ecrt_master_deactivate(master);		
ecrt_release_master(master);	
        break;
		
		
		case 3:
		printf("Stop...\n");


        printf("debug 1...\n");
       // free_Master_Topology(m_);
        free(m_);
        printf("debug 2...\n");
       // free_Slave(s_);
        printf("debug 3...\n");
        free(s_);
        printf("debug 4...\n");
        free(d_);
        printf("debug 5...\n");
        free(o_d_a);
        printf("debug 6...\n");
        free(o_c_a);
        printf("debug 7...\n");
        free(b_s_a);
        printf("debug 8...\n");
        free(d_a);
        printf("debug 9...\n");
        free(s_a);
        printf("debug 10...\n");

			
		return 0;
		break;
     
		
		default:
        printf("ERROR\n");
        break;
	
	

            }
        }


    return 0;
}


