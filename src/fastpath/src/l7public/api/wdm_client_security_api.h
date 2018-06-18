/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     wdm_client_security_api.h
*
* @purpose      Wireless Data Manager (WDM) API header
*
* @component    WDM
*
* @comments     none
*
* @create       12/20/2007
*
* @author       
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_WDM_CLIENT_SECURITY_API_H
#define INCLUDE_WDM_CLIENT_SECURITY_API_H

#include "datatypes.h"
#include "l7_packet.h"
#include "osapi.h"
#include "wireless_commdefs.h"
#include "wireless_comm_structs.h"
#include "wdm_nv_api.h"

typedef struct wdmWidsClientCfgData_s
{
  L7_uint32 widsClientDeauthThreshold; 
  L7_uint32 widsClientAuthThreshold; 
  L7_uint32 widsClientProbeThreshold; 
  L7_uint32 widsClientAuthFailureThreshold; 
  L7_ushort16 widsClientDetectedRogueTrapInterval;
  L7_ushort16 widsClientDeauthIntervalThreshold; 
  L7_ushort16 widsClientAuthIntervalThreshold; 
  L7_ushort16 widsClientProbeIntervalThreshold; 
  L7_uchar8 widsClientNotInOUIList; 
  L7_uchar8 widsClientNotInKnownDB; 
  L7_uchar8 widsClientConfiguredAuthRate; 
  L7_uchar8 widsClientConfiguredProbeRate; 
  L7_uchar8 widsClientConfiguredDeauthRate; 
  L7_uchar8 widsClientMaxAuthFailure; 
  L7_uchar8 widsClientAuthWithUnknownAP; 
  L7_uchar8 widsClientThreatMitigation; 
  L7_WDM_DETECTED_CLIENT_KNOWN_DATABASE_t widsClientKnownDB;
  L7_char8  widsClientKnownDBRadiusServerName[L7_WDM_RADIUS_SERVER_NAME_MAX + 1];
  L7_uint32 widsClientKnownDBRadiusServerStatus; 
} wdmWidsClientCfgData_t;

/**** Detected Clients Status ****/
typedef struct wdmDetectedClientStatus_s
{
 L7_enetMacAddr_t macAddr;
 L7_WDM_DETECTED_CLIENT_STATUS_t clientStatus;
 L7_WDM_CLIENT_AUTHENTICATION_STATUS_t clientAuthenticated;
 L7_uint32        entryUpdatedLastAt;
 L7_WDM_CLIENT_THREAT_DETECTION_STATUS_t threatDetected;
 L7_WDM_CLIENT_THREAT_MITIGATION_STATUS_t threatMitigationDone;
 L7_char8         clientName[L7_WDM_CLIENT_USERNAME_MAX+1];
 L7_uint32        entryCreateTime;
 L7_char8         oui[L7_WDM_OUI_MAX+1];  /* default . UNKNOWN */
 L7_uchar8        authRSSI;
 L7_uchar8        channel;
 L7_int32         authSignal;
 L7_int32         authNoise;
 L7_uint32        probeReqsRecorded;
 L7_uint32        probeCollIntvl;
 L7_uint32        highestNoProbesDetected;
 L7_uint32        authMsgsRecorded;
 L7_uint32        authCollIntvl;
 L7_uint32        highestNoAuthMsgs;
 L7_uint32        deAuthMsgsDetected;
 L7_uint32        deAuthCollIntvl;
 L7_uint32        highestNoDeAuthMsgs;
 L7_uint32        authFailures;
 L7_uint32        probesDetected;
 L7_uint32        bcastBSSIDProbes;
 L7_uint32        bcastSSIDProbes;
 L7_uint32        specificBSSIDProbes;
 L7_uint32        specificSSIDProbes;
 L7_enetMacAddr_t lastNonBcastBSSID;
 L7_char8         lastNonBcastSSID[L7_WDM_SSID_MAX_LENGTH+1];
 L7_uint32        threatMitigationSent;
 wdmWidsClientRogueClassificationTestResults_t result[L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_COUNT];

 /* History indices shall start from largest of their data type and keep 
    decrementing to track each entry.  This helps in snmp indexing mechanism */
 L7_uint32 detectedClientPreAuthHistoryEntryIndex;
 L7_uint32 detectedClientRoamHistoryEntryIndex;
 /* Number of events recorded in pre-auth history for this client */
 L7_uint32         preAuthHistoryEntries;
 /* Number of events recorded in roaming history for this client */
 L7_uint32         roamHistoryEntries;
 wdmDetectedClientTriangulationEntry_t sentryTriangulation[WDM_RFSCAN_TRIANGULATION_ENTRIES];
 wdmDetectedClientTriangulationEntry_t nonSentryTriangulation[WDM_RFSCAN_TRIANGULATION_ENTRIES]; 
 L7_BOOL             nvFlag; /* Flag indicates mapped detected client in NV */
 wdmNVCoordCfgData_t nvCfg;/* NV coordinates */  
 void             *avlPtr; /*MUST be last field,reserved for AVL tree*/
}wdmDetectedClientStatus_t;

/****************************************
*
*  Known Client Database
*
*****************************************/
typedef struct wsKnownClient_s
{
   L7_enetMacAddr_t             macAddr;    /* AVL key */
   L7_WDM_KNOWN_CLIENT_ACTION_t authAction;
   L7_char8                     nickName[L7_WDM_CLIENT_USERNAME_MAX+1];
}wsKnownClient_t;

typedef struct wsKnownClientDataEntry_s
{
   wsKnownClient_t kcData;
   void *avlPtr;                            /* Reserved for AVL tree,  must be last field */
} wsKnownClientEntryData_t;

typedef struct knownClientRadiusAttributes_s
{
  L7_WDM_KNOWN_CLIENT_ACTION_t authAction;
  L7_char8                     nickName[L7_WDM_CLIENT_USERNAME_MAX+1];
} knownClientRadiusAttributes_t;

typedef struct wdmDetectedClientPreAuthKey_s
{ 
 L7_enetMacAddr_t macAddr; /* client MAC Address */ 
 L7_uint32        index;    
} wdmDetectedClientPreAuthKey_t;

typedef struct wdmClientPreAuthStatus_s
{
 wdmDetectedClientPreAuthKey_t preAuthKey; /* AVL key */
 L7_enetMacAddr_t apMacAddr;
 L7_enetMacAddr_t vapMacAddr;

 /* Network name used by the VAP */
 L7_char8         ssid[L7_WDM_SSID_MAX_LENGTH+1];

 /* User name of the client that authenticated via 802.1X */  
 L7_char8         clientName[L7_WDM_CLIENT_USERNAME_MAX+1];
 
 L7_uchar8        radioIndex; 
 L7_WDM_CLIENT_AUTHENTICATION_STATUS_t preAuthStatus;         /* Status Flag */

 /* Used for age, displayed as xxh:xxm:xxs */
 L7_uint32        timeStamp;    

 /* MUST be last field, reserved for AVL tree */
 void             *avlPtr;      
}wdmDetectedClientPreAuthStatus_t;

typedef struct wdmDetectedClientRoamHistoryKey_s
{ 
 L7_enetMacAddr_t macAddr;
 L7_uint32        index;    
} wdmDetectedClientRoamHistoryKey_t;

typedef struct wdmDetectedClientRoamHistoryStatus_s
{
 wdmDetectedClientRoamHistoryKey_t roamKey; /* AVL key */
 L7_enetMacAddr_t apMac;
 L7_enetMacAddr_t vapMac;
 /* Network name used by the VAP */
 L7_char8         ssid[L7_WDM_SSID_MAX_LENGTH+1]; 
 /* Status Flag indicates new authentication/roam event*/
 L7_BOOL          roamStatus;         
 /* Used for age, displayed as xxh:xxm:xxs */
 L7_uint32        timeStamp;    
 L7_uchar8        radioIndex; 
 /* MUST be last field, reserved for AVL tree */
 void             *avlPtr;      
} wdmDetectedClientRoamHistoryStatus_t;

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
L7_RC_t wdmDetectedClientStatusGet(L7_enetMacAddr_t       macAddr,
                                   L7_WDM_DETECTED_CLIENT_STATUS_t *value);

/*********************************************************************
*
* @purpose  Set the detected client status
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_WDM_DETECTED_CLIENT_STATUS_t value @b{(input)} storing the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientStatusSet(L7_enetMacAddr_t       macAddr,
                                   L7_WDM_DETECTED_CLIENT_STATUS_t value);

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
L7_RC_t wdmDetectedClientAuthStatusGet(L7_enetMacAddr_t macAddr,
                                       L7_WDM_CLIENT_AUTHENTICATION_STATUS_t *value);

/*********************************************************************
*
* @purpose  Set the detected client authentication status
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_WDM_CLIENT_AUTHENTICATION_STATUS_t value @b{(input)} storing the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientAuthStatusSet(L7_enetMacAddr_t macAddr,
                                       L7_WDM_CLIENT_AUTHENTICATION_STATUS_t value);

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
L7_RC_t wdmDetectedClientEntryLastUpdatedTimeGet(L7_enetMacAddr_t macAddr,
                                              L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the detected client last updated entry time
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 value           @b{(input)} storing the time.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientEntryLastUpdatedTimeSet(L7_enetMacAddr_t macAddr,
                                              L7_uint32 value);

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
L7_RC_t wdmDetectedClientThreatDetectionStatusGet(L7_enetMacAddr_t macAddr,
                                                  L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the detected client threat detection status
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 value           @b{(input)} storing the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientThreatDetectionStatusSet(L7_enetMacAddr_t macAddr,
                                                  L7_uint32 value);

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
L7_RC_t wdmDetectedClientThreatMitigationStatusGet(L7_enetMacAddr_t macAddr,
                                                   L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the detected client threat mitigation status
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 value           @b{(input)} pointer storing the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientThreatMitigationStatusSet(L7_enetMacAddr_t macAddr,
                                                  L7_uint32 value);

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
L7_RC_t wdmDetectedClientNameGet(L7_enetMacAddr_t macAddr,
                                 L7_char8 *name);

/*********************************************************************
*
* @purpose  Set the detected client name
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_char8 *name           @b{(input)} pointer storing the name.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientNameSet(L7_enetMacAddr_t macAddr,
                                 L7_char8 *name);

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
L7_RC_t wdmDetectedClientEntryCreatedTimeGet(L7_enetMacAddr_t macAddr,
                                              L7_uint32 *value);

/*********************************************************************
* @purpose  Set the detected client entry created time
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 value           @b{(input)} storing the entry create time.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientEntryCreatedTimeSet(L7_enetMacAddr_t macAddr,
                                             L7_uint32 value);

#if 0 /* OUI NOT supported in this release */
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
L7_RC_t wdmDetectedClientOUIGet(L7_enetMacAddr_t macAddr,
                                 L7_char8 *oui);

/*********************************************************************
*
* @purpose  Set the detected client Organizationally Unique Identifier
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_char8 *oui           @b{(input)} pointer storing the oui.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientOUISet(L7_enetMacAddr_t macAddr,
                                 L7_char8 *oui);
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
L7_RC_t wdmDetectedClientAuthRSSIGet(L7_enetMacAddr_t macAddr,
                                     L7_uint8 *value);

/*********************************************************************
* @purpose  Set the detected client authenticated RSSI
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 value           @b{(input)} storing the auth RSSI.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientAuthRSSISet(L7_enetMacAddr_t macAddr,
                                     L7_uint8 value);

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
L7_RC_t wdmDetectedClientAuthSignalGet(L7_enetMacAddr_t macAddr,
                                       L7_int32 *value);

/*********************************************************************
* @purpose  Set the detected client Authenticated Client Signal
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_int32 value           @b{(input)} storing the auth signal.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientAuthSignalSet(L7_enetMacAddr_t macAddr,
                                       L7_int32 value);

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
L7_RC_t wdmDetectedClientAuthChannelNoiseGet(L7_enetMacAddr_t macAddr,
                                             L7_int32 *value);

/*********************************************************************
* @purpose  Set the detected client Authenticated Client Noise
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 value           @b{(input)} auth client noise
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientAuthChannelNoiseSet(L7_enetMacAddr_t macAddr,
                                             L7_int32 value);

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
L7_RC_t wdmDetectedClientProbeReqsRecordedGet(L7_enetMacAddr_t macAddr,
                                             L7_uint32 *value);

/*********************************************************************
* @purpose  Set the number of Probe Requests During Collection Interval
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 value           @b{(input)} number of probe requests
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientProbeReqsRecordedSet(L7_enetMacAddr_t macAddr,
                                             L7_uint32 value);

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
L7_RC_t wdmDetectedClientProbeCollIntvlGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 *value);

/*********************************************************************
* @purpose  Set the number of Time remaining in probe collection Interval
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 value           @b{(input)} time remaining in probe coll interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientProbeCollIntvlSet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 value);

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
L7_RC_t wdmDetectedClientMaxNumProbesDetectedGet(L7_enetMacAddr_t macAddr,
                                               L7_uint32 *value);

/*********************************************************************
* @purpose  Set the Highest Number of Probes Detected in Collection Interval
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 value           @b{(input)} highest num of probes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientMaxNumProbesDetectedSet(L7_enetMacAddr_t macAddr,
                                               L7_uint32 value);

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
L7_RC_t wdmDetectedClientAuthMsgsRecordedGet(L7_enetMacAddr_t macAddr,
                                               L7_uint32 *value);

/*********************************************************************
* @purpose  Set the Number of Authentication messages During Collection Interval
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 value           @b{(input)} number of auth msgs
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientAuthMsgsRecordedSet(L7_enetMacAddr_t macAddr,
                                               L7_uint32 value);

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
L7_RC_t wdmDetectedClientAuthCollIntvlGet(L7_enetMacAddr_t macAddr,
                                             L7_uint32 *value);

/*********************************************************************
* @purpose  Set the Time remaining in Authentication collection Interval
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 value           @b{(input)} auth collection interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientAuthCollIntvlSet(L7_enetMacAddr_t macAddr,
                                             L7_uint32 value);

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
L7_RC_t wdmDetectedClientMaxNumAuthMsgsGet(L7_enetMacAddr_t macAddr,
                                           L7_uint32 *value);

/*********************************************************************
* @purpose  Set the Highest Number of Authentications Detected in Collection Interval
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 value           @b{(input)} highest num of auth msgs
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientMaxNumAuthMsgsSet(L7_enetMacAddr_t macAddr,
                                           L7_uint32 value);

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
L7_RC_t wdmDetectedClientDeAuthMsgsDetectedGet(L7_enetMacAddr_t macAddr,
                                               L7_uint32 *value);

/*********************************************************************
* @purpose  Set the Number of De-Authentication messages During Collection Interval
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 value           @b{(input)} number of de-auth msgs
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientDeAuthMsgsDetectedSet(L7_enetMacAddr_t macAddr,
                                               L7_uint32 value);

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
L7_RC_t wdmDetectedClientDeAuthCollIntvlGet(L7_enetMacAddr_t macAddr,
                                               L7_uint32 *value);

/*********************************************************************
* @purpose  Set the Time remaining in De-Authentication collection Interval
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 value           @b{(input)} de-auth interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientDeAuthCollIntvlSet(L7_enetMacAddr_t macAddr,
                                               L7_uint32 value);

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
L7_RC_t wdmDetectedClientMaxNumDeAuthMsgsGet(L7_enetMacAddr_t macAddr,
                                               L7_uint32 *value);

/*********************************************************************
* @purpose  Set the Highest Number of De-Authentications Detected in Collection Interval
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 value           @b{(input)} number of de-auth
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientMaxNumDeAuthMsgsSet(L7_enetMacAddr_t macAddr,
                                             L7_uint32 value);

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
L7_RC_t wdmDetectedClientNumAuthFailuresGet(L7_enetMacAddr_t macAddr,
                                               L7_uint32 *value);

/*********************************************************************
* @purpose  Set the Number of 802.1X Authentication failures.
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 value           @b{(input)} number of failures
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientNumAuthFailuresSet(L7_enetMacAddr_t macAddr,
                                               L7_uint32 value);

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
L7_RC_t wdmDetectedClientNumProbesDetectedGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 *value);

/*********************************************************************
* @purpose  Set the Number of probes detected in the last RF Scan
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 value           @b{(input)} number of probes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientNumProbesDetectedSet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 value);

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
L7_RC_t wdmDetectedClientBcastBSSIDProbesGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 *value);

/*********************************************************************
* @purpose  Set the Number of probes to broadcast BSSID in the last RF Scan
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 value           @b{(input)} numner of probes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientBcastBSSIDProbesSet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 value);

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
L7_RC_t wdmDetectedClientBcastSSIDProbesGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 *value);

/*********************************************************************
* @purpose  Set the Number of probes to broadcast SSID in the last RF Scan
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 value           @b{(input)} number of probes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientBcastSSIDProbesSet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 value);

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
L7_RC_t wdmDetectedClientSpecificBSSIDProbesGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 *value);

/*********************************************************************
* @purpose  Set the Number of probes to specific broadcast BSSID in the last RF Scan
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 value           @b{(input)} number of specific probes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientSpecificBSSIDProbesSet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 value);

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
L7_RC_t wdmDetectedClientSpecificSSIDProbesGet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 *value);

/*********************************************************************
* @purpose  Set the Number of probes to specific broadcast SSID in the last RF Scan
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 value           @b{(input)} ssid probes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientSpecificSSIDProbesSet(L7_enetMacAddr_t macAddr,
                                            L7_uint32 value);

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
L7_RC_t wdmDetectedClientLastNonBcastBSSIDGet(L7_enetMacAddr_t macAddr,
                                              L7_enetMacAddr_t *addr);

/*********************************************************************
* @purpose  Set the Last non-broadcast BSSID detected in the RF Scan
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_enetMacAddr_t addr      @b{(input)} non-broadcast BSSID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientLastNonBcastBSSIDSet(L7_enetMacAddr_t macAddr,
                                              L7_enetMacAddr_t addr);

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
L7_RC_t wdmDetectedClientLastNonBcastSSIDGet(L7_enetMacAddr_t macAddr,
                                             L7_char8 *value);

/*********************************************************************
* @purpose  Set the Last non-broadcast SSID detected in the RF Scan
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_char8 *value           @b{(output)} pointer storing the value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientLastNonBcastSSIDSet(L7_enetMacAddr_t macAddr,
                                             L7_char8 *value);

/*********************************************************************
* @purpose  Get the detected client last threat mitigation msg sent time
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the
*                                               mitigation msg sent time.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientThreatMitigationSentGet(L7_enetMacAddr_t macAddr,
                                                 L7_uint32 *value);

/*********************************************************************
* @purpose  Set the detected client last threat mitigation msg sent time
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 value           @b{(input)} storing the last threat
*                                               mitigation msg sent time.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientThreatMitigationSentSet(L7_enetMacAddr_t macAddr,
                                                 L7_uint32 value);

/*********************************************************************
* @purpose  Get the detected client channel
*
* @param    L7_enetMacAddr_t macAddr  @b{(input)}  client MAC address
*           L7_uchar8 *value          @b{(output)} pointer to store the channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientChannelGet(L7_enetMacAddr_t macAddr, L7_uchar8 *value);

/*********************************************************************
*
* @purpose  Delete the Detected client entry
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
L7_RC_t wdmDetectedClientEntryDelete(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Delete all the Detected client entries
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientEntryDeleteAll();

/*********************************************************************
* @purpose  Purge all entries in the Detected client database.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is called only when the WS status is DISABLED
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientEntriesPurge();

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
L7_RC_t wdmDetectedClientHistoryPurge(L7_enetMacAddr_t macAddr);

/********************************************************************
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
L7_RC_t wdmDetectedClientHistoryPurgeAll();

/********************************************************************
* @purpose  This changes the client status from Rogue to
*           Known or Authenticated.
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
L7_RC_t wdmDetectedClientAckRogue(L7_enetMacAddr_t macAddr);

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
L7_RC_t wdmDetectedClientAckRogueAll();


/*********************************************************************
* @purpose Add new entry in Detected Client database or replace an
*          existing entry
*
* @param    wdmDetectedClientStatus_t *pData @b{(input)} entry pointer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If list is full, and a client is detected due to client
*           authentication, then delete a non-auth entry and add this
*           new client
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientEntryAddReplace(wdmDetectedClientStatus_t *pData);

/*********************************************************************
* @purpose  Add or update an entry
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*        L7_WDM_DETECTED_CLIENT_STATUS_t clientStatus,
*        L7_WDM_CLIENT_AUTHENTICATION_STATUS_t clientAuthenticated,
*        L7_WDM_CLIENT_THREAT_DETECTION_STATUS_t threatDetected,
*        L7_WDM_CLIENT_THREAT_MITIGATION_STATUS_t threatMitigationDone,
*        L7_char8 *clientName,
*        L7_char8 *oui,
*        L7_uchar8 authRSSI,
*        L7_int8 authSignal,
*        L7_int8 authNoise,
*        L7_uint32 probeReqsRecorded,
*        L7_uint32 probeCollIntvl,
*        L7_uint32 highestNoProbesDetected,
*        L7_uint32 authMsgsRecorded,
*        L7_uint32 authCollIntvl,
*        L7_uint32 highestNoAuthMsgs,
*        L7_uint32 deAuthMsgsDetected,
*        L7_uint32 deAuthCollIntvl,
*        L7_uint32 highestNoDeAuthMsgs,
*        L7_uint32 authFailures,
*        L7_uint32 probesDetected,
*        L7_uint32 bcastBSSIDProbes,
*        L7_uint32 bcastSSIDProbes,
*        L7_uint32 specificBSSIDProbes,
*        L7_uint32 specificSSIDProbes,
*        L7_enetMacAddr_t lastNonBcastBSSID,
*        L7_char8  *lastNonBcastSSID,
*        L7_uint32 threatMitigationSent
*        L7_uchar8 channel
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientEntryAddUpdate(L7_enetMacAddr_t macAddr,
                        L7_WDM_DETECTED_CLIENT_STATUS_t clientStatus,
                        L7_WDM_CLIENT_AUTHENTICATION_STATUS_t clientAuthenticated,
                        L7_WDM_CLIENT_THREAT_DETECTION_STATUS_t threatDetected,
                        L7_WDM_CLIENT_THREAT_MITIGATION_STATUS_t threatMitigationDone,
                        L7_char8 *clientName,
                        L7_uchar8 authRSSI,
                        L7_int32 authSignal,
                        L7_int32 authNoise,
                        L7_uint32 probeReqsRecorded,
                        L7_uint32 probeCollIntvl,
                        L7_uint32 highestNoProbesDetected,
                        L7_uint32 authMsgsRecorded,
                        L7_uint32 authCollIntvl,
                        L7_uint32 highestNoAuthMsgs,
                        L7_uint32 deAuthMsgsDetected,
                        L7_uint32 deAuthCollIntvl,
                        L7_uint32 highestNoDeAuthMsgs,
                        L7_uint32 authFailures,
                        L7_uint32 probesDetected,
                        L7_uint32 bcastBSSIDProbes,
                        L7_uint32 bcastSSIDProbes,
                        L7_uint32 specificBSSIDProbes,
                        L7_uint32 specificSSIDProbes,
                        L7_enetMacAddr_t lastNonBcastBSSID,
                        L7_char8  *lastNonBcastSSID,
                        L7_uint32 threatMitigationSent,
                        wdmWidsClientRogueClassificationTestResults_t *result,
                        L7_uchar8 channel);

/*********************************************************************
* @purpose  Determine if a client exists in the detected client list
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
L7_RC_t wdmDetectedClientEntryGet(L7_enetMacAddr_t macAddr);

/*********************************************************************
* @purpose  Get the mac address of the next entry
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)}  client MAC address
* @param    L7_enetMacAddr_t *nextMacAddr @b{(output)} next client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments macAddr 00:00:00:00:00:00 returns first entry.
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientEntryNextGet(L7_enetMacAddr_t  macAddr,
                                     L7_enetMacAddr_t *nextMacAddr);

/*********************************************************************
* @purpose  Determine whether rogue reporting for this test is enabled
*           or disabled
*
* @param    L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId @b{(input)} test identifier.
* @param    L7_uint32 *mode @b{(output)} location to store the mode value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t wdmWidsClientRogueDetectTestModeGet(
       L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId, L7_uint32 *mode);

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
L7_RC_t wdmDetectedClientNumOfEntriesGet(L7_uint32 *value);

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
L7_RC_t wdmDetectedClientMaxNumOfEntriesGet(L7_uint32 *value);

/*********************************************************************
* @purpose  Get the WIDS Client Rogue Classification Test Results for all tests.
*
* @param    macAddr @b{(input)} Client mac address
* @param    *result  @b{(output)} pointer to the classification test results
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmClientRogueClassificationGetAll(L7_enetMacAddr_t macAddr,
    wdmWidsClientRogueClassificationTestResults_t *result);


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
L7_RC_t wdmClientRogueClassificationGet(L7_enetMacAddr_t macAddr,
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
L7_char8 *wdmClientRogueClassificationDescrGet(
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
L7_RC_t wdmClientRogueClassificationNextGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t wdmClientRogueClassificationConditionDetectedGet(
                           L7_enetMacAddr_t macAddr,
                           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId,
                           L7_BOOL *value);

/*********************************************************************
* @purpose  Set the WIDS Client Rogue Classification condition detected
*
* @param    L7_enetMacAddr_t       client mac address
*           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t   testId  
*                   @b{(input)} rogue classification test identifier
*           L7_BOOL value  @b{(input)} rogue classification condition detected
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmClientRogueClassificationConditionDetectedSet(
                   L7_enetMacAddr_t macAddr,
                   L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId,
                   L7_BOOL value);

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
L7_RC_t wdmClientRogueClassificationApMacGet(L7_enetMacAddr_t macAddr,
                   L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId,
                   L7_enetMacAddr_t *apMac);

/*********************************************************************
* @purpose  Set the mac address of the ap which last detected this condition
*
* @param    L7_enetMacAddr_t       client mac address
*           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t   testId  
*                   @b{(input)} rogue classification test identifier
*           L7_enetMacAddr_t       ap Mac address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmClientRogueClassificationApMacSet(L7_enetMacAddr_t macAddr,
                   L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId,
                   L7_enetMacAddr_t apMac);

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
L7_RC_t wdmClientRogueClassificationRadioGet(L7_enetMacAddr_t macAddr,
                   L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId,
                   L7_uchar8 *radio);

/*********************************************************************
* @purpose  Set the radio of the ap which last detected this condition
*
* @param    L7_enetMacAddr_t       client mac address
*           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t   testId  
*                   @b{(input)} rogue classification test identifier
*           L7_uchar8      radio
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmClientRogueClassificationRadioSet(L7_enetMacAddr_t macAddr,
                   L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId,
                   L7_uchar8 radio);

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
L7_RC_t wdmClientRogueClassificationTestEnabledGet(L7_enetMacAddr_t macAddr,
                           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId,
                           L7_BOOL *value);

/*********************************************************************
* @purpose  Set the test enabled flag
*
* @param    L7_enetMacAddr_t       client mac address
*           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t   testId  
*                   @b{(input)} rogue classification test identifier
*           L7_uchar8      value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmClientRogueClassificationTestEnabledSet(L7_enetMacAddr_t macAddr,
                          L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId,
                          L7_uchar8 value);

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
L7_RC_t wdmClientRogueClassificationRogueDetectedGet(L7_enetMacAddr_t macAddr,
                            L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId,
                            L7_BOOL *value);

/*********************************************************************
* @purpose  Set the WIDS Client Rogue reported
*
* @param    L7_enetMacAddr_t       client mac address
*           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t   testId  
*                   @b{(input)} rogue classification test identifier
*           L7_BOOL value  @b{(input)} rogue reported
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmClientRogueClassificationRogueReportedSet(L7_enetMacAddr_t macAddr,
                           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId,
                           L7_BOOL value);

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
L7_RC_t wdmClientRogueClassificationFirstDetectedGet(L7_enetMacAddr_t macAddr,
                           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId,
                           L7_uint32 *value);

/*********************************************************************
* @purpose  Set the WIDS Client Rogue first detected
*
* @param    L7_enetMacAddr_t       client mac address
*           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t   testId  
*                    @b{(input)} rogue classification test identifier
*           L7_uint32 value  @b{(input)} first detected
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmClientRogueClassificationFirstDetectedSet(L7_enetMacAddr_t macAddr,
                            L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId,
                            L7_uint32 value);

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
L7_RC_t wdmClientRogueClassificationLastDetectedGet(L7_enetMacAddr_t macAddr,
                           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId,
                           L7_uint32 *value);

/*********************************************************************
* @purpose  Set the WIDS Client Rogue last detected
*
* @param    L7_enetMacAddr_t       client mac address
*           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t   testId  
*                   @b{(input)} rogue classification test identifier
*           L7_uint32 value  @b{(input)} last detected
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmClientRogueClassificationLastDetectedSet(L7_enetMacAddr_t macAddr,
                           L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t testId,
                           L7_uint32 value);



/* Client Security Configuration APIs */
/*********************************************************************
*
* @purpose  Set the Detected Rogue Trap Interval Configuration Status.
*
* @param    L7_uint32 value @b{(input)} value [0 - 3600]
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientDetectedRogueTrapIntervalSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the Detected Rogue Trap Interval Configuration Status.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the interval.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientDetectedRogueTrapIntervalGet(L7_uint32 *value);

/*****************************************************************************
*
* @purpose  Set the Client's OUI present in OUI List test Configuration Status.
*
* @param    L7_uint32 value @b{(input)} value L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @end
*
******************************************************************************/
L7_RC_t wdmWidsClientNotInOUIListTestSet(L7_uint32 value);

/******************************************************************************
*
* @purpose  Get the Client's OUI present in OUI List test Configuration Status.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @end
*
*******************************************************************************/
L7_RC_t wdmWidsClientNotInOUIListTestGet(L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the Client present in Known Database test Configuration Status.
*
* @param    L7_uint32 value @b{(input)} value L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientNotInKnownDBTestSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the Client present in Known Database test Configuration Status.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientNotInKnownDBTestGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the configuration status for the configured rate of
*           802.11 Authentication Requests test.
*
* @param    L7_uint32 value @b{(input)} value L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientConfiguredAuthRateSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the configuration status for the configured rate of
*           802.11 Authentication Requests test.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientConfiguredAuthRateGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the configuration status for the configured rate of
*           802.11 Probe Requests test.
*
* @param    L7_uint32 value @b{(input)} value L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientConfiguredProbeRateSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the configuration status for the configured rate of
*           802.11 Probe Requests test.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientConfiguredProbeRateGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the configuration status for the configured rate of
*           802.11 De-Authentication Requests test.
*
* @param    L7_uint32 value @b{(input)} value L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientConfiguredDeAuthRateSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the configuration status for the configured rate of
*           802.11 De-Authentication Requests test.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientConfiguredDeAuthRateGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the configuration status for the Maximum number of 
*           Authentication Failures test
*
* @param    L7_uint32 value @b{(input)} value L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientMaxAuthFailureSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the configuration status for the Maximum number of 
*           Authentication Failures test
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientMaxAuthFailureGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the configuration status for the Known Client is 
*           authenticated with unknown AP test 
*
* @param    L7_uint32 value @b{(input)} value L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientAuthWithUnknownAPSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the configuration status for the Known Client is 
*           authenticated with unknown AP test 
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientAuthWithUnknownAPGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the Client Security Threat Mitigation Configuration Status.
*
* @param    L7_uint32 value @b{(input)} value, L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientThreatMitigationSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the Client Security Threat Mitigation Configuration Status.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientThreatMitigationGet(L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the Number of seconds for counting the de-authentication 
*           messages.
*
* @param    L7_uint32 value @b{(input)} value [1 - 3600]
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientDeAuthThresholdIntervalSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the Number of seconds for counting the de-authentication 
*           messages.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the interval.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientDeAuthThresholdIntervalGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the DeAuth message threshold value. If switch receives
*           more deauth messages than this threshold value during the
*           threshold interval the test triggers.
*
* @param    L7_uint32 value @b{(input)} value [1 - 99999]
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientDeAuthThresholdValueSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Set the DeAuth message threshold value.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientDeAuthThresholdValueGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the Number of seconds for counting the authentication 
*           messages.
*
* @param    L7_uint32 value @b{(input)} value [1 - 3600]
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientAuthThresholdIntervalSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the Number of seconds for counting the authentication 
*           messages.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the interval.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientAuthThresholdIntervalGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the Auth message threshold value. If switch receives
*           more Auth messages than this threshold value during the
*           threshold interval the test triggers.
*
* @param    L7_uint32 value @b{(input)} value [1 - 99999]
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientAuthThresholdValueSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Set the Auth message threshold value.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientAuthThresholdValueGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the Number of seconds for counting the probe messages.
*
* @param    L7_uint32 value @b{(input)} value [1 - 3600]
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientProbeThresholdIntervalSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the Number of seconds for counting the Probe messages.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the interval.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientProbeThresholdIntervalGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the Probe message threshold value. If switch receives
*           more Probe messages than this threshold value during the
*           threshold interval the test triggers.
*
* @param    L7_uint32 value @b{(input)} value [1 - 99999]
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientProbeThresholdValueSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the Probe message threshold value.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientProbeThresholdValueGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the Authentication failures threshold value. This is
*           the number of 802.1X authentication failures that triggers 
*           the client to be reported as rogue. 
*
* @param    L7_uint32 value @b{(input)} value [1 - 99999]
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientAuthFailureThresholdValueSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the Authentication Failure threshold value.
*
* @param    L7_uint32   *value      @b{(output)} pointer to store the value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t wdmWidsClientAuthFailureThresholdValueGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the location(Local/Radius) where the Known Client
*           database is defined for Detected Client database.
*
* @param    L7_uint32 value @b{(input)} value - Local/Radius
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientKnownDBLocationSet(L7_uint32 value);

/*********************************************************************
*
* @purpose  Get the location where Known Client database is defined for
*           Detected Client database.
*
* @param    L7_uint32   *value  @b{(output)} pointer to store the value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientKnownDBLocationGet (L7_uint32 *value);

/*********************************************************************
*
* @purpose  Set the name of the Radius server where the Known Client
*           database is defined for Detected Client database.
*
* @param    L7_char8 *name       @b{(input)} pointer storing the name.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientKnownDBRadiusServerNameSet(L7_char8 *name);

/*********************************************************************
*
* @purpose  Get the name of the Radius server where the Known Client
*           database is defined for Detected Client database.
*
* @param    L7_char8 *name       @b{(output)} pointer storing the name.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientKnownDBRadiusServerNameGet(L7_char8 *name);

/*********************************************************************
*
* @purpose  Get the status of the Radius server where the Known Client
*           database is defined for Detected Client database.
*
* @param    L7_char8 *name       @b{(input)} pointer storing the name.
* @param    L7_uint32   *value   @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmWidsClientKnownDBRadiusServerStatusGet(L7_char8 *name, L7_uint32   *value);

/* Client Security Configuration APIs - End */

/*********************************************************************
* @purpose  Check if the client is a known client
*
* @param    L7_BOOL    entryFound
* @param    L7_WDM_KNOWN_CLIENT_ACTION_t    authAction
* @param    L7_BOOL    allowClient
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This check returns L7_FALSE in allowClient if the client is a rogue
*           and allowClient will be L7_TRUE if the client is not a rogue.
*
* @end
*********************************************************************/
L7_RC_t wdmWidsClientAllowClientCheck(L7_BOOL entryFound,
                                      L7_WDM_KNOWN_CLIENT_ACTION_t authAction,
                                      L7_BOOL *allowClient);

/*********************************************************************
* @purpose  Run the Known-client database test on the client
*
* @param    L7_enetMacAddr_t       client mac address
* @param    L7_uint32 *result @b{(output)} test result
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This test returns L7_FALSE in allowClient if the client is a rogue
*           and allowClient will be L7_TRUE if the client is not a rogue.
*
* @end
*********************************************************************/
L7_RC_t wdmWidsClientNotInKnownClientListTest(L7_enetMacAddr_t clientMac,
                                              L7_BOOL *allowClient,
                                              L7_BOOL *clientInKnownDb);

/*********************************************************************
* @purpose  Run the OUI database test on the client
*
* @param    L7_enetMacAddr_t       client mac address
* @param    L7_uint32 *result @b{(output)} test result
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This test returns L7_FAILURE if the client's OUI is found in
*           OUI database, otherwise it returns L7_SUCCESS.
*
* @end
*********************************************************************/
L7_RC_t wdmWidsClientNotInOuiListTest(L7_enetMacAddr_t clientMac);

/*********************************************************************
* @purpose  Run the exceeds max failing auth msgs threshold test
*
* @param    L7_enetMacAddr_t       client mac address
* @param    L7_uint32 *result @b{(output)} test result
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This test returns L7_TRUE in detected if the client is a rogue
*           and detected will be L7_FALSE if the client is not a rogue.
*
* @end
*********************************************************************/
L7_RC_t wdmWidsIsClientExceedingMaxAuthFailureTest(L7_enetMacAddr_t clientMac,
                                              L7_BOOL *detected);

/*********************************************************************
 * @purpose  Insert a test result into the threat results structure
 *
 * @returns  L7_SUCCESS, L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t wdmDetectedClientRogueTestResultInsert(
                               wdmWidsClientRogueClassificationTestResults_t *result,
                               L7_WIDS_CLIENT_ROGUE_CLASSIFICATION_TEST_t   testId,
                               L7_BOOL                                detected,
                               L7_enetMacAddr_t                       apMac,
                               L7_uchar8                              radioIf,
                               L7_uint32                              cTime);


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
L7_RC_t wdmDetectedClientDatabasePresent();

/*********************************************************************
* @purpose  Update the detected client entry with Authentication parameters
*           only
*
* @param    macAddr       @b{(input)} client MAC address
* @param    clientStatus  @b{(input)} client Status
* @param    threatDetected  @b{(input)} Threat Detected or not
* @param    userName     @b{(input)}  User Name
* @param    bssid        @b{(input)}  BSSID
* @param    ssid         @b{(input)}  SSID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientEntryUpdateAuthParams(L7_enetMacAddr_t client,
    L7_WDM_DETECTED_CLIENT_STATUS_t clientStatus, 
    L7_WDM_CLIENT_THREAT_DETECTION_STATUS_t threatDetected, L7_uchar8 *userName,
    L7_enetMacAddr_t bssid, L7_uchar8 *ssid,
    wdmWidsClientRogueClassificationTestResults_t *result, L7_uchar8 channel);

/*********************************************************************
 * @purpose  Update the detected client entry with Authentication Failure
 *           report parameters only
 *
 * @param    macAddr      @b{(input)}  client MAC address
 * @param    clientStatus @b{(input)}  Client Status
 * @param    threatDetected  @b{(input)} Threat Detected or not
 * @param    nickName     @b{(input)}  Client Name
 * @param    bssid        @b{(input)}  BSSID
 * @param    ssid         @b{(input)}  SSID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments
 *
 * @end
 *
 *********************************************************************/
L7_RC_t wdmDetectedClientEntryUpdateAuthFailureParams(L7_enetMacAddr_t client,
    L7_WDM_DETECTED_CLIENT_STATUS_t  clientStatus, 
    L7_WDM_CLIENT_THREAT_DETECTION_STATUS_t threatDetected, L7_uchar8 *nickName,
    L7_enetMacAddr_t bssid, L7_uchar8 *ssid);

/*********************************************************************
* @purpose  Update the detected client entry with MAC Authentication failure
*           parameters only
*
* @param    macAddr      @b{(input)}  client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientEntryUpdateMacAuthFailureParams(L7_enetMacAddr_t client);

/*********************************************************************
* @purpose  Update the detected client entry with Dis Assoc parameters only
*
* @param    macAddr      @b{(input)}  client MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientEntryUpdateDisAssocParams(L7_enetMacAddr_t client);

/*********************************************************************
* @purpose  Add a client to the Known Client Database.
*
* @param    clientMacAddr                 @b{(input)} client MAC address
* @param    L7_WDM_KNOWN_CLIENT_ACTION_t  @b{(input)} client MAC auth action
* @param    L7_char8                      @b{(input)} client nick name
*
* @returns  wsKnownClientEntryData_t      pointer to known client entry*
* @notes    none
*
* @end
*********************************************************************/
wsKnownClientEntryData_t *wirelessKnownClientEntryAdd(L7_enetMacAddr_t clientMacAddr,
                                                      L7_WDM_KNOWN_CLIENT_ACTION_t authAction,
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
L7_RC_t wirelessKnownClientEntryDelete(L7_enetMacAddr_t clientMacAddr);


/*********************************************************************
* @purpose  Delete all client entries from the Known Client Database.
*
* @returns  
*
* @notes    none
*
* @end
*********************************************************************/
void wirelessKnownClientEntryDeleteAll(void);


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
wsKnownClientEntryData_t *wirelessKnownClientEntryLookup(L7_enetMacAddr_t clientMacAddr);


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
L7_RC_t wirelessKnownClientEntryGet (L7_enetMacAddr_t clientMacAddr);


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
wsKnownClientEntryData_t *wirelessKnownClientEntryNextGet(wsKnownClientEntryData_t *clientEntry);


/*********************************************************************
*
* @purpose Set the Known Client entry MAC authentication action.
*
* @param    clientMacAddr                 @b{(input)} client MAC address
* @param    L7_WDM_KNOWN_CLIENT_ACTION_t  @b{(input)} client MAC auth action
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wirelessKnownClientActionSet(L7_enetMacAddr_t clientMacAddr, 
                                     L7_WDM_KNOWN_CLIENT_ACTION_t authAction);


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
L7_RC_t wirelessKnownClientNameSet(L7_enetMacAddr_t clientMacAddr, 
                                   L7_char8 *nickName);


/*********************************************************************
*
* @purpose Get the Known Client entry MAC authentication action.
*
* @param    clientMacAddr                 @b{(input)} client MAC address
* @param    L7_WDM_KNOWN_CLIENT_ACTION_t  @b{(input)} client MAC auth action

*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wirelessKnownClientActionGet(L7_enetMacAddr_t clientMacAddr, 
                                     L7_WDM_KNOWN_CLIENT_ACTION_t *authAction);


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
L7_RC_t wirelessKnownClientNameGet(L7_enetMacAddr_t clientMacAddr, 
                                   L7_char8 *nickName);

/*********************************************************************
 * @purpose  Get the count of the Known Client entries 
 *
 * @param    count      @b{(count)} count of entries
 *
 * @returns  L7_SUCCESS
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t wirelessKnownClientEntryCountGet(L7_ushort16 *count);

/*********************************************************************
*
* @purpose  Add Client MAC to the Known Client Config Table
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)} Client MAC Address
* @param    L7_WDM_KNOWN_CLIENT_ACTION_t  @b{(input)} client MAC auth action
* @param    L7_char8                      @b{(input)} client nick name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_TABLE_IS_FULL
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmKnownClientCfgAdd (L7_enetMacAddr_t macAddr,
                              L7_WDM_KNOWN_CLIENT_ACTION_t authAction,
                              L7_char8 *nickName);

/*********************************************************************
*
* @purpose  Delete Client MAC from the Known Client Config Table
*
* @param    L7_int32 index @b{(input)} Index into Client Authentication Table
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmKnownClientCfgDelete(L7_uint32 index);

/*********************************************************************
*
* @purpose Initialize the local Known Client Database.
*
* @retruns void
*
* @comments
*
* @end
*
*********************************************************************/
void wdmKnownClientDBInit(void);

/*********************************************************************
 * @purpose  This function is used to retrieve an entry from the 
 *           cached Known Client Radius database
 *
 * @param    macAddr            @b{(input)}  key1 to index the cached Radius database
 *           radiusServerName   @b{(input)}  key2 to index the cached Radius database
 *           inRadius           @b{(output)} indicate whether the entry is Radius Server
 *           attrib             @b{(output)} vendor specified Radius Attributes
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 * @returns  L7_NOT_EXIST
 *
 * @notes     
 *
 * @end
 *********************************************************************/
L7_RC_t knownClientCachedRadiusDbEntryGet(L7_enetMacAddr_t macAddr, L7_char8 *radiusServerName, 
                                          L7_BOOL *inRadius, knownClientRadiusAttributes_t *attrib);

/*********************************************************************
 * @purpose  This function is used to add an entry to the switch cached
 *           known client Radius database
 *
 * @param    macAddr            @b{(input)}  key1 to index the cached Radius database
 *           radiusServerName   @b{(input)}  key2 to index the cached Radius database
 *           inRadius           @b{(input)} indicate whether the entry is Radius Server
 *           attrib             @b{(input)} vendor specified Radius Attributes
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    This function is called to  build the RADIUS Database Cache in the
 *           wireless switch 
 *
 * @end
 *********************************************************************/
L7_RC_t knownClientCachedRadiusDbEntryAdd(L7_enetMacAddr_t macAddr, L7_char8 *radiusServerName, 
                                          L7_BOOL inRadius, knownClientRadiusAttributes_t attrib);

/*********************************************************************
 * @purpose  Start the procedure to validate the Known Client Entry 
 *           from the RADIUS Server
 *
 * @param    macAddr            @b{(input)}  key1 to index the cached Radius database
 *           radiusServerName   @b{(input)}  key2 to index the cached Radius database
 *
 * @returns  L7_SUCCESS - If no errors.
 * @returns  L7_FAILURE - If any errors.
 *
 * @notes    This function is invoked when the client entry is not present in
 *           the Switch Cached Known Client Radius Database.
 *           If the validation is successful, the entry is added into the
 *           Switch Cached Known Client Radius Database
 *
 * @end
 *********************************************************************/
L7_RC_t knownClientValidationStart(L7_enetMacAddr_t macAddr, L7_char8 *radiusServerName);

/*********************************************************************
*
* @purpose MAC authentication via local Known Client Database.
*
* @param    clientMacAddr                 @b{(input)} client MAC address
* @param    L7_uint32                     @b{(output)} client status flags
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wirelessMACAuthLocal(L7_enetMacAddr_t clientMacAddr, 
                             L7_uint32 *clientReplyFlags);

/*********************************************************************
*
* @purpose MAC authentication via Radius Known Client Database.
*
* @param    clientMacAddr             @b{(input)} client MAC address
* @param    radiusServerName          @b{(input)} Radius server name 
* @param    clientReplyFlags          @b{(output)} client status flags
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wirelessMACAuthRadius(L7_enetMacAddr_t clientMacAddr, 
                              L7_char8 *radiusServerName,
                              L7_uint32 *clientReplyFlags);

/*********************************************************************
*
* @purpose  To add the pre-auth event into pre-auth history.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} client mac address
* @param    L7_enetMacAddr_t  apMacAddr   @b{(input)} AP mac address
* @param    L7_uint8          radioIf     @b{(input)} radio interface no.
* @param    L7_enetMacAddr_t  vapMacAddr  @b{(input)} VAP mac address
* @param    L7_char8          *ssid       @b{(input)} pointer to the ssid string
* @param    L7_char8          *clientName @b{(input)} pointer to the user name
* @param    L7_WDM_CLIENT_AUTHENTICATION_STATUS_t status
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
L7_RC_t wdmDetectedClientPreAuthHistoryEntryAdd(
                     L7_enetMacAddr_t macAddr, L7_enetMacAddr_t apMacAddr, 
                     L7_uint8 radioIf, L7_enetMacAddr_t vapMacAddr, 
                     L7_char8 *ssid,  L7_char8 *clientName, 
                     L7_WDM_CLIENT_AUTHENTICATION_STATUS_t status);

/*********************************************************************
*
* @purpose  Verify if an event entry for specified client with given time 
*           stamp exist in the the detected client preAuth history.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_uint32         index     @b{(input)} index value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientPreAuthHistoryEntryGet(L7_enetMacAddr_t macAddr, 
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
L7_RC_t wdmDetectedClientPreAuthHistoryEntryGetNext(L7_enetMacAddr_t macAddr, 
                     L7_uint32 index, L7_enetMacAddr_t *nextMac, 
                     L7_uint32 *nextIndex);

/*********************************************************************
*
* @purpose  Get AP mac address and radioIf from the event record, to which
*           the specified client has pre-authenticated.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_uint32         index      @b{(input)} Time Stamp
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientPreAuthHistoryEntryAPRadioGet(
                     L7_enetMacAddr_t macAddr, 
                     L7_uint32 index,L7_enetMacAddr_t *apMac, 
                     L7_uint8 *radioIf );

/*********************************************************************
*
* @purpose  Get the SSID from the event record, to which
*           the specified client has pre-authenticated.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_uint32         index     @b{(input)} index value.
* @param    L7_char8          *ssid      @b{(input)} pointer to ssid.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientPreAuthHistoryEntrySSIDGet(
                     L7_enetMacAddr_t macAddr, 
                     L7_uint32 index,L7_char8 *ssid);

/*********************************************************************
*
* @purpose  Get the User Name from the event record, to which
*           the specified client has pre-authenticated.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_uint32         index      @b{(input)} index value of the entry
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
L7_RC_t wdmDetectedClientPreAuthHistoryEntryUserNameGet(
                     L7_enetMacAddr_t macAddr,
                     L7_uint32 index,L7_char8 *name);

/*********************************************************************
*
* @purpose  Get the Time since the pre-auth event occured for the specified 
*           client.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_uint32         index      @b{(input)} Time Stamp
* @param    L7_uint32         *timeVal  @b{(input)} pointer to timeVal.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientPreAuthHistoryEntryTimeGet(
                     L7_enetMacAddr_t macAddr, 
                     L7_uint32 index,L7_uint32 *timeVal);


/*********************************************************************
*
* @purpose  Get the auth. status result of the pre-auth event occured for
*           the specified client.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_uint32         index     @b{(input)} index value.
* @param    L7_WDM_CLIENT_AUTHENTICATION_STATUS_t *status
*                                       @b{(input)} pointer to status value.

* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientPreAuthHistoryEntryStatusGet(
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
L7_RC_t wdmDetectedClientPreAuthHistoryPurge(L7_enetMacAddr_t macAddr);


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
L7_RC_t wdmDetectedClientPreAuthHistoryPurgeAll(void);

/*********************************************************************
*
* @purpose  Delete the pre-auth history entry recorded for specified 
*           client and event occured time.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_uint32         index     @b{(input)} index value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientPreAuthHistoryEntryDelete(L7_enetMacAddr_t macAddr, 
                     L7_uint32 index);

/*********************************************************************
*
* @purpose  Delete all the pre-auth history entries recorded for 
*           specified client.
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
L7_RC_t wdmDetectedClientPreAuthHistoryDeleteAll(L7_enetMacAddr_t macAddr);


/*********************************************************************
*
* @purpose  Get the pre-auth record from the the preAuth history
*           for specified client with given time stamp 
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_uint32         index     @b{(input)} index value.
* @param    L7_uint32         matchType @b{(input)} search type 
* @param    L7_uint32         **pData   @b{(input)} double pointer for AVL entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientPreAuthHistoryEntryFind(L7_enetMacAddr_t macAddr,
                     L7_uint32 index, 
                     L7_uint32 matchType,
                     wdmDetectedClientPreAuthStatus_t  **pData);

/*********************************************************************
* @purpose  Determine if a client exists in the detected client list
*
* @param    L7_enetMacAddr_t macAddr           @b{(input)}  client MAC address
* @param    L7_uint32 matchType                @b{(input)}  match type.
* @param    wdmDetectedClientStatus_t  **pData @b{(output)}  double pointer to 
*                                                        detected client entry.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientEntryFind(L7_enetMacAddr_t macAddr, 
                     L7_uint32 matchType,
                     wdmDetectedClientStatus_t  **pData);

/*********************************************************************
*
* @purpose  Get VAP mac address from the event record, to which
*           the specified client has pre-authenticated.
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
L7_RC_t wdmDetectedClientPreAuthHistoryEntryVAPGet(
                     L7_enetMacAddr_t macAddr, 
                     L7_uint32 index,L7_enetMacAddr_t *vapMac);

/*********************************************************************
*
* @purpose  Get the max. allowed and accumulated depth of pre-auth history 
*           for the specified detected client.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
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
L7_RC_t wdmDetectedClientPreAuthHistoryDepthGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t wdmDetectedClientPreAuthHistoryGlobalDepthGet(L7_uint32 *maxVal, 
                     L7_uint32 *count);

/*********************************************************************
*
* @purpose  To add/update an event into Roam history.
*
* @param    L7_enetMacAddr_t  macAddr     @b{(input)} client mac address
* @param    L7_enetMacAddr_t  apMacAddr   @b{(input)} AP mac address
* @param    L7_uint8         radioIf     @b{(input)} radio interface no.
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
L7_RC_t wdmDetectedClientRoamHistoryEntryAdd(
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
* @param    L7_uint32         index     @b{(input)} index value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientRoamHistoryEntryGet(L7_enetMacAddr_t macAddr, 
                     L7_uint32 index);

/*********************************************************************
*
* @purpose  Get next Roam event record(key) for specified client with 
*           vap mac.
*
* @param   L7_enetMacAddr_t macAddr     @b{(input)} client mac address
* @param   L7_uint32        index       @b{(input)} index value.
* @param   L7_enetMacAddr_t *nextMac    @b{(input)} pointer to client mac add.
* @param   L7_uint32        *nextIndex  @b{(output)} pointer to next index.
*
* @returns L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientRoamHistoryEntryGetNext(L7_enetMacAddr_t macAddr, 
                     L7_uint32 index, L7_enetMacAddr_t *nextMac, 
                     L7_uint32 *nextIndex);

/*********************************************************************
*
* @purpose  Get AP mac address and radioIf from Roaming event record, 
*           to which the specified client has Roamed.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_uint32         index     @b{(input)} index value.
* @param    L7_enetMacAddr_t  *apMac    @b{(input)} pointer to ap mac.
* @param    L7_uint32         *radioIf  @b{(input)} pointer to radio no.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientRoamHistoryEntryAPRadioGet(
                     L7_enetMacAddr_t macAddr, 
                     L7_uint32 index,L7_enetMacAddr_t *apMac, 
                     L7_uint8 *radioIf );

/*********************************************************************
*
* @purpose  Get the SSID from Roaming event record, to which
*           the specified client has Roamed.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_uint32         index     @b{(input)} index value.
* @param    L7_char8          *ssid      @b{(input)} pointer to ssid.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientRoamHistoryEntrySSIDGet(
                     L7_enetMacAddr_t macAddr, 
                     L7_uint32 index,L7_char8 *ssid);

/*********************************************************************
*
* @purpose  Get VAP mac address from the event record, to which
*           the specified client has pre-authenticated.
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
L7_RC_t wdmDetectedClientRoamHistoryEntryVAPGet(
                     L7_enetMacAddr_t macAddr, 
                     L7_uint32 index,L7_enetMacAddr_t *vapMac);


/*********************************************************************
*
* @purpose  Get the Time since the Roam event occured for the specified 
*           client with given mac add.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_uint32         index     @b{(input)} index value.
* @param    L7_uint32         *timeVal  @b{(input)} pointer to time.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientRoamHistoryEntryTimeGet(
                     L7_enetMacAddr_t macAddr, 
                     L7_uint32 index,L7_uint32 *time);

/*********************************************************************
*
* @purpose  Get the auth. status result of the Roam event occured for
*           the specified client.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_uint32         index     @b{(input)} index value.
* @param    L7_WDM_CLIENT_AUTHENTICATION_STATUS_t *status
*                                       @b{(input)} pointer to status value.

* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientRoamHistoryEntryStatusGet(
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
L7_RC_t wdmDetectedClientRoamHistoryPurge(L7_enetMacAddr_t macAddr);

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
L7_RC_t wdmDetectedClientRoamHistoryPurgeAll(void);

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
L7_RC_t wdmDetectedClientRoamHistoryEntryDelete(L7_enetMacAddr_t macAddr, 
                     L7_uint32 index);

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
L7_RC_t wdmDetectedClientRoamHistoryDeleteAll(L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Get the event record from the the Roam history
*           for specified client with given vap mac add.
*
* @param    L7_enetMacAddr_t  macAddr   @b{(input)} client mac address
* @param    L7_uint32         index     @b{(input)} index value.
* @param    L7_uint32         matchType @b{(input)} search type 
* @param    L7_uint32         **pData   @b{(input)} double pointer for AVL entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientRoamHistoryEntryFind(L7_enetMacAddr_t macAddr,
                     L7_uint32 index, 
                     L7_uint32 matchType,
                     wdmDetectedClientRoamHistoryStatus_t  **pData);

/*********************************************************************
*
* @purpose  Get the max. and accumulated depth of Roam history 
*           for the specified detected client.
*
* @param    L7_enetMacAddr_t macAddr      @b{(input)}  client MAC address
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
L7_RC_t wdmDetectedClientRoamHistoryDepthGet(L7_enetMacAddr_t macAddr,
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
L7_RC_t wdmDetectedClientRoamHistoryGlobalDepthGet(L7_uint32 *maxVal, 
                     L7_uint32 *count);

/*********************************************************************
*
* @purpose To add/replace an entry in detected client triangulation status.
*
* @param  macAddr        @b{(input)} client MAC address
* @param  wdmDetectedClientTriangulationEntry_t entry
*                        @b{(input)} triangulation entry.
* @param  L7_uint8 sentryMode  @b{(input)} reporting AP's functionality 
                                             wrt sentry mode.(enable/disable)
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments  Addition: Input entry is inserted if there is a vacant slot.
*            Replacement of existing entry:
*            If the RSSI value of input entry is larger than that of any 
*            of the three entries of corresponding type(sentry/Non-sentry),
*            
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientTriangulationEntryAddReplace(L7_enetMacAddr_t macAddr, 
                    wdmDetectedClientTriangulationEntry_t entry, 
                    L7_uint32  sentryMode);

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
L7_RC_t wdmDetectedClientTriangulationEntryGet(L7_enetMacAddr_t macAddr, 
                     wdmDetectedClientTriangulationStatus_t *status);

/*********************************************************************
*
* @purpose Get sentry signal triangulation information of the specified
*          detected client.
*
* @param  L7_enetMacAddr_t macAddr @b{(input)} detected client mac addr.
* @param  wdmDetectedClientTriangulationStatus_t *status
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
L7_RC_t wdmDetectedClientTriangulationSentryEntryGet(L7_enetMacAddr_t macAddr,
                     wdmDetectedClientTriangulationEntry_t *status);

/*********************************************************************
*
* @purpose Get Non Sentry signal triangulation information of the specified
*          detected client.
*
* @param  L7_enetMacAddr_t macAddr @b{(input)} detected client mac addr.
* @param  wdmDetectedClientTriangulationStatus_t *status
*                        @b{(output)} pointer to store non sentry
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
L7_RC_t wdmDetectedClientTriangulationNonSentryEntryGet(L7_enetMacAddr_t macAddr,
                     wdmDetectedClientTriangulationEntry_t *status);


/*********************************************************************
* @purpose  To flushout triangulation status for the specified detected client.
*
* @param    wdmDetectedClientStatus_t  *pData @b{(input)}  pointer to 
*                                                        detected client entry.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientTriangulationStatusReset(wdmDetectedClientStatus_t 
                     *pData);
/*********************************************************************
*
* @purpose To fill the specified slot in triangulation entries, with 
*          given signal information.
*
* @param  wdmDetectedClientTriangulationEntry_t *entries
*                        @b{(input)} pointer to the target slot.
* @param  wdmDetectedClientTriangulationEntry_t entry
*                        @b{(input)} triangulation data.
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments  None.
*            
*
* @end
*
*********************************************************************/
void wdmDetectedClientTriangulationEntryFill(
                     wdmDetectedClientTriangulationEntry_t *entries,
                     wdmDetectedClientTriangulationEntry_t entry);

/*********************************************************************
* @purpose  Get the detected client's Organizationally Unique Identifier
*
* @param    L7_enetMacAddr_t macAddr   @b{(input)}  client MAC address
*           L7_uint32 *value           @b{(output)} pointer to store the status.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t wdmDetectedClientOUIGet(L7_enetMacAddr_t macAddr,
                                 L7_char8 *oui);

#endif /* INCLUDE_WDM_CLIENT_SECURITY_API_H */
