/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     wdm_trap_api.h
*
* @purpose      Wireless Data Manager (WDM) trap API header
*
* @component    WDM
*
* @comments     none
*
* @create       10/20/2005
*
* @author       dfowler
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_WDM_TRAP_API_H
#define INCLUDE_WDM_TRAP_API_H

#include "datatypes.h"
#include "comm_structs.h"


/*********************************************************************
*
* @purpose  Send trap for channel algorithm complete.
*
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapChannelAlgorithmComplete(L7_WDM_CHANNEL_PLAN_TYPE_t chType);

/*********************************************************************
*
* @purpose  Send trap for power algorithm complete.
*
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapPowerAlgorithmComplete();


/*********************************************************************
*
* @purpose  Send trap for newly discovered peer switch.
*
* @param    L7_IP_ADDR_t ip @b{(input)} Switch IP address
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapPeerSwitchDiscovered(L7_IP_ADDR_t ip);

/*********************************************************************
*
* @purpose  Send trap for peer switch connection failure.
*
* @param    L7_IP_ADDR_t ip @b{(input)} Switch IP address
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapPeerSwitchFailed(L7_IP_ADDR_t ip);


/*********************************************************************
*
* @purpose  Send trap for peer switch comamnd config receive.
*
* @param    L7_IP_ADDR_t ip @b{(input)} Switch IP address
* @param    L7_uint32    mask @b{(input)} Configuration mask received
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapPeerSwitchConfigurationCommandRx(L7_IP_ADDR_t ip, L7_uint32  mask);


/*********************************************************************
*
* @purpose  Send trap for peer switch discovered with unknown protocol version.
*
* @param    L7_IP_ADDR_t ip              @b{(input)}  Switch IP address
* @param    L7_ushort16  protocolVersion @b{(input))  protocol
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapPeerSwitchUnknownProtocol(L7_IP_ADDR_t ip,
                                      L7_ushort16  protocolVersion);

/*********************************************************************
*
* @purpose  Send trap for discovered and managed AP state changes.
*
* @param    L7_enetMacAddr_t           macAddr @b{(input)} AP MAC address
* @param    L7_WDM_MANAGED_AP_STATUS_t status  @b{(input)} new AP status
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapAPStateChange(L7_enetMacAddr_t           macAddr, 
                          L7_WDM_MANAGED_AP_STATUS_t status);

/*********************************************************************
*
* @purpose  Send trap for AP discovered with unknown protocol version.
*
* @param    L7_enetMacAddr_t macAddr           @b{(input)} AP MAC address
* @param    L7_ushort16      protocolVersion   @b{(input)} protocol
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapAPUnknownProtocol(L7_enetMacAddr_t macAddr,
                              L7_ushort16      protocolVersion);

/*********************************************************************
*
* @purpose  Send trap for AP association and authentication failures.
*
* @param    L7_enetMacAddr_t    macAddr @b{(input)} AP MAC address
* @param    L7_WDM_AP_FAILURE_t type    @b{(input)} failure type
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapAPFailure(L7_enetMacAddr_t    macAddr, 
                      L7_WDM_AP_FAILURE_t type);

/*********************************************************************
*
* @purpose  Load balancing invokes this function to send traps for 
*           exceeding configured maximum client associations.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)} AP MAC address
* @param    L7_enetMacAddr_t radioMacAddr @b{(input)} Radio interface MAC address 
* @param    L7_uchar8        radioIf      @b{(input)} Radio interface number 
* @param    L7_uchar8        numAssoc     @b{(input)} Current client association  
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapAPUsersMaxOverflow(L7_enetMacAddr_t  macAddr,
                               L7_enetMacAddr_t  radioMacAddr,
                               L7_uchar8         radioIf, 
                               L7_uchar8         numAssoc);

/*********************************************************************
*
* @purpose  Load balancing invokes this function to send traps for 
*           exceeding configured threshold client associations.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)} AP MAC address
* @param    L7_enetMacAddr_t radioMacAddr @b{(input)} Radio interface MAC address 
* @param    L7_uchar8        radioIf      @b{(input)} Radio interface number 
* @param    L7_uchar8        numAssoc     @b{(input)} Current client association  
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapAPUsersThrhldOverflow(L7_enetMacAddr_t  macAddr,
                                  L7_enetMacAddr_t  radioMacAddr,
                                  L7_uchar8         radioIf, 
                                  L7_uchar8         numAssoc);

/*********************************************************************
*
* @purpose  Load balancing invokes this function to send traps for 
*           exceeding configured wireless bandwidth utilization.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)} AP MAC address
* @param    L7_enetMacAddr_t radioMacAddr @b{(input)} Radio interface MAC address 
* @param    L7_uchar8        radioIf      @b{(input)} Radio interface number 
* @param    L7_uchar8        util         @b{(input)} Wireless bandwith utilization   
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapAPUtilizationOverflow(L7_enetMacAddr_t macAddr,
                                  L7_enetMacAddr_t radioMacAddr,
                                  L7_uchar8        radioIf,
                                  L7_uchar8        util);

/*********************************************************************
*
* @purpose  Send trap for Rogue AP detection.
*
* @param    L7_enetMacAddr_t    macAddr @b{(input)} Managed AP MAC address
* @param    L7_uchar8           radioIf @b{(input)} Radio interface number 
* @param    L7_enetMacAddr_t    macAddr @b{(input)} Rogue AP MAC address
* @param    L7_char8           *ssid    @b{(input)} Network SSID
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapRogueAPDetected(L7_enetMacAddr_t macAddr,
                            L7_uchar8        radioIf,
                            L7_enetMacAddr_t rogueMacAddr,
                            L7_char8        *ssid);

/*********************************************************************
*
* @purpose  Send trap for Rogue(s) Present in the network.
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapRoguesPresent(void);

/*********************************************************************
*
* @purpose  Send trap for Rogue Client(s) Present in the network.
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapRogueClientsPresent(void);

/*********************************************************************
*
* @purpose  Send trap for RF Scan AP detected.
*
* @param    L7_enetMacAddr_t    macAddr @b{(input)} Managed AP MAC address
* @param    L7_uchar8           radioIf @b{(input)} Radio interface number 
* @param    L7_enetMacAddr_t    macAddr @b{(input)} Neighbor AP MAC address
* @param    L7_char8           *ssid    @b{(input)} Network SSID
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapRFScanAPDetected(L7_enetMacAddr_t macAddr,
                             L7_uchar8        radioIf,
                             L7_enetMacAddr_t apMacAddr,
                             L7_char8        *ssid);

/*********************************************************************
*
* @purpose  Send trap for RF Scan Client detected.
*
* @param    L7_enetMacAddr_t    macAddr @b{(input)} Managed AP MAC address
* @param    L7_uchar8           radioIf @b{(input)} Radio interface number 
* @param    L7_enetMacAddr_t    macAddr @b{(input)} Client MAC address
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapRFScanClientDetected(L7_enetMacAddr_t macAddr,
                                 L7_uchar8        radioIf,
                                 L7_enetMacAddr_t clientMacAddr);

/*********************************************************************
*
* @purpose  Send trap for Ad Hoc client detected.
*
* @param    L7_enetMacAddr_t    macAddr @b{(input)} Client MAC address
* @param    L7_enetMacAddr_t    macAddr @b{(input)} AP MAC address
* @param    L7_uchar8           radioIf @b{(input)} Radio interface number 
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapAdHocClientDetected(L7_enetMacAddr_t clientMacAddr,
                                L7_enetMacAddr_t apMacAddr,
                                L7_uchar8        apRadioIf);

/*********************************************************************
*
* @purpose  Send trap for ad hoc list full.
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapAdHocClientListFull();

/*********************************************************************
*
* @purpose  Send trap for associated client state changes.
*
* @param    L7_enetMacAddr_t        macAddr        @b{(input)} client MAC address
* @param    L7_enetMacAddr_t        vapMacAddr     @b{(input)} VAP MAC address
* @param    L7_enetMacAddr_t        apMacAddr      @b{(input)} AP  MAC address
* @param    L7_WDM_CLIENT_STATUS_t  status         @b{(input)} new client status
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapClientStateChange(L7_enetMacAddr_t           macAddr,
                              L7_enetMacAddr_t           vapMacAddr,
                              L7_enetMacAddr_t           apMacAddr,
                              L7_WDM_CLIENT_STATUS_t     status);

/*********************************************************************
*
* @purpose  Send trap for Cluster Controller election.
*
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapWSWidsControllerElected();

/*********************************************************************
*
* @purpose  Send trap for Max AP exceeded in the peer group
*
* @param    L7_IP_ADDR_t widsIPAddr @b{(input)} Cluster Controller IP Address
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_IP_ADDR_t apIPAddr @b{(input)} AP IP Address
* @param    L7_IP_ADDR_t switchIPAddr @b{(input)} Switch IP Address
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapManagedAPNetworkDatabaseFull (L7_IP_ADDR_t widsIPAddr,
                            L7_enetMacAddr_t macAddr, L7_IP_ADDR_t apIPAddr,
                            L7_IP_ADDR_t switchIPAddr);

/*********************************************************************
*
* @purpose  Send trap for Max AP exceeded in the peer group
*
* @param    L7_IP_ADDR_t widsIPAddr @b{(input)} Cluster Controller IP Address
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_IP_ADDR_t apIPAddr @b{(input)} AP IP Address
* @param    L7_IP_ADDR_t switchIPAddr @b{(input)} Switch IP Address
*
* @returns  void
*
* @notes    This trap is generated by the local switch.
*
* @end
*********************************************************************/
void wdmTrapWSWidsNetworkMaxAPExceeded (L7_IP_ADDR_t widsIPAddr,
                            L7_enetMacAddr_t macAddr, L7_IP_ADDR_t apIPAddr,
                            L7_IP_ADDR_t switchIPAddr);

/*********************************************************************
*
* @purpose  Send trap for TSPEC client admission control bandwidth exceeded
*
* @param    L7_enetMacAddr_t      clientMacAddr   @b{(input)} client MAC address
* @param    L7_enetMacAddr_t      vapMacAddr      @b{(input)} VAP MAC address
* @param    L7_enetMacAddr_t      apMacAddr       @b{(input)} AP MAC address
* @param    L7_uchar8             tid             @b{(input)} traffic stream identifier
* @param    L7_uchar8             userPri         @b{(input)} TS user priority
* @param    L7_uchar8             direction       @b{(input)} TS direction
* @param    L7_ushort16           mediumTime      @b{(input)} TS medium time
* @param    L7_uint32             eventCount      @b{(input)} number of occurrences
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wdmTrapTspecClientAcmBandwidthExceeded(L7_enetMacAddr_t  clientMacAddr,
                                            L7_enetMacAddr_t  vapMacAddr,
                                            L7_enetMacAddr_t  apMacAddr,
                                            L7_uchar8         tid,
                                            L7_uchar8         userPri,
                                            L7_uchar8         direction,
                                            L7_ushort16       mediumTime,
                                            L7_uint32         eventCount);

/*********************************************************************
*
* @purpose  Send trap for TSPEC client traffic unauthorized
*
* @param    L7_enetMacAddr_t      clientMacAddr   @b{(input)} client MAC address
* @param    L7_enetMacAddr_t      vapMacAddr      @b{(input)} VAP MAC address
* @param    L7_enetMacAddr_t      apMacAddr       @b{(input)} AP MAC address
* @param    L7_uchar8             acindex         @b{(input)} access category index
*
* @returns  void
*
* @notes    This trap is generated by the local switch.
*
* @end
*********************************************************************/
void wdmTrapTspecClientTrafficUnauthorized(L7_enetMacAddr_t  clientMacAddr,
                                           L7_enetMacAddr_t  vapMacAddr,
                                           L7_enetMacAddr_t  apMacAddr,
                                           L7_uchar8         acindex);

#endif /* INCLUDE_WDM_TRAP_API_H */
