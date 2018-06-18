/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   usmdb_wdm_client_api.h
*
* @purpose    Wireless Data Manager Client Status USMDB API header
*
* @component  USMDB
*
* @comments   none
*
* @create     02/02/06
*
* @author     dfowler
*
* @end
*             
*********************************************************************/
#ifndef INCLUDE_USMDB_WDM_CLIENT_API_H
#define INCLUDE_USMDB_WDM_CLIENT_API_H

#include "usmdb_mib_diffserv_private_api.h"
#include "acl_exports.h"
#include "wdm_client_api.h"

/*********************************************************************
*
* @purpose  Get an entry from the associated client database.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientEntryGet(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Get the next entry from the associated client database.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)}  client MAC address
* @param    L7_enetMacAddr_t *nextMacAddr @b{(output)} client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments macAddr 00:00:00:00:00:00 returns first entry.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientEntryNextGet(L7_enetMacAddr_t  macAddr, 
                                        L7_enetMacAddr_t *nextMacAddr);

/*********************************************************************
*
* @purpose  Force switch to disassociate a client.
*
* @param    L7_enetMacAddr_t       macAddr  @b{(input)}  client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, client is not associated.
*
* @comments
*           
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientForceDisassoc(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Force switch to disassociate all clients.
*
* @param    None
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, client is not associated.
*
* @comments
*           
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientAllForceDisassoc();

/*********************************************************************
*
* @purpose  Force switch to disassociate a clients associated to the AP.
*
* @param    L7_enetMacAddr_t      macAddr  @b{(input)}  AP MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE            clients is not associated.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAPAssocClientsForceDisassoc(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Force switch to disassociate a clients associated to the VAP.
*
* @param    L7_enetMacAddr_t      macAddr  @b{(input)}  VAP MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE            clients is not associated.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmVAPAssocClientsForceDisassoc(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Force switch to disassociate a clients associated to an SSID.
*
* @param    L7_char8              *ssid  @b{(input)}  SSID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE            clients is not associated.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSSIDAssocClientsForceDisassoc(L7_char8 *ssid);

/*********************************************************************
*
* @purpose  Get status for an associated client entry.
*
* @param    L7_enetMacAddr_t        macAddr  @b{(input)}  client MAC address
* @param    L7_WDM_CLIENT_STATUS_t *status   @b{(output)}  client status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientStatusGet(L7_enetMacAddr_t        macAddr,
                                     L7_WDM_CLIENT_STATUS_t *status);

/*********************************************************************
*
* @purpose  Get Client Local status for an associated client entry.
*
* @param    L7_enetMacAddr_t        macAddr  @b{(input)}  client MAC address
* @param    L7_BOOL                 *clientLocal   @b{(output)} pointer to is Client Local
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientLocalGet (L7_enetMacAddr_t macAddr,
                            L7_BOOL *clientLocal);

/*********************************************************************
*
* @purpose  Get Switch IP Address for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr  @b{(input)} client MAC address
* @param    L7_IP_ADDR_t     *switchIPAddr @b{(output)} pointer to Switch IP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientSwitchIPAddrGet (L7_enetMacAddr_t macAddr,
                            L7_IP_ADDR_t *switchIPAddr);

/*********************************************************************
*
* @purpose  Get Switch MAC Address for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr  @b{(input)} client MAC address
* @param    L7_enetMacAddr_t *switchMacAddr @b{(output)} pointer to Switch MAC
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientSwitchMacAddrGet (L7_enetMacAddr_t macAddr,
                            L7_enetMacAddr_t *switchMacAddr);

/*********************************************************************
*
* @purpose  Get tunnel IP address for an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)}  client MAC address
* @param    L7_IP_ADDR_t     *ip             @b{(output)}  client IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments for non-tunneled clients IP will always be 0.0.0.0, this
*             should be shown as Unknown via CLI/Web.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientTunnelIPAddrGet(L7_enetMacAddr_t       macAddr,
                                           L7_IP_ADDR_t          *ip);

/*********************************************************************
*
* @purpose  Get VAP MAC address for an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)}  client MAC address
* @param    L7_enetMacAddr_t *vapMacAddr     @b{(output)}  VAP to which client is connected.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientVAPMacAddrGet(L7_enetMacAddr_t       macAddr,
                                         L7_enetMacAddr_t      *vapMacAddr);

/*********************************************************************
*
* @purpose  Get base AP MAC address and radio interface for connected VAP.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)}   client MAC address
* @param    L7_enetMacAddr_t *apMacAddr     @b{(output)}  managed AP base MAC address.
* @param    L7_uchar8        *apRadioIf     @b{(output)}  managed AP radio interface.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This provides the key to a managed AP radio entry.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientAPMacAddrRadioGet(L7_enetMacAddr_t macAddr,
                                             L7_enetMacAddr_t *apMacAddr,
                                             L7_uchar8        *apRadioIf);

/*********************************************************************
*
* @purpose  Get SSID for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr        @b{(input)}  client MAC address
* @param    L7_char8        *ssid           @b{(output)}  network to which client is connected.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientSSIDGet(L7_enetMacAddr_t       macAddr,
                                   L7_char8              *ssid);

/*********************************************************************
*
* @purpose  Get channel for an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)}  client MAC address
* @param    L7_uchar8        *channel        @b{(output)}  client channel.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientChannelGet(L7_enetMacAddr_t       macAddr,
                                      L7_uchar8             *channel);

/*********************************************************************
*
* @purpose  Get transmit rate for an associated client.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)}  client MAC address
* @param    L7_ushort16      *rate      @b{(output)}  client transmit rate, multiple of 100kbps.
* @param    L7_ushort16      *mod       @b{(output)}  mod value to handle floating numbers for UI.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientRateGet(L7_enetMacAddr_t       macAddr,
                                   L7_ushort16           *rate, 
                                   L7_ushort16           *mod);

/*********************************************************************
*
* @purpose  Get 802.11n Capable for an Associated Client.
*
* @param    L7_enetMacAddr_t  macAddr @b{(input)} Client MAC Address
* @param    L7_uint32        *dot11nCapable @b{(output)} 802.11n Capable
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientDot11nCapableGet (L7_enetMacAddr_t macAddr,
                            L7_uint32 *dot11nCapable);

/*********************************************************************
*
* @purpose  Get STBC Capable for an associated client.
*
* @param    L7_enetMacAddr_t  macAddr @b{(input)} Client MAC Address
* @param    L7_uint32        *stbcCapable @b{(output)} STBC Capable
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientStbcCapableGet (L7_enetMacAddr_t macAddr,
                                           L7_uint32 *stbcCapable);

/*********************************************************************
*
* @purpose  Get IP Address for an associated client.
*
* @param    L7_enetMacAddr_t  macAddr @b{(input)} Client MAC Address
* @param    L7_IP_ADDR_t     *ipAddr @b{(output)} IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientIPAddrGet (L7_enetMacAddr_t macAddr,
                            L7_IP_ADDR_t *ipAddr);

/*********************************************************************
*
* @purpose  Get 802.1x username for an associated client.
*
* @param    L7_enetMacAddr_t macAddr        @b{(input)}  client MAC address
* @param    L7_char8        *username       @b{(output)}  802.1x User
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientUserNameGet(L7_enetMacAddr_t       macAddr,
                                       L7_char8              *username);

/*********************************************************************
*
* @purpose  Get VLAN assigned to an associated client.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)}  client MAC address
* @param    L7_ushort16      *vlanId         @b{(output)}  assigned 802.1x VLAN (or default)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientVlanIDGet(L7_enetMacAddr_t       macAddr,
                                     L7_ushort16           *vlanId);

/*********************************************************************
*
* @purpose  Get inactive period for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *time         @b{(output)}  inactive period in milliseconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientInactivePeriodGet(L7_enetMacAddr_t  macAddr,
                                             L7_uint32        *time);

/*********************************************************************
*
* @purpose  Get age for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *age          @b{(output)}  seconds since last update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientAgeGet(L7_enetMacAddr_t  macAddr,
                                  L7_uint32        *age);

/*********************************************************************
*
* @purpose  Get network time for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *nwtime          @b{(output)}  seconds since first authenticated
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientNwTimeGet(L7_enetMacAddr_t  macAddr,
                                  L7_uint32        *nwtime);

/*********************************************************************
*
* @purpose  Get roam indicator for an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)}  client MAC address
* @param    L7_BOOL          *val            @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments A client is considered to have roamed if the client's association
*           database entry exists at the time of association/reassociation.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientRoamIndicatorGet(L7_enetMacAddr_t       macAddr,
                                            L7_BOOL               *val);



/*********************************************************************
*
* @purpose  Get RRM support indicator for an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)}  client MAC address
* @param    L7_BOOL          *supp     @b{(output)} pointer to output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Returns in output parm if client is known to support
*           the Radio Resource Measurement (RRM) portion of 802.11k.
*
* @end
*
*********************************************************************/
L7_RC_t 
usmDbWdmAssocClientRRMSupportedGet(L7_enetMacAddr_t macAddr,
                                   L7_BOOL          *val);



/*********************************************************************
*
* @purpose  Get RRM location config support indicator for 
*           an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)}  client MAC address
* @param    L7_BOOL          *supp     @b{(output)} pointer to output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Returns in output parm if client is known to support
*           location configuration reports within 
*           the Radio Resource Measurement (RRM) portion of 802.11k.
*
* @end
*
*********************************************************************/

L7_RC_t 
usmDbWdmAssocClientRRMLocationReportSupportedGet(L7_enetMacAddr_t macAddr,
                                                 L7_BOOL          *val);



/*********************************************************************
*
* @purpose  Get RRM Beacon Table Measurement support indicator for 
*           an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)}  client MAC address
* @param    L7_BOOL          *supp     @b{(output)} pointer to output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Returns in output parm if client is known to support
*           Beacon Table Measurement within 
*           the Radio Resource Measurement (RRM) portion of 802.11k.
*
* @end
*
*********************************************************************/
L7_RC_t 
usmDbWdmAssocClientRRMBeaconTableMeasurementSupportedGet(L7_enetMacAddr_t  macAddr,
                                                         L7_BOOL           *val);


/*********************************************************************
*
* @purpose  Get RRM beacon active measurement support indicator for 
*           an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)}  client MAC address
* @param    L7_BOOL          *supp     @b{(output)} pointer to output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Returns in output parm if client is known to support
*           beacon active measurement within 
*           the Radio Resource Measurement (RRM) portion of 802.11k.
*
* @end
*
*********************************************************************/
L7_RC_t 
usmDbWdmAssocClientRRMBeaconActiveMeasurementSupportedGet(L7_enetMacAddr_t macAddr,
                                                          L7_BOOL          *val);


/*********************************************************************
*
* @purpose  Get RRM beacon passive measurement support indicator for 
*           an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)}  client MAC address
* @param    L7_BOOL          *supp     @b{(output)} pointer to output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Returns in output parm if client is known to support
*           beacon passive measurement within 
*           the Radio Resource Measurement (RRM) portion of 802.11k.
*
* @end
*
*********************************************************************/
L7_RC_t 
usmDbWdmAssocClientRRMBeaconPassiveMeasurementSupportedGet(L7_enetMacAddr_t  macAddr,
                                                           L7_BOOL           *val);


/*********************************************************************
*
* @purpose  Get RRM channel load measurement support indicator for 
*           an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)}  client MAC address
* @param    L7_BOOL          *supp     @b{(output)} pointer to output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Returns in output parm if client is known to support
*           channel load measurement within 
*           the Radio Resource Measurement (RRM) portion of 802.11k.
*
* @end
*
*********************************************************************/
L7_RC_t 
usmDbWdmAssocClientRRMChannelLoadMeasurementSupportedGet(L7_enetMacAddr_t  macAddr,
                                                         L7_BOOL           *val);

/*********************************************************************
*
* @purpose  Check if specified client TSPEC traffic stream entry exists for this TID.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} client MAC address
* @param    L7_uint32        tid     @b{(input)} traffic stream identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Do not return entries with L7_WDM_CLIENT_NONE as these are
*           not exposed to the user.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientTsEntryGet(L7_enetMacAddr_t macAddr, L7_uint32 tid);

/*********************************************************************
*
* @purpose  Get next client TSPEC traffic stream by TID from the associated client database.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)}  client MAC address
* @param    L7_uint32         tid         @b{(input)}  traffic stream identifier
* @param    L7_enetMacAddr_t *nextMacAddr @b{(output)} next client MAC address
* @param    L7_uint32        *nextTid     @b{(output)} next traffic stream identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments macAddr 00:00:00:00:00:00 returns first entry.
*
* @comments If client macAddr exists, check if there is a next TID for that client.
*           Otherwise, look for first TID on the next client MAC address.
*
* @comments Do not return entries with L7_WDM_CLIENT_NONE as these are
*           not exposed to the user.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientTsEntryNextGet(L7_enetMacAddr_t macAddr, L7_uint32 tid,
                                          L7_enetMacAddr_t *nextMacAddr, L7_uint32 *nextTid);

/*********************************************************************
*
* @purpose  Determine if client has any TSPEC traffic streams.
*
* @param    L7_enetMacAddr_t macAddr  @b{(input)}  client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientTsAnyExist(L7_enetMacAddr_t  macAddr);

/*********************************************************************
*
* @purpose  Determine if client traffic stream exists for specified TID.
*
* @param    L7_enetMacAddr_t macAddr  @b{(input)}  client MAC address
* @param    L7_uint32        tid      @b{(input)}  traffic stream identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientTsTidValid(L7_enetMacAddr_t  macAddr,
                                      L7_uint32 tid);

/*********************************************************************
*
* @purpose  Determine if client traffic stream is active for specified TID.
*
* @param    L7_enetMacAddr_t macAddr  @b{(input)}  client MAC address
* @param    L7_uint32        tid      @b{(input)}  traffic stream identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This indicates a valid client TS that has a set of TSPEC
*           parameters applied to it.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientTsTidActive(L7_enetMacAddr_t  macAddr,
                                       L7_uint32 tid);

/*********************************************************************
*
* @purpose  Determine if any client traffic stream exists for specified access category.
*
* @param    L7_enetMacAddr_t macAddr  @b{(input)}  client MAC address
* @param    L7_uint32        acindex  @b{(input)}  access category index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientTsAcValid(L7_enetMacAddr_t  macAddr,
                                     L7_uint32 acindex);

/*********************************************************************
*
* @purpose  Determine if any client traffic stream is active for specified access category.
*
* @param    L7_enetMacAddr_t macAddr  @b{(input)}  client MAC address
* @param    L7_uint32        acindex  @b{(input)}  access category index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This indicates a valid client TS that has a set of TSPEC
*           parameters applied to it.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientTsAcActive(L7_enetMacAddr_t  macAddr,
                                      L7_uint32 acindex);

/*********************************************************************
*
* @purpose  Get access category for an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)}  client MAC address
* @param    L7_uint32         tid            @b{(input)}  traffic stream identifier
* @param    L7_uchar8        *val            @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientTsAccessCategoryGet(L7_enetMacAddr_t       macAddr,
                                               L7_uint32              tid,
                                               L7_uchar8             *val);

/*********************************************************************
*
* @purpose  Get traffic stream direction for an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)}  client MAC address
* @param    L7_uint32         tid            @b{(input)}  traffic stream identifier
* @param    L7_uchar8        *val            @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientTsDirectionGet(L7_enetMacAddr_t       macAddr,
                                          L7_uint32              tid,
                                          L7_uchar8             *val);

/*********************************************************************
*
* @purpose  Get traffic stream power save behavior (PSB) for an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)}  client MAC address
* @param    L7_uint32         tid            @b{(input)}  traffic stream identifier
* @param    L7_uchar8        *val            @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientTsPsbGet(L7_enetMacAddr_t       macAddr,
                                    L7_uint32              tid,
                                    L7_uchar8             *val);

/*********************************************************************
*
* @purpose  Get traffic stream user priority for an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)}  client MAC address
* @param    L7_uint32         tid            @b{(input)}  traffic stream identifier
* @param    L7_uchar8        *val            @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientTsUserPriorityGet(L7_enetMacAddr_t       macAddr,
                                             L7_uint32              tid,
                                             L7_uchar8             *val);

/*********************************************************************
*
* @purpose  Get traffic stream medium time for an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)}  client MAC address
* @param    L7_uint32         tid            @b{(input)}  traffic stream identifier
* @param    L7_uchar8        *val            @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientTsMediumTimeGet(L7_enetMacAddr_t       macAddr,
                                           L7_uint32              tid,
                                           L7_ushort16           *val);

/*********************************************************************
*
* @purpose  Get traffic stream roam indicator for an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)}  client MAC address
* @param    L7_uint32         tid            @b{(input)}  traffic stream identifier
* @param    L7_BOOL          *val            @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments A client is considered to have roamed if the client's association
*           database entry exists at the time of association/reassociation.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientTsRoamIndicatorGet(L7_enetMacAddr_t       macAddr,
                                               L7_uint32             tid,
                                               L7_BOOL               *val);

/*********************************************************************
*
* @purpose  Get authentication client QoS operational status for 
*           associated client.
*
* @param    L7_enetMacAddr_t   macAddr  @b{(input)}  client MAC address
* @param    L7_uint32          *stat  @b{(output)} client QoS operational status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientQosOperStatusGet(L7_enetMacAddr_t     macAddr,
                                            L7_uint32           *stat);

/*********************************************************************
*
* @purpose  Get authenticated client QoS ACL for associated client 
*           in the specified direction.
*
* @param    L7_enetMacAddr_t                macAddr   @b{(input)}  client MAC address
* @param    L7_WDM_CLIENT_QOS_PARAM_TYPE_t  type      @b{(input)}  status parameter type
* @param    L7_WDM_CLIENT_QOS_DIR_t         dir       @b{(input)}  direction 
* @param    wdmClientQosAcl_t              *acl       @b{(output)} ACL struct ptr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Caller must supply an aclName output buffer of at least
*           L7_ACL_NAME_LEN_MAX+1 bytes in length.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientQosAclGet(L7_enetMacAddr_t               macAddr,
                                     L7_WDM_CLIENT_QOS_PARAM_TYPE_t type,
                                     L7_WDM_CLIENT_QOS_DIR_t        dir,
                                     wdmClientQosAcl_t             *acl);

/*********************************************************************
*
* @purpose  Get authenticated client QoS bandwidth maximum rate for associated client 
*           in the specified direction.
*
* @param    L7_enetMacAddr_t                macAddr   @b{(input)}  client MAC address
* @param    L7_WDM_CLIENT_QOS_PARAM_TYPE_t  type      @b{(input)}  status parameter type
* @param    L7_WDM_CLIENT_QOS_DIR_t         dir       @b{(input)}  direction 
* @param    wdmClientQosBw_t               *bw        @b{(output)} bandwidth struct ptr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments A maxRate value of L7_WDM_NETWORK_CLIENT_QOS_BW_NONE means
*           rate limiting is turned off in the specified direction for this
*           network.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientQosBandwidthGet(L7_enetMacAddr_t               macAddr,
                                           L7_WDM_CLIENT_QOS_PARAM_TYPE_t type,
                                           L7_WDM_CLIENT_QOS_DIR_t        dir,
                                           wdmClientQosBw_t              *bw);

/*********************************************************************
*
* @purpose  Get authenticated client QoS DiffServ policy for associated client 
*           in the specified direction.
*
* @param    L7_enetMacAddr_t                   macAddr     @b{(input)}  client MAC address
* @param    L7_WDM_CLIENT_QOS_PARAM_TYPE_t     type        @b{(input)}  status parameter type
* @param    L7_WDM_CLIENT_QOS_DIR_t            dir         @b{(input)}  direction 
* @param    wdmClientQosDs_t                  *policy      @b{(output)} Diffserv Policy struct ptr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Caller must supply a policyName output buffer of at least
*           L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX+1 bytes in length.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientQosPolicyGet(L7_enetMacAddr_t                     macAddr,
                                        L7_WDM_CLIENT_QOS_PARAM_TYPE_t       type,
                                        L7_WDM_CLIENT_QOS_DIR_t              dir,
                                        wdmClientQosDs_t                    *policy);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint64       *pkts         @b{(output)} packets received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientSessionStatisticsPktsRxGet(L7_enetMacAddr_t  macAddr,
                                                      L7_uint64        *pkts);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint64       *bytes        @b{(output)} bytes received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientSessionStatisticsBytesRxGet(L7_enetMacAddr_t  macAddr,
                                                       L7_uint64        *bytes);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint64       *pkts         @b{(output)} packets transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientSessionStatisticsPktsTxGet(L7_enetMacAddr_t  macAddr,
                                                      L7_uint64        *pkts);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint64       *bytes        @b{(output)} bytes transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientSessionStatisticsBytesTxGet(L7_enetMacAddr_t  macAddr,
                                                       L7_uint64        *bytes);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint64       *pkts         @b{(output)} dropped receive packets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientSessionStatisticsDropPktsRxGet(L7_enetMacAddr_t  macAddr,
                                                          L7_uint64        *pkts);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint64       *bytes        @b{(output)} dropped receive bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientSessionStatisticsDropBytesRxGet(L7_enetMacAddr_t  macAddr,
                                                           L7_uint64        *bytes);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint64       *pkts         @b{(output)} drop transmit packets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientSessionStatisticsDropPktsTxGet(L7_enetMacAddr_t  macAddr,
                                                          L7_uint64        *pkts);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint64       *bytes        @b{(output)} drop transmit bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientSessionStatisticsDropBytesTxGet(L7_enetMacAddr_t  macAddr,
                                                           L7_uint64        *bytes);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *pkts         @b{(output)} duplicate packets received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientSessionStatisticsDupPktsRxGet(L7_enetMacAddr_t  macAddr,
                                                         L7_uint32        *pkts);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *pkts         @b{(output)} fragmented packets received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientSessionStatisticsFragPktsRxGet(L7_enetMacAddr_t  macAddr,
                                                          L7_uint32        *pkts);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *pkts         @b{(output)} fragmented packets transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientSessionStatisticsFragPktsTxGet(L7_enetMacAddr_t  macAddr,
                                                          L7_uint32        *pkts);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *count        @b{(output)} transmit retry count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientSessionStatisticsTxRetryGet(L7_enetMacAddr_t  macAddr,
                                                       L7_uint32        *count);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *count        @b{(output)} transmit retry failed count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientSessionStatisticsTxRetryFailedGet(L7_enetMacAddr_t  macAddr,
                                                             L7_uint32        *count);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *count        @b{(output)} Tspec violate Rx pkt count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientSessionStatisticsTsViolatePktsRxGet(L7_enetMacAddr_t  macAddr,
                                                               L7_uint32        *count);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *count        @b{(output)} TSpec violate Tx pkt count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientSessionStatisticsTsViolatePktsTxGet(L7_enetMacAddr_t  macAddr,
                                                               L7_uint32        *count);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32        acindex      @b{(input)}  TSPEC AC index
* @param    L7_uint64       *count        @b{(output)} TSpec Rx pkt count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientSessionStatisticsTsPktsRxGet(L7_enetMacAddr_t  macAddr,
                                                        L7_uint32  acindex,
                                                        L7_uint64 *count);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32        acindex      @b{(input)}  TSPEC AC index
* @param    L7_uint64       *count        @b{(output)} TSpec Tx pkt count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientSessionStatisticsTsPktsTxGet(L7_enetMacAddr_t  macAddr,
                                                        L7_uint32  acindex,
                                                        L7_uint64 *count);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32        acindex      @b{(input)}  TSPEC AC index
* @param    L7_uint64       *count        @b{(output)} TSpec Rx byte count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientSessionStatisticsTsBytesRxGet(L7_enetMacAddr_t  macAddr,
                                                        L7_uint32  acindex,
                                                        L7_uint64 *count);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32        acindex      @b{(input)}  TSPEC AC index
* @param    L7_uint64       *count        @b{(output)} TSpec Tx byte count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientSessionStatisticsTsBytesTxGet(L7_enetMacAddr_t  macAddr,
                                                        L7_uint32  acindex,
                                                        L7_uint64 *count);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint64       *pkts         @b{(output)} packets received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientStatisticsPktsRxGet(L7_enetMacAddr_t  macAddr,
                                               L7_uint64        *pkts);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint64       *bytes        @b{(output)} bytes received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientStatisticsBytesRxGet(L7_enetMacAddr_t  macAddr,
                                                L7_uint64        *bytes);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint64       *pkts         @b{(output)} packets transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientStatisticsPktsTxGet(L7_enetMacAddr_t  macAddr,
                                               L7_uint64        *pkts);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint64       *bytes        @b{(output)} bytes transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientStatisticsBytesTxGet(L7_enetMacAddr_t  macAddr,
                                                L7_uint64        *bytes);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint64       *pkts         @b{(output)} dropped receive packets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientStatisticsDropPktsRxGet(L7_enetMacAddr_t  macAddr,
                                                   L7_uint64        *pkts);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint64       *bytes        @b{(output)} dropped receive bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientStatisticsDropBytesRxGet(L7_enetMacAddr_t  macAddr,
                                                    L7_uint64        *bytes);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint64       *pkts         @b{(output)} dropped transmit packets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientStatisticsDropPktsTxGet(L7_enetMacAddr_t  macAddr,
                                                   L7_uint64        *pkts);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint64       *bytes        @b{(output)} drop transmit bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientStatisticsDropBytesTxGet(L7_enetMacAddr_t  macAddr,
                                                    L7_uint64        *bytes);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *pkts         @b{(output)} duplicate packets received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientStatisticsDupPktsRxGet(L7_enetMacAddr_t  macAddr,
                                                  L7_uint32        *pkts);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *pkts         @b{(output)} fragmented packets received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientStatisticsFragPktsRxGet(L7_enetMacAddr_t  macAddr,
                                                   L7_uint32        *pkts);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *pkts         @b{(output)} fragmented packets transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientStatisticsFragPktsTxGet(L7_enetMacAddr_t  macAddr,
                                                   L7_uint32        *pkts);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *count        @b{(output)} transmit retry count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientStatisticsTxRetryGet(L7_enetMacAddr_t  macAddr,
                                                L7_uint32        *count);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *count        @b{(output)} transmit retry failed count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientStatisticsTxRetryFailedGet(L7_enetMacAddr_t  macAddr,
                                                      L7_uint32        *count);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *count        @b{(output)} TSpec Violate Rx pkts
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientStatisticsTsViolatePktsRxGet(L7_enetMacAddr_t  macAddr,
                                                        L7_uint32        *count);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *count        @b{(output)} TSpec Violate Tx pkts
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientStatisticsTsViolatePktsTxGet(L7_enetMacAddr_t  macAddr,
                                                        L7_uint32        *count);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32        tid          @b{(input)}  traffic stream identifier
* @param    L7_uint64       *count        @b{(output)} TSpec Rx pkt count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientStatisticsTsPktsRxGet(L7_enetMacAddr_t  macAddr,
                                                 L7_uint32  tid,
                                                 L7_uint64 *count);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32        tid          @b{(input)}  traffic stream identifier
* @param    L7_uint64       *count        @b{(output)} TSpec Tx pkt count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientStatisticsTsPktsTxGet(L7_enetMacAddr_t  macAddr,
                                                 L7_uint32  tid,
                                                 L7_uint64 *count);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32        tid          @b{(input)}  traffic stream identifier
* @param    L7_uint64       *count        @b{(output)} TSpec Rx byte count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientStatisticsTsBytesRxGet(L7_enetMacAddr_t  macAddr,
                                                  L7_uint32  tid,
                                                  L7_uint64 *count);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32        tid          @b{(input)}  traffic stream identifier
* @param    L7_uint64       *count        @b{(output)} TSpec Tx byte count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientStatisticsTsBytesTxGet(L7_enetMacAddr_t  macAddr,
                                                  L7_uint32  tid,
                                                  L7_uint64 *count);

/*********************************************************************
*
* @purpose  Get TSPEC excess usage event statistic for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32        tid          @b{(input)}  traffic stream identifier
* @param    L7_uint32       *count        @b{(output)} current counter value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Represents the number of events that have not been reported
*           via the Trap Manager.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientStatisticsTsExcessUsageEventsGet(L7_enetMacAddr_t macAddr,
                                                            L7_uint32 tid,
                                                            L7_uint32 *count);

/*********************************************************************
*
* @purpose  Get Captive Portal Authentication flag for given client
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} client MAC address
* @param    L7_uint32 *cpAuthFlag @b{(output)} client CP Auth flag value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientCPAuthFlagGet(L7_enetMacAddr_t macAddr,
                                         L7_uint32 *cpAuthFlag);

/*********************************************************************
*
* @purpose  Determine if an entry exists in the Ad Hoc client network list.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)}  client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAdHocClientEntryGet(L7_enetMacAddr_t  macAddr);

/*********************************************************************
*
* @purpose  Get next entry in the Ad Hoc client network list.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)}  client MAC address
* @param    L7_enetMacAddr_t *nextMacAddr    @b{(output)} next client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbwdmAdHocClientEntryNextGet(L7_enetMacAddr_t   macAddr,
                                        L7_enetMacAddr_t  *nextMacAddr);

/*********************************************************************
*
* @purpose  Get last reporting AP MAC and radio If for an entry in the
* @purpose  Ad Hoc client network list.
*
* @param    L7_enetMacAddr_t  macAddr    @b{(input)} client MAC address
* @param    L7_enetMacAddr_t  *apMacAddr  @b{(output)} managed AP MAC address
* @param    L7_uchar8         *apRadioIf  @b{(output)} managed AP radio interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAdHocClientAPMACRadioGet(L7_enetMacAddr_t macAddr,
                                       L7_enetMacAddr_t *apMacAddr,
                                       L7_uchar8 *apRadioIf);

/*********************************************************************
*
* @purpose  Get detection mode for an entry in the Ad Hoc client network list.
*
* @param    L7_enetMacAddr_t  macAddr    @b{(input)} client MAC address
* @param    L7_WDM_ADHOC_CLIENT_DETECTION_MODE_t *mode @b{(output)} pointer to mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAdHocClientDetectionModeGet(L7_enetMacAddr_t macAddr, 
                                            L7_WDM_ADHOC_CLIENT_DETECTION_MODE_t *mode);

/*********************************************************************
*
* @purpose  Get age for an entry in the Ad Hoc client network list.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)}  client MAC address
* @param    L7_uint32       *age          @b{(output)}  seconds since last update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAdHocClientAgeGet(L7_enetMacAddr_t  macAddr,
                                  L7_uint32        *age);

/*********************************************************************
*
* @purpose  Purge all entries in the Ad Hoc client network list.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAdHocClientEntriesPurge();

/*********************************************************************
*
* @purpose  Determine if an entry exists SSID-Client mapping.
*
* @param    L7_char8         *ssid      @b{(input)}  network SSID
* @param    L7_enetMacAddr_t  macAddr   @b{(input)}  associated client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  entries are added to the SSID-Client mapping, when an 
*              entry is added to the associated client database, and
*            are removed when entry is removed from the associated client database.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSSIDAssocClientEntryGet(L7_char8          *ssid,
                                        L7_enetMacAddr_t   macAddr);

/*********************************************************************
*
* @purpose  Get next entry in the SSID-Client mapping.
*
* @param    L7_char8         *ssid        @b{(input)}  network SSID
* @param    L7_enetMacAddr_t  macAddr     @b{(input)}  associated client MAC address
* @param    L7_char8         *nextSsid    @b{(output)}  next network SSID
* @param    L7_enetMacAddr_t *nextMacAddr @b{(output)}  next associated client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  entries are added to the SSID-Client mapping, when an 
*              entry is added to the associated client database, and
*            are removed when entry is removed from the associated client database.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSSIDAssocClientEntryNextGet(L7_char8          *ssid,
                                            L7_enetMacAddr_t   macAddr,
                                            L7_char8          *nextSsid,
                                            L7_enetMacAddr_t  *nextMacAddr);

/*********************************************************************
*
* @purpose  Determine if an entry exists VAP-Client mapping.
*
* @param    L7_enetMacAddr_t vapMacAddr     @b{(input)}  VAP MAC address
* @param    L7_enetMacAddr_t clientMacAddr  @b{(input)}  associated client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  entries are added to the VAP-Client mapping, when an 
*              entry is added to the associated client database, and
*            are removed when entry is removed from the associated client database.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmVAPAssocClientEntryGet(L7_enetMacAddr_t  vapMacAddr,
                                       L7_enetMacAddr_t  clientMacAddr);

/*********************************************************************
*
* @purpose  Get next entry in the VAP-Client mapping.
*
* @param    L7_enetMacAddr_t vapMacAddr     @b{(input)}  VAP MAC address
* @param    L7_enetMacAddr_t clientMacAddr  @b{(input)}  associated client MAC address
* @param    L7_enetMacAddr_t *nextVAPMacAddr     @b{(output)}  next VAP MAC address
* @param    L7_enetMacAddr_t *nextClientMacAddr  @b{(output)}  next associated client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  entries are added to the VAP-Client mapping, when an 
*              entry is added to the associated client database, and
*            are removed when entry is removed from the associated client database.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmVAPAssocClientEntryNextGet(L7_enetMacAddr_t   vapMacAddr,
                                           L7_enetMacAddr_t   clientMacAddr,
                                           L7_enetMacAddr_t  *nextVAPMacAddr,
                                           L7_enetMacAddr_t  *nextClientMacAddr);

/*********************************************************************
*
* @purpose  Determine if an entry exists switch-associated client mapping.
*
* @param    L7_IP_ADDR_t switchIPAddr     @b{(input)}  switch IP address
* @param    L7_enetMacAddr_t clientMacAddr  @b{(input)}  associated client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  entries are added to the switch-associated client mapping, when an
*              entry is added to the associated client database, and
*            are removed when entry is removed from the associated client database.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchAssocClientEntryGet(L7_IP_ADDR_t switchIPAddr,
                                       L7_enetMacAddr_t  clientMacAddr);


/*********************************************************************
*
* @purpose  Get next entry in the switch-associated client mapping.
*
* @param    L7_IP_ADDR_t *switchIPAddr     @b{(input)}  switch's IP address
* @param    L7_enetMacAddr_t clientMacAddr  @b{(input)}  associated client MAC address
* @param    L7_IP_ADDR_t *nextSwitchIPAddr     @b{(output)}  next switch's IP address
* @param    L7_enetMacAddr_t *nextClientMacAddr  @b{(output)}  next associated client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  entries are added to the switch-associated client mapping, when an
*              entry is added to the associated client database, and
*            are removed when entry is removed from the associated client database.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmSwitchAssocClientEntryNextGet(L7_IP_ADDR_t switchIPAddr,
                                           L7_enetMacAddr_t   clientMacAddr,
                                           L7_IP_ADDR_t *nextSwitchIPAddr,
                                           L7_enetMacAddr_t  *nextClientMacAddr);

/*********************************************************************
*
* @purpose  Determine if an entry exists client - managed AP neighbor mapping.
*
* @param    L7_enetMacAddr_t macAddr    @b{(input)}  client MAC address
* @param    L7_enetMacAddr_t apMacAddr  @b{(input)}  managed AP MAC address
* @param    L7_enetMacAddr_t apRadioIf  @b{(input)}  managed AP radio interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  entries are added to the Client-AP Neighbor mapping, when an 
*              entry is added managed AP client neighbor list, and removed
*              when an entry is aged or deleted.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmClientManagedAPNbrEntryGet(L7_enetMacAddr_t  macAddr,
                                           L7_enetMacAddr_t  apMacAddr,
                                           L7_uchar8         apRadioIf);

/*********************************************************************
*
* @purpose  Determine if an entry exists client - managed AP neighbor mapping.
*
* @param    L7_enetMacAddr_t macAddr    @b{(input)}  client MAC address
* @param    L7_enetMacAddr_t apMacAddr  @b{(input)}  managed AP MAC address
* @param    L7_enetMacAddr_t apRadioIf  @b{(input)}  managed AP radio interface
* @param    L7_enetMacAddr_t nextMacAddr    @b{(output)}  next client MAC address
* @param    L7_enetMacAddr_t nextAPMacAddr  @b{(output)}  next managed AP MAC address
* @param    L7_enetMacAddr_t nextAPRadioIf  @b{(output)}  next managed AP radio interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  entries are added to the Client-AP Neighbor mapping, when an
*              entry is added managed AP client neighbor list, and removed
*              when an entry is aged or deleted.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmClientManagedAPNbrEntryNextGet(L7_enetMacAddr_t   macAddr,
                                               L7_enetMacAddr_t   apMacAddr,
                                               L7_uchar8          apRadioIf,
                                               L7_enetMacAddr_t  *nextMacAddr,
                                               L7_enetMacAddr_t  *nextAPMacAddr,
                                               L7_uchar8         *nextAPRadioIf);
/*********************************************************************
*
* @purpose  Get L2 Distributed Tunneling for an associated client
*           entry.
*
* @param    L7_enetMacAddr_t macAddr  @b{(input)} client MAC address
* @param    L7_uint32       *status   @b{(output)} pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientDistTunnelStatusGet (L7_enetMacAddr_t macAddr,
                                                L7_uint32 *status);
/*********************************************************************
*
* @purpose  Get L2 Distributed Tunneling roaming status for an
*           associated client entry.
*
* @param    L7_enetMacAddr_t macAddr  @b{(input)} client MAC address
* @param    L7_uint32       *status   @b{(output)} pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientDistTunnelRoamStatusGet (L7_enetMacAddr_t macAddr,
                                                    L7_uint32 *status);
/*********************************************************************
*
* @purpose  Get Home AP MAC Address for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr    @b{(input)} client MAC address
* @param    L7_enetMacAddr_t *apMacAddr @b{(output)} pointer to AP MAC
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientDistTunnelHomeAPMacGet (L7_enetMacAddr_t macAddr,
                                                   L7_enetMacAddr_t *apMacAddr);
/*********************************************************************
*
* @purpose  Get Association AP MAC Address for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr    @b{(input)} client MAC address
* @param    L7_enetMacAddr_t *apMacAddr @b{(output)} pointer to AP MAC
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientDistTunnelAssocAPMacGet (L7_enetMacAddr_t macAddr,
                                                    L7_enetMacAddr_t *apMacAddr);

/*********************************************************************
*
* @purpose  Get the NetBios Name for an associated client.
*
* @param    L7_enetMacAddr_t macAddr        @b{(input)}  client MAC address
* @param    L7_uchar8        *nbName        @b{(input)}  netbios name
*                                                        L7_WIRELESS_CLIENT_NETBIOS_NAME_LENGTH bytes.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmAssocClientNetBiosNameGet (L7_enetMacAddr_t macAddr,
                                                    L7_uchar8 *nbName);

#endif /* INCLUDE_USMDB_WDM_CLIENT_API_H */

