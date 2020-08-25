/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename trap_wireless_api.h
*
* @purpose Trap Manager wireless functions
*
* @component trapmgr
*
* @comments none
*
* @created  01/11/2006
*
* @author   mpolicharla
*
* @end
*
**********************************************************************/
#ifndef TRAP_WIRELESS_API_H
#define TRAP_WIRELESS_API_H

#include "wireless_commdefs.h"

#define TRAPMGR_SPECIFIC_WIRELESS_ENABLED                                      1
#define TRAPMGR_SPECIFIC_WIRELESS_DISABLED                                     2
#define TRAPMGR_SPECIFIC_WIRELESS_MANAGED_AP_DATABASE_FULL                     3
#define TRAPMGR_SPECIFIC_WIRELESS_MANAGED_AP_AP_NEIGHBOR_LIST_FULL             4
#define TRAPMGR_SPECIFIC_WIRELESS_MANAGED_AP_CLIENT_NEIGHBOR_LIST_FULL         5
#define TRAPMGR_SPECIFIC_WIRELESS_AP_FAILURE_LIST_FULL                         6
#define TRAPMGR_SPECIFIC_WIRELESS_RF_SCAN_AP_LIST_FULL                         7
#define TRAPMGR_SPECIFIC_WIRELESS_CLIENT_ASSOC_DATABASE_FULL                   8
#define TRAPMGR_SPECIFIC_WIRELESS_CLIENT_FAILURE_LIST_FULL                     9
#define TRAPMGR_SPECIFIC_WIRELESS_PEER_SWITCH_DISCOVERED                      10
#define TRAPMGR_SPECIFIC_WIRELESS_PEER_SWITCH_FAILED                          11
#define TRAPMGR_SPECIFIC_WIRELESS_PEER_SWITCH_UNKNOWN_PROTOCOL_DISCOVERED     12
#define TRAPMGR_SPECIFIC_WIRELESS_MANAGED_AP_DISCOVERED                       13
#define TRAPMGR_SPECIFIC_WIRELESS_MANAGED_AP_FAILED                           14
#define TRAPMGR_SPECIFIC_WIRELESS_MANAGED_AP_UNKNOWN_PROTOCOL_DISCOVERED      15
#define TRAPMGR_SPECIFIC_WIRELESS_LB_USERS_MAXIMUM_OVERFLOW                   16
#define TRAPMGR_SPECIFIC_WIRELESS_LB_USERS_THRESHOLD_OVERFLOW                 17
#define TRAPMGR_SPECIFIC_WIRELESS_LB_UTILIZATION_OVERFLOW                     18
#define TRAPMGR_SPECIFIC_WIRELESS_MANAGED_AP_ASSOC_FAILED                     19
#define TRAPMGR_SPECIFIC_WIRELESS_MANAGED_AP_AUTH_FAILED                      20
#define TRAPMGR_SPECIFIC_WIRELESS_RF_SCAN_ROGUE_AP_DETECTED                   21
#define TRAPMGR_SPECIFIC_WIRELESS_RF_SCAN_NEW_AP_DETECTED                     22
#define TRAPMGR_SPECIFIC_WIRELESS_RF_SCAN_NEW_CLIENT_DETECTED                 23
#define TRAPMGR_SPECIFIC_WIRELESS_RF_SCAN_ADHOC_CLIENT_DETECTED               24
#define TRAPMGR_SPECIFIC_WIRELESS_CLIENT_ASSOC_DETECTED                       25
#define TRAPMGR_SPECIFIC_WIRELESS_CLIENT_DISASSOC_DETECTED                    26
#define TRAPMGR_SPECIFIC_WIRELESS_CLIENT_ROAM_DETECTED                        27
#define TRAPMGR_SPECIFIC_WIRELESS_CLIENT_ASSOC_FAILED                         28
#define TRAPMGR_SPECIFIC_WIRELESS_CLIENT_AUTH_FAILED                          29
#define TRAPMGR_SPECIFIC_WIRELESS_AD_HOC_CLIENT_LIST_FULL                     30
#define TRAPMGR_SPECIFIC_WIRELESS_CHANNEL_ALGO_COMPLETE                       31
#define TRAPMGR_SPECIFIC_WIRELESS_POWER_ALGO_COMPLETE                         32
#define TRAPMGR_SPECIFIC_WIRELESS_PEER_SWITCH_COMMAND_RECEIVED                33
#define TRAPMGR_SPECIFIC_WIRELESS_PEER_SWITCH_MANAGED_AP_DATABASE_FULL        34
#define TRAPMGR_SPECIFIC_WIRELESS_PEER_SWITCH_CLIENT_DATABASE_FULL            35
#define TRAPMGR_SPECIFIC_WIRELESS_PEER_SWITCH_CLIENT_ROAM_DETECTED            36
#define TRAPMGR_SPECIFIC_WIRELESS_WIDS_CONTROLLER_ELECTED                     37
#define TRAPMGR_SPECIFIC_WIRELESS_ROGUES_PRESENT                              38
#define TRAPMGR_SPECIFIC_WIRELESS_DETECTED_CLIENT_LIST_FULL                   39
#define TRAPMGR_SPECIFIC_WIRELESS_TSPEC_CLIENT_ACM_BANDWIDTH_EXCEEDED         40
#define TRAPMGR_SPECIFIC_WIRELESS_TSPEC_CLIENT_TRAFFIC_UNAUTHORIZED           41


/*********************************************************************
*
* @purpose  Get global wireless trap mode.
*
* @param    L7_uint32 *mode      L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrWirelessTrapModeGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Allows a user to enable or disable all wireless traps.
*
* @param    L7_uint32 mode  L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  individual traps are configured within the wireless sytem.
*
* @end
*********************************************************************/
L7_RC_t trapMgrWirelessTrapModeSet(L7_uint32 mode);


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
L7_RC_t trapMgrWirelessSwitchEnabled( void );

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
L7_RC_t trapMgrWirelessSwitchDisabled( void );

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
L7_RC_t trapMgrWirelessManagedAPDatabaseFull( L7_enetMacAddr_t macAddr );


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
L7_RC_t trapMgrWirelessManagedAPNbrAPListFull( void );


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
L7_RC_t trapMgrWirelessManagedAPNbrClientListFull( void );


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
L7_RC_t trapMgrWirelessAPFailureListFull( void  );


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
L7_RC_t trapMgrWirelessRFScanAPListFull( void );



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
L7_RC_t trapMgrWirelessClientAssocDatabaseFull( L7_enetMacAddr_t macAddr );



/*********************************************************************
*
* @purpose  Send trap for client failure list Full.
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
L7_RC_t trapMgrWirelessClientFailureListFull( void );

/*********************************************************************
*
* @purpose  Send trap for channel algorithm complete.
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
L7_RC_t trapMgrWirelessChannelAlgorithmComplete(L7_uchar8 type );

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
L7_RC_t trapMgrWirelessPowerAlgorithmComplete( void  );

/*********************************************************************
*
* @purpose  Send trap for wids controller election
*
* @param    wsIp -IP address of the local swtich
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrWirelessWSWidsControllerElected(L7_IP_ADDR_t wsIp);

/*********************************************************************
*
* @purpose  Send trap for Network Managed AP max. exceeded.
*
* @param    L7_IP_ADDR_t        widsIPAddr @b{(input)} WIDS IP Address
* @param    L7_enetMacAddr_t    macAddr @b{(input)} AP MAC Address
* @param    L7_IP_ADDR_t        apIPAddr @b{(input)} AP IP Address
* @param    L7_IP_ADDR_t        switchIPAddr @b{(input)} Switch IP Address
*
* @returns  L7_SUCCESS if trap send successful 
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrWirelessWSWidsNetworkMaxAPExceeded (L7_IP_ADDR_t widsIPAddr,
                            L7_enetMacAddr_t macAddr,  L7_IP_ADDR_t apIPAddr,
                            L7_IP_ADDR_t switchIPAddr);

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
L7_RC_t trapMgrWirelessPeerSwitchDiscovered(L7_IP_ADDR_t ip);


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
L7_RC_t trapMgrWirelessPeerSwitchFailed(L7_IP_ADDR_t ip);


/*********************************************************************
*
* @purpose  Send trap for peer switch command config push received.
*
* @param    L7_IP_ADDR_t ip @b{(input)} Switch IP address
* @param    L7_uint32    mask @b{(input)} configuration mask received
*
* @returns  L7_SUCCESS if trap send successful 
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrWirelessPeerSwitchConfigurationCommandRx(L7_IP_ADDR_t ip,
                                                        L7_uint32 mask);


/*********************************************************************
*
* @purpose  Send trap for peer switch managed AP database Full.
*
* @param    L7_enetMacAddr_t    macAddr @b{(input)} AP MAC address
* @param    L7_IP_ADDR_t    ipAddr @b{(input)} peer switch IP address
*
* @returns  L7_SUCCESS if trap send successful 
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrWirelessPeerSwitchManagedAPDatabaseFull( L7_enetMacAddr_t macAddr,
                                                        L7_IP_ADDR_t    ipAddr );


/*********************************************************************
*
* @purpose  Send trap for peer switch client database Full.
*
* @param    L7_enetMacAddr_t    macAddr @b{(input)} client MAC address
* @param    L7_IP_ADDR_t    ipAddr @b{(input)} peer switch IP address
*
* @returns  L7_SUCCESS if trap send successful 
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrWirelessPeerSwitchClientDatabaseFull( L7_enetMacAddr_t macAddr,
                                                        L7_IP_ADDR_t  ipAddr );

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
L7_RC_t trapMgrWirelessPeerSwitchUnknownProtocol(L7_IP_ADDR_t ip,
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
L7_RC_t trapMgrWirelessManagedAPDiscovered(L7_enetMacAddr_t  macAddr );


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
L7_RC_t trapMgrWirelessManagedAPFailed(L7_enetMacAddr_t   macAddr );


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
L7_RC_t trapMgrWirelessManagedAPUnknownProtocol(L7_enetMacAddr_t macAddr,
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
L7_RC_t trapMgrWSLBUtilizationOverflow(L7_enetMacAddr_t macAddr,
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
L7_RC_t trapMgrWirelessAPAssocFailed(L7_enetMacAddr_t  macAddr );


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
L7_RC_t trapMgrWirelessAPAuthFailed(L7_enetMacAddr_t  macAddr );



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
L7_RC_t trapMgrWirelessRogueAPDetected(L7_enetMacAddr_t macAddr,
                                       L7_uchar8        radioIf,
                                       L7_enetMacAddr_t rogueMacAddr,
                                       L7_char8        *ssid);


/*********************************************************************
*
* @purpose  Send trap for Rogues present in the network
*
* @returns  L7_SUCCESS if trap send successful 
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrWirelessRoguesPresent(void);

/*********************************************************************
*
* @purpose  Send trap for Rogue Clients present in the network
*
* @returns  L7_SUCCESS if trap send successful
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrWirelessRogueClientsPresent(void);


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
L7_RC_t trapMgrWirelessRFScanAPDetected(L7_enetMacAddr_t macAddr,
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
L7_RC_t trapMgrWirelessRFScanClientDetected(L7_enetMacAddr_t macAddr,
                                            L7_uchar8        radioIf,
                                            L7_enetMacAddr_t clientMacAddr);


/*********************************************************************
*
* @purpose  Send trap for Ad Hoc client detected.
*
* @param    L7_enetMacAddr_t    clientMacAddr @b{(input)} Client MAC address
* @param    L7_enetMacAddr_t    apMacAddr       @b{(input)} AP MAC address
* @param    L7_uchar8           radioIf @b{(input)} Radio interface number 
*
* @returns  L7_SUCCESS if trap send successful 
* @returns  L7_FAILURE if any errors
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrWirelessAdHocClientDetected(L7_enetMacAddr_t clientMacAddr,
                                           L7_enetMacAddr_t apMacAddr,
                                           L7_uchar8        apRadioIf);


/*********************************************************************
*
* @purpose  Send trap for ad hoc client list full.
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
L7_RC_t trapMgrWirelessAdHocClientListFull( void );


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
L7_RC_t trapMgrWirelessClientAssocDetected( L7_enetMacAddr_t  clientMacAddr, 
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
L7_RC_t trapMgrWirelessClientDisAssocDetected( L7_enetMacAddr_t  clientMacAddr,
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
L7_RC_t trapMgrWirelessClientRoamDetected( L7_enetMacAddr_t  clientMacAddr,
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
L7_RC_t trapMgrWirelessClientAssocFailure( L7_enetMacAddr_t macAddr );



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
L7_RC_t trapMgrWirelessDetectedClientListFull( void );

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
L7_RC_t trapMgrWirelessTspecClientAcmBandwidthExceeded(L7_enetMacAddr_t  clientMacAddr,
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
L7_RC_t trapMgrWirelessTspecClientTrafficUnauthorized(L7_enetMacAddr_t  clientMacAddr,
                                                      L7_enetMacAddr_t  vapMacAddr,
                                                      L7_enetMacAddr_t  apMacAddr,
                                                      L7_uchar8         acindex);

#endif /* TRAP_WIRELESS_API_H */
