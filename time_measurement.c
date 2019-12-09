//Erstellt von Artur Kalteis
//am 4.2.2017
//artur.kalteis@gmx.net
//Programm zur Messung einer Aktion
// gcc time.c -o time -lrt   // -lrt if Preempt kernel

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sched.h>




#define MY_PRIORITY (1)

#define NSEC_PER_SEC    (1000000000) /* The number of nsecs per sec. */  // eine ganze sec

int main()
{
        
	
		struct timespec t;
		struct timespec t2;
		struct timespec result;
		
		struct sched_param param;
		
		param.sched_priority = MY_PRIORITY;
		if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) 
		{ 														// SCHED_FIFO = first-in/first-out
                perror("sched_setscheduler failed");		  	//SCHED_RR = SCHED_FIFO mit Zeitscheibenverfahren
                exit(-1);
        }

		clock_gettime(CLOCK_MONOTONIC  ,&t);
	
      


	 ////////////////////////////Start der Aktion///////////////////////////////

		int counter=0;
	
		for(int i=1; i< 10000; i++)
		{
			counter=i*i*3434;
		}

	 
	 ////////////////////////////Ende der Aktion////////////////////////////////
	
	
	clock_gettime(CLOCK_MONOTONIC  ,&t2);
		
    result.tv_sec = t2.tv_sec - t.tv_sec;
    result.tv_nsec= t2.tv_nsec- t.tv_nsec;

    if (result.tv_nsec<0) {
        result.tv_sec--; 										// muss - sein da nur so eine Sekunde dazuaddiert wird										
        result.tv_nsec = NSEC_PER_SEC + result.tv_nsec;			// result->tv_usec is negative, therefore we use "+"
    }
		
		printf(" Ausführungszeit :%9ld \n",result.tv_nsec);
		printf(" Ausführungszeit :%llu \n",result.tv_nsec);
	 


   return 0;
}
