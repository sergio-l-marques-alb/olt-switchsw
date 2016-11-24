/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_db.h
*
* @purpose   dot1x data structures include file
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
#ifndef INCLUDE_DOT1X_DB_H
#define INCLUDE_DOT1X_DB_H

#define DOT1X_USER_NAME_LEN L7_DOT1X_USER_NAME_LEN

#define DOT1X_PW_LEN L7_PASSWORD_SIZE

#define DOT1X_MD5_LEN   16
/*#define DOT1X_CHALLENGE_LEN 32*/
#define DOT1X_CHALLENGE_LEN  L7_DOT1X_CHALLENGE_LEN

#define DOT1X_SERVER_STATE_LEN 253
#define DOT1X_SERVER_CLASS_LEN 253

#define DOT1X_FILTER_NAME_LEN 256

#define DOT1X_MAX_USERS_PER_PORT  16
/*#define DOT1X_MAX_USERS           L7_MAX_PORT_COUNT + 2616 */ /*for testing---add in product files later on*/

#define DOT1X_CLIENT_TIMEOUT    300 /* seconds. Checks for client timeout every 5 minutes*/

#define DOT1X_LOGICAL_PORT_START(x) (((x)-1)*DOT1X_MAX_USERS_PER_PORT)
#define DOT1X_LOGICAL_PORT_END(x)   ((x)*DOT1X_MAX_USERS_PER_PORT)
#define DOT1X_PHYSICAL_PORT_GET(x)  (((x)/DOT1X_MAX_USERS_PER_PORT)+1) 
#define DOT1X_LOGICAL_PORT_ITERATE  0xFFFFFFFF

typedef enum dot1xFailureReason_s
{
  DOT1X_FAIL_REASON_AUTH_FAILED  = 1,
  DOT1X_FAIL_REASON_WRONG_AUTH     = 2,
  
}dot1xFailureReason_t;

/* switch info */
typedef struct dot1xInfo_s
{
  L7_uint32 traceId;
} dot1xInfo_t;

/* Per-port info */
typedef struct dot1xLogicalPortInfo_s
{
  L7_uint32   logicalPortNumber; 
  L7_BOOL   inUse;   /* TBD: need to be removed */
  L7_uint32   physPort;

  /* Timers */
  L7_uint32 aWhile;      /* Supplicant or Authentication Server timeout */
  L7_uint32 quietWhile;  /* Time period in which Authenticator will not attempt to acquire a Supplicant */
  L7_uint32 reAuthWhen;  /* Supplicant reauthentication timer */
  L7_uint32   txWhenLP;      /* EAP Request/Identity transmission timer */
  L7_uint32 guestVlanTimer;  /* Guest Vlan Timer */

  /* Inter-state machine communication and initialization */
  L7_BOOL                 authAbort;       /* Set to TRUE to signal Backend Auth SM to abort auth */
  L7_BOOL                 authFail;        /* TRUE if authentication process fails */
  L7_BOOL                 authStart;       /* Set to TRUE to signal Backend Auth SM to start auth */
  L7_BOOL                 authTimeout;     /* TRUE if no response from Supplicant */
  L7_BOOL                 authSuccess;     /* TRUE if authentication process succeeds */
  L7_uchar8               currentIdL;       /* ID of current auth session (0-255) */
  L7_BOOL                 reAuthenticate;  /* Set to TRUE when reAuthWhen timer expires */

  /* Authenticator PAE state machine parameters */
  L7_BOOL                 eapLogoff;    /* Set to TRUE when EAPOL-Logoff frame is received */
  L7_BOOL                 eapStart;     /* Set to TRUE when EAPOL-Start frame is received */
  L7_uint32               reAuthCount;  /* Number of times CONNECTING state is entered */
  L7_BOOL                 rxRespId;     /* Set to TRUE if EAP Response/Identity frame received */

  /* Backend Authentication state machine parameters */
  L7_uint32       reqCount;       /* No. of EAP Request/Identity frames sent w/ no resp from Supp */
  L7_BOOL         rxResp;         /* Set to TRUE if EAP-Packet (other than Req/Id) recved from Supp */
  L7_uchar8       idFromServer;   /* Most recent ID in EAP pkt received from Auth Server (0-255) */

  /* Key Receive state machine parameters */
  L7_BOOL rxKey;  /* Set to TRUE if EAPOL-Key frame is received from the Supplicant */

  /* State machine variables */
  DOT1X_APM_STATES_t    apmState;
  DOT1X_KTX_STATES_t    ktxState;
  DOT1X_RTM_STATES_t    rtmState;
  DOT1X_BAM_STATES_t    bamState;
  DOT1X_KRX_STATES_t    krxState;
  L7_DOT1X_PORT_STATUS_t  logicalPortStatus;      /* Current authorization state of the port */

  /* user Details */
  L7_uchar8 dot1xUserName[DOT1X_USER_NAME_LEN];
  L7_uint32 dot1xUserNameLength;
  L7_uint32 dot1xUserIndex;
  L7_uchar8 dot1xChallenge[DOT1X_CHALLENGE_LEN];
  L7_uint32 dot1xChallengelen;

  L7_uchar8 serverState[DOT1X_SERVER_STATE_LEN];
  L7_uint32 serverStateLen;

  L7_uchar8 serverClass[DOT1X_SERVER_CLASS_LEN];
  L7_uint32 serverClassLen;

  L7_uint32 sessionTimeout;
  L7_uint32 terminationAction;

  L7_netBufHandle suppBufHandle;  /* Hold onto buf handle for re-transmit */
  L7_enetMacAddr_t suppMacAddr;   /* MAC address of Supplicant */
  L7_uint32        vlanId;   /* Vlan Id of Supplicant */
 
  L7_BOOL       cnfgrEapolMacFlag;   /* Addl state info to help determine acc termination cause */
  L7_BOOL       reAuthenticating;
  L7_BOOL       suppRestarting;

  L7_USER_MGR_AUTH_METHOD_t authMethod; /* Auth method for the user of this port */
  L7_uint32                 vlanAssigned; /* stores the assigned vlan */
  L7_uint32                 sessionTime;  

  L7_uchar8    filterName[DOT1X_FILTER_NAME_LEN];
  
  L7_uint32    unauthVlan; /* unauthenticated vlan */
  L7_uint32    unauthReason; /* unauthenticated vlan assignment reason */
  L7_BOOL      reauth_auth_apply; /* indicates if authentication needs to be reapplied while client is reauthenticating*/
  L7_uint32    clientTimeout;
  

  L7_uint32    guestVlanId;    /*Guest Vlan Id on the interface */
  L7_uint32    voiceVlanId;    /* To enable Voip devices on the interface */ 
  L7_BOOL      isMABClient;    /* TRUE if client was authenticated using MAB */ 

  void         *next; /* This field must be the last one in this structure */
} dot1xLogicalPortInfo_t;


typedef enum
{
  DOT1X_PORT_BASED_AUTH = 1,
  DOT1X_MAC_BASED_AUTH
} DOT1X_AUTH_MODE_t;


/* Per-port info */
typedef struct dot1xPortInfo_s
{
  L7_DOT1X_PAE_PORT_PROTOCOL_VERSION_t paeProtocolVersion;  /* The PAE protocol version associated with this port */
  L7_uchar8            paeCapabilities;  /* Port PAE capabilities bitmask (Auth/Supp) */
  L7_uint32            maxUsers;
  L7_uint32            numUsers;

  

  /* Inter-state machine communication and initialization */
  L7_uchar8               currentId;       /* ID of current auth session (0-255) */
  L7_BOOL                 initialize;      /* Set to TRUE by mgmt to cause port initialization */
  L7_DOT1X_PORT_CONTROL_t portControlMode; /* Current control mode setting by mgmt */
  L7_BOOL                 portEnabled;     /* TRUE if port is active */
  L7_DOT1X_PORT_STATUS_t  portStatus;      /* Current authorization state of the port */
  L7_uint32               authCount;       /* number of authorized clients */  
  

  /* Authenticator PAE state machine parameters */
  L7_uint32               quietPeriod;  /* Initialization value for quietWhile timer */
  L7_uint32               txPeriod;     /* Initialization value for txWhen timer */
  L7_uint32               guestVlanPeriod; /*Initialization value for guest vlan timer*/

  /* Key Transmit state machine parameters */
  L7_BOOL   keyAvailable;  /* Set to TRUE if new global key is available to send to Supplicant;
                          * used by Key Transmit State Machine which is currently not supported
                          */
  L7_BOOL   keyTxEnabled;  /* TRUE if key transmission is enabled */

  /* Reauthentication Timer state machine parameters */
  L7_uint32 reAuthPeriod;   /* Number of seconds between periodic reauthentication */
  L7_BOOL   reAuthEnabled;  /* TRUE if reauthentication is enabled */

  L7_uint32 maxReq;         /* Max EAP Request/Identity retransmits before timing out Supplicant */
  L7_uint32 reAuthMax;    /* Max reauthentication attempts */

  /* Backend Authentication state machine parameters */
  L7_uint32 suppTimeout;    /* Initialization value for aWhile timer when timing out Supplicant */
  L7_uint32 serverTimeout;  /* Initialization value for aWhile timer when timing out Auth. Server */
  

  /* Controlled Direction state machine parameters (currently not implemented) */
  L7_DOT1X_PORT_DIRECTION_t adminControlledDirections;  /* Mgmt directional control setting (r/o) */
  L7_DOT1X_PORT_DIRECTION_t operControlledDirections;   /* Current operational direction */
  L7_BOOL                   bridgeDetected;   /* TRUE if bridge port (not edge port);
                                               * used by Controlled Directions State Machine
                                               * which is currently not supported
                                               */

  /* Key Receive state machine parameters */
  L7_BOOL rxKey;  /* Set to TRUE if EAPOL-Key frame is received from the Supplicant */
  
  L7_USER_MGR_AUTH_METHOD_t authMethod; /* Authentication method for the user of this port */

  AcquiredMask    acquiredList; /* Mask of components "acquiring" an interface */
  
  L7_uint32      incCurrentId;
  L7_BOOL        dot1xAcquire;  /* flag to keep track of dot1x acquire for a port */ 

  L7_BOOL        portMacAddrAdded; /* flag to keep track of if Client Mac address was added to the Mac DB for a port-based port */
  L7_uint32      mabEnabled;       /* L7_ENABLE if MAB has been enabled on the port and port control mode is mac-based*/
    
} dot1xPortInfo_t;

/* Supplicant port's operational Attributes */
typedef struct dot1xSupplicantPortInfo_s
{
  L7_DOT1X_PAE_PORT_PROTOCOL_VERSION_t paeProtocolVersion;  /* The PAE protocol 
                                                           version associated with this port */

  /* L7_uchar8 portControlMode;  Supplicant port mode */
  L7_BOOL   portEnabled; /* Supplicant port mode */
  

  /* Supplicant PAE state machine Timers */
  L7_uint32 startWhen;      /* Time interval to send the next start message */
  L7_uint32 heldWhile;      /* Time period in which Supplicant will not attempt to 
                               connect Authenticator */

 /* Supplicant Backend machine Timers */
  L7_uint32  authWhile;      /* Waiting time period to get an EAP challenge request from
                                Authenticator */

 /* Supplicant PAE Intra state machine communication attributes */

  L7_BOOL logoffSent; /* Set to true once the LOG off message is send */
                      /* Gets modified by the LOGOFF and DISCONNECTED
                         States */

  L7_uchar8   startCount;  /* The number of successive attempts to find the
                              Authenticator */
                           /* Gets modified by the DISCONNECTED and 
                              CONNECTING */
  L7_BOOL eapolEap;   /* Set to true on reception of Identity request */
                      /* Gets modified by the CONNECTING STATE and 
                         PDU receive function */
 L7_BOOL eapSuccess;
 L7_BOOL eapFail;    /* Set to true to indicate the eap Success or failure */
 L7_BOOL eapRestart;  /* Indication flag to indicate that Authentication is 
                         restarted*/
 L7_uchar8 sPortMode; /* Supplicant port mode */
                      /* Gets modified by the DISCONNECTED, FORCE_AUTH,
                         And FORCE_UNAUTH  states*/
 L7_uchar8  supplicantPortStatus; /* Supplicant port Authorization status */
                                  /* Gets modified by the DISCONNECTED, 
                                     FORCE_AUTH, FORCE_UNAUTH and
                                     AUTHNTICATED states*/

 /* Supplicant PAE & Backend state machine Inter communication attributes */
 L7_BOOL suppAbort; /* Set to TRUE to stop the running Authentication process*/
                    /* Modified by the DISCONNECTED and INITIALIZE states */
 L7_BOOL suppStart; /* Set to TRUE to initiate the Authentication process */
                    /* Modified by the AUTHENTICATING and IDLE states */
 L7_BOOL suppSuccess;
 L7_BOOL suppFail;
 L7_BOOL suppTimeout;
                     /* Set to TRUE by the Backend state Machine to indicate the
                        status of Authentication process. Gets modified by the
                        AUTHENTICATING, FAIL, TIMEOUT and SUCCESS
                       STATES*/

 /* State machine variables */
 DOT1X_SPM_STATES_t    spmState;
 DOT1X_SBM_STATES_t    sbmState;

 /* State machine variables to represent the Authenticator information */
 L7_uint32 currentId;  
 L7_uint32 requestType;
 L7_uchar8 dot1xChallange[DOT1X_CHALLENGE_LEN];
 L7_uint32 challangeLen;
} dot1xSupplicantPortInfo_t;






/* This structure is used to keep track of vlan addport/delport evetnts */
typedef struct dot1xMacBasedVlanParticipation_s
{
  L7_INTF_MASK_t    intfBitMask;
  L7_INTF_MASK_t    adminBitMask; /* Dot1q admin mode */
}dot1xMacBasedVlanParticipation_t;


typedef struct dot1xPortStats_s
{
  /* Authenticator Stats (9.4.2) */
  L7_uint32        authEapolFramesRx;
  L7_uint32        authEapolFramesTx;
  L7_uint32        authEapolStartFramesRx;
  L7_uint32        authEapolLogoffFramesRx;
  L7_uint32        authEapRespIdFramesRx;
  L7_uint32        authEapResponseFramesRx;
  L7_uint32        authEapReqIdFramesTx;
  L7_uint32        authEapReqFramesTx;
  L7_uint32        authInvalidEapolFramesRx;
  L7_uint32        authEapLengthErrorFramesRx;
  L7_uint32        authLastEapolFrameVersion;
  L7_enetMacAddr_t authLastEapolFrameSource;

  /* Authenticator Diagnostics (9.4.3) */
  L7_uint32 authEntersConnecting;
  L7_uint32 authEapLogoffsWhileConnecting;
  L7_uint32 authEntersAuthenticating;
  L7_uint32 authAuthSuccessWhileAuthenticating;
  L7_uint32 authAuthTimeoutsWhileAuthenticating;
  L7_uint32 authAuthFailWhileAuthenticating;
  L7_uint32 authAuthReauthsWhileAuthenticating;
  L7_uint32 authAuthEapStartsWhileAuthenticating;
  L7_uint32 authAuthEapLogoffWhileAuthenticating;
  L7_uint32 authAuthReauthsWhileAuthenticated;
  L7_uint32 authAuthEapStartsWhileAuthenticated;
  L7_uint32 authAuthEapLogoffWhileAuthenticated;
  L7_uint32 authBackendResponses;
  L7_uint32 authBackendAccessChallenges;
  L7_uint32 authBackendOtherRequestsToSupplicant;
  L7_uint32 authBackendNonNakResponsesFromSupplicant;
  L7_uint32 authBackendAuthSuccesses;
  L7_uint32 authBackendAuthFails;

  /* Supplicant Stats (9.5.3) */
  L7_uint32        suppEapolFramesRx;
  L7_uint32        suppEapolFramesTx;
  L7_uint32        suppEapolStartFramesTx;
  L7_uint32        suppEapolLogoffFramesTx;
  L7_uint32        suppEapRespIdFramesTx;
  L7_uint32        suppEapResponseFramesTx;
  L7_uint32        suppEapReqIdFramesRx;
  L7_uint32        suppEapReqFramesRx;
  L7_uint32        suppInvalidEapolFramesRx;
  L7_uint32        suppEapLengthErrorFramesRx;
  L7_uint32        suppLastEapolFrameVersion;
  L7_enetMacAddr_t suppLastEapolFrameSource;

} dot1xPortStats_t;

typedef struct dot1xPortSessionStats_s
{
  /* Authenticator Stats (9.4.4) */
  L7_uint32        sessionOctetsRx;
  L7_uint32        sessionOctetsTx;
  L7_uint32        sessionOctetsGbRx;
  L7_uint32        sessionOctetsGbTx;
  L7_uint32        sessionTime;
  L7_uchar8        userName[DOT1X_USER_NAME_LEN];
  L7_char8         sessionId[DOT1X_SESSION_ID_LEN];
} dot1xPortSessionStats_t;

extern dot1xInfo_t dot1xInfo;
extern dot1xPortInfo_t *dot1xPortInfo;
extern dot1xPortStats_t *dot1xPortStats;
extern L7_uint32 *dot1xMapTbl;
extern dot1xPortSessionStats_t  *dot1xPortSessionStats;

/* Supplicant data structures */
extern dot1xSupplicantPortInfo_t *dot1xSupplicantPortInfo;

/* Prototypes for the dot1x_db.c file */
L7_RC_t dot1xLogicalPortInfoDBInit(L7_uint32 nodeCount);
L7_RC_t dot1xLogicalPortInfoDBDeInit(void);

L7_RC_t dot1xLogicalPortInfoTakeLock(void);
L7_RC_t dot1xLogicalPortInfoGiveLock(void);

dot1xLogicalPortInfo_t *dot1xLogicalPortInfoAlloc(L7_uint32 intIfNum);
L7_RC_t dot1xLogicalPortInfoDeAlloc(dot1xLogicalPortInfo_t *node);

dot1xLogicalPortInfo_t *dot1xLogicalPortInfoGet(L7_uint32 lIntIfNum);
dot1xLogicalPortInfo_t *dot1xLogicalPortInfoGetNext(L7_uint32 lIntIfNum);

dot1xLogicalPortInfo_t *dot1xPhysicalPortInfoNodeGet(L7_uint32 intIfNum);

dot1xLogicalPortInfo_t *dot1xLogicalPortInfoFirstGet(L7_uint32 intIfNum,
                                                    L7_uint32 *lIntIfNum);
dot1xLogicalPortInfo_t *dot1xLogicalPortInfoGetNextNode(L7_uint32 intIfNum,
                                                        L7_uint32 *lIntIfNum);

#endif /* INCLUDE_DOT1X_DB_H */
