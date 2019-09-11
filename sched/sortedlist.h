#ifndef SORTEDLIST_H

#define SORTEDLIST_H

#include <stddef.h>

#ifndef DLIST_H
#define DLIST_H

typedef int(*do_func_t)(void *data, void *param);
typedef int(*find_func_t)(const void *data, void *param);
#endif /* DLIST_H */

typedef int(*is_before_t)(const void *data1, const void *data2, void *param);

typedef struct sdlist sdlist_t;
typedef struct sdlist_info sdlist_info_t;

struct sdlist_info
{
	void *info;
};

/* the function creates a new sorted list.
    the function creates also a node that is a dlist.
    returns the sorted list (or NULL if one of the allocation failed) */
sdlist_t *SortedListCreate(is_before_t func, void *param);

/* to destroy a sorted dlist */
void SortedListDestroy(sdlist_t *s_dlist);

/* to count how many iterators are in the sorted dlist */
size_t SortedListSize(const sdlist_t *s_dlist);

/* to check if the sorted dlist is empty.
	returns 1 if yes, else - 0 */
int SortedListIsEmpty(const sdlist_t *s_dlist);

/* to return the first node of a sorted dlist.
	if the sorted dlist is empty - returns the tail */
sdlist_info_t SortedListBegin(const sdlist_t *s_dlist);

/* to return the last node of a sorted dlist.
	if the sorted dlist is empty - returns the tail */
sdlist_info_t SortedListEnd(const sdlist_t *s_dlist);

/* to check if two iterators are the same.
	returns 1 if yes, else - 0 */
int SortedListIsSameIterator(const sdlist_info_t item1, const sdlist_info_t item2);

/* to get the data in a given iterator */
void *SortedListGetData(const sdlist_info_t item);

/* to return the next iterator of a given iterator.
	returns tail if item is end */
sdlist_info_t SortedListNext(const sdlist_info_t item);

/* to return the prev iterator of a given iterator.
	returns begin if item is begin */
sdlist_info_t SortedListPrev(const sdlist_info_t item);

/* to pop the last iterator in the sorted dlist.
	if the list is empty - returns the tail */
void SortedListPopBack(sdlist_t *s_dlist);

/* to pop the first iterator in the sorted dlist
	if the list is empty - returns the tail */
void SortedListPopFront(sdlist_t *s_dlist);

/* to do a given func on a scope of iterators ('from' to 'to' (not include 'to')).
    'from' have to be before 'to', otherwise - undefine.
    returns the value of the func if failed, 0 if success */
sdlist_info_t SortedListFind(const sdlist_info_t from, const sdlist_info_t to,
							 find_func_t func, void *param);

/* to do a given func on a scope of iterators ('from' to 'to' (not include 'to')).
    'from' have to be before 'to', otherwise - undefine.
    returns the value of the func if failed, 0 if success */
int SortedListForEach(sdlist_info_t from, sdlist_info_t to, do_func_t func, void *param);

/* to erase a given iterator and destroy it.
    returns the next iterator.
    if the given iterator is the tail - returns the tail */
sdlist_info_t SortedListErase(sdlist_info_t item);

/* to insert a new item to the sorted list.
	uses whith find to find the place to insert (before).
	returns the new item, or the tail if failed */
sdlist_info_t SortedListInsert(sdlist_t *s_dlist, const void *data);

/* to merge two sorted lists to a long one.
	insert every item from src to its place in dest.
	returns pointer to dest */
sdlist_t *SortedListMerge(sdlist_t *dest, sdlist_t *src);

#endif /* SORTEDLIST_H */
