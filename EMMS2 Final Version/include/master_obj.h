#ifndef MASTER_OBJ_H
#define MASTER_OBJ_H


struct cmd_master_obj{

    
    int cmd_master_state;

    int cmd_master_start;
    int cmd_master_on;
    int cmd_master_off;
    int cmd_master_pause;
    int cmd_master_sleep;
    int cmd_master_standby;
    int cmd_master_reset;
};


struct sts_master_obj{
    
        int sts_master_state;

        int sts_master_start;
        int sts_master_on;
        int sts_master_off;
        int sts_master_pause;
        int sts_master_sleep;
        int sts_master_standby;
        int sts_master_reset;
    };


typedef struct
{
    size_t used; 
    size_t size; 
    int slave_count;
    Slave *slave_objekt;

}Master_Topology;


void init_Master_Topology(Master_Topology *a, size_t initialSize)  
{
    // Allocate initial space
    a->slave_objekt = (Slave *)malloc(initialSize * sizeof(Slave));

    a->used = 0;           // no elements used
    a->size = initialSize; // available nr of elements

    int i=0;

    // Initialize all values of the array to 0
    for(i = 0; i<initialSize; i++)
    {
        memset(&a->slave_objekt[i],0,sizeof(Dataset));
    }
}

void insert_Master_Topology(Master_Topology *a, Slave *element) 
{
    if (a->used == a->size) // wenn size erreicht wurde wird es verdoppelt
    {
        a->size *= 2;
        a->slave_objekt = (Slave  *)realloc(a->slave_objekt, a->size * sizeof(Slave));
    }

    // Copy name
    //a->d_set[a->used]=(Dataset*)malloc(sizeof(element));
    
    a->slave_objekt[a->used]=*element;
    
    a->used++;
}


void free_Master_Topology(Master_Topology *a) 
{
    // Free all name variables of each array element first

    int i=0;
    for(i=0; i<a->used; i++)
    {
        free(a->slave_objekt[i].d_set);
        free(a->slave_objekt[i].output_data);
        free(a->slave_objekt[i].output_cmd);

        a->slave_objekt[i].d_set=NULL;
        a->slave_objekt[i].output_data=NULL;
        a->slave_objekt[i].output_cmd=NULL;  
        
    }

    // Now free the array 
    free(a);
    a = NULL;

    a->used = 0;
    a->size = 0;
}



void output_master_cmd_obj(struct cmd_master_obj *ob1)
{  
    
    printf("Output_cmd master state %i \n",ob1->cmd_master_state);


    printf("Output_cmd master %i \n",ob1->cmd_master_start);
    printf("Output_cmd master %i \n",ob1->cmd_master_on);
    printf("Output_cmd master %i \n",ob1->cmd_master_off);
    printf("Output_cmd master %i \n",ob1->cmd_master_pause);
    printf("Output_cmd master %i \n",ob1->cmd_master_sleep);
    printf("Output_cmd master %i \n",ob1->cmd_master_standby);
    printf("Output_cmd master %i \n",ob1->cmd_master_reset);
    printf("\n");
}

void output_master_sts_obj(struct sts_master_obj *ob1)
{  

    printf("Output_sts master state %i \n",ob1->sts_master_state);

    printf("Output_sts master %i \n",ob1->sts_master_start);
    printf("Output_sts master %i \n",ob1->sts_master_on);
    printf("Output_sts master %i \n",ob1->sts_master_off);
    printf("Output_sts master %i \n",ob1->sts_master_pause);
    printf("Output_sts master %i \n",ob1->sts_master_sleep);
    printf("Output_sts master %i \n",ob1->sts_master_standby);
    printf("Output_sts master %i \n",ob1->sts_master_reset);
    printf("\n");
}




//void write_master_value()



//output für Beckhoff slaves
/*
void output_master_obj(Master_Topology *m)
{

    printf("Der Master hat %i Slaves \n",m->slave_count);
    int i2=0;
    int i=0;
    for(i=0; i< m->slave_count; i++)
    {

            
    printf("Outut-Beckhoff-Slave-%i \n",m->slave_objekt[i].beckhoff_slave_count);
    for(i2=0; i2< m->slave_objekt[i].beckhoff_slave_count; i2++)
    {
        printf("%s\n", m->slave_objekt[i].beckhoff_data->beckhoff_s_array[i2].name);
        printf("%i\n", m->slave_objekt[i].beckhoff_data->beckhoff_s_array[i2].value);        
          
    }
    }


}
*/


//output für JM slaves

void output_master_obj(Master_Topology *m)
{


    printf("Der Master hat %i Slaves \n",m->slave_count);
    
    int i_for=0;
    int i2_for=0;
    int i3_for=0;
    int i4_for=0;

    for(i_for=0; i_for< m->slave_count; i_for++)
    {


        printf("par_id %s\n", m->slave_objekt[i_for].slave_par_id);
        printf("sts_id %s\n", m->slave_objekt[i_for].slave_sts_id);
        printf("state_id %s\n", m->slave_objekt[i_for].slave_state_id);
        printf("cmd_id %s\n", m->slave_objekt[i_for].slave_cmd_id);

      
          
       /* printf("data_output %i\n", m->slave_objekt[i_for].slave_data_output);
        printf("addr_output %i\n", m->slave_objekt[i_for].slave_addr_output);
        printf("cmd_output %i\n", m->slave_objekt[i_for].slave_cmd_output);
*/


                                                        
        printf("--------------input-Beckhoff-Slave-%i \n",m->slave_objekt[i_for].beckoff_slave_ana_in_count);
        for(i2_for=0; i2_for< m->slave_objekt[i_for].beckoff_slave_ana_in_count; i2_for++)
        {
            printf("name %s\n", m->slave_objekt[i_for].beckhoff_data_in->beckhoff_s_array[i2_for].name);
            printf("unit %s\n", m->slave_objekt[i_for].beckhoff_data_in->beckhoff_s_array[i2_for].unit);
           // printf("%i\n", m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].value);        
              
        }

        printf("--------------input-Beckhoff-Slave-%i \n",m->slave_objekt[i_for].beckoff_slave_ana_out_count);
        for(i2_for=0; i2_for< m->slave_objekt[i_for].beckoff_slave_ana_out_count; i2_for++)
        {
            printf("name %s\n", m->slave_objekt[i_for].beckhoff_data_out->beckhoff_s_array[i2_for].name);
            printf("unit %s\n", m->slave_objekt[i_for].beckhoff_data_out->beckhoff_s_array[i2_for].unit);
           // printf("%i\n", m->slave_objekt[i].beckhoff_data_out->beckhoff_s_array[i2].value);        
              
        }
    } 
       /*  
    printf("Output-Data-Count-%i \n",m->slave_objekt[i_for].slave_output_data_count);
    for(i2_for=0; i2_for< m->slave_objekt[i_for].slave_output_data_count; i2_for++)
    {
        printf("%s\n", m->slave_objekt[i_for].output_data->output_d_array[i2_for].name);
        printf("%i\n", m->slave_objekt[i_for].output_data->output_d_array[i2_for].value);        
          
    }


    printf("Slave %i Output-CMD-Count-%i \n",i_for,m->slave_objekt[i_for].slave_output_cmd_count);
    for(i2_for=0; i2_for< m->slave_objekt[i_for].slave_output_cmd_count; i2_for++)
    {
        printf("%s\n", m->slave_objekt[i_for].output_cmd->output_cmd_array[i2_for].name);
        printf("%i\n", m->slave_objekt[i_for].output_cmd->output_cmd_array[i2_for].value);        
          
    }

      

    
        printf("----->slave%i  has %i Datasets <------------- \n",i_for,m->slave_objekt[i_for].slave_dataset_count);
        //i2_for=0;

        for(i2_for=0; i2_for< m->slave_objekt[i_for].slave_dataset_count; i2_for++)
        {


            for(i3_for=0; i3_for< m->slave_objekt[i_for].d_set[i2_for].dataset_data_array_count; i3_for++)
            { 

            printf("%i Datasets  Norm Value =%.9f \n",i2_for,m->slave_objekt[i_for].d_set[i2_for].data->d_array[i3_for].norm);  
            }  
*/
           /* printf("---------------------Dataset------%i--------------------------------------------------------------------\n",i2_for);


          printf("dataset_no %i\n", m->slave_objekt[i_for].d_set[i2_for].dataset_no);
          printf("dataset_addr_var_out %i\n", m->slave_objekt[i_for].d_set[i2_for].dataset_addr_var_out);
          printf("dataset_cmd_var_out %i\n", m->slave_objekt[i_for].d_set[i2_for].dataset_cmd_var_out);  
          printf("dataset_cmd_var_read %i\n", m->slave_objekt[i_for].d_set[i2_for].dataset_cmd_var_read);  
          printf("dataset_cmd_var_interrupt %i\n", m->slave_objekt[i_for].d_set[i2_for].dataset_cmd_var_interrupt);  
          printf("dataset_data_var_input %i\n", m->slave_objekt[i_for].d_set[i2_for].dataset_data_var_input);  
          printf("dataset_status_var_input %i\n", m->slave_objekt[i_for].d_set[i2_for].dataset_status_var_input);
          printf("dataset_addr_var_input %i\n", m->slave_objekt[i_for].d_set[i2_for].dataset_addr_var_input);  
          printf("dataset_time_var_input %i\n", m->slave_objekt[i_for].d_set[i2_for].dataset_time_var_input);


          
                 
       
          printf("----->Slave %i STATE-Dataset-%i <------------- \n",i_for,i2_for);
          
          printf("%s\n", m->slave_objekt[i_for].d_set[i2_for].dataset_name);
      //    printf("%i\n", m->slave_objekt[i_for].d_set[i2_for].dataset_serialized);
          printf("%i\n", m->slave_objekt[i_for].d_set[i2_for].dataset_interrupt);
          printf("%i\n", m->slave_objekt[i_for].d_set[i2_for].dataset_timestamp);

            printf("###################################################\n");  
            printf("###################################################\n"); 
            printf("-----> Slave %i STATE-data-Dataset-%i <------------- \n",i_for,i2_for);
          for(i3_for=0; i3_for< m->slave_objekt[i_for].d_set[i2_for].dataset_data_array_count; i3_for++)
          { 

            printf("%s ",m->slave_objekt[i_for].d_set[i2_for].data->d_array[i3_for].name);  // test mit d_set[i2_for].data[i3].d_array->name); ebenfalls notwendig
            //printf("%i ",m->slave_objekt[i_for].d_set[i2_for].data->d_array[i3_for].value);

            if(!strcmp(m->slave_objekt[i_for].d_set[i2_for].data->d_array[i3_for].unit,"ui16"))
            {                            
                printf("%u",m->slave_objekt[i_for].d_set[i2_for].data->d_array[i3_for].buffer_ui);
               
            }

            else if(!strcmp(m->slave_objekt[i_for].d_set[i2_for].data->d_array[i3_for].unit,"fl32"))
            {
                printf("%f",m->slave_objekt[i_for].d_set[i2_for].data->d_array[i3_for].buffer_f);                                
            }
     
            else if(!strcmp(m->slave_objekt[i_for].d_set[i2_for].data->d_array[i3_for].unit,"in16"))
            {
                printf("%i",m->slave_objekt[i_for].d_set[i2_for].data->d_array[i3_for].buffer_i);
            }

           // printf("unit %s\n",m->slave_objekt[i_for].d_set[i2_for].data->d_array[i3_for].unit);

          }

          */
          printf("\n");

        /*  printf("----->STATE-status-Dataset-%i <------------- \n",i2_for);
          for(i4_for=0; i4_for< m->slave_objekt[i_for].d_set[i2_for].dataset_status_array_count; i4_for++)
          {

            printf("%s\n",m->slave_objekt[i_for].d_set[i2_for].status->s_array[i4_for].name);
            printf("%i\n",m->slave_objekt[i_for].d_set[i2_for].status->s_array[i4_for].value);


          }
          

          printf("----->Status_Array-%i <------------- \n",i_for);
          for(i4_for=0; i4_for< m->slave_objekt[i_for].slave_output_status_count; i4_for++)
          {

            printf("slave %i name %s\n",i_for,m->slave_objekt[i_for].output_status->output_status_array[i4_for].name);
            printf("slave %i value %i\n",i_for,m->slave_objekt[i_for].output_status->output_status_array[i4_for].value);


          }
          
                 
      //  }

    }
    }

*/
}




void write_cmd_master_obj(struct cmd_master_obj *ob1, int count, int value)
{

    switch(count)
    {
        
        case 0:
        ob1->cmd_master_state=value;
        break;    
        case 1:
        ob1->cmd_master_start=value;
        break;
        case 2:
        ob1->cmd_master_on=value;
        break;
        case 3:
        ob1->cmd_master_off=value;
        break;
        case 4:
        ob1->cmd_master_pause=value;
        break;
        case 5:
        ob1->cmd_master_sleep=value;
        break;
        case 6:
        ob1->cmd_master_standby=value;
        break;
        case 7:
        ob1->cmd_master_reset=value;
        break;
        

    }

}


void write_sts_master_obj(struct sts_master_obj *ob1, int count, int value)
{

    switch(count)
    {
        
        case 0:
        ob1->sts_master_state=value;
        break;    
        case 1:
        ob1->sts_master_start=value;
        break;
        case 2:
        ob1->sts_master_on=value;
        break;
        case 3:
        ob1->sts_master_off=value;
        break;
        case 4:
        ob1->sts_master_pause=value;
        break;
        case 5:
        ob1->sts_master_sleep=value;
        break;
        case 6:
        ob1->sts_master_standby=value;
        break;
        case 7:
        ob1->sts_master_reset=value;
        break;
        

    }

}

#endif





