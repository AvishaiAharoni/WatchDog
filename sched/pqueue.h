#ifndef PQUEUE_H
#define PQUEUE_H

#include "sortedlist.h"

typedef struct pqueue pqueue_t;

/************************Functions*************************************/

/* the function creates a new pqueue.
    the function creates also a node that is a sdlist.
    returns the queue (or NULL if one of the allocation failed) */
pqueue_t *PQCreate(is_before_t func, void *param);

/* to destroy a queue */
void PQDestroy(pqueue_t *to_destroy);

/* to count how many iterators are in the pqueue */
size_t PQSize(const pqueue_t *my_pqueue);

/* to show the data in the begin iterator of the pqueue 
	if the pqueue if empty - returns NULL */
void *PQPeek(const pqueue_t *my_pqueue);

/* to check if th pqueue is empty.
	returns 1 if empty, 0 - otherwise */
int PQIsEmpty(const pqueue_t *my_pqueue);

/* to clear all the iterators in the pqueue */
void PQClear(pqueue_t *pqueue_to_clear);

/* to insert a new iterator to its place in the pqueue (by the order).
	returns for success, and 1 for failure  */
int PQEnqueue(pqueue_t *my_pqueue, void *data);

/* to pop the first element in the pqueue */
void PQDequeue(pqueue_t *my_pqueue);

/* returns the data of the erased element */
void *PQErase(pqueue_t *my_pqueue, find_func_t func, void *param);

#endif /* PQUEUE_H */
