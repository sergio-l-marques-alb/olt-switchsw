/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   dtl_qos_cos.c
*
* @purpose    COS component DTL interface APIs
*
* @component  DTL
*
* @comments   none
*
* @create     03/16/2004
*
* @author     gpaussa
* @end
*
**********************************************************************/

#include "dtlinclude.h"
#include "dtl_qos_cos.h"

#include "l7_cos_api.h"

/*************************************************************************
* @purpose  Set the assigned traffic class (queue) for this IP precedence
*
* @param    intIfNum      @b{(input)}  Internal interface number     
* @param    precedence    @b{(input)}  IP precedence     
* @param    trafficClass  @b{(input)}  Traffic class value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This should only be called when the interface is in 
*           trust IP precedence mode.
*
* @end
*********************************************************************/
L7_RC_t dtlQosCosMapIpPrecTrafficClassSet(L7_uint32 intIfNum, 
                                          L7_uint32 precedence,
                                          L7_uint32 trafficClass)
{
  DAPI_QOS_CMD_t  dapiCmd;
  nimUSP_t        nimUsp;
  DAPI_USP_t      ddUsp;
  L7_RC_t         rc;
  
  if (nimGetUnitSlotPort(intIfNum, &nimUsp) != L7_SUCCESS)
    return L7_FAILURE;
  
  ddUsp.unit = (L7_int32)nimUsp.unit;
  ddUsp.slot = (L7_int32)nimUsp.slot;  
  ddUsp.port = (L7_int32)nimUsp.port-1;
  
  memset(&dapiCmd.cmdData.ipPrecedenceToTcMap, 0, sizeof(dapiCmd.cmdData.ipPrecedenceToTcMap));

  dapiCmd.cmdData.ipPrecedenceToTcMap.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.ipPrecedenceToTcMap.precedence = (L7_ulong32)precedence;
  dapiCmd.cmdData.ipPrecedenceToTcMap.traffic_class = (L7_ulong32)trafficClass;
  
  rc = dapiCtl(&ddUsp,DAPI_CMD_QOS_COS_IP_PRECEDENCE_TO_TC_MAP,&dapiCmd);
  
  if ((rc == L7_SUCCESS) || (rc == L7_ASYNCH_RESPONSE))
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*************************************************************************
* @purpose  Set the assigned traffic class (queue) for this IP DSCP
*
* @param    intIfNum      @b{(input)}  Internal interface number     
* @param    dscp          @b{(input)}  IP DSCP           
* @param    trafficClass  @b{(input)}  Traffic class value    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This should only be called when the interface is in 
*           trust IP DSCP mode.
*
* @end
*********************************************************************/
L7_RC_t dtlQosCosMapIpDscpTrafficClassSet(L7_uint32 intIfNum, 
                                          L7_uint32 dscp,
                                          L7_uint32 trafficClass)
{
  DAPI_QOS_CMD_t  dapiCmd;
  nimUSP_t        nimUsp;
  DAPI_USP_t      ddUsp;
  L7_RC_t         rc;
  
  if (nimGetUnitSlotPort(intIfNum, &nimUsp) != L7_SUCCESS)
    return L7_FAILURE;
  
  ddUsp.unit = (L7_int32)nimUsp.unit;
  ddUsp.slot = (L7_int32)nimUsp.slot;  
  ddUsp.port = (L7_int32)nimUsp.port-1;
  
  memset(&dapiCmd.cmdData.ipDscpToTcMap, 0, sizeof(dapiCmd.cmdData.ipDscpToTcMap));

  dapiCmd.cmdData.ipDscpToTcMap.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.ipDscpToTcMap.dscp = (L7_ulong32)dscp;
  dapiCmd.cmdData.ipDscpToTcMap.traffic_class = (L7_ulong32)trafficClass;
  
  rc = dapiCtl(&ddUsp,DAPI_CMD_QOS_COS_IP_DSCP_TO_TC_MAP,&dapiCmd);
  
  if ((rc == L7_SUCCESS) || (rc == L7_ASYNCH_RESPONSE))
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*************************************************************************
* @purpose  Set the COS trust mode for this interface
*
* @param    intIfNum    @b{(input)}  Internal interface number     
* @param    trustMode   @b{(input)}  Trust mode value    
* @param    pMapTable   @b{(input)}  Ptr to COS mapping table
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Sets the trust mode and supplies the contents of the active
*           COS map table.
*
* @end
*********************************************************************/
L7_RC_t dtlQosCosMapIntfTrustModeSet(L7_uint32 intIfNum, 
                                     L7_QOS_COS_MAP_INTF_MODE_t trustMode,
                                     DTL_QOS_COS_MAP_TABLE_t *pMapTable)
{
  DAPI_QOS_CMD_t  dapiCmd;
  nimUSP_t        nimUsp;
  DAPI_USP_t      ddUsp;
  L7_RC_t         rc;
  L7_uint32       i;
  DAPI_QOS_COS_INTF_MODE_t  intfMode;
#if defined(FEAT_METRO_CPE_V1_0)
  L7_uint32 globMode;
#endif

  if (nimGetUnitSlotPort(intIfNum, &nimUsp) != L7_SUCCESS)
    return L7_FAILURE;
  
  ddUsp.unit = (L7_int32)nimUsp.unit;
  ddUsp.slot = (L7_int32)nimUsp.slot;  
  ddUsp.port = (L7_int32)nimUsp.port-1;
  
  memset(&dapiCmd.cmdData.intfTrustModeConfig, 0, sizeof(dapiCmd.cmdData.intfTrustModeConfig));

  /* translate component enum to DAPI enum */
  switch (trustMode)
  {
  case L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED:
    intfMode = DAPI_QOS_COS_INTF_MODE_UNTRUSTED;
    break;

  case L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P:
    intfMode = DAPI_QOS_COS_INTF_MODE_TRUST_DOT1P;
    break;

  case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC:
    intfMode = DAPI_QOS_COS_INTF_MODE_TRUST_IPPREC;
    break;

  case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP:
    intfMode = DAPI_QOS_COS_INTF_MODE_TRUST_IPDSCP;
    break;

  default:
    return L7_FAILURE;
    break;

  } /* endswitch */

  /* copy 802.1p mapping table (contains either user or port default priority) */
  for (i= 0; i < (L7_DOT1P_MAX_PRIORITY+1); i++)
    dapiCmd.cmdData.intfTrustModeConfig.mapTable.dot1p_traffic_class[i] = 
      pMapTable->dot1pTrafficClass[i];

  /* copy IP precedence mapping table */
  for (i = 0; i < L7_QOS_COS_MAP_NUM_IPPREC; i++)
    dapiCmd.cmdData.intfTrustModeConfig.mapTable.ip_prec_traffic_class[i] = 
      pMapTable->ipPrecTrafficClass[i];

  /* copy IP DSCP mapping table */
  for (i = 0; i < L7_QOS_COS_MAP_NUM_IPDSCP; i++)
    dapiCmd.cmdData.intfTrustModeConfig.mapTable.ip_dscp_traffic_class[i] = 
      pMapTable->ipDscpTrafficClass[i];

  dapiCmd.cmdData.intfTrustModeConfig.getOrSet = DAPI_CMD_SET;
#if defined(FEAT_METRO_CPE_V1_0)
  if (cosMapIntfTrustModeGet(L7_ALL_INTERFACES, &globMode) != L7_SUCCESS)
   return L7_FAILURE;

  if (globMode != intfMode)
  {
    if (globMode == L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP && 
       intfMode == DAPI_QOS_COS_INTF_MODE_UNTRUSTED)
      intfMode =  DAPI_QOS_COS_INTF_MODE_TRUST_UNSET_IPDSCP;
    else if (globMode == L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P && 
             intfMode == DAPI_QOS_COS_INTF_MODE_UNTRUSTED)
      intfMode =  DAPI_QOS_COS_INTF_MODE_TRUST_UNSET_DOT1P;

  }
#endif
  dapiCmd.cmdData.intfTrustModeConfig.mode = intfMode;
  
  rc = dapiCtl(&ddUsp,DAPI_CMD_QOS_COS_INTF_TRUST_MODE_CONFIG,&dapiCmd);
  
  if ((rc == L7_SUCCESS) || (rc == L7_ASYNCH_RESPONSE))
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*************************************************************************
* @purpose  Set the COS interface config parameters
*
* @param    intIfNum        @b{(input)}  Internal interface number     
* @param    intfShapingRate @b{(input)}  Interface shaping rate
* @param    qMgmtTypeIntf   @b{(input)}  Queue mgmt type (per-interface)
* @param    wredDecayExp    @b{(input)}  WRED decay exponent   
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The qMgmtTypeIntf value is set to 'reserved' if the device
*           supports per-queue management type configuration instead, 
*           which is set with the other queue parameters.
*
* @end
*********************************************************************/
L7_RC_t dtlQosCosIntfConfigSet(L7_uint32 intIfNum, 
                               L7_uint32 intfShapingRate,
                               L7_QOS_COS_QUEUE_MGMT_TYPE_t qMgmtTypeIntf,
                               L7_uint32 wredDecayExp)
{
  DAPI_QOS_CMD_t  dapiCmd;
  nimUSP_t        nimUsp;
  DAPI_USP_t      ddUsp;
  L7_RC_t         rc;
  DAPI_QOS_COS_QUEUE_MGMT_TYPE_t  portMgmtType;
  
  if (nimGetUnitSlotPort(intIfNum, &nimUsp) != L7_SUCCESS)
    return L7_FAILURE;
  
  ddUsp.unit = (L7_int32)nimUsp.unit;
  ddUsp.slot = (L7_int32)nimUsp.slot;  
  ddUsp.port = (L7_int32)nimUsp.port-1;
  
  memset(&dapiCmd.cmdData.intfConfig, 0, sizeof(dapiCmd.cmdData.intfConfig));

  /* translate component enum to DAPI enum */
  switch (qMgmtTypeIntf)
  {
  case L7_QOS_COS_QUEUE_MGMT_TYPE_RESERVED:
    portMgmtType = DAPI_QOS_COS_QUEUE_MGMT_TYPE_NOT_SUPPORTED;
    break;
  case L7_QOS_COS_QUEUE_MGMT_TYPE_TAILDROP:
    portMgmtType = DAPI_QOS_COS_QUEUE_MGMT_TYPE_TAILDROP;
    break;
  case L7_QOS_COS_QUEUE_MGMT_TYPE_WRED:
    portMgmtType = DAPI_QOS_COS_QUEUE_MGMT_TYPE_WRED;
    break;
  default:
    return L7_FAILURE;
    break;
  }

  dapiCmd.cmdData.intfConfig.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.intfConfig.intfShapingRate = (L7_ulong32)intfShapingRate;
  dapiCmd.cmdData.intfConfig.queueMgmtTypePerIntf = portMgmtType;
  dapiCmd.cmdData.intfConfig.wredDecayExponent = (L7_ulong32)wredDecayExp;
  
  rc = dapiCtl(&ddUsp,DAPI_CMD_QOS_COS_INTF_CONFIG,&dapiCmd);
  
  if ((rc == L7_SUCCESS) || (rc == L7_ASYNCH_RESPONSE))
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*************************************************************************
* @purpose  Get the COS interface config parameters
*
* @param    intIfNum             @b{(input)}  Internal interface number     
* @param    intfShapingRate      @b{(input)}  Interface shaping rate in kbps
* @param    intfShapingBurstSize @b{(input)}  Interface shaping burst size in kbits
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t dtlQosCosIntfStatusGet(L7_uint32 intIfNum, 
                               L7_uint32 *intfShapingRate,
                               L7_uint32 *intfShapingBurstSize)
{
  DAPI_QOS_CMD_t  dapiCmd;
  nimUSP_t        nimUsp;
  DAPI_USP_t      ddUsp;
  L7_RC_t         rc;
  
  if (nimGetUnitSlotPort(intIfNum, &nimUsp) != L7_SUCCESS)
    return L7_FAILURE;
  
  ddUsp.unit = (L7_int32)nimUsp.unit;
  ddUsp.slot = (L7_int32)nimUsp.slot;  
  ddUsp.port = (L7_int32)nimUsp.port-1;
  
  memset(&dapiCmd.cmdData.intfStatus, 0, sizeof(dapiCmd.cmdData.intfStatus));

  dapiCmd.cmdData.intfStatus.getOrSet = DAPI_CMD_GET;
  
  rc = dapiCtl(&ddUsp,DAPI_CMD_QOS_COS_INTF_STATUS,&dapiCmd);
  
  if ((rc == L7_SUCCESS) || (rc == L7_ASYNCH_RESPONSE))
  {

    *intfShapingRate = dapiCmd.cmdData.intfStatus.intfShapingRate;
    *intfShapingBurstSize = dapiCmd.cmdData.intfStatus.intfShapingBurstSize;
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;
}

/*************************************************************************
* @purpose  Set the COS queue scheduler parameters for the interface
*
* @param    intIfNum        @b{(input)}  Internal interface number     
* @param    *pMinBwList     @b{(input)}  Ptr to minimum bandwidth parm list
* @param    *pMaxBwList     @b{(input)}  Ptr to maximum bandwidth parm list
* @param    *pSchedTypeList @b{(input)}  Ptr to scheduler type list   
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Each of the 'list' style parameters contains a separate
*           value for each supported COS queue.
*
* @end
*********************************************************************/
L7_RC_t dtlQosCosQueueSchedConfigSet(L7_uint32 intIfNum, 
                                     L7_qosCosQueueBwList_t *pMinBwList,
                                     L7_qosCosQueueBwList_t *pMaxBwList,
                                     L7_qosCosQueueSchedTypeList_t *pSchedTypeList)
{
  DAPI_QOS_CMD_t  dapiCmd;
  nimUSP_t        nimUsp;
  DAPI_USP_t      ddUsp;
  L7_RC_t         rc;
  L7_uint32       i;
  DAPI_QOS_COS_QUEUE_SCHED_TYPE_t queueSchedType;
  
  if (nimGetUnitSlotPort(intIfNum, &nimUsp) != L7_SUCCESS)
    return L7_FAILURE;
  
  ddUsp.unit = (L7_int32)nimUsp.unit;
  ddUsp.slot = (L7_int32)nimUsp.slot;  
  ddUsp.port = (L7_int32)nimUsp.port-1;

  memset(&dapiCmd.cmdData.queueSchedConfig, 0, sizeof(dapiCmd.cmdData.queueSchedConfig));
  
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    /* translate component enum to DAPI enum for scheduler type */
    switch (pSchedTypeList->schedType[i])
    {
    case L7_QOS_COS_QUEUE_SCHED_TYPE_STRICT:
      queueSchedType = DAPI_QOS_COS_QUEUE_SCHED_TYPE_STRICT;
      break;
    case L7_QOS_COS_QUEUE_SCHED_TYPE_WEIGHTED:
      queueSchedType = DAPI_QOS_COS_QUEUE_SCHED_TYPE_WEIGHTED;
      break;
    default:
      return L7_FAILURE;
      break;
    }

    dapiCmd.cmdData.queueSchedConfig.minBandwidth[i] = (L7_ulong32)pMinBwList->bandwidth[i];
    dapiCmd.cmdData.queueSchedConfig.maxBandwidth[i] = (L7_ulong32)pMaxBwList->bandwidth[i];
    dapiCmd.cmdData.queueSchedConfig.schedulerType[i] = queueSchedType;
  } /* endfor i */

  dapiCmd.cmdData.queueSchedConfig.getOrSet = DAPI_CMD_SET;
  
  rc = dapiCtl(&ddUsp,DAPI_CMD_QOS_COS_QUEUE_SCHED_CONFIG,&dapiCmd);
  
  if ((rc == L7_SUCCESS) || (rc == L7_ASYNCH_RESPONSE))
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*************************************************************************
* @purpose  Set the COS drop config (taildrop or WRED) parameters
*           on this interface
*
* @param    intIfNum      @b{(input)}  Internal interface number     
* @param    *pDropParms   @b{(input)}  Ptr to drop parms list
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Each of the 'list' style parameters contains a separate
*           value for each supported drop precedence level.
*
* @end
*********************************************************************/
L7_RC_t dtlQosCosQueueDropConfigSet(L7_uint32 intIfNum, 
                                    L7_qosCosDropParmsList_t *pDropParms)
{
  DAPI_QOS_CMD_t  dapiCmd;
  nimUSP_t        nimUsp;
  DAPI_USP_t      ddUsp;
  L7_RC_t         rc;
  L7_uint32       queueIndex, precIndex;
  
  if (nimGetUnitSlotPort(intIfNum, &nimUsp) != L7_SUCCESS)
    return L7_FAILURE;
  
  ddUsp.unit = (L7_int32)nimUsp.unit;
  ddUsp.slot = (L7_int32)nimUsp.slot;  
  ddUsp.port = (L7_int32)nimUsp.port-1;

  memset(&dapiCmd.cmdData.queueDropConfig, 0, sizeof(dapiCmd.cmdData.queueDropConfig));
    
  for(queueIndex=0; queueIndex<L7_MAX_CFG_QUEUES_PER_PORT; queueIndex++) 
  {
      dapiCmd.cmdData.queueDropConfig.parms[queueIndex].dropType = 
          pDropParms->queue[queueIndex].mgmtType;
      for (precIndex = 0; precIndex < (L7_MAX_CFG_DROP_PREC_LEVELS+1); precIndex++)
      {
        dapiCmd.cmdData.queueDropConfig.parms[queueIndex].minThreshold[precIndex] = 
            pDropParms->queue[queueIndex].minThreshold[precIndex];
        if (pDropParms->queue[queueIndex].mgmtType == L7_QOS_COS_QUEUE_MGMT_TYPE_TAILDROP) 
        {
            dapiCmd.cmdData.queueDropConfig.parms[queueIndex].maxThreshold[precIndex] = 
                pDropParms->queue[queueIndex].tailDropMaxThreshold[precIndex];            
        }
        else
        {
            dapiCmd.cmdData.queueDropConfig.parms[queueIndex].maxThreshold[precIndex] = 
                pDropParms->queue[queueIndex].wredMaxThreshold[precIndex];
        }
        dapiCmd.cmdData.queueDropConfig.parms[queueIndex].dropProb[precIndex] = 
            pDropParms->queue[queueIndex].dropProb[precIndex];
      }
  }
  dapiCmd.cmdData.queueDropConfig.getOrSet = DAPI_CMD_SET;
  
  rc = dapiCtl(&ddUsp,DAPI_CMD_QOS_COS_QUEUE_DROP_CONFIG,&dapiCmd);
  
  if ((rc == L7_SUCCESS) || (rc == L7_ASYNCH_RESPONSE))
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}


