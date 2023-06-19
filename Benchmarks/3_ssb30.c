/* Adapted from PGSQL benchmark from http://www.cs.ox.ac.uk/people/marcelo.sousa/poet/.
 * Test: SSB (Sleep set blocking executions)
*/
extern void abort(void);
void reach_error(){}

#include <pthread.h>
#define N 30

int x=0;
int y=0;
int z=0;

void *p(void * arg){
    x = 1;
    if(x!=1){
        reach_error();
    }
}

void *q(void * arg){
    y = 1;
}

void *r(void * arg){
    int aux=0;
    while(y < N){
      if (z == 1){
        x=2;
      }
      aux=y;
      y=aux+1;
      aux=0;
    }
}

void *s(void * arg){
    if (y == 0){
        z=1;
    }
}
int main(){
    /* references to the threads */
    pthread_t p_t;
    pthread_t q_t;
    pthread_t r_t;
    pthread_t s_t;

    /* create the threads and execute */
    pthread_create(&p_t, 0, p, 0);
    pthread_create(&q_t, 0, q, 0);
    pthread_create(&r_t, 0, r, 0);
    pthread_create(&s_t, 0, s, 0);

    /* wait for the threads to finish */
    pthread_join(p_t, 0);
    pthread_join(q_t, 0);
    pthread_join(r_t, 0);
    pthread_join(s_t, 0);
}
