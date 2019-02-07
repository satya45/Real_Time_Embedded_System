/*********************************************************************************************
* Vatsal Sheth, Satya Mehta, February 2019
*
* This code executes two threads to execute synthetic load which is Fibonacci series for 10ms
* and 20ms. This tasks are scheduled by fixed priority Rate Monotonic Policy. 
* 
* Parts of this course are leveraged from Independent study on RM Scheduling Feasibility Tests 
* conducted on TI DM3730 Processor - 1 GHz ARMCortex-A8 core with Angstrom and TimeSys Linux 
* ported on to BeagleBoard xM by Nisheeth Bhat and RT_simplethread code by Prof Sam Siewart. 
*********************************************************************************************/

#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_THREADS (2)

#define NSEC_PER_SEC (1000000000)
#define NSEC_PER_MSEC (1000000)
#define NSEC_PER_MICROSEC (1000)

unsigned int idx = 0, jdx = 1;
unsigned int seqIterations = 47;
unsigned int reqIterations_20ms = 12664800;
unsigned int reqIterations_10ms = 6332400;
volatile unsigned int fib = 0, fib0 = 0, fib1 = 1, abortTest_20 = 0, abortTest_10 = 0;

#define FIB_TEST(seqCnt, iterCnt)      \
   for(idx=0; idx < iterCnt; idx++)    \
   {                                   \
      fib = fib0 + fib1;               \
      while(jdx < seqCnt)              \
      {                                \
         fib0 = fib1;                  \
         fib1 = fib;                   \
         fib = fib0 + fib1;            \
         jdx++;                        \
      }                                \
   }                                   \
   

// POSIX thread declarations and scheduling attributes
//
pthread_t rt_threads[NUM_THREADS];
pthread_attr_t rt_sched_attr;
int rt_max_prio;
struct sched_param rt_param;
struct sched_param main_param;
pthread_attr_t main_attr;
pid_t mainpid;

sem_t Thread_sem[NUM_THREADS];

struct timespec start_time = {0, 0};
struct timespec finish_time0 = {0, 0};
struct timespec thread_dt0 = {0, 0};
struct timespec finish_time1 = {0, 0};
struct timespec thread_dt1 = {0, 0};
   
int delta_t(struct timespec *stop, struct timespec *start, struct timespec *delta_t)
{
  int dt_sec=stop->tv_sec - start->tv_sec;
  int dt_nsec=stop->tv_nsec - start->tv_nsec;

  if(dt_sec >= 0)
  {
    if(dt_nsec >= 0)
    {
      delta_t->tv_sec=dt_sec;
      delta_t->tv_nsec=dt_nsec;
    }
    else
    {
      delta_t->tv_sec=dt_sec-1;
      delta_t->tv_nsec=NSEC_PER_SEC+dt_nsec;
    }
  }
  else
  {
    if(dt_nsec >= 0)
    {
      delta_t->tv_sec=dt_sec;
      delta_t->tv_nsec=dt_nsec;
    }
    else
    {
      delta_t->tv_sec=dt_sec-1;
      delta_t->tv_nsec=NSEC_PER_SEC+dt_nsec;
    }
  }

  return(1);
}

void *Thread0_Fn(void *threadp)
{
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	
	while(!abortTest_10)
	{
		sem_wait(&Thread_sem[0]);
		
		pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
		
		FIB_TEST(seqIterations, reqIterations_10ms);
		
		printf("Thread idx=0 ran on core=%d, affinity contained:", sched_getcpu());
		for(int i=0; i<4; i++)
		{
			if(CPU_ISSET(i, &cpuset))  
				printf(" CPU-%d ", i);
		}
		printf("\n");
		
		clock_gettime(CLOCK_REALTIME, &finish_time0);
		delta_t(&finish_time0, &start_time, &thread_dt0);

		printf("Thread idx=0 timestamp %ld sec, %ld msec (%ld microsec)\n\n",thread_dt0.tv_sec, (thread_dt0.tv_nsec / NSEC_PER_MSEC), (thread_dt0.tv_nsec / NSEC_PER_MICROSEC));

		//pthread_exit(0);
	}
}

void *Thread1_Fn(void *threadp)
{
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	
	while(!abortTest_20)
	{
		sem_wait(&Thread_sem[1]);
		
		pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
		
		FIB_TEST(seqIterations, reqIterations_20ms);
		
		printf("Thread idx=1 ran on core=%d, affinity contained:", sched_getcpu());
		for(int i=0; i<4; i++)
		{
			if(CPU_ISSET(i, &cpuset))  
				printf(" CPU-%d ", i);
		}
		printf("\n");
		
		clock_gettime(CLOCK_REALTIME, &finish_time1);
		delta_t(&finish_time1, &start_time, &thread_dt1);

		printf("Thread idx=1 timestamp %ld sec, %ld msec (%ld microsec)\n\n",thread_dt1.tv_sec, (thread_dt1.tv_nsec / NSEC_PER_MSEC), (thread_dt1.tv_nsec / NSEC_PER_MICROSEC));

		//pthread_exit(0);
	}
}

void print_scheduler(void)
{
   int schedType;

   schedType = sched_getscheduler(getpid());

   switch(schedType)
   {
     case SCHED_FIFO:
           printf("Pthread Policy is SCHED_FIFO\n");
           break;
     case SCHED_OTHER:
           printf("Pthread Policy is SCHED_OTHER\n");
       break;
     case SCHED_RR:
           printf("Pthread Policy is SCHED_OTHER\n");
           break;
     default:
       printf("Pthread Policy is UNKNOWN\n");
   }

}

int main (int argc, char *argv[])
{
	int rc, scope, i;
	cpu_set_t cpuset;
	useconds_t t_20 = 20000;
	useconds_t t_10 = 10000;
	
	CPU_ZERO(&cpuset);
	CPU_SET(0, &cpuset);
	
	mainpid=getpid();
	
	rt_max_prio = sched_get_priority_max(SCHED_FIFO);
	
	rc=sched_getparam(mainpid, &main_param);
	main_param.sched_priority=rt_max_prio;
	rc=sched_setscheduler(getpid(), SCHED_FIFO, &main_param);
	
	if(rc < 0) 
		perror("main_param");
		
	print_scheduler();
	
	pthread_attr_getscope(&main_attr, &scope);

	if(scope == PTHREAD_SCOPE_SYSTEM)
		printf("PTHREAD SCOPE SYSTEM\n");
	else if (scope == PTHREAD_SCOPE_PROCESS)
		printf("PTHREAD SCOPE PROCESS\n");
	else
		printf("PTHREAD SCOPE UNKNOWN\n");
	
	rc=pthread_attr_init(&rt_sched_attr);
	rc=pthread_attr_setinheritsched(&rt_sched_attr, PTHREAD_EXPLICIT_SCHED);
	rc=pthread_attr_setschedpolicy(&rt_sched_attr, SCHED_FIFO);
	rc=pthread_attr_setaffinity_np(&rt_sched_attr, sizeof(cpu_set_t), &cpuset);

	for(i=0; i < NUM_THREADS; i++)
	{
		if (sem_init (&(Thread_sem[i]), 0, 1))
	    {
		    printf ("Failed to initialize semaphore for thread %d\n", i);
		    exit (-1);
	    }
	    
		printf("Setting thread %d to core 0\n", i);
	}
	
	clock_gettime(CLOCK_REALTIME, &start_time);
			
	rt_param.sched_priority = rt_max_prio-1;
	pthread_attr_setschedparam(&rt_sched_attr, &rt_param);
	
	pthread_create(&rt_threads[0],   // pointer to thread descriptor
                  (void *)&rt_sched_attr,     // use default attributes
                  Thread0_Fn, // thread function entry point
                  (void *)0 // parameters to pass in
                  );
    
    rt_param.sched_priority = rt_max_prio-2;
	pthread_attr_setschedparam(&rt_sched_attr, &rt_param);
	
	pthread_create(&rt_threads[1],   // pointer to thread descriptor
                  (void *)&rt_sched_attr,     // use default attributes
                  Thread1_Fn, // thread function entry point
                  (void *)0 // parameters to pass in
                  );	
	
	usleep(t_20);
	sem_post(&Thread_sem[0]);
	usleep(t_20);
	sem_post(&Thread_sem[0]);
	usleep(t_10);
	abortTest_20=1;
	sem_post(&Thread_sem[1]);
	usleep(t_10);
	sem_post(&Thread_sem[0]);
	usleep(t_20);
	abortTest_10=1;
	sem_post(&Thread_sem[0]);
	usleep(t_20);
	
	for(i=0; i<NUM_THREADS;i++)
	{
		pthread_join(rt_threads[i], NULL);
	}
	
	for(i=0; i<NUM_THREADS;i++)
	{
		sem_destroy(&(Thread_sem[i]));
	}
	
	printf("\nTEST COMPLETE !!!\n");
}
