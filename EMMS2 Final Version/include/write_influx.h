#ifndef WRITE_INFLUX_H
#define WRITE_INFLUX_H




void write_influx_curl(char s1[1240000],char s2[1024]) //s1=varlist s2=URL
{
	char post[1240000];
	
	static const char *postthis;


	char login[64];
    sprintf(login,"%s:%s",influxdb_user,influxdb_password);
	

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
	curl_easy_setopt(curl, CURLOPT_USERPWD, login);
		
	sprintf(post,"%s",s1);
	postthis=post;

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(postthis));
	res = curl_easy_perform(curl); //führt eine Dateiübertragung aus

	if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 		 
	curl_easy_cleanup(curl);
	
}



void initialize_influx(Master_Topology *m)
{

    char varlist[1240000];
    char buffer[1240000];
    
    char url[1024];
    
   // char server[64]"http://localhost:8086";

   	char server[64];
	sprintf(server,"http://%s",influxdb_ip);

    char write[64]="/write?db=";
   // char database[32]="sgcdm";
    
    sprintf(url,"%s%s%s",server,write,influxdb_database);
    
   
    int i3=0;
    int i2=0;
    int i=0;
    for(i=0; i< m->slave_count; i++)
    {
        sprintf(varlist,"");
        sprintf(buffer,"");
        
       
     
    if(slave_serilized[i]==0)// Beckhoff Slave erkannt
    {

        sprintf(buffer,"%s,serial_no=%s ",m->slave_objekt[i].slave_par_id,slave_serial[i]);  
        strcat(varlist,buffer);


        for(i2=0; i2< m->slave_objekt[i].beckoff_slave_ana_out_count; i2++)
        {
            
          

            //sprintf(buffer,"%s=%i",m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].name,m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].value);  
            //strcat(varlist,buffer);


            if(!strcmp(m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].unit,"ui16"))
            {                            
               
                sprintf(buffer,"%s=%u",m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].name,m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].u_value);
               
            }

            else if(!strcmp(m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].unit,"fl32"))
            {
               
                sprintf(buffer,"%s=%f",m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].name,m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].f_value);  
                                            
            }
     
            else if(!strcmp(m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].unit,"in16"))
            {
                
                sprintf(buffer,"%s=%i",m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].name,m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].value);
                
            }

            strcat(varlist,buffer);
            //wenn der letzte Wert erreicht ist
            if((i2+1) < (m->slave_objekt[i].beckoff_slave_ana_out_count))
            {
                sprintf(buffer,",");  
                strcat(varlist,buffer);
            }
            else
            {
                sprintf(buffer," ");  
                strcat(varlist,buffer);
            }

        }

        write_influx_curl(varlist,url);
    }
    else 
    {
      
        //if(!strcmp(slave_name[i],"AB1001") && m->slave_objekt[i].check_status==1)
        if(!strcmp(slave_name[i],"AB1001"))
        {
           // m->slave_objekt[i].check_status=0;
           
        sprintf(buffer,"%s,serial_no=%s ",m->slave_objekt[i].slave_cmd_id,slave_serial[i]);  
        strcat(varlist,buffer);
    
            for(i2=0; i2< m->slave_objekt[i].slave_output_cmd_count; i2++)
            {
                
                sprintf(buffer,"%s=%i",m->slave_objekt[i].output_cmd->output_cmd_array[i2].name,m->slave_objekt[i].output_cmd->output_cmd_array[i2].value);  
                strcat(varlist,buffer);
    
                //wenn der letzte Wert erreicht ist
                if((i2+1) < (m->slave_objekt[i].slave_output_cmd_count))
                {
                    sprintf(buffer,",");  
                    strcat(varlist,buffer);
                }
                else
                {
                    sprintf(buffer," ");  
                    strcat(varlist,buffer);
                }
    
            }
            write_influx_curl(varlist,url);

        }  
        
        
        if(!strcmp(slave_name[i],"AB1001"))  // Send status if check=1
        {
         
                sprintf(varlist,"");
                sprintf(buffer,"");
        
                sprintf(buffer,"%s,serial_no=%s ",m->slave_objekt[i].slave_sts_id,slave_serial[i]);  
                strcat(varlist,buffer);

                for(i2=0; i2< m->slave_objekt[i].slave_output_status_count; i2++) // I/O's des jeweiligen Slaves werden nacheinander abgearbeitet
                {

                    sprintf(buffer,"%s=%u",m->slave_objekt[i].output_status->output_status_array[i2].name,m->slave_objekt[i].output_status->output_status_array[i2].value);

                                    strcat(varlist,buffer);
                    
                                    //wenn der letzte Wert erreicht ist
                                    if((i2+1) < (m->slave_objekt[i].slave_output_status_count))
                                    {
                                        sprintf(buffer,",");  
                                        strcat(varlist,buffer);
                                    }
                                    else
                                    {
                                        sprintf(buffer," ");  
                                        strcat(varlist,buffer);
                                    }


                }    

                write_influx_curl(varlist,url);

            


        }

        

    }

    

    //  printf("hhieerrrr ---------write influx init---------> %s \n",varlist);  
   

    }

}


void write_influx_init(Master_Topology *m)
{

 
    char varlist[1240000];
    char buffer[1240000];
    
    char url[1024];
    
   // char server[64]"http://localhost:8086";

   	char server[64];
	sprintf(server,"http://%s",influxdb_ip);

    char write[64]="/write?db=";
   // char database[32]="sgcdm";
    
    sprintf(url,"%s%s%s",server,write,influxdb_database);
    
   
    int i3=0;
    int i2=0;
    int i=0;
    for(i=0; i< m->slave_count; i++)
    {
        sprintf(varlist,"");
        sprintf(buffer,"");
       
        //sprintf(varlist,"test_measurement2 value1=123,value2=456,value3=789\n");          
       // Slave not  Serilized here
    if(slave_serilized[i]==0)// Beckhoff Slave erkannt
    {
   


       /* for(i2=0; i2< m->slave_objekt[i].beckhoff_slave_count; i2++)
        {
        
            //sprintf(buffer,"%s",m->slave_objekt[i].beckhoff_data->beckhoff_s_array[i2].name);  
            strcat(varlist,buffer);
            //sprintf(buffer," value=%i",m->slave_objekt[i].beckhoff_data->beckhoff_s_array[i2].value);    
            strcat(varlist,buffer);  
            //printf("hhieerrrr -> %s \n",varlist);       
        }

        */

        sprintf(varlist,"");
        sprintf(buffer,"");

        sprintf(buffer,"%s,serial_no=%s ",m->slave_objekt[i].slave_state_id,slave_serial[i]);  
        strcat(varlist,buffer);

           
      

             for(i2=0; i2< m->slave_objekt[i].beckoff_slave_ana_in_count; i2++) // I/O's des jeweiligen Slaves werden nacheinander abgearbeitet
            {
 

            //sprintf(buffer,"%s=%f",m->slave_objekt[i].d_set[i2].data->d_array[i3].name,measurement_buffer[i][i2][i3]);  

                
            if(!strcmp(m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].unit,"ui16"))
            {                            
               
                sprintf(buffer,"%s=%u",m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].name,m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].u_value);
            }

            else if(!strcmp(m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].unit,"fl32"))
            {
               
                sprintf(buffer,"%s=%f",m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].name,m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].f_value);                                 
            }
     
            else if(!strcmp(m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].unit,"in16"))
            {
                
                sprintf(buffer,"%s=%i",m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].name,m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].value);
            }

            else if(!strcmp(m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].unit,"ui32"))
            {
                
                sprintf(buffer,"%s=%u",m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].name,m->slave_objekt[i].beckhoff_data_in->beckhoff_s_array[i2].value);
            }



            strcat(varlist,buffer);

            //wenn der letzte Wert erreicht ist
            if((i2+1) < (m->slave_objekt[i].beckoff_slave_ana_in_count))
            {
                sprintf(buffer,",");  
                strcat(varlist,buffer);
            }
            else
            {
                sprintf(buffer," ");  
                strcat(varlist,buffer);
            }
            
        
            
            }   
        
       


        //printf("Timestamp %lu \n", measurement_timestamp[i][i2]);
       // sprintf(buffer,"%lu", measurement_timestamp[i][i2]);  
        strcat(varlist,buffer);
        
        
        printf("hhieerrrr         Slave=%i          -> %s \n",i,varlist);
/*
        printf("\n");
        printf("\n");
        printf("\n");
        */
       write_influx_curl(varlist,url);

    }
    else 
    {

        if((!strcmp(slave_name[i],"AB1001")) && (m->slave_objekt[i].check_status==1))  // Send status if check=1
        {
            
                
                sprintf(varlist,"");
                sprintf(buffer,"");
        
                sprintf(buffer,"%s,serial_no=%s ",m->slave_objekt[i].slave_sts_id,slave_serial[i]);  
                strcat(varlist,buffer);

                for(i2=0; i2< m->slave_objekt[i].slave_output_status_count; i2++) // I/O's des jeweiligen Slaves werden nacheinander abgearbeitet
                {

                    sprintf(buffer,"%s=%u",m->slave_objekt[i].output_status->output_status_array[i2].name,m->slave_objekt[i].output_status->output_status_array[i2].value);

                                    strcat(varlist,buffer);
                    
                                    //wenn der letzte Wert erreicht ist
                                    if((i2+1) < (m->slave_objekt[i].slave_output_status_count))
                                    {
                                        sprintf(buffer,",");  
                                        strcat(varlist,buffer);
                                    }
                                    else
                                    {
                                        sprintf(buffer," ");  
                                        strcat(varlist,buffer);
                                    }


                }   
                
                printf("hhieerrrr         Slave=%i          -> %s \n",i,varlist);

                write_influx_curl(varlist,url);
            

        }


        for(i2=0; i2< m->slave_objekt[i].slave_dataset_count; i2++) 
        {

            sprintf(varlist,"");
            sprintf(buffer,"");
    
            sprintf(buffer,"%s,serial_no=%s,dataset=%s ",m->slave_objekt[i].slave_state_id,slave_serial[i],m->slave_objekt[i].d_set[i2].dataset_name);  
            strcat(varlist,buffer);

               
          

                for(i3=0; i3< m->slave_objekt[i].d_set[i2].dataset_data_array_count; i3++) // I/O's des jeweiligen Slaves werden nacheinander abgearbeitet
                {
     

                //sprintf(buffer,"%s=%f",m->slave_objekt[i].d_set[i2].data->d_array[i3].name,measurement_buffer[i][i2][i3]);  

                    
                if(!strcmp(m->slave_objekt[i].d_set[i2].data->d_array[i3].unit,"ui16"))
                {                            
                   
                    sprintf(buffer,"%s=%.6f",m->slave_objekt[i].d_set[i2].data->d_array[i3].name,m->slave_objekt[i].d_set[i2].data->d_array[i3].buffer_ui*m->slave_objekt[i].d_set[i2].data->d_array[i3].norm);
                }

                else if(!strcmp(m->slave_objekt[i].d_set[i2].data->d_array[i3].unit,"fl32"))
                {
                   
                    sprintf(buffer,"%s=%.6f",m->slave_objekt[i].d_set[i2].data->d_array[i3].name,m->slave_objekt[i].d_set[i2].data->d_array[i3].buffer_f*m->slave_objekt[i].d_set[i2].data->d_array[i3].norm);                                 
                }
         
                else if(!strcmp(m->slave_objekt[i].d_set[i2].data->d_array[i3].unit,"in16"))
                {
                    
                    sprintf(buffer,"%s=%.6f",m->slave_objekt[i].d_set[i2].data->d_array[i3].name,m->slave_objekt[i].d_set[i2].data->d_array[i3].buffer_i*m->slave_objekt[i].d_set[i2].data->d_array[i3].norm);
                }



                strcat(varlist,buffer);

                //wenn der letzte Wert erreicht ist
                if((i3+1) < (m->slave_objekt[i].d_set[i2].dataset_data_array_count))
                {
                    sprintf(buffer,",");  
                    strcat(varlist,buffer);
                }
                else
                {
                    sprintf(buffer," ");  
                    strcat(varlist,buffer);
                }
                
            
                
                }   
            
           


            //printf("Timestamp %lu \n", measurement_timestamp[i][i2]);
           // sprintf(buffer,"%lu", measurement_timestamp[i][i2]);  
            strcat(varlist,buffer);
            
            
            printf("hhieerrrr         Slave=%i    Dataset %i         -> %s \n",i,i2,varlist);

            //printf("\n");

            
            
           write_influx_curl(varlist,url);
        }


    }

    

   // printf("hhieerrrr -> %s \n",varlist);  
 //   write_influx_curl(varlist,url);

    }


}


#endif
