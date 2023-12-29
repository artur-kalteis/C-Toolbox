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

#include "../include/read_config.h" // erst nachdem alle Slaves includet sind


long master_state[8];
int status=0;


char var[256][50];
int j=1;

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

                        if(buffer_one!=0)
                        {
                         // fprintf(p,"%d\n", json_object_get_int(jvalue));
                          //printf("value  %d\n", json_object_get_int(jvalue));

                          master_state[master_cmd_count]=json_object_get_int(jvalue);
                          //printf("value neu  %d\n", master_state[master_cmd_count]);
                          master_cmd_count++;

                          
                         
                        }

                        buffer_one=1;
                        
                       // buffer_one=!buffer_one;
                        
                        break;/*
                       case json_type_string: 
                        
                        printf("time %s,\n", json_object_get_string(jvalue));
                        break; 
                        case json_type_null: 
                        
                        printf("value: %s,\n", json_object_get_string(jvalue));
                        break;
                        case json_type_boolean: 
                        
                        printf("value2: %s,\n", json_object_get_string(jvalue));
                        break;
                        case json_type_double: 
                        
                        printf("time2: %s,\n", json_object_get_string(jvalue));
                        break;*/
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
//printf("entered influx_curl\n");
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

	

//system("clear");

/********************************shared Memory Parameter*****************************************/
/*key_t PAR_key[REGISTER_PAR];
int PAR_id[REGISTER_PAR];
int *PAR_shm[REGISTER_PAR];
int PAR_keystart=2323; 


	
int i1=0;

for(i1=0;i1<REGISTER_PAR;i1++)
{

PAR_key[i1]=PAR_keystart+i1;


PAR_id[i1] = shmget(PAR_key[i1],size_PAR*sizeof(int),0666); 

if(PAR_id[i1] == -1)
{
	perror("Failed to shmget control \n");
	exit(1);

}

PAR_shm[i1]= shmat(PAR_id[i1], NULL , 0);


if(PAR_shm[i1]==(int *)-1)
{
	perror("Failed to shmat \n");
	exit(1);

}


}*/
/********************************shared Memory Parameter*****************************************/


  int old_status=0;



  while(1)
  {
	
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





