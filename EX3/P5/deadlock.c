/****************************************************************************************************************************************
*				File Name : Deadlock demonstration using pthread_mutex_timed_lock			
				Author : Satya Mehta	
				Two threads are created one updates the attitudes structure and
				another thread retrieves the timestamp.
				
*/
#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <signal.h>

#define THREADS 2
#define NSEC_PER_SEC (1000000000)
#define NSEC_PER_MSEC (1000000)
#define NSEC_PER_MICROSEC (1000)


/*Global variables*/
pthread_t my_thread[THREADS];
pthread_attr_t my_attributes;
unsigned long int t1_pid, t2_pid;
pthread_mutex_t lock;
static timer_t timerid;
struct timespec timestamp;
struct itimerspec trigger;
struct sigevent sev;
static timer_t timerid;
struct attitudes{
	double x;
	double y;
	double z;
	double roll;
	double pitch;
	double yaw;
	struct timespec timestamp;
};


double random_generator(void)
{
	double a = 5.0;
	return ((double)rand() / (double)(RAND_MAX) * a);
	
}

void *thread1(void* thread)
{	
	struct attitudes *attr = (struct attitudes *)thread;
	double  n;
	while(1)
	{
		printf("Something\n");
		pthread_mutex_lock(&lock);
		n = random_generator();
		attr->x=n;  //update x
		n = random_generator();
		attr->y=n; //update y
		n = random_generator();
		attr->z=n; //update z
		n = random_generator();
		attr->roll=n; //update roll
		n = random_generator();
		attr->pitch=n; //update pitch
		n = random_generator();
		attr->yaw=n;   //update yaw
		if(clock_gettime(CLOCK_REALTIME, &(attr->timestamp)) == -1)
		perror("Clock get_time");
		printf("*********Data Written************\n");
		printf("Data written : X-AXIS %lf, Y-AXIS %lf, Z-AXIS %lf\n", attr->x, attr->y, attr->z);
		printf("Roll %lf, Pitch %lf, Yaw %lf\n", attr->roll, attr->pitch, attr->yaw);
		sleep(15);
		pthread_mutex_unlock(&lock);
		sleep(1);
	}
}


void *thread2(void* thread)
{	
	struct attitudes *attr = (struct attitudes *)thread;
	struct timespec tptr;
	int flag = 0;
	sleep(1);
	while(1)
	{	
		clock_gettime(CLOCK_REALTIME, &tptr);
		tptr.tv_sec = time(NULL) + 10;
		int rc = pthread_mutex_timedlock(&lock, &tptr);
		if(rc ==  0)
		{
			printf("*********Data Read************\n");
			printf("Data written : X-AXIS %lf, Y-AXIS %lf, Z-AXIS %lf\n", attr->x, attr->y, attr->z);
			printf("Roll %lf, Pitch %lf, Yaw %lf\n", attr->roll, attr->pitch, attr->yaw);
			printf("Thread 2 timestamp : %ld\n\n", attr->timestamp.tv_nsec);
			pthread_mutex_unlock(&lock);
		}
		else if(rc == ETIMEDOUT)
		{
			if(clock_gettime(CLOCK_REALTIME, &(tptr)) == -1)
			perror("clock gettime failed");
			printf("NO new data available at %ld\n",tptr.tv_sec);
		}
	}
	
}


int main(int argc, char *argv[])
{
	int i;
	srand(time(NULL));
	struct attitudes *a = (struct attitudes *)malloc(sizeof(struct attitudes));
	if(a == NULL)
	{
		printf("Malloc failed");
		exit(EXIT_FAILURE);
	}
	if(pthread_mutex_init(&lock, NULL))
	{
		perror("Mutex initialization failed");
		exit(1);
	}	
	if(pthread_create(&my_thread[0],   // pointer to thread descriptor
                  (void *)&my_attributes,     // use default attributes
                  thread1,// thread function entry point
                  (void *)a // parameters to pass in
				  ))
	{
		perror("Pthread 1 create failed");
		exit(1);
		
	}
	if(pthread_create(&my_thread[1],   // pointer to thread descriptor
                  (void *)&my_attributes,     // use default attributes
                  thread2, // thread function entry point
                  (void *)a // parameters to pass in
				  ))
	{
		perror("Pthread 2 create failed");
		exit(1); 
	}
	for(i=0; i<THREADS;i++)
	{
		pthread_join(my_thread[i], NULL);
	}
	pthread_mutex_destroy(&lock);
	free(a);
	
}
