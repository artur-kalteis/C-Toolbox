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

    char var_ip[32];
    char var_user[32];
    char var_password[32];
    char var_database[32];
    char var_dev_id[32];




// -ljson-c -lcurl
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

void json_parse_array( json_object *jobj, char *key) {
  void json_parse(json_object * jobj); /*Forward Declaration*/
  enum json_type type;

  json_object *jarray = jobj; /*Simply get the array*/
  if(key) {
    jarray = json_object_object_get(jobj, key); /*Getting the array if it is a key value pair*/
  }

  int arraylen = json_object_array_length(jarray); /*Getting the length of the array*/
  //printf("Array Length: %d\n",arraylen);
  int i;
  json_object * jvalue;

  for (i=0; i< arraylen; i++){
    jvalue = json_object_array_get_idx(jarray, i); /*Getting the array element at position i*/
    type = json_object_get_type(jvalue);
    if (type == json_type_array) {
      json_parse_array(jvalue, NULL);
    }
    else if (type != json_type_object) {
     // printf("value[%d]: ",i);
    }
    else {
      json_parse(jvalue);
    }
  }
}

char sx1[124]; 

void json_parse(json_object *jobj)
        {
          
		
        enum json_type type;
        json_object_object_foreach(jobj, key, val)
                {
                type= json_object_get_type(val);
				
		  
		
			
			FILE *p;
		
		
		
		p=fopen("/root/ethercat/EMMS2/tmp/arango_parameter.cfg","a");
		if (!p){printf("fopen ERROR!!!\n");}
			
			
                switch (type)
                        {
                        case json_type_int: 
						//fprintf(p,"%s:", key);
                        fprintf(p,"%d \n", json_object_get_int(val));  
						
						//sprintf(sx1,"%s:%d",key,json_object_get_int(val));
						//printf("%s \n", sx1);
						//fprintf(p,"%s\n", sx1);
                        break;
                        case json_type_string:
						if (!strcmp(key,"_key")||!strcmp(key,"_id")||!strcmp(key,"_rev")) {break;}
						//fprintf(p,"%s:", key);						
                        fprintf(p,"%s \n", json_object_get_string(val));
						
						//sprintf(sx1,"%s:%s",key,json_object_get_int(val));
						//printf("%s \n", sx1);
						//fprintf(p,"%s\n", sx1);
                        break;
                    /*    case json_type_null:
						printf("%s:", key);						
                        printf("%s \n", json_object_get_string(val));
                        break;*/
                        case json_type_boolean: 
                        					
                        fprintf(p,"%s \n", json_object_get_string(val));
                        break;
                       /* case json_type_double: 
                        printf("%s:", key);						
                        printf("%s \n", json_object_get_string(val));
                        break;
                       case json_type_object: 
                      // printf("%s:", key);						
                        //printf("%s \n,", json_object_get_string(val));
                        break;
						*/
                         case json_type_array:  // array Ausgabe nicht notwendig // da nur der inhalt wichtig ist
                        //printf("%s:", key);						
                       // printf("%s \n,", json_object_get_string(val));
                        json_parse_array(jobj, key);
                      
					  break;
                        }
					fclose(p);		
                }
		
        }

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
            printf("zu wenig parameter wurden übergeben\n");
            return 0;
        
          }

         
          char login[64];
          sprintf(login,"%s:%s",var_user,var_password);          


system("rm -r /root/ethercat/EMMS2/tmp/arango_parameter.cfg"); // löscht die alte cfg

char URL[256];
char server[64];
char uebergabe[64];

sprintf(server, "http://%s/_db/",var_ip);



sprintf(uebergabe,"/");
strcat(uebergabe,var_dev_id); 
sprintf(var_dev_id, "%s",uebergabe);



char collection[64]="/_api/document/parameter";
//char collection_key[64]="/dev_3";

sprintf(URL,"");
strcat(URL,server); 
strcat(URL,var_database); 
strcat(URL,collection); 
strcat(URL,var_dev_id); //collection_key

CURL *curl = curl_easy_init();
CURLcode res;
if(curl) {

        struct string s;
    init_string(&s);
    curl_easy_setopt(curl, CURLOPT_URL, URL);
    curl_easy_setopt(curl, CURLOPT_USERPWD, login);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
    
    res = curl_easy_perform(curl);
    
        json_object *jobj = json_tokener_parse(s.ptr);
        
    curl_easy_cleanup(curl);
    
        json_parse(jobj);
       
    }

    return 0;
}
