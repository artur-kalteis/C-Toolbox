#ifndef SLAVE_OBJ_H
#define SLAVE_OBJ_H


typedef struct
{   
    char unit[5];

    int32_t value;
    int32_t u_value;
    float f_value;



    

    char * name;
} Beckhoff_Data;    


typedef struct
{
    char unit[5];
    int16_t value;
    uint16_t u_value;
    int32_t f_value;
    float buffer_f;     // buffer to store into InfluxDB
    int16_t buffer_i;   // buffer to store into InfluxDB
    uint16_t buffer_ui;   // buffer to store into InfluxDB

    double norm;
    
    

    char * name;
} Data;

typedef struct
{
    int32_t value;
    char * name;
} Output_Data;

typedef struct
{
    int value;
    int buffer; // buffer to save the value from the last cycle

    char * name;
} Output_CMD;

typedef struct
{
    int32_t value;
    char * name;
} Status;

typedef struct
{
    int32_t value;
    char * name;
} Output_CMD_Status;


typedef struct
{
    Beckhoff_Data *beckhoff_s_array;
    size_t used; //z.B 6 slaves sind befüllt in dieser struktur
    size_t size; //z.B. 10 slaves

} Beckhoff_Slave_Array;

typedef struct
{
    Output_Data *output_d_array;
    size_t used; //z.B 6 slaves sind befüllt in dieser struktur
    size_t size; //z.B. 10 slaves

} Output_Data_Array;

typedef struct
{
    Output_CMD *output_cmd_array;
    size_t used; //z.B 6 slaves sind befüllt in dieser struktur
    size_t size; //z.B. 10 slaves

} Output_CMD_Array;


// array of structs
typedef struct
{
    Status *s_array;
    size_t used; //z.B 6 slaves sind befüllt in dieser struktur
    size_t size; //z.B. 10 slaves

} Status_Array;

typedef struct
{
    Data *d_array;
    size_t used; //z.B 6 slaves sind befüllt in dieser struktur
    size_t size; //z.B. 10 slaves

} Data_Array;

typedef struct
{
    Output_CMD_Status *output_status_array;
    size_t used; //z.B 6 slaves sind befüllt in dieser struktur
    size_t size; //z.B. 10 slaves

} Output_Status_Array;


typedef struct
{
        
        //int testweise;


        //Slave Parameter Dataset, beschreiben die Ethercat-Variablen die angesteuert werden müssen
        
        //to send
        int dataset_no;
        int dataset_addr_var_out; //ethercat variable

        int dataset_cmd_var_out; //ethercat variable
        int dataset_cmd_var_read; //bool position of a ethercat variable
        int dataset_cmd_var_interrupt; //bool position of a ethercat variable

    	//to read    

        int dataset_data_var_input; // ehtercat var
        int dataset_status_var_input; // ehtercat var
        int dataset_addr_var_input; // ehtercat var
        int dataset_time_var_input; // ehtercat var   

        
        
        //Slave State Dataset    
        //for working cycle, not for upload
        
        int dataset_serialized;
        int dataset_interrupt;
        int dataset_counter;

        int dataset_addr_send;
        int dataset_act_addr;// for the actual address of the dataset
        int dataset_act_last_addr; // to check if new address is not the old one    
        int dataset_act_check;// Checkvariable if status is already read out

        int dataset_act_8bit_array[8];// working status arrays

        //int AIB_status[8];  // only for aib slaves
        //int AIB_cmd;        // only for aib slaves
        
    

        //for store date
        char dataset_name[32];
        int dataset_timestamp;

        int dataset_data_array_count;
        Data_Array *data;
        int dataset_status_array_count;
        Status_Array *status;

} Dataset;


//#define dataset_arango 4 // wird später aus der arangodb eingelsen

typedef struct 
{
    

    //Master Parameter     // wird mit dem Shell-Script ausgelesen
    int slave_id;
    char slave_name[64];
    int slave_position;
    
    int slave_dig_in;
    int slave_dig_out;
    int slave_ana_in;
    int slave_ana_out;

    int slave_serialized;
    int check_status;

    size_t used; 
    size_t size; 


    int slave_dataset_count; // wenn ==0 dann Beckhoff slave!!!
    Dataset *d_set;

    //Beckhoff Klemmen

   // int beckhoff_slave_count; // wird die größe des Arrays festgehalten
   // Beckhoff_Slave_Array *beckhoff_data;

    int beckoff_slave_ana_in_count;
    Beckhoff_Slave_Array  *beckhoff_data_in;

    int beckoff_slave_ana_out_count;
    Beckhoff_Slave_Array  *beckhoff_data_out;
    //Slave Parameter Output //für den Ausgang gibt es nur einen Datensatz

    int slave_data_output;
    int slave_addr_output;
    int slave_cmd_output;

    char slave_par_id[16];
    char slave_sts_id[16];
    char slave_state_id[16];
    char slave_cmd_id[16];


    int slave_output_data_count; // wird die größe des Arrays festgehalten
    Output_Data_Array *output_data;

    int slave_output_cmd_count;  // wird die größe des Arrays festgehalten
    Output_CMD_Array *output_cmd;

    // new for AIB 
    int slave_output_status_count; // wird die größe des Arrays festgehalten
    Output_Status_Array *output_status;

}Slave;


// hier wird in ein bereits vorhandenen Datensatz geschrieben
void write_slave_par_dataset2(Slave *ob1,int dataset_buffer, int count, int value)
{
    switch(count)
    {
      //  case 0:
       // ob1->d_set[dataset_buffer].dataset_serialized=value;
       // break;
        case 0:
        ob1->d_set[dataset_buffer].dataset_interrupt=value;
        break;
        case 1:
        ob1->d_set[dataset_buffer].dataset_timestamp=value;
        break;
           
    }

}
//hier wird ein neuer Datensatz beschrieben und  muss dann mit init_Slave(s_,d_);  an den jeweiligen Slave übergeben werden
void write_slave_par_dataset(Dataset *ob1,int count, int value)
{

    switch(count)
    {
        case 0:
        ob1->dataset_no=value;
        break;
        case 1:
        ob1->dataset_addr_var_out=value;
        break;
        case 2:
        ob1->dataset_cmd_var_out=value;
        break;
        case 3:
        ob1->dataset_cmd_var_read=value;
        break;
        case 4:
        ob1->dataset_cmd_var_interrupt=value;
        break;
        case 5:
        ob1->dataset_data_var_input=value;
        break;
        case 6:
        ob1->dataset_addr_var_input=value;
        break;
        case 7:
        ob1->dataset_status_var_input=value;
        break;  
        case 8:
        ob1->dataset_time_var_input=value;
        break;   
    }

}

void write_slave_par(Slave *ob1, int count, int value)
{

    switch(count)
    {
        case 0:
        ob1->slave_data_output=value;
        break;
        case 1:
        ob1->slave_addr_output=value;
        break;
        case 2:
        ob1->slave_cmd_output=value;
        break;     

    }

}


void output_slave_obj(Slave *s, int x)
{
    printf("Slave Objekt Ausgabe \n");
    int i=0;
  /*  printf("%i \n",s->slave_id);
    printf("%s \n",s->slave_name);
    printf("%i \n",s->slave_position);
    printf("%i \n",s->dataset_count);
    printf("%i \n",s->dig_in);
    printf("%i \n",s->dig_out);
    printf("%i \n",s->ana_in);
    printf("%i \n",s->ana_out);
    printf("%i \n",s->slave_data_output);
    printf("%i \n",s->slave_addr_output);
    printf("%i \n",s->slave_cmd_output);*/

   /* printf("Output-Data--------------------------\n");
    printf("%i \n",s->slave_output_data_count);
    
    for(i=0; i< s->slave_output_data_count; i++)
    {
        printf("%s\n", s->output_data->output_d_array[i].name); 
        printf("%i\n", s->output_data->output_d_array[i].value);        
        
    }
    printf("Outut-CMD---------------------------\n");
    printf("%i \n",s->slave_output_cmd_count);
    for(i=0; i< s->slave_output_cmd_count; i++)
    {
        printf("%s\n", s->output_cmd->output_cmd_array[i].name);
        printf("%i\n", s->output_cmd->output_cmd_array[i].value);        
          
    }
    
    //int beckhoff_slave_count; // wird die größe des Arrays festgehalten
    //Beckhoff_Slave_Array *beckhoff_data;


    printf("Outut-Beckhoff-Slave---------------------------\n");
    printf("%i \n",s->beckhoff_slave_count);
    for(i=0; i< s->beckhoff_slave_count; i++)
    {
        printf("%s\n", s->beckhoff_data->beckhoff_s_array[i].name);
        printf("%i\n", s->beckhoff_data->beckhoff_s_array[i].value);        
          
    }

    
   
    int i2=0;


    for(i=0; i< dataset_count; i++)
    {
    printf("Outout-DATASET-Intern %i ---------------------------\n",i);
    printf("data \n");
    for(i2=0; i2< s->d_set[i].dataset_data_array_count; i2++)
    {
        printf("%s\n",s->d_set[i].data->d_array[i2].name);
        printf("%i\n",s->d_set[i].data->d_array[i2].value);                 
    }

    printf("status \n");
    for(i2=0; i2< s->d_set[i].dataset_status_array_count; i2++)
    {
        printf("%s\n",s->d_set[i].status->s_array[i2].name);
        printf("%i\n",s->d_set[i].status->s_array[i2].value);                   
    }

    }
    



        for(i=0;i < dataset_count;i++)
        {
        printf("Outout-DATASET-Main---------------------------\n");
        printf("%i \n",s->d_set[i].dataset_no);
        printf("%i \n",s->d_set[i].dataset_addr_var_out);
        printf("%i \n",s->d_set[i].dataset_addr_var_start);
        printf("%i \n",s->d_set[i].dataset_addr_var_end);

        printf("%i \n",s->d_set[i].dataset_cmd_var_out);
        printf("%i \n",s->d_set[i].dataset_cmd_var_read);
        printf("%i \n",s->d_set[i].dataset_cmd_var_interrupt);

        printf("%i \n",s->d_set[i].dataset_data_var_input);
        printf("%i \n",s->d_set[i].dataset_status_var_input);
        printf("%i \n",s->d_set[i].dataset_time_var_input);
            
        }
    
*/
}


// Initialisierung  eines Slaves mit i Datasets
void init_Slave(Slave *a, size_t initialSize)  
{
    // Allocate initial space
    a->d_set = (Dataset *)malloc(initialSize * sizeof(Dataset));

    a->used = 0;           // no elements used
    a->size = initialSize; // available nr of elements

    int i=0;

    // Initialize all values of the array to 0
    for(i = 0; i<initialSize; i++)
    {
        memset(&a->d_set[i],0,sizeof(Dataset));
    }
}



void init_Beckhoff_Array(Beckhoff_Slave_Array *a, size_t initialSize)
{
    // Allocate initial space
    a->beckhoff_s_array = (Beckhoff_Data *)malloc(initialSize * sizeof(Beckhoff_Data));

    a->used = 0;           // no elements used
    a->size = initialSize; // available nr of elements

    int i=0;

    // Initialize all values of the array to 0
    for(i = 0; i<initialSize; i++)
    {
        memset(&a->beckhoff_s_array[i],0,sizeof(Beckhoff_Data));
    }
}



void init_Output_Data_Array(Output_Data_Array *a, size_t initialSize)
{
    // Allocate initial space
    a->output_d_array = (Output_Data *)malloc(initialSize * sizeof(Output_Data));

    a->used = 0;           // no elements used
    a->size = initialSize; // available nr of elements

    int i=0;

    // Initialize all values of the array to 0
    for(i = 0; i<initialSize; i++)
    {
        memset(&a->output_d_array[i],0,sizeof(Output_Data));
    }
}


void init_Output_CMD_Array(Output_CMD_Array *a, size_t initialSize)
{
    // Allocate initial space
    a->output_cmd_array = (Output_CMD *)malloc(initialSize * sizeof(Output_CMD));

    a->used = 0;           // no elements used
    a->size = initialSize; // available nr of elements

    int i=0;

    // Initialize all values of the array to 0
    for(i = 0; i<initialSize; i++)
    {
        memset(&a->output_cmd_array[i],0,sizeof(Output_CMD));
    }
}

void init_Output_Status_Array(Output_Status_Array *a, size_t initialSize)
{
    // Allocate initial space
    a->output_status_array = (Output_CMD_Status *)malloc(initialSize * sizeof(Output_CMD_Status));

    a->used = 0;           // no elements used
    a->size = initialSize; // available nr of elements

    int i=0;

    // Initialize all values of the array to 0
    for(i = 0; i<initialSize; i++)
    {
        memset(&a->output_status_array[i],0,sizeof(Output_CMD_Status));
    }
}



void init_Data_Array(Data_Array *a, size_t initialSize)
{
    // Allocate initial space

    printf("debug 1\n"); 
    a->d_array = (Data *)malloc(2* initialSize * sizeof(Data));
    printf("debug 2\n"); 
    a->used = 0;           // no elements used
    a->size = initialSize; // available nr of elements

    int i=0;

    // Initialize all values of the array to 0
    for(i = 0; i<initialSize; i++)
    {
        printf("MEMSET vorher\n");    
        memset(&a->d_array[i],0,2*sizeof(Data));
        printf("MEMSET nachher\n"); 
    }
}



void init_Status_Array(Status_Array *a, size_t initialSize)
{
    // Allocate initial space
    a->s_array = (Status *)malloc(initialSize * sizeof(Status));

    a->used = 0;           // no elements used
    a->size = initialSize; // available nr of elements

    int i=0;

    // Initialize all values of the array to 0
    for(i = 0; i<initialSize; i++)
    {
        memset(&a->s_array[i],0,sizeof(Status));
    }
}


void insert_Output_Status_Array(Output_Status_Array *a, Output_CMD_Status element) 
{
    if (a->used == a->size) // wenn size erreicht wurde wird es verdoppelt
    {
        a->size *= 2;
        a->output_status_array = (Output_CMD_Status *)realloc(a->output_status_array, a->size * sizeof(Output_CMD_Status));
    }

    // Copy name
    a->output_status_array[a->used].name = (char*)malloc(strlen(element.name) + 1);
    strcpy(a->output_status_array[a->used].name, element.name);

    // Copy ID
    a->output_status_array[a->used].value=element.value;

    a->used++;
}



void insert_Slave(Slave *a, Dataset *element) 
{
    if (a->used == a->size) // wenn size erreicht wurde wird es verdoppelt
    {
        a->size *= 2;
        a->d_set = (Dataset *)realloc(a->d_set, a->size * sizeof(Dataset));
    }

    // Copy name
    //a->d_set[a->used]=(Dataset*)malloc(sizeof(element));
    
    a->d_set[a->used]=*element;
    
    a->used++;
}



void insert_Beckhoff_Array(Beckhoff_Slave_Array *a, Beckhoff_Data element) 
{
    if (a->used == a->size) // wenn size erreicht wurde wird es verdoppelt
    {
        a->size *= 2;
        a->beckhoff_s_array = (Beckhoff_Data *)realloc(a->beckhoff_s_array, a->size * sizeof(Beckhoff_Data));
    }

    // Copy name
    a->beckhoff_s_array[a->used].name = (char*)malloc(strlen(element.name) + 1);
    strcpy(a->beckhoff_s_array[a->used].name, element.name);

    // Copy ID
    a->beckhoff_s_array[a->used].value=element.value;
    a->beckhoff_s_array[a->used].u_value=element.u_value;
    a->beckhoff_s_array[a->used].f_value=element.f_value;
    strcpy(a->beckhoff_s_array[a->used].unit,element.unit);

    a->used++;
}


/*void add_Slave_dataset(Slave *a,int i, Dataset *element) 
{
   // strcpy(a->d_set[i].dataset_name,element->dataset_name);
    a->d_set[i].dataset_serialized=element->dataset_serialized;
    a->d_set[i].dataset_interrupt=element->dataset_interrupt;
    a->d_set[i].dataset_timestamp=element->dataset_timestamp;
    //a->d_set[i].status_array_count=element->status_array_count;
    //a->d_set[i].data_array_count=element->data_array_count;

  //  a->d_set[i].status=element->status;
  //  a->d_set[i].data=element->data;
}*/

void insert_Output_CMD_Array(Output_CMD_Array *a, Output_CMD element) 
{
    if (a->used == a->size) // wenn size erreicht wurde wird es verdoppelt
    {
        a->size *= 2;
        a->output_cmd_array = (Output_CMD *)realloc(a->output_cmd_array, a->size * sizeof(Output_CMD));
    }

    // Copy name
    a->output_cmd_array[a->used].name = (char*)malloc(strlen(element.name) + 1);
    strcpy(a->output_cmd_array[a->used].name, element.name);

    // Copy ID
    a->output_cmd_array[a->used].value=element.value;

    a->used++;
}



void insert_Output_Data_Array(Output_Data_Array *a, Output_Data element) 
{
    if (a->used == a->size) // wenn size erreicht wurde wird es verdoppelt
    {
        a->size *= 2;
        a->output_d_array = (Output_Data *)realloc(a->output_d_array, a->size * sizeof(Output_Data));
    }

    // Copy name
    a->output_d_array[a->used].name = (char*)malloc(strlen(element.name) + 1);
    strcpy(a->output_d_array[a->used].name, element.name);

    // Copy ID
    a->output_d_array[a->used].value=element.value;

    a->used++;
}


// Add element to array
void insert_Data_Array(Data_Array *a, Data element) 
{
    if (a->used == a->size) // wenn size erreicht wurde wird es verdoppelt
    {
        a->size *= 2;
        a->d_array = (Data *)realloc(a->d_array, a->size * sizeof(Data));
    }

    // Copy name
    a->d_array[a->used].name = (char*)malloc(strlen(element.name) + 1);
    strcpy(a->d_array[a->used].name, element.name);

    // Copy ID
    a->d_array[a->used].value=element.value;
    a->d_array[a->used].u_value=element.u_value;
    a->d_array[a->used].f_value=element.f_value;
    a->d_array[a->used].norm=element.norm;
    strcpy(a->d_array[a->used].unit,element.unit);

    a->used++;
}

void insert_Status_Array(Status_Array *a, Status element) 
{
    if (a->used == a->size)
    {
        a->size *= 2;
        a->s_array = (Status *)realloc(a->s_array, a->size * sizeof(Status));
    }

    // Copy name
    a->s_array[a->used].name = (char*)malloc(strlen(element.name) + 1);
    strcpy(a->s_array[a->used].name, element.name);

    // Copy value
    a->s_array[a->used].value=element.value;

    a->used++;
}


void free_Slave(Slave *a) 
{
    // Free all name variables of each array element first

    int i=0;
    for(i=0; i<a->used; i++)
    {
        free(a->d_set[i].data);
        free(a->d_set[i].status);

        a->d_set[i].data=NULL;
        a->d_set[i].status=NULL;
    }

    // Now free the array 
    free(a);
    a = NULL;

    a->used = 0;
    a->size = 0;
}





void free_Beckhoff_Array(Beckhoff_Slave_Array *a) 
{
    // Free all name variables of each array element first

    int i=0;
    for(i=0; i<a->used; i++)
    {
        free(a->beckhoff_s_array[i].name);
        a->beckhoff_s_array[i].name=NULL;
    }

    // Now free the array 
    free(a->beckhoff_s_array);
    a->beckhoff_s_array = NULL;

    a->used = 0;
    a->size = 0;
}



void free_Output_CMD_Array(Output_CMD_Array *a) 
{
    // Free all name variables of each array element first

    int i=0;
    for(i=0; i<a->used; i++)
    {
        free(a->output_cmd_array[i].name);
        a->output_cmd_array[i].name=NULL;
    }

    // Now free the array 
    free(a->output_cmd_array);
    a->output_cmd_array = NULL;

    a->used = 0;
    a->size = 0;
}

void free_Output_Data_Array(Output_Data_Array *a) 
{
    // Free all name variables of each array element first

    int i=0;
    for(i=0; i<a->used; i++)
    {
        free(a->output_d_array[i].name);
        a->output_d_array[i].name=NULL;
    }

    // Now free the array 
    free(a->output_d_array);
    a->output_d_array = NULL;

    a->used = 0;
    a->size = 0;
}

void free_Data_Array(Data_Array *a) 
{
    // Free all name variables of each array element first

    int i=0;
    for(i=0; i<a->used; i++)
    {
        free(a->d_array[i].name);
        a->d_array[i].name=NULL;
    }

    // Now free the array 
    free(a->d_array);
    a->d_array = NULL;

    a->used = 0;
    a->size = 0;
}

void free_Status_Array(Status_Array *a) 
{
    // Free all name variables of each array element first

    int i=0;
    for(i=0; i<a->used; i++)
    {
        free(a->s_array[i].name);
        a->s_array[i].name=NULL;
    }

    // Now free the array 
    free(a->s_array);
    a->s_array = NULL;

    a->used = 0;
    a->size = 0;
}


    



#endif