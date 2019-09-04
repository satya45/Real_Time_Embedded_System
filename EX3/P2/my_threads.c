/****************************************************************************************************************************************
*				File Name : Thread Safety with global data			
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
struct timespec start_time = {0, 0};
struct timespec finish_time0 = {0, 0};
struct timespec thread_dt0 = {0, 0};
struct timespec timestamp;
struct itimerspec trigger;
struct sigevent sev;
static timer_t timerid;
int flag = 0;
int write_flag = 0;


struct attitudes{
	double x;
	double y;
	double z;
	double roll;
	double pitch;
	double yaw;
	struct timespec timestamp;
};

/*Timer handler*/
void thread_handler(union sigval sv)
{
	flag = 1;
}

double random_generator(void)
{
	double a = 5.0;
	return ((double)rand() / (double)(RAND_MAX) * a);
	
}

void *thread1(void* thread)
{	
	struct attitudes *attr = (struct attitudes *)thread;
	trigger.it_interval.tv_sec = 5;
	//trigger.it_value.tv_nsec = 100000000;
	timer_settime(timerid, 0, &trigger, NULL);
	while(1)
	{
		
		if(flag == 1)
		{	
			pthread_mutex_lock(&lock);
			double n;
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
			pthread_mutex_unlock(&lock);
			flag = 0;
		}
	}
}

void *thread2(void* thread)
{	
	struct attitudes *attr = (struct attitudes *)thread;
	while(1)
	{	
		pthread_mutex_lock(&lock);
		printf("*********Data Read************\n");
		printf("Data written : X-AXIS %lf, Y-AXIS %lf, Z-AXIS %lf\n", attr->x, attr->y, attr->z);
		printf("Roll %lf, Pitch %lf, Yaw %lf\n", attr->roll, attr->pitch, attr->yaw);
		printf("Thread 2 timestamp : %ld\n\n", attr->timestamp.tv_nsec);
		pthread_mutex_unlock(&lock);
		sleep(5);
	}
	
}


int main(int argc, char *argv[])
{
	int i;
	srand(time(NULL));
	struct attitudes *a = (struct attitudes *)malloc(sizeof(struct attitudes));
	if(pthread_mutex_init(&lock, NULL))
	{
		perror("Mutex initialization failed");
		exit(1);
	}	
	sev.sigev_notify = SIGEV_THREAD;
  	sev.sigev_notify_function = &thread_handler;
 	sev.sigev_value.sival_ptr = NULL;
	if(timer_create(CLOCK_REALTIME, &sev, &timerid))
	{
		perror("Timer creation failed");
		exit(EXIT_FAILURE);
	}
	trigger.it_value.tv_sec = 2;
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
	timer_delete(&timerid);
	
}
