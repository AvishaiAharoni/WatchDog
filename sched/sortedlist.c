#include <assert.h> /* assert */
#include <stdlib.h> /* malloc, free */

#include "dlist.h"
#include "sortedlist.h"

struct sdlist
{
	dlist_t *dlist;
	void *param;
    int (* is_before)(const void* data1, const void *data2, void *param);
};

/* the function creates a new sorted list.
    the function creates also a node that is a dlist.
    returns the sorted list (or NULL if one of the allocation failed) */
sdlist_t *SortedListCreate(is_before_t func, void *param)
{
	sdlist_t *new_sdlist = NULL;
	
    /* checking parameters */
    assert(NULL != func);
    	
	new_sdlist = (sdlist_t *)malloc(sizeof(sdlist_t));
	if (NULL == new_sdlist)
	{
		return (NULL);
	}
	
	/* to create the node dlist */
	new_sdlist->dlist = DLCreate();
	if (NULL == new_sdlist->dlist)
	{
		free(new_sdlist); new_sdlist = NULL;
		
		return (NULL);
	}
	
	new_sdlist->param = param;
	new_sdlist->is_before = func;
	
	return (new_sdlist);
}

/****************************************************************************/

/* to destroy a sorted dlist */
void SortedListDestroy(sdlist_t *s_dlist)
{
    /* checking parameters */
    assert(NULL != s_dlist);
    
    DLDestroy(s_dlist->dlist); s_dlist->dlist = NULL;
    
	free(s_dlist); s_dlist = NULL;
}

/****************************************************************************/

/* to return the first node of a sorted dlist.
	if the sorted dlist is empty - returns the tail */
sdlist_info_t SortedListBegin(const sdlist_t *s_dlist)
{
    sdlist_info_t begin = {NULL};
    
    /* checking parameters */
    assert(NULL != s_dlist);
	
	begin.info = DLBegin(s_dlist->dlist);
	
	return (begin);
}

/****************************************************************************/

/* to return the last node of a sorted dlist.
	if the sorted dlist is empty - returns the tail */
sdlist_info_t SortedListEnd(const sdlist_t *s_dlist)
{
    sdlist_info_t end = {NULL};
    
    /* checking parameters */
    assert(NULL != s_dlist);
	
	end.info = DLEnd(s_dlist->dlist);
	
	return (end);
}

/****************************************************************************/

/* to count how many iterators are in the sorted dlist */
size_t SortedListSize(const sdlist_t *s_dlist)
{
    /* checking parameters */
    assert(NULL != s_dlist);
	
	return (DLCount(s_dlist->dlist));
}

/****************************************************************************/

/* to check if the sorted dlist is empty.
	returns 1 if yes, else - 0 */
int SortedListIsEmpty(const sdlist_t *s_dlist)
{
   /* checking parameters */
    assert(NULL != s_dlist);

	return (DLIsEmpty(s_dlist->dlist));
}

/****************************************************************************/

/* to get the data in a given iterator */
void *SortedListGetData(const sdlist_info_t iter)
{
    /* checking parameters */
    assert(NULL != iter.info);
	
	return (DLGetData((dlist_iterator_t)iter.info));
}

/****************************************************************************/

/* to check if two iterators are the same.
	returns 1 if yes, else - 0 */
int SortedListIsSameIterator(const sdlist_info_t iter1,
							  const sdlist_info_t iter2)
{
    /* checking parameters */
    assert((NULL != iter1.info) && (NULL != iter2.info));
	
	return (DLIsSameIterator((dlist_iterator_t)iter1.info,
							   (dlist_iterator_t)iter2.info));
}

/****************************************************************************/

/* to return the next iterator of a given iterator.
	returns tail if item is end */
sdlist_info_t SortedListNext(const sdlist_info_t iter)
{
    sdlist_info_t next = {NULL};
    
    /* checking parameters */
    assert(NULL != iter.info);
	
	next.info = DLNext((dlist_iterator_t)iter.info);
	
	return (next);
}

/****************************************************************************/

/* to return the prev iterator of a given iterator.
	returns begin if item is begin */
sdlist_info_t SortedListPrev(const sdlist_info_t iter)
{
    sdlist_info_t prev = {NULL};
    
    /* checking parameters */
    assert(NULL != iter.info);

	prev.info = DLPrev((dlist_iterator_t)iter.info);
	
	return (prev);

}

/****************************************************************************/

/* to pop the last iterator in the sorted dlist.
	if the list is empty - returns the tail */
void SortedListPopBack(sdlist_t *s_dlist)
{
    /* checking parameters */
    assert(NULL != s_dlist);
	
	if (1 == SortedListIsEmpty(s_dlist))
	{
		return;
	}
	
	DLPopBack(s_dlist->dlist);
}

/****************************************************************************/

/* to pop the first iterator in the sorted dlist
	if the list is empty - returns the tail */
void SortedListPopFront(sdlist_t *s_dlist)
{
    /* checking parameters */
    assert(NULL != s_dlist);
	
	if (1 == SortedListIsEmpty(s_dlist))
	{
		return;
	}

	DLPopFront(s_dlist->dlist);
}

/****************************************************************************/

/* to do a given func on a scope of iterators ('from' to 'to' (not include 'to')).
    'from' have to be before 'to', otherwise - undefine.
    note: the list is sorted, and it's the user's responsiblity to left it like this
    after the use in this function.
    returns the value of the func if failed, 0 if success */
int SortedListForEach(sdlist_info_t from, sdlist_info_t to,
					   do_func_t func, void *param)
{
	int res = 0;
	
    /* checking parameters */
    assert((NULL != func) && (NULL != from.info) && (NULL != to.info));

	res = DLForEach((dlist_iterator_t)from.info, (dlist_iterator_t)to.info,
					 func, param);
	
	return (res);
}

/****************************************************************************/

/* to find the first occurrence of an iterator in a scope
	('from' - 'to' (not including)),
    by a find_func and parameter. 'from' have to be before 'to', otherwise - undefine.
    returns the iterator that found. if didn't find any - returns 'to' */
sdlist_info_t SortedListFind(const sdlist_info_t from, const sdlist_info_t to,
							 find_func_t func, void *param)
{
	sdlist_info_t find = {NULL};

    /* checking parameters */
    assert((NULL != func) && (NULL != from.info) && (NULL != to.info));

	find.info = DLFind((dlist_iterator_t)from.info,
					   (dlist_iterator_t)to.info,
						func, param);
	
	return (find);	
}
							
/****************************************************************************/

/* to erase a given iterator and destroy it.
    returns the next iterator.
    if the given iterator is the tail - returns the tail */
sdlist_info_t SortedListErase(sdlist_info_t iter)
{
	sdlist_info_t removed = {NULL};

    /* checking parameters */
    assert(NULL != iter.info);
	
	removed.info = DLErase((dlist_iterator_t)iter.info);
	
	return (removed);
}

/****************************************************************************/

/* to insert a new item to the sorted list.
	find the place to insert (before) by the function that given by the user.
	returns the new item, or the tail if failed */
sdlist_info_t SortedListInsert(sdlist_t *s_dlist, const void *data)
{
	sdlist_info_t find = {NULL};
		
    /* checking parameters */
    assert((NULL != s_dlist) && (NULL != data));

	find = SortedListBegin(s_dlist);
	
	/* loop until finding the place to insert the new data (or reaching the end) */
	while ((0 == SortedListIsSameIterator(find, SortedListEnd(s_dlist))) &&
		  (1 == s_dlist->is_before(SortedListGetData(find), data, s_dlist->param)))
	{
		find = SortedListNext(find);
	}
		
	find.info = DLInsert((dlist_iterator_t)find.info, s_dlist->dlist, data);
	
	return (find);
}

/****************************************************************************/

/* to merge two sorted lists to a long one.
	insert every item from src to its place in dest.
	returns pointer to dest */
sdlist_t *SortedListMerge(sdlist_t *dest, sdlist_t *src)
{
	sdlist_info_t from_src = {0};
	sdlist_info_t to_src = {0};
	sdlist_info_t where = {0};

    /* checking parameters */
    assert((NULL != dest) && (NULL != src));
    
	from_src = SortedListBegin(src);
	to_src = from_src;
	
	where = SortedListBegin(dest);
	
	/* loop until finishing src or dest */
	while ((0 == SortedListIsSameIterator(from_src, SortedListEnd(src))) &&
			(0 == SortedListIsSameIterator(where, SortedListEnd(dest))))
	{
		/* loop on dest to find where to insert src */
		while ((0 == SortedListIsSameIterator(where, SortedListEnd(dest))) &&
			(0 == dest->is_before(SortedListGetData(to_src),
								  SortedListGetData(where), dest->param)))
		{
			where = SortedListNext(where);
		}
		
		/* to check if where reach to the tail */
		if (1 == SortedListIsSameIterator(where, SortedListEnd(dest)))
		{
			break;
		}
		
		/* loop on src to find how many items to insert */
		while ((0 == SortedListIsSameIterator(to_src, SortedListEnd(src))) &&
				(0 == dest->is_before(SortedListGetData(where),
									  SortedListGetData(to_src), dest->param)))
		{
			to_src = SortedListNext(to_src);
		}
		
		/* if there only one item to insert */
		if (1 == SortedListIsSameIterator(from_src, to_src))
		{
			to_src = SortedListNext(to_src);
		}
		
		/* to splice the items to teir place */
		DLSplice((dlist_iterator_t)where.info, (dlist_iterator_t)from_src.info,
				 (dlist_iterator_t)to_src.info);

		from_src = to_src;
		where = SortedListNext(where);		
	}
	
	/* if there are items in src that needed to insert at the end of dest */
	if (0 == SortedListIsEmpty(src))
	{
		from_src = SortedListBegin(src);
		to_src = SortedListEnd(src);
		
		where = SortedListEnd(dest);
		DLSplice((dlist_iterator_t)where.info, (dlist_iterator_t)from_src.info,
				 (dlist_iterator_t)to_src.info);	
	}
	
	return (dest);
}
