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

#include "datatypes.h"
#include "l7_common.h"
#include "snmp_trap_api_captive_portal.h"
#include "usmdb_snmp_trap_captive_portal_api.h"
#include "trap_captive_portal_api.h"
#include "snmp_util_api.h"

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
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpCaptivePortalClientConnectTrapSend(L7_enetMacAddr_t macAddr, L7_uint32 ipAddr,
                                                    L7_enetMacAddr_t switchMacAddr, L7_uint32 cpId,
                                                    L7_uint32 cpAssocIfIndex)
{
  return SnmpCaptivePortalClientConnectTrapSend(macAddr, ipAddr,switchMacAddr, cpId, cpAssocIfIndex);
}

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
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpCaptivePortalClientDisconnectTrapSend(L7_enetMacAddr_t macAddr, L7_uint32 ipAddr,
                                                       L7_enetMacAddr_t switchMacAddr, L7_uint32 cpId,
                                                       L7_uint32 cpAssocIfIndex)
{
  return SnmpCaptivePortalClientDisconnectTrapSend(macAddr, ipAddr,switchMacAddr, cpId, cpAssocIfIndex);
}

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
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpCaptivePortalClientAuthFailureTrapSend( L7_enetMacAddr_t macAddr, L7_uint32 ipAddr,
                                                         L7_uchar8 *userName, L7_enetMacAddr_t switchMacAddr, 
                                                         L7_uint32 cpId, L7_uint32 cpAssocIfIndex, 
                                                         L7_uint32 attempts)
{
  return SnmpCaptivePortalClientAuthFailureTrapSend(macAddr, ipAddr,userName, switchMacAddr, cpId, cpAssocIfIndex, attempts);
}

/*********************************************************************
* @purpose  Send a Captive Portal Client Connection Database Full trap
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpCaptivePortalClientConnDatabaseFullTrapSend(void)
{
  return SnmpCaptivePortalClientConnDatabaseFullTrapSend();
}

/*********************************************************************
* @purpose  Send a Captive Portal Authentication Failure Log Wrap trap
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpCaptivePortalAuthFailureLogWrapTrapSend(void)
{
  return SnmpCaptivePortalAuthFailureLogWrapTrapSend();
}

/*********************************************************************
* @purpose  Send a Captive Portal Activity Log Wrap trap
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpCaptivePortalActivityLogWrapLogWrapTrapSend(void)
{
  return SnmpCaptivePortalActivityLogWrapTrapSend();
}

