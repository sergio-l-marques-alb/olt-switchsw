/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   k_mib_fastpathdvlan_api.c
*
* @purpose    Wrapper functions for Fastpath Mac based VLAN
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
#include "k_mib_fastpath_macvlan_api.h"
#include "usmdb_vlan_mac_api.h"
#include "usmdb_common.h"

L7_RC_t
snmpSwitchVlanMacAssociationEntryGet(L7_char8* macAddress,
                                  L7_uint32 agentSwitchVlanMacAssociationVlanId)
{
  L7_enetMacAddr_t    macAddr;
  L7_uint32 temp;

  memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));
  memcpy(&macAddr.addr, macAddress, L7_ENET_MAC_ADDR_LEN);

  if((usmDbVlanMacGet(USMDB_UNIT_CURRENT,macAddr,&temp) == L7_SUCCESS) &&
     (temp == agentSwitchVlanMacAssociationVlanId))
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

L7_RC_t
snmpSwitchVlanMacAssociationEntryGetNext(L7_char8* macAddress,
             L7_uint32 *agentSwitchVlanMacAssociationVlanId)
{
  L7_enetMacAddr_t    macAddr;
  L7_uint32 temp;
  memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));
  memcpy(&macAddr.addr, macAddress, L7_ENET_MAC_ADDR_LEN);

  if((usmDbVlanMacGet(USMDB_UNIT_CURRENT, macAddr, &temp) == L7_SUCCESS) &&
     (temp > *agentSwitchVlanMacAssociationVlanId))
  {
    *agentSwitchVlanMacAssociationVlanId = temp;
  }
  else
  {
    if(usmDbVlanMacGetNext(USMDB_UNIT_CURRENT, macAddr, &macAddr,
                           agentSwitchVlanMacAssociationVlanId) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    memset(macAddress, 0x00, L7_ENET_MAC_ADDR_LEN);
    memcpy(macAddress, macAddr.addr, sizeof(L7_enetMacAddr_t));
  }
  return L7_SUCCESS;
}

L7_RC_t snmpSwitchVlanMacAssociationEntryAdd(L7_char8* macAddress,
                                  L7_uint32 agentSwitchVlanMacAssociationVlanId)
{
  L7_enetMacAddr_t    macAddr;
  memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));
  memcpy(&macAddr.addr, macAddress, L7_ENET_MAC_ADDR_LEN);

  return usmDbVlanMacAdd(USMDB_UNIT_CURRENT, macAddr,
                         agentSwitchVlanMacAssociationVlanId);
  return L7_SUCCESS;
}

L7_RC_t snmpSwitchVlanMacAssociationEntryDelete(L7_char8* macAddress,
                                  L7_uint32 agentSwitchVlanMacAssociationVlanId)
{
  L7_enetMacAddr_t    macAddr;
  memset(&macAddr, 0x00, sizeof(L7_enetMacAddr_t));
  memcpy(&macAddr.addr, macAddress, L7_ENET_MAC_ADDR_LEN);
  return usmDbVlanMacDelete(USMDB_UNIT_CURRENT, macAddr,
                            agentSwitchVlanMacAssociationVlanId);
  return L7_SUCCESS;
}



