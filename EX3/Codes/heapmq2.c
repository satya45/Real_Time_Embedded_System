/***
 * Heap message queue ported 
 * from VxWorks code of Prof Sam Siewert
 * Author: Smitha Bhaskar and Satya Mehta
 * 
 * 
 */
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define SNDRCV_MQ "/send_receive_mq_heap"

struct mq_attr mq_attr;				//message queue attribute variable
static mqd_t mymq;
pthread_t receiver_thread, sender_thread;	//declare threads 
pthread_attr_t rt_sched_attr;		//thread attributes
struct sched_param rt_param;		//thread params
static char imagebuff[4096];

/* receives pointer to heap, reads it, and deallocate heap memory */

void *receiver(void *threadp)
{
  char buffer[sizeof(void *)+sizeof(int)];
  char *buffptr;
  int prio;
  int nbytes;
  int count = 0;
  int id;
  mymq = mq_open( SNDRCV_MQ, O_CREAT | O_RDWR, 0664, &mq_attr );
  while(1)
  {
    /* read oldest, highest priority msg from the message queue */
    printf("\nReading %ld bytes\n", sizeof(void *));
    if((nbytes = mq_receive(mymq, buffer, (size_t)(sizeof(void *)+sizeof(int)), &prio)) == -1)
    {
        perror("mq_receive");
    }
    else
    {
      memcpy(&buffptr, buffer, sizeof(void *));
      memcpy((void *)&id, &(buffer[sizeof(void *)]), sizeof(int));
      printf("\nReceive: ptr msg 0x%p received with priority = %d, length = %d, id = %d\n", buffptr, prio, nbytes, id);
      printf("\nContents Received = %s\n", buffptr);
      free(buffptr);
      printf( "Heap space memory freed\n" );
    }
    
  }

}

void *sender(void *threadp)
{
  char buffer[sizeof(void *)+sizeof(int)];
  void *buffptr;
  int prio;
  int nbytes;
  int id = 999;
  mymq = mq_open( SNDRCV_MQ, O_CREAT | O_RDWR, 0664, &mq_attr );

  while(1) 
  {

    buffptr = (void *)malloc(sizeof(imagebuff));
    strcpy(buffptr, imagebuff);
    printf("\nMessage to send = %s\n", (char *)buffptr);
    memcpy(buffer, &buffptr, sizeof(void *));
    memcpy(&(buffer[sizeof(void *)]), (void *)&id, sizeof(int));
    if((nbytes = mq_send(mymq, buffer, (size_t)(sizeof(void *)+sizeof(int)), 30)) == -1)
    {
      perror("mq_send");
    }
    else
    {
      printf("\nSend: message ptr 0x%s successfully sent\n", (char *)buffptr);
    }
    sleep(3);
  }
  
}
static int sid, rid;

void main()
{
  int i, j;
  char pixel = 'A';
  int rc;
  for(i=0;i<4096;i+=64)
  {
    pixel = 'A';
    for(j=i;j<i+64;j++) 
    {
      imagebuff[j] = (char)pixel++;
    }
    imagebuff[j-1] = '\n';
  }
  imagebuff[4095] = '\0';
  imagebuff[63] = '\0';
  printf("buffer =\n%s",imagebuff);
  //printf("%ld", sizeof(imagebuff));

  /* setup common message q attributes */
  mq_attr.mq_maxmsg = 100;
  mq_attr.mq_msgsize = sizeof(void *)+sizeof(int);
  mq_attr.mq_flags = 0;
  mymq = mq_open( SNDRCV_MQ, O_CREAT | O_RDWR, 0664, &mq_attr );
  if(pthread_create(&receiver_thread, (void*)&rt_sched_attr,(void *)sender, NULL)==0)
	printf("\n\rReceiver Thread Created Sucessfully!\n\r");
  else perror("thread creation failed");

  if(pthread_create(&sender_thread, (void*)&rt_sched_attr,(void *) receiver, NULL)==0)
	printf("\n\rSender Thread Created Sucessfully!\n\r");
  else perror("thread creation failed");

  pthread_join(receiver_thread, NULL);
  pthread_join(sender_thread, NULL);
  
}
