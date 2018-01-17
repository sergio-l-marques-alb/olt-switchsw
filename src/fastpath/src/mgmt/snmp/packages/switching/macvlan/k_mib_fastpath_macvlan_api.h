
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   k_mib_fastpathdvlan_api.h
*
* @purpose    Wrapper interface for Fastpath Mac based VLAN
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

#ifndef __K_MIB_FASTPATH_MACVLAN_API_H__
#define __K_MIB_FASTPATH_MACVLAN_API_H__

L7_RC_t
snmpSwitchVlanMacAssociationEntryGet(L7_char8* macAddress,
                                  L7_uint32 agentSwitchVlanMacAssociationVlanId);

L7_RC_t
snmpSwitchVlanMacAssociationEntryGetNext(L7_char8* macAddress,
             L7_uint32 *agentSwitchVlanMacAssociationVlanId);

L7_RC_t snmpSwitchVlanMacAssociationEntryAdd(L7_char8* macAddress,
                                  L7_uint32 agentSwitchVlanMacAssociationVlanId);

L7_RC_t snmpSwitchVlanMacAssociationEntryDelete(L7_char8* macAddress,
                                  L7_uint32 agentSwitchVlanMacAssociationVlanId);

#endif /* __K_MIB_FASTPATHDVLAN_API_H__ */

