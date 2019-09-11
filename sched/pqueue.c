#include <assert.h> /* assert */
#include <stdlib.h> /* malloc, free */

#include "dlist.h"
#include "sortedlist.h"
#include "pqueue.h"

struct pqueue
{
	sdlist_t *q_head;
};

/****************************************************************************/

/* the function creates a new pqueue.
    the function creates also a node that is a sdlist.
    returns the queue (or NULL if one of the allocation failed) */
pqueue_t *PQCreate(is_before_t func, void *param)
{
	pqueue_t *new_pqueue = NULL;

    /* checking parameters */
    assert(NULL != func);

	new_pqueue = (pqueue_t *)malloc(sizeof(pqueue_t));
	if (NULL == new_pqueue)
	{
		return (NULL);
	}
	
	/* to create the node dlist */
	new_pqueue->q_head = SortedListCreate(func, param);
	if (NULL == new_pqueue->q_head)
	{
		free(new_pqueue); new_pqueue = NULL;
		
		return (NULL);
	}
	
	return (new_pqueue);
}

/****************************************************************************/

/* to destroy a queue */
void PQDestroy(pqueue_t *to_destroy)
{
    /* checking parameters */
    assert(NULL != to_destroy);
    
    SortedListDestroy(to_destroy->q_head);
    
	free(to_destroy); to_destroy = NULL;
}

/****************************************************************************/

/* to show the data in the begin iterator of the pqueue 
	if the pqueue if empty - returns NULL */
void *PQPeek(const pqueue_t *my_pqueue)
{
    /* checking parameters */
    assert(NULL != my_pqueue);

	return (SortedListGetData(SortedListBegin(my_pqueue->q_head)));
}

/****************************************************************************/

/* to count how many iterators are in the pqueue */
size_t PQSize(const pqueue_t *my_pqueue)
{
    /* checking parameters */
    assert(NULL != my_pqueue);

	return (SortedListSize(my_pqueue->q_head));
}

/****************************************************************************/

/* to check if th pqueue is empty.
	returns 1 if empty, 0 - otherwise */
int PQIsEmpty(const pqueue_t *my_pqueue)
{
    /* checking parameters */
    assert(NULL != my_pqueue);

	return (SortedListIsEmpty(my_pqueue->q_head));
}

/****************************************************************************/

/* to clear all the iterators in the pqueue */
void PQClear(pqueue_t *pqueue_to_clear)
{
    /* checking parameters */
    assert(NULL != pqueue_to_clear);

	while (0 == PQIsEmpty(pqueue_to_clear))
	{
		PQDequeue(pqueue_to_clear);
	}
}

/****************************************************************************/

/* to insert a new iterator to its place in the pqueue (by the order).
	returns 0 for success, and 1 for failure  */
int PQEnqueue(pqueue_t *my_pqueue, void *data)
{
	sdlist_info_t inserted = {NULL};

    /* checking parameters */
    assert((NULL != my_pqueue) && (NULL != data));

	/* inserting the iterator */
	inserted = SortedListInsert(my_pqueue->q_head, data);

	/* checking if the insert succeed */
	return (SortedListIsSameIterator(inserted, SortedListEnd(my_pqueue->q_head)));
}

/****************************************************************************/

/* to pop the first element in the pqueue */
void PQDequeue(pqueue_t *my_pqueue)
{
    /* checking parameters */
    assert(NULL != my_pqueue);

	SortedListPopFront(my_pqueue->q_head);
}

/****************************************************************************/

/* to erase a specific iterator from the pqueue.
	returns the data of the iterator.
	if the param didn't find - returns NULL */
void *PQErase(pqueue_t *my_pqueue, find_func_t func, void *param)
{
	sdlist_info_t to_erase = {NULL};
	
    /* checking parameters */
    assert((NULL != my_pqueue) && (NULL != func) && (NULL != param));
	
	to_erase = SortedListFind(SortedListBegin(my_pqueue->q_head),
											SortedListEnd(my_pqueue->q_head),
											func, param);
	
	if (1 == SortedListIsSameIterator(to_erase, SortedListEnd(my_pqueue->q_head)))
	{
		return (NULL);
	}
	
	SortedListErase(to_erase);

	return (param);
}
