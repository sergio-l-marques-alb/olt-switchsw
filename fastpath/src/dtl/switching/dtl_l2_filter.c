/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: dtl_l2_filter.c
*
* Purpose: This file contains the functions to transform the static MAC filtering
*          's driver requests. 
*
* Component: Device Transformation Layer (DTL)
*
* Commnets: 
*
* Created by: Shekhar Kalyanam 1/7/2001 
*
*********************************************************************/
/*************************************************************
                    
*************************************************************/





/*
**********************************************************************
*                           HEADER FILES
**********************************************************************
*/

#define DTLCTRL_L2_FILTER_GLOBALS              /* Enable global space   */    
#include "dtlinclude.h"

#if DTLCTRL_COMPONENT_L2_FILTER


/*
**********************************************************************
*                           PRIVATE FUNCTIONS PROTOTYPES 
**********************************************************************
*/


/*
**********************************************************************
*                           API FUNCTIONS 
**********************************************************************
*/
/*********************************************************************
* @purpose  Creates a Static MAC Filter
*
* @param    macAddr         @b{(input)} MAC Address of the Filter
* @param    vlanId          @b{(input)} vlan ID associated
* @param    numSrcIntf      @b{(input)} Number of source interfaces in the list 
* @param    srcIntfList[]   @b{(input)} Array of source interfaces in the Filter
* @param    numDstList      @b{(input)} Number of destination interfaces in the list
* @param    dstIntfList[]   @b{(input)} Array of destination interfaces in the Filter
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if any specified interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    This routine is used to modify the filter as well
* @notes    Ensure that all required interfaces are in the two lists.
* @notes    Do not send just the changes, send all the data as if creating a new filter
*
* @end
*********************************************************************/
L7_RC_t dtlFilterAdd(L7_uchar8* macAddr,
                     L7_uint32  vlanId,
                     L7_uint32  numSrcIntf,
                     L7_uint32  srcIntfList[],
                     L7_uint32  numDstIntf,
                     L7_uint32  dstIntfList[])
{
  L7_uint32 i;
  DAPI_ADDR_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  L7_RC_t dr;
  DAPI_USP_t srcUSP[L7_FILTER_MAX_INTF];
  DAPI_USP_t dstUSP[L7_FILTER_MAX_INTF];

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  for (i = 0; i < numSrcIntf; i++)
  {
    if (nimGetUnitSlotPort(srcIntfList[i], &usp) != L7_SUCCESS)
      return L7_FAILURE;

    srcUSP[i].unit = usp.unit;
    srcUSP[i].slot = usp.slot;
    srcUSP[i].port = usp.port-1;
  }
  for (i = 0; i < numDstIntf; i++)
  {
    if (nimGetUnitSlotPort(dstIntfList[i], &usp) != L7_SUCCESS)
      return L7_FAILURE;

    dstUSP[i].unit = usp.unit;
    dstUSP[i].slot = usp.slot;
    dstUSP[i].port = usp.port-1;
  }

  dapiCmd.cmdData.macFilterAdd.getOrSet = DAPI_CMD_SET;

  memcpy((void *)dapiCmd.cmdData.macFilterAdd.macAddr.addr, macAddr, L7_MAC_ADDR_LEN);
  dapiCmd.cmdData.macFilterAdd.vlanID = vlanId;

  dapiCmd.cmdData.macFilterAdd.numOfSrcPortEntries = numSrcIntf;
  dapiCmd.cmdData.macFilterAdd.srcPorts = 
  (numSrcIntf != 0) ? srcUSP:(DAPI_USP_t *)L7_NULLPTR;

  dapiCmd.cmdData.macFilterAdd.numOfDestPortEntries = numDstIntf;
  dapiCmd.cmdData.macFilterAdd.destPorts = 
  (numDstIntf != 0) ? dstUSP:(DAPI_USP_t *)L7_NULLPTR;

  dr = dapiCtl(&ddUsp,DAPI_CMD_ADDR_MAC_FILTER_ADD,&dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Deletes a given MAC filter
*
* @param    macAddr         @b{(input)} MAC Address of the Filter
* @param    vlanId          @b{(input)} vlan ID associated
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if any specified interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dtlFilterDelete(L7_uchar8* macAddr,
                        L7_uint32  vlanId)
{
  DAPI_ADDR_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  L7_RC_t dr;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dapiCmd.cmdData.macFilterDelete.getOrSet = DAPI_CMD_SET;
  memcpy((void *)dapiCmd.cmdData.macFilterDelete.macAddr.addr, (void *)macAddr,L7_MAC_ADDR_LEN);
  dapiCmd.cmdData.macFilterDelete.vlanID = vlanId;

  dr = dapiCtl(&ddUsp,DAPI_CMD_ADDR_MAC_FILTER_DELETE,&dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}
/*
**********************************************************************
*                           PRIVATE FUNCTIONS 
**********************************************************************
*/


#endif /*DTLCTRL_COMPONENT_L2_FILTER*/

