/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     wdm_peer_switch_api.h
*
* @purpose      Wireless Data Manager (WDM) API header
*
* @component    WDM
*
* @comments     none
*
* @create       12/08/2006
*
* @author       mpolicharla
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_WDM_PEER_SWITCH_API_H
#define INCLUDE_WDM_PEER_SWITCH_API_H

#include "datatypes.h"
#include "wireless_commdefs.h"

/*********************************************************************
* @purpose  Verifies whether an entry is present or not.
*
* @param    ipAddr          @b{(input)} IP address of the switch
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      If entry doesn't exist
*
* @notes    This routine assumes ipAddr parameter is validated before call
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchEntryGet(L7_IP_ADDR_t ipAddr);

/*********************************************************************
* @purpose  Gets the next managed peer switch IP address.
*
* @param    ipAddr       @b{(input)} IP address of the switch
* @param    nextIpAddr   @b{(output)} Next Managed switch IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This routine assumes all parameters are validated before the call.
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchEntryNextGet(L7_IP_ADDR_t ipAddr,
                                  L7_IP_ADDR_t *nextIpAddr);

/*********************************************************************
* @purpose  Gets vendorId for the given peer switch.
*
* @param    ipAddr           @b{(input)} IP address of the switch
* @param    vendorId         @b{(output)} Vendor of the peer WS software
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid ip address is passed
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchVendorIdGet(L7_IP_ADDR_t ipAddr, L7_ushort16 *vendorId);

/*********************************************************************
* @purpose  Gets the protocolVersion of the given peer switch.
*
* @param    L7_IP_ADDR_t ip               @b{(input)}  IP address of the switch
* @param    L7_ushort16 *protocolVersion  @b{(output)} Version of WS software on the
*                                                      peer WS
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid ip address is passed
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchProtocolVersionGet(L7_IP_ADDR_t ip,
                                        L7_ushort16 *protocolVersion);

/*********************************************************************
* @purpose  Gets the software version for the given peer switch.
*
* @param    L7_IP_ADDR_t ip               @b{(input)}  IP address of the switch
* @param    L7_ushort16 *softwareVersion  @b{(output)} software version of the peer.
*                                                        buffer must be L7_WDM_SOFTWARE_VERSION_MAX+1.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchSoftwareVersionGet(L7_IP_ADDR_t ip,
                                        L7_char8    *softwareVersion);

/*********************************************************************
* @purpose  Gets the MAC address for the given peer switch.
*
* @param    L7_IP_ADDR_t ip               @b{(input)}  IP address of the switch
* @param    L7_enetMacAddr_t *macAddr     @b{(output)} mac of the peer.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchMacAddrGet(L7_IP_ADDR_t ip,
                                L7_enetMacAddr_t  *macAddr);

/*********************************************************************
* @purpose  Get discovery method of the given peer switch.
*
* @param    L7_IP_ADDR_t          ip      @b{(input)}  IP address of the switch
* @param    L7_WDM_DISC_REASON_t *reason  @b{(output)} discovery reason
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid ip address is passed
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchDiscoveryReasonGet(L7_IP_ADDR_t ip,
                                        L7_WDM_DISC_REASON_t *reason);


/*********************************************************************
* @purpose  Update age field for a given peer switch.
*
* @param    L7_IP_ADDR_t ip   @b{(input)}  IP address of the switch
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This routine assumes a valid ip address is passed
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchAgeUpdate(L7_IP_ADDR_t ip);


/*********************************************************************
* @purpose  Gets age field for a given peer switch.
*
* @param    L7_IP_ADDR_t ip   @b{(input)}  IP address of the switch
* @param    L7_uint32   *age  @b{(output)} seconds since last update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This routine assumes a valid ip address is passed
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchAgeGet(L7_IP_ADDR_t ip,
                            L7_uint32   *age);

/*********************************************************************
* @purpose  Gets peer switch IP address that pushed config.
*
* @param    ipAddr           @b{(input)} IP address of the switch
* @param    peerIPAddr       @b{(output)} peer switch IP address that pushed config
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid ip address is passed
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchConfigPushRxIPAddrGet(L7_IP_ADDR_t ipAddr,
                                           L7_IP_ADDR_t *peerIPAddr);

/*********************************************************************
* @purpose  Gets config push receive time from peer switch.
*
* @param    ipAddr         @b{(input)} IP address of the switch
* @param    rxTime         @b{(output)} config receive time stamp
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid ip address is passed
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchConfigPushRxTimeGet(L7_IP_ADDR_t ipAddr,L7_uint32 *rxTime);

/*********************************************************************
* @purpose  Gets config push mask receive from peer switch.
*
* @param    ipAddr        @b{(input)} IP address of the switch
* @param    mask          @b{(output)} config receive mask
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid ip address is passed
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchConfigPushRxMaskGet(L7_IP_ADDR_t ipAddr,L7_uint32 *mask);

/*********************************************************************
* @purpose  Gets config push send status to peer switch.
*
* @param    ipAddr        @b{(input)} IP address of the switch
* @param    status        @b{(output)} config push send status
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid ip address is passed
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchConfigPushReqStatusGet(L7_IP_ADDR_t ipAddr,
                                            L7_WDM_PEER_SWITCH_CP_SEND_STATUS_t  *status);

/*********************************************************************
* @purpose  Verifies whether an entry is present or not.
*
* @param    L7_enetMacAddr_t macAddr  @b{(input)} MAC address of AP
* @param    L7_IP_ADDR_t     ip       @b{(input)} IP address of the switch,
*                                                   ip == 0 if not specified
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      If entry doesn't exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchAPEntryGet(L7_enetMacAddr_t macAddr,
                                L7_IP_ADDR_t     ip);

/*********************************************************************
* @purpose  Gets the next managed AP from the given MAC address. This can
*           also be used to get the first MAC address index.
*
* @param   macAddr         @b{(input)}  MAC address of the AP
* @param   nextMacAddr     @b{(output)} Next managed MAC address of the AP
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  For getting nextMacAddr, macAddr can be either flushed with
*            all 0's or a valid one.
*
* @notes     For fetching macAddr of the first entry, macAddr must be
*            flushed with all 0's.
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchAPEntryNextGet(L7_enetMacAddr_t macAddr,
                                    L7_enetMacAddr_t *nextMacAddr);

/*********************************************************************
*
* @purpose  Determine if an entry exists Peer switch IP-AP MAC mapping.
*
* @param    L7_IP_ADDR_t     ipAddr    @b{(input)}  Peer switch IP address
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  Peer switch managed AP MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  entries are added to the Peer switch IP-AP MAC mapping, when an entry is
*            added to the peer switch managed AP database, and are removed
*            when entry is removed from the peer switch managed AP database.
*
* @end
*
*********************************************************************/
L7_RC_t wdmPeerSwitchIPAPEntryGet(L7_IP_ADDR_t ipAddr,L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Get next entry in the Peer switch IP-AP MAC mapping.
*
* @param    L7_IP_ADDR_t     ipAddr    @b{(input)}  Peer switch IP address
* @param    L7_enetMacAddr_t apMacAddr   @b{(input)}  Peer switch managed AP MAC address
* @param    L7_IP_ADDR_t     *nextIPAddr     @b{(output)}  next peer switch IP address
* @param    L7_enetMacAddr_t *nextAPMacAddr  @b{(output)}  next peer switch managed AP MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  entries are added to the Peer switch IP-AP MAC mapping, when an entry is
*            added to the peer switch managed AP database, and are removed
*            when entry is removed from the peer switch managed AP database.
*
* @end
*
*********************************************************************/
L7_RC_t wdmPeerSwitchIPAPEntryNextGet(L7_IP_ADDR_t      ipAddr,
                                      L7_enetMacAddr_t  apMacAddr,
                                      L7_IP_ADDR_t      *nextIPAddr,
                                      L7_enetMacAddr_t  *nextAPMacAddr);

/*********************************************************************
* @purpose  Gets switch IP address for a given AP MAC address.
*
* @param    macAddr          @b{(input)} AP MAC address
* @param    ipAddr           @b{(output)} Switch IP address managing the AP
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid MAC address is passed
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchAPSwitchIpAddrGet(L7_enetMacAddr_t macAddr,
                                       L7_IP_ADDR_t *ipAddr);

/*********************************************************************
* @purpose  Checks whether an entry is present or not.
*
* @param    macAddr         @b{(input)}MAC address of an Client
* @param    switchIpAddr    @b{(input)}IP address of the switch
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE                       If entry doesn't exist.
*
* @notes    This routine assumes all parameters are validated before the call.
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchClientEntryGet(L7_enetMacAddr_t macAddr,
                                    L7_IP_ADDR_t ipAddr);

/*********************************************************************
* @purpose  Gets the next managed client.
*
* @param   macAddr         @b{(input)}  MAC address of the client
* @param   nextMacAddr     @b{(output)} Next managed MAC address of the client
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchClientEntryNextGet(L7_enetMacAddr_t macAddr,
                                        L7_enetMacAddr_t *nextMacAddr);

/*********************************************************************
*
* @purpose  Delete an existing peer client entry in the associated
*           client database.
*
* @param    L7_enetMacAddr_t macAddr    @b{(input)}  client MAC address
* @param    L7_IP_ADDR_t  switchIPAddr  @b{(input)}  switch IP address
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmPeerAuthClientEntryDelete(L7_enetMacAddr_t macAddr,
                                     L7_IP_ADDR_t switchIPAddr);

/*********************************************************************
  APIs for wireless components, these are not available via usmDb
*********************************************************************/

/*********************************************************************
* @purpose  Add or update switch status entry.
*
* @param    L7_IP_ADDR_t         ip               @b{(input)} IP address of peer WS
* @param    L7_ushort16          vendorId         @b{(input)} vendor of the peer switch software
* @param    L7_ushort16          protocolVersion  @b{(input)} protocol version of the peer switch
* @param    L7_uchar8           *softwareVersion  @b{(input)} software version of the peer switch,
*                                                             size is L7_WDM_SOFTWARE_VERSION_MAX
* @param    L7_WDM_DISC_REASON_t reason           @b{(input)} discovery reason code
*
* returns   L7_SUCCESS, entry added or updated
* returns   L7_FAILURE, peer switch table full
*
*
* @comments This function adds an entry if the entry doesn't exist, or else
* @comments it will update the entry.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchEntryAddUpdate(L7_IP_ADDR_t         ip,
                                    L7_enetMacAddr_t     macAddr,
                                    L7_ushort16          vendorId,
                                    L7_ushort16          protocolVersion,
                                    L7_char8            *softwareVersion,
                                    L7_WDM_DISC_REASON_t reason);

/*********************************************************************
* @purpose  Delete switch status entry.
*
* @param    ipAddr               @{(input)} IP address of the switch
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This routine assumes all parameters are validated before the call.
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchDelete(L7_IP_ADDR_t ipAddr);

/*********************************************************************
* @purpose  Gets the notify pending flag for the peer switch.
*
* @param    L7_IP_ADDR_t   ip      @b{(input)} IP address of the switch
* @param    L7_BOOL       *flag    @b{(output)} notify pending flag
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    The notify pending flag indicates that we need to send
*            the initial AP and client status to this peer switch, its
*            set when a new peer switch entry is added.
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchNotifyPendingGet(L7_IP_ADDR_t ip,
                                      L7_BOOL     *flag);


/*********************************************************************
* @purpose  Gets and clears the global peer group ID changed pending flag.
*
* @param    L7_BOOL       *flag    @b{(output)} peer group ID changed pending flag
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    The peer group ID changed pending flag indicates that we need to
*           terminate connection to existing peer switches, its set when
*           peer group ID is changed.
*
* @end
*********************************************************************/
L7_RC_t wdmPeerGroupIdPendingGetClear(L7_BOOL  *flag);


/*********************************************************************
*
* @purpose  Read and clear global flag(s) to determine if any peer switches
*           have a pending action.
*
* @param    L7_WDM_PEER_SWITCH_FLAG_t  flags    @b((input))  type of pending action(s).
* @param    L7_WDM_PEER_SWITCH_FLAG_t *setFlags @b((output)) flags set and cleared.
*
* @returns  L7_SUCCESS, one or more flags was read and cleared
* @returns  L7_FAILURE, no flags set
*
* @comments If caller doesn't care which flags were read/cleared,
*             setFlags can be passed as L7_NULLPTR.
*
* @end
*
*********************************************************************/
L7_RC_t wdmPeerSwitchPendingFlagsGetClear(L7_WDM_PEER_SWITCH_FLAG_t  flags,
                                          L7_WDM_PEER_SWITCH_FLAG_t *setFlags);


/*********************************************************************
*
* @purpose  Get the next client entry flagged for a pending action.
*
* @param    L7_IP_ADDR_t                ip          @b{(input)}  peer switch IP address flagged
* @param    L7_WDM_PEER_SWITCH_FLAG_t  flags        @b{(input)}  type of pending flags
* @param    L7_WDM_PEER_SWITCH_FLAG_t  flagsCleared @b{(output)}  cleared pending flags
* @param    L7_IP_ADDR_t               *nextIP      @b{(output)} next peer switch IP address flagged
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments ip 0.0.0.0 returns first pending entry for specified flag. The flag
*           for the returned IP address is cleared.
*
* @end
*
*********************************************************************/
L7_RC_t wdmPeerSwitchPendingEntryNextGet(L7_IP_ADDR_t ip,
                                         L7_WDM_PEER_SWITCH_FLAG_t  flags,
                                         L7_WDM_PEER_SWITCH_FLAG_t  *flagsCleared,
                                         L7_IP_ADDR_t *nextIP);


/*********************************************************************
*
* @purpose  Set pending flag for peer switch.
*
* @param    L7_IP_ADDR_t               ip     @b{(input)}  peer switch IP address
* @param    L7_WDM_PEER_SWITCH_FLAG_t  flag   @b{(input)}  type of pending flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Flags are set by explicitly calling this API, not set automatically
*            by WDM.
*
* @end
*
*********************************************************************/
L7_RC_t wdmPeerSwitchFlagSet(L7_IP_ADDR_t ip, L7_WDM_PEER_SWITCH_FLAG_t flag);


/*********************************************************************
* @purpose  Sets peer switch config push send status.
*
* @param    ipAddr      @b{(input)} IP address of the switch
* @param    mask        @b{(input)} config push mask
* @param    timeStamp   @b{(input)} config push time stamp
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid ip address is passed
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchConfigPushReqStatusSet(L7_IP_ADDR_t ipAddr,
                                            L7_WDM_PEER_SWITCH_CP_SEND_STATUS_t status);


/*********************************************************************
* @purpose  Add or update AP status entry.
*
* @param    macAddr        @b{(input)} MAC address of AP
* @param    switchIpAddr   @b{(input)} IP address of the switch
* @param    apIPAddr       @b{(input)} IP address of the AP
* @param    apIPMask       @b{(input)} IP subnet mask of the AP
* @param    location       @b{(input)} AP location
* @param    profileId      @b{(input)} Profile ID associated with AP
* @param    profileName    @b{(input)} Profile Name associated with AP
* @param    hwType         @b{(input)} Hardware type of AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    This function adds an entry if the entry doesn't exist, or else
* @comments    it will update the entry
*
* @notes    This routine assumes all parameters are validated before the call.
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchAPEntryAddUpdate(L7_enetMacAddr_t macAddr,
                                      L7_IP_ADDR_t     switchIpAddr,
                                      L7_IP_ADDR_t     apIPAddr,
                                      L7_IP_MASK_t     apIPMask,
                                      L7_uchar8        *location,
                                      L7_uchar8        profileId,
                                      L7_uchar8        *profileName,
                                      L7_uint32        hwType);


/*********************************************************************
* @purpose  Delete AP status entry of a peer switch.
*
* @param    macAddr        @b{(input)} MAC address of the AP
* @param    ipAddr         @b{(input)} IP address of the switch
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments   Deletion can be done with or without specifying IP address of
* @comments   the peer switch. In the later case 0 must be passed as parameter.
*
* @notes      This routine assumes all parameters are validated before the call
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchAPEntryDelete(L7_enetMacAddr_t macAddr,
                                         L7_IP_ADDR_t ipAddr);

/*********************************************************************
* @purpose  Gets AP IP address for a given AP MAC address.
*
* @param    macAddr          @b{(input)} AP MAC address
* @param    ipAddr           @b{(output)} AP IP address
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid MAC address is passed
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchAPIPAddrGet(L7_enetMacAddr_t macAddr,
                                 L7_IP_ADDR_t    *ipAddr);

/*********************************************************************
* @purpose  Gets AP IP mask for a given AP MAC address.
*
* @param    macAddr          @b{(input)} AP MAC address
* @param    ipMask           @b{(output)} AP IP mask
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid MAC address is passed
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchAPIPMaskGet(L7_enetMacAddr_t macAddr,
                                 L7_IP_MASK_t    *ipMask);

/*********************************************************************
* @purpose  Gets AP IP subnet for a given AP MAC address.
*
* @param    macAddr          @b{(input)} AP MAC address
* @param    ipMask           @b{(output)} AP IP mask
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid MAC address is passed
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchAPIPSubnetGet(L7_enetMacAddr_t macAddr,
                                 L7_IP_MASK_t    *ipSubnet);

/*********************************************************************
* @purpose  Gets AP location Name for a given AP MAC address.
*
* @param    macAddr          @b{(input)} AP MAC address
* @param    location         @b{(output)} location name of managed AP
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid MAC address is passed
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchAPLocationGet(L7_enetMacAddr_t macAddr,
                                   L7_uchar8     *location);

/*********************************************************************
* @purpose  Gets AP profile Id for a given AP MAC address.
*
* @param    macAddr          @b{(input)} AP MAC address
* @param    profileId        @b{(output)} profile ID applied for managed AP
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid MAC address is passed
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchAPProfileIdGet(L7_enetMacAddr_t macAddr,
                                    L7_uchar8     *profileId);

/*********************************************************************
* @purpose  Gets AP profile name for a given AP MAC address.
*
* @param    macAddr         @b{(input)} AP MAC address
* @param    profileName     @b{(output)} profile name applied for managed AP
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid MAC address is passed
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchAPProfileNameGet(L7_enetMacAddr_t macAddr,
                                      L7_uchar8     *profileName);

/*********************************************************************
* @purpose  Gets AP hardware type for a given AP MAC address.
*
* @param    macAddr          @b{(input)} AP MAC address
* @param    hwType        @b{(output)} hardware type of managed AP
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid MAC address is passed
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchAPHwTypeGet(L7_enetMacAddr_t macAddr,
                                 L7_uint32     *hwType);

/*********************************************************************
* @purpose  Gets managed AP's count for the given peer switch.
*
* @param    ipAddr           @b{(input)} IP address of the switch
* @param    mapCount         @b{(output)} Managed AP's count for the peer switch
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @notes    This routine assumes a valid ip address is passed
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchManagedAPsGet(L7_IP_ADDR_t ip,L7_ushort16 *mapCount);

/*********************************************************************
* @purpose  Add or update Client status entry.
*
* @param    macAddr             @b{(input)} MAC address of client
* @param    switchIpAddr        @b{(input)} IP address of the switch
* @param    clientIpAddr        @b{(input)} IP address of the client
* @param    authenticationData  @b{(input)} Authentication data of the client
*
* returns   L7_SUCCESS
* returns   L7_ERROR            If entry  not added.
*
*
* @comments  This function adds an entry if the entry doesn't exists, or else
* @comments  it will updates the entry
*
* @notes     authenticationData is not defined yet in the FS
*
* @notes     This routine assumes all parameters are validated before the call
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchClientEntryAddUpdate(L7_enetMacAddr_t macAddr,
                                          L7_IP_ADDR_t switchIpAddr,
                                          L7_IP_ADDR_t clientIpAddr
                                          /*,TBD authenticationData */);

/*********************************************************************
* @purpose  Delete client status entry managed a peer switch.
*
* @param    macAddr        @b{(input)} MAC address of client
* @param    ipAddr         @b{(input)} IP address of the switch
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes     This routine assumes all parameters are validated before the call
*
* @comments  Deletion can be done with or without specifying IP address of
* @comments  the peer switch. In the later case 0 should be passed as parameter.
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchClientEntryDelete(L7_enetMacAddr_t macAddr,
                                       L7_IP_ADDR_t ipAddr);


/*********************************************************************
* @purpose  Returns the Cluster Priority for a given peer switch
*
* @param    ipAddr          @b{(input)} IP address of the switch
* @param    prio            @b{(output)} Cluster Priority
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchWidsPriorityGet(L7_IP_ADDR_t ipAddr,
                            L7_uchar8 *prio);

/*********************************************************************
* @purpose  Stores the Cluster Priority of a given peer switch
*
* @param    ipAddr          @b{(input)} IP address of the switch
* @param    prio            @b{(input)} priority
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchWidsPrioritySet(
                            L7_IP_ADDR_t ipAddr,
                            L7_uchar8    prio);

/*********************************************************************
*
* @purpose  Set the max. number of APs that are managed
*           by the speicified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the peer switch.
* @param    L7_uint32  value  @b{(input)} value specifying max APS.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments  None.
*
* @end
*
*********************************************************************/
L7_RC_t wdmPeerSwitchMaxManagedAPsSet(L7_IP_ADDR_t IPAddr, L7_uint32 value);

/*********************************************************************
* @purpose  Get AP image download mode supported by the peer switch
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} peer switch IP address
* @param    L7_uchar8     mode    @b{(output)} AP image download mode
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchAPImageDnloadModeGet(L7_IP_ADDR_t ipAddr, L7_uchar8 *mode);

/*********************************************************************
* @purpose  Set AP image download mode supported by the peer switch
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} peer switch IP address
* @param    L7_uchar8     mode    @b{(input)} AP image download mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchAPImageDnloadModeSet(L7_IP_ADDR_t ipAddr, L7_uchar8 mode);

/*********************************************************************
*
* @purpose  Update Per switch counters for managed AP database entry status
*           for specified peer switch.
*
* @param    L7_WDM_MANAGED_AP_STATUS_t prevStatus  @b{(input)} previous status
* @param    L7_WDM_MANAGED_AP_STATUS_t newStatus   @b{(input)} new status
*
* @returns  void
*
*
* @notes    This is an internal function to be called on any
*           transition for a managed AP entry status.
*           Caller should ensure that this function is invoked for peer switch
*           status only when WS acts Cluster Controller.
*
* @end
*********************************************************************/
void wdmPeerSwitchManagedAPCountersUpdate(L7_uint32 index,
                               L7_WDM_MANAGED_AP_STATUS_t prevStatus,
                               L7_WDM_MANAGED_AP_STATUS_t newStatus);


/*********************************************************************
* @purpose  Returns the X.509 certificate for a given peer switch
*
* @param    ipAddr          @b{(input)} IP address of the switch
* @param    certPtr         @b{(output)} pointer to store X.509 certificate 
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchX509CertificateGet(L7_IP_ADDR_t ipAddr,
                                        L7_uchar8 *certPtr);

/*********************************************************************
* @purpose  Set X.509 certificate supported by the peer switch
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} peer switch IP address
* @param    L7_uchar8     certPtr @b{(input)} X509 certificate 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t wdmPeerSwitchX509CertificateSet(L7_IP_ADDR_t ipAddr, 
                                        L7_uchar8    *certPtr);

#endif /* INCLUDE_WDM_PEER_SWITCH_API_H */
