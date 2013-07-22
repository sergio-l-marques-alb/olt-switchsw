/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     sshd_cfg.h
*
* @purpose      SSH configuration header
*
* @component    sshd
*
* @comments     none
*
* @create       09/15/2003
*
* @author       dcbii
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_SSHD_CFG_H
#define INCLUDE_SSHD_CFG_H

#include "l7_cnfgr_api.h"

#include "sshd_include.h"
#include "sshd_exports.h"
#include "sshd_api.h"

/* LVL7 Configuration file */

#define SSHD_CFG_FILENAME    "sshd.cfg"
#define SSHD_CFG_VER_1       0x1
#define SSHD_CFG_VER_2       0x2
#define SSHD_CFG_VER_3       0x3
#define SSHD_CFG_VER_CURRENT SSHD_CFG_VER_3

typedef enum
{
    SSHD_PHASE_INIT_0 = 0,
    SSHD_PHASE_INIT_1,
    SSHD_PHASE_INIT_2,
    SSHD_PHASE_WMU,
    SSHD_PHASE_INIT_3,
    SSHD_PHASE_EXECUTE,
    SSHD_PHASE_UNCONFIG_1,
    SSHD_PHASE_UNCONFIG_2,

} sshdCnfgrState_t;

typedef struct
{
    L7_BOOL sshdSave;
    L7_BOOL sshdRestore;
    L7_BOOL sshdHasDataChanged;

} sshdDeregister_t;

#define SSHD_PROTO_LEVEL_DEFAULT     L7_SSHD_PROTO_LEVEL
#define SSHD_PROTO_LEVEL_ALL         L7_SSHD_PROTO_LEVEL
#define SSHD_PROTO_LEVEL_V2_ONLY     L7_SSHD_PROTO_LEVEL_V2_ONLY
#define SSHD_PROTO_LEVEL_V1_ONLY     L7_SSHD_PROTO_LEVEL_V1_ONLY
#define SSHD_EMWEB_PORT_DEFAULT      L7_SSHD_EMWEB_PORT
#define SSHD_EMWEB_SERVER_ADDR       L7_SSHD_UNSECURE_SERVER_ADDR
#define SSHD_BANNER_FILE_NAME        CLI_BANNER_FILE_NAME

/* SSHD Operational configuration data */

typedef struct sshd_global_s
{
    L7_uint32 sshdAdminMode;
    L7_uint32 sshdOperMode;
    L7_uint32 sshdListenTaskId;
    L7_uint32 sshdProtoLevel;
    L7_uint32 sshdNumSessions; /* Used for DoS attacks, not Fastpath count */
    L7_uint32 sshdMaxSessions;
    L7_uint32 sshdIdleTimeout;
    L7_uchar8 sshdKeyGenerateFlags; /* bit mask to indicate key type is being generated */
    L7_uint32 sshdPort;

} sshdGlobal_t;


typedef struct
{
  L7_char8      username[L7_SSHD_USERNAME_SIZE_MAX];
  sshdKeyType_t type;
  L7_char8      key[L7_SSHD_KEY_DATA_SIZE_MAX + 1];
} sshdPubKeyEntry_t;

/* SSHD configuration data */

typedef struct
{
    L7_uint32         sshdAdminMode;
    L7_uint32         sshdProtoLevel;
    L7_uint32         sshdMaxSessions;
    L7_uint32         sshdIdleTimeout;
    L7_uint32         sshdPort;
    L7_uint32         sshdPubKeyAuthMode;
    sshdPubKeyEntry_t sshdPubKeys[L7_SSHD_PUBKEY_USERS_MAX];

} sshdCfgData_t;

typedef struct
{
    L7_fileHdr_t  hdr;
    sshdCfgData_t cfg;
    L7_uint32     checkSum;

} sshdCfg_t;

/*
Internal function prototypes
*/

L7_RC_t sshdSave(void);
L7_BOOL sshdHasDataChanged(void);
void sshdResetDataChanged(void);
void    sshdBuildDefaultConfigData(L7_uint32 ver);
void    sshdMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);
void    sshdApplyConfigData(void);
void    sshdMigrateBuild(L7_uint32 oldVer, L7_uint32 ver, L7_char8 *buffer);
void    sshdApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);
L7_RC_t sshdInit(L7_CNFGR_CMD_DATA_t *pCmdData);
void    sshdInitUndo();
L7_RC_t sshdCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t *pReason);
L7_RC_t sshdCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t *pReason);
L7_RC_t sshdCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t *pReason);
void    sshdCnfgrFiniPhase1Process();
void    sshdCnfgrFiniPhase2Process();
void    sshdCnfgrFiniPhase3Process();
L7_RC_t sshdCnfgrNoopProccess(L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t *pReason);
L7_RC_t sshdCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t *pReason);
void    sshdCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData);

#endif /* INCLUDE_SSHD_CFG_H */

