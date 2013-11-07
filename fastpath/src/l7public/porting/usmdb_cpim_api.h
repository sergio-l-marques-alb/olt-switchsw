/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename  usmdb_cpim_api.h
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

#ifndef INCLUDE_USMDB_CPIM_API_H
#define INCLUDE_USMDB_CPIM_API_H

#include "captive_portal_commdefs.h"

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
L7_RC_t usmDbCpimIntfNextGet(L7_uint32 intfId, L7_uint32 *nextIntfId);

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
L7_RC_t usmDbCpimIntfGet(L7_uint32 intfId);

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
L7_RC_t usmDbCpimIntfBlock(L7_uint32 intfId);

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
L7_RC_t usmDbCpimIntfUnblock(L7_uint32 intfId);

/*********************************************************************
*
* @purpose  Assign a CP configuration ID to an interface.
*
* @param    L7_uint32    intfId  @b{(input)} interface id
* @param    L7_ushort16  cpId    @b{(input)} cp id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfCPIDSet(L7_uint32 intfId, L7_ushort16 cpId);

/*********************************************************************
*
* @purpose  Get the CP configuration ID assigned to an interface.
*
* @param    L7_uint32    intfId  @b{(input)} interface id
* @param    L7_ushort16  *cpId   @b{(output)} pointer to cp id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfCPIDGet(L7_uint32 intfId, L7_ushort16 *cpId);

/*********************************************************************
*
* @purpose  Delete the CP configuration from an interface.
*
* @param    L7_uint32    intfId  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfCPIDDelete(L7_uint32 intfId);

/*********************************************************************
*
* @purpose  Get the switch port number for a given interface ID.
*
* @param    L7_uint32  intfId     @b{(input)} interface id
* @param    L7_uint32  *intIfNum  @b{(output)} pointer to interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfIntIfNumGet(L7_uint32 intfId, L7_uint32 *intIfNum);

/********************************************************************* 
* @purpose  Get interface capability field: state
*
* @param    L7_uint32 intfId    @b{(input)} interface ID
* @param    L7_CP_MODE_STATUS_t  *state    @b{(output)} pointer to state value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfCapabilityStateGet(L7_uint32 intfId, L7_CP_MODE_STATUS_t *state);

/********************************************************************* 
* @purpose  Get interface capability field: blockStatus
*
* @param    L7_uint32 intfId    @b{(input)} interface ID
* @param    L7_CP_INST_BLOCK_STATUS_t  *state    @b{(output)} pointer to state value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpimIntfCapabilityBlockStatusGet(L7_uint32 intfId, L7_CP_INST_BLOCK_STATUS_t *state);

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
L7_RC_t usmDbCpimIntfCapabilitySessionTimeoutGet(L7_uint32 intfId, 
						 L7_uchar8 *sessionTimeout);

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
L7_RC_t usmDbCpimIntfCapabilityPacketsReceivedCounterGet(L7_uint32 intfId, 
							 L7_uchar8 *pktsRxCounter);



L7_RC_t usmDbCpimIntfCapabilityIdleTimeoutGet(L7_uint32 intfId, L7_uchar8 *idleTimeout);

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
L7_RC_t usmDbCpimIntfCapabilityBandwidthUpRateControlGet(L7_uint32 intfId, L7_uchar8 *bwUpRateControl);

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
L7_RC_t usmDbCpimIntfCapabilityBandwidthDownRateControlGet(L7_uint32 intfId, L7_uchar8 *bwDownRateControl);

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
L7_RC_t usmDbCpimIntfCapabilityMaxInputOctetMonitorGet(L7_uint32 intfId, L7_uchar8 *inputOctetMonitor);

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
L7_RC_t usmDbCpimIntfCapabilityMaxOutputOctetMonitorGet(L7_uint32 intfId, L7_uchar8 *outputOctetMonitor);

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
L7_RC_t usmDbCpimIntfCapabilityBytesReceivedCounterGet(L7_uint32 intfId, L7_uchar8 *bytesRxCounter);

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
L7_RC_t usmDbCpimIntfCapabilityBytesTransmittedCounterGet(L7_uint32 intfId, L7_uchar8 *bytesTxCounter);

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
L7_RC_t usmDbCpimIntfCapabilityPacketsReceivedCounterGet(L7_uint32 intfId, L7_uchar8 *pktsRxCounter);

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
L7_RC_t usmDbCpimIntfCapabilityPacketsTransmittedCounterGet(L7_uint32 intfId, L7_uchar8 *pktsTxCounter);

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
L7_RC_t usmDbCpimIntfCapabilityRoamingSupportGet(L7_uint32 intfId, L7_uchar8 *roamingSupport);


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
					      L7_uchar8 *description);

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
L7_RC_t usmDbCpimIntfDeauthenticate(L7_uint32 intIfNum, L7_enetMacAddr_t * mac);

/*********************************************************************
* @purpose  Enable Captive Portal Trace Option
*
* @param    debug_level   @b{(input)} Trace Option
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t usmDbCpimDebugSet (L7_uint32 debug_level);

/*********************************************************************
* @purpose  Disable Captive Portal Trace Option
*
* @param    debug_level   @b{(input)} Trace Option
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t usmDbCpimDebugClear (L7_uint32 debug_level);

#endif /* INCLUDE_USMDB_CPIM_API_H */

