/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\l7public\porting\usmdb_snmp_trap_api.h
*
* @purpose Provide interface to SNMP Trap API's for unitmgr components
*
* @component unitmgr
*
* @comments tba
*
* @create 01/11/2001
*
* @author cpverne
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#ifndef USMDB_SNMP_TRAP_API_H
#define USMDB_SNMP_TRAP_API_H

#include "l7_common.h"
#include "usmdb_snmp_trap_api_stacking.h"

/* Begin Function Prototypes */
/*********************************************************************
* @purpose  Gets the current status of Trap master flag
*
* @param    UnitIndex      The unit for this operation
* @param    val   Status of Trap flag (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpEnableTrapsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the current status of Trap master flag
*
* @param    L7_uint32 UnitIndex      The unit for this operation
* @param    val   Status of Trap flag (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpEnableTrapsSet(L7_uint32 val);

/*********************************************************************
* @purpose  Gets the current status of the Authentication Trap
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    val   Status of Authentication Trap flag (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpEnableAuthenTrapsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the current status of the Authentication Trap
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    val   Status of Authentication Trap flag (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpEnableAuthenTrapsSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Send ColdStart Trap via SNMP Agent
*
* @param    UnitIndex   L7_uint32 the unit for this operation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpColdStartTrapSend(L7_uint32 UnitIndex);

/*********************************************************************
* @purpose  Send WarmStart Trap via SNMP Agent
*
* @param    UnitIndex   L7_uint32 the unit for this operation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpWarmStartTrapSend(L7_uint32 UnitIndex);

/*********************************************************************
* @purpose  Send LinkDown Trap via SNMP Agent
*
* @param    UnitIndex   L7_uint32 the unit for this operation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpLinkDownTrapSend(L7_uint32 UnitIndex, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Send LinkUp Trap via SNMP Agent
*
* @param    UnitIndex   L7_uint32 the unit for this operation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpLinkUpTrapSend(L7_uint32 UnitIndex, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Send AuthenFailure Trap via SNMP Agent
*
* @param    UnitIndex   L7_uint32 the unit for this operation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpAuthenFailureTrapSend(L7_uint32 UnitIndex);


/*********************************************************************
* @purpose  Send MultipleUsers Trap via SNMP Agent
*
* @param    UnitIndex   L7_uint32 the unit for this operation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpMultipleUsersTrapSend(L7_uint32 UnitIndex);

/*********************************************************************
* @purpose  Send BCastStormStart Trap via SNMP Agent
*
* @param    UnitIndex   L7_uint32 the unit for this operation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpBroadcastStormStartTrapSend(L7_uint32 UnitIndex);

/*********************************************************************
* @purpose  Send BCastStormEnd Trap via SNMP Agent
*
* @param    UnitIndex   L7_uint32 the unit for this operation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpBroadcastStormEndTrapSend(L7_uint32 UnitIndex);


/*********************************************************************
* @purpose  Send LinkFailure Trap via SNMP Agent
*
* @param    UnitIndex   L7_uint32 the unit for this operation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpLinkFailureTrapSend(L7_uint32 UnitIndex);


/*********************************************************************
* @purpose  Send VLAN request failed Trap via SNMP Agent
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    vlanId      L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpVlanRequestFailureTrapSend(L7_uint32 UnitIndex, L7_uint32 vlanID);


/*********************************************************************
* @purpose  Send last,default VLAN deleted Trap via SNMP Agent
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    vlanId      L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpVlanDeleteLastTrapSend(L7_uint32 UnitIndex, L7_uint32 vlanID);


/*********************************************************************
* @purpose  Send default cfg VLAN failure Trap via SNMP Agent
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    vlanId   L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpVlanDefaultCfgFailureTrapSend(L7_uint32 UnitIndex, L7_uint32 vlanID);

/*********************************************************************
* @purpose  Send VLAN restore failure Trap via SNMP Agent
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    vlan Id     L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpVlanRestoreFailureTrapSend(L7_uint32 UnitIndex, L7_uint32 vlanID);

/*********************************************************************
* @purpose  Send FanFailure Trap via SNMP Agent
*
* @param    UnitIndex   L7_uint32 the unit for this operation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpFanFailureTrapSend(L7_uint32 UnitIndex);

/*********************************************************************
*
* @purpose Send a MAC Locking Violation Trap via SNMP Agent.
*
* @param   L7_uint32 intIfNum
*
* @returns L7_SUCCESS, if success
* @returns L7_ERROR, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpMacLockViolationTrapSend(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Send Power Supply Status Change Trap via SNMP Agent
*
* @param    UnitIndex   L7_uint32 the unit for this operation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpPowerSupplyStatusChangeTrapSend(L7_uint32 UnitIndex);

/*********************************************************************
* @purpose  Send NewRoot Trap via SNMP Agent
*
* @param    UnitIndex   L7_uint32 the unit for this operation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpNewRootTrapSend(L7_uint32 UnitIndex);

/*********************************************************************
* @purpose  Send TopologyChange Trap via SNMP Agent
*
* @param    UnitIndex   L7_uint32 the unit for this operation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpTopologyChangeTrapSend(L7_uint32 UnitIndex);

/*********************************************************************
*
* @purpose Send a TopologyChange Trap for an STP instance via SNMP Agent.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param   instanceId   STP instance id
*
* @returns L7_SUCCESS, if success
* @returns L7_ERROR, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpStpInstanceTopologyChangeTrapSend(L7_uint32 UnitIndex, L7_uint32 instanceId);

/*********************************************************************
*
* @purpose Send a NewRoot Trap for an STP instance via SNMP Agent.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param   instanceId   STP instance id
*
* @returns L7_SUCCESS, if success
* @returns L7_ERROR, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpStpInstanceNewRootTrapSend(L7_uint32 UnitIndex, L7_uint32 instanceId);

/*********************************************************************
*
* @purpose Send a LoopInconsistentStart Trap via SNMP Agent.
*
* @param    UnitIndex     L7_uint32 the unit for this operation
* @param    instanceId    MSTP instance id
* @param    intIfNum      intIfNum of link which go into inconsistent state.
*
* @returns L7_SUCCESS, if success
* @returns L7_ERROR, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t 
usmDbSnmpStpInstanceLoopInconsistentStartTrapSend(L7_uint32 UnitIndex, L7_uint32 instanceId, L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose Send a LoopInconsistentEnd Trap via SNMP Agent.
*
* @param    UnitIndex     L7_uint32 the unit for this operation
* @param    instanceId    MSTP instance id
* @param    intIfNum      intIfNum of link which return from inconsistent state.
*
* @returns L7_SUCCESS, if success
* @returns L7_ERROR, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t 
usmDbSnmpStpInstanceLoopInconsistentEndTrapSend(L7_uint32 UnitIndex, L7_uint32 instanceId, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Send Rising Alarm Trap via SNMP Agent
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param     alarmIndex  L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpRisingAlarmTrapSend(L7_uint32 UnitIndex, L7_uint32 alarmIndex);

/*********************************************************************
* @purpose  Send Falling Alarm Trap via SNMP Agent
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param     alarmIndex  L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpFallingAlarmTrapSend(L7_uint32 UnitIndex, L7_uint32 alarmIndex);

/*********************************************************************
* @purpose  Send Card Mismatch Trap via SNMP Agent
*
* @param    unitIndex       The unit for this operation
* @param    slotIndex       The slot for this operation
* @param    insertedType    The slot for this operation
* @param    configuredType  The slot for this operation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpAgentInventoryCardMismatchTrapSend(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 insertedType, L7_uint32 configuredType);

/*********************************************************************
* @purpose  Send Card Unsupported Trap via SNMP Agent
*
* @param    unitIndex       The unit for this operation
* @param    slotIndex       The slot for this operation
* @param    insertedType    The slot for this operation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpAgentInventoryCardUnsupportedTrapSend(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 insertedType);

/******************************************************************************
* @purpose  Send Card Plugin Trap via SNMP Agent
*
* @param    unitIndex       The unit for this operation
* @param    slotIndex       The slot for this operation
* @param    insertedType    The slot for this operation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
******************************************************************************/
L7_RC_t
usmDbSnmpAgentInventoryCardPluginTrapSend(L7_uint32 unitIndex,
                                          L7_uint32 slotIndex,
                                          L7_uint32 insertedType);

/******************************************************************************
* @purpose  Send Card Unplug Trap via SNMP Agent
*
* @param    unitIndex       The unit for this operation
* @param    slotIndex       The slot for this operation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
******************************************************************************/
L7_RC_t
usmDbSnmpAgentInventoryCardUnplugTrapSend(L7_uint32 unitIndex,
                                          L7_uint32 slotIndex);

/******************************************************************************
* @purpose  Send Card Failure Trap via SNMP Agent
*
* @param    unitIndex       The unit for this operation
* @param    slotIndex       The slot for this operation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
******************************************************************************/
L7_RC_t
usmDbSnmpAgentInventoryCardFailureTrapSend(L7_uint32 unitIndex,
                                           L7_uint32 slotIndex);
/*********************************************************************
*
* @purpose  Send Stack Port Link Up Trap via SNMP Agent
*
* @param    agentInventoryStackPortIndex  Stack Port Index
* @param    agentInventoryStackPortUnit   Unit Index of the Unit the Stack Port is on
* @param    agentInventoryStackPortTag    Tag associated with that Stack Port
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if error
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpAgentInventoryStackPortLinkUpTrapSend(L7_uint32 agentInventoryStackPortIndex, L7_uint32 agentInventoryStackPortUnit, L7_char8 *agentInventoryStackPortTag);

/*********************************************************************
*
* @purpose  Send Stack Port Link Down Trap via SNMP Agent
*
* @param    agentInventoryStackPortIndex  Stack Port Index
* @param    agentInventoryStackPortUnit   Unit Index of the Unit the Stack Port is on
* @param    agentInventoryStackPortTag    Tag associated with that Stack Port
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if error
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpAgentInventoryStackPortLinkDownTrapSend(L7_uint32 agentInventoryStackPortIndex, L7_uint32 agentInventoryStackPortUnit, L7_char8 *agentInventoryStackPortTag);

/*********************************************************************
* @purpose  Send Entity Config Change Trap via SNMP Agent
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpEntConfigChangeTrapSend();

/*********************************************************************
* @purpose  Send User Login failure Trap via SNMP Agent
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpFailedUserLoginTrapSend();


/*********************************************************************
* @purpose  Send DHCP SNOOPING Error Disabled on an interface Trap via SNMP Agent
*
* @param    intIfNum    internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpDsIntfErrorDisabledTrapSend(L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Send User Login lockout Trap via SNMP Agent
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpLockedUserLoginTrapSend();

/*********************************************************************
* @purpose  Send DAI Error Disabled on an interface Trap via SNMP Agent
*
* @param    intIfNum    internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpDaiIntfErrorDisabledTrapSend(L7_uint32 intIfNum);

#ifdef FEAT_METRO_CPE_V1_0
/*********************************************************************
*
* @purpose Send a No Startup Config Trap via SNMP Agent.
*
* @param L7_uint32 UnitIndex      The unit for this operation
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpNoStartupConfigTrapSend(L7_uint32 UnitIndex);
#endif

/* End Function Prototypes */
#ifdef L7_CHASSIS
/*********************************************************************
* @purpose  Send Chassis Alarm Raise Trap via SNMP Agent
* 
* @param    alarmIndex  Alarm Index
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpChassisRisingAlarmTrapSend(L7_uint32 alarmIndex);

/*********************************************************************
* @purpose  Send Chassis Alarm Clear Trap via SNMP Agent
*
* @param    alarmIndex  Alarm Index
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpChassisFallingAlarmTrapSend(L7_uint32 alarmIndex);
#endif 

/*********************************************************************
* @purpose  Send Temperature Change Alarm Trap via SNMP Agent
*
* @param    sensorNum  Number of sensor
* @param    tempEventType  Event type
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpTemperatureChangeTrapSend(L7_uint32 sensorNum, L7_uint32 tempEventType);

/*********************************************************************
* @purpose  Send Fan State Change Alarm Trap via SNMP Agent
*
* @param    itemNum  Number of fan
* @param    fanEventType  Event type
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpFanStateChangeTrapSend(L7_uint32 itemNum, L7_uint32 fanEventType);

/*********************************************************************
* @purpose  Send Power Supply State Change Alarm Trap via SNMP Agent
*
* @param    itemNum  Number of power supply
* @param    powSupplyEventType  Event type
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpPowSupplyStateChangeTrapSend(L7_uint32 itemNum, L7_uint32 powSupplyEventType);

#ifdef L7_DOT1AG_PACKAGE
/*********************************************************************
* @purpose  Send Dot1ag CFM Defect Notification Trap
*
* @param    mdIndex   @b{(input)} MD whose FNGSM is reporting a defect
* @param    maIndex   @b{(input)} MA whose FNGSM is reporting a defect
* @param    mepId     @b{(input)} MEP whose FNGSM is reporting a defect
* @param    fngDefect   @b{(input)} Defect to be reported
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpDot1agCfmFaultAlarmTrapSend(L7_uint32 mdIndex, L7_uint32 maIndex, L7_uint32 mepId, L7_uint32 highestPriDefect);
#endif /* L7_DOT1AG_PACKAGE */

#endif /* USMDB_SNMP_TRAP_API_H */

