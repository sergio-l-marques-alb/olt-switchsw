/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename src\usmdb\usmdb_snmp_trap.c
*
* @purpose Provide interface to SNMP Trap API's for unitmgr components
*
* @component unitmgr
*
* @comments tba
*
* @create 01/10/2001
*
* @author cpverne
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#include "l7_common.h"
#include "usmdb_snmp_trap_api.h"
#include "usmdb_snmp_trap_api_stacking.h"
#include "nimapi.h"
#include "sysapi_hpc.h"
#include "snmp_trap_api.h"
#include "snmp_api.h"

/* Begin Function Declarations: usmdb_snmp_trap_api.h */

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
usmDbSnmpEnableTrapsGet(L7_uint32 *val)
{
#ifdef L7_SNMP_PACKAGE
  return SnmpEnableTrapsGet(val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

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
usmDbSnmpEnableTrapsSet(L7_uint32 val)
{
#ifdef L7_SNMP_PACKAGE
  return SnmpEnableTrapsSet(val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
* @purpose  Gets the current status of the Authentication Trap
*
* @param    UnitIndex      The unit for this operation
* @param    val   Status of Authentication Trap flag (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpEnableAuthenTrapsGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  return SnmpEnableAuthenTrapsGet(val);
}

/*********************************************************************
* @purpose  Sets the current status of the Authentication Trap
*
* @param    L7_uint32 UnitIndex      The unit for this operation
* @param    val   Status of Authentication Trap flag (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpEnableAuthenTrapsSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return SnmpEnableAuthenTrapsSet(val);
}

/*********************************************************************
*
* @purpose Send a ColdStart Trap via SNMP Agent.
*
* @param   L7_uint32 UnitIndex      The unit for this operation
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
L7_RC_t usmDbSnmpColdStartTrapSend(L7_uint32 UnitIndex)
{
  return SnmpColdStartTrapSend();
}

/*********************************************************************
*
* @purpose Send a WarmStart Trap via SNMP Agent.
*
* @param   L7_uint32 UnitIndex      The unit for this operation
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
L7_RC_t usmDbSnmpWarmStartTrapSend(L7_uint32 UnitIndex)
{
  return SnmpWarmStartTrapSend();
}

/*********************************************************************
*
* @purpose Send a LinkDown Trap via SNMP Agent.
*
* @param L7_uint32 UnitIndex      The unit for this operation
* @param L7_uint32 intIfNum  the internal interface number
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
L7_RC_t usmDbSnmpLinkDownTrapSend(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  L7_uint32 ifIndex;
  L7_RC_t   rc;

  rc = nimGetIntfIfIndex(intIfNum, &ifIndex);

  if (rc ==L7_SUCCESS)
  {
    /* @p0430 start */
    rc = SnmpLinkDownTrapSend(ifIndex);
  }

  return(rc);
  /* @p0430 end */
}

/*********************************************************************
*
* @purpose Send a LinkUp Trap via SNMP Agent.
*
* @param L7_uint32 UnitIndex      The unit for this operation
* @param L7_uint32 intIfNum  the internal interface number
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
L7_RC_t usmDbSnmpLinkUpTrapSend(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  L7_uint32 ifIndex;
  L7_RC_t   rc;

  rc = nimGetIntfIfIndex(intIfNum, &ifIndex);

  if (rc ==L7_SUCCESS)
  {
    /* @p0430 start */
    rc = SnmpLinkUpTrapSend(ifIndex);
  }

  return(rc);
  /* @p0430 end */
}

/*********************************************************************
*
* @purpose Send an AuthenFailure Trap via SNMP Agent.
*
* @param   L7_uint32 UnitIndex      The unit for this operation
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
L7_RC_t usmDbSnmpAuthenFailureTrapSend(L7_uint32 UnitIndex)
{
  return SnmpAuthenFailureTrapSend();
}

/*********************************************************************
*
* @purpose Send a MultipleUsers Trap via SNMP Agent.
*
* @param   L7_uint32 UnitIndex      The unit for this operation
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
L7_RC_t usmDbSnmpMultipleUsersTrapSend(L7_uint32 UnitIndex)
{
  return SnmpMultipleUsersTrapSend();
}

/*********************************************************************
*
* @purpose Send a BCastStormStart Trap via SNMP Agent.
*
* @param   L7_uint32 UnitIndex      The unit for this operation
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
L7_RC_t usmDbSnmpBroadcastStormStartTrapSend(L7_uint32 UnitIndex)
{
  return SnmpBroadcastStormStartTrapSend();
}

/*********************************************************************
*
* @purpose Send a BCastStormEnd Trap via SNMP Agent.
*
* @param   L7_uint32 UnitIndex      The unit for this operation
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
L7_RC_t usmDbSnmpBroadcastStormEndTrapSend(L7_uint32 UnitIndex)
{
  return SnmpBroadcastStormEndTrapSend();
}

/*********************************************************************
*
* @purpose Send a LinkFailure Trap via SNMP Agent.
*
* @param   L7_uint32 UnitIndex      The unit for this operation
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
L7_RC_t usmDbSnmpLinkFailureTrapSend(L7_uint32 UnitIndex)
{
  return SnmpLinkFailureTrapSend();
}

/*********************************************************************
*
* @purpose Send a VLAN request failed Trap via SNMP Agent.
*
* @param L7_uint32 UnitIndex      The unit for this operation
* @param L7_uint32 vlanID  the vlan id number
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
L7_RC_t usmDbSnmpVlanRequestFailureTrapSend(L7_uint32 UnitIndex, L7_uint32 vlanID)
{
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Send the last, default VLAN deleted Trap via SNMP Agent.
*
* @param L7_uint32 UnitIndex      The unit for this operation
* @param L7_uint32 vlanID  the vlan id number
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
L7_RC_t usmDbSnmpVlanDeleteLastTrapSend(L7_uint32 UnitIndex, L7_uint32 vlanID)
{
  return SnmpVlanDeleteLastTrapSend(vlanID);
}

/*********************************************************************
*
* @purpose Send the default cfg VLAN failure Trap via SNMP Agent.
*
* @param L7_uint32 UnitIndex      The unit for this operation
* @param L7_uint32 vlanID  the vlan id number
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
L7_RC_t usmDbSnmpVlanDefaultCfgFailureTrapSend(L7_uint32 UnitIndex, L7_uint32 vlanID)
{
  return SnmpVlanDefaultCfgFailureTrapSend(vlanID);
}

/*********************************************************************
*
* @purpose Send a VLAN restore failure Trap via SNMP Agent.
*
* @param L7_uint32 UnitIndex      The unit for this operation
* @param L7_uint32 vlanID  the vlan id number
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
L7_RC_t usmDbSnmpVlanRestoreFailureTrapSend(L7_uint32 UnitIndex, L7_uint32 vlanID)
{
  return SnmpVlanRestoreFailureTrapSend(vlanID);
}

/*********************************************************************
*
* @purpose Send a FanFailure Trap via SNMP Agent.
*
* @param   L7_uint32 UnitIndex      The unit for this operation
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
L7_RC_t usmDbSnmpFanFailureTrapSend(L7_uint32 UnitIndex)
{
  return SnmpFanFailureTrapSend();
}

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
L7_RC_t usmDbSnmpMacLockViolationTrapSend(L7_uint32 intIfNum)
{
  return SnmpMacLockViolationTrapSend(intIfNum);
}

/*********************************************************************
* @purpose  Send Power Supply Status Change Trap via SNMP Agent
*
* @param    L7_uint32 UnitIndex      The unit for this operation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpPowerSupplyStatusChangeTrapSend(L7_uint32 UnitIndex)
{
  return SnmpPowerSupplyStatusChangeTrapSend();
}

/*********************************************************************
*
* @purpose Send a NewRoot Trap via SNMP Agent.
*
* @param   L7_uint32 UnitIndex      The unit for this operation
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
L7_RC_t usmDbSnmpNewRootTrapSend(L7_uint32 UnitIndex)
{
  return SnmpNewRootTrapSend();
}

/*********************************************************************
*
* @purpose Send a TopologyChange Trap for an STP instance via SNMP Agent.
*
* @param   L7_uint32 UnitIndex      The unit for this operation
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
L7_RC_t usmDbSnmpStpInstanceTopologyChangeTrapSend(L7_uint32 UnitIndex, L7_uint32 instanceId)
{
  return SnmpStpInstanceTopologyChangeTrapSend(instanceId);
}

/*********************************************************************
*
* @purpose Send a NewRoot Trap for an STP instance via SNMP Agent.
*
* @param   L7_uint32 UnitIndex      The unit for this operation
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
L7_RC_t usmDbSnmpStpInstanceNewRootTrapSend(L7_uint32 UnitIndex, L7_uint32 instanceId)
{
  return SnmpStpInstanceNewRootTrapSend(instanceId);
}

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
L7_RC_t usmDbSnmpStpInstanceLoopInconsistentStartTrapSend(L7_uint32 UnitIndex, L7_uint32 instanceId, L7_uint32 intIfNum)
{
  L7_RC_t   rc;
  L7_uint32 ifIndex;

  rc = nimGetIntfIfIndex(intIfNum, &ifIndex);

  if (rc == L7_SUCCESS)
  {
    rc = SnmpStpInstanceLoopInconsistentStartTrapSend(instanceId, ifIndex);
  }
  return(rc);
}

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
L7_RC_t usmDbSnmpStpInstanceLoopInconsistentEndTrapSend(L7_uint32 UnitIndex, L7_uint32 instanceId, L7_uint32 intIfNum)
{
  L7_RC_t   rc;
  L7_uint32 ifIndex;

  rc = nimGetIntfIfIndex(intIfNum, &ifIndex);

  if (rc == L7_SUCCESS)
  {
    rc = SnmpStpInstanceLoopInconsistentEndTrapSend(instanceId, ifIndex);
  }
  return(rc);
}

/*********************************************************************
*
* @purpose Send a TopologyChange Trap via SNMP Agent.
*
* @param   L7_uint32 UnitIndex      The unit for this operation
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
L7_RC_t usmDbSnmpTopologyChangeTrapSend(L7_uint32 UnitIndex)
{
  return SnmpTopologyChangeTrapSend();
}

/*********************************************************************
* @purpose  Send Rising Alarm Trap via SNMP Agent
*
* @param    L7_uint32 UnitIndex      The unit for this operation
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
L7_RC_t usmDbSnmpRisingAlarmTrapSend(L7_uint32 UnitIndex, L7_uint32 alarmIndex)
{
  return SnmpRisingAlarmTrapSend(alarmIndex);
}

/*********************************************************************
* @purpose  Send Falling Alarm Trap via SNMP Agent
*
* @param    L7_uint32 UnitIndex      The unit for this operation
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
L7_RC_t usmDbSnmpFallingAlarmTrapSend(L7_uint32 UnitIndex, L7_uint32 alarmIndex)
{
  return SnmpFallingAlarmTrapSend(alarmIndex);
}

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
L7_RC_t usmDbSnmpAgentInventoryCardMismatchTrapSend(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 insertedType, L7_uint32 configuredType)
{
  return SnmpAgentInventoryCardMismatchTrapSend(unitIndex, slotIndex, insertedType, configuredType);
}

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
L7_RC_t usmDbSnmpAgentInventoryCardUnsupportedTrapSend(L7_uint32 unitIndex,
                                                       L7_uint32 slotIndex,
                                                       L7_uint32 insertedType)
{
  return SnmpAgentInventoryCardUnsupportedTrapSend(unitIndex, slotIndex, insertedType);
}

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
L7_RC_t usmDbSnmpAgentInventoryCardPluginTrapSend(L7_uint32 unitIndex,
                                                  L7_uint32 slotIndex,
                                                  L7_uint32 insertedType)
{
/*  return SnmpAgentInventoryCardPluginTrapSend(unitIndex, slotIndex, insertedType); */
  return L7_SUCCESS;
}

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
L7_RC_t usmDbSnmpAgentInventoryCardUnplugTrapSend(L7_uint32 unitIndex,
                                                  L7_uint32 slotIndex)
{
/*  return SnmpAgentInventoryCardUnplugTrapSend(unitIndex, slotIndex); */
  return L7_SUCCESS;
}

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
L7_RC_t usmDbSnmpAgentInventoryCardFailureTrapSend(L7_uint32 unitIndex, L7_uint32 slotIndex)
{
/* return SnmpAgentInventoryCardFailureTrapSend(unitIndex, slotIndex); */
  return L7_SUCCESS;
}

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
L7_RC_t usmDbSnmpAgentInventoryStackPortLinkUpTrapSend(L7_uint32 agentInventoryStackPortIndex, L7_uint32 agentInventoryStackPortUnit, L7_char8 *agentInventoryStackPortTag)
{
  return SnmpAgentInventoryStackPortLinkUpTrapSend(agentInventoryStackPortIndex, agentInventoryStackPortUnit, agentInventoryStackPortTag);
}

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
L7_RC_t usmDbSnmpAgentInventoryStackPortLinkDownTrapSend(L7_uint32 agentInventoryStackPortIndex, L7_uint32 agentInventoryStackPortUnit, L7_char8 *agentInventoryStackPortTag)
{
  return SnmpAgentInventoryStackPortLinkDownTrapSend(agentInventoryStackPortIndex, agentInventoryStackPortUnit, agentInventoryStackPortTag);
}

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
L7_RC_t usmDbSnmpEntConfigChangeTrapSend()
{
  return SnmpEntConfigChangeTrapSend();
}

/*********************************************************************
* @purpose  Send User Login failure Trap via SNMP Agent
*
* @param
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpFailedUserLoginTrapSend()
{
  return SnmpFailedUserLoginTrapSend();
}

/*********************************************************************
* @purpose  Send User Login failure Trap via SNMP Agent
*
* @param
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpLockedUserLoginTrapSend()
{
  return SnmpLockedUserLoginTrapSend();
}

/*********************************************************************
* @purpose  Send DAI Error Disabled on an interface Trap via SNMP Agent
*
* @param
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpDaiIntfErrorDisabledTrapSend(L7_uint32 intIfNum)
{
  L7_uint32 ifIndex;
  L7_RC_t   rc;

  rc = nimGetIntfIfIndex(intIfNum, &ifIndex);

  if (rc ==L7_SUCCESS)
  {
    return SnmpDaiIntfErrorDisabledTrapSend(ifIndex);
  }

  return(rc);
}

/*********************************************************************
* @purpose  Send DHCP Snooping  Error Disabled on an interface Trap
*            via SNMP Agent
*
* @param
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpDsIntfErrorDisabledTrapSend(L7_uint32 intIfNum)
{
  L7_uint32 ifIndex;
  L7_RC_t   rc;

  rc = nimGetIntfIfIndex(intIfNum, &ifIndex);

  if (rc ==L7_SUCCESS)
  {
    return SnmpDsIntfErrorDisabledTrapSend(ifIndex);
  }

  return(rc);
}

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
L7_RC_t usmDbSnmpNoStartupConfigTrapSend(L7_uint32 UnitIndex)
{
  L7_RC_t   rc;
  rc = SnmpNoStartupConfigTrapSend();
  return(rc);
}
#endif
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
L7_RC_t usmDbSnmpChassisRisingAlarmTrapSend(L7_uint32 alarmIndex)
{
  return SnmpAgentChassisRisingAlarmReportTrapSend(alarmIndex);
}

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
L7_RC_t usmDbSnmpChassisFallingAlarmTrapSend(L7_uint32 alarmIndex)
{
   return SnmpAgentChassisFallingAlarmReportTrapSend(alarmIndex);
}
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
L7_RC_t usmDbSnmpTemperatureChangeTrapSend(L7_uint32 sensorNum, L7_uint32 tempEventType)
{
  return SnmpAgentTemperatureChangeReportTrapSend(sensorNum, tempEventType);
}

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
L7_RC_t usmDbSnmpFanStateChangeTrapSend(L7_uint32 itemNum, L7_uint32 fanEventType)
{
  return SnmpAgentFanStateChangeReportTrapSend(itemNum, fanEventType);
}

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
L7_RC_t usmDbSnmpPowSupplyStateChangeTrapSend(L7_uint32 itemNum, L7_uint32 powSupplyEventType)
{
  return SnmpAgentPowerSupplyStateChangeReportTrapSend(itemNum, powSupplyEventType);
}

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
L7_RC_t usmDbSnmpDot1agCfmFaultAlarmTrapSend(L7_uint32 mdIndex, L7_uint32 maIndex, L7_uint32 mepId, L7_uint32 highestPriDefect)
{
  return SnmpDot1agCfmFaultAlarmTrapSend(mdIndex, maIndex, mepId, highestPriDefect);
}
#endif /* L7_DOT1AG_PACKAGE */

/* End Function Declarations */
