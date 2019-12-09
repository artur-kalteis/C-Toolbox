/**
 * @brief function to compare to files whether there are equal or not
 *
 * @author Artur Kalteis 
 *
 *     Artur.kalteis@gmx.net
 *
 * @version 1.0
 *
 * @date 06.02.2019
 *
 * @bug No known bugs.
 *
 */



#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>


int  compareFiles(FILE *fp1, FILE *fp2) 
{ 
    // fetching character of two file 
    // in two variable ch1 and ch2 
    char ch1 = getc(fp1); 
    char ch2 = getc(fp2); 
  
    // error keeps track of number of errors 
    // pos keeps track of position of errors 
    // line keeps track of error line 
    int error = 0, pos = 0, line = 1; 
  
    // iterate loop till end of file 
    while (ch1 != EOF && ch2 != EOF) 
    { 
        pos++; 
  
        // if both variable encounters new 
        // line then line variable is incremented 
        // and pos variable is set to 0 
        if (ch1 == '\n' && ch2 == '\n') 
        { 
            line++; 
            pos = 0; 
        } 
  
        // if fetched data is not equal then 
        // error is incremented 
        if (ch1 != ch2) 
        { 
            error++; 
            printf("Line Number : %d \tError"
               " Position : %d \n", line, pos); 
        } 
  
        // fetching character until end of file 
        ch1 = getc(fp1); 
        ch2 = getc(fp2); 
    } 
  

    printf("total Error's =%i\n",error);
    return error;
   
} 



int main() 
{

	


    
    // opening both file in read only mode 
    FILE *fp1 = fopen("/etc/network/interfaces", "r"); 
    FILE *fp2 = fopen("interfaces", "r"); 
  
    if (fp1 == NULL || fp2 == NULL) 
    { 
       printf("Error : Files not open"); 
       exit(0); 
    } 
  
   int i =compareFiles(fp1, fp2); 

   if(i==0)
   {
       printf("files are equal !\n");
   }
   else
    {
        printf("files are NOT equal !\n");
    }
   
  
    // closing both file 
    fclose(fp1); 
    fclose(fp2); 
    return 0; 

	

	
return 0;
}
