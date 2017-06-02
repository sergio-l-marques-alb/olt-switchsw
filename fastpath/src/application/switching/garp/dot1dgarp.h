/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    dot1dgarp.h
* @purpose     GARP Wrapper functions
* @component   GARP
* @comments    none
* @create      02/23/2001
* @author      Hassan
* @author
* @end
*
**********************************************************************/
/*********************************************************************
 *
 ********************************************************************/
/******************************************************************************
 *
 * MODULE: dot1dgarp.h
 * Date: April 18, 1999
 *
 * DESCRIPTION: GARP Wrapper Class Header File
 *
 * SPEC STANDARD: 802.1D-1998
 *
 *****************************************************************************/

#ifndef INCLUDE_DOT1D_GARP_H
#define INCLUDE_DOT1D_GARP_H

#include "garpctlblk.h"
#include "l7_product.h"
#include "l7_cnfgr_api.h"
#include "nimapi.h"
#include "dot1q_api.h"
#define GARP_QUEUE      "garpQueue"
#define GARP_PDU_QUEUE  "garpPduQueue"



typedef enum
{
  DISABLE_GARP,
  ENABLE_GARP
} GarpStateTable;

typedef enum
{
  G_COMMAND = 1,
  G_PORT_CONN,
  G_PORT_DISCONN,
  G_PDU,
  G_TIMER,
  G_LEAVEALL_TIMER,
  G_INTF_STATE_CHANGE,
  G_NIM_STARTUP,
  G_CNFGR,
  G_VLAN_NOTIFY,
  G_DOT1S_NOTIFY

} garpMsgId_t;


typedef enum
{
  LEAVE_EXPIRED,
  GMRP_LEAVE_EXPIRED,

  JOIN_EXPIRED,
  GMRP_JOIN_EXPIRED,

} garpTimer_t;


typedef struct garpNimNotifyData_s
{
    NIM_CORRELATOR_t nim_correlator;
    L7_uint32        nim_event;
} garpNimNotifyData_t;


typedef struct garpNimStartup_s
{
  NIM_STARTUP_PHASE_t startupPhase;
} garpNimStartup_t;

typedef struct garpPduData_s
{
    L7_uint32       vlan_id;     /* vlan id */
    L7_netBufHandle bufHandle;   /* buffer hanle */
} garpPduData_t;


typedef struct garpDot1sNotifyData_s
{
    L7_uint32     mstID;       /* Multiple Spanning Tree ID - only for dot1sSupport*/
    L7_uint32     event;

} garpDot1sNotifyData_t;


typedef struct garpDot1qNotifyData_s
{
    dot1qNotifyData_t vlanData;
    L7_uint32       event;
    L7_ushort16     gvrp_event;

} garpDot1qNotifyData_t;


typedef struct
{
  garpMsgId_t     msgId;       /* message type */
  GARPPort        port;        /* intIfNum, also GPDU's arriving port */

  union
  {
     garpNimNotifyData_t    nim;
     garpNimStartup_t       startup;
     garpDot1sNotifyData_t  dot1s;
     garpDot1qNotifyData_t  dot1q;
     GARPCommand            command;     /* used for G_COMMAND types */
     L7_CNFGR_CMD_DATA_t    CmdData;


  } msgData;



} GarpPacket;
typedef struct
{
  garpMsgId_t     msgId;       /* message type */
  GARPPort        port;        /* intIfNum, also GPDU's arriving port */

  union
  {
     garpPduData_t          pdu;
  } msgData;

} GarpPduPacket;

typedef struct
{
   L7_BOOL gvrp_join_in;
} failed_to_enqueue_timers_t;

/*********************************************************************
* @purpose  Create Garp
*
* @param    void
*
* @returns  GARP_SUCCESS
* @returns  GARP_FAILURE
*
* @notes
*
* @end
*********************************************************************/
extern GARPStatus GarpCreate(void *);

/*********************************************************************
* @purpose  Send a message to the Garp_Task to process the incomming packet.
*
* @param    bufHandle   buffer handle to the bpdu received
* @param    intIfNum    interface number
*
* @returns  L7_SUCCESS  if the message was put on the queue
* @returns  L7_FAILURE  if the message was not put on the queue
*
* @notes    void
*
* @end
*********************************************************************/
extern L7_RC_t GarpSendPDU(L7_netBufHandle bufHandle, L7_uint32 intIfNum, L7_uint32 vlan_id);

/*********************************************************************
* @purpose  Join timer expiry function.
*
* @param    Garp          application
* @param    L7_uint32     port number
*
* @returns  none
*
* @notes    Put this timer expiry message on a timer queue, thus returning
*           immediately to the timer task that called this routine. This will
*           avoid any processing on the timer thread.
*
* @end
*********************************************************************/
extern void gid_join_timer_expired_to_queue(L7_uint32 parm1, L7_uint32 port_no);
extern void gid_gmr_join_timer_expired_to_queue(L7_uint32 vid, L7_uint32 port_no);

/*********************************************************************
* @purpose  leave timer expiry function.
*
* @param    Garp          application
* @param    L7_uint32     port number
*
* @returns  none
*
* @notes    Put this timer expiry message on a timer queue, thus returning
*           immediately to the timer task that called this routine. This will
*           avoid any processing on the timer thread.
*
* @end
*********************************************************************/
extern void gid_leave_timer_expired_to_queue(L7_uint32 parm1, L7_uint32 port_no);
extern void gid_gmr_leave_timer_expired_to_queue(L7_uint32 vid, L7_uint32 port_no);

/*********************************************************************
* @purpose  Process Join Time modification
*
* @param (in) intIfNum  intIfNum
* @param (in) join_time join time value
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
extern void garpJoinTimeProcess(L7_uint32 intIfNum, L7_uint32 join_time);


/*********************************************************************
* @purpose  Process Leave Time modification
*
* @param (in) intIfNum  intIfNum
* @param (in) join_time join time value
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
extern void garpLeaveTimeProcess(L7_uint32 intIfNum, L7_uint32 leave_time);

/*********************************************************************
* @purpose  Process Leave All Time modification
*
* @param (in) intIfNum  intIfNum
* @param (in) join_time join time value
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
extern void garpLeaveAllTimeProcess(L7_uint32 intIfNum, L7_uint32 leaveall_time);

/*********************************************************************
* @purpose  Apply GARP application Mode
*
* @param    application  @b{(input)} GARP_GVRP_APP or  GARP_GMRP_APP
* @param    mode         @b{(input)} GARP_ENABLE or GARP_DISABLE
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Invoked for L7_DETACH
*
* @end
*********************************************************************/
L7_RC_t garpGarpModeProcess(GARPApplication application, L7_uint32 mode);

#endif


