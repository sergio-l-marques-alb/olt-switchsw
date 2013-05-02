/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename   heap_api.h
 *
 * @purpose    Memory Heap Storage
 *
 * @component  util
 *
 * @comments   none
 *
 * @create     02/02/2007
 *
 * @author     PKB
 * @end
 *
 **********************************************************************/
#ifndef HEAP_API_INCLUDE_H
#define HEAP_API_INCLUDE_H

#include <stdio.h>

#define HEAP_MAX_BUFFPOOLS     30     /* Maximum number of pools in the heap */
#define HEAP_POOLNAME_SIZE     64     /* Maximum size for the buffer pool name */
#define HEAP_FILENAME_SIZE     32     /* Maximum size for the Filename strings */

typedef L7_uint32 heapFlags_t;

/* Defines for heap flags (heapFlags_t) */
#define  HEAP_FLAGS_NONE            0x0000
#define  HEAP_DEBUG_FILE_INFO       0x0001
#define  HEAP_DEBUG_MEMORY_GUARD    0x0002
#define  HEAP_LOCK_GUARD            0x0004
#define  HEAP_MALLOC_ENABLE         0x0008
#define  HEAP_FLAGS_MAX             0x0010 /* need to be last always */

/* Defines for buffer pool flags */
#define HEAP_BUFFPOOL_OVERFLOW      0x0000 /* Higher sized pools may be used to honor requests */
#define HEAP_BUFFPOOL_NO_OVERFLOW   0x0001 /* Higher sized pools may not be used to honor requests */

typedef struct heapBuffPool_s
{
  L7_uint32  buffSize;
  L7_uint32  buffCount;
  L7_uchar8  name[HEAP_POOLNAME_SIZE];
  L7_uint32  flags;
}heapBuffPool_t;

/*********************************************************************
*
* @purpose  Create a Heap
*
* @param    compId @b{(input)} Component to which the heap belongs
* @param    size   @b{(input)} Size of the heap
*
* @returns  Valid heap id on success.
* @returns  L7_NULL on failure.
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 heapCreate(L7_COMPONENT_IDS_t compId,
                     size_t             size);

/*********************************************************************
*
* @purpose  Initialize a Heap
*
* @param    heapId @b{(input)} Heap ID
* @param    bList  @b{(input)} List of buffer pool defintions
* @param    flags  @b{(input)} Flags for the heap
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t heapInit(L7_uint32      heapId,
                 heapBuffPool_t bList[],
                 heapFlags_t    flags);

/*********************************************************************
*
* @purpose  De-initialize a Heap
*
* @param    heapId  @b{(input)} Heap ID
* @param    bForced @b{(input)} L7_TRUE/L7_FALSE - de-Init even if outstanding allocations
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t heapDeInit(L7_uint32 heapId,
				   L7_BOOL   bForced);

/*********************************************************************
*
* @purpose  Destroy a Heap
*
* @param    heapId  @b{(input)} Heap ID
* @param    bForced @b{(input)} L7_TRUE/L7_FALSE - destroy even if outstanding allocations
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t heapDestroy(L7_uint32 heapId,
					L7_BOOL   bForced);

/*********************************************************************
*
* @purpose  Allocation from the heap
*
* @param    heapId     @b{(input)} Heap ID
* @param    size       @b{(input)} Size being requested
* @param    fileName   @b{(input)} Name of the file invoking this function.
* @param    lineNumber @b{(input)} Line number in the invoking file.
*
* @returns  pointer to the allocated location
* @returns  L7_NULLPTR on failure
*
* @notes
*
* @end
*********************************************************************/
void *heapAlloc(L7_uint32 heapId,
                size_t    size,
                L7_uchar8 *fileName,
                L7_uint32 lineNumber);

/*********************************************************************
*
* @purpose  Free to the heap
*
* @param    heapId  @b{(input)} Heap ID
* @param    pMem    @b{(input)} Pointer to the buffer
*
* @returns  None
*
* @notes
*
* @end
*********************************************************************/
void heapFree(L7_uint32 heapId,
              void      *pMem);

/*********************************************************************
*
* @purpose  Check if a buffer is valid in a heap context
*
* @param    heapId  @b{(input)} Heap ID
* @param    pMem    @b{(input)} Pointer to the buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t heapBuffValidate(L7_uint32 heapId,
                         void      *pMem);

/*********************************************************************
*
* @purpose  To display the basic statistics for a heap
*
* @param    heapId  @b((input)) Heap ID
*
* @returns  None
*
* @notes    memShow equivalent data output
*
* @end
*********************************************************************/
void  heapShow(L7_uint32 heapId);

/*********************************************************************
*
* @purpose  Enable/disable debug prints for a heap
*
* @param    heapId   @b{(input)} Heap ID
* @param    dbgPrint @b{(input)} Enable/Disable debug Print (0 - disable, 1 - enable)
*
* @returns  None
*
* @notes
*
* @end
*********************************************************************/
void heapDbgPrintSet(L7_uint32 heapId,
                     L7_uint32 dbgPrint);

/*********************************************************************
*
* @purpose  To display the status for a heap
*
* @param    heapId  @b((input)) Heap ID
*
* @returns  None
*
* @notes
*
* @end
*********************************************************************/
void heapDbgStatsDisplay(L7_uint32 heapId);

/*********************************************************************
*
* @purpose  Display available information on a heap allocated buffer
*
* @param    heapId  @b{(input)} Heap ID
* @param    pMem    @b{(input)} Pointer to the buffer
*
* @returns  None
*
* @notes
*
* @end
*********************************************************************/
void heapDbgBuffInfo(L7_uint32 heapId,
                     void      *pMem);

#endif /* HEAP_API_INCLUDE_H */


