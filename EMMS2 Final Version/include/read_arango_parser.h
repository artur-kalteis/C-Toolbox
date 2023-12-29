#ifndef READ_ARANGO_H
#define READ_ARANGO_H


// -ljson-c -lcurl -lrt
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


  enum slave_structur{JM=0,beckhoff}; // Beschreibt was eingelesen wird
  enum slave_structur s_struct=0;


  enum beckhoff_format{ana_input=0,ana_output,dig_input,dig_output}; // is it analog in or out
  enum beckhoff_format b_format=0;

  enum write_slave_state{e_def=0,e_state_data,e_state_status,e_state_beckhoff,e_state_norm,e_state_harmonic_norm}; // Beschreibt was eingelesen wird //e_state_beckhoff=beckhoff wird eingelesen
  enum write_slave_state write_s=0;

  enum write_slave_parameter{e_default=0,e_output_data,e_par_dataset,e_par_beckhoff}; // Beschreibt was eingelesen wird
  enum write_slave_parameter write_p=0; //defualt 


	int slave_buffer=0; //buffer für die abgearbeiteten Slave objekte
  	int dataset_buffer=0; // buffer für die übergabe der einzelnen Slaves

	int first_count=0; // wird für den ersten Durchgang der foreach-Schleife benötigt
	int count_foreach=0; // counter zum befüllen der objekte

  void json_parse(json_object *jobj)
	{

		count_foreach=0;

		/*

		switch(obj_handler)	
		{
			case master:
			printf("Master wird eingelsen \n");
			break;

			case slave:
			printf("Slave wird eingelsen \n");

				
			

			break;

		}*/
		//printf("debug intern Hier \n");

			//printf("DEBUG 222 \n");
	enum json_type type;
	json_object_object_foreach(jobj, key, val)
					{	
						
						if (!strcmp(key,"_key")||!strcmp(key,"_rev")||!strcmp(key,"name")||!strcmp(key,"meta")||!strcmp(key,"type")||!strcmp(key,"hasMore")||!strcmp(key,"cached")||!strcmp(key,"error")||!strcmp(key,"code")) {continue;}


						switch(handler)	
						{
							case e_application:
							if(!strcmp(key,"slave_count"))
							{
								continue;
							}	

							if(!strcmp(key,"in16_slave_id"))
							{
								if(first_count==0)
								{
									//printf("Neuer Slave erkannt----------------1111 \n");
									first_count=1;
									count_foreach=0;
									continue;
								}
								else
								{
								
								//	uebergabe(p_slave_obj[slave_buffer],working);
									
									slave_buffer++;
									//printf("Neuer Slave erkannt----------------rest \n");
									count_foreach=0;
									continue;
								}	
						
							}
						
							
	
							break;

							case e_commando:

							if(obj_handler==1) // slave wird eingelsen
							{
								if(!strcmp(key,"_id"))
								{	
									strcpy(s_->slave_cmd_id,json_object_get_string(val));
									continue;	
								}	


								if(!strcmp(key,"slave_par_cmd")) //zum einlesen der Parameter
								{
									//CMD-Objekt wird erstellt
									printf("Output_CMD wird eingelsen \n");
									s_->slave_output_cmd_count=json_object_get_int(val);
									//Output_CMD_Array *o_c_a;
									o_c_a=(Output_CMD_Array*) malloc(sizeof(Output_CMD_Array));
									init_Output_CMD_Array(o_c_a, s_->slave_output_cmd_count);
									count_foreach=0;
									continue;								
								}

							}

							
							break;

							case e_status:

							if(obj_handler==1) // slave wird eingelsen
							{

								if(!strcmp(key,"_id"))
								{	
									
									strcpy(s_->slave_sts_id,json_object_get_string(val));
									s_->check_status=0;
									continue;	
								}	


								if(!strcmp(key,"slave_par_status"))
								{	 
									
									printf("Status wird eingelesens \n");

									s_->slave_output_status_count=json_object_get_int(val);
									o_s_a=(Output_Status_Array *) malloc(sizeof(Output_Status_Array));
									init_Output_Status_Array(o_s_a, s_->slave_output_status_count);
	
									count_foreach=0;
									continue;
								}
							}
							

							break;

							case e_state:
							if(obj_handler==1) // slave wird eingelsen
							{
								
								if(!strcmp(key,"_id"))
								{	
									strcpy(s_->slave_state_id,json_object_get_string(val));
									
									continue;	
								}	

								//printf("-------------READ STATUS FROM ARANGODB----------- value %i name $s \n",json_object_get_int(val),key);

								if(!strcmp(key,"slave_state_datasets"))
								{	
									printf("Slave State wird eingelsen \n");
									s_struct=0; //JM-Slave wird eingelesen
								}

								if(!strcmp(key,"beckhoff_ana_in_data"))
								{	
									
									printf("Beckhoff Slave wird eingelesen counter=%i \n",json_object_get_int(val));

									s_->beckoff_slave_ana_in_count=json_object_get_int(val);
									b_s_a_i=(Beckhoff_Slave_Array *) malloc(sizeof(Beckhoff_Slave_Array ));
									init_Beckhoff_Array(b_s_a_i, s_->beckoff_slave_ana_in_count);

									write_s=3;
									count_foreach=0;
									s_struct=1; // Beckhoff-Slave wird eingelesen
									b_format=0;	
									continue;
								}


								
								
								if(s_struct==0) // if JM-Slave
								{

								
								if(!strcmp(key,"no"))
								{	
									printf("nummer wird eingelsen \n");
									if(first_count==0)
									{
										first_count=1;
									}
									else
									{

										printf("----------------->SAVE DATASET<---------------------------\n");

										s_->d_set[dataset_buffer].data=d_a;
										s_->d_set[dataset_buffer].status=s_a;

									}	
									dataset_buffer=json_object_get_int(val);
									//printf("dataset_buffer %i ---------------------------\n",dataset_buffer);
									continue;
								}
								if(!strcmp(key,"dataset_name"))
								{	
									printf("Name wurde eingelsen \n");
					
									strcpy(s_->d_set[dataset_buffer].dataset_name,json_object_get_string(val));
									write_s=0;
									count_foreach=0;
									//printf("Name wurde eingelsen \n");
									continue;
								}

							

								if(!strcmp(key,"data"))
								{	
									printf("data wird eingelsen \n");

									s_->d_set[dataset_buffer].dataset_data_array_count=json_object_get_int(val);
									printf("data wird eingelsen debug 1 \n");
									d_a=(Data_Array*) malloc(sizeof(Data_Array));
									printf("data wird eingelsen debug 2 \n");
									printf("DATASET COUNTER %i \n",dataset_buffer);
									printf("DATASET ARRAY COUNTE %i \n",s_->d_set[dataset_buffer].dataset_data_array_count);
									init_Data_Array(d_a, s_->d_set[dataset_buffer].dataset_data_array_count);
									printf("data wird eingelsen debug 3 \n");
									write_s=1;
									continue;				
								}


								if(!strcmp(key,"norm"))
								{		
									printf("Norm wurde eingelsen \n");	
								
									count_foreach=0;
									write_s=4;
									continue;				
								}

								if(!strcmp(key,"harmonic_norm"))
								{		
									printf("Harmonic Norm wurde eingelsen \n");	
								
									count_foreach=0;
									write_s=5;
									continue;				
								}

								

								if(!strcmp(key,"status"))
								{	
									
									printf("Status wird eingelsen \n");
									s_->d_set[dataset_buffer].dataset_status_array_count=json_object_get_int(val);
									s_a=(Status_Array*) malloc(sizeof(Status_Array));
									init_Status_Array(s_a, s_->d_set[dataset_buffer].dataset_status_array_count);
									write_s=2;
									continue;	
								}

							}
							}
							break;

							case e_parameter:
							if(obj_handler==1) // slave wird eingelsen
							{

								if(!strcmp(key,"_id"))
								{	
									strcpy(s_->slave_par_id,json_object_get_string(val));
									continue;	
								}	

							//if(write_p==1) // slave wird eingelsen
							//{


								if(!strcmp(key,"beckhoff_ana_out_data"))
								{	
	
									printf("Beckhoff Slave wird eingelesen counter=%i \n",json_object_get_int(val));

									s_->beckoff_slave_ana_out_count=json_object_get_int(val);
									b_s_a_o=(Beckhoff_Slave_Array *) malloc(sizeof(Beckhoff_Slave_Array ));
									init_Beckhoff_Array(b_s_a_o, s_->beckoff_slave_ana_out_count);

									write_p=2;
									count_foreach=0;
									s_struct=1; // Beckhoff-Slave wird eingelesen
									b_format=1;	
									continue;
								}

								
							if(!strcmp(key,"slave_par_output"))
							{	
								printf("Slave wird eingelsen (slave_par_output) \n");
								s_struct=0; //JM-Slave wird eingelesen
								write_p=0;
								first_count=0; 
								count_foreach=0;
								//continue;
							}	

							if(!strcmp(key,"data")) //zum einlesen der Parameter
							{	
								

								//Data-Objekt wird erstellt
								printf("Output_Data wird eingelsen \n");
								s_->slave_output_data_count=json_object_get_int(val);
								//Output_Data_Array *o_d_a;
								o_d_a=(Output_Data_Array*) malloc(sizeof(Output_Data_Array));
								init_Output_Data_Array(o_d_a, s_->slave_output_data_count);
								write_p=1;
								continue;							
							}

							

						
							
							if(!strcmp(key,"no")) // Dataset mit der nummer no ausgelesen
							{	
								

								if(write_p==1)
								{
									printf("Save Output Data in Slave Objekt\n");
								s_->output_data=o_d_a; // this should be done only once
								
								s_->output_cmd=o_c_a;

							
								}

								
								
								if(first_count==0) //Beim ersten einlesen des Datasets
								{
									first_count=1;
									printf("#########################INIT#########################################################\n");
									printf("#########################%i#########################################################\n",slave_datasets[slave_x]);
									
									
								 // datasets werden fÃ¼r den Slave reserviert
								}
								//nicht sicher ob das wirklich sein muss
								else
								{
							//		printf("######################## ein Weiterer Datensatz#########################################################\n");
									insert_Slave(s_, d_);  //datensats wird dem Slave hinzugefügt

								}	
								

								dataset_buffer=json_object_get_int(val); // notwendig?
								write_p=3;	
								count_foreach=0;
								
															
							}
							
							
							
							//}
							}
							break;

						}	

						
						//printf("DEBUG 333 \n");

					type= json_object_get_type(val);
					switch (type)
									{
							
									case json_type_int:
									//printf("%s\t", key); 	
									//printf("Value=%i \n", json_object_get_int(val));

									//if(handler==5 && s_struct==0) 
									if(handler==5) 
									{

										
										if(count_foreach < 3 && write_p==0) // einlesen von slave_par_output
										{

										printf("counter %i ------------------------------------------> Value %i \n",count_foreach,json_object_get_int(val));
										write_slave_par(s_, count_foreach, json_object_get_int(val)); 		
										count_foreach++;
										}
										if(write_p==1) // einlesen von slave_par_output/data
										{
											//printf("value parameterdata-val %i \n",json_object_get_int(val));
											Output_Data o_d;
											o_d.name=(char*)malloc(strlen(key) + 1);
											//printf("value parameterdata-name %s \n",key);
											strcpy(o_d.name,key);
											//printf("Key saved as  %s \n",o_d.name);
											o_d.value=json_object_get_int(val);
											insert_Output_Data_Array(o_d_a, o_d);
											free(o_d.name);
										}

										if(write_p==3) // einlesen von slave_par_output
										{
											//printf("counter %i \n",count_foreach);
											printf("key %s value parameter %i ,dataset_buffer %i ,count_foreach %i \n",key,json_object_get_int(val),dataset_buffer, count_foreach);
										

										//	printf("######################--------------------> \n");


										//	printf("######################----counter=%i --------------Value=%i--> \n",count_foreach, json_object_get_int(val));

										//	printf("######################--------------------> \n");


											write_slave_par_dataset(d_,count_foreach, json_object_get_int(val)); 	
											
											
											count_foreach++;

										}
											
										



										if(write_p==2 && b_format==1) // beckhoff slave wird befüllt
										{

											//printf(" Beckhoff eingelesen %i \n",count_foreach);	
											Beckhoff_Data b_d;
											b_d.name=(char*)malloc(strlen(key) + 1);
											strcpy(b_d.name,key);
											
											strncpy(b_d.unit,key,4); //will copy first 4 characters from str.
											
											printf("reading arango name out %s and value %i\n",b_d.name,json_object_get_int(val));
											printf("unit %s\n",b_d.unit);
											
											b_d.unit[4]='\0';

											b_d.value=json_object_get_int(val);
											b_d.u_value=json_object_get_int(val);
											*((int32_t*)&b_d.f_value)=json_object_get_int(val);

											insert_Beckhoff_Array(b_s_a_o, b_d);
											free(b_d.name);

											
											count_foreach++;

											if(s_->beckoff_slave_ana_out_count==count_foreach)
											{
												s_->beckhoff_data_out=b_s_a_o;

												printf("DEbug hier 111\n");
											}
											
											
										}





									}
									if(handler==1)
									{	
										

										
										count_foreach++;

										

									}	
									
									

									if(handler==6)
									{

										if(write_s==4) // norm
										{

											printf("saving int Norm value \n");	

											d_a->d_array[count_foreach].norm=json_object_get_int(val);
											//s_->d_set[dataset_buffer].data->d_array[count_foreach].norm=json_object_get_int(val);
											count_foreach++;
										}

										if(write_s==5) // norm
										{
	
											printf("saving int harmonic Norm value \n");	
											int harmonic_count=0;
	
											for(harmonic_count=0; harmonic_count<51; harmonic_count++)  // hier wird der selbe wert immer wieder in einen anderen Array abgespeichert
											{
												d_a->d_array[count_foreach].norm=json_object_get_int(val);
												//s_->d_set[dataset_buffer].data->d_array[count_foreach].norm=json_object_get_int(val);
												
												count_foreach++;
	
											}
										
	
										}

										
										if(count_foreach < 2 && write_s==0)
										{
										//printf("Dataset-HeaderValue %i   ###################->>>>>>>>>>>>>>>>>>>>> \n",json_object_get_int(val));
										write_slave_par_dataset2(s_,dataset_buffer ,count_foreach, json_object_get_int(val)); 		
										count_foreach++;

										//printf("Dataset serialized %i\n", m_->slave_objekt[0].d_set[dataset_buffer].dataset_serialized);
										//printf("Dataset interrupt %i\n", m_->slave_objekt[0].d_set[dataset_buffer].dataset_interrupt);
										//printf("Dataset timestamp %i\n", m_->slave_objekt[0].d_set[dataset_buffer].dataset_timestamp);

										}

										if(write_s==1)
										{
											//printf("value state %i \n",json_object_get_int(val));

											if(json_object_get_int(val)>1) //Harmonic values
											{
												int z=0;
												char buffer[64]="";

												for(z=0;z< json_object_get_int(val); z++)
												{
													
														Data d;	
														
														sprintf(buffer,"%s_%i",key,z);
														d.name=(char*)malloc(strlen(buffer) + 1);
														strcpy(d.name,buffer);

														strncpy(d.unit,key,4); //will copy first 4 characters from str.
														

														printf(" --------------------Data name =%s\n",d.name);
														printf(" --------------------Data value =%i\n",json_object_get_int(val));
														
														
														d.unit[4]='\0';
														
														d.value=json_object_get_int(val);
														d.u_value=json_object_get_int(val);
														*((int32_t*)&d.f_value)=json_object_get_int(val);
														insert_Data_Array(d_a, d);
														free(d.name);

												}


											}
											else
											{	

												Data d;
												d.name=(char*)malloc(strlen(key) + 1);
												strcpy(d.name,key);

											
												
												strncpy(d.unit,key,4); //will copy first 4 characters from str.
												printf(" --------------------Data name =%s\n",d.name);
												printf(" --------------------Data value =%i\n",json_object_get_int(val));
												
												
												
												d.unit[4]='\0';

												d.value=json_object_get_int(val);
												d.u_value=json_object_get_int(val);
												*((int32_t*)&d.f_value)=json_object_get_int(val);
												insert_Data_Array(d_a, d);
												free(d.name);

											}

											

										}

										if(write_s==2)
										{
											//printf("value state %i \n",json_object_get_int(val));
											Status s;
											s.name=(char*)malloc(strlen(key) + 1);
											strcpy(s.name,key);
											s.value=json_object_get_int(val);
											insert_Status_Array(s_a, s);
											free(s.name);
											
										}

										if(write_s==3 && b_format==0) // beckhoff slave wird befüllt
										{

											//printf(" Beckhoff eingelesen %i \n",count_foreach);	
											Beckhoff_Data b_d;
											b_d.name=(char*)malloc(strlen(key) + 1);
											strcpy(b_d.name,key);
											
											strncpy(b_d.unit,key,4); //will copy first 4 characters from str.
											
											printf("reading arango analog in %s value %i \n",b_d.name,json_object_get_int(val));
											printf("unit %s\n",b_d.unit);
											
											b_d.unit[4]='\0';

											b_d.value=json_object_get_int(val);
											b_d.u_value=json_object_get_int(val);
											*((int32_t*)&b_d.f_value)=json_object_get_int(val);

											insert_Beckhoff_Array(b_s_a_i, b_d);
											free(b_d.name);

											
											count_foreach++;

											if(s_->beckoff_slave_ana_in_count==count_foreach)
											{
												s_->beckhoff_data_in=b_s_a_i;
											}
											
											//printf("DEbug 111\n");
										}

										

										
									}	
										
												
									break;
									case json_type_string:
									if (!strcmp(key,"_key")||!strcmp(key,"_id")||!strcmp(key,"_rev")){break;}
									if (!strcmp(key,"char_name")||!strcmp(key,"char_type")||!strcmp(key,"char_position")){break;}
														
								//	printf("%s\t", key); 
								//	printf("%s\n", json_object_get_string(val));
									
									break;
									case json_type_null:
								//	printf("%s\t", key); 
								//	printf("%s\n", json_object_get_string(val));
								
									break;
									case json_type_boolean:

									if(handler==5)
									{
										if(write_p==2 && b_format==1) // beckhoff slave wird befüllt
										{

											//printf(" Beckhoff eingelesen %i \n",count_foreach);	
											Beckhoff_Data b_d;
											b_d.name=(char*)malloc(strlen(key) + 1);
											strcpy(b_d.name,key);
											
											strncpy(b_d.unit,key,4); //will copy first 4 characters from str.
											
											//printf("reading arango value analog out %s\n",b_d.name);
											//printf("unit %s\n",b_d.unit);
											
											b_d.unit[4]='\0';

											//b_d.value=json_object_get_int(val);
											//b_d.u_value=json_object_get_int(val);
											*((int32_t*)&b_d.f_value)=strtod(json_object_get_string(val),NULL);

											insert_Beckhoff_Array(b_s_a_o, b_d);
											free(b_d.name);

											
											count_foreach++;

											if(s_->beckoff_slave_ana_out_count==count_foreach)
											{
												s_->beckhoff_data_out=b_s_a_o;
											}
											
											//printf("DEbug 111\n");
										}


									}


									if(handler==1)
									{	
										Output_CMD c_d;
										c_d.name=(char*)malloc(strlen(key) + 1);	
									//	printf("value parameterdata-cmd name %s \n",key);									
										strcpy(c_d.name,key);										
										c_d.value=json_object_get_int(val);	
										
										printf("-----------------CMD----------------\n");
										printf("Name %s \n",c_d.name);
										printf("Value %i\n",c_d.value);
										printf("----------------------------------------\n");

										insert_Output_CMD_Array(o_c_a, c_d);
										free(c_d.name);

											count_foreach++;
										
											if(s_->slave_output_cmd_count==count_foreach)
											{
												s_->output_cmd=o_c_a;
											}

									}
									
									if(handler==2)
									{
										//printf(" Beckhoff eingelesen %i \n",count_foreach);	
										Output_CMD_Status o_c_s;
										o_c_s.name=(char*)malloc(strlen(key) + 1);
										strcpy(o_c_s.name,key);
										o_c_s.value=json_object_get_int(val);

										printf("-----------------Status----------------\n");
										printf("Name %s \n",o_c_s.name);
										printf("Value %i\n",o_c_s.value);
										printf("----------------------------------------\n");

										
										insert_Output_Status_Array(o_s_a, o_c_s);
										free(o_c_s.name);

										

										
										count_foreach++;

										if(s_->slave_output_status_count==count_foreach)
										{
											s_->output_status=o_s_a;
										}
									}
									

									if(handler==6)
									{

										
										if(count_foreach < 2 && write_s==0)
										{
										



									//	printf("Dataset-HeaderValue %i   ###################->>>>>>>>>>>>>>>>>>>>> \n",json_object_get_int(val));
										write_slave_par_dataset2(s_,dataset_buffer ,count_foreach, json_object_get_int(val)); 	 		
										count_foreach++;

									//	printf("Dataset serialized %i\n", m_->slave_objekt[0].d_set[dataset_buffer].dataset_serialized);
									//	printf("Dataset interrupt %i\n", m_->slave_objekt[0].d_set[dataset_buffer].dataset_interrupt);
									//	printf("Dataset timestamp %i\n", m_->slave_objekt[0].d_set[dataset_buffer].dataset_timestamp);


										}


										
									}	
									break;
									case json_type_double:
								//	printf("%s\t", key); 
								//	printf("%s\n", json_object_get_string(val));

									if(handler==6) // Status
									{

									
									if(write_s==4) // norm
									{
										printf("saving float  Norm value \n");	

										d_a->d_array[count_foreach].norm=strtod(json_object_get_string(val),NULL);
										//s_->d_set[dataset_buffer].data->d_array[count_foreach].norm=strtod(json_object_get_string(val),NULL);
									
										count_foreach++;

									}

									if(write_s==5) // norm
									{

										printf("saving float Harmonic Norm value \n");	
										int harmonic_count=0;

										for(harmonic_count=0; harmonic_count<51; harmonic_count++)  // hier wird der selbe wert immer wieder in einen anderen Array abgespeichert
										{	
											d_a->d_array[count_foreach].norm=strtod(json_object_get_string(val),NULL);
											//s_->d_set[dataset_buffer].data->d_array[count_foreach].norm=strtod(json_object_get_string(val),NULL);
											
											count_foreach++;

										}
									

									}
									}	
									
									break;
									case json_type_object:
								//	printf("%s\t", key); 
								//	printf("%s\n", json_object_get_string(val));
									
									break;
									case json_type_array: 

									json_parse_array(jobj, key);

									break;
									}
								
					}

					//output_slave_obj(s_);			
							
}


 size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)//to mute a input
 {
	return size * nmemb;
 }



 void read_arango_curl(char s1[64],char s2[64], char s3[256],char s4[64]) //s=collection s2=collection_key s3=URL s4=login
 {
	
	char URL[256];
	sprintf(URL,"");
	strcat(URL,s3); 

	char post[1240000];
	 
	strcat(post,"");
	 
	 
	static const char *postthis;
	 
	sprintf(post,"{\"query\" : \"For a in device Filter a._id == '%s' For sp IN ANY a belongs_to OPTIONS {bfs: true, uniqueVertices: 'global'} Filter IS_SAME_COLLECTION('%s',sp._id) return sp\"}",s2,s1);
	 
	printf("URL %s \n",URL);
	printf("Query %s \n",post);


	 

	 CURL *curl = curl_easy_init();

	
	 CURLcode res;
	 if(curl) {
			 struct string s;
		 init_string(&s);
		 curl_easy_setopt(curl, CURLOPT_URL, URL);
		 curl_easy_setopt(curl, CURLOPT_USERPWD, s4); //s4=login
		 postthis=post;
		 curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);
		 curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
		 curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
		 
		 res = curl_easy_perform(curl);
		
			 json_object *jobj = json_tokener_parse(s.ptr);
			 curl_easy_cleanup(curl);
		
	 
		  // printf("debug intern 1 \n");
			 json_parse(jobj);

		
		  
		    switch(handler)	
		   {
			case e_parameter: 

			//b_s_a_o

			if(write_p!=2) // bei Beckhoff nicht notwendig 
			{	
				insert_Slave(s_, d_); // parameter muss zuerst eingelesen werden
			}	
				
				 
				 printf("debug ende----------- \n");
		    break;
		    case e_state:
		        s_->d_set[dataset_buffer].data=d_a; // anschließend werden die state Varibalen hinzugefügt
				s_->d_set[dataset_buffer].status=s_a;
			break;
			
			case e_status:
			
			break;

			case e_commando:
			
			break;
			
			default:
			   printf("not definde\n");
			break;

		   }
		 
		 
	 }
      
 
 }



#endif
