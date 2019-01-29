#ifndef TRACE_H_
#define TRACE_H_
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename    trace.h
*
* @purpose     File contains defines needed for a trace utility implimentation
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


#include "trace_api.h"

#define TRACE_MIN_ENTRY_SIZE  8
#define TRACE_BASE_POWER      2

#define TRACE_START_MARKER   0xBBBB
#define TRACE_END_MARKER     0xEEEE

typedef struct traceControl_s
{
  traceData_t               *traceFirst;           /*link to the first trace*/
  traceData_t               *traceLast;            /*link to the last trace*/
  L7_uint32                 traceCount;            /*number of allocated traces*/
  L7_uint32                 id;/*allways increment to generate a unique id*/
} traceControl_t;

/* Internal data-structure used for combining the component traces into one sorted system trace */
typedef struct
{
  L7_BOOL        valid;
  L7_uint32      numEnt; /* running count of entries displayed */
  L7_uint32      totalEnt; /* Total entries in the trace */
  traceData_t   *traceData; 
  L7_uchar8     *pEntry; /* Pointer to current entry */
} compTraceInfo_t;

/*internal Prototypes*/

/*********************************************************************
* @purpose  stop tracing into the trace for all the traces 
*          
* @param 		none
*	
* @returns  none
*
* @notes    puts a string of 0xF for easy identification
*           to be used via devshell 
*
* @end
*
*********************************************************************/
void traceStopAll();

/*********************************************************************
* @purpose  activate the trace block for all the traces
*          
* @param 		none
*	
* @returns  none
*
* @notes    puts a series of 0xB in the trace file for easy identification
*           to be used via devshell
* @end
*
*********************************************************************/
void traceStartAll();
/*********************************************************************
* @purpose  dump on screen the trace
*          
* @param 		traceId
*	
* @returns  none
*
* @notes    to be used via devshell
*           
* @end
*
*********************************************************************/
void traceDump(L7_uint32 traceId);



void traceKickStart1();
void traceKickStart2();









#endif /*TRACE_H_*/
