#ifndef DLIST_H

#define DLIST_H

#include <stddef.h>

typedef struct dlist_node * dlist_iterator_t;
typedef struct dlist dlist_t;

typedef int (*do_func_t)(void *data, void *param);
typedef int (*find_func_t)(const void *data, void *param);

typedef struct dlist_node dlist_node_t;


/*********** functions *************/

/* the function creates a new dlist.
    the function creates two nodes that are the head and the tail of the dlist.
    returns a pointer to the head node (or NULL if one of the allocation failed) */
dlist_t *DLCreate(void);

/* checks if the dlist is empty.
    returns 1 if true, and 0 else */
int DLIsEmpty(const dlist_t *my_dlist);

/* to destroy a dlist */
void DLDestroy(dlist_t *my_dlist);

/* to count how many iterators are in thr dlist */
size_t DLCount(const dlist_t *my_dlist);

/* to return the first iterator.
    if the dlist is empty, returns the tail */
dlist_iterator_t DLBegin(const dlist_t *my_dlist);

/* to return the tail iterator.
    if the dlist is empty, returns the tail */
dlist_iterator_t DLEnd(const dlist_t *my_dlist);

/* to get the next iterator.
    if the given iterator is the end - returns the end */
dlist_iterator_t DLNext(const dlist_iterator_t iter);

/* to get the prev iterator.
    if the given iterator is the first - returns the begin iterator */
dlist_iterator_t DLPrev(const dlist_iterator_t iter);

/* to return the data from a given iterator */
void *DLGetData(const dlist_iterator_t iter);

/* to check whether the data in two given iterators is the same.
    returns 1 if true, 0 else */
int DLIsSameIterator(const dlist_iterator_t iter1, const dlist_iterator_t iter2);

/* to insert an iterator before a given iterator (by allocation).
    the function can't insert an iterator before the 'head'.
    returns the tail if fail, and returns the new node iterator if success */
dlist_iterator_t DLInsert(dlist_iterator_t where, dlist_t *my_dlist, const void *data);

/* to insert an iterator after a given iterator.
	the function can't insert an iterator after the 'tail'.
    returns the tail if fail or if the given iterator is the 'tail',
    and returns the new iterator if success */
dlist_iterator_t DLInsertAfter(dlist_iterator_t where, dlist_t *my_dlist,
								const void *data);

/* to erase a given iterator and destroy it.
	returns the next iterator.
	if the given iterator is the tail - returns the tail */
dlist_iterator_t DLErase(dlist_iterator_t to_erase);

/* to insert an iterator at the start of a given dlist (by allocation).
    return the tail if fail, return the new iterator if success */
dlist_iterator_t DLPushFront(dlist_t *my_dlist, const void *data);

/* to insert an iterator at the end of a given dlist (by allocation).
    return the tail if fail, return the new iterator if success */
dlist_iterator_t DLPushBack(dlist_t *my_dlist, const void *data);

/* to remove (and destroy) the first iterator in a given dlist */
void DLPopFront(dlist_t *my_dlist);

/* to remove (and destroy) the last iterator in a given dlist */
void DLPopBack(dlist_t *my_dlist);

/* to splice some iterators (named by src_from to src_to) before a given iterator
    (where). 'from' and 'to' must not overlap with where.
    'from' have to be before 'to', otherwise - undefine.
    returns the next iterator after the splice (where) */
dlist_iterator_t DLSplice(dlist_iterator_t where, dlist_iterator_t src_from, dlist_iterator_t src_to);

/* to do a given func on a scope of iterators ('from' to 'to' (not include 'to')).
    'from' have to be before 'to', otherwise - undefine.
    returns the value of the func if failed, 0 if success */
int DLForEach(dlist_iterator_t from, dlist_iterator_t to, do_func_t func, void *param);

/* to find the first accuration of an iterator in a scope ('from' - 'to' (not include)),
    by a find_func and parameter. 'from' have to be before 'to', otherwise - undefine.
    there is also a flag that can be used to do a special search.
    returns the iterator that found. if didn't find any - returns 'to' */
dlist_iterator_t DLFind(const dlist_iterator_t from, const dlist_iterator_t to, find_func_t func, void *param);


#endif /* DLIST_H */
