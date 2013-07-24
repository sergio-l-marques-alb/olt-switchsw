/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename usmdb_wdm_client_security_api.h
*
* @purpose  Wireless Data Manager (WDM) USMDB API functions
*
* @component    WDM
*
* @comments none
*
* @create   12/20/2007
*
* @author   
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_USMDB_WDM_CLIENT_SECURITY_API_H
#define INCLUDE_USMDB_WDM_CLIENT_SECURITY_API_H

#include "wireless_commdefs.h"
#include "wireless_comm_structs.h"
#include "wdm_client_security_api.h"

/*********************************************************************
*
* @purpose  Get the detected client status
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientStatusGet(L7_enetMacAddr_t macAddr,
                                L7_WDM_DETECTED_CLIENT_STATUS_t *status);

/*********************************************************************
*
* @purpose  Get the detected client authentication status
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientAuthStatusGet(L7_enetMacAddr_t macAddr,
                                    L7_WDM_CLIENT_AUTHENTICATION_STATUS_t *status);

/*********************************************************************
* @purpose  Get the detected client  last updated entry time
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientEntryLastUpdatedTimeGet(L7_enetMacAddr_t macAddr,
                                              L7_uint32 *value);

/*********************************************************************
* @purpose  Get the detected client threat detected status
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientThreatDetectionStatusGet(L7_enetMacAddr_t macAddr,
                                               L7_WDM_CLIENT_THREAT_DETECTION_STATUS_t *status);

/*********************************************************************
* @purpose  Get the detected client threat mitigation status
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientThreatMitigationStatusGet(L7_enetMacAddr_t macAddr,
                                                L7_WDM_CLIENT_THREAT_MITIGATION_STATUS_t *status);

/*********************************************************************
* @purpose  Get the detected client name
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientNameGet(L7_enetMacAddr_t macAddr,
                              L7_char8 *name);

/*********************************************************************
* @purpose Delete all the Detected client entries
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientEntryDeleteAll();

/*********************************************************************
* @purpose Delete the Detected client entry
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientEntryDelete(L7_enetMacAddr_t macAddr);

/*********************************************************************
* @purpose  This deletes the roam and pre-authenticaiton history for
*           all the clients.It also resets the Threat Detection Status
*           and Threat Mitigation Status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientHistoryPurgeAll();

/********************************************************************
* @purpose  This deletes the roam and pre-authenticaiton history for
*           the client.It also resets the Threat Detection Status
*           and Threat Mitigation Status.
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientHistoryPurge(L7_enetMacAddr_t macAddr);

/********************************************************************
* @purpose  This changes the client status of all Rogue Clients from
*           Rogue to Known or Authenticated
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientAckRogueAll();

/*********************************************************************
* @purpose  This changes the client status from Rogue to
*           Known or Authenticated.
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientAckRogue(L7_enetMacAddr_t macAddr);


/*********************************************************************
* @purpose  Get the detected client entry created time
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientEntryCreatedTimeGet(L7_enetMacAddr_t macAddr,
                                          L7_uint32 *value);

#if 0 /* OUI NOT Supported in this release */
/*********************************************************************
* @purpose  Get the detected client Organizationally Unique Identifier
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientOUIGet(L7_enetMacAddr_t macAddr,
                             L7_uchar8 *name);
#endif

/*********************************************************************
* @purpose  Get the detected client authenticated RSSI
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientAuthRSSIGet(L7_enetMacAddr_t macAddr,
                                  L7_uint8 *value);


/*********************************************************************
* @purpose  Get the detected client Authenticated Client Signal
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_int32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientAuthSignalGet(L7_enetMacAddr_t macAddr,
                                    L7_int32 *value);


/*********************************************************************
* @purpose  Get the detected client Authenticated Client Noise
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_int32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientAuthChannelNoiseGet(L7_enetMacAddr_t macAddr,
                                          L7_int32 *value);


/*********************************************************************
* @purpose  Get the number of Probe Requests During Collection Interval
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientProbeReqsRecordedGet(L7_enetMacAddr_t macAddr,
                                          L7_uint32 *value);

/*********************************************************************
* @purpose  Get the number of Time remaining in probe collection Interval
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientProbeCollIntvlGet(L7_enetMacAddr_t macAddr,
                                        L7_uint32 *value);

/*********************************************************************
* @purpose  Get the Highest Number of Probes Detected in Collection Interval
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientMaxNumProbesDetectedGet(L7_enetMacAddr_t macAddr,
                                              L7_uint32 *value);

/*********************************************************************
* @purpose  Get the Number of Authentication messages During Collection Interval
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientAuthMsgsRecordedGet(L7_enetMacAddr_t macAddr,
                                          L7_uint32 *value);

/*********************************************************************
* @purpose  Get the Time remaining in Authentication collection Interval
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientAuthCollIntvlGet(L7_enetMacAddr_t macAddr,
                                       L7_uint32 *value);

/*********************************************************************
* @purpose  Get the Highest Number of Authentications Detected in Collection Interval
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientMaxNumAuthMsgsGet(L7_enetMacAddr_t macAddr,
                                        L7_uint32 *value);

/*********************************************************************
* @purpose  Get the Number of De-Authentication messages During Collection Interval
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientDeAuthMsgsDetectedGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 *value);

/*********************************************************************
* @purpose  Get the Time remaining in De-Authentication collection Interval
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientDeAuthCollIntvlGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 *value);

/*********************************************************************
* @purpose  Get the Highest Number of De-Authentications Detected in Collection Interval
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientMaxNumDeAuthMsgsGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 *value);

/*********************************************************************
* @purpose  Get the Number of 802.1X Authentication failures.
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientNumAuthFailuresGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 *value);

/*********************************************************************
* @purpose  Get the Number of probes detected in the last RF Scan
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientNumProbesDetectedGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 *value);

/*********************************************************************
* @purpose  Get the Number of probes to broadcast BSSID in the last RF Scan
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientBcastBSSIDProbesGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 *value);

/*********************************************************************
* @purpose  Get the Number of probes to broadcast SSID in the last RF Scan
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientBcastSSIDProbesGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 *value);

/*********************************************************************
* @purpose  Get the Number of probes to specific broadcast BSSID in the last RF Scan
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientNumSpecificBSSIDProbesGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 *value);

/*********************************************************************
* @purpose  Get the Number of probes to specific broadcast SSID in the last RF Scan
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientNumSpecificSSIDProbesGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 *value);

/*********************************************************************
* @purpose  Get the Last non-broadcast BSSID detected in the RF Scan
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientLastNonBcastBSSIDGet(L7_enetMacAddr_t macAddr,
                                           L7_enetMacAddr_t *addr);

/*********************************************************************
* @purpose  Get the Last non-broadcast SSID detected in the RF Scan
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientLastNonBcastSSIDGet(L7_enetMacAddr_t macAddr,
                                          L7_char8 *value);

/*********************************************************************
* @purpose  Get the detected client last threat mitigation msg sent time
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientThreatMitigationSentGet(L7_enetMacAddr_t macAddr,
                                                      L7_uint32 *value);

/*********************************************************************
* @purpose  Get the detected client channel
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uchar8 *value           @b{(output)} pointer to store the channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientChannelGet(L7_enetMacAddr_t macAddr,
                                         L7_uchar8 *value);

/*********************************************************************
* @purpose  Get the client entry details
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientEntryGet(L7_enetMacAddr_t macAddr);

/*********************************************************************
* @purpose  Get the next client entry details
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_enetMacAddr_t *addr     @b{(output)}  client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientEntryNextGet(L7_enetMacAddr_t macAddr,
                                   L7_enetMacAddr_t *addr);

/*********************************************************************
* @purpose  Get the current entries in detected client database
*
* @param    L7_uint32 *value     @b{(output)} pointer to store the value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientNumOfEntriesGet(L7_uint32 *value);
/*********************************************************************
* @purpose  Get the maximum entries in detected client database
*
* @param    L7_uint32 *value     @b{(output)} pointer to store the value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientMaxNumOfEntriesGet(L7_uint32 *value);


/*********************************************************************
*
* @purpose  Get the WIDS Client Rogue Classification Test Results.
*
* @param    L7_enetMacAddr_t                              macAddr
*                   @b{(input)} Client mac address
* @param    L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t    testId
*                   @b{(input)} rogue classification test identifier
* @param    wdmWidsClientRogueClassificationTestResults_t *result
*                   @b{(output)} pointer to the classification test results
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmClientRogueClassificationGet(L7_enetMacAddr_t macAddr,
                    L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId,
                    wdmWidsClientRogueClassificationTestResults_t *result);

/*********************************************************************
*
* @purpose  Get the WIDS Client Rogue Classification Test Description.
*
* @param    L7_enetMacAddr_t       client mac address
*           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t   testId
*                   @b{(input)} rogue classification test identifier
*
* @returns  Pointer to the test description
*
* @comments none
*
* @end
*
*********************************************************************/
L7_char8 *usmDbWdmClientRogueClassificationDescrGet(
              L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId);

/*********************************************************************
*
* @purpose  Get the next WIDS Client Rogue Classification Test Result.
*
* @param    L7_enetMacAddr_t                              macAddr
*                   @b{(input)} Client mac address
* @param    L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t    testId
*                   @b{(input)} rogue classification test identifier
* @param    wdmWidsClientRogueClassificationTestResults_t *result
*                   @b{(output)} pointer to the classification test results
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmClientRogueClassificationNextGet(L7_enetMacAddr_t macAddr,
                L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t *testId,
                wdmWidsClientRogueClassificationTestResults_t *result);

/*********************************************************************
* @purpose  Get the WIDS Client Rogue Classification condition detected
*
* @param    L7_enetMacAddr_t       client mac address
*           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t   testId
*                   @b{(input)} rogue classification test identifier
*           L7_BOOL value  @b{(output)} rogue classification condition detected
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmClientRogueClassificationConditionDetectedGet(
                           L7_enetMacAddr_t macAddr,
                           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId,
                           L7_BOOL *value);

/*********************************************************************
* @purpose  Get the AP mac address which detected this condition last
*
* @param    L7_enetMacAddr_t       client mac address
*           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t   testId
*                   @b{(input)} rogue classification test identifier
*           L7_enetMacAddr_t       *ap Mac address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmClientRogueClassificationApMacGet(L7_enetMacAddr_t macAddr,
                   L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId,
                   L7_enetMacAddr_t *apMac);

/*********************************************************************
* @purpose  Get the radio of the AP which detected this condition last
*
* @param    L7_enetMacAddr_t       client mac address
*           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t   testId
*                   @b{(input)} rogue classification test identifier
*           L7_uchar8       *radio
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmClientRogueClassificationRadioGet(L7_enetMacAddr_t macAddr,
                   L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId,
                   L7_uchar8 *radio);

/*********************************************************************
* @purpose  Get the test enabled flag
*
* @param    L7_enetMacAddr_t       client mac address
*           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t   testId
*                   @b{(input)} rogue classification test identifier
*           L7_uchar8       *value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmClientRogueClassificationTestEnabledGet(L7_enetMacAddr_t macAddr,
                           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId,
                           L7_BOOL *value);

/*********************************************************************
* @purpose  Get the WIDS Client Rogue reported
*
* @param    L7_enetMacAddr_t       client mac address
*           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t   testId
*                   @b{(input)} rogue classification test identifier
*           L7_BOOL value  @b{(output)} rogue reported
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbdmClientRogueClassificationRogueDetectedGet(
                            L7_enetMacAddr_t macAddr,
                            L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId,
                            L7_BOOL *value);

/*********************************************************************
* @purpose  Get the WIDS Client Rogue first detected
*
* @param    L7_enetMacAddr_t       client mac address
*           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t   testId
*                   @b{(input)} rogue classification test identifier
*           L7_uint32 *value  @b{(output)}first detected
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmClientRogueClassificationFirstDetectedGet(L7_enetMacAddr_t macAddr,
                           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId,
                           L7_uint32 *value);

/*********************************************************************
* @purpose  Get the WIDS Client Rogue last detected
*
* @param    L7_enetMacAddr_t       client mac address
*           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t   testId
*                   @b{(input)} rogue classification test identifier
*           L7_uint32 *value  @b{(output)}last detected
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmClientRogueClassificationLastDetectedGet(L7_enetMacAddr_t macAddr,
                           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId,
                           L7_uint32 *value);

/*********************************************************************
* @purpose  Check if Detected Client database is enabled
*
* @param
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientDatabasePresent();

/*********************************************************************
* @purpose  Add a client to the Known Client Database.
*
* @param    clientMacAddr                 @b{(input)} client MAC address
* @param    L7_uint32                     @b{(input)} client MAC auth action
* @param    L7_char8                      @b{(input)} client nick name
*
* @returns  wsKnownClientEntryData_t      pointer to known client entry
*
* @notes    none
*
* @end
*********************************************************************/
wsKnownClientEntryData_t *usmDbWdmKnownClientEntryAdd(L7_enetMacAddr_t clientMacAddr,
                                                      L7_uint32 authAction,
                                                      L7_char8 *nickName);


/*********************************************************************
* @purpose  Delete a client from the Known Client Database.
*
* @param    clientMacAddr     @b{(input)} client mac address
*
* @returns  L7_SUCCESS    if entry successfully deleted
* @returns  L7_FAILURE    otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmKnownClientEntryDelete(L7_enetMacAddr_t clientMacAddr);


/*********************************************************************
* @purpose  Lookup the entry for a given Known Client.
*
* @param    macAddr     @b{(input)} MAC address of the client
*
* @returns  pointer to Client Entry
*
* @notes    none
*
* @end
*********************************************************************/
wsKnownClientEntryData_t *usmDbWdmKnownClientEntryLookup(L7_enetMacAddr_t clientMacAddr);


/*********************************************************************
*
* @purpose  Find if the Konwn Client entry exists
*
* @param    L7_enetMacAddr_t clientMacAddr @b{(input)} Client MAC Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmKnownClientEntryGet (L7_enetMacAddr_t clientMacAddr);


/*********************************************************************
* @purpose  Get the MAC address of next Known Client entry.
*
* @param    L7_enetMacAddr_t clientMacAddr @b{(input)} Client MAC Address
* @param    L7_enetMacAddr_t  *nextMacAddr  @b{(output)} pointer to next mac
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmKnownClientEntryNextMACGet(L7_enetMacAddr_t macAddr, 
                                           L7_enetMacAddr_t *nextMacAddr);

/*********************************************************************
* @purpose  Get the next Known Client entry.
*
* @param    clientEntry   @b{(input)} Current Known Client entry
*
* @returns  pointer to the newxt Known Client Entry
*
* @notes    none
*
* @end
*********************************************************************/
wsKnownClientEntryData_t *usmDbWdmKnownClientEntryNextGet(wsKnownClientEntryData_t *clientEntry);


/*********************************************************************
*
* @purpose Set the Known Client entry MAC authentication action.
*
* @param    clientMacAddr   @b{(input)} client MAC address
* @param    L7_uint32       @b{(input)} client MAC auth action
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbKnownClientActionSet(L7_enetMacAddr_t clientMacAddr, 
                                     L7_uint32 authAction);


/*********************************************************************
*
* @purpose Set the Known Client entry nick name.
*
* @param    clientMacAddr                 @b{(input)} client MAC address
* @param    L7_char8                      @b{(input)} client nick name
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmKnownClientNameSet(L7_enetMacAddr_t clientMacAddr, 
                                   L7_char8 *nickName);


/*********************************************************************
*
* @purpose Get the Known Client entry MAC authentication action.
*
* @param    clientMacAddr  @b{(input)} client MAC address
* @param    L7_uint32      @b{(input)} client MAC auth action

*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmKnownClientActionGet(L7_enetMacAddr_t clientMacAddr, 
                                     L7_uint32 *authAction);


/*********************************************************************
*
* @purpose Get the Known Client entry nick name.
*
* @param    clientMacAddr                 @b{(input)} client MAC address
* @param    L7_char8                      @b{(input)} client nick name
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmKnownClientNameGet(L7_enetMacAddr_t clientMacAddr, 
                                   L7_char8 *nickName);


/*********************************************************************
*
* @purpose Check if the Known Client entry exists.
*
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmKnownClientEntryExist(void);


/*********************************************************************
*
* @purpose Get the number of Known Clients configured.
*
* @param    count      @b{(count)} count of entries
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmKnownClientCfgNumGet(L7_ushort16 *count);

/*********************************************************************
* @purpose Delete all the known client entries
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmKnownClientEntryDeleteAll(void);

/*********************************************************************
*
* @purpose  Verify if an event entry for specified client with given time 
*           stamp exist in the the detected client preAuth history.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_uint32         time      @b{(input)} Time Stamp
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientPreAuthHistoryEntryGet(L7_enetMacAddr_t macAddr, 
                     L7_uint32 index);

/*********************************************************************
*
* @purpose  Get next pre-auth event record(key) for specified client with 
*           given time stamp.
*
* @param    L7_enetMacAddr_t macAddr    @b{(input)} client mac address
* @param    L7_uint32        index      @b{(input)} index value.
* @param    L7_enetMacAddr_t *nextMac   @b{(output)} pointer to client 
*                                       mac address
* @param    L7_uint32        *nextIndex @b{(output)} pointer to next index.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientPreAuthHistoryEntryGetNext(L7_enetMacAddr_t macAddr, 
                     L7_uint32 index, L7_enetMacAddr_t *nextMac, 
                     L7_uint32 *nextIndex);

/*********************************************************************
*
* @purpose  Get AP mac address and radioIf from the event record, to which
*           the specified client has pre-authenticated.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_uint32         time      @b{(input)} Time Stamp
* @param    L7_enetMacAddr_t  apMac     @b{(output)} pointer to AP mac address
* @param    L7_uint8          *radioIf  @b{(output)} pointer to radio no.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientPreAuthHistoryEntryAPRadioGet(
                     L7_enetMacAddr_t macAddr, 
                     L7_uint32 index,L7_enetMacAddr_t *apMac, 
                     L7_uint8 *radioIf );

/*********************************************************************
*
* @purpose  Get the SSID from the event record, to which
*           the specified client has pre-authenticated.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_uint32         time      @b{(input)} index Stamp
* @param    L7_char8          *ssid      @b{(output)} pointer to ssid.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientPreAuthHistoryEntrySSIDGet(
                     L7_enetMacAddr_t macAddr, 
                     L7_uint32 index,L7_char8 *ssid);

/*********************************************************************
*
* @purpose  Get the User Name from the event record, to which
*           the specified client has pre-authenticated.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_uint32         index     @b{(input)} index value.
* @param    L7_char8          *name      @b{(output)} pointer to user name.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientPreAuthHistoryEntryUserNameGet(
                     L7_enetMacAddr_t macAddr,
                     L7_uint32 index, L7_char8 *name);

/*********************************************************************
*
* @purpose  Get the Time since the pre-auth event occured for the specified 
*           client.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_uint32         time      @b{(input)} Time Stamp
* @param    L7_uint32         *timeVal  @b{(output)} pointer to timeVal.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientPreAuthHistoryEntryTimeGet(
                     L7_enetMacAddr_t macAddr, 
                     L7_uint32 index,L7_uint32 *timeVal);


/*********************************************************************
*
* @purpose  Get the auth. status result of the pre-auth event occured for
*           the specified client.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_uint32         time      @b{(input)} Time Stamp
* @param    L7_WDM_CLIENT_AUTHENTICATION_STATUS_t *status
*                                       @b{(output)} pointer to status value.

* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientPreAuthHistoryEntryStatusGet(
                     L7_enetMacAddr_t macAddr, 
                     L7_uint32 index,
                     L7_WDM_CLIENT_AUTHENTICATION_STATUS_t *status);

/*********************************************************************
*
* @purpose  Purge all the pre-auth entries recorded for the specified 
*           detected client.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientPreAuthHistoryPurge(L7_enetMacAddr_t macAddr);


/*********************************************************************
*
* @purpose  Purge pre-auth history for all of the clients in detected 
*           clients.
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
L7_RC_t usmDbWdmDetectedClientPreAuthHistoryPurgeAll(void);

/*********************************************************************
*
* @purpose  Get VAP mac address from the event record, to which
*           the specified client has pre-authenticated.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_uint32         time      @b{(input)} Time Stamp
* @param    L7_enetMacAddr_t  vapMac     @b{(output)} pointer to VAP mac addr.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientPreAuthHistoryEntryVAPGet(
                     L7_enetMacAddr_t macAddr, 
                     L7_uint32 index,L7_enetMacAddr_t *vapMac);

/*********************************************************************
*
* @purpose  Get the max. allowed and accumulated depth of pre-auth history 
*           for the specified detected client.
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *maxVal          @b{(output)} pointer to store the 
*                                       max. no. of history entries can be 
*                                       recorded per client.
*           L7_uint32 *count           @b{(output)} pointer to store the 
*                                       no. of recorded entries.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientPreAuthHistoryDepthGet(L7_enetMacAddr_t macAddr,
                                   L7_uint32 *maxVal, L7_uint32 *count);

/*********************************************************************
*
* @purpose  Get the max. allowed and accumulated depth of pre-auth 
*           history for all of detected clients.
*
* @param    L7_uint32 *maxVal          @b{(output)} pointer to store the 
*                                       global max no. of pre-auth events.
* @param    L7_uint32 *count           @b{(output)} pointer to store the 
*                                       no. of recorded entries.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientPreAuthHistoryGlobalDepthGet(L7_uint32 *maxVal, 
                     L7_uint32 *count);

/*********************************************************************
*
* @purpose  To add/update an event into Roam history.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} client mac address
* @param    L7_enetMacAddr_t  apMacAddr   @b{(input)} AP mac address
* @param    L7_uint8          radioIf     @b{(input)} radio interface no.
* @param    L7_enetMacAddr_t  vapMacAddr  @b{(input)} VAP mac address
* @param    L7_char8          *ssid       @b{(input)} pointer to the ssid string
* @param    L7_WDM_DETECTED_CLIENT_HISTORY_EVENT_t status
*                                         @b{(input)} enumerated status value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientRoamHistoryEntryAdd(
                     L7_enetMacAddr_t macAddr, L7_enetMacAddr_t apMacAddr, 
                     L7_uint8 radioIf, L7_enetMacAddr_t vapMacAddr, 
                     L7_char8 *ssid,  
                     L7_WDM_DETECTED_CLIENT_HISTORY_EVENT_t status);

/*********************************************************************
*
* @purpose  Verify if an event entry for specified client with given vap
*           mac add. exist in the the detected client Roam history.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_enetMacAddr_t  vapMac    @b{(input)} vap mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientRoamHistoryEntryGet(L7_enetMacAddr_t macAddr, 
                     L7_uint32 index);

/*********************************************************************
*
* @purpose  Get next Roam event record(key) for specified client with 
*           vap mac.
*
* @param  L7_enetMacAddr_t macAddr     @b{(input)} client mac address
* @param  L7_enetMacAddr_t vapMac      @b{(input)} vap mac address
* @param  L7_enetMacAddr_t *nextMac    @b{(output)} pointer to client mac add.
* @param  L7_uint32        *nextIndex  @b{(output)} pointer to next index.
*
* @returns L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientRoamHistoryEntryGetNext(L7_enetMacAddr_t macAddr, 
                     L7_uint32 index, L7_enetMacAddr_t *nextMac, 
                     L7_uint32 *nextIndex);

/*********************************************************************
*
* @purpose  Get AP mac address and radioIf from Roaming event record, 
*           to which the specified client has Roamed.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_enetMacAddr_t  vapMac    @b{(input)} vap mac address
* @param    L7_enetMacAddr_t  *apMac    @b{(output)} pointer to ap mac.
* @param    L7_uint8          *radioIf  @b{(input)} pointer to radio no.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientRoamHistoryEntryAPRadioGet(
                     L7_enetMacAddr_t macAddr, 
                     L7_uint32 index,L7_enetMacAddr_t *apMac, 
                     L7_uint8 *radioIf );

/*********************************************************************
*
* @purpose  Get the SSID from Roaming event record, to which
*           the specified client has Roamed.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_enetMacAddr_t  vapMac    @b{(input)} vap mac address
* @param    L7_char8          *ssid     @b{(output)} pointer to ssid.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientRoamHistoryEntrySSIDGet(
                     L7_enetMacAddr_t macAddr, 
                     L7_uint32 index,L7_char8 *ssid);

/*********************************************************************
*
* @purpose  Get VAP mac address from the event record, to which
*           the specified client has roamed.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_uint32         index      @b{(input)} index value of the entry
* @param    L7_enetMacAddr_t  vapMac     @b{(output)} pointer to VAP mac addr.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientRoamHistoryEntryVAPGet(
                     L7_enetMacAddr_t macAddr, 
                     L7_uint32 index,L7_enetMacAddr_t *vapMac);

/*********************************************************************
*
* @purpose  Get the Time since the Roam event occured for the specified 
*           client with given mac add.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_enetMacAddr_t  vapMac    @b{(input)} vap mac address
* @param    L7_uint32         *time     @b{(output)} pointer to time.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientRoamHistoryEntryTimeGet(
                     L7_enetMacAddr_t macAddr, 
                     L7_uint32 index,L7_uint32 *time);

/*********************************************************************
*
* @purpose  Get the auth. status result of the Roam event occured for
*           the specified client.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_enetMacAddr_t  vapMac    @b{(input)} vap mac address
* @param    L7_WDM_CLIENT_AUTHENTICATION_STATUS_t *status
*                                       @b{(output)} pointer to status value.

* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientRoamHistoryEntryStatusGet(
                     L7_enetMacAddr_t macAddr, 
                     L7_uint32 index,
                     L7_WDM_DETECTED_CLIENT_HISTORY_EVENT_t *status);

/*********************************************************************
*
* @purpose  Purge all the Roam entries recorded for the specified 
*           detected client.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientRoamHistoryPurge(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Purge Roam history for all of the clients in detected 
*           clients.
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
L7_RC_t usmDbWdmDetectedClientRoamHistoryPurgeAll(void);

/*********************************************************************
*
* @purpose  Delete the Roam history event recorded for specified 
*           client with given vap mac add.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_enetMacAddr_t  vapMac    @b{(input)} vap mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientRoamHistoryEntryDelete(L7_enetMacAddr_t macAddr, 
                     L7_enetMacAddr_t vapMac);

/*********************************************************************
*
* @purpose  Delete all the Roam history events recorded for 
*           specified client.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_enetMacAddr_t  vapMac    @b{(input)} vap mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientRoamHistoryDeleteAll(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Get the max. and accumulated depth of Roam history 
*           for the specified detected client.
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *maxVal          @b{(output)} pointer to store the 
*                                       max. no. of history entries can be 
*                                       recorded per client.
*           L7_uint32 *count           @b{(output)} pointer to store the 
*                                       no. of recorded entries.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientRoamHistoryDepthGet(L7_enetMacAddr_t macAddr,
                                   L7_uint32 *maxVal, L7_uint32 *count);

/*********************************************************************
*
* @purpose  Get the global max. and accumulated depth of Roam 
*           history.
*
* @param    L7_uint32 *maxVal          @b{(output)} pointer to store the 
*                                       global max no. of Roam events.
* @param    L7_uint32 *count           @b{(output)} pointer to store the 
*                                       no. of recorded entries.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t smDbWdmDetectedClientRoamHistoryGlobalDepthGet(L7_uint32 *maxVal, 
                     L7_uint32 *count);

/*********************************************************************
*
* @purpose Get complete signal triangulation information of the specified
*          detected client.
*
* @param  L7_enetMacAddr_t macAddr @b{(input)} detected client mac addr.
* @param  wdmDetectedClientTriangulationStatus_t *status
*                        @b{(output)} pointer to store complete 
*                                    triangulation data.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments  Though wdm maintains the triangulation entries in ascending 
*            order of RSSI values, this API gives them in reverse order;
*            from entry with largest RSSI to that of smallest one.
*            
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientTriangulationEntryGet(L7_enetMacAddr_t macAddr, 
                     wdmDetectedClientTriangulationStatus_t *status);

/*********************************************************************
*
* @purpose Get Sentry signal triangulation information of the specified
*          detected client.
*
* @param  L7_enetMacAddr_t macAddr @b{(input)} detected client mac addr.
* @param  wdmDetectedClientTriangulationStatus_t *sentryEntry
*                        @b{(output)} pointer to store sentry
*                                    triangulation data.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments  Though wdm maintains the triangulation entries in ascending
*            order of RSSI values, this API gives them in reverse order;
*            from entry with largest RSSI to that of smallest one.
*
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientTriangulationSentryEntryGet(L7_enetMacAddr_t macAddr,
                     wdmDetectedClientTriangulationEntry_t *sentryEntry);

/*********************************************************************
*
* @purpose Get non sentry signal triangulation information of the specified
*          detected client.
*
* @param  L7_enetMacAddr_t macAddr @b{(input)} detected client mac addr.
* @param  wdmDetectedClientTriangulationStatus_t *nonSentryEntry
*                        @b{(output)} pointer to store complete
*                                    triangulation data.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments  Though wdm maintains the triangulation entries in ascending
*            order of RSSI values, this API gives them in reverse order;
*            from entry with largest RSSI to that of smallest one.
*
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientTriangulationNonSentryEntryGet(L7_enetMacAddr_t macAddr,
                     wdmDetectedClientTriangulationEntry_t *nonSentryEntry);

/*********************************************************************
*
* @purpose  Get the max. allowed and accumulated depth of pre-auth 
*           history for all of detected clients.
*
* @param    L7_uint32 *maxVal          @b{(output)} pointer to store the 
*                                       global max no. of pre-auth events.
* @param    L7_uint32 *count           @b{(output)} pointer to store the 
*                                       no. of recorded entries.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientPreAuthHistoryGlobalDepthGet(L7_uint32 *maxVal, 
                     L7_uint32 *count);

/*********************************************************************
*
* @purpose  Get the max. allowed and accumulated depth of roam 
*           history for all of detected clients.
*
* @param    L7_uint32 *maxVal          @b{(output)} pointer to store the 
*                                       global max no. of pre-auth events.
* @param    L7_uint32 *count           @b{(output)} pointer to store the 
*                                       no. of recorded entries.
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientRoamHistoryGlobalDepthGet(L7_uint32 *maxVal, 
                     L7_uint32 *count);

/*********************************************************************
* @purpose  Get the detected client Organizationally Unique Identifier
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbWdmDetectedClientOUIGet(L7_enetMacAddr_t macAddr,
                                 L7_char8 *oui);

#endif /* INCLUDE_USMDB_WDM_CLIENT_SECURITY_API_H */
