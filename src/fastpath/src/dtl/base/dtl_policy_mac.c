/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: dtl_policy_mac.c
*
* Purpose: This file contains the functions to transform the classifier
*          components or policies MAC driver requests. 
*
* Component: Device Transformation Layer (DTL)
*
* Commnets: 
*
* Created by: Shekhar Kalyanam 3/14/2001 
*
*********************************************************************/
/*************************************************************
                    
*************************************************************/





/*
**********************************************************************
*                           HEADER FILES
**********************************************************************
*/

#define DTLCTRL_POLICY_MAC_GLOBALS              /* Enable global space   */    
#include "dtlinclude.h"

#if DTLCTRL_COMPONENT_POLICY_MAC             /* Classifier support        */



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
* @purpose  Enables or disables flow control for an interface
*
* @param    intIfNum    @b{(input)} Physical or logical interface to be configured
* @param    mode        @b{(input)} Admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlPolicyIntfFlowCtrlModeSet(L7_uint32 intIfNum, 
                                     L7_uint32 mode)
{

  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t dr;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

  if (mode == L7_ENABLE)
  {
    dapiCmd.cmdData.flowControl.enable = L7_TRUE;
  }
  else
  {
    dapiCmd.cmdData.flowControl.enable = L7_FALSE;
  }

  dapiCmd.cmdData.flowControl.getOrSet = DAPI_CMD_SET;
  dr = dapiCtl(&ddUsp, DAPI_CMD_INTF_FLOW_CONTROL, &dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}


/*********************************************************************
* @purpose  Enables or disables flow control for the entire switch i.e. all
*           interfaces
*
* @param    mode        @b{(input)} Admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlPolicyIntfAllFlowCtrlModeSet(L7_uint32 mode)
{

  DAPI_USP_t ddUsp;
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t dr;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  if (mode == L7_ENABLE)
  {
    dapiCmd.cmdData.flowControl.enable = L7_TRUE;
  }
  else
  {
    dapiCmd.cmdData.flowControl.enable = L7_FALSE;
  }
  dapiCmd.cmdData.flowControl.getOrSet = DAPI_CMD_SET;
  dr = dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_FLOW_CONTROL, &dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Enables or disables the broadcast rate limit for an interface
*
* @param    intIfNum    @b{(input)} Physical or logical interface to be configured
* @param    mode        @b{(input)} Admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
* @param    threshold   @b{(input)} Broadcast Threshold
* @param    burstSize   @b{(input)} Burst Size (only for Percentage/Kbps units)
* @param    rate_unit   @b{(input)} Threshold unit of PERCENT or PPS
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlPolicyIntfBcastCtrlModeSet(L7_uint32 intIfNum, 
                                      L7_uint32 mode,
                                      L7_uint32 threshold,
                                      L7_uint32 burstSize /* PTin added: stormcontrol */, 
                                      L7_RATE_UNIT_t rate_unit)
{
  
  DAPI_SYSTEM_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  L7_RC_t dr;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_DTL, "dtlPolicyIntfBcastCtrlModeSet: intIfNum=%u mode=%u threshold=%u burstSize=%u rate_unit=%u",
            intIfNum, mode, threshold, burstSize, rate_unit);

  if (mode == L7_ENABLE)
  {
    dapiCmd.cmdData.broadcastControl.enable = L7_TRUE;
  }
  else
  {
    dapiCmd.cmdData.broadcastControl.enable = L7_FALSE;
  }

  dapiCmd.cmdData.broadcastControl.type = STORMCONTROL_BCAST;
  dapiCmd.cmdData.broadcastControl.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.broadcastControl.threshold = threshold;
  dapiCmd.cmdData.broadcastControl.bucket_size = burstSize;     /* PTin added: stormcontrol */
  dapiCmd.cmdData.broadcastControl.unit = rate_unit;

  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port-1;

  dr = dapiCtl(&ddUsp,DAPI_CMD_INTF_BROADCAST_CONTROL_MODE_SET, &dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enables or disables the multicast rate limit for an interface
*
* @param    intIfNum    @b{(input)} Physical or logical interface to be configured
* @param    mode        @b{(input)} Admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
* @param    threshold   @b{(input)} Multicast Threshold
* @param    burstSize   @b{(input)} Burst Size (only for Percentage/Kbps units)
* @param    rate_unit   @b{(input)} Threshold unit of PERCENT or PPS
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlPolicyIntfMcastCtrlModeSet(L7_uint32 intIfNum, 
                                      L7_uint32 mode,
                                      L7_uint32 threshold,
                                      L7_uint32 burstSize /* PTin added: stormcontrol */, 
                                      L7_RATE_UNIT_t rate_unit)
{
  
  DAPI_SYSTEM_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  L7_RC_t dr;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_DTL, "dtlPolicyIntfMcastCtrlModeSet: intIfNum=%u mode=%u threshold=%u burstSize=%u rate_unit=%u",
            intIfNum, mode, threshold, burstSize, rate_unit);

  if (mode == L7_ENABLE)
  {
    dapiCmd.cmdData.broadcastControl.enable = L7_TRUE;
  }
  else
  {
    dapiCmd.cmdData.broadcastControl.enable = L7_FALSE;
  }

  dapiCmd.cmdData.broadcastControl.type = STORMCONTROL_MCAST;
  dapiCmd.cmdData.broadcastControl.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.broadcastControl.threshold = threshold;
  dapiCmd.cmdData.broadcastControl.bucket_size = burstSize;     /* PTin added: stormcontrol */
  dapiCmd.cmdData.broadcastControl.unit = rate_unit;

  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port-1;

  dr = dapiCtl(&ddUsp,DAPI_CMD_INTF_BROADCAST_CONTROL_MODE_SET, &dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
    
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Enables or disables the destination lookup failure rate limit for an interface
*
* @param    intIfNum    @b{(input)} Physical or logical interface to be configured
* @param    mode        @b{(input)} Admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
* @param    threshold   @b{(input)} Unknown Unicast Threshold
* @param    burstSize   @b{(input)} Burst Size (only for Percentage/Kbps units)
* @param    rate_unit   @b{(input)} Threshold unit of PERCENT or PPS
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlPolicyIntfUcastCtrlModeSet(L7_uint32 intIfNum, 
                                      L7_uint32 mode,
                                      L7_uint32 threshold,
                                      L7_uint32 burstSize /* PTin added: stormcontrol */, 
                                      L7_RATE_UNIT_t rate_unit)
{
  
  DAPI_SYSTEM_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  L7_RC_t dr;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_DTL, "dtlPolicyIntfUcastCtrlModeSet: intIfNum=%u mode=%u threshold=%u burstSize=%u rate_unit=%u",
            intIfNum, mode, threshold, burstSize, rate_unit);

  if (mode == L7_ENABLE)
  {
    dapiCmd.cmdData.broadcastControl.enable = L7_TRUE;
  }
  else
  {
    dapiCmd.cmdData.broadcastControl.enable = L7_FALSE;
  }

  dapiCmd.cmdData.broadcastControl.type = STORMCONTROL_UCAST;
  dapiCmd.cmdData.broadcastControl.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.broadcastControl.threshold = threshold;
  dapiCmd.cmdData.broadcastControl.bucket_size = burstSize;     /* PTin added: stormcontrol */
  dapiCmd.cmdData.broadcastControl.unit = rate_unit;

  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port-1;

  dr = dapiCtl(&ddUsp,DAPI_CMD_INTF_BROADCAST_CONTROL_MODE_SET, &dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
    
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Enables or disables the broadcast rate limit for the system
*           i.e. all interfaces
*
* @param    mode        @b{(input)} Admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
* @param    threshold   @b{(input)} Broadcast Threshold
* @param    burstSize   @b{(input)} Burst Size (only for Percentage/Kbps units)
* @param    rate_unit   @b{(input)} Threshold unit of PERCENT or PPS
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlPolicyIntfAllBcastCtrlModeSet(L7_uint32 mode,
                                         L7_uint32 threshold,
                                         L7_uint32 burstSize /* PTin added: stormcontrol */, 
                                         L7_RATE_UNIT_t rate_unit)
{
  
  DAPI_SYSTEM_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  L7_RC_t dr;


  if (mode == L7_ENABLE)
  {
    dapiCmd.cmdData.broadcastControl.enable = L7_TRUE;
  }
  else
  {
    dapiCmd.cmdData.broadcastControl.enable = L7_FALSE;
  }

  dapiCmd.cmdData.broadcastControl.type = STORMCONTROL_BCAST;
  dapiCmd.cmdData.broadcastControl.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.broadcastControl.threshold = threshold;
  dapiCmd.cmdData.broadcastControl.bucket_size = burstSize;     /* PTin added: stormcontrol */
  dapiCmd.cmdData.broadcastControl.unit = rate_unit;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dr = dapiCtl(&ddUsp,DAPI_CMD_SYSTEM_BROADCAST_CONTROL_MODE_SET, &dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enables or disables the multicast rate limit for the system
*           i.e. all interfaces
*
* @param    mode        @b{(input)} Admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
* @param    threshold   @b{(input)} Multicast Threshold
* @param    burstSize   @b{(input)} Burst Size (only for Percentage/Kbps units)
* @param    rate_unit   @b{(input)} Threshold unit of PERCENT or PPS
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlPolicyIntfAllMcastCtrlModeSet(L7_uint32 mode,
                                         L7_uint32 threshold,
                                         L7_uint32 burstSize /* PTin added: stormcontrol */, 
                                         L7_RATE_UNIT_t rate_unit)
{
  
  DAPI_SYSTEM_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  L7_RC_t dr;


  if (mode == L7_ENABLE)
  {
    dapiCmd.cmdData.broadcastControl.enable = L7_TRUE;
  }
  else
  {
    dapiCmd.cmdData.broadcastControl.enable = L7_FALSE;
  }

  dapiCmd.cmdData.broadcastControl.type = STORMCONTROL_MCAST;
  dapiCmd.cmdData.broadcastControl.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.broadcastControl.threshold = threshold;
  dapiCmd.cmdData.broadcastControl.bucket_size = burstSize;     /* PTin added: stormcontrol */
  dapiCmd.cmdData.broadcastControl.unit = rate_unit;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dr = dapiCtl(&ddUsp,DAPI_CMD_SYSTEM_BROADCAST_CONTROL_MODE_SET, &dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enables or disables the destination lookup failure rate limit for the system
*           i.e. all interfaces
*
* @param    mode        @b{(input)} Admin mode,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
* @param    threshold   @b{(input)} Unknown Unicast Threshold
* @param    burstSize   @b{(input)} Burst Size (only for Percentage/Kbps units)
* @param    rate_unit   @b{(input)} Threshold unit of PERCENT or PPS
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlPolicyIntfAllUcastCtrlModeSet(L7_uint32 mode,
                                         L7_uint32 threshold,
                                         L7_uint32 burstSize /* PTin added: stormcontrol */, 
                                         L7_RATE_UNIT_t rate_unit)
{
  
  DAPI_SYSTEM_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  L7_RC_t dr;


  if (mode == L7_ENABLE)
  {
    dapiCmd.cmdData.broadcastControl.enable = L7_TRUE;
  }
  else
  {
    dapiCmd.cmdData.broadcastControl.enable = L7_FALSE;
  }

  dapiCmd.cmdData.broadcastControl.type = STORMCONTROL_UCAST;
  dapiCmd.cmdData.broadcastControl.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.broadcastControl.threshold = threshold;
  dapiCmd.cmdData.broadcastControl.bucket_size = burstSize;     /* PTin added: stormcontrol */
  dapiCmd.cmdData.broadcastControl.unit = rate_unit;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dr = dapiCtl(&ddUsp,DAPI_CMD_SYSTEM_BROADCAST_CONTROL_MODE_SET, &dapiCmd);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
    
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Upstream Ingress Policing discarded packet count
*
* @param    c        @b{(input)} pCounterValue_entry_t structure
*
* @returns  value in c->cValue
*
* @notes    none
*
* @end
*********************************************************************/

void dtlPolicyIntfUpstreamIngressPoliceDiscardsGet(pCounterValue_entry_t c)
{

  nimUSP_t usp;


  /* Obtain counters from driver */
  if (nimGetUnitSlotPort(c->cKey, &usp) != L7_SUCCESS)
  {
    return;
  }

  c->cSize       = C32_BITS;
  c->cValue.low  = 0;
  c->cValue.high = 0;
  c->status      = L7_SUCCESS;

}


/*********************************************************************
* @purpose  Gets the Upstream Ingress Threshold discarded packet count
*
* @param    c     @b{(input)}pCounterValue_entry_t structure
*
* @returns  value in c->cValue
*
* @notes    none
*
* @end
*********************************************************************/
void dtlPolicyIntfUpstreamIngressThresholdDiscardsGet(pCounterValue_entry_t c)
{
#if 0
  DAPI_INTF_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  L7_ulong64 stats[DAPI_STATS_NUM_OF_INTF_ENTRIES];


  /* Obtain counters from driver */
  if (nimGetUnitSlotPort(c->cKey, &usp) != L7_SUCCESS)
  {
    return;
  }

  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port-1;

  /* check for uplink port */
  if (ddUsp.port == 25)
  {
    c->cSize      = C32_BITS;
    c->cValue.low = 0;
    c->status     = L7_SUCCESS;
    return;
  }

  dapiCmd.cmdData.statistics.getOrSet = DAPI_CMD_GET;
  dapiCmd.cmdData.statistics.enable   = L7_TRUE;
  dapiCmd.cmdData.statistics.stats    = stats;

  dapiCtl(&ddUsp, DAPI_CMD_INTF_STATISTICS, &dapiCmd);

  c->cSize      = C32_BITS;
  c->cValue     = dapiCmd.cmdData.statistics.stats[DAPI_STATS_RECEIVED_UPSTREAM_THRESHOLD_DISCARDS];
  c->status     = L7_SUCCESS;

#else
  /* colinw: The above code has been disabled since the driver doesn't support 
             the operation */
#endif

}


/*********************************************************************
* @purpose  Gets the Downstream Threshold discarded packet count
*
* @param    c @b{(input)}pCounterValue_entry_t structure
*
* @returns  value in c->cValue
*
* @notes    none
*
* @end
*********************************************************************/
void dtlPolicyDomDownstreamThresholdDiscardsGet(pCounterValue_entry_t c)
{
  nimUSP_t usp;


  /* Obtain counters from driver */
  if (nimGetUnitSlotPort(c->cKey, &usp) != L7_SUCCESS)
  {
    return;
  }

  c->cSize       = C32_BITS;
  c->cValue.low  = 0;
  c->cValue.high = 0;
  c->status      = L7_SUCCESS;

}


/*********************************************************************
* @purpose  Sets traffic policing credit
*
* @param    intIfNum       @b{(input)} Internal Interface Number in application corresponding
*                                      to a unit, slot and port in the device driver
* @param    rate           @b{(input)} New policing credit rate
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dtlPolicyCreditSet(L7_uint32 intIfNum, 
                           L7_uint32 rate)
{
  return L7_FAILURE;
}


#endif /* DTLCTRL_COMPONENT_POLICY_MAC */
