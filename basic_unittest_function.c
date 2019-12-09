
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // of strlen(str)
#include <time.h>   // for time
#include <unistd.h> // in this script for getlogin_r 
#include <fcntl.h>  // for file open
#include <errno.h>  // for error message

// color for different printf output
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

char * current_date_time_string()
{
    time_t timer;
    char * ptr=(char*)malloc(26*sizeof(char)); 

    if(ptr !=NULL)
    {
        struct tm* tm_info;
        time(&timer);
        tm_info = localtime(&timer);
    
        strftime(ptr, 26, "%Y-%m-%d %H:%M:%S", tm_info);
   
        return ptr;   
    }

    return "";
}


char * current_system_user()
{
    
    char *user_buffer=(char*)malloc(128*sizeof(char)); 
    sprintf(user_buffer,"");
    if(getlogin_r(user_buffer, 128)==EXIT_SUCCESS)
    {
      return user_buffer;
    }
    else
    {
      return "";
    }
}




#define unit_test(value)\
{ if (EXIT_FAILURE == (value) || (value) < 0 )  {printf("%sUnit-Test-ERROR:\nFile %s \nfunction %s \nLine %i \nUser %s \ntime %s \nError-Message %s %s\n"\
  ,RED,__FILE__,__func__ ,__LINE__,current_system_user(),current_date_time_string(),strerror(errno),WHT); return EXIT_FAILURE; errno=EXIT_SUCCESS;}}  


// errno            = error number
// strerror(errno)  = error message

int main()
{



  int fd;
 

  unit_test(fd=open("test.txt",O_RDONLY));


  close(fd);

  printf("done \n");
   



  return EXIT_SUCCESS;
}
