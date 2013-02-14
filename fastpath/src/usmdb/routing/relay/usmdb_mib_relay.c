/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename    usmdb_mib_relay..c
*
* @purpose      Imlementation of USMDB layer for Dhcp Relay
*
* @component	USMDB
*
* @comments
*
* @create      11/27/2001
*
* @author
* @end
*
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/

#include "l7_common.h"
#include "usmdb_mib_relay_api.h"

#include "l7_relay_api.h"

/*********************************************************************
* @purpose  Gets the Maximum Hop count configured for DHCP relay agent
*
*

* @param    UnitIndex  @b{(input)}  Unit
* @param    maxHopCount        @b{(output)} Max Hop count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t usmDbBootpDhcpRelayMaxHopCountGet(L7_uint32 UnitIndex, 
                                          L7_uint32* maxHopCount)
{
    return bootpDhcpRelayMaxHopCountGet(maxHopCount);
}

/*********************************************************************
* @purpose  Gets the Minimum Wait time configured for DHCP relay agent
*
*
* @param    UnitIndex  @b{(input)}  Unit
* @param    minWaitTime  	   @b{(output)} Minimum Wait Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbBootpDhcpRelayMinWaitTimeGet(L7_uint32 UnitIndex, 
                                          L7_uint32* minWaitTime)
{
    return bootpDhcpRelayMinWaitTimeGet(minWaitTime);
}

/*********************************************************************
* @purpose  Find out if IP helper is enabled or disabled
*
* @param    adminMode  	   @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpHelperAdminModeGet(L7_uint32 *adminMode)
{
  return ihAdminModeGet(adminMode);
}

/*********************************************************************
* @purpose  Enable or disable IP Helper
*
* @param    adminMode  	   @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpHelperAdminModeSet(L7_uint32 adminMode)
{
  return ihAdminModeSet(adminMode);
}

/*********************************************************************
* @purpose  Gets the Relay Agent Circuit ID option mode
*
*
*
* @param    UnitIndex  @b{(input)}  Unit
* @param    circuitIDOptionMode  	   @b{(output)} Circuit ID Optin Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t usmDbBootpDhcpRelayCircuitIdOptionModeGet(L7_uint32 UnitIndex, 
                                                  L7_uint32* circuitIDOptionMode)
{
    return bootpDhcpRelayCircuitIdOptionModeGet(circuitIDOptionMode);
}

/*********************************************************************
* @purpose  Gets the Relay Agent rtr interface Circuit ID option mode
*
*
*
* @param    UnitIndex  @b{(input)}  Unit
* @param    circuitIDOptionMode @b{(output)} rtr Circuit ID Optin Mode
* @param    intIfNum   @b{(input)}  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t usmDbBootpDhcpRelayrtrCIDOptionModeGet(L7_uint32 intIfNum,
                                               dhcpRelayIntfValue_t *circuitIDintfOptionMode)
{
    return bootpDhcpRelayrtrCIDOptionModeGet (circuitIDintfOptionMode, intIfNum);
}

/*********************************************************************
* @purpose  Gets the Relay Agent global option check mode
*
* @param    UnitIndex          @b{(input)}  Unit
* @param    cidOptionCheckMode @b{(output)} Circuit ID Option check Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbBootpDhcpRelayCIDOptionCheckModeGet(L7_uint32 *cidOptionCheckMode)
{
  return bootpDhcpRelayCIDOptionCheckModeGet (cidOptionCheckMode);
}

/*********************************************************************
* @purpose  Gets the Relay Agent rtr interface cid  option check mode
*
* @param    UnitIndex          @b{(input)}  Unit
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
L7_RC_t usmDbBootpDhcpRelayrtrCIDOptionCheckModeGet(L7_uint32 intIfNum,
                                                    dhcpRelayIntfValue_t *cidOptionCheckMode)
{
  return bootpDhcpRelayrtrCIDOptionCheckModeGet (cidOptionCheckMode, intIfNum);
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
L7_RC_t usmDbIpHelperStatisticsGet(ipHelperStats_t *ihStats)
{
  return ihStatisticsGet(ihStats);
}

/*********************************************************************
* @purpose  Sets the Max. Hop count
*
*
*
* @param    UnitIndex  @b{(input)}  Unit
* @param    maxHopCount  	   @b{(input)} Maximum Hop count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t usmDbBootpDhcpRelayMaxHopCountSet ( L7_uint32 UnitIndex, 
                                            L7_uint32 maxHopCount )
{
    return bootpDhcpRelayMaxHopCountSet(maxHopCount);
}

/*********************************************************************
* @purpose  Sets the Minimum Wait time
*
*
*
* @param    UnitIndex  @b{(input)}  Unit
* @param    minWaitTime  	   @b{(input)} Minimum Wait time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t usmDbBootpDhcpRelayMinWaitTimeSet ( L7_uint32 UnitIndex, 
                                            L7_uint32 minWaitTime )
{
    return bootpDhcpRelayMinWaitTimeSet(minWaitTime);
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
*           L7_REQUEST_DENIED     if server address for an interface relay entry
*                                 is in a subnet attached to the interface
*           L7_FAILURE            other errors or failures
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbIpHelperAddressAdd(L7_uint32 intIfNum,
                                L7_ushort16 udpPort,
                                L7_IP_ADDR_t serverAddr)
{
  return ihHelperAddressAdd(intIfNum, udpPort, serverAddr);
}

/*********************************************************************
* @purpose  Removes a Helper IP address configured on an interface
*
* @param    intIfNum        internal interface number of ingress interface
* @param    udpPort         Destination UDP port
* @param    serverAddr      Helper IP Address of the interface
*
* @returns  L7_SUCCESS      ip address could be successfully removed
* @returns  L7_ERROR        invalid input parameters
* @returns  L7_FAILURE      other failures
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpHelperAddressDelete(L7_uint32 intIfNum,
                                   L7_ushort16 udpPort,
                                   L7_IP_ADDR_t serverAddr)
{
  return ihHelperAddressDelete(intIfNum, udpPort, serverAddr);
}

/*********************************************************************
* @purpose  Add an IP helper discard entry
*
* @param    intIfNum        internal interface number of ingress interface
* @param    udpPort         Destination UDP port
*
* @returns  L7_SUCCESS      
*           L7_ERROR        
*           L7_FAILURE            other failures
*           L7_TABLE_IS_FULL      exceeded maximum number of addresses allowable
*           L7_ALREADY_CONFIGURED if entry is a duplicate
*           
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpHelperDiscardEntryAdd(L7_uint32 intIfNum, L7_ushort16 udpPort)
{
  return ihDiscardEntryAdd(intIfNum, udpPort);
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
L7_RC_t usmDbIpHelperDiscardEntryDelete(L7_uint32 intIfNum, L7_ushort16 udpPort)
{
  return ihDiscardEntryDelete(intIfNum, udpPort);
}

/*********************************************************************
* @purpose  Removes all IP helper addresses configured on an interface
*
* @param    intIfNum        
*
* @returns  L7_SUCCESS      
* @returns  L7_FAILURE      
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpHelperGlobalAddressesClear(void)
{
  return ihGlobalHelperAddressesClear();
}

/*********************************************************************
* @purpose  Removes all IP helper addresses configured on an interface
*
* @param    intIfNum        
*
* @returns  L7_SUCCESS      
* @returns  L7_FAILURE      
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpHelperAddressesRemove(L7_uint32 intIfNum)
{
  return ihHelperAddressesRemove(intIfNum);
}

/*********************************************************************
* @purpose  Get the first relay entry
*
* @param    intIfNum     @b{(output)} internal interface number
* @param    udpPort      @b{(output)} UDP port number
* @param    serverAddr   @b{(input)}  IPv4 address of server
* @param    discard      @b{(input)}  L7_TRUE if a discard entry
* @param    hitCount     @b{(output)} Number of times this entry has 
*                                     been used to relay a packet
*
* @returns  L7_SUCCESS if a relay entry is found
* @returns  L7_FAILURE if no relay entries are configured
*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t usmDbIpHelperAddressFirst(L7_uint32 *intIfNum, L7_ushort16 *udpPort,
                                  L7_uint32 *serverAddr, L7_BOOL *discard, 
                                  L7_uint32 *hitCount)
{
  return ihHelperAddressFirst(intIfNum, udpPort, serverAddr, discard, hitCount);
}

/*********************************************************************
* @purpose  Get the next relay entry
*
* @param    intIfNum     @b{(output)} internal interface number
* @param    udpPort      @b{(output)} UDP port number
* @param    serverAddr   @b{(input)}  IPv4 address of server
* @param    discard      @b{(input)}  L7_TRUE if a discard entry
* @param    hitCount     @b{(output)} Number of times this entry has 
*                                     been used to relay a packet
*
* @returns  L7_SUCCESS if a relay entry is found
* @returns  L7_FAILURE if no more relay entries are configured
*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t usmDbIpHelperAddressNext(L7_uint32 *intIfNum, L7_ushort16 *udpPort,
                                 L7_uint32 *serverAddr, L7_BOOL *discard, 
                                 L7_uint32 *hitCount)
{
  return ihHelperAddressNext(intIfNum, udpPort, serverAddr, discard, hitCount);
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
L7_RC_t usmDbIpHelperAddressGet(L7_uint32 intIfNum, L7_ushort16 udpPort,
                                L7_uint32 serverAddr, L7_BOOL discard, 
                                L7_uint32 *hitCount)
{
  return ihHelperAddressGet(intIfNum, udpPort, serverAddr, discard, hitCount);
}

/*********************************************************************
* @purpose  Sets the Circuit ID Option mode
*
* @param    UnitIndex  @b{(input)}  Unit
* @param    circuitIDOptionMode  	   @b{(input)} Circit ID Option Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbBootpDhcpRelayCircuitIdOptionModeSet ( L7_uint32 UnitIndex, 
                                                    L7_uint32 circuitIDOptionMode )
{
    return bootpDhcpRelayCircuitIdOptionModeSet(circuitIDOptionMode);
}

/****************************************************************************
* @purpose  Sets the Circuit ID Option mode for rtr interface
*
* @param    UnitIndex  @b{(input)}  Unit
* @param    circuitIDIntfOptionMode  @b{(input)} Circit ID rtr intf Option Mode
* @param    intIfNum   @b{(input)}  internal interface number 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*****************************************************************************/
L7_RC_t usmDbBootpDhcpRelayrtrCIDOptionModeSet (L7_uint32 intIfNum,
                                                dhcpRelayIntfValue_t circuitIDintfOptionMode)
{
    return bootpDhcpRelayrtrCIDOptionModeSet (circuitIDintfOptionMode, intIfNum);
}

/*********************************************************************
* @purpose  Sets the Circuit ID Option check mode 
*
* @param    UnitIndex  @b{(input)}  Unit
* @param    cidOptionCheckMode @b{(input)} Circit ID Option check Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbBootpDhcpRelayCIDOptionCheckModeSet(L7_uint32 cidOptionCheckMode)
{
     return bootpDhcpRelayCIDOptionCheckModeSet (cidOptionCheckMode);
}

/*********************************************************************
* @purpose  Sets the rtr interface Circuit ID Option check mode
*
* @param    UnitIndex  @b{(input)}  Unit
* @param    cidOptionCheckMode  @b{(input)} Circit ID Option check
* @param    intIfNum   @b{(input)}  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbBootpDhcpRelayrtrCIDOptionCheckModeSet(L7_uint32 intIfNum,
                                                    dhcpRelayIntfValue_t cidOptionCheckMode)
{
   return bootpDhcpRelayrtrCIDOptionCheckModeSet (cidOptionCheckMode ,intIfNum);
}

/*********************************************************************
* @purpose  Clear IP helper statistics
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpHelperStatisticsClear(void)
{
  return ihStatisticsClear();
}
