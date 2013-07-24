/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename  dhcp_relay_api.c
*
* @purpose   dhcp relay API   routines
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
#include "osapi_support.h"

extern dhcpRelayCfg_t *pDhcpRelayCfgData;
extern dhcpRelayInfo_t *pDhcpRelayInfo;
extern dhcpRelayInfo_t *ihInfo;  


/*********************************************************************
* @purpose  Find out if IP helper is enabled or not
*
* @param    forwardMode  @b{(output)} Forward Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ihAdminModeGet(L7_uint32 *adminMode)
{
  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  *adminMode = pDhcpRelayCfgData->adminMode;
  osapiSemaGive(ihInfo->ihLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Global enable/disable of relay agent.
*
* @param    adminMode  @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    When relay agent is enabled, relay agent registers 
*           intercept function with sysnet. 
*
* @end
*********************************************************************/
L7_RC_t ihAdminModeSet(L7_uint32 adminMode)
{
  if ((adminMode != L7_ENABLE) && (adminMode != L7_DISABLE))
    return L7_FAILURE;

  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  if (adminMode == pDhcpRelayCfgData->adminMode)
  {
    osapiSemaGive(ihInfo->ihLock);
    return L7_SUCCESS;
  }

  pDhcpRelayCfgData->adminMode = adminMode;
  ihAdminModeApply(adminMode);    /* ignore return code of apply */
  pDhcpRelayInfo->dataChanged = L7_TRUE;
  osapiSemaGive(ihInfo->ihLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Maximum Hop count configured for DHCP relay 
*
* @param    maxHopCount  @b{(output)} Max Hop count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t bootpDhcpRelayMaxHopCountGet(L7_uint32 *maxHopCount)
{
  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  *maxHopCount = pDhcpRelayCfgData->maxHopCount;
  osapiSemaGive(ihInfo->ihLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the Max Hop count for DHCP relay
*
* @param    maxHopCount  @b{(input)} Maximum Hop count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
********************************************************************/
L7_RC_t bootpDhcpRelayMaxHopCountSet(L7_uint32 maxHopCount)
{
  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  pDhcpRelayCfgData->maxHopCount = maxHopCount;
  pDhcpRelayInfo->dataChanged = L7_TRUE;
  osapiSemaGive(ihInfo->ihLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Minimum Wait time configured for DHCP relay agent
*
* @param    minWaitTime  @b{(output)} Minimum Wait Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t bootpDhcpRelayMinWaitTimeGet(L7_uint32 *minWaitTime)
{
  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  *minWaitTime = pDhcpRelayCfgData->minWaitTime;
  osapiSemaGive(ihInfo->ihLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Minimum Wait time
*
* @param    minWaitTime  @b{(input)} Minimum Wait time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t bootpDhcpRelayMinWaitTimeSet(L7_uint32 minWaitTime)
{
  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  pDhcpRelayCfgData->minWaitTime = minWaitTime;
  pDhcpRelayInfo->dataChanged = L7_TRUE;
  osapiSemaGive(ihInfo->ihLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Relay Agent Circuit ID option mode
*
*
* @param    circuitIDOptionMode  @b{(output)} Circuit ID Optin Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t bootpDhcpRelayCircuitIdOptionModeGet(L7_uint32 *circuitIDOptionMode)
{
  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  *circuitIDOptionMode = pDhcpRelayCfgData->circuitIDOptionMode;
  osapiSemaGive(ihInfo->ihLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Circuit ID Option mode
*
*
* @param    circuitIDOptionMode  @b{(input)} Circit ID Option Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t bootpDhcpRelayCircuitIdOptionModeSet(L7_uint32 circuitIDOptionMode)
{
  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  pDhcpRelayCfgData->circuitIDOptionMode = circuitIDOptionMode;
  pDhcpRelayInfo->dataChanged = L7_TRUE;
  osapiSemaGive(ihInfo->ihLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Relay Agent Circuit ID Interface option mode
*
*
* @param    circuitIDIntfOptionMode  @b{(output)} Circuit ID router
*           interface Option Mode
* @param    intIfNum  @b{(input)} Internal intf number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t bootpDhcpRelayrtrCIDOptionModeGet(dhcpRelayIntfValue_t *circuitIDIntfOptionMode,L7_uint32 intIfNum)
{
  L7_uint32 rtrIntfNum;
 
  if(ipMapIntIfNumToRtrIntf(intIfNum,&rtrIntfNum)!= L7_SUCCESS)
    return L7_FAILURE;
  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  *circuitIDIntfOptionMode = pDhcpRelayCfgData->relayrtrIntfCfgData[rtrIntfNum].circuitIDIntfOptionMode;
  osapiSemaGive(ihInfo->ihLock);
  return L7_SUCCESS;
}

/*****************************************************************************
* @purpose  Sets the Circuit ID Option mode for the router intf
*
*
* @param    circuitIDIntfOptionMode  @b{(input)} Circit ID rtr intf Option Mode
* @param    intIfNum                 @b{(input)} Internal intf number 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*******************************************************************************/
L7_RC_t bootpDhcpRelayrtrCIDOptionModeSet(dhcpRelayIntfValue_t circuitIDIntfOptionMode,L7_uint32 intIfNum)
{
  L7_uint32 rtrIntfNum;

  if(ipMapIntIfNumToRtrIntf(intIfNum,&rtrIntfNum)!= L7_SUCCESS)
    return L7_FAILURE;
  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  pDhcpRelayCfgData->relayrtrIntfCfgData[rtrIntfNum].circuitIDIntfOptionMode = circuitIDIntfOptionMode;
  pDhcpRelayInfo->dataChanged = L7_TRUE;
  osapiSemaGive(ihInfo->ihLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Relay Agent global option check mode
*
*
* @param    cidOptionCheckMode  @b{(output)} Circuit ID Option check Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t bootpDhcpRelayCIDOptionCheckModeGet(L7_uint32 *cidOptionCheckMode)
{
  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  *cidOptionCheckMode = pDhcpRelayCfgData->cidOptionCheckMode;
  osapiSemaGive(ihInfo->ihLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Circuit ID Option check mode
*
*
* @param    cidOptionCheckMode  @b{(input)} Circit ID Option check 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t bootpDhcpRelayCIDOptionCheckModeSet(L7_uint32 cidOptionCheckMode)
{
  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  pDhcpRelayCfgData->cidOptionCheckMode = cidOptionCheckMode;
  pDhcpRelayInfo->dataChanged = L7_TRUE;
  osapiSemaGive(ihInfo->ihLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Relay Agent rtr interface cid  option check mode
*
*
* @param    cidOptionCheckMode  @b{(output)} Circuit ID Option check Mode
* @param    intIfNum            @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t bootpDhcpRelayrtrCIDOptionCheckModeGet(dhcpRelayIntfValue_t *cidOptionCheckMode,L7_uint32 intIfNum)
{
  L7_uint32 rtrIntfNum;
    
  if(ipMapIntIfNumToRtrIntf(intIfNum,&rtrIntfNum)!= L7_SUCCESS)
    return L7_FAILURE;
  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  *cidOptionCheckMode = pDhcpRelayCfgData->relayrtrIntfCfgData[rtrIntfNum].cidOptionIntfCheckMode;
  osapiSemaGive(ihInfo->ihLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the rtr interface Circuit ID Option check mode
*
*
* @param    cidOptionCheckMode  @b{(input)} Circit ID Option check
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t bootpDhcpRelayrtrCIDOptionCheckModeSet(dhcpRelayIntfValue_t cidOptionCheckMode,L7_uint32 intIfNum)
{
  L7_uint32 rtrIntfNum;

  if(ipMapIntIfNumToRtrIntf(intIfNum,&rtrIntfNum)!= L7_SUCCESS)
    return L7_FAILURE;
  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  pDhcpRelayCfgData->relayrtrIntfCfgData[rtrIntfNum].cidOptionIntfCheckMode = cidOptionCheckMode;
  pDhcpRelayInfo->dataChanged = L7_TRUE;
  osapiSemaGive(ihInfo->ihLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get IP Helper statistics
*
* @param    ihStats @b{(output)} IP helper stats
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ihStatisticsGet(ipHelperStats_t *ihStats)
{
  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  *ihStats = ihInfo->ihStats.publicStats;
  osapiSemaGive(ihInfo->ihLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Adds an IP Helper address to an interface
*
* @param    intIfNum        internal interface number of ingress interface
* @param    udpPort         Destination UDP port
* @param    serverAddr      Helper IP Address of the interface. 
*
* @returns  L7_SUCCESS            
*           L7_ERROR              Invalid server address
*           L7_TABLE_IS_FULL      exceeded maximum number of addresses allowable
*           L7_ALREADY_CONFIGURED if entry is a duplicate
*           L7_NOT_SUPPORTED      if interface is not valid for IP helper
*           L7_ADDR_INUSE         if server address is a local interface address
*           L7_REQUEST_DENIED     if server address for an interface relay entry
*                                 is in a subnet attached to the interface
*           L7_FAILURE            other errors or failures
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ihHelperAddressAdd(L7_uint32 intIfNum,
                           L7_ushort16 udpPort,
                           L7_IP_ADDR_t serverAddr)
{
  L7_RC_t rc;
  L7_uint32 localIntf;

  if ((intIfNum != IH_INTF_ANY) &&
      (dhcpRelayIsValidIntf(intIfNum) != L7_TRUE))
    return L7_NOT_SUPPORTED;

  /* Check if the IP address is valid */
  if (!inetIsValidIpv4HostAddress(serverAddr))
    return L7_ERROR;

  /* Don't allow server address that is local to this router */
  if (ipMapIpAddressToIntf(serverAddr, &localIntf) == L7_SUCCESS)
    return L7_ADDR_INUSE;

  /* Don't allow server address on same subnet as client */
  if ((intIfNum != IH_INTF_ANY) &&
      ipMapIpAddrIsLocal(intIfNum, serverAddr))
    return L7_REQUEST_DENIED;

  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = ihRelayEntryAdd(intIfNum, udpPort, L7_FALSE, serverAddr);
  if (rc == L7_SUCCESS)
    pDhcpRelayInfo->dataChanged = L7_TRUE;

  osapiSemaGive(ihInfo->ihLock);
  return rc;
}

/*********************************************************************
* @purpose  Removes a Helper IP address configured on an interface
*
* @param    intIfNum        internal interface number of ingress interface
* @param    udpPort         Destination UDP port
* @param    serverAddr      Helper IP Address of the interface
*
*
* @returns  L7_SUCCESS      ip address could be successfully removed
* @returns  L7_ERROR        invalid input parameters
* @returns  L7_FAILURE      other failures
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ihHelperAddressDelete(L7_uint32 intIfNum,
                              L7_ushort16 udpPort,
                              L7_IP_ADDR_t serverAddr)
{
  L7_RC_t rc;

  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = ihRelayEntryDelete(intIfNum, udpPort, L7_FALSE, serverAddr);

  if (rc == L7_SUCCESS)
    pDhcpRelayInfo->dataChanged = L7_TRUE;

  osapiSemaGive(ihInfo->ihLock);
  return rc;
}

/*********************************************************************
* @purpose  Add an IP helper discard entry
*
* @param    intIfNum        internal interface number of ingress interface
* @param    udpPort         Destination UDP port
*
* @returns  L7_SUCCESS      
*           L7_ERROR              if entry is global
*           L7_TABLE_IS_FULL      exceeded maximum number of addresses allowable
*           L7_ALREADY_CONFIGURED if entry is a duplicate
*           L7_NOT_SUPPORTED      if interface is not valid for IP helper
*           L7_FAILURE            other failures
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ihDiscardEntryAdd(L7_uint32 intIfNum, L7_ushort16 udpPort)
{
  L7_RC_t rc;

  if (intIfNum == IH_INTF_ANY)
  {
    /* Can't create a global discard entry */
    return L7_ERROR;
  }

  if (dhcpRelayIsValidIntf(intIfNum) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = ihRelayEntryAdd(intIfNum, udpPort, L7_TRUE, 0);
  if (rc == L7_SUCCESS)
    pDhcpRelayInfo->dataChanged = L7_TRUE;

  osapiSemaGive(ihInfo->ihLock);
  return rc;
}

/*********************************************************************
* @purpose  Delete an IP helper discard entry
*
* @param    intIfNum        internal interface number of ingress interface
* @param    udpPort         Destination UDP port
*
* @returns  L7_SUCCESS      
* @returns  L7_FAILURE        
* @returns  L7_NOT_EXIST  if discard entry not found
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ihDiscardEntryDelete(L7_uint32 intIfNum, L7_ushort16 udpPort)
{
  L7_RC_t rc;

  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = ihRelayEntryDelete(intIfNum, udpPort, L7_TRUE, 0);

  if (rc == L7_SUCCESS)
    pDhcpRelayInfo->dataChanged = L7_TRUE;

  osapiSemaGive(ihInfo->ihLock);
  return rc;
}

/*********************************************************************
* @purpose  Delete all global relay entries.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ihGlobalHelperAddressesClear(void)
{
  L7_RC_t rc;

  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = ihGlobalRelayEntriesClear();

  osapiSemaGive(ihInfo->ihLock);
  return rc;
}

/*********************************************************************
* @purpose  Removes all IP helper addresses configured on an interface
*
* @param    intIfNum        the usual
*
* @returns  L7_SUCCESS      
* @returns  L7_FAILURE      
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ihHelperAddressesRemove(L7_uint32 intIfNum)
{
  L7_RC_t rc;

  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = ihRelayEntriesClear(intIfNum);

  osapiSemaGive(ihInfo->ihLock);
  return rc;
}

/*********************************************************************
* @purpose  Get the first relay entry
*
* @param    intIfNum     @b{(output)} internal interface number
* @param    udpPort      @b{(output)} UDP port number
* @param    serverAddr   @b{(input)}  IPv4 address of server
* @param    discard      @b{(input)}  L7_TRUE if a discard entry
* @param    hitCount     @b{(output)}  Number of times this entry has 
*                                     been used to relay a packet
*
* @returns  L7_SUCCESS if a relay entry is found
* @returns  L7_FAILURE if no relay entries are configured
*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t ihHelperAddressFirst(L7_uint32 *intIfNum, L7_ushort16 *udpPort,
                             L7_uint32 *serverAddr, L7_BOOL *discard, 
                             L7_uint32 *hitCount)
{
  L7_RC_t rc;
  
  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* Return interface-specific entries before global entries */
  if (nimFirstValidIntfNumber(intIfNum) != L7_SUCCESS)
  {
    osapiSemaGive(ihInfo->ihLock);
    return L7_FAILURE;
  }

  if (ihIntfEntryGetFirst(*intIfNum, udpPort, serverAddr, discard, hitCount) == L7_SUCCESS)
  {
    osapiSemaGive(ihInfo->ihLock);
    return L7_SUCCESS;
  }

  rc = nimNextValidIntfNumber(*intIfNum, intIfNum);
  while (rc == L7_SUCCESS)
  {
    if (ihIntfEntryGetFirst(*intIfNum, udpPort, serverAddr, discard, hitCount) == L7_SUCCESS)
    {
      osapiSemaGive(ihInfo->ihLock);
      return L7_SUCCESS;
    }
    rc = nimNextValidIntfNumber(*intIfNum, intIfNum);
  }

  /* No interface-specific entries. Any globals? */
  *intIfNum = IH_INTF_ANY;
  if (ihIntfEntryGetFirst(*intIfNum, udpPort, serverAddr, discard, hitCount) == L7_SUCCESS)
  {
    osapiSemaGive(ihInfo->ihLock);
    return L7_SUCCESS;
  }

  osapiSemaGive(ihInfo->ihLock);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get a specific relay entry
*
* @param    intIfNum     @b{(input)} internal interface number
* @param    udpPort      @b{(input)} UDP port number
* @param    serverAddr   @b{(input)} IPv4 address of server
* @param    discard      @b{(input)} L7_TRUE if a discard entry
* @param    hitCount     @b{(output)}  Number of times this entry has 
*                                      been used to relay a packet
*
* @returns  L7_SUCCESS if matching relay entry is found
* @returns  L7_FAILURE if no matching entry is found
*
* @notes    To support SNMP get.
*
* @end
*********************************************************************/
L7_RC_t ihHelperAddressGet(L7_uint32 intIfNum, L7_ushort16 udpPort,
                           L7_uint32 serverAddr, L7_BOOL discard, 
                           L7_uint32 *hitCount)
{
  L7_RC_t rc;

  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (intIfNum)
    rc = ihIntfEntryGet(intIfNum, udpPort, serverAddr, discard, hitCount);
  else
    rc = ihGlobalEntryGet(udpPort, serverAddr, hitCount);

  osapiSemaGive(ihInfo->ihLock);
  return rc;
}

/*********************************************************************
* @purpose  Get the next relay entry
*
* @param    intIfNum     @b{(output)} internal interface number
* @param    udpPort      @b{(output)} UDP port number
* @param    serverAddr   @b{(input)}  IPv4 address of server
* @param    discard      @b{(input)}  L7_TRUE if a discard entry
* @param    hitCount     @b{(output)}  Number of times this entry has 
*                                      been used to relay a packet
*
* @returns  L7_SUCCESS if a relay entry is found
* @returns  L7_FAILURE if no more relay entries are configured
*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t ihHelperAddressNext(L7_uint32 *intIfNum, L7_ushort16 *udpPort,
                            L7_uint32 *serverAddr, L7_BOOL *discard, 
                            L7_uint32 *hitCount)
{
  L7_RC_t rc;
  
  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (ihIntfEntryGetNext(*intIfNum, udpPort, serverAddr, discard, hitCount) == L7_SUCCESS)
  {
    /* Found next entry on same interface */
    osapiSemaGive(ihInfo->ihLock);
    return L7_SUCCESS;
  }

  if (*intIfNum == IH_INTF_ANY)
  {
    /* We just reported the last global entry. We're done. */
    osapiSemaGive(ihInfo->ihLock);
    return L7_FAILURE;
  }

  /* No more entries on this interface. Go to next interface. */
  rc = nimNextValidIntfNumber(*intIfNum, intIfNum);
  while (rc == L7_SUCCESS)
  {
    if (ihIntfEntryGetFirst(*intIfNum, udpPort, serverAddr, discard, hitCount) == L7_SUCCESS)
    {
      osapiSemaGive(ihInfo->ihLock);
      return L7_SUCCESS;
    }
    rc = nimNextValidIntfNumber(*intIfNum, intIfNum);
  }

  /* No more interface entries. Look for global entry. */
  *intIfNum = IH_INTF_ANY;
  if (ihIntfEntryGetFirst(*intIfNum, udpPort, serverAddr, discard, hitCount) == L7_SUCCESS)
  {
    osapiSemaGive(ihInfo->ihLock);
    return L7_SUCCESS;
  }

  osapiSemaGive(ihInfo->ihLock);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Clear IP helper statistics
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
L7_RC_t ihStatisticsClear(void)
{
  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  memset(&ihInfo->ihStats, 0, sizeof(ihStats_t));

  osapiSemaGive(ihInfo->ihLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Inject a DHCP packet into the Relay component
*
* @param    ipData       @b{(input)} IP packet
* @param    ipLen        @b{(input)} IP packet len
* @param    intIfNum     @b{(input)} Ingress routing interface
* @param    rxPort       @b{(input)} Ingress physical port
* @param    vlanId       @b{(input)} VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This gets called from wireless tunnel. Only DHCP packets
*           at this point, but could be used for other UDP relay (with mods
*           to the destination address check). 
*
* @end
*********************************************************************/
L7_RC_t bootpDhcpRelayPacketInject(L7_uchar8 *ipData, L7_uint32 ipLen, 
                                   L7_uint32 intIfNum, L7_uint32 rxPort, 
                                   L7_uint32 vlanId)
{
  L7_ipHeader_t *ipHeader = (L7_ipHeader_t *)ipData;
  L7_RC_t rc;

  if (bootpDhcpRelayIsValidDest(osapiNtohl(ipHeader->iph_dst)) != L7_TRUE)
  {
    return L7_FAILURE;
  }
  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  rc = ihUdpPacketQueue(ipHeader, ipLen, intIfNum, rxPort, vlanId);
  osapiSemaGive(ihInfo->ihLock);
  return rc;
}
