/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename trap_captive_portal_api.h
*
* @purpose Trap Manager Captive Portal Functions
*
* @component trapmgr
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

#ifndef TRAP_CAPTIVE_PORTAL_API_H
#define TRAP_CAPTIVE_PORTAL_API_H

#include "l7_common.h"

#define TRAPMGR_SPECIFIC_CAPTIVE_NONE                             0
#define TRAPMGR_SPECIFIC_CAPTIVE_PORTAL_CLIENT_CONN               1
#define TRAPMGR_SPECIFIC_CAPTIVE_PORTAL_CLIENT_DISCONN            2
#define TRAPMGR_SPECIFIC_CAPTIVE_PORTAL_CLIENT_AUTH_FAILURE       3
#define TRAPMGR_SPECIFIC_CAPTIVE_PORTAL_CONN_DATABASE_FULL        4
#define TRAPMGR_SPECIFIC_CAPTIVE_PORTAL_AUTH_FAILURE_LOG_WRAPPED  5
#define TRAPMGR_SPECIFIC_CAPTIVE_PORTAL_ACTIVITY_LOG_WRAPPED      6

/*********************************************************************
* @purpose  Send a Captive Portal Client Connection trap
*
* @param    macAddr         @b{(input)} client MAC address
* @param    ipAddr          @b{(input)} client IP address
* @param    switchMacAddr   @b{(input)} MAC address of authenticating switch
* @param    cpId            @b{(input)} captive portal instance handling the authentication
* @param    cpAssocIfIndex  @b{(input)} ifIndex of port on authenticating switch
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrCaptivePortalClientConnectTrap(L7_enetMacAddr_t macAddr, L7_uint32 ipAddr,
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
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrCaptivePortalClientDisconnectTrap(L7_enetMacAddr_t macAddr, L7_uint32 ipAddr,
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
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrCaptivePortalClientAuthFailureTrap(L7_enetMacAddr_t macAddr, L7_uint32 ipAddr,
                                                  L7_uchar8 *userName, L7_enetMacAddr_t switchMacAddr, 
                                                  L7_uint32 cpId, L7_uint32 cpAssocIfIndex, 
                                                  L7_uint32 attempts);

/*********************************************************************
* @purpose  Send a Captive Portal Client Connection Database Full trap
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrCaptivePortalClientConnDatabaseFullTrap(void);

/*********************************************************************
* @purpose  Send a Captive Portal Authentication Failure Log Wrap trap
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrCaptivePortalAuthFailureLogWrapTrap(void);

/*********************************************************************
* @purpose  Send a Captive Portal Activity Log Wrap trap
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrCaptivePortalActivityLogWrapTrap(void);

/*********************************************************************
* @purpose Get the Captive Portal traps configuration mode.
*          
* @param   L7_uint32  *val  @b((output)) L7_ENABLE or L7_DISABLE
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t trapMgrCaptivePortalTrapGet(L7_uint32 *val);

/*********************************************************************
* @purpose Set the Captive Portal traps configuration mode.
*          
* @param   L7_uint32  val  @b((input)) L7_ENABLE or L7_DISABLE
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrCaptivePortalTrapSet(L7_uint32 val);

#endif /* TRAP_QOS_API_H */

