/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename  cpim_api.h
*
* @purpose   Captive Portal Interface Manager (CPIM) API functions
*
* @component CPIM
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

#ifndef INCLUDE_CPIM_API_H
#define INCLUDE_CPIM_API_H

#include "captive_portal_commdefs.h"
#include "intf_cb_api.h"
#include "nimapi.h"


/********************************************************************* 
* @purpose  Returns whether or not configurator status is complete.
*
* @param    void
*
* @returns  L7_BOOL 
*
* @comments 
*
* @end
*
*********************************************************************/
L7_BOOL captivePortalCnfgrComplete(void);


/*********************************************************************
*
* @purpose  Return if captive portal is supported on an interface
*           
*
* @param    L7_uint32 intIfNum, interface number in question
* @param    L7_BOOL *isCPEnabled, output 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  
*
* @end
*
*********************************************************************/

L7_RC_t cpimIntfSupportedGet(L7_uint32 intIfNum, L7_BOOL * isCPEnabled);

/*********************************************************************
* @purpose  To provide status on the CP task
*
* @param    void
*
* @returns  L7_BOOL - L7_TRUE if task is active
*           
* @end
*********************************************************************/

L7_BOOL captivePortalTaskActive(void);

/********************************************************************* 
* @purpose  NIM callback for captive portal
*
* @param    L7_uint32 intIfnum @b{(input)} interface ID
* @param    L7_uint32 intIfEvent @b{(input)} interface event
* @param    NIM_CORRELATOR_t correlator @b{(input)} 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only NIM should be calling this function.
*           If you're not the NIM task, don't even think about it.
*
* @end
*
*********************************************************************/

L7_RC_t cpimIntfChangeCallback(L7_uint32        intIfNum, 
                               L7_uint32        intfEvent, 
                               NIM_CORRELATOR_t correlator);

/*********************************************************************
*
* @purpose  Return a pointer to the set of I/F owner callbacks
*           
*
* @param    intIfNum, interface number in question
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Right now, our I/F parameter isn't used.  But it 
*            probably will be someday soon as we expand CP to
*            use multiple interface types.
*
* @end
*
*********************************************************************/

intfCpCallbacks_t * cpimIntfOwnerCallbacksGet(L7_uint32 intIfNum);

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

L7_RC_t cpimIntfNextGet(L7_uint32 intfId, L7_uint32 *nextIntfId);


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
L7_RC_t cpimIntfGet(L7_uint32 intfId);



/*********************************************************************
*
* @purpose  Add an interface to our DBs, let the
*           NIM task fill in the details via a callback.
*
* @param    L7_uint32    intfId  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments WILL trigger the CP task to do work.  Be warned!
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfAdd(L7_uint32 intfId);


/*********************************************************************
*
* @purpose  Delete an interface from our DBs,
*           let the NIM task do some of the work via a callback.
*
* @param    L7_uint32    intfId  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments May trigger the CP task to do work.  Be warned!
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfDelete(L7_uint32 intfId);


/*********************************************************************
*
* @purpose  Enable an interface for use with captive portal
*
* @param    L7_uint32    intfId  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfEnable(L7_uint32 intfId);

/*********************************************************************
*
* @purpose  Disable an interface for use with captive portal
*
* @param    L7_uint32    intfId  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfDisable(L7_uint32 intfId);


/*********************************************************************
*
* @purpose  Unblock an interface 
*
* @param    L7_uint32    intIfNum  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfUnblock(L7_uint32 intIfNum);


/*********************************************************************
*
* @purpose  Block an interface
*
* @param    L7_uint32    intIfNum  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfBlock(L7_uint32 intIfNum);


/*********************************************************************
*
* @purpose  Verify the existence of an interface
*
* @param    L7_uint32    intIfNum  @b{(input)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfGet(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Get the "next" interface that supports Captive Portal
*
* @param    L7_uint32  intIfNum       @b{(input)} interface id
* @param    L7_uint32  *nextIntIfNum  @b{(output)} interface id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfNextGet(L7_uint32 intIfNum, L7_uint32 * nextIntIfNum);



/*********************************************************************
*
* @purpose  Check to see if interface belongs to a wireless network
*           and that it is ready for use
*
* @param    L7_uint32 intIfNum -- corresponding interface number 
* @param    L7_BOOL * pReady   -- is I/F wireless?
* @param    L7_BOOL * pWireless-- is wireless I/F ready?
*
* @returns  L7_SUCCESS if parms are good, and calls succeeded
* @returns  L7_FAILURE if something very bad beyond this module's
*                      control happened.
*
* @comments "Ready" only means that the wireless interface has
*           enabled VAPs.  No other checks are performed.
* 
* @end
*
*********************************************************************/

L7_RC_t cpimIntfIsReady(L7_uint32 intIfNum, 
	   	        L7_BOOL * pWireless,
		        L7_BOOL * pReady);

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
L7_RC_t cpimIntfDeauthenticate(L7_uint32 intIfNum, 
			       L7_enetMacAddr_t * mac);


/********************************************************************* 
* @purpose  Get statistics for a given interface
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_enetMacAddr_t *mac @b{(input)} MAC address
* @param    txRxCount_t * txRx @b{(output)} gathered statistics
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimGetStatistics(L7_uint32 intIfNum,
			  L7_enetMacAddr_t * macAddr,
			  txRxCount_t * txRx);

/********************************************************************* 
* @purpose  Get interface capability field: state
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_CP_MODE_STATUS_t *state @b{(output)} state of 
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
L7_RC_t cpimIntfCapabilityStateGet(L7_uint32 intIfNum, 
				   L7_CP_MODE_STATUS_t *state);

/********************************************************************* 
* @purpose  Get interface capability field: blockStatus
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_CP_INST_BLOCK_STATUS_t *state @b{(output)} state of 
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
L7_RC_t cpimIntfCapabilityBlockStatusGet(L7_uint32 intIfNum,
                   L7_CP_INST_BLOCK_STATUS_t *blockStatus);

/********************************************************************* 
* @purpose  Get interface capability field: sessionTimeout
*
* @param    L7_uint32 intfId @b{(input)} interface ID
* @param    L7_uchar8 *sessionTimeout @b{(output)} sessionTimeout of 
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

L7_RC_t cpimIntfCapabilitySessionTimeoutGet(L7_uint32 intfId, L7_uchar8 *sessionTimeout);


/********************************************************************* 
* @purpose  Get interface capability field: idleTimeout
*
* @param    L7_uint32 intfId @b{(input)} interface ID
* @param    L7_uchar8 *idleTimeout @b{(output)} idleTimeout of 
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

L7_RC_t cpimIntfCapabilityIdleTimeoutGet(L7_uint32 intfId, L7_uchar8 *idleTimeout);


/********************************************************************* 
* @purpose  Get interface capability field: bandwidthUpRateControl
*
* @param    L7_uint32 intfId @b{(input)} interface ID
* @param    L7_uchar8 *bandwidthUpRateControl @b{(output)} bandwidthUpRateControl of 
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

L7_RC_t cpimIntfCapabilityBandwidthUpRateControlGet(L7_uint32 intfId, L7_uchar8 *bandwidthUpRateControl);


/********************************************************************* 
* @purpose  Get interface capability field: bandwidthDownRateControl
*
* @param    L7_uint32 intfId @b{(input)} interface ID
* @param    L7_uchar8 *bandwidthDownRateControl @b{(output)} bandwidthDownRateControl of 
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

L7_RC_t cpimIntfCapabilityBandwidthDownRateControlGet(L7_uint32 intfId, L7_uchar8 *bandwidthDownRateControl);


/********************************************************************* 
* @purpose  Get interface capability field: maxInputOctetMonitor
*
* @param    L7_uint32 intfId @b{(input)} interface ID
* @param    L7_uchar8 *maxInputOctetMonitor @b{(output)} maxInputOctetMonitor of 
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

L7_RC_t cpimIntfCapabilityMaxInputOctetMonitorGet(L7_uint32 intfId, L7_uchar8 *maxInputOctetMonitor);


/********************************************************************* 
* @purpose  Get interface capability field: maxOutputOctetMonitor
*
* @param    L7_uint32 intfId @b{(input)} interface ID
* @param    L7_uchar8 *maxOutputOctetMonitor @b{(output)} maxOutputOctetMonitor of 
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

L7_RC_t cpimIntfCapabilityMaxOutputOctetMonitorGet(L7_uint32 intfId, L7_uchar8 *maxOutputOctetMonitor);


/********************************************************************* 
* @purpose  Get interface capability field: bytesReceivedCounter
*
* @param    L7_uint32 intfId @b{(input)} interface ID
* @param    L7_uchar8 *bytesReceivedCounter @b{(output)} bytesReceivedCounter of 
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

L7_RC_t cpimIntfCapabilityBytesReceivedCounterGet(L7_uint32 intfId, L7_uchar8 *bytesReceivedCounter);


/********************************************************************* 
* @purpose  Get interface capability field: bytesTransmittedCounter
*
* @param    L7_uint32 intfId @b{(input)} interface ID
* @param    L7_uchar8 *bytesTransmittedCounter @b{(output)} bytesTransmittedCounter of 
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

L7_RC_t cpimIntfCapabilityBytesTransmittedCounterGet(L7_uint32 intfId, L7_uchar8 *bytesTransmittedCounter);


/********************************************************************* 
* @purpose  Get interface capability field: packetsReceivedCounter
*
* @param    L7_uint32 intfId @b{(input)} interface ID
* @param    L7_uchar8 *packetsReceivedCounter @b{(output)} packetsReceivedCounter of 
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

L7_RC_t cpimIntfCapabilityPacketsReceivedCounterGet(L7_uint32 intfId, 
					   L7_uchar8 *packetsRXedCounter);


/********************************************************************* 
* @purpose  Get interface capability field: packetsTransmittedCounter
*
* @param    L7_uint32 intfId @b{(input)} interface ID
* @param    L7_uchar8 *packetsTransmittedCounter @b{(output)} 
*           packetsTransmittedCounter of 
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

L7_RC_t cpimIntfCapabilityPacketsTransmittedCounterGet(L7_uint32 intfId, 
				    L7_uchar8 *packetsTXedCounter);


/********************************************************************* 
* @purpose  Get interface capability field: roamingSupport
*
* @param    L7_uint32 intfId @b{(input)} interface ID
* @param    L7_uchar8 *roamingSupport @b{(output)} roamingSupport of 
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

L7_RC_t cpimIntfCapabilityRoamingSupportGet(L7_uint32 intfId, 
					    L7_uchar8 *roamingSupport);


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
L7_RC_t cpimIntfCapabilityDescriptionGet(L7_uint32 intfId, L7_uchar8 *description);


/********************************************************************* 
* @purpose  Get interface capability field: sessionTimeout
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 sessionTimeout @b{(output)} sessionTimeout of interface capability 
*                     item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilitySessionTimeoutSet(L7_uint32 intIfNum, 
					    L7_uchar8 sessionTimeout);


/********************************************************************* 
* @purpose  Get interface capability field: idleTimeout
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 idleTimeout @b{(output)} idleTimeout of interface capability 
*                     item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityIdleTimeoutSet(L7_uint32 intIfNum, 
					 L7_uchar8 idleTimeout);


/********************************************************************* 
* @purpose  Get interface capability field: bandwidthUpRateControl
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 bandwidthUpRateControl @b{(output)} 
*            bandwidthUpRateControl of interface capability 
*             item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityBandwidthUpRateControlSet(L7_uint32 intIfNum, 
					    L7_uchar8 bandwidthUpRateControl);


/********************************************************************* 
* @purpose  Get interface capability field: bandwidthDownRateControl
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 bandwidthDownRateControl @b{(output)}
*                bandwidthDownRateControl of interface capability 
*                     item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityBandwidthDownRateControlSet(L7_uint32 intIfNum, 
				      L7_uchar8 bandwidthDownRateControl);


/********************************************************************* 
* @purpose  Get interface capability field: maxInputOctetMonitor
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 maxInputOctetMonitor @b{(output)} 
*             maxInputOctetMonitor of interface capability 
*                     item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityMaxInputOctetMonitorSet(L7_uint32 intIfNum, 
				  L7_uchar8 maxInputOctetMonitor);


/********************************************************************* 
* @purpose  Get interface capability field: maxOutputOctetMonitor
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 maxOutputOctetMonitor @b{(output)} 
*             maxOutputOctetMonitor of interface capability 
*             item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityMaxOutputOctetMonitorSet(L7_uint32 intIfNum, 
				   L7_uchar8 maxOutputOctetMonitor);


/********************************************************************* 
* @purpose  Get interface capability field: bytesReceivedCounter
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 bytesReceivedCounter @b{(output)} bytesReceivedCounter
*               of interface capability item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityBytesReceivedCounterSet(L7_uint32 intIfNum, 
				  L7_uchar8 bytesReceivedCounter);


/********************************************************************* 
* @purpose  Get interface capability field: bytesTransmittedCounter
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 bytesTransmittedCounter @b{(output)}
*              bytesTransmittedCounter of interface capability 
*              item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityBytesTransmittedCounterSet(L7_uint32 intIfNum, 
				     L7_uchar8 bytesTransmittedCounter);



/********************************************************************* 
* @purpose  Get interface capability field: packetsReceivedCounter
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 packetsReceivedCounter @b{(output)} 
*              packetsReceivedCounter of interface capability 
*                     item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityPacketsReceivedCounterSet(L7_uint32 intIfNum, 
				    L7_uchar8 packetsReceivedCounter);


/********************************************************************* 
* @purpose  Get interface capability field: packetsTransmittedCounter
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 packetsTransmittedCounter @b{(output)} 
*              packetsTransmittedCounter of interface capability 
*                     item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityPacketsTransmittedCounterSet(L7_uint32 intIfNum, 
				       L7_uchar8 packetsTransmittedCounter);


/********************************************************************* 
* @purpose  Get interface capability field: roamingSupport
*
* @param    L7_uint32 intIfNum @b{(input)} interface ID
* @param    L7_uchar8 roamingSupport @b{(output)} 
*              roamingSupport of interface capability 
*                     item with given ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpimIntfCapabilityRoamingSupportSet(L7_uint32 intIfNum, 
					    L7_uchar8 roamingSupport);

/*********************************************************************
*
* @purpose  Block access to an interface
*
* @param    L7_uint32 intIfNum @b{(input)} interface in question
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  
*
* @end
*
*********************************************************************/
L7_RC_t   cpimBlockAccess(L7_uint32 intIfNum);


/*********************************************************************
*
* @purpose  Unblock captive portal access to an interface
*
* @param    L7_uint32 intIfNum @b{(input)} interface in question
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  
*
* @end
*
*********************************************************************/
L7_RC_t cpimUnblockAccess(L7_uint32 intIfNum);


/*********************************************************************
*
* @purpose  Authenticate a MAC address on an interface
*
* @param    L7_uint32 intIfNum @b{(input)} interface in question
* @param    L7_enetMacAddr_t * @b{(input)} client mac address
* @param    L7_uint32 upRate @b{(input)} upstream bitrate ceiling
* @param    L7_uint32 downRate @b{(input)} downstream bitrate ceiling
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  
*
* @end
*
*********************************************************************/
L7_RC_t  cpimAuthenticate(L7_uint32 intIfNum, 
		 	  L7_enetMacAddr_t * clientMacAddr, 
                          L7_uint32 upRate, L7_uint32 downRate);


/*********************************************************************
*
* @purpose  Deauthenticate a MAC address on an interface
*
* @param    L7_uint32 intIfNum @b{(input)} interface in question
* @param    L7_enetMacAddr_t * @b{(input)} client mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  
*
* @end
*
*********************************************************************/
L7_RC_t  cpimDeauthenticate(L7_uint32 intIfNum, 
		   	    L7_enetMacAddr_t * clientMacAddr);

/*********************************************************************
*
* @purpose  Set the port used for authentication
*
* @param    L7_uint32 intIfNum @b{(input)} interface in question
* @param    L7_ushort16 authPortNum @b{(input)} new port value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  
*
* @end
*
*********************************************************************/
L7_RC_t cpimSetAuthPort(L7_uint32   intIfNum,
			L7_ushort16 authPortNum);



#endif /* INCLUDE_CPIM_API_H */

