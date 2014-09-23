/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename usmdb_trap_captive_portal.c
*
* @purpose usmdb captive portal trap functions
*
* @component unitmgr
*
* @comments none
*
* @created  08/20/2007
*
* @author   wjacobs
*
* @end
*
**********************************************************************/

#ifndef USMDB_TRAP_CAPTIVE_PORTAL_API_H
#define USMDB_TRAP_CAPTIVE_PORTAL_API_H

/*********************************************************************
*
* @purpose  Send a Captive Portal Client Connection trap
*
* @param    macAddr         @b{(input)} client MAC address
* @param    ipAddr          @b{(input)} client IP address
* @param    switchMacAddr   @b{(input)} MAC address of authenticating switch
* @param    cpId            @b{(input)} captive portal instance handling the authentication
* @param    cpAssocIfIndex  @b{(input)} ifIndex of port on authenticating switch
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpCaptivePortalClientConnectTrapSend( L7_enetMacAddr_t macAddr, L7_uint32 ipAddr,
                                                     L7_enetMacAddr_t switchMacAddr, L7_uint32 cpId,
                                                     L7_uint32 cpAssocIfIndex);

/*********************************************************************
*
* @purpose  Send a Captive Portal Client Disconnect trap
*
* @param    macAddr         @b{(input)} client MAC address
* @param    ipAddr          @b{(input)} client IP address
* @param    switchMacAddr   @b{(input)} MAC address of authenticating switch
* @param    cpId            @b{(input)} captive portal instance handling the authentication
* @param    cpAssocIfIndex  @b{(input)} ifIndex of port on authenticating switch
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpCaptivePortalClientDisconnectTrapSend( L7_enetMacAddr_t macAddr, L7_uint32 ipAddr,
                                                        L7_enetMacAddr_t switchMacAddr, L7_uint32 cpId,
                                                        L7_uint32 cpAssocIfIndex);

/*********************************************************************
*
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
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpCaptivePortalClientAuthFailureTrapSend( L7_enetMacAddr_t macAddr, L7_uint32 ipAddr,
                                                         L7_uchar8 *userName, L7_enetMacAddr_t switchMacAddr, 
                                                         L7_uint32 cpId, L7_uint32 cpAssocIfIndex, 
                                                         L7_uint32 attempts);

/*********************************************************************
*
* @purpose  Send a Captive Portal Client Connection Database Full trap
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpCaptivePortalClientConnDatabaseFullTrapSend( void );


/*********************************************************************
*
* @purpose  Send a Captive Portal Authentication Failure Log Wrap trap
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpCaptivePortalAuthFailureLogWrapTrapSend( void );

/*********************************************************************
*
* @purpose  Send a Captive Portal Activity Log Wrap trap
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpCaptivePortalActivityLogWrapLogWrapTrapSend( void );

#endif /* USMDB_TRAP_CAPTIVE_PORTAL_API_H */
