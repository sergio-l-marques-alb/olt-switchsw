/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename  dhcp_relay.c
*
* @purpose   dhcp relay layer routines
*
* @component DHCP_RELAY  Layer
*
* @comments  none
*
* @create    11/27/2001
*
* @author
*
* @end
*
**********************************************************************/

#include "l7_relayinclude.h"
#include "buff_api.h"
#include "sysnet_api.h"

dhcpRelayCfg_t       *pDhcpRelayCfgData;
extern dhcpRelayCfg_t *ihCfg;
void                    *dhcpRelayQueue = L7_NULLPTR;
dhcpRelayInfo_t         *pDhcpRelayInfo;
extern dhcpRelayInfo_t *ihInfo; 

extern L7_BOOL dhcpRelay_is_ready();

/*********************************************************************
* @purpose  Saves DHCP relay config file
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    Can no longer store this as a binary config
*
* @end
*********************************************************************/
L7_RC_t dhcpRelaySave(void)
{
  pDhcpRelayInfo->dataChanged = L7_FALSE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Restores DHCP relay config file to factory defaults
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dhcpRelayRestoreProcess(void)
{
  dhcpRelayBuildDefaultConfigData(L7_DHCP_RELAY_CFG_VER_CURRENT);
  pDhcpRelayInfo->dataChanged = L7_TRUE;
  dhcpRelayApplyConfigData();
  return;
}

/*********************************************************************
* @purpose  Checks if DHCP relay  config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dhcpRelayHasDataChanged(void)
{
  return pDhcpRelayInfo->dataChanged;
}
void dhcpRelayResetDataChanged(void)
{
  pDhcpRelayInfo->dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Builds default DHCP relay  config data
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    Don't zero config structure. That will wipe out the AVL tree.
*
* @end
*********************************************************************/
void dhcpRelayBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 i;

  pDhcpRelayInfo->dataChanged   = L7_FALSE;

  pDhcpRelayCfgData->maxHopCount = FD_RTR_DEFAULT_BOOTP_DHCP_RELAY_MAXHOPCOUNT;
  pDhcpRelayCfgData->adminMode = FD_RTR_DEFAULT_UDP_RELAY_ADMIN_MODE;
  pDhcpRelayCfgData->minWaitTime = FD_RTR_DEFAULT_BOOTP_DHCP_RELAY_MINWAITTIME;
  pDhcpRelayCfgData->circuitIDOptionMode = FD_RTR_DEFAULT_BOOTP_DHCP_RELAY_CIRCUITIDOPTION;
  pDhcpRelayCfgData->cidOptionCheckMode = FD_RTR_DEFAULT_BOOTP_DHCP_RELAY_CIDOPTIONCHECK;

  for(i = 0; i <= L7_RTR_MAX_RTR_INTERFACES; i++)
  {
    pDhcpRelayCfgData->relayrtrIntfCfgData[i].circuitIDIntfOptionMode = FD_RTR_DEFAULT_BOOTP_DHCP_RELAY_CIRCUITIDOPTION;
    pDhcpRelayCfgData->relayrtrIntfCfgData[i].cidOptionIntfCheckMode = FD_RTR_DEFAULT_BOOTP_DHCP_RELAY_CIDOPTIONCHECK;
    
  }


  /* Clear all relay entries */
  ihRelayEntriesClear(IH_INTF_ANY);
}

/*********************************************************************
* @purpose  Initializes the helper addresses list
*
* @param   pCfg  pointer to interface configuration information.
*
* @returns void 
*
* @notes    none
*
* @end
*********************************************************************/
void dhcpRelayIntfBuildDefaultConfigData(L7_uint32 intIfNum)
{
  ihRelayEntriesClear(intIfNum);
}

/*********************************************************************
* @purpose  Enable or disable BOOTP/DHCP Relay
*
* @param    mode     @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  if other failure
*
* @notes   none
*
*
* @end
*********************************************************************/
L7_RC_t ihAdminModeApply(L7_uint32 mode)
{
    sysnetPduIntercept_t sysnetPduIntercept;

    sysnetPduIntercept.addressFamily = L7_AF_INET;
    sysnetPduIntercept.hookId = SYSNET_INET_IN;
    sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_UDP_RELAY_PRECEDENCE;
    sysnetPduIntercept.interceptFunc = ihUdpRelayIntercept;
    strcpy(sysnetPduIntercept.interceptFuncName, "udpRelayIntercept");

    if ((mode == L7_ENABLE) && !ihInfo->regWithSysnet)
    {
      if (sysNetPduInterceptRegister(&sysnetPduIntercept) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_RELAY_COMPONENT_ID,
                "Failed to register packet intercept.");
        return L7_FAILURE;
      }
      else
      {
        ihInfo->regWithSysnet = L7_TRUE;
      }
    }
    else if ((mode == L7_DISABLE) && ihInfo->regWithSysnet)
    {
      if (sysNetPduInterceptDeregister(&sysnetPduIntercept) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_RELAY_COMPONENT_ID,
                "Failed to deregister packet intercept.");
        return L7_FAILURE;
      }
      else 
      {
        ihInfo->regWithSysnet = L7_FALSE;
      }
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Applies relay config data
*
* @param    void
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @notes    This may be invoked from dhcpRelayCnfgrInitPhase3Process() or 
            clear config.
            DHCP relay does not maintain any config information per interface.
*
* @end
*********************************************************************/
L7_RC_t dhcpRelayApplyConfigData(void)
{
  return ihAdminModeApply(pDhcpRelayCfgData->adminMode);
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t dhcpRelayIntfCreate(L7_uint32 intIfNum)
{
  if (dhcpRelayIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;
    
  dhcpRelayIntfBuildDefaultConfigData(intIfNum);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_DELETE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t dhcpRelayIntfDelete(L7_uint32 intIfNum)
{
  return ihRelayEntriesClear(intIfNum);
}

/*********************************************************************
* @purpose  Determine whether a given UDP port is one of the ports that
*           the relay agent forwards when a port is not explicitly specified.
*
* @param    udpPort     a UDP port number
*
* @returns  L7_TRUE if port is among those relayed by default
*
* @comments 
*
* @end
*
*********************************************************************/
L7_BOOL ihDestPortOnDefaultList(L7_uint32 udpPort)
{
  switch (udpPort)
  {
    case UDP_PORT_DHCP_SERV:
    case UDP_PORT_TIME:
    case UDP_PORT_NAMESERVER:
    case UDP_PORT_TACACS:
    case UDP_PORT_DNS:
    case UDP_PORT_TFTP:
    case UDP_PORT_NETBIOS_NS:
    case UDP_PORT_NETBIOS_DGM:
      return L7_TRUE;
    default:
      return L7_FALSE;
  }
}

/*********************************************************************
*
* @purpose  Convert a UDP port number to a string
*
* @param    udpPort     a UDP port number
* @param    portString  buffer to store result. At least IH_UDP_PORT_NAME_LEN. 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Certain well-known ports are given names. Others are written
*           as numbers.
*
* @end
*
*********************************************************************/
L7_RC_t ihUdpPortToString(L7_ushort16 udpPort, L7_uchar8 *portString)
{
  switch (udpPort)
  {
    case IH_UDP_PORT_DEFAULT:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "Default");
      break;
    case UDP_PORT_DHCP_SERV:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "dhcp");
      break;
    case UDP_PORT_TIME:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "time");
      break;
    case UDP_PORT_NAMESERVER:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "nameserver");
      break;
    case UDP_PORT_TACACS:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "tacacs");
      break;
    case UDP_PORT_DNS:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "dns");
      break;
    case UDP_PORT_TFTP:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "tftp");
      break;
    case UDP_PORT_NTP:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "ntp");
      break;
    case UDP_PORT_NETBIOS_NS:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "netbios-ns");
      break;
    case UDP_PORT_NETBIOS_DGM:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "netbios-dgm");
      break;
    case UDP_PORT_MOBILE_IP:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "mobile-ip");
      break;
    case UDP_PORT_PIM_AUTO_RP:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "pim-auto-rp");
      break;
    case UDP_PORT_ISAKMP:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "isakmp");
      break;
    case UDP_PORT_RIP:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%s", "rip");
      break;

    default:
      osapiSnprintf(portString, IH_UDP_PORT_NAME_LEN, "%u", udpPort);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initialize buffer pool for server list entries.
*
* @param    None.
*
* @returns  L7_SUCCESS if ok
*           L7_FAILURE otherwise
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t ihPacketPoolCreate(void)
{
  L7_uint32 buffer_size = IH_PACKET_POOL_BUF_LEN;
  L7_uint32 num_buffers = IH_PACKET_POOL_BUF_NUM;

  if (bufferPoolInit(num_buffers, buffer_size, "IP Helper Pkt", 
                     &pDhcpRelayInfo->packetPoolId) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_EMERGENCY, L7_DHCP_RELAY_COMPONENT_ID,
            "Failed to allocate IP helper packet pool.");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Delete pool of client connection entries.
*
* @param    None.
*
* @returns  L7_SUCCESS if ok
*           L7_FAILURE otherwise
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t ihPacketPoolDelete(void)
{
  return bufferPoolTerminate(pDhcpRelayInfo->packetPoolId);
}

/*********************************************************************
* @purpose  Write a debug trace message.
*
* @param    traceMsg @b{(input)} A string to be displayed as a trace message.
*
* @notes    The input string must be no longer than IH_TRACE_LEN_MAX
*           characters. This function will prepend the message with the
*           current time and will insert a new line character at the
*           beginning of the message.
*
* @end
*********************************************************************/
void ihTraceWrite(L7_uchar8 *traceMsg)
{
    L7_uint32 msecSinceBoot;
    L7_uint32 secSinceBoot;
    L7_uint32 msecs;
    L7_uchar8 debugMsg[1024];

    if (traceMsg == NULL)
        return;

    msecSinceBoot = osapiTimeMillisecondsGet();
    secSinceBoot = msecSinceBoot / 1000;
    msecs = msecSinceBoot % 1000;

    /* For now, just print the message with a timestamp. */
    sprintf(debugMsg, "\n%d.%03d:  ", secSinceBoot, msecs);
    strncat(debugMsg, traceMsg, IH_TRACE_LEN_MAX);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, debugMsg);
}

/* Set debug trace flags */
void ihTraceFlagsSet(L7_uint32 traceFlags)
{
  pDhcpRelayInfo->ihTraceFlags = traceFlags;
}

/* print debug stats */
void ihDebugShow(void)
{
  L7_PORT_EVENTS_t event;

  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    printf("\nFailed to take IP helper semaphore at %#x.", (L7_uint32) ihInfo->ihLock);
    return;
  }
  printf("\nIP helper thread ID:  %#x", ihInfo->dhcpRelayTaskId);
  printf("\nMax number of relay entries:  %u", ihCfg->relayEntryList.maxEntries);
  printf("\nCurrent number of relay entries:  %u", ihRelayEntryCount());
  printf("\nServer addr pool ID:  %u", pDhcpRelayInfo->serverPoolId);
  printf("\nPacket pool ID:  %u", pDhcpRelayInfo->packetPoolId);
  printf("\nIP helper semaphore: %#x", (L7_uint32) ihInfo->ihLock);
  printf("\nSysnet intercept registered?:  %s", ihInfo->regWithSysnet ? "Yes" : "No");
  printf("\nCurrent trace flags:  %#x", ihInfo->ihTraceFlags);

  printf("\nIP Helper registered for the following NIM events:");
  for (event = L7_PORT_DISABLE; event < L7_LAST_PORT_EVENT; event++)
  {
    if (PORTEVENT_ISMASKBITSET(ihInfo->ihNimEventMask, event))
    {
      printf("\n  %s", nimGetIntfEvent(event)); 
    }
  }

  printf("\n\nStatistics:");
  printf("\nNumber of valid DHCP client msgs received.........................%u", 
         ihInfo->ihStats.publicStats.dhcpClientMsgsReceived);
  printf("\nNumber of DHCP client msgs relayed................................%u", 
         ihInfo->ihStats.publicStats.dhcpClientMsgsRelayed);
  printf("\nNumber of valid DHCP server msgs received.........................%u", 
         ihInfo->ihStats.publicStats.dhcpServerMsgsReceived);
  printf("\nNumber of DHCP server msgs relayed................................%u", 
         ihInfo->ihStats.publicStats.dhcpServerMsgsRelayed);
  printf("\nNumber of valid UDP client msgs received..........................%u", 
         ihInfo->ihStats.publicStats.udpClientMsgsReceived);
  printf("\nNumber of UDP client msgs relayed.................................%u", 
         ihInfo->ihStats.publicStats.udpClientMsgsRelayed);
  printf("\nNumber of packets intercepted.....................................%u", 
         ihInfo->ihStats.packetsIntercepted);
  printf("\nNumber of packets queued to relay thread..........................%u", 
         ihInfo->ihStats.packetsQueued);
  printf("\nNumber of packets that matched a discard entry....................%u", 
         ihInfo->ihStats.publicStats.matchDiscardEntry);
  printf("\nClient message matches discard entry or no server configured......%u", 
         ihInfo->ihStats.noServer);
  printf("\nNumber of BOOTP messages relayed to clients.......................%u", 
         ihInfo->ihStats.bootpMsgToClient);

  printf("\n\nError counters:");
  printf("\nNo packet buffer available.........................................%u", 
         ihInfo->ihStats.noPacketBuffer);
  printf("\nFailed to enqueue a packet to our threads msg queue................%u", 
         ihInfo->ihStats.pktEnqueueFailed);
  printf("\nReceived a DHCP server packet whose giaddr is not local............%u", 
         ihInfo->ihStats.unknownGiaddr);
  printf("\nDHCP client message arrived with hops greater than max.............%u", 
         ihInfo->ihStats.publicStats.tooManyHops);
  printf("\nDHCP client message arrived with secs field lower than min.........%u", 
         ihInfo->ihStats.publicStats.tooEarly);
  printf("\nReceived DHCP client message on interface with no IP address.......%u", 
         ihInfo->ihStats.noLocalIpAddress);
  printf("\nReceived DHCP client message with giaddr set to our own address....%u", 
         ihInfo->ihStats.publicStats.spoofedGiaddr);
  printf("\nReceived DHCP server message whose dest IP address is not ours.....%u", 
         ihInfo->ihStats.dhcpServerMsgToUnknownAddr);
  printf("\nSocket create failed...............................................%u", 
         ihInfo->ihStats.noSocket);
  printf("\nSocket bind failed.................................................%u", 
         ihInfo->ihStats.socketBindFailed);
  printf("\nSocket send failed.................................................%u", 
         ihInfo->ihStats.socketSendFailed);
  printf("\nPackets intercepted with expired TTL...............................%u", 
         ihInfo->ihStats.publicStats.ttlExpired);
  printf("\nPackets intercepted with other destination IP address..............%u", 
         ihInfo->ihStats.notToMe);
  printf("\nMessage too big to be relayed......................................%u", 
         ihInfo->ihStats.msgTooBig);
  printf("\nFailed to allocate mbuf for UDP message............................%u", 
         ihInfo->ihStats.mbufFailures);
  printf("\nNo IPv4 address on outgoing interface to DHCP client...............%u", 
         ihInfo->ihStats.noIpAddrOnOutIf);

  osapiSemaGive(ihInfo->ihLock);
}

/* See if there is any inconsistency in the configuration or state of IP helper. */
void ihValidate(void)
{
  L7_BOOL valid = L7_TRUE;

  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    printf("\nFailed to take IP helper semaphore.");
    return;
  }

  if ((ihCfg->adminMode != L7_ENABLE) && (ihCfg->adminMode != L7_DISABLE))
  {
    printf("\nAdmin mode is invalid (%u).", ihCfg->adminMode);
    valid = L7_FALSE;
  }

  if ((ihCfg->circuitIDOptionMode != L7_ENABLE) && 
      (ihCfg->circuitIDOptionMode != L7_DISABLE))
  {
    printf("\nOption 82 setting is invalid (%u).", ihCfg->circuitIDOptionMode);
    valid = L7_FALSE;
  }

  if ((ihCfg->maxHopCount < L7_DHCP_MIN_HOP_COUNT) || 
      (ihCfg->maxHopCount > L7_DHCP_MAX_HOP_COUNT))
  {
    printf("\nMax hop count %u is out of range [%u, %u].", 
           ihCfg->maxHopCount,
           L7_DHCP_MIN_HOP_COUNT, L7_DHCP_MAX_HOP_COUNT);
    valid = L7_FALSE;
  }

  if ((ihCfg->minWaitTime < L7_DHCP_MIN_WAIT_TIME) || 
      (ihCfg->minWaitTime > L7_DHCP_MAX_WAIT_TIME))
  {
    printf("\nMinimum wait time %u is out of range [%u, %u].", 
           ihCfg->minWaitTime,
           L7_DHCP_MIN_WAIT_TIME, L7_DHCP_MIN_WAIT_TIME);
    valid = L7_FALSE;
  }

  /* Validate all the relay entries */
  if (ihRelayEntriesValid() != L7_TRUE)
  {
    valid = L7_FALSE;
  }

  osapiSemaGive(ihInfo->ihLock);

  if (valid)
    printf("\nIP Helper configuration and state are valid");
  else
    printf("\nIP Helper configuration or state invalid");
}



/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/



/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    void
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void dhcpRelayBuildTestConfigData(void)
{

  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/
  
   pDhcpRelayCfgData->maxHopCount = 6;
   pDhcpRelayCfgData->adminMode = L7_ENABLE;
   pDhcpRelayCfgData->minWaitTime = 800;
   pDhcpRelayCfgData->circuitIDOptionMode = L7_ENABLE;


 /* End of Component's Test Non-default configuration Data */


   /* Force write of config file */
   pDhcpRelayInfo->dataChanged = L7_TRUE;  
   sysapiPrintf("Built test config data\n");


}

/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void dhcpRelayConfigDataTestShow(void)
{
    /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/
   sysapiPrintf( "\n");
   sysapiPrintf( "pDhcpRelayCfgData->maxHopCount = %d \n", pDhcpRelayCfgData->maxHopCount);
   sysapiPrintf( "pDhcpRelayCfgData->forwardMode = %d \n",pDhcpRelayCfgData->adminMode);
   sysapiPrintf( "pDhcpRelayCfgData->minWaitTime = %d \n",pDhcpRelayCfgData->minWaitTime);
   sysapiPrintf( "pDhcpRelayCfgData->circuitIDOptionMode = %d \n",pDhcpRelayCfgData->circuitIDOptionMode);
   sysapiPrintf( "pDhcpRelayCfgData->cidOptionCheckMode = %d \n",pDhcpRelayCfgData->cidOptionCheckMode);

   /*-------------------------------*/
   /* Scaling Constants             */
   /*-------------------------------*/
    sysapiPrintf( "Scaling Constants\n");
    sysapiPrintf( "-----------------\n");
    sysapiPrintf( "None - \n");
}

void dhcpRelayrouterInterfaceConfigDataDump(L7_uint32 intIfNum)
{
  /*______router cfg parameter________*/
  L7_uint32 rtrIfNum; 

  if(ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum)!= L7_SUCCESS)
     return;
  if(rtrIfNum > (L7_RTR_MAX_RTR_INTERFACES))
    return;
  else
  {
   sysapiPrintf( "\n");
   sysapiPrintf( "relayrtrIntfCfgData->circuitIDIntfOptionMode = %d \n",
                                pDhcpRelayCfgData->relayrtrIntfCfgData[rtrIfNum].circuitIDIntfOptionMode);
   sysapiPrintf( "relayrtrIntfCfgData->cidOptionIntfCheckMode = %d \n",                                                                    pDhcpRelayCfgData->relayrtrIntfCfgData[rtrIfNum].cidOptionIntfCheckMode);
  }
  return;
}


/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/




