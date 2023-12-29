#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include <json-c/json.h>
#include <sys/types.h>
#include <stdbool.h>

int slave_x=0; // Für das durchlaufen der Slaves in verschiedenen SChleifen
//#define slave_counter 3

//#define dataset_count 2

#include "../include/slaves.h"

#include "../include/slave_obj.h"

#include "../include/master_obj.h"

char collection[17][32]={"application","commando","status","description","external_sender","file","function","geography","internal_receiver","internal_sender","message","parameter","specification","state","external_receiver","temporary","user"};

//char collection_key[slave_counter][16]={"/dev_00","/dev_01","/dev_02"};

enum attribut_list{e_application=0,e_commando,e_status,e_description,e_mesg,e_parameter,e_state};
enum attribut_list handler=6;

enum attribut_obj{e_master=0,e_slave}; // Beschreibt was eingelesen wird
enum attribut_obj obj_handler=1;

struct cmd_master_obj *cmd;
struct sts_master_obj *sts;

Master_Topology *m_;
Slave *s_;
Dataset *d_;
Output_Data_Array *o_d_a;
Output_CMD_Array *o_c_a;
Beckhoff_Slave_Array *b_s_a; // if Beckhoff Slave
Data_Array *d_a;
Status_Array *s_a;

#include "../include/read_arango_parser.h"
#include "../include/read_config.h"
#include "../include/write_influx.h"


/*
void write_influx_curl(char s1[1240000],char s2[1024]) //s1=varlist s2=URL
{
	char post[1240000];
	
	static const char *postthis;
	

	CURL *curl;
	CURLcode res;
	sprintf(post,"");
	
	curl = curl_easy_init();
	if(!curl) 
	{
	printf("error no cerl\n");
	//return 0;
	}
	
	curl_easy_setopt(curl, CURLOPT_URL, s2);
	curl_easy_setopt(curl, CURLOPT_USERPWD, "root:sgcdm2017");
		
	sprintf(post,"%s",s1);
	postthis=post;
	
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(postthis));
	res = curl_easy_perform(curl); //führt eine Dateiübertragung aus

	if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 		 
	curl_easy_cleanup(curl);
	
}*/

/*
void write_influx_init(Master_Topology *m)
{

 
    char varlist[1240000];
    char buffer[1240000];
    
    char url[1024];
    
    char server[64]="http://localhost:8086";
    char write[64]="/write?db=";
    char database[32]="ecat_master";
    
    sprintf(url,"%s%s%s",server,write,database);
    
   
    printf("Debug start \n");
    
    


    int i2=0;
    int i=0;
    for(i=0; i< m->slave_count; i++)
    {
        sprintf(varlist,"");
        sprintf(buffer,"");
        printf("Debug for1= %i\n",i);
        //sprintf(varlist,"test_measurement2 value1=123,value2=456,value3=789\n");          
    
    for(i2=0; i2< m->slave_objekt[i].beckhoff_slave_count; i2++)
    {
        printf("Debug for2= %i2\n",i);
      
        sprintf(buffer,"%s",m->slave_objekt[i].beckhoff_data->beckhoff_s_array[i2].name);  
        strcat(varlist,buffer);
        sprintf(buffer," value=%i\n",m->slave_objekt[i].beckhoff_data->beckhoff_s_array[i2].value);    
        strcat(varlist,buffer);       
    }

    printf("hhieerrrr -> %s \n",varlist);  
    write_influx_curl(varlist,url);

    }


}
*/
int main()
{
   
    read_config();
    
            char login[64];
              sprintf(login,"%s:%s",arangodb_user,arangodb_password); 
    
              //printf("Hier-------------->%s\n",login);
    
              char URL[256];
              char server[64];
              char uebergabe[64];
              
              sprintf(server, "http://%s/_db/",arangodb_ip);
              char collection_a[64]="/_api/document/";
      
              
              sprintf(URL,"");
              strcat(URL,server); 
              strcat(URL,arangodb_database); 
              strcat(URL,collection_a); 
    
             // printf("Hier-------------->%s\n",URL);

    
    
    
        m_=(Master_Topology*) malloc(sizeof(Master_Topology));
        s_=(Slave*) malloc(sizeof(Slave));
        d_=(Dataset*) malloc(sizeof(Dataset));
    
        cmd=(struct cmd_master_obj*) malloc(sizeof(struct cmd_master_obj));
        sts=(struct sts_master_obj*) malloc(sizeof(struct sts_master_obj));
    
    
        m_->slave_count=slave_count;
        init_Master_Topology(m_,m_->slave_count);
    
        
        for(slave_x=0;slave_x< slave_count;slave_x++)
        {
          //  printf("Debug %i \n",slave_x);
            read_arango_curl(collection[13],slave_id[slave_x],URL,login);
            insert_Master_Topology(m_,s_);
        }
    
       
        printf("--------------------------------------------- \n");
        output_master_obj(m_);
   write_influx_init(m_);
   
   

free(cmd);
free(sts);
free(m_);
free(s_);
free(d_);

free(o_d_a);
free(o_c_a);
free(b_s_a);
free(d_a);
free(s_a);

return 0;	
}
