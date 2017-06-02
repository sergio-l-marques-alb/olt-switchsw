/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename  dtl_l2_vlan_ipsubnet.c
*
* @purpose   This file contains the functions to transform layer 2
*            IP subnet VLAN component's requests into driver requests.
*
* @component dtl    
*
* @comments
*
* @create   05/20/2005 
*
* @author   tsrikanth 
*
* @end
*
**********************************************************************/

#include "dtlinclude.h"
                           

/**********************************************************************
 * @purpose Apply a configuration for IP VLAN
 *
 * @param   cmd        @b{(input)} Command
 * @param   subnet     @b{(input)} IP subnet
 * @param   netmask    @b{(input)} network mask
 * @param   vlanid     @b{(input)} Vlan identifier
 *
 * @returns L7_SUCCESS if success
 * @returns L7_FAILURE if failure
 *
 * @notes none
 *
 * @end
*********************************************************************/
L7_RC_t  dtlVlanIpSubnetConfig(L7_uint32 cmd, L7_uint32 subnet,
                                   L7_uint32 netmask,L7_uint32 vlanid)
{
  DAPI_USP_t ddUsp;
  DAPI_QVLAN_MGMT_CMD_t dapiCmd;
  L7_RC_t rc;

  ddUsp.unit = 0;
  ddUsp.slot = 0;
  ddUsp.port = 0;

  dapiCmd.cmdData.ipSubnetVlanConfig.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.ipSubnetVlanConfig.vlanId = vlanid;
  dapiCmd.cmdData.ipSubnetVlanConfig.ipSubnet = subnet;
  dapiCmd.cmdData.ipSubnetVlanConfig.netMask = netmask;

  rc = dapiCtl(&ddUsp, cmd, &dapiCmd);

  if (rc != L7_FAILURE)
    rc = L7_SUCCESS;

  return (rc);
}



/*********************************************************************
* @purpose  Create an IP Subnet VLAN
*          
* @param    subnet      @b{(input)}    IP Subnet
* @param    netmask     @b{(input)}    network mask
* @param    vlanId      @b{(input)}    vlan ID 
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
* 
* @end
*********************************************************************/
L7_RC_t dtlVlanIpSubnetCreate(L7_uint32 subnet,L7_uint32 netmask,L7_uint32 vlanid)
{
  L7_RC_t rc;

  rc = dtlVlanIpSubnetConfig(DAPI_CMD_IPSUBNET_VLAN_CREATE,subnet, netmask, vlanid);
  return (rc);
}



/*********************************************************************
* @purpose  Delete an IP Subnet VLAN
*          
* @param    subnet      @b{(input)}    IP Subnet
* @param    netmask     @b{(input)}    network mask
* @param    vlanId      @b{(input)}    vlan ID 
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
* 
* @end
*********************************************************************/
L7_RC_t dtlVlanIpSubnetDelete(L7_uint32 subnet,L7_uint32 netmask,L7_uint32 vlanid)
{
  L7_RC_t rc;

  rc = dtlVlanIpSubnetConfig(DAPI_CMD_IPSUBNET_VLAN_DELETE,subnet, netmask, vlanid);
  return (rc);
}
