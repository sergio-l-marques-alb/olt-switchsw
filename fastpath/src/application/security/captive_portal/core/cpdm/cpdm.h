/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 **********************************************************************
 *
 * @filename     cpdm.h
 *
 * @purpose      Captive Portal Data Manager (CPDM) header
 *
 * @component    CPDB
 *
 * @comments     none
 *
 * @create       6/28/2007
 *
 * @authors      darsenault,dcaugherty,rjindal
 *
 * @end
 *
 **********************************************************************/

#ifndef INCLUDE_CPDM_H
#define INCLUDE_CPDM_H

#include <avl_api.h>
#include "captive_portal_commdefs.h"
#include "captive_portal_defaultconfig.h"
#include "cpcm_api.h"
#include "cpcm.h"
#include "cp_trap_api.h"
#include "clustering_commdefs.h"
#include "clustering_defaultconfig.h"

/* LVL7 Configuration file */
#define CPDM_CFG_FILENAME        "cp.cfg"
#define CPDM_CFG_FILENAME2       "cp_images.cfg"
#define CP_CFG_CURRENT_VERSION   1

#ifndef CPDM_WRITE_LOCK_TAKE
#define CPDM_WRITE_LOCK_TAKE(lock,wait) cpdmSemaTake(lock, wait, __FILE__, __LINE__)
#endif

#ifndef CPDM_WRITE_LOCK_GIVE
#define CPDM_WRITE_LOCK_GIVE(lock) cpdmSemaGive(lock)
#endif

#ifndef CPDM_READ_LOCK_TAKE
#define CPDM_READ_LOCK_TAKE(lock,wait) cpdmSemaTake(lock, wait, __FILE__, __LINE__)
#endif

#ifndef CPDM_READ_LOCK_GIVE
#define CPDM_READ_LOCK_GIVE(lock) cpdmSemaGive(lock)
#endif


/****************************************************************************/
/*                        OPERATIONAL DATA STRUCTURES                       */
/****************************************************************************/

/* Type and structure definitions for global Captive Portal data */
/* From the commdefs.h file:         typedef L7_ushort16 cpId_t; */


typedef struct globalPortalsData_s      /* Global data on all captive portals */
{
  L7_uint32          supported;    /* Number of supported CPs in system  */
  L7_uint32          configured;   /* Number of administratively enabled 
                                    * CPs in system
                                    */
} globalPortalsData_t;

typedef struct globalUsersData_s        /* Global data on all users */
{
  L7_uint32               supported;     /* Number authenticated users supported */
  L7_uint32               local;         /* Number of local users supported      */
  L7_uint32               authenticated; /* Current number of such users         */
} globalUsersData_t;

typedef struct globalTableDescriptor_s  /* Global data about tables */
{
  L7_uint32               capacity;                  /* Max number of entries  */
  L7_uint32               size;                      /* Current number entries */
} globalTableDescriptor_t;
 
typedef struct cpdmGlobalStatus_s
{
  L7_CP_MODE_STATUS_t     status;            /* enabled, disabled, pending..    */
  L7_CP_MODE_REASON_t     reason;            /* Reason for no CP capability now */
  L7_IP_ADDR_t            address;           /* IP address selected             */
  L7_uint32               mask;              /* IP subnet mask                  */
  L7_uchar8               intIfMac[L7_ENET_MAC_ADDR_LEN];  /* Interface MAC address */
#ifdef L7_ROUTING_PACKAGE
  L7_uint32               cpIntIfNum;        /* Loopback/Routing interface      */
  L7_uint32               cpRoutingMode;     /* Global routing mode             */
  L7_BOOL                 cpLoopBackIFInUse; /* loopback interface use status   */
#endif /* L7_ROUTING_PACKAGE */
#ifdef L7_MGMT_SECURITY_PACKAGE
  L7_BOOL                 certStatus;        /* Current status for tracking     */
#endif
  globalPortalsData_t     portals;           /* Global portal data (see above)  */
  globalUsersData_t       users;             /* Global user data (see above)    */
  globalTableDescriptor_t authFailureUsers;  /* Data on users in auth failure 
                                              * table 
                                              */
  globalTableDescriptor_t activityLog;       /* Data on activity log entries    */
} cpdmGlobalStatus_t;


typedef struct cpdmIntfCapabilityStatusData_s
{
  L7_uint32                 intfId;                    /* AVL KEY */
  L7_CP_MODE_STATUS_t       state;                     /* L7 NIM  State codes */
  L7_CP_INST_BLOCK_STATUS_t blockStatus;               /* Blocked or not */
  L7_uchar8                 sessionTimeout;            /* L7_ENABLE or L7_DISABLE */
  L7_uchar8                 idleTimeout;               /* L7_ENABLE or L7_DISABLE */
  L7_uchar8                 bandwidthUpRateControl;    /* L7_ENABLE or L7_DISABLE */
  L7_uchar8                 bandwidthDownRateControl;  /* L7_ENABLE or L7_DISABLE */
  L7_uchar8                 maxInputOctetMonitor;      /* L7_ENABLE or L7_DISABLE */
  L7_uchar8                 maxOutputOctetMonitor;     /* L7_ENABLE or L7_DISABLE */
  L7_uchar8                 bytesReceivedCounter;      /* L7_ENABLE or L7_DISABLE */
  L7_uchar8                 bytesTransmittedCounter;   /* L7_ENABLE or L7_DISABLE */
  L7_uchar8                 packetsReceivedCounter;    /* L7_ENABLE or L7_DISABLE */
  L7_uchar8                 packetsTransmittedCounter; /* L7_ENABLE or L7_DISABLE */
  L7_uchar8                 roamingSupport;            /* L7_ENABLE or L7_DISABLE */
  /* L7_uchar8                 description[CP_INTF_DESCRIPTION_MAX_LEN + 1]; */
  void *                    avlPtr;
} cpdmIntfCapabilityStatusData_t;

typedef struct cpdmIntfData_s
{
  cpdmIntfCapabilityStatusData_t cpdmIntfCapabilityStatusData[CP_INTERFACE_MAX];
} cpdmIntfData_t;

typedef struct cpdmIntfClientConnStatusData_s
{
  ifNumMACPair_t   assoc;     /* AVL Key */
  void *           avlPtr;
} cpdmIntfClientConnStatusData_t;

typedef struct cpdmClientAssocConnStatusData_s
{
  CPMACPair_t      assoc;     /* AVL Key */
  void *           avlPtr;
} cpdmClientAssocConnStatusData_t;


typedef struct cpdmIntfStatusData_s
{
  L7_uint32                 intfId;    /* AVL Key */
  L7_BOOL                   isEnabled; /* Is I/F enabled for CP ? */
  L7_INTF_WHY_DISABLED_t    why;       /* Why not? */
  L7_uint32                 users;     /* Number of CP users on I/F */
  void *                    avlPtr;
} cpdmIntfStatusData_t;

typedef struct cpdmActivityStatusData_s
{
  cpId_t                      cpId;      /* AVL Key */
  L7_CP_INST_OPER_STATUS_t    status;    /* What's this portal's status? */
  L7_CP_INST_DISABLE_REASON_t reason;    /* Why is it disabled? */
  L7_CP_INST_BLOCK_STATUS_t   blocked;   /* Is this CP instance blocked? */
  L7_uint32                   users;     /* Number of CP users on I/F */
  void *                      avlPtr;
} cpdmActivityStatusData_t;



typedef struct cpdmClientConnStatusData_s
{
  L7_enetMacAddr_t        macAddr;       /* AVL Key, associated client MAC address */
  L7_IP_ADDR_t            ip;            /* Client IP address */

  L7_enetMacAddr_t        switchMacAddr; /* MAC of switch authenticating client */
  L7_IP_ADDR_t            switchIp;      /* IP  of switch authenticating client */
  cpdmAuthWhichSwitch_t   whichSwitch;   /* auth'ed on local or peer switch? */
  uId_t                   uId;           /* user id (for locally verified logins) */
  L7_uchar8               userName[CP_USER_LOCAL_USERNAME_MAX + 1];
                                         /* user name (for radius, guest logins) */
  L7_LOGIN_TYPE_t         protocolMode;  /* FP reused */
  CP_VERIFY_MODE_t        verifyMode;
  L7_CP_USER_LOGOUT_FLAG_t userLogoutFlag;/* Enables delayed de-authentication */
  L7_uint32               intfId;
  cpId_t                  cpId;
  time_t                  sessionStart;   /* time of authorization */
  time_t                  idleStart;      /* time of current idle period */
  time_t                  lastRefresh;    /* time of last statistics refresh */
#if 1
  cpConnectionLimits_t    limits;
#else
  L7_uint32               sessionTimeout; /* time allowed in seconds before timeout */
  L7_uint32               idleTimeout;    /* time allowed in seconds before timeout */
  L7_uint32               maxBandwidthUp;
  L7_uint32               maxBandwidthDown;
  L7_uint32               maxInputOctets;
  L7_uint32               maxOutputOctets;
  L7_uint32               maxTotalOctets;
#endif
  txRxCount_t             previous;
  txRxCount_t             current;
  txRxCount_t             cumulative;
  void *                  avlPtr;
} cpdmClientConnStatusData_t;


#define CP_CLIENT_MAC_AUTH_REQUEST_TIMEOUT    10 /* seconds */
#define CP_CLUSTER_MEMBERS                    (FD_CLUSTER_MAX_PEER_SWITCHES-1) /* excluding local switch */

typedef enum
{
  CP_CLUSTER_PEER_JOINED              = 0x0001,
  CP_CLUSTER_CONTROLLER_ELECTED       = 0x0002,
  CP_CLUSTER_CLIENT_DELETED           = 0x0004, /* DEPRECATED, do not use */
  CP_CLUSTER_INST_STATUS_SEND         = 0x0008,
  CP_CLUSTER_INST_INTF_STATUS_SEND    = 0x0010,
  CP_CLUSTER_DEAUTH_CLIENT_SEND       = 0x0020, /* DEPRECATED, do not use */
  CP_CLUSTER_DEAUTH_INST_CLIENTS_SEND = 0x0040,
  CP_CLUSTER_DEAUTH_ALL_CLIENTS_SEND  = 0x0080,
  CP_CLUSTER_INST_BLOCK_STATUS_SEND   = 0x0100,
  CP_CLUSTER_CLIENT_AUTH_REPLY_SEND   = 0x0200,
  CP_CLUSTER_CONFIG_SEND              = 0x0400,
  CP_CLUSTER_CONFIG_RX                = 0x0800
} CP_CLUSTER_FLAG_t;

typedef enum
{
  CP_CLUSTER_RESEND_REQ_CLIENT_NOTIFY = 0x001,
  CP_CLUSTER_RESEND_REQ_CONN_CLIENTS  = 0x002,
  CP_CLUSTER_RESEND_REQ_INST_INTF     = 0x004,
  CP_CLUSTER_RESEND_CLIENT_NOTIFY     = 0x008,
  CP_CLUSTER_RESEND_CONN_CLIENTS      = 0x010,
  CP_CLUSTER_RESEND_INST_INTF         = 0x020
} CP_CLUSTER_RESEND_FLAG_t;

typedef struct cpdmClusterConfigCmd_s
{
  clusterConfigCmd            cmd;
} cpdmClusterConfigCmd_t;

typedef struct cpdmPeerSwClientAuthInfo_s
{
  L7_enetMacAddr_t            swMacAddr;      /* MAC of switch authenticating client */
  L7_enetMacAddr_t            macAddr;        /* client MAC address */
  L7_uint32                   cpId;           /* CP instance 1 - 10 */
  L7_uint32                   intf;           /* internal interface number */
  L7_uint32                   sessionTime;    /* time allowed in seconds before timeout */
  L7_char8                    uName[CP_USER_LOCAL_USERNAME_MAX+1];
  L7_char8                    pwd[CP_USER_LOCAL_PASSWORD_MAX+1];
  L7_uint64                   bytesTx;
  L7_uint64                   bytesRx;
  L7_uint64                   pktsTx;
  L7_uint64                   pktsRx;
  L7_uint32                   userUpRate;
  L7_uint32                   userDownRate;
  L7_BOOL                     authPending;
} cpdmPeerSwClientAuthInfo_t;

typedef struct cpdmLocalSwStatus_s
{
  L7_enetMacAddr_t            macAddr;        /* local switch MAC address */
  L7_uchar8                   status;         /* CP_CLUSTER_SWITCH_STATUS_t values */
  L7_uint32                   cpId;
  L7_uint32                   intf;
  L7_enetMacAddr_t            authClient;
  L7_enetMacAddr_t            peer;
  L7_uint32                   authReqTime;
  L7_uint32                   authReplyTime;
  L7_BOOL                     userInitiated;  /* client deletion, based on user initiation or msg processing */
} cpdmLocalSwStatus_t;

typedef struct cpdmPeerSwClientStatus_s
{
  L7_enetMacAddr_t            macAddr;        /* key, client MAC address */
  L7_IP_ADDR_t                ip;             /* Client IP address */
  L7_enetMacAddr_t            swMac;          /* MAC of switch authenticating client */
  L7_IP_ADDR_t                swIP;           /* IP  of switch authenticating client */
  L7_ushort16                 cpId;
  void                       *avlPtr;         /* MUST be last field, reserved for AVL tree */
} cpdmPeerSwClientStatus_t;

typedef struct cpdmPeerSwCPStatus_s
{
  L7_uchar8                   status;         /* CP_CLUSTER_CP_STATUS_t values */
  L7_uint32                   users;          /* number of CP users */
} cpdmPeerSwCPStatus_t;

typedef struct cpdmPeerSwIntfStatus_s
{
  L7_uchar8                   status;         /* CP_CLUSTER_INTF_STATUS_t values */
  L7_uint32                   users;          /* number of CP users on this interface */
} cpdmPeerSwIntfStatus_t;

typedef struct cpdmPeerSwStatus_s
{
  L7_enetMacAddr_t            macAddr;        /* Key, cluster switch MAC address */
  L7_IP_ADDR_t                ipAddr;         /* Cluster switch IP Address */
  L7_uchar8                   status;         /* L7_CPDM_CLUSTER_SWITCH_STATUS_t values */
  cpdmPeerSwCPStatus_t        cp[CP_ID_MAX];
  cpdmPeerSwIntfStatus_t      cpIf[CP_INTERFACE_MAX];
  void                       *avlPtr;         /* MUST be last field, reserved for AVL tree */
} cpdmPeerSwStatus_t;

typedef struct cpdmCkptData_s
{
  L7_enetMacAddr_t            macAddr;
  L7_IP_ADDR_t                ip;
  L7_uint32                   intf;
  L7_uchar8                   uName[CP_USER_LOCAL_USERNAME_MAX+1];
  L7_uint32                   sessionTimeout; /* used on backup unit */
  L7_uint32                   stackUpTimeAtClientConn; /* time when client first connects, used on backup unit during failovers */
  L7_BOOL                     flag;
  void                       *avlPtr;
} cpdmCkptData_t;

typedef struct cpdmOprData_s
{
  cpdmGlobalStatus_t              cpGlobalStatus;
  cpdmIntfCapabilityStatusData_t  cpIntfCapabilityStatusData;
  cpdmIntfData_t                  cpIntfData;
  cpcmAuthInProgressStatusData_t  cpAuthInProgressStatusData;
  cpdmClientAssocConnStatusData_t cpClientAssocConnStatusData;
  cpdmClientConnStatusData_t      cpClientConnStatusData;
  cpdmLocalSwStatus_t             cpLocalSw;
  cpdmPeerSwClientStatus_t        cpPeerSwClientTree;
  cpdmPeerSwStatus_t              cpPeerSwTree;
  L7_enetMacAddr_t                cpNewPeer;
  L7_enetMacAddr_t                cpCtrlMacAddr;
  CP_CLUSTER_FLAG_t               cpClusterFlag;
  cpdmPeerSwClientAuthInfo_t      cpClientAuthInfo;
  L7_enetMacAddr_t                cpCfgPeerTx;
  cpdmClusterConfigCmd_t          cpConfigCmd;
  CP_CLUSTER_RESEND_FLAG_t        cpClusterResendFlag;

  PORTEVENT_MASK_t                nimEventMask;
  L7_BOOL                         cpWarmRestart;     /* set to L7_TRUE when the previous restart is warm */
  avlTree_t                       cpCkptTree;        /* ckptd clients' tree */
  void                           *cpCkptPendingSema; /* set to L7_TRUE when ckpt is pending and/or during data sync */
  L7_BOOL                         cpBackupElected;   /* set to L7_TRUE when a new backup is elected */
  L7_BOOL                         cpCkptReqd;        /* set to L7_TRUE when any client info needs to be ckptd */
  L7_BOOL                         cpCkptInProg;      /* set to L7_TRUE when ckpt is in progress */

} cpdmOprData_t;




/****************************************************************************/
/*                         PERSISTENT DATA STRUCTURES                       */
/****************************************************************************/

typedef struct
{
  L7_uint32 cpMode;
  L7_uint32 peerSwitchStatsReportInterval;
  L7_uint32 sessionTimeout;
  L7_uint32 httpPort;
  L7_uint32 httpsPort1; /* default */
  L7_uint32 httpsPort2; /* additional */
  L7_uint32 trapFlags;
} cpdmGlobal_t;

typedef struct
{
  uId_t     uId; 
  L7_char8  loginName[CP_USER_LOCAL_USERNAME_MAX + 1];
  L7_char8  password[L7_ENCRYPTED_PASSWORD_SIZE];
  L7_char8  groupName[CP_USER_LOCAL_USERGROUP_MAX + 1];
  L7_uint32 sessionTimeout;
  L7_uint32 idleTimeout;
  L7_uint32 maxBandwidthUp;
  L7_uint32 maxBandwidthDown;
  L7_uint32 maxInputOctets;
  L7_uint32 maxOutputOctets;
  L7_uint32 maxTotalOctets;
} cpdmUser_t;

typedef struct
{
  cpdmUser_t cpUsers[FD_CP_LOCAL_USERS_MAX];
} cpdmUsersData_t;

typedef struct
{
  gpId_t        gpId; 
  L7_char8      groupName[CP_USER_LOCAL_USERGROUP_MAX + 1];
} cpdmUserGroupData_t;

typedef struct
{
  uId_t         uId;
  gpId_t        gpId; 
} cpdmUserGroupAssocData_t;

typedef struct
{
  cpId_t        cpId; 
  webId_t       webId;
} cpLocaleKey_t;

typedef struct
{
  L7_char8      code[CP_LANG_CODE_MAX+1];
  L7_char8      link[CP_LOCALE_LINK_MAX+1];
} cpLocaleCodeLink_t;

typedef struct
{ /* general web customization content */
  cpLocaleKey_t    key;
  L7_char8         accountImageName[L7_MAX_FILENAME + 1];
  L7_char8         brandingImageName[L7_MAX_FILENAME + 1];
  L7_char8         backgroundImageName[L7_MAX_FILENAME + 1];
  L7_char8         browserTitleText[CP_BROWSER_TITLE_TEXT_MAX + 1];
  L7_char8         langCode[CP_LANG_CODE_MAX + 1];
  L7_char8         localeLink[CP_LOCALE_LINK_MAX + 1];
  L7_char8         titleText[CP_TITLE_TEXT_MAX + 1];
  L7_char8         accountLabel[CP_ACCOUNT_LABEL_MAX + 1];
  L7_char8         fontList[CP_FONT_LIST_MAX + 1];
  L7_char8         userLabel[CP_USER_LABEL_MAX + 1];
  L7_char8         passwordLabel[CP_PASSWORD_LABEL_MAX + 1];
  L7_char8         buttonLabel[CP_BUTTON_LABEL_MAX + 1];
  L7_char8         instructionalText[CP_INSTRUCTIONAL_TEXT_MAX + 1];
  L7_char8         aupText[CP_AUP_TEXT_MAX + 1];
  L7_char8         acceptText[CP_ACCEPT_TEXT_MAX + 1];
  L7_char8         scriptText[CP_SCRIPT_TEXT_MAX + 1];
  L7_char8         popupText[CP_POPUP_TEXT_MAX + 1];
                    /* message response content */
  L7_char8         noAcceptMsg[CP_MSG_TEXT_MAX + 1];
  L7_char8         wipMsg[CP_MSG_TEXT_MAX + 1];
  L7_char8         deniedMsg[CP_MSG_TEXT_MAX + 1];
  L7_char8         resourceMsg[CP_MSG_TEXT_MAX + 1];
  L7_char8         timeoutMsg[CP_MSG_TEXT_MAX + 1];
                    /* welcome web page content */
  L7_char8         welcomeTitleText[CP_WELCOME_TITLE_TEXT_MAX + 1];   
  L7_char8         welcomeText[CP_WELCOME_TEXT_MAX + 1];
                    /* User Logout Window content */
  L7_char8         logoutBrowserTitleText[CP_LOGOUT_BROWSER_TITLE_TEXT_MAX + 1];
  L7_char8         logoutTitleText[CP_LOGOUT_TITLE_TEXT_MAX + 1];
  L7_char8         logoutContentText[CP_LOGOUT_CONTENT_TEXT_MAX + 1];
  L7_char8         logoutButtonLabel[CP_LOGOUT_BUTTON_LABEL_MAX + 1];
  L7_char8         logoutConfirmText[CP_LOGOUT_CONFIRM_TEXT_MAX + 1];
                    /* User Logout Success page content */
  L7_char8         logoutSuccessBrowserTitleText[CP_LOGOUT_SUCCESS_BROWSER_TITLE_TEXT_MAX + 1];
  L7_char8         logoutSuccessTitleText[CP_LOGOUT_SUCCESS_TITLE_TEXT_MAX + 1];
  L7_char8         logoutSuccessContentText[CP_LOGOUT_SUCCESS_CONTENT_TEXT_MAX + 1];
  L7_char8         logoutSuccessBackgroundImageName[L7_MAX_FILENAME + 1];
} cpdmCustomLocale_t;

/* This structure is used for loading default values, and resembles
 * the structure above except for two key differences: no key, and the 
 * use of pointers instead of arrays.  
 * This definition should not be exported publicly; there is no reason 
 * for its use outside this module.
 */
typedef struct
{ 
  L7_char8        * accountImageName;
  L7_char8        * brandingImageName;
  L7_char8        * backgroundImageName;
  L7_char8        * browserTitleText;
  L7_char8        * langCode;
  L7_char8        * localeLink;
  L7_char8        * titleText;
  L7_char8        * accountLabel;
  L7_char8        * fontList;
  L7_char8        * userLabel;
  L7_char8        * passwordLabel;
  L7_char8        * buttonLabel;
  L7_char8        * instructionalText;
  L7_char8        * aupText;
  L7_char8        * acceptText;
  L7_char8        * noAcceptMsg;
  L7_char8        * wipMsg;
  L7_char8        * deniedMsg;
  L7_char8        * resourceMsg;
  L7_char8        * timeoutMsg;
  L7_char8        * welcomeTitleText;
  L7_char8        * welcomeText;
  L7_char8        * scriptText;
  L7_char8        * popupText;
  L7_char8        * logoutBrowserTitleText;
  L7_char8        * logoutTitleText;
  L7_char8        * logoutContentText;
  L7_char8        * logoutButtonLabel;
  L7_char8        * logoutConfirmText;
  L7_char8        * logoutSuccessBrowserTitleText;
  L7_char8        * logoutSuccessTitleText;
  L7_char8        * logoutSuccessContentText;
  L7_char8        * logoutSuccessBackgroundImageName;
} cpdmCustomLocaleSkeleton_t;

typedef struct cpdmConfigData_s
{
  cpId_t             cpId; 
  L7_char8           name[CP_NAME_MAX + 1];
  L7_CP_MODE_STATUS_t  enabled;
  L7_ushort16        httpPort; 
  L7_LOGIN_TYPE_t    protocolMode; /* FP reused */
  CP_VERIFY_MODE_t   verifyMode;
  L7_uchar8          userLogoutMode;
  gpId_t             gpId;
  L7_char8           radiusAuthServer[CP_RADIUS_AUTH_SERVER_MAX + 1];
  L7_BOOL            radiusAccounting;
  L7_ushort16        localeCount; 
  L7_char8           redirectURL[CP_WELCOME_URL_MAX + 1];
  L7_uchar8          redirectMode;
  L7_uint32          userUpRate;        /* WISPr Max Bandwidth Up */
  L7_uint32          userDownRate;      /* WISPr Max Bandwidth Down */
  L7_uint64          maxInputOctets;
  L7_uint64          maxOutputOctets;
  L7_uint64          maxTotalOctets;
  L7_uint32          sessionTimeout;
  L7_uint32          idleTimeout;
  L7_uint32          intrusionThreshold;
  L7_char8           foregroundColor[CP_FOREGROUND_COLOR_MAX + 1]; 
  L7_char8           backgroundColor[CP_BACKGROUND_COLOR_MAX + 1]; 
  L7_char8           separatorColor[CP_SEPARATOR_COLOR_MAX + 1]; 
} cpdmConfigData_t;

typedef struct cpdmRAMCfgDataToStore_s
{
  L7_fileHdr_t  hdr;
  L7_uchar8     images[L7_CAPTIVE_PORTAL_IMAGES_NVRAM_SIZE];
  L7_uint32     eoi; /* End of images data */
  L7_uint32     eot; /* Temp storage aid for intermediate CLI commands */
  L7_uint32     checksum;
} cpdmRAMCfgDataToStore_t;

typedef struct cpdmData_s
{
  L7_fileHdr_t              hdr;
  cpdmGlobal_t              cpGlobalData;
  cpdmUsersData_t           cpUsersData;
  cpdmUserGroupData_t       cpUserGroupData[FD_CP_USER_GROUP_MAX];
  cpdmUserGroupAssocData_t  cpUserGroupAssocData[FD_CP_USER_GROUP_ASSOC_MAX];
  cpdmConfigData_t          cpConfigData[FD_CP_CONFIG_MAX];
  cpdmCustomLocale_t        cpLocaleData[FD_CP_CONFIG_MAX * FD_CP_CUSTOM_LOCALE_MAX];
  ifNumCPPair_t             cpInterfaceAssocData[CP_INTERFACE_MAX];
  L7_uint32                 checksum;
} cpdmCfgData_t;

extern cpdmCfgData_t            *cpdmCfgData;
extern cpdmOprData_t            *cpdmOprData;
extern cpdmRAMCfgDataToStore_t  *cpdmRAMCfgDataToStore;

extern void *cpdmSema;

/*********************************************************************
 *
 * @purpose  Take the write lock.
 *
 * @param   rwLock - CPDM Read/Write Lock. 
 * @param   timeout - Timeout option
 * @param   file - File that got the lock.
 * @param   line_num - Line number that got the lock.
 *
 * returns   return code from osapiWriteLockTake
 *
 * @comments 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmSemaTake (void *sem, L7_int32 timeout, 
    L7_uchar8 *file, L7_uint32 line_num);

/*********************************************************************
 *
 * @purpose  Give the write lock.
 *
 * @param   rwLock - CPDM Read/Write Lock. 
 *
 * returns   return code from osapiWriteLockGive
 *
 * @comments 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cpdmSemaGive (void *sem);

#endif /* INCLUDE_CPDM_H */

