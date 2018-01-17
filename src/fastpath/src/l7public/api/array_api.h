/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2009
*
**********************************************************************
*
* @filename   array_api.h
*
* @purpose    To provide a more sophisticated array type that allows 
*             for searching and sorting of structures, and to provide
*             searching and sorting functions for basic unsigned
*             types.
*
* @component  Base
*
* @comments   This is an implementation of an array type that is 
*             far more sophisticated that the basic C array.  
*             Insertion of new elements is sped up by using an 
*             intervening table of buffer indices.  Elements can
*             be accessed and deleted, and the array can also be
*             searched (via binary search) and sorted (via Shell
*             sort).
*
*             Unfortunately, the user of this data type must supply
*             the required memory, see the description of 
*             arrayInit() for details.
*
* @create     02/09/2009
*
* @author     dcaugherty
* @end
*
**********************************************************************/

#ifndef ARRAY_API_H
#define ARRAY_API_H

#include <stdlib.h>
#include "l7_common.h"

/*******************************************************************
 *
 *                 Defintions of Types and Macros
 * 
 *******************************************************************/

typedef L7_short16 array_idx_t; /* must be signed */

/* Element comparison function.  Note that this function has the
 * same semantics of memcmp(): Return value is > 0 if first parm 
 * is "greater than" second, is < 0 if first parm is "less than"
 * second, and is == 0 otherwise (parms compare equally).
 */

typedef L7_int32 (*array_elt_cmp)(void *, void *);

/* Element functor.  Void pointer is for additonal function data.
 * see arrayForEach(), forAll().. below.
 */

typedef void (*array_elt_fn)(void * pElement, void * otherData);

/* Element predicate.  Void pointer is for additonal function data.
 * see arrayDeleteIf() below.
 */

typedef L7_BOOL (*array_pred_fn)(void * pElement, void * otherData);



#define IS_BAD_INDEX(array_idx) (0 > ((array_idx_t) array_idx))
#define BAD_INDEX               ((array_idx_t) -1)


/* Array type definition.  Note that our array of indices
 * is exactly that, not an array of pointers.  It would be
 * nice to have used a pointer approach; it would have allowed
 * us to support array slice operations, for example.  But
 * this would also have required us to use a 4-byte machine word 
 * for each element of that array to hold the pointer, instead of
 * two bytes for an index.  Saving memory is more important
 * right now, sadly.
 */

typedef struct array_s {
   array_idx_t  eltSize, capacity, size;
   void         * buf;
   array_idx_t  * idxs;
} array_t;


/* Some useful utility macros for the array_t type. */

#define IN_BOUNDS(pA, idx) ((idx >= 0) && (idx < (pA)->size))
#define ARRAY_SIZE(pA)     ((pA)->size)
#define ARRAY_FULL(pA)     ((pA)->size == (pA)->capacity)
#define ARRAY_EMPTY(pA)    (0 == (pA)->size)


/*******************************************************************
 *
 *           Declarations of Functions for Array Type
 * 
 *******************************************************************/

/*********************************************************************
* @purpose  To initialize an array_t structure
*
* @param    array_t *     @b{(output)} array_t structure to init
* @param    array_idx_t   @b{(input)}  size of an array element
* @param    array_idx_t   @b{(input)}  capacity of array (max # elements)
* @param    void    *     @b{(input)}  buffer for holding element data
* @param    array_idx_t * @b{(input)}  buffer for holding array indices
*
* @returns  pointer to initialized array_t structure if successful
* @returns  L7_NULLPTR if unsuccessful
*
* @notes    The element data buffer must have a minimum length of
*           (element size * capacity) bytes. The index buffer must
*           have a minimum length of (element size * 
*           sizeof(array_idx_t)) bytes.  Undefined behavior will 
*           occur if these constraints cannot be met.
*
* @end
*********************************************************************/
array_t *
arrayInit(array_t      *pA,
	  array_idx_t  eltSize,
	  array_idx_t  capacity,
	  void         *pDataBuf,
	  array_idx_t  *pIdxBuf);


/*********************************************************************
* @purpose  To append an item to an array
*
* @param    array_t * @b{(output)} array to augment
* @param    void    * @b{(input)}  element to copy into array
*
* @returns  L7_TRUE  if successful
* @returns  L7_FALSE otherwise
*
* @notes    array_t pointer is NOT CHECKED for null value!
*           Caller must do so before invoking this function.
*
* @end
*********************************************************************/

L7_BOOL 
arrayAppend(array_t *pA, void *pElt);

/*********************************************************************
* @purpose  To insert an item into an array at a given index
*
* @param    array_t     * @b{(output)} array to change
* @param    void        * @b{(input)}  element to copy into array
* @param    array_idx_t * @b{(input)}  index for placement
*
* @returns  L7_FALSE if index value is out of bounds,
*           element pointer is null, or array is full.
* @returns  L7_TRUE if item was successfully inserted.
*
* @notes    Existing elements will be moved "one space forward".
*           Specifying the array size as the index will effectively
*           append an array item (though arrayAppend() is a better,
*           faster choice).
*
* @notes    array_t pointer is NOT CHECKED for null value!
*           Caller must do so before invoking this function.
*
* @end
*********************************************************************/

L7_BOOL 
arrayInsert(array_t *pA, void *pElt, array_idx_t before);

/*********************************************************************
* @purpose  Delete an item from an array at the given index
*
* @param    array_t     * @b{(output)} array to change
* @param    array_idx_t   @b{(input)}  index of victim 
*
* @returns  L7_FALSE if index is faulty
* @returns  L7_TRUE if item succesfully deleted
*
* @notes    none
* @notes    array_t pointer is NOT CHECKED for null value!
*           Caller must do so before invoking this function.
*
* @end
*********************************************************************/

L7_BOOL 
arrayDelete(array_t *pA, array_idx_t at);

/*********************************************************************
* @purpose  Delete all items for which a predicate is L7_TRUE
*
* @param    array_t     * @b{(output)}    array to change
* @param    array_pred    @b{(input)}     predicate function
* @param    void        * @b{(in/output)} additional optional data
*
* @returns  none
*
* @notes    none
* @notes    array_t pointer is NOT CHECKED for null value!
*           Caller must do so before invoking this function.
*
* @end
*********************************************************************/

void
arrayDeleteIf(array_t *pA, array_pred_fn pred, void * pData);

/*********************************************************************
* @purpose  To remove all elements from an array
*
* @param    array_t     * @b{(output)} array to change
*
* @returns  void
*
* @notes    array_t pointer is NOT CHECKED for null value!
*           Caller must do so before invoking this function.
*
* @end
*********************************************************************/

void
arrayPurge(array_t *pA);

/*********************************************************************
* @purpose  To find a matching item in an array
*
* @param    array_t     * @b{(input)} array to inspect
* @param    void        * @b{(input)} item to compare
* @param    array_elt_cmp @b{(input)} comparison function
*
* @returns  array_idx_t, index of first matching item
* @returns  BAD_INDEX
*
* @notes    returns BAD_INDEX if no such item present or
*           pointer to element is null.
* @notes    array_t pointer is NOT CHECKED for null value!
*           Caller must do so before invoking this function.
*
* @end
*********************************************************************/
array_idx_t  
arrayFind(array_t *pA, void *pElt, array_elt_cmp cmp);

/*********************************************************************
* @purpose  To retrieve (index) a pointer to an array item 
*
* @param    array_t     * @b{(input)}  array to inspect
* @param    array_idx_t   @b{(input)}  index to inspect
* @param    void *        @b{(output)} pointer for element copy
*
* @returns  non-null void pointer if index is within bounds
* @returns  null void pointer if index is not within bounds
*
* @notes    Final pointer parameter can be null; copy will simply
*           not be attempted if so.
*
* @notes    array_t pointer is NOT CHECKED for null value!
*           Caller must do so before invoking this function.
*
* @end
*********************************************************************/

void *
arrayAt(array_t *pA, array_idx_t idx, void * dest /* can be null */);

/*********************************************************************
* @purpose  To swap two items in an array
*
* @param    array_t     * @b{(output)} array to change
* @param    array_idx_t   @b{(input)}  index of first  item
* @param    array_idx_t   @b{(input)}  index of second item
*
* @returns  L7_TRUE if items successfully swapped
* @returns  L7_FALSE if either index is out of bounds
*
* @notes    array_t pointer is NOT CHECKED for null value!
*           Caller must do so before invoking this function.
*
* @end
*********************************************************************/

L7_BOOL 
arraySwap(array_t *pA, array_idx_t idx1, array_idx_t idx2);

/*********************************************************************
* @purpose  To insert an element into an array in order 
*
* @param    array_t      * @b{(output)} array to change
* @param    void         * @b{(input)}  element to copy into array
* @param    array_elt_cmp  @b{(input)}  comparison function
*
* @returns  L7_TRUE if successful
* @returns  L7_FALSE otherwise (e.g. array already full)
*
* @notes    If the array is NOT ordered according to the order
*           proscribed by the comparison function, there is NO
*           guarantee that this function will ever terminate!
*
* @notes    array_t pointer is NOT CHECKED for null value!
*           Caller must do so before invoking this function.
*
* @end
*********************************************************************/

L7_BOOL 
arrayOrdInsert(array_t *pA, void *pElt, array_elt_cmp cmp);

/*********************************************************************
* @purpose  To insert an element into an array in order EXCEPT if
*           a matching element already exists within the array
*
* @param    array_t      * @b{(output)} array to change
* @param    void         * @b{(input)}  element to copy into array
* @param    array_elt_cmp  @b{(input)}  comparison function
*
* @returns  L7_TRUE if successful or item already present
* @returns  L7_FALSE otherwise (e.g. array already full)
*
* @notes    If the array is NOT ordered according to the order
*           proscribed by the comparison function, there is NO
*           guarantee that this function will ever terminate!
*
* @notes    If match already exists, this function returns 
*           L7_TRUE.  It only returns L7_FALSE for a serious error. 
*           And, it's the caller's responsibility to test for 
*           uniqueness if the comparison function isn't capable 
*           of making this distinction.
*
* @notes    array_t pointer is NOT CHECKED for null value!
*           Caller must do so before invoking this function.
*
* @end
*********************************************************************/

L7_BOOL 
arrayOrdExclInsert(array_t *pA, void *pElt, array_elt_cmp cmp);

/*********************************************************************
* @purpose  To search a subset of an array for a matching element
*
* @param    array_t      * @b{(output)} array to change
* @param    void         * @b{(input)}  element to copy into array
* @param    array_elt_cmp  @b{(input)}  comparison function
* @param    startidx       @b{(input)}  starting index of subsection
* @param    length         @b{(input)}  length (in indices) of
*                                       subsection
*
* @returns  array_idx_t if matching element exists
* @returns  BAD_INDEX if element does not exist
*
* @notes    If the array is NOT ordered according to the order
*           proscribed by the comparison function, there is NO
*           guarantee that this function will ever terminate!
*
* @notes    array_t pointer is NOT CHECKED for null value!
*           Caller must do so before invoking this function.
*
* @end
*********************************************************************/
array_idx_t  /* binary search on ordered array */
arraySubSearch(array_t *pA, void *pElt, array_elt_cmp cmp,
	       array_idx_t startidx, array_idx_t length);

/*********************************************************************
* @purpose  To search an array for a matching element
*
* @param    array_t      * @b{(output)} array to change
* @param    void         * @b{(input)}  element to copy into array
* @param    array_elt_cmp  @b{(input)}  comparison function
*
* @returns  array_idx_t if matching element exists
* @returns  BAD_INDEX if element does not exist
*
* @notes    If the array is NOT ordered according to the order
*           proscribed by the comparison function, there is NO
*           guarantee that this function will ever terminate!
*
* @notes    array_t pointer is NOT CHECKED for null value!
*           Caller must do so before invoking this function.
*
* @end
*********************************************************************/

array_idx_t  /* binary search on ordered array */
arraySearch(array_t *pA, void *pElt, array_elt_cmp cmp);


/*********************************************************************
* @purpose  To search an array for the "next" element
*
* @param    array_t      * @b{(output)} array to change
* @param    void         * @b{(input)}  element to compare against
* @param    array_elt_cmp  @b{(input)}  comparison function
*
* @returns  array_idx_t if a "next" element exists
* @returns  BAD_INDEX if element does not exist
*
* @notes    If the array is NOT ordered according to the order
*           proscribed by the comparison function, there is NO
*           guarantee that this function will ever terminate!
*
* @notes    array_t pointer is NOT CHECKED for null value!
*           Caller must do so before invoking this function.
*
* @end
*********************************************************************/
array_idx_t  
arraySearchNext(array_t *pA, void *pElt, array_elt_cmp cmp);

/*********************************************************************
* @purpose  To sort a section of an array based on the order given by
*           a comparison function
*
* @param    array_t       * @b{(output)} array to change
* @param    array_elt_cmp   @b{(input)}  comparison function
* @param    array_idx_t     @b{(input)}  starting index to sort
* @param    array_idx_t     @b{(input)}  "one past" end index
*
* @returns  void
*
* @notes    Comparison function has the semantics of memcmp().
*           See array_api.h for details.
*
* @notes    This function implements Shell sort, which is
*           for most purposes the fastest generally available
*           in-place sorting algorithm that doesn't require
*           recursion. (Recursion is not all that welcome on
*           embedded platforms.)
*
* @notes    array_t pointer is NOT CHECKED for null value!
*           Caller must do so before invoking this function.
*
* @end
*********************************************************************/
void
arraySubSort(array_t * pA, array_elt_cmp cmp, 
	     array_idx_t start, array_idx_t onePastEnd);

/*********************************************************************
* @purpose  To sort an array based on the order given by
*           a comparison function
*
* @param    array_t       * @b{(output)} array to change
* @param    array_elt_cmp   @b{(input)}  comparison function
*
* @returns  void
*
* @notes    Comparison function has the semantics of memcmp().
*           See array_api.h for details.
*
* @notes    This function implements Shell sort, which is
*           for most purposes the fastest generally available
*           in-place sorting algorithm that doesn't require
*           recursion. (Recursion is not all that welcome on
*           embedded platforms.)
*
* @notes    array_t pointer is NOT CHECKED for null value!
*           Caller must do so before invoking this function.
*
* @end
*********************************************************************/

void 
arraySort(array_t *pA, array_elt_cmp cmp);

/*********************************************************************
* @purpose  To perform an action with each array element from
*           a starting index to JUST BEFORE ending index. 
*
* @param    array_t       * @b{(output)} array to inspect
* @param    array_idx_t     @b{(input)}  starting index
* @param    array_idx_t     @b{(input)}  ending index
* @param    array_elt_fn    @b{(input)}  function to execute
* @param    void          * @b{(input)}  additional function data
*                                        (may be null)
*
* @returns  void
*
* @notes    Executes the passed-in function with a pointer to
*           an element and pointer to additional function data
*           as arguments.  Note that element at the end index
*           will NOT be used; that is, for indices S (start) and
*           E (end), the function will be called for elements at
*           S, S+1, S+2, ... E-2, E-1 only. 
*
* @notes    array_t pointer is NOT CHECKED for null value!
*           Caller must do so before invoking this function.
*
* @end
*********************************************************************/

void 
arrayForEach(array_t *pA, 
             array_idx_t start, array_idx_t end,
             array_elt_fn fn,   void * pData);

/*********************************************************************
* @purpose  To perform an action on ALL array elements from
*           start to end
*
* @param    array_t       * @b{(output)} array to inspect
* @param    array_elt_fn    @b{(input)}  function to execute
* @param    void          * @b{(input)}  additional function data
*                                        (may be null)
*
* @returns  void
*
* @notes    Executes the passed-in function with a pointer to
*           an element and pointer to additional function data
*           as arguments.  
*
* @notes    array_t pointer is NOT CHECKED for null value!
*           Caller must do so before invoking this function.
*
* @end
*********************************************************************/

void 
arrayForAll(array_t *pA, array_elt_fn fn, void * pData);



/*******************************************************************
 *
 *  Declarations of Functions for Searches/Sorts of Simple Arrays
 * 
 *******************************************************************/

/*********************************************************************
* @purpose  To sort an array of L7_uint32 values
*
* @param    L7_uint32     * @b{(output)} array to sort
* @param    L7_uint32       @b{(input)}  length of array to sort
*
* @returns  void
*
* @notes    Sorts the array via Shell sort, since Quicksort uses
*           recursion. (If you must use Quicksort, consider the
*           qsort() function as defined in <stdlib.h> instead.)
*
* @notes    arr pointer is NOT CHECKED for null value!
*           Caller must do so before invoking this function.
*
* @end
*********************************************************************/

void
L7_uint32_sort(L7_uint32 * arr, L7_uint32 len);

/*********************************************************************
* @purpose  To sort an array of L7_ushort16 values
*
* @param    L7_short16    * @b{(output)} array to sort
* @param    L7_uint32       @b{(input)}  length of array to sort
*
* @returns  void
*
* @notes    Sorts the array via Shell sort, since Quicksort uses
*           recursion. (If you must use Quicksort, consider the
*           qsort() function as defined in <stdlib.h> instead.)
*
* @notes    arr pointer is NOT CHECKED for null value!
*           Caller must do so before invoking this function.
*
* @end
*********************************************************************/

void
L7_ushort16_sort(L7_ushort16 * arr, L7_uint32 len);

/*********************************************************************
* @purpose  To search an array of L7_uint32's for a value
*
* @param    L7_uint32   * @b{(input)}  array to search
* @param    L7_uint32     @b{(input)}  length of array to search
* @param    L7_uint32     @b{(input)}  element to find
* @param    L7_uint32   * @b{(output)} index of found element
*
* @returns  L7_TRUE  if element found
* @returns  L7_FALSE if element not found
*
* @notes    Chances are, you don't need this function, and would
*           be better off using bsearch() as defined in <stdlib.h>.
*           (Type "man bsearch" at the prompt of any respectable
*           Unix shell for details.)  Use this function only if
*           you find the point in the next note useful.

* @notes    If return value is L7_FALSE, the index of the "found"
*           element will equal the index where this value would
*           need to be inserted. (All values at this index and
*           greater would need to be "pushed up" one element
*           for an ordered insertion.)
*
* @notes    arr and idx pointers are NOT CHECKED for null values!
*           Caller must do so before invoking this function.
*
* @end
*********************************************************************/

L7_BOOL
L7_uint32_bsearch(L7_uint32 *arr, L7_uint32 len, 
		  L7_uint32 elt, L7_uint32 *idx);

/*********************************************************************
* @purpose  To search an array of L7_ushort16's for a value
*
* @param    L7_ushort16 * @b{(input)}  array to search
* @param    L7_uint32     @b{(input)}  length of array to search
* @param    L7_ushort16   @b{(input)}  element to find
* @param    L7_uint32   * @b{(output)} index of found element
*
* @returns  L7_TRUE  if element found
* @returns  L7_FALSE if element not found
*
* @notes    Chances are, you don't need this function, and would
*           be better off using bsearch() as defined in <stdlib.h>.
*           (Type "man bsearch" at the prompt of any respectable
*           Unix shell for details.)  Use this function only if
*           you find the point in the next note useful.

* @notes    If return value is L7_FALSE, the index of the "found"
*           element will equal the index where this value would
*           need to be inserted. (All values at this index and
*           greater would need to be "pushed up" one element
*           for an ordered insertion.)
*
* @notes    arr and idx pointers are NOT CHECKED for null values!
*           Caller must do so before invoking this function.
*
* @end
*********************************************************************/

L7_BOOL
L7_ushort16_bsearch(L7_ushort16 *arr, L7_uint32  len, 
  		    L7_ushort16  elt, L7_uint32 *idx);




#endif
