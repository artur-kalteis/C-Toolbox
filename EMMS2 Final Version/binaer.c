#include <stdio.h>
#include <stdlib.h>


typedef struct{
    
        
        int status_value;
        int status_8bit_msb; //most significant  bit
        int status_8bit_lsb; //less significant bit // for the address
        int status_bit_buffer;
        int status_8bit_array[8];
    
    
    } slave_cycle_hanlder;
    
   


int bit_array[16]={1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768};

int main()
{
    printf("debug 1 \n");
    slave_cycle_hanlder s_c_h; // for the handling during the cyclic process


        printf("debug 2 \n");
    //send adresss works !!
/*
    int value_16_bit=0;
    int buffer=0;
    int address=17; // addresse muss einmal an die ersten 8 bits und an die letzten 8 bits weitergeleitet werden 
    //int a=17, b=17, c;
    buffer= address<<8; // shift this value
    value_16_bit = address + buffer;
    //printf("c: %d\n", c);
    printf("c: %i\n", value_16_bit);

*/
    //send single bit
/*
    int value=1;
    int buffer1=0;
    int buffer2=0;
    int result=0;

    int shift_to1=3;
    int shift_to2=7;

    buffer1=value<<shift_to1;
    buffer2=value<<shift_to2;

    result=buffer1+buffer2; // 2 steps

    int result2=bit_array[3]+bit_array[7]; // 1 step

    printf("output: %i\n", result);// = 10001000
    printf("output: %i\n", result2);// = 10001000


    */
   // http://manderc.com/concepts/umrechner/


    // read out address
    // 16 bit wert bitweise auslesen

  /*      int readout=12345;
        int bit_buffer=0;
        int readout2=readout >>8;
        int i=0;

        for(i=0; i<8 ;i++)
        {
            bit_buffer=readout2 >> i;
            bit_buffer= bit_buffer & 1;

            printf("Bit [%i]=%i \n",i,bit_buffer );

        }

        printf("-----------------------------\n");

    //cut in 2 8bit blöcke

    //int value=4096;
    //printf("most segnificant 8bit-Value%i \n",value >>8 ); // wird um 8 bit verschoben
    readout=255 & readout;
    
    printf("most segnificant 8bit-Value%i \n",readout); // wird um 8 bit verschoben
    
    */  
/*
    int value = 6293;
    


    printf("debug 2.1 \n");
     // reading the Ethercat status variable
     s_c_h.status_value=value;    
     printf("debug 3 \n");

     if(s_c_h.status_value)
     {
         // 8 bool bits
         s_c_h.status_8bit_msb=s_c_h.status_value >>8 ; //schiftet to the last 8 bits
         //address
         s_c_h.status_8bit_lsb=255 & s_c_h.status_value; // cutout the first 8 bits


        printf("MSB %i \n",s_c_h.status_8bit_msb);
        printf("LSB %i \n", s_c_h.status_8bit_lsb);

        int i=0;
        
         for(i=0; i<8 ;i++)
         {

             s_c_h.status_bit_buffer=s_c_h.status_8bit_msb >> i;
             s_c_h.status_8bit_array[i]=s_c_h.status_bit_buffer & 1;
 
         }

         int f=0;

         for(f=0; f< 8;f++ )
         {
            printf("ARRAY %i \n",s_c_h.status_8bit_array[f]);


         }



     }

*/

     char test[32]="testing";
     char buffer[32]="";
     int p=0;

     for(p=0; p< 10;p++)
     {

        sprintf(buffer,"%s_%i",test,p);

        printf("Ausgabe %s \n",buffer);

     }


    return 0;
}