/* Adapted from from http://link.springer.com/chapter/10.1007%2F978-3-642-37036-6_28 */

extern void abort(void);
void reach_error(){}
void assume_abort_if_not(int cond) {
  if(!cond) {abort();}
}

#include <pthread.h>
#include <assert.h>
#define N 50


int shared = 0;
int judge = 0;

void error(void)
{
  ERROR: {reach_error();abort();}
  return;
}

void * thread(void *arg)
{
	shared = shared + 1;
}

void * thread1(void *arg)
{
    if(shared > 15) error();
}

int main(void)
{
	pthread_t t[N],t1;
    for(int i=0;i<N;i++)
	    pthread_create(&t[i], 0, thread, 0);
    pthread_create(&t1, 0, thread1, 0);

    for(int i=0;i<N;i++)
	    pthread_join(t[i], 0);
    pthread_join(t1, 0);
    return 0;
}

