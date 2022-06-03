#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "csesem.h"
#include "pcq.h"

/* This structure must contain everything you need for an instance of a
 * PCQueue.  The given definition is ABSOLUTELY NOT COMPLETE.  You will
 * have to add several items to this structure. */
struct PCQueue {
    int slots;
    void **queue;
    CSE_Semaphore m; //mutex
    CSE_Semaphore s; //slots
    CSE_Semaphore i; //items
    int head;
    int tail;
};

/* The given implementation performs no error checking and simply
 * allocates the queue itself.  You will have to create and initialize
 * (appropriately) semaphores, mutexes, condition variables, flags,
 * etc. in this function. */
PCQueue pcq_create(int slots) {
    if (slots>0){
        PCQueue pcq;
        pcq = calloc(1, sizeof(*pcq));
        pcq->queue = calloc(slots, sizeof(void *));
        pcq->slots = slots;
        pcq->head = pcq->tail = 0;
        pcq->m = csesem_create(1);
        pcq->s = csesem_create(slots);
        pcq->i = csesem_create(0);
        return pcq;
    }
    return NULL;
}

/* This implementation does nothing, as there is not enough information
 * in the given struct PCQueue to even usefully insert a pointer into
 * the data structure. */
void pcq_insert(PCQueue pcq, void *data){
    csesem_wait(pcq->s);
    csesem_wait(pcq->m);
    pcq->queue[((++pcq->tail)%(pcq->slots))] = data;
    csesem_post(pcq->m);
    csesem_post(pcq->i);
}

/* This implementation does nothing, for the same reason as
 * pcq_insert(). */
void *pcq_retrieve(PCQueue pcq){
    if(pcq==NULL){
        return NULL;
    }
    else{
        void *outcome;
        csesem_wait(pcq->i);
        csesem_wait(pcq->m);
        outcome = pcq->queue[((++pcq->head)%(pcq->slots))];
        csesem_post(pcq->m);
        csesem_post(pcq->s);
        return outcome;
    }
}

/* The given implementation blindly frees the queue.  A minimal
 * implementation of this will need to work harder, and ensure that any
 * synchronization primitives allocated here are destroyed; a complete
 * and correct implementation will have to synchronize with any threads
 * blocked in pcq_insert() or pcq_retrieve().
 *
 * You should implement the complete and correct clean teardown LAST.
 * Make sure your other operations work, first, as they will be tightly
 * intertwined with teardown and you don't want to be debugging it all
 * at once!
 */
void pcq_destroy(PCQueue pcq) {
    csesem_post(pcq->i);
    csesem_destroy(pcq->i);
    csesem_destroy(pcq->m);
    csesem_destroy(pcq->s);
    free(pcq->queue);
    free(pcq);
}
