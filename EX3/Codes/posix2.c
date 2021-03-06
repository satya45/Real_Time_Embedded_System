#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SNDRCV_MQ       "/send_receive_mq"
#define MAX_MSG_SIZE    128
#define ERROR           -1

#define NUM_THREADS     2

struct mq_attr mq_attr;

void receiver(void)
{
   mqd_t mymq;
   char buffer[ MAX_MSG_SIZE ];
   int prio;
   int nbytes;

   mymq = mq_open( SNDRCV_MQ, O_CREAT | O_RDWR, S_IRWXU, &mq_attr );

   if( mymq == ( mqd_t )ERROR )
   {
      perror( "receiver mq_open" );
      exit( -1 );
   }

   /* read oldest, highest priority msg from the message queue */
   if( ( nbytes = mq_receive( mymq, buffer, MAX_MSG_SIZE, &prio ) ) == ERROR )
   {
      perror( "mq_receive" );
   }
   else
   {
      buffer[ nbytes ] = '\0';
      printf( "receive: msg %s received with priority = %d, length = %d\n",
               buffer, prio, nbytes );
   }
}

static char canned_msg[ ] = "this is a test, and only a test, in the event of a real emergency, you would be instructed ...";

void sender( void )
{
   mqd_t mymq;
   int prio;
   int nbytes;

   mymq = mq_open( SNDRCV_MQ, O_CREAT | O_RDWR, S_IRWXU, &mq_attr );

   if( mymq < 0 )
   {
      perror( "sender mq_open" );
      exit( -1 );
   }
   else
   {
      printf( "sender opened mq\n" );
   }

   /* send message with priority=30 */
   if( ( nbytes = mq_send( mymq, canned_msg, sizeof( canned_msg ), 30 ) ) == ERROR )
   {
      perror( "mq_send" );
   }
   else
   {
      printf( "send: message successfully sent\n" );
   }
}

void main( void )
{
   pthread_t threads[ NUM_THREADS ];
   int thread_args[ NUM_THREADS ];
   int result_code;
   unsigned index;
   /* setup common message q attributes */
   mq_attr.mq_maxmsg = 10;
   mq_attr.mq_msgsize = MAX_MSG_SIZE;

   mq_attr.mq_flags = 0;

   static void ( *ThreadFunctions[ NUM_THREADS ] ) =
              { sender,
                receiver };

   // Create two communicating processes right here
   for( index = 0; index < NUM_THREADS; ++index )
   {
      thread_args[ index ] = index;
      printf("In main: creating thread %d\n", index);
      result_code = pthread_create( &threads[ index ],
                                    NULL,
                                    ThreadFunctions[ index ],
                                    &thread_args[index] );
   }

   // wait for each thread to complete
   for( index = 0; index < NUM_THREADS; ++index )
   {
      // block until thread 'index' completes
      result_code = pthread_join( threads[ index ], NULL );

      printf( "In main: thread %d has completed\n", index );
   }

   printf( "In main: All threads completed successfully\n" );
}