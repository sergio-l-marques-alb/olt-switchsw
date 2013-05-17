
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   k_mib_fastpathdvlan_api.h
*
* @purpose    Wrapper functions interface for Fastpath IP Subnet VLAN
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

#ifndef __K_MIB_FASTPATH_IPVLAN_API_H__
#define __K_MIB_FASTPATH_IPVLAN_API_H__

L7_RC_t
snmpSwitchVlanSubnetAssociationEntryGet(L7_uint32 agentSwitchVlanSubnetAssociationIPAddress,
                     L7_uint32 agentSwitchVlanSubnetAssociationSubnetMask,
                     L7_uint32 agentSwitchVlanSubnetAssociationVlanId);

L7_RC_t
snmpSwitchVlanSubnetAssociationEntryGetNext(L7_uint32 *agentSwitchVlanSubnetAssociationIPAddress,
                    L7_uint32 *agentSwitchVlanSubnetAssociationSubnetMask,
                    L7_uint32 *agentSwitchVlanSubnetAssociationVlanId);

L7_RC_t snmpSwitchVlanSubnetAssociationEntryAdd(L7_uint32 agentSwitchVlanSubnetAssociationIPAddress,
                         L7_uint32 agentSwitchVlanSubnetAssociationSubnetMask,
                         L7_uint32 agentSwitchVlanSubnetAssociationVlanId);

L7_RC_t snmpSwitchVlanSubnetAssociationEntryDelete(
                         L7_uint32 agentSwitchVlanSubnetAssociationIPAddress,
                         L7_uint32 agentSwitchVlanSubnetAssociationSubnetMask,
                         L7_uint32 agentSwitchVlanSubnetAssociationVlanId);


#endif /* __K_MIB_FASTPATH_IPVLAN_API_H__ */

