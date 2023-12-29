#ifndef READ_CONFIG_H
#define READ_CONFIG_H




char dev_id[32];
char arangodb_ip[32];
char arangodb_user[32];
char arangodb_password[32];
char arangodb_database[32];

char influxdb_ip[32];
char influxdb_user[32];
char influxdb_password[32];
char influxdb_database[32];


void read_config()
{


char config_Ausgang[9][32]; 
char config_buffer[32];
FILE *config;
config=fopen("config/tm_config.cfg","r");  
if (!config) {printf("ERROR, file not exist \n");} 
int a=0;
int i=0;
int g=0;
for(a=0;a<18;a++)
{   
    fscanf(config,"%s",&config_buffer);
    if(i)
    {
        sprintf(config_Ausgang[g],"");    
        //printf("Ausgang = %s \n",config_buffer);
        strcat(config_Ausgang[g],config_buffer);
        //sprintf(Ausgang[g],"%s",buffer);
        g++;
    }

    
    i=!i;

}

strcat(dev_id,config_Ausgang[0]);
strcat(arangodb_ip,config_Ausgang[1]);
strcat(arangodb_user,config_Ausgang[2]);
strcat(arangodb_password,config_Ausgang[3]);
strcat(arangodb_database,config_Ausgang[4]);
strcat(influxdb_ip,config_Ausgang[5]);
strcat(influxdb_user,config_Ausgang[6]);
strcat(influxdb_password,config_Ausgang[7]);
strcat(influxdb_database,config_Ausgang[8]);

}




#endif