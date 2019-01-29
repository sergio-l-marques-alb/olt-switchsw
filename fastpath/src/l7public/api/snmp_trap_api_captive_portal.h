/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: snmp_trap_api_captive_portal.h
*
* Purpose: API functions to initiate sending of SNMP traps
*
* Author: wjacobs, rjindal
*
* Component: SNMP
*
*********************************************************************/

#ifndef _SNMP_TRAP_API_CAPTIVE_PORTAL_H
#define _SNMP_TRAP_API_CAPTIVE_PORTAL_H

#include "l7_common.h"
        
/*********************************************************************
* @purpose  Send a Captive Portal Client Connection trap
*
* @param    macAddr         @b{(input)} client MAC address
* @param    ipAddr          @b{(input)} client IP address
* @param    switchMacAddr   @b{(input)} MAC address of authenticating switch
* @param    cpId            @b{(input)} captive portal instance handling the authentication
* @param    cpAssocIfIndex  @b{(input)} ifIndex of port on authenticating switch
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpCaptivePortalClientConnectTrapSend(L7_enetMacAddr_t macAddr, L7_uint32 ipAddr,
                                               L7_enetMacAddr_t switchMacAddr, L7_uint32 cpId,
                                               L7_uint32 cpAssocIfIndex);

/*********************************************************************
* @purpose  Send a Captive Portal Client Disconnect trap
*
* @param    macAddr         @b{(input)} client MAC address
* @param    ipAddr          @b{(input)} client IP address
* @param    switchMacAddr   @b{(input)} MAC address of authenticating switch
* @param    cpId            @b{(input)} captive portal instance handling the authentication
* @param    cpAssocIfIndex  @b{(input)} ifIndex of port on authenticating switch
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpCaptivePortalClientDisconnectTrapSend(L7_enetMacAddr_t macAddr, L7_uint32 ipAddr,
                                                  L7_enetMacAddr_t switchMacAddr, L7_uint32 cpId,
                                                  L7_uint32 cpAssocIfIndex);

/*********************************************************************
* @purpose  Send a Captive Portal Client Authentication Failure trap
*
* @param    macAddr         @b{(input)} client MAC address
* @param    ipAddr          @b{(input)} client IP address
* @param    userName        @b{(input)} client user name
* @param    switchMacAddr   @b{(input)} MAC address of authenticating switch
* @param    cpId            @b{(input)} captive portal instance handling the authentication
* @param    cpAssocIfIndex  @b{(input)} ifIndex of port on authenticating switch
* @param    attempts        @b{(input)} number of connection attempts
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpCaptivePortalClientAuthFailureTrapSend(L7_enetMacAddr_t macAddr, L7_uint32 ipAddr,
                                                   L7_uchar8 *userName, L7_enetMacAddr_t switchMacAddr, 
                                                   L7_uint32 cpId, L7_uint32 cpAssocIfIndex, 
                                                   L7_uint32 attempts);

/*********************************************************************
* @purpose  Send a Captive Portal Client Connection Database Full trap
*
* @param    void
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpCaptivePortalClientConnDatabaseFullTrapSend(void);

/*********************************************************************
* @purpose  Send a Captive Portal Authentication Failure Log Wrap trap
*
* @param    void
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpCaptivePortalAuthFailureLogWrapTrapSend(void);

/*********************************************************************
* @purpose  Send a Captive Portal Activity Log Wrap trap
*
* @param    void
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpCaptivePortalActivityLogWrapTrapSend(void);

#endif /* _SNMP_TRAP_API_BASE_H */

