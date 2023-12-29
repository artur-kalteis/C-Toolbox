#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/ipc.h> //for Shared-Memory
#include <sys/shm.h> //for Shared-Memory
//#include "shared_memory.h"
#include <curl/curl.h>
#include <string.h>
#include <json-c/json.h>

#include <time.h>

#include "ecrt.h"

int i_struct=0; 
int act_slave_count=0;


#include "../include/slaves.h" // mit dem Watchdog ausgelesene Topologie
#include "../include/pdo.h"
#include "../include/domain_reg.h" // erst nachdem alle Slaves includet sind
#include "../include/read_config.h" // erst nachdem alle Slaves includet sind
#include "../include/slave_obj.h"

#include "../include/master_obj.h"



//Array to store the cmd from aib-slave

//enum influx_structur{state_machine=0,beckhoff_slave,jm_slave,aib_slave}; // Beschreibt was eingelesen wird
   
  
char name_buffer[32];

char aib_name_buffer[16][64];
char bridge_name_buffer[20][64];


int bit_array[16]={1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768}; // für das bitweise beschreiben eines 16Bit Values

//Arango Parser Handling
char collection[17][32]={"app","cmd","sts","desc","external_sender","file","fct","geo","internal_receiver","internal_sender","msg","par","state","spec","external_receiver","temp","user"};

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


long master_state[8];
int status=0;

int x=0; // beim durchlaufen der fork for-schleife

int slave_x=0; 

char var[256][50];
int j=1;

#include "../include/read_arango_parser.h"



/*
struct string {
  char *ptr;
  size_t len;
};

void init_string(struct string *s) {
  s->len = 0;
  s->ptr = malloc(s->len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
  size_t new_len = s->len + size*nmemb;
  s->ptr = realloc(s->ptr, new_len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
}
*/
void json_parse_influxdb(json_object *jobj)
        {
	int x,p;
        enum json_type type;
        json_object_object_foreach(jobj, key, val)
                {
		x = json_object_get_int(val);
		if(x<256)
		strcpy(var[x],key);
                }
        }

  
        
           

void json_parse_array_influxdb( json_object *jobj, char *key) {

  int buffer_one=0;
  int buffer_count=0; // für das hochzählen der variablen eines Slaves
  int aib_buffer=0;
  int bridge_buffer=0; // für die analog ausgänge
  int master_cmd_count=0;

    //FILE *p;

    // p=fopen("influx_parameter.cfg","a");
    //if (!p){printf("fopen ERROR!!!\n");}


  void json_parse_influx(json_object * jobj); /*Forward Declaration*/
  enum json_type type;

  json_object *jarray = jobj; /*Simply get the array*/
  if(key) {
    jarray = json_object_object_get(jobj, key); /*Getting the array if it is a key value pair*/
  }

  int arraylen = json_object_array_length(jarray); /*Getting the length of the array*/
  int i;
  json_object * jvalue;

  for (i=0; i< arraylen; i++){
    jvalue = json_object_array_get_idx(jarray, i); /*Getting the array element at position i*/
    type = json_object_get_type(jvalue);
    if (type == json_type_array) {
      json_parse_array_influxdb(jvalue, NULL);
    }
    else if (type != json_type_object) {
	  switch (type)
                        {


                        



                        case json_type_int: 

                         // printf("-------------hier-- mitten drin-----%i-----------\n",i_struct);
                         // printf("------------slave%i--------------\n",act_slave_count);
                        
                          if(i_struct==0)
                          {
                            if(buffer_one!=0)
                            {
                             // fprintf(p,"%d\n", json_object_get_int(jvalue));
                              //printf("value  %d\n", json_object_get_int(jvalue));
    
                              master_state[master_cmd_count]=json_object_get_int(jvalue);
                              //printf("value neu  %d\n", master_state[master_cmd_count]);
                              master_cmd_count++;
    
                              
                             
                            }
    
                            buffer_one=1;
                          }
                          
                          if(i_struct==1)
                          {



                           // printf("--------------testing..........-----------------\n");

                            if(buffer_one!=0)
                            {
                            // printf("reading influx......\n");
                            
    
                             //old m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[buffer_count].value=json_object_get_int(jvalue);
                             
                             //old  buffer_count++;

                             int ix2=0;
                             

                             for(ix2=0; ix2 < m_->slave_objekt[act_slave_count].beckoff_slave_ana_out_count; ix2++)
                             { 

                                /* old
                                 if(!strcmp(bridge_name_buffer[buffer_count],m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].name))
                               {

                                 //printf(" %s  und %s  in array %i\n",bridge_name_buffer[ix2],m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[buffer_count].name,ix2);
                                // testbuffering=json_object_get_int(jvalue);
                                 *((int32_t*)&m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].f_value)=json_object_get_int(jvalue);

                                 //printf("int =%i  float =%f  \n",json_object_get_int(jvalue),m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].f_value );
                                 //printf("writing value done \n");
                               }
                              
                              */
                              
                                //printf("reading influx.... in for..\n");

                              

                                sprintf(name_buffer,"");
                                sprintf(name_buffer,"last_%s",m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].name);  
                               


                                //printf(" %s  und %s  comparing\n",bridge_name_buffer[buffer_count],m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].name);
                               if(!strcmp(bridge_name_buffer[buffer_count],name_buffer))
                               {

                                 //printf(" %s  und %s  in array %i\n",bridge_name_buffer[ix2],m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[buffer_count].name,ix2);
                                // testbuffering=json_object_get_int(jvalue);
                                 //*((int32_t*)&m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].f_value)=json_object_get_int(jvalue);



                                  /*
                                    m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].f_value=strtod(json_object_get_string(jvalue),NULL);
                               
                              *((float*)&m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].value)=m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].f_value;
 
                                  */
                                  *((float*)&m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].f_value)=json_object_get_int(jvalue);

                               //  m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].value=json_object_get_int(jvalue);
                                 *((float*)&m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].value)=m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].f_value;
                                 

                             //   printf("11111int =%i   float=%f \n",m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].value,m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].f_value );
                                 //printf("writing value done \n");
                               } 
                               
                             }
                                                 
                           //   printf("11111value neu  %i\n",  m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[buffer_count].value);
                             buffer_count++;                                

                             
                            }
    
                            buffer_one=1;

                          }

                          if(i_struct==3)
                          {


                        //    printf(" from influx read value  %d\n", json_object_get_int(jvalue));

                            if(buffer_one!=0)
                            {
                             // fprintf(p,"%d\n", json_object_get_int(jvalue));

                            // printf("testing  --------------%i-\n",buffer_count);
                              int ix=0;

                              for(ix=0; ix < m_->slave_objekt[act_slave_count].slave_output_cmd_count;ix++)
                              { 
                                

                                sprintf(name_buffer,"");
                                sprintf(name_buffer,"last_%s",m_->slave_objekt[act_slave_count].output_cmd->output_cmd_array[ix].name);  

                                //printf(" %s  und %s  comparing\n",aib_name_buffer[buffer_count],name_buffer);
                                if(!strcmp(aib_name_buffer[buffer_count],name_buffer))
                                {

                                //  printf(" %s  und %s  in array %i\n",aib_name_buffer[ix],m_->slave_objekt[act_slave_count].output_cmd->output_cmd_array[buffer_count].name,ix);
                                  m_->slave_objekt[act_slave_count].output_cmd->output_cmd_array[ix].value=json_object_get_int(jvalue);
                                } 
                              }
                                                  
                              //printf("value neu  %i\n",  m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[buffer_count].value);
                              buffer_count++;

                             
                            }
                            

                            buffer_one=1;

                          }
                        
                        break;
                       case json_type_string: 
                        
                       if(i_struct==3)
                       {

                        if(strcmp(json_object_get_string(jvalue),"time"))
                        {
                          // printf("Name %s  \n", json_object_get_string(jvalue));
                        
                            sprintf(aib_name_buffer[aib_buffer],"%s",json_object_get_string(jvalue));
                          //  printf("Name %s   %i\n", aib_name_buffer[aib_buffer],aib_buffer);
                                               
                            aib_buffer++;
                        }

                       

                       
                       }

                       if(i_struct==1)
                       {
                        if(strcmp(json_object_get_string(jvalue),"time"))
                        {
                         sprintf(bridge_name_buffer[bridge_buffer],"%s",json_object_get_string(jvalue));


                       //  printf("Name %s   bridge_buffer= %i\n", bridge_name_buffer[bridge_buffer],bridge_buffer);
                                             
                          bridge_buffer++;
                        }
                       }

                        break; 

                        /*
                        case json_type_null: 
                        
                        printf("value: %s,\n", json_object_get_string(jvalue));
                        break;
                        */
                        case json_type_boolean: 
                        
                        //printf("bool detected value2: %s,\n", json_object_get_string(jvalue));

                         

                        


                        break;
                        
                        case json_type_double: 
                        
                    //    printf("time2: %s\n", json_object_get_string(jvalue));

                    
                   
                        if(i_struct==1)
                        {

                          if(buffer_one!=0)
                          {
                         

                           int ix2=0;
                           

                           for(ix2=0; ix2 < m_->slave_objekt[act_slave_count].beckoff_slave_ana_out_count; ix2++)
                           { 

                             
                            sprintf(name_buffer,"");
                            sprintf(name_buffer,"last_%s",m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].name);  
                           


                            //printf(" %s  und %s  comparing\n",bridge_name_buffer[buffer_count],m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].name);
                           if(!strcmp(bridge_name_buffer[buffer_count],name_buffer))
                           {

                               //printf(" %s  und %s  in array %i\n",bridge_name_buffer[ix2],m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[buffer_count].name,ix2);
                              // testbuffering=json_object_get_int(jvalue);
                               //*((int32_t*)&m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].f_value)=json_object_get_int(jvalue);

                             //  m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].value=json_object_get_int(jvalue);
                              // *((int32_t*)&m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].f_value)=json_object_get_string(jvalue);

                              m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].f_value=strtod(json_object_get_string(jvalue),NULL);
                               
                              *((float*)&m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].value)=m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].f_value;


                              //  printf("Value %i float=%f \n",m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].value,m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[ix2].f_value);
                               //printf("writing value done \n");
                             } 


                             //printf("22222value neu  %i\n",  m_->slave_objekt[act_slave_count].beckhoff_data_out->beckhoff_s_array[buffer_count].value);
                             
                           }
                                               
                      
                           buffer_count++;                                

                           
                          }
  
                          buffer_one=1;

                        }
                        



                        break;
                        }
    }
    else {
      json_parse_influx(jvalue);
    }
  }

 
}


void json_parse_influx(json_object *jobj)
        {
        int x,p;
        enum json_type type;
        json_object_object_foreach(jobj, key, val)
                {
		type= json_object_get_type(val);
			if (type == json_type_array){
				json_parse_array_influxdb(jobj, key);
			}
        }
    }

void read_influx_curl(char command [15000], char query[100000],char user_login[128]) // unterscheidet sich von Anwendung zu Anwendung
{



CURL *curl = curl_easy_init();
CURLcode res;
if(curl) {
        struct string s;
        init_string(&s);
        curl_easy_setopt(curl, CURLOPT_URL, command);
        curl_easy_setopt(curl, CURLOPT_USERPWD, user_login);
		    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, query);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        res = curl_easy_perform(curl);
        json_object *jobj = json_tokener_parse(s.ptr);
        curl_easy_cleanup(curl);
        json_parse_influx(jobj);
        }
}

  
int pid;


get_pid()
{

system("pgrep -u root ecat_master > info.txt");

FILE *p;
p=fopen("info.txt","r");
fscanf(p,"%i",&pid);
fclose (p);
system("rm info.txt");

}


int main(int argc, char **argv)
{
  



     printf("-------------controler debug -----1-----------\n");

  char command_read[15000], query_read[100000];

/*************************Statmachine*****************************************************/  

          read_config();

          char login[64];
          sprintf(login,"%s:%s",influxdb_user,influxdb_password); 


  //int i=0;
  char command[15000], query[100000];
  
  sprintf(command,"");          
  sprintf(command, "http://%s/query?pretty=true&db=%s&epoch=ns",influxdb_ip,influxdb_database);        


  strcpy(query, "q=SELECT * from master_cmd_statemachine");



  strcat(query," ORDER BY DESC LIMIT 1"); // select Last Value // wird aber alphabetisch zurück gegebens

	
	
get_pid();
printf("-------------controler debug -----2-----------\n");


/*****************arangodb*******************************************************************/
char login2[64];
sprintf(login2,"%s:%s",arangodb_user,arangodb_password); 

 // printf("Hier-------------->%s\n",login);

  char URL[256];
  char server[64];
  char uebergabe[64];
  
  sprintf(server, "http://%s/_db/",arangodb_ip);
  char collection_a[64]="/_api/cursor/";

  
  sprintf(URL,"");
  strcat(URL,server); 
  strcat(URL,arangodb_database); 
  strcat(URL,collection_a); 

  m_=(Master_Topology*) malloc(sizeof(Master_Topology));
  
  s_=(Slave*) malloc(sizeof(Slave));
  
  d_=(Dataset*) malloc(sizeof(Dataset));

  m_->slave_count=slave_count;
  init_Master_Topology(m_,m_->slave_count);
  
  printf("-------------controler debug -----3-----------\n");
   
   for(slave_x=0;slave_x< slave_count;slave_x++)
   {  

      

       s_->slave_dataset_count=slave_datasets[slave_x]; // Anzahl der Datasets wird dan das Slaveobjekt übergeben 
      // s_buffer->slave_dataset_count=slave_datasets[slave_x]; // Anzahl der Datasets wird dan das Slaveobjekt übergeben 
       
       init_Slave(s_,s_->slave_dataset_count);
      // init_Slave(s_buffer,s_buffer->slave_dataset_count);


      if(slave_serilized[slave_x]==1)// Beckhoff Slave erkannt // hat keine Parameters
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
       printf("-------------controler debug -----4-----------\n");
     
       insert_Master_Topology(m_,s_);

       printf("-------------controler debug -----5-----------\n");
     
   }

   printf("-------------controler debug -----6-----------\n");
  // output_master_obj(m_);



/********************************shared Memory Parameter*****************************************/



int me_id[slave_count];
key_t key[slave_count];
int *shm[slave_count]; // ist ein Zeigerarray d.h. shm[REGISTER_MESS][Array-Position]
int keystart=9876; // ab hier startet der KEY

int old_status=0;
int i1=0;
int i2=0;

for(i1=0;i1<slave_count;i1++)
{
   
//(Flag_info) für JM muss eine eigene seperate Bedinung eingeführt werden    
if(slave_serilized[i1]==0)// Beckhoff Slave erkannt
{   

  printf("-------------controler debug -----7-----------\n");
  
key[i1]=keystart+i1;

printf("--------------------------Controler  beckhoff-------------------------------- \n");

printf("--------------------------Controler  beckoff_slave_ana_out_count=%i-------------------------------- \n",m_->slave_objekt[i1].beckoff_slave_ana_out_count);


me_id[i1] = shmget(key[i1], m_->slave_objekt[i1].beckoff_slave_ana_out_count*sizeof(int),0666); 

if(me_id[i1] == -1)
{
    perror("Failed to shmget basic_mess controler \n");
    exit(1);

}

shm[i1]= shmat(me_id[i1], NULL , 0);


if(shm[i1]==(int *)-1)
{
    perror("Failed to shmat \n");
    exit(1);

}
 

} // end of if(slave_serilized[i]==0) 

else
{

  printf("-------------controler debug -----8-----------\n");
    if(!strcmp(slave_name[i1],"AB1001"))
    {
      printf("--------------------------Controler  AIB-------------------------------- \n");
      

        key[i1]=keystart+i1;
        
        
        me_id[i1] = shmget(key[i1], m_->slave_objekt[i1].slave_output_cmd_count*sizeof(int), 0666); 
        
        if(me_id[i1] == -1)
        {
            perror("Failed to shmget basic_mess \n");
            exit(1);
        
        }
        
        shm[i1]= shmat(me_id[i1], NULL , 0);
        
        
        if(shm[i1]==(int *)-1)
        {
            perror("Failed to shmat \n");
            exit(1);
        
        }

        
    }
    

}

}
/********************************shared Memory Parameter*****************************************/

printf("-------------controler debug -----10-----------\n");
  
printf("--------------------------Controler  while loop-------------------------------- \n");

  while(1)
  {
  

    printf("-------------controler debug -----llooopppp-----------\n");
    
    for(i1=0;i1<slave_count;i1++)
    {
       
    //(Flag_info) für JM muss eine eigene seperate Bedinung eingeführt werden    
    if(slave_serilized[i1]==0)// Beckhoff Slave erkannt
    {   

    
      act_slave_count=i1;
      i_struct=1; // influxdb parser use beckhoff slave


      

      sprintf(query_read,""); 
      sprintf(command_read,"");          
      sprintf(command_read, "http://%s/query?pretty=true&db=%s&epoch=ns",influxdb_ip,influxdb_database);        
    
    
      sprintf(query_read, "q=SELECT last(*) from \"%s\"",m_->slave_objekt[i1].slave_par_id);
    
    
    
     // strcat(query_read," ORDER BY DESC LIMIT 1"); // select Last Value // wird aber alphabetisch zurück gegebens



        printf("command =%s\n",command_read);
        printf("query   =%s\n",query_read);

     
  

        read_influx_curl(command_read,query_read,login);

    


    
    
    
        for(i2=0;i2<m_->slave_objekt[i1].beckoff_slave_ana_out_count;i2++)
        {

          printf("save the Influx values=>%s= %i\n",m_->slave_objekt[i1].beckhoff_data_out->beckhoff_s_array[i2].name,m_->slave_objekt[i1].beckhoff_data_out->beckhoff_s_array[i2].value);
          
        shm[i1][i2]= m_->slave_objekt[i1].beckhoff_data_out->beckhoff_s_array[i2].value;      
      
        }
    }
    else
    { 
      if(!strcmp(slave_name[i1],"AB1001"))
      {


        act_slave_count=i1;
        i_struct=3; // influxdb parser use aib_slave
        
        sprintf(query_read,""); 
        sprintf(command_read,"");          
        sprintf(command_read, "http://%s/query?pretty=true&db=%s&epoch=ns",influxdb_ip,influxdb_database);        
      
      
        sprintf(query_read, "q=SELECT last(*) from \"%s\"",m_->slave_objekt[i1].slave_cmd_id);
      
      
      
       // strcat(query_read," ORDER BY DESC LIMIT 1"); // select Last Value // wird aber alphabetisch zurück gegebens
  
       printf("command =%s\n",command_read);
       printf("query   =%s\n",query_read);
       printf("i_struct  =%i\n",i_struct);
  
  
      read_influx_curl(command_read,query_read,login);
  
  
      
      
      
      for(i2=0;i2<m_->slave_objekt[i1].slave_output_cmd_count;i2++)
      {
  
        printf("save the Influx values=>%s= %i\n",m_->slave_objekt[i1].output_cmd->output_cmd_array[i2].name,m_->slave_objekt[i1].output_cmd->output_cmd_array[i2].value);
      shm[i1][i2]= m_->slave_objekt[i1].output_cmd->output_cmd_array[i2].value;      
    
      }



      } 
    }

  }

     
 
  
   i_struct=0;

    read_influx_curl(command,query,login);
    
      status=master_state[7];
     
      

      if(old_status!=status)
      {
	
      old_status=status;
	//printf("status %i\n",status );
	//printf("read_value[2] %i\n", read_value[2]);
	switch (status)
	 {
        case 0:
		//do
		
//	sprintf(write_arango,"./write_arango.sh %i",status);
//	system(write_arango);
        break;
			
			
		case 1:
		//do
		
		printf("Status=PREOP\n");
		kill(pid,SIGHUP);
	//sprintf(write_arango,"./write_arango.sh %i",status);
	//system(write_arango);
        break;
			
			
		case 2:
		//do
		
		printf("Status=OP\n");
		kill(pid,SIGCONT);
	//sprintf(write_arango,"./write_arango.sh %i",status);
	//system(write_arango);
        break;
		
		case 3:
		//do
		printf("Status=beenden\n");
		kill(pid,SIGINT);
	//kill(pid1,SIGKILL);
	//sprintf(write_arango,"./write_arango.sh %i",status);
  //system(write_arango);
  

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
          
          int ix1=0;

          for(ix1=0;ix1<slave_count;ix1++)
          {
          shmdt(shm[ix1]); //Zum loslosen des Speichers vom Prozess  (speicher wird dabei nicht gel�scht)
          
          shmctl(me_id[ix1],IPC_RMID,NULL); //L�scht den Speicher
          }   


	return 0;
		
		
        break;

		
		default:
        printf("Control-ERROR\n");
		break;
	}
  }
			sleep(2);
	
	}
	

	
	
		

return 0;
}





