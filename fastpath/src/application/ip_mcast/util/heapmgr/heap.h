/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename   heap.h
 *
 * @purpose    Memory Heap Storage
 *
 * @component  util
 *
 * @comments   All The Structure in this file needs to have a aligned memory
 *             size or else add padding
 *
 * @create     02/02/2007
 *
 * @author     PKB
 * @end
 *
 **********************************************************************/
#ifndef HEAP_INCLUDE_H
#define HEAP_INCLUDE_H

#include "heap_api.h"

/* This macro determines the memory alignment for all the operations of
   heap management */
#define HEAP_BYTE_ALIGNMENT       4

#define HEAP_PRINT_ALWAYS         1

#define HEAP_GUARD_WORD_HEADER    (0xABABABAB)
#define HEAP_GUARD_WORD_TRAILER   (0xDEADBEEF)

/* Macro used to trace execution within the library */
#define HEAP_TRACE(flag, __fmt__, __args__...)                     \
  if ((flag) != 0 )  {                                             \
   L7_uchar8  __buf1__[256];                                       \
   (void)osapiSnprintf (__buf1__, 256, __fmt__, ## __args__);      \
   sysapiPrintf(__buf1__);}

/* This structure needs to have a aligned memory size or else add padding */
typedef struct heapDebugInfo_s
{
  L7_uchar8 fileName[HEAP_FILENAME_SIZE];
  L7_uint32 lineNumber;
  L7_uint32 taskId;
  L7_uint32 sizeRequest;
}heapDebugInfo_t;

/* This structure would be prefixed and suffixed for every buffer allocated */
typedef struct heapDebugMemGuard_s
{
  L7_uint32 guardWord;     /* Guard Word around the user area of a buffer */
}heapDebugMemGuard_t;

typedef struct heapStats_s
{
  L7_uint32 allocCount;     /* Number of buffers currently allocated */
  L7_uint32 allocSuccess;   /* Number of buffer requests successfully fulfilled */
  L7_uint32 allocFail;      /* Number of buffer requests that could not be satistied */
  L7_uint32 cumFreeCount;   /* Number of buffer frees - cummulative */
  L7_uint32 maxWaterLevel;  /* Maximum number of buffers allocated at any given instant of time */
  L7_uint32 overflowCount;  /* Number of buffer requests that were passed on to the next buffer pool */
}heapStats_t;

/* This Structure needs to have a aligned memory size or else add padding */
typedef struct buffPool_s
{
  L7_uint32   buffSize;        /* Adjusted user available size per buffer */
  L7_uint32   buffCount;       /* Number of buffers in this pool */
  L7_uchar8   descr[HEAP_POOLNAME_SIZE]; /* Name of the pool */
  L7_uint32   flags;           /* Flags defining extra properties of the pool */
  L7_uint32   startLoc;        /* Start location (absolute address) */
  L7_uint32   endLoc;          /* End location (absolute address) */
  heapStats_t stats;           /* Buffer pool statistics */
  void        *freeList;       /* Singly linked free buffers in the pool */
}buffPool_t;

typedef struct heap_s
{
  L7_uint32          validateMem;     /* Need to be first item always */
  L7_COMPONENT_IDS_t compId;
  L7_uint32          heapSize;        /* Size of heap, excluding the heap node */
  L7_uint32          inUse;           /* Size of heap initialized; 0 if un-initialized*/
  buffPool_t         *buffPool;       /* would be the sorted list */
  L7_uint32          buffPoolCount;   /* number of buffer pools configured */
  L7_uint32          headerSize;      /* Size of headers, if any */
  void               *semId;          /* Semaphore to protect bufferPools accesses */
  void               *heapSemId;          /* Semaphore to protect heap struct accesses */  
  heapStats_t        sysMemStats;     /* Statistics for System Memory accessess */
  heapFlags_t        flags;           /* Flags */
  L7_uint32          dbgPrint;        /* To print debug statements */
}heap_t;

#endif /* HEAP_INCLUDE_H */
