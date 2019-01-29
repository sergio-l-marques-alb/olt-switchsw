/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   usmdb_wdm_ap_api.h
*
* @purpose    Wireless Data Manager WDS Link USMDB API header
*
* @component  USMDB
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

#ifndef INCLUDE_USMDB_WDM_WDS_LINK_API_H
#define INCLUDE_USMDB_WDM_WDS_LINK_API_H

#include "l7_common.h"
#include "wireless_comm_structs.h"

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
L7_RC_t usmDbWdmWDSLinkEntryAdd (wdmWDSLinkKey_t wdsLink);

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
L7_RC_t usmDbWdmWDSLinkEntryDelete (wdmWDSLinkKey_t wdsLink);

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
L7_RC_t usmDbWdmWDSLinkEntryGet (wdmWDSLinkKey_t wdsLink);

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
L7_RC_t usmDbWdmWDSLinkEntryNextGet (wdmWDSLinkKey_t wdsLink,
                            wdmWDSLinkKey_t *nextWDSLink);

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
L7_RC_t usmDbWdmWDSLinkSTPCostSet (wdmWDSLinkKey_t wdsLink, L7_uchar8 stpCost);

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
L7_RC_t usmDbWdmWDSLinkSTPCostGet (wdmWDSLinkKey_t wdsLink, L7_uchar8 *stpCost);

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
L7_RC_t usmDbWdmWDSLinkSourceEndPointDetectedGet (wdmWDSLinkKey_t wdsLink,
                            L7_uchar8 *detected);

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
L7_RC_t usmDbWdmWDSLinkDestEndPointDetectedGet (wdmWDSLinkKey_t wdsLink,
                            L7_uchar8 *detected);

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
L7_RC_t usmDbWdmWDSLinkSourceSTPStateGet (wdmWDSLinkKey_t wdsLink,
                            L7_uchar8 *state);

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
L7_RC_t usmDbWdmWDSLinkDestSTPStateGet (wdmWDSLinkKey_t wdsLink,
                            L7_uchar8 *state);

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
L7_RC_t usmDbWdmWDSLinkAggregationModeGet (wdmWDSLinkKey_t wdsLink,
                            L7_uchar8 *mode);

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
L7_RC_t usmDbWdmWDSLinkSourceAPPktsSentGet (wdmWDSLinkKey_t wdsLink,
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
L7_RC_t usmdbWdmWDSLinkSourceAPBytesSentGet (wdmWDSLinkKey_t wdsLink,
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
L7_RC_t usmDbWdmWDSLinkSourceAPPktsRcvdGet (wdmWDSLinkKey_t wdsLink,
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
L7_RC_t usmDbWdmWDSLinkSourceAPBytesRcvdGet (wdmWDSLinkKey_t wdsLink,
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
L7_RC_t usmDbWdmWDSLinkDestAPPktsSentGet (wdmWDSLinkKey_t wdsLink,
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
L7_RC_t usmDbWdmWDSLinkDestAPBytesSentGet (wdmWDSLinkKey_t wdsLink,
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
L7_RC_t usmDbWdmWDSLinkDestAPPktsRcvdGet (wdmWDSLinkKey_t wdsLink,
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
L7_RC_t usmDbWdmWDSLinkDestAPBytesRcvdGet (wdmWDSLinkKey_t wdsLink,
                            L7_uint64 *bytes);

#endif /* INCLUDE_USMDB_WDM_WDS_LINK_API_H */

