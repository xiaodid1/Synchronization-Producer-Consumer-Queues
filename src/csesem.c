#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "csesem.h"

/* This definition of struct CSE_Semaphore is only available _inside_
 * your semaphore implementation.  This prevents calling code from
 * inadvertently invalidating the internal representation of your
 * semaphore.  See csesem.h for more information.
 *
 * You may place any data you require in this structure. */
struct CSE_Semaphore {
    int i; //# of count
    pthread_cond_t c; //device allows threads to suspend
    pthread_mutex_t m;
};

/* This function must both allocate space for the semaphore and perform
 * any initialization that is required for safe operation on the
 * semaphore.  The user should be able to immediately call csesem_post()
 * or csesem_wait() after this routine returns. */
CSE_Semaphore csesem_create(int count) {
    if(count >= 0){
        CSE_Semaphore sem = calloc(1, sizeof(struct CSE_Semaphore));
        sem->i = count;
        pthread_cond_init(&(sem->c),NULL);
        pthread_mutex_init(&(sem->m), NULL);
        return sem;
    }
    return NULL;
}

void csesem_post(CSE_Semaphore sem) {
    pthread_mutex_lock(&(sem->m));
    sem->i+=1;
    pthread_mutex_unlock(&(sem->m));
    pthread_cond_broadcast(&(sem->c));
}

void csesem_wait(CSE_Semaphore sem) {
    pthread_mutex_lock(&(sem->m));
    while(sem->i<=0){
        pthread_cond_wait(&(sem->c),&(sem->m));
    }
    sem->i-=1;
    pthread_mutex_unlock(&(sem->m));
}

/* This function should destroy any resources allocated for this
 * semaphore; this includes mutexes or condition variables. */
void csesem_destroy(CSE_Semaphore sem) {
    pthread_cond_destroy(&(sem->c));
    pthread_mutex_destroy(&(sem->m));
    free(sem);
}
