#include "MultiSensorSimulator.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#define NUMOFVALUE 5
#define NUMOFCHILDREN 19
void initial_time(struct timespec* time)
{
	time->tv_sec=0;
	time->tv_nsec=0;
}

struct timespec start_read(int* sensorDescriptor){
	Tmeas measurement[NUMOFVALUE];
	struct timespec diff,sum;
	int i;
	initial_time(&sum);

	for(i=0;i<NUMOFVALUE;i++){
		read(*sensorDescriptor, &measurement[i], sizeof(Tmeas));
		printf("Measurement value was %d\n", measurement[i].value);
		if(i>0){
			diff=diff_timespec(&measurement[i-1].moment,&measurement[i].moment);
			//printf("time value was %lld.%.9ld\n",diff.tv_sec,diff.tv_nsec );
			//printf("test sum value was %lld.%.9ld\n",sum.tv_sec,getnanosec(&sum));
			increment_timespec(&sum,&diff);
			
			
		}

	}
	printf("pid: %d sum value was %lld.%.9ld\n",getpid(),sum.tv_sec,sum.tv_nsec);
	return sum;
}

int main(void)
{
	int noOfSensors,i=0;
	pid_t pid;
	int sensorDescriptor [20];
	int status;
	int pipe_arr[2];
	struct timespec return_csum,total_sum,return_psum;
	initial_time(&total_sum);
	initial_time(&return_csum);
	pipe(pipe_arr);


	noOfSensors = StartSimulator(sensorDescriptor, NUMOFVALUE);

    while (i < NUMOFCHILDREN) {
    //i++;
	//printf("test\n");
	pid = fork();
  
	//printf("%d\n",getpid());
    if (pid == 0) { // this is child
        // this is child.
        // tasks of child process are performed.
		close(pipe_arr[0]);
		return_csum=start_read(&sensorDescriptor[i]);
    	write(pipe_arr[1],&return_csum,sizeof(return_csum));

		//printf("child: %d    childpid:%d\n\n",i,getpid());
		exit(0);
        //exit(0);    // child terminates.
        }
        i++;
        //close(pipe_arr[1]);
        //flag=fcntl(pipe_arr[0],F_GETFL);
        //fcntl(pipe_arr[0],F_SETFL,flag|O_NONBLOCK);
		
		//read(pipe_arr[0],&return_csum,sizeof(return_csum));


		//increment_timespec(&total_sum,&return_csum);
		//printf("child total time value was %lld.%.9ld\n",return_csum.tv_sec,return_csum.tv_nsec );
		
		//printf("child total time value was %lld.%.9ld\n",total_sum.tv_sec,total_sum.tv_nsec );
        // Parent continues from here after creating   // a child.
        //
		//printf("parent1 %d     parentpid:%d \n",i,getpid());
		
    }
    // Parent continues from here after creating
    // all children.
    // Tasks of parent process  are performed.
	//sum up children
	close(pipe_arr[1]);
	while(read(pipe_arr[0],&return_csum,sizeof(return_csum))>0)
		increment_timespec(&total_sum,&return_csum);
	printf("child total time value was %lld.%.9ld\n",total_sum.tv_sec,total_sum.tv_nsec );
	//sum up with parent
	return_psum=start_read(&sensorDescriptor[i]);
	increment_timespec(&total_sum,&return_psum);
	//print parent
	printf("parent  time value was %lld.%.9ld\n",return_psum.tv_sec,return_psum.tv_nsec );
	//print total
	printf("total time value was %lld.%.9ld\n",total_sum.tv_sec,total_sum.tv_nsec );
	
	//printf("parent %d     parentpid:%d \n\n",i,getpid());

    while(wait(NULL) > 0);
    exit(0);

	return 0;
}