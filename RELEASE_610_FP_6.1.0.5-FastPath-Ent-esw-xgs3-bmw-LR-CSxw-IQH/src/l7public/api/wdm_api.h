/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2008
*
**********************************************************************
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
#ifndef INCLUDE_WDM_API_H
#define INCLUDE_WDM_API_H

#include "datatypes.h"
#include "wireless_commdefs.h"
#include "wireless_comm_structs.h"
#include "transfer_exports.h"
#include "wireless_exports.h"

/* WDM TSPEC enumerations, used as array indices. */
typedef enum
{
  WDM_TSPEC_TRAFFIC_AC_VO = 0,     /* voice */
  WDM_TSPEC_TRAFFIC_AC_VI,         /* video */
  WDM_TSPEC_TRAFFIC_AC_MAX,
} WDM_TSPEC_TRAFFIC_AC_t;

typedef struct wdmCounters64_s
{
  L7_uint64 clear; /* value when stat was last cleared */
  L7_uint64 final; /* value reported to the user */
} wdmCounters64_t;

typedef struct wdmCounters32_s
{
  L7_uint32 clear; /* value when stat was last cleared */
  L7_uint32 final; /* value reported to the user */
} wdmCounters32_t;

/****************************************
*   Tspec Statistics
****************************************/
typedef struct wdmTspecGenStats_s
{
  wdmCounters32_t         totTspecAccepted;
  wdmCounters32_t         totTspecRejected;
  wdmCounters32_t         totRoamTspecAccepted;
  wdmCounters32_t         totRoamTspecRejected;
} wdmTspecGenStats_t;

typedef struct wdmTspecTrafficStats_s
{
  wdmCounters64_t         tsPktsRx;            /* voice TS pkts received */
  wdmCounters64_t         tsBytesRx;           /* voice TS bytes received */
  wdmCounters64_t         tsPktsTx;            /* voice TS pkts transmitted */
  wdmCounters64_t         tsBytesTx;           /* voice TS btyes transmitted */
} wdmTspecTrafficStats_t;

/*
 * TSPEC global statistics data. This is used to temporarily manipulate summary
 * data while being processed in the WS. The values are copied into the
 * WLAN statistics after processing. Since this overlays the data, the order
 * must be the same as in the destination data structure.
 */
typedef struct wdmWlanTSGlobalStats_s
{
  wdmTspecGenSumStats_t     totalTspecGenStats[WDM_TSPEC_TRAFFIC_AC_MAX];
  wdmTspecTrafficSumStats_t totalTspecTrafficStats[WDM_TSPEC_TRAFFIC_AC_MAX];
} wdmWlanTSGlobalStats_t;

/*********************************************************************
  APIs for usmDb layer, these APIs can be called via usmDb and
  by wireless application components.
*********************************************************************/

/*********************************************************************
*
* @purpose  Set the global admin mode for the Wireless Switch (WS).
*
* @param    mode @b{(input)} new mode value, L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSModeSet(L7_uint32 mode);

/*********************************************************************
*
* @purpose  Set the global admin mode for the Wireless Switch (WS).
*
* @param    mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSModeGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Get the global admin mode status for the Wireless Switch.
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value,
*                                        of type L7_WDM_WS_MODE_STATUS_t.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSModeStatusGet(L7_WDM_WS_MODE_STATUS_t *mode);

/*********************************************************************
*
* @purpose  Get the global admin mode disable reason for the Wireless Switch.
*
* @param    L7_WDM_WS_MODE_REASON_t *reason @b{(output)} location to store the reason value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSModeReasonGet(L7_WDM_WS_MODE_REASON_t *reason);

/*********************************************************************
*
* @purpose  Register an application component with WDM.
*
* @param    L7_uchar8 *name @b{(input)} component name 1-32 characters.
* @param    L7_uint32 *componentId @b{(output)} unique id for component to
*                                                use in mode ack calls
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmComponentRegister(L7_uchar8 *name,L7_COMPONENT_IDS_t *componentId);

/*********************************************************************
*
* @purpose  For each registered application component to acknowledge
*           the L7_WDM_WS_MODE_ENABLE_PENDING state.
*
* @param    L7_uint32 componentId @b{(input)} unique id provided in
*                                              register call.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSModeEnableAck(L7_COMPONENT_IDS_t componentId);

/*********************************************************************
*
* @purpose  For each registered application component to acknowledge
*           the L7_WDM_WS_MODE_DISABLE_PENDING state.
*
* @param    L7_uint32 componentId @b{(input)} unique id provided in
*                                              register call.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSModeDisableAck( L7_COMPONENT_IDS_t componentId);

/*********************************************************************
*
* @purpose  Set the transmitted WLAN bytes value for the Wireless Switch (WS).
*
* @param    L7_uint64 bytes @b{(input)} WLAN bytes transmitted.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWLANBytesTxSet(L7_uint64 bytes);

/*********************************************************************
*
* @purpose  Get the transmitted WLAN bytes value for the Wireless Switch (WS).
*
* @param    L7_uint64 *bytes @b{(input)} Location to store WLAN transmitted
*                                        bytes value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWLANBytesTxGet(L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Set the received WLAN bytes value for the Wireless Switch (WS).
*
* @param    L7_uint64 bytes @b{(input)} WLAN bytes received.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWLANBytesRxSet(L7_uint64 bytes);

/*********************************************************************
*
* @purpose  Get the received WLAN bytes value for the Wireless Switch (WS).
*
* @param    L7_uint64 *bytes @b{(input)} Location to store WLAN received
*                                        bytes value.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWLANBytesRxGet(L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Set the transmitted WLAN packets value for the Wireless Switch (WS).
*
* @param    L7_uint64 pkts @b{(input)} WLAN packets transmitted.
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWLANPktsTxSet(L7_uint64 pkts);

/*********************************************************************
*
* @purpose  Get the transmitted WLAN packets value for the Wireless Switch (WS).
*
* @param    L7_uint64 *pkts @b{(output)} location to store transmitted
*                                         WLAN packets.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWLANPktsTxGet(L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Set the received WLAN packets value for the Wireless Switch (WS).
*
* @param    L7_uint64 value @b{(input)} WLAN packets received value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWLANPktsRxSet(L7_uint64 pkts);

/*********************************************************************
*
* @purpose  Get the received WLAN packets value for the Wireless Switch (WS).
*
* @param    L7_uint64 *pkts @b{(output)} location to store the received
*                                         WLAN packets value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWLANPktsRxGet(L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Set the dropped transmit WLAN bytes value for the Wireless Switch (WS).
*
* @param    L7_uint64 bytes @b{(input)} WLAN dropped transmit bytes.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWLANDropBytesTxSet(L7_uint64 bytes);

/*********************************************************************
*
* @purpose  Get the dropped transmit WLAN bytes value for the Wireless Switch (WS).
*
* @param    L7_uint64 *bytes @b{(input)} Location to store WLAN dropped transmit
*                                         bytes value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWLANDropBytesTxGet(L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Set the dropped receive WLAN bytes value for the Wireless Switch (WS).
*
* @param    L7_uint64 bytes @b{(input)} WLAN dropped receive bytes.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWLANDropBytesRxSet(L7_uint64 bytes);

/*********************************************************************
*
* @purpose  Get the drop receive WLAN bytes value for the Wireless Switch (WS).
*
* @param    L7_uint64 *bytes @b{(input)} Location to store WLAN dropped receive
*                                         bytes value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWLANDropBytesRxGet(L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Set the drop transmit WLAN packets value for the Wireless Switch (WS).
*
* @param    L7_uint64 pkts @b{(input)} WLAN dropped transmit packets.
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWLANDropPktsTxSet(L7_uint64 pkts);

/*********************************************************************
*
* @purpose  Get the dropped transmit WLAN packets value for the Wireless Switch (WS).
*
* @param    L7_uint64 *pkts @b{(output)} location to store WLAN dropped transmit
*                                         packets.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWLANDropPktsTxGet(L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Set the dropped receive WLAN packets value for the Wireless Switch (WS).
*
* @param    L7_uint64 value @b{(input)} WLAN dropped receive packets value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWLANDropPktsRxSet(L7_uint64 pkts);

/*********************************************************************
*
* @purpose  Get the dropped receive WLAN packets value for the Wireless Switch (WS).
*
* @param    L7_uint64 *pkts @b{(output)} location to store the WLAN dropped receive
*                                         packets value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWLANDropPktsRxGet(L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the WLAN TSPECs accepted for the Wireless Switch (WS).
*
* @param    L7_uint32  acindex @b{(inpout)} AC index value (voice or video)
* @param    L7_uint32 *value   @b{(output)} location to store the data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmTspecTsAcceptedGet(L7_uint32 acindex, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the WLAN TSPECs rejected for the Wireless Switch (WS).
*
* @param    L7_uint32  acindex @b{(inpout)} AC index value (voice or video)
* @param    L7_uint32 *value   @b{(output)} location to store the data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmTspecTsRejectedGet(L7_uint32 acindex, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the WLAN roaming TSPECs accepted for the Wireless Switch (WS).
*
* @param    L7_uint32  acindex @b{(inpout)} AC index value (voice or video)
* @param    L7_uint32 *value   @b{(output)} location to store the data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmTspecRoamTsAcceptedGet(L7_uint32 acindex, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the WLAN roaming TSPECs rejected for the Wireless Switch (WS).
*
* @param    L7_uint32  acindex @b{(inpout)} AC index value (voice or video)
* @param    L7_uint32 *value   @b{(output)} location to store the data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmTspecRoamTsRejectedGet(L7_uint32 acindex, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the WLAN TSPEC Rx packets value for the Wireless Switch (WS).
*
* @param    L7_uint32  acindex @b{(inpout)} AC index value (voice or video)
* @param    L7_uint64 *pkts @b{(output)} location to store the data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmTspecPktsRxGet(L7_uint32 acindex, L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the WLAN TSPEC Rx bytes value for the Wireless Switch (WS).
*
* @param    L7_uint32  acindex @b{(inpout)} AC index value (voice or video)
* @param    L7_uint64 *bytes   @b{(output)} location to store the data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmTspecBytesRxGet(L7_uint32 acindex, L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the WLAN TSPEC Tx packets value for the Wireless Switch (WS).
*
* @param    L7_uint32  acindex @b{(inpout)} AC index value (voice or video)
* @param    L7_uint64 *pkts @b{(output)} location to store the data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmTspecPktsTxGet(L7_uint32 acindex, L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the WLAN TSPEC Tx bytes value for the Wireless Switch (WS).
*
* @param    L7_uint32  acindex @b{(inpout)} AC index value (voice or video)
* @param    L7_uint64 *bytes   @b{(output)} location to store the data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmTspecBytesTxGet(L7_uint32 acindex, L7_uint64 *bytes);

L7_RC_t wdmWSPeerSwitchesGet(L7_ushort16 *value);
L7_RC_t wdmWSStandaloneAPsGet(L7_ushort16 *value);
L7_RC_t wdmWSRogueAPsGet(L7_ushort16 *value);
L7_RC_t wdmWSUnknownAPsGet(L7_ushort16 *value);
L7_RC_t wdmWSUtilizationWLANGet(L7_ushort16 *value);
L7_RC_t wdmWSUtilizationWLANSet(L7_ushort16 value);

/*********************************************************************
*
* @purpose Get the TSPEC total voice TS status of the Wireless switch server.
*
* @param   L7_uint32    *val      @b{(output)} pointer to value output location
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSTspecTotalVoiceTsGet(L7_uint32 *val);

/*********************************************************************
*
* @purpose Get the TSPEC total video TS status of the Wireless switch server.
*
* @param   L7_uint32    *val      @b{(output)} pointer to value output location
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSTspecTotalVideoTsGet(L7_uint32 *val);

/*********************************************************************
*
* @purpose Get the TSPEC total TS clients status of the Wireless switch server.
*
* @param   L7_uint32    *val      @b{(output)} pointer to value output location
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSTspecTotalTsClientsGet(L7_uint32 *val);

/*********************************************************************
*
* @purpose Get the TSPEC total TS roaming clients status of the Wireless switch server.
*
* @param   L7_uint32    *val      @b{(output)} pointer to value output location
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSTspecTotalTsRoamClientsGet(L7_uint32 *val);

L7_RC_t wdmWSAdHocAgeSet(L7_ushort16 age);
L7_RC_t wdmWSAdHocAgeGet(L7_ushort16 *age);
L7_RC_t wdmWSAPFailureAgeSet(L7_ushort16 age);
L7_RC_t wdmWSAPFailureAgeGet(L7_ushort16 *age);
L7_RC_t wdmWSClientFailureAgeSet(L7_ushort16 age);
L7_RC_t wdmWSClientFailureAgeGet(L7_ushort16 *age);
L7_RC_t wdmWSClientRoamAgeSet(L7_ushort16 age);
L7_RC_t wdmWSClientRoamAgeGet(L7_ushort16 *age);
L7_RC_t wdmWSRFScanAgeSet(L7_ushort16 age);
L7_RC_t wdmWSRFScanAgeGet(L7_ushort16 *age);
L7_RC_t wdmWSDetectedClientsAgeSet(L7_ushort16 age);
L7_RC_t wdmWSDetectedClientsAgeGet(L7_ushort16 *age);
L7_RC_t wdmWSAPAuthenticationSet(L7_BOOL mode);
L7_RC_t wdmWSAPAuthenticationGet(L7_BOOL *mode);

/*********************************************************************
*
* @purpose  Get the total number of WS managed APs in the database.
*
* @param    L7_ushort16  value  @b{(output)} pointer to the value 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSTotalAPsGet(L7_ushort16 *value);

/*********************************************************************
*
* @purpose  Get the number of APs that are authenticated, configured, 
*           and have an active connection with WS.
*
* @param    L7_ushort16  value  @b{(output)} pointer to the value
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t wdmWSManagedAPsGet(L7_ushort16 *value);

/*********************************************************************
*
* @purpose  Get the number of APs that were previously authenticated 
*           and managed, but currently do not have connection with WS.
*
* @param    L7_ushort16  value  @b{(output)} pointer to the value
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t wdmWSConnectionFailedAPsGet(L7_ushort16 *value);

/*********************************************************************
*
* @purpose  Get the number of APs that are connected to the switch, 
*           but are not completely configured.
*
* @param    L7_ushort16  value  @b{(output)} pointer to the value
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t wdmWSDiscoveredAPsGet(L7_ushort16 *value);

/*********************************************************************
*
* @purpose  Get the maximum number of APs WS can manage.
*
* @param    L7_ushort16  *value  @b{(output)} pointer to the value 
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t wdmWSMaxAPsGet(L7_ushort16 *value);

/*********************************************************************
*
* @purpose  Get the total authenticated clients in the database.
*
* @param    L7_ushort16  value  @b{(output)} pointer to the value 
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t wdmWSPeerAuthClientsGet(L7_ushort16 *value);

/*********************************************************************
*
* @purpose  Get the total number of clients in the database.
*
* @param    L7_ushort16  value  @b{(output)} pointer to the value 
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t wdmWSTotalClientsGet(L7_ushort16 *value);

/*********************************************************************
*
* @purpose  Get the number of authentictaed clients in the database.
*
* @param    L7_ushort16  value  @b{(output)} pointer to the value 
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments 
*       
* @end
*
*********************************************************************/
L7_RC_t wdmWSAuthenticatedClientsGet(L7_ushort16 *value);

/*********************************************************************
*
* @purpose  Get the total number of clients that associate with the 
*           wireless system.
*
* @param    L7_ushort16  value  @b{(output)} pointer to the value 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSMaxAssocClientsGet(L7_ushort16 *value);

/*********************************************************************
*
* @purpose  Get the count of WS managed APs with Managed state for the WS
*
* @param    L7_ushort16 *value @b{(output)} location to store the count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSManagedAPsWithManagedStatusGet(L7_ushort16 *value);

/*********************************************************************
*
* @purpose  Set WS summary TSPEC statistics
*
* @param    wdmWlanTSGlobalStats_t *pData   @b{(input)} ptr to Tspec stats data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSTspecStatsSet(wdmWlanTSGlobalStats_t *pData);

/*********************************************************************
*
* @purpose  Set the global AP code download file name.
*
* @param    L7_uint32 imageID @b{(input)} Image type ID
* @param    L7_char8 *name @b{(input)} file name, L7_MAX_FILENAME
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadFileNameSet(L7_uint32 imageID,L7_char8 *name);

/*********************************************************************
*
* @purpose  Set the global AP code download file path.
*

* @param    L7_uint32 imageID @b{(input)} image type ID.
* @param    L7_char8 *path @b{(input)} file path, L7_MAX_FILEPATH.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadFilePathSet(L7_uint32 imageID, L7_char8 *path);

/*********************************************************************
*
* @purpose  Check if an entry exists in AP image table.
*
* @param    L7_uint32 imageID @b{(input)} AP image Type ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPImageTypeIDGet(L7_uint32 imageID);

/*********************************************************************
*
* @purpose  Get next AP image type ID entry from AP image table.
*
* @param    L7_uint32  imageID @b{(input)} AP image Type ID
* @param    L7_uint32 *nextImageID @b{(input)} Next AP image Type ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPImageTypeIDNextGet(L7_uint32 imageID,L7_uint32 *nextImageID);

/*********************************************************************
*
* @purpose  Get image type description from AP image capability table.
*
* @param    L7_uint32 imageID @b{(input)} AP image Type ID
* @param    L7_char8  *desc @b{(input)} AP image Description
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPImageTypeDescGet(L7_uint32 imageID, L7_char8 *desc);

/*********************************************************************
*
* @purpose  Get the global AP code download file name.
*
* @param    L7_char8 imageID @b{(input)} image type ID 
* @param    L7_char8 *name @b{(output)} location to store file name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments name buffer must be at least L7_MAX_FILENAME+1.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadFileNameGet(L7_uint32 imageID,L7_char8 *name);

/*********************************************************************
*
* @purpose  Get the global AP code download file path.
*
* @param    L7_uint32 imageID @b{(input)} image type ID.
* @param    L7_char8 *path @b{(output)} location to store file path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments path buffer must be at least L7_MAX_FILEPATH+1.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadFilePathGet(L7_uint32 imageID, L7_char8 *path);

/*********************************************************************
*
* @purpose  Set the global AP code download server IP address.
*
* @param    L7_IP_ADDR_t addr @b{(input)} server IP address.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadServerSet(L7_IP_ADDR_t addr);

/*********************************************************************
*
* @purpose  Get the global AP code download server IP address.
*
* @param    L7_IP_ADDR_t *addr @b{(output)} location to store IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadServerGet(L7_IP_ADDR_t *addr);

/*********************************************************************
*
* @purpose  Set the global AP code download group size.
*
* @param    L7_uchar8 size @b{(input)} group size 1 - max APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This indicates the number of APs to update at a time.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadGroupSizeSet(L7_uchar8 size);

/*********************************************************************
*
* @purpose  Get the global AP code download group size.
*
* @param    L7_uchar8 *size @b{(output)} group size 1 - max APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This indicates the number of APs to update at a time.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadGroupSizeGet(L7_uchar8 *size);

/*********************************************************************
*
* @purpose  Set the global AP code download type.
*
* @param    L7_uint32 type @b{(output)} current download type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadTypeSet(L7_uint32 type);

/*********************************************************************
*
* @purpose  Get the global AP code download type.
*
* @param    L7_uint32 *type @b{(output)} current download type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadTypeGet(L7_uint32 *type);

/*********************************************************************
*
* @purpose  Get the global AP code download status.
*
* @param    L7_WDM_ACTION_STATUS_t *status @b{(output)} current status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadStatusGet(L7_WDM_ACTION_STATUS_t *status);

/*********************************************************************
*
* @purpose  Set the global AP code download status.
*
* @param    L7_WDM_ACTION_STATUS_t status @b{(input)} current status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadStatusSet(L7_WDM_ACTION_STATUS_t status);

/*********************************************************************
*
* @purpose  Start the global AP code download.
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, already in progress or there are no managed APs
*
* @comments This also resets all the status counters.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadStart();

/*********************************************************************
*
* @purpose  Start the code download on the specified managed APs.
*
* @param    L7_enetMacAddr_t  apList  @b{(input)} AP mac addresses
* @param    L7_uint32         numAPs  @b{(input)} number of APs
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This also resets all the status counters.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadMultipleStart(L7_enetMacAddr_t *apList, L7_uint32 numAPs);

/*********************************************************************
*
* @purpose  Abort the code download on managed AP(s).
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadAbort(void);

/*********************************************************************
*
* @purpose  Update code download state and set total APs at the time a
*             code download request is started by the application.
*
* @param    L7_uchar8 total @b{(input)} 1 - max APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadInProgressSet(L7_uchar8 total);

/*********************************************************************
*
* @purpose  Get the total number of APs in the current download request.
*
* @param    L7_uchar8 *total @b{(output)} 1 - max APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadTotalGet(L7_uchar8 *total);

/*********************************************************************
*
* @purpose  Get the number of APs that have successfully downloaded
*             code for the current download request.
*
* @param    L7_uchar8 *count @b{(output)} 1 - max APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadSuccessCountGet(L7_uchar8 *count);

/*********************************************************************
*
* @purpose  Get the number of APs that have failed to download
*             code for the current download request.
*
* @param    L7_uchar8 *count @b{(output)} 1 - max APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadFailCountGet(L7_uchar8 *count);

/*********************************************************************
*
* @purpose  Get the number of managed APs on which the code download 
*           is aborted.
*
* @param    L7_uchar8 *count  @b{(output)} pointer to the count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadAbortCountGet(L7_uchar8 *count);

/*********************************************************************
*
* @purpose  Get the total number of reset count in the download request.
*
* @param    L7_uchar8  *total  @b{(output)} 1 - max APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadResetCountGet(L7_uchar8 *count);

/*********************************************************************
*
* @purpose  Set the number of APs that have successfully downloaded
*             code for the current download request.
*
* @param    L7_uchar8 count @b{(input)} 1 - max APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadSuccessCountSet(L7_uchar8 count);

/*********************************************************************
*
* @purpose  Set the number of APs that have failed to download
*             code for the current download request.
*
* @param    L7_uchar8 count @b{(input)} 1 - max APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPCodeDownloadFailCountSet(L7_uchar8 count);

/*********************************************************************
*
* @purpose  Set the AP validation value for the Wireless Switch (WS).
*
* @param    L7_uint32 value @b{(input)} new AP validation value,
*                                       L7_WDM_AP_VALIDATION_LOCAL or
*                                       L7_WDM_AP_VALIDATION_RADIUS
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSAPValidationSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the AP validation value for the Wireless Switch (WS).
*
* @param    L7_uint32 *value @b{(output)} location to store the AP
*                                         validation value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSAPValidationGet(L7_uint32 *value);

/*********************************************************************
*
* @purpose  Determine if phy mode is supported for configured country.
*
* @param    L7_WDM_RADIO_PHY_MODE_t  mode  physical radio mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, not supported
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSRadioPhyModeSupported(L7_WDM_RADIO_PHY_MODE_t mode);

/*********************************************************************
*
* @purpose  Determine if channel is supported based on configured country.
*
* @param    L7_uchar8    channel     @b{(input)}  channel.
* @param    L7_ushort16 *phyModesMask    @b{(output)} physical radio modes for channel.
* @param    L7_BOOL     *rdrDetRequired  @b{(output)} if radar detection
* @param                                  required or not.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, channel not supported.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSChannelEntryGet(L7_uchar8                channel,
                             L7_uint32               *phyModesMask,
                             L7_BOOL                 *rdrDetRequired);

/*********************************************************************
*
* @purpose  Get list of supported channels based on configured country.
*
* @param    L7_uchar8    channel     @b{(input)}  previous channel, 0 to start.
* @param    L7_uchar8   *nextChannel @b{(output)} next supported channel.
* @param    L7_ushort16 *phyModesMask    @b{(output)} physical radio modes for 
@ @param                             @b{(output)} channel.
* @param    L7_BOOL  *rdrDetRequired  @b{(output)} if radar detection required.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, end of list
*
* @comments channel list is ordered based on channel number.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSChannelEntryNextGet(L7_uchar8                channel,
                                 L7_uchar8               *nextChannel,
                                 L7_uint32               *phyModesMask,
                                 L7_BOOL                 *rdrDetRequired);

/*********************************************************************
*
* @purpose  Get the first channel from the all-country aggregate channel list.
*
* @param    L7_uchar8   *channel @b{(output)} first supported channel.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSChannelListEntryGetFirst(L7_uchar8 *channel);

/*********************************************************************
*
* @purpose  Get the next channel from the all-country aggregate channel list.
*
* @param    L7_uchar8    channel     @b{(input)}  previous channel, 0 to start.
* @param    L7_uchar8   *nextChannel @b{(output)} next supported channel.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, end of list
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSChannelListEntryNextGet(L7_uchar8 channel, L7_uchar8 *nextChannel);

/*********************************************************************
*
* @purpose  Determine if channel is supported based on configured country for a PHY Mode.
*
* @param    L7_WDM_RADIO_PHY_MODE_t phyMode     @b{(input)} PHY Mode
* @param    L7_uchar8    channel     @b{(input)}  channel.
* @param    L7_uint32   *phyModeMask    @b{(output)} physical radio modes for channel.
* @param    L7_BOOL     *rdrDetRequired @b{(output)} If radar detection required.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, channel not supported.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSPhyModeChannelEntryGet(L7_WDM_RADIO_PHY_MODE_t phyMode,
                            L7_uchar8 channel, L7_uint32 *phyModesMask,
                            L7_BOOL *rdrDetRequired);

/*********************************************************************
*
* @purpose  Get list of supported channels based on configured country for a PHY Mode.
*
* @param    L7_WDM_RADIO_PHY_MODE_t phyMode     @b{(input)} PHY Mode
* @param    L7_uchar8    channel     @b{(input)}  previous channel, 0 to start.
* @param    L7_uchar8   *nextChannel @b{(output)} next supported channel.
* @param    L7_uint32   *phyModeMask    @b{(output)} physical radio modes for channel.
* @param    L7_BOOL     *rdrDetRequired @b{(output)} If radar detection required
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, end of list
*
* @comments channel list is ordered based on channel number.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSPhyModeChannelEntryNextGet(L7_WDM_RADIO_PHY_MODE_t phyMode,
                            L7_uchar8 channel, L7_uchar8 *nextChannel,
                            L7_uint32 *phyModesMask, L7_BOOL *rdrDetRequired);



/*********************************************************************
*
* @purpose  Copy list of supported channels based on configured country
*           for a PHY Mode.
*
* @param    L7_WDM_RADIO_PHY_MODE_t phyMode    @b{(input)} PHY Mode
* @param    L7_uchar8 *pChannels   @b{(output)} list of channel values
* @param    L7_uint32 *pNumCopied  @b{(output)} number of copied values
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Output array must be a least L7_WDM_REG_ALL_CHANNELS
*           L7_uchar8's long.  
*
* @end
*
*********************************************************************/

L7_RC_t wdmWSPhyModeChannelsCopy(L7_WDM_RADIO_PHY_MODE_t phyMode,
                                 L7_uchar8 *pChannels,
                                 L7_uint32 *pNumCopied);


/*********************************************************************
*
* @purpose  Look up a valid country entry.
*
* @param    L7_uchar8 index @b{(input)}  country code index
* @param    L7_char8 *code  @b{(output)} country code
* @param    L7_char8 *str  @b{(output)}  country string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, invalid index
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSCountryCodeEntryGet(L7_uchar8 index,
                                 L7_char8 *code,
                                 L7_char8 *str);

/*********************************************************************
*
* @purpose  Get next valid country entry.
*
* @param    L7_uchar8 index @b{(input)}  country code index
* @param    L7_char8 *next  @b{(output)} next country code index
* @param    L7_char8 *code  @b{(output)} country code
*                                        (L7_WDM_COUNTRY_CODE_SIZE + NULL byte)
* @param    L7_char8 *str   @b{(output)} country string
*                                        (L7_WDM_COUNTRY_STRING_LENGTH_MAX + NULL byte)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, no more entries
*
* @comments index == 0 returns first entry.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSCountryCodeEntryNextGet(L7_uchar8  index,
                                     L7_uchar8 *next,
                                     L7_char8  *code,
                                     L7_char8  *str);

/*********************************************************************
*
* @purpose  Set the country code for the Wireless Switch (WS).
*
* @param    L7_uchar8 index @b{(input)} country code index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, wireless system is enabled
*
* @comments country configuration can only be modified when
*           the wireless system is disabled.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSCountryCodeSet(L7_uchar8 index);

/*********************************************************************
*
* @purpose  Get the country code for the Wireless Switch (WS).
*
* @param    L7_uchar8 *index @b{(output)} location to store the country index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSCountryCodeGet(L7_uchar8 *index);

/*********************************************************************
*
* @purpose  Get the regulatory domains for the Wireless Switch (WS).
*
* @param    L7_char8 *twoGHz @b{(output)} 2GHz regulatory domain
* @param    L7_char8 *fiveGHz @b{(output)} 5GHz regulatory domain
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Regulatory domains are returned based on the country code
* @comments configured
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSRegDomainsGet(L7_char8 *twoGHz, L7_char8* fiveGHz);

/*********************************************************************
*
* @purpose  Add the IP addres to the discovery IP list for the Wireless
*           Switch (WS).
*
* @param    L7_IP_ADDR_t addr @b{(input)} new IP address to be added,
*                                         in the form a.b.c.d
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSIPListEntryAdd(L7_IP_ADDR_t addr);

/*********************************************************************
*
* @purpose  Set the status of the discovered IP in the list for the Wireless
*           Switch (WS).
*
* @param    addr @b{(input)} to locate the status of the IP address index
* @param    status @b{(input)} new status of the discovered IP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSIPListStatusSet(L7_IP_ADDR_t addr, L7_WDM_IP_STATUS_t status);

/*********************************************************************
*
* @purpose  Get the status of the discovered IP in the list for the Wireless
*           Switch (WS).
*
* @param    L7_IP_ADDR_t        addr   @b{(input)}  IP Address
* @param    L7_WDM_IP_STATUS_t *status @b{(output)} status for configured IP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSIPListStatusGet(L7_IP_ADDR_t addr, L7_WDM_IP_STATUS_t *status);

L7_RC_t wdmWSIPListEntryDelete(L7_IP_ADDR_t addr);
L7_RC_t wdmWSIPListEntryPurge();
L7_RC_t wdmWSIPListEntryGet(L7_IP_ADDR_t addr);
L7_RC_t wdmWSIPListEntryNextGet(L7_IP_ADDR_t addr, L7_IP_ADDR_t *nextAddr);
L7_RC_t wdmWSVLANListEntryAdd(L7_ushort16 vlanId);
L7_RC_t wdmWSVLANListEntryDelete(L7_ushort16 vlanId);
L7_RC_t wdmWSVLANListEntryPurge();
L7_RC_t wdmWSVLANListEntryGet(L7_ushort16 vlanId);
L7_RC_t wdmWSVLANListEntryNextGet(L7_ushort16 vlanId, L7_ushort16 *nextVlanId);
L7_RC_t wdmWSIPDiscoverySet(L7_BOOL mode);
L7_RC_t wdmWSIPDiscoveryGet(L7_BOOL *mode);
L7_RC_t wdmWSL2DiscoverySet(L7_BOOL mode);
L7_RC_t wdmWSL2DiscoveryGet(L7_BOOL *mode);
L7_RC_t wdmWSRFDiscoverySet(L7_BOOL mode);
L7_RC_t wdmWSRFDiscoveryGet(L7_BOOL *mode);
L7_RC_t wdmWSPeerGroupSet(L7_uint32 id);
L7_RC_t wdmWSPeerGroupGet(L7_uint32 *id);
L7_RC_t wdmWSTunnelMTUSet(L7_uint32 mtu);
L7_RC_t wdmWSTunnelMTUGet(L7_uint32 *mtu);

/*********************************************************************
*
* @purpose Set the TSPEC client violation report interval value of the Wireless switch.
*
* @param L7_uint32 val @b{(input)} value
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSTspecViolationReportIntervalSet(L7_uint32 val);

/*********************************************************************
*
* @purpose Get the TSPEC client violation report interval value of the Wireless switch.
*
* @param L7_uint32 *val @b{(input)} pointer to value output location
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSTspecViolationReportIntervalGet(L7_uint32 *val);

/*********************************************************************
*
* @purpose Enable/Disable a trap group for the wireless system.
*
* @param   L7_WDM_TRAP_FLAGS_t  trap @b{(input)} trap group flag
* @param   L7_uint32            mode @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSTrapModeSet(L7_WDM_TRAP_FLAGS_t flag,
                         L7_uint32           mode);

/*********************************************************************
*
* @purpose Enable/Disable a trap group for the wireless system.
*
* @param   L7_WDM_TRAP_FLAGS_t  trap @b{(input)} trap group flag
* @param   L7_uint32           *mode @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSTrapModeGet(L7_WDM_TRAP_FLAGS_t flag,
                         L7_uint32          *mode);


/*********************************************************************
*
* @purpose Enable/Disable a peer config push group for the wireless system.
*
* @param   L7_WDM_CONFIG_PUSH_MASK_t  mask @b{(input)} config push mask
* @param   L7_uint32                  mode @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSConfigPushSet(L7_WDM_CONFIG_PUSH_MASK_t mask,
                           L7_uint32                 mode);

/*********************************************************************
*
* @purpose Status of a config push group for the wireless system.
*
* @param   L7_WDM_CONFIG_PUSH_MASK_t  mask @b{(input)} config group mask
* @param   L7_uint32                 *mode @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSConfigPushGet(L7_WDM_CONFIG_PUSH_MASK_t mask,
                           L7_uint32                *mode);

/*********************************************************************
*
* @purpose Get config push group status MASK for the wireless system
*          in one gulp.
*
* @param   L7_uint32                 *mask @b{(output)}
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSConfigPushMaskGet(L7_uint32  *mask);

/*********************************************************************
*
* @purpose  Start the config push to all peer switches.
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, already in progress or there are no peer switches
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSConfigPushStart();

/*********************************************************************
*
* @purpose  Start the config push to given peer switch.
*
* @param    L7_IP_ADDR_t peerSwitchIP   @b{(input)} Peer switch IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, already in progress or if peer switch not present
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t wdmPeerSwitchConfigPushStart(L7_IP_ADDR_t ipAddr);

/*********************************************************************
*
* @purpose  Start the config push to the specified peer switches.
*
* @param    L7_IP_ADDR_t *ipList   @b{(input)} Peer switch IP Address
* @param    L7_uint32    numWs    @b{(input)} number of peer switches

* @returns  L7_SUCCESS
* @returns  L7_FAILURE, already in progress 
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSConfigPushMultipleStart(L7_IP_ADDR_t *ipList ,L7_uint32  numWs);

/*********************************************************************
*
* @purpose function to Get last peer switch IP address that pushed config 
*          to the switch      
*
* @param   L7_IP_ADDR_t  ipAddr  @b{(output)} Peer switch IP address that 
*                                             pushed config.
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSConfigPushRxPeerSwitchIPAddrGet(L7_IP_ADDR_t *ipAddr);


/*********************************************************************
*
* @purpose function to Get time of config push from peer switch
*
* @param   L7_uint32     rxTime  @b{(output)} config receive time
*
* @returns L7_SUCCESS 
*          L7_FAILURE
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSConfigPushRxTimeGet(L7_uint32 *rxTime);

/*********************************************************************
*
* @purpose function to update config push receive parameters 
*      
* @param    L7_IP_ADDR_t peerSwitchIP   @b{(input)} IP Address
* @param    L7_uint32    mask           @b{(input)} config push receive mask
* @param    L7_WDM_CONFIG_PUSH_RX_STATUS_t   status   @b{(input)} config push 
*                                       receive status
* @param    L7_uint32   rxTime          @b{(input)} config push received time
*
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSConfigPushRxSet(L7_IP_ADDR_t peerSwitchIP,
                             L7_uint32 mask,
                             L7_uint32 rxTime );


/*********************************************************************
* @purpose  Sets peer switch config push status.
*
* @param    ipAddr      @b{(input)} IP address of the switch
* @param    cpIPAddr    @b{(input)} IP address of the switch pushing config
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
L7_RC_t wdmPeerSwitchConfigPushStatusSet(L7_IP_ADDR_t ipAddr,
                                         L7_IP_ADDR_t cpIPAddr,
                                         L7_uint32 mask,
                                         L7_uint32 rxTime);

/*********************************************************************
*
* @purpose function to Set status of config push received from peer switch
*
* @param    rxStatus    @b{(output)} config push receive status
*
* @returns L7_SUCCESS 
*          L7_FAILURE
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSConfigPushRxStatusSet(L7_WDM_CONFIG_PUSH_RX_STATUS_t rxStatus);

/*********************************************************************
*
* @purpose function to Get status of config push send to peer switch
*
* @param   txStatus    @b{(input)} config push send status
*
* @returns L7_SUCCESS 
*          L7_FAILURE
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSConfigPushSendStatusSet(L7_WDM_CONFIG_PUSH_SEND_STATUS_t txStatus);

/*********************************************************************
*
* @purpose function to Get status of config push received from peer switch
*
* @param    rxStatus    @b{(output)} config push receive status
*
* @returns L7_SUCCESS 
*          L7_FAILURE
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSConfigPushRxStatusGet(L7_WDM_CONFIG_PUSH_RX_STATUS_t *rxStatus);

/*********************************************************************
*
* @purpose function to Get status of config push receive from peer switch
*
* @param    rxMask    @b{(output)} config push receive mask
*
* @returns L7_SUCCESS 
*          L7_FAILURE
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSConfigPushRxMaskGet(L7_uint32 *rxMask);


/*********************************************************************
*
* @purpose function to Get status of config push send to peer switch
*
* @param    txStatus    @b{(output)} config push send status
*
* @returns L7_SUCCESS 
*          L7_FAILURE
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSConfigPushReqStatusGet(L7_WDM_CONFIG_PUSH_SEND_STATUS_t *txStatus);


/*********************************************************************
*
* @purpose Reset all wireless statistics.
*
* @param
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE, clear in progress.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmStatisticsClear();

/*********************************************************************
*
* @purpose  Get the global status for a statistics clear operation.
*
* @param    L7_WDM_ACTION_STATUS_t *status @b{(output)} current clear status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmStatisticsClearStatusGet(L7_WDM_ACTION_STATUS_t *status);

/*********************************************************************
*
* @purpose  Set the global status for a statistics clear operation.
*
* @param    L7_WDM_ACTION_STATUS_t status @b{(input)} current clear status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmStatisticsClearStatusSet(L7_WDM_ACTION_STATUS_t status);

/*********************************************************************
*
* @purpose  Get the number of peer switches config push has been requested.
*
* @param    L7_uchar8 count @b{(output)} 1 - max peer switches.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSConfigPushTotalCountGet(L7_uchar8 *count);

/*********************************************************************
*
* @purpose  Set the number of peer switches that have failed config
*           push request.
*
* @param    L7_uchar8 count @b{(input)} 1 - max peer switches.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSConfigPushFailCountIncr();


/*********************************************************************
*
* @purpose  Get the number of peer switches that have failed config
*           push request.
*
* @param    L7_uchar8 count @b{(output)} 1 - max peer switches.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSConfigPushFailCountGet(L7_uchar8 *count);

/*********************************************************************
*
* @purpose  Set the number of peer switches that have succeded config
*           push request.
*
* @param    L7_uchar8 count @b{(input)} 1 - max peer switches.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSConfigPushSuccCountIncr();

/*********************************************************************
*
* @purpose  Get the number of peer switches that have succeded config
*           push request.
*
* @param    L7_uchar8 count @b{(output)} 1 - max peer switches.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSConfigPushSuccCountGet(L7_uchar8 *count);

/*********************************************************************
*
* @purpose Initialize peer config push data for sending to peer switch.
*
* @param   None
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSConfigPushSendDataInit();

/*********************************************************************
*
* @purpose Reset peer config push data after sending to peer switch.
*
* @param   None
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSConfigPushSendDataReset();

/*********************************************************************
*
* @purpose Read peer config push data for sending to peer switch.
*
* @param   L7_uchar8 buf @b{(input/output)} buffer pointer to return data.
* @param   L7_uchar8 len @b{(input)} len of data to return.
*
* @returns on success number of bytes actually returned in the buffer.
* @retruns -1 on failure to return the data.
*
* @comments
*
* @end
*
*********************************************************************/
L7_int32 wdmWSConfigPushSendDataGet(L7_uchar8 *buf, L7_uint32 len);


/*********************************************************************
*
* @purpose Initialize peer config push data for receiving from peer switch.
*
* @param   None
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSConfigPushRxDataInit();


/*********************************************************************
*
* @purpose Save peer config push data after receiving from peer switch.
*
* @param   L7_uchar8 buf @b{(input)} buffer pointer to data.
* @param   L7_uchar8 len @b{(input)} len of data to return.
*
* @returns L7_SUCCESS on saving config successfully.
* @retruns L7_FAILURE if any errors.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSConfigPushRxDataSet(L7_uchar8 *buf, L7_uint32 len);

/*********************************************************************
*
* @purpose function to get country code change status
*      
* @param    none   
*
* @returns L7_BOOL L7_TRUE
* @returns L7_BOOL L7_FALSE
*          
*
* @notes 
*
* @end
*
*********************************************************************/
L7_BOOL wdmWSConfigPushCountryCodeChanged(void);

/*********************************************************************
*
* @purpose  Add AP MAC Address to the Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_TABLE_IS_FULL
*
* @comments AP entries are added in the ascending order, based on mac
* @comments address.
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPEntryAdd (L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Delete AP MAC Address from the Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPEntryDelete (L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Delete all AP MAC Addresses from the Local AP Database
*
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPEntryDeleteAll ( );

/*********************************************************************
*
* @purpose  Find if the AP MAC Address exists in the Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPEntryGet (L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Find the next AP MAC Address in the Local AP Database
*
* @param    L7_enetMacAddr_t *macAddr @b{(input)} AP MAC Address
* @param    L7_enetMacAddr_t *macAddr @b{(output)} AP MAC Address of the next AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPEntryNextGet (L7_enetMacAddr_t *macAddr);

/*********************************************************************
*
* @purpose  Get the AP MAC Address count in the Local AP Database
*
* @param    L7_uint32 *count @b{(output)} AP MAC Address Entry Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPEntryCountGet (L7_uint32 *count);

/*********************************************************************
*
* @purpose  Set the Location Information of the AP in the Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_char8 *location @b{(input)} Location String for the AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPLocationSet (L7_enetMacAddr_t macAddr, L7_char8 *location);

/*********************************************************************
*
* @purpose  Get the Location Information of the AP from the Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_char8 *location @b{(output)} Location String of the AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPLocationGet (L7_enetMacAddr_t macAddr, L7_char8 *location);

/*********************************************************************
*
* @purpose  Set the Managed Mode of the AP in the Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_WDM_AP_MANAGED_MODE mode @b{(input)} Managed Mode of the AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPModeSet (L7_enetMacAddr_t macAddr, L7_WDM_AP_MANAGED_MODE_t mode);

/*********************************************************************
*
* @purpose  Get the Managed Mode of the AP in the Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_WDM_AP_MANAGED_MODE *mode @b{(output)} Managed Mode of the AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPModeGet (L7_enetMacAddr_t macAddr, L7_WDM_AP_MANAGED_MODE_t *mode);

/*********************************************************************
*
* @purpose  Set the Authentication Password of the AP in the Local AP Database
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)} AP MAC Address
* @param    L7_char8        *password  @b{(input)} Authentication Password
* @param    L7_BOOL          encrypted @b{(input)} L7_TRUE if password in encrypted format
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPPasswordSet(L7_enetMacAddr_t macAddr, L7_char8 *password, L7_BOOL encrypted);

/*********************************************************************
*
* @purpose  Get the Authentication Password of the AP in the Local AP Database
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)} AP MAC Address
* @param    L7_char8        *password  @b{(input)} Authentication Password
* @param    L7_BOOL          encrypted @b{(input)} L7_TRUE if password in encrypted format
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPPasswordGet(L7_enetMacAddr_t macAddr, L7_char8 *password, L7_BOOL encrypted);

/*********************************************************************
*
* @purpose  Check if the Authencation Password is configured or not.
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_BOOL *pVal @b{(output)} Password is configured or not.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPPasswordConfigured (L7_enetMacAddr_t macAddr, L7_BOOL *pVal);

/*********************************************************************
*
* @purpose  Set the Profile ID of the AP in the Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uint32 profileId @b{(input)} Profile ID of the AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileIDSet (L7_enetMacAddr_t macAddr, L7_uint32 profileId);

/*********************************************************************
*
* @purpose  Get the Profile ID of the AP in the Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uint32 *profileId @b{(output)} Profile ID of the AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProfileIDGet (L7_enetMacAddr_t macAddr, L7_uint32 *profileId);

/*********************************************************************
*
* @purpose  Set a fixed channel per radio for an AP database entry.
*
* @param    L7_enetMacAddr_t macAddr    @b{(input)} AP MAC Address
* @param    L7_uint32        radioIndex @b{(input)} Radio Index to be configured
* @param    L7_uchar8        channel    @b{(input)} 0 (Auto) or fixed channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPRadioChannelSet(L7_enetMacAddr_t macAddr,
                             L7_uchar8        radioIndex,
                             L7_uchar8        channel);

/*********************************************************************
*
* @purpose  Get a fixed channel per radio for an AP database entry.
*
* @param    L7_enetMacAddr_t macAddr    @b{(input)}  AP MAC Address
* @param    L7_uint32        radioIndex @b{(input)}  Radio Index to be configured
* @param    L7_uchar8       *channel    @b{(output)} 0 (Auto) or fixed channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPRadioChannelGet(L7_enetMacAddr_t macAddr,
                             L7_uchar8        radioIndex,
                             L7_uchar8       *channel);

/*********************************************************************
*
* @purpose  Set fixed power for a physical AP in the local database.
*
* @param    L7_enetMacAddr_t macAddr    @b{(input)}  AP MAC Address
* @param    L7_uint32        radioIndex @b{(input)}  Radio Index to be configured
* @param    L7_uchar8        power      @b{(input)}  0 (Auto) or fixed power
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPRadioPowerSet(L7_enetMacAddr_t macAddr,
                           L7_uchar8        radioIndex,
                           L7_uchar8        power);

/*********************************************************************
*
* @purpose  Set fixed power for a physical AP in the local database.
*
* @param    L7_enetMacAddr_t macAddr    @b{(input)}  AP MAC Address
* @param    L7_uint32        radioIndex @b{(input)}  Radio Index to be configured
* @param    L7_uchar8       *power      @b{(output)}  0 (Auto) or fixed power
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPRadioPowerGet(L7_enetMacAddr_t macAddr,
                           L7_uchar8        radioIndex,
                           L7_uchar8       *power);



/*********************************************************************
*
* @purpose  Enable/disable radio resource measurement for the physical
*           AP and radio in the local database.
*
* @param    L7_enetMacAddr_t macAddr    @b{(input)}  AP MAC Address
* @param    L7_uint32        radioIndex @b{(input)}  Radio Index to be configured
* @param    L7_uchar8        power      @b{(input)}  0 (Auto) or fixed power
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
wdmAPRadioResourceMeasEnabledSet(L7_enetMacAddr_t macAddr,
                                 L7_uchar8        radioIndex,
                                 L7_BOOL          bEnable);



/*********************************************************************
*
* @purpose  To get radio resource measurement status for the physical
*           AP and radio in the local database.
*
* @param    L7_enetMacAddr_t macAddr    @b{(input)}  AP MAC Address
* @param    L7_uint32        radioIndex @b{(input)}  Radio Index to be configured
* @param    L7_BOOL         *pEnabled   @b{(output)} status
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
wdmAPRadioResourceMeasEnabledGet(L7_enetMacAddr_t macAddr,
                                 L7_uchar8        radioIndex,
                                 L7_BOOL          *pEnabled);


/*********************************************************************
*
* @purpose  Validate an AP MAC address in the local DB and return configuration.
*
* @param    L7_enetMacAddr_t          macAddr       @b{(input)}  AP MAC Address
* @param    L7_WDM_AP_MANAGED_MODE_t *mode          @b{(output)} configured managed mode
* @param    L7_char8                 *location      @b{(output)} configured location
* @param    L7_uint32                *profileId     @b{(output)} configured profile ID
* @param    L7_uchar8                *radio1Power   @b{(output)} fixed power for radio
* @param    L7_uchar8                *radio1Channel @b{(output)} fixed channel for radio
* @param    L7_uchar8                *radio2Power   @b{(output)} fixed power for radio
* @param    L7_uchar8                *radio2Channel @b{(output)} fixed channel for radio
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPValidationParamsGet(L7_enetMacAddr_t          macAddr,
                                 L7_WDM_AP_MANAGED_MODE_t *mode,
                                 L7_char8                 *location,
                                 L7_uint32                *profileId,
                                 L7_uchar8                *radio1Power,
                                 L7_uchar8                *radio1Channel,
                                 L7_uchar8                *radio2Power,
                                 L7_uchar8                *radio2Channel);

/*********************************************************************
*
* @purpose  Set the AP client QoS mode of the Wireless switch server.
*
* @param    mode        @b{(input)} AP client QoS mode (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Setting the mode to L7_ENABLE enables AP client QoS in the Wireless switch.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSAPClientQosModeSet(L7_uint32 mode);

/*********************************************************************
*
* @purpose  Get the AP client QoS mode of the Wireless switch server.
*
* @param    *mode       @b{(output)} Ptr to AP client QoS mode output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSAPClientQosModeGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose function to Get wireless system IP address
*
* @param    ipAddr    @b{(output)} Wireless system IP Address
*
* @returns L7_SUCCESS, wireless system IP address configured
*          L7_FAILURE
*
* @notes this should not block in any manner, or do any lengthy processing,
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSIPAddressGet(L7_IP_ADDR_t *ipAddr);


/*********************************************************************
*
* @purpose  Function to Get wireless system IP Mask
*
* @param    L7_IP_MASK_t *ipAddr @b{(output)} Wireless system IP Mask
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSIPMaskGet(L7_IP_MASK_t *ipMask);

/*********************************************************************
* @purpose  This function sets specified debug flag.
*
* @param    level
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void wirelessDebugLevelSet(L7_uint32 level);

/*********************************************************************
* @purpose  This function clears specified debug flag.
*
* @param    level
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void wirelessDebugLevelClear(L7_uint32 level);

/*********************************************************************
* @purpose  This function clears all wireless debug flags.
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wirelessDebugReset(void);

/*********************************************************************
*
* @purpose  Check if the tunnel MTU has been changed for the Wireless Switch (WS).
*
* @param    
*
* @returns  L7_TRUE     if the tunnel MTU has been changed
* @returns  L7_FALSE    if the tunnel MTU has not been changed
*
* @comments none
*
* @end
*
*********************************************************************/
L7_BOOL wdmWSMtuChanged(void);

/*********************************************************************
*
* @purpose  Reset the tunnel MTU changed flag for the Wireless Switch (WS).
*
* @param    
*
* @returns  none
*
* @comments none
*
* @end
*
*********************************************************************/
void wdmWSMtuChangedReset(void);

/*********************************************************************
*
* @purpose  Check if the AP client QoS config has been changed for the Wireless Switch (WS).
*
* @param    
*
* @returns  L7_TRUE     mode changed
* @returns  L7_FALSE    mode not changed
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL wdmWSApClientQosChanged(void);

/*********************************************************************
*
* @purpose  Reset the AP client QoS changed flag for the Wireless Switch (WS).
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void wdmWSApClientQosChangedReset(void);

/*********************************************************************
* @purpose  To retrieve the Cluster Priority.
*
* @param    prio   @b{(output)} Priority
*
* @returns   L7_SUCCESS
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmWidsPriorityGet (L7_uchar8 *prio);

/*********************************************************************
* @purpose  To set the Cluster Priority.
*
* @param    prio   @b{(input)} Priority
*
* @returns   L7_SUCCESS
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmWidsPrioritySet (L7_uchar8 prio);

/*********************************************************************
* @purpose  Get the AP auto upgrade feature mode configured on the WS
*
* @param    L7_uint32  upgradeMode  @b{(output)} pointer to mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t wdmAPAutoUpgradeModeGet(L7_uint32 *upgradeMode);

/*********************************************************************
* @purpose  Set the AP auto upgrade feature mode on the WS
*
* @param    L7_uint32  upgradeMode  @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t wdmAPAutoUpgradeModeSet(L7_uint32 upgradeMode);

/*********************************************************************
* @purpose  Check if the cluster switches have different AP image 
*           download modes
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t wdmAPAutoUpdImageModeMixed(void);

/*********************************************************************
* @purpose  To set the Cluster Controller.
*
* @param    ipAddr   @b{(input)} Cluster Controller IP Address
*
* @returns   L7_SUCCESS
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmWidsControllerSet (L7_IP_ADDR_t ipAddr);

/*********************************************************************
* @purpose  To get the Cluster Controller
*
* @param    ipAddr   @b{(output)} Cluster Controller
*
* @returns   L7_SUCCESS
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmWidsControllerGet (L7_IP_ADDR_t *ipAddr);

/*********************************************************************
* @purpose  To get the Cluster Controller Indicator
*
* @param    ctrlInd   @b{(output)} Cluster Controller indicator
*
* @returns   L7_SUCCESS
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmWidsControllerIndGet (L7_BOOL *ctrlInd);


/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled or disabled
*
* @param    L7_WIDS_AP_ROGUE_CLASSIFICATION_TEST_t testId @b{(input)} test identifier.
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectTestModeGet(L7_WIDS_AP_ROGUE_CLASSIFICATION_TEST_t testId, L7_uint32 *mode);

/*********************************************************************
* @purpose  To retrieve the Administrator-Configured Rogue Detection mode
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments L7_ENABLE : If local database indicates that the AP is rogue 
*           then report the AP as rogue in the RF Scan.
*
* @end
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectAdminConfiguredGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Set the Administrator-Configured Rogue Detection mode.
*
* @param    L7_uint32 mode @b{(input)} new mode value, L7_ENABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments L7_ENABLE : If local database indicates that the AP is rogue 
*           then report the AP as rogue in the RF Scan.
*           Decided that this is always enabled ... can not be disabled.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectAdminConfiguredSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled or disabled
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Managed SSID Received from an unknown AP Rogue Detection
*
* @end
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectManagedSsidUnknownApGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Set the Managed SSID Received from an unknown AP Rogue Detection mode.
*
* @purpose  Enable or disable rogue reporting for this test.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Managed SSID Received from an unknown AP Rogue Detection
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectManagedSsidUnknownApSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled or disabled
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Managed SSID Received from a fake managed AP Rogue Detection
*
* @end
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectManagedSsidFakeManagedApGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Set the Managed SSID Received from an unknown AP Rogue Detection mode.
*
* @purpose  Enable or disable rogue reporting for this test.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Managed SSID Received from a fake managed AP Rogue Detection
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectManagedSsidFakeManagedApSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled or disabled
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Beacon received from a fake managed AP without SSID Rogue Detection
*
* @end
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectFakeManagedApNoSsidGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Set the Beacon received from a fake managed AP without SSID Rogue Detection mode.
*
* @purpose  Enable or disable rogue reporting for this test.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Beacon received from a fake managed AP without SSID Rogue Detection
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectFakeManagedApNoSsidSet(L7_uint32 mode);

/*********************************************************************
*
* @purpose  Set the Beacon received from a fake managed AP with an invalid channel Rogue Detection mode.
*
* @purpose  Enable or disable rogue reporting for this test.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Beacon received from a fake managed AP with an invalid channel Rogue Detection
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectFakeManagedApChanInvalidSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled or disabled
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Beacon received from a fake managed AP with an invalid channel Rogue Detection
*
* @end
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectFakeManagedApChanInvalidGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled or disabled
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Managed SSID detected with incorrect security configuration Rogue Detection
*
* @end
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectManagedSsidSecurityInvalidGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Set the Managed SSID detected with incorrect security configuration Rogue Detection mode.
*
* @purpose  Enable or disable rogue reporting for this test.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Managed SSID detected with incorrect security configuration Rogue Detection
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectManagedSsidSecurityInvalidSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled or disabled
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Invalid SSID Received from a managed AP Rogue Detection
*
* @end
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectManagedApSsidInvalidGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Set the Invalid SSID Received from a managed AP Rogue Detection mode.
*
* @purpose  Enable or disable rogue reporting for this test.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Invalid SSID Received from a managed AP Rogue Detection
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectManagedApSsidInvalidSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled or disabled
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments AP is operating on an illegal channel Rogue Detection
*
* @end
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectChanIllegalGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Enable or disable rogue reporting for this test.
*
* @param    L7_uint32 mode @b{(input)} new mode value, L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments AP is operating on an illegal channel Rogue Detection
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectChanIllegalSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled or disabled
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Stand-alone AP is operating with unexpected channel, SSID, security, or WIDS mode Rogue Detection
*
* @end
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectStandaloneConfigInvalidGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Enable or disable rogue reporting for this test.
*
* @param    L7_uint32 mode @b{(input)} new mode value, L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Stand-alone AP is operating with unexpected channel, SSID, security, or WIDS mode Rogue Detection
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectStandaloneConfigInvalidSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled or disabled
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Unexpected WDS device is detected on the network Rogue Detection
*
* @end
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectWdsDeviceInvalidGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Enable or disable rogue reporting for this test.
*
* @param    L7_uint32 mode @b{(input)} new mode value, L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Unexpected WDS device is detected on the network Rogue Detection
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectWdsDeviceInvalidSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled or disabled
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Unmanaged AP is detected on a wired network Rogue Detection
*
* @end
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectWiredUnManagedGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Enable or disable rogue reporting for this test.
*
* @param    L7_uint32 mode @b{(input)} new mode value, L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Unmanaged AP is detected on a wired network Rogue Detection
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectWiredUnManagedSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether the AP De-authentication attack is enabled or disabled
*
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None
*
* @end
*********************************************************************/
L7_RC_t wdmWidsAPDeAuthenticationAttackGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Enable/Disable the AP De-authentication attack.
*
* @param    L7_uint32 mode @b{(input)} new mode value, L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsAPDeAuthenticationAttackSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Retrieve the Minimum Wired Detection Interval
*
* @param    L7_ushort16 *val @b{(output)} location to store the Minimum Wired Detection Interval.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Minimum number of seconds that the AP waits before starting a new wired network detection cycle.
*           Zero indicates that Network detection is done on every RF-Scan cycle.
*
* @end
*********************************************************************/
L7_RC_t wdmWidsAPWiredDetectionIntervalGet(L7_ushort16 *val);

/*********************************************************************
*
* @purpose  Set the Minimum Wired Detection Interval.
*
* @param    L7_ushort16 val @b{(input)} new Minimum Wired Detection Interval in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Minimum number of seconds that the AP waits before starting a new wired network detection cycle.
*           Zero indicates that Network detection is done on every RF-Scan cycle.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsAPWiredDetectionIntervalSet(L7_ushort16 val);

/*********************************************************************
* @purpose  Retrieve the Rogue-Detected trap interval.
*
* @param    L7_ushort16 *val @b{(output)} location to store the Rogue-Detected trap interval.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The interval in seconds between transmissions of the trap telling the administrator that Rogues are present in the RF Scan database.
*           Zero indicates that this trap is never sent.
*
* @end
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectionTrapIntervalGet(L7_ushort16 *val);

/*********************************************************************
*
* @purpose  Set the Rogue-Detected trap interval.
*
* @param    L7_ushort16 val @b{(input)} new Rogue-Detected trap interval in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The interval in seconds between transmissions of the trap telling the administrator that Rogues are present in the RF Scan database.
*           Zero indicates that this trap is never sent.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsAPRogueDetectionTrapIntervalSet(L7_ushort16 val);

/*********************************************************************
*
* @purpose  Read and clear global flag(s) to determine if there are any
*           pending action. E.g., Cluster Controller election
*
* @param    L7_WDM_WS_FLAG_t  flags    @b((input))  type of pending action(s).
* @param    L7_WDM_WS_FLAG_t *setFlags @b((output)) flags set and cleared.
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
L7_RC_t wdmWSPendingFlagsGetClear(L7_WDM_WS_FLAG_t flags,
                                  L7_WDM_WS_FLAG_t *setFlags);

/*********************************************************************
*
* @purpose  Check if an entry exists in AP hardware capability table.
*
* @param    L7_uchar8 hwTypeID @b{(input)} AP Hardware Type ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPHWTypeIDGet(L7_uchar8 hwTypeID);

/*********************************************************************
*
* @purpose  Get next hardware type ID entry from AP hardware capability table.
*
* @param    L7_uchar8  hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8 *nextHWTypeID @b{(input)} Next AP Hardware Type ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPHWTypeIDNextGet(L7_uchar8 hwTypeID,L7_uchar8 *nextHWTypeID);

/*********************************************************************
*
* @purpose  Get hardware type description from AP hardware capability table.
*
* @param    L7_uchar8  hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_char8   *hwTypeDescription @b{(input)} AP Hardware Type Description
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPHWTypeDescriptionGet(L7_uchar8 hwTypeID,
                                  L7_char8 *hwTypeDescription);

/*********************************************************************
*
* @purpose  Get radio count from AP hardware capability table.
*
* @param    L7_uchar8  hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8 *radioCnt @b{(input)} Radio Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPHWTypeNumRadiosGet(L7_uchar8 hwTypeID,L7_uchar8 *radioCnt);

/*********************************************************************
*
* @purpose  Get image type ID from AP image type capability table.
*
* @param    L7_uchar8  hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8 *imageID @b{(input)} image ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPHWTypeImageIDGet(L7_uchar8 hwTypeID,L7_uint32 *imageID);

/*********************************************************************
*
* @purpose  Get dual boot support from AP hardware capability table.
*
* @param    L7_uchar8  hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_BOOL *dual @b{(input)} dual boot support
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPHWTypeDualBootSupportGet(L7_uchar8 hwTypeID,L7_BOOL *dual);

/*********************************************************************
*
* @purpose  Check if an entry exists in AP hardware capability table.
*
* @param    L7_uchar8 hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8 radioIndex @b{(input)} Radio Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPHWTypeRadioIndexGet(L7_uchar8 hwTypeID,L7_uchar8 radioIndex);

/*********************************************************************
*
* @purpose  Get next radio index entry from AP hardware capability table.
*
* @param    L7_uchar8 hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8 radioIndex @b{(input)} Radio Index
* @param    L7_uchar8 *nextRadioIndex @b{(input)}Next Radio Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPHWTypeRadioIndexNextGet(L7_uchar8 hwTypeID,
                                     L7_uchar8 radioIndex,
                                     L7_uchar8 *nextRadioIndex);

/*********************************************************************
*
* @purpose  Get radio type ID for this radio from AP radio hardware 
*           capability table.
*
* @param    L7_uchar8   hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8   radioIndex @b{(input)} Radio Index
* @param    L7_uchar8  *radioTypeID @b{(input)} Radio Type ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPHWTypeRadioIDGet(L7_uchar8 hwTypeID,
                              L7_uchar8 radioIndex,
                              L7_uchar8 *radioTypeID);

/*********************************************************************
*
* @purpose  Get radio type description for this from AP radio hardware 
*           capability table.
*
* @param    L7_uchar8   hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8   radioIndex @b{(input)} Radio Index
* @param    L7_uchar8  *radioDescription @b{(input)} Radio Type Description
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPHWTypeRadioDescriptionGet(L7_uchar8 hwTypeID,
                                       L7_uchar8 radioIndex,
                                       L7_char8 *radioDescription);

/*********************************************************************
*
* @purpose  Get VAP count for this radio from AP radio hardware capability table.
*
* @param    L7_uchar8   hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8   radioIndex @b{(input)} Radio Index
* @param    L7_uchar8  *radioDescription @b{(input)} Radio Type Description
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPHWTypeRadioVAPCountGet(L7_uchar8 hwTypeID,
                                    L7_uchar8 radioIndex,
                                    L7_uchar8 *vapCnt);

/*********************************************************************
*
* @purpose  Get A mode support for this radio from AP radio hardware 
*           capability table.
*
* @param    L7_uchar8   hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8   radioIndex @b{(input)} Radio Index
* @param    L7_BOOL    *aMode @b{(input)} A mode support 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPHWTypeRadio80211aSupportGet(L7_uchar8 hwTypeID,
                                         L7_uchar8 radioIndex,
                                         L7_BOOL *aMode);

/*********************************************************************
*
* @purpose  Get BG mode support for this radio from AP radio hardware 
*           capability table.
*
* @param    L7_uchar8   hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8   radioIndex @b{(input)} Radio Index
* @param    L7_BOOL    *bgMode @b{(input)} BG mode support 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPHWTypeRadio80211bgSupportGet(L7_uchar8 hwTypeID,
                                          L7_uchar8 radioIndex,
                                          L7_BOOL *bgMode);

/*********************************************************************
*
* @purpose  Get N mode support for this radio from AP radio hardware 
*           capability table.
*
* @param    L7_uchar8   hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8   radioIndex @b{(input)} Radio Index
* @param    L7_BOOL    *nMode@b{(input)} N mode support 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPHWTypeRadio80211nSupportGet(L7_uchar8 hwTypeID,
                                         L7_uchar8 radioIndex,
                                         L7_BOOL *nMode);

/*********************************************************************
*
* @purpose  Set the expected channel of the stand-alone AP in the Local 
*           AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uchar8 channel @b{(input)} Expected channel for the 
*           stand-alone AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPStandaloneExpectedChannelSet(L7_enetMacAddr_t macAddr, L7_uchar8 channel);

/*********************************************************************
*
* @purpose  Get the expected channel of the stand-alone AP in the Local 
*           AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uchar8 *channel @b{(output)} Expected channel for the 
*           stand-alone AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPStandaloneExpectedChannelGet(L7_enetMacAddr_t macAddr, L7_uchar8 *channel);

/*********************************************************************
*
* @purpose  Set the expected SSID of the stand-alone AP in the Local 
*           AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_char8 *ssid @b{(input)} Expected SSID string for the 
*           stand-alone AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPStandaloneExpectedSsidSet(L7_enetMacAddr_t macAddr, L7_char8 *ssid);

/*********************************************************************
*
* @purpose  Get the expected SSID of the stand-alone AP in the Local 
*           AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_char8 *ssid @b{(output)} Expected SSID string for the 
*           stand-alone AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPStandaloneExpectedSsidGet(L7_enetMacAddr_t macAddr, L7_char8 *ssid);

/*********************************************************************
*
* @purpose  Set the expected security mode of the stand-alone AP in the  
*           Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uchar8 mode @b{(input)} Expected security mode for the 
*           stand-alone AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPStandaloneExpectedSecurityModeSet(L7_enetMacAddr_t macAddr, L7_WDM_AP_STANDALONE_SECURITY_MODE_t mode);

/*********************************************************************
*
* @purpose  Get the expected security mode of the stand-alone AP in the  
*           Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uchar8 *mode @b{(output)} Expected security mode for the 
*           stand-alone AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPStandaloneExpectedSecurityModeGet(L7_enetMacAddr_t macAddr, L7_WDM_AP_STANDALONE_SECURITY_MODE_t *mode);

/*********************************************************************
*
* @purpose  Set the expected WDS mode of the stand-alone AP in the  
*           Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uchar8 mode @b{(input)} Expected WDS mode for the 
*           stand-alone AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPStandaloneExpectedWdsModeSet(L7_enetMacAddr_t macAddr, L7_WDM_AP_STANDALONE_WDS_MODE_t mode);

/*********************************************************************
*
* @purpose  Get the expected WDS mode of the stand-alone AP in the  
*           Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uchar8 *mode @b{(output)} Expected WDS mode for the 
*           stand-alone AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPStandaloneExpectedWdsModeGet(L7_enetMacAddr_t macAddr, L7_WDM_AP_STANDALONE_WDS_MODE_t *mode);

/*********************************************************************
*
* @purpose  Set the expected Wired Network mode of the stand-alone AP in the  
*           Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uchar8 mode @b{(input)} Expected Wired Network mode for the 
*           stand-alone AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPStandaloneExpectedWiredModeSet(L7_enetMacAddr_t macAddr, L7_WDM_AP_STANDALONE_WIRED_NETWORK_MODE_t mode);

/*********************************************************************
*
* @purpose  Get the expected Wired Network mode of the stand-alone AP in the  
*           Local AP Database
*
* @param    L7_enetMacAddr_t macAddr @b{(input)} AP MAC Address
* @param    L7_uchar8 *mode @b{(output)} Expected Wired Network mode for the 
*           stand-alone AP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPStandaloneExpectedWiredModeGet(L7_enetMacAddr_t macAddr, L7_WDM_AP_STANDALONE_WIRED_NETWORK_MODE_t *mode);

/*********************************************************************
*
* @purpose  Handle Peer Switch Failure
*
* @param    peerIPAddr @b{(input)} Peer Switch IP Address.
* @param    status  @b{(input)} status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsElectionStatusUpdate(L7_IP_ADDR_t peerswitch, L7_WDM_WIDS_ELECTION_STATUS_t status);

/*********************************************************************
*
* @purpose  To fetch the proposed new Cluster Controller IP address.
*
* @param    peerIPAddr @b{(output)} IP Address.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsProposedControllerGet(L7_IP_ADDR_t *ipAddr);


/*********************************************************************
*
* @purpose  Handle Cluster Controller change to self
*
* @param    L7_IP_ADDR_t newWidsCtrl @b{(input)} Newly Elected Cluster Controller
*
* @returns  void
*
* @comments none.
*
* @notes
*
* @end
*
*********************************************************************/
void wdmWIDSControllerToSelf (L7_IP_ADDR_t newWidsCtrl);

/*********************************************************************
*
* @purpose  Removing itself from being Cluster Controller to normal switch.
*
*
* @returns L7_FAILURE
* @returns L7_SUCCESS
*
* @comments none.
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsControllerRemoveSelf(void);

/*********************************************************************
*
* @purpose  Handle Cluster Controller change to a peer-switch while
*           setting up the AP database to sending new controller
*
* @param    L7_IP_ADDR_t newWidsCtrl @b{(input)} Newly Elected Cluster Controller
*
* @returns L7_FAILURE
*          L7_SUCCESS
*
* @comments none.
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t wdmWIDSControllerFromSelf (L7_IP_ADDR_t newWidsCtrl);

/*********************************************************************
*
* @purpose  Get the number of APs to which the wireless system is currently 
*           sending de-authentication messages to mitigate against rogue APs
*
* @param     L7_uint32 *mitigationCount @b{(input)} WIDS Rogue AP Mitigation Count.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsRogueAPMitigationCountGet(L7_uint32 *mitigationCount);

/*********************************************************************
*
* @purpose  Get the maximum number of APs for which the system can 
*           send de-authentication frames.
*
* @param     L7_uint32 *mitigationLimit @b{(input)} maximum number of APs.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsRogueAPMitigationLimitGet(L7_uint32 *mitigationLimit);

/*********************************************************************
*
* @purpose  Clear out all rogue AP states in the RF Scan database. 
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmRogueAPStateClearAll(void);

/*********************************************************************
*
* @purpose  Clear out the rogue AP state for the specified AP. 
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmRogueAPStateClear(L7_enetMacAddr_t macAddr);


/*********************************************************************
*
* @purpose  Get the total number of managed APs in the database
*           for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint32  *value  @b{(output)} pointer to the value.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    Caller should ensure that this function is invoked for peer switch 
*           AP status only when WS acts Cluster Controller.
*       
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchTotalAPsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the number of APs that are connected to the switch, 
*           but are not completely configured.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint32  *value  @b{(output)} pointer to the value.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    Caller should ensure that this function is invoked for peer switch 
*           AP status only when WS acts Cluster Controller.
*       
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchDiscoveredAPsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the number of APs that are authenticated, configured, 
*           and have an active connection with the speicified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint32  *value  @b{(output)} pointer to the value.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    Caller should ensure that this function is invoked for peer switch 
*           AP status only when WS acts Cluster Controller.
*       
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchManagedAPsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the number of APs that were previously 
*           authenticated and managed, but currently do not 
*           have connection with the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint32  *value  @b{(output)} pointer to the value.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    Caller should ensure that this function is invoked for peer switch 
*           AP status only when WS acts Cluster Controller.
*       
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchConnFailedAPsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the max. number of APs that are managed 
*           by the speicified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint32  *value  @b{(output)} pointer to the value.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    Caller should ensure that this function is invoked for peer switch 
*           AP status only when WS acts Cluster Controller.
*       
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchMaxManagedAPsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the total number of clients in the database 
*           for the speicifed switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint32  *value  @b{(output)} pointer to the value.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    Caller should ensure that this function is invoked for peer switch 
*           client status only when WS acts Cluster Controller.
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchTotalClientsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the number of authentictaed clients in the database
*           specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint32  *value  @b{(output)} pointer to the value.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    Caller should ensure that this function is invoked for peer switch 
*           client status only when WS acts Cluster Controller.
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchAuthClientsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *value);

/*********************************************************************
* @purpose  Get AP image download mode supported by the specified switch
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} switch IP address
* @param    L7_uchar8     mode    @b{(output)} AP image download mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t wdmSwitchAPImageDnloadModeGet(L7_IP_ADDR_t ipAddr, L7_uchar8 *mode);

/*********************************************************************
*
* @purpose Get the WLAN utilization status of the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_ushort16 *value @b{(output)} pointer to store the percentage 
* @param                                    of WLAN unitilization.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    Caller should ensure that this function is invoked for peer switch 
*           status only when WS acts Cluster Controller.
*
*       
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchWLANUtilizationGet(L7_IP_ADDR_t IPAddr, L7_ushort16 *value);

/*********************************************************************
*
* @purpose Get the WLAN utilization status of the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_ushort16 value @b{(input)} percentage of WLAN unitilization.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    Caller should ensure that this function is invoked for peer switch 
*           status only when WS acts Cluster Controller.
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchWLANUtilizationSet(L7_IP_ADDR_t IPAddr, L7_ushort16 value);

/*********************************************************************
*
* @purpose Get the TSPEC total voice TS status for the specified switch.
*
* @param    L7_IP_ADDR_t IPAddr    @b{(input)} IP address of the switch
* @param    L7_uint32    *val      @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @retruns  L7_FAILURE
*
* @comments Caller should ensure that this function is invoked for peer switch
*           client status only when WS acts Cluster Controller.
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchTspecTotalVoiceTsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *val);

/*********************************************************************
*
* @purpose Get the TSPEC total video TS status for the specified switch.
*
* @param    L7_IP_ADDR_t IPAddr    @b{(input)} IP address of the switch
* @param    L7_uint32    *val      @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @retruns  L7_FAILURE
*
* @comments Caller should ensure that this function is invoked for peer switch
*           client status only when WS acts Cluster Controller.
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchTspecTotalVideoTsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *val);

/*********************************************************************
*
* @purpose Get the TSPEC total TS clients status for the specified switch.
*
* @param    L7_IP_ADDR_t IPAddr    @b{(input)} IP address of the switch
* @param    L7_uint32    *val      @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @retruns  L7_FAILURE
*
* @comments Caller should ensure that this function is invoked for peer switch
*           client status only when WS acts Cluster Controller.
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchTspecTotalTsClientsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *val);

/*********************************************************************
*
* @purpose Get the TSPEC total TS roaming clients status for the specified switch.
*
* @param    L7_IP_ADDR_t IPAddr    @b{(input)} IP address of the switch
* @param    L7_uint32    *val      @b{(output)} pointer to value output location
*
* @returns  L7_SUCCESS
* @retruns  L7_FAILURE
*
* @comments Caller should ensure that this function is invoked for peer switch
*           client status only when WS acts Cluster Controller.
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchTspecTotalTsRoamClientsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the received WLAN bytes value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 *bytes @b{(output)} pointer to store WLAN received
*                                        bytes value.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to get the peer switch client status.
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchWLANBytesRxGet(L7_IP_ADDR_t IPAddr, L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the received WLAN bytes value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 bytes @b{(input)} pointer to store WLAN received
*                                        bytes value.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to get the peer switch client status.
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchWLANBytesRxSet(L7_IP_ADDR_t IPAddr, L7_uint64 bytes);

/*********************************************************************
*
* @purpose  Get the transmitted WLAN bytes value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 *bytes @b{(output)} pointer to store WLAN transmitted 
*                                        bytes value.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to get the peer switch client status.
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchWLANBytesTxGet(L7_IP_ADDR_t IPAddr, L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the transmitted WLAN bytes value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 bytes @b{(input)} pointer to store WLAN transmitted 
*                                        bytes value.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to get the peer switch client status.
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchWLANBytesTxSet(L7_IP_ADDR_t IPAddr, L7_uint64 bytes);

/*********************************************************************
*
* @purpose  Get the received WLAN packets value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 *bytes @b{(output)} pointer to store WLAN received
*                                        packets value.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to get the peer switch client status.
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchWLANPktsRxGet(L7_IP_ADDR_t IPAddr, L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the received WLAN pkts value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 bytes @b{(input)} WLAN received pkts value.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to get the peer switch client status.
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchWLANPktsRxSet(L7_IP_ADDR_t IPAddr, L7_uint64 pkts);

/*********************************************************************
*
* @purpose  Get the transmitted WLAN pkts value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 *pkts @b{(output)} pointer to store WLAN transmitted 
*                                        pkts value.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to get the peer switch client status.
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchWLANPktsTxGet(L7_IP_ADDR_t IPAddr, L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the transmitted WLAN pkts value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 pkts @b{(input)} WLAN transmitted pkts value.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to get the peer switch client status.
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchWLANPktsTxSet(L7_IP_ADDR_t IPAddr, L7_uint64 pkts);

/*********************************************************************
*
* @purpose  Get the dropped receive WLAN bytes value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 *bytes @b{(input)} pointer to store WLAN dropped receive
*                                         bytes value.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to get the peer switch client status.
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchWLANDropBytesRxGet(L7_IP_ADDR_t IPAddr, L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the dropped receive WLAN bytes value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 bytes @b{(input)} WLAN dropped receive bytes value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to get the peer switch client status.
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchWLANDropBytesRxSet(L7_IP_ADDR_t IPAddr, L7_uint64 bytes);

/*********************************************************************
*
* @purpose  Get the dropped transmit WLAN bytes value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 *bytes @b{(output)} pointer to store WLAN dropped transmit
*                                          bytes value.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to get the peer switch client status.
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchWLANDropBytesTxGet(L7_IP_ADDR_t IPAddr, L7_uint64 *bytes);

/*********************************************************************
*
* @purpose  Get the dropped transmit WLAN bytes value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 bytes @b{(input)} WLAN dropped transmit bytes value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to get the peer switch client status.
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchWLANDropBytesTxSet(L7_IP_ADDR_t IPAddr, L7_uint64 bytes);

/*********************************************************************
*
* @purpose  Get the dropped receive WLAN packets value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 *bytes @b{(output)} pointer to store WLAN dropped receive
*                                          packets value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to get the peer switch client status.
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchWLANDropPktsRxGet(L7_IP_ADDR_t IPAddr, L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the dropped receive WLAN pkts value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 pkts @b{(input)} WLAN dropped receive pkts value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to get the peer switch client status.
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchWLANDropPktsRxSet(L7_IP_ADDR_t IPAddr, L7_uint64 pkts);

/*********************************************************************
*
* @purpose  Get the drop transmit WLAN pkts value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 *pkts @b{(output)} pointer to store WLAN dropped transmit
*                                         pkts value.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to get the peer switch client status.
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchWLANDropPktsTxGet(L7_IP_ADDR_t IPAddr, L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the dropped transmit WLAN pkts value for the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 pkts @b{(input)} WLAN dropped transmit pkts value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to get the peer switch client status.
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchWLANDropPktsTxSet(L7_IP_ADDR_t IPAddr, L7_uint64 pkts);

/*********************************************************************
*
* @purpose  Set the WS WLAN TSPEC statistics for the specified switch.
*
* @param    L7_IP_ADDR_t addr        @b{(input)} IP address of the switch.
* @param    wdmWlanTSGlobalStats_t   @b{(input)} TSPEC statistics data.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to set the peer switch client statistics.
*
* @end
*
*********************************************************************/
L7_RC_t wdmPeerSwitchTspecStatsSet(L7_IP_ADDR_t IPAddr, wdmWlanTSGlobalStats_t *pData);

/*********************************************************************
*
* @purpose  Get the receive TSPEC WLAN pkts value for the specified switch.
*
* @param    L7_IP_ADDR_t addr    @b{(input)} IP address of the switch.
* @param    L7_uint32    acindex @b{(input)} AC index (voice, video)
* @param    L7_uint64    *value  @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchTspecPktsRxGet(L7_IP_ADDR_t IPAddr, L7_uint32 acindex, L7_uint64 *value);

/*********************************************************************
*
* @purpose  Get the transmit TSPEC WLAN pkts value for the specified switch.
*
* @param    L7_IP_ADDR_t addr    @b{(input)} IP address of the switch.
* @param    L7_uint32    acindex @b{(input)} AC index (voice, video)
* @param    L7_uint64    *value  @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchTspecPktsTxGet(L7_IP_ADDR_t IPAddr, L7_uint32 acindex, L7_uint64 *value);

/*********************************************************************
*
* @purpose  Get the receive TSPEC WLAN bytes value for the specified switch.
*
* @param    L7_IP_ADDR_t addr    @b{(input)} IP address of the switch.
* @param    L7_uint32    acindex @b{(input)} AC index (voice, video)
* @param    L7_uint64    *value  @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchTspecBytesRxGet(L7_IP_ADDR_t IPAddr, L7_uint32 acindex, L7_uint64 *value);

/*********************************************************************
*
* @purpose  Get the transmit TSPEC WLAN bytes value for the specified switch.
*
* @param    L7_IP_ADDR_t addr    @b{(input)} IP address of the switch.
* @param    L7_uint32    acindex @b{(input)} AC index (voice, video)
* @param    L7_uint64    *value  @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchTspecBytesTxGet(L7_IP_ADDR_t IPAddr, L7_uint32 acindex, L7_uint64 *value);

/*********************************************************************
*
* @purpose  Get the WLAN accepted TSPECs for the specified switch.
*
* @param    L7_IP_ADDR_t addr    @b{(input)} IP address of the switch.
* @param    L7_uint32    acindex @b{(input)} AC index (voice, video)
* @param    L7_uint32    *value  @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchTspecTsAcceptedGet(L7_IP_ADDR_t IPAddr, L7_uint32 acindex, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the WLAN rejected TSPECs for the specified switch.
*
* @param    L7_IP_ADDR_t addr    @b{(input)} IP address of the switch.
* @param    L7_uint32    acindex @b{(input)} AC index (voice, video)
* @param    L7_uint32    *value  @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchTspecTsRejectedGet(L7_IP_ADDR_t IPAddr, L7_uint32 acindex, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the WLAN accepted roaming TSPECs for the specified switch.
*
* @param    L7_IP_ADDR_t addr    @b{(input)} IP address of the switch.
* @param    L7_uint32    acindex @b{(input)} AC index (voice, video)
* @param    L7_uint32    *value  @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchTspecRoamTsAcceptedGet(L7_IP_ADDR_t IPAddr, L7_uint32 acindex, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the WLAN rejected roaming TSPECs for the specified switch.
*
* @param    L7_IP_ADDR_t addr    @b{(input)} IP address of the switch.
* @param    L7_uint32    acindex @b{(input)} AC index (voice, video)
* @param    L7_uint32    *value  @b{(output)} ptr to stored value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchTspecRoamTsRejectedGet(L7_IP_ADDR_t IPAddr, L7_uint32 acindex, L7_uint32 *value);

/*********************************************************************
* @purpose  Gets the next managed switch IP address which may be of WS
*           itself of or that of peer switch.
*
* @param    ipAddr       @b{(input)} IP address of the switch
* @param    nextIpAddr   @b{(output)} Next Managed switch IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Next higher ipAddr can be either of WS itself or may be searched 
*           in peer switch entries.  To acquire the next higher IPAddr from 
*           peer group, WS should be acting as Cluster Controller.
*           if ip is passed with 0, least ipAddr is returned.
*
*
* @end
*********************************************************************/
L7_RC_t wdmSwitchEntryNextGet(L7_IP_ADDR_t  ip,
                                  L7_IP_ADDR_t *nextIp);


/*********************************************************************
* @purpose  Verifies whether an entry is present or not.
*
* @param    L7_IP_ADDR_t ip   @b{(input)} IP address of peer switch
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE    If entry doesn't exist
*
* @comments IPAddr being verified may be of WS itself or that may be searched 
*           in peer switch entries. To check the IPAddr from the peer group, 
*           WS should be acting as Cluster Controller.
*
* @end
*********************************************************************/
L7_RC_t wdmSwitchEntryGet(L7_IP_ADDR_t  IPAddr);

/*********************************************************************
* @purpose  Returns the Cluster Priority for a given switch
*
* @param    IPAddr          @b{(input)} IP address of the switch
* @param    prio            @b{(output)} Cluster Priority
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmSwitchWidsPriorityGet(L7_IP_ADDR_t IPAddr,
                            L7_uchar8 *prio);

/*********************************************************************
* @purpose  Returns the de-authentication attack status for the requested index
*
* @param    index           @b{(input)} index of the entry (1-WDM_AP_DE_AUTHENTICATION_ATTACK_MAX)
* @param   *entry           @b{(output)}de-authentication attack status entry
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmWidsDeAuthenticationAttackStatusGet(L7_uint32 index, wdmWidsDeAuthenticationAttackStatus_t *entry);

/*********************************************************************
* @purpose  Returns the de-authentication attack status for the next index
*
* @param   *index           @b{(input)} index of the previous entry 
* @param   *entry           @b{(output)}de-authentication attack status entry
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
*********************************************************************/
L7_RC_t wdmWidsDeAuthenticationAttackStatusNextGet(L7_uint32 *index, wdmWidsDeAuthenticationAttackStatus_t *entry);

/*********************************************************************
*
* @purpose  Set the global auth radius server name.
*
* @param    L7_char8 *name @b{(input)} auth radius server name, L7_WDM_RADIUS_SERVER_NAME_MAX
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAuthRadiusServerNameSet(L7_char8 *name);

/*********************************************************************
*
* @purpose  Get the global auth radius server name.
*
* @param    L7_char8 *name @b{(output)} auth radius server name, L7_WDM_RADIUS_SERVER_NAME_MAX
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAuthRadiusServerNameGet(L7_char8 *name);

/*********************************************************************
*
* @purpose  Set the global acct radius server name.
*
* @param    L7_char8 *name @b{(input)} acct radius server name, L7_WDM_RADIUS_SERVER_NAME_MAX
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAcctRadiusServerNameSet(L7_char8 *name);

/*********************************************************************
*
* @purpose  Get the global acct radius server name.
*
* @param    L7_char8 *name @b{(output)} auth radius server name, L7_WDM_RADIUS_SERVER_NAME_MAX
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAcctRadiusServerNameGet(L7_char8 *name);

/*********************************************************************
*
* @purpose  Set the accounting mode for wireless clients.
*
* @param    L7_uint32 mode @b{(input)} Accounting mode for wireless clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAcctModeSet(L7_uint32 mode);

/*********************************************************************
*
* @purpose  Get the accounting mode for wireless clients.
*
* @param    L7_uint32 *mode @b{(output)} Accounting mode for wireless clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAcctModeGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Set the MAC authentication mode.
*
* @param    L7_uint32 mode @b{(input)} MAC authentication mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmMACAuthModeSet(L7_uint32 mode);

/*********************************************************************
*
* @purpose  Get the MAC authentication mode.
*
* @param    L7_uint32 *mode @b{(output)} MAC authentication mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmMACAuthModeGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose Get the global authentication server configuration status.
*
* @param    L7_uint32   *value       @b{(output)} pointer to store the status.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAuthRadiusConfigurationGet(L7_uint32 *value);

/*********************************************************************
*
* @purpose Set the global authentication server configuration status.
*
* @param    L7_uint32    value        @b{(input)} configured status.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAuthRadiusConfigurationSet(L7_uint32  value);

/*********************************************************************
*
* @purpose Get the global accounting server configuration status.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the status.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAcctRadiusConfigurationGet(L7_uint32 *value);

/*********************************************************************
*
* @purpose Set the global accounting server configuration status.
*
* @param    L7_uint32    value      @b{(input)} configured status.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAcctRadiusConfigurationSet(L7_uint32  value);

/*********************************************************************
*
* @purpose  Checks if indexed VAP entry exists for the specified hardware 
*           type.
*
* @param    L7_uchar8 hwTypeID @b{(input)} AP Hardware Type ID
* @param    L7_uchar8 radioIndex @b{(input)} Radio Index
* @param    L7_uchar8 vapIndex @b{(input)} VAP Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPHWTypeVAPIndexGet(L7_uchar8 hwTypeID,L7_uchar8 radioIndex, 
                     L7_uchar8 vapIndex);

/*********************************************************************
*
* @purpose  Get the number of dist tunnel clients in the database.
*
* @param    L7_ushort16  value  @b{(output)} pointer to the value
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSDistTunnelClientsGet(L7_uint32 *value);
/*********************************************************************
*
* @purpose Get the global L2 Dist Tunneling idle timeout value.
*
* @param    L7_uint32   *value       @b{(output)} pointer to store
*                                                 the vlaue.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelIdleTimeoutGet( L7_uint32 *value);
/*********************************************************************
*
* @purpose Set the global L2 Dist Tunneling idle timeout value.
*
* @param    L7_uint32   value @b{(input)} value to be configured.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelIdleTimeoutSet( L7_uint32 value);
/*********************************************************************
*
* @purpose Get the global L2 Dist Tunneling max timeout value.
*
* @param    L7_uint32   *value       @b{(output)} pointer to store
*                                                 the vlaue.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelMaxTimeoutGet( L7_uint32 *value);
/*********************************************************************
*
* @purpose Set the global L2 Dist Tunneling max timeout value.
*
* @param    L7_uint32   value @b{(input)} value to be configured.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelMaxTimeoutSet( L7_uint32 value);
/*********************************************************************
*
* @purpose Get the global L2 Dist Tunneling max clients value.
*
* @param    L7_uint32   *value       @b{(output)} pointer to store
*                                                 the vlaue.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelMaxClientsGet( L7_uint32 *value);
/*********************************************************************
*
* @purpose Set the global L2 Dist Tunneling max clients value.
*
* @param    L7_uint32   value @b{(input)} value to be configured.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelMaxClientsSet( L7_uint32 value);
/*********************************************************************
*
* @purpose Get the global L2 Dist Tunneling max mcast replications
*          value.
*
* @param    L7_uint32   *value       @b{(output)} pointer to store
*                                                 the vlaue.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelMcastReplGet( L7_uint32 *value);
/*********************************************************************
*
* @purpose Set the global L2 Dist Tunneling max mcast replications
*          value.
*
* @param    L7_uint32   value @b{(input)} value to be configured.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDistTunnelMcastReplSet( L7_uint32 value);
/*********************************************************************
*
* @purpose  set the Total number of packets sent by all APs via
*           distributed tunnels for the wireless switch (WS).
*
* @param    L7_uint64 pkts @b{(input)} total no of packets 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*           
*********************************************************************/
L7_RC_t wdmWLANDistTunnelWlanPktsTxSet(L7_uint64 pkts);
/*********************************************************************
*
* @purpose   set the Total number of clients that successfully roamed
*            away from Home AP using distributed tunneling for 
*            wireless switch(WS).
*
* @param    L7_uint32 clients @b{(input)} clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*           
*********************************************************************/
L7_RC_t wdmWLANDistTunnelRoamedClientSet(L7_uint64 clients);
/*********************************************************************
*
* @purpose   set total number of clients for which the system was
*            unable to set up a distributed tunnel when client roamed.
*
* @param    L7_uint32 clients @b{(input)}clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*           
*********************************************************************/
L7_RC_t wdmWLANDistTunnelDeniedClientsSet(L7_uint64 clients);
/*********************************************************************
*
* @purpose  Get the Total number of packets sent by all APs via
*            distributed tunnels for wireless switch(WS).
*
* @param    L7_uint64 *pkts @b{(output)} pointer to store packet value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*           
*********************************************************************/
L7_RC_t wdmWLANDistTunnelWlanPktsTxGet(L7_uint64 *pkts);
/*********************************************************************
*
* @purpose   Get the Total number of clients that successfully roamed
*            away from Home AP using distributed tunneling for the
*            switch.
*
* @param    L7_uint32 *clients @b{(output)} pointer to store clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*           
*********************************************************************/
L7_RC_t wdmWLANDistTunnelRoamedClientGet(L7_uint32 *clients);
/*********************************************************************
*
* @purpose   Get Total number of clients for which the system was
*            unable to set up a distributed tunnel when client roamed.
*
* @param    L7_uint32 *clients @b{(output)} pointer to store clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*           
*********************************************************************/
L7_RC_t wdmWLANDistTunnelDeniedClientsGet(L7_uint32 *clients);

/*********************************************************************
*
* @purpose  Get the Total number of packets sent by all APs via 
*            distributed tunnels the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 *pkts @b{(output)} pointer to store packet value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to get the peer switch client status
*********************************************************************/
L7_RC_t wdmSwitchWLANDistTunnelWlanPktsTxGet(L7_IP_ADDR_t IPAddr, L7_uint64 *pkts);

/*********************************************************************
*
* @purpose  Get the number of dist tunnel clients in the database
*           specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint32  *value  @b{(output)} pointer to the value.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @notes    Caller should ensure that this function is invoked for peer switch
*           client status only when WS acts Cluster Controller.
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchDistTunnelClientsGet(L7_IP_ADDR_t IPAddr, L7_uint32 *value);
/*********************************************************************
*
* @purpose   Get the Total number of clients that successfully roamed 
*            away from Home AP using distributed tunneling for the 
*            specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint32 *clients @b{(output)} pointer to store clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to get the peer switch client status
*********************************************************************/
L7_RC_t wdmSwitchWLANDistTunnelRoamedClientGet(L7_IP_ADDR_t IPAddr, 
                                               L7_uint32 *clients);
/*********************************************************************
*
* @purpose   Get Total number of clients for which the system was 
*            unable to set up a distributed tunnel when client roamed.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint32 *clients @b{(output)} pointer to store clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to get the peer switch client status
*********************************************************************/
L7_RC_t wdmSwitchWLANDistTunnelDeniedClientsGet(L7_IP_ADDR_t IPAddr,
                                                L7_uint32 *clients);
/*********************************************************************
*
* @purpose   Set the Total number of packets sent by all APs via
*            distributed tunnels the specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint64 *pkts @b{(output)} pointer to store packet value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to Set the peer switch client status
*********************************************************************/

L7_RC_t wdmSwitchWLANDistTunnelWlanPktsTxSet(L7_IP_ADDR_t IPAddr, L7_uint64 pkts);

/*********************************************************************
*
* @purpose   Set the Total number of clients that successfully roamed
*            away from Home AP using distributed tunneling for the
*            specified switch.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint32 *bytes @b{(output)} pointer to store clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to Set the peer switch client status
*********************************************************************/

L7_RC_t wdmSwitchWLANDistTunnelRoamedClientSet(L7_IP_ADDR_t IPAddr, 
                                               L7_uint32 clients);

/*********************************************************************
*
* @purpose   Set Total number of clients for which the system was
*            unable to set up a distributed tunnel when client roamed.
*
* @param    L7_IP_ADDR_t addr @b{(input)} IP address of the switch.
* @param    L7_uint32 *clients @b{(output)} pointer to store clients
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that only Cluster Controller calls this
*           function to set the peer switch client status
*********************************************************************/
L7_RC_t wdmSwitchWLANDistTunnelDeniedClientsSet(L7_IP_ADDR_t IPAddr, 
                                                L7_uint32 clients);


/*********************************************************************
* @purpose  Determine the default PHY Mode for a given profile 
*           and radio
*
* @param    L7_uint32 profileId @b{(input)} AP Profile ID
*           L7_uint32 radioIndex @b{(input)} Radio Index
*
* returns   L7_WDM_RADIO_PHY_MODE_t
*
* @notes    L7_WDM_RADIO_PHY_MODE_NONE returned on error
*
* @end
***********************************************************************/

L7_WDM_RADIO_PHY_MODE_t
wdmAPProfileRadioDefaultModeGet(L7_uint32 profileId,
                                    L7_uint32 radioIndex);

/*********************************************************************
*
* @purpose function to Get th MAC address used by wireless system 
*
* @param    mac    @b{(output)} MAC address used by wireless system. 
*
* @returns L7_SUCCESS always.
*
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSMacAddressGet(L7_uchar8 *mac);

/*********************************************************************
*
* @purpose  Set the auto IP address assignment mode for the Wireless Switch (WS).
*
* @param    L7_uint32 mode @b{(input)} new mode value, L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSAutoIPAssignModeSet(L7_uint32 mode);

/*********************************************************************
*
* @purpose  Get the auto IP address assignment mode for the Wireless Switch (WS).
*
* @param    L7_uint32 *mode @b{(input)} pointer to return mode value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSAutoIPAssignModeGet(L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Set static IP address to wireless switch.
*
* @param    L7_IP_ADDR_t ipAddr @b{(input)} IP address of the switch.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller should ensure that assigned IP address is same as network 
*           interface on a switching package or IP address of active routing 
*           interface if routing is enabled.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSSwitchStaticIPAddrSet(L7_IP_ADDR_t ipAddr );

/*********************************************************************
*
* @purpose  Get static IP address assigned to wireless switch.
*
* @param    L7_IP_ADDR_t *IPAddr @b{(input)} IP address of the switch.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSSwitchStaticIPAddrGet(L7_IP_ADDR_t *IPAddr);

/*********************************************************************
*
* @purpose  Initiate command to regenerate certificate and key for the Wireless Switch (WS).
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
L7_RC_t wdmWSCertificateRegenerateStart(void);

/*********************************************************************
*
* @purpose  Get the flag to regenerate certificate and key for the Wireless Switch (WS).
*
* @param    L7_BOOL certRegenerate @b{(output)} flag to indicate to regenerate certificate and keys, L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSCertificateRegenerateFlagGet(L7_BOOL *certRegenerate);


/*********************************************************************
*
* @purpose  Set the status to regenerate certificate and key for the Wireless Switch (WS).
*
* @param    L7_WDM_CERTIFICATE_GEN_STATUS_t status @b{(input)} Regenerate certificate status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSCertificateRegenerateStatusSet(L7_WDM_CERTIFICATE_GEN_STATUS_t status);

/*********************************************************************
*
* @purpose  Get the status to regenerate certificate and key for the Wireless Switch (WS).
*
* @param    L7_uint32 status @b{(output)} status to indicate to regenerate certificate and keys, L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSCertificateRegenerateStatusGet(L7_uint32 *status);

/*********************************************************************
*
* @purpose  Set the Switch Provisioning mode
*
* @param    L7_BOOL  mode  @b{(input)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchProvisioningModeSet (L7_BOOL mode);

/*********************************************************************
*
* @purpose  Get the Switch Provisioning mode
*
* @param    L7_BOOL  *mode  @b{(output)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchProvisioningModeGet(L7_BOOL *mode);

/*********************************************************************
*
* @purpose  Get the exchange certificates mode
*
* @param    L7_BOOL  *mode  @b{(output)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchExchangeCertificatesModeGet(L7_BOOL *mode);

/*********************************************************************
*
* @purpose  Set the exchange certificates mode
*
* @param    L7_BOOL  mode  @b{(input)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmSwitchExchangeCertificatesModeSet(L7_BOOL mode);

/*********************************************************************
*
* @purpose  Set the Network Mutual Authentication mode.
*
* @param    L7_BOOL  mode  @b{(input)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmNetworkMutualAuthModeSet (L7_BOOL mode);

/*********************************************************************
*
* @purpose  Update the Network Mutual Authentication mode.
*
* @param    L7_BOOL  mode  @b{(input)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This update function is invoked only on non-cluster controller
*
* @end
*
*********************************************************************/
L7_RC_t wdmNetworkMutualAuthModeUpdate (L7_BOOL mode);

/*********************************************************************
*
* @purpose  Get the Network Mutual Authentication mode.
*
* @param    L7_BOOL  *mode  @b{(output)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmNetworkMutualAuthModeGet(L7_BOOL *mode);

/*********************************************************************
*
* @purpose  Set the Network Mutual Authentication status.
*
* @param    L7_WDM_NW_MUTUAL_AUTH_STATUS_t  status  @b{(input)} Network mutual auth status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmNetworkMutualAuthStatusSet (L7_WDM_NW_MUTUAL_AUTH_STATUS_t status);

/*********************************************************************
*
* @purpose  Get the Network Mutual Authentication status.
*
* @param    L7_uint32  *status  @b{(output)} store network mutual auth status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmNetworkMutualAuthStatusGet(L7_uint32 *status);


/*********************************************************************
*
* @purpose  Set the Network Mutual Authentication enable/disable time stamp.
*
* @param    timeStamp  @b{(input)} Network mutual auth enable/disable time stamp
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmNetworkMutualAuthStartTimeSet (L7_uint32 timeStamp);


/*********************************************************************
*
* @purpose  Get the Network Mutual Authentication enable/disable time stamp.
*
* @param    timeStamp  @b{(output)} Pointer to network mutual auth enable/disable time stamp
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmNetworkMutualAuthStartTimeGet (L7_uint32 *timeStamp);

/*********************************************************************
*
* @purpose  Set the AP Reprovisioning mode.
*
* @param    L7_BOOL  mode  @b{(input)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPReprovisioningModeSet (L7_BOOL mode);

/*********************************************************************
*
* @purpose  Get the AP Reprovisioning mode.
*
* @param    L7_BOOL  *mode  @b{(output)} L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPReprovisioningModeGet(L7_BOOL *mode);

/*********************************************************************
*
* @purpose  Set the AP Provisioning Age Time
*
* @param    L7_uint32 ageTime  @b{(input)} Age time value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningAgeTimeSet (L7_ushort16 agetime);

/*********************************************************************
*
* @purpose  Get the AP Provisioning Age Time
*
* @param    L7_uint32 *ageTime  @b{(output)} pointer to Age time value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmAPProvisioningAgeTimeGet (L7_ushort16 *agetime);

/*********************************************************************
*
* @purpose  Start exchange of X.509 certificates
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
L7_RC_t wdmNetworkExchangeCertificate ();

/*********************************************************************
*
* @purpose  Get X.509 certificate of the local switch from wssl_cert.pem file
*
* @param    L7_uchar8 *x509CertBuf  @b{(output)} pointer to X.509 certificate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSX509CertGet(L7_uchar8  *x509CertBuf);

/*********************************************************************
*
* @purpose  Write AP/Switch X.509 certificate to wlan.pem file 
*
* @param    L7_uchar8 *x509CertBuf  @b{(output)} pointer to X.509 certificate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSPeerX509CertUpdate(L7_uchar8  *x509CertBuf);

/*********************************************************************
*
* @purpose  Set IP address of the switch from which this switch 
*           requests a X.509 certificate
*
* @param    certTargetIPAddr    @b{(input)} IP address of the switch
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSX509CertificateReqTargetSet(L7_IP_ADDR_t certTargetIPAddr);

/*********************************************************************
*
* @purpose  Get IP address of the switch from which this switch 
*           requested a X.509 certificate
*
* @param    certTargetIPAddr   @b{(output)} ptr to IP address of the switch 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSX509CertificateReqTargetGet(L7_IP_ADDR_t *certTargetIPAddr);

/*********************************************************************
*
* @purpose  Initiate X.509 certificate request
*
* @param    none
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSX509CertificateReqCommand(void);

/*********************************************************************
*
* @purpose  Start switch provisioning 
*
* @param    none
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSProvisioningStart(void);

/*********************************************************************
*
* @purpose  Get the X.509 certificate request status
*
* @param    certReqStatus    @b{(output)} ptr to certificate status
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSX509CertificateReqStatusGet(
                L7_WDM_WS_X509_CERTIFICATE_REQ_STATUS_t *certReqStatus);

/*********************************************************************
*
* @purpose  Set the X.509 certificate request status
*
* @param    certReqStatus   @b{(input)} certificate status
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSX509CertificateReqStatusSet(
                L7_WDM_WS_X509_CERTIFICATE_REQ_STATUS_t certReqStatus);

/*********************************************************************
*
* @purpose  Get switch provisioning status
*
* @param    status   @b{(output)} ptr to provisioning status
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSProvisioningStatusGet(L7_WDM_WS_PROVISIONING_STATUS_t *status);

/*********************************************************************
*
* @purpose  Set switch provisioning status
*
* @param    status    @b{(input)} provisioning status
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSProvisioningStatusSet(L7_WDM_WS_PROVISIONING_STATUS_t status);

/*********************************************************************
*
* @purpose  Set the IP address of the switch to obtain provisioning information
*
* @param    switchIPAddr @b{(input)} IP address of switch
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSProvisioningSwitchIPAddrSet(L7_IP_ADDR_t switchIPAddr);

/*********************************************************************
*
* @purpose  Get the IP address of the switch from which provisioning 
*           information is obtained
*
* @param    switchIPAddr   @b{(output)} ptr to IP address of switch
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSProvisioningSwitchIPAddrGet(L7_IP_ADDR_t *switchIPAddr);

/*********************************************************************
*
* @purpose  Get the reqCommand Flag and Clear the flag
*
* @param    reqCommand  @b{(output)} ptr to reqCommand flag
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSX509CertificateReqCommandGetClear(L7_BOOL *reqCommand);

/*********************************************************************
*
* @purpose  Get the startProvisioning Flag and Clear the flag
*
* @param    provCommand  @b{(output)} ptr to provCommand flag
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSProvisioningCommandGetClear(L7_BOOL *provCommand);

/*********************************************************************
*
* @purpose  Get cluster switch provisioning status
*
* @param    status   @b{(output)} ptr to provisioning status of cluster
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSProvisioningClusterStatusGet(L7_WDM_WS_CLUSTER_PROV_STATUS_t *status);

/*********************************************************************
*
* @purpose  Set the cluster switch provisioning status info.
*
* @param    clusterStatus   @b{(input)} status of cluster switch provisioning
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSProvisioningClusterStatusSet(
                L7_WDM_WS_CLUSTER_PROV_STATUS_t clusterStatus);

/*********************************************************************
*
* @purpose  Get the IP address of the switch which requested provisioning 
*           information 
*
* @param    newIPAddr   @b{(output)} ptr to IP address of switch
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSProvisioningNewSwitchIPAddrGet(L7_IP_ADDR_t *newIPAddr);

/*********************************************************************
*
* @purpose  Set the IP address of the switch which requested provisioning 
*           information
*
* @param    newIPAddr    @b{(input)} IP address of switch
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSProvisioningNewSwitchIPAddrSet(L7_IP_ADDR_t newIPAddr);

/*********************************************************************
*
* @purpose  Purge the provisioning opearational data
*
* @param    none  @b{(input)} none 
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_RC_t wdmWSProvisioningDataPurge(void);

/*********************************************************************
*
* @purpose  Mutual Authentication provisioning initiate status check.
*
* @param    none  
*
* @returns  L7_SUCCESS if all states are valid to initiate new request
* @returns  L7_FAILURE if atleat one state is invalid to initiate new request
*
* @comments This API should be used internal to wdm and the wdm lock should be 
*           aquired before invocation.
*
* @end
*
*********************************************************************/
L7_RC_t wdmMutualAuthProvisioningInitiateStatusGet(void);

/*********************************************************************
*
* @purpose  Get the IP port for UDP Tunnel communication
*
* @param    L7_short16 *ipTnlUdpPort  @b{(output)} pointer to Tunnel UDP port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmIpTnlUdpPortGet (L7_ushort16 *ipTnlUdpPort);

/*********************************************************************
*
* @purpose  Get the IP port for UDP communication
*
* @param    L7_short16 *ipUdpPort  @b{(output)} pointer to UDP port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmIpUdpPortGet (L7_ushort16 *ipUdpPort);


/*********************************************************************
*
* @purpose  Get the IP port for SSL communication
*
* @param    L7_short16 *ipSslPort  @b{(output)} pointer to SSL port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmIpSslPortGet (L7_ushort16 *ipSslPort);

/*********************************************************************
*
* @purpose  Get the IP port for CAPWAP Src communication
*
* @param    L7_short16 *ipCapwapSrcPort  @b{(output)} pointer to CAPWAP Src port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmIpCapwapSrcPortGet (L7_ushort16 *ipCapwapSrcPort);

/*********************************************************************
*
* @purpose  Get the IP port for CAPWAP Dst communication
*
* @param    L7_short16 *ipCapwapDstPort  @b{(output)} pointer to CAPWAP Dst port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmIpCapwapDstPortGet (L7_ushort16 *ipCapwapDstPort);

/*********************************************************************
*
* @purpose  Get the base IP port for WS communication
*
* @param    L7_short16 *ipBasePort  @b{(output)} pointer to base port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmIpBasePortGet (L7_ushort16 *ipBasePort);

/*********************************************************************
*
* @purpose  Get the configured base IP port for WS communication
*
* @param    L7_short16 *ipBasePort  @b{(output)} pointer to base port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmCfgIpBasePortGet (L7_ushort16 *ipBasePort);

/*********************************************************************
*
* @purpose  Set the configured base IP port for WS communication
*
* @param    L7_short16 ipBasePort  @b{(input)} base port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Set to port 0 to restore default
*
* @end
*
*********************************************************************/
L7_RC_t wdmCfgIpBasePortSet (L7_ushort16 ipBasePort);

#endif /* INCLUDE_WDM_API_H */
