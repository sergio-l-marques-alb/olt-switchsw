/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_sm.h
*
* @purpose   dot1x State Machine header file
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
#ifndef INCLUDE_DOT1X_SM_H
#define INCLUDE_DOT1X_SM_H

typedef enum dot1xEvents_s
{
  /***************************************************************/
  /* Events just for Authenticator PAE Machine (APM)             */
  /***************************************************************/
  /*0*/apmInitialize = 0,                               /*E1.apm*/
  /*1*/apmPortControlEqualAutoPortModeNotEqualPortCtrl, /*E2*/
  /*2*/apmNotPortEnabled,                               /*E3.apm*/
  /*3*/apmEapLogoff,                                    /*E4*/
  /*4*/apmReAuthCountGreaterThanReAuthMax,              /*E5*/
  /*5*/apmQuietWhileEqualZero,                          /*E6*/
  /*6*/apmTxWhenZeroReAuthCountLessThanEqualReAuthMax,  /*E7*/
  /*7*/apmEapStartReAuthCountLessThanEqualReAuthMax,    /*E8*/
  /*8*/apmReauthAndReAuthCountLessThanEqualReAuthMax,   /*E9*/
  /*9*/apmRxRespIdAndReAuthCountLessThanEqualReAuthMax, /*E10*/
  /*10*/apmNotEapLogoffAndNotAuthAbort,                 /*E11*/
  /*11*/apmEapLogoffAndNotAuthAbort,                    /*E12*/
  /*12*/apmEapStart,                                    /*E13*/
  /*13*/apmReAuthenticate,                              /*E14*/
  /*14*/apmAuthSuccess,                                 /*E15*/
  /*15*/apmAuthFail,                                    /*E16*/
  /*16*/apmAuthTimeout,                                 /*E17*/
  /*17*/apmPortControlEqualForceAuthorized,             /*E18*/
  /*18*/apmPortControlEqualForceUnauthorized,           /*E19*/
  /*19*/apmAssignedVlanDelete,                          /*E37*/  
  /*20*/apmGuestVlanTimerExpired,                       /*E40*/
  /*21*/apmAssignUnauthenticatedVlan,                   /*E41*/

  /*22*/apmAssignVoiceVlanDelete,                       /*E42*/
  /*23*/apmMABDisable,                                  /*E43*/
  /*24*/apmMABTimerExpired,                              /*E44*/
  /*25*/apmMABFailAssignGuestVlan,                       /*E45*/
  /*26*/dot1xApmEvents,/*keep this last in sub group*/
  
  /***************************************************************/
  /* Events just for Authenticator Key Transmit Machine (KTX)    */
  /***************************************************************/
  /*27*/ktxInitialize,                                  /*E1.ktx*/
  /*28*/ktxPortControlNotEqualAuto,                     /*E20.ktx*/
  /*29*/ktxKeyTxEnabledAndKeyAvailableAndPortAuthorized,/*E21*/
  /*30*/ktxKeyAvailable,                                /*E22*/
  /*31*/ktxNotKeyTxEnabled,                             /*E23*/
  /*32*/ktxPortStatusEqualUnauthorized,                 /*E24.ktx*/
  
  /*33*/dot1xKtxEvents,/*keep this last in sub group*/
  
  /***************************************************************/
  /* Events just for Reauthentication Timer Machine (RTM)        */
  /***************************************************************/
  /*34*/rtmInitialize,                                  /*E1.rtm*/
  /*35*/rtmPortControlNotEqualAuto,                     /*E20.rtm*/
  /*36*/rtmPortStatusEqualUnauthorized,                 /*E24.rtm*/
  /*37*/rtmNotReauthEnabled,                            /*E25*/
  /*38*/rtmReauthWhenEqualZero,                         /*E26*/

  /*39*/dot1xRtmEvents,/*keep this last in sub group*/
  
  /***************************************************************/
  /* Events just for Backend Authentication Machine (BAM)        */
  /***************************************************************/
  /*40*/bamInitialize,                                  /*E1.bam*/
  /*41*/bamPortControlNotEqualAuto,                     /*E20.bam*/
  /*42*/bamAuthAbort,                                   /*E27*/
  /*43*/bamAWhileEqualZeroAndReqCountNotEqualMaxReq,    /*E28*/
  /*44*/bamRxResp,                                      /*E29*/
  /*45*/bamAWhileZeroAndReqCountGreaterThanEqualMaxReq, /*E30*/
  /*46*/bamASuccess,                                    /*E31*/
  /*47*/bamAFail,                                       /*E32*/
  /*48*/bamAWhileEqualZero,                             /*E33*/
  /*49*/bamAReq,                                        /*E34*/
  /*50*/bamAuthStart,                                   /*E35*/
  /*51*/bamAssignedVlanDelete,                          /*E37*/
  /*52*/bamGuestVlanTimerExpired,                       /*E40*/
  /*53*/bamAssignUnauthenticatedVlan,                   /*E41*/
  /*54*/bamMABFailAssignGuestVlan,                       /*E45*/
 
  /*55*/dot1xBamEvents,/*keep this last in sub group*/

  /***************************************************************/
  /* Events just for Key Receive Machine (KRX)                   */
  /***************************************************************/
  /*56*/krxInitialize,                                  /*E1.krx*/
  /*57*/krxNotPortEnabled,                              /*E3.krx*/
  /*58*/krxRxKey,                                       /*E36*/

  /*59*/dot1xKrxEvents, /*keep this last in sub group*/

  /************************************************************/
  /* Events just for Supplicant PAE state machine (SPM)       */
  /************************************************************/

  /*60*/spmInitialize ,                                 /*E1.spm*/
  /*61*/spmPortControlEqualAutoPortModeNotEqualPortCtrl,/*E2.spm*/
  /*62*/spmNotPortEnabled,                              /*E3.spm*/
  /*63*/spmPortEnabledUserLogoffLogoffNotSent,          /*E4.spm*/
  /*64*/spmPortUserLoggedIn,                             /*E5.spm*/
  /*65*/spmHeldWhileEqualZero,                          /*E6.spm*/
  /*66*/spmStartWhenZeroStartCountLessThanMaxStart,     /*E7.spm*/
  /*67*/spmStartWhenZeroStartCountntEqualMaxStart,      /*E8.spm*/
  /*68*/spmConnectingEapolEap,                          /*E9.spm*/
  /*69*/spmSuppTimeOut,                                  /*E10.spm*/
  /*70*/spmSuppFail,                                     /* E11.spm */
  /*71*/spmSuppSuccess,                                      /* E12.spm */
  /*72*/spmAuthenticatedEapolEap,                        /* E13.spm */
  /*73*/spmPortControlEqualForceAuthorized,             /*E14.spm*/
  /*74*/spmPortControlEqualForceUnauthorized,           /*E15.spm*/
  /*75*/spmEapSuccess,                                   /*E16.spm*/
  /*76*/spmEapFail,                                      /*E17.spm*/
  /*77*/dot1xSpmEvents,

  /************************************************************/
  /* Events just for Supplicant Backend state  machine (SPM)  */
  /************************************************************/

  /*78*/sbmInitialize,                                  /*E1.sbm*/
  /*79*/sbmPortControlNotEqualAuto,                     /*E2.sbm*/
  /*80*/sbmSuppAbort,                                   /*E3.sbm*/
  /*81*/sbmSuppStart,                                   /*E4.sbm*/
  /*82*/sbmEapResp,                                      /*E5.sbm*/
  /*83*/sbmEapNoResp,                                    /*E6.sbm*/
  /*84*/sbmAuthWhileZero,                                /*E7.sbm*/
  /*85*/ sbmEapFail,                                     /*E8.sbm*/
  /*86*/sbmEapSuccess,                                   /*E9.sbm*/
  /*87*/sbmEapolEap,                                     /*E10.sbm*/
  /*88*/dot1xSbmEvents
}dot1xEvents_t;


/* Authenticator PAE State Machine (APM) states (needed by UI) */
typedef enum
{
  APM_INITIALIZE = 0,
  APM_DISCONNECTED,
  APM_CONNECTING,
  APM_AUTHENTICATING,
  APM_AUTHENTICATED,
  APM_ABORTING,
  APM_HELD,
  APM_FORCE_AUTH,
  APM_FORCE_UNAUTH,
  APM_STATES
} DOT1X_APM_STATES_t;

/* Authenticator Key Transmit State Machine (KTX) states */
typedef enum
{
  KTX_NO_KEY_TRANSMIT = 0,
  KTX_KEY_TRANSMIT,
  KTX_STATES
} DOT1X_KTX_STATES_t;

/* Reauthentication Timer State Machine (RTM) states */
typedef enum
{
  RTM_INITIALIZE = 0,
  RTM_REAUTHENTICATE,
  RTM_STATES
} DOT1X_RTM_STATES_t;

/* Backend Authenticator State Machine (BAM) states (needed by UI) */
typedef enum
{
  BAM_REQUEST = 0,
  BAM_RESPONSE,
  BAM_SUCCESS,
  BAM_FAIL,
  BAM_TIMEOUT,
  BAM_IDLE,
  BAM_INITIALIZE,
  BAM_STATES
} DOT1X_BAM_STATES_t;

/* Key Receive State Machine (KRX) states */
typedef enum
{
  KRX_NO_KEY_RECEIVE = 0,
  KRX_KEY_RECEIVE,
  KRX_STATES
} DOT1X_KRX_STATES_t;

/* Supplicant state Machine state */
typedef enum
{
  DOT1X_SPM_LOGOFF = 0,
  DOT1X_SPM_DISCONNECTED,
  DOT1X_SPM_CONNECTING,
  DOT1X_SPM_AUTHENTICATING,
  DOT1X_SPM_HELD,
  DOT1X_SPM_AUTHENTICATED,
  DOT1X_SPM_RESTART,
  DOT1X_SPM_FORCE_AUTH,
  DOT1X_SPM_FORCE_UNAUTH,
  DOT1X_SPM_STATES
} DOT1X_SPM_STATES_t;

/* Supplicant Backend State Machine (SBM) states  */
typedef enum
{
  DOT1X_SBM_REQUEST = 0,
  DOT1X_SBM_RESPONSE,
  DOT1X_SBM_SUCCESS,
  DOT1X_SBM_FAIL,
  DOT1X_SBM_TIMEOUT,
  DOT1X_SBM_IDLE,
  DOT1X_SBM_INITIALIZE,
  DOT1X_SBM_RECEIVE,
  DOT1X_SBM_STATES
} DOT1X_SBM_STATES_t;


struct dot1xLogicalPortInfo_s;

extern L7_RC_t dot1xStateMachineClassifier(dot1xEvents_t dot1xEvent, L7_uint32 intIfNum, 
                                           L7_netBufHandle bufHandle, void *msg);

extern L7_RC_t dot1xApmMachine(L7_uint32 dot1xEvent, L7_uint32 intIfNum, L7_netBufHandle bufHandle);
extern L7_RC_t dot1xApmInitializeAction(struct dot1xLogicalPortInfo_s *logicalPortInfo);
extern L7_RC_t dot1xApmDisconnectedAction(struct dot1xLogicalPortInfo_s *logicalPortInfo);
extern L7_RC_t dot1xApmConnectingAction(struct dot1xLogicalPortInfo_s *logicalPortInfo);
extern L7_RC_t dot1xApmAuthenticatingAction(struct dot1xLogicalPortInfo_s *logicalPortInfo);
extern L7_RC_t dot1xApmAuthenticatedAction(struct dot1xLogicalPortInfo_s *logicalPortInfo);
extern L7_RC_t dot1xApmAbortingAction(struct dot1xLogicalPortInfo_s *logicalPortInfo);
extern L7_RC_t dot1xApmHeldAction(struct dot1xLogicalPortInfo_s *logicalPortInfo);
extern L7_RC_t dot1xApmForceAuthAction(struct dot1xLogicalPortInfo_s *logicalPortInfo);
extern L7_RC_t dot1xApmForceUnauthAction(struct dot1xLogicalPortInfo_s *logicalPortInfo);
extern L7_RC_t dot1xApmGenerateEvents(struct dot1xLogicalPortInfo_s *logicalPortInfo, void *msg);

extern L7_RC_t dot1xKtxMachine(L7_uint32 dot1xEvent, L7_uint32 intIfNum, void *msg);
extern L7_RC_t dot1xKtxNoKeyTransmitAction(struct dot1xLogicalPortInfo_s *logicalPortInfo);
extern L7_RC_t dot1xKtxKeyTransmitAction(struct dot1xLogicalPortInfo_s *logicalPortInfo);

extern L7_RC_t dot1xRtmMachine(L7_uint32 dot1xEvent, L7_uint32 intIfNum, void *msg);
extern L7_RC_t dot1xRtmInitializeAction(struct dot1xLogicalPortInfo_s *logicalPortInfo);
extern L7_RC_t dot1xRtmReauthenticateAction(struct dot1xLogicalPortInfo_s *logicalPortInfo);
extern L7_RC_t dot1xRtmGenerateEvents(struct dot1xLogicalPortInfo_s *logicalPortInfo, void *msg);

extern L7_RC_t dot1xBamMachine(L7_uint32 dot1xEvent, L7_uint32 intIfNum, L7_netBufHandle bufHandle);
extern L7_RC_t dot1xBamRequestAction(struct dot1xLogicalPortInfo_s *logicalPortInfo, L7_netBufHandle bufHandle);
extern L7_RC_t dot1xBamResponseAction(struct dot1xLogicalPortInfo_s *logicalPortInfo, L7_netBufHandle bufHandle);
extern L7_RC_t dot1xBamSuccessAction(struct dot1xLogicalPortInfo_s *logicalPortInfo);
extern L7_RC_t dot1xBamFailAction(struct dot1xLogicalPortInfo_s *logicalPortInfo);
extern L7_RC_t dot1xBamTimeoutAction(struct dot1xLogicalPortInfo_s *logicalPortInfo);
extern L7_RC_t dot1xBamIdleAction(struct dot1xLogicalPortInfo_s *logicalPortInfo);
extern L7_RC_t dot1xBamInitializeAction(struct dot1xLogicalPortInfo_s *logicalPortInfo);
extern L7_RC_t dot1xBamGenerateEvents(struct dot1xLogicalPortInfo_s *logicalPortInfo, void *msg);

extern L7_RC_t dot1xKrxMachine(L7_uint32 dot1xEvent, L7_uint32 intIfNum, L7_netBufHandle bufHandle);
extern L7_RC_t dot1xKrxNoKeyReceiveAction(struct dot1xLogicalPortInfo_s *logicalPortInfo);
extern L7_RC_t dot1xKrxKeyReceiveAction(struct dot1xLogicalPortInfo_s *logicalPortInfo, L7_netBufHandle bufHandle);

extern void dot1xAbortAuth(struct dot1xLogicalPortInfo_s *logicalPortInfo);

extern L7_RC_t dot1xSmAuthMethodGet(struct dot1xLogicalPortInfo_s *logicalPortInfo, 
                                    L7_netBufHandle bufHandle,
                                    L7_USER_MGR_AUTH_METHOD_t *authMethod);

/* Supplicant PAE state machine (SPMS)*/
extern L7_RC_t dot1xSpmMachine(L7_uint32 dot1xEvent, L7_uint32 intIfNum, L7_netBufHandle bufHandle);
extern L7_RC_t dot1xSpmDisconnectedAction(L7_uint32 intIfNum);
extern L7_RC_t dot1xSpmConnectingAction(L7_uint32 intIfNum);
extern L7_RC_t dot1xSpmForceAuthAction(L7_uint32 intIfNum);
extern L7_RC_t dot1xSpmForceUnAuthAction(L7_uint32 intIfNum);
extern L7_RC_t dot1xSpmHeldAction(L7_uint32 intIfNum);
extern L7_RC_t dot1xSpmAuthenticatingAction(L7_uint32 intIfNum,
                                      L7_netBufHandle bufHandle);
extern L7_RC_t dot1xSpmRestartAction(L7_uint32 intIfNum,
                                      L7_netBufHandle bufHandle);

/* Supplicant Backend state machine (SBMS) */
extern L7_RC_t dot1xSbmMachine(L7_uint32 dot1xEvent, L7_uint32 intIfNum, L7_netBufHandle bufHandle);
extern L7_RC_t dot1xSbmInitializeAction(L7_uint32 intIfNum);
extern L7_RC_t dot1xSbmIdleAction(L7_uint32 intIfNum);
extern L7_RC_t dot1xSbmRequestAction(L7_uint32 intIfNum,
                                   L7_netBufHandle bufHandle);
extern L7_RC_t dot1xSbmReceiveAction(L7_uint32 intIfNum);
extern L7_RC_t dot1xSbmResponseAction(L7_uint32 intIfNum,
                                   L7_netBufHandle bufHandle);
extern L7_RC_t dot1xSbmTimeoutAction(L7_uint32 intIfNum);
extern L7_RC_t dot1xSbmSuccessAction(L7_uint32 intIfNum);
extern L7_RC_t dot1xSbmFailAction(L7_uint32 intIfNum);

L7_RC_t dot1xDisableAuthenticatorPorts(intIfNum);
L7_RC_t dot1xEnableAuthenticatorPorts(intIfNum);



#endif /* INCLUDE_DOT1X_SM_H */
