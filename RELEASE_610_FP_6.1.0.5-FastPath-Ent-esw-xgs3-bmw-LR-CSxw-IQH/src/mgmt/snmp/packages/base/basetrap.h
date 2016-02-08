/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: privatetrap.h
*
* Purpose: Private traps wrapper functions
*
* Created by: Colin Verne, Soma 05/08/2001
*
* Component:
*
*********************************************************************/

/**********************************************************************

**********************************************************************/
#ifndef PRIVATETRAP_H
#define PRIVATETRAP_H

#include <l7_common.h>

int
snmp_AuthenticationFailureTrapSend();

int
snmp_WarmStartTrapSend();

int
snmp_ColdStartTrapSend();

int
snmp_FanFailureTrapSend();

int
snmp_PowerSupplyStatusChangeTrapSend();

int
snmp_LinkFailureTrapSend();

int
snmp_BroadcastStormEndTrapSend();

int
snmp_BroadcastStormStartTrapSend();

int
snmp_MultipleUsersTrapSend();

int
snmp_FallingAlarmTrapSend( L7_int32 alarmIndex);

int
snmp_RisingAlarmTrapSend ( L7_int32 alarmIndex);


int
snmp_TopologyChangeTrapSend();

int
snmp_NewRootTrapSend();

int
snmp_LinkUpTrapSend( L7_int32 ifIndex);

int
snmp_LinkDownTrapSend( L7_int32 ifIndex);

int
snmp_failedUserLoginTrapSend();

L7_RC_t send_UserLoginTrap_trap();

int
snmp_lockedUserLoginTrapSend();

L7_RC_t send_UserLockoutTrap_trap();

int
snmp_daiIntfErrorDisabledTrapSend(L7_uint32 ifIndex);

L7_RC_t send_daiIntfErrorDisabled_trap(L7_uint32 ifIndex);

int
snmp_dhcpSnoopingIntfErrorDisabledTrapSend(L7_uint32 ifIndex);

L7_RC_t send_dhcpSnoopingIntfErrorDisabled_trap(L7_uint32 ifIndex);

#ifdef FEAT_METRO_CPE_V1_0
L7_RC_t snmp_noStartupConfigNotificationTrapSend() ;
#endif

#endif
