/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   wdm_ap_statistics_api.h
*
* @purpose    Wireless Data Manager Access Point (WDM AP) Statistics API header
*
* @component  WDM
*
* @comments   none
*
* @create     1/25/2006
*
* @author     rjindal
*
* @end
*
*********************************************************************/

#ifndef INCLUDE_WDM_AP_STATISTICS_API_H
#define INCLUDE_WDM_AP_STATISTICS_API_H

#include "datatypes.h"
#include "wireless_commdefs.h"
#include "wdm_api.h"

/****************************************
*   Managed AP Statistics
****************************************/
typedef struct wdmManagedAPStatistics_s
{
  wdmCounters64_t         pktsRx;                 /* WLAN pkts received */
  wdmCounters64_t         bytesRx;                /* WLAN bytes received */
  wdmCounters64_t         pktsTx;                 /* WLAN pkts transmitted */
  wdmCounters64_t         bytesTx;                /* WLAN btyes transmitted */
  wdmCounters64_t         ethPktsRx;              /* Ethernet pkts received */
  wdmCounters64_t         ethBytesRx;             /* Ethernet btyes received */
  wdmCounters64_t         ethMultPktsRx;          /* Ethernet multicast pkts received */
  wdmCounters64_t         ethPktsTx;              /* Ethernet pkts transmitted */
  wdmCounters64_t         ethBytesTx;             /* Ethernet btyes transmitted */
  wdmCounters64_t         ethTxErrorCount;        /* Total transmit errors */
  wdmCounters64_t         ethRxErrorCount;        /* Total receive errors */
  wdmCounters64_t         dropPktsRx;             /* Dropped WLAN pkts received */
  wdmCounters64_t         dropBytesRx;            /* Dropped WLAN bytes received */
  wdmCounters64_t         dropPktsTx;             /* Dropped WLAN pkts transmitted */
  wdmCounters64_t         dropBytesTx;            /* Dropped WLAN btyes transmitted */
  wdmCounters64_t         cl2tunnelBytesRx;       /* Bytes received */
  wdmCounters64_t         cl2tunnelPktsRx;        /* Packets received */
  wdmCounters64_t         cl2tunnelMcastRx;       /* Multicast packets received */
  wdmCounters64_t         cl2tunnelBytesTx;       /* Bytes transmitted */
  wdmCounters64_t         cl2tunnelPktsTx;        /* Packets transmitted */
  wdmCounters64_t         cl2tunnelMcastTx;       /* Multicast packets transmitted */
  wdmCounters32_t         dl2TunnelMaxTunnels;
  wdmCounters32_t         dl2TunnelVlanMaxTunnels;
  wdmCounters64_t         dl2TunnelBytesTx;       /* Total bytes Tx via all dist tunnels*/
  wdmCounters64_t         dl2TunnelBytesRx;       /* Total bytes Rx via all dist tunnels*/
  wdmCounters64_t         dl2TunnelPktsTx;        /* Total pkts Tx via all dist tunnels */
  wdmCounters64_t         dl2TunnelPktsRx;        /* Total pkts Rx via all dist tunnels*/
  wdmCounters64_t         dl2TunnelMcastPktsTx;   /* Total Macst pkts Tx via all dist tunnels.*/
  wdmCounters64_t         dl2TunnelMcastPktsRx;   /* Total Macst pkts Rx via all dist tunnels.*/
  wdmCounters32_t         dl2TunnelRoamedClients;
  wdmCounters32_t         dl2TunnelIdleTimedoutRoamedClients; /* Total roamed DL2 clients timed out due to idle time */
  wdmCounters32_t         dl2TunnelAgeTimedoutRoamedClients;  /* Total roamed DL2 clients timed out due to total age time */
  wdmCounters32_t         dl2TunnelDeniesMaxClientLimit;      /* Total DL2 clients denied tunnel setup due to max client limit */
  wdmCounters32_t         dl2TunnelDeniesMaxRepl;             /* Total DL2 clients denied tunnel setup due to max multicast replications */
  wdmTspecGenStats_t      tsSumStats[WDM_TSPEC_TRAFFIC_AC_MAX];     /* TSPEC summary AP statistics */
  wdmTspecTrafficStats_t  tsTrafficStats[WDM_TSPEC_TRAFFIC_AC_MAX]; /* TSPEC traffic AP statistics */
} wdmManagedAPStatistics_t;

typedef struct WdmManagedAPStatistics2_s
{
  wdmCounters32_t         arpSuppBcastToUcast;
  wdmCounters32_t         arpReqsFiltered;
  wdmCounters32_t         bcastARPReqs;
} wdmManagedAPStatistics2_t;

/****************************************
*   AP Radio Statistics
****************************************/
typedef struct wdmManagedAPRadioStatistics_s
{
  wdmCounters64_t         pktsRx;                 /* WLAN pkts received */
  wdmCounters64_t         bytesRx;                /* WLAN bytes received */
  wdmCounters64_t         pktsTx;                 /* WLAN pkts transmitted */
  wdmCounters64_t         bytesTx;                /* WLAN btyes transmitted */
  wdmCounters32_t         txFragmentCount;        /* Fragments transmitted */
  wdmCounters32_t         mcastTxFrameCount;      /* Multicast frames transmitted */
  wdmCounters32_t         failedCount;            /* Tx failed count */
  wdmCounters32_t         retryCount;             /* Tx retry count */
  wdmCounters32_t         multRetryCount;         /* Multiple retry count */
  wdmCounters32_t         frameDupCount;          /* Duplicate frame count */
  wdmCounters32_t         rtsSuccessCount;        /* RTS success count */
  wdmCounters32_t         rtsFailureCount;        /* RTS failure count */
  wdmCounters32_t         ackFailureCount;        /* ACK failure count */
  wdmCounters32_t         rxFragmentCount;        /* Fragments received */
  wdmCounters32_t         mcastRxFrameCount;      /* Multicast frames received */
  wdmCounters32_t         fcsErrorCount;          /* FCS error count */
  wdmCounters32_t         txFrameCount;           /* Frames transmitted */
  wdmCounters32_t         wepUndecryptCount;      /* WEP undecryptable count */
  wdmCounters64_t         dropPktsRx;             /* Dropped WLAN pkts received */
  wdmCounters64_t         dropBytesRx;            /* Dropped WLAN bytes received */
  wdmCounters64_t         dropPktsTx;             /* Dropped WLAN pkts transmitted */
  wdmCounters64_t         dropBytesTx;            /* Dropped WLAN btyes transmitted */
  wdmTspecTrafficStats_t  tspecStats[WDM_TSPEC_TRAFFIC_AC_MAX];
} wdmManagedAPRadioStatistics_t;

/****************************************
*   AP Radio VAP Statistics
****************************************/
typedef struct wdmManagedAPVAPStatistics_s
{
  wdmCounters64_t           pktsRx;               /* WLAN pkts received */
  wdmCounters64_t           bytesRx;              /* WLAN bytes received */
  wdmCounters64_t           pktsTx;               /* WLAN pkts transmitted */
  wdmCounters64_t           bytesTx;              /* WLAN btyes transmitted */
  wdmCounters32_t           assocFailures;        /* Client association failures */
  wdmCounters32_t           authFailures;         /* Client authentication failures */
  wdmCounters64_t           dropPktsRx;           /* Dropped WLAN pkts received */
  wdmCounters64_t           dropBytesRx;          /* Dropped WLAN bytes received */
  wdmCounters64_t           dropPktsTx;           /* Dropped WLAN pkts transmitted */
  wdmCounters64_t           dropBytesTx;          /* Dropped WLAN btyes transmitted */
  wdmTspecTrafficStats_t    tspecStats[WDM_TSPEC_TRAFFIC_AC_MAX];
} wdmManagedAPVAPStatistics_t;



/* Following APIs can be called via usmDb and wireless application components. */


/* Start per managed AP statistics APIs */

/*********************************************************************
*
* @purpose  Set all statistics for managed AP entry.
*
* @param    L7_enetMacAddr_t         macAddr @b{(input)} AP MAC address
* @param    wdmManagedAPStatistics_t *stats  @b{(input)} per AP statistics
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This API sets all values (input values are NOT added to current values).
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatisticsSet(L7_enetMacAddr_t macAddr,
                                  wdmManagedAPStatistics_t *stats);

/*********************************************************************
*
* @purpose  Get all statistics for managed AP entry.
*
* @param    L7_enetMacAddr_t         macAddr @b{(input)} AP MAC address
* @param    wdmManagedAPStatistics_t *stats  @b{(output)} pointer to per AP statistics
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatisticsGet(L7_enetMacAddr_t macAddr,
                                  wdmManagedAPStatistics_t *stats);

/*********************************************************************
*
* @purpose  Set all statistics-2 for managed AP entry.
*
* @param    L7_enetMacAddr_t         macAddr @b{(input)} AP MAC address
* @param    wdmManagedAPStatistics2_t *stats  @b{(input)} per AP statistics
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This API sets all values (input values are NOT added to current values).
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatistics2Set(L7_enetMacAddr_t macAddr,
                            wdmManagedAPStatistics2_t *stats);

/*********************************************************************
*
* @purpose  Get all statistics-2 for managed AP entry.
*
* @param    L7_enetMacAddr_t         macAddr @b{(input)} AP MAC address
* @param    wdmManagedAPStatistics2_t *stats  @b{(output)} pointer to per AP statistics
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatistics2Get(L7_enetMacAddr_t macAddr,
                            wdmManagedAPStatistics2_t *stats);

/*********************************************************************
*
* @purpose  Get the number of WLAN packets received on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *pktsRx  @b{(output)} pointer to packets received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatPktsRxGet(L7_enetMacAddr_t macAddr, L7_uint64 *pktsRx);

/*********************************************************************
*
* @purpose  Get the number of WLAN bytes received on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP MAC address
* @param    L7_uint64         *bytesRx  @b{(output)} pointer to bytes received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatBytesRxGet(L7_enetMacAddr_t macAddr, L7_uint64 *bytesRx);

/*********************************************************************
*
* @purpose  Get the number of WLAN packets transmitted from managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *pktsTx  @b{(output)} pointer to packets transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatPktsTxGet(L7_enetMacAddr_t macAddr, L7_uint64 *pktsTx);

/*********************************************************************
*
* @purpose  Get the number of WLAN bytes transmitted from managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP MAC address
* @param    L7_uint64         *bytesTx  @b{(output)} pointer to bytes transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatBytesTxGet(L7_enetMacAddr_t macAddr, L7_uint64 *bytesTx);

/*********************************************************************
*
* @purpose  Get the number of WLAN dropped receive packets on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *pkts    @b{(output)} pointer to dropped receive packets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatDropPktsRxGet(L7_enetMacAddr_t macAddr, L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the number of WLAN dropped receive bytes on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP MAC address
* @param    L7_uint64         *bytes    @b{(output)} pointer to dropped receive bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatDropBytesRxGet(L7_enetMacAddr_t macAddr, L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the number of WLAN dropped transmit packets from managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *pkts    @b{(output)} pointer to dropped transmit packets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatDropPktsTxGet(L7_enetMacAddr_t macAddr, L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the number of WLAN dropped transmit bytes from managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP MAC address
* @param    L7_uint64         *bytes    @b{(output)} pointer to dropped transmit bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatDropBytesTxGet(L7_enetMacAddr_t macAddr, L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the number of ethernet packets received on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *value   @b{(output)} pointer to pkts received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatEthPktsRxGet(L7_enetMacAddr_t macAddr, L7_uint64 *value);

/*********************************************************************
*
* @purpose  Get the number of ethernet bytes received on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *value   @b{(output)} pointer to bytes received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatEthBytesRxGet(L7_enetMacAddr_t macAddr, L7_uint64 *value);

/*********************************************************************
*
* @purpose  Get the number of ethernet multicast pkts received on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *value   @b{(output)} pointer to pkts received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatEthMultPktsRxGet(L7_enetMacAddr_t macAddr, L7_uint64 *value);

/*********************************************************************
*
* @purpose  Get the number of ethernet pkts transmitted on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *value   @b{(output)} pointer to pkts transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatEthPktsTxGet(L7_enetMacAddr_t macAddr, L7_uint64 *value);

/*********************************************************************
*
* @purpose  Get the number of ethernet bytes transmitted on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *value   @b{(output)} pointer to bytes transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatEthBytesTxGet(L7_enetMacAddr_t macAddr, L7_uint64 *value);

/*********************************************************************
*
* @purpose  Get the number of total transmit errors on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *value   @b{(output)} pointer to error count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatTotalTxErrorsGet(L7_enetMacAddr_t macAddr, L7_uint64 *value);

/*********************************************************************
*
* @purpose  Get the number of total receive errors on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *value   @b{(output)} pointer to error count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatTotalRxErrorsGet(L7_enetMacAddr_t macAddr, L7_uint64 *value);

/* CL2 TUNNEL STATS Begin */
/*********************************************************************
*
* @purpose  Get the number of Bytes received on managed AP L2 tunnel.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *value   @b{(output)} pointer to error count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatCL2TunnelBytesRxGet(L7_enetMacAddr_t macAddr, L7_uint64 *value);

/*********************************************************************
*
* @purpose  Get the number of Packets received on managed AP L2 tunnel.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *value   @b{(output)} pointer to error count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatCL2TunnelPktsRxGet(L7_enetMacAddr_t macAddr, L7_uint64 *value);

/*********************************************************************
*
* @purpose  Get the number of Multicast packets received on managed AP L2 tunnel.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *value   @b{(output)} pointer to error count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatCL2TunnelMcastRxGet(L7_enetMacAddr_t macAddr, L7_uint64 *value);

/*********************************************************************
*
* @purpose  Get the number of Bytes transmitted on managed AP L2 tunnel.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *value   @b{(output)} pointer to error count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatCL2TunnelBytesTxGet(L7_enetMacAddr_t macAddr, L7_uint64 *value);

/*********************************************************************
*
* @purpose  Get the number of Packets transmitted on managed AP L2 tunnel.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *value   @b{(output)} pointer to error count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatCL2TunnelPktsTxGet(L7_enetMacAddr_t macAddr, L7_uint64 *value);

/*********************************************************************
*
* @purpose  Get the number of multicast packets transmitted on managed AP L2 tunnel.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *value   @b{(output)} pointer to error count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatCL2TunnelMcastTxGet(L7_enetMacAddr_t macAddr, L7_uint64 *value);

/* CL2 TUNNEL STATS End */

/* TSPEC statistics begin */
/*********************************************************************
*
* @purpose  Get the total number of Tspecs accepted for a managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint32         acindex  @b{(input)} AC index value
* @param    L7_uint32         *value   @b{(output)} pointer to error count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatTSTotalTsAcceptedGet(L7_enetMacAddr_t macAddr,
                                             L7_uint32 acindex,
                                             L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the total number of Tspecs rejected for a managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint32         acindex  @b{(input)} AC index value
* @param    L7_uint32         *value   @b{(output)} pointer to error count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatTSTotalTsRejectedGet(L7_enetMacAddr_t macAddr,
                                             L7_uint32 acindex,
                                             L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the number of roaming Tspecs accepted for a managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint32         acindex  @b{(input)} AC index value
* @param    L7_uint32         *value   @b{(output)} pointer to error count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatTSRoamTsAcceptedGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 acindex,
                                            L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the number of roaming Tspecs rejected for a managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint32         acindex  @b{(input)} AC index value
* @param    L7_uint32         *value   @b{(output)} pointer to error count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPStatTSRoamTsRejectedGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 acindex,
                                            L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get Tspec global statistics for a managed AP entry.
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
L7_RC_t wdmManagedAPStatTsPktsRxGet(L7_enetMacAddr_t macAddr,
                                    L7_uint32 acindex,
                                    L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get Tspec global statistics for a managed AP entry.
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
L7_RC_t wdmManagedAPStatTsBytesRxGet(L7_enetMacAddr_t macAddr,
                                     L7_uint32 acindex,
                                     L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get Tspec global statistics for a managed AP entry.
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
L7_RC_t wdmManagedAPStatTsPktsTxGet(L7_enetMacAddr_t macAddr,
                                    L7_uint32 acindex,
                                    L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get Tspec global statistics for a managed AP entry.
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
L7_RC_t wdmManagedAPStatTsBytesTxGet(L7_enetMacAddr_t macAddr,
                                     L7_uint32 acindex,
                                     L7_uint64 *bytes);

/* TSPEC statistics end */
/* End per managed AP statistics APIs */

/* Start per radio statistics APIs */

/*********************************************************************
*
* @purpose  Set all per radio statistics for a managed AP entry.
*
* @param    L7_enetMacAddr_t              macAddr @b{(input)} AP MAC address
* @param    L7_uchar8                     radioIf @b{(input)} radio interface number
* @param    wdmManagedAPRadioStatistics_t *stats  @b{(input)} per radio statistics
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This API sets all values (input values are NOT added to current values).
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatisticsSet(L7_enetMacAddr_t macAddr,
                                       L7_uchar8 radioIf,
                                       wdmManagedAPRadioStatistics_t *stats);

/*********************************************************************
*
* @purpose  Get all per radio statistics for a managed AP entry.
*
* @param    L7_enetMacAddr_t              macAddr @b{(input)} AP MAC address
* @param    L7_uchar8                     radioIf @b{(input)} radio interface number
* @param    wdmManagedAPRadioStatistics_t *stats  @b{(output)} pointer to per radio statistics
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatisticsGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                       wdmManagedAPRadioStatistics_t *stats);

/*********************************************************************
*
* @purpose  Get the number of WLAN packets received per radio on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface number
* @param    L7_uint64         *pktsRx  @b{(output)} pointer to packets received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatPktsRxGet(L7_enetMacAddr_t macAddr,
                                       L7_uchar8 radioIf, L7_uint64 *pktsRx);

/*********************************************************************
*
* @purpose  Get the number of WLAN bytes received per radio on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP MAC address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface number
* @param    L7_uint64         *bytesRx  @b{(output)} pointer to bytes received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatBytesRxGet(L7_enetMacAddr_t macAddr,
                                        L7_uchar8 radioIf, L7_uint64 *bytesRx);

/*********************************************************************
*
* @purpose  Get the number of WLAN packets transmitted per radio on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface number
* @param    L7_uint64         *pktsTx  @b{(output)} pointer to packets transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatPktsTxGet(L7_enetMacAddr_t macAddr,
                                       L7_uchar8 radioIf, L7_uint64 *pktsTx);

/*********************************************************************
*
* @purpose  Get the number of WLAN bytes transmitted per radio on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP MAC address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface number
* @param    L7_uint64         *bytesTx  @b{(output)} pointer to bytes transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatBytesTxGet(L7_enetMacAddr_t macAddr,
                                        L7_uchar8 radioIf, L7_uint64 *bytesTx);

/*********************************************************************
*
* @purpose  Get the number of WLAN dropped receive packets per radio on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface number
* @param    L7_uint64         *pkts    @b{(output)} pointer to dropped receive packets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatDropPktsRxGet(L7_enetMacAddr_t macAddr,
                                           L7_uchar8 radioIf, L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the number of WLAN dropped receive bytes per radio on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP MAC address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface number
* @param    L7_uint64         *bytes    @b{(output)} pointer to dropped receive bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatDropBytesRxGet(L7_enetMacAddr_t macAddr,
                                            L7_uchar8 radioIf, L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the number of WLAN dropped transmit packets per radio on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface number
* @param    L7_uint64         *pkts    @b{(output)} pointer to dropped transmit packets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatDropPktsTxGet(L7_enetMacAddr_t macAddr,
                                           L7_uchar8 radioIf, L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the number of WLAN dropped transmit bytes per radio on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP MAC address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface number
* @param    L7_uint64         *bytes    @b{(output)} pointer to dropped transmit bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatDropBytesTxGet(L7_enetMacAddr_t macAddr,
                                            L7_uchar8 radioIf, L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the number of fragments transmitted per radio on managed AP.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC address
* @param    L7_uchar8        radioIf @b{(input)} radio interface number
* @param    L7_uint32        *fragTx @b{(output)} pointer to fragments transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatFragmentsTxGet(L7_enetMacAddr_t macAddr,
                                            L7_uchar8 radioIf, L7_uint32 *fragTx);

/*********************************************************************
*
* @purpose  Get the number of multicast frames transmitted per radio on managed AP.
*
* @param    L7_enetMacAddr_t macAddr       @b{(input)} AP MAC address
* @param    L7_uchar8        radioIf       @b{(input)} radio interface number
* @param    L7_uint32        *mcastFrameTx @b{(output)} pointer to mcast frames transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatMcastFrameTxGet(L7_enetMacAddr_t macAddr,
                                             L7_uchar8 radioIf, L7_uint32 *mcastFrameTx);

/*********************************************************************
*
* @purpose  Get the failed count per radio on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface number
* @param    L7_uint32         *failed  @b{(output)} pointer to failed count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatFailedCountGet(L7_enetMacAddr_t macAddr,
                                            L7_uchar8 radioIf, L7_uint32 *failed);

/*********************************************************************
*
* @purpose  Get the retry count per radio on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface number
* @param    L7_uint32         *retry   @b{(output)} pointer to retry count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatRetryGet(L7_enetMacAddr_t macAddr,
                                      L7_uchar8 radioIf, L7_uint32 *retry);

/*********************************************************************
*
* @purpose  Get the retry count per radio on managed AP.
*
* @param    L7_enetMacAddr_t macAddr    @b{(input)} AP MAC address
* @param    L7_uchar8        radioIf    @b{(input)} radio interface number
* @param    L7_uint32        *multRetry @b{(output)} pointer to multiple retry count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatMultRetryGet(L7_enetMacAddr_t macAddr,
                                          L7_uchar8 radioIf, L7_uint32 *multRetry);

/*********************************************************************
*
* @purpose  Get the number of duplicate frames per radio on managed AP.
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)} AP MAC address
* @param    L7_uchar8        radioIf   @b{(input)} radio interface number
* @param    L7_uint32        *frameDup @b{(output)} pointer to duplicate frame count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatDupFramesGet(L7_enetMacAddr_t macAddr,
                                          L7_uchar8 radioIf, L7_uint32 *frameDup);

/*********************************************************************
*
* @purpose  Get the number of RTS successes per radio on managed AP.
*
* @param    L7_enetMacAddr_t macAddr     @b{(input)} AP MAC address
* @param    L7_uchar8        radioIf     @b{(input)} radio interface number
* @param    L7_uint32        *rtsSuccess @b{(output)} pointer to RTS success count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatRTSSuccessGet(L7_enetMacAddr_t macAddr,
                                           L7_uchar8 radioIf, L7_uint32 *rtsSuccess);

/*********************************************************************
*
* @purpose  Get the number of RTS failures per radio on managed AP.
*
* @param    L7_enetMacAddr_t macAddr     @b{(input)} AP MAC address
* @param    L7_uchar8        radioIf     @b{(input)} radio interface number
* @param    L7_uint32        *rtsFailure @b{(output)} pointer to RTS failure count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatRTSFailuresGet(L7_enetMacAddr_t macAddr,
                                            L7_uchar8 radioIf, L7_uint32 *rtsFailure);

/*********************************************************************
*
* @purpose  Get the number of ACK failures per radio on managed AP.
*
* @param    L7_enetMacAddr_t macAddr     @b{(input)} AP MAC address
* @param    L7_uchar8        radioIf     @b{(input)} radio interface number
* @param    L7_uint32        *ackFailure @b{(output)} pointer to ACK failure count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatACKFailuresGet(L7_enetMacAddr_t macAddr,
                                            L7_uchar8 radioIf, L7_uint32 *ackFailure);

/*********************************************************************
*
* @purpose  Get the number of fragments received per radio on managed AP.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC address
* @param    L7_uchar8        radioIf @b{(input)} radio interface number
* @param    L7_uint32        *fragRx @b{(output)} pointer to fragments received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatFragmentsRxGet(L7_enetMacAddr_t macAddr,
                                            L7_uchar8 radioIf, L7_uint32 *fragRx);

/*********************************************************************
*
* @purpose  Get the number of multicast frames received per radio on managed AP.
*
* @param    L7_enetMacAddr_t macAddr       @b{(input)} AP MAC address
* @param    L7_uchar8        radioIf       @b{(input)} radio interface number
* @param    L7_uint32        *mcastFrameRx @b{(output)} pointer to mcast frames received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatMcastFrameRxGet(L7_enetMacAddr_t macAddr,
                                             L7_uchar8 radioIf, L7_uint32 *mcastFrameRx);

/*********************************************************************
*
* @purpose  Get the number of FCS errors per radio on managed AP.
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)} AP MAC address
* @param    L7_uchar8        radioIf   @b{(input)} radio interface number
* @param    L7_uint32        *fcsError @b{(output)} pointer to FCS error count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatFCSErrorsGet(L7_enetMacAddr_t macAddr,
                                          L7_uchar8 radioIf, L7_uint32 *fcsError);

/*********************************************************************
*
* @purpose  Get the number of frames transmitted per radio on managed AP.
*
* @param    L7_enetMacAddr_t macAddr  @b{(input)} AP MAC address
* @param    L7_uchar8        radioIf  @b{(input)} radio interface number
* @param    L7_uint32        *frameTx @b{(output)} pointer to frames transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatFramesTxGet(L7_enetMacAddr_t macAddr,
                                         L7_uchar8 radioIf, L7_uint32 *frameTx);

/*********************************************************************
*
* @purpose  Get the number of encrypted frames received per radio on managed AP.
*
* @param    L7_enetMacAddr_t macAddr       @b{(input)} AP MAC address
* @param    L7_uchar8        radioIf       @b{(input)} radio interface number
* @param    L7_uint32        *frameEncrypt @b{(output)} pointer to encrypted frame count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPRadioStatWEPUndecryptableGet(L7_enetMacAddr_t macAddr,
                                                 L7_uchar8 radioIf, L7_uint32 *frameEncrypt);

/*********************************************************************
*
* @purpose  Get the TSPEC stats per radio on managed AP.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint8               rif         @b{(input)}  radio (1,2)
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
L7_RC_t wdmManagedAPRadioStatTsPktsRxGet(L7_enetMacAddr_t macAddr,
                                         L7_uint8 radioIf,
                                         L7_uint32 acindex,
                                         L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the TSPEC stats per radio on managed AP.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint8               rif         @b{(input)}  radio (1,2)
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
L7_RC_t wdmManagedAPRadioStatTsBytesRxGet(L7_enetMacAddr_t macAddr,
                                          L7_uint8 radioIf,
                                          L7_uint32 acindex,
                                          L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the TSPEC stats per radio on managed AP.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint8               rif         @b{(input)}  radio (1,2)
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
L7_RC_t wdmManagedAPRadioStatTsPktsTxGet(L7_enetMacAddr_t macAddr,
                                         L7_uint8 radioIf,
                                         L7_uint32 acindex,
                                         L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the TSPEC stats per radio on managed AP.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint8               rif         @b{(input)}  radio (1,2)
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
L7_RC_t wdmManagedAPRadioStatTsBytesTxGet(L7_enetMacAddr_t macAddr,
                                          L7_uint8 radioIf,
                                          L7_uint32 acindex,
                                          L7_uint64 *bytes);

/* End per radio statistics APIs */

/* Start per vap statistics APIs */

/*********************************************************************
*
* @purpose  Set all VAP statistics for a managed AP entry.
*
* @param    L7_enetMacAddr_t            macAddr @b{(input)} AP MAC address
* @param    L7_uchar8                   radioIf @b{(input)} radio interface
* @param    L7_uint32                   vapId   @b{(input)} VAP ID
* @param    wdmManagedAPVAPStatistics_t *stats  @b{(input)} per VAP statistics
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  This API sets all values (input values are NOT added to current values).
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPStatisticsSet(L7_enetMacAddr_t macAddr,
                                     L7_uchar8 radioIf,
                                     L7_uint32 vapId,
                                     wdmManagedAPVAPStatistics_t *stats);

/*********************************************************************
*
* @purpose  Get all VAP statistics for a managed AP entry.
*
* @param    L7_enetMacAddr_t            macAddr @b{(input)} AP MAC address
* @param    L7_uchar8                   radioIf @b{(input)} radio interface
* @param    L7_uint32                   vapId   @b{(input)} VAP ID
* @param    wdmManagedAPVAPStatistics_t *stats  @b{(output)} pointer to per VAP statistics
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPStatisticsGet(L7_enetMacAddr_t macAddr,
                                     L7_uchar8 radioIf,
                                     L7_uint32 vapId,
                                     wdmManagedAPVAPStatistics_t *stats);

/*********************************************************************
*
* @purpose  Get the number of WLAN packets received per VAP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uint32         vapId    @b{(input)} VAP ID
* @param    L7_uint64         *pktsRx  @b{(output)} pointer to packets received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPStatPktsRxGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                     L7_uint32 vapId, L7_uint64 *pktsRx);

/*********************************************************************
*
* @purpose  Get the number of WLAN bytes received per VAP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP MAC address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uint32         vapId     @b{(input)} VAP ID
* @param    L7_uint64         *bytesRx  @b{(output)} pointer to bytes received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPStatBytesRxGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                      L7_uint32 vapId, L7_uint64 *bytesRx);

/*********************************************************************
*
* @purpose  Get the number of WLAN packets transmitted per VAP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uint32         vapId    @b{(input)} VAP ID
* @param    L7_uint64         *pktsTx  @b{(output)} pointer to packets transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPStatPktsTxGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                     L7_uint32 vapId, L7_uint64 *pktsTx);

/*********************************************************************
*
* @purpose  Get the number of WLAN bytes transmitted per VAP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP MAC address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uint32         vapId     @b{(input)} VAP ID
* @param    L7_uint64         *bytesTx  @b{(output)} pointer to bytes transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPStatBytesTxGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                      L7_uint32 vapId, L7_uint64 *bytesTx);

/*********************************************************************
*
* @purpose  Get the number of WLAN dropped receive packets per VAP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uint32         vapId    @b{(input)} VAP ID
* @param    L7_uint64         *pkts    @b{(output)} pointer to dropped receive packets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPStatDropPktsRxGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                         L7_uint32 vapId, L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the number of WLAN dropped receive bytes per VAP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP MAC address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uint32         vapId     @b{(input)} VAP ID
* @param    L7_uint64         *bytes    @b{(output)} pointer to dropped receive bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPStatDropBytesRxGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                          L7_uint32 vapId, L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the number of WLAN dropped transmit packets per VAP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface
* @param    L7_uint32         vapId    @b{(input)} VAP ID
* @param    L7_uint64         *pkts    @b{(output)} pointer to dropped transmit packets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPStatDropPktsTxGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                         L7_uint32 vapId, L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the number of WLAN dropped transmit bytes per VAP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP MAC address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface
* @param    L7_uint32         vapId     @b{(input)} VAP ID
* @param    L7_uint64         *bytes    @b{(output)} pointer to dropped transmit bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPStatDropBytesTxGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                          L7_uint32 vapId, L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the number of client association failures per VAP.
*
* @param    L7_enetMacAddr_t macAddr        @b{(input)} AP MAC address
* @param    L7_uchar8        radioIf        @b{(input)} radio interface
* @param    L7_uint32        vapId          @b{(input)} VAP ID
* @param    L7_uint32        *assocFailures @b{(output)} pointer to failure count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPStatAssocFailuresGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                            L7_uint32 vapId, L7_uint32 *assocFailures);

/*********************************************************************
*
* @purpose  Get the number of client authentication failures per VAP.
*
* @param    L7_enetMacAddr_t macAddr       @b{(input)} AP MAC address
* @param    L7_uchar8        radioIf       @b{(input)} radio interface
* @param    L7_uint32        vapId         @b{(input)} VAP ID
* @param    L7_uint32        *authFailures @b{(output)} pointer to failure count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPVAPStatAuthFailuresGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf,
                                           L7_uint32 vapId, L7_uint32 *authFailures);

/*********************************************************************
*
* @purpose  Get the TSPEC stats per VAP on managed AP.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint8               rif         @b{(input)}  radio (1,2)
* @param    L7_uint32              vap         @b{(input)}  VAP 
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
L7_RC_t wdmManagedAPVAPStatTsPktsRxGet(L7_enetMacAddr_t macAddr,
                                       L7_uint8 radioIf,
                                       L7_uint32 vapId,
                                       L7_uint32 acindex,
                                       L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the TSPEC stats per VAP on managed AP.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint8               rif         @b{(input)}  radio (1,2)
* @param    L7_uint32              vap         @b{(input)}  VAP 
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
L7_RC_t wdmManagedAPVAPStatTsBytesRxGet(L7_enetMacAddr_t macAddr,
                                        L7_uint8 radioIf,
                                        L7_uint32 vapId,
                                        L7_uint32 acindex,
                                        L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the TSPEC stats per VAP on managed AP.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint8               rif         @b{(input)}  radio (1,2)
* @param    L7_uint32              vap         @b{(input)}  VAP 
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
L7_RC_t wdmManagedAPVAPStatTsPktsTxGet(L7_enetMacAddr_t macAddr,
                                       L7_uint8 radioIf,
                                       L7_uint32 vapId,
                                       L7_uint32 acindex,
                                       L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the TSPEC stats per VAP on managed AP.
*
* @param    L7_enetMacAddr_t       macAddr     @b{(input)}  client MAC address
* @param    L7_uint8               rif         @b{(input)}  radio (1,2)
* @param    L7_uint32              vap         @b{(input)}  VAP 
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
L7_RC_t wdmManagedAPVAPStatTsBytesTxGet(L7_enetMacAddr_t macAddr,
                                        L7_uint8 radioIf,
                                        L7_uint32 vapId,
                                        L7_uint32 acindex,
                                        L7_uint64 *bytes);

/* End per vap statistics APIs */

/* Start of L2 Distributed Tunnleing Statistics APIs */
/*********************************************************************
*
* @purpose  Get the Total bytes transmitted by the AP through all
*           the Dist Tunnels.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *bytesTx  @b{(output)} pointer to bytes transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelAPStatBytesTxGet(L7_enetMacAddr_t macAddr,
                                          L7_uint64 *bytesTx);
/*********************************************************************
*
* @purpose  Get the Total packets transmitted by the AP through all
*           the Dist Tunnels.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *pktsTx  @b{(output)} pointer to packets transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelAPStatPktsTxGet(L7_enetMacAddr_t macAddr,
                                          L7_uint64 *pktsTx);
/*********************************************************************
*
* @purpose  Get the Total mcast packets transmitted by the AP through all
*           the Dist Tunnels.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *pktsTx  @b{(output)} pointer to packets transmitted
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelAPStatMcastPktsTxGet(L7_enetMacAddr_t macAddr,
                                               L7_uint64 *pktsTx);
/*********************************************************************
*
* @purpose  Get the Total bytes received by the AP through all
*           the Dist Tunnels.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *bytesTx  @b{(output)} pointer to bytes received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelAPStatBytesRxGet(L7_enetMacAddr_t macAddr,
                                          L7_uint64 *bytesRx);
/*********************************************************************
*
* @purpose  Get the Total packets received by the AP through all
*           the Dist Tunnels.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *pktsTx  @b{(output)} pointer to packets received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelAPStatPktsRxGet(L7_enetMacAddr_t macAddr,
                                          L7_uint64 *pktsRx);
/*********************************************************************
*
* @purpose  Get the Total mcast packets received by the AP through all
*           the Dist Tunnels.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint64         *pktsTx  @b{(output)} pointer to packets received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelAPStatMcastPktsRxGet(L7_enetMacAddr_t macAddr,
                                               L7_uint64 *pktsRx);
/*********************************************************************
*
* @purpose  Get the Total clients that have roamed away from and roamed
*           to this AP
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint32        *clients  @b{(output)} pointer to number
*                                                   of clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelAPRoamedClientsGet(L7_enetMacAddr_t macAddr,
                                             L7_uint32 *clients);
/*********************************************************************
*
* @purpose  Get the Total clients that have been disconnected because
*           idle timer timed out
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint32        *clients  @b{(output)} pointer to number
*                                                   of clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelAPRoamedClientsIdleTimedoutGet(L7_enetMacAddr_t macAddr,
                                                         L7_uint32 *clients);
/*********************************************************************
*
* @purpose  Get the Total clients that have been disconnected because
*           max tunnel age timer timed out
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint32        *clients  @b{(output)} pointer to number
*                                                   of clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelAPRoamedClientsAgedoutGet(L7_enetMacAddr_t macAddr,
                                                    L7_uint32 *clients);
/*********************************************************************
*
* @purpose  Get the Total number of tunnel set up denials by the AP
*           because of reaching maximum client limit
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint32        *denials  @b{(output)} pointer to number
*                                                   of denials
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelAPClientLimitDenialsGet(L7_enetMacAddr_t macAddr,
                                                  L7_uint32 *denials);
/*********************************************************************
*
* @purpose  Get the Total number of tunnel set up denials by the AP
*           because of reaching maximum allowed multicast replications
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint32        *denials  @b{(output)} pointer to number
*                                                   of denials
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelAPMaxMcastReplDenialsGet(L7_enetMacAddr_t macAddr,
                                                   L7_uint32 *denials);

/*********************************************************************
*
* @purpose  Get the Total number of ARP Suppression Broadcast to Unicast
*           converted ARP Request per-AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint32        *converts  @b{(output)} pointer to number
*                                                   of converts
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPARPSuppBcastToUcastCounterGet (L7_enetMacAddr_t macAddr,
                            L7_uint32 *converts);

/*********************************************************************
*
* @purpose  Get the Total number of ARP Requests Filtered by Wireless ARP
*           Suppression module per-AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint32        *filtered  @b{(output)} pointer to number
*                                                   of filtered
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPARPReqsFilteredCounterGet (L7_enetMacAddr_t macAddr,
                            L7_uint32 *filtered);

/*********************************************************************
*
* @purpose  Get the Total number of Broadcasted ARP Requests per-AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uint32        *bcastedARP  @b{(output)} pointer to number
*                                                   of broadcasted ARP Reqs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmManagedAPBcastedARPReqsCounterGet (L7_enetMacAddr_t macAddr,
                            L7_uint32 *bcastedARP);

#endif /* INCLUDE_WDM_AP_STATISTICS_API_H */

