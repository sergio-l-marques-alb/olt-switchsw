/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
**********************************************************************
* @filename  usmdb_mib_relay_api.h
*
* @purpose   USMDB MIB relay api Include files
*
*
* @create    11/27/2001
*
* @author
*
* @end
*
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/


#ifndef _USMDB_MIB_RELAY_API_H

#define _USMDB_MIB_RELAY_API_H
#include "l7_common.h"
#include "l3_comm_structs.h"

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
                                          L7_uint32* maxHopCount);

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
                                          L7_uint32* minWaitTime);

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
L7_RC_t usmDbIpHelperAdminModeGet(L7_uint32 *adminMode);

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
L7_RC_t usmDbIpHelperAdminModeSet(L7_uint32 adminMode);

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
                                                  L7_uint32* circuitIDOptionMode);

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
L7_RC_t usmDbIpHelperStatisticsGet(ipHelperStats_t *ihStats);

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
                                            L7_uint32 maxHopCount );

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
                                            L7_uint32 minWaitTime );

/*********************************************************************
* @purpose  Adds an IP Helper address to an interface
*
* @param    intIfNum        internal interface number of ingress interface
* @param    udpPort         Destination UDP port
* @param    serverAddr      Helper IP Address of the interface. Unused for 
*                           discard entries.
*
* @returns  L7_SUCCESS            
*           L7_ERROR              Invalid server address
*           L7_TABLE_IS_FULL      exceeded maximum number of addresses allowable
*           L7_ALREADY_CONFIGURED if entry is a duplicate
*           L7_FAILURE            other errors or failures
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbIpHelperAddressAdd(L7_uint32 intIfNum,
                                L7_ushort16 udpPort,
                                L7_IP_ADDR_t serverAddr);

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
                                   L7_IP_ADDR_t serverAddr);

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
L7_RC_t usmDbIpHelperDiscardEntryAdd(L7_uint32 intIfNum, L7_ushort16 udpPort);

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
L7_RC_t usmDbIpHelperDiscardEntryDelete(L7_uint32 intIfNum, L7_ushort16 udpPort);

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
L7_RC_t usmDbIpHelperGlobalAddressesClear(void);

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
L7_RC_t usmDbIpHelperAddressesRemove(L7_uint32 intIfNum);

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
L7_RC_t usmDbIpHelperAddressNext(L7_uint32 *intIfNum, L7_ushort16 *udpPort,
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
L7_RC_t usmDbIpHelperAddressGet(L7_uint32 intIfNum, L7_ushort16 udpPort,
                                L7_uint32 serverAddr, L7_BOOL discard, 
                                L7_uint32 *hitCount);

/*********************************************************************
* @purpose  Sets the Circuit ID Option mode
*
*
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
L7_RC_t usmDbBootpDhcpRelayCircuitIdOptionModeSet(L7_uint32 UnitIndex, 
                                                  L7_uint32 circuitIDOptionMode);

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
L7_RC_t usmDbIpHelperStatisticsClear(void);

#endif
