/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: snmp_trap_api.c
*
* Purpose: API interface for SNMP Agent Traps
*
* Created by: Colin Verne 06/07/2001
*
* Component: SNMP
*
*********************************************************************/
/*************************************************************
                    
*************************************************************/



#include "l7_common.h"
#include "dtlapi.h"
#include "osapi.h"
#include "snmptrap.h"
#include "basetrap.h"
#include "privatetrap_vlan.h"
#include "privatetrap_entity.h"
#include "privatetrap_fastpathinventory.h"
#include "privatetrap_fastpath_portsecurity.h"
#include "snmp_trap_api.h"
#include "snmp_api.h"
#include "usmdb_spm_api.h"
#include "string.h"
#include "sysapi.h"
#ifdef L7_CHASSIS
#include "privatetrap_fastpathchassis.h"
#endif

#include "privatetrap_box_services.h"
#ifdef L7_DOT1AG_PACKAGE
#include "privatetrap_dot1ag.h"
#endif /* L7_DOT1AG_PACKAGE */
/* defined in snmp_sr\src\snmpd\shared\v_snmpv1.c */
extern L7_uint32 k_snmp_snmpEnableAuthenTrapsGet();
extern void k_snmp_snmpEnableAuthenTrapsSet(L7_uint32 val);


L7_uint32 SnmpAuthenTrapsValue = L7_DISABLE;
L7_uint32 SnmpAuthenTrapsEnable = L7_DISABLE;

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
L7_RC_t SnmpInitializeTraps()
{
  if ( SnmpStatusGet() != L7_ENABLE)
  {
    return L7_FAILURE;
  }

  if (SnmpAuthenTrapsEnable == L7_ENABLE)
  {
    k_snmp_snmpEnableAuthenTrapsSet(SnmpAuthenTrapsValue);
  }

  return L7_SUCCESS;
}

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
L7_RC_t SnmpEnableAuthenTrapsGet(L7_uint32 *val)
{
  if ( SnmpStatusGet() == L7_ENABLE )
  {
    *val = k_snmp_snmpEnableAuthenTrapsGet();
  }
  else if (SnmpAuthenTrapsEnable == L7_ENABLE)
  {
    *val = SnmpAuthenTrapsValue;
  }
  else
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

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
L7_RC_t SnmpEnableAuthenTrapsSet(L7_uint32 val)
{
  if ( SnmpStatusGet() != L7_ENABLE )
  {
    SnmpAuthenTrapsValue = val;
    SnmpAuthenTrapsEnable = L7_TRUE;
    return L7_SUCCESS;
  }

  k_snmp_snmpEnableAuthenTrapsSet(val);
  return L7_SUCCESS;
}



/*********************************************************************/
typedef enum
{
  L7_SNMP_TRAP_SWITCHING_ColdStart=0,
  L7_SNMP_TRAP_SWITCHING_WarmStart,
  L7_SNMP_TRAP_SWITCHING_LinkDown,
  L7_SNMP_TRAP_SWITCHING_LinkUp,
  L7_SNMP_TRAP_SWITCHING_AuthenFailure,
  L7_SNMP_TRAP_SWITCHING_FanFailure,
  L7_SNMP_TRAP_SWITCHING_MacLockViolation,
  L7_SNMP_TRAP_SWITCHING_PowerSupplyStatusChange,
  L7_SNMP_TRAP_SWITCHING_VlanRestoreFailure,
  L7_SNMP_TRAP_SWITCHING_VlanDefaultCfgFailure,
  L7_SNMP_TRAP_SWITCHING_VlanDeleteLast,
  L7_SNMP_TRAP_SWITCHING_LinkFailure,
  L7_SNMP_TRAP_SWITCHING_BroadcastStormEnd,
  L7_SNMP_TRAP_SWITCHING_BroadcastStormStart,
  L7_SNMP_TRAP_SWITCHING_MultipleUsers,
  L7_SNMP_TRAP_SWITCHING_FallingAlarm,
  L7_SNMP_TRAP_SWITCHING_RisingAlarm,
  L7_SNMP_TRAP_SWITCHING_TopologyChange,
  L7_SNMP_TRAP_SWITCHING_NewRoot,
  L7_SNMP_TRAP_SWITCHING_StpInstanceTopologyChange,
  L7_SNMP_TRAP_SWITCHING_StpInstanceNewRoot,
  L7_SNMP_TRAP_SWITCHING_StpInstanceLoopInconsistentStart,
  L7_SNMP_TRAP_SWITCHING_StpInstanceLoopInconsistentEnd,    
  L7_SNMP_TRAP_SWITCHING_AgentInventoryCardMismatch,
#ifdef L7_CHASSIS
  L7_SNMP_TRAP_SWITCHING_AgentChassisFallingAlarm,
  L7_SNMP_TRAP_SWITCHING_AgentChassisRisingAlarm,
  L7_SNMP_TRAP_SWITCHING_AgentChassisCardMismatch,
  L7_SNMP_TRAP_SWITCHING_AgentChassisCardUnsupported,
  L7_SNMP_TRAP_SWITCHING_AgentChassisBackplanePortLinkUp,
  L7_SNMP_TRAP_SWITCHING_AgentChassisBackplanePortLinkDown,
#endif
  L7_SNMP_TRAP_SWITCHING_AgentInventoryCardUnsupported,
  L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPortLinkUp,
  L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPortLinkDown,
  L7_SNMP_TRAP_SWITCHING_EntConfigChange,
  L7_SNMP_TRAP_SWITCHING_FailedUserLogin,
  L7_SNMP_TRAP_SWITCHING_LockedUserLogin,
  L7_SNMP_TRAP_SWITCHING_DaiIntfErrorDisabled,
  L7_SNMP_TRAP_SWITCHING_DsIntfErrorDisabled,
  L7_SNMP_TRAP_SWITCHING_TemperatureChange,
  L7_SNMP_TRAP_SWITCHING_FanStateChange,
  L7_SNMP_TRAP_SWITCHING_PowSupplyStateChange,
#ifdef L7_DOT1AG_PACKAGE
  L7_SNMP_TRAP_SWITCHING_Dot1agCfmFaultAlarmNotify,
#endif /* L7_DOT1AG_PACKAGE */
  L7_SNMP_TRAP_BASE_noStartupConfig  
} L7_SNMP_TRAP_TYPE_SWITCHING_t;


typedef struct
{
  L7_uint32 ifIndex;
} L7_SNMP_TRAP_SWITCHING_ifIndex_t;

typedef struct
{
  L7_uint32 dot1qVlanIndex;
} L7_SNMP_TRAP_SWITCHING_dot1qVlanIndex_t;

typedef struct
{
  L7_uint32 alarmIndex;
} L7_SNMP_TRAP_SWITCHING_alarmIndex_t;

typedef struct
{
  L7_int32 instanceID;
} L7_SNMP_TRAP_SWITCHING_instanceID_t;

typedef struct
{
  L7_int32 unitIndex;
  L7_int32 slotIndex;
  L7_int32 insertedType;
  L7_int32 configuredType;
} L7_SNMP_TRAP_SWITCHING_AgentInventoryCardMismatch_t;

typedef struct
{
  L7_int32 unitIndex;
  L7_int32 slotIndex;
  L7_int32 insertedType;
} L7_SNMP_TRAP_SWITCHING_AgentInventoryCardUnsupported_t;

typedef struct
{
  L7_uint32 agentInventoryStackPortIndex;
  L7_uint32 agentInventoryStackPortUnit;
  L7_char8 agentInventoryStackPortTag[SPM_STACK_PORT_TAG_SIZE];
} L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPort_t;


typedef struct
{
  L7_uint32 alarmId;
} L7_SNMP_TRAP_SWITCHING_alarmId_t;

typedef struct
{
  L7_int32 instanceID;  
  L7_uint32 ifIndex;
} L7_SNMP_TRAP_SWITCHING_instanceID_ifIndex_t;

typedef struct
{
  L7_uint32 sensorNum;
  L7_uint32 tempEventType;
} L7_SNMP_TRAP_SWITCHING_temperatureEvent_t;

/* Following applies to both fans and power supplies */
typedef struct
{
  L7_uint32 itemNum;
  L7_uint32 itemState;
} L7_SNMP_TRAP_SWITCHING_itemStateEvent_t;


typedef struct
{
  L7_uint32 mdIndex;
  L7_uint32 maIndex;
  L7_uint32 mepId;
  L7_uint32 highestPriDefect;
} L7_SNMP_TRAP_SWITCHING_dot1agCfmFaultAlarmNotify_t;

/*********************************************************************
*
* @purpose  Handles calling the trap code to send traps
*
* @param    trap_id   enumeration of the trap to send
* @param    data      data structure holding trap information
*
* @returns  void
*
* @notes    The datastructure is determined by the trap_id indicated.
*
* @end
*********************************************************************/
void SnmpTrapSendCallback_switching(L7_uint32 trap_id, void *data)
{
  switch (trap_id)
  {
  case L7_SNMP_TRAP_SWITCHING_ColdStart:
    (void)snmp_ColdStartTrapSend();
    break;

  case L7_SNMP_TRAP_SWITCHING_WarmStart:
    (void)snmp_WarmStartTrapSend();
    break;

  case L7_SNMP_TRAP_SWITCHING_LinkDown:
    (void)snmp_LinkDownTrapSend(((L7_SNMP_TRAP_SWITCHING_ifIndex_t*)data)->ifIndex);
    break;

  case L7_SNMP_TRAP_SWITCHING_LinkUp:
    (void)snmp_LinkUpTrapSend(((L7_SNMP_TRAP_SWITCHING_ifIndex_t*)data)->ifIndex);
    break;

  case L7_SNMP_TRAP_SWITCHING_AuthenFailure:
    (void)snmp_AuthenticationFailureTrapSend();
    break;

  case L7_SNMP_TRAP_SWITCHING_FanFailure:
    (void)snmp_FanFailureTrapSend();
    break;

  case L7_SNMP_TRAP_SWITCHING_MacLockViolation:
    (void)snmp_agentPortSecurityViolationTrapSend(((L7_SNMP_TRAP_SWITCHING_ifIndex_t*)data)->ifIndex);
    break;

  case L7_SNMP_TRAP_SWITCHING_PowerSupplyStatusChange:
    (void)snmp_PowerSupplyStatusChangeTrapSend();
    break;

  case L7_SNMP_TRAP_SWITCHING_VlanRestoreFailure:
    (void)snmp_VlanRestoreFailureTrapSend(((L7_SNMP_TRAP_SWITCHING_dot1qVlanIndex_t*)data)->dot1qVlanIndex);
    break;

  case L7_SNMP_TRAP_SWITCHING_VlanDefaultCfgFailure:
    (void)snmp_VlanDefaultCfgFailureTrapSend(((L7_SNMP_TRAP_SWITCHING_dot1qVlanIndex_t*)data)->dot1qVlanIndex);
    break;

  case L7_SNMP_TRAP_SWITCHING_VlanDeleteLast:
    (void)snmp_VlanDeleteLastTrapSend(((L7_SNMP_TRAP_SWITCHING_dot1qVlanIndex_t*)data)->dot1qVlanIndex);
    break;

  case L7_SNMP_TRAP_SWITCHING_LinkFailure:
    (void)snmp_LinkFailureTrapSend();
    break;

  case L7_SNMP_TRAP_SWITCHING_BroadcastStormEnd:
    (void)snmp_BroadcastStormEndTrapSend();
    break;

  case L7_SNMP_TRAP_SWITCHING_BroadcastStormStart:
    (void)snmp_BroadcastStormStartTrapSend();
    break;

  case L7_SNMP_TRAP_SWITCHING_MultipleUsers:
    (void)snmp_MultipleUsersTrapSend();
    break;

  case L7_SNMP_TRAP_SWITCHING_FallingAlarm:
    (void)snmp_FallingAlarmTrapSend(((L7_SNMP_TRAP_SWITCHING_alarmIndex_t*)data)->alarmIndex);
    break;

  case L7_SNMP_TRAP_SWITCHING_RisingAlarm:
    (void)snmp_RisingAlarmTrapSend(((L7_SNMP_TRAP_SWITCHING_alarmIndex_t*)data)->alarmIndex);
    break;

  case L7_SNMP_TRAP_SWITCHING_TopologyChange:
    (void)snmp_TopologyChangeTrapSend();
    break;

  case L7_SNMP_TRAP_SWITCHING_NewRoot:
    (void)snmp_NewRootTrapSend();
    break;

  case L7_SNMP_TRAP_SWITCHING_StpInstanceTopologyChange:
    (void)snmp_StpInstanceTopologyChangeTrapSend(((L7_SNMP_TRAP_SWITCHING_instanceID_t*)data)->instanceID);
    break;

  case L7_SNMP_TRAP_SWITCHING_StpInstanceNewRoot:
    (void)snmp_StpInstanceNewRootTrapSend(((L7_SNMP_TRAP_SWITCHING_instanceID_t*)data)->instanceID);
    break;
    
  case L7_SNMP_TRAP_SWITCHING_StpInstanceLoopInconsistentStart:
    (void)snmp_StpInstanceLoopInconsistentStartTrapSend(((L7_SNMP_TRAP_SWITCHING_instanceID_ifIndex_t*)data)->instanceID,
                                                        ((L7_SNMP_TRAP_SWITCHING_instanceID_ifIndex_t*)data)->ifIndex);
    break;

  case L7_SNMP_TRAP_SWITCHING_StpInstanceLoopInconsistentEnd:
    (void)snmp_StpInstanceLoopInconsistentEndTrapSend(((L7_SNMP_TRAP_SWITCHING_instanceID_ifIndex_t*)data)->instanceID,
                                                      ((L7_SNMP_TRAP_SWITCHING_instanceID_ifIndex_t*)data)->ifIndex);
    break;
    
#ifndef L7_CHASSIS
  case L7_SNMP_TRAP_SWITCHING_AgentInventoryCardMismatch:
    (void)snmp_agentInventoryCardMismatchTrapSend(((L7_SNMP_TRAP_SWITCHING_AgentInventoryCardMismatch_t*)data)->unitIndex,
                                                  ((L7_SNMP_TRAP_SWITCHING_AgentInventoryCardMismatch_t*)data)->slotIndex,
                                                  ((L7_SNMP_TRAP_SWITCHING_AgentInventoryCardMismatch_t*)data)->insertedType,
                                                  ((L7_SNMP_TRAP_SWITCHING_AgentInventoryCardMismatch_t*)data)->configuredType);
    break;
#endif
#ifdef L7_CHASSIS
  case L7_SNMP_TRAP_SWITCHING_AgentChassisFallingAlarm:
    (void)snmp_agentChassisFallingAlarmReportTrapSend(((L7_SNMP_TRAP_SWITCHING_alarmId_t*)data)->alarmId);
    break;

  case L7_SNMP_TRAP_SWITCHING_AgentChassisRisingAlarm:
    (void)snmp_agentChassisRisingAlarmReportTrapSend(((L7_SNMP_TRAP_SWITCHING_alarmId_t*)data)->alarmId);
    break;

  case L7_SNMP_TRAP_SWITCHING_AgentChassisCardMismatch:
    (void)snmp_agentChassisCardMismatchTrapSend(((L7_SNMP_TRAP_SWITCHING_AgentInventoryCardMismatch_t*)data)->unitIndex,
                                                  ((L7_SNMP_TRAP_SWITCHING_AgentInventoryCardMismatch_t*)data)->slotIndex,
                                                  ((L7_SNMP_TRAP_SWITCHING_AgentInventoryCardMismatch_t*)data)->insertedType,
                                                  ((L7_SNMP_TRAP_SWITCHING_AgentInventoryCardMismatch_t*)data)->configuredType);
    break;

  case L7_SNMP_TRAP_SWITCHING_AgentChassisCardUnsupported:
    (void)snmp_agentChassisCardUnsupportedTrapSend(((L7_SNMP_TRAP_SWITCHING_AgentInventoryCardUnsupported_t*)data)->unitIndex,
                                                     ((L7_SNMP_TRAP_SWITCHING_AgentInventoryCardUnsupported_t*)data)->slotIndex,
                                                     ((L7_SNMP_TRAP_SWITCHING_AgentInventoryCardUnsupported_t*)data)->insertedType);
    break;

  case L7_SNMP_TRAP_SWITCHING_AgentChassisBackplanePortLinkUp:
    (void)snmp_agentChassisBackplanePortLinkUpTrapSend(((L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPort_t*)data)->agentInventoryStackPortIndex,
                                                     ((L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPort_t*)data)->agentInventoryStackPortUnit,
                                                     ((L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPort_t*)data)->agentInventoryStackPortTag);
    break;

  case L7_SNMP_TRAP_SWITCHING_AgentChassisBackplanePortLinkDown:
    (void)snmp_agentChassisBackplanePortLinkDownTrapSend(((L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPort_t*)data)->agentInventoryStackPortIndex,
                                                       ((L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPort_t*)data)->agentInventoryStackPortUnit,
                                                       ((L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPort_t*)data)->agentInventoryStackPortTag);
    break;
#endif
#ifndef L7_CHASSIS
  case L7_SNMP_TRAP_SWITCHING_AgentInventoryCardUnsupported:
    (void)snmp_agentInventoryCardUnsupportedTrapSend(((L7_SNMP_TRAP_SWITCHING_AgentInventoryCardUnsupported_t*)data)->unitIndex,
                                                     ((L7_SNMP_TRAP_SWITCHING_AgentInventoryCardUnsupported_t*)data)->slotIndex,
                                                     ((L7_SNMP_TRAP_SWITCHING_AgentInventoryCardUnsupported_t*)data)->insertedType);
    break;

  case L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPortLinkUp:
    (void)snmp_agentInventoryStackPortLinkUpTrapSend(((L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPort_t*)data)->agentInventoryStackPortIndex,
                                                     ((L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPort_t*)data)->agentInventoryStackPortUnit,
                                                     ((L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPort_t*)data)->agentInventoryStackPortTag);
    break;

  case L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPortLinkDown:
    (void)snmp_agentInventoryStackPortLinkDownTrapSend(((L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPort_t*)data)->agentInventoryStackPortIndex,
                                                       ((L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPort_t*)data)->agentInventoryStackPortUnit,
                                                       ((L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPort_t*)data)->agentInventoryStackPortTag);
    break;
#endif
  case L7_SNMP_TRAP_SWITCHING_EntConfigChange:
    (void)snmp_entConfigChangeTrapSend();
    break;

  case L7_SNMP_TRAP_SWITCHING_FailedUserLogin:
    (void)snmp_failedUserLoginTrapSend();
    break;

  case L7_SNMP_TRAP_SWITCHING_LockedUserLogin:
    (void)snmp_lockedUserLoginTrapSend();
    break;

  case L7_SNMP_TRAP_SWITCHING_DaiIntfErrorDisabled:
    (void)snmp_daiIntfErrorDisabledTrapSend(((L7_SNMP_TRAP_SWITCHING_ifIndex_t*)data)->ifIndex);
    break;

  case L7_SNMP_TRAP_SWITCHING_DsIntfErrorDisabled:
    (void)snmp_dhcpSnoopingIntfErrorDisabledTrapSend(((L7_SNMP_TRAP_SWITCHING_ifIndex_t*)data)->ifIndex);
    break;
	
#ifdef FEAT_METRO_CPE_V1_0
 case L7_SNMP_TRAP_BASE_noStartupConfig: 
    (void)snmp_noStartupConfigNotificationTrapSend();
    break;
#endif

  case L7_SNMP_TRAP_SWITCHING_TemperatureChange:
	(void)snmp_boxsTemperatureChangeTrapSend( ((L7_SNMP_TRAP_SWITCHING_temperatureEvent_t*)data)->sensorNum, 
										((L7_SNMP_TRAP_SWITCHING_temperatureEvent_t*)data)->tempEventType);
	break;

  case L7_SNMP_TRAP_SWITCHING_FanStateChange:
	(void)snmp_boxsFanStateChangeTrapSend( ((L7_SNMP_TRAP_SWITCHING_itemStateEvent_t*)data)->itemNum, 
										((L7_SNMP_TRAP_SWITCHING_itemStateEvent_t*)data)->itemState);
	break;

  case L7_SNMP_TRAP_SWITCHING_PowSupplyStateChange:
	(void)snmp_boxsPowSupplyStateChangeTrapSend( ((L7_SNMP_TRAP_SWITCHING_itemStateEvent_t*)data)->itemNum, 
										((L7_SNMP_TRAP_SWITCHING_itemStateEvent_t*)data)->itemState);
	break;
#ifdef L7_DOT1AG_PACKAGE
  case L7_SNMP_TRAP_SWITCHING_Dot1agCfmFaultAlarmNotify:
   (void)snmp_Dot1agCfmFaultAlarmNotifySend(((L7_SNMP_TRAP_SWITCHING_dot1agCfmFaultAlarmNotify_t*)data)->mdIndex,
                    ((L7_SNMP_TRAP_SWITCHING_dot1agCfmFaultAlarmNotify_t*)data)->maIndex,
                    ((L7_SNMP_TRAP_SWITCHING_dot1agCfmFaultAlarmNotify_t*)data)->mepId,
                    ((L7_SNMP_TRAP_SWITCHING_dot1agCfmFaultAlarmNotify_t*)data)->highestPriDefect);

  break;
#endif /* L7_DOT1AG_PACKAGE */
  default:
    break;
  }

  return;
}
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
L7_RC_t SnmpColdStartTrapSend ()
{
  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_ColdStart,  L7_NULLPTR, &SnmpTrapSendCallback_switching);
}

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
L7_RC_t SnmpWarmStartTrapSend ()
{
  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_WarmStart,  L7_NULLPTR, &SnmpTrapSendCallback_switching);
}

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
L7_RC_t SnmpLinkDownTrapSend ( L7_uint32 ifIndex )
{
  L7_SNMP_TRAP_SWITCHING_ifIndex_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_ifIndex_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->ifIndex = ifIndex;

  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_LinkDown, (void*)trap, &SnmpTrapSendCallback_switching);
}


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
L7_RC_t SnmpLinkUpTrapSend ( L7_uint32 ifIndex )
{
  L7_SNMP_TRAP_SWITCHING_ifIndex_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_ifIndex_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->ifIndex = ifIndex;

  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_LinkUp, (void*)trap, &SnmpTrapSendCallback_switching);
}

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
L7_RC_t SnmpAuthenFailureTrapSend ()
{
  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_AuthenFailure,  L7_NULLPTR, &SnmpTrapSendCallback_switching);
}


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
L7_RC_t SnmpFanFailureTrapSend()
{
  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_FanFailure,  L7_NULLPTR, &SnmpTrapSendCallback_switching);
}

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
L7_RC_t SnmpMacLockViolationTrapSend(L7_uint32 intIfNum)
{
  L7_SNMP_TRAP_SWITCHING_ifIndex_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_ifIndex_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->ifIndex = intIfNum;

  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_MacLockViolation, (void*)trap, &SnmpTrapSendCallback_switching);
}

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
L7_RC_t SnmpPowerSupplyStatusChangeTrapSend()
{
  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_PowerSupplyStatusChange,  L7_NULLPTR, &SnmpTrapSendCallback_switching);
}

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
L7_RC_t SnmpVlanRestoreFailureTrapSend(L7_int32 dot1qVlanIndex)
{
  L7_SNMP_TRAP_SWITCHING_dot1qVlanIndex_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_dot1qVlanIndex_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->dot1qVlanIndex = dot1qVlanIndex;

  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_VlanRestoreFailure, (void*)trap, &SnmpTrapSendCallback_switching);
}

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
L7_RC_t SnmpVlanDefaultCfgFailureTrapSend(L7_int32 dot1qVlanIndex)
{
  L7_SNMP_TRAP_SWITCHING_dot1qVlanIndex_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_dot1qVlanIndex_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->dot1qVlanIndex = dot1qVlanIndex;

  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_VlanDefaultCfgFailure, (void*)trap, &SnmpTrapSendCallback_switching);
}


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
L7_RC_t SnmpVlanDeleteLastTrapSend(L7_int32 dot1qVlanIndex)
{
  L7_SNMP_TRAP_SWITCHING_dot1qVlanIndex_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_dot1qVlanIndex_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->dot1qVlanIndex = dot1qVlanIndex;

  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_VlanDeleteLast, (void*)trap, &SnmpTrapSendCallback_switching);
}

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
L7_RC_t SnmpLinkFailureTrapSend()
{
  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_LinkFailure,  L7_NULLPTR, &SnmpTrapSendCallback_switching);
}


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
L7_RC_t SnmpBroadcastStormEndTrapSend()
{
  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_BroadcastStormEnd,  L7_NULLPTR, &SnmpTrapSendCallback_switching);
}


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
L7_RC_t SnmpBroadcastStormStartTrapSend()
{
  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_BroadcastStormStart,  L7_NULLPTR, &SnmpTrapSendCallback_switching);
}


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
L7_RC_t SnmpMultipleUsersTrapSend()
{
  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_MultipleUsers,  L7_NULLPTR, &SnmpTrapSendCallback_switching);
}


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
L7_RC_t SnmpFallingAlarmTrapSend( L7_int32 alarmIndex)
{
  L7_SNMP_TRAP_SWITCHING_alarmIndex_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_alarmIndex_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->alarmIndex = alarmIndex;

  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_FallingAlarm, (void*)trap, &SnmpTrapSendCallback_switching);
}


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
L7_RC_t SnmpRisingAlarmTrapSend ( L7_int32 alarmIndex)
{
  L7_SNMP_TRAP_SWITCHING_alarmIndex_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_alarmIndex_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->alarmIndex = alarmIndex;

  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_RisingAlarm, (void*)trap, &SnmpTrapSendCallback_switching);
}

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
L7_RC_t SnmpTopologyChangeTrapSend()
{
  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_TopologyChange,  L7_NULLPTR, &SnmpTrapSendCallback_switching);
}


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
L7_RC_t SnmpNewRootTrapSend()
{
  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_NewRoot,  L7_NULLPTR, &SnmpTrapSendCallback_switching);
}

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
L7_RC_t SnmpStpInstanceTopologyChangeTrapSend(L7_int32 instanceID)
{
  L7_SNMP_TRAP_SWITCHING_instanceID_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_instanceID_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->instanceID = instanceID;

  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_StpInstanceTopologyChange, (void*)trap, &SnmpTrapSendCallback_switching);
}


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
L7_RC_t SnmpStpInstanceNewRootTrapSend(L7_int32 instanceID)
{
  L7_SNMP_TRAP_SWITCHING_instanceID_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_instanceID_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->instanceID = instanceID;

  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_StpInstanceNewRoot, (void*)trap, &SnmpTrapSendCallback_switching);
}

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
L7_RC_t SnmpStpInstanceLoopInconsistentStartTrapSend(L7_int32 instanceID, L7_uint32 ifIndex)
{
  L7_SNMP_TRAP_SWITCHING_instanceID_ifIndex_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_instanceID_ifIndex_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->instanceID = instanceID;
  trap->ifIndex = ifIndex;

  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_StpInstanceLoopInconsistentStart, (void*)trap, &SnmpTrapSendCallback_switching);
}

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
L7_RC_t SnmpStpInstanceLoopInconsistentEndTrapSend(L7_int32 instanceID, L7_uint32 ifIndex)
{
  L7_SNMP_TRAP_SWITCHING_instanceID_ifIndex_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_instanceID_ifIndex_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->instanceID = instanceID;
  trap->ifIndex = ifIndex;
  
  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_StpInstanceLoopInconsistentEnd, (void*)trap, &SnmpTrapSendCallback_switching);
}

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
L7_RC_t SnmpAgentInventoryCardMismatchTrapSend(L7_int32 unitIndex, L7_int32 slotIndex, L7_int32 insertedType, L7_int32 configuredType)
{
  L7_SNMP_TRAP_SWITCHING_AgentInventoryCardMismatch_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_AgentInventoryCardMismatch_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->unitIndex = unitIndex;
  trap->slotIndex = slotIndex;
  trap->insertedType = insertedType;
  trap->configuredType = configuredType;

#ifndef L7_CHASSIS
  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_AgentInventoryCardMismatch, (void*)trap, &SnmpTrapSendCallback_switching);
#else
  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_AgentChassisCardMismatch, (void*)trap, &SnmpTrapSendCallback_switching);
#endif

}

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
L7_RC_t SnmpAgentInventoryCardUnsupportedTrapSend(L7_int32 unitIndex, L7_int32 slotIndex, L7_int32 insertedType)
{

  L7_SNMP_TRAP_SWITCHING_AgentInventoryCardUnsupported_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_AgentInventoryCardUnsupported_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->unitIndex = unitIndex;
  trap->slotIndex = slotIndex;
  trap->insertedType = insertedType;

#ifndef L7_CHASSIS
  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_AgentInventoryCardUnsupported, (void*)trap, &SnmpTrapSendCallback_switching);
#else
  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_AgentChassisCardUnsupported, (void*)trap, &SnmpTrapSendCallback_switching);
#endif
}


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
L7_RC_t SnmpAgentInventoryStackPortLinkUpTrapSend(L7_uint32 agentInventoryStackPortIndex, L7_uint32 agentInventoryStackPortUnit, L7_char8 *agentInventoryStackPortTag)
{
  L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPort_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPort_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->agentInventoryStackPortIndex = agentInventoryStackPortIndex;
  trap->agentInventoryStackPortUnit = agentInventoryStackPortUnit;
  strncpy(trap->agentInventoryStackPortTag, agentInventoryStackPortTag, sizeof(trap->agentInventoryStackPortTag));

#ifndef L7_CHASSIS
  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPortLinkUp, (void*)trap, &SnmpTrapSendCallback_switching);
#else
  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_AgentChassisBackplanePortLinkUp, (void*)trap, &SnmpTrapSendCallback_switching);
#endif
}

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
L7_RC_t SnmpAgentInventoryStackPortLinkDownTrapSend(L7_uint32 agentInventoryStackPortIndex, L7_uint32 agentInventoryStackPortUnit, L7_char8 *agentInventoryStackPortTag)
{
  L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPort_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPort_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->agentInventoryStackPortIndex = agentInventoryStackPortIndex;
  trap->agentInventoryStackPortUnit = agentInventoryStackPortUnit;
  strncpy(trap->agentInventoryStackPortTag, agentInventoryStackPortTag, sizeof(trap->agentInventoryStackPortTag));

#ifndef L7_CHASSIS
  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_AgentInventoryStackPortLinkDown, (void*)trap, &SnmpTrapSendCallback_switching);
#else
  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_AgentChassisBackplanePortLinkDown, (void*)trap, &SnmpTrapSendCallback_switching);
#endif
}

/*********************************************************************
*
* @purpose  Send entConfigChange Trap
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if error
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t SnmpEntConfigChangeTrapSend()
{
  if ( SnmpStatusGet() == L7_ENABLE &&
       snmp_entConfigChangeTrapSend() == 0 )
    return L7_SUCCESS;

  return L7_FAILURE;
}

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
L7_RC_t SnmpFailedUserLoginTrapSend()
{
  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_FailedUserLogin,  L7_NULLPTR, &SnmpTrapSendCallback_switching);
}

/*********************************************************************
*
* @purpose  Send lockedUserLogin Trap
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t SnmpLockedUserLoginTrapSend()
{
  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_LockedUserLogin,  L7_NULLPTR, &SnmpTrapSendCallback_switching);
}

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
L7_RC_t SnmpDaiIntfErrorDisabledTrapSend(L7_uint32 ifIndex)
{
  L7_SNMP_TRAP_SWITCHING_ifIndex_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_ifIndex_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->ifIndex = ifIndex;

  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_DaiIntfErrorDisabled, (void*)trap, &SnmpTrapSendCallback_switching);
}

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
L7_RC_t SnmpDsIntfErrorDisabledTrapSend(L7_uint32 ifIndex)
{
  L7_SNMP_TRAP_SWITCHING_ifIndex_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_ifIndex_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->ifIndex = ifIndex;

  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_DsIntfErrorDisabled, (void*)trap, &SnmpTrapSendCallback_switching);
}

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
L7_RC_t SnmpNoStartupConfigTrapSend ()
{
  return snmpTrapSend(L7_SNMP_TRAP_BASE_noStartupConfig, L7_NULLPTR, &SnmpTrapSendCallback_switching);
}
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
L7_RC_t SnmpAgentChassisRisingAlarmReportTrapSend(L7_uint32 alarmId)
{
  L7_SNMP_TRAP_SWITCHING_alarmId_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_alarmId_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->alarmId = alarmId;

  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_AgentChassisRisingAlarm, (void*)trap, &SnmpTrapSendCallback_switching);
}

/*********************************************************************
*
* @purpose  Sent when a ChassisAlarm is cleared.
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
L7_RC_t SnmpAgentChassisFallingAlarmReportTrapSend(L7_uint32 alarmId)
{
  L7_SNMP_TRAP_SWITCHING_alarmId_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_alarmId_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->alarmId = alarmId;

  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_AgentChassisFallingAlarm, (void*)trap, &SnmpTrapSendCallback_switching);
}
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
L7_RC_t SnmpAgentTemperatureChangeReportTrapSend(L7_uint32 sensorNum, L7_uint32 tempEventType)
{
  L7_SNMP_TRAP_SWITCHING_temperatureEvent_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_temperatureEvent_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->sensorNum = sensorNum;
  trap->tempEventType = tempEventType;

  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_TemperatureChange, (void*)trap, &SnmpTrapSendCallback_switching);
}


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
L7_RC_t SnmpAgentFanStateChangeReportTrapSend(L7_uint32 itemNum, L7_uint32 fanEventType)
{
  L7_SNMP_TRAP_SWITCHING_itemStateEvent_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_itemStateEvent_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->itemNum = itemNum;
  trap->itemState = fanEventType;

  return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_FanStateChange, (void*)trap, &SnmpTrapSendCallback_switching);
}

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
L7_RC_t SnmpAgentPowerSupplyStateChangeReportTrapSend(L7_uint32 itemNum, L7_uint32 powSupplyEventType)
{
	L7_SNMP_TRAP_SWITCHING_itemStateEvent_t *trap = L7_NULLPTR;

	trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_itemStateEvent_t));

	if (trap == L7_NULLPTR)
	  return L7_ERROR;

	trap->itemNum = itemNum;
	trap->itemState = powSupplyEventType;

	return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_PowSupplyStateChange, (void*)trap, &SnmpTrapSendCallback_switching);
}
/* End Function Declarations */

void SnmpTestTraps_switching()
{
  L7_RC_t rc;

  rc = SnmpColdStartTrapSend();
  sysapiPrintf("SnmpColdStartTrapSend -> %d\n", rc);

  rc = SnmpWarmStartTrapSend();
  sysapiPrintf("SnmpWarmStartTrapSend -> %d\n", rc);

  rc = SnmpLinkDownTrapSend(1);
  sysapiPrintf("SnmpLinkDownTrapSend -> %d\n", rc);

  rc = SnmpLinkUpTrapSend(1);
  sysapiPrintf("SnmpLinkUpTrapSend -> %d\n", rc);

  rc = SnmpAuthenFailureTrapSend();
  sysapiPrintf("SnmpAuthenFailureTrapSend -> %d\n", rc);

  rc = SnmpFanFailureTrapSend();
  sysapiPrintf("SnmpFanFailureTrapSend -> %d\n", rc);

  rc = SnmpMacLockViolationTrapSend(1);
  sysapiPrintf("SnmpMacLockViolationTrapSend -> %d\n", rc);

  rc = SnmpPowerSupplyStatusChangeTrapSend();
  sysapiPrintf("SnmpPowerSupplyStatusChangeTrapSend -> %d\n", rc);

  rc = SnmpVlanRestoreFailureTrapSend(1);
  sysapiPrintf("SnmpVlanRestoreFailureTrapSend -> %d\n", rc);

  rc = SnmpVlanDefaultCfgFailureTrapSend(1);
  sysapiPrintf("SnmpVlanDefaultCfgFailureTrapSend -> %d\n", rc);

  rc = SnmpVlanDeleteLastTrapSend(1);
  sysapiPrintf("SnmpVlanDeleteLastTrapSend -> %d\n", rc);

  rc = SnmpLinkFailureTrapSend();
  sysapiPrintf("SnmpLinkFailureTrapSend -> %d\n", rc);

  rc = SnmpBroadcastStormEndTrapSend();
  sysapiPrintf("SnmpBroadcastStormEndTrapSend -> %d\n", rc);

  rc = SnmpBroadcastStormStartTrapSend();
  sysapiPrintf("SnmpBroadcastStormStartTrapSend -> %d\n", rc);

  rc = SnmpMultipleUsersTrapSend();
  sysapiPrintf("SnmpMultipleUsersTrapSend -> %d\n", rc);

  rc = SnmpFallingAlarmTrapSend(2);
  sysapiPrintf("SnmpFallingAlarmTrapSend -> %d\n", rc);

  rc = SnmpRisingAlarmTrapSend(1);
  sysapiPrintf("SnmpRisingAlarmTrapSend -> %d\n", rc);

  rc = SnmpTopologyChangeTrapSend();
  sysapiPrintf("SnmpTopologyChangeTrapSend -> %d\n", rc);

  rc = SnmpNewRootTrapSend();
  sysapiPrintf("SnmpNewRootTrapSend -> %d\n", rc);

  rc = SnmpStpInstanceTopologyChangeTrapSend(1);
  sysapiPrintf("SnmpStpInstanceTopologyChangeTrapSend -> %d\n", rc);

  rc = SnmpStpInstanceNewRootTrapSend(1);
  sysapiPrintf("SnmpStpInstanceNewRootTrapSend -> %d\n", rc);

  rc = SnmpStpInstanceLoopInconsistentStartTrapSend(1, 2); 
  sysapiPrintf("SnmpStpInstanceLoopInconsistentStartTrapSend -> %d\n", rc);

  rc = SnmpStpInstanceLoopInconsistentEndTrapSend(1, 2);
  sysapiPrintf("SnmpStpInstanceLoopInconsistentEndTrapSend -> %d\n", rc);

  rc = SnmpAgentInventoryCardMismatchTrapSend(1, 2, 3, 4);
  sysapiPrintf("SnmpAgentInventoryCardMismatchTrapSend -> %d\n", rc);

  rc = SnmpAgentInventoryCardUnsupportedTrapSend(1, 2, 3);
  sysapiPrintf("SnmpAgentInventoryCardUnsupportedTrapSend -> %d\n", rc);

  rc = SnmpAgentInventoryStackPortLinkUpTrapSend(1, 2, "abc");
  sysapiPrintf("SnmpAgentInventoryStackPortLinkUpTrapSend -> %d\n", rc);

  rc = SnmpAgentInventoryStackPortLinkDownTrapSend(1, 2, "abc");
  sysapiPrintf("SnmpAgentInventoryStackPortLinkDownTrapSend -> %d\n", rc);

  rc = SnmpEntConfigChangeTrapSend();
  sysapiPrintf("SnmpEntConfigChangeTrapSend -> %d\n", rc);

  rc = SnmpFailedUserLoginTrapSend();
  sysapiPrintf("SnmpFailedUserLoginTrapSend -> %d\n", rc);

  rc = SnmpLockedUserLoginTrapSend();
  sysapiPrintf("SnmpLockedUserLoginTrapSend -> %d\n", rc);

  rc = SnmpDaiIntfErrorDisabledTrapSend(1);
  sysapiPrintf("SnmpDaiIntfErrorDisabledTrapSend -> %d\n", rc);

  rc = SnmpDsIntfErrorDisabledTrapSend(1);
  sysapiPrintf("SnmpDsIntfErrorDisabledTrapSend -> %d\n", rc);

#ifdef FEAT_METRO_CPE_V1_0
  rc = SnmpNoStartupConfigTrapSend();
  sysapiPrintf("SnmpNoStartupConfigTrapSend -> %d\n", rc);
#endif

  rc = SnmpAgentTemperatureChangeReportTrapSend(0, 1);
  sysapiPrintf("SnmpAgentTemperatureChangeReportTrapSend(0, 1)-> %d\n", rc);

  rc = SnmpAgentFanStateChangeReportTrapSend(0, 1);
  sysapiPrintf("SnmpAgentFanStateChangeReportTrapSend(0, 1)-> %d\n", rc);

  rc = SnmpAgentPowerSupplyStateChangeReportTrapSend(0, 1);
  sysapiPrintf("SnmpAgentPowerSupplyStateChangeReportTrapSend(0, 1)-> %d\n", rc);

  return;
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
L7_RC_t SnmpDot1agCfmFaultAlarmTrapSend(L7_uint32 mdIndex, L7_uint32 maIndex, L7_uint32 mepId, L7_uint32 highestPriDefect)
{
 	L7_SNMP_TRAP_SWITCHING_dot1agCfmFaultAlarmNotify_t *trap = L7_NULLPTR;

	trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_SWITCHING_dot1agCfmFaultAlarmNotify_t));

	if (trap == L7_NULLPTR)
	  return L7_ERROR;

	trap->mdIndex = mdIndex;
	trap->maIndex = maIndex;
  trap->mepId = mepId;
  trap->highestPriDefect = highestPriDefect;

	return snmpTrapSend(L7_SNMP_TRAP_SWITCHING_Dot1agCfmFaultAlarmNotify, (void*)trap, &SnmpTrapSendCallback_switching);
}
#endif /* L7_DOT1AG_PACKAGE */
