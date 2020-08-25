/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  dtl_l2_arp_inspection.c
*
* @purpose   This file contains the functions to transform layer 2
*            Dynamic ARP Inspection requests into driver requests.
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
* @purpose  Configure Dynamic ARP Inspection on an interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    trusted     @b{(input)} Intf is trusted for ARP
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dtlDynamicArpInspectionConfig(L7_uint32 intIfNum,
                                      L7_BOOL   trusted)
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

    cmd.cmdData.dynamicArpInspectionConfig.getOrSet = DAPI_CMD_SET;
    cmd.cmdData.dynamicArpInspectionConfig.trusted = trusted;

    rc = dapiCtl(&ddUsp, DAPI_CMD_INTF_DYNAMIC_ARP_INSPECTION_CONFIG, &cmd);
  }

  return rc;
}


/*
 * *******************************************************************
 *                           PRIVATE FUNCTIONS
 * *******************************************************************
 */
