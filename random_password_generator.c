
#include <stdio.h>
#include <stdlib.h>
#include <time.h>  


char * rand_alphanumeric_string( size_t length) 
{
    
    if(length == 0)
    {
      return "empty length";
    }
    else
    {
      char * dest = malloc(length * sizeof(char*));
        
      length--;
   
       srand(time(NULL));
   
       //4 Different charsets for all possible character
       char charset1[10] = {'1','2','3','4','5','6','7','8','9','0'};
       char charset2[26] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
       char charset3[22] = {'!','$','&','(',')','=','?','{','}','[',']','<','>','|',',','.','-',';',':','_','#','+'};
       char charset4[26] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
   
       int charset_nr=0;           // Charset number 
       int increment_loop=0;       //
       size_t index=0;
       int counter_i=0;             // Index for the random value of one charset
       while (length > 0) 
       {
          
           charset_nr = rand() % 4;         // every cycle it starts from a other charset array
           increment_loop=0;
      
       while(increment_loop <4)    // this loop goes through every charset, than a new cycle starts with a new first charset
       {
   
      
           if(charset_nr==4)
           {
               charset_nr=0;
           }
   
           
           switch(charset_nr)
           {
               case 0:  {  index = rand() % (sizeof(charset1));   
                           dest[counter_i] = charset1[index]; 
           
                           break;}
   
               case 1:  {  index = rand() % (sizeof(charset2));   
                           dest[counter_i] = charset2[index]; 
                      
                           break;}
   
               case 2:  { index = rand() % (sizeof(charset2));   
                           dest[counter_i] = charset2[index]; 
                          
                           break;}
   
               case 3:  {  index = rand() % (sizeof(charset4));   
                           dest[counter_i] = charset4[index]; 
                         
                           break;}
           }
   
      
           if(length <= 0)
           {
               break;
           }
   
   
           length--;
   
           
           charset_nr++;
           increment_loop++;
           counter_i++;
       }    
    
       }
       dest[counter_i] = '\0';

       return dest;
    }

}
int main()
{

    char *test=rand_alphanumeric_string(10);
  
  
    printf("ausgabe1 %s\n",test);

    char *test2=rand_alphanumeric_string(0);
    printf("ausgabe2 %s\n",test2);
 
  return EXIT_SUCCESS;
}
