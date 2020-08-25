/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dtl_sflow.c
*
* @purpose   This file contains the functions to transform sFlow
*            commands to request the hardware driver.
*
* @component Device Transformation Layer (DTL)
*
* @comments  none
*
* @create    30 Nov 2007
*
* @author    drajendra
*
* @end
*
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/

#include "dtlinclude.h"

/*********************************************************************
* @purpose  Sets the priority of the sampled packet with which it will
*           reach the CPU
*
* @param    priority
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlsFlowSamplePrioritySet(L7_uint32 priority)
{
  DAPI_USP_t ddUsp;
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t dr;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dapiCmd.cmdData.sFlowConfig.getOrSet       = DAPI_CMD_SET;
  dapiCmd.cmdData.sFlowConfig.sampleCpuPrio  = priority;

  dr = dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_CPU_SAMPLE_PRIORITY, &dapiCmd);
  if (dr == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }
}

/*********************************************************************
* @purpose  Set the seed value for the random generator used by
*           the sampler
*
* @param    seedValue
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlsFlowRandomSeedSet(L7_uint32 seedValue)
{
  DAPI_USP_t ddUsp;
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t dr;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dapiCmd.cmdData.sFlowConfig.getOrSet    = DAPI_CMD_SET;
  dapiCmd.cmdData.sFlowConfig.RandomSeed  = seedValue;
  dapiCmd.cmdData.sFlowConfig.direction   = L7_FALSE;

  dr = dapiCtl(&ddUsp, DAPI_CMD_SYSTEM_SAMPLE_RANDOM_SEED, &dapiCmd);
  if (dr == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }
}
/*********************************************************************
* @purpose  Sets the sampling rate and enables/disables sampling on
*           the specified interface
*
* @param    intIfNum       interface identified
* @param    samplingRate   sampling rate intended
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    A sampling rate of zero means disable sampling, any
*           non zero value means enable sampling. Only ingress
*           sample is enabled
*
* @end
*********************************************************************/
L7_RC_t dtlsFlowIntfSamplingRateSet(L7_uint32 intIfNum, 
                                    L7_uint32 sampleRate)
{
  DAPI_USP_t        ddUsp;
  nimUSP_t          usp;
  L7_RC_t           dr;
  DAPI_SYSTEM_CMD_t dapiCmd;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

  dapiCmd.cmdData.sFlowConfig.getOrSet            = DAPI_CMD_SET;
  dapiCmd.cmdData.sFlowConfig.ingressSamplingRate = sampleRate;
  dapiCmd.cmdData.sFlowConfig.egressSamplingRate  = L7_NULL;

  dr = dapiCtl(&ddUsp, DAPI_CMD_INTF_SAMPLE_RATE, &dapiCmd);
  if (dr != L7_FAILURE)
  {
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }
}
/*********************************************************************
* @purpose  Gets the sampling rate and enables/disables sampling on
*           the specified interface
*
* @param    intIfNum       interface identified
* @param   *samplingRate   sampling rate configured in hardware
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    A sampling rate of zero means disable sampling, any
*           non zero value means enable sampling. Only ingress
*           sample is enabled
*
* @end
*********************************************************************/
L7_RC_t dtlsFlowIntfSamplingRateGet(L7_uint32 intIfNum,
                                    L7_uint32 *sampleRate)
{
  DAPI_USP_t        ddUsp;
  nimUSP_t          usp;
  L7_RC_t           dr;
  DAPI_SYSTEM_CMD_t dapiCmd;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

  dapiCmd.cmdData.sFlowConfig.getOrSet            = DAPI_CMD_GET;
  dapiCmd.cmdData.sFlowConfig.ingressSamplingRate = L7_NULL;
  dapiCmd.cmdData.sFlowConfig.egressSamplingRate  = L7_NULL;

  dr = dapiCtl(&ddUsp, DAPI_CMD_INTF_SAMPLE_RATE, &dapiCmd);
  if (dr != L7_FAILURE)
  {
    *sampleRate = dapiCmd.cmdData.sFlowConfig.ingressSamplingRate;
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }
}

