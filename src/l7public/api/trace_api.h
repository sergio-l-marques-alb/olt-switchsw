#ifndef TRACE_API_H_
#define TRACE_API_H_
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename    trace_api.h
*
* @purpose     File contains defines and prototypes 
*              needed for a trace utility implimentation
*
* @component 
*
* @comments 
*
* @create      7/5/2001
*
* @author      skalyanam
* @end
*
**********************************************************************/
/*************************************************************
                    
*************************************************************/



#include <string.h>
#include "commdefs.h"
#include "datatypes.h"
#include "osapi.h"

typedef struct traceData_s
{
  L7_uchar8               traceName[15+1];        /*name of the trace*/
  L7_uint32               traceId;                /*id associated with this trace*/
  L7_uchar8               *head;                  /* first line of trace area   */
  L7_uchar8               *tail;                  /* last line of trace area    */
  L7_uchar8               *next;                  /* next entry line to write   */
  L7_uchar8               *nextByte;              /* working ptr within entry   */

  L7_ulong32              entryCount;             /*number of trace entries made*/
  L7_BOOL                 active;                 /* flag: trace is active      */
  L7_BOOL                 initialized;            /* Has the trace been init'd? */
  L7_uint32               numOfEntries;           /*number of entries in this trace*/
  L7_uint32               bytesPerEntry;          /*number of bytes per entry must be power of 2*/
  void                    *semaId;                /*semaphore handler*/

  struct traceData_s      *nextTrace;             /*link to the next trace*/
  struct traceData_s      *prevTrace;             /*link to the prev trace*/
  L7_uchar8               pad[4];                 /*padding for alignment for 8 bytes*/
} traceData_t;


/* 
**  Generic header used by all trace points
*/
typedef struct
{
  L7_uchar8         handle;                     /* 00   : trace buffer handle */
  L7_uchar8         compId;                     /* 01   : component id        */
  L7_ushort16       traceId;                    /* 02-03: trace id code       */
  L7_ulong32        timeStamp;                  /* 04-07: entry timestamp     */
} compTracePtHdr_t;

/* System trace registration prototypes and data-structure */

#define SYS_TRACE_DISABLE          0
#define SYS_TRACE_GLOBAL_EVENTS    1
#define SYS_TRACE_ALL_EVENTS       2


typedef void (*compTraceModeSetFunc_t)(L7_uint32 mode);
typedef void (*compTraceFmtFunc_t)(L7_ushort16 traceId, L7_uint32 timeStamp, L7_uchar8 *pDataStart);

typedef struct
{
  L7_uint32                compTraceHandle; /* Trace handle of the component */
  L7_BOOL                  enable;          /* Whether to register or unregister */
  compTraceModeSetFunc_t   compTraceModeSet; /* Function to enable/disable the component trace */
  compTraceFmtFunc_t       compTraceFmtFunc; /* Display function for the component trace */
} compTraceRegistrationInfo_t;

#define SYS_PROFILE_DISABLE          0
#define SYS_PROFILE_GLOBAL_EVENTS    1
#define SYS_PROFILE_ALL_EVENTS       2

typedef void (*compProfileModeSetFunc_t)(L7_uint32 mode);
typedef void (*compProfileInfoShowFunc_t)();

typedef struct
{
  L7_BOOL                    enable;          /* Whether to register or unregister */
  compProfileModeSetFunc_t   compProfileModeSet; /* Function to enable/disable the component trace */
  compProfileInfoShowFunc_t  compProfileInfoShow; /* Display function for the component trace */
} compProfileRegistrationInfo_t;

/*API Prototypes*/


/*********************************************************************
* @purpose  This function creates and initializes a trace block
*          
* @param 		numOfEntries  @b{(input)}   number of entries in the trace
* @param 		bytesPerEntry @b{(input)}   number of bytes per entry
* @param    *traceName    @b{(input)}   the name for this trace
* @param    traceId       @b{(output)}  the trace ID with which to uniquely identify this trace
*	
* @returns  L7_SUCCESS - on a successful creation of the trace
* @returns  L7_FAILURE - if any operation failed
*
* @notes    bytesPerEntry must be to the power of 2, minimum 8 bytes
*           the trace is initialized. if bytesPerEntry is not a power of 2
*           the closest larger power of two is substituted for bytesPerEntry
*           Header information is a part of the trace block, the head and tail
*           pointers are appropriately  assigned, to the begining and end of 
*           the actual trace data area.
*           Allow two bytes for traceId(to identify a component) and 
*           traceCode(to be used for internal modules of a components).
*           Calculate the bytes Per Entry to include these two bytes. 
*
* @end
*
*********************************************************************/
EXT_API L7_RC_t traceBlockCreate(L7_uint32 numOfEntries, 
                                 L7_uint32 bytesPerEntry, 
                                 L7_uchar8 *traceName,
                                 L7_uint32 *traceId);


/*********************************************************************
* @purpose  locates a trace block in the list of trace blocks
*          
* @param 		traceId @b{(input)}   the trace ID associated with this trace
*	
* @returns  the traceData_t pointer of the found trace block
* @returns  L7_NULLPTR  if the trace was not found
*
* @notes none
*
* @end
*
*********************************************************************/
EXT_API traceData_t *traceBlockFind(L7_uint32 traceId) ;

/*********************************************************************
* @purpose  removes a trace block from the list and frees memory allocated for it
*          
* @param 		traceId @b{(input)} the trace id associated with it
*	
* @returns L7_SUCCESS if the trace was successfully removed
* @returns L7_FAILURE if the trace block was not found
*
* @notes    all memory associated with this trace block is freed on 
*           a successful operation
*
* @end
*
*********************************************************************/
EXT_API L7_RC_t traceBlockDelete(L7_uint32 traceId);

/*********************************************************************
* @purpose  activate the trace block
*          
* @param 		traceId @b{(input)} the trace id associated with it
*	
* @returns  L7_SUCCESS on a successful completion of the begin
* @returns  L7_FAILURE if the trace was not found
*
* @notes    puts a series of 0xB in the trace file for easy identification
*
* @end
*
*********************************************************************/
EXT_API L7_RC_t traceBlockStart(L7_uint32 traceId);

/*********************************************************************
* @purpose  stop tracing into the trace 
*          
* @param 		traceId @b{(input)} the trace id associated with it
*	
* @returns  L7_SUCCESS if the trace was successfully stoped
* @returns  L7_FAILURE if the trace block ws not found
*
* @notes    puts a string of 0xF for easy identification 
*
* @end
*
*********************************************************************/
EXT_API L7_RC_t traceBlockStop(L7_uint32 traceId);



/*********************************************************************
* @purpose  the following are the macros that write into the trace area allocated 
*          
* @param 		traceId @b{(input)} the trace id associated with it
* @param    *traceData @b{(input)} the pointer to a traceData_t structure
* @param    traceCode @b{(input)} the code for easy identification of this trace entry
*	
* @returns  none
*
* @notes    USAGE: the macro TRACE_BEGIN must precede any actual trace entry byte,word
*                  or dword and the TRACE_END must follow the last entry
*                  the above two macros form a bracket for the inner macros
*                  typically between the TRACE_BEGIN and TRACE_END there may be 
*                  one or more macros of the byte and/or word and/or dword
*           When entries which are less than the bytesPerEntry are written the remaining bytes
*           are zeroed out and the next entry will begin with an offset of bytesPerEntry
*           from the previous entry's begining byte.
*           BYTE  - 1 byte
*           2BYTE - 2 bytes
*           4BYTE - 4 bytes 
*
* @end
*
*********************************************************************/

#define TRACE_BEGIN( traceId, traceCode)                                      \
  {                                                                           \
    traceData_t *traceData = traceBlockFind((traceId));                       \
    if(traceData != L7_NULLPTR)                                               \
    {                                                                         \
      if ( traceData->active)                                                 \
      {                                                                       \
        if (osapiSemaTake(traceData->semaId,L7_WAIT_FOREVER) == L7_SUCCESS)   \
        {                                                                     \
          traceData->nextByte = traceData->next;                              \
          *(L7_uchar8 *)traceData->nextByte = (L7_uchar8)((traceId));         \
          traceData->nextByte += sizeof(L7_uchar8);                           \
          *(L7_uchar8 *)traceData->nextByte = (L7_uchar8)((traceCode));       \
          traceData->nextByte += sizeof(L7_uchar8);


#define TRACE_ENTER_BYTE(Item )                                               \
          *(L7_uchar8 *)traceData->nextByte = (Item);                         \
          traceData->nextByte += sizeof(L7_uchar8);


#define TRACE_ENTER_2BYTE(Item )                                              \
          *(L7_ushort16 *)traceData->nextByte = (Item);                       \
          traceData->nextByte += sizeof(L7_ushort16);


#define TRACE_ENTER_4BYTE(Item )                                              \
          *(L7_ulong32 *)traceData->nextByte = (Item);                        \
          traceData->nextByte += sizeof(L7_ulong32);

#define TRACE_ENTER_BLOCK(Item, Size)                                         \
          memcpy(traceData->nextByte, Item, Size);                            \
          traceData->nextByte += Size;


#define TRACE_END()                                                           \
          if ((L7_uint32)(traceData->nextByte - traceData->next) <            \
               traceData->bytesPerEntry)                                      \
          {                                                                   \
            memset(traceData->nextByte,0x00,                                  \
                   (size_t)(traceData->nextByte - traceData->next));          \
          }                                                                   \
          traceData->next = traceData->next + traceData->bytesPerEntry;       \
          if ( traceData->next >= traceData->tail )                           \
          {                                                                   \
            traceData->next = traceData->head;                                \
          }                                                                   \
          traceData->entryCount++;                                            \
          (void)osapiSemaGive(traceData->semaId);                             \
        }                                                                     \
      }                                                                       \
    }                                                                         \
  } /* close trace block */

/*********************************************************************
* @purpose  Initialize the system trace/profile manager
*
* @param    none
*
* @returns  none
*
* @notes    Any FASTPATH component can register it's trace/profile routines
*           with this utility. When sysTraceShow is invoked, the trace of 
*           all the registered components is shown sorted by timestamp.
*
*
* @end
*********************************************************************/
void sysTraceProfileMgrInit();

/*********************************************************************
* @purpose Register the trace utility of a component with sysTrace
*
* @param    none
*
* @returns  none 
*
* @end
*********************************************************************/
void sysTraceRegisterComponent(L7_COMPONENT_IDS_t cid, compTraceRegistrationInfo_t regInfo);

/*********************************************************************
* @purpose Register the profile utility of a component with sysProfile
*
* @param    none
*
* @returns  none 
*
* @end
*********************************************************************/
void sysProfileRegisterComponent(L7_COMPONENT_IDS_t cid, compProfileRegistrationInfo_t regInfo);




#endif /*TRACE_API_H_*/
