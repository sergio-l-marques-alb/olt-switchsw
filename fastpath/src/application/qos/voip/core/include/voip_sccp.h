/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename voip_sccp.h
*
* @purpose SCCP message parsing routines
*
* @component VOIP |SCCP
*
* @comments none
*
* @create 05/12/2007
*
* @author aprashant
* @end
*
**********************************************************************/
#ifndef VOIP_SCCP_H
#define VOIP_SCCP_H
#include "voip_parse.h"

typedef enum {
  /* Client -> Server */
  KeepAliveMessage                                              = 0x0000,
  RegisterMessage                                               = 0x0001,
  IpPortMessage                                                 = 0x0002,
  KeypadButtonMessage                                           = 0x0003,
  EnblocCallMessage                                             = 0x0004,
  StimulusMessage                                               = 0x0005,
  OffHookMessage                                                = 0x0006,
  OnHookMessage                                                 = 0x0007,
  HookFlashMessage                                              = 0x0008,
  ForwardStatReqMessage                                         = 0x0009,
  SpeedDialStatReqMessage                                       = 0x000A,
  LineStatReqMessage                                            = 0x000B,
  ConfigStatReqMessage                                          = 0x000C,
  TimeDateReqMessage                                            = 0x000D,
  ButtonTemplateReqMessage                                      = 0x000E,
  VersionReqMessage                                             = 0x000F,
  CapabilitiesResMessage                                        = 0x0010,
  MediaPortListMessage                                          = 0x0011,
  ServerReqMessage                                              = 0x0012,
  AlarmMessage                                                  = 0x0020,
  MulticastMediaReceptionAck                                    = 0x0021,
  OpenReceiveChannelAck                                         = 0x0022,
  ConnectionStatisticsRes                                       = 0x0023,
  OffHookWithCgpnMessage                                        = 0x0024,
  SoftKeySetReqMessage                                          = 0x0025,
  SoftKeyEventMessage                                           = 0x0026,
  UnregisterMessage                                             = 0x0027,
  SoftKeyTemplateReqMessage                                     = 0x0028,
  RegisterTokenReq                                              = 0x0029,
  HeadsetStatusMessage                                          = 0x002B,
  MediaResourceNotification                                     = 0x002C,
  RegisterAvailableLinesMessage                                 = 0x002D,
  DeviceToUserDataMessage                                       = 0x002E,
  DeviceToUserDataResponseMessage                               = 0x002F,
  UpdateCapabilitiesMessage                                     = 0x0030,
  OpenMultiMediaReceiveChannelAckMessage                        = 0x0031,
  ClearConferenceMessage                                        = 0x0032,
  ServiceURLStatReqMessage                                      = 0x0033,
  FeatureStatReqMessage                                         = 0x0034,
  CreateConferenceResMessage                    	        = 0x0035,
  DeleteConferenceResMessage                           	        = 0x0036,
  ModifyConferenceResMessage                           	        = 0x0037,
  AddParticipantResMessage                                      = 0x0038,
  AuditConferenceResMessage                                     = 0x0039,
  AuditParticipantResMessage                                    = 0x0040,
  DeviceToUserDataVersion1Message                               = 0x0041,
  DeviceToUserDataResponseVersion1Message                       = 0x0042,

  /* Server -> Client */
  RegisterAckMessage                                            = 0x0081,
  StartToneMessage                                              = 0x0082,
  StopToneMessage                                               = 0x0083,
  /* ?? */
  SetRingerMessage                                              = 0x0085,
  SetLampMessage                                                = 0x0086,
  SetHkFDetectMessage                                           = 0x0087,
  SetSpeakerModeMessage                                         = 0x0088,
  SetMicroModeMessage                                           = 0x0089,
  StartMediaTransmission                                        = 0x008A,
  StopMediaTransmission                                         = 0x008B,
  StartMediaReception                                           = 0x008C,
  StopMediaReception                                            = 0x008D,
  /* ?? */
  CallInfoMessage                                               = 0x008F,

  ForwardStatMessage                                            = 0x0090,
  SpeedDialStatMessage                                          = 0x0091,
  LineStatMessage                                               = 0x0092,
  ConfigStatMessage                                             = 0x0093,
  DefineTimeDate                                                = 0x0094,
  StartSessionTransmission                                      = 0x0095,
  StopSessionTransmission                                       = 0x0096,
  ButtonTemplateMessage                                         = 0x0097,
  VersionMessage                                                = 0x0098,
  DisplayTextMessage                                            = 0x0099,
  ClearDisplay                                                  = 0x009A,
  CapabilitiesReqMessage                                        = 0x009B,
  EnunciatorCommandMessage                                      = 0x009C,
  RegisterRejectMessage                                         = 0x009D,
  ServerResMessage                                              = 0x009E,
  Reset                                                         = 0x009F,

  KeepAliveAckMessage                                           = 0x0100,
  StartMulticastMediaReception                                  = 0x0101,
  StartMulticastMediaTransmission                               = 0x0102,
  StopMulticastMediaReception                                   = 0x0103,
  StopMulticastMediaTransmission                                = 0x0104,
  OpenReceiveChannel                                            = 0x0105,
  CloseReceiveChannel                                           = 0x0106,
  ConnectionStatisticsReq                                       = 0x0107,
  SoftKeyTemplateResMessage                                     = 0x0108,
  SoftKeySetResMessage                                          = 0x0109,

  SelectSoftKeysMessage                                         = 0x0110,
  CallStateMessage                                              = 0x0111,
  DisplayPromptStatusMessage                                    = 0x0112,
  ClearPromptStatusMessage                                      = 0x0113,
  DisplayNotifyMessage                                          = 0x0114,
  ClearNotifyMessage                                            = 0x0115,
  ActivateCallPlaneMessage                                      = 0x0116,
  DeactivateCallPlaneMessage                                    = 0x0117,
  UnregisterAckMessage                                          = 0x0118,
  BackSpaceReqMessage                                           = 0x0119,
  RegisterTokenAck                                              = 0x011A,
  RegisterTokenReject                                           = 0x011B,
  StartMediaFailureDetection                                    = 0x011C,
  DialedNumberMessage                                           = 0x011D,
  UserToDeviceDataMessage                                       = 0x011E,
  FeatureStatMessage                                            = 0x011F,
  DisplayPriNotifyMessage                                       = 0x0120,
  ClearPriNotifyMessage                                         = 0x0121,
  StartAnnouncementMessage                                      = 0x0122,
  StopAnnouncementMessage                                       = 0x0123,
  AnnouncementFinishMessage                                     = 0x0124,
  NotifyDtmfToneMessage                                         = 0x0127,
  SendDtmfToneMessage                                           = 0x0128,
  SubscribeDtmfPayloadReqMessage                                = 0x0129,
  SubscribeDtmfPayloadResMessage                                = 0x012A,
  SubscribeDtmfPayloadErrMessage                                = 0x012B,
  UnSubscribeDtmfPayloadReqMessage                              = 0x012C,
  UnSubscribeDtmfPayloadResMessage                              = 0x012D,
  UnSubscribeDtmfPayloadErrMessage                              = 0x012E,
  ServiceURLStatMessage                                         = 0x012F,
  CallSelectStatMessage                                         = 0x0130,
  OpenMultiMediaChannelMessage                                  = 0x0131,
  StartMultiMediaTransmission                                   = 0x0132,
  StopMultiMediaTransmission                                    = 0x0133,
  MiscellaneousCommandMessage                                   = 0x0134,
  FlowControlCommandMessage                                     = 0x0135,
  CloseMultiMediaReceiveChannel                                 = 0x0136,
  CreateConferenceReqMessage                                    = 0x0137,
  DeleteConferenceReqMessage                                    = 0x0138,
  ModifyConferenceReqMessage                                    = 0x0139,
  AddParticipantReqMessage                                      = 0x013A,
  DropParticipantReqMessage                                     = 0x013B,
  AuditConferenceReqMessage                                     = 0x013C,
  AuditParticipantReqMessage                                    = 0x013D,
  UserToDeviceDataVersion1Message                               = 0x013F

}voipSccpMessage_t;

typedef struct voipOperSccp_s
{
   L7_uchar8          callManager[4];
   L7_uint32          length;
   L7_uint32          lel_conferenceId;
   L7_uint32          lel_passThruPartyId;
   L7_uint32          lel_conferenceId1;
   L7_uint32          lel_messageId;
   L7_uint32          bel_remoteIpAddr; /* This field is apparently in big-endian
                                           format, even though most other fields are
                                           little-endian. */
   L7_uint32          lel_remotePortNumber;
}voipOperSccp_t;

typedef struct voipSccpCall_s 
{
    L7_uint32              conferenceId;
    L7_uint32              remoteIP;
    L7_uint32              remotePort;
    L7_ulong64             prev_counter;
    struct voipSccpCall_s         *next;
}voipSccpCall_t;

typedef struct voipSccpCms_s 
{
    L7_uchar8            callManager[4];  /* call manager ip address. This only to maintain the call database per callmanager*/ 
    voipSccpCall_t       *calls;
    L7_uint32            count;
    struct  voipSccpCms_s *next;
} voipSccpCms_t;

/* So in theory, a message should never be bigger than this.
 * If it is, we abort the connection */
#define SCCP_MAX_PACKET sizeof(voipSccpMoo_t)
/*********************************************************************
* @purpose Parse SCCP message
*
* @param   const L7_uchar8 *data     Pointer to data packet
* @param   L7_uint32       length    length of message
* @param   L7_uint32       interface interface number
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipSccpMessageParse (const L7_uchar8 *data, L7_uint32 length);
/*********************************************************************
* @purpose  Clear all call entries corrosponding to an interface
*
* @param    L7_uint32 interface       Interface number for which entries
*           need to be cleared
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipSccpClearInterfaceEntries();

/*********************************************************************
* @purpose Look up for a SCCP cms
*
* @param   const L7_uchar8 *ip       Ip to be searched
* @param   voipSccpCms_t   **entry    SCCP Cms entry
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
voipSccpCmsLookup (const L7_uchar8 *callManager, voipSccpCms_t **entry);

/*********************************************************************
* @purpose Insert a new SCCP cms entry
*
* @param   const L7_uchar8 *ip      Ip to be inserted
* @param   voipSccpCms_t   **ins    Pointer to inserted  entry
*
* @returns  L7_SUCCESS or L7_FAILURE or L7_ALREADY_CONFIGURED(if entry
*           already exists )
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
voipSccpCmsInsert(const L7_uchar8 *callManager, voipSccpCms_t **ins);

/*********************************************************************
* @purpose Insert a new SCCP call entry
*
* @param   voipSccpCms_t   *cm   List of SCCP cms
* @param   voipSccpMoo_t    *m   Pointer to voipSccpMoo_t
* @param   voipSccpCall_t  **ins Pointer to inserted entry
*
* @returns  L7_SUCCESS or L7_FAILURE or L7_ALREADY_CONFIGURED(if entry
*           already exists )
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
voipSccpCallInsert(voipSccpCms_t *cm,voipOperSccp_t m,voipSccpCall_t **ins);

/*********************************************************************
* @purpose Remove SCCP call entry
*
* @param   voipSccpCms_t   **cm  List of SCCP Cms
* @param   voipSccpCall_t   *del     entry to be deleted
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
voipSccpCallRemove(voipSccpCms_t **cm,voipSccpCall_t *del);

/*********************************************************************
* @purpose Enable/disable a SCCP session
*
* @param   voipSccpCall_t *t        Pointer to voipSccpCall_t
* @param   L7_uint32 enable         L7_ENABLE | L7_DISABLE
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipSetupSccpSession(voipSccpCall_t *t, L7_BOOL callStatus);

#ifdef L7_NSF_PACKAGE
/*********************************************************************
* @purpose  Rebuild all checkpointed data.
*
* @param    void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void voipSccpCkptRebuild();
#endif

void voipSccpCallDump();
#endif
