/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  wio_api.c
*
* @purpose   Public APIs for captive portal wired interface owner
*
* @component captive portal wired interface owner
*
* @comments  yeah, these are public APIs, but the only references to 
*            them will be through wioCallbacks.
*
* @create    2/27/2008
*
* @author    rrice
*
* @end
*
**********************************************************************/


#include "captive_portal_commdefs.h"
#include "wio_util.h"
#include "log.h" 
#include "l7utils_api.h"

extern wioInfo_t *wioInfo;


/*********************************************************************
* @purpose  Get pointer to Captive Portal call back functions for 
*           wired interface owner
*
* @param    void *cpCbPtr       pointer to a pointer of structure of  
*                               wireless CP call back function pointers
*
* @notes    none
*            
*       
* @end
*********************************************************************/
void wioCpCallbackGet(void **cpCbPtr)
{
    *cpCbPtr = (void *)wioInfo->wioCallbacks;
    return;
}

/*********************************************************************
* @purpose  Set a capability on a wired captive portal interface.
*
* @param    intIfNum     Internal Interface Number  
* @param    cpCapType	   CP Capability Type
* @param    cpCap        CP Capability to be set
*
* @returns  L7_FAILURE
*
* @notes    no-op. Not supporting this. Not described in FS.
*       
* @end
*********************************************************************/
L7_RC_t wioCpCapabilitySet(L7_uint32 intIfNum,
                           L7_INTF_PARM_CP_TYPES_t  cpCapType,
                           L7_uint32 cpCap)
{
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Get the set of capabilities on a captive portal wired interface. 
*
* @param    intIfNum     Internal Interface Number  
* @param    cpCapType	   CP Capability Type
* @param    cpCap        CP Capability to be get
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    handled synchronously    
*
* @end
*********************************************************************/
L7_RC_t wioCpCapabilityGet(L7_uint32 intIfNum,
                           L7_INTF_PARM_CP_TYPES_t  cpCapType,
                           L7_uint32 *cpCap)
{
  if (osapiSemaTake(wioInfo->wioLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (cpCapType == L7_INTF_PARM_CP_ALL)
  {
    /* Asking for all capabilities */
    *cpCap = wioIntfCapabilities(intIfNum);
  }
  else
  {
    /* Asking about an individual capability */
    if (wioIntfIsCapable(intIfNum, cpCapType))
      *cpCap = L7_ENABLE;
    else
      *cpCap = L7_DISABLE;
  }

  osapiSemaGive(wioInfo->wioLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  CP calls this API to tell the wired interface owner that
*           a client is either authenticated or unauthenticated. 
*
* @param    msgType       CP Operation type
* @param    clientMacAddr Client MAC address
* @param    upRate        Up Data Rate 
* @param    downRate      Down Data Rate     
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    notification is handled asynchronously  
*           upRate and downRate intentionally ignored  
*
* @end
*********************************************************************/
L7_RC_t wioCpAuthenUnauthenClient(CP_OP_t cpCmd, 
                                  L7_enetMacAddr_t clientMacAddr, 
                                  L7_uint32 upRate, 
                                  L7_uint32 downRate)
{
  e_WioAuthEvent event;
  wioEventMsg_t msg; 
  L7_RC_t rc = L7_SUCCESS;

  if (cpCmd == CP_CLIENT_NOT_AUTH)
    event = WIO_CLIENT_EVENT_UNAUTH;
  else if (cpCmd == CP_CLIENT_AUTH)
    event = WIO_CLIENT_EVENT_AUTH;
  else
  {
    L7_uchar8 macAddrStr[WIO_MAC_STR_LEN];
    l7utilsMacAddrHexToString(clientMacAddr.addr, WIO_MAC_STR_LEN, macAddrStr);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
            "Captive portal reported client %s in invalid state %u.",
            macAddrStr, (L7_uint32) cpCmd);
    return L7_FAILURE; 
  }

  if (osapiSemaTake(wioInfo->wioLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (wioInfo->wioTraceFlags & WIO_TRACE_CLIENT_LIST)
  {
    L7_uchar8 wioTrace[WIO_MAX_TRACE_LEN];
    L7_uchar8 macAddrStr[WIO_MAC_STR_LEN];
    l7utilsMacAddrHexToString(clientMacAddr.addr, WIO_MAC_STR_LEN, macAddrStr);
    osapiSnprintf(wioTrace, WIO_MAX_TRACE_LEN, 
                  "CP reports client %s is %s.", 
                  macAddrStr, 
                  (event == WIO_CLIENT_AUTH) ? "authenticated" : "unauthenticated");
    wioTraceWrite(wioTrace);
  }

  msg.wioMsgData.clientAuthMsg.auth = event;
  memcpy(&msg.wioMsgData.clientAuthMsg.clientMac, &clientMacAddr, sizeof(L7_enetMacAddr_t));
  msg.msgType = WIO_CP_AUTH;
  if (osapiMessageSend(wioInfo->wioMsgQ, &msg, sizeof(wioEventMsg_t),
                       L7_NO_WAIT, L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
  {
    osapiSemaGive(wioInfo->msgQSema);
  }
  else
  {
    L7_LOG(L7_LOG_SEVERITY_ALERT, L7_CP_WIO_COMPONENT_ID, 
           "Failure sending CP wired interface owner authentication message.");
    rc = L7_FAILURE;
  }

  osapiSemaGive(wioInfo->wioLock);
  return rc;
}

/*********************************************************************
* @purpose  Deauthenticate a captive portal client on a wired interface. 
*
* @param    clientMacAddr Client MAC address 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wioCpDeauthenClient(L7_enetMacAddr_t clientMacAddr)
{
  wioEventMsg_t msg; 
  L7_RC_t rc = L7_SUCCESS;

  if (osapiSemaTake(wioInfo->wioLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (wioInfo->wioTraceFlags & WIO_TRACE_CLIENT_LIST)
  {
    L7_uchar8 wioTrace[WIO_MAX_TRACE_LEN];
    L7_uchar8 macAddrStr[WIO_MAC_STR_LEN];
    l7utilsMacAddrHexToString(clientMacAddr.addr, WIO_MAC_STR_LEN, macAddrStr);
    osapiSnprintf(wioTrace, WIO_MAX_TRACE_LEN, 
                  "CP reports client %s is deauthenticated.", macAddrStr);
    wioTraceWrite(wioTrace);
  }

  msg.wioMsgData.clientAuthMsg.auth = WIO_CLIENT_EVENT_DEAUTH;
  memcpy(&msg.wioMsgData.clientAuthMsg.clientMac, &clientMacAddr, sizeof(L7_enetMacAddr_t));
  msg.msgType = WIO_CP_AUTH;
  if (osapiMessageSend(wioInfo->wioMsgQ, &msg, sizeof(wioEventMsg_t),
                       L7_NO_WAIT, L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
  {
    osapiSemaGive(wioInfo->msgQSema);
  }
  else
  {
    L7_LOG(L7_LOG_SEVERITY_ALERT, L7_CP_WIO_COMPONENT_ID, 
           "Failure sending CP wired interface owner deauthentication message.");
    rc = L7_FAILURE;
  }

  osapiSemaGive(wioInfo->wioLock);
  return rc;
}

/*********************************************************************
* @purpose  Block or unblock user access to the network interface 
*
* @param    cpCmd      CP Operation type
* @param    intIfNum   Internal Interface Number
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wioCpBlockUnblockAccess(CP_OP_t cpCmd, 
                                L7_uint32 intIfNum)
{
  wioEventMsg_t msg; 
  e_WioIntfBlock block;
  L7_RC_t rc = L7_SUCCESS;

  if (cpCmd == CP_BLOCK)
    block = WIO_INTF_BLOCK;
  else if (cpCmd == CP_UNBLOCK)
    block = WIO_INTF_UNBLOCK;
  else
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
            "Captive portal reported bad block value %u for interface %s",
            (L7_uint32) cpCmd, ifName);
    return L7_FAILURE; 
  }

  if (osapiSemaTake(wioInfo->wioLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (wioInfo->wioTraceFlags & WIO_TRACE_BLOCK)
  {
    L7_uchar8 wioTrace[WIO_MAX_TRACE_LEN];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    osapiSnprintf(wioTrace, WIO_MAX_TRACE_LEN, 
                  "CP reports interface %s is %s",
                  ifName, (block == WIO_INTF_BLOCK) ? "blocked" : "unblocked");
    wioTraceWrite(wioTrace);
  }

  msg.wioMsgData.blockMsg.block = block;
  msg.wioMsgData.blockMsg.intIfNum = intIfNum;
  msg.msgType = WIO_CP_BLOCK;
  if (osapiMessageSend(wioInfo->wioMsgQ, &msg, sizeof(wioEventMsg_t),
                       L7_NO_WAIT, L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
  {
    osapiSemaGive(wioInfo->msgQSema);
  }
  else
  {
    L7_LOG(L7_LOG_SEVERITY_ALERT, L7_CP_WIO_COMPONENT_ID, 
           "Failure sending CP wired interface owner block message.");
    rc = L7_FAILURE;
  }

  osapiSemaGive(wioInfo->wioLock);
  return rc;
}

/*********************************************************************
* @purpose  Enable or disable captive portal on a wired interface. 
*
* @param    cpCmd       CP Operation type
* @param    intIfNum    Internal Interface Number
* @param    redirIp     IP address of local authentication server 
* @param    redirIpMask IP subnet mask of local authentication server 
* @param    intfMac     mac address with length L7_MAC_ADDR_LEN
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wioCpEnableDisableFeature(CP_OP_t cpCmd, 
                                  L7_uint32 intIfNum, 
                                  cpRedirIpAddr_t redirIp,
                                  L7_uint32 redirIpMask,
                                  L7_uchar8 intfMac[L7_ENET_MAC_ADDR_LEN])
{
  wioEventMsg_t msg; 
  e_WioIntfEvent intfEvent;
  L7_RC_t rc = L7_SUCCESS;

  if (cpCmd == CP_ENABLE)
    intfEvent = WIO_INTF_EVENT_ENABLE;
  else if (cpCmd == CP_DISABLE)
    intfEvent = WIO_INTF_EVENT_DISABLE;
  else
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
            "Captive portal reported bad interface state %u for interface %s",
            (L7_uint32) cpCmd, ifName);
    return L7_FAILURE; 
  }

  /* Make sure auth server address is an IPv4 address */
  if (redirIp.redirIpAddrType != CP_IPV4)
  {
    L7_LOG(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
            "Captive portal reported invalid address type for local authentication server.");
    return L7_FAILURE; 
  }

  if (osapiSemaTake(wioInfo->wioLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (wioInfo->wioTraceFlags & WIO_TRACE_CP_STATE)
  {
    L7_uchar8 wioTrace[WIO_MAX_TRACE_LEN];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    osapiSnprintf(wioTrace, WIO_MAX_TRACE_LEN, 
                  "CP reports interface %s is %s",
                  ifName, (intfEvent == WIO_INTF_EVENT_ENABLE) ? "enabled" : "disabled");
    wioTraceWrite(wioTrace);
  }

  msg.wioMsgData.cpEnableMsg.cpEvent = intfEvent;
  msg.wioMsgData.cpEnableMsg.intIfNum = intIfNum;
  msg.wioMsgData.cpEnableMsg.authServer = redirIp.ipAddr.redirIpv4Addr;
  msg.msgType = WIO_CP_INTF_STATE;
  if (osapiMessageSend(wioInfo->wioMsgQ, &msg, sizeof(wioEventMsg_t),
                       L7_NO_WAIT, L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
  {
    osapiSemaGive(wioInfo->msgQSema);
  }
  else
  {
    L7_LOG(L7_LOG_SEVERITY_ALERT, L7_CP_WIO_COMPONENT_ID, 
           "Failure sending CP wired interface owner interface state message.");
    rc = L7_FAILURE;
  }

  osapiSemaGive(wioInfo->wioLock);
  return rc;
}

/*********************************************************************
*
* @purpose  Get Client Captive Portal Statistics
*
* @param    L7_enetMacAddr_t macAddrClient @b{(input)} client MAC address
* @param    txRxCount_t      clientCounter @b{(output)} client stats
* @param    L7_uint32        timePassed @b{(output)} time passed since 
*                                                    last update  
*
* @returns  L7_NOT_SUPPORTED
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wioCpClientStatisticsGet(L7_enetMacAddr_t macAddrClient, 
                                 txRxCount_t *clientCounter, 
                                 L7_uint32 *timePassed)
{
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Implementation function to set one of the optional 
*           authentication ports. 
*
* @param    intIfNum      Internal Interface Number 
* @param    authPortNum   Authentication Port Number to be set      
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
static L7_RC_t wioCpAuthPortNumSet(L7_uint32 intIfNum, L7_ushort16 authPortNum, 
                                   e_WioOptPortFlavor portFlavor)
{
  wioEventMsg_t msg; 
  L7_RC_t rc = L7_SUCCESS;

  if (osapiSemaTake(wioInfo->wioLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (wioInfo->wioTraceFlags & WIO_TRACE_AUTH_SERVER)
  {
    L7_uchar8 wioTrace[WIO_MAX_TRACE_LEN];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    osapiSnprintf(wioTrace, WIO_MAX_TRACE_LEN, 
                  "CP setting web server port %d to %d on interface %s",
                  portFlavor, authPortNum, ifName);
    wioTraceWrite(wioTrace);
  }

  msg.wioMsgData.authPortMsg.tcpPortNum = authPortNum;
  msg.wioMsgData.authPortMsg.intIfNum = intIfNum;
  msg.wioMsgData.authPortMsg.portFlavor = portFlavor;
  msg.msgType = WIO_AUTH_PORT;
  if (osapiMessageSend(wioInfo->wioMsgQ, &msg, sizeof(wioEventMsg_t),
                       L7_NO_WAIT, L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
  {
    osapiSemaGive(wioInfo->msgQSema);
  }
  else
  {
    L7_LOG(L7_LOG_SEVERITY_ALERT, L7_CP_WIO_COMPONENT_ID, 
           "Failure setting user configured authentication port number.");
    rc = L7_FAILURE;
  }

  osapiSemaGive(wioInfo->wioLock);
  return rc;
}

/*********************************************************************
* @purpose  Set the authentication port 
*
* @param    intIfNum      Internal Interface Number 
* @param    authPortNum   Authentication Port Number to be set      
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t wioCpSetAuthPortNum(L7_uint32 intIfNum, L7_ushort16 authPortNum)
{
  return wioCpAuthPortNumSet(intIfNum, authPortNum, WIO_PORT_FLAVOR_HTTP);
}

/*********************************************************************
* @purpose  Set the first user-configured secure authentication port 
*
* @param    intIfNum      Internal Interface Number 
* @param    authPortNum   Authentication Port Number to be set      
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    It turns out this callback really doesn't do anything. CP does
*           call this with authPortNum set to 443. But CPWIO already listens
*           to 443. So it's nor really necessary for CPWIO to record 443
*           as an additional secure port. I believe the original idea was for
*           CP to send the port number configured with "ip http secure-port,"
*           but ultimately, it was decided that captive portal would always
*           listen on 443 and not react to the user configured secure port.
*           It's not harmful to accept 443 on this callback. So I'm leaving 
*           this CPWIO code in place in case someone changes his mind and decides
*           to send the "ip http secure-port" via this callback.      
*
* @end
*********************************************************************/
L7_RC_t wioCpAuthSecurePort1Num(L7_uint32 intIfNum, L7_ushort16 authPortNum)
{
  return wioCpAuthPortNumSet(intIfNum, authPortNum, WIO_PORT_FLAVOR_HTTPS_1);
}
  
/*********************************************************************
* @purpose  Set the second user-configured secure authentication port 
*
* @param    intIfNum      Internal Interface Number 
* @param    authPortNum   Authentication Port Number to be set      
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    A non-zero port number indicates that CPWIO should intercept
*           packets to this port. If authPortNum is 0, CPWIO removes the
*           port number previously learned through this callback. If this
*           callback is invoked twice with non-zero authPortNum, CPWIO
*           overwrites the first port number with the second one.    
*
* @end
*********************************************************************/
L7_RC_t wioCpAuthSecurePort2Num(L7_uint32 intIfNum, L7_ushort16 authPortNum)
{
  return wioCpAuthPortNumSet(intIfNum, authPortNum, WIO_PORT_FLAVOR_HTTPS_2);
}


