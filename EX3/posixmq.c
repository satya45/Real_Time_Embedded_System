#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>



#define SNDRCV_MQ "/send_receive_mq_posix"		//shared message queue
#define MAX_MSG_SIZE 128
#define NUMTHREADS 2

struct mq_attr attr;				//message queue attribute variable

pthread_t receiver_thread, sender_thread;	//declare threads 

pthread_attr_t rt_sched_attr[NUMTHREADS];		//thread attributes

struct sched_param rt_param[NUMTHREADS];		//thread params

void *receiver(void *threadp)

{
  printf("Reciever\n\r");
  mqd_t mymq;
  char buffer[MAX_MSG_SIZE];
  int prio;
  int nbytes;

  /* note that VxWorks does not deal with permissions? */
  mymq = mq_open(SNDRCV_MQ, O_CREAT|O_RDWR, 0664, &attr);

  printf("Recieving Message from Message Queue\n\r");
  /* read oldest, highest priority msg from the message queue */
  if((nbytes = mq_receive(mymq, &buffer[0], MAX_MSG_SIZE, &prio)) == -1)
  {
    perror("mq_receive");
  }
  else
  {
    buffer[nbytes] = '\0';
    printf("Received: msg %s \n\rReceived with priority = %d, length = %d\n",
           buffer, prio, nbytes);
  }
    
}

static char canned_msg[] = "this is a test, and only a test, in the event of a real emergency, you would be instructed ...";

void *sender(void *threadp)
{
  printf("Sender\n\r");
  mqd_t mymq;
  int prio;
  int nbytes;

  /* note that VxWorks does not deal with permissions? */
  mymq = mq_open(SNDRCV_MQ, O_RDWR, 0664, &attr);

  printf("Sending Message from Message Queue\n\r");
  /* send message with priority=30 */
  if((nbytes = mq_send(mymq, canned_msg, sizeof(canned_msg), 30)) == -1)
  {
    perror("mq_send");
  }
  else
  {
    printf("Send: Message successfully sent\n");
  }
  
}


void main()
{

int rc;

for (int i=0; i<2; i++)
{
  rc=pthread_attr_init(&rt_sched_attr[i]);
  rc=pthread_attr_setinheritsched(&rt_sched_attr[i], PTHREAD_EXPLICIT_SCHED);
  rc=pthread_attr_setschedpolicy(&rt_sched_attr[i], SCHED_FIFO);
  rt_param[i].sched_priority=99-i;
  pthread_attr_setschedparam(&rt_sched_attr[i], &rt_param[i]);
}
  /* setup common message q attributes */
  attr.mq_maxmsg = 100;
  attr.mq_msgsize = MAX_MSG_SIZE;

  attr.mq_flags = 0;

  if(pthread_create(&receiver_thread, (void*)&rt_sched_attr[0], receiver, NULL)==0)
	printf("Receiver Thread Created Sucessfully!\n\r");
  else perror("thread creation failed");
  
  if(pthread_create(&sender_thread, (void*)&rt_sched_attr[1], sender, NULL)==0)
	printf("Sender Thread Created Sucessfully!\n\r");
  else perror("thread creation failed");

  pthread_join(receiver_thread, NULL);
  pthread_join(sender_thread, NULL);

   
}
