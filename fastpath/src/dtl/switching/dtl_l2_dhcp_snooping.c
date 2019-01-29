/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  dtl_l2_dhcp_snooping.c
*
* @purpose   This file contains the functions to transform layer 2
*            DHCP Snooping requests into driver requests.
*
* @component Device Transformation Layer (DTL)
*
* @comments  none
*
* @create    03/30/2007
*
* @author    colinw
*
* @end
*
*********************************************************************/

/*
 * *******************************************************************
 *                           HEADER FILES
 * *******************************************************************
 */

#include "dtlinclude.h"

/*
 * *******************************************************************
 *                           PRIVATE FUNCTIONS PROTOTYPES
 * *******************************************************************
 */

/*
 * *******************************************************************
 *                           API FUNCTIONS
 * *******************************************************************
 */

/*********************************************************************
* @purpose  Configure DHCP Snooping on an interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    enabled     @b{(input)} Intf is enabled for DHCP Snooping
* @param    trusted     @b{(input)} Intf is trusted for DHCP Snooping
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlDhcpSnoopingConfig(L7_uint32 intIfNum,
                              L7_BOOL   enabled)
{
  DAPI_INTF_MGMT_CMD_t cmd;
  nimUSP_t             usp;
  DAPI_USP_t           ddUsp;
  L7_RC_t              rc = L7_FAILURE;

  if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;

    cmd.cmdData.dhcpSnoopingConfig.getOrSet = DAPI_CMD_SET;
    cmd.cmdData.dhcpSnoopingConfig.enabled = enabled;

    rc = dapiCtl(&ddUsp, DAPI_CMD_INTF_DHCP_SNOOPING_CONFIG, &cmd);
  }

  return rc;
}


/*
 * *******************************************************************
 *                           PRIVATE FUNCTIONS
 * *******************************************************************
 */
