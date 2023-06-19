/* Adapted from http://link.springer.com/chapter/10.1007%2F978-3-642-37036-6_28 */

extern void abort(void);
void reach_error(){}

#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
void __VERIFIER_assert(int expression) { if (!expression) { ERROR: {reach_error();abort();}}; return; }

int a1,a2,a3,a4,a5,a6,a7,a8,a9,a10;
int b1,b2,b3,b4,b5,b6,b7,b8,b9,b10;
int c1,c2,c3,c4,c5,c6,c7,c8,c9,c10;

void *threadone(void * arg)
{
    a1=1;
    return 0;
}

void *threadtwo(void *arg)
{
    a2=a1;
    return 0;
}

void *threadthree(void *arg)
{
    a3=a2;
    return 0;
}

void *threadfour(void *arg)
{
    a4=a3;
    return 0;
}

void *threadfive(void *arg)
{
    a5=a4;
    return 0;
}

void *threadsix(void * arg)
{
    a6=a5;
    return 0;
}

void *threadseven(void *arg)
{
    a7=a6;
    return 0;
}

void *threadeight(void *arg)
{
    a8=a7;
    return 0;
}

void *threadnine(void *arg)
{
    a9=a8;
    return 0;
}

void *threadten(void *arg)
{
    a10=a9;
    return 0;
}

void *threadone1(void * arg)
{
    b1=1;
    return 0;
}

void *threadtwo1(void *arg)
{
    b2=b1;
    return 0;
}

void *threadthree1(void *arg)
{
    b3=b2;
    return 0;
}

void *threadfour1(void *arg)
{
    b4=b3;
    return 0;
}

void *threadfive1(void *arg)
{
    b5=b4;
    return 0;
}

void *threadsix1(void * arg)
{
    b6=b5;
    return 0;
}

void *threadseven1(void *arg)
{
    b7=b6;
    return 0;
}

void *threadeight1(void *arg)
{
    b8=b7;
    return 0;
}

void *threadnine1(void *arg)
{
    b9=b8;
    return 0;
}

void *threadten1(void *arg)
{
    b10=b9;
    return 0;
}

void *threadone2(void * arg)
{
    c1=1;
    return 0;
}

void *threadtwo2(void *arg)
{
    c2=c1;
    return 0;
}

void *threadthree2(void *arg)
{
    c3=c2;
    return 0;
}

void *threadfour2(void *arg)
{
    c4=c3;
    return 0;
}

void *threadfive2(void *arg)
{
    c5=c4;
    return 0;
}

void *threadsix2(void * arg)
{
    c6=c5;
    return 0;
}

void *threadseven2(void *arg)
{
    c7=c6;
    return 0;
}

void *threadeight2(void *arg)
{
    c8=c7;
    return 0;
}

void *threadnine2(void *arg)
{
    c9=c8;
    return 0;
}

void *threadten2(void *arg)
{
    c10=c9;
    return 0;
}

int main()
{
    pthread_t t1,t2,t3,t4,t5,t6,t7,t8,t9,t10;
	pthread_t t11,t12,t13,t14,t15,t16,t17,t18,t19,t20;
	pthread_t t21,t22,t23,t24,t25,t26,t27,t28,t29,t30;

    pthread_create(&t1, 0, threadone, 0);
    pthread_create(&t2, 0, threadseven, 0);
    pthread_create(&t3, 0, threadthree, 0);
    pthread_create(&t4, 0, threadfour, 0);
    pthread_create(&t5, 0, threadfive, 0);
    pthread_create(&t6, 0, threadsix, 0);
    pthread_create(&t7, 0, threadseven, 0);
    pthread_create(&t8, 0, threadeight, 0);
    pthread_create(&t9, 0, threadnine, 0);
    pthread_create(&t10, 0, threadten1, 0);
    pthread_create(&t11, 0, threadone1, 0);
    pthread_create(&t12, 0, threadseven1, 0);
    pthread_create(&t13, 0, threadthree1, 0);
    pthread_create(&t14, 0, threadfour1, 0);
    pthread_create(&t15, 0, threadfive1, 0);
    pthread_create(&t16, 0, threadsix1, 0);
    pthread_create(&t17, 0, threadseven1, 0);
    pthread_create(&t18, 0, threadeight1, 0);
    pthread_create(&t19, 0, threadnine1, 0);
    pthread_create(&t20, 0, threadten1, 0);
    pthread_create(&t21, 0, threadone2, 0);
    pthread_create(&t22, 0, threadseven2, 0);
    pthread_create(&t23, 0, threadthree2, 0);
    pthread_create(&t24, 0, threadfour2, 0);
    pthread_create(&t25, 0, threadfive2, 0);
    pthread_create(&t26, 0, threadsix2, 0);
    pthread_create(&t27, 0, threadseven2, 0);
    pthread_create(&t28, 0, threadeight2, 0);
    pthread_create(&t29, 0, threadnine2, 0);
    pthread_create(&t30, 0, threadten2, 0);
	
    assert( !(a1&&a2&&a3&&a4&&a5&&a6&&a7&&a8&&a9&&a10) );

    return 0;
}

