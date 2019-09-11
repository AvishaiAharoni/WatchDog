#include <assert.h> /* assert */
#include <stdlib.h> /* malloc, free */

#include "dlist.h"

struct dlist_node
{
    void *val;
    struct dlist_node *next;
    struct dlist_node *prev;
};

struct dlist
{
    dlist_node_t head;
    dlist_node_t tail;
};

/****************************************************************************/

/* the function creates a new dlist.
    the function creates two nodes that are the head and the tail of the dlist.
    returns the dlist (or NULL if one of the allocation failed) */
dlist_t *DLCreate(void)
{
    dlist_t *new_dlist = (dlist_t *)malloc(sizeof(dlist_t));
    if (NULL == new_dlist)
    {
        return (NULL);
    }
   
    new_dlist->head.val = NULL;
    new_dlist->tail.val = NULL;
   
    /* to link the head and the tail together */
    new_dlist->head.next = &new_dlist->tail;
    new_dlist->tail.prev = &new_dlist->head;
  
    new_dlist->tail.next = NULL;
    new_dlist->head.prev = NULL;
  
    return (new_dlist);
}

/****************************************************************************/

/* to return the first iterator.
    if the dlist is empty, returns the tail */
dlist_iterator_t DLBegin(const dlist_t *my_dlist)
{
    /* checking parameters */
    assert(NULL != my_dlist);

    return (DLNext((dlist_iterator_t)&my_dlist->head));
}

/****************************************************************************/

/* to return the tail iterator.
    if the dlist is empty, returns the tail */
dlist_iterator_t DLEnd(const dlist_t *my_dlist)
{
    /* checking parameters */
    assert(NULL != my_dlist);
  
    return ((dlist_iterator_t)&my_dlist->tail);
}

/****************************************************************************/

/* checks if the dlist is empty.
    returns 1 if true, and 0 else */
int DLIsEmpty(const dlist_t *my_dlist)
{
    /* checking parameters */
    assert(NULL != my_dlist);

    return (DLBegin(my_dlist) == DLEnd(my_dlist));
}

/****************************************************************************/

/* to return the data from a given iterator */
void *DLGetData(const dlist_iterator_t iter)
{
    /* checking parameters */
    assert(NULL != iter);
  
    return (iter->val);
}

/****************************************************************************/

/* to get the next iterator.
    if the given iterator is the end - returns the end */
dlist_iterator_t DLNext(const dlist_iterator_t iter)
{
    /* checking parameters */
    assert(NULL != iter);

    return ((NULL != iter->next) ? iter->next : iter);
}

/****************************************************************************/

/* to count how many iterators are in thr dlist */
size_t DLCount(const dlist_t *my_dlist)
{
    size_t counter = 0;
    dlist_node_t *my_node = NULL;

    /* checking parameters */
    assert(NULL != my_dlist);
  
    my_node = DLBegin(my_dlist);
    while (NULL != my_node->next)
    {
        ++counter;
        my_node = DLNext(my_node);
    }
  
    return (counter);
}

/****************************************************************************/

/* to get the prev iterator.
    if the given iterator is the first - returns the begin iterator */
dlist_iterator_t DLPrev(const dlist_iterator_t iter)
{
    /* checking parameters */
    assert(NULL != iter);

    return ((NULL != iter->prev->prev) ? iter->prev : iter);
}

/****************************************************************************/

/* to check whether the data in two given iterators is the same.
    returns 1 if true, 0 else */
int DLIsSameIterator(const dlist_iterator_t iter1, const dlist_iterator_t iter2)
{
    /* checking parameters */
    assert((NULL != iter1) && (NULL != iter2));

    return (iter1 == iter2);
}

/****************************************************************************/

/* to erase a given iterator and destroy it.
    returns the next iterator.
    if the given iterator is the tail - returns the tail */
dlist_iterator_t DLErase(dlist_iterator_t to_erase)
{
    dlist_iterator_t removed = NULL;

    /* checking parameters */
    assert(NULL != to_erase);
  
    /* to check if the iterator is the tail */
    if (NULL == to_erase->next)
    {
        return (to_erase);
    }
   
    removed = to_erase;
   
    /* to set the pointers above the removed iterator */
    removed = removed->prev;
    removed->next = removed->next->next;
   
    removed = DLNext(removed);  
    removed->prev = removed->prev->prev;
  
    to_erase->prev = NULL;
    to_erase->next = NULL;
  
    free(to_erase); to_erase = NULL;
   
    return (removed);
}

/****************************************************************************/

/* to remove (and destroy) the first iterator in a given dlist */
void DLPopFront(dlist_t *my_dlist)
{
    /* checking parameters */
    assert(NULL != my_dlist);
  
    if (0 == DLIsEmpty(my_dlist))
    {
        DLErase(DLBegin(my_dlist));
    }
}

/****************************************************************************/

/* to remove (and destroy) the last iterator in a given dlist */
void DLPopBack(dlist_t *my_dlist)
{
    /* checking parameters */
    assert(NULL != my_dlist);
  
    if (0 == DLIsEmpty(my_dlist))
    {
        DLErase(DLPrev(DLEnd(my_dlist)));
    }
}

/****************************************************************************/

/* to insert an iterator before a given iterator (by allocation).
    the function can't insert an iterator before the 'head'.
    returns the tail if fail, and returns the new node iterator if success */
dlist_iterator_t DLInsert(dlist_iterator_t where, dlist_t *my_dlist, const void *data)
{
    dlist_iterator_t new_iter = NULL;
  
    /* checking parameters */
    assert((NULL != my_dlist) && (NULL != where) && (NULL != data));
   
    /* to create a new iterator */
    new_iter = (dlist_iterator_t)malloc(sizeof(dlist_node_t));
    if (NULL == new_iter)
    {
        return (DLEnd(my_dlist));
    }
  
    new_iter->val = (void *)data;
  
    /* to set the new iteraton in its place in the dlist */
    new_iter->next = where;
    new_iter->prev = where->prev;
   
    where->prev->next = new_iter;
    where->prev = new_iter;
  
    return (new_iter);
}

/****************************************************************************/

/* to insert an iterator after a given iterator (by allocation).
    the function can't insert an iterator after the 'tail'.
    returns the tail if fail or if the given iterator is the 'tail',
    and returns the new iterator if success */
dlist_iterator_t DLInsertAfter(dlist_iterator_t where, dlist_t *my_dlist,
                                const void *data)
{
    /* checking parameters */
    assert((NULL != my_dlist) && (NULL != where));

    /* to check if the given iterator is the tail */
    if (NULL == where->next)
    {
        return (where);
    }

    return (DLInsert(DLNext(where), my_dlist, data));
}

/****************************************************************************/

/* to insert an iterator at the start of a given dlist (by allocation).
    return the tail if fail, return the new iterator if success */
dlist_iterator_t DLPushFront(dlist_t *my_dlist, const void *data)
{
    /* checking parameters */
    assert(NULL != my_dlist);
  
    return (DLInsertAfter(&my_dlist->head, my_dlist, data));
}

/****************************************************************************/

/* to insert an iterator at the end of a given dlist (by allocation).
    return the tail if fail, return the new iterator if success */
dlist_iterator_t DLPushBack(dlist_t *my_dlist, const void *data)
{
    /* checking parameters */
    assert(NULL != my_dlist);
  
    return (DLInsert(DLEnd(my_dlist), my_dlist, data));
}

/****************************************************************************/

/* to destroy a dlist */
void DLDestroy(dlist_t *my_dlist)
{
    dlist_iterator_t to_remove = NULL;
    dlist_iterator_t remove_next = NULL;
   
    /* checking parameters */
    assert(NULL != my_dlist);

    to_remove = DLBegin(my_dlist);
       
    while (0 == DLIsSameIterator(to_remove, DLEnd(my_dlist)))
    {
        remove_next = DLNext(to_remove);       
        free(to_remove); to_remove = NULL;
        to_remove = remove_next;       
    }
  
    free(my_dlist); my_dlist = NULL;
}

/****************************************************************************/

/* to splice some iterators (named by src_from to src_to) before a given iterator
    (where). 'from' and 'to' must not overlap with where.
    'from' have to be before 'to', otherwise - undefine.
    returns the next iterator after the splice (where) */
dlist_iterator_t DLSplice(dlist_iterator_t where,
                            dlist_iterator_t src_from, dlist_iterator_t src_to)
{
    dlist_iterator_t iter_p = NULL;

   /* checking parameters */
    assert((NULL != where) && (NULL != src_from) && (NULL != src_to));

    /* for splices an empty dlist */
    if (NULL == src_from->next)
    {
        return (where);
    }
   
    /* to set the rest of src linked together */
    iter_p = where->prev;

    src_to->prev->next = where;
    where->prev->next = src_from;    
    src_from->prev->next = src_to;
    
    where->prev = src_to->prev;
    src_to->prev = src_from->prev;
    src_from->prev = iter_p;
  
    return (where);
}

/****************************************************************************/

/* to do a given func on a scope of iterators ('from' to 'to' (not include 'to')).
    'from' have to be before 'to', otherwise - undefine.
    returns the value of the func if failed, 0 if success */
int DLForEach(dlist_iterator_t from, dlist_iterator_t to,
                int (* func)(void *data, void *param), void *param)
{
    dlist_iterator_t current = NULL;
    int res = 0;
  
    /* checking parameters */
    assert((NULL != func) && (NULL != from) && (NULL != to));
  
    current = from;
    while (0 == DLIsSameIterator(current, to))
    {
        res = func(DLGetData(current), param);
        if (0 != res)
        {
            return (res);
        }
        current = DLNext(current);
    }
  
    return (0);
}

/****************************************************************************/

/* to find the first accuration of an iterator in a scope ('from' - 'to' (not include)),
    by a find_func and parameter. 'from' have to be before 'to', otherwise - undefine.
    there is also a flag that can be used to do a special search.
    returns the iterator that found. if didn't find any - returns 'to' */
dlist_iterator_t DLFind(const dlist_iterator_t from, const dlist_iterator_t to,
                        int (* func)(const void *data, void *param), void *param)
{
    dlist_iterator_t current = NULL;

    /* checking parameters */
    assert((NULL != func) && (NULL != from) && (NULL != to));
  
    current = from;
    while (0 == DLIsSameIterator(current, to))
    {
        if (1 == (func(current->val, param)))
        {
            return (current);
        }
        current = DLNext(current);
    }
  
    return (to);
}
