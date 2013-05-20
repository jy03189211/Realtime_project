#include "MultiSensorSimulator.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#define NUMOFVALUE 5

void initial_time(struct timespec* time)
{
	time->tv_sec=0;
	time->tv_nsec=0;
}

void start_read(int* sensorDescriptor){
	Tmeas measurement[NUMOFVALUE];
	struct timespec diff,sum;
	int i;
	initial_time(&sum);

	for(i=0;i<NUMOFVALUE;i++){
		read(*sensorDescriptor, &measurement[i], sizeof(Tmeas));
		printf("Measurement value was %d\n", measurement[i].value);
		if(i>0){
			diff=diff_timespec(&measurement[i-1].moment,&measurement[i].moment);
			printf("time value was %lld.%.9ld\n",diff.tv_sec,diff.tv_nsec );
			//printf("test sum value was %lld.%.9ld\n",sum.tv_sec,getnanosec(&sum));
			increment_timespec(&sum,&diff);

			printf("pid: %d sum value was %lld.%.9ld\n",sum.tv_sec,sum.tv_nsec,getpid());
		}

	}
}

int main(void)
{
	int noOfSensors,i=0;
	pid_t pid;
	int sensorDescriptor [20];
	

	noOfSensors = StartSimulator(sensorDescriptor, NUMOFVALUE);

    while (i < 5) {
    pid = fork();
    i++;
    if (pid == 0) { // this is child
        // this is child.
        // tasks of child process are performed.
    	start_read(&sensorDescriptor[i-1]);
        exit(0);    // child terminates.
        }
        
        // Parent continues from here after creating   // a child.
        start_read(&sensorDescriptor[4]);
    } 
    // Parent continues from here after creating // all children.
    // Tasks of parent process  are performed.
    while(wait(NULL) > 0);
    exit(0);

	return 0;
}