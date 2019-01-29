/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   k_mib_fastpath_pbvlan_api.c
*
* @purpose    Wrapper functions for Fastpath IPsubnet VLAN
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

#include "k_private_base.h"
#include "k_mib_fastpath_ipvlan_api.h"
#include "usmdb_vlan_ipsubnet_api.h"
#include "usmdb_common.h"

L7_RC_t
snmpSwitchVlanSubnetAssociationEntryGet(L7_uint32 agentSwitchVlanSubnetAssociationIPAddress,
                     L7_uint32 agentSwitchVlanSubnetAssociationSubnetMask,
                     L7_uint32 agentSwitchVlanSubnetAssociationVlanId)
{
  L7_uint32 temp;
  if((usmDbVlanIpSubnetSubnetGet(USMDB_UNIT_CURRENT,
                                 agentSwitchVlanSubnetAssociationIPAddress,
                                 agentSwitchVlanSubnetAssociationSubnetMask,
                                 &temp) == L7_SUCCESS) &&
     (temp == agentSwitchVlanSubnetAssociationVlanId))
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

L7_RC_t
snmpSwitchVlanSubnetAssociationEntryGetNext(L7_uint32 *agentSwitchVlanSubnetAssociationIPAddress,
                    L7_uint32 *agentSwitchVlanSubnetAssociationSubnetMask,
                    L7_uint32 *agentSwitchVlanSubnetAssociationVlanId)
{
  L7_uint32 temp;
  if((usmDbVlanIpSubnetSubnetGet(USMDB_UNIT_CURRENT,
                                 *agentSwitchVlanSubnetAssociationIPAddress,
                                 *agentSwitchVlanSubnetAssociationSubnetMask,
                                 &temp) == L7_SUCCESS) &&
    (temp > *agentSwitchVlanSubnetAssociationVlanId))
  {
    *agentSwitchVlanSubnetAssociationVlanId = temp;
  }
  else
  {
    return usmDbVlanIpSubnetSubnetGetNext(USMDB_UNIT_CURRENT,
                                *agentSwitchVlanSubnetAssociationIPAddress,
                                *agentSwitchVlanSubnetAssociationSubnetMask,
                                agentSwitchVlanSubnetAssociationIPAddress,
                                agentSwitchVlanSubnetAssociationSubnetMask,
                                agentSwitchVlanSubnetAssociationVlanId);
  }
  return L7_SUCCESS;
}

L7_RC_t snmpSwitchVlanSubnetAssociationEntryAdd(L7_uint32 agentSwitchVlanSubnetAssociationIPAddress,
                         L7_uint32 agentSwitchVlanSubnetAssociationSubnetMask,
                         L7_uint32 agentSwitchVlanSubnetAssociationVlanId)
{
  return usmDbVlanIpSubnetSubnetAdd(USMDB_UNIT_CURRENT,
                                    agentSwitchVlanSubnetAssociationIPAddress,
                                    agentSwitchVlanSubnetAssociationSubnetMask,
                                    agentSwitchVlanSubnetAssociationVlanId);
}

L7_RC_t snmpSwitchVlanSubnetAssociationEntryDelete(
                         L7_uint32 agentSwitchVlanSubnetAssociationIPAddress,
                         L7_uint32 agentSwitchVlanSubnetAssociationSubnetMask,
                         L7_uint32 agentSwitchVlanSubnetAssociationVlanId)
{
  return usmDbVlanIpSubnetSubnetDelete(USMDB_UNIT_CURRENT,
                                     agentSwitchVlanSubnetAssociationIPAddress,
                                     agentSwitchVlanSubnetAssociationSubnetMask,
                                     agentSwitchVlanSubnetAssociationVlanId);
}

