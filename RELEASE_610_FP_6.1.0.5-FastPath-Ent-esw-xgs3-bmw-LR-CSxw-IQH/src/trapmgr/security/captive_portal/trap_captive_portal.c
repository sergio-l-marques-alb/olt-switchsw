/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename trap_captive_portal.c
*
* @purpose Trap Manager Captive Portal functions
*
* @component trapmgr
*
* @comments none
*
* @created  08/20/2007
*
* @author   wjacobs, rjindal
*
* @end
*
**********************************************************************/

#include <stdio.h>
#include "l7_common.h"
#include "usmdb_sim_api.h"
#include "usmdb_snmp_api.h"
#include "usmdb_util_api.h"
#include "trapapi.h"
#include "trap.h" 
#include "snmp_trap_api_captive_portal.h"
#include "trap_captive_portal_api.h"
#include "usmdb_snmp_trap_captive_portal_api.h"
#include "trapstr_captive_portal.h"

extern trapMgrCfgData_t trapMgrCfgData;
extern trapMgrTrapData_t trapMgrTrapData;

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
L7_RC_t trapMgrCaptivePortalClientConnectTrap(L7_enetMacAddr_t macAddr, L7_uint32 ipAddr,
                                              L7_enetMacAddr_t switchMacAddr, L7_uint32 cpId,
                                              L7_uint32 cpAssocIfIndex)
{
  L7_RC_t  rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 ipAddressBuf[16];
  L7_uint32 unit;

  if (trapMgrCfgData.trapCaptivePortal == L7_ENABLE)
  {
    usmDbInetNtoa(ipAddr, ipAddressBuf);
    sprintf(trapStringBuf, captivePortalClientConnect_str,
            macAddr.addr[0], macAddr.addr[1], macAddr.addr[2], macAddr.addr[3], macAddr.addr[4], macAddr.addr[5],
            ipAddressBuf,
            switchMacAddr.addr[0], switchMacAddr.addr[1], switchMacAddr.addr[2],
            switchMacAddr.addr[3], switchMacAddr.addr[4], switchMacAddr.addr[5],
            cpId, cpAssocIfIndex);

    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen  = TRAPMGR_GENERIC_CAPTIVE_PORTAL;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_CAPTIVE_PORTAL_CLIENT_CONN;
    trapMgrLogTrapToLocalLog(trapStringBuf);

    unit = usmDbThisUnitGet();
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
    {
      /* call into SNMP */
      usmDbSnmpCaptivePortalClientConnectTrapSend(macAddr, ipAddr, switchMacAddr, cpId, cpAssocIfIndex);
    }
  }

  return rc;
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
L7_RC_t trapMgrCaptivePortalClientDisconnectTrap(L7_enetMacAddr_t macAddr, L7_uint32 ipAddr,
                                                 L7_enetMacAddr_t switchMacAddr, L7_uint32 cpId,
                                                 L7_uint32 cpAssocIfIndex)
{
  L7_RC_t  rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 ipAddressBuf[16];
  L7_uint32 unit;

  if (trapMgrCfgData.trapCaptivePortal == L7_ENABLE)
  {
    usmDbInetNtoa(ipAddr, ipAddressBuf);
    sprintf(trapStringBuf, captivePortalClientDisconnect_str,
            macAddr.addr[0], macAddr.addr[1], macAddr.addr[2], macAddr.addr[3], macAddr.addr[4], macAddr.addr[5],
            ipAddressBuf,
            switchMacAddr.addr[0], switchMacAddr.addr[1], switchMacAddr.addr[2],
            switchMacAddr.addr[3], switchMacAddr.addr[4], switchMacAddr.addr[5],
            cpId, cpAssocIfIndex);

    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen  = TRAPMGR_GENERIC_CAPTIVE_PORTAL;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_CAPTIVE_PORTAL_CLIENT_DISCONN;
    trapMgrLogTrapToLocalLog(trapStringBuf);

    unit = usmDbThisUnitGet();
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
    {
      /* call into SNMP */
      usmDbSnmpCaptivePortalClientDisconnectTrapSend(macAddr, ipAddr, switchMacAddr, cpId, cpAssocIfIndex);
    }
  }

  return rc;
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
* @notes    We don't currently "log" the number of attempts because
*           we're not currently keeping track.
*
* @end
*********************************************************************/
L7_RC_t trapMgrCaptivePortalClientAuthFailureTrap(L7_enetMacAddr_t macAddr, L7_uint32 ipAddr,
                                                  L7_uchar8 *userName, L7_enetMacAddr_t switchMacAddr, 
                                                  L7_uint32 cpId, L7_uint32 cpAssocIfIndex, 
                                                  L7_uint32 attempts)
{
  L7_RC_t  rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 ipAddressBuf[16];
  L7_uint32 unit;

  if (trapMgrCfgData.trapCaptivePortal == L7_ENABLE)
  {
    usmDbInetNtoa(ipAddr, ipAddressBuf);
    sprintf(trapStringBuf, captivePortalClientAuthFailure_str,
            macAddr.addr[0], macAddr.addr[1], macAddr.addr[2], macAddr.addr[3], macAddr.addr[4], macAddr.addr[5],
            ipAddressBuf,
            switchMacAddr.addr[0], switchMacAddr.addr[1], switchMacAddr.addr[2],
            switchMacAddr.addr[3], switchMacAddr.addr[4], switchMacAddr.addr[5],
            cpId, cpAssocIfIndex, userName); /* not logging attempts */

    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen  = TRAPMGR_GENERIC_CAPTIVE_PORTAL;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_CAPTIVE_PORTAL_CLIENT_AUTH_FAILURE;
    trapMgrLogTrapToLocalLog(trapStringBuf);

    unit = usmDbThisUnitGet();
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
    {
      /* call into SNMP */
      usmDbSnmpCaptivePortalClientAuthFailureTrapSend(macAddr, ipAddr, userName, switchMacAddr, 
                                                      cpId, cpAssocIfIndex, attempts);
    }
  }

  return rc;
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
L7_RC_t trapMgrCaptivePortalClientConnDatabaseFullTrap(void)
{
  L7_RC_t  rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;

  if (trapMgrCfgData.trapCaptivePortal == L7_ENABLE)
  {
    sprintf(trapStringBuf, captivePortalConnectionDatabaseFull_str);

    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen  = TRAPMGR_GENERIC_CAPTIVE_PORTAL;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_CAPTIVE_PORTAL_CONN_DATABASE_FULL;
    trapMgrLogTrapToLocalLog(trapStringBuf);

    unit = usmDbThisUnitGet();
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
    {
      /* call into SNMP */
      usmDbSnmpCaptivePortalClientConnDatabaseFullTrapSend();
    }
  }

  return rc;
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
L7_RC_t trapMgrCaptivePortalAuthFailureLogWrapTrap(void)
{
  L7_RC_t  rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;

  if (trapMgrCfgData.trapCaptivePortal == L7_ENABLE)
  {
    sprintf(trapStringBuf, captivePortalAuthFailureLogWrapped_str);

    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen  = TRAPMGR_GENERIC_CAPTIVE_PORTAL;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_CAPTIVE_PORTAL_AUTH_FAILURE_LOG_WRAPPED;
    trapMgrLogTrapToLocalLog(trapStringBuf);

    unit = usmDbThisUnitGet();
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
    {
      /* call into SNMP */
      usmDbSnmpCaptivePortalAuthFailureLogWrapTrapSend();
    }
  }

  return rc;
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
L7_RC_t trapMgrCaptivePortalActivityLogWrapTrap( void )
{
  L7_RC_t  rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;

  if (trapMgrCfgData.trapCaptivePortal == L7_ENABLE)
  {
    sprintf(trapStringBuf, captivePortalActivityLoggedWrapped_str);

    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen  = TRAPMGR_GENERIC_CAPTIVE_PORTAL;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_CAPTIVE_PORTAL_ACTIVITY_LOG_WRAPPED;
    trapMgrLogTrapToLocalLog(trapStringBuf);

    unit = usmDbThisUnitGet();
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
    {
      /* call into SNMP */
      usmDbSnmpCaptivePortalActivityLogWrapLogWrapTrapSend();
    }
  }

  return rc;
}

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
L7_RC_t trapMgrCaptivePortalTrapGet(L7_uint32 *val)
{
  if (val == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *val = trapMgrCfgData.trapCaptivePortal;
  return L7_SUCCESS;
}

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
L7_RC_t trapMgrCaptivePortalTrapSet(L7_uint32 val)
{
  if (val != L7_ENABLE && val != L7_DISABLE)
  {
    return L7_FAILURE;
  }

  if (val != trapMgrCfgData.trapCaptivePortal)
  {
    trapMgrCfgData.trapCaptivePortal = val;
    trapMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
  }

  return L7_SUCCESS;
}

