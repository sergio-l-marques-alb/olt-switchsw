/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename      xx.c
 *
 * @purpose       Portable system services implementation
 *
 * @component     Routing Utils Component
 *
 * @comments
 * These services are used throughout the entire
 * stack. The services provided here are for memory
 * management, thread scheduling and creating and
 * managing timer tasks. Memory management is implemented
 * by XX_Malloc and XX_Free. Memory blocks of any size
 * are allocated from the  system heap: after they
 * have been allocated, the blocks are not freed back
 * to the heap, but stored in a hash tables (which is
 * indexed by block size). This makes for quick and
 * flexible memory allocation/deallocation.
 *
 * Timer tasks are tasks registered with the RTOS
 * (or event scheduler) that provide a periodic timer
 * service used by the TIMER object.
 *
 * Threads are created to execute a chain of routine
 * calls. They are usually scheduled (in tasks) by the
 * RTOS, but if no underlying RTOS is used, the built-in
 * event scheduler (a prioritzed, non-preemptive
 * event handler) may be used. The configuration flag
 * RTOS selects between a "real" RTOS (VxWorks, pSOS,
 * etc.) or the built-in scheduler.
 *
 * @create        01/09/1993
 *
 * @author        Jonathan Masel
 *                Igor Bryskin
 *
 * @end
 *
 * ********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: lvl7dev\src\system_support\routing\utils\xx.c 1.1.3.3 2002/06/21 17:50:21EDT hassan Exp  $";
#endif

#include "std.h"
#include "local.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "xx.ext"
#include "mem.ext"
#include "queue.ext"
#include "timer.ext"
#include "ll.ext"

#include "log.h"
#include "l7utils_api.h"


/* Default thread handle */
static ulng StaticL7_tid;


/* PORTABLE SYSTEM SERVICES */

/* XX_Malloc control structures */

/* used to build a temporary free list for debugging */
typedef struct t_FreeListEntry
{
   struct t_FreeListEntry   *next;
#ifdef MEMCHK
   word               allocLine;
   word               freeLine;

   /* these are aliased to values in t_XXOvrHead since they're just around
    * long enough to print the list. */
   char        *allocFile;
   char        *freeFile;
   ulng               reqSize;  /* size requested, zero if exact size requested */
#endif
   ulng               size;
   ulng               count;   /* number of freed blocks with same size, reqSize,
                                * alloc and free locations */
} t_FreeListEntry;

/* XX_Malloc buffer header */
typedef struct t_XXOvrHead
{
   struct t_BufHdr   *next;
   struct t_BufHdr   *prev;
#ifdef MEMCHK
   word               sig;
   word               allocLine;
   word               freeLine;
   char        *allocFile;
   char        *freeFile;
   ulng               reqSize;  /* size requested, zero if called from XX_Malloc */
#endif
   ulng               size;
   t_Handle           heap;
   /* The following field should always be the last one in this structure ! */
   word               align_bias;
} t_XXOvrHead;

typedef struct t_BufHdr
{
  union
  {
     t_XXOvrHead bufHdr;
     byte        align[sizeof(t_XXOvrHead)+(
                 sizeof(t_XXOvrHead)%ALIGN?
                 ALIGN - sizeof(t_XXOvrHead)%ALIGN: 0)];
  }u;
} t_BufHdr;

#define ALLOC_SIGNATURE 0xABAB
#define FREE_SIGNATURE  0xEFEF

#ifdef MEMCHK
#define ZERO_MEM(ptr, size) memset(ptr, 0, size)   /* to track down bugs */
#define MEMORY_OVERHEAD (sizeof(t_BufHdr) + sizeof(word))
#define SET_END_MARKER(bh) \
   *((byte *)bh + bh->u.bufHdr.size - sizeof(word)) = LO_U8(ALLOC_SIGNATURE);\
   *((byte *)bh + bh->u.bufHdr.size - sizeof(word) +1) = HI_U8(ALLOC_SIGNATURE)

#define GET_END_MARKER(bh) \
   MAKE_U16(*((byte *)bh + bh->u.bufHdr.size - sizeof(word)), *((byte *)bh + bh->u.bufHdr.size - sizeof(word)+1))
#else
#define ZERO_MEM(ptr, size)
#define MEMORY_OVERHEAD sizeof(t_BufHdr)
#endif

typedef struct tagt_FreeBufInfo
{
   struct tagt_FreeBufInfo *next;
   ulng  bufLen;
   t_BufHdr *firstFree;
} t_FreeBufInfo;

#define XX_MALLOC_HT_SIZE 37   /* XX_Malloc hash table size*/


typedef struct tagt_Heap
{

   struct tagt_Heap *next;                       /* chaining element                    */
   struct tagt_Heap *prev;
   t_Name         Name;                          /* heap's name                          */
   t_FreeBufInfo *xxMallocHT[XX_MALLOC_HT_SIZE]; /* heap's hash table                    */
   t_BufHdr      *allocList;                     /* heap's list of allocated buffers     */
   t_Handle       mallocPoolHndle;               /* memory pool handle                   */
   ulng           mallocPoolSize;                /* memory pool size                     */
   ulng           totalSize;                     /* current total heap memory usage      */
   ulng           hiWaterMark;                   /* highest found heap memory usage      */
   ulng           freeListSize;                  /* how much memory in free lists        */
   ulng           overAllocation;                /* over allocated bytes                 */
} t_Heap;

/* XX_Malloc default heap */
static t_Heap   DefaultHeap;
/* Heap list */
static t_Heap  *HeapList;
static t_Heap  *DefaultHeapHandle;

/* Timer task info */
typedef struct tagt_TmrTaskInfo
{
   struct tagt_TmrTaskInfo *next;
   struct tagt_TmrTaskInfo *prev;
#ifdef ERRCHK
   XX_Status status;
#endif
   word      ticks;
   word      period;
   e_Err     (*tmrFunc)(void);
} t_TmrTaskInfo;

/* Message event info */
typedef struct tagt_MsgEvent
{
   struct tagt_MsgEvent *next;
   void *msg;
} t_MsgEvent;


/* Thread info structure */
typedef struct tagt_ThreadInfo
{
   struct tagt_ThreadInfo *next;
   struct tagt_ThreadInfo *prev;
#ifdef ERRCHK
   XX_Status   status;
#endif
   byte        priority;
   Bool        fPermanent;
   e_Err     (*threadFunc)(void *);
   t_MsgEvent *eventHead;
   t_MsgEvent *eventTail;
} t_ThreadInfo;

static Bool memXXInit;

/****** Internal RAM management */
#define  INRAM_MAGIC          0xDADCAFE
#define  INRAM_ALLOC_SIGN     0xCAFE
#define  MIN_INRAMBLOCK_SIZE  4

/* Internal RAM block descriptor */
typedef struct tagt_InRAMBlock
{
#ifdef MEMCHK
   ulng         allocSign;
   const char  *allocFile;
   word         allocLine;
#endif
   struct tagt_InRAMBlock *next;
   struct tagt_InRAMBlock *nextFree;
   byte                   *absAddr;
   byte                   *algAddr;
   ulng                    size;
   Bool                    active;

} t_InRAMBlock;

/* Internal RAM control structure */
typedef struct tagt_InRAM
{
   struct tagt_InRAM *nextFree;
   byte              *startAddr;
   ulng              activeSize;
   ulng              totalSize;
   t_InRAMBlock      *head;
   ulng              magic;

} t_InRAM;

/* Allocate statically descriptors for INRAM manager */
#define XX_MAX_INRAM_POOLS    4
#define XX_MAX_INRAM_BLOCKS   100
/* InRAM pools array */
static t_InRAM       inRamPoolArray[XX_MAX_INRAM_POOLS];
/* InRAM blocks array */
static t_InRAMBlock  inRamBlockArray[XX_MAX_INRAM_BLOCKS];
/* InRAM Pool free list head */
static t_InRAM       *inRamFreeListHead;
/* InRAM Block free list head */
static t_InRAMBlock  *inRamBlockFreeListHead;
/* TRUE=initialized */
static Bool          inRamMngrInitialized;

/* static functions for Intrenal RAM management */
static t_InRAM *GetInRAMPoolHdr( void );
static void PutInRAMPoolHdr( t_InRAM *poolHdr );

static t_InRAMBlock *GetInRAMBlockHdr( void );

static void PutInRAMBlockHdr( t_InRAMBlock *blockHdr );

static t_InRAMBlock *addr_SearchB( t_InRAM *pInRAM,
                                   byte *Addr,
                                   t_Handle *phPrev );

static t_InRAMBlock *opts_SearchB( t_InRAM *pInRAM,
                                   ulng Size,
                                   ulng Align,
                                   ulng *pAlignDelta,
                                   t_Handle *phPrev );

static t_InRAMBlock *rsrv_SearchB( t_InRAM *pInRAM,
                                   byte *Start,
                                   ulng Size,
                                   ulng *pPrevDelta,
                                   ulng *pNextDelta,
                                   t_Handle *phPrev );

void XX_SetL7_tid(t_Handle thread)
{
    StaticL7_tid = (ulng)thread;
}

ulng XX_GetL7_tid(void)
{
    return StaticL7_tid;
}

/*** Initiate system services ***/
e_Err  XX_InitAll( void *mallocPoolStartAddr, ulng malloc_pool_size)
{
   e_Err  rc;

   MEM_InitAll();
   Q_InitAll();

   /* create MALLOC pool */
   rc = OS_MEM_Init(mallocPoolStartAddr, DefaultHeap.mallocPoolSize=malloc_pool_size, &DefaultHeap.mallocPoolHndle);
   if(rc != E_OK)
   {
      ASSERT(FALSE);
      return rc;
   }
   memset(&DefaultHeap.xxMallocHT, 0, sizeof(DefaultHeap.xxMallocHT));
   memcpy((byte *)&DefaultHeap.Name, "DefH", sizeof(t_Name));
   DefaultHeap.allocList = NULLP;
   DefaultHeapHandle = &DefaultHeap;
   memXXInit = TRUE;
   HeapList = NULLP;
   XX_AddToDLList(DefaultHeapHandle, HeapList);

   /* Initialize LL service */
   LL_InitAll();

   TIMER_InitAll();
   return E_OK;
}


/*** Create specific heap ***/
e_Err  XX_InitHeap( const char *Name, void *mallocPoolStartAddr, ulng malloc_pool_size, t_Handle *Heap)
{
   e_Err    rc;
   t_Handle mallocPoolHndle;
   t_Heap  *heap;

   rc = OS_MEM_Init(mallocPoolStartAddr,
      2*malloc_pool_size - OS_MEM_SizeofInitAlign( (Bool)(mallocPoolStartAddr ? TRUE : FALSE), malloc_pool_size, ALIGN),
      &mallocPoolHndle);
   if(rc != E_OK)
   {
      ASSERT(FALSE);
      return rc;
   }

   heap = (t_Heap *)OS_GetMem(mallocPoolHndle, sizeof(t_Heap));

   if( !heap )
      return E_NOMEMORY;

   memset((byte *)heap, 0, sizeof(t_Heap));
   if(Name)
      memcpy(&heap->Name, Name, sizeof(t_Name));
   heap->mallocPoolSize  = malloc_pool_size - sizeof(t_Heap);
   heap->mallocPoolHndle = mallocPoolHndle;

   *Heap = (t_Handle)heap;
   XX_AddToDLList(heap, HeapList);
   return E_OK;
}



/* Release all resources allocated by system services */
void  XX_ShutDown(void)
{
   if(!memXXInit)
      return;
   TIMER_KillAll();
   Q_ShutDown();
   MEM_ShutDown();

   /* delete all heaps */
   {
      t_Heap *heap;

      for(heap = HeapList; heap; heap=heap->next)
      {
         OS_MEM_Delete(heap->mallocPoolHndle);
         heap->allocList    = NULLP;
         heap->freeListSize = 0;
         memset(&heap->xxMallocHT, 0, sizeof(heap->xxMallocHT[XX_MALLOC_HT_SIZE]));
      }
   }
   memXXInit = FALSE;
}

/* Print assertion failure info and quit  */
int __assertfail(const char *msg, const char *cond, const char *file, int line)
{
   char prntBuf[180];
#if defined(WIN) || defined (_WINDOWS)
   char *p;
   if ((p = strrchr(file, '\\')) != NULL) /* skip over directory */
      file = p + 1;
#endif

   sprintf(prntBuf, msg, cond, file, line);
   XX_Print(prntBuf);

   XX_DisplayMemExt(0,4);
   sleep(10);

   log_error_code(0xdeadbeef, (L7_uchar8 *) file, (L7_uint32) line);
   return 1;
}

/* Return memory in bytes currently used from specific MALLOC heap */
ulng XX_GetCurUsedMemExt(t_Handle Heap)
{
    t_Heap *p_Heap = (t_Heap *)(Heap? Heap: DefaultHeapHandle);
    return  p_Heap->totalSize;
}


/* Return highest so far used memory in bytes from specific MALLOC heap */
ulng XX_GetHiUsedMemExt(t_Handle Heap)
{
    t_Heap *p_Heap = (t_Heap *)(Heap? Heap: DefaultHeapHandle);
    return p_Heap->hiWaterMark;
}



#if L7_DEBUG >= 3
/* Default XX_Trace routine */
static void xx_traceSMUnpack(t_XXCallInfo *xx)
{
    char *Name = (char *)PACKET_GET(xx, 1);
    byte Event = (byte ) PACKET_GET(xx, 2);
    byte State = (byte ) PACKET_GET(xx, 3);
    byte name[5];
    byte buf[120];

    memset(name, 0, 5);
    strncpy(name, Name, 4);

    sprintf(buf,"TraceSM:%s event:%3d(0x%2x) state %3d(0x%2x)",
       name, Event, Event, State, State);
    XX_Print(buf);
}
#endif /* L7_DEBUG >= 3 */

#if L7_DEBUG >= 1
/* Error unpack routine */
static const char *errorStrings[ERR_LAST_ENTRY] = {"",
    "System error", "Lock failed", "Receive discard",
    "Flash failed", "Invalid FMMID", "Error on frame read",
    "Invalid PVC frame", "Invalid LMI frame", "Invalid message type",
    "Unexpected FMM", "Frame too short", "Invalid report type",
    "Invalid inf.element type", "Missing inf. element",
    "Received sequence number", "Protocol error", "Bad inf.element",
    "Bad sequence type", "Bad sequence number", "Bad report type",
    "Bad multi-status", "No report type", "No sequence",
    "No match", "Invalid frame", "Bad match", "No routing",
    "No memory", "Pool isn't empty", "Too many cells",
    "Match exists", "Not configured", "Configuration failed",
    "Bad target"};
static void xx_errorUnpack(t_XXCallInfo *xx)
{
    e_ErrLog err   = (e_ErrLog)PACKET_GET(xx, 1);
    char     *Parm = (char *)  PACKET_GET(xx, 2);
    char     name[5];
    char     buf[120];

    memset(name, 0, 5);
    strncpy(name, Parm, 4);
    if(err >= ERR_LAST_ENTRY)
        return;
    sprintf(buf,"Err:%s object:%s", errorStrings[err], name);
    XX_Print(buf);
}
#endif
#if L7_DEBUG >= 2
/* Default XX_Event routine */
static const char *eventString[EV_LAST_EVENT] =
{   "", "Down", "Too Long", "Transmit discarded",
    "Receive discarded","Rx congested", "Tx congested", "Port Rx clear", "Port Tx clear",
    "Tighten gauge", "Loosen gauge", "CIR exceded", "EIR exceded",
    "Connection updated", "Destination conn. updated", "Bad CRC",
    "SAR transmit overflow", "OAM transmit overflow", "No perfmon",
    "Invalid cell", "Received AIS", "Received RDI", "Received CC",
    "AIS Set", "AIS Cleared", "RDI Set", "RDI Cleared", "CC Activated",
    "CC Deactivated", "Bad CPI", "Interrupt", "IDTSAR alarm. Status[15:0]=", "Lack of memory"
};
static void xx_eventUnpack( t_XXCallInfo *xx)
{
    e_Event Ev = (e_Event)PACKET_GET(xx, 1);
    char    *Parm = (char *) PACKET_GET(xx, 2);
    static  char name[5];
    static  char buf[120];

    memset(name, 0, 5);
    strncpy(name, Parm, 4);

    if(Ev >= EV_LAST_EVENT)
        return;
    sprintf(buf,"Event:%s object:%s", eventString[Ev], name);
    XX_Print(buf);
}
#endif /*  L7_DEBUG >= 2 */


#if L7_DEBUG >= 3
/* default XX_Trace routine */
static const char *traceString[TRC_LAST_ENTRY] = {"", "FP transmit", "FP receive", "FP down",
    "FP Up", "FP Rx alloc", "FP Tx free", "FC transmit", "FC Tx frame",
    "FC receive", "FC receive frame", "FE transmit", "FE receive",
    "FC Tx congested", "FC active", "FC deactive", "FC delete",
    "FU pour", "PVCN receive", "PVCU receive", "LMIN receive", "LMIU receive",
    "AP receive", "AP transmit", "ATM receive", "ATM transmit", "AAL receive",
    "AAL transmit", "AAL Tx Frame", "AAL Delete", "CP5 transmit", "CP5 receive", "SA5 transmit",
    "SA5 receive", "SA34 receive", "SA34 transmit", "OAM transmit", "OAM receive",
    "AE transmit", "AE receive", "CE transmit", "SA1 transmit", "SA1 receive",
    "Received RM cell", "Transmit RM cell", "CP0 transmit", "CP0 receive",
    "CP2 transmit", "CP2 receive", "AL2 transmit", "AL2 receive"};
static void xx_traceUnpack(t_XXCallInfo *xx)
{
    byte    name[5];
    byte    buf[120];
    e_Trace Prim = (e_Trace)PACKET_GET(xx, 1);
    char    *Id  = (char *) PACKET_GET(xx, 2);

    memset(name, 0, 5);
    strncpy(name, Id, 4);

    if(Prim >= TRC_LAST_ENTRY)
        return;
    sprintf(buf,"Trace:'%s' object:'%s'", traceString[Prim], name);
    XX_Print(buf);
}
#endif /* L7_DEBUG >= 3 */

/* unpack call code for the xx_unpackInternal routine */
#define XX_TRACE_CALL   0
#define XX_TRACESM_CALL 1
#define XX_EVENT_CALL   2
#define XX_ERROR_CALL   3

#if L7_DEBUG >= 1
/* Internal static routine to unpack
   XX_Call from XX_Trace(); XX_TraceSM(); XX_Event()
   and XX_Error() API calls
*/
static e_Err xx_unpackInternal(void *msg)
{
    t_XXCallInfo *xx = (t_XXCallInfo *)msg;
    ulng         code = PACKET_GET(xx, 0);
    switch(code)
    {
#if L7_DEBUG >= 3
    case XX_TRACE_CALL:   xx_traceUnpack(xx);   return E_OK;
    case XX_TRACESM_CALL: xx_traceSMUnpack(xx); return E_OK;
#endif
#if L7_DEBUG >= 2
    case XX_EVENT_CALL:   xx_eventUnpack(xx);   return E_OK;
#endif
    case XX_ERROR_CALL:   xx_errorUnpack(xx);   return E_OK;
    default:
       break;
    }
    return E_OK;
}
#endif /* L7_DEBUG >= 1 */

#if L7_DEBUG >= 3

/* General procedures for tracing */
static F_XXTraceSM F_ExtTraceSM;

/* Redirect dafault trace SM procedure */
F_XXTraceSM XX_RedirectTraceSM(F_XXTraceSM f_traceSM)
{
    F_XXTraceSM f_old = F_ExtTraceSM;
    if(f_traceSM)
        F_ExtTraceSM = f_traceSM;
    else
        F_ExtTraceSM = (F_XXTraceSM)0xffffffff;
    return f_old;
}

/* Trace SM procedure */
void XX_TraceSM(const char *Name, byte Event, byte State)
{
    t_XXCallInfo *xx;
    if(F_ExtTraceSM)
    {
        /* if XX_TraceSM turned off just return */
        if(F_ExtTraceSM == (F_XXTraceSM)0xffffffff)
            return;
        /* If XX_Trace was redirected by user call user function */
        F_ExtTraceSM((char *)Name, Event, State);
        return;
    }
    if(!L7_DEFAULT_THREAD)
        return;
    PACKET_INIT(xx, xx_unpackInternal, NULLP, NULLP, 4, XX_TRACESM_CALL);
    if(xx)
    {
      PACKET_PUT(xx, Name);
      PACKET_PUT(xx, Event);
      PACKET_PUT(xx, State);
    }
    XX_Call(L7_DEFAULT_THREAD, xx);
}
static F_XXTrace F_ExtTrace;

/* Redirect dafault trace procedure */
F_XXTrace XX_RedirectTrace(F_XXTrace f_trace)
{
    F_XXTrace f_old = F_ExtTrace;
    if(f_trace)
        F_ExtTrace = f_trace;
    else
        F_ExtTrace = (F_XXTrace)0xffffffff;
    return f_old;
}

/* Trace procedure */
void    XX_Trace( e_Trace Prim, const char *Id )
{
    t_XXCallInfo *xx;
    if(F_ExtTrace)
    {
        /* if XX_Trace turned off just return */
        if(F_ExtTrace == (F_XXTrace)0xffffffff)
            return;
        /* If XX_Trace was redirected by user call user function */
        F_ExtTrace(Prim, (char *)Id);
        return;
    }
    if(!L7_DEFAULT_THREAD)
        return;
    PACKET_INIT(xx, xx_unpackInternal, NULLP, NULLP, 3, XX_TRACE_CALL);
    if(xx)
    {
      PACKET_PUT(xx, Prim);
      PACKET_PUT(xx, Id);
    }
    XX_Call(L7_DEFAULT_THREAD, xx);
}
#endif /* L7_DEBUG >= 3 */

#if L7_DEBUG >= 2
static F_XXEvent F_ExtEvent;

/* Redirect default Event procedure */
F_XXEvent XX_RedirectEvent(F_XXEvent f_event)
{
    F_XXEvent f_old = F_ExtEvent;
    if(f_event)
        F_ExtEvent = f_event;
    else
        F_ExtEvent = (F_XXEvent)0xffffffff;
    return f_old;
}

/* Event procedure */
void  XX_Event( e_Event Ev, const char *Parm )
{
    t_XXCallInfo *xx;
    if(F_ExtEvent)
    {
        /* if XX_event turned off just return */
        if(F_ExtEvent == (F_XXEvent)0xffffffff)
            return;
        /* If XX_Event was redirected by user call user function */
        F_ExtEvent(Ev, (char *)Parm);
        return;
    }
    if(!L7_DEFAULT_THREAD)
        return;
    PACKET_INIT(xx, xx_unpackInternal, NULLP, NULLP, 3, XX_EVENT_CALL);
    if(xx)
    {
       PACKET_PUT(xx, Ev);
       PACKET_PUT(xx, Parm);
    }
    XX_Call(L7_DEFAULT_THREAD, xx);
}
#endif

#if L7_DEBUG >= 1
/* Error procedure */
void XX_Error(e_ErrLog err, const char *Parm)
{
    t_XXCallInfo *xx;
    if(!L7_DEFAULT_THREAD)
    {
        char  name[5];
        char  buf[80];
        memset(name, 0, 5);
        strncpy(name, Parm, 4);
        sprintf(buf, "Error:%s, object:%s", errorStrings[err], name);
        XX_Print(buf);
        return;
    }
    PACKET_INIT(xx, xx_unpackInternal, NULLP, NULLP, 3, XX_ERROR_CALL);
    if(xx)
    {
      PACKET_PUT(xx, err);
      PACKET_PUT(xx, Parm);
    }
    XX_Call(L7_DEFAULT_THREAD, xx);
}
#endif

/* Allocate buffer of the specified size with specified alignment from the specified MALLOC heap
 */
#ifdef MEMCHK
void *_XX_MallocAlignExt(t_Handle Heap, ulng size, word align, const char *file, word line)
#else
void *XX_MallocAlignExt(t_Handle Heap, ulng size, word align)
#endif
{
   void *buf;

#ifdef MEMCHK
   UNUSED( file );
   UNUSED( line );
#endif
   if( !(buf = XX_MallocExt( Heap, (align <= ALIGN) ? size : size + (align - ALIGN), TRUE)) )
      return NULLP;
   else
   {
      byte *origin = (byte *)buf;

      ALIGN_ADDRESS(buf, align, void *);
      /* Fill in alignment bias */
      *((word *)buf-1) = (word)((ulng)buf - (ulng)origin);
   }
   return buf;
}



/* Allocate buffer of the specified size from the specified MALLOC heap
 */
#ifdef MEMCHK
void *_XX_MallocExt(t_Handle Heap, ulng reqSize, Bool isExact, const char *file, word line)
#else
void *XX_MallocExt(t_Handle Heap, ulng reqSize, Bool isExact)
#endif
{
   t_FreeBufInfo *freeBufInfo;
   word           index;
   t_BufHdr      *bufHdr = NULLP;
   t_Heap        *heap   = (t_Heap *)(Heap? Heap: DefaultHeapHandle);
   ulng           size = 0;

   ASSERT(memXXInit);
   ASSERT(reqSize != 0);
   reqSize += MEMORY_OVERHEAD;

   if(isExact)
   {
     size = reqSize;
   }
   else if(reqSize > 32768)
   {
     /* beyond 32k treat as an exact allocation */
     size = reqSize;
     isExact = TRUE;
   }
   else
   {
     /* min chunk size is 32 */
     size = 32;
     while(size < reqSize)
       size <<= 1;
   }

   index = (word)((size >> 2) % XX_MALLOC_HT_SIZE);

   XX_Freeze();

   /* Try to find free list of buffers of specified size */
   for(freeBufInfo = heap->xxMallocHT[index]; freeBufInfo; freeBufInfo = freeBufInfo->next)
   {
      if(freeBufInfo->bufLen == size)
      {
         if((bufHdr = freeBufInfo->firstFree) != NULLP)
            freeBufInfo->firstFree = bufHdr->u.bufHdr.next;
         break;
      }
   }

   if(!bufHdr)
   {
      /* no free buffers available, ask OS for memory */
      bufHdr = (t_BufHdr *)OS_GetMem(heap->mallocPoolHndle, size+ALIGN);
      if( !bufHdr )
      {
        L7_uchar8 msg[512];
         XX_Thaw();
         osapiSnprintf(msg, 512,
                       "\nFailed to allocate memory chunk of size %u from routing heap",
                       size);
         XX_Print(msg);
#ifdef MEMCHK
         osapiSnprintf(msg, 512,
                       "\nMemory allocation from %s:%u", file, line);
         XX_Print(msg);
#endif
         ASSERT(0);        /* print memory usage and die */
         return 0;
      }

#if ALIGN > 0
      ALIGN_ADDRESS(bufHdr, ALIGN, t_BufHdr *);
#endif

      heap->overAllocation += (size - reqSize);
   }
   else
      heap->freeListSize -= (size + ALIGN);

   heap->totalSize += (size + ALIGN);
   if (heap->totalSize > heap->hiWaterMark)
      heap->hiWaterMark = heap->totalSize;

   bufHdr->u.bufHdr.size = size;
   bufHdr->u.bufHdr.heap = heap;
#ifdef MEMCHK
   bufHdr->u.bufHdr.sig       = ALLOC_SIGNATURE;
   bufHdr->u.bufHdr.allocFile = (char*) file;
   bufHdr->u.bufHdr.allocLine = line;
   bufHdr->u.bufHdr.reqSize   = (isExact ? 0 : reqSize);
   SET_END_MARKER(bufHdr);
#endif
   bufHdr->u.bufHdr.prev = NULLP;
   if((bufHdr->u.bufHdr.next = heap->allocList) != NULLP)
      bufHdr->u.bufHdr.next->u.bufHdr.prev = bufHdr;
   heap->allocList = bufHdr;
   XX_Thaw();
   /* Move pointer over buffer overhead to data itself */
   bufHdr++;
   /* Mark buffer as not specially aligned by clearing alignment bias */
   *(((word *)bufHdr) - 1) = 0;
   return (void *)bufHdr;
}


/*
 * Return buffer to the proper MALLOC heap
 */
#ifdef MEMCHK
void _XX_Free(void *buf, const char *file, word line)
#else
void XX_Free(void *buf)
#endif
{
   t_FreeBufInfo *freeBufInfo, **p;
   t_BufHdr *bufHdr;
   word      index;
   ulng      size;
   t_Heap   *heap;

   ASSERT(memXXInit);
   ASSERT(buf);
   bufHdr = (t_BufHdr *)((byte *)buf - *((word *)buf - 1)) - 1;
   size = bufHdr->u.bufHdr.size;

#ifdef MEMCHK

   /* If the buffer has already been freed previously
      exit gracefully */
   if(bufHdr->u.bufHdr.sig == FREE_SIGNATURE)
     return;

   if (GET_END_MARKER(bufHdr) != ALLOC_SIGNATURE)
       return;

   ASSERT(bufHdr->u.bufHdr.sig == ALLOC_SIGNATURE);

#endif

   index = (word)((size >> 2) % XX_MALLOC_HT_SIZE);
   heap = (t_Heap *)bufHdr->u.bufHdr.heap;

   XX_Freeze();

   heap->totalSize    -= (size + ALIGN);
   heap->freeListSize += (size + ALIGN);

#ifdef MEMCHK
   /* memset(bufHdr, 0, size); */
   bufHdr->u.bufHdr.sig = FREE_SIGNATURE;
   bufHdr->u.bufHdr.freeFile = (char*) file;
   bufHdr->u.bufHdr.freeLine = line;
#endif

   /* Take out of list of allocated buffers */
   if(bufHdr->u.bufHdr.next)
      bufHdr->u.bufHdr.next->u.bufHdr.prev = bufHdr->u.bufHdr.prev;
   if(bufHdr->u.bufHdr.prev)
      bufHdr->u.bufHdr.prev->u.bufHdr.next = bufHdr->u.bufHdr.next;
   else
      heap->allocList = bufHdr->u.bufHdr.next;

   /* Put the buffer into free list */
   p= &(heap->xxMallocHT[index]);
   for(freeBufInfo = heap->xxMallocHT[index]; freeBufInfo; freeBufInfo = freeBufInfo->next)
   {
      if(freeBufInfo->bufLen == size)
      {
         bufHdr->u.bufHdr.next = freeBufInfo->firstFree;
         freeBufInfo->firstFree = bufHdr;
         break;
      }
      p= &freeBufInfo->next;
   }

   if(!freeBufInfo)
   {
      /* no free buffers list yet, ask OS for memory */
      freeBufInfo = (t_FreeBufInfo *)OS_GetMem(heap->mallocPoolHndle, sizeof(t_FreeBufInfo)+ALIGN);
      /* Can't create free list for this block because we're out of memory!
       * Better to continue and leak this block than to crash. */
      if (freeBufInfo == NULL)
      {
        L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_IP_MAP_COMPONENT_ID,
                "Unable to create free list for memory chunk of size %lu freed to routing heap.",
                size);
        XX_Thaw();
        return;
      }

#if ALIGN > 0
      ALIGN_ADDRESS(freeBufInfo, ALIGN, t_FreeBufInfo *);
#endif
      freeBufInfo->next = NULLP;
      bufHdr->u.bufHdr.next = NULLP;
      freeBufInfo->bufLen = bufHdr->u.bufHdr.size;
      freeBufInfo->firstFree = bufHdr;
      *p = freeBufInfo;
   }
   XX_Thaw();
}


/* Returns size of allocated data buffer pointed by buf */
ulng XX_SizeOfBuf(void *buf)
{
   t_BufHdr *bufHdr;

   ASSERT(memXXInit);
   ASSERT(buf);

   bufHdr = (t_BufHdr *)((byte *)buf - *((word *)buf - 1)) - 1;
   return bufHdr->u.bufHdr.size - MEMORY_OVERHEAD;
}

/* Check resources of the specified MALLOC heap */
e_Err XX_ChkResourcesExt(t_Handle Heap, word *status)
{
   t_Heap  *heap   = (t_Heap *)(Heap? Heap: DefaultHeapHandle);

   *status = (word) ((heap->freeListSize + OS_GetFreeMem(heap->mallocPoolHndle))*100 / heap->mallocPoolSize);
   return E_OK;
}


/* Display of the allocated through XX_Malloc memory*/

#define SIZE_CNT_SIZE 150

typedef struct tagt_SizeCnt
{
   ulng       size;
   ulng       cnt;
#ifdef MEMCHK
   ulng       reqSize;
   word       line;
   char *file;
#endif
} t_SizeCnt;

#ifdef WIN
static int _USERENTRY SizeCntCmp(const t_SizeCnt *e1, const t_SizeCnt *e2)
#else
static int SizeCntCmp(const void *e1, const void *e2)
#endif
{
   int res;

   res = ((t_SizeCnt *)e1)->size - ((t_SizeCnt *)e2)->size;
#ifdef MEMCHK
   if (res)
      return res;
   res = ((t_SizeCnt *)e1)->file - ((t_SizeCnt *)e2)->file;
   if (res)
      return res;
   return ((t_SizeCnt *)e1)->line - ((t_SizeCnt *)e2)->line;
#else
   return res;
#endif
}

#ifdef MEMCHK
static int FileLocCmp(const void *e1, const void *e2)
{
   int res;

   res = ((t_SizeCnt *)e1)->file - ((t_SizeCnt *)e2)->file;
   if (res)
      return res;
   return ((t_SizeCnt *)e1)->line - ((t_SizeCnt *)e2)->line;
}
#endif

#ifdef MEMCHK
e_Err FreeListInsert(t_FreeListEntry **freeBufList, t_BufHdr *bufHdr)
{
  t_FreeListEntry *freeListEntry;

  if (!freeBufList || !bufHdr)
    return E_FAILED;

  freeListEntry = (t_FreeListEntry*) osapiMalloc(L7_IP_MAP_COMPONENT_ID,
                                                 sizeof(t_FreeListEntry));
  if (!freeListEntry)
    return E_FAILED;
  freeListEntry->count = 1;
  freeListEntry->allocLine = bufHdr->u.bufHdr.allocLine;
  freeListEntry->allocFile = bufHdr->u.bufHdr.allocFile;  /* ok to alias */
  freeListEntry->freeLine = bufHdr->u.bufHdr.freeLine;
  freeListEntry->freeFile = bufHdr->u.bufHdr.freeFile;    /* ok to alias */
  freeListEntry->size = bufHdr->u.bufHdr.size;
  freeListEntry->reqSize = bufHdr->u.bufHdr.reqSize;
  freeListEntry->next = NULL;
  if (*freeBufList)
  {
    freeListEntry->next = *freeBufList;
  }
  *freeBufList = freeListEntry;
  return E_OK;
}

t_FreeListEntry *FreeListFind(t_FreeListEntry *freeBufList, t_BufHdr *bufHdr)
{
  t_FreeListEntry *freeListEntry;
  if (!freeBufList || !bufHdr)
    return NULL;

  freeListEntry = freeBufList;
  while (freeListEntry)
  {
    if ((freeListEntry->size == bufHdr->u.bufHdr.size) &&
        (freeListEntry->reqSize == bufHdr->u.bufHdr.reqSize) &&
        (freeListEntry->allocLine == bufHdr->u.bufHdr.allocLine) &&
        (freeListEntry->freeLine == bufHdr->u.bufHdr.freeLine) &&
        (!strcmp(freeListEntry->allocFile, bufHdr->u.bufHdr.allocFile)) &&
        (!strcmp(freeListEntry->freeFile, bufHdr->u.bufHdr.freeFile)))
      return freeListEntry;

    freeListEntry = freeListEntry->next;
  }
  return NULL;
}

#endif

/* Display the free list.
 * Set Heap to 0 to display the default heap.
 * Set detail to TRUE for more info. */
void XX_DisplayFreeList(t_Handle Heap, Bool detail)
{
  char prntBuf[256];
  word index;
  t_Heap   *heap = (t_Heap *)(Heap? Heap: DefaultHeapHandle);
  t_BufHdr *bufHdr;
  ulng bufferSize;
  ulng numFreeBufs;          /* number of free buffers of a given size */
  ulng numFreeTotal = 0;     /* total number of free buffers */
  ulng freeMemTotal = 0;     /* total memory on free list (bytes) */
  ulng overAlloc = 0;        /* overallocation for a single buffer size */
  ulng overAllocTotal = 0;   /* total overallocation due to chunk malloc */
  t_FreeBufInfo *freeBufInfo;

#ifdef MEMCHK
  /* For each freeBufInfo, create a list of freed buffers. Each element
  * on the list represents a unique allocation (allocated at same file/line
  * and freed at same file/line. t_XXOvrHead.sig is used as a count of the
  * number of buffers. List is created an printed for each buffer size.
  * Elements on the sort list are allocated using osapiMalloc(), since this
  * function may get called when the heap is exhausted. */
  t_FreeListEntry *freeBufList = NULL;    /* head of the list */
  t_FreeListEntry *freeListEntry;         /* a list element */
  t_FreeListEntry *nextEntry;
  char *allocFilename;
  char *freeFilename;
#endif

  XX_Print("\nFree list.");

  for (index = 0; index < XX_MALLOC_HT_SIZE; index++)
  {
    for (freeBufInfo = heap->xxMallocHT[index]; freeBufInfo; freeBufInfo = freeBufInfo->next)
    {
      bufferSize = freeBufInfo->bufLen;
      numFreeBufs = 0;
      overAlloc = 0;

      bufHdr = freeBufInfo->firstFree;
      while (bufHdr != NULLP)
      {
        if (bufHdr->u.bufHdr.size != bufferSize)
        {
          sprintf(prntBuf, "buffer size mismatch on free buf info list");
          XX_Print(prntBuf);
        }
#ifdef MEMCHK
        if (detail)
        {
          /* Add buffer to temporary free list */
          freeListEntry = FreeListFind(freeBufList, bufHdr);
          if (freeListEntry)
          {
            freeListEntry->count++;
          }
          else
          {
            FreeListInsert(&freeBufList, bufHdr);
          }
        }
#endif
        numFreeBufs++;
        freeMemTotal += (bufHdr->u.bufHdr.size + ALIGN);

#ifdef MEMCHK
        /* update overallocation stat */
        if (bufHdr->u.bufHdr.reqSize)
        {
          overAlloc += (bufHdr->u.bufHdr.size - bufHdr->u.bufHdr.reqSize);
        }
#endif
        bufHdr = bufHdr->u.bufHdr.next;
      }

      /* Print stats for this buffer size */
      if (numFreeBufs)
      {
        sprintf(prntBuf, "%lu free bufs of %lu bytes each. %lu bytes overalloc",
                numFreeBufs, bufferSize - MEMORY_OVERHEAD, overAlloc);
        XX_Print(prntBuf);
      }

      overAllocTotal += overAlloc;
      numFreeTotal += numFreeBufs;

#ifdef MEMCHK
      /* Print list of buffers of this size */
      if (detail)
      {
        freeListEntry = freeBufList;
        while (freeListEntry)
        {
          allocFilename = freeListEntry->allocFile;
          l7utilsFilenameStrip(&allocFilename);
          freeFilename = freeListEntry->freeFile;
          l7utilsFilenameStrip(&freeFilename);
          osapiSnprintf(prntBuf,256, " %lu a: (%s: %u), f: (%s: %u), ovr: %lu",
                  freeListEntry->count,
                  allocFilename, freeListEntry->allocLine,
                  freeFilename, freeListEntry->freeLine,
                  freeListEntry->reqSize ?
                  (freeListEntry->size - freeListEntry->reqSize) : 0);
          XX_Print(prntBuf);

          nextEntry = freeListEntry->next;
          osapiFree(L7_IP_MAP_COMPONENT_ID, freeListEntry);
          freeListEntry = nextEntry;
        }
        freeBufList = NULL;
      }
#endif
      if (numFreeBufs)
        XX_Print(" ");  /* new line */
    }
  }
  /* print accumulated stats */
  sprintf(prntBuf, "\nTotal memory on free list:  %lu", freeMemTotal);
  XX_Print(prntBuf);
  sprintf(prntBuf, "Total overhead on free list:  %lu (%lu%%)",
          (MEMORY_OVERHEAD + ALIGN) * numFreeTotal,
          ((MEMORY_OVERHEAD + ALIGN) * numFreeTotal * 100) / freeMemTotal);
  XX_Print(prntBuf);
  sprintf(prntBuf, "Total overallocation on free list:  %lu (%lu%%)",
          overAllocTotal, (overAllocTotal * 100) / freeMemTotal);
  XX_Print(prntBuf);
}

/*
 * debugging routine:
 * Shows the status of memory blocks currently allocated in the specified heap.
 * Information is gathered about memory blocks according to their size:
 * this is usually a good indication about what type of block they are.
 *
 * Each block allocated has a block header allocated with it. The size of the
 * block header is MEMORY_OVERHEAD. MEMORY_OVERHEAD is included in the "size"
 * field. However, there is an addition overhead for alignment of size ALIGN.
 * ALIGN is not included in the "size" field.
 *
 * Trace levels (level parameter) are defined as follows:
 *  1: summarized information only
 *  2: display number of allocated blocks of each size
 *  3: also display summary of free list
 *  4: also display free list detail
 */
void XX_DisplayMemExt(t_Handle Heap, byte level)
{
   char      prntBuf[128];
   word      arySize;
   ulng      computedSize;
   ulng      computedBlocks;
   t_BufHdr  *bh;
   t_SizeCnt *sc;
#ifdef MEMCHK
   t_SizeCnt *lastsc = 0;
   char *fileName;
#endif
   t_SizeCnt sizeCntAry[SIZE_CNT_SIZE];
   t_Heap   *heap = (t_Heap *)(Heap? Heap: DefaultHeapHandle);

   if(!memXXInit)
      return;

   arySize        = 0;
   computedSize   = 0;
   computedBlocks = 0;
   memset(sizeCntAry, 0, sizeof(sizeCntAry));

   XX_Freeze();

   for (bh = heap->allocList; bh != NULL; bh = bh->u.bufHdr.next)
   {
#ifdef MEMCHK
      ASSERT(bh->u.bufHdr.sig == ALLOC_SIGNATURE);
      ASSERT(GET_END_MARKER(bh) == ALLOC_SIGNATURE);
      ASSERT(bh->u.bufHdr.heap == heap);
#endif
      computedSize += bh->u.bufHdr.size;
      ++computedBlocks;

      for (sc = sizeCntAry; sc <= &sizeCntAry[SIZE_CNT_SIZE-1]; ++sc)
      {
         if (sc->size == 0)
         {
            sc->size = bh->u.bufHdr.size;   /* new entry */
#ifdef MEMCHK
            sc->reqSize = bh->u.bufHdr.reqSize;
            sc->file    = bh->u.bufHdr.allocFile;
            sc->line    = bh->u.bufHdr.allocLine;
#endif
            ++arySize;
         }
         /* intentionally not 'else' clause. Count first chunk of this size. */
         if (sc->size == bh->u.bufHdr.size
#ifdef MEMCHK
             && sc->file == bh->u.bufHdr.allocFile
             && sc->line == bh->u.bufHdr.allocLine
#endif
             )
         {
            ++sc->cnt;
            break;
         }
         if (sc > &sizeCntAry[SIZE_CNT_SIZE-1])
           XX_Print("Unable to report all allocations. Need to increase SIZE_CNT_SIZE.");
      }
   }

   XX_Thaw();

   /* Account for alignment overhead */
   computedSize += (computedBlocks * ALIGN);

   sprintf(prntBuf, "%lu in %lu allocations", computedSize, computedBlocks);
   XX_Print(prntBuf);
   sprintf(prntBuf, "Memory left on original heap:  %lu bytes", OS_GetFreeMem(heap->mallocPoolHndle));
   XX_Print(prntBuf);
   sprintf(prntBuf, "totalSize = %lu, freeListSize = %lu", heap->totalSize, heap->freeListSize);
   XX_Print(prntBuf);
   sprintf(prntBuf, "poolSize = %lu bytes, %lu%% alloc, %lu%% on freelist",
           heap->mallocPoolSize,
           (heap->totalSize*100)/(heap->mallocPoolSize),
           (heap->freeListSize*100)/(heap->mallocPoolSize));
   XX_Print(prntBuf);
   sprintf(prntBuf, "overhead on allocation list = %lu (%lu%% of allocated)",
           (MEMORY_OVERHEAD + ALIGN) * computedBlocks,
           (100 * (MEMORY_OVERHEAD + ALIGN) * computedBlocks) / computedSize);
   XX_Print(prntBuf);
   sprintf(prntBuf, "overAllocation = %lu (%lu%% of total)",
           heap->overAllocation, (heap->overAllocation * 100) / (heap->totalSize + heap->freeListSize));
   XX_Print(prntBuf);
   sprintf(prntBuf, "high water mark was: %lu\n", heap->hiWaterMark);
   XX_Print(prntBuf);

   if (level > 1)
   {
     sprintf(prntBuf, "Buffer sizes do not include overhead of %u bytes per buffer.\n",
             (L7_uint32) (MEMORY_OVERHEAD + ALIGN));
     XX_Print(prntBuf);

      qsort((void *)sizeCntAry, arySize, sizeof(t_SizeCnt), SizeCntCmp);

      for (sc = sizeCntAry; sc <= &sizeCntAry[SIZE_CNT_SIZE-1]; ++sc)
      {
         if (sc->size == 0 || sc->cnt == 0)
            break;
#ifdef MEMCHK
         fileName = sc->file;
         l7utilsFilenameStrip(&fileName);
         osapiSnprintf(prntBuf,128, "     %u of size %u (%s: %u)",
            (unsigned int) sc->cnt, (unsigned int)(sc->size - MEMORY_OVERHEAD), fileName, sc->line);
#else
         sprintf(prntBuf, "     %u of size %u",
            (unsigned int) sc->cnt, (unsigned int)(sc->size - MEMORY_OVERHEAD));
#endif
         XX_Print(prntBuf);
      }
   }

#ifdef MEMCHK
   /* check for incorrect usage of XX_Malloc */
   qsort((void *)sizeCntAry, arySize, sizeof(t_SizeCnt), FileLocCmp);

   for (sc = sizeCntAry; sc <= &sizeCntAry[SIZE_CNT_SIZE-1]; ++sc)
   {
      if (sc->size == 0 || sc->cnt == 0)
         break;

      if(lastsc && sc->reqSize == 0 &&
         sc->file == lastsc->file && sc->line == lastsc->line)
      {
        osapiSnprintf(prntBuf,128, "XX_Malloc misuse detected at (%s: %u)", sc->file, sc->line);
        XX_Print(prntBuf);
      }

      lastsc = sc;
   }
#endif

   /*****************************************************
    * Dump Freelist
    *****************************************************/
   if (level == 3)
     XX_DisplayFreeList(heap, 0);
   else if (level == 4)
     XX_DisplayFreeList(heap, 1);
}

/********* Internal RAM management */


 /*********************************************************************
 * @purpose         Perform initialization for the Internal RAM
 *
 *
 * @param Start     @b{(input)} start address of the RAM
 * @param Size      @b{(input)} RAM size
 * @param phInRAM   @b{(input)} Internal RAM handle (passed by reference)
 *
 * @returns         E_OK       - success
 * @returns         E_NOMEMORY - out of memory
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err XX_IRAM_Init(byte *Start, ulng Size, t_Handle *phInRAM)
{
   t_InRAM       *pInRAM;
   t_InRAMBlock  *first;

   /* memory allocation for the control structure */
   pInRAM = GetInRAMPoolHdr( );
   ASSERT(pInRAM);
   /* memory allocation for the first block == all Internal RAM */
   first = GetInRAMBlockHdr( );
   ASSERT(first);

   first->active   = FALSE;
   first->absAddr  = Start;
   first->algAddr = Start;
   first->next     = NULLP;
   first->size     = Size;

   pInRAM->startAddr  = Start;
   pInRAM->activeSize = 0;
   pInRAM->totalSize  = Size;
   pInRAM->head       = first;
   pInRAM->magic      = INRAM_MAGIC;

   *phInRAM = pInRAM;

   return E_OK;
}
/* !END XX_IRAM_Init */



 /*********************************************************************
 * @purpose          Allocate block of Intermal RAM
 *
 *
 * @param hInRAM     @b{(input)}  internal RAM Handle
 * @param Size       @b{(input)}  size of the block
 * @param Align      @b{(input)}  alignment (must be 2**n)
 *
 * @returns          Pointer of the allocated block on success
 * @returns          NULLP otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
#ifdef MEMCHK
void *_XX_IRAM_Alloc(t_Handle hInRAM, ulng Size, ulng Align, const char *file, word line)
#else
void *XX_IRAM_Alloc(t_Handle hInRAM, ulng Size, ulng Align)
#endif
{
   ulng           alg_delta = 0;
   t_InRAM        *pInRAM = (t_InRAM *)hInRAM;
   t_InRAMBlock   *new_block, *rval_block, *relevant = NULLP, *prev;

   ASSERT(pInRAM);
   ASSERT(Size);
   ASSERT(pInRAM->magic == INRAM_MAGIC);

   /* find a relevant memory block of the optimal size */
   relevant = opts_SearchB(pInRAM, Size, Align, &alg_delta, (t_Handle*)&prev);

   if ( !relevant )
      return NULLP;
   Size += alg_delta;

   /* memory block found with size exactly match the requested size */
   if ( relevant->size == Size )
   {
      relevant->active = TRUE;
      relevant->algAddr = relevant->absAddr + alg_delta;
      pInRAM->activeSize += Size;
      return relevant->algAddr;
   }
   /* The memory block found if of size bigger than requested.Split it.
      Insert the new memory block as allocated into the link list */
   new_block = GetInRAMBlockHdr( );

   if ( !new_block )
      return NULLP;
   new_block->active = TRUE;
   new_block->size   = Size;
   new_block->next   = relevant;
   new_block->absAddr   = relevant->absAddr;
   new_block->algAddr   = relevant->absAddr + alg_delta;
   rval_block = new_block;

   if ( alg_delta >= MIN_INRAMBLOCK_SIZE )
   {
      new_block = GetInRAMBlockHdr();

      if ( !new_block )
      {
         PutInRAMBlockHdr( rval_block );
         return NULLP;
      }

      new_block->active = FALSE;
      new_block->size   = alg_delta;
      new_block->next   = rval_block;
      new_block->absAddr   = rval_block->absAddr;
      new_block->algAddr   = rval_block->absAddr;

      rval_block->absAddr = rval_block->algAddr;
      rval_block->size -= alg_delta;
   }

   if ( prev )
      prev->next = new_block;
   else
      pInRAM->head = new_block;

   /* memory block has been split */
   relevant->absAddr += Size;
   relevant->algAddr  = relevant->absAddr;
   relevant->size    -= Size;

   pInRAM->activeSize += rval_block->size;

#ifdef MEMCHK
   rval_block->allocSign = INRAM_ALLOC_SIGN;
   rval_block->allocFile = file;
   rval_block->allocLine = line;
#endif

   return rval_block->algAddr;
}
/* !END XX_IRAM_Alloc */




 /*********************************************************************
 * @purpose           Free the Internal RAM block.
 *
 *
 * @param  hInRAM     @b{(input)}  Internal RAM Handle
 * @param  vFreeM     @b{(input)}  RAM address
 *
 * @returns           E_OK -         success
 * @returns           E_BADPARM -    invalid parameter/s
 * @returns           E_NOT_FOUND -  allocated memory block not found
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err XX_IRAM_Free(t_Handle hInRAM, void *vFreeM)
{
   byte           *FreeM = (byte *)vFreeM;
   t_InRAM        *pInRAM = (t_InRAM *)hInRAM;
   t_InRAMBlock   *to_free, *next, *prev = NULLP;

   ASSERT(pInRAM);
   ASSERT(FreeM);
   ASSERT(pInRAM->magic == INRAM_MAGIC);

   /* find a relevant memory block according to address in RAM */
   to_free = addr_SearchB(pInRAM, FreeM, (t_Handle *)&prev);
   ASSERT(to_free);
   ASSERT(to_free->active);
   pInRAM->activeSize -= to_free->size;
   to_free->active = FALSE;

#ifdef MEMCHK
   to_free->allocSign = 0;
   to_free->allocFile = NULLP;
   to_free->allocLine = 0;
#endif

   /* merge with free neighbors */
   if ( to_free->next && (to_free->next->active == FALSE) )
   {
      next            = to_free->next;
      to_free->size  += next->size;
      to_free->next   = next->next;
      PutInRAMBlockHdr( next );
   }
   if ( prev && (prev->active == FALSE) )
   {
      prev->size += to_free->size;
      prev->next = to_free->next;
      PutInRAMBlockHdr( to_free );
   }
   return E_OK;
}
/* !END XX_IRAM_Free */



 /*********************************************************************
 * @purpose         Reserve internal RAM block.
 *
 *
 * @param hInRAM    @b{(input)}  Internal RAM Handle
 * @param Start     @b{(input)}  block start address
 * @param Size      @b{(input)}  size of block
 *
 * @returns         Pointer of the reserved block on success
 * @returns         NULLP otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
#ifdef MEMCHK
void *_XX_IRAM_Reserve( t_Handle hInRAM, byte *Start, ulng Size,
                        const char *file, word line )
#else
void *XX_IRAM_Reserve( t_Handle hInRAM, byte *Start, ulng Size )
#endif
{
   ulng prev_delta = 0, next_delta = 0;
   t_InRAM        *pInRAM = (t_InRAM *)hInRAM;
   t_InRAMBlock   *new_prev, *new_next, *relevant;
   t_InRAMBlock   *prev = NULLP;

   ASSERT(pInRAM);
   ASSERT(Size);
   ASSERT(pInRAM->magic == INRAM_MAGIC);
   /* find a relevant memory block in link list according to address
      in RAM and necessary size */
   relevant = rsrv_SearchB( pInRAM, Start, Size, &prev_delta, &next_delta,
                            (t_Handle *)&prev);
   ASSERT(relevant);

   if ( !prev_delta && !next_delta )
   {
      relevant->active = TRUE;
      pInRAM->activeSize += Size;
      return (void *)(relevant->absAddr);
   }
   /* if address different between reserved block and relevant block != 0
      insert the new free memory blocks into the link list
      on front and behind to the reserved block */
   if ( prev_delta )
   {
      new_prev = GetInRAMBlockHdr( );
      ASSERT(new_prev);
      new_prev->active    = FALSE;
      new_prev->size      = prev_delta;
      new_prev->next      = relevant;
      new_prev->absAddr   = relevant->absAddr;
      new_prev->algAddr   = relevant->absAddr;
      if ( prev )
         prev->next = new_prev;
      else
         pInRAM->head = new_prev;
   }
   if ( next_delta )
   {
      new_next = GetInRAMBlockHdr( );
      ASSERT(new_next);
      new_next->active    = FALSE;
      new_next->size      = next_delta;
      new_next->next      = relevant->next;
      new_next->absAddr   = Start + Size;
      new_next->algAddr   = Start + Size;
      relevant->next = new_next;
   }
   /* reserved block == relevant block */
   relevant->active = TRUE;
   relevant->size = Size;
   relevant->absAddr = Start;
   relevant->algAddr = Start;

   pInRAM->activeSize += Size;

#ifdef MEMCHK
   relevant->allocSign = INRAM_ALLOC_SIGN;
   relevant->allocFile = file;
   relevant->allocLine = line;
#endif

   return relevant->absAddr;
}
/* !END XX_IRAM_Reserve */


/*==================== XX_IRAM_Delete ======================*/
/*
 *  Release all blocks and pool control structure
 *
 */
e_Err XX_IRAM_Delete(t_Handle phInRAM)
{
   t_InRAM      *pInRAM = (t_InRAM *)phInRAM;
   t_InRAMBlock *curr;

   ASSERT(pInRAM);
   ASSERT(pInRAM->magic == INRAM_MAGIC);

   curr = pInRAM->head;

   while(curr)
   {
      XX_Freeze();
      PutInRAMBlockHdr( curr );
      curr = curr->next;
      XX_Thaw();
   }
   PutInRAMPoolHdr(pInRAM);

   return E_OK;
}
/* !END XX_IRAM_Delete */

/*==================== XX_IRAM_Display ======================*/
/*
 *  Display all blocks of a some Internal RAM.
 *
 */
void XX_IRAM_Display(t_Handle hInRAM)
{
   word count = 0;
   char str[140];
   t_InRAM        *pInRAM = (t_InRAM *)hInRAM;
   t_InRAMBlock   *curr;

   ASSERT(pInRAM);
   ASSERT(pInRAM->magic == INRAM_MAGIC);

   sprintf(str,"======================================================");
   XX_Print(str);
   sprintf(str,"Internal RAM Handle = %p", (void *)pInRAM);
   XX_Print(str);
   sprintf( str,
            "StartAddr = %p, TotalSize = %lx, ActiveSize = %lx",
            (void *)pInRAM->startAddr,
            pInRAM->totalSize,
            pInRAM->activeSize);
   XX_Print(str);
   sprintf(str,"------------------------------------------------------");
   XX_Print(str);

   for ( curr = pInRAM->head; curr; curr = curr->next, count++ )
   {
      sprintf(str,
              "Block Number %d, Handle = %p, Activity = %d",
              count, (void *)curr, (word)curr->active);
      XX_Print(str);

      sprintf( str,
               "B_Size = %lx, B_Start = %p, B_AlignAddr = %p",
               curr->size,
               (void *)curr->absAddr,
               (void *)curr->algAddr);
      XX_Print(str);

#ifdef MEMCHK
      if ( curr->active )
      {
         osapiSnprintf( str,140,
                  "AllocSign = %x, AllocFile = %s, AllocLine = %d",
                  (unsigned int)curr->allocSign,
                  curr->allocFile,
                  curr->allocLine );
         XX_Print(str);
      }
#endif
      sprintf(str,"----------------------");
      XX_Print(str);
   }
}
/* !END XX_IRAM_Display */

/*================== addr_SearchB ======================*/
/*
 *  Find a memory block according to its address in RAM.
 *
 */
static t_InRAMBlock  *addr_SearchB( t_InRAM *pInRAM,
                                    byte *Start,
                                    t_Handle *phPrev )
{
   t_InRAMBlock *curr, *prev = NULLP;

   for ( curr = pInRAM->head; curr; curr = curr->next )
   {
      if ( curr->algAddr == Start )
      {
         *phPrev = (t_Handle)prev;
         break;
      }
      prev = curr;
   }
   return curr;
}
/* !END addr_SearchB */


/*================= opts_SearchB ======================*/
/*
 * Find a memory block according to optimal suitable size.
 *
 */
static t_InRAMBlock  *opts_SearchB( t_InRAM *pInRAM,
                                    ulng Size,
                                    ulng Align,
                                    ulng *pAlignDelta,
                                    t_Handle *phPrev )
{
   ulng offset;
   ulng  min_szdelta = pInRAM->totalSize;
   t_InRAMBlock   *curr, *prev = NULLP, *rval = NULLP;

   *phPrev = NULLP;

   for ( curr = pInRAM->head; curr; curr = curr->next )
   {
      if ( (curr->active == FALSE) && (Size <= curr->size) )
      {
         offset = (ulng)curr->absAddr & (Align-1);
         if( offset )
               offset = Align - offset;
         if ( ((Size + offset) <= curr->size) &&
              ((curr->size - (Size + offset)) < min_szdelta) )
         {
            min_szdelta = curr->size - (Size + *pAlignDelta);
            *pAlignDelta = offset;
            *phPrev = (t_Handle)prev;
            rval = curr;
            if ( !min_szdelta )
               break;
         }
      }
      if ( (curr == pInRAM->head)&&(!curr->next) )
         prev = NULLP;
      else
         prev = curr;
   }
   return rval;
}
/* !END opts_SearchB */


/*====================== rsrv_SearchB =======================*/
/*
 *  Find a memory block according to some start address in RAM
 *  and defined size (for XX_IRAM_Reserve).
 */
static t_InRAMBlock  *rsrv_SearchB( t_InRAM *pInRAM,
                                    byte *Start,
                                    ulng Size,
                                    ulng *pPrevDelta,
                                    ulng *pNextDelta,
                                    t_Handle *phPrev )
{
   t_InRAMBlock *curr, *prev = NULLP, *rval = NULLP;

   if ( (pInRAM->startAddr > Start) ||
        (Start >= (pInRAM->startAddr + pInRAM->totalSize)) )
      return NULLP;

   for ( curr = pInRAM->head; curr; curr = curr->next )
   {
      if ((curr->absAddr <= Start) &&
         (Start < (curr->absAddr + curr->size)) )
      {
         if ( (curr->active == FALSE) &&
              ((Start + Size) <= (curr->absAddr + curr->size)) )
         {
            *pPrevDelta = (ulng)Start - (ulng)(curr->absAddr);
            *pNextDelta = (ulng)(curr->absAddr + curr->size) -
                          (ulng)(Start + Size);
            *phPrev = (t_Handle)prev;
            rval = curr;
            break;
         }
         break;
      }
      if ( (curr == pInRAM->head)&&(!curr->next) )
         prev = NULLP;
      else
         prev = curr;
   }

   return rval;
}
/* !END rsrv_SearchB */


/* Get Internal RAM pool header                       */
/* Free header is taken from inRamFreeListHead list   */
static t_InRAM *GetInRAMPoolHdr( void )
{
   t_InRAM *pool;

   /* First call ? */
   if ( !inRamMngrInitialized )
   {
      word i;

      /* Initialize pools and blocks free lists      */
      /* Note that both static inRamPoolArray and    */
      /* inRamBlockArray are initially filled with 0s*/

      for( i=0; i<XX_MAX_INRAM_POOLS-1; i++ )
         inRamPoolArray[i].nextFree = &inRamPoolArray[i+1];
      inRamFreeListHead = &inRamPoolArray[0];

      /* Initialize InRAM free blocks list */
      for( i=0; i<XX_MAX_INRAM_BLOCKS-1; i++ )
         inRamBlockArray[i].nextFree = &inRamBlockArray[i+1];
      inRamBlockFreeListHead = &inRamBlockArray[0];

      inRamMngrInitialized = TRUE;
   }

   /* First try to allocate from free list */
   pool = inRamFreeListHead;
   if (pool)
   {
      inRamFreeListHead = pool->nextFree;
      pool->nextFree = NULLP;
   }
   else
   {  /* Free list is empty. Allocate from heap */
      if ( memXXInit )
         pool = (t_InRAM *)XX_Malloc( sizeof( t_InRAM ) );
   }

   return pool;

}

/* Return InRAM pool header */
static void PutInRAMPoolHdr( t_InRAM *poolHdr )
{
   poolHdr->nextFree = inRamFreeListHead;
   inRamFreeListHead = poolHdr;
}


/* Get Internal RAM block header                         */
/* Free header is taken from inRamBlockFreeListHead list */
static t_InRAMBlock *GetInRAMBlockHdr( void )
{
   t_InRAMBlock *block=inRamBlockFreeListHead;

   /* Allocate from free list. */
   /* If free list is empty - allocate from heap */
   if (block)
   {
      inRamBlockFreeListHead = block->nextFree;
      block->nextFree = NULLP;
   }
   else
   {
      ASSERT( inRamMngrInitialized );
      if ( memXXInit )
         block = (t_InRAMBlock *)XX_Malloc( sizeof( t_InRAMBlock ) );
   }

   return block;

}

/* Return InRAM block header */
static void PutInRAMBlockHdr( t_InRAMBlock *blockHdr )
{
   blockHdr->nextFree = inRamBlockFreeListHead;
   inRamBlockFreeListHead = blockHdr;
}


/*-******** Internal RAM management */
