/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dot1s_control.h
*
* @purpose   Multiple Spanning tree Control header
*
* @component dot1s
*
* @comments 
*
* @create    9/19/2002
*
* @author    skalyanam
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_DOT1S_CONTROL_H
#define INCLUDE_DOT1S_CONTROL_H

#include "dot1s_txrx.h"
#include "dot1s_sm.h"
#include "dot1q_api.h"



#define DOT1S_MGMT_EVENTS_BEGIN dot1sEvents_lastEvent

#define DOT1S_QUEUE_WEIGHT              1
#define DOT1S_STATE_CHANGE_QUEUE_WEIGHT 1

typedef enum dot1sMgmtEvents_s
{
  /***************************************************************/
  /* Events shared with all                                      */
  /***************************************************************/
  dot1sMgmtBegin = DOT1S_MGMT_EVENTS_BEGIN,

  /***************************************************************/
  /* Events from switch commands                                 */
  /***************************************************************/
  switchModeEnable,
  switchModeDisable,
  switchForceVersionSet,
  switchConfigFormatSelectorSet,
  switchConfigNameSet,
  switchConfigRevisionSet,
  switchUnused01,                    /* available for use */
  switchUnused02,                    /* available for use */
  switchPortAdminModeEnable,
  switchPortAdminModeDisable,
  switchPortStateSet,
  switchPortMigrationCheck,
  switchMacAddrChange,
  switchBpduGuardModeSet,
  switchBpduFilterModeSet,

  dot1sMgmtSwitchEvents, /*keep this last in sub group*/

  /***************************************************************/
  /* Events from common commands                                 */
  /***************************************************************/
  commonPrioritySet,
  commonBridgeMaxAgeSet,
  commonBridgeHelloTimeSet,
  commonBridgeFwdDelaySet,
  commonPortPathCostSet,
  commonPortPrioritySet,
  commonPortEdgePortEnable,
  commonPortEdgePortDisable,
  commonPortPointToPointMacSet,
  commonBridgeMaxHopSet,
  commonBridgeTxHoldCountSet,
  commonPortExtPathCostSet,
  commonPortAdminHelloTimeSet,
  commonPortBpduFilterModeSet,
  commonPortBpduFloodModeSet,
  commonPortRestrictedRoleSet,
  commonPortLoopGuardSet,  
  commonPortRestrictedTcnSet,
  commonPortAutoEdgeSet,


  dot1sMgmtCommonEvents, /*keep this last in sub group*/
  
  /***************************************************************/
  /* Events from instance commands                               */
  /***************************************************************/
  instanceCreate,
  instanceDelete,
  instancePrioritySet,
  instanceVlanAdd,
  instanceVlanRemove,
  instancePortPathCostSet,
  instancePortPrioritySet,
  instanceQueueSynchronize,

  dot1sMgmtInstanceEvents, /*keep this last in sub group*/
  
  /***************************************************************/
  /* Events from network pdu received                            */
  /***************************************************************/
  dot1sBpduReceived,

  dot1sNetworkEvents,
  
  /***************************************************************/
  /* Events from interface state changes                         */
  /***************************************************************/
  dot1sIntfChange,
  dot1sIntfStateSet,
  dot1sIntfStartup,
  dot1sStateChangeDone,

  dot1sIntfChangeEvents,

  /***************************************************************/
  /* Events due to error conditions                              */
  /***************************************************************/
  dot1sStateSetError,
  dot1sReconError,

  dot1sErrorEvents,

  /***************************************************************/
  /* Events from configurator                                    */
  /***************************************************************/
  dot1sCnfgr,

  dot1sCnfgrEvents

}dot1sMgmtEvents_t;

typedef struct
{
  DOT1S_CIST_PRIORITY_VECTOR_t 	priVec;
  DOT1S_CIST_TIMES_t		   	times;
  L7_uchar8						flag;
}DOT1S_CIST_MSG_t;

typedef struct
{
  DOT1S_MSTI_PRIORITY_VECTOR_t 	priVec;
  DOT1S_MSTI_TIMES_t			times;
  L7_uchar8						flag;

  /*CIST part of the bpdu that is required by each msti*/
  DOT1S_BRIDGEID_t				cistRootId;
  DOT1S_PATHCOST_t				cistExtRootPathCost;
  DOT1S_BRIDGEID_t				cistRegRootId;
  
}DOT1S_MSTI_MSG_t;

typedef struct
{
  L7_uint32 event;
  L7_uint32 intf;
  L7_uint32 instance;
  L7_uint32 timeStamp;
  union
  {
    L7_uint32        	p;    /* or key or priority or value */
    L7_BOOL        	    mode;
    L7_enetMacAddr_t 	mac;
    L7_uchar8        	state;
    L7_uchar8			*bpdu;
    L7_uchar8        	name[L7_DOT1S_MAX_CONFIG_NAME];
	DOT1S_CIST_MSG_t	cistMsg;
	DOT1S_MSTI_MSG_t	mstiMsg;
	L7_CNFGR_CMD_DATA_t CmdData;
	NIM_EVENT_COMPLETE_INFO_t status;
    NIM_STARTUP_PHASE_t phase;
    dot1s_stateCB_t stateStatus;
    L7_VLAN_MASK_t vlan_mask;
  }data;
}DOT1S_MSG_t;

typedef struct
{
  L7_ushort16 event;
  L7_ushort16 instance;
  L7_uint32 intf;
  L7_uint32 timeStamp;
  union
  {
    dot1s_stateCB_t stateStatus;
  }data;
}DOT1S_STATE_CHANGE_MSG_t;

typedef struct
{
  L7_uchar8 flag;
} DOT1S_SIGNAL_MSG_t;

typedef struct
{
  L7_uint32 msgId;
  L7_uint32 parm1;
  L7_uint32 parm2;

} dot1sTimerMsg_t;

#define DOT1S_MSG_SIZE   sizeof(DOT1S_MSG_t)
#define DOT1S_TIMER_TICK	(125) /*in milliseconds*/
#define DOT1S_TIMER_UNIT    (256)
#define DOT1S_TIMER_UNIT_PER_TICK (DOT1S_TIMER_UNIT/8) 
#define DOT1S_TICKS_PER_SECOND (1000/DOT1S_TIMER_TICK)
#define DOT1S_VALID_PORT_NUM	1

/***************************************************************************
 **************************FUNCTION PROTOTYPES******************************
 ***************************************************************************
 */
L7_RC_t dot1sIssueCmd(L7_uint32 event, 
              L7_uint32 intIfNum, 
              L7_uint32 instanceId, 
              void* data);
L7_RC_t dot1sDispatchCmd(DOT1S_MSG_t msg);/*the dot1s classifier may be renamed*/ 
L7_RC_t dot1sFillMsg(void *data, DOT1S_MSG_t *msg);             
L7_RC_t dot1sTimerAction();
L7_RC_t dot1sSwitchModeEnableSet(L7_uint32 mode);
L7_RC_t dot1sSwitchModeDisableSet(L7_uint32 mode);
L7_RC_t dot1sSwitchForceVerSet(L7_uint32 forceVer);
L7_RC_t dot1sSwitchCfgForSelSet(L7_uint32 formatSelector);
L7_RC_t dot1sSwitchPortMigrationCheck(L7_uint32 intIfNum);
L7_RC_t dot1sSwitchRevSet(L7_uint32 switchRevision);
L7_RC_t dot1sSwitchPortAdminModeEnableSet(L7_uint32 intIfNum);
L7_RC_t dot1sSwitchPortAdminModeDisableSet(L7_uint32 intIfNum);
L7_RC_t dot1sSwitchPortEnableSet(L7_uint32 intIfNum);
L7_RC_t dot1sSwitchPortDisableSet(L7_uint32 intIfNum);
L7_RC_t dot1sSwitchPortStateSet(L7_uint32 intIfNum);
L7_RC_t dot1sSwitchMacAddrChange();
L7_RC_t dot1sCommonPriSet(L7_uint32 instId, L7_uint32 priority);
L7_RC_t dot1sCommonBridgeMaxAgeSet(L7_uint32 maxAge);
L7_RC_t dot1sCommonBridgeMaxHopSet(L7_uint32 maxHop);
L7_RC_t dot1sCommonBridgeTxHoldCountSet(L7_uint32 holdCount);
L7_RC_t dot1sCommonBridgeHelloTimeSet(L7_uint32 helloTime);
L7_RC_t dot1sCommonBridgeFwdDelaySet(L7_uint32 fwdDelay);
L7_RC_t dot1sCommonPortPathCostSet(L7_uint32 intIfNum, L7_uint32 instId,L7_uint32 pathCost);
L7_RC_t dot1sCommonPortPriSet(L7_uint32 intIfNum, L7_uint32 instId,L7_uint32 portPri);
L7_RC_t dot1sCommonPortEdgePortEnable(L7_uint32 intIfNum, L7_uint32 instId,L7_uint32 edgeMode);
L7_RC_t dot1sCommonEdgePortDisable(L7_uint32 intIfNum, L7_uint32 instId,L7_uint32 edgeMode);
L7_RC_t dot1sCommonPortRestrictedRoleSet(L7_uint32 intIfNum, L7_uint32 instId,L7_BOOL mode);
L7_RC_t dot1sCommonPortLoopGuardSet(L7_uint32 intIfNum, L7_uint32 instId,L7_BOOL mode);
L7_RC_t dot1sCommonPortRestrictedTcnSet(L7_uint32 intIfNum, L7_uint32 instId,L7_uint32 mode);
L7_RC_t dot1sCommonPortAutoEdgeSet(L7_uint32 intIfNum, L7_uint32 instId,L7_uint32 mode);
L7_RC_t dot1sInstanceAvailIndexFind(L7_uint32 *index);
L7_RC_t dot1sApplyCfgInstanceCreate(L7_uint32 instId);
L7_RC_t dot1sUserInstanceCreate(L7_uint32 instId);
L7_RC_t dot1sInstanceCreate(L7_uint32 instIndex, L7_uint32 instId);
L7_RC_t dot1sInstanceDelete(L7_uint32 instId);
L7_RC_t dot1sInstancePortPriSet(L7_uint32 intIfNum, L7_uint32 instId, L7_uint32 portPri);
L7_RC_t dot1sSwitchCfgNameSet(L7_uchar8	*name);
L7_RC_t dot1sApplyCfgInstanceVlanAdd(L7_uint32 instId, L7_uint32 vlanId);
L7_RC_t dot1sUserInstanceVlanAdd(L7_uint32 instId, L7_uint32 vlanId);
L7_RC_t dot1sInstanceVlanAdd(L7_uint32 instId, L7_uint32 vlanId);
L7_RC_t dot1sInstanceVlanRemove(L7_uint32 instId, L7_uint32 vlanId);
L7_RC_t dot1sInstanceVlanRangeAdd(L7_uint32 instId, L7_VLAN_MASK_t *vlan_mask);
L7_RC_t dot1sInstanceVlanRangeRemove(L7_uint32 instId,L7_VLAN_MASK_t *vlan_mask);

L7_RC_t dot1sInstancePortPathCostSet(L7_uint32 intIfNum, L7_uint32 instId, L7_uint32 pathCost);
L7_RC_t dot1sInstancePriSet(L7_uint32 instId, L7_uint32 pri);
L7_RC_t dot1sStateMachineRxBpdu(L7_uint32 intIfNum, DOT1S_MSTP_ENCAPS_t 	*bpdu);
void dot1s_task();
void dot1s_timer_task();
L7_RC_t dot1sVlanChangeCallback(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, L7_uint32 event);
L7_RC_t dot1sInstanceVlanDelete(L7_uint32 instId, L7_uint32 vlanId);
void dot1sBridgeIdEndianSafe(DOT1S_BRIDGEID_t *bridgeID, L7_uchar8 *buf);
L7_RC_t dot1sPortAdminHelloTimeSet(L7_uint32 instId, L7_uint32 intIfNum, L7_uint32 helloTime);
L7_RC_t dot1sPortExtPathCostSet(L7_uint32 instId, L7_uint32 intIfNum, L7_uint32 pathCost);
L7_RC_t dot1sPortStateMachineUpTimeReset(DOT1S_PORT_COMMON_t *p);
L7_RC_t dot1s_task_signal(void);


void dot1sQueueSyncSemCreate (void);
void dot1sQueueSyncSemGet (void);
void dot1sQueueSyncSemFree (void);
void dot1sQueueSyncSemDelete (void);
L7_RC_t dot1sSwitchBpduGuardModeSet(L7_BOOL val);
L7_RC_t dot1sSwitchBpduFilterModeSet(L7_BOOL val);
L7_RC_t dot1sCommonPortBpduFilterModeSet(L7_uint32 intIfNum, L7_uint32 instId,L7_BOOL val);
L7_RC_t dot1sCommonPortBpduFloodModeSet(L7_uint32 intIfNum, L7_uint32 instId,L7_BOOL val);

#endif /* INCLUDE_DOT1S_CONTROL_H*/
