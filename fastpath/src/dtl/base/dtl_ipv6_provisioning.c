/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: dtl_ipv6_provisioning.c
*
* Purpose: This file contains the dtl API for IPv6 provisioning functionality
*
* Component: Device Transformation Layer (DTL)
*
* Commnets: 
*
* Created by: nshrivastav 9/02/2003 
*
*********************************************************************/

/*
**********************************************************************
*                           HEADER FILES
**********************************************************************
*/
#include "dtlinclude.h"


/*********************************************************************
* @purpose  Enable/Disable forwarding of ipv6 packets between ports or copying
            of ipv6 packets to CPU
*
* @param    srcIfNum       @b{(input)} Internal Interface Number in application
*                                     corresponding to a unit, slot and port in
*                                     the device driver from which ipv6 packets
*                                     must be forwarded
* @param    dstIfNum       @b{(input)} Internal Interface Number in application
*                                     corresponding to a unit, slot and port in
*                                     the device driver to which ipv6 packets
*                                     must be forwarded
* @param    mode           @b{(input)} L7_ENABLE or L7_DISABLE
*
* @param    flag           @b{(input)} L7_ENABLE - To enable copying of packets
*                                      to CPU when bridging between ports
*                                      L7_DISABLE - Copying of packets to CPU
*                                      will not be enabled while bridging 
*                                      between ports 
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dtlIpv6ProvConfig(L7_uint32 srcIfNum, L7_uint32 dstIfNum, L7_uint32 mode, L7_uint32 copyToCpu)
{
#if 0
  nimUSP_t nUsp;
  DAPI_USP_t ddUsp;
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t dr;

  dr = nimGetUnitSlotPort(srcIfNum, &nUsp);

  if (dr == L7_FAILURE)
    return L7_FAILURE;

  ddUsp.unit = nUsp.unit;
  ddUsp.slot = nUsp.slot;
  ddUsp.port = nUsp.port - 1;

  dr = nimGetUnitSlotPort(dstIfNum, &nUsp);

  if (dr == L7_FAILURE)
    return L7_FAILURE;

  dapiCmd.cmdData.ipv6ProvConfig.getOrSet = DAPI_CMD_SET;
  if (mode == L7_ENABLE)
    dapiCmd.cmdData.ipv6ProvConfig.enable = L7_TRUE;
  else
    dapiCmd.cmdData.ipv6ProvConfig.enable = L7_FALSE;

  if (copyToCpu == L7_ENABLE)
    dapiCmd.cmdData.ipv6ProvConfig.copyToCpu = L7_TRUE;
  else
    dapiCmd.cmdData.ipv6ProvConfig.copyToCpu = L7_FALSE;

  dapiCmd.cmdData.ipv6ProvConfig.DstUsp.unit = nUsp.unit;
  dapiCmd.cmdData.ipv6ProvConfig.DstUsp.slot = nUsp.slot;
  dapiCmd.cmdData.ipv6ProvConfig.DstUsp.port = nUsp.port - 1;

  dr = dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_IPV6_PROVISIONING_CONFIG, &dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
#else
  return L7_SUCCESS;
#endif
}

