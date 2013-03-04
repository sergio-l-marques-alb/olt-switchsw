/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename dtl_l2_dot1x.c
*
* @purpose DTL interface
*
* @component DTL
*
* @comments none
*
* @create 04/08/2003
*
* @author mfiorito
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "log.h"
#include "nimapi.h"
#include "dot1x_api.h"
#include "dapi.h"

/*********************************************************************
* @purpose  Enable/disable dot1x
*
* @param    mode  @b{(input)} dot1x mode (L7_ENABLE/L7_DISABLE)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1xModeSet(L7_uint32 mode)
{
  DAPI_USP_t ddUsp;
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t ddRc;

  /* ddUsp must be a valid usp so set it to zeros */
  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dapiCmd.cmdData.dot1xConfig.getOrSet = DAPI_CMD_SET;
  if (mode == L7_ENABLE)
    dapiCmd.cmdData.dot1xConfig.enable = L7_TRUE;
  else
    dapiCmd.cmdData.dot1xConfig.enable = L7_FALSE;

  ddRc = dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_DOT1X_CONFIG, &dapiCmd);
  if (ddRc == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the authorization status of the specified interface
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    status    @b{(input)} dot1x port status
*                                 (L7_DOT1X_PORT_STATUS_AUTHORIZED/L7_DOT1X_PORT_STATUS_UNAUTHORIZED)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1xIntfStatusSet(L7_uint32 intIfNum, L7_DOT1X_PORT_STATUS_t status, L7_BOOL violationCallbackEnabled)
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  L7_RC_t ddRc;
  DAPI_INTF_MGMT_CMD_t cmd;
  L7_RC_t rc = L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    switch (status)
    {
    case L7_DOT1X_PORT_STATUS_AUTHORIZED:
    case L7_DOT1X_PORT_STATUS_UNAUTHORIZED:
      cmd.cmdData.dot1xStatus.status = status;
      cmd.cmdData.dot1xStatus.violationCallbackEnabled = violationCallbackEnabled;
      break;

    default:
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DTL_COMPONENT_ID,
              "received invalid dot1x status\n");
      return rc;
    }

    cmd.cmdData.dot1xStatus.getOrSet = DAPI_CMD_SET;

    ddRc = dapiCtl(&ddUsp, DAPI_CMD_INTF_DOT1X_STATUS, &cmd);
    if (ddRc == L7_SUCCESS)
      rc =  L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
* @purpose  Add an authorized client to an unauthorized port.
*
* @param    intIfNum   @b{(input)}  internal interface number
* @param    macAddr    @b{(input)}  MAC address of authorized client
* @param    vlanId     @b{(input)}  set to non-zero value to assign this client to a VLAN
* @param    pTLV       @b{(input)}  pass a non-NULL pointer to apply a policy for this client
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1xIntfClientAdd(L7_uint32 intIfNum, L7_enetMacAddr_t macAddr, L7_ushort16 vlanId, L7_tlv_t *pTLV)
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  L7_RC_t ddRc;
  DAPI_INTF_MGMT_CMD_t cmd;
  L7_RC_t rc = L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    cmd.cmdData.dot1xClientAdd.getOrSet = DAPI_CMD_SET;
    cmd.cmdData.dot1xClientAdd.macAddr  = macAddr;
    cmd.cmdData.dot1xClientAdd.vlanId   = vlanId;
    cmd.cmdData.dot1xClientAdd.pTLV     = pTLV;

    ddRc = dapiCtl(&ddUsp, DAPI_CMD_INTF_DOT1X_CLIENT_ADD, &cmd);
    if (ddRc == L7_SUCCESS)
      rc =  L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
* @purpose  Remove an authorized client from an unauthorized port.
*
* @param    intIfNum   @b{(input)}  internal interface number
* @param    macAddr    @b{(input)}  MAC address of authorized client
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1xIntfClientRemove(L7_uint32 intIfNum, L7_enetMacAddr_t macAddr)
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  L7_RC_t ddRc;
  DAPI_INTF_MGMT_CMD_t cmd;
  L7_RC_t rc = L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    cmd.cmdData.dot1xClientRemove.getOrSet = DAPI_CMD_SET;
    cmd.cmdData.dot1xClientRemove.macAddr  = macAddr;

    ddRc = dapiCtl(&ddUsp, DAPI_CMD_INTF_DOT1X_CLIENT_REMOVE, &cmd);
    if (ddRc == L7_SUCCESS)
      rc =  L7_SUCCESS;
  }

  return rc;
}



/*********************************************************************
* @purpose  Set port authentication mode
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    mode        @b{(input)} port control mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dtlDot1xIntfAuthModeSet( L7_uint32 intIfNum, L7_DOT1X_PORT_CONTROL_t mode)
{
  /* This function needs to be deprecated */
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  L7_RC_t rc = L7_FAILURE;

  if ( L7_SUCCESS == nimGetUnitSlotPort( intIfNum, &usp ) )
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    rc = L7_SUCCESS;
  }

  return( rc );
}

/*********************************************************************
* @purpose  Check if a client has timeout i.e. no acitivity from that client
*
* @param    intIfNum   @b{(input)}  internal interface number
* @param    macAddr    @b{(input)}  MAC address of authorized client
* @param    timeout    @b{(output)} Flag indicating if client has timed out.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1xIntfClientTimeoutGet(L7_uint32 intIfNum, L7_enetMacAddr_t macAddr,L7_BOOL *timeout)
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  L7_RC_t ddRc;
  DAPI_INTF_MGMT_CMD_t cmd;
  L7_RC_t rc = L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    cmd.cmdData.dot1xClientTimeout.getOrSet = DAPI_CMD_GET;
    cmd.cmdData.dot1xClientTimeout.macAddr  = macAddr;
    cmd.cmdData.dot1xClientTimeout.timeout = timeout;

    ddRc = dapiCtl(&ddUsp, DAPI_CMD_INTF_DOT1X_CLIENT_TIMEOUT, &cmd);
    if (ddRc == L7_SUCCESS)
      rc =  L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
* @purpose  Drop packets coming from the specified MAC/VLAN pair on intIfNum.
*
* @param    intIfNum   @b{(input)}  internal interface number
* @param    macAddr    @b{(input)}  MAC address of blocked client
* @param    vlanId     @b{(input)}  vlan ID
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1xIntfClientBlock(L7_uint32 intIfNum, L7_enetMacAddr_t macAddr, L7_ushort16 vlanId)
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  L7_RC_t ddRc;
  DAPI_INTF_MGMT_CMD_t cmd;
  L7_RC_t rc = L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    cmd.cmdData.dot1xClientBlock.getOrSet = DAPI_CMD_SET;
    cmd.cmdData.dot1xClientBlock.macAddr  = macAddr;
    cmd.cmdData.dot1xClientBlock.vlanId = vlanId;

    ddRc = dapiCtl(&ddUsp, DAPI_CMD_INTF_DOT1X_CLIENT_BLOCK, &cmd);
    if (ddRc == L7_SUCCESS)
      rc =  L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
* @purpose  Allow packets coming from the specified MAC/VLAN pair on intIfNum.
*
* @param    intIfNum   @b{(input)}  internal interface number
* @param    macAddr    @b{(input)}  MAC address of blocked client
* @param    vlanId     @b{(input)}  vlan ID
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1xIntfClientUnblock(L7_uint32 intIfNum, L7_enetMacAddr_t macAddr, L7_ushort16 vlanId)
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  L7_RC_t ddRc;
  DAPI_INTF_MGMT_CMD_t cmd;
  L7_RC_t rc = L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    cmd.cmdData.dot1xClientUnblock.getOrSet = DAPI_CMD_SET;
    cmd.cmdData.dot1xClientUnblock.macAddr  = macAddr;
    cmd.cmdData.dot1xClientUnblock.vlanId = vlanId;

    ddRc = dapiCtl(&ddUsp, DAPI_CMD_INTF_DOT1X_CLIENT_UNBLOCK, &cmd);
    if (ddRc == L7_SUCCESS)
      rc =  L7_SUCCESS;
  }

  return rc;
}
