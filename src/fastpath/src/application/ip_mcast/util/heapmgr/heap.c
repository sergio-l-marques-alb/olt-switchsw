/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename   heap.c
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
#include <stdio.h>
#include <string.h>
#include "osapi.h"
#include "sysapi.h"
#include "osapi_support.h"
#include "l7utils_api.h"
#include "heap_api.h"
#include "heap.h"

/*********************************************************************
*
* @purpose  Compute the memory aligned size for the given size
*
* @param    size  @b{(input)} Input for alignment
*
* @returns  Aligned size
*
* @notes
*
* @end
*********************************************************************/
static
L7_uint32 heapMemorySizeAlign(L7_uint32 size)
{
  size += (size % HEAP_BYTE_ALIGNMENT == 0) ?
          0 :
          (HEAP_BYTE_ALIGNMENT - size % HEAP_BYTE_ALIGNMENT);
  return size;
}

/*********************************************************************
*
* @purpose
*
* @param    heapId  @b{(input)} Heap ID
* @param
*
* @returns  None
*
* @notes
*
* @end
*********************************************************************/
static L7_BOOL heapIsInUse(heap_t  *heap)
{
  L7_BOOL      inUse = L7_FALSE;
  if(osapiSemaTake(heap->heapSemId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    HEAP_TRACE(HEAP_PRINT_ALWAYS,
               "[%s:%u] ERROR: Semaphore Take failed for heapId - %x\n",
               __FUNCTION__,__LINE__,heap);
    return inUse;
  }
  if(heap->inUse != 0)
  {
    inUse = L7_TRUE;
  }
  osapiSemaGive(heap->heapSemId);
  return inUse;
}

/*********************************************************************
*
* @purpose
*
* @param    heapId  @b{(input)} Heap ID
* @param
*
* @returns  None
*
* @notes
*
* @end
*********************************************************************/
static void heapSizeUpdate(heap_t  *heap, L7_uint32 size)
{
  if(osapiSemaTake(heap->heapSemId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    HEAP_TRACE(HEAP_PRINT_ALWAYS,
               "[%s:%u] ERROR: Semaphore Take failed for heapId - %x\n",
               __FUNCTION__,__LINE__,heap);
    return;
  }
  heap->inUse = size;
  osapiSemaGive(heap->heapSemId);
  return;
}
/*********************************************************************
*
* @purpose  Compute the memory requirements for the heap
*
* @param    bList         @b{(input)} List of buffer pool defintions
* @param    flags         @b{(input)} Flags for the heap
* @param    heapSize      @b{(output)} Pointer to return the total size of the heap
* @param    buffPoolCount @b{(output)} Pointer to return the number of buffer pools
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The function also attempts to validate the given parameters
*           as much as possible while computing the size
*
* @end
*********************************************************************/
static
L7_RC_t heapBuffPoolSizeCompute(heapBuffPool_t bList[],
                                heapFlags_t flags,
                                L7_uint32   *heapSize,
                                L7_uint32   *buffPoolCount)
{
  L7_uint32       buffSize;
  heapBuffPool_t  *tmpBuffPool = (heapBuffPool_t*) bList;

  /* Basic validations */
  if(heapSize == L7_NULLPTR)
  {
    HEAP_TRACE(HEAP_PRINT_ALWAYS,"\n[%s:%u]Error: heapSize is NULL\n",
               __FUNCTION__,__LINE__);
    return L7_FAILURE;
  }
  if(buffPoolCount == L7_NULLPTR)
  {
    HEAP_TRACE(HEAP_PRINT_ALWAYS,"\n[%s:%u]Error: buffPoolCount is NULL\n",
               __FUNCTION__,__LINE__);
    return L7_FAILURE;
  }
    /* validate the debug Flag */
  if(flags < HEAP_FLAGS_NONE || flags > HEAP_FLAGS_MAX)
  {
    HEAP_TRACE(HEAP_PRINT_ALWAYS,"\n[%s:%u]Error: Invalid Flags - 0x%x\n",
               __FUNCTION__,__LINE__,flags);
    return L7_FAILURE;
  }

  *heapSize = 0;
  *buffPoolCount = 0;
  if(tmpBuffPool == L7_NULLPTR)
  {
    HEAP_TRACE(HEAP_PRINT_ALWAYS,"\n[%s:%u]Error: Buffer Pool List is NULL\n",
               __FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  /* Cycle through all the buffer pool definitions and compute the
     total buffer area size */
  while(tmpBuffPool != L7_NULLPTR && tmpBuffPool->buffSize != 0)
  {
    buffSize = heapMemorySizeAlign(tmpBuffPool->buffSize);
    if((flags & HEAP_DEBUG_MEMORY_GUARD) != 0)
    {
      buffSize += 2 * sizeof(heapDebugMemGuard_t);
    }
    if((flags & HEAP_DEBUG_FILE_INFO) != 0)
    {
      buffSize += sizeof(heapDebugInfo_t);
    }
    *heapSize += (buffSize * (tmpBuffPool->buffCount));
    tmpBuffPool++;
    (*buffPoolCount)++;
  }

  /* Ensure we are well within the num buffer pool limit */
  if(*buffPoolCount > HEAP_MAX_BUFFPOOLS)
  {
    *heapSize = 0;
    *buffPoolCount = 0;
    HEAP_TRACE(HEAP_PRINT_ALWAYS,"\n[%s:%u]Error: Number of pools requested - %u, HEAP_MAX - %u\n",
               __FUNCTION__,__LINE__,*buffPoolCount, HEAP_MAX_BUFFPOOLS);
    return L7_FAILURE;
  }

  /* Add the size requirement for the pool descriptors */
  *heapSize += (*buffPoolCount) * sizeof(buffPool_t);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Initialize the heap
*
* @param    heap          @b{(input)} Heap to initialize
* @param    bList         @b{(input)} List of buffer pool defintions
* @param    flags         @b{(input)} Flags for the heap
* @param    heapSize      @b{(input)} Total size of the heap
* @param    buffPoolCount @b{(input)} Number of buffer pools
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    1. Checks for duplicate buffer pool sizes performed here
*           2. Freelists are setup here
*
* @end
*********************************************************************/
static
L7_RC_t heapInitialize(heap_t         *heap,
                       heapBuffPool_t bList[],
                       heapFlags_t    flags,
                       L7_uint32      heapSize,
                       L7_uint32      buffPoolCount)
{
  heapBuffPool_t  *tmpBuffPool= (heapBuffPool_t*)bList;
  buffPool_t      *heapBuffPool, *freeBuffPool, *buffPool;
  L7_uint32       dataPtr, buffOverhead, i, j, buffSize;

  /* Simple validations */
  if((heap == L7_NULLPTR) || (tmpBuffPool == L7_NULLPTR) ||
     (heapSize == 0) || (buffPoolCount == 0))
  {
    HEAP_TRACE(HEAP_PRINT_ALWAYS,"\n[%s:%u]Error: Invalid Params - heap %x, bList %x, heapSize %u, buffPoolCount %u\n",
               __FUNCTION__,__LINE__,heap,tmpBuffPool,heapSize,buffPoolCount);
    return L7_FAILURE;
  }

  /* Set-up the heap structure */
  memset((void *)((L7_uint32)heap + sizeof(heap_t)), 0, heapSize);
  heap->buffPool = (buffPool_t *)((L7_uint32)heap + sizeof(heap_t));
  heap->buffPoolCount = buffPoolCount;
  heap->flags = flags;
  heap->dbgPrint = 0;  /* Turn-off debug prints by default */
  if((flags & HEAP_DEBUG_FILE_INFO) != 0)
  {
    heap->headerSize += sizeof(heapDebugInfo_t);
  }
  if((flags & HEAP_DEBUG_MEMORY_GUARD) != 0)
  {
    heap->headerSize += sizeof(heapDebugMemGuard_t);
  }

  /* Setup the Buffer Pools */
  freeBuffPool = heap->buffPool;
  for(i = 0; i < buffPoolCount; i++, tmpBuffPool++)
  {
    if(tmpBuffPool == L7_NULLPTR || tmpBuffPool->buffSize == 0)
    {
      /* Should never happen */
      HEAP_TRACE(HEAP_PRINT_ALWAYS,"\n[%s:%u]Error: Invalid heap storage. tmpBuffPool - 0x%x, buffSize - %u\n",
                 __FUNCTION__,__LINE__,tmpBuffPool,tmpBuffPool->buffSize);
      return L7_FAILURE;
    }

    /* Look for an insertion point - in sorted order */
    buffSize = heapMemorySizeAlign(tmpBuffPool->buffSize);
    for(heapBuffPool = heap->buffPool;
         heapBuffPool->buffSize != 0;
         heapBuffPool++)
    {
      if(buffSize < heapBuffPool->buffSize)
      {
        buffPool_t *buffPool;

        /* Found an insertion point; move all higher sizes one entry up */
        for(buffPool = freeBuffPool; buffPool != heapBuffPool; buffPool--)
        {
          if(buffPool < heapBuffPool)
          {
            /* Something terribly wrong with the logic here! */
            return L7_FAILURE;
          }
          memcpy(buffPool, buffPool - 1, sizeof(buffPool_t));
        }

        /* Populate the entry at insertion point (Insert!) */
        heapBuffPool->buffSize = buffSize;
        heapBuffPool->buffCount = tmpBuffPool->buffCount;
        osapiStrncpy(heapBuffPool->descr, tmpBuffPool->name,
                     sizeof(heapBuffPool->descr));
        heapBuffPool->flags = tmpBuffPool->flags;
        break;
      }
      else if(buffSize == heapBuffPool->buffSize)
      {
        /* Duplicate buffer pool size, reject the heap create */
        HEAP_TRACE(HEAP_PRINT_ALWAYS,"\n[%s:%u]Error: Duplicate Sized Buffers - Pools are %s & %s, size - %u\n",
                   __FUNCTION__,__LINE__,heapBuffPool->descr,tmpBuffPool->name,buffSize);
        return L7_FAILURE;
      }
    }

    /* No insertion point found: add at end */
    if(heapBuffPool->buffSize == 0)
    {
      freeBuffPool->buffSize = buffSize;
      freeBuffPool->buffCount = tmpBuffPool->buffCount;
      osapiStrncpy(freeBuffPool->descr, tmpBuffPool->name,
                   sizeof(freeBuffPool->descr));
      freeBuffPool->flags = tmpBuffPool->flags;
    }
    freeBuffPool++;
  }

  /* Populate the free list and the start & end locs */
  dataPtr = (L7_uint32)(heap->buffPool) + buffPoolCount * sizeof(buffPool_t);
  buffOverhead = heap->headerSize +
                 (((flags & HEAP_DEBUG_MEMORY_GUARD) != 0) ?
                  sizeof(heapDebugMemGuard_t) : 0);
  for(i = 0, buffPool = heap->buffPool; i < buffPoolCount; i++, buffPool++)
  {
    buffPool->startLoc = dataPtr;
    buffPool->endLoc = dataPtr +
      (buffPool->buffSize + buffOverhead) * buffPool->buffCount - 1;
    for(j = 0; j < buffPool->buffCount; j++)
    {
      *(L7_uint32 *)(dataPtr + heap->headerSize) = (L7_uint32)(buffPool->freeList);
      buffPool->freeList = (void *)(dataPtr + heap->headerSize);
      dataPtr += buffPool->buffSize + buffOverhead;
    }
  }
  /* All OK; set the heap to be in use */
  heapSizeUpdate(heap, heapSize);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Retrieve the buffer pool structure for which the given buffer is part of
*
* @param    heap       @b{(input)} Pointer to the heap structure
* @param    pMem       @b{(input)} Pointer to the buffer in question
* @param    bufferPool @b{(output)} Pointer to the buffer pool structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static
L7_RC_t heapBuffPoolGet(heap_t     *heap,
                        void       *pMem,
                        buffPool_t **buffPool)
{
  L7_uint32 pBuff, i;

  /* Get the pointer to the whole buffer (including the debug header info) */
  pBuff =  (L7_uint32)pMem - heap->headerSize;

  /* Iterate through the various pools and see which pool does this buffer belong */
  *buffPool = heap->buffPool;
  for(i = 0; (i < heap->buffPoolCount) && (*buffPool != L7_NULLPTR);
     i++, (*buffPool)++)
  {
    if(pBuff < (*buffPool)->startLoc)
    {
      /* Invalid buffer pointer. Does not belong to this memory heap */
      HEAP_TRACE(heap->dbgPrint,"[%s:%u] Buffer 0x%x does not belong to heap 0x%x\n",
                 __FUNCTION__,__LINE__,pMem,heap);
      *buffPool = L7_NULLPTR;
      return L7_FAILURE;
    }
    if(pBuff <= (*buffPool)->endLoc)
    {
      L7_uint32 wholeBuffSize;

      /* Could belong to this pool */
      wholeBuffSize = (*buffPool)->buffSize + heap->headerSize +
        ((heap->flags & HEAP_DEBUG_MEMORY_GUARD) != 0) ?
        sizeof(heapDebugMemGuard_t) : 0;
      if((pBuff - (*buffPool)->startLoc) % wholeBuffSize != 0)
      {
        /* Given Buffer pointer is not aligned to a valid buffer boundary */
        HEAP_TRACE(heap->dbgPrint,"[%s:%u] Buffer 0x%x does not align to a valid buffer boundary in heap 0x%x\n",
                   __FUNCTION__,__LINE__,pMem,heap);
        return L7_FAILURE;
      }

      /* Valid buffer */
      return L7_SUCCESS;
    }
  }
  /* Not within the address range of any pools we know of */
  HEAP_TRACE(heap->dbgPrint,"[%s:%u] Buffer 0x%x does not lie within heap 0x%x\n",
             __FUNCTION__,__LINE__,pMem,heap);
  *buffPool = L7_NULLPTR;
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Create a Heap
*
* @param    compId @b{(input)} Component to which the heap belongs
* @param    size   @b{(input)} Size of the heap
*
* @returns  Valid heap ID on success.
* @returns  L7_NULL on failure.
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 heapCreate(L7_COMPONENT_IDS_t compId,
                     size_t             size)
{
  L7_uint32 heapSize, usrSize;
  heap_t    *heap = L7_NULL;

  /* Basic sanity checks */
  if(size <= 0)
  {
    return L7_NULL;
  }

  /* Compute the size and allocate */
  usrSize = heapMemorySizeAlign(size);
  heapSize = sizeof(heap_t) + usrSize;
  heap = (heap_t *)osapiMalloc(compId, heapSize);
  if(heap == L7_NULLPTR)
  {
    HEAP_TRACE(HEAP_PRINT_ALWAYS,
               "[%s:%u] ERROR: Insufficient Memory for heap; requested size = %d bytes \n",
               __FUNCTION__,__LINE__,heapSize);
    return L7_NULL;
  }

  /* Initialize the heap structure and return the heapID */
  memset(heap, 0, sizeof(heap_t));
  heap->validateMem = (L7_uint32)heap;
  heap->compId = compId;
  heap->heapSize = usrSize;
  heap->inUse = 0;
  heap->heapSemId = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if(heap->heapSemId == L7_NULLPTR)
  {
    HEAP_TRACE(HEAP_PRINT_ALWAYS,"\n[%s:%u]Error: Heap semaphore creation failed\n",
               __FUNCTION__,__LINE__);
    return L7_NULL;
  }

  heap->semId = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if(heap->semId == L7_NULLPTR)
  {
    HEAP_TRACE(HEAP_PRINT_ALWAYS,"\n[%s:%u]Error: Semaphore creation failed\n",
               __FUNCTION__,__LINE__);
    return L7_FAILURE;
  }
  return (L7_uint32)heap;
}

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
                 heapFlags_t    flags)
{
  heap_t    *heap = (heap_t*)heapId;
  L7_uint32 heapSize, buffPoolCount;

  /* Basic sanity checks */
  if(heap == L7_NULLPTR || (*((L7_uint32*)heapId) != heapId))
  {
    HEAP_TRACE(HEAP_PRINT_ALWAYS,
               "[%s:%u] ERROR: Heap Initialization failed for heapId - %x\n",
               __FUNCTION__,__LINE__,heapId);
    return L7_FAILURE;
  }
  if(heapIsInUse(heap) == L7_TRUE)
  {
    /* While changing one MRP to other MRP, very fast
       we dont want this message on the console
     */
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_MCAST_MAP_COMPONENT_ID,
            "[%s:%u] Heap with ID 0x%x is already in use\n",
               __FUNCTION__,__LINE__,heapId);

    return L7_FAILURE;
  }

  /* Validate the params & Compute the heap size */
  if(heapBuffPoolSizeCompute(bList, flags, &heapSize, &buffPoolCount) != L7_SUCCESS)
  {
    HEAP_TRACE(heap->dbgPrint,"[%s:%u] Invalid Params to computing size of heap\n",
               __FUNCTION__,__LINE__);
    return L7_FAILURE;
  }
  if(heapSize > heap->heapSize)
  {
    HEAP_TRACE(HEAP_PRINT_ALWAYS, "[%s:%u] ERROR: Insufficient memory for heap init; Avail %d; Ask %d\n",
               __FUNCTION__,__LINE__,heap->heapSize, heapSize);
    return L7_FAILURE;
  }
  /* Initialize the heap */
  return heapInitialize(heap, bList, flags, heapSize, buffPoolCount);
}

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
                   L7_BOOL   bForced)
{
  heap_t     *heap = (heap_t*)heapId;
  L7_uint32  i;
  buffPool_t *buffPool;

  /* Validate the heap */
  if(heap == L7_NULLPTR || (*((L7_uint32*)heapId) != heapId))
  {
    HEAP_TRACE(HEAP_PRINT_ALWAYS,
               "[%s:%u] ERROR: Invalid Heap ID %x. De-Init failed\n",
               __FUNCTION__,__LINE__,heapId);
    return L7_FAILURE;
  }
  if(heapIsInUse(heap) == L7_FALSE)
  {
    HEAP_TRACE(HEAP_PRINT_ALWAYS,
               "[%s:%u] ERROR: Heap %x is not inited yet; De-Init failed.\n",
               __FUNCTION__,__LINE__,heapId);
    return L7_FAILURE;
  }
  /* Lock Heap accesses, if needed */
  if((heap->flags & HEAP_LOCK_GUARD) != 0)
  {
    if(osapiSemaTake(heap->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      HEAP_TRACE(HEAP_PRINT_ALWAYS,
                 "[%s:%u] ERROR: Semaphore acquisition failed for heap 0x%x.\n",
                 __FUNCTION__,__LINE__,heapId);
      return L7_FAILURE;
    }
  }

  /* Check if any outstanding allocations */
  if(bForced != L7_TRUE)
  {
    for(i = 0, buffPool = heap->buffPool; i < heap->buffPoolCount; i++, buffPool++)
    {
      if(buffPool->stats.allocCount != 0)
      {
        /* Outstanding allocated buffers */
        HEAP_TRACE(HEAP_PRINT_ALWAYS,
                   "[%s:%u] Outstanding buffers (%u) allocated in buffpool %s, heap 0x%x. De-Init failed.\n",
                   __FUNCTION__,__LINE__,buffPool->stats.allocCount,buffPool->descr,heapId);
        /* Unlock access */
        if((heap->flags & HEAP_LOCK_GUARD) != 0)
        {
          osapiSemaGive(heap->semId);
        }
        return L7_FAILURE;
      }
    }
  }

  /* Unlock access */
  if((heap->flags & HEAP_LOCK_GUARD) != 0)
  {
    osapiSemaGive(heap->semId);
  }

  /* Invalidate the heap and then free-up the resources */
  heapSizeUpdate(heap, 0);

  return L7_SUCCESS;
}

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
                    L7_BOOL   bForced)
{
  heap_t     *heap = (heap_t*)heapId;

  /* Validate the heap */
  if(heap == L7_NULLPTR || (*((L7_uint32*)heapId) != heapId))
  {
    HEAP_TRACE(HEAP_PRINT_ALWAYS,
               "FAILED to destroy the heap with ID - %x. Invalid heapID\n",
               heapId);
    return L7_FAILURE;
  }
  /* Check if any outstanding allocations */
  if((bForced != L7_TRUE) && heapIsInUse(heap) == L7_TRUE)
  {
    /* Still in use */
    HEAP_TRACE(HEAP_PRINT_ALWAYS,
               "FAILED to destroy the heap with ID - %x. Heap in use\n",
               heapId);
    return L7_FAILURE;
  }

  /* Invalidate the heap and then free-up the resources */
  heap->validateMem = (L7_uint32)L7_NULL;
  heapSizeUpdate(heap, 0);

  osapiSemaDelete(heap->heapSemId);
  heap->semId = L7_NULLPTR;

  osapiSemaDelete(heap->semId);
  heap->semId = L7_NULLPTR;

  osapiFree(heap->compId, heap);

  return L7_SUCCESS;
}

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
                L7_uint32 lineNumber)
{
  heap_t     *heap = (heap_t*)heapId;
  buffPool_t *buffPool;
  void       *output = L7_NULLPTR;
  L7_uint32  i;

  l7utilsFilenameStrip((L7_char8 **)&fileName);

  /* Basic sanity checks */
  if((heap == L7_NULL) || (*((L7_uint32*)heapId) != heapId) || (size == 0))
  {
    HEAP_TRACE(HEAP_PRINT_ALWAYS,"Failed to allocate memory -Invalid Params\n");
    return L7_NULLPTR;
  }
  if(heapIsInUse(heap) == L7_FALSE)
  {
    /* Un-initialized heap access */
    HEAP_TRACE(HEAP_PRINT_ALWAYS,"Failed to allocate memory - Uninitialized heap\n");
    return L7_NULLPTR;
  }

  /* Lock the access, if needed */
  if((heap->flags & HEAP_LOCK_GUARD) != 0)
  {
    if(osapiSemaTake(heap->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      return L7_NULLPTR;
    }
  }

  /* Iterate through the various pools and identify a free buffer as necessary */
  buffPool = heap->buffPool;
  for(i = 0; (i < heap->buffPoolCount) && (buffPool != L7_NULLPTR);
     i++, buffPool++)
  {
    /* Ignore any pools with smaller sized buffers */
    if(buffPool->buffSize < size)
    {
      continue;
    }
    /* Check if there is any free buffer for use */
    if(buffPool->freeList != L7_NULLPTR)
    {
      void *wholeBuff;

      /* Extract the free buffer from the freeList */
      output = (void *)buffPool->freeList;
      buffPool->freeList = (void *)(*(L7_uint32 *)output);
      *(L7_uint32 *)output = 0;

      /* Setup the debug information */
      wholeBuff = (void *)((L7_uint32)(output) - heap->headerSize);
      if((heap->flags & HEAP_DEBUG_FILE_INFO) != 0)
      {
        heapDebugInfo_t *dbgInfo = (heapDebugInfo_t *)wholeBuff;
        osapiStrncpy(dbgInfo->fileName, fileName, sizeof(dbgInfo->fileName));
        dbgInfo->lineNumber = lineNumber;
        if(osapiTaskIDSelfGet(&(dbgInfo->taskId)) != L7_SUCCESS)
        {
          dbgInfo->taskId = 0;
        }
        dbgInfo->sizeRequest = size;
        wholeBuff = (void *)((L7_uint32)(wholeBuff) + sizeof(heapDebugInfo_t));
      }
      if((heap->flags & HEAP_DEBUG_MEMORY_GUARD) != 0)
      {
        heapDebugMemGuard_t *dbgInfo = (heapDebugMemGuard_t *)wholeBuff;
        dbgInfo->guardWord = HEAP_GUARD_WORD_HEADER;
        dbgInfo = (heapDebugMemGuard_t *)((L7_uint32)(output) + buffPool->buffSize);
        dbgInfo->guardWord = HEAP_GUARD_WORD_TRAILER;
      }

      /* Update stats and exit */
      buffPool->stats.allocSuccess++;
      buffPool->stats.allocCount++;
      if(buffPool->stats.maxWaterLevel < buffPool->stats.allocCount)
      {
        buffPool->stats.maxWaterLevel = buffPool->stats.allocCount;
      }
      break;
    }
    else
    {
      if((buffPool->flags & HEAP_BUFFPOOL_NO_OVERFLOW) != 0)
      {
        /* Update the stats and exit */
        buffPool->stats.allocFail++;
        break;
      }
      buffPool->stats.overflowCount++;
    }
  }

  /* If system mallocs are allowed, then try and satisfy the request using mallocs */
  if((i == heap->buffPoolCount) || (buffPool == L7_NULLPTR))
  {
    if((heap->flags & HEAP_MALLOC_ENABLE) != 0)
    {
      HEAP_TRACE(heap->dbgPrint,
                 "Allocating from system memory for heapID - %x and size -%d \n",
                 heapId, size);
      output = (void*)osapiMalloc(heap->compId, size);
      if(output != L7_NULLPTR)
      {
        heap->sysMemStats.allocCount++;
        heap->sysMemStats.allocSuccess++;
        if(heap->sysMemStats.maxWaterLevel < heap->sysMemStats.allocCount)
        {
          heap->sysMemStats.maxWaterLevel = heap->sysMemStats.allocCount;
        }
      }
      else
      {
        heap->sysMemStats.allocFail++;
      }
    }
    else
    {
      HEAP_TRACE(heap->dbgPrint,
                 "Failed: to allocate memory for heapID - %x and size -%d \n",
                 heapId,size);
      heap->sysMemStats.allocFail++;
    }
  }

  /* Unlock access */
  if((heap->flags & HEAP_LOCK_GUARD) != 0)
  {
    osapiSemaGive(heap->semId);
  }

  return output;
}

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
              void      *pMem)
{
  heap_t     *heap = (heap_t*)heapId;
  buffPool_t *buffPool;

  /* Basic sanity checks */
  if((heap == L7_NULL) || (pMem == L7_NULLPTR) ||
     (*((L7_uint32*)heapId) != heapId))
  {
    HEAP_TRACE(HEAP_PRINT_ALWAYS,"Failed to free to heap - Invalid Params\n");
    return;
  }
  if(heapIsInUse(heap) == L7_FALSE)
  {
    /* Un-initialized heap access */
    HEAP_TRACE(HEAP_PRINT_ALWAYS,"Failed to free to heap - Uninitialized heap\n");
    return;
  }

  /* Lock the access, if needed */
  if((heap->flags & HEAP_LOCK_GUARD) != 0)
  {
    if(osapiSemaTake(heap->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      return;
    }
  }

  /* Retrieve the buffer pool structure associated with the given buffer */
  if(heapBuffPoolGet(heap, pMem, &buffPool) == L7_SUCCESS)
  {
    /* Valid buffer to be freed-up */
    *(L7_uint32 *)pMem = (L7_uint32)(buffPool->freeList);
    buffPool->freeList = pMem;

    /* Update stats */
    buffPool->stats.allocCount--;
    buffPool->stats.cumFreeCount++;
  }
  else if((buffPool == L7_NULLPTR) && ((heap->flags & HEAP_MALLOC_ENABLE) != 0))
  {
    /* Not within our heap's address range. Try freeing up to the system heap */
    osapiFree(heap->compId, pMem);
    heap->sysMemStats.allocCount--;
    heap->sysMemStats.cumFreeCount++;
  }
  else
  {
    HEAP_TRACE(HEAP_PRINT_ALWAYS,"\n[%s:%u] ERROR: Buffer/Mem 0x%x is not a valid buffer block within heap %x; heapFree failed.\n",
               __FUNCTION__,__LINE__,pMem,heap);
  }

  /* Unlock access */
  if((heap->flags & HEAP_LOCK_GUARD) != 0)
  {
    osapiSemaGive(heap->semId);
  }
}

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
                         void      *pMem)
{
  heap_t     *heap = (heap_t*)heapId;
  buffPool_t *buffPool;
  L7_RC_t    retVal = L7_FAILURE;

  /* Basic sanity checks */
  if((heap == L7_NULL) || (pMem == L7_NULLPTR) ||
     (*((L7_uint32*)heapId) != heapId))
  {
    return retVal;
  }
  if(heapIsInUse(heap) == L7_FALSE)
  {
    /* Un-initialized heap access */
    HEAP_TRACE(HEAP_PRINT_ALWAYS,"Uninitialized heap\n");
    return retVal;
  }

  /* Lock the access, if needed */
  if((heap->flags & HEAP_LOCK_GUARD) != 0)
  {
    if(osapiSemaTake(heap->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      return retVal;
    }
  }

  /* Retrieve the buffer pool structure associated with the given buffer */
  retVal = heapBuffPoolGet(heap, pMem, &buffPool);

  /* Unlock access */
  if((heap->flags & HEAP_LOCK_GUARD) != 0)
  {
    osapiSemaGive(heap->semId);
  }

  return retVal;
}

/*********************************************************************
*
* @purpose  To display the basic statistics for a heap
*
* @param    heapId @b((input)) Heap ID
*
* @returns  None
*
* @notes    memShow equivalent data output
*
* @end
*********************************************************************/
void  heapShow(L7_uint32 heapId)
{
  heap_t *heap = (heap_t *)heapId;

  /* validate heap ID */
  if((heap == L7_NULLPTR) || (*((L7_uint32*)heapId) != heapId))
  {
    sysapiPrintf("Invalid Heap ID : 0x%x\n", heapId);
    return;
  }
  /* First dump the heap structure info */
  sysapiPrintf(" \n *********************** HEAP INFORMATION ********************** \n");
  sysapiPrintf(" Heap ID                        - 0x%x\n",heapId);
  sysapiPrintf(" Component ID                   - %u\n",heap->compId);
  sysapiPrintf(" Total heap size                - %u\n",heap->heapSize);
  if(heapIsInUse(heap) == L7_FALSE)
  {
    sysapiPrintf(" Heap Not In Use\n");
  }
  else
  {
    sysapiPrintf(" Heap BufferPool Access SemId   - %p\n",heap->semId);
    sysapiPrintf(" Heap Access Protection SemId   - %p\n",heap->heapSemId);
    sysapiPrintf(" Heap size in use               - %u\n",heap->inUse);
    sysapiPrintf(" Number of Buffer Pools         - %u\n",heap->buffPoolCount);
    sysapiPrintf(" Header Size                    - %u\n",heap->headerSize);
    sysapiPrintf(" Heap Flags                     - 0x%x\n",heap->flags);
    sysapiPrintf(" Outstanding system heap allocs - %u\n",heap->sysMemStats.allocCount);
    sysapiPrintf(" System heap alloc success      - %u\n",heap->sysMemStats.allocSuccess);
    sysapiPrintf(" System heap alloc failures     - %u\n",heap->sysMemStats.allocFail);
    sysapiPrintf(" System heap cummulative frees  - %u\n",heap->sysMemStats.cumFreeCount);
    sysapiPrintf(" System heap usage max watermark- %u\n",heap->sysMemStats.maxWaterLevel);
  }
}

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
                     L7_uint32 dbgPrint)
{
  heap_t *heap = (heap_t*)heapId;

  /* validate heap ID */
  if((heap == L7_NULLPTR) || (*((L7_uint32*)heapId) != heapId))
  {
    return;
  }
  heap->dbgPrint = dbgPrint;
}

/*********************************************************************
*
* @purpose  To display the statistics for a heap
*
* @param    heapId  @b{(input)} Heap ID
*
* @returns  None
*
* @notes
*
* @end
*********************************************************************/
void heapDbgStatsDisplay(L7_uint32 heapId)
{
  heap_t     *heap = (heap_t*)heapId;
  buffPool_t *tempPool;
  L7_uint32  i;

  /* Basic sanity checks */
  if(heap == L7_NULLPTR || (*((L7_uint32*)heapId) != heapId))
  {
    sysapiPrintf(" \n Invalid Heap ID : 0x%x\n", heapId);
    return;
  }

  /* First dump the heap structure info */
  sysapiPrintf(" \n *********************** HEAP INFORMATION ********************** \n");
  sysapiPrintf(" Heap ID                        - 0x%x\n",heapId);
  sysapiPrintf(" Component ID                   - %u\n",heap->compId);
  sysapiPrintf(" Total heap size                - %u\n",heap->heapSize);
  if(heapIsInUse(heap) == L7_FALSE)
  {
    sysapiPrintf(" Heap Not In Use\n");
  }
  else
  {
    sysapiPrintf(" Heap size in use               - %u\n",heap->inUse);
    sysapiPrintf(" Number of Buffer Pools         - %u\n",heap->buffPoolCount);
    sysapiPrintf(" Header Size                    - %u\n",heap->headerSize);
    sysapiPrintf(" Heap Flags                     - 0x%x\n",heap->flags);
    sysapiPrintf(" Outstanding system heap allocs - %u\n",heap->sysMemStats.allocCount);
    sysapiPrintf(" System heap alloc success      - %u\n",heap->sysMemStats.allocSuccess);
    sysapiPrintf(" System heap alloc failures     - %u\n",heap->sysMemStats.allocFail);
    sysapiPrintf(" System heap cummulative frees  - %u\n",heap->sysMemStats.cumFreeCount);
    sysapiPrintf(" System heap usage max watermark- %u\n",heap->sysMemStats.maxWaterLevel);

    /* Statistics of the individual pools */
    tempPool = heap->buffPool;
    for(i = 0; (i < heap->buffPoolCount) && (tempPool != L7_NULLPTR); i++, tempPool++)
    {
      sysapiPrintf(" \n ******** Buffer Pool #%d Stats :\n", i + 1);
      sysapiPrintf("   Buffer Size                - %u\n",tempPool->buffSize);
      sysapiPrintf("   Number of buffers          - %u\n",tempPool->buffCount);
      sysapiPrintf("   Description of the pool    - %s\n",tempPool->descr);
      sysapiPrintf("   Flags                      - 0x%x\n",tempPool->flags);
      sysapiPrintf("   Starting Memory Address    - 0x%x\n",tempPool->startLoc);
      sysapiPrintf("   Ending Memory Address      - 0x%x\n",tempPool->endLoc);
      sysapiPrintf("   Current outstanding allocs - %u\n",tempPool->stats.allocCount);
      sysapiPrintf("   Total successful allocs    - %u\n",tempPool->stats.allocSuccess);
      sysapiPrintf("   Total failed allocs        - %u\n",tempPool->stats.allocFail);
      sysapiPrintf("   Cummulative frees          - %u\n",tempPool->stats.cumFreeCount);
      sysapiPrintf("   Usage maximum watermark    - %u\n",tempPool->stats.maxWaterLevel);
      sysapiPrintf("   Total request overflows    - %u\n",tempPool->stats.overflowCount);
    }
  }
}

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
                     void      *pMem)
{
  heap_t         *heap = (heap_t*)heapId;
  buffPool_t     *buffPool;

  /* Basic sanity checks */
  if((heap == L7_NULLPTR) || (*((L7_uint32*)heapId) != heapId))
  {
    sysapiPrintf(" \n Invalid Heap ID : 0x%x\n", heapId);
    return;
  }
  if(pMem == L7_NULLPTR)
  {
    sysapiPrintf(" \n Invalid Buffer Pointer of NULL\n");
    return;
  }
  if(heapIsInUse(heap) == L7_FALSE)
  {
    /* Un-initialized heap access */
    sysapiPrintf(" \n Uninitialized heap\n");
    return;
  }

  /* Lock the access, if needed */
  if((heap->flags & HEAP_LOCK_GUARD) != 0)
  {
    if(osapiSemaTake(heap->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      sysapiPrintf(" \n Failed to acquire heap semaphore for heapId 0x%x\n", heapId);
      return;
    }
  }

  /* Retrieve the buffer pool structure associated with the given buffer */
  if(heapBuffPoolGet(heap, pMem, &buffPool) != L7_SUCCESS)
  {
    sysapiPrintf(" \n Buffer at 0x%x is not a valid buffer for heapId 0x%x\n",
           (L7_uint32)pMem, heapId);
  }
  else
  {
    L7_uint32 wholeBuffAddress;

    wholeBuffAddress = (L7_uint32)pMem - heap->headerSize;

    /* display the info */
    sysapiPrintf(" \n ************************** Memory Details ****************************** \n");
    sysapiPrintf(" Buffer/memory 0x%x resides in Buffer Pool - %s \n",
           (L7_uint32)pMem, buffPool->descr);
    if((heap->flags & HEAP_DEBUG_FILE_INFO) != 0)
    {
      heapDebugInfo_t *pDbgInfo = (heapDebugInfo_t *)wholeBuffAddress;
      sysapiPrintf(" Last allocated at File - %s:%d in task - %d and for size - %d \n",
             pDbgInfo->fileName,pDbgInfo->lineNumber,pDbgInfo->taskId,
             pDbgInfo->sizeRequest);
    }
    if((heap->flags & HEAP_DEBUG_MEMORY_GUARD) != 0)
    {
      heapDebugMemGuard_t *pDbgInfo =
        (heapDebugMemGuard_t *)((L7_uint32)pMem - sizeof(heapDebugMemGuard_t));
      sysapiPrintf(" Header Guard Word - 0x%x\n", pDbgInfo->guardWord);
      pDbgInfo = (heapDebugMemGuard_t *)((L7_uint32)pMem + buffPool->buffSize);
      sysapiPrintf(" Footer Guard Word - 0x%x\n", pDbgInfo->guardWord);
    }
  }

  /* Unlock access */
  if((heap->flags & HEAP_LOCK_GUARD) != 0)
  {
    osapiSemaGive(heap->semId);
  }
}

