/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename    l7_relay_api.h
*
* @purpose     File contains all the APIs of Relay
*
* @component
*
* @comments
*
* @create      11/27/2001
*
* @author
* @end
*
**********************************************************************/

#ifndef _L7_DHCP_RELAY_API__H_
#define _L7_DHCP_RELAY_API__H_

#include "l3_comm_structs.h"

/*********************************************************************
* @purpose  Gets the Maximum Hop count configured for DHCP relay agent
*
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

L7_RC_t bootpDhcpRelayMaxHopCountGet(L7_uint32* maxHopCount);

/*********************************************************************
* @purpose  Gets the Minimum Wait time configured for DHCP relay agent
*
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

L7_RC_t bootpDhcpRelayMinWaitTimeGet(L7_uint32* minWaitTime);

/*********************************************************************
* @purpose  Gets the Forward Mode of DHCP relay agent
*
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
L7_RC_t ihAdminModeGet(L7_uint32* forwardMode);

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
L7_RC_t bootpDhcpRelayCircuitIdOptionModeGet(L7_uint32* circuitIDOptionMode);

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
L7_RC_t ihStatisticsGet(ipHelperStats_t *ihStats);

/*********************************************************************
* @purpose  Sets the Max. Hop count
*
*
* @param    maxHopCount  @b{(input)} Maximum Hop count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t bootpDhcpRelayMaxHopCountSet(L7_uint32 maxHopCount);

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
L7_RC_t bootpDhcpRelayMinWaitTimeSet(L7_uint32 minWaitTime);

/*********************************************************************
* @purpose  Sets the Forward mode of Relay
*
*
* @param    forwardMode  @b{(input)} Forward Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ihAdminModeSet(L7_uint32 adminMode);


/*********************************************************************
* @purpose  Adds an IP Helper address to an interface
*
* @param    intIfNum        Routing Internal interface number
* @param    udpPort         Destination UDP port
* @param    serverAddr      Helper IP Address of the interface. Unused for 
*                           discard entries.
*
* @returns  L7_SUCCESS            ip address could be successfully configured
*           L7_TABLE_IS_FULL      exceeded maximum number of addresses allowable
*           L7_ALREADY_CONFIGURED if entry is a duplicate
*           L7_FAILURE            other errors or failures
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ihHelperAddressAdd(L7_uint32 intIfNum,
                           L7_ushort16 udpPort,
                           L7_IP_ADDR_t serverAddr);

/*********************************************************************
* @purpose  Removes a Helper IP address configured on an interface
*
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
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
                              L7_IP_ADDR_t serverAddr);

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
L7_RC_t ihGlobalHelperAddressesClear(void);

/*********************************************************************
* @purpose  Removes all Helper IP addresses configured on an interface
*
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
*
* @returns  L7_SUCCESS      ip address could be successfully removed
* @returns  L7_FAILURE      The interface is not a valid IP interface
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ihHelperAddressesRemove(L7_uint32 intIfNum);

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
L7_RC_t ihHelperAddressFirst(L7_uint32 *intIfNum, L7_ushort16 *udpPort,
                             L7_uint32 *serverAddr, L7_BOOL *discard, 
                            L7_uint32 *hitCount);

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
                           L7_uint32 *hitCount);

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
L7_RC_t ihHelperAddressNext(L7_uint32 *intIfNum, L7_ushort16 *udpPort,
                            L7_uint32 *serverAddr, L7_BOOL *discard, 
                            L7_uint32 *hitCount);

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
*           L7_FAILURE            other failures
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ihDiscardEntryAdd(L7_uint32 intIfNum, L7_ushort16 udpPort);

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
L7_RC_t ihDiscardEntryDelete(L7_uint32 intIfNum, L7_ushort16 udpPort);

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
L7_RC_t bootpDhcpRelayCircuitIdOptionModeSet(L7_uint32 circuitIDOptionMode);

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
L7_RC_t ihStatisticsClear(void);

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
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t bootpDhcpRelayPacketInject(L7_uchar8 *ipData, L7_uint32 ipLen, 
                                   L7_uint32 intIfNum, L7_uint32 rxPort,
                                   L7_uint32 vlanId);

#endif
