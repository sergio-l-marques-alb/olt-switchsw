/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2009
*
**********************************************************************
*
* @filename   array_api.c
*
* @purpose    A more sophisticated array type that allows searching
*             and sorting
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
#include <string.h>
#include "sysapi.h"
#include "array_api.h"


#define ARR_DATA_AT(pA, at) \
  &(((L7_uchar8 *) (pA)->buf)[ (at) * (pA)->eltSize])

#define ARR_DATA_IDX(pA, idx) \
  &(((L7_uchar8 *) (pA)->buf)[(pA)->idxs[(idx)] * (pA)->eltSize])


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
	  array_idx_t  *pIdxBuf)
{
  register array_t * pRet = L7_NULLPTR;

  do 
  {
    if (L7_NULLPTR == pA)        /* Nothing to do  */
      break;
    if (L7_NULLPTR == pDataBuf)  /* No data buffer */
      break;
    if (L7_NULLPTR == pIdxBuf)   /* No index buffer */
      break;
    pA->eltSize  = eltSize;
    pA->size     = 0;
    pA->capacity = capacity;
    pA->buf      = pDataBuf;

    pA->idxs     = pIdxBuf;
    pRet = pA;
  } while(0);
  return pRet;
}


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
arrayAppend(array_t *pA, void *pElt)
{
  register L7_BOOL ok = L7_FALSE;

  do 
  { 
    if (L7_NULLPTR == pElt) 
      break;
    if (pA->capacity == pA->size) /* full house */
      break; 
    memcpy(ARR_DATA_AT(pA, (pA)->size), pElt, pA->eltSize);
    pA->idxs[pA->size] = pA->size;
    pA->size++;
    ok = L7_TRUE;
  } while(0);
  return ok;
}


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
arrayInsert(array_t *pA, void *pElt, array_idx_t before)
{
  register L7_BOOL ok = L7_FALSE;

  do 
  { 
    /* don't bother checking pA, for speed's sake */
    if (L7_NULLPTR == pElt) 
      break;
    if (pA->capacity == pA->size)
      break; /* full house */

    if ((0 > before) || (before > pA->size))
      break;

    /* Append item to backing store */

    memcpy( ARR_DATA_AT(pA, (pA)->size), pElt, pA->eltSize);

    /* Insert index to new item into our index array */  
    memmove( &((pA->idxs)[before + 1]), &((pA->idxs)[before]),
             (pA->size - before) * sizeof(array_idx_t));

    pA->idxs[before] = pA->size;
    pA->size++;
    ok = L7_TRUE;
  } while(0);
  return ok;
}

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
arrayDeleteIf(array_t *pA, array_pred_fn pred, void * pData)
{
  array_idx_t idx = 0;

  while(idx < ARRAY_SIZE(pA))
  {
    if (L7_TRUE == pred(arrayAt(pA, idx, L7_NULLPTR), pData)) 
    {
      arrayDelete(pA, idx);
      continue;
    }
    idx++;
  }
}

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
arrayDelete(array_t *pA, array_idx_t at)
{
  register L7_BOOL ok = L7_FALSE;

  do 
  { 
    register array_idx_t real_idx;

    /* don't bother checking pA, for speed's sake */
    if (! IN_BOUNDS(pA, at))
      break;

    real_idx = pA->idxs[at];

    /* Remove item from backing store */

    memmove(ARR_DATA_AT(pA, real_idx),
	    ARR_DATA_AT(pA, real_idx + 1),
	    (pA->size - real_idx - 1) * pA->eltSize);
    
    /* Remove index, then decrement every index greater
     * than "real_idx"
     */

    memmove(&(pA->idxs[at]), &(pA->idxs[at + 1]),
	    (pA->size - at - 1) * sizeof(array_idx_t) );
    {
      register array_idx_t i = ARRAY_SIZE(pA);
      while(i > 0)
      {
        if (pA->idxs[--i] > real_idx)
	  pA->idxs[i]--;
      }
    }
    pA->size--;
    ok = L7_TRUE;
  } while(0);
  return ok;
}

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
arrayPurge(array_t *pA)
{
  /* don't bother checking pA, for speed's sake */
  pA->size = 0;  
}

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
array_idx_t  /* brute force search, from first to last */
arrayFind(array_t *pA, void *pElt, array_elt_cmp cmp)
{
  register array_idx_t ctr = BAD_INDEX;

  /* don't bother checking pA, for speed's sake */
  if (L7_NULLPTR == pElt) 
    return ctr;

  for(ctr = 0; ctr < pA->size; ctr++) 
  {
    if (0 == cmp(pElt,  ARR_DATA_IDX(pA, ctr)))
      break;
  }    
  if (ctr == pA->size)
    ctr = BAD_INDEX;
  return ctr;
}


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
arrayAt(array_t *pA, array_idx_t idx, void * dest /* can be null */)
{
  void * pV = L7_NULLPTR;

  /* don't bother checking pA, for speed's sake */
  
  if (IN_BOUNDS(pA, idx))
  {
    pV = ARR_DATA_IDX(pA, idx);
  }
  if ((L7_NULLPTR != pV) && (L7_NULLPTR != dest))
  {
    memcpy((L7_uchar8 *) dest, (L7_uchar8 *) pV, pA->eltSize);
  }
  return pV;    
}


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
arraySwap(array_t *pA, array_idx_t idx1, array_idx_t idx2)
{
  array_idx_t tmp;
  /* don't bother checking pA, for speed's sake */
 
  if (!IN_BOUNDS(pA, idx1))
    return L7_FALSE;
  if (!IN_BOUNDS(pA, idx2))
    return L7_FALSE;
  
  tmp = pA->idxs[idx1];  
  pA->idxs[idx1] = pA->idxs[idx2];
  pA->idxs[idx2] = tmp;
  return L7_TRUE;
}


/*********************************************************************
* @purpose  To perform a modified binary search on an array
*
* @param    array_t      * @b{(input)}  array to inspect
* @param    void         * @b{(input)}  element to search for
* @param    array_idx_t  * @b{(output)} index of matching element
* @param    array_elt_cmp  @b{(input)}  comparison function
* @param    startidx       @b{(input)}  starting index of subsection
* @param    length         @b{(input)}  length (in indices) of
*                                       subsection
*
* @returns  L7_FALSE if item cannot be found
* @returns  L7_TRUE  if matching item found
*
* @notes    Comparison function has the semantics of memcmp().
*           See array_api.h for details.
*
* @notes    A return code of L7_FALSE will still cause the value 
*           pointed to by the array_idx_t pointer to be changed:
*           the value will hold the index value to use for inserting
*           the element if it is to be inserted in order.
*
* @notes    array_t pointer is NOT CHECKED for null value!
*           Caller must do so before invoking this function.
*
* @end
*********************************************************************/
static
L7_BOOL
arrayBinSearch(array_t *pA, void *pElt, 
               array_idx_t* pIdx, array_elt_cmp cmp,
	       array_idx_t startidx, array_idx_t length)
{
  array_idx_t begin = startidx, mid = 0, end = (startidx + length - 1);
  L7_int32 cmp_rc;
  L7_BOOL rc = L7_FALSE;

  if (end >= ARRAY_SIZE(pA)) 
  {
    end = ARRAY_SIZE(pA) - 1;
  }

  if (begin < 0)
  {
    return rc;
  }

  while (begin <= end)
  {
    mid = (begin + end) / 2;
    cmp_rc = cmp(ARR_DATA_IDX(pA, mid), pElt);
    if (0 == cmp_rc)
    {
      rc = L7_TRUE;
      break;
    }
    else if (cmp_rc > 0) /* pElt is "bigger" */
    {
      end = mid - 1;
    } 
    else
    {
      begin = mid + 1;
    }
  }

  /* mid should now either be at the index where we would
   * we would insert a new element, or just one index less.
   */

  if (L7_FALSE == rc)
  {
    while(mid < pA->size)
    {
      if (cmp(ARR_DATA_IDX(pA, mid), pElt) > 0)
	break;
      mid++;
    }
  }
  *pIdx = mid;
  return rc;
}


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
arrayOrdInsert(array_t *pA, void *pElt, array_elt_cmp cmp)
{
  array_idx_t idx;
  /* don't bother checking pA, for speed's sake */

  if (L7_NULLPTR == pElt)
    return L7_FALSE;

  /* we're just interested in finding the right index,
   * so throw away the return code in the next line
   */

  (void) arrayBinSearch(pA, pElt, &idx, cmp, 0, ARRAY_SIZE(pA)); 
  return arrayInsert(pA, pElt, idx); 
}


/*********************************************************************
* @purpose  To insert an element into an array in order EXCEPT if
*           a matching element already exists within the array
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
* @notes    If match already exists, this function returns 
*           L7_TRUE.  It only returns L7_FALSE for a serious error. 
*           And, it's the caller's responsibility to test for 
*           uniqueness if the comparison function isn't capable 
*           of making this distinction.
*
* @end
*********************************************************************/
L7_BOOL 
arrayOrdExclInsert(array_t *pA, void *pElt, array_elt_cmp cmp)
{
  array_idx_t idx;
  /* don't bother checking pA, for speed's sake */

  if (L7_NULLPTR == pElt)
    return L7_FALSE;

  return (arrayBinSearch(pA, pElt, &idx, cmp, 0, ARRAY_SIZE(pA)) ||
          arrayInsert(pA, pElt, idx)); 
}


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
	       array_idx_t startidx, array_idx_t length)
{
  array_idx_t idx;
  /* don't bother checking pA, for speed's sake */

  if (L7_NULLPTR == pElt)
    return L7_FALSE;

  if (L7_FALSE == arrayBinSearch(pA, pElt, &idx, cmp, startidx, length))
    return BAD_INDEX;
  return idx;
}

/*********************************************************************
* @purpose  To search an array for a matching element
*
* @param    array_t      * @b{(output)} array to change
* @param    void         * @b{(input)}  element to search for
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
arraySearch(array_t *pA, void *pElt, array_elt_cmp cmp)
{
  array_idx_t idx;

  /* don't bother checking pA, for speed's sake */

  if (L7_NULLPTR == pElt)
    return L7_FALSE;

  if (L7_FALSE == arrayBinSearch(pA, pElt, &idx, cmp, 0, ARRAY_SIZE(pA)))
    return BAD_INDEX;
  return idx;
}


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
arraySearchNext(array_t *pA, void *pElt, array_elt_cmp cmp)
{
  array_idx_t idx;
  L7_BOOL elt_exists;

  if ((L7_NULLPTR == pElt) ||  ARRAY_EMPTY(pA))
      return BAD_INDEX;

  elt_exists = arrayBinSearch(pA, pElt, &idx, cmp, 0, ARRAY_SIZE(pA));

  if (L7_TRUE == elt_exists)
  {
      idx++;
  }

  if (idx >= ARRAY_SIZE(pA))
  {
      idx = BAD_INDEX;
  }
  return idx;
}




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
	     array_idx_t start, array_idx_t onePastEnd)
{
  array_idx_t i, j, step, idx, size;

  if ((start >= onePastEnd) || (start < 0) || (onePastEnd > pA->size))
    return;

  size = onePastEnd - start;

  step = (size / 2) + (size % 2);
  if (0 == step)
    return;

  while(1)
  {
    for(i = step + start; i < size + start; i++)
    {
      idx = pA->idxs[i];
      j = i;
      while((j >= (step + start)) && 
            (0 < cmp(ARR_DATA_IDX(pA, j - step),
                     ARR_DATA_AT(pA, idx)))) 
      {
	pA->idxs[j] = pA->idxs[j - step];
	j -= step;
      }
      pA->idxs[j] = idx;
    }

    if (1 == step)  /* Done */
      break;
    step = (step / 2); /* Halve step value, but..       */
    if (0 == step % 2) /* ..make sure new value is odd. */
      step++;
  }
}


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
arraySort(array_t * pA, array_elt_cmp cmp)
{
  arraySubSort(pA, cmp, 0, pA->size);
}


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
             array_elt_fn fn,   void * pData)
{
  if (start >= end)          return;
  if (! IN_BOUNDS(pA,start)) return;
  if (end > pA->size)
    end = pA->size;

  while( start < end) 
  {
    (*fn)(ARR_DATA_IDX(pA, start), pData);
    start++;
  }
}

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
arrayForAll(array_t *pA, array_elt_fn fn, void * pData)
{
  array_idx_t ctr; 
  for(ctr = 0; ctr < pA->size; ctr++)
  {
    (*fn)(ARR_DATA_IDX(pA, ctr), pData);
  }
}


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
L7_uint32_sort(L7_uint32 * arr, L7_uint32 len)
{
  L7_uint32 i, j, step;
  register L7_uint32 val;

  step = (len / 2) + (len % 2);
  if (0 == step)
    return;

  while(1)
  {
    for(i = step; i < len; i++)
    {
      val = arr[i];
      j = i;
      while((j >= step) && (arr[j - step] > arr[i]))
      {
	arr[j] = arr[j - step];
	j -= step;
      }
      arr[j] = val;
    }

    if (1 == step)  /* Done */
      break;
    step = (step / 2); /* Halve step value, but..       */
    if (0 == step % 2) /* ..make sure new value is odd. */
      step++;
  }
}


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
L7_ushort16_sort(L7_ushort16 * arr, L7_uint32 len)
{
  L7_uint32 i, j, step;
  register L7_ushort16 val;

  step = (len / 2) + (len % 2);
  if (0 == step)
    return;

  while(1)
  {
    for(i = step; i < len; i++)
    {
      val = arr[i];
      j = i;
      while((j >= step) && (arr[j - step] > arr[i]))
      {
	arr[j] = arr[j - step];
	j -= step;
      }
      arr[j] = val;
    }

    if (1 == step)  /* Done */
      break;
    step = (step / 2); /* Halve step value, but..       */
    if (0 == step % 2) /* ..make sure new value is odd. */
      step++;
  }
}


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
		  L7_uint32 elt, L7_uint32 *idx)
{
  L7_uint32 begin = 0, mid = 0, end = len;
  L7_BOOL rc = L7_FALSE;

  while (end >= begin)
  {
    mid = (begin + end) / 2;
    if (elt == arr[mid])
    {
      rc = L7_TRUE;
      break;
    }
    if (elt < arr[mid]) 
    {
      end = mid - 1;
      continue;
    } 
    begin = mid + 1;
  }

  /* mid should now either be at the index where we would
   * we would insert a new element, or just one index less.
   */

  if (L7_FALSE == rc)
  {
    while(mid < len)
    {
      if (arr[mid] > elt)
	break;
      mid++;
    }
  }
  *idx = mid;
  return rc;
}


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
  		    L7_ushort16  elt, L7_uint32 *idx)
{
  L7_uint32 begin = 0, mid = 0, end = len;
  L7_BOOL rc = L7_FALSE;

  while (end >= begin)
  {
    mid = (begin + end) / 2;
    if (elt == arr[mid])
    {
      rc = L7_TRUE;
      break;
    }
    if (elt < arr[mid]) 
    {
      end = mid - 1;
      continue;
    } 
    begin = mid + 1;
  }

  /* mid should now either be at the index where we would
   * we would insert a new element, or just one index less.
   */

  if (L7_FALSE == rc)
  {
    while(mid < len)
    {
      if (arr[mid] > elt)
	break;
      mid++;
    }
  }
  *idx = mid;
  return rc;
}
