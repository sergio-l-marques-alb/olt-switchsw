/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
*
* Name: privatetrap_captive_portal.c
*
* Purpose: Captive Portal trap functions
*
* @component snmp
*
* @comments none
*
* @created 08/20/2007
*
* @author wjacobs, rjindal
*
* @end
*    
*********************************************************************/

#include "sr_conf.h"

#ifdef HAVE_STDIO_H
 #include <stdio.h>
#endif
#ifdef HAVE_MEMORY_H
 #include <memory.h>
#endif

#include "sr_snmp.h"
#include "diag.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "mibout.h"
#include "getvar.h"
#include "sr_ntfy.h"
#include "k_private_base.h" 
#include "l7_common.h"
#include "osapi_support.h"
#include "snmp_trap_api_captive_portal.h"
#include "k_mib_captive_portal_api.h"

/*********************************************************************
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
L7_RC_t SnmpCaptivePortalClientConnectTrapSend(L7_enetMacAddr_t macAddr, L7_uint32 ipAddr,
                                               L7_enetMacAddr_t switchMacAddr, L7_uint32 cpId,
                                               L7_uint32 cpAssocIfIndex)
{
    L7_uint32 status;
    VarBind *temp_vb = NULL;
    L7_IP_ADDR_t netIP;

    OID *snmpTrapOID       = MakeOIDFromDot("cpClientConnect");
    OID *oid_mac           = MakeOIDFromDot("cpCaptivePortalClientMacAddress");
    OID *oid_ip            = MakeOIDFromDot("cpCaptivePortalClientIpAddress");
    OID *oid_switch_mac    = MakeOIDFromDot("cpCaptivePortalClientSwitchMacAddress");
    OID *oid_cp_id         = MakeOIDFromDot("cpCaptivePortalClientCPID");
    OID *oid_assoc_ifIndex = MakeOIDFromDot("cpCaptivePortalClientAssocIfIndex");

    OctetString *os_mac = L7_NULLPTR;
    VarBind *var_mac = L7_NULLPTR;
    OctetString *os_ip = L7_NULLPTR;
    VarBind *var_ip = L7_NULLPTR;
    OctetString *os_switch_mac = L7_NULLPTR;
    VarBind *var_switch_mac = L7_NULLPTR;
    VarBind *var_cp_id = L7_NULLPTR;
    VarBind *var_assoc_ifIndex = L7_NULLPTR;

    os_mac = MakeOctetString((L7_uchar8 *)&macAddr.addr, L7_ENET_MAC_ADDR_LEN);
    var_mac = MakeVarBindWithValue(oid_mac, NULL, OCTET_PRIM_TYPE, os_mac);

    netIP = osapiHtonl(ipAddr);
    os_ip = MakeOctetString((char *)&netIP, L7_IP_ADDR_LEN);
    var_ip = MakeVarBindWithValue(oid_ip, NULL, IP_ADDR_PRIM_TYPE, os_ip); 

    os_switch_mac = MakeOctetString((L7_uchar8 *)&switchMacAddr.addr, L7_ENET_MAC_ADDR_LEN);
    var_switch_mac = MakeVarBindWithValue(oid_switch_mac, NULL, OCTET_PRIM_TYPE, os_switch_mac);

    var_cp_id = MakeVarBindWithValue(oid_cp_id, NULL, INTEGER_TYPE, &cpId);
    var_assoc_ifIndex = MakeVarBindWithValue(oid_assoc_ifIndex, NULL, INTEGER_TYPE, &cpAssocIfIndex);

    FreeOID(oid_mac);
    FreeOID(oid_ip);
    FreeOID(oid_switch_mac);
    FreeOID(oid_cp_id);
    FreeOID(oid_assoc_ifIndex);

    if (snmpTrapOID == NULL || var_mac == NULL || var_ip == NULL || var_switch_mac == NULL ||
        var_cp_id == NULL || var_assoc_ifIndex == NULL)
    {
        FreeOID(snmpTrapOID);

        if (var_mac == NULL)
          FreeOctetString(os_mac);
        else
          FreeVarBind(var_mac);

        if (var_ip == NULL)
          FreeOctetString(os_ip);
        else
          FreeVarBind(var_ip);

        if (var_switch_mac == NULL)
          FreeOctetString(os_switch_mac);
        else
          FreeVarBind(var_switch_mac);

        if (var_cp_id != NULL)
          FreeVarBind(var_cp_id);

        if (var_assoc_ifIndex != NULL)
          FreeVarBind(var_assoc_ifIndex);

        return L7_FAILURE;
    }

    temp_vb = var_mac;
    var_mac->next_var = var_ip;
    var_ip->next_var = var_switch_mac;
    var_switch_mac->next_var = var_cp_id;
    var_cp_id->next_var = var_assoc_ifIndex;
    var_assoc_ifIndex->next_var = NULL;

    status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

    FreeOID(snmpTrapOID);
    FreeVarBindList(temp_vb);

    if (status == 0)
      return L7_SUCCESS;
    else
      return L7_FAILURE;
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
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t SnmpCaptivePortalClientDisconnectTrapSend(L7_enetMacAddr_t macAddr, L7_uint32 ipAddr,
                                                  L7_enetMacAddr_t switchMacAddr, L7_uint32 cpId,
                                                  L7_uint32 cpAssocIfIndex)
{
    L7_uint32 status;
    VarBind *temp_vb = NULL;
    L7_IP_ADDR_t netIP;

    OID *snmpTrapOID    = MakeOIDFromDot("cpClientDisconnect");
    OID *oid_mac        = MakeOIDFromDot("cpCaptivePortalClientMacAddress");
    OID *oid_ip         = MakeOIDFromDot("cpCaptivePortalClientIpAddress");
    OID *oid_switch_mac = MakeOIDFromDot("cpCaptivePortalClientSwitchMacAddress");
    OID *oid_cp_id      = MakeOIDFromDot("cpCaptivePortalClientCPID");
    OID *oid_assoc_ifIndex = MakeOIDFromDot("cpCaptivePortalClientAssocIfIndex");

    OctetString *os_mac = L7_NULLPTR;
    VarBind *var_mac = L7_NULLPTR;
    OctetString *os_ip = L7_NULLPTR;
    VarBind *var_ip = L7_NULLPTR;
    OctetString *os_switch_mac = L7_NULLPTR;
    VarBind *var_switch_mac = L7_NULLPTR;
    VarBind *var_cp_id = L7_NULLPTR;
    VarBind *var_assoc_ifIndex = L7_NULLPTR;

    os_mac = MakeOctetString((L7_uchar8 *)&macAddr.addr, L7_ENET_MAC_ADDR_LEN);
    var_mac = MakeVarBindWithValue(oid_mac, NULL, OCTET_PRIM_TYPE, os_mac);

    netIP = osapiHtonl(ipAddr);
    os_ip = MakeOctetString((char *)&netIP, L7_IP_ADDR_LEN);
    var_ip = MakeVarBindWithValue(oid_ip, NULL, IP_ADDR_PRIM_TYPE, os_ip); 

    os_switch_mac = MakeOctetString((L7_uchar8 *)&switchMacAddr.addr, L7_ENET_MAC_ADDR_LEN);
    var_switch_mac = MakeVarBindWithValue(oid_switch_mac, NULL, OCTET_PRIM_TYPE, os_switch_mac);

    var_cp_id = MakeVarBindWithValue(oid_cp_id, NULL, INTEGER_TYPE, &cpId);
    var_assoc_ifIndex = MakeVarBindWithValue(oid_assoc_ifIndex, NULL, INTEGER_TYPE, &cpAssocIfIndex);

    FreeOID(oid_mac);
    FreeOID(oid_ip);
    FreeOID(oid_switch_mac);
    FreeOID(oid_cp_id);
    FreeOID(oid_assoc_ifIndex);

    if (snmpTrapOID == NULL || var_mac == NULL || var_ip == NULL || var_switch_mac == NULL ||
        var_cp_id == NULL || var_assoc_ifIndex == NULL)
    {
        FreeOID(snmpTrapOID);

        if (var_mac == NULL)
          FreeOctetString(os_mac);
        else
          FreeVarBind(var_mac);

        if (var_ip == NULL)
          FreeOctetString(os_ip);
        else
          FreeVarBind(var_ip);

        if (var_switch_mac == NULL)
          FreeOctetString(os_switch_mac);
        else
          FreeVarBind(var_switch_mac);

        if (var_cp_id != NULL)
          FreeVarBind(var_cp_id);

        if (var_assoc_ifIndex != NULL)
          FreeVarBind(var_assoc_ifIndex);

        return L7_FAILURE;
    }

    temp_vb = var_mac;
    var_mac->next_var = var_ip;
    var_ip->next_var = var_switch_mac;
    var_switch_mac->next_var = var_cp_id;
    var_cp_id->next_var = var_assoc_ifIndex;
    var_assoc_ifIndex->next_var = NULL;

    status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

    FreeOID(snmpTrapOID);
    FreeVarBindList(temp_vb);

    if (status == 0)
      return L7_SUCCESS;
    else
      return L7_FAILURE;
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
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t SnmpCaptivePortalClientAuthFailureTrapSend(L7_enetMacAddr_t macAddr, L7_uint32 ipAddr,
                                                   L7_uchar8 *userName, L7_enetMacAddr_t switchMacAddr, 
                                                   L7_uint32 cpId, L7_uint32 cpAssocIfIndex, 
                                                   L7_uint32 attempts)
{
    L7_uint32 status;
    VarBind *temp_vb = NULL;
    L7_IP_ADDR_t netIP;

    OID *snmpTrapOID    = MakeOIDFromDot("cpClientAuthenticationFailure");
    OID *oid_mac        = MakeOIDFromDot("cpCaptivePortalClientMacAddress");
    OID *oid_ip         = MakeOIDFromDot("cpCaptivePortalClientIpAddress");
    OID *oid_user_name  = MakeOIDFromDot("cpCaptivePortalClientUserName");
    OID *oid_switch_mac = MakeOIDFromDot("cpCaptivePortalClientSwitchMacAddress");
    OID *oid_cp_id      = MakeOIDFromDot("cpCaptivePortalClientCPID");
    OID *oid_switch_ifIndex = MakeOIDFromDot("cpCaptivePortalClientAssocIfIndex");
    OID *oid_attempts = MakeOIDFromDot("cpCaptivePortalClientAuthFailureConnectionAttempts");

    OctetString *os_mac = L7_NULLPTR;
    VarBind *var_mac = L7_NULLPTR;
    OctetString *os_ip = L7_NULLPTR;
    VarBind *var_ip = L7_NULLPTR;
    OctetString *os_switch_mac = L7_NULLPTR;
    VarBind *var_switch_mac = L7_NULLPTR;
    OctetString *os_user_name = L7_NULLPTR;
    VarBind *var_user_name = L7_NULLPTR;
    VarBind *var_cp_id = L7_NULLPTR;
    VarBind *var_switch_ifIndex = L7_NULLPTR;
    VarBind *var_attempts = L7_NULLPTR;

    os_mac = MakeOctetString((L7_uchar8 *)&macAddr.addr, L7_ENET_MAC_ADDR_LEN);
    var_mac = MakeVarBindWithValue(oid_mac, NULL, OCTET_PRIM_TYPE, os_mac);

    netIP = osapiHtonl(ipAddr);
    os_ip = MakeOctetString((char *)&netIP, L7_IP_ADDR_LEN);
    var_ip = MakeVarBindWithValue(oid_ip, NULL, IP_ADDR_PRIM_TYPE, os_ip); 

    os_switch_mac = MakeOctetString((L7_uchar8 *)&switchMacAddr.addr, L7_ENET_MAC_ADDR_LEN);
    var_switch_mac = MakeVarBindWithValue(oid_switch_mac, NULL, OCTET_PRIM_TYPE, os_switch_mac);

    os_user_name = MakeOctetString((L7_uchar8 *)userName, 32);
    var_user_name = MakeVarBindWithValue(oid_user_name, NULL, OCTET_PRIM_TYPE, os_user_name);

    var_cp_id = MakeVarBindWithValue(oid_cp_id, NULL, INTEGER_TYPE, &cpId);
    var_switch_ifIndex = MakeVarBindWithValue(oid_switch_ifIndex, NULL, INTEGER_TYPE, &cpAssocIfIndex);
    var_attempts = MakeVarBindWithValue(oid_attempts, NULL, INTEGER_TYPE, &attempts);

    FreeOID(oid_mac);
    FreeOID(oid_ip);
    FreeOID(oid_switch_mac);
    FreeOID(oid_user_name);
    FreeOID(oid_cp_id);
    FreeOID(oid_switch_ifIndex);
    FreeOID(oid_attempts);

    if (snmpTrapOID == NULL || var_mac == NULL || var_ip == NULL || var_switch_mac == NULL ||
        var_user_name == NULL || var_cp_id == NULL || var_switch_ifIndex == NULL || var_attempts == NULL)
    {
        FreeOID(snmpTrapOID);

        if (var_mac == NULL)
          FreeOctetString(os_mac);
        else
          FreeVarBind(var_mac);

        if (var_ip == NULL)
          FreeOctetString(os_ip);
        else
          FreeVarBind(var_ip);

        if (var_switch_mac == NULL)
          FreeOctetString(os_switch_mac);
        else
          FreeVarBind(var_switch_mac);

        if (var_user_name == NULL)
          FreeOctetString(os_user_name);
        else
          FreeVarBind(var_user_name);

        if (var_cp_id != NULL)
          FreeVarBind(var_cp_id);

        if (var_switch_ifIndex != NULL)
          FreeVarBind(var_switch_ifIndex);

        if (var_attempts != NULL)
          FreeVarBind(var_attempts);

        return L7_FAILURE;
    }

    temp_vb = var_mac;
    var_mac->next_var = var_ip;
    var_ip->next_var = var_switch_mac;
    var_switch_mac->next_var = var_cp_id;
    var_cp_id->next_var = var_switch_ifIndex;
    var_switch_ifIndex->next_var = var_user_name;
    var_user_name->next_var = var_attempts;
    var_attempts->next_var = NULL;

    status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

    FreeOID(snmpTrapOID);
    FreeVarBindList(temp_vb);

    if (status == 0)
      return L7_SUCCESS;
    else
      return L7_FAILURE;
}

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
L7_RC_t SnmpCaptivePortalClientConnDatabaseFullTrapSend(void)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("cpClientDatabaseFull");

  if (snmpTrapOID == NULL)
    return L7_FAILURE;

  status = SendNotificationsSMIv2Params(snmpTrapOID, NULL, NULL);

  FreeOID(snmpTrapOID);

  if (status == 0)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

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
L7_RC_t SnmpCaptivePortalAuthFailureLogWrapTrapSend(void)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("cpAuthFailureLogWrapped");

  if (snmpTrapOID == NULL)
    return L7_FAILURE;

  status = SendNotificationsSMIv2Params(snmpTrapOID, NULL, NULL);

  FreeOID(snmpTrapOID);

  if (status == 0)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

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
L7_RC_t SnmpCaptivePortalActivityLogWrapTrapSend(void)
{
  L7_uint32 status;
  OID *snmpTrapOID = MakeOIDFromDot("cpActivityLogWrapped");

  if (snmpTrapOID == NULL)
    return L7_FAILURE;

  status = SendNotificationsSMIv2Params(snmpTrapOID, NULL, NULL);

  FreeOID(snmpTrapOID);

  if (status == 0)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

L7_RC_t captivePortalTrapsTest(L7_int32 trapType)
{
  L7_enetMacAddr_t macAddr;
  L7_enetMacAddr_t switchMacAddr;
  L7_IP_ADDR_t ipAddr = 0xAC16232Cul; /* 172.22.35.44 */
  L7_uint32 cpId = 5;
  L7_uint32 cpAssocIfIndex = 18;
  L7_char8 userName[] = "MyTestCaptivePortalUser";
  L7_int32 attempts   = 75;
  L7_RC_t rc = L7_FAILURE;

  memset(&macAddr, 0x00, sizeof(macAddr));
  macAddr.addr[0] = 0x11;
  macAddr.addr[1] = 0x11;
  macAddr.addr[2] = 0x11;
  macAddr.addr[3] = 0x11;
  macAddr.addr[4] = 0x11;
  macAddr.addr[5] = 0x11;

  memset(&switchMacAddr, 0x00, sizeof(switchMacAddr));
  switchMacAddr.addr[0] = 0x12;
  switchMacAddr.addr[1] = 0x12;
  switchMacAddr.addr[2] = 0x12;
  switchMacAddr.addr[3] = 0x12;
  switchMacAddr.addr[4] = 0x12;
  switchMacAddr.addr[5] = 0x12;

  switch(trapType)
  {
   case 1:
     rc = SnmpCaptivePortalClientConnectTrapSend(macAddr, ipAddr, switchMacAddr, cpId, cpAssocIfIndex);
     break;
   case 2:
     rc = SnmpCaptivePortalClientDisconnectTrapSend(macAddr, ipAddr, switchMacAddr, cpId, cpAssocIfIndex);
     break;
   case 3:
     rc = SnmpCaptivePortalClientAuthFailureTrapSend(macAddr, ipAddr, userName, switchMacAddr, cpId, cpAssocIfIndex, attempts);
     break;
   case 4:
     rc = SnmpCaptivePortalClientConnDatabaseFullTrapSend();
     break;
   case 5:
     rc = SnmpCaptivePortalAuthFailureLogWrapTrapSend();
     break;
   case 6:
     rc = SnmpCaptivePortalActivityLogWrapTrapSend();
     break;
   default:
     break;
  }
  return rc;
}

