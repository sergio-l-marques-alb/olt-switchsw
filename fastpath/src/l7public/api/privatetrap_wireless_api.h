/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: privatetrap_wireless_api.h
*
* Purpose: Wireless-related trap functions
*
* Created by: Bala 03/15/2006
*
* Component: SNMP
*
*********************************************************************/

#ifndef PRIVATETRAP_WIRELESS_API_H
#define PRIVATETRAP_WIRELESS_API_H

#include "datatypes.h"
#include "trap_wireless_api.h"

/* Begin Function Prototypes */

/*********************************************************************
*
* @purpose  Send trap for Wireless Switch Enabled.
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
L7_RC_t SnmpWirelessSwitchEnabled(void);

/*********************************************************************
*
* @purpose  Send trap for Wireless Switch Disabled.
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
L7_RC_t SnmpWirelessSwitchDisabled( void );

/*********************************************************************
*
* @purpose  Send trap for Managed AP Database Full.
*
* @param    L7_enetMacAddr_t    macAddr @b{(input)} AP MAC address
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessManagedAPDatabaseFull( L7_enetMacAddr_t macAddr );

/*********************************************************************
*
* @purpose  Send trap for Managed AP Neighbor AP list Full.
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
L7_RC_t SnmpWirelessManagedAPNbrAPListFull( void  );

/*********************************************************************
*
* @purpose  Send trap for Managed AP Neighbor Client list Full.
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
L7_RC_t SnmpWirelessManagedAPNbrClientListFull( void  );

/*********************************************************************
*
* @purpose  Send trap for AP Failure list Full.
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
L7_RC_t SnmpWirelessAPFailureListFull( void  );

/*********************************************************************
*
* @purpose  Send trap for RF Scan AP list Full.
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
L7_RC_t SnmpWirelessRFScanAPListFull( void  );

/*********************************************************************
*
* @purpose  Send trap for client association database Full.
*
* @param    L7_enetMacAddr_t    macAddr @b{(input)} client MAC address
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessClientAssocDatabaseFull( L7_enetMacAddr_t macAddr );

/*********************************************************************
*
* @purpose  Send trap for newly discovered peer switch.
*
* @param    L7_IP_ADDR_t ip @b{(input)} Switch IP address
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessPeerSwitchDiscovered(L7_IP_ADDR_t ip);

/*********************************************************************
*
* @purpose  Send trap for peer switch connection failure.
*
* @param    L7_IP_ADDR_t ip @b{(input)} Switch IP address
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessPeerSwitchFailed(L7_IP_ADDR_t ip);

/*********************************************************************
*
* @purpose  Send trap for peer switch discovered with unknown protocol version.
*
* @param    L7_IP_ADDR_t ip              @b{(input)}  Switch IP address
* @param    L7_ushort16  protocolVersion @b{(input))  protocol
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessPeerSwitchUnknownProtocol(L7_IP_ADDR_t ip,
                                              L7_ushort16  protocolVersion);

/*********************************************************************
*
* @purpose  Send trap for managed AP discovered.
*
* @param    L7_enetMacAddr_t           macAddr @b{(input)} AP MAC address
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessManagedAPDiscovered(L7_enetMacAddr_t  macAddr );

/*********************************************************************
*
* @purpose  Send trap for managed AP failed.
*
* @param    L7_enetMacAddr_t           macAddr @b{(input)} AP MAC address
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessManagedAPFailed(L7_enetMacAddr_t   macAddr );

/*********************************************************************
*
* @purpose  Send trap for AP discovered with unknown protocol version.
*
* @param    L7_enetMacAddr_t macAddr           @b{(input)} AP MAC address
* @param    L7_ushort16      protocolVersion   @b{(input)} protocol
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessManagedAPUnknownProtocol(L7_enetMacAddr_t macAddr,
                                             L7_ushort16      protocolVersion);

/*********************************************************************
*
* @purpose  User inovkes this function to send traps for exceeding configured
*           wireless bandwidth utilization.
*
* @param    L7_enetMacAddr_t macAddr           AP MAC address
* @param    L7_enetMacAddr_t radioMacAddr      Radio interface MAC address
* @param    L7_uchar8        radioIf           Radio interface number
* @param    L7_uchar8        util              Wireless bandwith utilization
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWSLBUtilizationOverflow(L7_enetMacAddr_t macAddr,
                                    L7_enetMacAddr_t radioMacAddr,
                                    L7_uchar8        radioIf,
                                    L7_uchar8        util);

/*********************************************************************
*
* @purpose  Send trap for AP association failures.
*
* @param    L7_enetMacAddr_t    macAddr @b{(input)} AP MAC address
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessAPAssocFailed(L7_enetMacAddr_t  macAddr );

/*********************************************************************
*
* @purpose  Send trap for AP authentication failures.
*
* @param    L7_enetMacAddr_t    macAddr @b{(input)} AP MAC address
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessAPAuthFailed(L7_enetMacAddr_t  macAddr );

/*********************************************************************
*
* @purpose  Send trap for Rogue AP detection.
*
* @param    L7_enetMacAddr_t    macAddr @b{(input)} Managed AP MAC address
* @param    L7_uchar8           radioIf @b{(input)} Radio interface number
* @param    L7_enetMacAddr_t    macAddr @b{(input)} Rogue AP MAC address
* @param    L7_char8           *ssid    @b{(input)} Network SSID
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessRogueAPDetected(L7_enetMacAddr_t macAddr,
                                       L7_uchar8        radioIf,
                                       L7_enetMacAddr_t rogueMacAddr,
                                       L7_char8        *ssid);


/*********************************************************************
*
* @purpose  Send trap for Rogue(s) present in the network.
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessRoguesPresent(void);

/*********************************************************************
*
* @purpose  Send trap for Rogue Client(s) present in the network.
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessRogueClientsPresent(void);

/*********************************************************************
*
* @purpose  Send trap for RF Scan AP detected.
*
* @param    L7_enetMacAddr_t    macAddr @b{(input)} Managed AP MAC address
* @param    L7_uchar8           radioIf @b{(input)} Radio interface number
* @param    L7_enetMacAddr_t    nbrAPMacAddr @b{(input)} Neighbor AP MAC address
* @param    L7_char8           *ssid    @b{(input)} Network SSID
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessRFScanAPDetected(L7_enetMacAddr_t macAddr,
                                        L7_uchar8        radioIf,
                                        L7_enetMacAddr_t nbrAPMacAddr,
                                        L7_char8        *ssid);

/*********************************************************************
*
* @purpose  Send trap for RF Scan Client detected.
*
* @param    L7_enetMacAddr_t    macAddr @b{(input)} Managed AP MAC address
* @param    L7_uchar8           radioIf @b{(input)} Radio interface number
* @param    L7_enetMacAddr_t    macAddr @b{(input)} Client MAC address
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessRFScanClientDetected(L7_enetMacAddr_t macAddr,
                                            L7_uchar8        radioIf,
                                            L7_enetMacAddr_t clientMacAddr);

/*********************************************************************
*
* @purpose  Send trap for Ad Hoc client detected.
*
* @param    L7_enetMacAddr_t    clientMacAddr @b{(input)} Client MAC address
* @param    L7_enetMacAddr_t    apMacAddr     @b{(input)} AP MAC address
* @param    L7_uchar8           radioIf       @b{(input)} Radio interface number
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessAdHocClientDetected(L7_enetMacAddr_t clientMacAddr,
                                           L7_enetMacAddr_t apMacAddr,
                                           L7_uchar8        apRadioIf);

/*********************************************************************
*
* @purpose  Send trap for ad hoc client list Full.
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
L7_RC_t SnmpWirelessAdHocClientListFull( void  );

/*********************************************************************
*
* @purpose  Send trap for associated client.
*
* @param    L7_enetMacAddr_t      clientMacAddr    @b{(input)} client MAC address
* @param    L7_enetMacAddr_t      vapMacAddr @b{(input)} VAP MAC address
* @param    L7_enetMacAddr_t      apMacAddr  @b{(input)} AP  MAC address
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessClientAssocDetected( L7_enetMacAddr_t  clientMacAddr,
                                            L7_enetMacAddr_t  vapMacAddr,
                                            L7_enetMacAddr_t  apMacAddr );

/*********************************************************************
*
* @purpose  Send trap for disassociated client.
*
* @param    L7_enetMacAddr_t     clientMacAddr    @b{(input)} client MAC address
* @param    L7_enetMacAddr_t     vapMacAddr @b{(input)} VAP MAC address
* @param    L7_enetMacAddr_t     apMacAddr  @b{(input)} AP  MAC address
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessClientDisAssocDetected( L7_enetMacAddr_t  clientMacAddr,
                                               L7_enetMacAddr_t  vapMacAddr,
                                               L7_enetMacAddr_t  apMacAddr );

/*********************************************************************
*
* @purpose  Send trap for client roam.
*
* @param    L7_enetMacAddr_t     clientMacAddr    @b{(input)} client MAC address
* @param    L7_enetMacAddr_t     vapMacAddr @b{(input)} VAP MAC address
* @param    L7_enetMacAddr_t     apMacAddr  @b{(input)} AP  MAC address
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessClientRoamDetected( L7_enetMacAddr_t  clientMacAddr,
                                           L7_enetMacAddr_t  vapMacAddr,
                                           L7_enetMacAddr_t  apMacAddr );

/*********************************************************************
*
* @purpose  Send trap for AP association failures.
*
* @param    L7_enetMacAddr_t    macAddr @b{(input)} AP MAC address
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessClientAssocFailure( L7_enetMacAddr_t macAddr );

/*********************************************************************
*
* @purpose  Send trap for AP authentication failures.
*
* @param    L7_enetMacAddr_t    macAddr @b{(input)} AP MAC address
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessClientAuthFailure( L7_enetMacAddr_t macAddr );

/*********************************************************************
*
* @purpose  Send trap for the command received from the peer switch. The
*           command can be a Configuration/Channel/Power related command.
*
* @param    L7_IP_ADDR_t ip @b{(input)} Peer Switch IP address
* @param    L7_WDM_PEER_SWITCH_COMMAND_RX_t type @b{(input)} Trap Type
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessPeerSwitchCommandReceived(L7_IP_ADDR_t ip, 
                                              L7_uint32 type);


/*********************************************************************
*
* @purpose  Send trap for Managed AP List Exceeded for a peer switch.
*
* @param    L7_IP_ADDR_t ip @b{(input)} Peer Switch IP address
* @param    L7_enetMacAddr_t    macAddr @b{(input)} AP MAC address
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessPeerSwitchManagedAPLimitExceeded(L7_IP_ADDR_t ip,
                                                     L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose   Send trap for Client List Exceeded for a peer switch.
*
* @param    L7_IP_ADDR_t ip @b{(input)} Peer Switch IP address
* @param    L7_enetMacAddr_t    macAddr @b{(input)} Client MAC address
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessPeerSwitchClientLimitExceeded(L7_IP_ADDR_t ip,
                                                  L7_enetMacAddr_t macAddr);
 
/*********************************************************************
*
* @purpose  Send trap for channel algorithm complete.
*
* @param    L7_uint32    channelPlanType @b{(input)} Channel Plan Type
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t SnmpWirelessChannelAlgorithmComplete (L7_uint32 planType);

/*********************************************************************
  *
  * @purpose  Send trap for power algorithm complete.
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

L7_RC_t SnmpWirelessPowerAlgorithmComplete(void);

/*********************************************************************
  *
  * @purpose  Send trap for WIDS Controller elected.
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

L7_RC_t SnmpWirelessWidsControllerElected(L7_IP_ADDR_t wsIp);

/*********************************************************************
*
* @purpose  Send trap for Wireless Network Managed AP Exceeded.
*
* @param    L7_IP_ADDR_t widsIPAddr @b{(input)} WIDS IP Address
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_IP_ADDR_t apIPAddr @b{(input)} AP IP Address
* @param    L7_IP_ADDR_t switchIPAddr @b{(input)} Switch IP Address
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessWidsNetworkMaxAPExceeded(L7_IP_ADDR_t widsIPAddr,
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
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessTspecClientAcmBandwidthExceeded(L7_enetMacAddr_t  clientMacAddr,
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
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t SnmpWirelessTspecClientTrafficUnauthorized(L7_enetMacAddr_t  clientMacAddr,
                                                   L7_enetMacAddr_t  vapMacAddr,
                                                   L7_enetMacAddr_t  apMacAddr,
                                                   L7_uchar8         acindex);

/*********************************************************************
*
* @purpose  Send trap for detected client list Full.
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
L7_RC_t SnmpWirelessDetectedClientListFull( void  );

/* End Function Prototypes */

#endif /* PRIVATETRAP_WIRELESS_API_H */
