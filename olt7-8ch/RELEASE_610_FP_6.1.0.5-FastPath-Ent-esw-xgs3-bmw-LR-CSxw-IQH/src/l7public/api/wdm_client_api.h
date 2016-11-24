/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
*
* @filename     wdm_api.h
*
* @purpose      Wireless Data Manager (WDM) API header
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
#ifndef INCLUDE_WDM_CLIENT_API_H
#define INCLUDE_WDM_CLIENT_API_H

#include "datatypes.h"
#include "wireless_commdefs.h"
#include "wireless_comm_structs.h"
#include "intf_cb_api.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "acl_exports.h"
#include "captive_portal_commdefs.h"
#include "acl_exports.h"

typedef struct wdmClientCounters64_s
{
  L7_uint64 sessCounter;    /* stats reported by previous APs for the session */
  L7_uint64 sessFinal;      /* session value reported to the user */
  L7_uint64 assocClear;     /* assoc value when stats were cleared */
  L7_uint64 assocFinal;     /* assoc value reported to the user */
} wdmClientCounters64_t;

typedef struct wdmClientCounters32_s
{
  L7_uint32 sessCounter;    /* stats reported by previous APs for the session */
  L7_uint32 sessFinal;      /* session value reported to the user */
  L7_uint32 assocClear;     /* assoc value when stats were cleared */
  L7_uint32 assocFinal;     /* assoc value reported to the user */
} wdmClientCounters32_t;

typedef struct wdmClientStatistics_s
{
  wdmClientCounters64_t bytesTx;
  wdmClientCounters64_t bytesRx;
  wdmClientCounters64_t pktsTx;
  wdmClientCounters64_t pktsRx;
  wdmClientCounters32_t dupPktsRx;
  wdmClientCounters32_t fragPktsRx;
  wdmClientCounters32_t fragPktsTx;
  wdmClientCounters32_t txRetry;
  wdmClientCounters32_t txRetryFailed;
  wdmClientCounters64_t dropBytesTx;
  wdmClientCounters64_t dropBytesRx;
  wdmClientCounters64_t dropPktsTx;
  wdmClientCounters64_t dropPktsRx;
  wdmClientCounters32_t tsViolatePktsRx;
  wdmClientCounters32_t tsViolatePktsTx;
  L7_uint32             lastUpdateTimeStamp;
  L7_char8              signal;
  L7_char8              noise;
} wdmClientStatistics_t;

typedef struct wdmClientTSStatus_s
{
  L7_uchar8             active;         /* internal flag indicating if this TS entry is active */
  L7_uchar8             tid;
  L7_uchar8             dir;            /* TS direction per L7_WIRELESS_TSPEC_DIR_t */
  L7_uchar8             psb;
  L7_uchar8             usrPri;         /* frame user priority corresponding to this TS */
  L7_ushort16           mediumTime;     /* in units of 32 usec-per-sec */
} wdmClientTSStatus_t;

typedef struct wdmClientTSStatistics_s
{
  wdmClientCounters64_t pktsRx;         /* TSpec packets received */
  wdmClientCounters64_t pktsTx;         /* TSpec packets transmitted */
  wdmClientCounters64_t bytesRx;        /* TSpec bytes received */
  wdmClientCounters64_t bytesTx;        /* TSpec bytes transmitted */
} wdmClientTSStatistics_t;

typedef struct WdmClientQosBw_s
{
  L7_uchar8                             dot1xValid; /* L7_TRUE indicates value retrieved via 802.1x,
                                                       used only for client 802.1x cache status */
  L7_uint32                             maxRate;  /* bits-per-second */
} wdmClientQosBw_t;

typedef struct WdmClientQosAcl_s
{
  L7_ACL_TYPE_t                         aclType;
  L7_char8                              aclName[L7_ACL_NAME_LEN_MAX+1];
} wdmClientQosAcl_t;

typedef struct WdmClientQosDs_s
{
  L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t   policyType;
  L7_char8                              policyName[L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX+1];
} wdmClientQosDs_t;

typedef struct WdmClientQos_s
{
  wdmClientQosBw_t                      bw[L7_WDM_CLIENT_QOS_DIR_MAX];
  #ifdef L7_ACL_PACKAGE
    wdmClientQosAcl_t                   acl[L7_WDM_CLIENT_QOS_DIR_MAX];
  #endif /* L7_ACL_PACKAGE */
  #ifdef L7_DIFFSERV_PACKAGE
    wdmClientQosDs_t                    ds[L7_WDM_CLIENT_QOS_DIR_MAX];
  #endif /* L7_DIFFSERV_PACKAGE */
} wdmClientQos_t;

/*********************************************************************
*
* @purpose  Add a new or update an existing entry in the associated client database.
*
* @param    L7_enetMacAddr_t macAddr        @b{(input)}  client MAC address
* @param    L7_WDM_CLIENT_STATUS_t status   @b{(input)}  client status  (ASSOC or AUTH).
* @param    L7_enetMacAddr_t switchMACAddr  @b{(input)}  MAC address of associated switch.
* @param    L7_IP_ADDR_t     switchIPAddr   @b{(input)}  IP address of associated switch.
* @param    L7_enetMacAddr_t apMacAddr      @b{(input)}  managed AP base MAC address.
* @param    L7_uchar8        apRadioIf      @b{(input)}  managed AP radio interface.
* @param    L7_enetMacAddr_t vapMacAddr     @b{(input)}  managed AP VAP to which client is connected.
* @param    L7_char8        *ssid           @b{(input)}  network to which client is connected.
* @param    L7_uchar8        channel        @b{(input)}  client channel.
* @param    L7_ushort16      rate           @b{(input)}  client transmit rate, multiple of 100kbps.
* @param    L7_char8        *userName       @b{(input)}  802.1x username
* @param    L7_ushort16      vlanId         @b{(input)}  assigned 802.1x VLAN (or default)
* @param    L7_char8        *pmk            @b{(input)}  802.1x preshared master key
* @param    L7_uint32        macAccepted    @b{(input)}  MAC authentication staus
* @param    L7_uint32        clientFlags    @b{(input)}  client authentication status flags
* @param    L7_uint32        reauthIntvl    @b{(input)}  802.1x reauthentication interval
* @param    L7_uchar8        homeAPFlag     @b{(input)}  Flag indicating if client is associated to home AP
* @param    L7_enetMacAddr_t homeAPMacAddr  @b{(input)}  client home AP MAC address
* @param    L7_in6_addr_t    homeAPIPv6Addr @b{(input)}  client home AP IP address
* @param    L7_WDM_CLIENT_NOTIFICATION_TYPE_t notification @b{(input)} type of notification received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This API will set the peer notification flag for new entries only.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAuthClientEntryAddUpdate(L7_enetMacAddr_t       macAddr,
                                    L7_WDM_CLIENT_STATUS_t status,
                                    L7_enetMacAddr_t       switchMacAddr,
                                    L7_IP_ADDR_t           switchIPAddr,
                                    L7_enetMacAddr_t       apMacAddr,
                                    L7_uchar8              apRadioIf,
                                    L7_enetMacAddr_t       vapMacAddr,
                                    L7_char8              *ssid,
                                    L7_uchar8              channel,
                                    L7_ushort16            rate,
                                    L7_uchar8             *userName,
                                    L7_ushort16            vlanId,
                                    L7_char8              *pmk,
                                    L7_uint32              macAccepted,
                                    L7_WDM_CLIENT_FLAG_t   clientFlags,
                                    L7_uint32              reauthIntvl,
                                    L7_uchar8              homeAPFlag,
                                    L7_enetMacAddr_t       homeAPMacAddr,
                                    L7_in6_addr_t          homeAPIPv6Addr,
                        rrmEnabCapab_t        *rrmCapab,
                                    L7_WDM_CLIENT_NOTIFICATION_TYPE_t notification);

/*********************************************************************
*
* @purpose  Set associated client IP address.
*
* @param    L7_enetMacAddr_t macAddr        @b{(input)}  client MAC address
* @param    L7_IP_ADDR_t     ip             @b{(input)}  client IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientTunnelIPAddrSet(L7_enetMacAddr_t       macAddr,
                                      L7_IP_ADDR_t           ip);

/*********************************************************************
*
* @purpose  Set associated MAC accepted status.
*
* @param    L7_enetMacAddr_t macAddr        @b{(input)}  client MAC address
* @param    L7_uint32        macAccepted    @b{(input)}  MAC accepted status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientMacAcceptedSet(L7_enetMacAddr_t       macAddr,
                                     L7_uint32              macAccepted);

/*********************************************************************
*
* @purpose  Set associated client status parameters obtained via 802.1x.
*
* @param    L7_enetMacAddr_t macAddr        @b{(input)}  client MAC address
* @param    L7_WDM_CLIENT_STATUS_t status   @b{(input)}  client status  (ASSOC or AUTH).
* @param    L7_char8        *userName       @b{(input)}  802.1x username
* @param    L7_ushort16      vlanId         @b{(input)}  assigned 802.1x VLAN (or default)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientDot1xParametersSet(L7_enetMacAddr_t       macAddr,
                                         L7_char8              *userName,
                                         L7_ushort16            vlanId);

/*********************************************************************
*
* @purpose  Set the pre-shared master key for an associated client.
*
* @param    L7_enetMacAddr_t macAddr        @b{(input)}  client MAC address
* @param    L7_WDM_CLIENT_STATUS_t status   @b{(input)}  client status  (ASSOC or AUTH).
* @param    L7_uchar8        *pmk           @b{(input)}  pre-shared master key,
*                                                        L7_WDM_CLIENT_PMK_SIZE bytes.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments update to pmk will set the L7_WDM_CLIENT_PEER_NOTIFY and
*             L7_WDM_CLIENT_PRE_AUTH_UPDATE flags.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientPreSharedMasterKeySet(L7_enetMacAddr_t       macAddr,
                                            L7_WDM_CLIENT_STATUS_t status,
                                            L7_WDM_CLIENT_FLAG_t   clientAuthFlag,
                                            L7_uchar8             *pmk);

/*********************************************************************
*
* @purpose  Get the pre-shared master key for an associated client.
*
* @param    L7_enetMacAddr_t macAddr        @b{(input)}  client MAC address
* @param    L7_uchar8        *pmk           @b{(output)}  pre-shared master key,
*                                                         L7_WDM_CLIENT_PMK_SIZE bytes.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientPreSharedMasterKeyGet(L7_enetMacAddr_t       macAddr,
                                            L7_uchar8             *pmk);

/*********************************************************************
*
* @purpose  Set transmit rate for an associated client.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)}  client MAC address
* @param    L7_ushort16       rate      @b{(input)}  client transmit rate, multiple of 100kbps.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientRateSet(L7_enetMacAddr_t       macAddr,
                              L7_ushort16            rate);

/*********************************************************************
*
* @purpose  Set 802.11n Capable for an associated client.
*
* @param    L7_enetMacAddr_t  macAddr @b{(input)} Client MAC Address
* @param    L7_uint32         dot11nCapable @b{(input)} 802.11n Capable
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientDot11nCapableSet (L7_enetMacAddr_t macAddr,
                              L7_uint32 dot11nCapable);


/*********************************************************************
*
* @purpose  Set STBC Capable for an associated client.
*
* @param    L7_enetMacAddr_t  macAddr @b{(input)} Client MAC Address
* @param    L7_uint32         stbcCapable @b{(input)} STBC Capable
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientStbcCapableSet (L7_enetMacAddr_t macAddr,
                                      L7_uint32 stbcCapable);

/*********************************************************************
*
* @purpose  Set IP Address for an associated client.
*
* @param    L7_enetMacAddr_t  macAddr @b{(input)} Client MAC Address
* @param    L7_IP_ADDR_t      ipAddr @b{(input)} IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientIPAddrSet (L7_enetMacAddr_t macAddr, L7_IP_ADDR_t ipAddr);

/*********************************************************************
*
* @purpose  Set the associated client entry status.
*
* @param    L7_enetMacAddr_t       macAddr  @b{(input)}  client MAC address
* @param    L7_WDM_CLIENT_STATUS_t status   @b{(input)}  client status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments update to L7_WDM_CLIENT_DISASSOC will start the entry age process.
*           update to L7_WDM_CLIENT_DELETED will immediately delete the entry,
*             this is only done after reading the L7_WDM_CLIENT_FORCE_DISASSOC flag
*             and the AP and peer switches have been notified.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientStatusSet(L7_enetMacAddr_t macAddr, L7_WDM_CLIENT_STATUS_t status);

/*********************************************************************
*
* @purpose  Set inactive period for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32        time         @b{(input)}  inactive period in milliseconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientInactivePeriodSet(L7_enetMacAddr_t  macAddr,
                                        L7_uint32         time);

/*********************************************************************
*
* @purpose  Set all statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    wdmClientStatistics_t *stats       @b{(input)}  pointer to all client statistics
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This API sets all values (input values are NOT added to current values).
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientStatisticsSet(L7_enetMacAddr_t       macAddr,
                                    wdmClientStatistics_t *stats);

/*********************************************************************
*
* @purpose  Get all statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    wdmClientStatistics_t *stats       @b{(output)} pointer to all client statistics
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This API sets all values (input values are NOT added to current values).
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientStatisticsGet(L7_enetMacAddr_t       macAddr,
                                    wdmClientStatistics_t *stats);

/*********************************************************************
*
* @purpose  Set TSpec statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    wdmClientTSStatistics_t *stats     @b{(input)}  pointer to client TS statistics
* @param    L7_uint32              acindex,
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This API sets all values (input values are NOT added to current values).
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientTSStatisticsSet(L7_enetMacAddr_t macAddr,
                                      L7_uint32 acindex,
                                      wdmClientTSStatistics_t *stats);

/*********************************************************************
*
* @purpose  Get Tspec statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint32              acindex     @b{(input)}  TSPEC AC index
* @param    wdmClientTSStatistics_t *stats     @b{(output)} pointer to all client statistics
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This API sets all values (input values are NOT added to current values).
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientTSStatisticsGet(L7_enetMacAddr_t macAddr,
                                      L7_uint32 acindex,
                                      wdmClientTSStatistics_t *stats);

/*********************************************************************
*
* @purpose  Set TSPEC excess usage events counter for an associated client entry.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint32              acindex     @b{(input)}  TSPEC AC index
* @param    wdmClientCounters32_t *counter     @b{(input)}  ptr to counter set contents
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Operates on the raw counter element, not just the statistic.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientTSExcessUsageEventsCounterSet(L7_enetMacAddr_t macAddr,
                                                    L7_uint32 acindex,
                                                    wdmClientCounters32_t *counter);

/*********************************************************************
*
* @purpose  Get TSPEC excess usage events counter for an associated client entry.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint32              acindex     @b{(input)}  TSPEC AC index
* @param    wdmClientCounters32_t *counter     @b{(output)} counter output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Operates on the raw counter element, not just the statistic.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientTSExcessUsageEventsCounterGet(L7_enetMacAddr_t macAddr,
                                                    L7_uint32 acindex,
                                                    wdmClientCounters32_t *counter);

/*********************************************************************
*
* @purpose  Set TSPEC excess usage first time flag for an associated client entry.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint32              acindex     @b{(input)}  TSPEC AC index
* @param    L7_uint32              value       @b{(input)}  flag value to set
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Flag must be set to either L7_FALSE or L7_TRUE.  All other values
*           are rejected.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientTSExcessUsageFirstTimeFlagSet(L7_enetMacAddr_t macAddr,
                                                    L7_uint32 acindex,
                                                    L7_uint32 value);

/*********************************************************************
*
* @purpose  Get TSPEC excess usage first time flag for an associated client entry.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint32              acindex     @b{(input)}  TSPEC AC index
* @param    L7_uint32             *value       @b{(output)} current flag value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments A value of L7_TRUE means the first excess usage event for
*           this client TS was detected.  Otherwise a value of L7_FALSE
*           is provided.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientTSExcessUsageFirstTimeFlagGet(L7_enetMacAddr_t macAddr,
                                                    L7_uint32 acindex,
                                                    L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set TSPEC excess usage clear flag for an associated client entry.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint32              acindex     @b{(input)}  TSPEC AC index
* @param    L7_uint32              value       @b{(input)}  flag value to set
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Flag must be set to either L7_FALSE or L7_TRUE.  All other values
*           are rejected.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientTSExcessUsageClearFlagSet(L7_enetMacAddr_t macAddr,
                                                L7_uint32 acindex,
                                                L7_uint32 value);

/*********************************************************************
*
* @purpose  Get TSPEC excess usage clear flag for an associated client entry.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint32              acindex     @b{(input)}  TSPEC AC index
* @param    L7_uint32             *value       @b{(output)} current flag value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments A value of L7_TRUE means an excess usage event counter clear
*           was requested but has not occurred yet.  Otherwise a value
*           of L7_FALSE is provided.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientTSExcessUsageClearFlagGet(L7_enetMacAddr_t macAddr,
                                                L7_uint32 acindex,
                                                L7_uint32 *value);

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
L7_RC_t wdmAssocClientEntryGet(L7_enetMacAddr_t macAddr);

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
L7_RC_t wdmAssocClientEntryNextGet(L7_enetMacAddr_t  macAddr,
                                   L7_enetMacAddr_t *nextMacAddr);

/*********************************************************************
*
* @purpose  Get an entry from associated client for all switches.
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
L7_RC_t wdmAuthClientEntryGet(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Get the next entry from associated client for all switches.
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
L7_RC_t wdmAuthClientEntryNextGet(L7_enetMacAddr_t  macAddr,
                                   L7_enetMacAddr_t *nextMacAddr);

/*********************************************************************
*
* @purpose  Get an entry from a local associated client for all switches.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only considers local clients.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAuthClientLocalEntryGet(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Get the next entry from local associated clients.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)}  client MAC address
* @param    L7_enetMacAddr_t *nextMacAddr @b{(output)} client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only considers local clients.
*
* @comments macAddr 00:00:00:00:00:00 returns first local entry.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAuthClientLocalEntryNextGet(L7_enetMacAddr_t  macAddr,
                                       L7_enetMacAddr_t *nextMacAddr);

/*********************************************************************
*
* @purpose  Read and clear global flag(s) to determine if any associated
*             client entries have a pending action.
*
* @param    L7_WDM_CLIENT_FLAG_t  flags    @b((input))  type of pending action(s).
* @param    L7_WDM_CLIENT_FLAG_t *setFlags @b((output)) flags set and cleared.
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
L7_RC_t wdmAssocClientPendingFlagsGetClear(L7_WDM_CLIENT_FLAG_t  flags,
                                           L7_WDM_CLIENT_FLAG_t *setFlags);

/*********************************************************************
*
* @purpose  Get the next client entry flagged for a pending action.
*
* @param    L7_enetMacAddr_t        macAddr     @b{(input)}  last client MAC address flagged
* @param    L7_WDM_CLIENT_FLAG_t    flag        @b{(input)}  type of pending flag
* @param    L7_enetMacAddr_t       *nextMacAddr @b{(output)} next client MAC address flagged
* @param    L7_WDM_CLIENT_STATUS_t *status      @b{(output)} current client entry status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments macAddr 00:00:00:00:00:00 returns first pending entry for specified flag.
*           If flag is L7_WDM_CLIENT_PEER_NOTIFY and client status is L7_WDM_CLIENT_DELETED,
*             the entry for the returned macAddr has also been deleted.
*           For all other flags and status, the flag for the return macAddr is cleared.
*
*           The client L7_WDM_CLIENT_PEER_NOTIFY flags is set in three cases:
*             1.  client entry is added (i.e. client associates to a managed AP for the first time).
*             2.  client status is L7_WDM_CLIENT_AUTH and pre-shared master key is set/updated.
*             3.  client status is L7_WDM_CLIENT_DELETED, a client entry with status L7_WDM_CLIENT_DISASSOC has aged out.
*
*           The client L7_WDM_CLIENT_PMK_UPDATE and L7_WDM_CLIENT_PRE_AUTH_UPDATE flags are set
*             when the related data fields are updated.
*
*           The client L7_WDM_CLIENT_FORCE_DISASSOC flag is set when an administrator requests for
*             the switch to force a client disassociation.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientPendingEntryNextGet(L7_enetMacAddr_t        macAddr,
                                          L7_WDM_CLIENT_FLAG_t    flag,
                                          L7_WDM_CLIENT_FLAG_t   *flagsCleared,
                                          L7_enetMacAddr_t       *nextMacAddr,
                                          L7_WDM_CLIENT_STATUS_t *status);

/*********************************************************************
*
* @purpose  Set pending flag for associated client.
*
* @param    L7_enetMacAddr_t       macAddr  @b{(input)}  client MAC address
* @param    L7_WDM_CLIENT_FLAG_t   flag     @b{(input)}  type of pending flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only required if a component needs to explicity set a flag,
*            most flags are automatically set by WDM on status changes.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientFlagSet(L7_enetMacAddr_t     macAddr,
                              L7_WDM_CLIENT_FLAG_t flag);

/*********************************************************************
*
* @purpose  Get pending flag for associated client.
*
* @param    L7_enetMacAddr_t       macAddr  @b{(input)}  client MAC address
* @param    L7_WDM_CLIENT_FLAG_t   flag     @b{(output)} returned flags
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Only required if a component needs to explicity get a flag
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientFlagGet(L7_enetMacAddr_t     macAddr,
                              L7_WDM_CLIENT_FLAG_t *flag);

/*********************************************************************
*
* @purpose  Clear pending flag for associated client.
*
* @param    L7_enetMacAddr_t       macAddr  @b{(input)}  client MAC address
* @param    L7_WDM_CLIENT_FLAG_t   flag     @b{(input)}  type of pending flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This API only needs to be called if wdmAssocClientPendingEntryNextGet
*            is not used to read the flag, wdmAssocClientPendingEntryNextGet both
*            reads and clears the flag.
*
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientFlagClear(L7_enetMacAddr_t     macAddr,
                                L7_WDM_CLIENT_FLAG_t flag);

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
L7_RC_t wdmAssocClientForceDisassoc(L7_enetMacAddr_t macAddr);

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
L7_RC_t wdmAssocClientAllForceDisassoc();

/*********************************************************************
*
* @purpose  Force switch to disassociate a client associated to the AP.
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
L7_RC_t wdmAPAssocClientsForceDisassoc(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Force switch to disassociate a client associated to the VAP.
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
L7_RC_t wdmVAPAssocClientsForceDisassoc(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Force switch to disassociate a client associated to the SSID.
*
* @param    L7_char8              *ssid  @b{(input)} SSID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE            clients is not associated.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmSSIDAssocClientsForceDisassoc(L7_char8 *ssid);

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
L7_RC_t wdmAssocClientStatusGet(L7_enetMacAddr_t        macAddr,
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
L7_RC_t wdmAssocClientLocalGet (L7_enetMacAddr_t macAddr, L7_BOOL *clientLocal);

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
L7_RC_t wdmAssocClientSwitchIPAddrGet (L7_enetMacAddr_t macAddr,
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
L7_RC_t wdmAssocClientSwitchMacAddrGet (L7_enetMacAddr_t macAddr,
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
* @comments IP address will always be 0.0.0.0 for non-tunneled clients
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientTunnelIPAddrGet(L7_enetMacAddr_t       macAddr,
                                      L7_IP_ADDR_t          *ip);

/*********************************************************************
*
* @purpose  Get MAC accepted status for an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)}  client MAC address
* @param    L7_uint32        *macAccepted    @b{(output)}  MAC accepted status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientMacAcceptedGet(L7_enetMacAddr_t       macAddr,
                                     L7_uint32             *macAccepted);

/*********************************************************************
*
* @purpose  Get AP MAC address for an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr       @b{(input)}   client MAC address
* @param    L7_enetMacAddr_t *apMacAddr     @b{(output)}  managed AP base MAC address.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientAPMacAddrGet(L7_enetMacAddr_t       macAddr,
                                   L7_enetMacAddr_t      *apMacAddr);

/*********************************************************************
*
* @purpose  Get Disassoc AP MAC address for an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr           @b{(input)}   client MAC address
* @param    L7_enetMacAddr_t *disassocApMacAddr @b{(output)}  client disassoc AP MAC address.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientDisassocAPMacAddrGet(L7_enetMacAddr_t       macAddr,
                                           L7_enetMacAddr_t      *disassocApMacAddr);

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
L7_RC_t wdmAssocClientAPMacAddrRadioGet(L7_enetMacAddr_t       macAddr,
                                        L7_enetMacAddr_t      *apMacAddr,
                                        L7_uchar8             *apRadioIf);

/*********************************************************************
*
* @purpose  Get VAP MAC address for an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)}   client MAC address
* @param    L7_enetMacAddr_t *vapMacAddr     @b{(output)}  managed AP VAP MAC address.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientVAPMacAddrGet(L7_enetMacAddr_t       macAddr,
                                    L7_enetMacAddr_t      *vapMacAddr);

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
L7_RC_t wdmAssocClientSSIDGet(L7_enetMacAddr_t       macAddr,
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
L7_RC_t wdmAssocClientChannelGet(L7_enetMacAddr_t       macAddr,
                                 L7_uchar8             *channel);

/*********************************************************************
*
* @purpose  Get transmit rate for an associated client.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)}  client MAC address
* @param    L7_ushort16      *rate      @b{(output)}  client transmit rate, multiple of 100kbps.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientRateGet(L7_enetMacAddr_t       macAddr,
                              L7_ushort16           *rate);

/*********************************************************************
*
* @purpose  Get 802.11n Capable for an associated client.
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
L7_RC_t wdmAssocClientDot11nCapableGet (L7_enetMacAddr_t macAddr,
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
L7_RC_t wdmAssocClientStbcCapableGet (L7_enetMacAddr_t macAddr,
                                      L7_uint32 *stbcCapable);

/*********************************************************************
*
* @purpose  Get IP Address for an associated client.
*
* @param    L7_enetMacAddr_t  macAddr @b{(input)} Client MAC Address
* @param    L7_IP_ADDR_t     *ipAddr @b{(output)} IP Adderss
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientIPAddrGet (L7_enetMacAddr_t macAddr, L7_IP_ADDR_t *ipAddr);

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
L7_RC_t wdmAssocClientUserNameGet(L7_enetMacAddr_t       macAddr,
                                  L7_char8              *username);

/*********************************************************************
*
* @purpose  Set VLAN assigned to an associated client.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)}  client MAC address
* @param    L7_ushort16       vlanId         @b{(output)}  assigned 802.1x VLAN (or default)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientVlanIDSet(L7_enetMacAddr_t       macAddr,
                                L7_ushort16            vlanId);

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
L7_RC_t wdmAssocClientVlanIDGet(L7_enetMacAddr_t       macAddr,
                                L7_ushort16           *vlanId);

/*********************************************************************
*
* @purpose  Get reauth interval for associated client.
*
* @param    L7_enetMacAddr_t       macAddr  @b{(input)}  client MAC address
* @param    L7_uint32             *reauthIntvl  @b{(input)}  client reauth interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientReauthIntvlGet(L7_enetMacAddr_t     macAddr,
                                     L7_uint32            *reauthIntvl);

/*********************************************************************
*
* @purpose  Set authentication interval for associated client.
*
* @param    L7_enetMacAddr_t       macAddr  @b{(input)}  client MAC address
* @param    L7_uint32              authAge  @b{(input)}  client authentication age
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientAuthAgeSet(L7_enetMacAddr_t     macAddr,
                                 L7_uint32            authAge);

/*********************************************************************
*
* @purpose  Get authentication interval for associated client.
*
* @param    L7_enetMacAddr_t       macAddr  @b{(input)}  client MAC address
* @param    L7_uint32             *authAge  @b{(input)}  client authentication age
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientAuthAgeGet(L7_enetMacAddr_t     macAddr,
                                 L7_uint32            *authAge);

/*********************************************************************
*
* @purpose  Set home AP flag for associated client.
*
* @param    L7_enetMacAddr_t       macAddr  @b{(input)}  client MAC address
* @param    L7_uchar               homeAPFlag  @b{(input)}  client home AP flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientHomeAPFlagSet(L7_enetMacAddr_t    macAddr,
                                    L7_uchar8           homeAPFlag);

/*********************************************************************
*
* @purpose  Get home AP flag for associated client.
*
* @param    L7_enetMacAddr_t       macAddr  @b{(input)}  client MAC address
* @param    L7_uchar              *homeAPFlag  @b{(input)}  client home AP flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientHomeAPFlagGet(L7_enetMacAddr_t    macAddr,
                                    L7_uchar8           *homeAPFlag);

/*********************************************************************
*
* @purpose  Set home AP MAC Address for associated client.
*
* @param    L7_enetMacAddr_t       macAddr  @b{(input)}  client MAC address
* @param    L7_enetMacAddr_t       homeAPMacAddr  @b{(input)}  client home AP MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientHomeAPMacAddrSet(L7_enetMacAddr_t     macAddr,
                                       L7_enetMacAddr_t     homeAPMacAddr);

/*********************************************************************
*
* @purpose  Get home AP MAC Address for associated client.
*
* @param    L7_enetMacAddr_t       macAddr  @b{(input)}  client MAC address
* @param    L7_enetMacAddr_t      *homeAPMacAddr  @b{(input)}  client home AP MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientHomeAPMacAddrGet(L7_enetMacAddr_t     macAddr,
                                       L7_enetMacAddr_t     *homeAPMacAddr);

/*********************************************************************
*
* @purpose  Set associate AP MAC Address for associated client.
*
* @param    L7_enetMacAddr_t       macAddr  @b{(input)}  client MAC address
* @param    L7_enetMacAddr_t       assocAPMacAddr  @b{(input)}  client assoc AP MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientAssocAPMacAddrSet(L7_enetMacAddr_t     macAddr,
                                       L7_enetMacAddr_t      assocAPMacAddr);

/*********************************************************************
*
* @purpose  Get associate AP MAC Address for associated client.
*
* @param    L7_enetMacAddr_t       macAddr  @b{(input)}  client MAC address
* @param    L7_enetMacAddr_t      *assocAPMacAddr  @b{(input)}  client assoc AP MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientAssocAPMacAddrGet(L7_enetMacAddr_t     macAddr,
                                       L7_enetMacAddr_t     *assocAPMacAddr);

/*********************************************************************
*
* @purpose  Set home AP IP Address for associated client.
*
* @param    L7_enetMacAddr_t       macAddr  @b{(input)}  client MAC address
* @param    L7_in6_addr_t          homeAPIPAddr  @b{(input)}  client home AP IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientHomeAPIPAddrSet(L7_enetMacAddr_t     macAddr,
                                      L7_in6_addr_t        homeAPIPAddr);

/*********************************************************************
*
* @purpose  Get home AP IP Address for associated client.
*
* @param    L7_enetMacAddr_t       macAddr  @b{(input)}  client MAC address
* @param    L7_in6_addr_t         *homeAPIPAddr  @b{(input)}  client home AP IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientHomeAPIPAddrGet(L7_enetMacAddr_t     macAddr,
                                      L7_in6_addr_t       *homeAPIPAddr);

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
L7_RC_t wdmAssocClientInactivePeriodGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmAssocClientAgeGet(L7_enetMacAddr_t  macAddr,
                             L7_uint32        *age);

/*********************************************************************
*
* @purpose  Get network time for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *nwtime       @b{(output)}  seconds since it is first authenticated
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientNwTimeGet(L7_enetMacAddr_t  macAddr,
                             L7_uint32        *nwtime);

/*********************************************************************
*
* @purpose  Get RRM capabilities for an associated client.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *rrmcapab     @b{(output)} RRM Capabilities
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientRrmCapabilitiesGet(L7_enetMacAddr_t  macAddr,
                             rrmEnabCapab_t        *rrmcapab);

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
L7_RC_t wdmAssocClientRoamIndicatorGet(L7_enetMacAddr_t       macAddr,
                                       L7_BOOL               *val);

/*********************************************************************
*
* @purpose  Set roam indicator for an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr @b{(input)} Client MAC Address
* @param    L7_BOOL           val     @b{(input)} Client roam indicator flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientRoamIndicatorSet(L7_enetMacAddr_t macAddr,
                                       L7_BOOL val);




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
L7_RC_t wdmAssocClientRRMSupportedGet(L7_enetMacAddr_t macAddr,
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
wdmAssocClientRRMLocationReportSupportedGet(L7_enetMacAddr_t macAddr,
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
wdmAssocClientRRMBeaconTableMeasurementSupportedGet(L7_enetMacAddr_t  macAddr,
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
wdmAssocClientRRMBeaconActiveMeasurementSupportedGet(L7_enetMacAddr_t macAddr,
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
wdmAssocClientRRMBeaconPassiveMeasurementSupportedGet(L7_enetMacAddr_t  macAddr,
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
wdmAssocClientRRMChannelLoadMeasurementSupportedGet(L7_enetMacAddr_t  macAddr,
                                                    L7_BOOL           *val);

/*********************************************************************
*
* @purpose  Creates a client TSPEC traffic stream entry for the specified
*           access category.
*
* @param    L7_enetMacAddr_t        macAddr       @b{(input)}  client MAC address
* @param    L7_uint32               acindex       @b{(input)}  access category index
* @param    L7_BOOL                 *newAlloc     @b{(output)} output ptr to new allocation flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Internal function for WDM use -- not intended to be called from
*           the user interface.
*
* @comments Allocates a new TS node to the associated client entry
*           only if one does not already exist for the AC index.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientTsEntryCreate(L7_enetMacAddr_t macAddr, L7_uint32 acindex, L7_BOOL *newAlloc);

/*********************************************************************
*
* @purpose  Deletes a client TSPEC traffic stream entry for the specified
*           access category.
*
* @param    L7_enetMacAddr_t        macAddr       @b{(input)}  client MAC address
* @param    L7_uint32               acindex       @b{(input)}  access category index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Internal function for WDM use -- not intended to be called from
*           the user interface.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientTsEntryDelete(L7_enetMacAddr_t macAddr, L7_uint32 acindex);

/*********************************************************************
*
* @purpose  Check if specified client TSPEC traffic stream entry exists.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} client MAC address
* @param    L7_uint32        acindex @b{(input)} access category index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientTsEntryGet(L7_enetMacAddr_t macAddr, L7_uint32 acindex);

/*********************************************************************
*
* @purpose  Get next client TSPEC traffic stream from the associated client database.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)}  client MAC address
* @param    L7_uint32         acindex     @b{(input)}  access category index
* @param    L7_enetMacAddr_t *nextMacAddr @b{(output)} next client MAC address
* @param    L7_uint32        *nextAcindex @b{(output)} next access category index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments macAddr 00:00:00:00:00:00 returns first entry.
*
* @comments If client macAddr exists, check if there is a next AC index on that client.
*           Otherwise, look for first supported AC index on the next client MAC address.
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientTsEntryNextGet(L7_enetMacAddr_t macAddr, L7_uint32 acindex,
                                     L7_enetMacAddr_t *nextMacAddr, L7_uint32 *nextAcindex);

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
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientTsEntryByTidGet(L7_enetMacAddr_t macAddr, L7_uint32 tid);

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
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientTsEntryByTidNextGet(L7_enetMacAddr_t macAddr, L7_uint32 tid,
                                          L7_enetMacAddr_t *nextMacAddr, L7_uint32 *nextTid);

/*********************************************************************
*
* @purpose  Get access category index from the TID for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)}  client MAC address
* @param    L7_uint32        tid     @b{(input)}  traffic stream identifier
* @param    L7_uint32        *val    @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientTsAcFromTidGet(L7_enetMacAddr_t macAddr, L7_uint32 tid, L7_uint32 *val);

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
L7_RC_t wdmAssocClientTsAnyExist(L7_enetMacAddr_t  macAddr);

/*********************************************************************
*
* @purpose  Determine if client traffic stream exists for specified access category.
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
L7_RC_t wdmAssocClientTsAcValid(L7_enetMacAddr_t  macAddr,
                                L7_uint32 acindex);

/*********************************************************************
*
* @purpose  Determine if client traffic stream is active for specified access category.
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
L7_RC_t wdmAssocClientTsAcActive(L7_enetMacAddr_t  macAddr,
                                 L7_uint32 acindex);

/*********************************************************************
*
* @purpose  Get traffic stream identifier (TID) for an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)}  client MAC address
* @param    L7_uint32         acindex        @b{(input)}  access category index
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
L7_RC_t wdmAssocClientTsTidGet(L7_enetMacAddr_t       macAddr,
                               L7_uint32              acindex,
                               L7_uchar8             *val);

/*********************************************************************
*
* @purpose  Get traffic stream direction for an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)}  client MAC address
* @param    L7_uint32         acindex        @b{(input)}  access category index
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
L7_RC_t wdmAssocClientTsDirectionGet(L7_enetMacAddr_t       macAddr,
                                     L7_uint32              acindex,
                                     L7_uchar8             *val);

/*********************************************************************
*
* @purpose  Get traffic stream power save bahavior (PSB) for an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)}  client MAC address
* @param    L7_uint32         acindex        @b{(input)}  access category index
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
L7_RC_t wdmAssocClientTsPsbGet(L7_enetMacAddr_t       macAddr,
                               L7_uint32              acindex,
                               L7_uchar8             *val);

/*********************************************************************
*
* @purpose  Get traffic stream user priority for an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)}  client MAC address
* @param    L7_uint32         acindex        @b{(input)}  access category index
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
L7_RC_t wdmAssocClientTsUserPriorityGet(L7_enetMacAddr_t       macAddr,
                                        L7_uint32              acindex,
                                        L7_uchar8             *val);

/*********************************************************************
*
* @purpose  Get traffic stream medium time for an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)}  client MAC address
* @param    L7_uint32         acindex        @b{(input)}  access category index
* @param    L7_ushort16      *val            @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientTsMediumTimeGet(L7_enetMacAddr_t       macAddr,
                                      L7_uint32              acindex,
                                      L7_ushort16           *val);

/*********************************************************************
*
* @purpose  Get current traffic stream status parameters for an associated client entry.
*
* @param    L7_enetMacAddr_t      macAddr       @b{(input)}  client MAC address
* @param    L7_uint32             acindex       @b{(input)}  access category index
* @param    wdmClientTSStatus_t   *pTsStatus    @b{(output)} TS status values to update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is intended for internal use by the TSPEC application.  It is
*           not meant to be called via the user interface (USMDB).
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientTsStatusGet(L7_enetMacAddr_t       macAddr,
                                  L7_uint32              acindex,
                                  wdmClientTSStatus_t    *pTsStatus);

/*********************************************************************
*
* @purpose  Set all traffic stream status parameters for an associated client entry.
*
* @param    L7_enetMacAddr_t      macAddr       @b{(input)}  client MAC address
* @param    L7_uint32             acindex       @b{(input)}  access category index
* @param    wdmClientTSStatus_t   *pTsStatus    @b{(input)}  TS status values to update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is intended for internal use by the TSPEC application.  It is
*           not meant to be called via the user interface (USMDB).
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientTsStatusSet(L7_enetMacAddr_t       macAddr,
                                  L7_uint32              acindex,
                                  wdmClientTSStatus_t    *pTsStatus);

/*********************************************************************
*
* @purpose  Get traffic stream roam indicator for an associated client entry.
*
* @param    L7_enetMacAddr_t  macAddr        @b{(input)}  client MAC address
* @param    L7_uint32         acindex        @b{(input)}  access category index
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
L7_RC_t wdmAssocClientTsRoamIndicatorGet(L7_enetMacAddr_t       macAddr,
                                         L7_uint32              acindex,
                                         L7_BOOL               *val);

/*********************************************************************
*
* @purpose  Set network time for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32        nwtime       @b{(input)}  seconds since it is first authenticated
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientNwTimeSet(L7_enetMacAddr_t  macAddr,
                             L7_uint32        nwtime);

/*********************************************************************
*
* @purpose  Get authentication client QoS operational status for associated client.
*
* @param    L7_enetMacAddr_t       macAddr  @b{(input)}  client MAC address
* @param    L7_uint32             *stat     @b{(output)} client QoS operational stat
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientQosOperStatusGet(L7_enetMacAddr_t     macAddr,
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
L7_RC_t wdmAssocClientQosAclGet(L7_enetMacAddr_t               macAddr,
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
L7_RC_t wdmAssocClientQosBandwidthGet(L7_enetMacAddr_t               macAddr,
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
L7_RC_t wdmAssocClientQosPolicyGet(L7_enetMacAddr_t                     macAddr,
                                   L7_WDM_CLIENT_QOS_PARAM_TYPE_t       type,
                                   L7_WDM_CLIENT_QOS_DIR_t              dir,
                                   wdmClientQosDs_t                    *policy);

/*********************************************************************
*
* @purpose  Set authenticated client QoS parameters.
*
* @param    L7_enetMacAddr_t                macAddr     @b{(input)}  client MAC address
* @param    L7_WDM_CLIENT_QOS_PARAM_TYPE_t  type        @b{(input)}  status parameter type
* @param    wdmClientQos_t                 *clientQos   @b{(input)}  all client qos params
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  used by application layer to set all QoS params from AP and peer switch
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientQosSet(L7_enetMacAddr_t               macAddr,
                             L7_WDM_CLIENT_QOS_PARAM_TYPE_t type,
                             wdmClientQos_t                *clientQos);

/*********************************************************************
*
* @purpose  Get authenticated client QoS parameters.
*
* @param    L7_enetMacAddr_t                macAddr     @b{(input)}  client MAC address
* @param    L7_WDM_CLIENT_QOS_PARAM_TYPE_t  type        @b{(input)}  status parameter type
* @param    wdmClientQos_t                 *clientQos   @b{(output)} all client qos params
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments used by application layer to get all QoS params to send AP and peer switch
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientQosGet(L7_enetMacAddr_t               macAddr,
                             L7_WDM_CLIENT_QOS_PARAM_TYPE_t type,
                             wdmClientQos_t                *clientQos);

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
L7_RC_t wdmAssocClientSessionStatisticsPktsRxGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmAssocClientSessionStatisticsBytesRxGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmAssocClientSessionStatisticsPktsTxGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmAssocClientSessionStatisticsBytesTxGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmAssocClientSessionStatisticsDropPktsRxGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmAssocClientSessionStatisticsDropBytesRxGet(L7_enetMacAddr_t  macAddr,
                                                      L7_uint64        *bytes);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
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
L7_RC_t wdmAssocClientSessionStatisticsDropPktsTxGet(L7_enetMacAddr_t  macAddr,
                                                     L7_uint64        *pkts);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint64       *bytes        @b{(output)} dropped transmit bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientSessionStatisticsDropBytesTxGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmAssocClientSessionStatisticsDupPktsRxGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmAssocClientSessionStatisticsFragPktsRxGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmAssocClientSessionStatisticsFragPktsTxGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmAssocClientSessionStatisticsTxRetryGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmAssocClientSessionStatisticsTxRetryFailedGet(L7_enetMacAddr_t  macAddr,
                                                        L7_uint32        *count);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *pkts         @b{(output)} packets received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientSessionStatisticsTsViolatePktsRxGet(L7_enetMacAddr_t  macAddr,
                                                          L7_uint32        *pkts);

/*********************************************************************
*
* @purpose  Get session statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *pkts         @b{(output)} packets received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientSessionStatisticsTsViolatePktsTxGet(L7_enetMacAddr_t  macAddr,
                                                          L7_uint32        *pkts);

/*********************************************************************
*
* @purpose  Get Tspec statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint32              acindex     @b{(input)}  TSPEC AC index
* @param    L7_uint64             *pkts        @b{(output)} packet count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientSessionStatisticsTsPktsRxGet(L7_enetMacAddr_t macAddr,
                                                   L7_uint32 acindex,
                                                   L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get Tspec statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint32              acindex     @b{(input)}  TSPEC AC index
* @param    L7_uint64             *pkts        @b{(output)} packet count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientSessionStatisticsTsPktsTxGet(L7_enetMacAddr_t macAddr,
                                                   L7_uint32 acindex,
                                                   L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get Tspec statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint32              acindex     @b{(input)}  TSPEC AC index
* @param    L7_uint64             *bytes       @b{(output)} byte count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientSessionStatisticsTsBytesRxGet(L7_enetMacAddr_t macAddr,
                                                    L7_uint32 acindex,
                                                    L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get Tspec statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint32              acindex     @b{(input)}  TSPEC AC index
* @param    L7_uint64             *bytes       @b{(output)} byte count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientSessionStatisticsTsBytesTxGet(L7_enetMacAddr_t macAddr,
                                                    L7_uint32 acindex,
                                                    L7_uint64 *bytes);

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
L7_RC_t wdmAssocClientStatisticsPktsRxGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmAssocClientStatisticsBytesRxGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmAssocClientStatisticsPktsTxGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmAssocClientStatisticsBytesTxGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmAssocClientStatisticsDropPktsRxGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmAssocClientStatisticsDropBytesRxGet(L7_enetMacAddr_t  macAddr,
                                               L7_uint64        *bytes);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
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
L7_RC_t wdmAssocClientStatisticsDropPktsTxGet(L7_enetMacAddr_t  macAddr,
                                              L7_uint64        *pkts);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint64       *bytes        @b{(output)} dropped transmit bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientStatisticsDropBytesTxGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmAssocClientStatisticsDupPktsRxGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmAssocClientStatisticsFragPktsRxGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmAssocClientStatisticsFragPktsTxGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmAssocClientStatisticsTxRetryGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmAssocClientStatisticsTxRetryFailedGet(L7_enetMacAddr_t  macAddr,
                                                 L7_uint32        *count);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *pkts         @b{(output)} packets received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientStatisticsTsViolatePktsRxGet(L7_enetMacAddr_t  macAddr,
                                                   L7_uint32        *pkts);

/*********************************************************************
*
* @purpose  Get statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
* @param    L7_uint32       *pkts         @b{(output)} packets received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientStatisticsTsViolatePktsTxGet(L7_enetMacAddr_t  macAddr,
                                                   L7_uint32        *pkts);

/*********************************************************************
*
* @purpose  Get Tspec statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint32              acindex     @b{(input)}  TSPEC AC index
* @param    L7_uint64             *pkts        @b{(output)} pointer pkt count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientStatisticsTsPktsRxGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 acindex,
                                            L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get Tspec statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint32              acindex     @b{(input)}  TSPEC AC index
* @param    L7_uint64             *pkts        @b{(output)} pointer pkt count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientStatisticsTsPktsTxGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 acindex,
                                            L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get Tspec statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint32              acindex     @b{(input)}  TSPEC AC index
* @param    L7_uint64             *bytes       @b{(output)} byte count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientStatisticsTsBytesRxGet(L7_enetMacAddr_t macAddr,
                                             L7_uint32 acindex,
                                             L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get Tspec statistics for an associated client entry.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint32              acindex     @b{(input)}  TSPEC AC index
* @param    L7_uint64             *pkts        @b{(output)} pointer pkt count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientStatisticsTsBytesTxGet(L7_enetMacAddr_t macAddr,
                                             L7_uint32 acindex,
                                             L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get TSPEC excess usage event statistic for an associated client entry.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint32              acindex     @b{(input)}  TSPEC AC index
* @param    L7_uint32             *count       @b{(output)} current counter value
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
L7_RC_t wdmAssocClientStatisticsTsExcessUsageEventsGet(L7_enetMacAddr_t macAddr,
                                                       L7_uint32 acindex,
                                                       L7_uint32 *count);

/*********************************************************************
*
* @purpose  Purge ALL associated client entries, regardless of state.
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
L7_RC_t wdmAssocClientEntriesPurge();


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
L7_RC_t wdmAssocClientCPAuthFlagSet(L7_enetMacAddr_t macAddr,
                                    L7_uint32 cpAuthFlag);

/*********************************************************************
*
* @purpose  Set the NetBios Name for an associated client.
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
L7_RC_t wdmAssocClientNetBiosNameSet(L7_enetMacAddr_t       macAddr,
                                     L7_uchar8             *nbName);

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
L7_RC_t wdmAssocClientCPAuthFlagGet(L7_enetMacAddr_t macAddr,
                                    L7_uint32 *cpAuthFlag);

/*********************************************************************
*
* @purpose  Purge ALL associated client entries for a manged AP, regardless of state.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  AP MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is called when the AP moves from L7_WDM_MANAGED_AP_MANAGED
*           state to L7_WDM_MANAGED_AP_FAILED state. This removes all associated
*           clients from the Associated client database, SSID /VAP mapping
*           databases and resets the association flag for the client in the
*           AP neighbor database.
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPAssocClientEntriesPurge(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Add or update an entry in the Ad Hoc client network list.
*
* @param    L7_enetMacAddr_t macAddr     @b{(input)}  client MAC address
* @param    L7_enetMacAddr_t apMacAddr   @b{(input)}  managed AP MAC address.
* @param    L7_enetMacAddr_t apRadioIf   @b{(input)}  managed AP radio interface.
* @param    L7_WDM_ADHOC_CLIENT_DETECTION_MODE_t mode @b{(input)} detection mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If entry exists, age is updated appropriately.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAdHocClientEntryAddUpdate(L7_enetMacAddr_t   macAddr,
                                     L7_enetMacAddr_t   apMacAddr,
                                     L7_uchar8          apRadioIf,
                                     L7_WDM_ADHOC_CLIENT_DETECTION_MODE_t mode);

/*********************************************************************
*
* @purpose  Determine if an entry exists in the Ad Hoc client network list.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAdHocClientEntryGet(L7_enetMacAddr_t  macAddr);

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
L7_RC_t wdmAdHocClientEntryNextGet(L7_enetMacAddr_t   macAddr,
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
L7_RC_t wdmAdHocClientAPMACRadioGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t wdmAdHocClientDetectionModeGet(L7_enetMacAddr_t macAddr,
                                       L7_WDM_ADHOC_CLIENT_DETECTION_MODE_t *mode);

/*********************************************************************
*
* @purpose  Get age for an entry in the Ad Hoc client network list.
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
L7_RC_t wdmAdHocClientAgeGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmAdHocClientEntriesPurge();

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
L7_RC_t wdmSSIDAssocClientEntryGet(L7_char8          *ssid,
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
L7_RC_t wdmSSIDAssocClientEntryNextGet(L7_char8          *ssid,
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
L7_RC_t wdmVAPAssocClientEntryGet(L7_enetMacAddr_t  vapMacAddr,
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
L7_RC_t wdmVAPAssocClientEntryNextGet(L7_enetMacAddr_t   vapMacAddr,
                                      L7_enetMacAddr_t   clientMacAddr,
                                      L7_enetMacAddr_t  *nextVAPMacAddr,
                                      L7_enetMacAddr_t  *nextClientMacAddr);

/*********************************************************************
*
* @purpose  Determine if an entry exists switch-associated client mapping.
*
* @param    L7_IP_ADDR_t switchIPAddr     @b{(input)}  IP address of associated switch
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
L7_RC_t wdmSwitchAssocClientEntryGet(L7_IP_ADDR_t switchIPAddr,
                                  L7_enetMacAddr_t  clientMacAddr);

/*********************************************************************
*
* @purpose  Get next entry in the switch-associated client mapping.
*
* @param    L7_IP_ADDR_t switchIPAddr     @b{(input)}  switch IP address
* @param    L7_enetMacAddr_t clientMacAddr  @b{(input)}  associated client MAC address
* @param    L7_IP_ADDR_t nextSwitchIPAddr     @b{(output)}  next switch's IP address
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
L7_RC_t wdmSwitchAssocClientEntryNextGet(L7_IP_ADDR_t switchIPAddr,
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
L7_RC_t wdmClientManagedAPNbrEntryGet(L7_enetMacAddr_t  macAddr,
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
L7_RC_t wdmClientManagedAPNbrEntryNextGet(L7_enetMacAddr_t   macAddr,
                                          L7_enetMacAddr_t   apMacAddr,
                                          L7_uchar8          apRadioIf,
                                          L7_enetMacAddr_t  *nextMacAddr,
                                          L7_enetMacAddr_t  *nextAPMacAddr,
                                          L7_uchar8         *nextAPRadioIf);

/*********************************************************************
*
* @purpose  Get Client Associated Network Interface Number
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} client MAC address
* @param    L7_uint32        intfNum @b{(output)} Network Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmCPClientIfGet(L7_enetMacAddr_t macAddr, L7_uint32 *intfNum);


/*********************************************************************
*
* @purpose  Get Client Captive Portal Statistics
*
* @param    L7_enetMacAddr_t macAddrClient @b{(input)} client MAC address
* @param    txRxCount_t      clientCounter @b{(output)} client stats
* @param    L7_uint32        timePassed @b{(output)} time passed since
*                                                    last update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmCPClientStatisticsGet(L7_enetMacAddr_t macAddrClient,
                                 txRxCount_t *clientCounter,
                                 L7_uint32 *timePassed);


/*********************************************************************
*
* @purpose  Get home AP IP Address for associated client.
*
* @param    L7_enetMacAddr_t       macAddr  @b{(input)}  client MAC address
* @param    L7_enetMacAddr_t      *homeAPIPAddr  @b{(input)}  client home AP IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*********************************************************************/

L7_RC_t wdmAssocClientManagedAPIPAddrGet(L7_enetMacAddr_t     macAddr,
                                         L7_IP_ADDR_t        *homeAPIPAddr);

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
L7_RC_t wdmAssocClientDistTunnelStatusGet (L7_enetMacAddr_t macAddr,
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
L7_RC_t wdmAssocClientDistTunnelRoamStatusGet (L7_enetMacAddr_t macAddr,
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
L7_RC_t wdmAssocClientDistTunnelHomeAPMacGet (L7_enetMacAddr_t macAddr,
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
L7_RC_t wdmAssocClientDistTunnelAssocAPMacGet (L7_enetMacAddr_t macAddr,
                                               L7_enetMacAddr_t *apMacAddr);
/*********************************************************************
*
* @purpose  Get Client Associated Network Subnet
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} client MAC address
* @param    L7_uint32        managedAPSubnet @b{(output)} Managed AP subnet
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAssocClientManagedAPSubnetGet(L7_enetMacAddr_t macAddr,
                                         L7_IP_ADDR_t     *subnet);

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
L7_RC_t wdmAssocClientNetBiosNameGet(L7_enetMacAddr_t       macAddr,
                                     L7_uchar8             *nbName);

#endif /* INCLUDE_WDM_CLIENT_API_H */
