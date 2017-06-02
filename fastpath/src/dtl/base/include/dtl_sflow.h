/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dtl_sflow.h
*
* @purpose   This file contains the prototypes of functions to transform 
*            sFlow commands to request the hardware driver.
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
#ifndef DTL_SFLOW_H
#define DTL_SFLOW_H
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
L7_RC_t dtlsFlowSamplePrioritySet(L7_uint32 priority);
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
L7_RC_t dtlsFlowRandomSeedSet(L7_uint32 seedValue);
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
*           non zero value means enable sampling
*
* @end
*********************************************************************/
L7_RC_t dtlsFlowIntfSamplingRateSet(L7_uint32 intIfNum, 
                                    L7_uint32 sampleRate);
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
                                    L7_uint32 *sampleRate);
#endif /* DTL_SFLOW_H */
