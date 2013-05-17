/*********************************************************************
*                                                                 
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: dtl_acl.c
*
* Purpose: This file contains the functions to setup AUTO-VOIP
*          management calls to the driver.
*
* Component: Device Transformation Layer (DTL)
*
* Comments: 
*
* Created by: Murali krishna Peddireddy
*
*********************************************************************/


#include "dtlinclude.h"
#include "unitmgr_api.h"
#include "osapi_support.h"
#include "voip_parse.h"

/*********************************************************************
* @purpose  Add/Delete VOIP profile 
*
* @param    intIfNum       @b{(input)} Interface number to apply to
* @param    mode           @b{(input)} Enable/Disable
* @param    bandWidth      @b{(input)} Min bandwidth 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t dtlVoipProfileSet(L7_uint32 intIfNum,L7_BOOL mode, L7_uint32 bandWidth)
{
  DAPI_QOS_CMD_t dapiCmd;
  DAPI_USP_t     ddUsp;
  L7_RC_t  rc;
  nimUSP_t       nimUsp;
 
  if (nimGetUnitSlotPort(intIfNum, &nimUsp) != L7_SUCCESS)
    return L7_FAILURE;
  
  ddUsp.unit = (L7_ushort16)nimUsp.unit;
  ddUsp.slot = (L7_ushort16)nimUsp.slot;  
  ddUsp.port = (L7_ushort16)nimUsp.port-1; 
  

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_VOIP_COMPONENT_ID, L7_QOS_VOIP_PROTOCOL_SIP) == L7_TRUE)
    dapiCmd.cmdData.voipProfile.protocol[L7_QOS_VOIP_PROTOCOL_SIP] = L7_TRUE;
  else
     dapiCmd.cmdData.voipProfile.protocol[L7_QOS_VOIP_PROTOCOL_SIP] = L7_FALSE;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_VOIP_COMPONENT_ID, L7_QOS_VOIP_PROTOCOL_MGCP) == L7_TRUE)
    dapiCmd.cmdData.voipProfile.protocol[L7_QOS_VOIP_PROTOCOL_MGCP] = L7_TRUE;
  else
     dapiCmd.cmdData.voipProfile.protocol[L7_QOS_VOIP_PROTOCOL_MGCP] = L7_FALSE;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_VOIP_COMPONENT_ID, L7_QOS_VOIP_PROTOCOL_H323) == L7_TRUE)
    dapiCmd.cmdData.voipProfile.protocol[L7_QOS_VOIP_PROTOCOL_H323] = L7_TRUE;
  else
     dapiCmd.cmdData.voipProfile.protocol[L7_QOS_VOIP_PROTOCOL_H323] = L7_FALSE;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_VOIP_COMPONENT_ID, L7_QOS_VOIP_PROTOCOL_SCCP) == L7_TRUE)
    dapiCmd.cmdData.voipProfile.protocol[L7_QOS_VOIP_PROTOCOL_SCCP] = L7_TRUE;
  else
     dapiCmd.cmdData.voipProfile.protocol[L7_QOS_VOIP_PROTOCOL_SCCP] = L7_FALSE;

  dapiCmd.cmdData.voipProfile.getOrSet=DAPI_CMD_SET;
  dapiCmd.cmdData.voipProfile.val = mode;
  dapiCmd.cmdData.voipProfile.guarentedBw = bandWidth;
  
  rc = dapiCtl(&ddUsp,DAPI_CMD_QOS_VOIP_PROFILE,&dapiCmd);
  
  if (rc == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
  
}
/*********************************************************************
* @purpose  Add/Delete VOIP calls 
*
* @param    intIfNum       @b{(input)} Interface number to apply to
* @param    mode           @b{(input)} Enable/Disable
* @param    voipFpEntry_t  @b{(input)} VOIP entry parameters
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t dtlVoipProfileParamSet(L7_BOOL val,
                               L7_uint32 protocol,L7_uint32 srcIp,
                               L7_uint32 dstIp, L7_ushort16 srcL4port,
                               L7_ushort16 dstL4port)
{
  DAPI_QOS_CMD_t dapiCmd;
  DAPI_USP_t     ddUsp;
  L7_RC_t  rc;


  ddUsp.unit =0;
  ddUsp.slot =0; 
  ddUsp.port =0; 

  dapiCmd.cmdData.voipSession.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.voipSession.protoType = protocol; 
  dapiCmd.cmdData.voipSession.val = val;
  dapiCmd.cmdData.voipSession.srcIpAddr = (L7_uint32)osapiHtonl(srcIp);
  dapiCmd.cmdData.voipSession.dstIpAddr = (L7_uint32)osapiHtonl(dstIp);
  dapiCmd.cmdData.voipSession.srcL4Port = osapiHtons(srcL4port);
  dapiCmd.cmdData.voipSession.dstL4Port = osapiHtons(dstL4port);

  rc = dapiCtl(&ddUsp,DAPI_CMD_QOS_VOIP_SESSION,&dapiCmd);

  if (rc == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;

}

/*********************************************************************
* @purpose  Add/Delete VOIP calls
*
* @param    intIfNum       @b{(input)} Interface number to apply to
* @param    mode           @b{(input)} Enable/Disable
* @param    voipFpEntry_t  @b{(input)} VOIP entry parameters
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t dtlVoipSessionStatsGet(L7_uint32 srcIp,
                               L7_uint32 dstIp, 
                               L7_ushort16 srcL4port,
                               L7_ushort16 dstL4port,
                               L7_ulong64  *pHitcount)
{
  DAPI_QOS_CMD_t dapiCmd;
  DAPI_USP_t     ddUsp;
  L7_RC_t  rc;


  ddUsp.unit =0;
  ddUsp.slot =0;
  ddUsp.port =0;

  dapiCmd.cmdData.voipStats.getOrSet = DAPI_CMD_GET;
  dapiCmd.cmdData.voipStats.srcIpAddr = (L7_uint32)osapiHtonl(srcIp);
  dapiCmd.cmdData.voipStats.dstIpAddr = (L7_uint32)osapiHtonl(dstIp);
  dapiCmd.cmdData.voipStats.srcL4Port = osapiHtons(srcL4port);
  dapiCmd.cmdData.voipStats.dstL4Port = osapiHtons(dstL4port);
  dapiCmd.cmdData.voipStats.pHitcount = pHitcount; 
  
  rc = dapiCtl(&ddUsp,DAPI_CMD_QOS_VOIP_STATS_GET,&dapiCmd);

  if (rc == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

