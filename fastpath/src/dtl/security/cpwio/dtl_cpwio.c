/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename dtl_cpwio.c
*
* @purpose DTL interface for Wired Captive Portal
*
* @component DTL
*
* @comments none
*
* @create 04/09/2008
*
* @author nshrivastav
*
* @end
*             
**********************************************************************/

#include "l7_common.h"
#include "nimapi.h"
#include "dapi.h"

/*********************************************************************
* @purpose  Enable/disable Captive Portal on an Interface
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    mode      @b{(input)} L7_TRUE enable, L7_FALSE disable
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlCaptivePortalConfig(L7_uint32 intIfNum, L7_BOOL mode)
{
  nimUSP_t                usp;
  DAPI_USP_t              dapiUsp;
  DAPI_INTF_MGMT_CMD_t    cmd;
  L7_RC_t                 rc = L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    dapiUsp.unit = usp.unit;
    dapiUsp.slot = usp.slot;
    dapiUsp.port = usp.port - 1;

    cmd.cmdData.captivePortalPortState.getOrSet = DAPI_CMD_SET;
    if (mode == L7_TRUE) 
    {
      cmd.cmdData.captivePortalPortState.cpState  = DAPI_PORT_CAPTIVE_PORTAL_ENABLED;
    }
    else
    {
      cmd.cmdData.captivePortalPortState.cpState  = DAPI_PORT_CAPTIVE_PORTAL_DISABLED;
    }

    rc = dapiCtl(&dapiUsp, DAPI_CMD_INTF_CAPTIVE_PORTAL_CONFIG, &cmd);
  }

  return rc;
}

/*********************************************************************
* @purpose  Block/Unblock an Interface
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    mode      @b{(input)}  L7_TRUE block, L7_FALSE unblock
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlCaptivePortalBlock(L7_uint32 intIfNum, L7_BOOL mode)
{
  nimUSP_t                    usp;
  DAPI_USP_t                  dapiUsp;
  DAPI_INTF_MGMT_CMD_t        cmd;
  L7_RC_t                     rc = L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    dapiUsp.unit = usp.unit;
    dapiUsp.slot = usp.slot;
    dapiUsp.port = usp.port - 1;

    cmd.cmdData.captivePortalPortState.getOrSet = DAPI_CMD_SET;
    if (mode == L7_TRUE) 
    {
      cmd.cmdData.captivePortalPortState.cpState  = DAPI_PORT_CAPTIVE_PORTAL_BLOCKED;
    }
    else
    {
      cmd.cmdData.captivePortalPortState.cpState  = DAPI_PORT_CAPTIVE_PORTAL_UNBLOCKED;
    }

    rc = dapiCtl(&dapiUsp, DAPI_CMD_INTF_CAPTIVE_PORTAL_CONFIG, &cmd);
  }

  return rc;
}
