
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   k_mib_fastpath_pbvlan_api.h
*
* @purpose    Wrapper functions for Fastpath PBVLAN
*
* @component  SNMP
*
* @comments
*
* @create     3/6/2007
*
* @author     akulkarni
* @end
*
**********************************************************************/

#ifndef __K_MIB_FASTPATH_PBVLAN_API_H__
#define __K_MIB_FASTPATH_PBVLAN_API_H__

/**************************************************************************************************************/

L7_RC_t
snmpAgentProtocolGroupCreateSet(L7_uint32 UnitIndex, L7_uint32 groupID);
L7_RC_t
snmpAgentProtocolGroupVlanIdSet(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 val);

L7_RC_t
snmpAgentProtocolGroupProtocolIPGet(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 *val);

L7_RC_t
snmpAgentProtocolGroupProtocolIPSet(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 val);

L7_RC_t
snmpAgentProtocolGroupProtocolARPGet(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 *val);

L7_RC_t
snmpAgentProtocolGroupProtocolARPSet(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 val);

L7_RC_t
snmpAgentProtocolGroupProtocolIPXGet(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 *val);

L7_RC_t
snmpAgentProtocolGroupProtocolIPXSet(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 val);

L7_RC_t
snmpAgentProtocolGroupStatusSet(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 val);

L7_RC_t
snmpAgentProtocolGroupPortEntryGetExact(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 ifIndex);

L7_RC_t
snmpAgentProtocolGroupPortEntryGetNext(L7_uint32 UnitIndex, L7_uint32 *groupId, L7_uint32 *ifIndex);

L7_RC_t
snmpAgentProtocolGroupProtocolEntryGetNext(L7_uint32 UnitIndex, L7_uint32 *groupId, L7_uint32 *proto);

#endif /* __K_MIB_FASTPATHDVLAN_API_H__ */

