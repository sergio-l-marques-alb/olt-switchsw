/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   wdm_wds_link_api.h
*
* @purpose    Wireless Data Manager WDS Link API header
*
* @component  WDM
*
* @comments   none
*
* @create     5/13/2009
*
* @author     nagendra
*
* @end
*
*********************************************************************/
#ifndef INCLUDE_WDM_WDS_LINK_API_H
#define INCLUDE_WDM_WDS_LINK_API_H

#include "datatypes.h"
#include "wdm_api.h"
#include "wireless_commdefs.h"
#include "wireless_comm_structs.h"

typedef struct WdmWDSLinkStatistics_s
{
  wdmCounters64_t           sourceAPPktsSent;
  wdmCounters64_t           sourceAPBytesSent;
  wdmCounters64_t           sourceAPPktsRcvd;
  wdmCounters64_t           sourceAPBytesRcvd;
  wdmCounters64_t           destAPPktsSent;
  wdmCounters64_t           destAPBytesSent;
  wdmCounters64_t           destAPPktsRcvd;
  wdmCounters64_t           destAPBytesRcvd;
} wdmWDSLinkStatistics_t;

/*********************************************************************
*
* @purpose  Add a WDS Link to the Database.
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkEntryAdd (wdmWDSLinkKey_t wdsLink);

/*********************************************************************
*
* @purpose  Delete a WDS link from the database
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkEntryDelete (wdmWDSLinkKey_t wdsLink);

/*********************************************************************
*
* @purpose  Get a WDS link from the database
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkEntryGet (wdmWDSLinkKey_t wdsLink);

/*********************************************************************
*
* @purpose  Get the next WDS link from the database
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkEntryNextGet (wdmWDSLinkKey_t wdsLink,
                            wdmWDSLinkKey_t *nextWDSLink);

/*********************************************************************
*
* @purpose  Check if the AP-Radio is part of a WDS Configured Link.
*
* @param    L7_enetMacAddr_t macAddr  @b{(input)} AP Mac Address
* @param    L7_uchar8        radioId  @b{(input)} Radio ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSAPRadioInLinkExists (L7_enetMacAddr_t macAddr, L7_uchar8 radioId);

/*********************************************************************
*
* @purpose  Set the WDS Link STP Cost
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
* @param    L7_uchar8         stpCost  @b{(input)} STP Cost
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkSTPCostSet (wdmWDSLinkKey_t wdsLink, L7_uchar8 stpCost);

/*********************************************************************
*
* @purpose  Get the WDS Link STP Cost
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
* @param    L7_uchar8        *stpCost  @b{(output)} STP Cost
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkSTPCostGet (wdmWDSLinkKey_t wdsLink, L7_uchar8 *stpCost);

/*********************************************************************
*
* @purpose  Set the Source AP WDS Link Remote End Point Detected flag
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
* @param    L7_uchar8         detected  @b{(input)} Remote End Pointed Detected
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkSourceEndPointDetectedSet (wdmWDSLinkKey_t wdsLink,
                            L7_uchar8 detected);

/*********************************************************************
*
* @purpose  Get the Source AP WDS Link Remote End Point Detected flag
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
* @param    L7_uchar8        *detected  @b{(output)} Remote End Pointed Detected
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkSourceEndPointDetectedGet (wdmWDSLinkKey_t wdsLink,
                            L7_uchar8 *detected);

/*********************************************************************
*
* @purpose  Set the Destination AP WDS Link Remote End Point Detected flag
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
* @param    L7_uchar8         detected  @b{(input)} Remote End Pointed Detected
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkDestEndPointDetectedSet (wdmWDSLinkKey_t wdsLink,
                            L7_uchar8 detected);

/*********************************************************************
*
* @purpose  Get the Destination AP WDS Link Remote End Point Detected flag
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
* @param    L7_uchar8        *detected  @b{(output)} Remote End Pointed Detected
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkDestEndPointDetectedGet (wdmWDSLinkKey_t wdsLink,
                            L7_uchar8 *detected);

/*********************************************************************
*
* @purpose  Set the Source AP WDS Link STP State
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
* @param    L7_uchar8         state  @b{(input)} STP State
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkSourceSTPStateSet (wdmWDSLinkKey_t wdsLink,
                            L7_uchar8 state);

/*********************************************************************
*
* @purpose  Get the Source AP WDS Link STP State
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
* @param    L7_uchar8        *state  @b{(output)} STP State
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkSourceSTPStateGet (wdmWDSLinkKey_t wdsLink,
                            L7_uchar8 *state);

/*********************************************************************
*
* @purpose  Set the Destination AP WDS Link STP State
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
* @param    L7_uchar8         state  @b{(input)} STP State
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkDestSTPStateSet (wdmWDSLinkKey_t wdsLink,
                            L7_uchar8 state);

/*********************************************************************
*
* @purpose  Get the Destination AP WDS Link STP State
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
* @param    L7_uchar8        *state  @b{(output)} STP State
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkDestSTPStateGet (wdmWDSLinkKey_t wdsLink,
                            L7_uchar8 *state);

/*********************************************************************
*
* @purpose  Set the Aggregation Mode for the AP WDS Link
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
* @param    L7_uchar8         mode  @b{(input)} Aggregation Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkAggregationModeSet (wdmWDSLinkKey_t wdsLink,
                            L7_uchar8 mode);

/*********************************************************************
*
* @purpose  Get the Aggregation Mode for the AP WDS Link
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
* @param    L7_uchar8        *mode  @b{(output)} Aggregation Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkAggregationModeGet (wdmWDSLinkKey_t wdsLink,
                            L7_uchar8 *mode);

/*********************************************************************
*
* @purpose  Set source AP statistics for WDS Link entry.
*
* @param    wdmWDSLinkKey_t         wdsLink @b{(input)} WDS Link
* @param    wdmWDSLinkStatistics_t  *stats  @b{(input)} per WDS Link statistics
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSSourceAPLinkStatisticsSet (wdmWDSLinkKey_t wdsLink,
                            wdmWDSLinkStatistics_t *stats);

/*********************************************************************
*
* @purpose  Set destination AP statistics for WDS Link entry.
*
* @param    wdmWDSLinkKey_t         wdsLink @b{(input)} WDS Link
* @param    wdmWDSLinkStatistics_t  *stats  @b{(input)} per WDS Link statistics
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSDestAPLinkStatisticsSet (wdmWDSLinkKey_t wdsLink,
                            wdmWDSLinkStatistics_t *stats);

/*********************************************************************
*
* @purpose  Set all statistics for WDS Link entry.
*
* @param    wdmWDSLinkKey_t         wdsLink @b{(input)} WDS Link
* @param    wdmWDSLinkStatistics_t  *stats  @b{(input)} per WDS Link statistics
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkStatisticsSet (wdmWDSLinkKey_t wdsLink,
                            wdmWDSLinkStatistics_t *stats);

/*********************************************************************
*
* @purpose  Get all statistics for WDS Link entry.
*
* @param    wdmWDSLinkKey_t           wdsLink @b{(input)} WDS Link
* @param    wdmWDSLinkStatistics_t   *stats  @b{(output)} pointer to stats
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkStatisticsGet (wdmWDSLinkKey_t wdsLink,
                            wdmWDSLinkStatistics_t *stats);

/*********************************************************************
*
* @purpose  Get the Source AP Radio reported Sent Packets
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
* @param    L7_uint64        *pkts  @b{(output)} Packets Sent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkSourceAPPktsSentGet (wdmWDSLinkKey_t wdsLink,
                            L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the Source AP Radio reported Sent Bytes
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
* @param    L7_uint64        *bytes  @b{(output)} Bytes Sent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkSourceAPBytesSentGet (wdmWDSLinkKey_t wdsLink,
                            L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the Source AP Radio reported Received Packets
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
* @param    L7_uint64        *pkts  @b{(output)} Packets Received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkSourceAPPktsRcvdGet (wdmWDSLinkKey_t wdsLink,
                            L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the Source AP Radio reported Received Bytes
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
* @param    L7_uint64        *bytes  @b{(output)} Bytes Sent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkSourceAPBytesRcvdGet (wdmWDSLinkKey_t wdsLink,
                            L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the Destination AP Radio reported Sent Packets
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
* @param    L7_uint64        *pkts  @b{(output)} Packets Sent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkDestAPPktsSentGet (wdmWDSLinkKey_t wdsLink,
                            L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the Destination AP Radio reported Sent Bytes
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
* @param    L7_uint64        *bytes  @b{(output)} Bytes Sent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkDestAPBytesSentGet (wdmWDSLinkKey_t wdsLink,
                            L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the Destination AP Radio reported Received Packets
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
* @param    L7_uint64        *pkts  @b{(output)} Packets Received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkDestAPPktsRcvdGet (wdmWDSLinkKey_t wdsLink,
                            L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the Destination AP Radio reported Received Bytes
*
* @param    wdmWDSLinkKey_t   wdsLink  @b{(input)} WDS Link Key
* @param    L7_uint64        *bytes  @b{(output)} Bytes Sent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWDSLinkDestAPBytesRcvdGet (wdmWDSLinkKey_t wdsLink,
                            L7_uint64 *bytes);

#endif /* INCLUDE_WDM_WDS_LINK_API_H */

