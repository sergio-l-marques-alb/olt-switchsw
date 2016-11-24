/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename  usmdb_cpim.c
*
* @purpose   Captive Portal Interface Manager (CPIM) USMDB API functions
*
* @component USMDB
*
* @comments
*
* @create    7/13/2007
*
* @author    rjindal
*
* @end
*
*********************************************************************/

#include "l7_common.h"
#include "usmdb_cpim_api.h"
#include "cpim_api.h"

/*********************************************************************
*
* @purpose  Get the next interface ID.
*
* @param    L7_uint32  intfId      @b{(input)} interface id
* @param    L7_uint32  nextIntfId  @b{(output)} next interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfNextGet(L7_uint32 intfId, L7_uint32 *nextIntfId)
{
  return cpimIntfNextGet(intfId, nextIntfId);
}

/*********************************************************************
*
* @purpose  Check for existing interface ID.
*
* @param    L7_uint32  intfId  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfGet(L7_uint32 intfId)
{
  return cpimIntfGet(intfId);
}

/*********************************************************************
*
* @purpose  Block a specific CP interface.
*
* @param    L7_uint32  intfId  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfBlock(L7_uint32 intfId)
{
  return cpimIntfBlock(intfId);
}

/*********************************************************************
*
* @purpose  Unblock a specific CP interface.
*
* @param    L7_uint32  intfId  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfUnblock(L7_uint32 intfId)
{
  return cpimIntfUnblock(intfId);
}

/********************************************************************* 
* @purpose  Get interface capability field: state
*
* @param    L7_uint32 intfId @b{(input)} interface ID
* @param    L7_CP_MODE_STATUS_t *state @b{(output)} pointer to state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfCapabilityStateGet(L7_uint32 intfId, L7_CP_MODE_STATUS_t *state)
{
  return cpimIntfCapabilityStateGet(intfId, state);
}

/********************************************************************* 
* @purpose  Get interface capability field: blockStatus
*
* @param    L7_uint32 intfId @b{(input)} interface ID
* @param    L7_CP_INST_BLOCK_STATUS_t *state @b{(output)} pointer to state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfCapabilityBlockStatusGet(L7_uint32 intfId, L7_CP_INST_BLOCK_STATUS_t *blockStatus)
{
  return cpimIntfCapabilityBlockStatusGet(intfId, blockStatus);
}

/********************************************************************* 
* @purpose  Get interface capability field: sessionTimeout
*
* @param    L7_uint32 intfId          @b{(input)} interface ID
* @param    L7_uchar8 *sessionTimeout @b{(output)} pointer to sessionTimeout
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfCapabilitySessionTimeoutGet(L7_uint32 intfId, L7_uchar8 *sessionTimeout)
{
  return cpimIntfCapabilitySessionTimeoutGet(intfId, sessionTimeout);
}

/********************************************************************* 
* @purpose  Get interface capability field: idleTimeout
*
* @param    L7_uint32 intfId       @b{(input)} interface ID
* @param    L7_uchar8 *idleTimeout @b{(output)} pointer to idleTimeout
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfCapabilityIdleTimeoutGet(L7_uint32 intfId, L7_uchar8 *idleTimeout)
{
  return cpimIntfCapabilityIdleTimeoutGet(intfId, idleTimeout);
}

/********************************************************************* 
* @purpose  Get interface capability field: bandwidthUpRateControl
*
* @param    L7_uint32 intfId           @b{(input)} interface ID
* @param    L7_uchar8 *bwUpRateControl @b{(output)} pointer to Rate Control
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfCapabilityBandwidthUpRateControlGet(L7_uint32 intfId, L7_uchar8 *bwUpRateControl)
{
  return cpimIntfCapabilityBandwidthUpRateControlGet(intfId, bwUpRateControl);
}

/********************************************************************* 
* @purpose  Get interface capability field: bandwidthDownRateControl
*
* @param    L7_uint32 intfId             @b{(input)} interface ID
* @param    L7_uchar8 *bwDownRateControl @b{(output)} pointer to Rate Control
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfCapabilityBandwidthDownRateControlGet(L7_uint32 intfId, L7_uchar8 *bwDownRateControl)
{
  return cpimIntfCapabilityBandwidthDownRateControlGet(intfId, bwDownRateControl);
}

/********************************************************************* 
* @purpose  Get interface capability field: maxInputOctetMonitor
*
* @param    L7_uint32 intfId             @b{(input)} interface ID
* @param    L7_uchar8 *inputOctetMonitor @b{(output)} pointer to monitor
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfCapabilityMaxInputOctetMonitorGet(L7_uint32 intfId, L7_uchar8 *inputOctetMonitor)
{
  return cpimIntfCapabilityMaxInputOctetMonitorGet(intfId, inputOctetMonitor);
}

/********************************************************************* 
* @purpose  Get interface capability field: maxOutputOctetMonitor
*
* @param    L7_uint32 intfId              @b{(input)} interface ID
* @param    L7_uchar8 *outputOctetMonitor @b{(output)} pointer to monitor
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfCapabilityMaxOutputOctetMonitorGet(L7_uint32 intfId, L7_uchar8 *outputOctetMonitor)
{
  return cpimIntfCapabilityMaxOutputOctetMonitorGet(intfId, outputOctetMonitor);
}

/********************************************************************* 
* @purpose  Get interface capability field: bytesReceivedCounter
*
* @param    L7_uint32 intfId          @b{(input)} interface ID
* @param    L7_uchar8 *bytesRxCounter @b{(output)} pointer to counter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfCapabilityBytesReceivedCounterGet(L7_uint32 intfId, L7_uchar8 *bytesRxCounter)
{
  return cpimIntfCapabilityBytesReceivedCounterGet(intfId, bytesRxCounter);
}

/********************************************************************* 
* @purpose  Get interface capability field: bytesTransmittedCounter
*
* @param    L7_uint32 intfId          @b{(input)} interface ID
* @param    L7_uchar8 *bytesTxCounter @b{(output)} pointer to counter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfCapabilityBytesTransmittedCounterGet(L7_uint32 intfId, L7_uchar8 *bytesTxCounter)
{
  return cpimIntfCapabilityBytesTransmittedCounterGet(intfId, bytesTxCounter);
}

/********************************************************************* 
* @purpose  Get interface capability field: packetsReceivedCounter
*
* @param    L7_uint32 intfId         @b{(input)} interface ID
* @param    L7_uchar8 *pktsRxCounter @b{(output)} pointer to counter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfCapabilityPacketsReceivedCounterGet(L7_uint32 intfId, L7_uchar8 *pktsRxCounter)
{
  return cpimIntfCapabilityPacketsReceivedCounterGet(intfId, pktsRxCounter);
}

/********************************************************************* 
* @purpose  Get interface capability field: packetsTransmittedCounter
*
* @param    L7_uint32 intfId         @b{(input)} interface ID
* @param    L7_uchar8 *pktsTxCounter @b{(output)} pointer to counter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfCapabilityPacketsTransmittedCounterGet(L7_uint32 intfId, L7_uchar8 *pktsTxCounter)
{
  return cpimIntfCapabilityPacketsTransmittedCounterGet(intfId, pktsTxCounter);
}

/********************************************************************* 
* @purpose  Get interface capability field: roamingSupport
*
* @param    L7_uint32 intfId          @b{(input)} interface ID
* @param    L7_uchar8 *roamingSupport @b{(output)} pointer to roamingSupport
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfCapabilityRoamingSupportGet(L7_uint32 intfId, L7_uchar8 *roamingSupport)
{
  return cpimIntfCapabilityRoamingSupportGet(intfId, roamingSupport);
}


/********************************************************************* 
* @purpose  Get interface capability field: description
*
* @param    L7_uint32 intfId @b{(input)} interface ID
* @param    L7_uchar8 *description @b{(output)} description of 
*           interface capability item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfCapabilityDescriptionGet(L7_uint32 intfId, 
					      L7_uchar8 *description)
{
  return cpimIntfCapabilityDescriptionGet(intfId, description);
}

/********************************************************************* 
* @purpose  Deauthenticate client with MAC addr on the given interface
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_enetMacAddr_t * mac @b{(input)} client MAC
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfDeauthenticate(L7_uint32 intIfNum, L7_enetMacAddr_t * mac)
{
  return cpimIntfDeauthenticate(intIfNum, mac);
}

