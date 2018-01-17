/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   usmdb_wdm_ap_statistics_api.h
*
* @purpose    Wireless Data Manager Access Point (WDM AP) Statistics 
*             USMDB API header
*
* @component  USMDB
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

#ifndef INCLUDE_USMDB_WDM_AP_STATISTICS_API_H
#define INCLUDE_USMDB_WDM_AP_STATISTICS_API_H

#include "l7_common.h"

/* Start per managed AP statistics APIs */

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
L7_RC_t usmDbWdmManagedAPStatPktsRxGet(L7_enetMacAddr_t macAddr, 
                                       L7_uint64 *pktsRx);

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
L7_RC_t usmDbWdmManagedAPStatBytesRxGet(L7_enetMacAddr_t macAddr, 
                                        L7_uint64 *bytesRx);

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
L7_RC_t usmDbWdmManagedAPStatPktsTxGet(L7_enetMacAddr_t macAddr, 
                                       L7_uint64 *pktsTx);

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
L7_RC_t usmDbWdmManagedAPStatBytesTxGet(L7_enetMacAddr_t macAddr, 
                                        L7_uint64 *bytesTx);

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
L7_RC_t usmDbWdmManagedAPStatDropPktsRxGet(L7_enetMacAddr_t macAddr, L7_uint64 *pkts);

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
L7_RC_t usmDbWdmManagedAPStatDropBytesRxGet(L7_enetMacAddr_t macAddr, L7_uint64 *bytes);

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
L7_RC_t usmDbWdmManagedAPStatDropPktsTxGet(L7_enetMacAddr_t macAddr, L7_uint64 *pkts);

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
L7_RC_t usmDbWdmManagedAPStatDropBytesTxGet(L7_enetMacAddr_t macAddr, L7_uint64 *bytes);

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
L7_RC_t usmDbWdmManagedAPStatEthPktsRxGet(L7_enetMacAddr_t macAddr, 
                                          L7_uint64 *value);

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
L7_RC_t usmDbWdmManagedAPStatEthBytesRxGet(L7_enetMacAddr_t macAddr, 
                                           L7_uint64 *value);

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
L7_RC_t usmDbWdmManagedAPStatEthMultPktsRxGet(L7_enetMacAddr_t macAddr, 
                                              L7_uint64 *value);

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
L7_RC_t usmDbWdmManagedAPStatEthPktsTxGet(L7_enetMacAddr_t macAddr, 
                                          L7_uint64 *value);

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
L7_RC_t usmDbWdmManagedAPStatEthBytesTxGet(L7_enetMacAddr_t macAddr, 
                                           L7_uint64 *value);

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
L7_RC_t usmDbWdmManagedAPStatTotalTxErrorsGet(L7_enetMacAddr_t macAddr, 
                                              L7_uint64 *value);

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
L7_RC_t usmDbWdmManagedAPStatTotalRxErrorsGet(L7_enetMacAddr_t macAddr, 
                                              L7_uint64 *value);

/* Begin CL2 tunnel stats */
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
L7_RC_t usmDbWdmManagedAPStatCL2TunnelBytesRxGet(L7_enetMacAddr_t macAddr, 
                                                 L7_uint64 *value);

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
L7_RC_t usmDbWdmManagedAPStatCL2TunnelPktsRxGet(L7_enetMacAddr_t macAddr, 
                                                L7_uint64 *value);

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
L7_RC_t usmDbWdmManagedAPStatCL2TunnelMcastRxGet(L7_enetMacAddr_t macAddr, 
                                                 L7_uint64 *value);

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
L7_RC_t usmDbWdmManagedAPStatCL2TunnelBytesTxGet(L7_enetMacAddr_t macAddr, 
                                                 L7_uint64 *value);

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
L7_RC_t usmDbWdmManagedAPStatCL2TunnelPktsTxGet(L7_enetMacAddr_t macAddr, 
                                                L7_uint64 *value);

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
L7_RC_t usmDbWdmManagedAPStatCL2TunnelMcastTxGet(L7_enetMacAddr_t macAddr, 
                                                 L7_uint64 *value);
/* End CL2 tunnel stats */

/* Begin TSPEC statistics APIs */
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
L7_RC_t usmDbWdmManagedAPStatTsPktsRxGet(L7_enetMacAddr_t macAddr,
                                         L7_uint32 acindex,
                                         L7_uint64 *pkts);

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
L7_RC_t usmDbWdmManagedAPStatTsPktsTxGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmManagedAPStatTsBytesRxGet(L7_enetMacAddr_t macAddr,
                                     L7_uint32 acindex,
                                     L7_uint64 *bytes);

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
L7_RC_t usmDbWdmManagedAPStatTsBytesTxGet(L7_enetMacAddr_t macAddr,
                                     L7_uint32 acindex,
                                     L7_uint64 *bytes);

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
L7_RC_t usmDbWdmManagedAPStatTSTotalTsAcceptedGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmManagedAPStatTSTotalTsRejectedGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmManagedAPStatTSRoamTsAcceptedGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmManagedAPStatTSRoamTsRejectedGet(L7_enetMacAddr_t macAddr,
                                                 L7_uint32 acindex,
                                                 L7_uint32 *value);

/* End TSPEC statistics APIs */
/* End per managed AP statistics APIs */

/* Start per radio statistics APIs */

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
L7_RC_t usmDbWdmManagedAPRadioStatPktsRxGet(L7_enetMacAddr_t macAddr, 
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
L7_RC_t usmDbWdmManagedAPRadioStatBytesRxGet(L7_enetMacAddr_t macAddr, 
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
L7_RC_t usmDbWdmManagedAPRadioStatPktsTxGet(L7_enetMacAddr_t macAddr, 
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
L7_RC_t usmDbWdmManagedAPRadioStatBytesTxGet(L7_enetMacAddr_t macAddr, 
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
L7_RC_t usmDbWdmManagedAPRadioStatDropPktsRxGet(L7_enetMacAddr_t macAddr, 
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
L7_RC_t usmDbWdmManagedAPRadioStatDropBytesRxGet(L7_enetMacAddr_t macAddr, 
                                                 L7_uchar8 radioIf, L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the number of WLAN drop transmit packets per radio on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr  @b{(input)} AP MAC address
* @param    L7_uchar8         radioIf  @b{(input)} radio interface number
* @param    L7_uint64         *pkts    @b{(output)} pointer to drop transmit packets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioStatDropPktsTxGet(L7_enetMacAddr_t macAddr, 
                                                L7_uchar8 radioIf, L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the number of WLAN drop transmit bytes per radio on managed AP.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} AP MAC address
* @param    L7_uchar8         radioIf   @b{(input)} radio interface number
* @param    L7_uint64         *bytes    @b{(output)} pointer to drop transmit bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmManagedAPRadioStatDropBytesTxGet(L7_enetMacAddr_t macAddr, 
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
L7_RC_t usmDbWdmManagedAPRadioStatFragmentsTxGet(L7_enetMacAddr_t macAddr, 
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
L7_RC_t usmDbWdmManagedAPRadioStatMcastFrameTxGet(L7_enetMacAddr_t macAddr, 
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
L7_RC_t usmDbWdmManagedAPRadioStatFailedCountGet(L7_enetMacAddr_t macAddr, 
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
L7_RC_t usmDbWdmManagedAPRadioStatRetryGet(L7_enetMacAddr_t macAddr, 
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
L7_RC_t usmDbWdmManagedAPRadioStatMultRetryGet(L7_enetMacAddr_t macAddr, 
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
L7_RC_t usmDbWdmManagedAPRadioStatDupFramesGet(L7_enetMacAddr_t macAddr, 
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
L7_RC_t usmDbWdmManagedAPRadioStatRTSSuccessGet(L7_enetMacAddr_t macAddr, 
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
L7_RC_t usmDbWdmManagedAPRadioStatRTSFailuresGet(L7_enetMacAddr_t macAddr, 
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
L7_RC_t usmDbWdmManagedAPRadioStatACKFailuresGet(L7_enetMacAddr_t macAddr, 
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
L7_RC_t usmDbWdmManagedAPRadioStatFragmentsRxGet(L7_enetMacAddr_t macAddr, 
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
L7_RC_t usmDbWdmManagedAPRadioStatMcastFrameRxGet(L7_enetMacAddr_t macAddr, 
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
L7_RC_t usmDbWdmManagedAPRadioStatFCSErrorsGet(L7_enetMacAddr_t macAddr, 
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
L7_RC_t usmDbWdmManagedAPRadioStatFramesTxGet(L7_enetMacAddr_t macAddr, 
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
L7_RC_t usmDbWdmManagedAPRadioStatWEPUndecryptableGet(L7_enetMacAddr_t macAddr, 
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
L7_RC_t usmDbWdmManagedAPRadioStatTsPktsRxGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmManagedAPRadioStatTsBytesRxGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmManagedAPRadioStatTsPktsTxGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmManagedAPRadioStatTsBytesTxGet(L7_enetMacAddr_t macAddr,
                                               L7_uint8 radioIf,
                                               L7_uint32 acindex,
                                               L7_uint64 *bytes);

/* End per radio statistics APIs */

/* Start per vap statistics APIs */

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
L7_RC_t usmDbWdmManagedAPVAPStatPktsRxGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf, 
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
L7_RC_t usmDbWdmManagedAPVAPStatBytesRxGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf, 
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
L7_RC_t usmDbWdmManagedAPVAPStatPktsTxGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf, 
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
L7_RC_t usmDbWdmManagedAPVAPStatBytesTxGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf, 
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
L7_RC_t usmDbWdmManagedAPVAPStatDropPktsRxGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf, 
                                              L7_uint32 vapId, L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the number of WLAN drop receive bytes per VAP.
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
L7_RC_t usmDbWdmManagedAPVAPStatDropBytesRxGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf, 
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
L7_RC_t usmDbWdmManagedAPVAPStatDropPktsTxGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf, 
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
L7_RC_t usmDbWdmManagedAPVAPStatDropBytesTxGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf, 
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
L7_RC_t usmDbWdmManagedAPVAPStatAssocFailuresGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf, 
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
L7_RC_t usmDbWdmManagedAPVAPStatAuthFailuresGet(L7_enetMacAddr_t macAddr, L7_uchar8 radioIf, 
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
L7_RC_t usmDbWdmManagedAPVAPStatTsPktsRxGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmManagedAPVAPStatTsBytesRxGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmManagedAPVAPStatTsPktsTxGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmManagedAPVAPStatTsBytesTxGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmDistTunnelAPStatBytesTxGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmDistTunnelAPStatPktsTxGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmDistTunnelAPStatMcastPktsTxGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmDistTunnelAPStatBytesRxGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmDistTunnelAPStatPktsRxGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmDistTunnelAPStatMcastPktsRxGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmDistTunnelAPRoamedClientsGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmDistTunnelAPRoamedClientsIdleTimedoutGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmDistTunnelAPRoamedClientsAgedoutGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmDistTunnelAPClientLimitDenialsGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmDistTunnelAPMaxMcastReplDenialsGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmManagedAPARPSuppBcastToUcastCounterGet (
                            L7_enetMacAddr_t macAddr, L7_uint32 *converts);

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
L7_RC_t usmDbWdmManagedAPARPReqsFilteredCounterGet (L7_enetMacAddr_t macAddr,
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
L7_RC_t usmDbWdmManagedAPBcastedARPReqsCounterGet (
                            L7_enetMacAddr_t macAddr, L7_uint32 *bcastedARP);

#endif /* INCLUDE_USMDB_WDM_AP_STATISTICS_API_H */

