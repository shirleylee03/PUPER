/* Adapted from http://link.springer.com/chapter/10.1007%2F978-3-642-37036-6_28 */

extern void abort(void);
void reach_error(){}

#include <stdlib.h>
#include <pthread.h>
#include <string.h>

void __VERIFIER_assert(int expression) { if (!expression) { ERROR: {reach_error();abort();}}; return; }

int a,b,c,d,e;

void *thread1(void * arg)
{
    a=1;
    return 0;
}

void *thread2(void *arg)
{
    b=1;
    return 0;
}

void *thread3(void *arg)
{
    c=1;
    return 0;
}

void *thread4(void *arg)
{
    d=1;
    return 0;
}

void *thread5(void *arg)
{
    e=1;
    return 0;
}
void *thread6(void * arg)
{
    a=1;
    return 0;
}

void *thread7(void *arg)
{
    b=1;
    return 0;
}

void *thread8(void *arg)
{
    c=1;
    return 0;
}

void *thread9(void *arg)
{
    d=1;
    return 0;
}

void *thread10(void *arg)
{
    e=1;
    return 0;
}

int main()
{
    pthread_t t1,t2,t3,t4,t5;
	pthread_t t6,t7,t8,t9,t10;

    pthread_create(&t1, 0, thread1, 0);
    pthread_create(&t2, 0, thread2, 0);
    pthread_create(&t3, 0, thread3, 0);
    pthread_create(&t4, 0, thread4, 0);
    pthread_create(&t5, 0, thread5, 0);
    pthread_create(&t6, 0, thread6, 0);
    pthread_create(&t7, 0, thread7, 0);
    pthread_create(&t8, 0, thread8, 0);
    pthread_create(&t9, 0, thread9, 0);
    pthread_create(&t10, 0, thread10, 0);

    __VERIFIER_assert( a||b||c||d||e);

    return 0;
}

