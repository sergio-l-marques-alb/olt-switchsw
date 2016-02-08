/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dot1s_debug.h
*
* @purpose   Multiple Spanning tree debug utilities
*
* @component dot1s
*
* @comments 
*
* @create    12/02/2002
*
* @author    spetriccione
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_DOT1S_DEBUG_H
#define INCLUDE_DOT1S_DEBUG_H

#include "dot1s_include.h"

typedef struct 
{
  L7_uchar8   handle;  /* 00   : trace buffer handle */
  L7_uchar8   compId;  /* 01   : component id        */
  L7_ushort16 traceId;
  L7_uint32   time;
  L7_uint32   task;   
  /*Header End*/
  L7_uint32   depth;
  L7_uint32   event;
  L7_uint32   portNum;
  L7_uint32   instIndex;
}dot1sEventsTrace_t; 

/***************************************************************************
 **************************FUNCTION PROTOTYPES******************************
 ***************************************************************************
 */
/* Flag to store the Debug paramters */
extern L7_uint32 dot1sDebug ;
extern L7_uint32 dot1sDebugMsgs; /* Working flag to display selected statements */
extern L7_uint32 debugInstIndex;

extern L7_BOOL dot1sDebugPacketTraceTxFlag;
extern L7_BOOL dot1sDebugPacketTraceRxFlag;

#define DOT1S_USER_TRACE_TX(__fmt__, __args__... )                              \
          if (dot1sDebugPacketTraceTxFlag == L7_TRUE)                               \
          {                                                                   \
          LOG_USER_TRACE(L7_DOT1S_COMPONENT_ID, __fmt__,##__args__);        \
          }

#define DOT1S_USER_TRACE_RX(__fmt__, __args__... )                              \
          if (dot1sDebugPacketTraceRxFlag == L7_TRUE)                               \
          {                                                                   \
          LOG_USER_TRACE(L7_DOT1S_COMPONENT_ID, __fmt__,##__args__);        \
          }

#define DOT1S_DEBUG_TEST(args...) {\
    if (dot1sDebugMsgs == L7_TRUE) \
    {\
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,args);\
    }\
}

#define DOT1S_DEBUG_MSG(args...) SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,args)

/*#define DOT1S_DEBUG_MSG(args...) LOG_MSG(args)*/ 


 
#define DOT1S_DEBUG_NONE          0x00000000
    /*DOT1S_DEBUG_PROTO_CLASS */
#define DOT1S_DEBUG_PROTO_HANDSHAKE         ((L7_uint32)(1 << 0)) 
#define DOT1S_DEBUG_PROTO_ROLE              ((L7_uint32)(1 << 1)) 
#define DOT1S_DEBUG_PROTO_DISPUTE  			((L7_uint32)(1 << 2))
#define DOT1S_DEBUG_PROTO_SYNC              ((L7_uint32)(1 << 3))

	/*DOT1S_DEBUG_RCVD_CLASS */
#define DOT1S_DEBUG_RCVD_BPDU               ((L7_uint32)(1 << 4)) 
#define DOT1S_DEBUG_RCVD_FLAGS              ((L7_uint32)(1 << 5)) 
#define DOT1S_DEBUG_RCVD_INTERNAL  			((L7_uint32)(1 << 6))
#define DOT1S_DEBUG_RCVD_INFO  			    ((L7_uint32)(1 << 7))
#define DOT1S_DEBUG_RCVD_INFO_DETAIL        ((L7_uint32)(1 << 8))

	/*DOT1S_DEBUG_TIMERS_CLASS */
#define DOT1S_DEBUG_TIMER_EXPIRY  		    ((L7_uint32)(1 << 9))
#define DOT1S_DEBUG_TIMER_EXPIRY_DETAIL     ((L7_uint32)(1 << 10))

	/*DOT1S_DEBUG_TC_CLASS */
#define DOT1S_DEBUG_RCVD_TC  		        ((L7_uint32)(1 << 11))
#define DOT1S_DEBUG_DETECTED_TC             ((L7_uint32)(1 << 12))
#define DOT1S_DEBUG_FLUSH                   ((L7_uint32)(1 << 13))

	/*DOT1S_DEBUG_STATE_CLASS */
#define DOT1S_DEBUG_STATE_CHANGE            ((L7_uint32)(1 << 14))
/* Place Holder                             ((L7_uint32)(1 << 15)) */
   /*DOT1S_DEBUG_DETAIL_CLASS,   */
#define DOT1S_DEBUG_DESIG_PRT               ((L7_uint32)(1 << 16))
#define DOT1S_DEBUG_ROOT_PRT                ((L7_uint32)(1 << 17))
#define DOT1S_DEBUG_MASTER_PRT              ((L7_uint32)(1 << 18))
#define DOT1S_DEBUG_ALTBKUP_PRT             ((L7_uint32)(1 << 19))

#define DOT1S_DEBUG_PIM                     ((L7_uint32)(1 << 20))

#define DOT1S_DEBUG_BDM                     ((L7_uint32)(1 << 21))
#define DOT1S_DEBUG_PPM                     ((L7_uint32)(1 << 22))

	/*DOT1S_DEBUG_MIGRATION_CLASS */
#define DOT1S_DEBUG_STATE_MACHINE           ((L7_uint32)(1 << 23))

/*DOT1S_DEBUG_MSG_PROC_TIME*/
#define DOT1S_DEBUG_MSG_PROC_TIME           ((L7_uint32)(1<< 24)) /* flag set to track time spent */ 
                                                                  /* by last 10 recived messages in queue*/ 
                                                                  /* and time spent in processing last 10 BPDUS*/

 

/* This macro takes the first parameter as the flag and second as the inst index to be debugged
*/
#define DOT1S_DEBUG(a,b) ((dot1sDebug & (a)) && ((b) == debugInstIndex))
#define DOT1S_DEBUG_COMMON(a) (dot1sDebug & (a)) 

#define DOT1S_DEBUG_FLAG_PRINT(flag) {\
    if (dot1sDebug & (flag)) \
    {\
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s : L7_TRUE\n",#flag);\
    }\
}

#define DOT1S_EVENTS_TRACE_NAME "dot1s_trace.txt"
#define DOT1S_EVENTS_TRACE_ENTRIES_MAX 1024
#define DOT1S_EVENTS_TRACEID 0

#define DOT1S_THRESHOLD_STATEMACHINE_EVENTS_DEPTH 100 
#define DOT1S_STATEMACHINE_EVENTS_BUF 50

/*   TRACE_ID(2) | TIMESTAMP(4) | TASKID(4)
     | DEPTH(4) | EVENT(4)   | PORT NUM(4) |  INST INDEX(4) |
         
*/
#define DOT1S_DEBUG_EVENTS_TRACE_WIDTH 26
#define DOT1S_DEBUG_EVENTS_DISPLAY_BUF 128


void dot1sDebugHelp();
L7_RC_t dot1sDebugInit();
void dot1sSmStates(L7_uint32 intIfNum, L7_uint32 instIndex);
void dot1sTraceIdGet();
void dot1sSizesShow();
void dot1sMsgQueue();
void dot1sPortStates(L7_uint32 intIfNum);
void dot1sDebugFlagsSet(L7_uint32 debugFlag, L7_uint32 instIndex);
void dot1sDebugPacketRxTrace(L7_uint32 intIfNum, DOT1S_MSTP_ENCAPS_t *pdu);
void dot1sDebugPacketTxTrace(L7_uint32 intIfNum, DOT1S_MSTP_ENCAPS_t *pdu);

/* Dot1s debug events trace functions*/
L7_RC_t dot1sDebugEventsTraceFini(void);
void dot1sDebugEventsTraceStart(L7_uint32 numEvents);
L7_RC_t dot1sDebugEventsTraceShow(L7_BOOL writeToFlash, L7_BOOL unFormatted);
L7_RC_t dot1sDebugEventsTraceStop();
L7_RC_t dot1sDebugEventsTrace(DOT1S_PORT_COMMON_t *p, dot1sEvents_t dot1sEvent, L7_uint32 instIndex, L7_uint32 depth);


void dot1sDebugPrintQueueTime();
void dot1sDebugPrintMsgProcTime();

#endif /* INCLUDE_DOT1S_DEBUG_H*/
