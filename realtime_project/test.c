/* 

	author: jinjin
			zhen shi
*/
			
#include "MultiSensorSimulator.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#define NUMOFVALUE 5
#define NUMOFCHILDREN 19

//we found a rough 0.14 initializing delay in every created time value
//so we try to write a self-defined function to initialize it
void initial_time(struct timespec* time)
{
	time->tv_sec=0;
	time->tv_nsec=0;
}
//what per single process need to do
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
	struct timespec return_csum,total_sum,return_psum,csum_max,csum_min;
	initial_time(&total_sum);
	initial_time(&return_csum);
	initial_time(&csum_max);
	initial_time(&csum_min);
	pipe(pipe_arr);


	noOfSensors = StartSimulator(sensorDescriptor, NUMOFVALUE);

    while (i < NUMOFCHILDREN) {
	pid = fork();
  
    if (pid == 0) { 
        // this is child.
        // tasks of child process are performed.
		close(pipe_arr[0]);
		return_csum=start_read(&sensorDescriptor[i]);
    	write(pipe_arr[1],&return_csum,sizeof(return_csum));

		//printf("child: %d    childpid:%d\n\n",i,getpid());
		exit(0);
        // child terminates.
        }
        i++;
        
    }
    // Parent continues from here after creating
    // all children.
    // Tasks of parent process  are performed.
	//sum up children
	close(pipe_arr[1]);

	while(read(pipe_arr[0],&return_csum,sizeof(return_csum))>0){
		increment_timespec(&total_sum,&return_csum);
		//bubble compare
		//calculate csum_max
		if (return_csum.tv_sec>csum_max.tv_sec)
			csum_max=return_csum;
		else if (return_csum.tv_sec==csum_max.tv_sec)	
		{
			if (return_csum.tv_nsec>csum_max.tv_nsec)
				csum_max=return_csum;
			
		}
		//initial min
		if (csum_min.tv_sec==0&&csum_min.tv_nsec==0)
		{
			csum_min=return_csum;
		}
		//calculate csum_min
		if (return_csum.tv_sec<csum_min.tv_sec)
			csum_min=return_csum;
		else if (return_csum.tv_sec==csum_min.tv_sec)	
		{
			if (return_csum.tv_nsec<csum_min.tv_nsec)
				csum_min=return_csum;
		}			
	}
	//printf("max and min value was %lld.%.9ld  %lld.%.9ld\n",csum_max.tv_sec,csum_max.tv_nsec,csum_min.tv_sec,csum_min.tv_nsec );


	//sum up with parent
	return_psum=start_read(&sensorDescriptor[i]);
	increment_timespec(&total_sum,&return_psum);


	//compare child and parent for max
	printf("\n\n");
	if (csum_max.tv_sec>return_psum.tv_sec)
	{
		printf("max  value was %lld.%.9ld\n",csum_max.tv_sec,csum_max.tv_nsec);
	}
	else if(csum_max.tv_sec==return_psum.tv_sec){
		if (csum_max.tv_nsec>return_psum.tv_nsec)
			printf("max  value was %lld.%.9ld\n",csum_max.tv_sec,csum_max.tv_nsec);
		else
			printf("max  value was %lld.%.9ld\n",return_psum.tv_sec,return_psum.tv_nsec);
	}else
		printf("max  value was %lld.%.9ld\n",return_psum.tv_sec,return_psum.tv_nsec);
	//compare child and paret for min
	if (csum_min.tv_sec<return_psum.tv_sec)
	{
		printf("min  value was %lld.%.9ld\n",csum_min.tv_sec,csum_min.tv_nsec);
	}
	else if(csum_min.tv_sec==return_psum.tv_sec){
		if (csum_min.tv_nsec<return_psum.tv_nsec)
			printf("min  value was %lld.%.9ld\n",csum_min.tv_sec,csum_min.tv_nsec);
		else
			printf("min  value was %lld.%.9ld\n",return_psum.tv_sec,return_psum.tv_nsec);
	}else
		printf("min  value was %lld.%.9ld\n",return_psum.tv_sec,return_psum.tv_nsec);

	//average value
	printf("total time value was %lld.%.9ld  average delay is %lld.%.9ld\n",total_sum.tv_sec,total_sum.tv_nsec,total_sum.tv_sec/(NUMOFCHILDREN+1),total_sum.tv_nsec/(NUMOFCHILDREN+1) );
	

    while(wait(NULL) > 0);
    exit(0);

	return 0;
}