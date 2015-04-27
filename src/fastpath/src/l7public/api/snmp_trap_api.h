/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: snmp_trap_api.h
*
* Purpose: API interface for SNMP Agent Traps
*
* Created by: Colin Verne 02/16/2001
*
* Component: SNMP
*
*********************************************************************/
/*************************************************************
                    
*************************************************************/



#ifndef SNMP_TRAP_API_H
#define SNMP_TRAP_API_H

/*********************************************************************
* @purpose  Registers base traps with trapmgr
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
void SnmpTrapRegistrationBase(void);

/*********************************************************************
* @purpose  Registers customer traps with trapmgr
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
void SnmpTrapRegistrationCustomer(void);

/*********************************************************************
* @purpose  Sets the status of any stored trap configurations
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t
SnmpInitializeTraps();

/*********************************************************************
* @purpose  Gets the current status of the Authentication Trap
*
* @param    val   Status of Authentication Trap flag (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t
SnmpEnableAuthenTrapsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the current status of the Authentication Trap
*
* @param    val   Status of Authentication Trap flag (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t
SnmpEnableAuthenTrapsSet(L7_uint32 val);

/*********************************************************************
*
* @purpose  Send a Cold Start Trap
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
SnmpColdStartTrapSend ();

/*********************************************************************
*
* @purpose  Send a Warm Start Trap
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
SnmpWarmStartTrapSend ();

/*********************************************************************
*
* @purpose  Send a Link Down Trap
*
* @param    ifIndex     External ifIndex of link that went down.
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
SnmpLinkDownTrapSend ( L7_uint32 ifIndex );


/*********************************************************************
*
* @purpose  Send Authentication failure
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
SnmpAuthenFailureTrapSend ();


/*********************************************************************
*
* @purpose  Send FanFailure Trap
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
SnmpFanFailureTrapSend();

/*********************************************************************
*
* @purpose  Send MAC Locking violation Trap
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
SnmpMacLockViolationTrapSend(L7_uint32 intIfNum);


/*********************************************************************
*
* @purpose  Send Power Supply Trap
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
SnmpPowerSupplyStatusChangeTrapSend();

/*********************************************************************
*
* @purpose  Send VlanRestoreFailure Trap
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
SnmpVlanRestoreFailureTrapSend(L7_int32 dot1qVlanIndex);

/*********************************************************************
*
* @purpose  Send VlanDefaultCfgFailure Trap
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
SnmpVlanDefaultCfgFailureTrapSend(L7_int32 dot1qVlanIndex);



/*********************************************************************
*
* @purpose  Send VlanDeleteLast Trap
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
SnmpVlanDeleteLastTrapSend(L7_int32 dot1qVlanIndex);



/*********************************************************************
*
* @purpose  Send LinkFailure Trap
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
SnmpLinkFailureTrapSend();


/*********************************************************************
*
* @purpose  Send BroadcastStormEnd Trap
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
SnmpBroadcastStormEndTrapSend();


/*********************************************************************
*
* @purpose  Send BroadcastStormStart Trap
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
SnmpBroadcastStormStartTrapSend();


/*********************************************************************
*
* @purpose  Send MultipleUsers Trap
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
SnmpMultipleUsersTrapSend();


/*********************************************************************
*
* @purpose  Send FallingAlarm Trap
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
SnmpFallingAlarmTrapSend( L7_int32 alarmIndex);


/*********************************************************************
*
* @purpose  Send RisingAlarm Trap
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
SnmpRisingAlarmTrapSend ( L7_int32 alarmIndex);



/*********************************************************************
*
* @purpose  Send TopologyChange Trap
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
SnmpTopologyChangeTrapSend();


/*********************************************************************
*
* @purpose  Send NewRoot Trap
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
SnmpNewRootTrapSend();

/*********************************************************************
*
* @purpose  Send TopologyChange Trap for an STP instance
*
* @param    instanceID  the STP instance ID
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
SnmpStpInstanceTopologyChangeTrapSend(L7_int32 instanceID);

/*********************************************************************
*
* @purpose  Send agentInventoryCardMismatch Trap
*
* @param    unitIndex       the unit identifier
* @param    slotIndex       the slot identifier
* @param    insertedType    the card type inserted into this slot
* @param    configuredType  the card type configured for this slot
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
SnmpAgentInventoryCardMismatchTrapSend(L7_int32 unitIndex, L7_int32 slotIndex, L7_int32 insertedType, L7_int32 configuredType);

/*********************************************************************
*
* @purpose  Send agentInventoryCardUnsupported Trap
*
* @param    unitIndex       the unit identifier
* @param    slotIndex       the slot identifier
* @param    insertedType    the card type inserted into this slot
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
SnmpAgentInventoryCardUnsupportedTrapSend(L7_int32 unitIndex, L7_int32 slotIndex, L7_int32 insertedType);

/*********************************************************************
*
* @purpose  Send agentInventoryStackPortLinkUp Trap
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
SnmpAgentInventoryStackPortLinkUpTrapSend(L7_uint32 agentInventoryStackPortIndex, L7_uint32 agentInventoryStackPortUnit, L7_char8 *agentInventoryStackPortTag);

/*********************************************************************
*
* @purpose  Send agentInventoryStackPortLinkDown Trap
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
SnmpAgentInventoryStackPortLinkDownTrapSend(L7_uint32 agentInventoryStackPortIndex, L7_uint32 agentInventoryStackPortUnit, L7_char8 *agentInventoryStackPortTag);

/*********************************************************************
*
* @purpose  Send NewRoot Trap for an STP instance
*
* @param    instanceID  the STP instance ID
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
SnmpStpInstanceNewRootTrapSend(L7_int32 instanceID);

/*********************************************************************
*
* @purpose  Send LoopInconsistentStart Trap 
*
* @param    instanceID  the MSTP instance ID
* @param    ifIndex     External ifIndex of link which go into inconsistent state.
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
SnmpStpInstanceLoopInconsistentStartTrapSend(L7_int32 instanceID, L7_uint32 ifIndex);

/*********************************************************************
*
* @purpose  Send LoopInconsistentEnd Trap 
*
* @param    instanceID  the MSTP instance ID
* @param    ifIndex     External ifIndex of link which return from inconsistent state.
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
SnmpStpInstanceLoopInconsistentEndTrapSend(L7_int32 instanceID, L7_uint32 ifIndex);

/*********************************************************************
*
* @purpose  Send a Link Up Trap
*
* @param    ifIndex     External ifIndex of link that came up.
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
SnmpLinkUpTrapSend ( L7_uint32 ifIndex );

/*********************************************************************
*
* @purpose  Send Authentication failure
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
SnmpAuthenFailureTrapSend ();

/*********************************************************************
*
* @purpose  Send agentStackingCardMismatch Trap
*
* @param    unitIndex       the unit identifier
* @param    slotIndex       the slot identifier
* @param    insertedType    the card type inserted into this slot
* @param    configuredType  the card type configured for this slot
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
SnmpAgentStackingCardMismatchTrapSend(L7_int32 unitIndex, L7_int32 slotIndex, L7_int32 insertedType, L7_int32 configuredType);

/*********************************************************************
*
* @purpose  Send agentStackingCardUnsupported Trap
*
* @param    unitIndex       the unit identifier
* @param    slotIndex       the slot identifier
* @param    insertedType    the card type inserted into this slot
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
SnmpAgentStackingCardUnsupportedTrapSend(L7_int32 unitIndex, L7_int32 slotIndex, L7_int32 insertedType);

/*********************************************************************
*
* @purpose  Send agentStackingCardMismatch Trap
*
* @param    unitIndex       the unit identifier
* @param    slotIndex       the slot identifier
* @param    insertedType    the card type inserted into this slot
* @param    configuredType  the card type configured for this slot
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
SnmpEntConfigChangeTrapSend();

/*********************************************************************
*
* @purpose  Send failedUserLogin Trap
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
SnmpFailedUserLoginTrapSend();

/*********************************************************************
*
* @purpose  Send DHCP Snooping error disabled Trap
*
* @param    ifIndex  interface index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
SnmpDsIntfErrorDisabledTrapSend(L7_uint32 ifIndex);


/*********************************************************************
*
* @purpose  Send failedUserLogin Trap
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
SnmpLockedUserLoginTrapSend();

/*********************************************************************
*
* @purpose  Send Dai error disabled Trap
*
* @param    ifIndex  interface index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
SnmpDaiIntfErrorDisabledTrapSend(L7_uint32 ifIndex);

#ifdef FEAT_METRO_CPE_V1_0
/*********************************************************************
*
* @purpose  Send a No Startup Config Trap
*
* @param    none
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if error
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t SnmpNoStartupConfigTrapSend ();
#endif

#ifdef L7_CHASSIS
/*********************************************************************
*
* @purpose  Sent when a ChassisAlarm occurs for an alarm id.
*
* @param    alarmId          Alarm ID of the alarm raised
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Creates an SNMP trap OID and a VarBind structure.
*
* @end
*********************************************************************/
L7_RC_t SnmpAgentChassisRisingAlarmReportTrapSend(L7_uint32 alarmId);

/*********************************************************************
*
* @purpose  Sent when a ChassisAlarm is cleared.
*
* @param    alarmId          Alarm ID of the alarm to be cleared
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Creates an SNMP trap OID and a VarBind structure.
*
* @end
*********************************************************************/
L7_RC_t SnmpAgentChassisFallingAlarmReportTrapSend(L7_uint32 alarmId);

#endif

/*********************************************************************
*
* @purpose  Sent when a temperature change crosses min/max boundaries
*
* @param    sensorNum  Number of sensor
* @param    tempEventType  Event type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Creates an SNMP trap OID and a VarBind structure.
*
* @end
*********************************************************************/
L7_RC_t SnmpAgentTemperatureChangeReportTrapSend(L7_uint32 sensorNum, L7_uint32 tempEventType);

/*********************************************************************
*
* @purpose  Sent when a fan state is changed
*
* @param    itemNum  Number of fan
* @param    fanEventType  Event type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Creates an SNMP trap OID and a VarBind structure.
*
* @end
*********************************************************************/
L7_RC_t SnmpAgentFanStateChangeReportTrapSend(L7_uint32 itemNum, L7_uint32 fanEventType);


/*********************************************************************
*
* @purpose  Sent when a power supply state is changed
*
* @param    itemNum  Number of power supply
* @param    powSupplyEventType  Event type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Creates an SNMP trap OID and a VarBind structure.
*
* @end
*********************************************************************/
L7_RC_t SnmpAgentPowerSupplyStateChangeReportTrapSend(L7_uint32 itemNum, L7_uint32 powSupplyEventType);

/*********************************************************************
*
* @purpose  Send IP address conflict detection trap
*
* @param    conflictingIp  @b{(input)} conflicting IP address
* @param    conflictingMac @b{(input)} conflicting host's MAC address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t SnmpAgentIpAddressConflictTrapSend(L7_uint32 conflictingIp, L7_uchar8 *conflictingMac);

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
L7_RC_t SnmpDot1agCfmFaultAlarmTrapSend(L7_uint32 mdIndex, L7_uint32 maIndex, L7_uint32 mepId, L7_uint32 highestPriDefect);
#endif /* L7_DOT1AG_PACKAGE */

/*********************************************************************
* @purpose  Registers switching traps with Trapmgr
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
void SnmpTrapRegistrationSwitching(void);

/*********************************************************************
* @purpose  Send a CPU Rising utilization threshold trap
*
* @param    risingThreshold  @b{(input)} configured CPU rising threshold
* @param    buf              @b{(input)} buf containing task names
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snmpSwitchCpuRisingThresholdTrapSend(L7_uint32 risingThreshold, L7_char8* buf);

/*********************************************************************
* @purpose  Send a CPU Falling utilization threshold trap
*
* @param    fallingThreshold  @b{(input)} configured CPU falling threshold
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snmpSwitchCpuFallingThresholdTrapSend(L7_uint32 fallingThreshold);

/*********************************************************************
* @purpose  Send a CPU Free Memory below threshold trap
*
* @param    freeMemThreshold  @b{(input)} configured CPU free memory threshold
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snmpSwitchCpuFreeMemBelowThresholdTrapSend(L7_uint32 freeMemThreshold);

/*********************************************************************
* @purpose  Send a CPU Free Memory above threshold trap
*
* @param    freeMemThreshold  @b{(input)} configured CPU free memory threshold
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snmpSwitchCpuFreeMemAboveThresholdTrapSend(L7_uint32 freeMemThreshold);

/*********************************************************************
* @purpose  Send a CPU Falling utilization threshold trap
*
* @param    fallingThreshold  @b{(input)} configured CPU falling threshold
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrLogemailAlertSendEmailFailureTrapSend(L7_uint32  noEmailFailures);


#endif /* SNMP_TRAP_API_H */
