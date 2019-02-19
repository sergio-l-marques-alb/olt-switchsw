/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
*
* @filename   wdm_ap_api.h
*
* @purpose    Wireless Data Manager Access Point (WDM AP) API header
*
* @component  WDM
*
* @comments   none
*
* @create     1/15/2006
*
* @author     rjindal
*
* @end
*
*********************************************************************/

#ifndef INCLUDE_WDM_AP_API_H
#define INCLUDE_WDM_AP_API_H

#include "datatypes.h"
#include "osapi.h"
#include "wireless_commdefs.h"
#include "wireless_comm_structs.h"
#include "captive_portal_commdefs.h"
#include "intf_cb_api.h"
/* Following APIs can be called via usmDb and wireless application components. */

#define WS_WIDS_CONFIG_MAX_MSG_LEN \
            ((WS_MSG_HDR_SIZE) + \
             (WS_IE_HDR_SIZE) + (WS_IE_AP_MAC_ADDRESS_SIZE) + \
             (WS_IE_HDR_SIZE) + (WS_IE_WIDS_WIRED_DETECTION_MODE_SIZE) + \
             (WS_IE_HDR_SIZE) + (WS_IE_WIDS_WIRED_DETECTION_INTERVAL_SIZE) + \
             (WS_IE_HDR_SIZE) + (WS_IE_WIDS_AP_ATTACK_INTERVAL_SIZE) + \
             (WS_IE_HDR_SIZE) + (WS_IE_WIDS_ATTACK_LIST_COUNT_SIZE) + \
             (WS_IE_HDR_SIZE) + (WS_IE_WIDS_ATTACK_LIST_ENTRY_SIZE * WDM_AP_DE_AUTHENTICATION_ATTACK_MAX))

typedef enum
{
  WDM_WIDS_CONFIG_MSG_ALL_FIELDS = 0,
  WDM_WIDS_CONFIG_MSG_WIRED_DETECTION,
  WDM_WIDS_CONFIG_MSG_ATTACK_LIST,
  WDM_WIDS_CONFIG_MSG_NEW_AP,
  WDM_WIDS_CONFIG_MSG_DISABLE
} WDM_WIDS_CONFIG_MSG_TYPE_t;


typedef struct cpCmdData_s
{
  L7_uint32                 intIfNum;
  L7_uint32                 networkId;
  L7_char8                  ssid[L7_WDM_SSID_MAX_LENGTH+1];
  L7_enetMacAddr_t          apMacAddr;
  L7_uint8                  radioIndex;
  L7_uint8                  vapIndex;
  L7_uint8                  redirIpAddrType;
  L7_IP_ADDR_t              redirIpv4Addr;
  L7_in6_addr_t             redirIpv6Addr;
  L7_uint8                  ipMaskLength;
  L7_char8                  cpIntfMac[L7_ENET_MAC_ADDR_LEN];
  L7_enetMacAddr_t          clientMacAddr;
  L7_uint32                 upRate;
  L7_uint32                 downRate;
  L7_ushort16               authPort;
  L7_ushort16               authSecurePort1;
  L7_ushort16               authSecurePort2;
} cpCmdData_t;

/* TSPEC client summary status */
typedef struct wdmTspecClientSumStatus_s
{
  L7_uchar8                 accessCategory;     /* per L7_WIRELESS_TSPEC_AC_t */
  L7_uchar8                 operStatus;
  L7_uint32                 numActiveTs;
  L7_uint32                 numTsClients;
  L7_uint32                 numTsRoamClients;
  L7_uint32                 medTimeAdmitted;
  L7_uint32                 medTimeUnalloc;
  L7_uint32                 medTimeRoamUnalloc;
} wdmTspecClientSumStatus_t;

/* TSPEC AP TS status */
typedef struct wdmTspecTsSumStatus_s
{
  L7_uchar8                 accessCategory;     /* per L7_WIRELESS_TSPEC_AC_t */
  L7_uint32                 numActiveTs;
  L7_uint32                 numTsClients;
  L7_uint32                 numTsRoamClients;
} wdmTspecTsSumStatus_t;

/* Start per managed AP APIs */

/*********************************************************************
*
* @purpose  Send the WIDS Configuration Message to identified Managed APs.
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*
*********************************************************************/
void wirelessWidsConfigMsgSend(WDM_WIDS_CONFIG_MSG_TYPE_t msgType, L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Check to see if a channel is configured as eligible for
*           a given AP and radio
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uchar8         channel  @b{(input)} input channel
* @param    L7_BOOL        *  pElig    @b{(output)} eligibility
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t
wdmManagedAPRadioChannelEligibleCheck(L7_enetMacAddr_t macAddr,
                                      L7_uchar8 radioIf,
                                      L7_uchar8 channel,
                                      L7_BOOL * pElig);

/*********************************************************************
*
* @purpose  Update a managed AP entry.
*
* @param    L7_enetMacAddr_t      *macAddr    @b{(input)} AP mac address
* @param    L7_IP_ADDR_t          ipAddr      @b{(input)} ip address
* @param    L7_IP_MASK_t          ipMask      @b{(input)} ip subnet mask
* @param    L7_enetMacAddr_t      switchMACAddr @b{(input)} Switch mac address
* @param    L7_IP_ADDR_t          switchIPAddr @b{(input)} switch ip address
* @param    L7_ushort16           vendorId    @b{(input)} vendor id
* @param    L7_ushort16           prtlVersion @b{(input)} prtl version
* @param    L7_char8              *swVersion  @b{(input)} sw version
* @param    L7_uint32             hwType      @b{(input)} hw type
* @param    L7_char8              *serialNum  @b{(input)} serial number
* @param    L7_char8              *partNum    @b{(input)} part number
* @param    L7_WDM_DISC_REASON_t  discReason  @b{(input)} method
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPEntryAddUpdate(L7_enetMacAddr_t macAddr, L7_IP_ADDR_t ipAddr,
                                   L7_IP_MASK_t ipMask,
                                   L7_enetMacAddr_t switchMACAddr,
                                   L7_IP_ADDR_t switchIPAddr,
                                   L7_ushort16 vendorId, L7_ushort16 prtlVersion,
                                   L7_char8 *swVersion, L7_uint32 hwType,
                                   L7_char8 *serialNum, L7_char8 *partNum,
                                   L7_WDM_DISC_REASON_t discReason);

/*********************************************************************
*
* @purpose  Set managed AP parameters learned from local or Radius AP entry.
*
* @param    L7_enetMacAddr_t          macAddr       @b{(input)}  AP MAC Address
* @param    L7_char8                  *location     @b{(input)} configured location
* @param    L7_uint32                 profileId     @b{(input)} configured profile ID
* @param    L7_uchar8                 radio1Power   @b{(input)} fixed power for radio
* @param    L7_uchar8                 radio1Channel @b{(input)} fixed channel for radio
* @param    L7_uchar8                 radio2Power   @b{(input)} fixed power for radio
* @param    L7_uchar8                 radio2Channel @b{(input)} fixed channel for radio
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPValidationParamsSet(L7_enetMacAddr_t macAddr,
                                        L7_char8 *location, L7_uint32 profileId,
                                        L7_uchar8 radio1Power, L7_uchar8 radio1Channel,
                                        L7_uchar8 radio2Power, L7_uchar8 radio2Channel);

/*********************************************************************
*
* @purpose  Get managed AP parameters from local or Radius AP entry.
*
* @param    L7_enetMacAddr_t  macAddr         @b{(input)}  AP MAC Address
* @param    L7_char8          *location       @b{(output)} configured location
* @param    L7_uint32         *profileId      @b{(output)} configured profile ID
* @param    L7_uchar8         *radio1Power    @b{(output)} fixed power for radio
* @param    L7_uchar8         *radio1Channel  @b{(output)} fixed channel for radio
* @param    L7_uchar8         *radio2Power    @b{(output)} fixed power for radio
* @param    L7_uchar8         *radio2Channel  @b{(output)} fixed channel for radio
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPValidationParamsGet(L7_enetMacAddr_t macAddr,
                                        L7_char8 *location, L7_uint32 *profileId,
                                        L7_uchar8 *radio1Power, L7_uchar8 *radio1Channel,
                                        L7_uchar8 *radio2Power, L7_uchar8 *radio2Channel);

/*********************************************************************
*
* @purpose  Read and clear global flag(s) to determine if any managed
*             AP entries have a pending action.
*
* @param    L7_WDM_MANAGED_AP_FLAG_t  flags    @b((input))  type of pending action(s).
* @param    L7_WDM_MANAGED_AP_FLAG_t *setFlags @b((output)) flags set and cleared.
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
L7_RC_t wdmManagedAPPendingFlagsGetClear(L7_WDM_MANAGED_AP_FLAG_t  flags,
                                         L7_WDM_MANAGED_AP_FLAG_t *setFlags);

/*********************************************************************
*
* @purpose  Get the next managed AP entry flagged for a pending action.
*
* @param    L7_enetMacAddr_t            macAddr     @b{(input)}  last AP MAC flagged
* @param    L7_WDM_MANAGED_AP_FLAG_t    flag        @b((input))  type of pending entry to lookup
* @param    L7_enetMacAddr_t           *nextMacAddr @b{(output)} next AP MAC flagged
* @param    L7_WDM_MANAGED_AP_STATUS_t *status      @b{(output)} managed AP entry status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments macAddr 00:00:00:00:00:00 returns first pending entry for the specified flag.
*           If flag is L7_WDM_MANAGED_AP_PEER_NOTIFY and AP status is L7_WDM_MANAGED_AP_DELETED,
*             then the entry for the returned macAddr is also deleted.
*           For all other flag and AP status values, the entry pending flag for the returned macAddr is cleared.
*
*           A managed AP entry is flagged for L7_WDM_MANAGED_AP_PEER_NOTIFY in the following cases:
*           1. The entry status is updated to L7_WDM_MANAGED_AP_AUTHENTICATED.
*           2. The entry status is updated to L7_WDM_MANAGED_AP_DELETED (admin deletes a
*              L7_WDM_MANAGED_AP_FAILED entry, or AP is no longer a valid managed AP).
*
*           A managed AP entry is flagged for L7_WDM_MANAGED_AP_INIT_CHANNEL when
*             the entry status is updated to L7_WDM_MANAGED_AP_MANAGED or when a
*             manual channel adjustment is requested.
*
*           A managed AP entry is flagged for L7_WDM_MANAGED_AP_INIT_STAT_INTERVAL
*             when the entry status is updated to L7_WDM_MANAGED_AP_MANAGED.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPPendingEntryNextGet(L7_enetMacAddr_t            macAddr,
                                        L7_WDM_MANAGED_AP_FLAG_t    flag,
                                        L7_enetMacAddr_t           *nextMacAddr,
                                        L7_WDM_MANAGED_AP_STATUS_t *status);

/*********************************************************************
*
* @purpose  Get the next managed AP entry flagged for a WS for a pending action.
*
* @param    L7_IP_ADDR_t switchIPAddr @b{(input)} Switch IP Address to lookup
* @param    L7_enetMacAddr_t macAddr @b{(input)} last AP MAC flagged
* @param    L7_WDM_MANAGED_AP_FLAG_t flag @b{(input)} type of pending entry to lookup
* @param    L7_enetMacAddr_t *nextMacAddr @b{(output)} next AP MAC flagged
* @param    L7_WDM_MANAGED_AP_STATUS_t *status @b{(output)} managed AP entry status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchManagedAPPendingEntryNextGet (L7_IP_ADDR_t switchIPAddr,
                            L7_enetMacAddr_t macAddr,
                            L7_WDM_MANAGED_AP_FLAG_t flag,
                            L7_enetMacAddr_t *nextMacAddr,
                            L7_WDM_MANAGED_AP_STATUS_t *status);

/*********************************************************************
*
* @purpose  Set the Cluster-AP-Status parameters received
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_char8          *location  @b{(input)} location
* @param    L7_uint32         profileId  @b{(input)} Profile Id
* @param    L7_char8          profileName  @b{(input)} Profile Name
* @param    L7_uint32         apStatus  @b{(input)} Managed AP Status
* @param    L7_uint32         apCfgStatus  @b{(input)} AP Cfg Status
* @param    L7_ushort16       elementId  @b{(input)} Failing element ID
* @param    L7_char8          failureMsg  @b{(input)} Error Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPWIDSParamsSet(L7_enetMacAddr_t macAddr,
                            L7_char8 *location, L7_uint32 profileId,
                            L7_char8 *profileName, L7_uint32 apStatus,
                            L7_uint32 apCfgStatus, L7_ushort16 elementId,
                            L7_char8 *failureMsg);

/*********************************************************************
*
* @purpose  Set the Cluster-Radio-VAP-Status related parameters received
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP MAC address
* @param    L7_uchar8         rif         @b{(input)} Radio Interface
* @param    L7_enetMacAddr_t  radioMACAddr @b{(input)} Radio MAC address
* @param    L7_WDM_RADIO_PHY_MODE_t mode  @b{(output)} PHY mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioWIDSParamsSet(L7_enetMacAddr_t macAddr,
                            L7_uchar8 rif, L7_uint32 adminMode,
                            L7_enetMacAddr_t radioMACAddr,
                            L7_WDM_RADIO_PHY_MODE_t phyMode,
                            L7_WDM_RADIO_PHY_MODE_MASK_t phyModesMask);

/*********************************************************************
*
* @purpose  Set the network IP adddress of managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_IP_ADDR_t      *ipAddr  @b{(input)} ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPIPAddrSet(L7_enetMacAddr_t macAddr, L7_IP_ADDR_t ipAddr);

/*********************************************************************
*
* @purpose  Get the network IP adddress of managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_IP_ADDR_t      *ipAddr  @b{(output)} pointer to ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPIPAddrGet(L7_enetMacAddr_t macAddr, L7_IP_ADDR_t *ipAddr);

/*********************************************************************
*
* @purpose  Get the network IP Subnet Mask of managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_IP_MASK_t      *ipMask  @b{(output)} pointer to ip subnet
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPIPMaskGet (L7_enetMacAddr_t macAddr, L7_IP_MASK_t *ipMask);

/*********************************************************************
*
* @purpose  Get the IP Address of WS managing this AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_IP_ADDR_t      *siwtchIPAddr  @b{(output)} pointer to ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPSwitchIPAddrGet(L7_enetMacAddr_t macAddr,
                            L7_IP_ADDR_t *switchIPAddr);

/*********************************************************************
*
* @purpose  Get the MAC Address of WS managing this AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_enetMacAddr_t  *switchMACAddr  @b{(output)} pointer to MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPSwitchMACAddrGet(L7_enetMacAddr_t macAddr,
                            L7_enetMacAddr_t *switchMACAddr);

/*********************************************************************
*
* @purpose  Is the AP managed by the local switch (Cluster Controller only)
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_BOOL           *apLocal  @b{(output)} pointer to is AP Local
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPLocalGet(L7_enetMacAddr_t macAddr, L7_BOOL *apLocal);

/*********************************************************************
*
* @purpose  Is the AP managed by the local switch (Any switch)
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_BOOL           *apLocal  @b{(output)} pointer to is AP Local
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPLocalGetAny(L7_enetMacAddr_t macAddr, L7_BOOL *apLocal);

/*********************************************************************
*
* @purpose  Get the IP Address to which forwarded commands has to be sent
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_IP_ADDR_t      *pperIPAddr  @b{(output)} pointer to ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments When acting as Cluster Controller, some commands are sent to the
* @comments AP directly and some to the WS managing the AP for the sake of
* @comments message forwarding. Based on the whether AP is self managed or
* @comments peer managed, the AP IP Address or peer managed AP's Switch IP
* @comments Address is returned respectively.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPPeerRcvrIPAddrGet(L7_enetMacAddr_t macAddr,
                            L7_IP_ADDR_t *peerIPAddr);

/*********************************************************************
*
* @purpose  Get the configuration profile ID applied to managed AP.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_uint32         *profileId  @b{(output)} pointer to profile id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPProfileIdGet(L7_enetMacAddr_t macAddr, L7_uint32 *profileId);

/*********************************************************************
*
* @purpose  Get the configuration profile name applied to managed AP.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_uint32         *profileName  @b{(output)} pointer to profile name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPProfileNameGet(L7_enetMacAddr_t macAddr,
                            L7_char8 *profileName);

/*********************************************************************
*
* @purpose  Get the managed AP radio mode.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} Radio Index
* @param    L7_uint32         *mode  @b{(output)} pointer to radio mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioAdminModeGet(L7_enetMacAddr_t macAddr,
                            L7_uchar8 radioIf, L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Set the managed AP radio Sentry Mode
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} Radio Index
* @param    L7_uint32         sentryMode  @b{(output)} Sentry Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioScanSentryModeSet (L7_enetMacAddr_t macAddr,
                            L7_uchar8 radioIf, L7_uint32 sentryMode);

/*********************************************************************
*
* @purpose  Get the managed AP radio sentry mode.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} Radio Index
* @param    L7_uint32         *sentryMode  @b{(output)} pointer to sentry mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioScanSentryModeGet(L7_enetMacAddr_t macAddr,
                            L7_uchar8 radioIf, L7_uint32 *sentryMode);

/*********************************************************************
*
* @purpose  Set the managed AP radio Auto Channel Mode
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} Radio Index
* @param    L7_uint32         autoChannel  @b{(output)} Auto Channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioAutoChannelModeSet (L7_enetMacAddr_t macAddr,
                            L7_uchar8 radioIf, L7_uint32 autoChannel);

/*********************************************************************
*
* @purpose  Get the managed AP radio Auto Channel Mode
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} Radio Index
* @param    L7_uint32         *autoChannel  @b{(output)} pointer to Auto Channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioAutoChannelModeGet(L7_enetMacAddr_t macAddr,
                            L7_uchar8 radioIf, L7_uint32 *autoChannel);

/*********************************************************************
*
* @purpose  Set the managed AP radio Auto Power Mode
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} Radio Index
* @param    L7_uint32         autoPower  @b{(output)} Auto Power
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioAutoPowerModeSet (L7_enetMacAddr_t macAddr,
                            L7_uchar8 radioIf, L7_uint32 autoPower);

/*********************************************************************
*
* @purpose  Get the managed AP radio Auto Power Mode
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} Radio Index
* @param    L7_uint32         *autoPower  @b{(output)} pointer to Auto Power
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioAutoPowerModeGet(L7_enetMacAddr_t macAddr,
                            L7_uchar8 radioIf, L7_uint32 *autoPower);

/*********************************************************************
*
* @purpose  Set the managed AP radio Default Tx Power
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} Radio Index
* @param    L7_uint32         defaultTxPower  @b{(output)} Default Tx Power
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioDefaultTxPowerSet (L7_enetMacAddr_t macAddr,
                            L7_uchar8 radioIf, L7_uint32 defaultTxPower);

/*********************************************************************
*
* @purpose  Get the managed AP radio Default Tx Power
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} Radio Index
* @param    L7_uint32         *defaultTxPower  @b{(output)} pointer to Def. Tx Power
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioDefaultTxPowerGet(L7_enetMacAddr_t macAddr,
                            L7_uchar8 radioIf, L7_uint32 *defaultTxPower);

/*********************************************************************
*
* @purpose  Set the managed AP VAP mode
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} Radio Index
* @param    L7_uchar8         vapId  @b{(input)} VAP id
* @param    L7_uint32         vapMode  @b{(output)} Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPModeSet(L7_enetMacAddr_t macAddr,
                            L7_uchar8 radioIf, L7_uchar8 vapId,
                            L7_uint32 vapMode);

/*********************************************************************
*
* @purpose  Get the managed AP VAP mode
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} Radio Index
* @param    L7_uchar8         vapId  @b{(input)} VAP id
* @param    L7_uint32         *vapMode  @b{(output)} pointer to Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPModeGet(L7_enetMacAddr_t macAddr,
                            L7_uchar8 radioIf, L7_uchar8 vapId,
                            L7_uint32 *vapMode);

/*********************************************************************
*
* @purpose  Set the managed AP VAP SSID
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} Radio Index
* @param    L7_uchar8         vapId  @b{(input)} VAP id
* @param    L7_char8          *ssid  @b{(output)} pointer to SSID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPSSIDSet(L7_enetMacAddr_t macAddr,
                            L7_uchar8 radioIf, L7_uchar8 vapId,
                            L7_char8 *ssid);

/*********************************************************************
*
* @purpose  Get the managed AP VAP SSID
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} Radio Index
* @param    L7_uchar8         vapId  @b{(input)} VAP id
* @param    L7_char8          *ssid  @b{(output)} pointer to SSID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPSSIDGet(L7_enetMacAddr_t macAddr,
                            L7_uchar8 radioIf, L7_uchar8 vapId,
                            L7_char8 *ssid);

/*********************************************************************
*
* @purpose  Get the VAP ID from AP mac, VAP mac address and radio ID.
*
* @param    L7_enetMacAddr_t   apMacAddr   @b{(input)} AP mac
* @param    L7_uchar8          radioIf     @b{(input)} radio interface
* @param    L7_enetMacAddr_t   vapMacAddr  @b{(input)} VAP mac
* @param    L7_uint32          *vapId      @b{(output)} pointer to vap id
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPMACVAPIDGet(L7_enetMacAddr_t   apMacAddr,
                                L7_uchar8          radioIf,
                                L7_enetMacAddr_t   vapMacAddr,
                                L7_uint32          *vapId);

/*********************************************************************
*
* @purpose  Get the location of managed AP.
*
* @param    L7_enetMacAddr_t  macAddr    @b{(input)} AP mac address
* @param    L7_char8          *location  @b{(output)} pointer to location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPLocationGet(L7_enetMacAddr_t macAddr, L7_char8 *location);

/*********************************************************************
*
* @purpose  Get the vendor ID of managed AP software.
*
* @param    L7_enetMacAddr_t  macAddr    @b{(input)} AP mac address
* @param    L7_ushort16       *vendorId  @b{(output)} pointer to vendor id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVendorIdGet(L7_enetMacAddr_t macAddr, L7_ushort16 *vendorId);

/*********************************************************************
*
* @purpose  Get the protocol version supported on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} AP mac address
* @param    L7_ushort16       *prtlVersion  @b{(output)} pointer to prtl version
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPProtocolVersionGet(L7_enetMacAddr_t macAddr, L7_ushort16 *prtlVersion);

/*********************************************************************
*
* @purpose  Get the software version on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_char8          *swVersion  @b{(output)} pointer to sw version
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPSoftwareVersionGet(L7_enetMacAddr_t macAddr, L7_char8 *swVersion);

/*********************************************************************
*
* @purpose  Get the hardware platform for managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uint32         *hwType  @b{(output)} pointer to hw type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPHardwareTypeGet(L7_enetMacAddr_t macAddr, L7_uint32 *hwType);

/*********************************************************************
*
* @purpose  Get the unique serial number assigned to managed AP.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_char8          *serialNum  @b{(output)} pointer to serial number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPSerialNumGet(L7_enetMacAddr_t macAddr, L7_char8 *serialNum);

/*********************************************************************
*
* @purpose  Get the hardware part number for managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_char8          *partNum  @b{(output)} pointer to part number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPPartNumGet(L7_enetMacAddr_t macAddr, L7_char8 *partNum);

/*********************************************************************
*
* @purpose  Get the managed AP discovery method.
*
* @param    L7_enetMacAddr_t     macAddr     @b{(input)} AP mac address
* @param    L7_WDM_DISC_REASON_t *discReason @b{(output)} pointer to method
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPDiscoveryReasonGet(L7_enetMacAddr_t macAddr,
                                       L7_WDM_DISC_REASON_t *discReason);

/*********************************************************************
*
* @purpose  Purge a failed AP entry.
*
* @param    L7_enetMacAddr_t           macAddr @b{(input)} AP mac address
* @param    L7_WDM_MANAGED_AP_STATUS_t status  @b{(input)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatusSet(L7_enetMacAddr_t macAddr, L7_WDM_MANAGED_AP_STATUS_t status);

/*********************************************************************
*
* @purpose  Get the current state of managed AP.
*
* @param    L7_enetMacAddr_t           macAddr @b{(input)} AP mac address
* @param    L7_WDM_MANAGED_AP_STATUS_t *status @b{(output)} pointer to state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatusGet(L7_enetMacAddr_t macAddr, L7_WDM_MANAGED_AP_STATUS_t *status);


/*********************************************************************
*
* @purpose  Update age for a managed AP entry.
*
* @param    L7_enetMacAddr_t       macAddr @b{(input)} AP mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPAgeUpdate(L7_enetMacAddr_t macAddr);


/*********************************************************************
*
* @purpose  Set the current configuration status of managed AP.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)} AP mac address
* @param    L7_WDM_ACTION_STATUS_t status      @b{(input)} status
* @param    L7_ushort16            elementId   @b{(input)} failing config element ID
* @param    L7_char8               *failureMsg @b{(input)} failure message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPCfgStatusSet(L7_enetMacAddr_t macAddr, L7_WDM_ACTION_STATUS_t status,
                                 L7_ushort16 elementId, L7_char8 *failureMsg);

/*********************************************************************
*
* @purpose  Get the current configuration status of managed AP.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)} AP mac address
* @param    L7_WDM_ACTION_STATUS_t *status     @b{(output)} pointer to status
* @param    L7_ushort16            elementId   @b{(output)} failing config element ID
* @param    L7_char8               *failureMsg @b{(output)} failure message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPCfgStatusGet(L7_enetMacAddr_t macAddr, L7_WDM_ACTION_STATUS_t *status,
                                 L7_ushort16 *elementId, L7_char8 *failureMsg);

/*********************************************************************
*
* @purpose  Set the current status of code download for managed AP.
*
* @param    L7_enetMacAddr_t        macAddr  @b{(input)} AP mac address
* @param    L7_WDM_ACTION_STATUS_t  status   @b{(input)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPDownloadStatusSet(L7_enetMacAddr_t macAddr, L7_WDM_ACTION_STATUS_t status);

/*********************************************************************
*
* @purpose  Get the current status of code download for managed AP.
*
* @param    L7_enetMacAddr_t        macAddr  @b{(input)} AP mac address
* @param    L7_WDM_ACTION_STATUS_t  *status  @b{(output)} pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPDownloadStatusGet(L7_enetMacAddr_t macAddr, L7_WDM_ACTION_STATUS_t *status);

/*********************************************************************
*
* @purpose  Start the code download for managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments UI calls this API via usmDb to ensure that the user request
*           is not made in the wrong state.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPDownloadStart(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Set the code download initiated by Cluster Controller.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_BOOL status @b{(input)} status - L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPDownloadWIDSCInitiatedSet (L7_enetMacAddr_t macAddr,
                            L7_BOOL status);

/*********************************************************************
*
* @purpose  Get the code download initiated by Cluster Controller.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP mac address
* @param    L7_BOOL *status @b{(output)} pointer to the status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPDownloadWIDSCInitiatedGet (L7_enetMacAddr_t macAddr,
                            L7_BOOL *status);

/*********************************************************************
*
* @purpose  Get the number of AP authenticated clients.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uint32         *clients  @b{(output)} pointer to number of clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPAuthenticatedClientsGet(L7_enetMacAddr_t macAddr, L7_uint32 *clients);

/*********************************************************************
*
* @purpose  Set the time since last power-on reset of managed AP.
*
* @param    L7_enetMacAddr_t  macAddr    @b{(input)} AP mac address
* @param    L7_uint32         sysUpTime  @b{(input)} system up time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPSysUptimeSet(L7_enetMacAddr_t macAddr, L7_uint32 sysUpTime);

/*********************************************************************
*
* @purpose  Get the time since last power-on reset of managed AP.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_uint32         *sysUpTime  @b{(output)} pointer to system up time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPSysUptimeGet(L7_enetMacAddr_t macAddr, L7_uint32 *sysUpTime);

/*********************************************************************
*
* @purpose  Get the time since last communication with managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uint32         *age     @b{(output)} pointer to age
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPAgeGet(L7_enetMacAddr_t macAddr, L7_uint32 *age);

/*********************************************************************
*
* @purpose  Get the reset status of managed AP.
*
* @param    L7_enetMacAddr_t       macAddr @b{(input)} AP mac address
* @param    L7_WDM_ACTION_STATUS_t *status @b{(output)} pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPResetStatusGet(L7_enetMacAddr_t macAddr, L7_WDM_ACTION_STATUS_t *status);

/*********************************************************************
*
* @purpose  Reset a managed AP.
*
* @param    L7_enetMacAddr_t       macAddr @b{(input)} AP mac address
* @param    L7_WDM_ACTION_STATUS_t status  @b{(input)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPResetStatusSet(L7_enetMacAddr_t macAddr, L7_WDM_ACTION_STATUS_t status);

/*********************************************************************
*
* @purpose  Reset all  managed APs.
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPResetAllStart();

/*********************************************************************
*
* @purpose  Set password to Enable/Disable debug on a managed AP.
*
* @param    L7_enetMacAddr_t   macAddr     @b{(input)} AP mac address
* @param    L7_char8          *password    @b{(input)} debug password
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPDebugPasswordSet(L7_enetMacAddr_t macAddr,
                                     L7_char8        *password);

/*********************************************************************
*
* @purpose  Get password to Enable/Disable debug on a managed AP.
*
* @param    L7_enetMacAddr_t   macAddr     @b{(input)}  AP mac address
* @param    L7_char8          *password    @b{(output)} debug password
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPDebugPasswordGet(L7_enetMacAddr_t macAddr,
                                     L7_char8        *password);

/*********************************************************************
*
* @purpose  Enable/Disable debug mode for a managed AP.
*
* @param    L7_enetMacAddr_t       macAddr @b{(input)} AP mac address
* @param    L7_uint32              mode    @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This API sets the debug mode and initiates action to send command.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPDebugModeSet(L7_enetMacAddr_t macAddr,
                                 L7_uint32        mode);

/*********************************************************************
*
* @purpose  Get requested debug mode for a managed AP.
*
* @param    L7_enetMacAddr_t       macAddr @b{(input)}  AP mac address
* @param    L7_uint32             *mode    @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPDebugModeGet(L7_enetMacAddr_t macAddr,
                                 L7_uint32       *mode);

/*********************************************************************
*
* @purpose  Set the debug command status for a managed AP.
*
* @param    L7_enetMacAddr_t        macAddr @b{(input)}  AP mac address
* @param    L7_WDM_ACTION_STATUS_t  status  @b{(input)}  command status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPDebugStatusSet(L7_enetMacAddr_t        macAddr,
                                   L7_WDM_ACTION_STATUS_t  status);

/*********************************************************************
*
* @purpose  Get the debug command status for a managed AP.
*
* @param    L7_enetMacAddr_t        macAddr @b{(input)} AP mac address
* @param    L7_WDM_ACTION_STATUS_t *status  @b{(output)} pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPDebugStatusGet(L7_enetMacAddr_t        macAddr,
                                   L7_WDM_ACTION_STATUS_t *status);

/*********************************************************************
*
* @purpose  Verify presence of a managed AP entry.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPEntryGet(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Get the mac address of next managed AP entry.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} AP mac address
* @param    L7_enetMacAddr_t  *nextMacAddr  @b{(output)} pointer to next mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPEntryNextGet(L7_enetMacAddr_t macAddr, L7_enetMacAddr_t *nextMacAddr);

/*********************************************************************
*
* @purpose  Get the mac address of next managed AP entry with managed status.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} AP mac address
* @param    L7_enetMacAddr_t  *nextMacAddr  @b{(output)} pointer to next mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPNextEntryWithManagedStatusGet(L7_enetMacAddr_t macAddr, L7_enetMacAddr_t *nextMacAddr);

/*********************************************************************
*
* @purpose  Get the mac address of next managed AP entry with managed status
*           of given image type.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} AP mac address
* @param    L7_enetMacAddr_t  *nextMacAddr  @b{(output)} pointer to next mac address
* @param    L7_uint32         *imageType  @b{(input)} Image type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPImageTypeEntryNextGet(L7_enetMacAddr_t macAddr,
                                          L7_enetMacAddr_t *nextMacAddr,
                                          L7_WDM_AP_IMAGE_TYPE_t imageType);

/*********************************************************************
*
* @purpose  Set all AP TSPEC Traffic Stream status fields for a managed AP entry.
*
* @param    L7_enetMacAddr_t            macAddr  @b{(input)} AP MAC address
* @param    L7_uchar8                   acindex  @b{(input)} access category index
* @param    wdmTspecTsSumStatus_t      *tsStatus @b{(input)} per AC TS status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This API sets all values (input values are NOT added to current values).
*
* @comments  This function must be invoked separately for each supported AC.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPTspecTsStatusSet(L7_enetMacAddr_t macAddr,
                                     L7_uchar8 acindex,
                                     wdmTspecTsSumStatus_t *tsStatus);

/*********************************************************************
*
* @purpose  Get all AP TSPEC Traffic Stream status fields for a managed AP entry.
*
* @param    L7_enetMacAddr_t            macAddr  @b{(input)} AP MAC address
* @param    L7_uchar8                   acindex  @b{(input)} access category index
* @param    wdmTspecTsSumStatus_t      *tsStatus @b{(input)} per AC TS status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This function must be invoked separately for each supported AC.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPTspecTsStatusGet(L7_enetMacAddr_t macAddr,
                                     L7_uchar8 acindex,
                                     wdmTspecTsSumStatus_t *tsStatus);

/*********************************************************************
*
* @purpose  Get the TSPEC number of active TS for a managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPTspecNumActiveTsGet(L7_enetMacAddr_t macAddr,
                                        L7_uchar8 acindex,
                                        L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC number of TS clients for a managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPTspecNumTsClientsGet(L7_enetMacAddr_t macAddr,
                                         L7_uchar8 acindex,
                                         L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC number of TS roaming clients for a managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPTspecNumTsRoamClientsGet(L7_enetMacAddr_t macAddr,
                                             L7_uchar8 acindex,
                                             L7_uint32 *val);

/* End per managed AP APIs */

/* Start per radio APIs */

/*********************************************************************
*
* @purpose  Get the mac address for radio on managed AP.
*
* @param    L7_enetMacAddr_t macAddr       @b{(input)} AP mac address
* @param    L7_uchar8        radioIf       @b{(input)} radio interface
* @param    L7_enetMacAddr_t *radioMacAddr @b{(output)} pointer to radio mac
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioMacAddrGet(L7_enetMacAddr_t macAddr,
                                    L7_uchar8 radioIf, L7_enetMacAddr_t *radioMacAddr);

/*********************************************************************
*
* @purpose  Get the physical mode for radio on managed AP.
*
* @param    L7_enetMacAddr_t        macAddr @b{(input)} AP mac address
* @param    L7_uchar8               radioIf @b{(input)} radio interface
* @param    L7_WDM_RADIO_PHY_MODE_t *mode   @b{(output)} pointer to mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioPhyModeGet(L7_enetMacAddr_t macAddr,
                                    L7_uchar8 radioIf, L7_WDM_RADIO_PHY_MODE_t *mode);

/*********************************************************************
*
* @purpose  Get the physical mode mask for radio on managed AP.
*
* @param    L7_enetMacAddr_t        macAddr @b{(input)} AP mac address
* @param    L7_uchar8               radioIf @b{(input)} radio interface
* @param    L7_WDM_RADIO_PHY_MODE_MASK_t *mask   @b{(output)} mode mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioPhyModeMaskGet(L7_enetMacAddr_t macAddr,
                            L7_uchar8 radioIf,
                            L7_WDM_RADIO_PHY_MODE_MASK_t *mask);

/*********************************************************************
*
* @purpose  Set the transmit channel for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uchar8         channel  @b{(input)} channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioChannelSet(L7_enetMacAddr_t macAddr,
                                    L7_uchar8 radioIf,
                                    L7_uchar8 channel);

/*********************************************************************
*
* @purpose  Get the transmit channel for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uchar8        *channel  @b{(output)} pointer to channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioChannelGet(L7_enetMacAddr_t macAddr,
                                    L7_uchar8 radioIf,
                                    L7_uchar8 *channel);

/*********************************************************************
*
* @purpose  Set the channel bandwidth for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uchar8         bandwidth  @b{(input)} bandwidth
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioBandwidthSet(L7_enetMacAddr_t macAddr,
                            L7_uchar8 radioIf, L7_uchar8 bandwidth);

/*********************************************************************
*
* @purpose  Get the channel bandwidth for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uchar8        *bandwidth  @b{(output)} pointer to bandwidth
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioBandwidthGet(L7_enetMacAddr_t macAddr,
                            L7_uchar8 radioIf, L7_uchar8 *channel);

/*********************************************************************
*
* @purpose  Add a supported channel for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uchar8         channel  @b{(input)} input channel
* @param    L7_uchar8         eligForACA @b{(input)} indicates if eligible for
* @param                      auto channel adjustment or not (0/1)
* @param    L7_uchar8         rdrDetRequired @b{(input)} indicates if radar
* @param                      detection is required or not on this channel(0/1)
* @param    L7_uchar8         rdrDetected   @b{(input)} indicates if a radar
* @param                      is detected on this channel(0/1)
* @param    L7_uint32         lastRdrDetTime   @b{(input)} indicates if time
* @param                      at which radar last detected
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioSupportedChannelAddUpdate(L7_enetMacAddr_t macAddr,
                                         L7_uchar8 radioIf, L7_uchar8 channel,
                                         L7_uchar8 eligForACA,
                                         L7_uchar8 rdrDetRequired,
                                         L7_uchar8 rdrDetected,
                                         L7_uint32 lastRdrDetTime);

/*********************************************************************
*
* @purpose  Delete a supported channel for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uchar8         channel  @b{(input)} input channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioSupportedChannelDelete(L7_enetMacAddr_t macAddr,
                                               L7_uchar8 radioIf,
                                               L7_uchar8 channel);

/*********************************************************************
*
* @purpose  Verify presence of a supported channel for radio interface
*           on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uchar8         channel  @b{(input)} input channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioSupportedChGet(L7_enetMacAddr_t macAddr,
                                       L7_uchar8 radioIf,
                                       L7_uchar8 channel);

/*********************************************************************
*
* @purpose  Get the next Supported channel for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} AP mac address
* @param    L7_uchar8         radioIf       @b{(input)} radio interface
* @param    L7_uchar8         channel       @b{(input)} input channel
* @param    L7_uchar8        *nextChannel  @b{(output)} pointer to next channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioSupportedChNextGet(L7_enetMacAddr_t macAddr,
                                           L7_uchar8 radioIf,
                                           L7_uchar8 channel,
                                           L7_uchar8 *nextChannel);

/*********************************************************************
*
* @purpose  Get the eligible for ACA flag for radio interface on
* @purpose  managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uchar8*        eligACA  @b{(output)} flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioSupportedChEligACAGet(L7_enetMacAddr_t macAddr,
                                       L7_uchar8 radioIf, L7_uchar8 channel,
                                       L7_uchar8* eligACA);


/*********************************************************************
*
* @purpose  Get the radar detected flag for radio interface on
* @purpose  managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uchar8*        radarDetected  @b{(output)} flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_EXIST -- If the AP Entry is not present in the Managed AP
*                           database
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioSupportedChRdrDetectedGet(L7_enetMacAddr_t macAddr,
                                       L7_uchar8 radioIf, L7_uchar8 channel,
                                       L7_uchar8* radarDetected);
/*********************************************************************
*
* @purpose  Get the radar detection required flag for radio interface on
* @purpose  managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uchar8*        radarDetectionRequired  @b{(output)} flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioSupportedChRdrDetectionRequiredGet(L7_enetMacAddr_t macAddr,
                                       L7_uchar8 radioIf, L7_uchar8 channel,
                                       L7_uchar8* radarDetectionRequired);
/*********************************************************************
*
* @purpose  Get the radar detected time for radio interface on
* @purpose  managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uchar8         channel  @b{(input)} channel
* @param    L7_uint32         time     @b{(output)} radar det time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioSupportedChRdrDetectedTimeGet(L7_enetMacAddr_t macAddr,
                                       L7_uchar8 radioIf, L7_uchar8 channel,
                                       L7_uint32* time);



/*********************************************************************
*
* @purpose  Test the fixed channel to see if it is a valid channel
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_BOOL *         chVal    @b{(output)} test result
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioFixedChValidTest(L7_enetMacAddr_t macAddr,
                                          L7_uchar8 radioIf, L7_BOOL* chVal);

/*********************************************************************
*
* @purpose  Set the fixed channel flag for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_BOOL           chInd    @b{(input)} flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioFixedChIndSet(L7_enetMacAddr_t macAddr,
                                       L7_uchar8 radioIf, L7_BOOL chInd);

/*********************************************************************
*
* @purpose  Get the fixed channel flag for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_BOOL           *chInd   @b{(output)} pointer to flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioFixedChIndGet(L7_enetMacAddr_t macAddr,
                                       L7_uchar8 radioIf, L7_BOOL *chInd);

/*********************************************************************
*
* @purpose  Set the fixed channel for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uchar8         channel  @b{(input)} channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioFixedChannelSet(L7_enetMacAddr_t macAddr,
                            L7_uchar8 radioIf, L7_uchar8 channel);


/*********************************************************************
*
* @purpose  Get the enabled status for resource measurement
*           for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_BOOL *         pEnabled @b{(output)} is enabled?
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioResourceMeasEnabledGet(L7_enetMacAddr_t macAddr,
                                                L7_uchar8 radioIf, 
                                                L7_BOOL * pEnabled);


/*********************************************************************
*
* @purpose  Set the enabled status for resource measurement
*           for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr @b{(input)} AP mac address
* @param    L7_uchar8         radioIf @b{(input)} radio interface
* @param    L7_BOOL           enabled @b{(input)} is enabled?
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioResourceMeasEnabledSet(L7_enetMacAddr_t macAddr,
                                                L7_uchar8 radioIf, 
                                                L7_BOOL enabled);



/*********************************************************************
*
* @purpose  Get the fixed channel for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uchar8         *channel  @b{(output)} pointer to channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioFixedChannelGet(L7_enetMacAddr_t macAddr,
                                         L7_uchar8 radioIf, L7_uchar8 *channel);

/*********************************************************************
*
* @purpose  Get the Admin channel for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uchar8         *channel  @b{(output)} pointer to channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioAdminChannelGet(L7_enetMacAddr_t macAddr,
                                         L7_uchar8 radioIf, L7_uchar8 *channel);

/*********************************************************************
*
* @purpose  Set the channel change request status for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t       macAddr  @b{(input)} AP mac address
* @param    L7_uchar8              radioIf  @b{(input)} radio interface
* @param    L7_WDM_ACTION_STATUS_t status   @b{(input)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioManChAdjStatusSet(L7_enetMacAddr_t       macAddr,
                                           L7_uchar8              radioIf,
                                           L7_WDM_ACTION_STATUS_t status);

/*********************************************************************
*
* @purpose  Get the channel change request status for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t        macAddr  @b{(input)} AP mac address
* @param    L7_uchar8               radioIf  @b{(input)} radio interface
* @param    L7_WDM_ACTION_STATUS_t *status   @b{(output)} pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioManChAdjStatusGet(L7_enetMacAddr_t        macAddr,
                                           L7_uchar8               radioIf,
                                           L7_WDM_ACTION_STATUS_t *status);

/*********************************************************************
*
* @purpose  Set the cell recovery status for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t       macAddr  @b{(input)} AP mac address
* @param    L7_uchar8              radioIf  @b{(input)} radio interface
* @param    L7_WDM_ACTION_STATUS_t status   @b{(input)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioCellRecoveryStatusSet(L7_enetMacAddr_t   macAddr,
                                           L7_uchar8              radioIf,
                                           L7_WDM_ACTION_STATUS_t status);
/*********************************************************************
*
* @purpose  Get the cell recovery status for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t        macAddr  @b{(input)} AP mac address
* @param    L7_uchar8               radioIf  @b{(input)} radio interface
* @param    L7_WDM_ACTION_STATUS_t  *status   @b{(output)} pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioCellRecoveryStatusGet(L7_enetMacAddr_t   macAddr,
                                           L7_uchar8              radioIf,
                                           L7_WDM_ACTION_STATUS_t *status);
/*********************************************************************
*
* @purpose  Adds the cell recovery info  for a given failed radio
*
* @param    L7_enetMacAddr_t     macAddr     @b{(input)} AP mac address
* @param    L7_uchar8            radioIf     @b{(input)} radio interface
* @param    L7_uchar8            nbrCount    @b{(input)} num nbrs to add
* @param    wdmCellRecoveryNbrInfo_t pNbrs   @b{(input)} ptr to neighbor info
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t
wdmManagedAPRadioCellRecoveryInfoAdd(L7_enetMacAddr_t         macAddr,
                                     L7_uchar8                radioIf,
                                     L7_uchar8                nbrCount,
                                     wdmCellRecoveryNbrInfo_t *pNbrs);


/*********************************************************************
*
* @purpose  Gets the cell recovery info for a given failed radio
*
* @param    L7_enetMacAddr_t     macAddr     @b{(input)}  AP mac address
* @param    L7_uchar8            radioIf     @b{(input)}  radio interface
* @param    L7_uchar8            nbrCount    @b{(input)}  max nbrs to get
* @param    wdmCellRecoveryNbrInfo_t *pNbrs  @b{(output)} gotten nbr data
* @param    L7_WDM_ACTION_STATUS_t  *status  @b{(ouptut)} recovery status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If maxNbrCount (which is the length of the buffer pointed
*           to by pNbrs) is less than L7_WDM_MAX_CELL_RECOVERY_NBRS,
*           unused area of buffer will contain zeroes.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioCellRecoveryInfoGet(L7_enetMacAddr_t    macAddr,
                                             L7_uchar8           radioIf,
                                             L7_uchar8           maxNbrCount,
                                             wdmCellRecoveryNbrInfo_t * pNbrs,
                                             L7_WDM_ACTION_STATUS_t  *status);

/*********************************************************************
*
* @purpose  Start a manual channel change request for a managed AP radio.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uchar8         channel  @b{(input)} channel to assign
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad entry or channel change in progress
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioManChAdjStart(L7_enetMacAddr_t macAddr,
                                       L7_uchar8        radioIf,
                                       L7_uchar8        channel);

/*********************************************************************
*
* @purpose  Set the transmit power for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uint32         power    @b{(input)} power
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioTxPwrSet(L7_enetMacAddr_t macAddr,
                                  L7_uchar8 radioIf, L7_uint32 power);

/*********************************************************************
*
* @purpose  Get the transmit power for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uint32         *power   @b{(output)} pointer to power
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioTxPwrGet(L7_enetMacAddr_t macAddr,
                                  L7_uchar8 radioIf, L7_uchar8 *power);

/*********************************************************************
*
* @purpose  Set the fixed power flag for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_BOOL           pwrInd   @b{(input)} flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioFixedPwrIndSet(L7_enetMacAddr_t macAddr,
                                        L7_uchar8 radioIf, L7_BOOL pwrInd);

/*********************************************************************
*
* @purpose  Get the fixed power flag for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_BOOL           *pwrInd  @b{(output)} pointer to flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioFixedPwrIndGet(L7_enetMacAddr_t macAddr,
                                        L7_uchar8 radioIf, L7_BOOL *pwrInd);

/*********************************************************************
*
* @purpose  Set the fixed power for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uchar8         power  @b{(input)} power
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioFixedPowerSet(L7_enetMacAddr_t macAddr,
                            L7_uchar8 radioIf, L7_uchar8 power);

/*********************************************************************
*
* @purpose  Get the fixed power for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uchar8         *power   @b{(output)} pointer to power
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioFixedPowerGet(L7_enetMacAddr_t macAddr,
                                       L7_uchar8 radioIf, L7_uchar8 *power);

/*********************************************************************
*
* @purpose  Get the admin power for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uchar8         *power   @b{(output)} pointer to power
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioAdminPowerGet(L7_enetMacAddr_t macAddr,
                                       L7_uchar8 radioIf, L7_uchar8 *power);

/*********************************************************************
*
* @purpose  Set the power change request status for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t       macAddr  @b{(input)} AP mac address
* @param    L7_uchar8              radioIf  @b{(input)} radio interface
* @param    L7_WDM_ACTION_STATUS_t status   @b{(input)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioManPwrAdjStatusSet(L7_enetMacAddr_t       macAddr,
                                            L7_uchar8              radioIf,
                                            L7_WDM_ACTION_STATUS_t status);

/*********************************************************************
*
* @purpose  Get the power change request status for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t        macAddr  @b{(input)} AP mac address
* @param    L7_uchar8               radioIf  @b{(input)} radio interface
* @param    L7_WDM_ACTION_STATUS_t *status   @b{(output)} pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioManPwrAdjStatusGet(L7_enetMacAddr_t        macAddr,
                                            L7_uchar8               radioIf,
                                            L7_WDM_ACTION_STATUS_t *status);

/*********************************************************************
*
* @purpose  Start a manual power change request for a managed AP radio.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uchar8         power    @b{(input)} power to assign
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, bad entry or power change in progress
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioManPwrAdjStart(L7_enetMacAddr_t macAddr,
                                        L7_uchar8        radioIf,
                                        L7_uchar8        power);

/*********************************************************************
*
* @purpose  Get the number of clients authenticated on radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uint32         *clients  @b{(output)} pointer to clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioAuthClientsGet(L7_enetMacAddr_t macAddr,
                                        L7_uchar8 radioIf, L7_uint32 *clients);

/*********************************************************************
*
* @purpose  Set the number of neighbors on radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uint32         nbrs     @b{(input)} number of neighbors
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioTotalNbrsSet(L7_enetMacAddr_t macAddr,
                                      L7_uchar8 radioIf, L7_uint32 nbrs);

/*********************************************************************
*
* @purpose  Get the number of neighbors on radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uint32         *nbrs    @b{(output)} pointer to neighbors
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioTotalNbrsGet(L7_enetMacAddr_t macAddr,
                                      L7_uchar8 radioIf, L7_uint32 *nbrs);

/*********************************************************************
*
* @purpose  Set the total network utilization for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uint32         util     @b{(input)} utilization
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioWLANUtilSet(L7_enetMacAddr_t macAddr,
                                     L7_uchar8 radioIf, L7_uint32 util);

/*********************************************************************
*
* @purpose  Get the total network utilization for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uint32         *util    @b{(output)} pointer to utilization
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioWLANUtilGet(L7_enetMacAddr_t macAddr,
                                     L7_uchar8 radioIf, L7_uint32 *util);

/*********************************************************************
*
* @purpose  Set all Radio TSPEC Traffic Stream status fields for a managed AP entry.
*
* @param    L7_enetMacAddr_t            macAddr  @b{(input)} AP MAC address
* @param    L7_uchar8                   radioIf  @b{(input)} radio interface
* @param    L7_uchar8                   acindex  @b{(input)} access category index
* @param    wdmTspecClientSumStatus_t  *tsStatus @b{(input)} per VAP, per AC TS status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This API sets all values (input values are NOT added to current values).
*
* @comments  This function must be invoked separately for each supported AC.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioTspecTsStatusSet(L7_enetMacAddr_t macAddr,
                                          L7_uchar8 radioIf,
                                          L7_uchar8 acindex,
                                          wdmTspecClientSumStatus_t *tsStatus);

/*********************************************************************
*
* @purpose  Get all Radio TSPEC Traffic Stream status fields for a managed AP entry.
*
* @param    L7_enetMacAddr_t            macAddr  @b{(input)} AP MAC address
* @param    L7_uchar8                   radioIf  @b{(input)} radio interface
* @param    L7_uchar8                   acindex  @b{(input)} access category index
* @param    wdmTspecClientSumStatus_t  *tsStatus @b{(input)} per VAP, per AC TS status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This function must be invoked separately for each supported AC.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioTspecTsStatusGet(L7_enetMacAddr_t macAddr,
                                          L7_uchar8 radioIf,
                                          L7_uchar8 acindex,
                                          wdmTspecClientSumStatus_t *tsStatus);

/*********************************************************************
*
* @purpose  Get the TSPEC operational status for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioTspecOperStatusGet(L7_enetMacAddr_t macAddr,
                                            L7_uchar8 radioIf,
                                            L7_uchar8 acindex,
                                            L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC number of active TS for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioTspecNumActiveTsGet(L7_enetMacAddr_t macAddr,
                                             L7_uchar8 radioIf,
                                             L7_uchar8 acindex,
                                             L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC number of TS clients for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioTspecNumTsClientsGet(L7_enetMacAddr_t macAddr,
                                              L7_uchar8 radioIf,
                                              L7_uchar8 acindex,
                                              L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC number of TS roaming clients for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioTspecNumTsRoamClientsGet(L7_enetMacAddr_t macAddr,
                                                  L7_uchar8 radioIf,
                                                  L7_uchar8 acindex,
                                                  L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC medium time admitted for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioTspecMedTimeAdmittedGet(L7_enetMacAddr_t macAddr,
                                                 L7_uchar8 radioIf,
                                                 L7_uchar8 acindex,
                                                 L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC medium time unallocated for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This represents the unallocated medium time available for
*           non-roaming and roaming clients.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioTspecMedTimeUnallocGet(L7_enetMacAddr_t macAddr,
                                                L7_uchar8 radioIf,
                                                L7_uchar8 acindex,
                                                L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC medium time roaming unallocated for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This represents the unallocated medium time available for
*           roaming clients only.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioTspecMedTimeRoamUnallocGet(L7_enetMacAddr_t macAddr,
                                                    L7_uchar8 radioIf,
                                                    L7_uchar8 acindex,
                                                    L7_uint32 *val);

/*********************************************************************
*
* @purpose  Verify presence of a radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioEntryGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf);

/*********************************************************************
*
* @purpose  Get the next radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} AP mac address
* @param    L7_uchar8         radioIf       @b{(input)} radio interface
* @param    L7_uchar8         *nextRadioIf  @b{(output)} pointer to next radio
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioEntryNextGet(L7_enetMacAddr_t macAddr,
                                      L7_uchar8 radioIf, L7_uchar8 *nextRadioIf);

/* End per radio APIs */

/* Start per vap APIs */

/*********************************************************************
*
* @purpose  Get tunnel subnet associated with a managed AP VAP.
*
* @param    L7_enetMacAddr_t macAddr     @b{(input)}  base AP mac address
* @param    L7_uchar8        radioIf     @b{(input)}  radio interface
* @param    L7_enetMacAddr_t vapMacAddr  @b{(input)}  VAP MAC address
* @param    L7_IP_ADDR_t    *ip          @b{(output)} tunnel subnet IP
* @param    L7_IP_ADDR_t    *mask        @b{(output)} tunnel subnet mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is a helper function for use by the tunneling component,
*            it looks up the VAP, profile, and network IDs to get the
*            tunnel subnet parameters.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPMacAddrTunnelSubnetGet(L7_enetMacAddr_t macAddr,
                                              L7_uchar8        radioIf,
                                              L7_enetMacAddr_t vapMacAddr,
                                              L7_IP_ADDR_t    *ip,
                                              L7_IP_ADDR_t    *mask);

/*********************************************************************
*
* @purpose  Verify the entry VAP mac address.
*
* @param    L7_enetMacAddr_t vapMacAddr @b{(input)} VAP MAC Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPMACAddrEntryGet (L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Get the VAP mac address.
*
* @param    L7_enetMacAddr_t macAddr     @b{(input)} AP mac address
* @param    L7_uchar8        radioIf     @b{(input)} radio interface
* @param    L7_uint32        vapId       @b{(input)} vap id
* @param    L7_enetMacAddr_t *vapMacAddr @b{(output)} pointer to VAP mac
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPMacAddrGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                  L7_uint32 vapId, L7_enetMacAddr_t *vapMacAddr);

/*********************************************************************
*
* @purpose  Get the radio I/F and vap ID, given a VAP mac address.
*           (The inverse of wdmManagedAPVAPMacAddrGet().)
*
* @param    L7_enetMacAddr_t macAddr     @b{(input)} AP mac address
* @param    L7_enetMacAddr_t vapMacAddr  @b{(input)} VAP mac
* @param    L7_uchar8        *radioIf    @b{(output)} radio interface
* @param    L7_uint32        *vapId      @b{(output)} vap id

*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPMacAddrFind(L7_enetMacAddr_t macAddr,
                                   L7_enetMacAddr_t vapMacAddr,
                                   L7_uchar8 * pRadioIf,
                                   L7_uint32 * pVapId);

/*********************************************************************
*
* @purpose  Get the number of clients authenticated to the vap on a radio.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uint32         vapId     @b{(input)} vap id
* @param    L7_uint32         *clients  @b{(output)} pointer to clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPAuthClientsGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                      L7_uint32 vapId, L7_uint32 *clients);

/*********************************************************************
*
* @purpose  Set all VAP TSPEC Traffic Stream status fields for a managed AP entry.
*
* @param    L7_enetMacAddr_t            macAddr  @b{(input)} AP MAC address
* @param    L7_uchar8                   radioIf  @b{(input)} radio interface
* @param    L7_uint32                   vapId    @b{(input)} VAP ID
* @param    L7_uchar8                   acindex  @b{(input)} access category index
* @param    wdmTspecClientSumStatus_t  *tsStatus @b{(input)} per VAP, per AC TS status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This API sets all values (input values are NOT added to current values).
*
* @comments  This function must be invoked separately for each supported AC.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPTspecTsStatusSet(L7_enetMacAddr_t macAddr,
                                        L7_uchar8 radioIf,
                                        L7_uint32 vapId,
                                        L7_uchar8 acindex,
                                        wdmTspecClientSumStatus_t *tsStatus);

/*********************************************************************
*
* @purpose  Get all VAP TSPEC Traffic Stream status fields for a managed AP entry.
*
* @param    L7_enetMacAddr_t            macAddr  @b{(input)} AP MAC address
* @param    L7_uchar8                   radioIf  @b{(input)} radio interface
* @param    L7_uint32                   vapId    @b{(input)} VAP ID
* @param    L7_uchar8                   acindex  @b{(input)} access category index
* @param    wdmTspecClientSumStatus_t  *tsStatus @b{(input)} per VAP, per AC TS status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This function must be invoked separately for each supported AC.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPTspecTsStatusGet(L7_enetMacAddr_t macAddr,
                                        L7_uchar8 radioIf,
                                        L7_uint32 vapId,
                                        L7_uchar8 acindex,
                                        wdmTspecClientSumStatus_t *tsStatus);

/*********************************************************************
*
* @purpose  Get the TSPEC operational status for the vap on a radio.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uint32         vapId     @b{(input)} vap id
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPTspecOperStatusGet(L7_enetMacAddr_t macAddr,
                                          L7_uchar8 radioIf,
                                          L7_uint32 vapId,
                                          L7_uchar8 acindex,
                                          L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC number of active TS for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uint32         vapId     @b{(input)} vap id
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPTspecNumActiveTsGet(L7_enetMacAddr_t macAddr,
                                           L7_uchar8 radioIf,
                                           L7_uint32 vapId,
                                           L7_uchar8 acindex,
                                           L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC number of TS clients for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uint32         vapId     @b{(input)} vap id
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPTspecNumTsClientsGet(L7_enetMacAddr_t macAddr,
                                            L7_uchar8 radioIf,
                                            L7_uint32 vapId,
                                            L7_uchar8 acindex,
                                            L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC number of TS roaming clients for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uint32         vapId     @b{(input)} vap id
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPTspecNumTsRoamClientsGet(L7_enetMacAddr_t macAddr,
                                                L7_uchar8 radioIf,
                                                L7_uint32 vapId,
                                                L7_uchar8 acindex,
                                                L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC medium time admitted for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uint32         vapId     @b{(input)} vap id
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPTspecMedTimeAdmittedGet(L7_enetMacAddr_t macAddr,
                                               L7_uchar8 radioIf,
                                               L7_uint32 vapId,
                                               L7_uchar8 acindex,
                                               L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC medium time unallocated for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uint32         vapId     @b{(input)} vap id
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This represents the unallocated medium time available for
*           non-roaming and roaming clients.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPTspecMedTimeUnallocGet(L7_enetMacAddr_t macAddr,
                                              L7_uchar8 radioIf,
                                              L7_uint32 vapId,
                                              L7_uchar8 acindex,
                                              L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the TSPEC medium time roaming unallocated for radio interface on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uint32         vapId     @b{(input)} vap id
* @param    L7_uchar8         acindex   @b{(input)} access category index
* @param    L7_uint32        *val       @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This represents the unallocated medium time available for
*           roaming clients only.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPTspecMedTimeRoamUnallocGet(L7_enetMacAddr_t macAddr,
                                                  L7_uchar8 radioIf,
                                                  L7_uint32 vapId,
                                                  L7_uchar8 acindex,
                                                  L7_uint32 *val);

/*********************************************************************
*
* @purpose  Verify presence of a vap entry on a radio.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uint32         vapId    @b{(input)} vap id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPEntryGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf, L7_uint32 vapId);

/*********************************************************************
*
* @purpose  Get the next vap entry on a radio.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_uchar8         radioIf     @b{(input)} radio interface
* @param    L7_int32          vapId       @b{(input)} vap id
* @param    L7_uint32         *nextVapId  @b{(output)} pointer to next id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPEntryNextGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                    L7_int32 vapId, L7_uint32 *nextVapId);

/* End per vap APIs */

/* Start neighbor AP APIs */

/*********************************************************************
*
* @purpose  Update a neighbor AP entry.
*
* @param    L7_enetMacAddr_t    macAddr   @b{(input)} AP mac address
* @param    L7_uchar8           radioIf   @b{(input)} radio interface
* @param    L7_enetMacAddr_t    nbrApMac  @b{(input)} neighbor AP mac
* @param    L7_char8            *ssid     @b{(input)} ssid
* @param    L7_uint32           rssi      @b{(input)} indicator
* @param    L7_WDM_AP_STATUS_t  status    @b{(input)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioNbrAPEntryAddUpdate(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                             L7_enetMacAddr_t nbrApMac,
                                             L7_char8 *ssid,
                                             L7_uint32 rssi,
                                             L7_WDM_AP_STATUS_t status);

/*********************************************************************
*
* @purpose  Get the service set ID of neighbor AP network.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_enetMacAddr_t  nbrApMac  @b{(input)} neighbor AP mac
* @param    L7_char8          *ssid     @b{(output)} pointer to ssid
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioNbrAPSSIDGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                      L7_enetMacAddr_t nbrApMac, L7_char8 *ssid);

/*********************************************************************
*
* @purpose  Get the channel of neighbor AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_enetMacAddr_t  nbrApMac  @b{(input)} neighbor AP mac
* @param    L7_uint32         *rssi     @b{(output)} pointer to RSSI
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioNbrAPRSSIGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                      L7_enetMacAddr_t nbrApMac, L7_uint32 *rssi);

/*********************************************************************
*
* @purpose  Get the managed status of neighbor AP.
*
* @param    L7_enetMacAddr_t   macAddr  @b{(input)} AP mac address
* @param    L7_uchar8          radioIf  @b{(input)} radio interface
* @param    L7_enetMacAddr_t   nbrApMac @b{(input)} neighbor AP mac
* @param    L7_WDM_AP_STATUS_t *status  @b{(output)} pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioNbrAPStatusGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                        L7_enetMacAddr_t nbrApMac, L7_WDM_AP_STATUS_t *status);

/*********************************************************************
*
* @purpose  Get the time since the neighbor AP was last detected.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_enetMacAddr_t  nbrApMac  @b{(input)} neighbor AP mac
* @param    L7_uint32         *age      @b{(output)} pointer to time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioNbrAPAgeGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                     L7_enetMacAddr_t nbrApMac, L7_uint32 *age);

/*********************************************************************
*
* @purpose  Verify presence of a neighbor AP entry.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_enetMacAddr_t  nbrApMac  @b{(input)} neighbor AP mac
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioNbrAPEntryGet(L7_enetMacAddr_t macAddr,
                                       L7_uchar8 radioIf, L7_enetMacAddr_t nbrApMac);

/*********************************************************************
*
* @purpose  Get the next neighbor AP entry.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)} AP mac address
* @param    L7_uchar8         radioIf        @b{(input)} radio interface
* @param    L7_enetMacAddr_t  nbrApMac       @b{(input)} neighbor AP mac
* @param    L7_enetMacAddr_t  *nextNbrApMac  @b{(output)} pointer to next mac
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioNbrAPEntryNextGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                           L7_enetMacAddr_t nbrApMac, L7_enetMacAddr_t *nextNbrApMac);

/* End neighbor AP APIs */

/* Start neighbor client APIs */

/*********************************************************************
*
* @purpose  Update a neighbor client entry.
*
* @param    L7_enetMacAddr_t           macAddr   @b{(input)} AP mac address
* @param    L7_uchar8                  radioIf   @b{(input)} radio interface
* @param    L7_enetMacAddr_t           nbrCltMac @b{(input)} neighbor client mac
* @param    L7_uint32                  rssi      @b{(input)} indicator
* @param    L7_uchar8                  channel   @b{(input)} channel
* @param    L7_WDM_CLIENT_NBR_FLAGS_t  flags     @b{(input)} status flags
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioNbrClientEntryAddUpdate(L7_enetMacAddr_t          macAddr,
                                                 L7_uchar8                 radioIf,
                                                 L7_enetMacAddr_t          nbrCltMac,
                                                 L7_uint32                 rssi,
                                                 L7_uchar8                 channel,
                                                 L7_WDM_CLIENT_NBR_FLAGS_t flags);

/*********************************************************************
*
* @purpose  Disassociate a neighbor client entry.
*
* @param    L7_enetMacAddr_t           macAddr   @b{(input)} AP mac address
* @param    L7_uchar8                  radioIf   @b{(input)} radio interface
* @param    L7_enetMacAddr_t           nbrCltMac @b{(input)} neighbor client mac
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This clears the L7_WDM_CLIENT_NBR_ASSOC_THIS_AP flag, if
*             L7_WDM_CLIENT_NBR_RF_SCAN and L7_WDM_CLIENT_NBR_PROBE_REQUEST
*             are not set the neighbor entry will be deleted.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioNbrClientDisassoc(L7_enetMacAddr_t          macAddr,
                                           L7_uchar8                 radioIf,
                                           L7_enetMacAddr_t          nbrCltMac);

/*********************************************************************
*
* @purpose  Get the received signal strength of neighbor client.
*
* @param    L7_enetMacAddr_t  macAddr    @b{(input)} AP mac address
* @param    L7_uchar8         radioIf    @b{(input)} radio interface
* @param    L7_enetMacAddr_t  nbrCltMac  @b{(input)} neighbor client mac
* @param    L7_uint32         *rssi      @b{(output)} pointer to indicator
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioNbrClientRSSIGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                          L7_enetMacAddr_t nbrCltMac, L7_uint32 *rssi);

/*********************************************************************
*
* @purpose  Get the channel the neighbor client frame was received on.
*
* @param    L7_enetMacAddr_t  macAddr    @b{(input)} AP mac address
* @param    L7_uchar8         radioIf    @b{(input)} radio interface
* @param    L7_enetMacAddr_t  nbrCltMac  @b{(input)} neighbor client mac
* @param    L7_uchar8        *channel   @b{(output)} pointer to channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioNbrClientChannelGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                             L7_enetMacAddr_t nbrCltMac, L7_uchar8 *channel);

/*********************************************************************
*
* @purpose  Get the status of neighbor client.
*
* @param    L7_enetMacAddr_t           macAddr   @b{(input)} AP mac address
* @param    L7_uchar8                  radioIf   @b{(input)} radio interface
* @param    L7_enetMacAddr_t           nbrCltMac @b{(input)} neighbor client mac
* @param    L7_WDM_CLIENT_NBR_FLAGS_t *flags     @b{(output)} status flags
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioNbrClientStatusGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                            L7_enetMacAddr_t nbrCltMac, L7_WDM_CLIENT_NBR_FLAGS_t *status);

/*********************************************************************
*
* @purpose  Get the time since the neighbor client was last detected.
*
* @param    L7_enetMacAddr_t  macAddr    @b{(input)} AP mac address
* @param    L7_uchar8         radioIf    @b{(input)} radio interface
* @param    L7_enetMacAddr_t  nbrCltMac  @b{(input)} neighbor client mac
* @param    L7_uint32         *age       @b{(output)} pointer to time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioNbrClientAgeGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                         L7_enetMacAddr_t nbrCltMac, L7_uint32 *age);

/*********************************************************************
*
* @purpose  Verify presence of a neighbor client entry.
*
* @param    L7_enetMacAddr_t  macAddr    @b{(input)} AP mac address
* @param    L7_uchar8         radioIf    @b{(input)} radio interface
* @param    L7_enetMacAddr_t  nbrCltMac  @b{(input)} neighbor client mac
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioNbrClientEntryGet(L7_enetMacAddr_t macAddr,
                                           L7_uchar8 radioIf, L7_enetMacAddr_t nbrCltMac);

/*********************************************************************
*
* @purpose  Get the next neighbor client entry.
*
* @param    L7_enetMacAddr_t  macAddr         @b{(input)} AP mac address
* @param    L7_uchar8         radioIf         @b{(input)} radio interface
* @param    L7_enetMacAddr_t  nbrCltMac       @b{(input)} neighbor client mac
* @param    L7_enetMacAddr_t  *nextNbrCltMac  @b{(output)} pointer to next mac
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioNbrClientEntryNextGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                               L7_enetMacAddr_t nbrCltMac, L7_enetMacAddr_t *nextNbrCltMac);

/* End neighbor client APIs */

/*********************************************************************
*
* @purpose  Purge client and AP neighbor lists for all managed APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is what would be used by usmDb to
*              cleanup neighbor entries.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPNbrEntriesPurge();


/*********************************************************************
*
* @purpose  Purge managed AP entry if in L7_WDM_MANAGED_AP_FAILED status.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(output)} Managed AP MAC Address
*
* @returns  L7_SUCCESS
*
* @comments  This is used by usmDb to cleanup failed Managed AP entry.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPFailedDelete (L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Purge all managed AP entries with a L7_WDM_AP_FAILED status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This is used by usmDb to cleanup failed entries.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPFailedEntriesPurge();

/*********************************************************************
*
* @purpose  Purge ALL managed AP entries, regardless of state.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is ONLY intended to be called when the WS is in
*            the L7_WDM_WS_MODE_DISABLE_PENDING state, never from usmDb.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPEntriesPurge();

/* Start AP association and authentication failure APIs */

/*********************************************************************
*
* @purpose  Update an AP failure entry.
*
* @param    L7_enetMacAddr_t    macAddr     @b{(input)} AP mac address
* @param    L7_IP_ADDR_t        ipAddr      @b{(input)} ip address
* @param    L7_ushort16         vendorId    @b{(input)} vendor id
* @param    L7_ushort16         prtlVersion @b{(input)} prtl version
* @param    L7_char8            *swVersion  @b{(input)} sw version
* @param    L7_uint32           hwType      @b{(input)} hw type
* @param    L7_WDM_AP_FAILURE_t failType    @b{(input)} failure type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPFailureEntryAddUpdate(L7_enetMacAddr_t macAddr, L7_IP_ADDR_t ipAddr,
                                   L7_IP_ADDR_t switchIPAddr,
                                   L7_enetMacAddr_t switchMACAddr,
                                   L7_ushort16 vendorId, L7_ushort16 prtlVersion,
                                   L7_char8 *swVersion, L7_uint32 hwType,
                                   L7_WDM_AP_FAILURE_t failType);

/*********************************************************************
*
* @purpose  Read and clear global flag(s) to determine if any AP Failure
*           have a pending action.
*
* @param    L7_WDM_AP_FAILURE_FLAG_t  flags    @b((input))  type of pending action(s).
* @param    L7_WDM_AP_FAILURE_FLAG_t *setFlags @b((output)) flags set and cleared.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If caller doesn't care which flags were read/cleared,
*             setFlags can be passed as L7_NULLPTR.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPFailurePendingFlagsGetClear(L7_WDM_AP_FAILURE_FLAG_t  flags,
                            L7_WDM_AP_FAILURE_FLAG_t *setFlags);

/*********************************************************************
*
* @purpose  Get the next AP Failure entry flagged for a pending action.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} last AP MAC flagged
* @param    L7_WDM_AP_FAILURE_FLAG_t flag @b((input)) type of pending entry to lookup
* @param    L7_enetMacAddr_t *nextMacAddr @b{(output)} next AP MAC flagged
* @param    L7_WDM_AP_FAILURE_STATUS_t *status @b{(output)} AP failure entry status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPFailurePendingEntryNextGet(L7_enetMacAddr_t macAddr,
                            L7_WDM_MANAGED_AP_FLAG_t flag,
                            L7_enetMacAddr_t *nextMacAddr,
                            L7_WDM_MANAGED_AP_STATUS_t *status);

/*********************************************************************
*
* @purpose  Get the network IP adddress of AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_IP_ADDR_t      *ipAddr  @b{(output)} pointer to ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPFailureIPAddrGet(L7_enetMacAddr_t macAddr, L7_IP_ADDR_t *ipAddr);

/*********************************************************************
*
* @purpose  Get the IP Address of the WS who detected this AP failure latest
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_IP_ADDR_t      *switchIPAddr  @b{(output)} pointer to ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPFailureSwitchIPAddrGet(L7_enetMacAddr_t macAddr,
                            L7_IP_ADDR_t *switchIPAddr);

/*********************************************************************
*
* @purpose  Get the MAC Address of the WS who detected this AP failure latest
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_enetMacAddr_t  *switchMACAddr  @b{(output)} pointer to MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPFailureSwitchMACAddrGet(L7_enetMacAddr_t macAddr,
                            L7_enetMacAddr_t *switchMACAddr);

/*********************************************************************
*
* @purpose  Is the AP failure detected by the local switch or peer switch
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_BOOL           *apLocal  @b{(output)} pointer to is AP Local
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPFailureAPFailLocalGet(L7_enetMacAddr_t macAddr, L7_BOOL *apFailLocal);

/*********************************************************************
*
* @purpose  Get the vendor ID of AP software.
*
* @param    L7_enetMacAddr_t  macAddr    @b{(input)} AP mac address
* @param    L7_ushort16       *vendorId  @b{(output)} pointer to vendor id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPFailureVendorIdGet(L7_enetMacAddr_t macAddr, L7_ushort16 *vendorId);

/*********************************************************************
*
* @purpose  Get the protocol version supported by software on AP.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} AP mac address
* @param    L7_ushort16       *prtlVersion  @b{(output)} pointer to prtl version
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPFailureProtocolVersionGet(L7_enetMacAddr_t macAddr, L7_ushort16 *prtlVersion);

/*********************************************************************
*
* @purpose  Get the software version on AP.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} AP mac address
* @param    L7_uchar8         *swVersion  @b{(output)} pointer to sw version
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPFailureSoftwareVersionGet(L7_enetMacAddr_t macAddr, L7_uchar8 *swVersion);

/*********************************************************************
*
* @purpose  Get the hardware platform for AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uint32         *hwType  @b{(output)} pointer to hw type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPFailureHardwareTypeGet(L7_enetMacAddr_t macAddr, L7_uint32 *hwType);

/*********************************************************************
*
* @purpose  Get the last occurred failure type.
*
* @param    L7_enetMacAddr_t     macAddr  @b{(input)} AP mac address
* @param    L7_WDM_AP_FAILURE_t  *type    @b{(output)} pointer to type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPFailureTypeGet(L7_enetMacAddr_t macAddr, L7_WDM_AP_FAILURE_t *type);

/*********************************************************************
*
* @purpose  Set the number of association failures for the AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uint32         value   @b{(output)} value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPFailureValidFailSet (L7_enetMacAddr_t macAddr, L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the number of association failures for the AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uint32         *value   @b{(output)} pointer to value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPFailureValidFailGet(L7_enetMacAddr_t macAddr, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the number of authentication failures for the AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uint32         value   @b{(output)} value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPFailureAuthFailSet (L7_enetMacAddr_t macAddr, L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the number of authentication failures for the AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uint32         *value   @b{(output)} pointer to value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPFailureAuthFailGet(L7_enetMacAddr_t macAddr, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the time since failure occurred.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uint32         *age     @b{(output)} pointer to time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPFailureAgeGet(L7_enetMacAddr_t macAddr, L7_uint32 *age);

/*********************************************************************
*
* @purpose  Verify presence of a failed AP entry.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPFailureEntryGet(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Delete a failed AP entry.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPFailureEntryDelete(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Get the mac address of next failed AP entry.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} AP mac address
* @param    L7_enetMacAddr_t  *nextMacAddr  @b{(output)} pointer to next mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPFailureEntryNextGet(L7_enetMacAddr_t macAddr, L7_enetMacAddr_t *nextMacAddr);

/*********************************************************************
*
* @purpose  Purge all failed AP entries.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPFailureEntriesPurge(void);

/* End AP association and authentication failure APIs */

/* Start AP RF scan APIs */


/*********************************************************************
*
* @purpose  Find the index to insert the triangulation entry.
*
* @param    wdmRFScanTriangulationEntry_t   *entry    @b{(input)} Triangulation table
* @param    L7_enetMacAddr_t                 apMac      @b{(input)} mac of the detecting ap
* @param    L7_uchar8                        rssi       @b{(input)} rssi
*
* @returns  Returns the index where the triangulation should go.
* @returns  Returns an invalid index, WDM_RFSCAN_TRIANGULATION_ENTRIES,
*           if the table is full and all entries are stronger than the newly discovered entry.
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 wdmRfScanTriangulationEntryIndexGet(wdmRFScanTriangulationEntry_t *entry, L7_enetMacAddr_t apMac, L7_uchar8 rssi);


/*********************************************************************
*
* @purpose  Add or update an RF scan AP entry.
*
* @param    L7_enetMacAddr_t        macAddr    @b{(input)} AP mac address
* @param    wdmWsRFScanAPData_t    *pRFData   @b{(input)} scan data 
* @param    L7_WDM_RADIO_PHY_MODE_t mode       @b{(input)} mode
* @param    L7_uchar8               radioIf    @b{(input)} AP radio interface (if managed)
* @param    L7_WDM_AP_STATUS_t      status     @b{(input)} status
* @param    L7_enetMacAddr_t        apMac      @b{(input)} mac of the detecting ap
* @param    L7_uchar8               apRadio    @b{(input)} radio of the detecting ap
* @param    L7_uchar8               sentryMode @b{(input)} sentry mode of the detecting ap
* @param    L7_uchar8              peerManaged @b{(input)} 
* @param    wdmWidsWirelessThreatResults_t  *result @b{(output)} 
* @param    L7_uchar8          rogueMitigation @b{(input)} 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanEntryAddUpdate(wdmWsRFScanAPData_t     *pRFData,
                                L7_WDM_RADIO_PHY_MODE_t mode, 
                                L7_uchar8               radioIf,
                                L7_WDM_AP_STATUS_t      status,
                                L7_enetMacAddr_t        apMac,
                                L7_uchar8               apRadio,
                                L7_uchar8               sentryMode,
                                L7_uchar8               peerManaged,
                                wdmWidsWirelessThreatResults_t  *result,
                                L7_uchar8               rogueMitigation);

/*********************************************************************
*
* @purpose  Add or update an invalid channel entry.
*
* @param    wdmRFScanInvalidChannel_t   entry         @b{(input)} Invalid Channel entry
* @param    L7_BOOL                     rogueDetected @b{(input)} Invalid channel tests for rogue
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmInvalidChannelEntryAddUpdate(L7_enetMacAddr_t detectedMac,
                                        L7_enetMacAddr_t reportingMac,
                                        L7_uint32        cTime,
                                        L7_uint32        ttl,
                                        L7_uchar8        channel,
                                        L7_BOOL *rogueDetected);


/*********************************************************************
*
* @purpose  Add or update an entry in the AP de-authentication attack table.
*
* @param       entry         @b{(input)}
* @param    L7_BOOL                      @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsApAttackEntryAddUpdate(L7_enetMacAddr_t apMac,
                                      L7_uchar8        channel,
                                      L7_uint32        attackStart,
                                      L7_uint32        rfscanAge);

/*********************************************************************
*
* @purpose  Add or update an entry in the AP de-authentication attack table.
*
* @param       entry         @b{(input)}
* @param    L7_BOOL                      @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsApAttackEntryRemove(L7_enetMacAddr_t apMac);

/*********************************************************************
*
* @purpose  Remove all entries in the AP de-authentication attack table.
*
* @param       entry         @b{(input)}
* @param    L7_BOOL                      @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsApAttackEntryRemoveAll(void);


/*********************************************************************
*
* @purpose  Check to see if the  AP de-authentication attack list has
*           changed and send it to the APs if needed.
*
* @returns  none
*
* @comments none
*
* @end
*
*********************************************************************/
void wdmWidsApAttackListCheck(void);

/*********************************************************************
*
* @purpose  Get the service set ID of the detected network.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_char8          *ssid    @b{(output)} pointer to ssid
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanSSIDGet(L7_enetMacAddr_t macAddr, L7_char8 *ssid);



/*********************************************************************
*
* @purpose  Get the OUI of the detected network.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_char8          *oui    @b{(output)} pointer to oui 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanOUIParamGet(L7_enetMacAddr_t macAddr, L7_char8 *oui);


/*********************************************************************
*
* @purpose  Get the BSSID for the RF scan entry.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} MAC Address
* @param    L7_enetMacAddr_t  *bssid  @b{(output)} pointer to BSSID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanBSSIDGet(L7_enetMacAddr_t macAddr, L7_enetMacAddr_t *bssid);

/*********************************************************************
*
* @purpose  Get the 802.11 mode being used on the AP.
*
* @param    L7_enetMacAddr_t          macAddr  @b{(input)} AP mac address
* @param    L7_WDM_RADIO_PHY_MODE_t  *mode    @b{(output)} pointer to mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanPhysicalModeGet(L7_enetMacAddr_t macAddr, L7_WDM_RADIO_PHY_MODE_t *mode);

/*********************************************************************
*
* @purpose  Get the transmit channel of the AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uchar8        *channel  @b{(output)} pointer to channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanChannelGet(L7_enetMacAddr_t macAddr, L7_uchar8 *channel);

/*********************************************************************
*
* @purpose  Get the data transmit rate of the AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8        *rate     @b{(output)} pointer to rate (mult of 500 kbps)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanTxRateGet(L7_enetMacAddr_t macAddr, L7_ushort16 *rate);

/*********************************************************************
*
* @purpose  Get the beacon interval for the neighbor AP network.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_ushort16       *value   @b{(output)} pointer to value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanBeaconIntervalGet(L7_enetMacAddr_t macAddr, L7_ushort16 *value);

/*********************************************************************
*
* @purpose  Get the managed AP MAC address for an RF scan entry.
*
* @param    L7_enetMacAddr_t   macAddr  @b{(input)} AP mac address
* @param    L7_enetMacAddr_t *apMacAddr @b{(output)} pointer to AP MAC
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments AP MAC is only set if RF scan status is managed or peer managed.
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanAPMacAddrGet(L7_enetMacAddr_t macAddr, L7_enetMacAddr_t *apMacAddr);

/*********************************************************************
*
* @purpose  Get the managed AP MAC radio interface for an RF scan entry.
*
* @param    L7_enetMacAddr_t   macAddr  @b{(input)} AP mac address
* @param    L7_uchar8         *radioIf  @b{(output)} radio interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments AP MAC is only set if RF scan status is managed or peer managed.
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanAPRadioGet(L7_enetMacAddr_t macAddr, L7_uchar8 *radioIf);

/*********************************************************************
*
* @purpose  Get the managed status of AP.
*
* @param    L7_enetMacAddr_t   macAddr @b{(input)} AP mac address
* @param    L7_WDM_AP_STATUS_t *status @b{(output)} pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanStatusGet(L7_enetMacAddr_t macAddr, L7_WDM_AP_STATUS_t *status);

/*********************************************************************
*
* @purpose  Get the initial status of AP.
*
* @param    L7_enetMacAddr_t   macAddr @b{(input)} AP mac address
* @param    L7_WDM_AP_STATUS_t *status @b{(output)} pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanStatusInitialGet(L7_enetMacAddr_t macAddr, L7_WDM_AP_STATUS_t *status);

/*********************************************************************
*
* @purpose  Get the time since the AP was first detected in a scan.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uint32         *discAge  @b{(output)} pointer to age
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanDiscoveredAgeGet(L7_enetMacAddr_t macAddr, L7_uint32 *discAge);

/*********************************************************************
*
* @purpose  Get the time since the AP was last detected in a scan.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uint32         *age     @b{(output)} pointer to age
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanAgeGet(L7_enetMacAddr_t macAddr, L7_uint32 *age);

/*********************************************************************
*
* @purpose  Get the security mode of the AP.
*
* @param    L7_enetMacAddr_t    macAddr  @b{(input)} AP mac address
* @param    L7_WDM_AP_SECURITY_MODE_t  *mode  @b{(output)} pointer to security mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanSecurityGet(L7_enetMacAddr_t macAddr, L7_WDM_AP_SECURITY_MODE_t *mode);

/*********************************************************************
*
* @purpose  Get the highest supported rate advertised by this AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_char8         *rate     @b{(output)} pointer to 
*                                    highest supported rate string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanHighRateGet(L7_enetMacAddr_t macAddr, L7_char8 *rate);

/*********************************************************************
*
* @purpose  Get the 802.11n support for this AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_BOOL          *flag     @b{(output)} pointer to 802.11n flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments L7_FALSE : Not supported
*           L7_TRUE  : Supported
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanDot11nGet(L7_enetMacAddr_t macAddr, L7_BOOL *flag);

/*********************************************************************
*
* @purpose  Was the beacon frame for this AP received from an ad hoc network.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_BOOL          *flag     @b{(output)} pointer to ad hoc flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments L7_FALSE : Not Ad hoc
*           L7_TRUE  : Ad hoc
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanAdHocGet(L7_enetMacAddr_t macAddr, L7_BOOL *flag);

/*********************************************************************
*
* @purpose  Is this AP managed by a peer switch.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_BOOL          *flag     @b{(output)} pointer to peer switch flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments L7_FALSE : AP is managed by the local switch
*           L7_TRUE  : AP is managed by a peer switch
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanPeermanagedGet(L7_enetMacAddr_t macAddr, L7_BOOL *flag);

/*********************************************************************
*
* @purpose  Get the WIDS Rogue AP Mitigation Status.
*
* @param    L7_enetMacAddr_t    macAddr  @b{(input)} AP mac address
* @param    L7_WDM_WIDS_ROGUE_AP_STATUS_t  *status  @b{(output)} pointer to status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t 
wdmRFScanRogueMitigationGet(L7_enetMacAddr_t macAddr, 
                            L7_WDM_ROGUE_MITIGATION_STATUS_t *status);

/*********************************************************************
*
* @purpose  Determine if AP supports Radio Resource Measurement (RRM)
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_BOOL           *rrm     @b{(output)} flag for RRM 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t 
wdmRFScanResourceMeasSupportedGet(L7_enetMacAddr_t macAddr, 
                                  L7_BOOL *rrm);


/*********************************************************************
*
* @purpose  Verify presence of an AP entry detected in an RF scan.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanEntryGet(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Get the mac address of next AP entry detected in an RF scan.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)} AP mac address
* @param    L7_enetMacAddr_t  *nextMacAddr  @b{(output)} pointer to next mac
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanEntryNextGet(L7_enetMacAddr_t macAddr, L7_enetMacAddr_t *nextMacAddr);

/*********************************************************************
*
* @purpose  Purge all RF scan entries.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanEntriesPurge(void);

/*********************************************************************
*
* @purpose  Get the specified RF Scan Access Point Signal Triangulation entry information.
*
* @param    L7_enetMacAddr_t  macAddr               @b{(input)} AP mac address
* @param    wdmRFScanTriangulationStatus_t *status  @b{(output)} pointer to the triangulation entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanTriangulationSentryEntryGet(L7_enetMacAddr_t macAddr, L7_uint32 idx, wdmRFScanTriangulationEntry_t *entry);

/*********************************************************************
*
* @purpose  Get the next RF Scan Access Point Signal Triangulation entry information.
*
* @param    L7_enetMacAddr_t  macAddr               @b{(input)} AP mac address
* @param    wdmRFScanTriangulationStatus_t *status  @b{(output)} pointer to the triangulation entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanTriangulationSentryEntryNextGet(L7_enetMacAddr_t macAddr, L7_uint32 *idx, wdmRFScanTriangulationEntry_t *entry);

/*********************************************************************
*
* @purpose  Get the specified RF Scan Access Point Signal Triangulation entry information.
*
* @param    L7_enetMacAddr_t  macAddr               @b{(input)} AP mac address
* @param    wdmRFScanTriangulationStatus_t *status  @b{(output)} pointer to the triangulation entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanTriangulationNonSentryEntryGet(L7_enetMacAddr_t macAddr, L7_uint32 idx, wdmRFScanTriangulationEntry_t *entry);

/*********************************************************************
*
* @purpose  Get the next RF Scan Access Point Signal Triangulation entry information.
*
* @param    L7_enetMacAddr_t  macAddr               @b{(input)} AP mac address
* @param    wdmRFScanTriangulationStatus_t *status  @b{(output)} pointer to the triangulation entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanTriangulationNonSentryEntryNextGet(L7_enetMacAddr_t macAddr, L7_uint32 *idx, wdmRFScanTriangulationEntry_t *entry);

/*********************************************************************
*
* @purpose  Get the RF Scan Access Point Signal Triangulation entry information.
*
* @param    L7_enetMacAddr_t  macAddr               @b{(input)} AP mac address
* @param    wdmRFScanTriangulationStatus_t *status  @b{(output)} pointer to the triangulation entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmRFScanTriangulationStatusGet(L7_enetMacAddr_t macAddr, 
                                        wdmRFScanTriangulationStatus_t *status);


/*********************************************************************
*
* @purpose  Get the WIDS AP Rogue Classification Test Results.
*
* @param    L7_enetMacAddr_t                         macAddr @b{(input)} AP mac address
* @param    L7_WIDS_AP_ROGUE_CLASSIFICATION_TEST_t   testId  @b{(input)} rogue classification test identifier
* @param    wdmWidsRogueClassificationTestResults_t *result  @b{(output)} pointer to the classification test results
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPRogueClassificationGet(L7_enetMacAddr_t macAddr,
                                    L7_WIDS_AP_ROGUE_CLASSIFICATION_TEST_t testId,
                                    wdmWidsRogueClassificationTestResults_t *result);

/*********************************************************************
*
* @purpose  Get the WIDS AP Rogue Classification Test Description.
*
* @param    L7_WIDS_AP_ROGUE_CLASSIFICATION_TEST_t   testId  @b{(input)} rogue classification test identifier
*
* @returns  Pointer to the test description
* @returns  L7_NULLPTR for an invalid test index
*
* @comments none
*
* @end
*
*********************************************************************/
L7_char8 *wdmAPRogueClassificationDescrGet(L7_WIDS_AP_ROGUE_CLASSIFICATION_TEST_t testId);

/*********************************************************************
*
* @purpose  Get the next WIDS AP Rogue Classification Test Result.
*
* @param    L7_enetMacAddr_t                         macAddr @b{(input)} AP mac address
* @param    L7_WIDS_AP_ROGUE_CLASSIFICATION_TEST_t   testId  @b{(input)} rogue classification test identifier
* @param    wdmWidsRogueClassificationTestResults_t *result  @b{(output)} pointer to the classification test results
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPRogueClassificationNextGet(L7_enetMacAddr_t macAddr,
                                        L7_WIDS_AP_ROGUE_CLASSIFICATION_TEST_t *testId,
                                        wdmWidsRogueClassificationTestResults_t *result);
/* End AP RF scan APIs */

/*********************************************************************
* @purpose  Build and transmit the Captive Portal Command message.
*
* @param    msgType     @b{(input)} CP command message type
*           cmdData     @b{(input)} CP command message data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function will invoke the Switch Communication APIs to transmit
*           the config messages over the SSL connection.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wirelessCpCommandMessageSingle(CP_OP_t msgType, cpCmdData_t cmdData);

/*********************************************************************
* @purpose  Build and transmit the Captive Portal Command message to all
*           the managed APs for Network Configuration.
*
* @param    msgType     @b{(input)} CP command message type
*           cmdData     @b{(input)} CP command message data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function will invoke the Switch Communication APIs to transmit
*           the config messages over the SSL connection.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wirelessCpCommandMessageAllNet(CP_OP_t msgType, cpCmdData_t cmdData);

/*********************************************************************
* @purpose  Build and transmit the Captive Portal Command message to all
*           the managed APs for Global Configuration.
*
* @param    msgType     @b{(input)} CP command message type
*           cmdData     @b{(input)} CP command message data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function will invoke the Switch Communication APIs to transmit
*           the config messages over the SSL connection.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wirelessCpCommandMessageAll(CP_OP_t msgType, cpCmdData_t cmdData);

/*********************************************************************
*
* @purpose  Get client data for Captive Portal from associated client database.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} client MAC address
*           wdmCPCmdData_t  *cmdData @b{(Output)} CP command data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmCPClientDataGet(L7_enetMacAddr_t macAddr, cpCmdData_t *cmdData);

/*********************************************************************
*
* @purpose  Set value of Captive Portal Authentication flag of the client
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} client MAC address
*  @param   L7_uchar8 cpAuthFlag     @b{(input)} client CP Auth flag value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmCPClientAuthFlagSet(L7_enetMacAddr_t macAddr, L7_uchar8 cpAuthFlag);

/*********************************************************************
* @purpose  Enable or disable the Network Captive Portal Capacity.
*
* @param    cpCmd       CP Operation type
* @param    intIfNum    Internal Interface Number
* @param    redirIp     Redirect IP type and address
* @param    redirIpMask Redirect IP subnet mask
* @param    intfMac     mac address with length L7_MAC_ADDR_LEN
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wdmCpEnableDisableFeature(CP_OP_t cpCmd,
                                  L7_uint32 intIfNum,
                                  cpRedirIpAddr_t redirIp,
                                  L7_uint32 redirIpMask,
                                  L7_uchar8 intfMac[L7_ENET_MAC_ADDR_LEN]);

/*********************************************************************
* @purpose  Block or unblock user access to the network interface
*
* @param    cpCmd      CP Operation type
* @param    intIfNum   Internal Interface Number
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wdmCpBlockUnblockAccess(CP_OP_t cpCmd,
                                L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Authenticate or unauthenticate a wireless client
*
* @param    msgType       CP Operation type
* @param    clientMacAddr Client MAC address
* @param    upRate        Up Data Rate
* @param    downRate      Down Data Rate
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wdmCpAuthenUnauthenClient(CP_OP_t cpCmd,
                                  L7_enetMacAddr_t clientMacAddr,
                                  L7_uint32 upRate,
                                  L7_uint32 downRate);

/*********************************************************************
* @purpose  Deauthenticate a wireless client
*
* @param    clientMacAddr Client MAC address
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wdmCpDeauthenClient(L7_enetMacAddr_t clientMacAddr);

/*********************************************************************
* @purpose  Set the authentication port
*
* @param    intIfNum      Internal Interface Number
* @param    authPortnum   Authentication Port Number to be set
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wdmCpSetAuthPortNum(L7_uint32 intIfNum, L7_ushort16 authPortNum);

/*********************************************************************
* @purpose  Set the first optional authentication secure port
*
* @param    intIfNum            Internal Interface Number
* @param    authSecurePort1Num  Authentication Port Number to be set
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wdmCpSetAuthSecurePort1Num(L7_uint32 intIfNum, L7_ushort16 authSecurePort1Num);

/*********************************************************************
* @purpose  Set the second optional authentication secure port
*
* @param    intIfNum            Internal Interface Number
* @param    authSecurePort2Num  Authentication Port Number to be set
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wdmCpSetAuthSecurePort2Num(L7_uint32 intIfNum, L7_ushort16 authSecurePort2Num);

/*********************************************************************
* @purpose  Block or unblock user access to the network interface
*
* @param    intIfNum     Internal Interface Number
* @param    cpCapType         CP Capability Type
* @param    cpCap        CP Capability to be get
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wdmCpCapabilityGet(L7_uint32 intIfNum,
                           L7_INTF_PARM_CP_TYPES_t  cpCapType,
                           L7_uint32 *cpCap);

/*********************************************************************
* @purpose  Block or unblock user access to the network interface
*
* @param    intIfNum     Internal Interface Number
* @param    cpCapType         CP Capability Type
* @param    cpCap        CP Capability to be set
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wdmCpCapabilitySet(L7_uint32 intIfNum,
                           L7_INTF_PARM_CP_TYPES_t  cpCapType,
                           L7_uint32 cpCap);

/*********************************************************************
*
* @purpose  Get tunnel subnet associated with a managed AP VAP.
*
* @param    L7_enetMacAddr_t macAddr     @b{(input)}  base AP mac address
* @param    L7_uchar8        radioIf     @b{(input)}  radio interface
* @param    L7_enetMacAddr_t vapMacAddr  @b{(input)}  VAP MAC address
* @param    L7_IP_ADDR_t    *ip          @b{(output)} tunnel subnet IP
* @param    L7_IP_ADDR_t    *mask        @b{(output)} tunnel subnet mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is a helper function for use by the tunneling component,
*            it looks up the VAP, profile, and network IDs to get the
*            tunnel subnet parameters.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPNetworkIDGet(L7_enetMacAddr_t apMacAddr,
                                 L7_enetMacAddr_t vapMacAddr,
                                 L7_uchar8 radioIf,
                                 L7_uint32    *networkId);

/*********************************************************************
*
* @purpose  Set the Distributed Tunneling status parameters received 
*           received from the AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uint32         numTunnels  @b{(output)} Number of distributed tunnels
* @param    L7_uint32         numClientsHome  @b{(output)} Number of home clients.
* @param    L7_uint32         numClientsAssoc  @b{(output)} Number of associatin 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelAPStatusSet (L7_enetMacAddr_t *macAddr,
                                  L7_uint32 numTunnels,
                                  L7_uint32 numClientsHome,
                                  L7_uint32 numClientsAssoc);

/*********************************************************************
*
* @purpose  Get the number of clients currently using this AP as
*           Home AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uint32         *clients  @b{(output)} pointer to number of clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelAPHomeClientsGet(L7_enetMacAddr_t macAddr,
                                         L7_uint32 *clients);
/*********************************************************************
*
* @purpose  Get the number of clients currently using this AP as
*           Associate AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uint32         *clients  @b{(output)} pointer to number of clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelAPAssocClientsGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 *clients);

/*********************************************************************
*
* @purpose  Get the number of tunnels currently formed in this AP
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uint32         *tunnels  @b{(output)} pointer to number of tunnels
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelAPTunnelsFormedGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 *tunnels);

/*********************************************************************
*
* @purpose  Get the number of multicast replications being made by the
*          AP
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uint32         *val      @b{(output)} pointer to number
*                                                    of mcast replications
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelAPMcastReplicationsGet(L7_enetMacAddr_t macAddr,
                                                 L7_uint32 *val);
/*********************************************************************
*
* @purpose  Get the vlan which has the maximum number of mutlicast
*           replications for this AP
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP mac address
* @param    L7_uint32         *val      @b{(output)} pointer to vlan
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelAPMaxMcastReplicationsVlanGet(L7_enetMacAddr_t macAddr,
                                                        L7_uint32 *val);

L7_RC_t wirelessL2DistTunnelHomeApFailedHandle(L7_enetMacAddr_t macAddr);
/* End of L2 Distributed Tunneling APIs */

/*********************************************************************
*
* @purpose  Get the primary switch IP address type for managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8       *primarySWIPType  @b{(output)} pointer to IP address type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPPrimarySWIPTypeGet(L7_enetMacAddr_t macAddr,
                                       L7_uchar8 *primarySWIPType);

/*********************************************************************
*
* @purpose  Set the primary switch IP address type for managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8        primarySWIPType  @b{(output)} IP address type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPPrimarySWIPTypeSet(L7_enetMacAddr_t macAddr,
                                       L7_uchar8 primarySWIPType);

/*********************************************************************
*
* @purpose  Get the primary switch IP address for managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_IP_ADDR_t     *primarySWIP  @b{(output)} pointer to primary switch IP address 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPPrimarySWIPGet(L7_enetMacAddr_t macAddr, 
                                   L7_IP_ADDR_t *primarySWIP);

/*********************************************************************
*
* @purpose  Set the primary switch IP address for managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_IP_ADDR_t     primarySWIP  @b{(input)} primary switch IP address 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPPrimarySWIPSet(L7_enetMacAddr_t macAddr,
                                   L7_IP_ADDR_t  primarySWIP);

/*********************************************************************
*
* @purpose  Get the backup switch IP address type for managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8       *backupSWIPType  @b{(output)} pointer to IP address type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPBackupSWIPTypeGet(L7_enetMacAddr_t macAddr, 
                                      L7_uchar8 *backupSWIPType);

/*********************************************************************
*
* @purpose  Set the backup switch IP address type for managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8        backupSWIPType  @b{(output)} IP address type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPBackupSWIPTypeSet(L7_enetMacAddr_t macAddr,
                                      L7_uchar8 backupSWIPType);

/*********************************************************************
*
* @purpose  Get the backup switch IP address for managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_IP_ADDR_t     *backupSWIP  @b{(output)} pointer to backup switch IP address 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPBackupSWIPGet(L7_enetMacAddr_t macAddr, 
                                  L7_IP_ADDR_t *backupSWIP);

/*********************************************************************
*
* @purpose  Set the backup switch IP address for managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_IP_ADDR_t      backupSWIP  @b{(output)} backup switch IP address 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPBackupSWIPSet(L7_enetMacAddr_t macAddr,
                                  L7_IP_ADDR_t backupSWIP);

/*********************************************************************
*
* @purpose  Get the mutual authentication mode for managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8        *mutAuthMode  @b{(output)} Pointer to mutual authenticatio mode 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPMutualAuthModeGet(L7_enetMacAddr_t macAddr,
                                      L7_uchar8 *mutAuthMode);

/*********************************************************************
*
* @purpose  Set the mutual authentication mode for managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8        mutAuthMode  @b{(output)} Mutual authenticatio mode. 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPMutualAuthModeSet(L7_enetMacAddr_t macAddr,
                                      L7_uchar8 mutAuthMode);

/*********************************************************************
*
* @purpose  Get the AP reprovision in unmanaged mode for managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8        *reProvisioning  @b{(output)} Pointer to reprovisioning in unmanaged mode. 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPReProvisioningModeGet(L7_enetMacAddr_t macAddr,
                                          L7_uchar8 *reProvisioning);

/*********************************************************************
*
* @purpose  Set the AP reprovision in unmanaged mode for managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8        reProvisioning  @b{(output)} Reprovisioning in unmanaged mode. 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPReProvisioningModeSet(L7_enetMacAddr_t macAddr,
                                          L7_uchar8 reProvisioning);
/*********************************************************************
*
* @purpose  Get the X509 certificate for managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8        *X509Cert  @b{(output)} Pointer to X509 certificate. 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPX509CertificateGet(L7_enetMacAddr_t macAddr,
                                       L7_uchar8 *X509Cert);

/*********************************************************************
*
* @purpose  Set the X509 certificate for managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP mac address
* @param    L7_uchar8        *X509Cert  @b{(output)}  pointer to X509 certificate. 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPX509CertificateSet(L7_enetMacAddr_t macAddr,
                                          L7_uchar8 *X509Cert);
#endif /* INCLUDE_WDM_AP_API_H */

