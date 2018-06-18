/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2005-2007
 *
 **********************************************************************
 *
 * @filename  dtl_l2_voice_vlan.c
 *
 * @purpose   This file contains the voice vlan dtl APIs
 *
 * @component hapi voice vlan
 *
 * @comments
 *
 * @create   01/22/2007 
 *
 * @author   PKB
 *
 * @end
 *
 **********************************************************************/

#include "dtlinclude.h"

/*********************************************************************
* @purpose  Add a voice device on the specified interface
*
* @param    intIfNum @b{(input)} internal interface number
* @param    mac_addr @b{(input)} mac address of the voip device
* @param    vlanID   @b{(input)} vlan Id associated with this device
*                                 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlVoiceVlanDeviceAdd(L7_uint32 intIfNum,L7_enetMacAddr_t macAddr,L7_uint32 vlanId)
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  DAPI_INTF_MGMT_CMD_t cmd;
  
  L7_RC_t rc = L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    cmd.cmdData.voiceVlanDeviceAdd.getOrSet = DAPI_CMD_SET;
    cmd.cmdData.voiceVlanDeviceAdd.macAddr = macAddr;
    cmd.cmdData.voiceVlanDeviceAdd.vlanId = vlanId;
    
    rc = dapiCtl(&ddUsp,DAPI_CMD_VOICE_VLAN_DEVICE_ADD, &cmd);
  }
  return rc;
}

/*********************************************************************
* @purpose  Remove a voice device on the specified interface
*
* @param    intIfNum @b{(input)} internal interface number
* @param    mac_addr @b{(input)} mac address of the voip device
* @param    vlanID   @b{(input)} vlan Id associated with this device
*                                 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlVoiceVlanDeviceRemove(L7_uint32 intIfNum,L7_enetMacAddr_t macAddr,L7_uint32 vlanId)
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  DAPI_INTF_MGMT_CMD_t cmd;
  
  L7_RC_t rc = L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    cmd.cmdData.voiceVlanDeviceAdd.getOrSet = DAPI_CMD_SET;
    cmd.cmdData.voiceVlanDeviceAdd.macAddr = macAddr;
    cmd.cmdData.voiceVlanDeviceAdd.vlanId = vlanId;
    
    rc = dapiCtl(&ddUsp,DAPI_CMD_VOICE_VLAN_DEVICE_REMOVE, &cmd);
  }
  return rc;
}

/*********************************************************************
* @purpose  Set the COS Override mode on the specified interface
*
* @param    intIfNum @b{(input)} internal interface number
* @param    flag     @b{(input)} Either set or reset
*                                 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlVoiceVlanCosOverrideSet(L7_uint32 intIfNum,L7_BOOL flag)
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  DAPI_INTF_MGMT_CMD_t cmd;
  
  L7_RC_t rc = L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    cmd.cmdData.voiceVlanCosOverride.getOrSet = DAPI_CMD_SET;
    cmd.cmdData.voiceVlanCosOverride.enable = flag;
    
    rc = dapiCtl(&ddUsp,DAPI_CMD_VOICE_VLAN_COS_OVERRIDE, &cmd);
  }
  return rc;
}

/*********************************************************************
* @purpose  Set the Auth State on the specified interface
*
* @param    intIfNum @b{(input)} internal interface number
* @param    flag     @b{(input)} Either set or reset
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlVoiceVlanAuthSet(L7_uint32 intIfNum, L7_BOOL flag)
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  DAPI_INTF_MGMT_CMD_t cmd;
  
  L7_RC_t rc = L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;
    
    memset( &(cmd.cmdData.voiceVlanAuth), 0, sizeof(cmd.cmdData.voiceVlanAuth));

    cmd.cmdData.voiceVlanAuth.getOrSet = DAPI_CMD_SET;
    cmd.cmdData.voiceVlanAuth.enable = flag;
    rc = dapiCtl(&ddUsp, DAPI_CMD_VOICE_VLAN_AUTH, &cmd);
  }
  return rc;
}

