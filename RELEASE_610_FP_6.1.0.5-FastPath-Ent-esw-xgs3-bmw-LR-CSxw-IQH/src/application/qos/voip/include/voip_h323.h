/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip_h323.h
*
* @purpose H323 protocol definations
*
* @component VOIP | H323
*
* @comments
*
* @create 05/12/2007
*
* @author aprashant
* @end
*
**********************************************************************/
#ifndef VOIP_H323_H
#define VOIP_H323_H

#include "voip_parse.h"

typedef struct transportAddrIpAddr_s
{          		                     /* SEQUENCE */ 
  L7_uint32 options;    		     /* No use */
  L7_uint32 ip;
} transportAddrIpAddr_t;

typedef enum
{
  transportAddr_ipAddr,
  transportAddr_ipSourceRoute,
  transportAddr_ipxAddr,
  transportAddr_ip6Addr,
  transportAddr_netBios,
  transportAddr_nsap,
  transportAddr_nonStandardAddr,
}transportAddrType_t;

typedef struct transportAddr_s 
{ 				 	     /* CHOICE */
  transportAddrType_t choice;
  transportAddrIpAddr_t ipAddr;
} transportAddr_t;


typedef enum
{
  dataProtocolCapability_nonStandard,
  dataProtocolCapability_v14buffered,
  dataProtocolCapability_v42lapm,
  dataProtocolCapability_hdlcFrameTunnelling,
  dataProtocolCapability_h310SeparateVCStack,
  dataProtocolCapability_h310SingleVCStack,
  dataProtocolCapability_transparent,
  dataProtocolCapability_segmentationAndReassembly,
  dataProtocolCapability_hdlcFrameTunnelingwSAR,
  dataProtocolCapability_v120,
  dataProtocolCapability_separateLANStack,
  dataProtocolCapability_v76wCompression,
  dataProtocolCapability_tcp,
  dataProtocolCapability_udp,
}dataProtocolCapabilityType_t;

typedef struct dataProtocolCapability_s 
{						 /* CHOICE */
  dataProtocolCapabilityType_t choice;
} dataProtocolCapability_t;

typedef enum
{
  dataApplnCapabilityAppln_nonStandard,
  dataApplnCapabilityAppln_t120,
  dataApplnCapabilityAppln_dsm_cc,
  dataApplnCapabilityAppln_userData,
  dataApplnCapabilityAppln_t84,
  dataApplnCapabilityAppln_t434,
  dataApplnCapabilityAppln_h224,
  dataApplnCapabilityAppln_nlpid,
  dataApplnCapabilityAppln_dsvdControl,
  dataApplnCapabilityAppln_h222DataPartitioning,
  dataApplnCapabilityAppln_t30fax,
  dataApplnCapabilityAppln_t140,
  dataApplnCapabilityAppln_t38fax,
  dataApplnCapabilityAppln_genericDataCapability,
}dataApplnCapabilityApplntype_t;

typedef struct dataApplnCapabilityAppln_s
{
  dataApplnCapabilityApplntype_t choice;
  dataProtocolCapability_t t120;
} dataApplnCapabilityAppln_t;

typedef struct dataApplnCapability_s
{					     	          /* SEQUENCE */
  L7_uint32 options;          				  /* No use */
  dataApplnCapabilityAppln_t application;
} dataApplnCapability_t;

typedef enum 
{
  dataType_nonStandard,
  dataType_nullData,
  dataType_videoData,
  dataType_audioData,
  dataType_data,
  dataType_encryptionData,
  dataType_h235Control,
  dataType_h235Media,
  dataType_multiplexedStream,   
}dataTypeType_t;

typedef struct dataType_s 
{ 						      /* CHOICE */
  dataTypeType_t choice;
  dataApplnCapability_t data;
} dataType_t;

typedef struct unicastAddrIPAddr_s
{ 						      /* SEQUENCE */
  L7_uint32 options;       		              /* No use */
  L7_uint32 network;
} unicastAddrIPAddr_t;

typedef enum
{
  unicastAddr_ipAddr,
  unicastAddr_ipXAddr,
  unicastAddr_ip6Addr,
  unicastAddr_netBios,
  unicastAddr_iPSourceRouteAddr,
  unicastAddr_nsap,
  unicastAddr_nonStandardAddr,
}unicastAddrType_t;

typedef struct unicastAddr_s 
{ 							/* CHOICE */
  unicastAddrType_t choice;
  unicastAddrIPAddr_t ipAddr;
}unicastAddr_t;

typedef enum
{
  transportAddr_unicastAddr,
  transportAddr_multicastAddr  
}H245TransportAddrType_t;

typedef struct H245TransportAddr_s 
{							 /* CHOICE */
  H245TransportAddrType_t choice;
  unicastAddr_t unicastAddr;
} H245TransportAddr_t;

typedef enum
{
  H2250LogParams_nonStandard = (1 << 31),
  H2250LogParams_associatedSessionID = (1 << 30),
  H2250LogParams_mediaChannel = (1 << 29),
  H2250LogParams_mediaGuaranteedDelivery = (1 << 28),
  H2250LogParams_mediaControlChannel = (1 << 27),
  H2250LogParams_mediaControlGuaranteedDelivery   = (1 << 26),
  H2250LogParams_silenceSuppression = (1 << 25),
  H2250LogParams_destination = (1 << 24),
  H2250LogParams_dynamicRTPPayloadType = (1 << 23),
  H2250LogParams_mediaPacketization = (1 << 22),
  H2250LogParams_transportCapability =(1 << 21),
  H2250LogParams_redundancyEncoding = (1 << 20),
  H2250LogParams_source = (1 << 19),
}H2250LogParamsType_t;

typedef struct H2250LogParams_s				/* SEQUENCE */
{
  H2250LogParamsType_t options;
  H245TransportAddr_t mediaChannel;
  H245TransportAddr_t mediaControlChannel;
}H2250LogParams_t;
/* Open logical forwading multiplex parameters */
typedef enum
{
  openLogFwdMpexParams_h222Params,
  openLogFwdMpexParams_h223Params,
  openLogFwdMpexParams_v76Params,
  openLogFwdMpexParams_h2250Params,
  openLogFwdMpexParams_none,
}openLogFwdMpexParamsType_t;

typedef struct openLogFwdMpexParams_s
{
  openLogFwdMpexParamsType_t choice;
  H2250LogParams_t h2250LogParams;
}openLogFwdMpexParams_t;

/* Open Logical channel forwarding logical channel parms */
typedef enum
{
  openLogFwdParams_portNumber = (1 << 31),
  openLogFwdParams_forwardLogicalChannelDependency = (1 << 30),
  openLogFwdParams_replacementFor = (1 << 29),
}openLogFwdParamsType_t;

typedef struct openLogFwdParams_s
{     /* SEQUENCE */
  openLogFwdParamsType_t options;
  dataType_t dataType;
  openLogFwdMpexParams_t multiplexParams;
} openLogFwdParams_t;

/* Open Logical channel Reversing logical channel mulitplex parms */
typedef enum
{
  openLogRevMpexParams_h223Params,
  openLogRevMpexParams_v76Params,
  openLogRevMpexParams_h2250Params,
}openLogRevMpexParamsType_t;

typedef struct openLogRevMpexParams_s
{
  openLogRevMpexParamsType_t choice;
  H2250LogParams_t h2250LogParams;  
}openLogRevMpexParams_t;

/* Open Logical channel Reversing logical channel  params*/
typedef enum
{
  openLogRevParams_multiplexParams = (1 << 31),
  openLogRevParams_reverseLogicalChannelDependency = (1 << 30),
  openLogRevParams_replacementFor = (1 << 29),
}openLogRevParamsType_t;

typedef struct openLogRevParams_s
{
  openLogRevParamsType_t options;
  openLogRevMpexParams_t multiplexParams;
}openLogRevParams_t;

typedef enum 
{  
 networkAccess_q2931Addr,
 networkAccess_e164Addr,
 networkAccss_localAreaAddr,  
}networkAccessAddrType_t;

typedef struct networkAccessAddr_s
{
  networkAccessAddrType_t choice;
  H245TransportAddr_t  localAreaAddr;
}networkAccessAddr_t;

typedef enum 
{
  networkAccessParams_distribution = (1 << 31),
  networkAccessParams_externalReference = (1 << 30),
  networkAccessParams_t120SetupProcedure = (1 << 29),
}networkAccessParamsType_t;

typedef struct networkAccessParams_s
{
  networkAccessParamsType_t options;
  networkAccessAddr_t networkAddr;
}networkAccessParams_t;

typedef enum 
{
  openLogParam_revParams = ( 1 << 31),
  openLogParam_separateStack = (1 << 30),
  openLogParam_encryptionSync = (1 << 29),  
}openLogParamsType_t;

typedef struct openLogicalChannel_s
{
  openLogParamsType_t 	options;
  openLogFwdParams_t  	fwdLogParams;
  openLogRevParams_t  	revLogParams;  
  networkAccessParams_t separateStack;
}openLogicalChannel_t;

typedef struct setupUUIEFastStart_s 
{	   /* SEQUENCE OF */
  L7_uint32 count;
  openLogicalChannel_t item[30];
} setupUUIEFastStart_t;

typedef enum 
{
  setup_h245Addr = (1 << 31),
  setup_sourceAddr = (1 << 30),
  setup_destinationAddr = (1 << 29),
  setup_destCallSignalAddr = (1 << 28),
  setup_destExtraCallInfo = (1 << 27),
  setup_destExtraCRV = (1 << 26),
  setup_callServices = (1 << 25),
  setup_sourceCallSignalAddr = (1 << 24),
  setup_remoteExtensionAddr = (1 << 23),
  setup_callIdentifier = (1 << 22),
  setup_h245SecurityCapability = (1 << 21),
  setup_tokens = (1 << 20),
  setup_cryptoTokens = (1 << 19),
  setup_fastStart = (1 << 18),
  setup_mediaWaitForConnect = (1 << 17),
  setup_canOverlapSend = (1 << 16),
  setup_endpointIdentifier = (1 << 15),
  setup_multipleCalls = (1 << 14),
  setup_maintainConnection = (1 << 13),
  setup_connectionParameters = (1 << 12),
  setup_language = (1 << 11),
  setup_presentationIndicator = (1 << 10),
  setup_screeningIndicator = (1 << 9),
  setup_serviceControl = (1 << 8),
  setup_symmetricOperationRequired = (1 << 7),
  setup_capacity = (1 << 6),
  setup_circuitInfo = (1 << 5),
  setup_desiredProtocols = (1 << 4),
  setup_neededFeatures = (1 << 3),
  setup_desiredFeatures = (1 << 2),
  setup_supportedFeatures = (1 << 1),
  setup_parallelH245Control = (1 << 0),
  
}setupUUIEType_t;

typedef struct setupUUIE_s
{
  setupUUIEType_t options;
  transportAddr_t h245Addr;
  transportAddr_t destCallSignalAddr;
  transportAddr_t sourceCallSignalAddr;
  setupUUIEFastStart_t faststart;   
}setupUUIE_t;

typedef struct callUUIEFastStart_s
{
  L7_uint32 count;
  openLogicalChannel_t item[30];
}callUUIEFastStart_t;

typedef enum
{ 
  call_h245Addr = (1 << 31),
  call_callIdentifier = (1 << 30),
  call_h245SecurityMode = (1 << 29),
  call_tokens = (1 << 28),
  call_cryptoTokens = (1 << 27),
  call_fastStart = (1 << 26),
  call_multipleCalls = (1 << 25),
  call_maintainConnection = (1 << 24),
  call_fastConnectRefused = (1 << 23),
  call_featureSet = (1 << 22),
}callUUIEType_t;

typedef struct callUUIE_s
{
  callUUIEType_t options;
  transportAddr_t h245Addr;
  callUUIEFastStart_t fastStart;
}callUUIE_t;

typedef struct connectUUIEFastStart_s 
{ /* SEQUENCE OF */
  L7_uint32 count;
  openLogicalChannel_t item[30];
} connectUUIEFastStart_t;

typedef enum 
{
  connect_h245Addr = (1 << 31),
  connect_callIdentifier = (1 << 30),
  connect_h245SecurityMode = (1 << 29),
  connect_tokens = (1 << 28),
  connect_cryptoTokens = (1 << 27),
  connect_fastStart = (1 << 26),
  connect_multipleCalls = (1 << 25),
  connect_maintainConnection = (1 << 24),
  connect_language = (1 << 23),
  connect_connectedAddr = (1 << 22),
  connect_presentationIndicator = (1 << 21),
  connect_screeningIndicator = (1 << 20),
  connect_fastConnectRefused = (1 << 19),
  connect_serviceControl = (1 << 18),
  connect_capacity = (1 << 17),
  connect_featureSet = (1 << 16),
}connectUUIEType_t;

typedef struct connectUUIE_s
{
  connectUUIEType_t options;
  transportAddr_t h245Addr;
  connectUUIEFastStart_t fastStart;
  
}connectUUIE_t;

typedef struct alertingUUIEFastStart_s
{
  L7_uint32 count;
  openLogicalChannel_t item[30];
}alertingUUIEFastStart_t;

typedef enum
{
  alerting_h245Addr = (1 << 31),
  alerting_callIdentifier = (1 << 30),
  alerting_h245SecurityMode = (1 << 29),
  alerting_tokens = (1 << 28),
  alerting_cryptoTokens = (1 << 27),
  alerting_fastStart = (1 << 26),
  alerting_multipleCalls = (1 << 25),
  alerting_maintainConnection = (1 << 24),
  alerting_alertingAddr = (1 << 23),
  alerting_presentationIndicator = (1 << 22),
  alerting_screeningIndicator = (1 << 21),
  alerting_fastConnectRefused = (1 << 20),
  alerting_serviceControl = (1 << 19),
  alerting_capacity = (1 << 18),
  alerting_featureSet = (1 << 17),
}alertingUUIEType_t;

typedef struct alertingUUIE_s
{
  alertingUUIEType_t options;
  transportAddr_t h245Addr;
  alertingUUIEFastStart_t fastStart;
}alertingUUIE_t;

typedef struct informationUUIEFastStart_s
{
  L7_uint32 count;
  openLogicalChannel_t item[30];
}informationUUIEFastStart_t;

typedef enum 
{
  information_callIdentifier = (1 << 31),
  information_tokens = (1 << 30),
  information_cryptoTokens = (1 << 29),
  information_fastStart = (1 << 28),
  information_fastConnectRefused = (1 << 27),
  information_circuitInfo = (1 << 26),
}informationUUIEType_t;

typedef struct informationUUIE_s
{
  informationUUIEType_t options;
  informationUUIEFastStart_t fastStart;
}informationUUIE_t;

typedef struct facilityUUIEFastStart_s
{
  L7_uint32 count;
  openLogicalChannel_t item[30];
}facilityUUIEFastStart_t;

typedef enum 
{
   reason_routeCallToGatekeeper,
   reason_callForwarded,
   reason_routeCallToMC,
   reason_undefinedReason,
   reason_conferenceListChoice,
   reason_startH245,
   reason_noH245,
   reason_newTokens,
   reason_featureSetUpdate,
   reason_forwardedElements,
   reason_transportedInformation,
}facilityReasonUUIEType_t;

typedef enum
{
  facility_alternativeAddr = (1 << 31),
  facility_alternativeAliasAddr = (1 << 30),
  facility_conferenceID = (1 << 29),
  facility_callIdentifier = (1 << 28),
  facility_destExtraCallInfo = (1 << 27),
  facility_remoteExtensionAddr = (1 << 26),
  facility_tokens = (1 << 25),
  facility_cryptoTokens = (1 << 24),
  facility_conferences = (1 << 23),
  facility_h245Addr = (1 << 22),
  facility_fastStart = (1 << 21),
  facility_multipleCalls = (1 << 20),
  facility_maintainConnection = (1 << 19),
  facility_fastConnectRefused = (1 << 18),
  facility_serviceControl = (1 << 17),
  facility_circuitInfo = (1 << 16),
  facility_featureSet = (1 << 15),
  facility_destinationInfo = (1 << 14),
  facility_h245SecurityMode = (1 << 13),
}facilityUUIEType_t;

typedef struct facilityUUIE_s
{
  facilityUUIEType_t options;
  facilityReasonUUIEType_t reason;
  transportAddr_t h245Addr;
  facilityUUIEFastStart_t fastStart;
}facilityUUIE_t;

typedef struct progressUUIEFastStart_s
{
  L7_uint32 count;  
  openLogicalChannel_t item[30];
}progressUUIEFastStart_t;

typedef enum
{
  progress_h245Addr = (1 << 31),
  progress_h245SecurityMode = (1 << 30),
  progress_tokens = (1 << 29),
  progress_cryptoTokens = (1 << 28),
  progress_fastStart = (1 << 27),
  progress_multipleCalls = (1 << 26),
  progress_maintainConnection = (1 << 25),
  progress_fastConnectRefused = (1 << 24),
}progressUUIEType_t;

typedef struct progressUUIE_s
{
  progressUUIEType_t options;
  transportAddr_t h245Addr;
  progressUUIEFastStart_t fastStart;
}progressUUIE_t;

typedef enum
{
  h323MessageBody_setup,
  h323MessageBody_callProceeding,
  h323MessageBody_connect,
  h323MessageBody_alerting,
  h323MessageBody_information,
  h323MessageBody_releaseComplete,
  h323MessageBody_facility,
  h323MessageBody_progress,
  h323MessageBody_empty,
  h323MessageBody_status,
  h323MessageBody_statusInquiry,
  h323MessageBody_setupAcknowledge,
  h323MessageBody_notify,
}h323MessageBodyType_t;

typedef struct h323MessageBody_s
{
  h323MessageBodyType_t choice;
  union
  { 
    setupUUIE_t       setup;
    callUUIE_t        call;
    connectUUIE_t     connect;
    alertingUUIE_t    alerting;
    informationUUIE_t information;
    facilityUUIE_t    facility;
    progressUUIE_t    progress;
  }u;
}h323MessageBody_t;

typedef enum
{
  req_nonStandard,
  req_masterSlaveDetermination,
  req_terminalCapabilitySet,
  req_openLogicalChannel,
  req_closeLogicalChannel,
  req_requestChannelClose,
  req_multiplexEntrySend,
  req_requestMultiplexEntry,
  req_requestMode,
  req_roundTripDelayRequest,
  req_maintenanceLoopRequest,
  req_communicationModeRequest,
  req_conferenceRequest,
  req_multilinkRequest,
  req_logicalChannelRateRequest,
}requestMessageType_t;

typedef struct requestMessage_s
{
  requestMessageType_t choice;
  openLogicalChannel_t openLogicalChannel;
}requestMessage_t;

typedef enum
{
  openLogAckRevMpexParams_h222Params,
  openLogAckRevMpexParams_h2250Params,  
}openLogAckRevMpexParamsType_t;

typedef struct openLogAckRevMpexParams_s
{
  openLogAckRevMpexParamsType_t choice;
  H2250LogParams_t h2250LogParams;
    
}openLogAckRevMpexParams_t;

typedef enum
{
  openLogAckRevParams_portNumber = (1 << 31),
  openLogAckRevParams_multiplexParams = (1 << 30),
  openLogAckRevParams_replacementFor = (1 << 29),
}openLogAckRevParamsType_t;

typedef struct openLogAckRevParams_s
{
  openLogAckRevParamsType_t options;
  openLogAckRevMpexParams_t multiplexParams;
}openLogAckRevParams_t;

typedef enum
{
  H2250LogAckParams_nonStandard = (1 << 31),
  H2250LogAckParams_sessionID = (1 << 30),
  H2250LogAckParams_mediaChannel = (1 << 29),
  H2250LogAckParams_mediaControlChannel = (1 << 28),
  H2250LogAckParams_dynamicRTPPayloadType = (1 << 27),
  H2250LogAckParams_flowControlToZero = (1 << 26),
  H2250LogAckParams_portNumber = (1 << 25),
}H2250LogAckParamsType_t;

typedef struct H2250LogAckParams_s
{
  H2250LogAckParamsType_t options;
  H245TransportAddr_t  mediaChannel;
  H245TransportAddr_t  mediaControlChannel;
}H2250LogAckParams_t;

typedef enum 
{
  openLogAckFwdMpexParams_h2250Params,
}openLogAckFwdMpexParamsType_t;

typedef struct openLogAckFwdMpexParams_s
{
  openLogAckFwdMpexParamsType_t choice;
  H2250LogAckParams_t H2250LogAckParams;
}openLogAckFwdMpexParams_t;

typedef enum
{
  openLogAckParams_reverseParams = (1 << 31),
  openLogAckParams_separateStack = (1 << 30),
  openLogAckParams_forwardMpexAckParams = (1 << 29),
  openLogAckParams_encryptionSync = (1 << 28),
}openLogAckParamsType_t;

typedef struct openLogAckParams_s
{
  openLogAckParamsType_t options;
  openLogAckRevParams_t reverseParams;
  openLogAckFwdMpexParams_t fwdMpexParams;
}openLogAckParams_t;

typedef enum 
{
  resp_nonStandard,
  resp_masterSlaveDeterminationAck,
  resp_masterSlaveDeterminationReject,
  resp_terminalCapabilitySetAck,
  resp_terminalCapabilitySetReject,
  resp_openLogicalChannelAck,
  resp_openLogicalChannelReject,
  resp_closeLogicalChannelAck,
  resp_requestChannelCloseAck,
  resp_requestChannelCloseReject,
  resp_multiplexEntrySendAck,
  resp_multiplexEntrySendReject,
  resp_requestMultiplexEntryAck,
  resp_requestMultiplexEntryReject,
  resp_requestModeAck,
  resp_requestModeReject,
  resp_roundTripDelayResponse,
  resp_maintenanceLoopAck,
  resp_maintenanceLoopReject,
  resp_communicationModeResponse,
  resp_conferenceResponse,
  resp_multilinkResponse,
  resp_logicalChannelRateAcknowledge,
  resp_logicalChannelRateReject,
}responseMessageType_t;

typedef struct responseMessage_s
{
  responseMessageType_t choice;
  openLogAckParams_t openLogAckParams;
}responseMessage_t;

typedef enum 
{
  mediaMessage_request,
  mediaMessage_response,
  mediaMessage_command,
  mediaMessage_indication,  
}mediaMessageType_t;

typedef struct mediaMessage_s
{ 
  mediaMessageType_t choice;
  union
  {
     requestMessage_t request;
     responseMessage_t response; 
  }u;
}mediaMessage_t;

typedef struct h323PduH245Control_s
{
  L7_uint32 count;
  mediaMessage_t item[4];  
}h323PduH245Control_t;

typedef enum
{
  h323Pdu_nonStandardData = (1 << 31),
  h323Pdu_h4501SupplementaryService = (1 << 30),
  h323Pdu_h245Tunneling = (1 << 29),
  h323Pdu_h245Control = (1 << 28),
  h323Pdu_nonStandardControl = (1 << 27),
  h323Pdu_callLinkage = (1 << 26),
  h323Pdu_tunnelledSignallingMessage = (1 << 25),
  h323Pdu_provisionalRespToH245Tunneling = (1 << 24),
  h323Pdu_stimulusControl = (1 << 23),
  h323Pdu_genericData = (1 << 22),
}h323PduType_t;

typedef struct h323Pdu_s
{
  h323PduType_t options;
  h323MessageBody_t    h323MessageBody;
  h323PduH245Control_t h245Control;
}h323Pdu_t;

typedef enum
{
  h323UserInfo_user_data = (1 << 31),
}h323UserInfoType_t;

typedef struct h323UserInfo_s
{
  h323UserInfoType_t options;
  h323Pdu_t h323Pdu;
}h323UserInfo_t;

typedef enum
{
  Q931_NationalEscape = 0x00,
  Q931_Alerting = 0x01,
  Q931_CallProceeding = 0x02,
  Q931_Connect = 0x07,
  Q931_ConnectAck = 0x0F,
  Q931_Progress = 0x03,
  Q931_Setup = 0x05,
  Q931_SetupAck = 0x0D,
  Q931_Resume = 0x26,
  Q931_ResumeAck = 0x2E,
  Q931_ResumeReject = 0x22,
  Q931_Suspend = 0x25,
  Q931_SuspendAck = 0x2D,
  Q931_SuspendReject = 0x21,
  Q931_UserInformation = 0x20,
  Q931_Disconnect = 0x45,
  Q931_Release = 0x4D,
  Q931_ReleaseComplete = 0x5A,
  Q931_Restart = 0x46,
  Q931_RestartAck = 0x4E,
  Q931_Segment = 0x60,
  Q931_CongestionCtrl = 0x79,
  Q931_Information = 0x7B,
  Q931_Notify = 0x6E,
  Q931_Status = 0x7D,
  Q931_StatusEnquiry = 0x75,
  Q931_Facility = 0x62
}q931MessageType_t;

typedef struct Q931_s
{
  q931MessageType_t messageType;
  h323UserInfo_t    UUIE;
}Q931_t;


/*****************************************************************************
 * Decode Functions Return Codes
 ****************************************************************************/

#define H323_ERROR_NONE 0       /* Decoded successfully */
#define H323_ERROR_STOP 1       /* Decoding stopped, not really an error */
#define H323_ERROR_BOUND -1
#define H323_ERROR_RANGE -2

void voipH323CallDump();
L7_RC_t  voipH323ClearInterfaceEntries();
/*********************************************************************
* @purpose Parse Q931 Message
*
* @param   const L7_uchar8 *buf      Pointer to packet data
* @param   L7_uint32       length    length of message
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes  Q931 Message is ASN.1 Encoded ,one needs to decode and then
*         Parse the packet
*
* @end
*********************************************************************/
L7_RC_t voipQ931MessageParse(const L7_uchar8 *buf, L7_uint32 length);
/*********************************************************************
* @purpose Parse H245 message
*
* @param   const L7_uchar8 *buf      Pointer to packet data
* @param   L7_uint32       length    length of message
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes   H245 message is ASN.1 encoded and needs to be docoded first
*          before parsing
*
* @end
*********************************************************************/
L7_RC_t voipH245MessageParse(const L7_uchar8 *buf,L7_uint32 length);

/*********************************************************************
* @purpose Look up for a H245 call
*
* @param   L7_ushort16   srcport  SourcePort
* @param   L7_ushort16   dstport  Destination Port
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t vopiH245CallLookup(L7_ushort16 srcport,L7_ushort16 dstport);


#define FNAME(name)
/**************************************************************************
* ASN definitions
***************************************************************************/
/* ASN.1 Types */
#define ASN_NUL 0
#define ASN_BOOL 1
#define ASN_OID 2
#define ASN_INT 3
#define ASN_ENUM 4
#define ASN_BITSTR 5
#define ASN_NUMSTR 6
#define ASN_NUMDGT 6
#define ASN_TBCDSTR 6
#define ASN_OCTSTR 7
#define ASN_PRTSTR 7
#define ASN_IA5STR 7
#define ASN_GENSTR 7
#define ASN_BMPSTR 8
#define ASN_SEQ 9
#define ASN_SET 9
#define ASN_SEQOF 10
#define ASN_SETOF 10
#define ASN_CHOICE 11

/* Constraint Types */
#define ASN_FIXD 0
/* #define ASN_BITS 1-8 */
#define ASN_BYTE 9
#define ASN_WORD 10
#define ASN_CONS 11
#define ASN_SEMI 12
#define ASN_UNCO 13

/* ASN.1 Type Attributes */
#define ASN_SKIP 0
#define ASN_STOP 1
#define ASN_DECODE 2
#define ASN_EXT 4
#define ASN_OPEN 8
#define ASN_OPT 16

/* ASN.1 Field Structure */
typedef struct asnField_s
{
  L7_uchar8 type;
  L7_uchar8 sz;
  L7_uchar8 lb;
  L7_uchar8 ub;
  L7_ushort16 attr;
  L7_ushort16 offset;
  struct asnField_s *fields;
} asnField_t;

/* Bit Stream */
typedef struct asnBitStr_s
{
  L7_uchar8 *buf;
  L7_uchar8 *beg;
  L7_uchar8 *end;
  L7_uchar8 *cur;
  L7_uint32 bit;
} asnBitStr_t;

/* Tool Functions */
#define INC_BIT(bs) if((++bs->bit)>7){bs->cur++;bs->bit=0;}
#define INC_BITS(bs,b) if((bs->bit+=b)>7){bs->cur+=bs->bit>>3;bs->bit&=7;}
#define BYTE_ALIGN(bs) if(bs->bit){bs->cur++;bs->bit=0;}
#define CHECK_BOUND(bs,n) if(bs->cur+(n)>bs->end)return(L7_ERROR)

typedef struct connPair_s 
{
  L7_uint32 src_ip;
  L7_uint32 dst_ip;
  L7_ushort16 src_port;
  L7_ushort16 dst_port;
}connPair_t;

typedef struct connInfo_s 
{
  connPair_t  pair;
  L7_uint32   h245_ip;
  L7_ushort16 h245_port;
  L7_BOOL     h245_SessionStatus;
  L7_ushort16 dir;
  L7_ushort16 tpktlen[2];
  L7_uint32   media_ip[2];
  L7_ushort16 media_port[2];
  L7_BOOL     rtp_SessionStatus;
  L7_BOOL     tunneling;
}connInfo_t;

typedef struct voipH323Call_s 
{
  connInfo_t       ct_info;
  L7_ulong64       prev_counter;
  struct voipH323Call_s *next;
} voipH323Call_t;

#endif



