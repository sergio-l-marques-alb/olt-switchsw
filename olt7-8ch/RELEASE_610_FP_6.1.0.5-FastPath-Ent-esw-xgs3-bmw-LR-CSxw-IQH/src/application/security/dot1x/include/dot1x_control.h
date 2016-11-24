/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_control.h
*
* @purpose   dot1x Control header
*
* @component dot1x
*
* @comments  none
*
* @create    04/08/2003
*
* @author    mfiorito
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_DOT1X_CONTROL_H
#define INCLUDE_DOT1X_CONTROL_H

typedef enum dot1xControlEvents_s
{
  /***************************************************************/
  /* Events shared with all                                      */
  /***************************************************************/
  /*100*/dot1xControlBegin = 100,

  /***************************************************************/
  /* Events from Management commands                             */
  /***************************************************************/
  /*101*/dot1xMgmtAdminModeEnable,
  /*102*/dot1xMgmtAdminModeDisable,
  
  /*103*/dot1xMgmtPortInitializeSet,
  /*104*/dot1xMgmtLogicalPortInitializeSet,
  /*105*/dot1xMgmtPortReauthenticateSet,
  /*106*/dot1xMgmtPortAdminControlledDirectionsSet,
  /*107*/dot1xMgmtPortControlModeSet,
  /*108*/dot1xMgmtPortGuestVlanModeEnable,
  /*109*/dot1xMgmtPortGuestVlanModeDisable,
  /*110*/dot1xMgmtPortGuestVlanTimerExpired,
  /*111*/dot1xMgmtPortGuestVlanPeriod,
  /*112*/dot1xMgmtPortQuietPeriodSet,
  /*113*/dot1xMgmtPortTxPeriodSet,
  /*114*/dot1xMgmtPortSuppTimeoutSet,
  /*115*/dot1xMgmtPortServerTimeoutSet,
  /*116*/dot1xMgmtPortMaxReqSet,
  /*117*/dot1xMgmtPortReAuthPeriodSet,
  /*118*/dot1xMgmtPortReAuthEnabledSet,
  /*119*/dot1xMgmtPortKeyTransmissionEnabledSet,
  /*120*/dot1xMgmtPortStatsClear,
  /*121*/dot1xMgmtApplyConfigData,
  /*122*/dot1xMgmtApplyPortConfigData,
  /*123*/dot1xMgmtPortMaxUsersSet,
  /*124*/dot1xMgmtPortUnauthenticatedVlanSet,
  /*125*/dot1xMgmtLogicalPortReauthenticateSet,
  /*126*/dot1xMgmtPortPaeCapabilitySet,
  /*127*/dot1xMgmtSupplicantPortControlModeSet,
  /*126*/dot1xMgmtPortMABEnabledSet,
  /*127*/dot1xMgmtPortMABDisabledSet,
  /*128*/dot1xMgmtPortMABTimerExpired,

  /*129*/dot1xMgmtEvents, /*keep this last in sub group*/

  /***************************************************************/
  /* Events from network pdu received                            */
  /***************************************************************/
  /*130*/dot1xPduReceived,

  /*131*/dot1xNetworkEvents,

  /***************************************************************/
  /* Events from AAA client                                      */
  /***************************************************************/
  /*132*/dot1xAaaInfoReceived,

  /*133*/dot1xAaaEvents,
  
  /***************************************************************/
  /* Events from interface state changes                         */
  /***************************************************************/
  /*134*/dot1xIntfChange,

  /*135*/dot1xIntfChangeEvents,

  /***************************************************************/
  /* Events from Vlan state changes                              */
  /***************************************************************/

  /*136*/dot1xVlanDeleteEvent,
  /*137*/dot1xVlanAddEvent,
  /*138*/dot1xVlanAddPortEvent,
  /*139*/dot1xVlanDeletePortEvent,
  /*140*/dot1xVlanPvidChangeEvent,
  /***************************************************************/
  /* Events from configurator                                    */
  /***************************************************************/
  /*141*/dot1xCnfgr,        

  /*142*/dot1xCnfgrEvents,

  /*143*/dot1xUnauthAddrCallBackEvent,
  /*144*/dot1xVoiceVlanModeEvent
}dot1xControlEvents_t;

/* Message structure to hold responses from AAA client (i.e. RADIUS) */
typedef struct dot1xAaaMsg_s
{
  L7_uint32 status;      /* status of response (i.e. RADIUS_STATUS_SUCCESS, etc.) */
  L7_uint32 respLen;     /* length of data (response) being passed */
  L7_uchar8 *pResponse;  /* pointer to response from AAA server */
} dot1xAaaMsg_t;

typedef struct dot1xIntfChangeParms_s 
{
  L7_uint32 intfEvent;
  NIM_CORRELATOR_t nimCorrelator;
} dot1xIntfChangeParms_t;

typedef struct dot1xUnauthCallbackParms_s
{
  L7_enetMacAddr_t  macAddr;
  L7_ushort16       vlanId;
} dot1xUnauthCallbackParms_t;

typedef struct dot1xVoiceVlanModeParms_s
{
  L7_enetMacAddr_t  macAddr;
  L7_ushort16       vlanId;
  L7_BOOL           flag;
} dot1xVoiceVlanModeParms_t;

typedef struct dot1xMsg_s
{
  L7_uint32 event;
  L7_uint32 intf;
  union
  {
    L7_uint32              msgParm;
    L7_netBufHandle        bufHandle;
    dot1xAaaMsg_t          dot1xAaaMsg;
    L7_CNFGR_CMD_DATA_t    CmdData;
    dot1xIntfChangeParms_t dot1xIntfChangeParms;
    dot1xUnauthCallbackParms_t   unauthParms;
    dot1xVoiceVlanModeParms_t    voiceVlanParms; 
    dot1qNotifyData_t      vlanData;
  }data;
} dot1xMsg_t;


#define DOT1X_MSG_COUNT   FD_DOT1X_MSG_COUNT
#define DOT1X_TIMER_TICK  1000 /*in milliseconds*/


extern L7_RC_t dot1xStartTasks();
extern void dot1xTask();
extern void dot1xTimerTask();
extern L7_RC_t dot1xFillMsg(void *data, dot1xMsg_t *msg);
extern L7_RC_t dot1xIssueCmd(L7_uint32 event, L7_uint32 intIfNum, void *data);
extern L7_RC_t dot1xDispatchCmd(dot1xMsg_t *msg);
extern L7_RC_t dot1xStateMachineRxPdu(L7_uint32 intIfNum, L7_netBufHandle bufHandle);
extern L7_RC_t dot1xProcessRxPdu(L7_uint32 lIntIfNum, L7_netBufHandle bufHandle);
extern L7_RC_t dot1xEapPktProcess(dot1xLogicalPortInfo_t *logicalPortInfo, L7_netBufHandle bufHandle);
extern L7_RC_t dot1xEapolAlertPktProcess(L7_uint32 intIfNum, L7_netBufHandle bufHandle);
extern L7_RC_t dot1xTimerAction();

extern L7_RC_t dot1xCtlAdminModeEnable();
extern L7_RC_t dot1xCtlAdminModeDisable();
extern L7_RC_t dot1xCtlPortInitializeSet(L7_uint32 intIfNum, L7_BOOL initialize);
extern L7_RC_t dot1xCtlLogicalPortInitializeSet(L7_uint32 lIntIfNum, L7_BOOL initialize);
extern L7_RC_t dot1xCtlPortReauthenticateSet(L7_uint32 intIfNum, L7_BOOL reauthenticate);
extern L7_RC_t dot1xCtlLogicalPortReauthenticateSet(L7_uint32 lIntIfNum, L7_BOOL reauthenticate);
extern L7_RC_t dot1xCtlPortAdminControlledDirectionsSet(L7_uint32 intIfNum,
                                              L7_DOT1X_PORT_DIRECTION_t adminControlledDirections);
extern L7_RC_t dot1xCtlPortControlModeSet(L7_uint32 intIfNum, L7_DOT1X_PORT_CONTROL_t portControl);
extern L7_RC_t dot1xCtlPortPaeCapabilitiesSet(L7_uint32 intIfNum, L7_uint32 capabilities);
extern L7_RC_t dot1xCtlPortQuietPeriodSet(L7_uint32 intIfNum, L7_uint32 quietPeriod);
extern L7_RC_t dot1xCtlPortTxPeriodSet(L7_uint32 intIfNum, L7_uint32 txPeriod);
extern L7_RC_t dot1xCtlPortGuestVlanOperational(L7_uint32 intIfNum);
extern L7_RC_t dot1xCtlPortGuestVlanMode(L7_uint32 intIfNum,L7_uint32 guestVlanId);
extern L7_RC_t dot1xCtlPortGuestVlanModeDisable(L7_uint32 lIntIfNum,L7_uint32 guestVlanId);
extern L7_RC_t dot1xCtlPortGuestVlanTimerStart(L7_uint32 ifindex,L7_uint32 VlanId,dot1xPortType_t type);
extern L7_RC_t dot1xCtlPortGuestVlanRemove(L7_uint32 intIfNum,L7_uint32 guestVlanId);
extern L7_RC_t dot1xCtlPortGuestVlanPeriodSet(L7_uint32 intIfNum,L7_uint32 guestVlanPeriod);
extern L7_RC_t dot1xCtlPortSuppTimeoutSet(L7_uint32 intIfNum, L7_uint32 suppTimeout);
extern L7_RC_t dot1xCtlPortServerTimeoutSet(L7_uint32 intIfNum, L7_uint32 serverTimeout);
extern L7_RC_t dot1xCtlPortMaxReqSet(L7_uint32 intIfNum, L7_uint32 maxReq);
extern L7_RC_t dot1xCtlPortReAuthPeriodSet(L7_uint32 intIfNum, L7_uint32 reAuthPeriod);
extern L7_RC_t dot1xCtlPortReAuthEnabledSet(L7_uint32 intIfNum, L7_BOOL reAuthEnabled);
extern L7_RC_t dot1xCtlPortKeyTransmissionEnabledSet(L7_uint32 intIfNum, L7_BOOL keyTxEnabled);
extern L7_RC_t dot1xCtlPortStatsClear(L7_uint32 intIfNum);
extern L7_RC_t dot1xCtlApplyConfigData(void);
extern L7_RC_t dot1xCtlApplyPortConfigData(L7_uint32 intIfNum);
extern L7_RC_t dot1xRadiusServerVlanAssignmentHandle(L7_uint32 intIfNum,L7_uint32 vlanId);
extern L7_RC_t dot1xVlanAssignmentEnable(dot1xLogicalPortInfo_t *logicalPortInfo,L7_uint32 vlanId);
extern L7_RC_t dot1xVlanAssignmentDisable(L7_uint32 intIfNum,L7_uint32 vlanId);
extern L7_RC_t dot1xPortVlanAssignmentDisable(L7_uint32 intIfNum);
extern L7_RC_t dot1xVlanChangeCallback(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, L7_uint32 event);
extern L7_RC_t dot1xApplyConfigCompleteCb(L7_uint32 event);
extern L7_RC_t dot1xCtlPortDot1xAcquire(L7_uint32 intIfNum,L7_uint32 VlanId);
extern L7_RC_t dot1xCtlPortDot1xRelease(L7_uint32 intIfNum,L7_uint32 VlanId);
extern L7_RC_t dot1xCtlPortDot1xGuestVlanAdd(L7_uint32 intIfNum,L7_uint32 VlanId);
extern L7_RC_t dot1xCtlPortDot1xGuestVlanDelete(L7_uint32 intIfNum,L7_uint32 VlanId);
extern L7_RC_t dot1xCtlPortGuestVlanTimerStop(L7_uint32 intIfNum);
extern void dot1xVlanChangeProcess(L7_uint32 event, L7_uint32 intIfNum, dot1qNotifyData_t *vlanData);
extern L7_RC_t dot1xCheckMapPdu(L7_uint32 intIfNum, L7_char8 *srcMac, L7_uint32 *logicalPort,L7_BOOL *existing_node);
extern L7_RC_t dot1xCtlPortMaxUsersSet(L7_uint32 intIfNum, L7_uint32 maxUsers);
extern L7_RC_t dot1xCtlApplyLogicalPortConfigData(L7_uint32 lIntIfNum);
extern L7_RC_t dot1xCtlResetPortSessionData(L7_uint32 intIfNum);
extern L7_RC_t dot1xCtlResetLogicalPortSessionData(dot1xLogicalPortInfo_t *logicalPortInfo);
extern L7_RC_t dot1xCtlStopPortSessionData(L7_uint32 intIfNum);
extern L7_RC_t dot1xCtlStopLogicalPortSessionData(dot1xLogicalPortInfo_t *logicalPortInfo);
extern L7_RC_t dot1xCtlLogicalPortSuppTimeoutSet(dot1xLogicalPortInfo_t *logicalPortInfo, L7_uint32 suppTimeout);
extern L7_RC_t dot1xPortVlanMembershipSet(L7_uint32 intIfNum, L7_uint32 vlanId,L7_BOOL flag);
extern L7_RC_t dot1xVlanAddPortEventProcess(L7_uint32 intIfNum,L7_uint32 VlanId);
extern L7_RC_t dot1xVlanDeletePortEventProcess(L7_uint32 intIfNum,L7_uint32 VlanId);
extern L7_RC_t dot1xVlanPVIDChangeEventProcess(L7_uint32 intIfNum,L7_uint32 vlanId);
extern L7_RC_t dot1xApplyAssignedPolicy(L7_uint32 intIfNum,L7_uint32 vlanId,
                                        L7_uchar8 *macAddr, L7_uchar8 *policyString,L7_BOOL  flag);

extern L7_RC_t dot1xCtlPortUnauthenticatedVlanSet(L7_uint32 intIfNum,L7_uint32 vlanId);
extern L7_RC_t dot1xCtlLogicalPortUnauthenticatedVlanReset(L7_uint32 lIntIfNum);
extern L7_RC_t dot1xCtlPortUnauthenticatedVlanOperational(dot1xLogicalPortInfo_t *logicalPortInfo,L7_uint32 reason);
extern L7_RC_t dot1xCtlLogicalPortVlanAssignedReset(L7_uint32 lIntIfNum);
extern L7_RC_t dot1xCtlLogicalPortVlanAssignmentDisable(dot1xLogicalPortInfo_t *logicalPortInfo);
extern L7_RC_t dot1xCtlPortUnauthAddrCallbackProcess(L7_uint32 intIfNum,L7_enetMacAddr_t macAddr,L7_ushort16 vlanId);
extern L7_RC_t dot1xCtlPortVoiceVlanOperational(L7_uint32 intIfNum,L7_enetMacAddr_t macAddr,L7_ushort16 vlanId,L7_BOOL flag);

/*MAB*/
extern L7_RC_t dot1xCtlLogicalPortMABTimerStart(L7_uint32 lIntIfNum);
extern L7_RC_t dot1xCtlPortMABEnableSet(L7_uint32 intIfNum);
extern L7_RC_t dot1xCtlPortMABDisableSet(L7_uint32 intIfNum);
extern L7_RC_t dot1xCtlLogicalPortMABRemove(L7_uint32 llIntIfNum);
extern L7_RC_t dot1xCtlLogicalPortMABOperational(L7_uint32 llIntIfNum);
extern L7_RC_t dot1xCtlLogicalPortMABAuthFailGuestVlanSet(dot1xLogicalPortInfo_t *logicalPortInfo);
extern L7_RC_t dot1xCtlLogicalPortMABGuestVlanReset(L7_uint32 lIntIfNum);
extern L7_RC_t dot1xCtlLogicalPortMABGenResp(L7_uint32 lIntIfNum);

/* Supplicant APIs */
extern L7_RC_t dot1xCtlApplySupplicantPortConfigData(L7_uint32 intIfNum);
extern L7_RC_t dot1xCtlSupplicantPortControlModeSet(L7_uint32 intIfNum, 
                                                    L7_DOT1X_PORT_CONTROL_t portControl);

extern L7_RC_t dot1xSupplicantStateMachineRxPdu(L7_uint32 intIfNum, L7_netBufHandle bufHandle);



#endif /* INCLUDE_DOT1X_CONTROL_H */
