/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************

* @filename  garp_debug.h
*
* @purpose   GARP debug functions
*
* @component GARP
*
* @comments  This file contains all the debug routines for GARP
*
* @create   10/11/2005
*
* @author    W. Jacobs
*
* @end
*             
**********************************************************************/

#ifndef __GARP_DEBUG_H__
#define __GARP_DEBUG_H__

                
#include "l7_common.h"
#include <stdarg.h>
#include <stdio.h>
#include "log.h"
#include "gidapi.h"
#include "garpapi.h"
#include "garpctlblk.h"
#include "dot1dgarp.h" 

/* for debug */
/* for tracking applicant and registrar transitions for specified vlan*/ 

/* for tracking applicant and registrar transitions for all vlans*/ 
#define L7_GARP_DEBUG_MAX_EVNT_CNT 40  /* count of number of transitions*/
                                        /*to be tracked for a single vlan */
#define L7_MAX_VLAN_TRACK 5 /* max number of vlans to track*/

typedef enum
{
  GVRP_APP=1,   /* event for applicant machine*/
  GVRP_REG      /* event for registrar machine */
}gvrpType;

typedef struct garpTransitions_s
{
  Gid_event event;
  L7_uchar8 val; /* stores applicant or resgistrar value */
} garpTransitions_t;

typedef struct garpVlanTransitions_s
{
  garpTransitions_t gvrpApplicantTrans[L7_GARP_DEBUG_MAX_EVNT_CNT];
  garpTransitions_t gvrpRegistrarTrans[L7_GARP_DEBUG_MAX_EVNT_CNT];
  L7_uint32 nAppEvtCnt;
  L7_uint32 nRegEvtCnt;
  L7_BOOL bVlanCreated;
  L7_uint32 nAppEvtEx;
  L7_uint32 nRegEvtEx;

} garpVlanTransitions_t;

#define GARP_QUEUE_USAGE_FIFTY_PERCENT  (GARP_MSG_COUNT/2)
#define GARP_QUEUE_USAGE_EIGHTY_PERCENT ( (GARP_MSG_COUNT*8) /10 )
#define GARP_QUEUE_USAGE_NINETY_PERCENT ( (GARP_MSG_COUNT*9) /10 )

/* Trace any message put on or received from the garp queue  */
#define GARP_TRACE_QUEUE_EVENTS(__fmt__, __args__... )             \
          garpTraceEvents(GARP_TRACE_QUEUE, __fmt__, ## __args__)

/* Internal tracing allows for more specific tracing of non-queue transitions.
  For example, tracing g*r_proc_command, which are invoked via many sources */
#define GARP_TRACE_INTERNAL_EVENTS(__fmt__, __args__... )           \
          garpTraceEvents(GARP_TRACE_INTERNAL, __fmt__, ## __args__)

/* Trace any timer event for GARP */
#define GARP_TRACE_TIMER_EVENTS(__fmt__, __args__... )           \
          garpTraceEvents(GARP_TRACE_TIMERS, __fmt__, ## __args__)
        
#define GARP_TRACE_PROTOCOL_EVENTS(__fmt__, __args__... )           \
          garpTraceEvents(GARP_TRACE_PROTOCOL,__fmt__, ## __args__)

  

/* Prototypes */

void garpDebugHelp(void);
void garpDebugGvrpStateInfo(L7_ushort16 no_of_attr_to_print, L7_ushort16 intIfNum);
void garpDebugGmrpStateInfo(L7_ushort16 no_of_attr_to_print, L7_ushort16 intIfNum, 
                            Vlan_id     vlan_id);
void garpDebugGvrpLeaveAllTimerInfo(L7_uint32 intIfNum);
void garpDebugGmrpLeaveAllTimerInfo(L7_uint32 intIfNum);
void garpDebugGvdRecordsInfo();
void garpDebugMsgQueueShow(void);
void garpDebugMsgQueueCountsClear(void);
void garpDebugTraceModePrint(void);
L7_RC_t garpDebugTraceModeSet(L7_uint32 mode);
L7_RC_t garpDebugTraceModeClear(L7_uint32 mode);
void gvrp_state_info(L7_ushort16 no_of_attr_to_print, L7_ushort16 port_no);
void gmrp_state_info(L7_ushort16 no_of_attr_to_print, 
                     L7_ushort16 port_no, 
                     Vlan_id     vlan_id);
void garp_timer_info(L7_uint32 port_no);
void gmrp_timer_info(L7_uint32 port_no);
void gvd_records_info(); 
void garpDebugTraceModeHelp(void);
void gvd_records_info();
L7_BOOL garpDebugTraceModeGet(L7_uint32 traceFlag);
void garpTraceWrite(L7_uchar8 *traceMsg);
void garpTraceQueueEvents(L7_char8 * format, ...);
void garpTraceEvents(L7_uint32 event_flag, L7_char8 * format, ...);
void garpTraceProcessCommand(GARPCommand *cmd);
void garpTraceProcessGvrCommand(GARPCommand *cmd);
void garpTraceProcessGmrCommand(GARPCommand *cmd);
void garpTraceProcessCommandNameFormat(GARPCommand *cmd, L7_uchar8 *traceCommandName);
void garpTraceProcessCommandAppNameFormat(GARPCommand *cmd, L7_uchar8 *traceGarpApp);
void garpTraceMsgQueueUsage(void);
void garpTraceQueueMsgSend(GarpPacket *msg);
void garpTraceQueueMsgRecv(GarpPacket *msg);
        

/* for tracking specified vlans*/
void garpDebugTrackVlanRecord(L7_uchar8 app_state,Gid_event event,gvrpType type);
void garpDebugTrackVlanSet(Vlan_id vlanId);
void garpDebugTrackVlanReset();

/* Other debug routines */
void garpDebugGvrpGidShow(L7_uint32 intIfNum);
void garpDebugGmrpGidShow(L7_uint32 vlanId,L7_uint32 intIfNum);
void garpDebugGidShow(Gid *my_port);
void garpDebugGarpStatusShow(void);
void garpDebugIntfCountersShow(L7_uint32 intIfNum);
void garpDebugGmrpDot1sInstanceShow(void);
void garpDebugDot1sInstanceShow(L7_uint32 dot1sInstanceId);
void garpDebugShowAll(void);

#endif /* __GARP_DEBUG_H__ */
