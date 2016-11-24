
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename   user_mgr.h
*
* @purpose    User Manager file
*
* @component  user_mgr component
*
* @comments   none
*
* @create     09/26/2002
*
* @author     Jill Flanagan
*
* @end
*
**********************************************************************/

#ifndef USER_MGR_H
#define USER_MGR_H

#include "l7_common.h"
#include "cli_web_exports.h"
#include "snmp_exports.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"
#include "defaultconfig.h"
#include "default_cnfgr.h"
#include "user_mgr_util.h"
#include "user_mgr_api.h"
#include "nvstoreapi.h"
#include "cnfgr.h"
#include "nimapi.h"
#include "user_manager_exports.h"

#define USER_MGR_CFG_FILENAME  "user_mgr.cfg"

#define USER_MGR_CFG_VER_1      0x1
#define USER_MGR_CFG_VER_2      0x2   /* Development version */
#define USER_MGR_CFG_VER_3      0x3
#define USER_MGR_CFG_VER_4      0x4
#define USER_MGR_CFG_VER_5      0x5
#define USER_MGR_CFG_VER_6      0x6
#define USER_MGR_CFG_VER_7      0x7
#define USER_MGR_CFG_VER_8      0x8
#define USER_MGR_CFG_VER_CURRENT        USER_MGR_CFG_VER_8

/* TO DO: Clean this up to save space. This only needs to be as many bits
    as L7_MAX_LOGINS */

typedef L7_uint32 userMgrPortUser_t;

typedef struct
{
  nimConfigID_t       configID;
  userMgrPortUser_t   portUsers;
} userMgrPortUserCfg_t;

typedef L7_char8 L7_authMethodType;

typedef struct
{
  L7_char8 name[ L7_MAX_APL_NAME_SIZE + 1 ];
  L7_authMethodType methodList[ L7_MAX_AUTH_METHODS ];
  L7_ushort16 inUse;

} APL_t;

typedef struct
{
  L7_char8 APLName[ L7_MAX_APL_NAME_SIZE + 1 ];
  L7_COMPONENT_IDS_t componentId;
} user_component_auth_t;

typedef struct
{
  L7_char8       loginName[L7_LOGIN_SIZE];
  L7_char8       password[L7_ENCRYPTED_PASSWORD_SIZE];
  L7_char8       deleteMode;        /* Indicates whether this user can be deleted or not. FALSE indicates user cannot be deleted */
  L7_uint32      accessMode;
  L7_uint32      loginStatus;
  L7_uint32      blockStatus;
  L7_ushort16    passwdLockoutCount;
  L7_ushort16    passwdFailureTotal; /* keep a running total number of failures */
  L7_ushort16    passwdNumLockouts;  /* number of times user account has been locked */
  L7_ushort16    passwdHistoryCount;
  L7_char8       passwdHistory[L7_MAX_HISTORY_SIZE][L7_ENCRYPTED_PASSWORD_SIZE];
  L7_uint32      passwdTimeStamp;
  /* SNMPv3 Fields to be moved to separate structure */
  L7_uint32      authProt;
  L7_uint32      encryptProt;
  L7_char8       encryptKey[L7_ENCRYPTION_KEY_SIZE];
  L7_uint32      snmpv3AccessMode;
  /* end of SNMPv3 Fields */
  user_component_auth_t userAuth[ L7_APL_COMPONENT_COUNT ];

} logins_t;

typedef struct
{
  L7_uchar8       aplName[L7_MAX_APL_NAME_SIZE + 1];
  L7_char8        password[L7_ENCRYPTED_PASSWORD_SIZE]; /* line/enable password ?TODO */
  /* should we have another member for default list? TODO */
} access_line_info_t;

#define L7_USER_MGR_ZERO       0
#define L7_USER_MGR_ONE        1
#define L7_SECS_PER_MIN        60

typedef struct
{
  L7_uint32             passwdTimeStamp;
  L7_char8              loginName[L7_LOGIN_SIZE];
  L7_inet_addr_t        location;
  L7_ACCESS_LINE_t      protocol;
  L7_BOOL               inUse;
} login_history_t;

typedef struct
{
  APL_t                 loginAuthLists[L7_MAX_LOGIN_AUTHLIST_COUNT];
  APL_t                 enableAuthLists[L7_MAX_ENABLE_AUTHLIST_COUNT];
  /* following lists are for internal design; externally the
     authentication methods are not specified as a list */
  APL_t                 httpAuthLists[L7_MAX_HTTP_AUTHLIST_COUNT];
  APL_t                 httpsAuthLists[L7_MAX_HTTPS_AUTHLIST_COUNT];
  APL_t                 dot1xAuthLists[L7_MAX_DOT1X_AUTHLIST_COUNT];

  L7_char8              authListName[L7_ACCESS_LINE_NUM][L7_ACCESS_LEVEL_NUM][L7_MAX_APL_NAME_SIZE + 1];
} userMgrAuthListInfo_t;

typedef struct
{
  L7_fileHdr_t          cfgHdr;
  logins_t              systemLogins[L7_MAX_USERS_LOGINS];
  user_component_auth_t nonConfiguredUserAuth[ L7_APL_COMPONENT_COUNT ];
  userMgrPortUserCfg_t     portData[L7_MAX_INTERFACE_COUNT];
  L7_char8              linePassword[L7_ACCESS_LINE_NUM][L7_ENCRYPTED_PASSWORD_SIZE];
  L7_char8              enablePassword[L7_ENABLE_LEVEL_NUM][L7_ENCRYPTED_PASSWORD_SIZE];
  userMgrAuthListInfo_t authListInfo;
  access_line_info_t    lineInfo[L7_ACCESS_LINE_NUM][L7_ACCESS_LEVEL_NUM];
  L7_ushort16   minPasswdLength;
  L7_ushort16   passwdAgingDays;
  L7_ushort16   userLockoutCount;
  L7_ushort16   passwdHistoryLength;
  login_history_t   loginHistory[L7_MAX_LOGIN_HISTORY_SIZE];
  L7_uint32          latest;
  L7_uint32             checkSum;         /* keep this as last 4 bytes */

} userMgrCfgData_t;

typedef enum
{
  USER_MANAGER_COMPONENT_OFFSET = 0,
  DOT1X_COMPONENT_OFFSET
} USER_AUTH_OFFSET;

#define SNMP_USER_SUPPORTED  (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNMP_COMPONENT_ID, L7_SNMP_USER_SUPPORTED))

#define SNMP_AUTH_SUPPORTED  (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNMP_COMPONENT_ID, L7_SNMP_USER_AUTHENTICATION_FEATURE_ID))

#define SNMP_ENCRYPT_SUPPORTED  (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNMP_COMPONENT_ID, L7_SNMP_USER_ENCRYPTION_FEATURE_ID))

extern logins_t FD_logins[L7_MAX_USERS_LOGINS];
extern user_component_auth_t FD_nonConfiguredUserAuth[FD_NONCONFIGUREDAUTH_SIZE];

/*********************************************************************
* @purpose  Callback function to process interface state changes.
*
* @param    intIfnum    @b{(input)} internal interface whose state has changed
* @param    event       @b{(input)} new state (see L7_PORT_EVENTS_t for list)
* @param    correlator  @b{(input)} correlator for the event
*
* @returns  L7_SUCCESS
*
* @notes    Only Delete and Create messages are processed
*
* @end
*********************************************************************/
L7_RC_t userMgrIntfStateChangeCallback(L7_uint32 intIfNum, L7_uint32 event,NIM_CORRELATOR_t correlator);

/*********************************************************************
* @purpose Initialize the userMgr for Phase 1
*
* @param   void
*
* @returns L7_SUCCESS  Phase 1 completed
* @returns L7_FAILURE  Phase 1 incomplete
*
* @notes  If phase 1 is incomplete, it is up to the caller to call the fini
*         function if desired.  If this fails, it is due to an inability to
*         to acquire resources.
*
* @end
*********************************************************************/
L7_RC_t userMgrPhaseOneInit(void);

/*********************************************************************
* @purpose  Release all resources collected during phase 1
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void userMgrPhaseOneFini(void);

/*********************************************************************
* @purpose Initialize the userMgr for Phase 2
*
* @param   void
*
* @returns L7_SUCCESS  Phase 2 completed
* @returns L7_FAILURE  Phase 2 incomplete
*
* @notes  If phase 2 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
L7_RC_t userMgrPhaseTwoInit(void);

/*********************************************************************
* @purpose Free the resource for phase 2
*
* @param   void
*
* @returns void
*
* @notes  If phase 2 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
void userMgrPhaseTwoFini(void);

/*********************************************************************
* @purpose Initialize the userMgr for Phase 3
*
* @param   void
*
* @returns L7_SUCCESS  Phase 3 completed
* @returns L7_FAILURE  Phase 3 incomplete
*
* @notes  If phase 3 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
L7_RC_t userMgrPhaseThreeInit(void);

/*********************************************************************
* @purpose Reset the userMgr to prior to phase 3
*
* @param   void
*
* @returns void
*
* @notes  If phase 3 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
void userMgrPhaseThreeFini(void);

/*********************************************************************
* @purpose  Phase userMgr to configurator phase unconfigurator
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void userMgrUnconfigure(void);

/* TODO fn header */
APL_t *userMgrAuthListFromIndexGet(L7_ACCESS_LINE_t line, L7_ACCESS_LEVEL_t level, L7_uint32 index);

/* TODO fn header */
APL_t *userMgrAuthListFromNameGet(L7_ACCESS_LINE_t line, L7_ACCESS_LEVEL_t level, L7_char8 *name);

#endif /* USER_MGR_H */
