#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>



char * current_date_time_string()
{
    //#include <time.h>
    time_t timer;
    char * ptr=(char*)malloc(26*sizeof(char)); 

    if(ptr !=NULL)
    {
        struct tm* tm_info;
        time(&timer);
        tm_info = localtime(&timer);
    
        strftime(ptr, 26, "%Y-%m-%d %H:%M:%S", tm_info);
       // printf("%s\n",ptr);
        return ptr;   
    }

    return "";
    

}

#define KNRM  "\x1B[0m"     //normal
#define KRED  "\x1B[31m"    //red
#define KGRN  "\x1B[32m"    //green
#define KYEL  "\x1B[33m"    //Yellow
#define KBLU  "\x1B[34m"    //blue
#define KMAG  "\x1B[35m"    //magenta 
#define KCYN  "\x1B[36m"    //cyan
#define KWHT  "\x1B[37m"    //white


int debugCounter=0;
int debugMode=0;


#define debug(){  if (debugMode == 1)  { printf("%s DEBUG COUNTER [%i] File=%s function=%s Line=%i time=%s   %s\n",KGRN,debugCounter,__FILE__,__func__ ,__LINE__,current_date_time_string(),KWHT); debugCounter++;}}  


int main()
{



	printf("hallo 1\n");
		debug();

	printf("hallo 1\n");

	debug();

	printf("hallo 1\n");

	debug();

	printf("hallo 1\n");
	return 0;
}
