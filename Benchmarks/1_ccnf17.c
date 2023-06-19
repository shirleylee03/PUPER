/* Adapted from https://svn.sosy-lab.org/software/sv-benchmarks/trunk/c/pthread/stateful01_false-unreach-call.c */

extern void abort(void);
void reach_error(){}
void __VERIFIER_assert(int expression) { if (!expression) { ERROR: {reach_error();abort();}}; return; }

#include "pthread.h"

int a=0;
int b=0;
int c=0;
int d=0;
int e=0;
int f=0;
int g=0;
int h=0;

void *t1(void *arg){a=1;}
void *t2(void *arg){a=2;}
void *t3(void *arg){b=1;}
void *t4(void *arg){b=2;}
void *t5(void *arg){c=1;}
void *t6(void *arg){c=2;}
void *t7(void *arg){d=1;}
void *t8(void *arg){d=2;}
void *t9(void *arg){e=1;}
void *t10(void *arg){e=2;}
void *t11(void *arg){f=1;}
void *t12(void *arg){f=2;}
void *t13(void *arg){g=1;}
void *t14(void *arg){g=2;}
void *t15(void *arg){h=1;}
void *t16(void *arg){h=2;}

int main()
{
  pthread_create(id1, NULL, t1, NULL);
  pthread_create(id2, NULL, t2, NULL);
  pthread_create(id3, NULL, t3, NULL);
  pthread_create(id4, NULL, t4, NULL);
  pthread_create(id5, NULL, t5, NULL);
  pthread_create(id6, NULL, t6, NULL);
  pthread_create(id7, NULL, t7, NULL);
  pthread_create(id8, NULL, t8, NULL);
  pthread_create(id9, NULL, t9, NULL);
  pthread_create(id10, NULL, t10, NULL);
  pthread_create(id11, NULL, t11, NULL);
  pthread_create(id12, NULL, t12, NULL);
  pthread_create(id13, NULL, t13, NULL);
  pthread_create(id14, NULL, t14, NULL);
  pthread_create(id15, NULL, t15, NULL);
  pthread_create(id16, NULL, t16, NULL);
  
  __VERIFIER_assert(a==1&&d==2); 
  
}
