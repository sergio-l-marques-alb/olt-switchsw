/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_cfg.h
*
* @purpose   dot1x configuration include file
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
#ifndef INCLUDE_DOT1X_CFG_H
#define INCLUDE_DOT1X_CFG_H

#define DOT1X_CFG_FILENAME     "dot1x.cfg"
#define DOT1X_CFG_VER_1        0x1
#define DOT1X_CFG_VER_2        0x2
#define DOT1X_CFG_VER_3        0x3
#define DOT1X_CFG_VER_4        0x4
#define DOT1X_CFG_VER_5        0x5
#define DOT1X_CFG_VER_6        0x6
#define DOT1X_CFG_VER_7        0x7
#define DOT1X_CFG_VER_8        0x8
#define DOT1X_CFG_VER_CURRENT  DOT1X_CFG_VER_8

#define DOT1X_IS_READY (((dot1xCnfgrState == DOT1X_PHASE_INIT_3) || \
                        (dot1xCnfgrState == DOT1X_PHASE_EXECUTE) || \
                        (dot1xCnfgrState == DOT1X_PHASE_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))

#define DOT1X_DEBUG_CFG_FILENAME "dot1x_debug.cfg"
#define DOT1X_DEBUG_CFG_VER_1          0x1
#define DOT1X_DEBUG_CFG_VER_CURRENT    DOT1X_DEBUG_CFG_VER_1
                                                                                    
typedef enum {
  DOT1X_PHASE_INIT_0 = 0,
  DOT1X_PHASE_INIT_1,
  DOT1X_PHASE_INIT_2,
  DOT1X_PHASE_WMU,
  DOT1X_PHASE_INIT_3,
  DOT1X_PHASE_EXECUTE,
  DOT1X_PHASE_UNCONFIG_1,
  DOT1X_PHASE_UNCONFIG_2,
} dot1xCnfgrState_t;

typedef enum
{
  DOT1X_LOGICAL_PORT = 0,
  DOT1X_PHYSICAL_PORT
}dot1xPortType_t;

typedef struct dot1xSupplicantPortCfg_s
{

  L7_DOT1X_PORT_DIRECTION_t adminControlledDirections;  /* Mgmt directional control setting (r/o) */
  L7_DOT1X_PORT_CONTROL_t   portControlMode;     /* Current control mode setting by mgmt */
  L7_uint32 maxStart;       /* maximum number of successive EAPOL start messages
                                              That are send */
  L7_uint32 startPeriod;    /* Initialization value for startWhen timer */
  L7_uint32 heldPeriod;    /* Initialization value for heldWhile timer */
  L7_uint32 authPeriod;    /* Initialization value for authWhile timer */

  /* user Details */
  L7_uchar8 dot1xSupplicantUserName [DOT1X_USER_NAME_LEN];
  L7_uint32 dot1xSupplicantUserNameLen;

#if 0
  /* user Details */
  L7_uchar8 dot1xSupplicantUserName [DOT1X_USER_NAME_LEN];
  L7_uint32 dot1xSupplicantUserNameLength;
  L7_uint32 dot1xSupplicantUserIndex;
  L7_uchar8 dot1xSupplicantChallenge [DOT1X_CHALLENGE_LEN];
#endif

} dot1xSupplicantPortCfg_t;

typedef struct dot1xPortCfg_s
{
  nimConfigID_t configId;     /* NIM config ID for this interface*/
  L7_uchar8 paeCapabilities;  /* PAE capabilitiy /* Supplicant or Authenticator */
  L7_DOT1X_PORT_DIRECTION_t adminControlledDirections;  /* Mgmt directional control setting (r/o) */
  L7_DOT1X_PORT_CONTROL_t   portControlMode;     /* Current control mode setting by mgmt */
  L7_uint32 quietPeriod;    /* Initialization value for quietWhile timer */
  L7_uint32 txPeriod;       /* Initialization value for txWhen timer */
  L7_uint32 suppTimeout;    /* Initialization value for aWhile timer when timing out Supplicant */
  L7_uint32 serverTimeout;  /* Initialization value for aWhile timer when timing out Auth. Server */
  L7_uint32 maxReq;         /* Max EAP Request/Identity retransmits before time out Supplicant */
  L7_uint32 reAuthPeriod;   /* Number of seconds between periodic reauthentication */
  L7_BOOL   reAuthEnabled;  /* TRUE if reauthentication is enabled */
  L7_BOOL   keyTxEnabled;   /* TRUE if key transmission is enabled */
  L7_uint32 guestVlanId;    /* Guest Vlan Id for the interface*/
  L7_uint32 guestVlanPeriod; /* Guest Vlan Period for the interface*/
  L7_uint32 maxUsers;        /*Maximum no. users in Mac-Based Authentication */
  L7_uint32 unauthenticatedVlan;  /* Vlan id for unauthenticated supplicants */
  dot1xSupplicantPortCfg_t supplicantPortCfg; /* Supplicant port configurations */
  L7_uint32 mabEnabled;           /*enabled if MAB is enabled for the port*/
  
} dot1xPortCfg_t;

typedef struct dot1xCfg_s
{
  L7_fileHdr_t             cfgHdr;
  L7_uint32                dot1xAdminMode;    /* Global admin mode (SystemAuthControl) */
  L7_uint32                dot1xLogTraceMode; /* Enable/disable log file tracing */
  L7_uint32                vlanAssignmentMode;/* Global mode to enable vlan assignment */
  dot1xPortCfg_t           dot1xPortCfg[L7_DOT1X_INTF_MAX_COUNT]; /* Per-port config info */
  L7_uint32              checkSum;        /* check sum of config file NOTE: needs to be last entry */
} dot1xCfg_t;

extern dot1xCfg_t *dot1xCfg;

typedef struct dot1xDebugCfgData_s 
{
  L7_BOOL dot1xDebugPacketTraceTxFlag;
  L7_BOOL dot1xDebugPacketTraceRxFlag;
} dot1xDebugCfgData_t;

typedef struct dot1xDebugCfg_s
{
  L7_fileHdr_t          hdr;
  dot1xDebugCfgData_t   cfg;
  L7_uint32             checkSum;
} dot1xDebugCfg_t;

extern L7_RC_t dot1xSave(void);
extern L7_BOOL dot1xHasDataChanged(void);
extern void dot1xResetDataChanged(void);

extern L7_RC_t dot1xCfgDump(void);
extern void dot1xBuildDefaultConfigData(L7_uint32 ver);
extern void dot1xBuildDefaultIntfConfigData(nimConfigID_t *configId, dot1xPortCfg_t *pCfg);
extern void dot1xBuildDefaultSupplicantIntfConfigData(nimConfigID_t *configId, dot1xPortCfg_t *pCfg);

extern L7_RC_t dot1xApplyConfigData(void);
extern L7_RC_t dot1xApplyPortConfigData(L7_uint32 intIfNum);
extern L7_RC_t dot1xPortReset(L7_uint32 intIfNum);
extern L7_RC_t dot1xPortInfoInitialize(L7_uint32 intIfNum,L7_BOOL flag);

/* Supplicant APIs */
extern L7_RC_t dot1xSupplicantPortReset(L7_uint32 intIfNum);
extern L7_RC_t dot1xSupplicantPortInfoInitialize(L7_uint32 intIfNum,L7_BOOL flag);


extern void dot1xApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);
extern L7_RC_t dot1xInit(L7_CNFGR_CMD_DATA_t *pCmdData);
extern void dot1xInitUndo();
extern L7_RC_t dot1xCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                            L7_CNFGR_ERR_RC_t   *pReason );
extern L7_RC_t dot1xCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                            L7_CNFGR_ERR_RC_t   *pReason );
extern L7_RC_t dot1xCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                            L7_CNFGR_ERR_RC_t   *pReason );
extern void dot1xCnfgrFiniPhase1Process();
extern void dot1xCnfgrFiniPhase2Process();
extern void dot1xCnfgrFiniPhase3Process();
extern L7_RC_t dot1xCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason );
extern L7_RC_t dot1xCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason );
extern void dot1xCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData);

extern L7_RC_t dot1xLogicalPortInfoSetPortInfo(dot1xLogicalPortInfo_t *logicalPortInfo);
extern L7_RC_t dot1xLogicalPortInfoInitialize(dot1xLogicalPortInfo_t *logicalPortInfo);
extern L7_uint32 dot1xPhysPortGet(L7_uint32 intIfNum);
extern L7_RC_t dot1xLogicalPortReset(dot1xLogicalPortInfo_t *logicalPortInfo);


/* dot1x_migrate.c */

extern void dot1xMigrateConfigData(L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer); 

#endif /* INCLUDE_DOT1X_CFG_H */
