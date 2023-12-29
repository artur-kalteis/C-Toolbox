#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include <json-c/json.h>
#include <sys/types.h>
#include <stdbool.h>


    // Reihenfolge der Übergabeparameter
    // 1=IP
    // 2=user 
    // 3=password
    // 4=database
    // 5=device_id
    char var_ip[32];
    char var_user[32];
    char var_password[32];
    char var_database[32];
    char var_dev_id[32];



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

int buffer_one=0;

void json_parse_array_influxdb( json_object *jobj, char *key) {

    FILE *p;
    
    p=fopen("/root/ethercat/EMMS2/tmp/influx_parameter.cfg","a");
    if (!p){printf("fopen ERROR!!!\n");}


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
                          fprintf(p,"%d\n", json_object_get_int(jvalue));
                          //printf("value  %d\n", json_object_get_int(jvalue));
                         
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

  fclose(p);
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

void influx_curl(char command [15000], char query[100000])
{
//printf("entered influx_curl\n");

char login[64];
sprintf(login,"%s:%s",var_user,var_password);


CURL *curl = curl_easy_init();

CURLcode res;
if(curl) {
        struct string s;
        init_string(&s);
        curl_easy_setopt(curl, CURLOPT_URL, command);
        curl_easy_setopt(curl, CURLOPT_USERPWD,login);
		    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, query);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        res = curl_easy_perform(curl);
        json_object *jobj = json_tokener_parse(s.ptr);
        curl_easy_cleanup(curl);
        json_parse_influx(jobj);
        }
}



char vars[8][32]={"bool_cmd_topology_start","bool_cmd_topology_on","bool_cmd_topology_off","bool_cmd_topology_pause","bool_cmd_topology_sleep","bool_cmd_topology_standby","bool_cmd_topology_reset","ui16_cmd_master"};

int main(int argc, char* argv[])
{




  if(argc==6)
  {
    printf("6 parameter wurden übergeben\n");

    sprintf(var_ip,argv[1]);
    sprintf(var_user,argv[2]);
    sprintf(var_password,argv[3]);
    sprintf(var_database,argv[4]);
    sprintf(var_dev_id,argv[5]);

  }
  else
  {
   // printf("zu wenig parameter wurden übergeben\n");
    return 0;

  }

    
    char login[64];
    sprintf(login,"%s:%s",var_user,var_password);
    
    system("rm -r /root/ethercat/EMMS2/tmp/influx_parameter.cfg"); // löscht die alte cfg
    
    int i=0;
    char command[15000], varlist[100000], query[100000];
    sprintf(command, "http://%s/query?pretty=true&db=%s&epoch=ns",var_ip,var_database);
    
    //strcpy(command, "http://192.168.72.139:8086/query?pretty=true&db=ecat_master&epoch=ns");
    strcpy(query, "q=SELECT * from topology_cmd_statemachine");
   // strcat(query,varlist);
    strcat(query," ORDER BY DESC LIMIT 1"); // select Last Value // wird aber alphabetisch zurück gegebens
    
    //printf("%s\n",query);
   // printf("--------------------------------------\n");
    influx_curl(command,query);

     
   
return 0;	
}
