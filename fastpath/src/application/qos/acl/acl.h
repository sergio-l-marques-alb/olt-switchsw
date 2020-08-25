/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename acl.h
*
* @purpose Access Control Lists' Utilities Header File
*
* @component Access Control List
*
* @comments
*
* @create 06/03/2002
*
* @author djohnson
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_ACL_H
#define INCLUDE_ACL_H

#include "l7_common.h"
#include "l3_addrdefs.h"
#include "acl_api.h"
#include "avl_api.h"
#include "tlv_api.h"
#include "nimapi.h"
#include "default_cnfgr.h"
#include "l7_cnfgr_api.h"
#include "usmdb_qos_acl_api.h"
#ifdef L7_PRODUCT_SMARTPATH
#include "easyacl_api.h"
#endif

#define ACL_CFG_FILENAME        "acl.cfg"
#define ACL_CFG_VER_1           0x1             /* Rel 4.0.0.2                */
#define ACL_CFG_VER_2           0x2             /* Rel 4.1.2.0                */
#define ACL_CFG_VER_3           0x3             /* Rel 4.2.1.0                */
#define ACL_CFG_VER_4           0x4             /* Rel 4.3.x (rel_i)          */
#define ACL_CFG_VER_5           0x5             /* Rel 4.4.x (rel_k)          */
#define ACL_CFG_VER_6           0x6             /* Rel 4.6.x (rel_l)          */
#define ACL_CFG_VER_CURRENT     ACL_CFG_VER_6

#define ACL_IS_READY (((aclCnfgrState == ACL_PHASE_INIT_3) || \
                                           (aclCnfgrState == ACL_PHASE_EXECUTE) || \
                                           (aclCnfgrState == ACL_PHASE_UNCONFIG_1)) ? (L7_TRUE) : (L7_FALSE))

/* Semaphore take/give macros
 *
 * NOTE: DO NOT USE if specifying a timeout other than L7_WAIT_FOREVER
 *       where awareness of the time expiration is needed.  Make a direct
 *       call to osapiXxxxTake() instead.
 */
#define ACL_SEMA_TAKE(_s, _t) \
  aclSemaTake((_s), (_t), __FILE__, __LINE__)

#define ACL_SEMA_GIVE(_s) \
  aclSemaGive((_s), __FILE__, __LINE__)

#define ACL_READ_LOCK_TAKE(_l, _t) \
  aclReadLockTake((_l), (_t), __FILE__, __LINE__)

#define ACL_READ_LOCK_GIVE(_l) \
  aclReadLockGive((_l), __FILE__, __LINE__)

#define ACL_WRITE_LOCK_TAKE(_l, _t) \
  aclWriteLockTake((_l), (_t), __FILE__, __LINE__)

#define ACL_WRITE_LOCK_GIVE(_l) \
  aclWriteLockGive((_l), __FILE__, __LINE__)

/* Index 1-based normalization macros for named ACLs (assumes index is in range) */
#define ACL_MAC_INDEX_NORMALIZE(_n) \
  ((_n) - L7_ACL_MAC_MIN_INDEX + 1)

#define ACL_IPV6_INDEX_NORMALIZE(_n) \
  ((_n) - L7_ACL_IPV6_MIN_INDEX + 1)

#define ACL_NAMED_IPV4_INDEX_NORMALIZE(_n) \
  ((_n) - L7_ACL_NAMED_IPV4_MIN_INDEX + 1)

/* ACL Event Queue - Used by Time Based ACLs currently */
#define ACL_EVENT_QUEUE_NAME       "aclEventQueue"
#define ACL_EVENT_QUEUE_COUNT       L7_TIMERANGE_MAX_NUM 
#define ACL_EVENT_QUEUE_MSG_SIZE    sizeof(aclEventMsg_t)

typedef enum 
{
  ACL_EVENT_TIME_RANGE_MSG =1,
} aclEventMsgType_t;

typedef struct aclTimeRangeMsg_s
{
   L7_uchar8       timeRangeName[L7_TIMERANGE_NAME_LEN_MAX+1];
   aclTimeRangeEvents_t event;
} aclTimeRangeMsg_t; 

typedef struct aclEventMsg_s 
{
  aclEventMsgType_t type;
  union
  {
    aclTimeRangeMsg_t timeRangeMsg;
  }u;

}aclEventMsg_t;
/* ACL interface direction indication (used internally by application) */
typedef enum
{       
  ACL_INTF_DIR_IN = 0,
  ACL_INTF_DIR_OUT,

  ACL_INTF_DIR_MAX

} aclIntfDir_t;

/* bit flags for detecting 'match every' rules when building TLVs */
typedef enum
{
  ACL_TLV_MATCH_EVERY_NONE      = 0,

  ACL_TLV_MATCH_EVERY_IP        = (1 << 0),
  ACL_TLV_MATCH_EVERY_MAC       = (1 << 1),
  ACL_TLV_MATCH_EVERY_IPV6      = (1 << 2) 

} aclTlvMatchEvery_t;

/* the following flag combinations determine when to bypass processing 
 * a rule of a particular ACL type
 */
#define ACL_TLV_RULE_BYPASS_IP \
  (ACL_TLV_MATCH_EVERY_IP | ACL_TLV_MATCH_EVERY_MAC)

#define ACL_TLV_RULE_BYPASS_IPV6 \
  (ACL_TLV_MATCH_EVERY_IPV6 | ACL_TLV_MATCH_EVERY_MAC)

#define ACL_TLV_RULE_BYPASS_MAC \
  (ACL_TLV_MATCH_EVERY_MAC)

/* ACL update operations */
typedef enum
{
  ACL_INTF_ADD = 1,
  ACL_INTF_REMOVE,
  ACL_MODIFY,
  ACL_VLAN_ADD,
  ACL_VLAN_REMOVE,
  ACL_MAX_UPDATE_CMD
} aclUpdateCommands_t;

/* ACL notification events (internal definitions) */
typedef enum
{
  ACL_EVENT_CREATE = 1,
  ACL_EVENT_MODIFY,
  ACL_EVENT_ATTR_ONLY_MODIFY,
  ACL_EVENT_ROUTE_FILTER_MODIFY,
  ACL_EVENT_DELETE,
  ACL_EVENT_RENAME
} aclEvent_t;

/* Maximum number of ACL callback registrants allowed.
 *
 * As of release G, each ACL can be used as a redistribution route
 * filter by each of the three routing protocols.
 */
#define ACL_CALLBACK_REGISTRANTS_MAX_ROUTE_FILTER     FD_CNFGR_ACL_MAX_CALLBACK_REGISTRATIONS
#define ACL_CALLBACK_REGISTRANTS_MAX_NOTIFY_EVENT     FD_CNFGR_ACL_MAX_CALLBACK_REGISTRATIONS
#define ACL_CALLBACK_REGISTRANTS_MAX_CHANGE_APPROVAL  FD_CNFGR_ACL_MAX_CALLBACK_REGISTRATIONS

/* ACL callback table identifiers */
typedef enum
{
  ACL_CALLBACK_TABLE_ID_FIRST = 0,              /* range checking alias for first table ID */
  ACL_CALLBACK_TABLE_ID_ROUTE_FILTER = 0,       /* must be assigned to same value as ID_FIRST */
  ACL_CALLBACK_TABLE_ID_NOTIFY_EVENT,
  ACL_CALLBACK_TABLE_ID_CHANGE_APPROVAL,
  ACL_CALLBACK_TABLE_ID_TOTAL                   /* total number of callback tables supported */
} aclCallbackTableId_t;

/* ACL callback table entry definition */
#define ACL_CALLBACK_DISPLAY_STR_MAX      63
typedef struct
{
  L7_VOIDFUNCPTR_t      funcPtr;                                      /* callback function */
  L7_COMPONENT_IDS_t    compId;                                       /* registrant component ID */
  L7_uchar8             displayStr[ACL_CALLBACK_DISPLAY_STR_MAX+1];   /* display string (for debugging) */
} aclCallbackEntry_t;

typedef struct
{
  aclCallbackEntry_t    *pStart;        /* pointer to first entry in table */
  L7_uint32             maxEntries;     /* maximum number of entries supported */
} aclCallbackTable_t;

/* ACL callback table control information */
typedef struct
{
  aclCallbackTable_t    table[ACL_CALLBACK_TABLE_ID_TOTAL];
  L7_BOOL               isInitialized;
} aclCallbackInfo_t;

extern aclCallbackInfo_t  *pAclCallbackInfo_g;

/* checks if a particular rule field is currently configured (any ACL type) */
#define ACL_RULE_FIELD_IS_SET(_cfgmask, _field) \
  ((((_cfgmask) & (1 << (_field))) != 0) ? L7_TRUE : L7_FALSE)

/* masks for checking validity of rule match fields
 *
 * NOTE:  Does not include any rule attributes here, only classifier fields.
 */
#define ACL_RULE_CFG_MASK \
  ( (1 << ACL_PROTOCOL) | (1 << ACL_PROTOCOL_MASK) | \
    (1 << ACL_SRCIP) | (1 << ACL_SRCIP_MASK) | \
    (1 << ACL_SRCPORT) | (1 << ACL_SRCSTARTPORT) | (1 << ACL_SRCENDPORT) | \
    (1 << ACL_DSTIP) | (1 << ACL_DSTIP_MASK) | \
    (1 << ACL_DSTPORT) | (1 << ACL_DSTSTARTPORT) | (1 << ACL_DSTENDPORT) | \
    (1 << ACL_IPPREC) | (1 << ACL_IPTOS) | (1 << ACL_IPDSCP) \
  )

#define ACL_RULE_CFG_MASK_EVERY \
  ( (1 << ACL_EVERY) | ACL_RULE_CFG_MASK )

/* masks for manipulating IPv6 rule classifier fields
 *
 * NOTE:  Does not include any rule attributes here, only classifier fields.
 */
#define ACL_IPV6_RULE_BASIC_CFG_MASK \
  ( (1 << ACL_SRCIPV6) | \
    (1 << ACL_DSTIPV6) | \
    (1 << ACL_IPDSCP) | \
    (1 << ACL_FLOWLBLV6) \
  )

#define ACL_IPV6_RULE_BASIC_CFG_MASK_EVERY \
  ( (1 << ACL_EVERY) | ACL_IPV6_RULE_BASIC_CFG_MASK )

#define ACL_IPV6_RULE_CFG_MASK \
  ( ACL_IPV6_RULE_BASIC_CFG_MASK | \
    (1 << ACL_PROTOCOL) | (1 << ACL_PROTOCOL_MASK) | \
    (1 << ACL_SRCPORT) | (1 << ACL_SRCSTARTPORT) | (1 << ACL_SRCENDPORT) | \
    (1 << ACL_DSTPORT) | (1 << ACL_DSTSTARTPORT) | (1 << ACL_DSTENDPORT) \
  )

#define ACL_IPV6_RULE_CFG_MASK_EVERY \
  ( (1 << ACL_EVERY) | ACL_IPV6_RULE_CFG_MASK )

/* masks for manipulating MAC rule classifier fields (except for ACL_MAC_EVERY)
 *
 * NOTE:  Does not include any rule attributes here, only classifier fields.
 */
#define ACL_MAC_RULE_CFG_MASK \
  ( (1 << ACL_MAC_COS) | (1 << ACL_MAC_COS2) | \
    (1 << ACL_MAC_DSTMAC) | (1 << ACL_MAC_DSTMAC_MASK) | \
    (1 << ACL_MAC_ETYPE_KEYID) | (1 << ACL_MAC_ETYPE_VALUE) | \
    (1 << ACL_MAC_SRCMAC) | (1 << ACL_MAC_SRCMAC_MASK) | \
    (1 << ACL_MAC_VLANID) | (1 << ACL_MAC_VLANID_START) | (1 << ACL_MAC_VLANID_END) | \
    (1 << ACL_MAC_VLANID2) | (1 << ACL_MAC_VLANID2_START) | (1 << ACL_MAC_VLANID2_END) \
  )

#define ACL_MAC_RULE_CFG_MASK_EVERY \
  ( (1 << ACL_MAC_EVERY) | ACL_MAC_RULE_CFG_MASK )

/* the following are used for displaying the ACL TLV contents */
#define ACL_PARSE_MARGIN_STEP   2 
#define ACL_PARSE_MARGIN_MAX    20
#define ACL_IPV6_ADDR_STR_MAX   63

/* definitions for working with ACL rule correlators */
#define ACL_CORR_ACLTYPE_SHIFT      28
#define ACL_CORR_ACLTYPE_MASK       0xF0000000
#define ACL_CORR_ACLID_SHIFT        16
#define ACL_CORR_ACLID_MASK         0x0FFF0000
#define ACL_CORR_RULENUM_SHIFT      1
#define ACL_CORR_RULENUM_MASK       0x0000FFFE
#define ACL_CORR_RULEACT_SHIFT      0 
#define ACL_CORR_RULEACT_MASK       0x00000001

/* ACL message levels for displaying debug info */
#define ACL_MSGLVL_ON    0              /* use where printf is desired */
#define ACL_MSGLVL_HI    1
#define ACL_MSGLVL_MED   50
#define ACL_MSGLVL_LO    100
#define ACL_MSGLVL_LO_2  200
#define ACL_MSGLVL_OFF   10000          /* some arbitrarily large value */

extern L7_uint32         aclMsgLvl_g;

#define ACL_PRT(mlvl, fmt, args...) \
          { \
          if (aclMsgLvl_g >= (mlvl)) \
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, fmt, ##args); \
          }


typedef enum {
  ACL_PHASE_INIT_0 = 0,
  ACL_PHASE_INIT_1,
  ACL_PHASE_INIT_2,
  ACL_PHASE_WMU,
  ACL_PHASE_INIT_3,
  ACL_PHASE_EXECUTE,
  ACL_PHASE_UNCONFIG_1,
  ACL_PHASE_UNCONFIG_2,
} aclCnfgrState_t;

typedef struct {
  L7_BOOL aclSave;
  L7_BOOL aclRestore;
  L7_BOOL aclHasDataChanged;
  L7_BOOL aclIntfChange;
} aclDeregister_t;

 /* Time based ACL */
typedef struct aclTRCorrEntry_s
{
  L7_uint32 corrId;                 /* The correlator Id.*/
  L7_uint32 trId;                   /* Time Range ID.*/
  L7_uint32 refCount;               /* Count signifies number of times rule is applied on an interface*/
  struct aclTRCorrEntry_s *next;    /* Pointer to the next correlator entry.*/
  struct aclTRCorrEntry_s *prev;    /* Pointer to the previous correlator entry in the aclCorrEntryList */
} aclTRCorrEntry_t;

typedef struct aclTRTableEntry_s
{
  L7_uchar8 timeRangeName[L7_TIMERANGE_NAME_LEN_MAX+1]; /* Time Range Name */
  L7_uint32 refCount;                                   /* Count signifies number of ACL Rules refferring time Range name */
  L7_ACL_RULE_STATUS_t status;                          /* Time Range Status */
  aclTRCorrEntry_t *aclCorrEntryList;
} aclTRTableEntry_t;

typedef struct
{
  L7_uint32       tlvTimeRangeCorrCount;
  aclTRCorrEntry_t   *tlvTimeRangeCorrList[L7_ACL_TIME_RANGE_RULE_PER_INTF_DIR_LIMIT];  /* TLV correlator issued for Time based ACL's */
} aclIntfTRInfo_t;

typedef struct
{
  L7_BOOL         tlvInDevice;                  /* denotes whether TLV issued to device    */
  L7_uint32       tlvInstanceKey;               /* TLV instance key (for deletion)         */
  L7_uint32       tlvCorrCount;                 /* number of TLV correlators in list       */  
  L7_uint32       tlvCorrList[L7_ACL_LOG_RULE_PER_INTF_DIR_LIMIT];  /* TLV correlators issued */    
  aclIntfTRInfo_t *tlvTRInfo;
} aclIntfDirInfo_t;
typedef struct 
{
  aclIntfDirInfo_t  dir[ACL_INTF_DIR_MAX];        /* directional information                 */
} aclIntfInfo_t;

typedef struct
{
  aclIntfDirInfo_t dir[ACL_INTF_DIR_MAX];
} aclVlanInfo_t;

typedef struct
{
  L7_uint32       correlator;                   /* rule correlator (search key)        */
  L7_ushort16     refCt;                        /* entry reference count               */
  L7_ushort16     queries;                      /* number of times DTL was queried     */
  /* This field is needed by the AVL Library */
  void            *avlNext;
} aclCorrTable_t;

typedef union
{
  struct
  {
    L7_uint32     addr;                         /*  IPv4 address                       */
    L7_uint32     mask;                         /*  IPv4 netmask                       */
  } v4;

  L7_in6_prefix_t v6;

} aclIpAddr_t;

/* L3 IP ACL classifier rule definition */
typedef struct aclRuleParms_s
{
  L7_uint32       configMask;

  L7_uint32       ruleNum;                       /* access list rule num               */
  L7_uint32       action;                        /* action: permit/deny                */

  L7_uint32       assignQueueId;                 /* assigned queue id                  */
  aclTRCorrEntry_t *timeRangeEntry;              /* Time Range Entry                    */
  nimConfigID_t   redirectConfigId;              /* redirect intf config id            */
  nimConfigID_t   mirrorConfigId;                /* mirror intf config id              */
  L7_BOOL         logging;                       /* denotes if rule hits are logged    */

  L7_BOOL         every;                         /* let every IP packet match this rule*/

  L7_uchar8       protocol;                      /* protocol number                    */
  L7_uchar8       protmask;                      /* protocol mask value                */

  aclIpAddr_t     srcIp;                         /* combined v4/v6 addr, mask/prefix   */
  L7_ushort16     srcPort;                       /* source port: keyword               */
  L7_ushort16     srcStartPort;                  /* source port range: start port      */
  L7_ushort16     srcEndPort;                    /* source port range: end port        */

  aclIpAddr_t     dstIp;                         /* combined v4/v6 addr, mask/prefix   */
  L7_ushort16     dstPort;                       /* destination port: keyword          */
  L7_ushort16     dstStartPort;                  /* destination port range: start port */
  L7_ushort16     dstEndPort;                    /* destination port range: end port   */

  L7_uchar8       tosbyte;                       /* 8 bit tos byte                     */
  L7_uchar8       tosmask;                       /* tos mask                           */

  L7_uint32       flowlbl;                       /* ipv6 flow label (20-bit)           */

  L7_BOOL         last;                          /* check for last rule in an acl      */
  struct aclRuleParms_s   *next;                 /* used by AVL - pointer to next rule */
} aclRuleParms_t;

/* L2 MAC ACL classifier rule definition */
typedef struct aclMacRuleParms_s
{
  L7_uint32       configMask;

  L7_uint32       ruleNum;                       /* access list rule num               */
  L7_uint32       action;                        /* action: permit/deny                */

  L7_uint32       assignQueueId;                 /* assigned queue id                  */
  aclTRCorrEntry_t       *timeRangeEntry;        /* Time Range Entry                   */
  nimConfigID_t   redirectConfigId;              /* redirect intf config id            */
  nimConfigID_t   mirrorConfigId;                /* mirror intf config id              */
  L7_BOOL         logging;                       /* denotes if rule hits are logged    */

  L7_BOOL         every;                         /* let every MAC packet match this rule*/

  L7_uchar8       cos;                           /* class of service (802.1p)          */
  L7_uchar8       cos2;                          /* secondary class of service         */
  L7_uchar8       dstMac[L7_MAC_ADDR_LEN];       /* destination MAC addr               */
  L7_uchar8       dstMacMask[L7_MAC_ADDR_LEN];   /* destination MAC mask               */
  L7_ushort16     etypeKeyId;                    /* ethertype key identifier           */
  L7_ushort16     etypeValue;                    /* ethertype custom value             */
  L7_uchar8       srcMac[L7_MAC_ADDR_LEN];       /* source MAC addr                    */
  L7_uchar8       srcMacMask[L7_MAC_ADDR_LEN];   /* source MAC mask                    */
  L7_ushort16     vlanId;                        /* vlan id: single value              */
  L7_ushort16     vlanIdStart;                   /* vlan id range: start value         */
  L7_ushort16     vlanIdEnd;                     /* vlan id range: end value           */
  L7_ushort16     vlanId2;                       /* secondary vlan id: single value    */
  L7_ushort16     vlanId2Start;                  /* secondary vlan id range: start val */
  L7_ushort16     vlanId2End;                    /* secondary vlan id range: end value */

  L7_BOOL         last;                          /* check for last rule in an acl      */

  struct aclMacRuleParms_s   *next;              /* used by AVL - pointer to next rule */
} aclMacRuleParms_t;

typedef struct aclStructure_s
{
  L7_uint32                 aclNum;              /* ACL identifying number             */
  L7_uint32                 ruleCount;           /* num rules (except implicit deny)   */
  L7_uchar8                 aclName[L7_ACL_NAME_LEN_MAX+1];  /* for named IP ACLs      */
  aclRuleParms_t            *head;               /* pointer to the first acl rule      */
  struct aclStructure_s     *nextacl;            /* pointer needed by AVL library      */

} aclStructure_t;

typedef struct aclMacStructure_s
{
  L7_uint32                 aclIndex;            /* MAC ACL identifying number         */
  L7_uint32                 ruleCount;           /* num rules (except implicit deny)   */
  L7_uchar8                 aclName[L7_ACL_NAME_LEN_MAX+1];  /* MAC ACL name           */
  aclMacRuleParms_t         *head;               /* pointer to the first acl rule      */
  struct aclMacStructure_s  *nextacl;            /* pointer needed by AVL library      */

} aclMacStructure_t;

typedef union aclCfgRuleParms_s
{
  aclRuleParms_t            ip;                   /* IP/IPV6 ACL rules                 */
  aclMacRuleParms_t         mac;                  /* MAC ACL rules                     */

} aclCfgRuleParms_t;

typedef struct aclId_s
{
  L7_ACL_TYPE_t     aclType;
  L7_uint32         aclId;                            /* IP: aclNum; MAC: aclIndex     */

} aclId_t;

typedef struct
{
  aclId_t           id;                               /* ACL identification info       */
  L7_uchar8         aclName[L7_ACL_NAME_LEN_MAX+1];   /* ACL name (as needed)          */
  aclCfgRuleParms_t ruleData[L7_ACL_MAX_RULE_NUM+1];  /* content depends on id.aclType */

} aclCfgData_t;

typedef struct
{
  L7_BOOL           inUse;                            /* denotes entry is in use    */
  L7_uint32         seqNum;                           /* list order sequence number */
  aclId_t           id;                               /* ACL identification info    */

} aclIntfParms_t;

typedef struct
{
  aclIntfParms_t    assignList[L7_ACL_MAX_LISTS_PER_INTF_DIR];

} aclIntfList_t;

typedef struct
{
  aclIntfParms_t    assignList[L7_ACL_MAX_LISTS_PER_VLAN_DIR];

} aclVlanList_t;

/* Struct to hold information about the ACLs configured for 
 * a given interface in each direction
 */
typedef struct
{
  nimConfigID_t     configId; /* must be first in the struct */
  aclIntfList_t     intf_dir_Tb[ACL_INTF_DIR_MAX];

} aclIntfCfgData_t;

/*
 * Struct to hold information about the ACLs configured for a given VLAN in
 * each direction.
 */
typedef struct
{
  L7_uint32      vlanId;
  L7_BOOL        inUse;
  aclVlanList_t  vlan_dir_Tb[ACL_INTF_DIR_MAX];
} aclVlanCfgData_t;

typedef struct
{
#ifdef PROD_USE_BINARY_CONFIGURATION  
  /* NOTE: aclData is indexed by list entry number, not directly by ACL ID.
   *       Entry 0 of this array is reserved, but unused (mainly for backwards
   *       compatibility with config files from earlier releases).
   */
  aclCfgData_t      aclData[L7_ACL_MAX_LISTS+1];
#endif

  /* Array to hold information about the ACLs an interface is configured with */
  aclIntfCfgData_t  aclIntfCfgData[L7_ACL_INTF_MAX_COUNT];
  /*
   * Array to hold information about the ACLs with which a VLAN is configured.
   * The array is indexed via aclVlanMapTbl.  This index table is built when
   * the configuration is read and is an index both for this configuration and
   * for pAclVlanInfo.
   */
  aclVlanCfgData_t  aclVlanCfgData[L7_ACL_VLAN_MAX_COUNT+1];  /* index 0 not used */

  L7_uint32         aclMsgLvl;

} L7_aclCfgParms_t;

typedef struct
{
  L7_fileHdr_t      cfgHdr;
  L7_aclCfgParms_t  cfgParms;
#ifdef L7_PRODUCT_SMARTPATH
  easyacl_rule_t    easyRules_cfg[L7_ACL_MAX_LISTS];
#endif
  L7_uint32         checkSum;
  
} aclCfgFileData_t;

/*********************************************************************
*
* @purpose  Build default acl intf config data.
*
* @param    L7_uint32  ver  Software version of Config Data
*
* @returns  void
*
* @comments None.
*
* @end
*
*********************************************************************/
void aclBuildDefaultIntfConfigData(nimConfigID_t *configId, aclIntfCfgData_t *pCfg);

/*********************************************************************
*
* @purpose  Inicates whether the ACL component has been initialized
*
* @param    
*
* @returns  L7_TRUE if the ACL component has been initialized.
*           L7_FALSE otherwise.
*
* @comments 
*
* @end
*
*********************************************************************/
L7_BOOL aclComponentInitialized(void);

/*********************************************************************
*
* @purpose  Indicates whether the ACL component can accept callback registrations
*
* @param    
*
* @returns  L7_TRUE if the ACL component ready for callback registrations
*           L7_FALSE otherwise.
*
* @comments An L7_FALSE return from this routine usually means the caller
*           is attempting to register too soon, such as during phase1 init
*           instead of phase2.
*
* @end
*
*********************************************************************/
L7_BOOL aclCallbackRegisterIsReady(void);

/*********************************************************************
*
* @purpose  Build default acl config data.
*
* @param    L7_uint32   ver   Software version of Config Data
*
* @returns  void
*
* @comments None.
*
* @end
*
*********************************************************************/
void aclBuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
* @purpose  Migrate old config to new
*
* @param    oldVer      @b{(input)} version of old config definition
* @param    ver         @b{(input)} version of current config definition
* @param    pCfgBuffer  @b{(input)} ptr to location of read configuration
*
* @returns  void
*
* @notes    This is the callback function provided to the sysapiCfgFileGet
*           routine to handle cases where the config file is of an older
*           version.
*
* @notes
*
* @end
*********************************************************************/
void aclMigrateConfigData(L7_uint32 oldVer, L7_uint32 ver, L7_char8 *pCfgBuffer);

/*********************************************************************
*
* @purpose  Apply config data
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclApplyConfigData(void);

#ifdef PROD_USE_BINARY_CONFIGURATION  
/*********************************************************************
*
* @purpose  Apply config data for IP access lists (of any type)
*
* @param    aclCfgData_t *aclData  access list data pointer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclApplyConfigDataIp(aclCfgData_t *aclData);

/*********************************************************************
*
* @purpose  Apply config data for MAC access lists
*
* @param    aclCfgData_t *aclData  access list data pointer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclApplyConfigDataMac(aclCfgData_t *aclData);
#endif

/*********************************************************************
*
* @purpose  Save acl user config file to NVStore.
*
* @param    void
*
* @returns  L7_SUCCESS or
* @returns  L7_FAILURE
*
* @comments None. 
*       
* @end
*
*********************************************************************/
L7_RC_t aclSave(void);

/*********************************************************************
*
* @purpose  Save IP ACL user config.
*
* @param    L7_uint32   indexStart    starting index in saved cfg array
* @param    L7_uint32   *savedCount   number of ACL entries saved
*
* @returns  L7_SUCCESS or
* @returns  L7_FAILURE
*
* @comments None. 
*       
* @end
*
*********************************************************************/
L7_uint32 aclSaveIp(L7_uint32 indexStart, L7_uint32 *savedCount);

/*********************************************************************
*
* @purpose  Save IPv6 ACL user config.
*
* @param    L7_uint32   indexStart    starting index in saved cfg array
* @param    L7_uint32   *savedCount   number of ACL entries saved
*
* @returns  L7_SUCCESS or
* @returns  L7_FAILURE
*
* @comments None. 
*       
* @end
*
*********************************************************************/
L7_uint32 aclSaveIpv6(L7_uint32 indexStart, L7_uint32 *savedCount);

/*********************************************************************
*
* @purpose  Save MAC ACL user config.
*
* @param    L7_uint32   indexStart    starting index in saved cfg array
* @param    L7_uint32   *savedCount   number of ACL entries saved
*
* @returns  L7_SUCCESS or
* @returns  L7_FAILURE
*
* @comments None. 
*       
* @end
*
*********************************************************************/
L7_uint32 aclSaveMac(L7_uint32 indexStart, L7_uint32 *savedCount);

/*********************************************************************
*
* @purpose  Remove all the configured ACL VLAN bindings.
*
* @param    void
*
* @returns  Nothing
*
* @comments Removes all ACLs configured on VLAN ids. 
*       
* @end
*
*********************************************************************/
void aclVlanConfigRestore(void);

/*********************************************************************
*
* @purpose  Restore ACL user config to defaults
*
* @param    void
*
* @returns  L7_SUCCESS or
* @returns  L7_FAILURE
*
* @comments Removes all existing ACLs and re-creates based on
*           the specs in the factory-default configuration. 
*       
* @end
*
*********************************************************************/
L7_uint32 aclRestore(void);

/*********************************************************************
*
* @purpose  Check if acl user config data has changed.
*
* @param    void
*
* @returns  L7_TRUE or
* @returns  L7_FALSE
*
* @comments None. 
*       
* @end
*
*********************************************************************/
L7_BOOL aclHasDataChanged(void);
void aclResetDataChanged(void);

/*********************************************************************
*
* @purpose  To process link state changes.
*
* @param    L7_uint32  intIfNum  internal interface number
* @param    L7_uint32  event     event, defined by L7_PORT_EVENTS_t
* @param    L7_uint32  correlator event, defined by L7_PORT_EVENTS_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator);

/*********************************************************************
* @purpose  Process NIM Startup notifications to ACL component
*
* @param    NIM_STARTUP_PHASE_t  startupPhase
*
* @notes
*
* @end
*********************************************************************/
void aclStartupNotifyCallback(NIM_STARTUP_PHASE_t startupPhase);

/*********************************************************************
*
* @purpose  Adds/removes an updated ACL interface assignment list 
*           to/from the driver
*
* @param    L7_uint32   intf     the internal interface number
* @param    L7_uint32   aclDir   the interface direction (internal)
* @param    L7_BOOL     op       operation (ACL_INTF_ADD, ACL_INTF_REMOVE)
*
* @returns  L7_SUCCESS, if the next rule ID was found
* @returns  L7_ERROR,   if aclnum or rule does not exist
*
* @comments Will build a TLV and send the ACL to the driver
*
* @end
*
*********************************************************************/
L7_RC_t aclIntfUpdate(L7_uint32 intf, L7_uint32 aclDir, L7_uint32 op);
/*********************************************************************
*
* @purpose  Adds/removes an updated ACL VLAN assignment list
*           to/from the driver
*
* @param    L7_uint32   vlan     the internal VLAN number
* @param    L7_uint32   aclDir   the VLAN direction (internal)
* @param    L7_BOOL     op       operation (ACL_VLAN_ADD, ACL_VLAN_REMOVE)
*
* @returns  L7_SUCCESS, if the next rule ID was found
* @returns  L7_ERROR,   if aclnum or rule does not exist
*
* @comments Will build a TLV and send the ACL to the driver
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanUpdate( L7_uint32 vlanNum, L7_uint32 aclDir, L7_uint32 op );

/*********************************************************************
*
* @purpose  Sends a recently updated ACL to the driver
*
* @param    L7_uint32   aclnum   the access-list identifying number
* @param    L7_BOOL     op       operation (ACL_MODIFY)
*
* @returns  L7_SUCCESS, if all interfaces updated
* @returns  L7_ERROR,   if TLV operation failed
* @returns  L7_FAILURE, if other failure 
*
* @comments Will build a TLV and send the ACL to the driver
*
* @end
*
*********************************************************************/
L7_RC_t aclUpdate(L7_uint32 aclnum, L7_uint32 op);

/*********************************************************************
*
* @purpose  Sends a recently updated ACL to the driver, except for
*           the specified interface
*
* @param    L7_uint32   aclnum   the access-list identifying number
* @param    L7_BOOL     op       operation (ACL_MODIFY)
* @param    L7_uint32   intIfNum internal interface number, or 0
*
* @returns  L7_SUCCESS, if all interfaces updated
* @returns  L7_ERROR,   if TLV operation failed
* @returns  L7_FAILURE, if other failure 
*
* @comments Will build a TLV and send the ACL to the driver for each
*           interface to which it is attached EXCEPT for the specified
*           intIfNum.  If the intIfNum is 0, all of the ACL's interfaces
*           are updated.
*
* @end
*
*********************************************************************/
L7_RC_t aclUpdateExceptIntf(L7_uint32 aclnum, L7_uint32 op, L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Creates a single TLV containing active rules of the specified
*           ACL within it and return the data in TLV block storage
*           provided by the caller.
*
* @param    L7_ACL_TYPE_t     aclType   access-list type
* @param    L7_uint32         aclnum    access-list identifying number
* @param    L7_tlvHandle_t    tlvHandle TLV block handle (supplied by caller)
*
* @returns  L7_SUCCESS, if TLV successfully built
* @returns  L7_ERROR,   if problem with TLV creation
* @returns  L7_FAILURE  if any other failure
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The information contained in the TLV represents a snapshot of
*           the data in the access list  database at time of invocation.
*           The tlvHandle parm designates the TLV block supplied by the caller.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpTlvGet(L7_ACL_TYPE_t aclType, L7_uint32 aclnum, L7_tlvHandle_t tlvHandle);

/*********************************************************************
*
* @purpose  Builds an ACL Create TLV and sends it to DTL
*
* @param    L7_uint32   intf     interface associated with this ACL
* @param    L7_uint32   vlan     vlan ID associated with this ACL
* @param    L7_uint32   aclDir   interface direction (internal)
*
* @returns  L7_SUCCESS, if TLV successfully built and issued
* @returns  L7_ERROR,   if problem with TLV creation
* @returns  L7_FAILURE  if any other failure
*
* @comments Invoke multiple times for each interface, direction or 
*           VLAN ID, direction
*
* @comments This function is used to build TLV data when either applying
*           ACLs to an interface or binding ACLs to a VLAN ID.
*           All rules from each ACL currently assigned to this interface/VLAN ID
*           and direction are combined into a single ACL List TLV for
*           use by the device.  There is one implicit 'deny all' rule
*           added to the end of the TLV (if no configured 'match every' 
*           rule is used).
*
* @note     Only one of the two parameters intf or vlan shall be valid.
*           A zero value for vlan will cause the intf parameter to be the valid
*           input and the TLV will be constructed for that interface.  If vlan is non-zero, it will
*           be used to construct a VLAN ID associated TLV.  This is possible since 0 is not
*           a valid VLAN ID value.
*
* @end
*
*********************************************************************/
L7_RC_t aclBuildTLVCreate(L7_uint32 intf, L7_uint32 vlan, L7_uint32 aclDir);

/*********************************************************************
*
* @purpose  Builds an ACL Create TLV and optionally sends it to DTL
*
* @param    L7_uint32       intf        interface associated with this ACL
* @param    L7_uint32       vlan        vlan ID associated with this ACL
* @param    L7_uint32       aclDir      interface direction (internal)
* @param    L7_tlvHandle_t  tlvHandle   TLV block handle to use
* @param    aclId_t         *pAclIdent  Single ACL identifier, or L7_NULLPTR
*
* @returns  L7_SUCCESS, if TLV successfully built and issued
* @returns  L7_ERROR,   if problem with TLV creation
* @returns  L7_FAILURE  if any other failure
*
* @comments Invoke multiple times for each interface, direction or 
*           VLAN ID, direction
*
* @comments This function is used to build TLV data when either applying
*           ACLs to an interface or binding ACLs to a VLAN ID.
*           All rules from each ACL currently assigned to this interface/VLAN ID
*           and direction are combined into a single ACL List TLV for
*           use by the device.  There is one implicit 'deny all' rule
*           added to the end of the TLV (if no configured 'match every' 
*           rule is used).
*
* @comments Only one of the two parameters intf or vlan shall be valid.
*           A zero value for vlan will cause the intf parameter to be the valid
*           input and the TLV will be constructed for that interface.  If vlan is non-zero, it will
*           be used to construct a VLAN ID associated TLV.  This is possible since 0 is not
*           a valid VLAN ID value.
*
* @comments The pAclIdent parm is normally set to L7_NULLPTR when issuing a TLV to
*           an interface or VLAN.  However, if this parm is non-null, then this 
*           function will only produce the TLV data for the specified ACL and will
*           not perform any other activity related to interface or VLAN.  In this 
*           case, the intf and vlan parms are both passed in as 0 by the caller
*           and are ignored by this routine.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpBuildTLVCreate(L7_uint32 intf, L7_uint32 vlan, L7_uint32 aclDir,
                             L7_tlvHandle_t tlvHandle, aclId_t *pAclIdent);

/*********************************************************************
*
* @purpose  Builds an ACL Delete TLV and send it to DTL
*
* @param    L7_uint32   intf     interface associated with this ACL
* @param    L7_uint32   vlan     VLAN associated with this ACL
* @param    L7_uint32   aclDir   interface direction (internal)
*
* @returns  L7_SUCCESS, if the TLV was successfully build and sent
* @returns  L7_ERROR,   if TLV build error
* @returns  L7_FAILURE, if any other failure
*
* @comments Invoke multiple times for more than one interface/VLAN_ID, direction
*
* @note     Only one of the parameters intf or vlan can be valid at the same time.
*           If vlan is non-zero it is considered the valid parameter and the intf
*           parameter is ignored.
*
* @end
*
*********************************************************************/
L7_RC_t aclBuildTLVDelete(L7_uint32 intf, L7_uint32 vlan, L7_uint32 aclDir);

/*********************************************************************
*
* @purpose  Builds TLV entries for an IPv6 implicit deny all rule
*
* @param    L7_uint32       vlan       Specifies if TLV is being applied
*                                      to VLAN versus interface(s).  This
*                                      affects how the implicit deny rules
*                                      are created.  (vlan == 0 implies 
*                                      application to interface(s); vlan != 0
*                                      implies application to vlan)
* @param    L7_tlvHandle_t  tlvHandle  TLV block handle to use
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    TLV build error
* @returns  L7_FAILURE
*
* @comments An IPv6 'deny all' rule consists of multiple classifier entries,
*           each of which contains multiple match fields.
*
* @end
*
*********************************************************************/
L7_RC_t aclTlvRuleIpv6DenyAllBuild(L7_uint32 vlan, L7_tlvHandle_t tlvHandle);

/*********************************************************************
*
* @purpose  Builds a set of IP ACL Rule TLVs
*
* @param    L7_uint32   aclnum              IP ACL number
* @param    L7_uint32   *ruleCount          number of rules written to TLV
* @param    aclTlvMatchEvery_t *matchEveryFlags set if a 'match every' rule used
* @param    aclIntfDirInfo_t   *pDirInfo    Ptr to intf,dir information
* @param    L7_uint32   *vlan    VLAN ID if ACL binding to VLAN
* @param    L7_tlvHandle_t  tlvHandle  TLV block handle to use
*
* @returns  L7_SUCCESS, if this ACL successfully processed
* @returns  L7_ERROR,   if problem with TLV creation
* @returns  L7_FAILURE  if any other failure
*
* @comments Does not append an implicit 'deny all' rule to the TLV.
*
* @note     If the vlan parameter is non-zero, an implied rule is generated
*           to match on that VLAN ID.  
*
* @end
*
*********************************************************************/
L7_RC_t aclTlvRuleDefBuild(L7_uint32 aclnum, L7_uint32 *ruleCount,
                           aclTlvMatchEvery_t *matchEveryFlags,
                           aclIntfDirInfo_t *pDirInfo,
                           L7_uint32 vlan, L7_tlvHandle_t tlvHandle,
                           L7_uint8 *actionFlag);

/*********************************************************************
*
* @purpose  Checks if the ACL rule is effectively non-empty after factoring 
*           in a mask value, if any
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the ACL rule number
*
* @returns  L7_TRUE, if the TLV rule exists
* @returns  L7_FALSE, if the TLV rule is empty (i.e., masked value is 0)
*
* @comments The reason for this check is to prevent maskable fields
*           whose mask value is 0 from appearing in the TLV issued to the
*           device.
*
* @end
*
*********************************************************************/
L7_BOOL aclIsTlvMaskedRuleValid(L7_uint32 aclnum, L7_uint32 rulenum);

/*********************************************************************
* @purpose  Parse and display TLV contents
*
* @param    pTlv        @{(input)} TLV start location
* @param    intIfNum    @{(input)} Internal interface number (or 0)
* @param    vlanId      @{(input)} VLAN ID (or 0)
* @param    direction   @{(input)} Interface.VLAN direction (external)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Specify the intIfNum parameter as 0 if this TLV is not 
*           associated with a particular interface.  If TLV is associated
*           with a VLAN ID, this parameter is non-zero.
*
* @end
*********************************************************************/
L7_RC_t aclTlvParse(L7_tlv_t *pTlv, L7_uint32 intIfNum, L7_uint32 vlanId, L7_uint32 direction);

/*********************************************************************
* @purpose  Display formatted content of a single TLV entry native info
*
* @param    pTlv        @{(input)}  TLV start location
* @param    nestLvl     @{(input)}  Nesting level of this TLV (0=top level)
* @param    pSize       @{(output)} Pointer to TLV entry size output location
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    The nestLvl parameter indicates the nesting depth of this TLV
*           relative to the top-level (i.e., outermost) TLV.  A top-level
*           TLV is designated by a nestLvl of 0.
*
* @notes    The user function must provide the TOTAL size of the TLV, which
*           includes the size of the type and length fields, based on its
*           internal knowledge of the TLV type-based definition.
*
* @notes    This parser is specific to the ACL TLV definitions.  This function
*           name is passed to the TLV utility when parsing the TLV.
*
* @notes    Only displays the TLV type, length and native data.
*
* @end
*********************************************************************/
L7_RC_t aclTlvEntryDisplay(L7_tlv_t *pTlv, L7_uint32 nestLvl, 
                           L7_uint32 *pEntrySize);

/*********************************************************************
*
* @purpose  To create an access list tree.
*
* @param    L7_uint32  maxListSize  
*
* @returns  void
*
* @comments Creates a new instance of ACL, allocating space for up to
*           list_size ACLs.
*
* @end
*
*********************************************************************/
void aclCreateTree(L7_uint32 maxListSize);

/*********************************************************************
*
* @purpose  To delete an access list tree.
*
* @param    none
*
* @comments Destroys the instance of acl, releasing the previously
*           allocated database and control space.
*
* @end
*
*********************************************************************/
void aclDeleteTree(void);

/*********************************************************************
*
* @purpose  To find an access list identified by an acl number.
*
* @param    void       *acl_ptr         pointer to database
* @param    L7_uint32  aclnum           the acl identifying number
* @param    L7_uint32  *found_at_index  the acl table index
*
* @returns  pointer to node if the acl is found
* @returns  L7_NULLPTR, if the acl is not found
*
* @comments None.
*
* @end
*
*********************************************************************/
void* aclFindACL(L7_uint32 aclnum);

/*********************************************************************
*
* @purpose  Checks to see if a user has configured a field in a rule
*           in an access list
*
* @param    L7_uint32  acl_ptr   the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  field     ruleFields_t enum 
*
* @returns  L7_TRUE, if the field has been configured
* @returns  L7_FALSE, if the field has not been configured
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_BOOL aclIsRuleFieldConfigured(void* acl_ptr, L7_uint32 rulenum, L7_uint32 field);

/*********************************************************************
*
* @purpose  Provide the current value of the named ACL index next variable
*
* @param    aclType     @b{(input)}  access list type
* @param    *next       @b{(output)} access list index next value
*
* @returns  void
*
* @comments Only provides an output value if the next parm is non-null.
*
* @comments Does not guarantee this index value will be valid at the time
*           an ACL create is attempted.
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*       
* @end
*
*********************************************************************/
void aclImpNamedIndexNextRead(L7_ACL_TYPE_t aclType, L7_uint32 *next);

/*********************************************************************
*
* @purpose  Determine next available named acess list index value and 
*           update the 'IndexNext' value
*
* @param    aclType     @b{(input)}  access list type
*
* @returns  void  
*
* @comments Always looks for first index not already in use, starting with 1.
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*       
* @end
*
*********************************************************************/
void aclImpNamedIndexNextUpdate(L7_ACL_TYPE_t aclType);

/*********************************************************************
*
* @purpose  Initialize in-use table entry for a named ACL
*
* @param    aclType     @b{(input)}  access list type
*
* @returns  void  
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*       
* @end
*
*********************************************************************/
void aclImpNamedIndexInUseInit(L7_ACL_TYPE_t aclType);

/*********************************************************************
*
* @purpose  Updates an in-use table entry for a named ACL
*
* @param    aclType     @b{(input)}  access list type
* @param    aclnum      @b{(input)}  access-list identifying number
* @param    val         @b{(input)}  value to set (L7_FALSE or L7_TRUE)
*
* @returns  void  
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*       
* @end
*
*********************************************************************/
void aclImpNamedIndexInUseUpdate(L7_ACL_TYPE_t aclType, L7_uint32 aclnum, L7_BOOL val);

/*********************************************************************
*
* @purpose  Retrieve the min/max index values allowed for the named ACL tables
*
* @param    aclType     @b{(input)}  access list type
* @param    *pMin       @b{(output)} Pointer to index min output location
* @param    *pMax       @b{(output)} Pointer to index max output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*       
* @end
*
*********************************************************************/
L7_RC_t aclImpNamedIndexMinMaxGet(L7_ACL_TYPE_t aclType, L7_uint32 *pMin, L7_uint32 *pMax);

/*********************************************************************
*
* @purpose  Get the next named access list index created in the system.
*
* @param    aclType     @b{(input)}  access list type
* @param    aclIndex    @b{(input)}  current ACL index value
* @param    *pAclIndex  @b{(output)} pointer to next ACL index value
*
* @returns  L7_SUCCESS        Next named access list index was found
* @returns  L7_ERROR          No subsequent ACL index was found
* @returns  L7_FAILURE        All other failures
*
* @comments This internal function expects the caller to check
*           for an appropriate aclType value if there are any 
*           restrictions in place.
*
*
* @comments This function is also used for a 'getFirst' search by 
*           specifying an aclIndex value of 0.
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*       
* @end
*
*********************************************************************/
L7_RC_t aclImpNamedIndexGetNext(L7_ACL_TYPE_t aclType, L7_uint32 aclIndex, 
                                L7_uint32 *pAclIndex);

/*********************************************************************
*
* @purpose  Checks if aclIndex is a valid, configured named access list
*
* @param    aclType     @b{(input)}  access list type
* @param    aclIndex    @b{(input)}  ACL index value
*
* @returns  L7_SUCCESS        Access list index was found
* @returns  L7_FAILURE        Access list index not found
*
* @comments The only valid aclType values are L7_ACL_TYPE_IPV6 and 
*           L7_ACL_TYPE_IP.
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*       
* @end
*
*********************************************************************/
L7_RC_t aclImpNamedIndexCheckValid(L7_ACL_TYPE_t aclType, L7_uint32 aclIndex);

/*********************************************************************
*
* @purpose  Checks if the aclIndex is within proper range
*
* @param    aclType     @b{(input)}  access list type
* @param    aclIndex    @b{(input)}  ACL index value
*
* @returns  L7_SUCCESS        Access list index is in range
* @returns  L7_FAILURE        Access list index is out of range
*
* @comments The only valid aclType value is L7_ACL_TYPE_IPV6.
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*       
* @end
*
*********************************************************************/
L7_RC_t aclImpNamedIndexRangeCheck(L7_ACL_TYPE_t aclType, L7_uint32 aclIndex);

/*********************************************************************
*
* @purpose  Checks if the ACL ID belongs to the named IPV6 ACL index range
*
* @param    L7_uint32   aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list ID belongs to a supported range
* @returns  L7_FAILURE, if the access list ID is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpNamedIpv6IndexRangeCheck(L7_uint32 aclnum);

/*********************************************************************
*
* @purpose  Checks if the ACL ID belongs to the named IPV4 ACL index range
*
* @param    L7_uint32   aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list ID belongs to a supported range
* @returns  L7_FAILURE, if the access list ID is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpNamedIpv4IndexRangeCheck(L7_uint32 aclnum);

/*********************************************************************
*
* @purpose  Check if specified named ACL type is supported
*
* @param    aclType     @b{(input)}  access list type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments No semaphore protection required.
*       
* @end
*
*********************************************************************/
L7_RC_t aclImpNamedAclTypeSupported(L7_ACL_TYPE_t aclType);

/*********************************************************************
*
* @purpose  Given aclnum, get the next IP access list ID
*
* @param    L7_uint32   aclnum   the current access-list ID
* @param    L7_uint32  *next     the next ACL ID
*
* @returns  L7_SUCCESS        Next named access list index was found
* @returns  L7_ERROR          No subsequent ACL index was found
* @returns  L7_FAILURE        All other failures
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments This function is also used for a 'getFirst' search by 
*           specifying an aclIndex value of 0.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpNumGetNext(L7_uint32 aclnum, L7_uint32 *next);

/*********************************************************************
*
* @purpose  Checks if aclnum is a valid, configured IP access list
*
* @param    L7_uint32  aclnum   the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list was found
* @returns  L7_FAILURE, if the access list does not exist 
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpNumCheckValid(L7_uint32 aclnum);

/*********************************************************************
*
* @purpose  Checks if the ACL ID belongs to one of the supported ranges
*
* @param    L7_uint32   aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list ID belongs to a supported range
* @returns  L7_FAILURE, if the access list ID is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments This function does not distinguish between the various 
*           IP ACL identifiers, but considers whether the ACL ID belongs
*           to any numbered or named IP ACL index range that is supported.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpNumRangeCheck(L7_uint32 aclnum);

/*********************************************************************
*
* @purpose  To create an access list identified by an acl number.
*
* @param    L7_uint32  aclnum             the acl identifying number
* @param    L7_uint32  *created_at_index  the acl table index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_TABLE_IS_FULL
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclCreateACL(L7_uint32 aclnum);

/*********************************************************************
*
* @purpose  To delete an access list identified by an acl number.
*
* @param    L7_uint32  aclnum             the acl identifying number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclDeleteACL(void *acl_ptr);

/*********************************************************************
*
* @purpose  To add the name to this named access list.
*
* @param    *acl_ptr    @b{(input)} access list element pointer
* @param    *name       @b{(input)} access list name
*                                            
* @returns  L7_SUCCESS  name is added
* @returns  L7_FAILURE  invalid access list index, invalid name, or other failure
* @returns  L7_ERROR    access list does not exist
*
* @comments Does not check for name validity (e.g. alphanumeric string).
*
* @end
*
*********************************************************************/
L7_RC_t aclImpNameAdd(void *acl_ptr, L7_uchar8 *name);

/*********************************************************************
*
* @purpose  To get the name of this named access list.
*
* @param    *acl_ptr    @b{(input)}  access list element pointer
* @param    *name       @b{(output)} access list name
*                                            
* @returns  void
*
* @comments Only outputs a value if the acl_ptr and name parms are both non-null.
*
* @comments Assumes caller provides a name buffer of at least 
*           (L7_ACL_NAME_LEN_MAX+1) characters.
*
* @end
*
*********************************************************************/
void aclImpNameGet(void *acl_ptr, L7_uchar8 *name);

/*********************************************************************
*
* @purpose  Checks if the ACL name string is alpha-numeric
*
* @param    *name       @b{(input)} access-list name string
*
* @returns  L7_TRUE     ACL name is alpha-numeric
* @returns  L7_FALSE    ACL name string not alpha-numeric
*
* @comments This function only checks the name string syntax for a named ACL. 
*           It does not check if any named ACL currently contains this name.
*
* @comments An alpha-numeric string is defined as consisting of only
*           alphabetic, numeric, dash, underscore, or space characters.
*
* @end
*
*********************************************************************/
L7_BOOL aclImpNameIsAlphanumeric(L7_uchar8 *name);

/*********************************************************************
*
* @purpose  Find the index of a named access list, given its name.
*
* @param    aclType    @b{(input)}   named access list type
* @param    *name      @b{(input)}   access list name
* @param    *pIndex    @b{(output)}  ptr to access list index output location
*                                            
* @returns  L7_SUCCESS  named access list index is retrieved
* @returns  L7_FAILURE  invalid parms, or other failure
* @returns  L7_ERROR    access list name does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclImpNameByTypeIndexFind(L7_ACL_TYPE_t aclType, L7_uchar8 *name, L7_uint32 *pIndex);

/*********************************************************************
*
* @purpose  To get the index of a named access list, given its name.
*
* @param    *name      @b{(input)} access list name
* @param    *aclnum  @b{(output)} access list index
*                                            
* @returns  L7_SUCCESS  named access list index is retrieved
* @returns  L7_FAILURE  invalid parms, or other failure
* @returns  L7_ERROR    access list name does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclImpNameToIndexGet(L7_uchar8 *name, L7_uint32 *aclnum);

/*********************************************************************
*
* @purpose  To get the name of a named access list, given its index
*
* @param    *aclnum   @b{(input)}  access list index
* @param    *name       @b{(output)} access list name
*                                            
* @returns  L7_SUCCESS  name is retrieved
* @returns  L7_FAILURE  invalid parms, or other failure
* @returns  L7_ERROR    access list does not exist
*
* @comments Assumes caller provides a name buffer of at least 
*           (L7_ACL_NAME_LEN_MAX+1) characters.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpNameFromIndexGet(L7_uint32 aclnum, L7_uchar8 *name);

/*********************************************************************
*
* @purpose  Get a name string for any type of access list, given its index
*
* @param    aclnum      @b{(input)}  access list identifying number
* @param    *aclType    @b{(output)} access list type
* @param    *name       @b{(output)} access list name
*                                            
* @returns  L7_SUCCESS  name is retrieved
* @returns  L7_FAILURE  invalid parms, or other failure
* @returns  L7_ERROR    access list does not exist
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments Assumes caller provides a name buffer of at least 
*           (L7_ACL_NAME_LEN_MAX+1) characters.
*
* @comments Determines the ACL type from its index number and outputs
*           the list name.  For IPV4 numbered access lists, the 
*           ACL number is output in string notation, e.g. "101".
*
* @end
*
*********************************************************************/
L7_RC_t aclImpCommonNameStringGet(L7_uint32 aclnum, L7_ACL_TYPE_t *aclType,
                                  L7_uchar8 *name);

/*********************************************************************
*
* @purpose  To check if a rule number exists in an access list.
*
* @param    aclEntryParms_t  *headPtr  pointer to first rule in an acl
* @param    L7_uint32        rulenum   the acl rule number
*
* @returns  L7_TRUE, if the rule number exists
* @returns  L7_FALSE, if the rule number does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_BOOL aclRuleNumExist(aclRuleParms_t *headPtr, L7_uint32 rulenum);

/*********************************************************************
*
* @purpose  Gets the current number of configured rules in an ACL
*
* @param    aclType     @b{(input)}  access list type        
* @param    aclId       @b{(input)}  access list identifier
* @param    *ruleCount  @b{(output)} number of rules in ACL
*
* @returns  L7_SUCCESS  ACL rule count retrieved
* @returns  L7_ERROR    access list does not exist
* @returns  L7_FAILURE  access list index is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The output ruleCount does not include the implicit 'deny all'
*           rule contained in every ACL definition.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleCountGet(L7_ACL_TYPE_t aclType, L7_uint32 aclId, L7_uint32 *ruleCount);

/*********************************************************************
*
* @purpose  To add an access option to a rule in an access list.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  action   access action (permit/deny)
*
* @returns  L7_SUCCESS, if the access option is added
* @returns  L7_FAILURE, if invalid rule number
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclActionAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 action);

void *aclRuleNodeAllocate(L7_uint32 size);
void aclRuleNodeFree(void *node);
/*********************************************************************
*
* @purpose  To remove a rule from an existing access list.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
*                                            
* @returns  L7_SUCCESS, if the rule number is removed
* @returns  L7_FAILURE, if invalid access-list number or rule number
* @returns  L7_ERROR, if the access-list or rule number does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRemoveRule(void *acl_ptr, L7_uint32 rulenum);

/*********************************************************************
*
* @purpose  To set the assigned queue id for an access list rule.
*
* @param    L7_uint32  *acl_ptr  the access-list element pointer   
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  queueId   the queue id value              
*
* @returns  L7_SUCCESS, if assigned queue id is added
* @returns  L7_FAILURE, if invalid queue id or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclAssignQueueIdAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 queueId);

/*********************************************************************
*
* @purpose  To set the time range entry to an access list rule.
*
* @param    L7_uint32        *acl_ptr  the access-list element pointer
* @param    L7_uint32        rulenum   the acl rule number
* @param    aclTRCorrEntry_t *timeRangeEntry the time range entry
*
* @returns  L7_SUCCESS, if assigned time range Entry name is added
* @returns  L7_FAILURE, if invalid time range Entry or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclTimeRangeEntryAdd(void *acl_ptr, L7_uint32 rulenum, aclTRCorrEntry_t *timeRangeEntry);

/*********************************************************************
*
* @purpose  To set the redirect interface config id for an access list rule.
*
* @param    L7_uint32  *acl_ptr   the access-list element pointer   
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *configId  the redirect interface config id
*                                            
* @returns  L7_SUCCESS, if redirect interface is added
* @returns  L7_FAILURE, if invalid redirect interface or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRedirectConfigIdAdd(void *acl_ptr, L7_uint32 rulenum, nimConfigID_t *configId);

/*********************************************************************
*
* @purpose  To set the mirror interface config id for an access list rule.
*
* @param    L7_uint32  *acl_ptr   the access-list element pointer   
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *configId  the mirror interface config id
*                                            
* @returns  L7_SUCCESS, if mirror interface is added
* @returns  L7_FAILURE, if invalid mirror interface or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclMirrorConfigIdAdd(void *acl_ptr, L7_uint32 rulenum, nimConfigID_t *configId);

/*********************************************************************
*
* @purpose  To set the logging flag for an access list rule.
*
* @param    L7_uint32  *acl_ptr  the access-list element pointer   
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  logFlag   rule logging flag value         
*
* @returns  L7_SUCCESS, if logging flag is added
* @returns  L7_FAILURE, if invalid flag value or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclLoggingAdd(void *acl_ptr, L7_uint32 rulenum, L7_BOOL logFlag);

/*********************************************************************
*
* @purpose  To add a protocol to a rule in an access list.
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  protocol  the protocol number
*
* @returns  L7_SUCCESS, if the protocol is added
* @returns  L7_FAILURE, if the protocol is not added
* @returns  L7_ERROR, if the rule does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclProtocolAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 protocol);

/*********************************************************************
*
* @purpose  To add the source ip address and net mask.
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  ip       the ip address
* @param    L7_uint32  mask     the net mask
*
* @returns  L7_SUCCESS, if source ip address and net mask are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclSrcIPMaskAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 ipAddr, L7_uint32 mask);

/*********************************************************************
*
* @purpose  To add the source IPv6 address and prefix length.
*
* @param    L7_uint32       aclnum   the access-list identifying number
* @param    L7_uint32       rulenum  the acl rule number
* @param    L7_in6_prefix_t *addr6   ptr to ipv6 address and prefix length
*
* @returns  L7_SUCCESS, if source ipv6 address and prefix length are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclSrcIpv6AddrAdd(void *acl_ptr, L7_uint32 rulenum, 
                          L7_in6_prefix_t *addr6);

/*********************************************************************
*
* @purpose  To add the source port.
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  port      the ip address
*
* @returns  L7_SUCCESS, if source ip address and net mask are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclSrcPortAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 port);

/*********************************************************************
*
* @purpose  To add the range of the source layer 4 ports to a rule
*           in an existing access list.
*
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the access-list entry id
* @param    L7_uint32  startport  the start port number
* @param    L7_uint32  endport    the end port number
*
* @returns  L7_SUCCESS, if the port range is added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclSrcPortRangeAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 startPort, L7_uint32 endPort);

/*********************************************************************
*
* @purpose  To add the destination ip address and net mask.
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  ip       the ip address
* @param    L7_uint32  mask     the net mask
*
* @returns  L7_SUCCESS, if destination ip address and net mask are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclDstIPMaskAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 ipAddr, L7_uint32 mask);
  
/*********************************************************************
*
* @purpose  To add the destination IPv6 address and prefix length.
*
* @param    L7_uint32       aclnum   the access-list identifying number
* @param    L7_uint32       rulenum  the acl rule number
* @param    L7_in6_prefix_t *addr6   ptr to ipv6 address and prefix length
*
* @returns  L7_SUCCESS, if destination ipv6 address and prefix length are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclDstIpv6AddrAdd(void *acl_ptr, L7_uint32 rulenum, 
                          L7_in6_prefix_t *addr6);

/*********************************************************************
*
* @purpose  To add the destination port.
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  port      the ip address
*
* @returns  L7_SUCCESS, if destination ip address, net mask are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclDstPortAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 port);

/*********************************************************************
*
* @purpose  To add the range of the destination layer 4 ports to a
*           rule in an existing access list.
*
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the access-list entry id
* @param    L7_uint32  startport  the start port number
* @param    L7_uint32  endport    the end port number
*
* @returns  L7_SUCCESS, if the port range is added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclDstPortRangeAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 startport, L7_uint32 endport);

/*********************************************************************
*
* @purpose  Set the match condition whereby all packets match for an
*           ACL rule
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_BOOL    match    the match condition (L7_TRUE/L7_FALSE)
*
* @returns  L7_SUCCESS, if
* @returns  L7_FAILURE, if invalid list or rule number
* @returns  L7_ERROR, if the access-list or rule does not exist
*
* @comments This routine sets the range of all the filtering criteria
*           in a rule to the maximum, in an existing access list.
*
* @end
*
*********************************************************************/
L7_RC_t aclEveryAdd(void *acl_ptr, L7_uint32 rulenum, L7_BOOL match);

/*********************************************************************
*
* @purpose  To add the differentiated services code point (dscp) value
*           to a rule in an existing access list.
*
* @param    L7_uint32  rulenum  the access-list entry id
* @param    L7_uint32  dscpval  the diffserv code point value
*
* @returns  L7_SUCCESS, if
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclIPDscpAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 dscpval);

/*********************************************************************
*
* @purpose  To add the precedence value to a rule in an existing
*           access list.
*
* @param    L7_uint32  aclnum         access-list identifying number
* @param    L7_uint32  rulenum        the access-list entry id
* @param    L7_uint32  precedenceval  the precedence value
*
* @returns  L7_SUCCESS, if the precedence value is added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclIPPrecedenceAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 precedenceval);

/*********************************************************************
*
* @purpose  To add the type of service bits and mask.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the access-list entry id
* @param    L7_uint32  tosbits  tos bits
* @param    L7_uint32  tosmask  tos mask
*
* @returns  L7_SUCCESS, if the service type parameters are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclIPTosAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 tosbits, L7_uint32 tosmask);

/*********************************************************************
*
* @purpose  To add the IPv6 flow label.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the access-list entry id
* @param    L7_uint32  flowlbl  flow label value
*
* @returns  L7_SUCCESS, if the value is added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclIpv6FlowLabelAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 flowlbl);

/*************************************************************************
* @purpose  Translate external interface direction to internal value
*
* @param    direction   @b{(input)}  External direction value
*                                      (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    *aclDir     @b{(output)} Internal direction value
*                                      (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Translates the ACL interface direction value used by the 
*           public APIs into a value used internally by the ACL application.
*
* @end
*********************************************************************/
L7_RC_t aclDirExtToInt(L7_uint32 direction, L7_uint32 *aclDir);

/*************************************************************************
* @purpose  Translate internal interface direction to external value
*
* @param    aclDir      @b{(input)}  Internal direction value
*                                      (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    *direction  @b{(output)} External direction value
*                                      (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Translates the ACL interface direction value used internally
*           by the ACL application into the value used by the ACL public APIs.
*
* @end
*********************************************************************/
L7_RC_t aclDirIntToExt(L7_uint32 aclDir, L7_uint32 *direction);

/*********************************************************************
*
* @purpose  To add an access list to the specified interface and direction.
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    aclDir      @b{(input)}  interface direction (internal)
* @param    aclType     @b{(input)}  access-list type
* @param    aclId       @b{(input)}  access-list identifying number
* @param    seqNum      @b{(input)}  ACL evaluation order sequence number
*
* @returns  L7_SUCCESS  ACL successfully added to intf, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
* @returns  L7_ERROR    ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED  intf, dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED  feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL  ACL assignment at capacity for intf, dir
*
* @comments This internal function is used for assigning all types of ACLs
*           to an interface.  Any type-specific validity checking must be
*           performed by the caller.
*
* @comments The caller is required to provide proper semaphore protection.
*
* @comments A seqnum of 0 causes the next highest sequence number to be used.
*
* @end
*
*********************************************************************/
L7_RC_t aclIntfDirListAdd(L7_uint32 intIfNum, L7_uint32 aclDir, 
                          L7_ACL_TYPE_t aclType, L7_uint32 aclId,
                          L7_uint32 seqNum);

/*********************************************************************
*
* @purpose  To remove an access list from the specified interface and direction.
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    aclDir      @b{(input)}  interface direction (internal)
* @param    aclType     @b{(input)}  access-list type
* @param    aclId       @b{(input)}  access-list identifying number
*
* @returns  L7_SUCCESS  ACL successfully removed from intf, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
* @returns  L7_ERROR    ACL not currently attached to intf, dir
*
* @comments This internal function is used for removing all types of ACLs
*           from an interface.  Any type-specific validity checking must be
*           performed by the caller.
*
* @comments The caller is required to provide proper semaphore protection.
*
* @end
*
*********************************************************************/
L7_RC_t aclIntfDirListRemove(L7_uint32 intIfNum, L7_uint32 aclDir,
                             L7_ACL_TYPE_t aclType, L7_uint32 aclId);

/*********************************************************************
*
* @purpose  To purge all access lists from the specified interface and direction.
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    aclDir      @b{(input)}  interface direction (internal)
*
* @returns  L7_SUCCESS  all ACLs successfully purged from intf, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
*
* @comments This internal function is used for purging all types of ACLs
*           from an interface.  Any type-specific validity checking must be
*           performed by the caller.
*
* @comments The caller is required to provide proper semaphore protection.
*
* @end
*
*********************************************************************/
L7_RC_t aclIntfDirListPurge(L7_uint32 intIfNum, L7_uint32 aclDir);

/*********************************************************************
*
* @purpose  Check if the specified interface and direction is valid
*           for use with ACL
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    aclDir      @b{(input)}  interface direction (internal)
*
* @returns  L7_SUCCESS  intf,dir is valid for ACL
* @returns  L7_ERROR    intf,dir not valid for ACL
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The interface type and direction supported for ACL use   
*           depends on the capabilities of the platform.
*
* @comments Note that this API only indicates whether the interface
*           and direction are usable for ACLs.  It does not indicate
*           whether any ACLs are currently assigned to the interface
*           and direction (see aclImpIsInterfaceInUse).
*
* @end
*
*********************************************************************/
L7_RC_t aclImpIntfDirGet(L7_uint32 intIfNum, L7_uint32 aclDir);

/*********************************************************************
*
* @purpose  Get next sequential interface and direction that is valid
*           for use with ACL
*
* @param    intIfNum        @b{(input)}  internal interface number
* @param    aclDir          @b{(input)}  interface direction (internal)
* @param    *nextIntIfNum   @b{(output)} next internal interface number
* @param    *nextAclDir     @b{(output)} next interface direction (internal)
*
* @returns  L7_SUCCESS  next intf,dir for ACL was obtained
* @returns  L7_ERROR    no more valid intf,dir for ACL
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The interface type and direction supported for ACL use   
*           depends on the capabilities of the platform.
*
* @comments Note that this API only provides an interface and direction
*           that is usable for ACLs.  It does not indicate whether
*           any ACLs are currently assigned to the interface
*           and direction (see aclImpIsInterfaceInUse).
*
* @end
*
*********************************************************************/
L7_RC_t aclImpIntfDirGetNext(L7_uint32 intIfNum, L7_uint32 aclDir,
                             L7_uint32 *nextIntIfNum, L7_uint32 *nextAclDir);

/*********************************************************************
*
* @purpose  Check if the ACL sequence number is in use for the specified
*           interface and direction
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    aclDir      @b{(input)}  interface direction (internal)
* @param    seqNum      @b{(input)}  ACL interface sequence number
*
* @returns  L7_SUCCESS  sequence number is in use
* @returns  L7_ERROR    sequence number not in use
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments A sequence number value may be used only once for a given
*           interface and direction pair.  
*
* @end
*
*********************************************************************/
L7_RC_t aclImpIntfDirSequenceGet(L7_uint32 intIfNum, L7_uint32 aclDir,
                                 L7_uint32 seqNum);

/*********************************************************************
*
* @purpose  Get next sequential ACL interface sequence number in use 
*           for the specified interface and direction.
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    aclDir      @b{(input)}  interface direction (internal)
* @param    seqNum      @b{(input)}  ACL interface sequence number
* @param    *nextSeqNum @b{(output)} next ACL interface sequence number
*
* @returns  L7_SUCCESS  sequence number is in use
* @returns  L7_ERROR    sequence number not in use
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments Use a seqNum value of 0 to find the first ACL sequence   
*           number for this interface and direction, if it exists.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpIntfDirSequenceGetNext(L7_uint32 intIfNum, L7_uint32 aclDir,
                                     L7_uint32 seqNum, L7_uint32 *nextSeqNum);

/*********************************************************************
*
* @purpose  To add an access list to the specified vlan and direction.
*
* @param    vlanNum     @b{(input)}  internal vlan number
* @param    aclDir      @b{(input)}  vlan direction (internal)
* @param    aclType     @b{(input)}  access-list type
* @param    aclId       @b{(input)}  access-list identifying number
* @param    seqNum      @b{(input)}  ACL evaluation order sequence number
*
* @returns  L7_SUCCESS  ACL successfully added to vlan, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
* @returns  L7_ERROR    ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED  vlan, dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED  feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL  ACL assignment at capacity for vlan, dir
*
* @comments This internal function is used for assigning all types of ACLs
*           to an vlan.  Any type-specific validity checking must be
*           performed by the caller.
*
* @comments The caller is required to provide proper semaphore protection.
*
* @comments A seqnum of 0 causes the next highest sequence number to be used.
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanDirListAdd( L7_uint32     vlanNum,
                           L7_uint32     aclDir,
                           L7_ACL_TYPE_t aclType,
                           L7_uint32     aclId,
                           L7_uint32     seqNum );

/*********************************************************************
*
* @purpose  To remove an access list from the specified vlan and direction.
*
* @param    vlanNum     @b{(input)}  internal vlan number
* @param    aclDir      @b{(input)}  vlan direction (internal)
* @param    aclType     @b{(input)}  access-list type
* @param    aclId       @b{(input)}  access-list identifying number
*
* @returns  L7_SUCCESS  ACL successfully removed from vlan, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
* @returns  L7_ERROR    ACL not currently attached to vlan, dir
*
* @comments This internal function is used for removing all types of ACLs
*           from an vlan.  Any type-specific validity checking must be
*           performed by the caller.
*
* @comments The caller is required to provide proper semaphore protection.
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanDirListRemove( L7_uint32     vlanNum,
                              L7_uint32     aclDir,
                              L7_ACL_TYPE_t aclType,
                              L7_uint32     aclId );

/*********************************************************************
*
* @purpose  To purge all access lists from the specified vlan and direction.
*
* @param    vlanNum     @b{(input)}  internal vlan number
* @param    aclDir      @b{(input)}  vlan direction (internal)
*
* @returns  L7_SUCCESS  all ACLs successfully purged from vlan, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
*
* @comments This internal function is used for purging all types of ACLs
*           from an vlan.  Any type-specific validity checking must be
*           performed by the caller.
*
* @comments The caller is required to provide proper semaphore protection.
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanDirListPurge( L7_uint32 vlanNum, L7_uint32 aclDir );

/*********************************************************************
*
* @purpose  Check if the specified vlan and direction is valid
*           for use with ACL
*
* @param    vlanNum     @b{(input)}  internal vlan number
* @param    aclDir      @b{(input)}  vlan direction (internal)
*
* @returns  L7_SUCCESS  vlan,dir is valid for ACL
* @returns  L7_ERROR    vlan,dir not valid for ACL
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The vlan type and direction supported for ACL use
*           depends on the capabilities of the platform.
*
* @comments Note that this API only indicates whether the vlan
*           and direction are usable for ACLs.  It does not indicate
*           whether any ACLs are currently assigned to the vlan
*           and direction (see aclImpIsVlanInUse).
*
* @end
*
*********************************************************************/
L7_RC_t aclImpVlanDirGet( L7_uint32 vlanNum, L7_uint32 aclDir );

/*********************************************************************
*
* @purpose  Get next sequential vlan and direction that is valid
*           for use with ACL
*
* @param    vlanNum         @b{(input)}  internal vlan number
* @param    aclDir          @b{(input)}  vlan direction (internal)
* @param    *nextVlanNum    @b{(output)} next internal vlan number
* @param    *nextAclDir     @b{(output)} next vlan direction (internal)
*
* @returns  L7_SUCCESS  next vlan,dir for ACL was obtained
* @returns  L7_ERROR    no more valid vlan,dir for ACL
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The vlan type and direction supported for ACL use
*           depends on the capabilities of the platform.
*
* @comments Note that this API only provides an vlan and direction
*           that is usable for ACLs.  It does not indicate whether
*           any ACLs are currently assigned to the vlan
*           and direction (see aclImpIsVlanInUse).
*
* @end
*
*********************************************************************/
L7_RC_t aclImpVlanDirGetNext( L7_uint32 vlanNum,
                              L7_uint32 aclDir,
                              L7_uint32 *nextVlanNum,
                              L7_uint32 *nextAclDir );

/*********************************************************************
*
* @purpose  Check if the ACL sequence number is in use for the specified
*           vlan and direction
*
* @param    vlanNum     @b{(input)}  internal vlan number
* @param    aclDir      @b{(input)}  vlan direction (internal)
* @param    seqNum      @b{(input)}  ACL vlan sequence number
*
* @returns  L7_SUCCESS  sequence number is in use
* @returns  L7_ERROR    sequence number not in use
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments A sequence number value may be used only once for a given
*           vlan and direction pair.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpVlanDirSequenceGet( L7_uint32 vlanNum,
                                  L7_uint32 aclDir,
                                  L7_uint32 seqNum );

/*********************************************************************
*
* @purpose  Get next sequential ACL vlan sequence number in use
*           for the specified vlan and direction.
*
* @param    vlanNum     @b{(input)}  internal vlan number
* @param    aclDir      @b{(input)}  vlan direction (internal)
* @param    seqNum      @b{(input)}  ACL vlan sequence number
* @param    *nextSeqNum @b{(output)} next ACL vlan sequence number
*
* @returns  L7_SUCCESS  next sequence number found
* @returns  L7_ERROR    next sequence number not found
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments Use a seqNum value of 0 to find the first ACL sequence
*           number for this vlan and direction, if it exists.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpVlanDirSequenceGetNext( L7_uint32 vlanNum,
                                      L7_uint32 aclDir,
                                      L7_uint32 seqNum,
                                      L7_uint32 *nextSeqNum );

/*********************************************************************
*
* @purpose  Get a aclStructure_t where the head is rule rulenum
*
* @param    L7_uint32   rulenum  the current rule ID
* @param    L7_uint32   *acl_ptr  reference to the rule
*
* @returns  L7_SUCCESS, if the next rule ID was found
* @returns  L7_ERROR,   if aclnum or rule does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclRuleGet(L7_uint32 aclnum, L7_uint32 rulenum, aclRuleParms_t **acl_rp);

/*********************************************************************
*
* @purpose  Apply collateral config related to this interface 
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is typically used to update the device when config
*           such as for redirect and mirror interfaces come and go.
*
* @comments The intIfNum parm represents the potential target interface
*           for redirect or mirroring, not the current interface(s) to 
*           which an ACL containing those attributes is applied.
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpCollateralConfigApply(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Get the first access list rule given an ACL ID
*
* @param    L7_uint32   aclnum   the access-list identifying number
* @param    L7_uint32   *rule    the next rule ID
*
* @returns  L7_SUCCESS, if the first rule for this ACL ID was found
* @returns  L7_FAILURE, 
* @returns  L7_ERROR,   if no rules have been created for this ACL
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpRuleGetFirst(L7_uint32 aclnum, L7_uint32 *rule);

/*********************************************************************
*
* @purpose  Get the next access list rule given an ACL ID
*
* @param    L7_uint32   aclnum   the access-list ID
* @param    L7_uint32   rule     the current rule ID
* @param    L7_uint32  *next     the next rule ID
*
* @returns  L7_SUCCESS, if the next rule ID was found
* @returns  L7_FAILURE, if rule is the last valid rule ID for this ACL
* @returns  L7_ERROR,   if aclnum or rule does not exist
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpRuleGetNext(L7_uint32 aclnum, L7_uint32 rule, L7_uint32 *next);

/*********************************************************************
*
* @purpose  Checks to see if a user has configured a field in a rule
*           in an access list
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  field     ruleFields_t enum 
*
* @returns  L7_TRUE, if the field has been configured
* @returns  L7_FALSE, if the field has not been configured
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_BOOL aclImpIsFieldConfigured(L7_uint32 aclnum, L7_uint32 rulenum, 
                                L7_uint32 field);

/*********************************************************************
*
* @purpose  Gets the list of ACLs currently assigned to the specified
*           interface and direction
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    aclDir      @b{(input)}  interface direction (internal)
* @param    *listInfo   @b{(output)} list of assigned ACLs
*
* @returns  L7_SUCCESS  list of ACLs was built
* @returns  L7_FAILURE  invalid interface, direction, or other failure
* @returns  L7_ERROR    no access list assigned to the interface in this 
*                       direction
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments An ACL can be shared/associated with multiple interfaces.
*
* @comments The aclId contained in the output listInfo must be interpreted 
*           based on the aclType (for example, aclnum for IP, aclIndex for 
*           MAC).
*
* @end
*
*********************************************************************/
L7_RC_t aclImpIntfDirAclListGet(L7_uint32 intIfNum, L7_uint32 aclDir,
                                L7_ACL_INTF_DIR_LIST_t *listInfo);

/*********************************************************************
*
* @purpose  Gets a list of interfaces to which an ACL is assigned in the
*           specified direction
*
* @param    L7_uint32  aclnum      the access-list identifying number
* @param    L7_uint32  aclDir      the interface direction (internal) 
* @param    L7_uint32  *intfList   list of interfaces (L7_ACL_INTF_MAX_COUNT long)
*
* @returns  L7_SUCCESS, if a list of interfaces was built
* @returns  L7_FAILURE, if invalid identifying number or 
*                       internal interface number does not exist
* @returns  L7_ERROR,   if access-list not assigned to any interface
*                       in this direction (no interface list was built)
*
* @comments An ACL can be shared/associated with multiple interfaces.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpAssignedIntfDirListGet(L7_uint32 aclnum, L7_uint32 aclDir,
                                     L7_ACL_ASSIGNED_INTF_LIST_t *intfList);

/*********************************************************************
*
* @purpose  To add an access list to the specified interface and direction.
*
* @param    L7_uint32  intIfNum   the internal interface number
* @param    L7_uint32  aclDir     the interface direction (internal) 
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  seqNum     the ACL evaluation order sequence number
*
* @returns  L7_SUCCESS, if ACL successfully added to intf,dir
* @returns  L7_FAILURE, if invalid parameter, or other failure
* @returns  L7_ERROR, if ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED, if intf,dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED, if feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL, if ACL assignment at capacity for intf, dir
* @returns  L7_REQUEST_DENIED, if error applying ACL to hardware
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpInterfaceDirectionAdd(L7_uint32 intIfNum, L7_uint32 aclDir, 
                                    L7_uint32 aclnum, L7_uint32 seqNum);

/*********************************************************************
*
* @purpose  Checks if ACL type is compatible for interface and direction.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    aclDir      @b{(input)} interface direction to apply ACL (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    aclType     @b{(input)} access list type
*
* @returns  L7_SUCCESS        ACL type combination allowed
* @returns  L7_NOT_SUPPORTED  ACL type combination not allowed
* @returns  L7_FAILURE        invalid parameter, or other failure
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments Function compares ACL types already assigned to this interface
*           and direction with this ACL against the feature support.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpIntfAclTypeCompatCheck(L7_uint32 intIfNum, L7_uint32 aclDir, 
                                     L7_ACL_TYPE_t aclType);

/*********************************************************************
*
* @purpose  To add an access list to the specified interface and direction.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    aclDir      @b{(input)} interface direction to apply ACL (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    aclType     @b{(input)} access list type  
* @param    aclId       @b{(input)} access list identifier
* @param    seqNum      @b{(input)} ACL evaluation order sequence number
*
* @returns  L7_SUCCESS  ACL successfully added to intf, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
* @returns  L7_ERROR    ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED  intf, dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED  feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL  ACL assignment at capacity for intf, dir
* @returns  L7_REQUEST_DENIED Error applying ACL to hardware
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpIntfDirAdd(L7_uint32 intIfNum, L7_uint32 aclDir, 
                         L7_ACL_TYPE_t aclType, L7_uint32 aclId, 
                         L7_uint32 seqNum);

/*********************************************************************
*
* @purpose  To remove an access list from the specified interface and direction.
*
* @param    L7_uint32  intIfNum   the internal interface number
* @param    L7_uint32  aclDir     the interface direction (internal) 
* @param    L7_uint32  aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if intIfNum is removed from the access-list
* @returns  L7_FAILURE, if invalid identifying number or 
*                       internal interface number does not exist
* @returns  L7_ERROR, if access-list not found for this intf, dir
* @returns  L7_REQUEST_DENIED, if error removing ACL from hardware
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpInterfaceDirectionRemove(L7_uint32 intIfNum, L7_uint32 alcDir, 
                                       L7_uint32 aclnum);

/*********************************************************************
*
* @purpose  To remove an access list from the specified interface and direction.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    aclDir      @b{(input)} interface direction to apply ACL (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    aclType     @b{(input)} access list type
* @param    aclId       @b{(input)} access list identifier
*
* @returns  L7_SUCCESS  ACL successfully removed from intf,dir
* @returns  L7_FAILURE  invalid ACL identifier, or interface does not exist
* @returns  L7_ERROR    ACL does not exist for this id, or not assigned
*                         to the intf,dir
* @returns  L7_REQUEST_DENIED error removing ACL from hardware
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpIntfDirRemove(L7_uint32 intIfNum, L7_uint32 aclDir, 
                            L7_ACL_TYPE_t aclType, L7_uint32 aclId);

/*********************************************************************
*
* @purpose  To check if an interface and direction is in use by any access list.
*
* @param    L7_uint32  intIfNum  the internal interface number
* @param    L7_uint32  direction the interface direction
*
* @returns  L7_TRUE, if the intf,dir has at least one ACL attached
* @returns  L7_FALSE, if the intf,dir has no ACL attached
*
* @comments The function assumes the interface, intIfNum is configurable
*           i.e. aclMapIntfIsConfigurable() returns L7_TRUE
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_BOOL aclImpIsInterfaceInUse(L7_uint32 intIfNum, L7_uint32 direction);

/*********************************************************************
*
* @purpose  To check if an ACL is assigned to this interface and direction.
*
* @param    L7_uint32     intIfNum    the internal interface number
* @param    L7_uint32     aclDir      the interface direction (internal)
* @param    L7_ACL_TYPE_t aclType     the type of ACL
* @param    L7_uint32     aclId       the ACL identifier
* @param    L7_uint32*    listIndex   the assign list index for this ACL
*
* @returns  L7_SUCCESS, if the ACL id was found for intf,dir
* @returns  L7_FAILURE, if the ACL id not found for intf,dir
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The function assumes the interface, intIfNum is configurable
*           i.e. aclMapIntfIsConfigurable() returns L7_TRUE
* @end
*
*********************************************************************/
L7_RC_t aclImpIntfDirAclIdFind(L7_uint32 intIfNum, L7_uint32 aclDir,
                               L7_ACL_TYPE_t aclType, L7_uint32 aclId,
                               L7_uint32 *listIndex);

/*********************************************************************
*
* @purpose  Gets the list of ACLs currently assigned to the specified
*           vlan and direction
*
* @param    vlanNum     @b{(input)}  internal vlan number
* @param    aclDir      @b{(input)}  vlan direction (internal)
* @param    *listInfo   @b{(output)} list of assigned ACLs
*
* @returns  L7_SUCCESS  list of ACLs was built
* @returns  L7_FAILURE  invalid vlan, direction, or other failure
* @returns  L7_ERROR    no access list assigned to the vlan in this
*                       direction
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments An ACL can be shared/associated with multiple vlans.
*
* @comments The aclId contained in the output listInfo must be interpreted
*           based on the aclType (for example, aclnum for IP, aclIndex for
*           MAC).
*
* @end
*
*********************************************************************/
L7_RC_t aclImpVlanDirAclListGet( L7_uint32               vlanNum,
                                 L7_uint32               aclDir,
                                 L7_ACL_VLAN_DIR_LIST_t *listInfo );

/*********************************************************************
*
* @purpose  Gets a list of vlans to which an ACL is assigned in the
*           specified direction
*
* @param    L7_uint32  aclnum      the access-list identifying number
* @param    L7_uint32  aclDir      the vlan direction (internal)
* @param    L7_uint32  *vlanList   list of vlans (L7_ACL_VLAN_MAX_COUNT long)
*
* @returns  L7_SUCCESS, if a list of vlans was built
* @returns  L7_FAILURE, if invalid identifying number or
*                       internal vlan number does not exist
* @returns  L7_ERROR,   if access-list not assigned to any vlan
*                       in this direction (no vlan list was built)
*
* @comments An ACL can be shared/associated with multiple vlans.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpAssignedVlanDirListGet( L7_uint32                    aclnum,
                                      L7_uint32                    aclDir,
                                      L7_ACL_ASSIGNED_VLAN_LIST_t *vlanList );

/*********************************************************************
*
* @purpose  To add an access list to the specified vlan and direction.
*
* @param    L7_uint32  vlanNum    the internal vlan number
* @param    L7_uint32  aclDir     the vlan direction (internal)
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  seqNum     the ACL evaluation order sequence number
*
* @returns  L7_SUCCESS, if ACL successfully added to vlan,dir
* @returns  L7_FAILURE, if invalid parameter, or other failure
* @returns  L7_ERROR, if ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED, if vlan,dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED, if feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL, if ACL assignment at capacity for vlan, dir
* @returns  L7_REQUEST_DENIED, if error applying ACL to hardware
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpVlanDirectionAdd( L7_uint32 vlanNum,
                                L7_uint32 aclDir,
                                L7_uint32 aclnum,
                                L7_uint32 seqNum );

/*********************************************************************
*
* @purpose  Checks if ACL type is compatible for VLAN and direction.
*
* @param    blanNum     @b{(input)} VLAN ID 
* @param    aclDir      @b{(input)} interface direction to apply ACL (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    aclType     @b{(input)} access list type
*
* @returns  L7_SUCCESS        ACL type combination allowed
* @returns  L7_NOT_SUPPORTED  ACL type combination not allowed
* @returns  L7_FAILURE        invalid parameter, or other failure
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments Function compares ACL types already assigned to this VLAN ID
*           and direction with this ACL against the feature support.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpVlanAclTypeCompatCheck(L7_uint32 vlanNum, L7_uint32 aclDir, 
                                     L7_ACL_TYPE_t aclType);

/*********************************************************************
*
* @purpose  To add an access list to the specified vlan and direction.
*
* @param    vlanNum     @b{(input)} internal vlan number
* @param    aclDir      @b{(input)} vlan direction to apply ACL (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    aclType     @b{(input)} access list type
* @param    aclId       @b{(input)} access list identifier
* @param    seqNum      @b{(input)} ACL evaluation order sequence number
*
* @returns  L7_SUCCESS  ACL successfully added to vlan, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
* @returns  L7_ERROR    ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED  vlan, dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED  feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL  ACL assignment at capacity for vlan, dir
* @returns  L7_REQUEST_DENIED Error applying ACL to hardware
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpVlanDirAdd( L7_uint32     vlanNum,
                          L7_uint32     aclDir,
                          L7_ACL_TYPE_t aclType,
                          L7_uint32     aclId,
                          L7_uint32     seqNum );

/*********************************************************************
*
* @purpose  To remove an access list from the specified vlan and direction.
*
* @param    L7_uint32  vlanNum    the internal vlan number
* @param    L7_uint32  aclDir     the vlan direction (internal)
* @param    L7_uint32  aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if vlanNum is removed from the access-list
* @returns  L7_FAILURE, if invalid identifying number or
*                       internal vlan number does not exist
* @returns  L7_ERROR, if access-list not found for this vlan, dir
* @returns  L7_REQUEST_DENIED, if error removing ACL from hardware
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpVlanDirectionRemove( L7_uint32 vlanNum,
                                   L7_uint32 aclDir,
                                   L7_uint32 aclnum );

/*********************************************************************
*
* @purpose  To remove an access list from the specified vlan and direction.
*
* @param    vlanNum     @b{(input)} internal vlan number
* @param    aclDir      @b{(input)} vlan direction to apply ACL (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    aclType     @b{(input)} access list type
* @param    aclId       @b{(input)} access list identifier
*
* @returns  L7_SUCCESS  ACL successfully removed from vlan,dir
* @returns  L7_FAILURE  invalid ACL identifier, or vlan does not exist
* @returns  L7_ERROR    ACL does not exist for this id, or not assigned
*                         to the vlan,dir
* @returns  L7_REQUEST_DENIED error removing ACL from hardware
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpVlanDirRemove( L7_uint32     vlanNum,
                             L7_uint32     aclDir,
                             L7_ACL_TYPE_t aclType,
                             L7_uint32     aclId );

/*********************************************************************
*
* @purpose  Get next sequential vlan ID configured with an ACL binding.
*
* @param    vlanNum       @b{(input)}  current vlan ID
* @param    *nextVlanNum  @b{(input)}  next vlan ID, if any
*
* @returns  L7_SUCCESS  next vlan ID was obtained
* @returns  L7_ERROR    invalid parameter or other failure
* @returns  L7_FAILURE  no more vlan IDs found
*
* @comments Use a vlanid value of 0 to find the first vlan ID configured
*           with an ACL binding.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpVlanIdGetNext(L7_uint32 vlanNum, L7_uint32 *nextVlanNum);

/*********************************************************************
*
* @purpose  To check if an vlan and direction is in use by any access list.
*
* @param    L7_uint32  vlanNum   the internal vlan number
* @param    L7_uint32  aclDir    the vlan direction (internal)
*
* @returns  L7_TRUE, if the vlan,dir has at least one ACL attached
* @returns  L7_FALSE, if the vlan,dir has no ACL attached
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The function assumes the vlan, vlanNum is configurable
*           i.e. aclMapVlanIsConfigurable() returns L7_TRUE
* @end
*
*********************************************************************/
L7_BOOL aclImpIsVlanInUse( L7_uint32 vlanNum, L7_uint32 aclDir );

/*********************************************************************
*
* @purpose  To check if an ACL is assigned to this vlan and direction.
*
* @param    L7_uint32     vlanNum     the internal vlan number
* @param    L7_uint32     aclDir      the vlan direction (internal)
* @param    L7_ACL_TYPE_t aclType     the type of ACL
* @param    L7_uint32     aclId       the ACL identifier
* @param    L7_uint32*    listIndex   the assign list index for this ACL
*
* @returns  L7_SUCCESS, if the ACL id was found for vlan,dir
* @returns  L7_FAILURE, if the ACL id not found for vlan,dir
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The function assumes the vlan, vlanNum is configurable
*           i.e. aclMapVlanIsConfigurable() returns L7_TRUE
* @end
*
*********************************************************************/
L7_RC_t aclImpVlanDirAclIdFind( L7_uint32     vlanNum,
                                L7_uint32     aclDir,
                                L7_ACL_TYPE_t aclType,
                                L7_uint32     aclId,
                                L7_uint32    *listIndex );

/*********************************************************************
*
* @purpose  Notify registered users of a change to an ACL.
*
* @param    event       Event indication
* @param    aclType     Access list type
* @param    aclnum      ACL associated with the event
* @param    aclOldName  Previous ACL name, or L7_NULLPTR
*
* @returns  void
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments It is important that the caller only hold a read lock.
*           The route filter callback functions are likely to call 
*           aclRouteFilter(), so using a write lock here would
*           cause a deadlock during the callback.
*
* @comments The aclOldName parm is used for a rename or delete event
*           only, and is set to L7_NULLPTR otherwise.
*
* @end
*
*********************************************************************/
void aclImpNotify(aclEvent_t event, L7_ACL_TYPE_t aclType, 
                  L7_uint32 aclnum, L7_uchar8 *aclOldName);

/*********************************************************************
*
* @purpose  Ask registered applications if the proposed ACL change is permissible.
*
* @param    aclChange   Change approval ID
* @param    aclnum      ACL associated with the change
* @param    pData       Ptr to request-specific data, or L7_NULLPTR
*
* @returns  L7_SUCCESS  Allow ACL change
* @returns  L7_FAILURE  Do not allow ACL change
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments Gives registered apps the chance to block an ACL from 
*           being changed by the user when it is being used elsewhere
*           in the switch configuration.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpChangeApprovalQuery(aclChangeApproval_t aclChange, L7_uint32 aclnum, void *pData);

/*************************************************************************
* @purpose  Get the current number of IP ACLs configured
*
* @param    pCurr  @b{(output)} Pointer to the current value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*********************************************************************/
L7_RC_t aclImpCurrNumGet(L7_uint32 *pCurr);

/*************************************************************************
* @purpose  Check if IP ACL table is full
*
* @param    void
*
* @returns  L7_TRUE     ACL table is full
* @returns  L7_FALSE    ACL table not full
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*********************************************************************/
L7_BOOL aclImpIsTableFull(void);

/*************************************************************************
* @purpose  Check if maximum number of access lists of any type have been created
*
* @param    void
*
* @returns  L7_TRUE     maximum number of ACLs exist
* @returns  L7_FALSE    ACLs below maximum number
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*********************************************************************/
L7_BOOL aclImpIsTotalListsAtMax(void);

/*********************************************************************
*
* @purpose  Checks if the ACL ID belongs to one of the standard ACL number ranges
*
* @param    L7_uint32   aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list ID belongs to a supported range
* @returns  L7_FAILURE, if the access list ID is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpStdAclNumRangeCheck(L7_uint32 aclnum);

/*********************************************************************
*
* @purpose  Checks if the ACL ID belongs to one of the extended ACL number ranges
*
* @param    L7_uint32   aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list ID belongs to a supported range
* @returns  L7_FAILURE, if the access list ID is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpExtAclNumRangeCheck(L7_uint32 aclnum);

/*********************************************************************
*
* @purpose  Checks if the ACL ID belongs to any extended IP ACL range
*
* @param    L7_uint32   aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list ID belongs to a supported range
* @returns  L7_FAILURE, if the access list ID is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments Both numbered and named IP extended ACLs are considered here,
*           since these support the same ACL rule definitions.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpExtGroupRangeCheck(L7_uint32 aclnum);

/*********************************************************************
*
* @purpose  Checks if the ACL ID belongs to any extended IP or IPV6 ACL range
*
* @param    L7_uint32   aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list ID belongs to a supported range
* @returns  L7_FAILURE, if the access list ID is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments Both numbered and named IP extended ACLs are considered here,
*           as well as named IPV6 ACLs.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpExtGroupOrIpv6RangeCheck(L7_uint32 aclnum);

/*********************************************************************
*
* @purpose  Checks if the ACL number belongs to any of the supported named 
*           IP ACL index ranges
*
* @param    L7_uint32   aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list ID belongs to a supported range
* @returns  L7_FAILURE, if the access list ID is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments This function only considers whether the aclnum belongs to
*           any supported named ACL index range.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpIndexRangeCheck(L7_uint32 aclnum);

/*********************************************************************
*
* @purpose  Output the ACL type based on the specified ACL number
*
* @param    aclnum      @b{(input)}  access-list identifying number
*
* @returns  L7_ACL_TYPE_t  the access-list type
*
* @comments Outputs L7_ACL_TYPE_NONE if the ACL number is unknown
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_ACL_TYPE_t aclImpTypeFromIndexGet(L7_uint32 aclnum);

/*********************************************************************
*
* @purpose  Outputs the IP ACL type if it belongs to one of the supported 
*           number ranges
*
* @param    L7_uint32     aclnum    the access-list identifying number
* @param    L7_ACL_TYPE_t *acltype  the access-list type
*
* @returns  L7_SUCCESS, if the access list ID belongs to a supported range
* @returns  L7_FAILURE, if the access list ID is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpAclTypeGet(L7_uint32 aclnum, L7_ACL_TYPE_t *acltype);

/*********************************************************************
*
* @purpose  Checks if the ACL rule number belongs to a supported number range
*
* @param    L7_uint32   aclnum     the access-list identifying number
* @param    L7_uint32   rulenum    the current rule ID
*
* @returns  L7_SUCCESS, if the rule ID belongs to a supported range
* @returns  L7_FAILURE, if the rule ID is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The aclnum parameter is required by this function in case there 
*           is ever a need to support separate rule number ranges based
*           on the type of ACL (i.e., standard vs. extended).
*
* @end
*
*********************************************************************/
L7_RC_t aclImpRuleNumRangeCheck(L7_uint32 aclnum, L7_uint32 rulenum);

/*********************************************************************
*
* @purpose  Gets the current number of configured rules in an IP ACL
*
* @param    aclnum      @b{(input)}  access-list identifying number
* @param    *ruleCount  @b{(output)} number of rules in ACL
*
* @returns  L7_SUCCESS  ACL rule count retrieved
* @returns  L7_ERROR    access list does not exist
* @returns  L7_FAILURE  access list index is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The output ruleCount does not include the implicit 'deny all'
*           rule contained in every ACL definition.
*
* @end
*
*********************************************************************/
L7_RC_t aclImpAclRuleCountGet(L7_uint32 aclnum, L7_uint32 *ruleCount);

/*************************************************************************
*
* @purpose  Check if rule logging is allowed for a given rule action
*
* @param    action      @b{(input)}  access list rule action
*
* @returns  L7_TRUE     logging allowed for rule
* @returns  L7_FALSE    logging not allowed for rule
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*********************************************************************/
L7_BOOL aclImpLoggingIsAllowed(L7_ACL_ACTION_t action);

/*********************************************************************
* @purpose  Display the current IP ACL Table contents
*
* @param    showRules   @b{(input)} Indicates if ACL rules are displayed
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void aclIpShow(L7_BOOL showRules);

/*********************************************************************
* @purpose  Display content of all rules for a specific IP ACL
*
* @param    aclNum      @b{(input)} IP access list index
*
* @returns  void
*
* @comments Assumes caller handles any necessary semaphore protection.
*
* @end
*********************************************************************/
void aclIpRulesShow(L7_uint32 aclNum);

/*********************************************************************
* @purpose  Display content of all rules for a specific IPV6 ACL
*
* @param    aclIndex    @b{(input)} IPV6 access list index
*
* @returns  void
*
* @comments Assumes caller handles any necessary semaphore protection.
*
* @end
*********************************************************************/
void aclIpv6RulesShow(L7_uint32 aclIndex);

/*********************************************************************
* @purpose  Display contents of an IP ACL rule
*
* @param    aclType     @b{(input)} Access list type
* @param    *pRule      @b{(input)} ACL rule pointer
* @param    msgLvlReqd  @b{(input)} Desired output message level
*
* @returns  void
*
* @comments Assumes caller handles any necessary semaphore protection.
*
* @end
*********************************************************************/
void aclIpRuleDisplay(L7_ACL_TYPE_t aclType, aclRuleParms_t *pRule, L7_uint32 msgLvlReqd);

/*********************************************************************
* @purpose  Display the ACL rule config mask contents
*
* @param    aclType     @b{(input)} Access list type
* @param    mask        @b{(input)} ACL rule config mask
* @param    msgLvlReqd  @b{(input)} Debug print message level
*
* @returns  void
*
* @comments This can be used for any ACL type.
*
* @end
*********************************************************************/
void aclRuleCfgMaskPrint(L7_ACL_TYPE_t aclType, L7_uint32 mask, L7_uint32 msgLvlReqd);

/*********************************************************************
*
* @purpose  Creates the ACL correlator table
*
* @param    maxEntries  @{(input)}  maximum number of entries in table
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t aclCorrTableCreate(L7_uint32 maxEntries);

/*********************************************************************
*
* @purpose  Deletes the ACL correlator table
*
* @param    void
*
* @returns  void
*
* @comments 
*
* @end
*
*********************************************************************/
void aclCorrTableDelete(void);

/*********************************************************************
*
* @purpose  Finds the specified correlator entry in ACL correlator table
*
* @param    correlator  @b{(input)}  correlator of entry to find
* @param    flags       @b{(input)}  search flags (L7_MATCH_EXACT, L7_MATCH_GETNEXT)
*
* @returns  aclCorrTable_t *   Ptr to ACL correlator table entry, if found
* @returns  L7_NULLPTR, if entry not found
*
* @comments Use a correlator value of 0 to find first entry in table.
*
* @end
*
*********************************************************************/
aclCorrTable_t *aclCorrTableEntryFind(L7_uint32 correlator, L7_uint32 flags);

/*********************************************************************
*
* @purpose  Insert specified correlator entry in ACL correlator table
*
* @param    L7_uint32       correlator  rule correlator to be added
*
* @returns  L7_SUCCESS        correlator entry was inserted/updated
* @returns  L7_TABLE_IS_FULL  correlator table is full
* @returns  L7_FAILURE        all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclCorrTableEntryInsert(L7_uint32 correlator);

/*********************************************************************
*
* @purpose  Remove specified correlator entry from ACL correlator table
*
* @param    L7_uint32   correlator  rule correlator to be removed
*
* @returns  L7_SUCCESS  correlator entry was removed/updated
* @returns  L7_ERROR    correlator entry not found
* @returns  L7_FAILURE  all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclCorrTableEntryRemove(L7_uint32 correlator);

/*********************************************************************
*
* @purpose  Displays contents of ACL correlator table
*
* @param    correlator  @b{(input)}  specific correlator, or 0 to show all
*
* @returns  void
*
* @comments 
*
* @end
*
*********************************************************************/
void aclCorrTableShow(L7_uint32 correlator);

/*********************************************************************
*
* @purpose  Encodes an ACL rule correlator
*
* @param    aclType       @b{(input)}  access list type        
* @param    aclId         @b{(input)}  access list identifier
* @param    rulenum       @b{(input)}  access list rule number
* @param    ruleAction    @b{(input)}  access list rule action
*
* @returns  L7_uint32     correlator value
*
* @comments The correlator value is treated as opaque by HAPI.
*
* @end
*
*********************************************************************/
L7_uint32 aclCorrEncode(L7_ACL_TYPE_t aclType, L7_uint32 aclId,
                        L7_uint32 rulenum, L7_ACL_ACTION_t ruleAction);

/*********************************************************************
*
* @purpose  Decodes an ACL rule correlator
*
* @param    correlator    @b{(input)}  correlator value
* @param    *aclType      @b{(output)} access list type        
* @param    *aclId        @b{(output)} access list identifier
* @param    *rulenum      @b{(output)} access list rule number
* @param    *ruleAction   @b{(output)} access list rule action
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      correlator value was null
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t aclCorrDecode(L7_uint32 correlator, L7_ACL_TYPE_t *aclType, 
                      L7_uint32 *aclId, L7_uint32 *rulenum,
                      L7_ACL_ACTION_t *ruleAction);

/*********************************************************************
* @purpose  Initialize ACL component callback support
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t aclCallbackInit(void);

/*********************************************************************
* @purpose  Clean up ACL component callback support after a failed init
*
* @param    void
*
* @returns  void
*
* @comments 
*       
* @end
*********************************************************************/
void aclCallbackFini(void);

/*********************************************************************
*
* @purpose  ACL log task
*
* @param    void
*
* @returns  void
*
* @comments Collects ACL rule log counts from the hardware and presents
*           them via trap mechanism.
*
* @comments Not all platforms support ACL rule logging, so there are 
*           cases where the ACL correlator table is not allocated and
*           this task is never started.
*
* @end
*
*********************************************************************/
void aclLogTask(void);

/*********************************************************************
*
* @purpose  Sets ACL message level value
*
* @param    sleepTime   @b{(input)}  task sleep time (in seconds)
*
* @returns  void
*
* @comments This is a debug function intended for ENGINEERING USE ONLY!
*
* @comments Setting a value of 0 restores the default sleep interval.
*
* @end
*
*********************************************************************/
void aclLogTaskSleepTimeSet(L7_int32 sleepSecs);

/**************************************************************************
* @purpose  Take an ACL semaphore
*
* @param    semId   @{(input)} Semaphore id
* @param    timeout @{(input)} Time to wait (in ticks), or L7_WAIT_FOREVER 
*                                or L7_NO_WAIT
* @param    file    @{(input)} File name of caller (for error logging)
* @param    line    @{(input)} File line number of caller (for error logging)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Wrapper function for OSAPI call, so that errors can be checked
*           in one place.  The assumption here is that there are no "expected"
*           errors (i.e., the semaphore id is legitimate and the caller
*           typically waits forever), so any error gets logged.
*
* @notes    DO NOT USE if specifying a timeout other than L7_WAIT_FOREVER  
*           where awareness of the time expiration is needed.  Make a direct
*           call to osapiSemaTake() instead.
*
* @end
*************************************************************************/
L7_RC_t aclSemaTake(void *semId, L7_int32 timeout, char *file, L7_ulong32 line);

/**************************************************************************
* @purpose  Give an ACL semaphore
*
* @param    semId   @{(input)} Semaphore id
* @param    file    @{(input)} File name of caller (for error logging)
* @param    line    @{(input)} File line number of caller (for error logging)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Wrapper function for OSAPI call, so that errors can be checked
*           in one place.  The assumption here is that there are no "expected"
*           errors (i.e., the semaphore id is legitimate) so any error gets
*           logged.
*
* @end
*************************************************************************/
L7_RC_t aclSemaGive(void *semId, char *file, L7_ulong32 line);

/**************************************************************************
* @purpose  Take an ACL read lock
*
* @param    rwlock  @{(input)} Read/Write lock id
* @param    timeout @{(input)} Time to wait (in ticks), or L7_WAIT_FOREVER 
*                                or L7_NO_WAIT
* @param    file    @{(input)} File name of caller (for error logging)
* @param    line    @{(input)} File line number of caller (for error logging)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Wrapper function for OSAPI call, so that errors can be checked
*           in one place.  The assumption here is that there are no "expected"
*           errors (i.e., the r/w lock id is legitimate and the caller
*           typically waits forever), so any error gets logged.
*
* @notes    DO NOT USE if specifying a timeout other than L7_WAIT_FOREVER  
*           where awareness of the time expiration is needed.  Make a direct
*           call to osapiReadLockTake() instead.
*
* @end
*************************************************************************/
L7_RC_t aclReadLockTake(osapiRWLock_t rwlock, L7_int32 timeout, char *file, L7_ulong32 line);

/**************************************************************************
* @purpose  Give an ACL read lock
*
* @param    rwlock  @{(input)} Read/Write lock id
* @param    file    @{(input)} File name of caller (for error logging)
* @param    line    @{(input)} File line number of caller (for error logging)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Wrapper function for OSAPI call, so that errors can be checked
*           in one place.  The assumption here is that there are no "expected"
*           errors (i.e., the r/w lock id is legitimate) so any error gets
*           logged.
*
* @end
*************************************************************************/
L7_RC_t aclReadLockGive(osapiRWLock_t rwlock, char *file, L7_ulong32 line);

/**************************************************************************
* @purpose  Take an ACL write lock
*
* @param    rwlock  @{(input)} Read/Write lock id
* @param    timeout @{(input)} Time to wait (in ticks), or L7_WAIT_FOREVER 
*                                or L7_NO_WAIT
* @param    file    @{(input)} File name of caller (for error logging)
* @param    line    @{(input)} File line number of caller (for error logging)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Wrapper function for OSAPI call, so that errors can be checked
*           in one place.  The assumption here is that there are no "expected"
*           errors (i.e., the r/w lock id is legitimate and the caller
*           typically waits forever), so any error gets logged.
*
* @notes    DO NOT USE if specifying a timeout other than L7_WAIT_FOREVER  
*           where awareness of the time expiration is needed.  Make a direct
*           call to osapiWriteLockTake() instead.
*
* @end
*************************************************************************/
L7_RC_t aclWriteLockTake(osapiRWLock_t rwlock, L7_int32 timeout, char *file, L7_ulong32 line);

/**************************************************************************
* @purpose  Give an ACL write lock
*
* @param    rwlock  @{(input)} Read/Write lock id
* @param    file    @{(input)} File name of caller (for error logging)
* @param    line    @{(input)} File line number of caller (for error logging)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Wrapper function for OSAPI call, so that errors can be checked
*           in one place.  The assumption here is that there are no "expected"
*           errors (i.e., the r/w lock id is legitimate) so any error gets
*           logged.
*
* @end
*************************************************************************/
L7_RC_t aclWriteLockGive(osapiRWLock_t rwlock, char *file, L7_ulong32 line);

/*********************************************************************
*
* @purpose  Gets current ACL message level setting
*
* @param    void                       
*
* @returns  L7_uint32   message level value
*
* @comments The ACL message level does not persist across a system reset.
*
* @end
*
*********************************************************************/
L7_uint32 aclMsgLvlGet(void);

/*********************************************************************
*
* @purpose  Sets ACL message level value
*
* @param    L7_uint32   msgLvl     
*
* @returns  void
*
* @comments The ACL message level does not persist across a system reset.
*
* @end
*
*********************************************************************/
void aclMsgLvlSet(L7_uint32 msgLvl);


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t aclCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t aclCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t aclCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );

/*********************************************************************
* @purpose  This function undoes aclCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void aclCnfgrFiniPhase1Process();

/*********************************************************************
* @purpose  This function undoes aclCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void aclCnfgrFiniPhase2Process();

/*********************************************************************
* @purpose  This function undoes aclCnfgrInitPhase3Process
*
* @param    none
*     
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void aclCnfgrFiniPhase3Process();


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.   
*
* @param    pResponse - @b{(output)}  Response always command complete.
*     
* @param    pReason   - @b{(output)}  Always 0                    
*
* @returns  L7_SUCCESS - Always return this value. onse is available.
*
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           None.
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t aclCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason );


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/

L7_RC_t aclCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason );



/*********************************************************************
*
* @purpose  To process the Callback for L7_PORT_INSERT 
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t aclApplyIntfConfigData(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  To remove the ACL config data from the interface
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This differs from L7_DELETE processing in that the interface
*           config is removed from the device, but is not removed from the
*           ACL operational config data.
*
* @end
*
*********************************************************************/
L7_RC_t aclUnApplyIntfConfigData(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Apply collateral config related to this interface 
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is typically used to update the device when config
*           such as for redirect and mirror interfaces come and go.
*
* @comments The intIfNum parm represents the potential target interface
*           for redirect or mirroring, not the current interface(s) to 
*           which an ACL containing those attributes is applied.
*
* @end
*
*********************************************************************/
L7_RC_t aclCollateralConfigApply(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE 
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t aclIntfCreate(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to acl interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL aclMapIntfIsConfigurable(L7_uint32 intIfNum, aclIntfCfgData_t **pCfg);

/*********************************************************************
*
* @purpose  To process the Callback for L7_DETACH 
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t aclIntfDetach(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  To process the Callback for L7_PORT_INSERT 
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t aclIntfDelete(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Determines if an interface has been acquired by another 
*           component and is therefore unavailable to ACL.
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_TRUE    interface has been acquired
* @returns  L7_FALSE
*
* @comments The interface is only considered to be not available for 
*           ACL purposes if it is acquired by any of these components:
*              - DOT3AD (LAGs)
*              - port mirroring
*       
* @end
*********************************************************************/
L7_BOOL aclIsIntfAcquired(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Determines if an interface is in an attached state. 
*
* @param    L7_uint32         intIfNum     internal interface number
* @param    L7_INTF_STATES_t  *pIntfState  ptr to output location, or L7_NULLPTR
*
* @returns  L7_TRUE    interface is attached
* @returns  L7_FALSE   interface not attached
*
* @comments An 'attached' state by this definition is one of:
*           L7_INTF_ATTACHING, L7_INTF_ATTACHED, or L7_INTF_DETACHING.
*
* @comments Pass a non-null pointer for the pIntfState parameter
*           to retrieve the NIM interface state value used here.
*       
* @end
*********************************************************************/
L7_BOOL aclIsIntfAttached(L7_uint32 intIfNum, L7_INTF_STATES_t *pIntfState);

/*********************************************************************
*
* @purpose  Apply configuration data for given VLAN ID
*
* @param    L7_uint32  vlanNum  VLAN ID number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t aclApplyVlanConfigData(L7_uint32 vlanNum); 

/*********************************************************************
* @purpose  Obtain a pointer to the specified VLAN configuration data
*           for this interface
*
* @param    vlanNum  @b{(input)} Internal VLAN Number
* @param    **pCfg   @b{(output)}  Ptr  to acl VLAN config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the
*           VLAN exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    The caller can set the pCfg parm to L7_NULLPTR if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL aclMapVlanIsConfigurable(L7_uint32 vlanNum, aclVlanCfgData_t **pCfg);

/*********************************************************************
* @purpose  Obtain a pointer to the first available VLAN config structure
*
* @param    vlanNum  @b{(input)}   Internal VLAN Number
* @param    **pCfg   @b{(output)}  Ptr  to acl VLAN config structure
*                                  or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks whether the 
*           component is in a state to be configured (regardless of 
*           whether the component is enabled or not).
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL aclMapVlanConfigEntryGet( L7_uint32 vlanNum, aclVlanCfgData_t **pCfg );

/*********************************************************************
* @purpose  Free and clear contents of a VLAN config structure
*
* @param    vlanNum  @b{(input)}   Internal VLAN Number
*
*
* @notes    Removes data from a config entry and marks it not in use.
*
* @notes
*
* @end
*********************************************************************/
void aclMapVlanConfigEntryRelease(L7_uint32 vlanNum);

/*********************************************************************
* @purpose  Display a list of debug commands available for ACL
*
* @param    void
*
* @returns  void
*
* @end
*********************************************************************/
void aclDebugHelp(void);

/*********************************************************************
*
* @purpose  Dump the contents of the acl config data.
*
* @param    void
*
* @returns  void
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t aclConfigDataShow(void);

/*********************************************************************
* @purpose  Show the ACL feature support list for this platform
*
* @param    none
*
* @returns  none
*
* @comments
*       
* @end
*********************************************************************/
void aclFeatureShow(void);

/*********************************************************************
*
* @purpose  Build test ACL intf config data
*
* @param    *configId   Interface config ID
* @param    *pCfg       Configuration data ptr for this interface
*
* @returns  void
*
* @comments The test config data consists of arbitrary non-default values.
*
* @end
*
*********************************************************************/
void aclBuildTestIntfConfigData(nimConfigID_t *configId, aclIntfCfgData_t *pCfg);

/*********************************************************************
*
* @purpose  Build test ACL config data
*
* @param    void
*
* @returns  void
*
* @comments The test config data consists of arbitrary non-default values.
*
* @end
*
*********************************************************************/
void aclBuildTestConfigData(void);

/*********************************************************************
*
* @purpose  Save test ACL config data to NVStore
*
* @param    void
*
* @returns  void
*
* @comments Cannot use normal config save, since the config data contents 
*           get overwritten by the operational config.
*
* @end
*
*********************************************************************/
void aclSaveTestConfigData(void);

/*********************************************************************
*
* @purpose  Dump test ACL config data contents
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void aclConfigDataTestShow(void);


/*** MAC ACL Prototypes ***/

/*********************************************************************
*
* @purpose  To create a MAC access list tree.
*
* @param    maxListSize @b{(input)} number of nodes to create in ACL tree
*
* @returns  void
*
* @comments Creates a new instance of MAC ACL, allocating space for up to
*           maxListSize ACLs.
*
* @end
*
*********************************************************************/
void aclMacCreateTree(L7_uint32 maxListSize);

/*********************************************************************
*
* @purpose  To delete a MAC access list tree.
*
* @param    void
*
* @returne  void
*
* @comments Destroys the instance of MAC ACL, releasing the previously
*           allocated database and control space.
*
* @end
*
*********************************************************************/
void aclMacDeleteTree(void);

/*********************************************************************
*
* @purpose  Provide the current value of the MAC ACL index next variable
*
* @param    *next       @b{(output)} access list index next value
*
* @returns  void
*
* @comments Only provides an output value if the next parm is non-null.
*
* @comments Does not guarantee this index value will be valid at the time
*           an ACL create is attempted.
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*       
* @end
*
*********************************************************************/
void aclMacImpIndexNextRead(L7_uint32 *next);

/*********************************************************************
*
* @purpose  Retrieve the min/max index values allowed for the MAC ACL table 
*
* @param    *pMin       @b{(output)} Pointer to index min output location
* @param    *pMax       @b{(output)} Pointer to index max output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*       
* @end
*
*********************************************************************/
L7_RC_t aclMacImpIndexMinMaxGet(L7_uint32 *pMin, L7_uint32 *pMax);

/*********************************************************************
*
* @purpose  Initialize in-use table entry for a MAC ACL
*
* @param    void
*
* @returns  void  
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*       
* @end
*
*********************************************************************/
void aclMacImpIndexInUseInit(void);

/*********************************************************************
*
* @purpose  Update in-use table entry for a MAC ACL
*
* @param    aclIndex    @b{(input)} access list index
* @param    inUse       @b{(input)} mark list in use or not
*
* @returns  void  
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*       
* @end
*
*********************************************************************/
void aclMacImpIndexInUseUpdate(L7_uint32 aclIndex, L7_BOOL inUse);

/*********************************************************************
*
* @purpose  Determine next available MAC acess list index value and 
*           update the 'IndexNext' value
*
* @param    void
*
* @returns  void  
*
* @comments Always looks for first index not already in use, starting with 1.
*
* @comments Assumes caller manages ACL write semaphore, if needed.
*       
* @end
*
*********************************************************************/
void aclMacImpIndexNextUpdate(void);

/*********************************************************************
*
* @purpose  Get the next sequential MAC access list index
*
* @param    aclIndex    @b{(input)}  access list index to begin search
* @param    *next       @b{(output)} next access list index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no MAC access lists exist, or aclIndex is the last
* @returns  L7_FAILURE
*
* @comments This function is also used for a 'getFirst' search by 
*           specifying an aclIndex value of 0.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpIndexGetNext(L7_uint32 aclIndex, L7_uint32 *next);

/*********************************************************************
*
* @purpose  Checks if aclIndex is a valid, configured MAC access list
*
* @param    aclIndex    @b{(input)}  access list index to begin search
*
* @returns  L7_SUCCESS  MAC access list is valid
* @returns  L7_FAILURE  MAC access list not valid
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpIndexCheckValid(L7_uint32 aclIndex);

/*********************************************************************
*
* @purpose  Checks if the MAC ACL index is within proper range
*
* @param    aclIndex    @b{(input)}  access list index to begin search
*
* @returns  L7_SUCCESS  MAC access list index is in range
* @returns  L7_FAILURE  MAC access list index out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpIndexRangeCheck(L7_uint32 aclIndex);

/*********************************************************************
*
* @purpose  Get the first access list rule given a MAC ACL index
*
* @param    aclIndex    @b{(input)}  access list index
* @param    *rule       @b{(output)} first configured rule number
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no rules exist for this ACL
* @returns  L7_FAILURE 
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpRuleGetFirst(L7_uint32 aclIndex, L7_uint32 *rule);

/*********************************************************************
*
* @purpose  Get the next access list rule given a MAC ACL index
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  current rule number
* @param    *next       @b{(output)} next configured rule number
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no more rules exist for this ACL
* @returns  L7_FAILURE 
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpRuleGetNext(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 *next);

/*********************************************************************
*
* @purpose  Checks if the MAC ACL rule number is within proper range
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  current rule number
*
* @returns  L7_SUCCESS  rule in range
* @returns  L7_ERROR    rule out of range
* @returns  L7_FAILURE  access list does not exist, or other failures
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The aclIndex parameter is required by this function in case there 
*           is ever a need to support separate rule number ranges based
*           on the type of ACL (i.e., standard vs. extended).
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpRuleNumRangeCheck(L7_uint32 aclIndex, L7_uint32 rulenum);

/*********************************************************************
*
* @purpose  To find a MAC access list identified by its aclIndex.
*
* @param    aclIndex    @b{(input)}  access list index
*
* @returns  void *      access list element pointer
* @returns  L7_NULLPTR  access list element not found for this aclIndex
*
* @comments
*
* @end
*
*********************************************************************/
void * aclMacFindACL(L7_uint32 aclIndex);

/*********************************************************************
*
* @purpose  Checks if specified MAC access list rule field is configured
*
* @param    *acl_ptr    @b{(input)}  access list element pointer
* @param    rulenum     @b{(input)}  current rule number
* @param    field       @b{(input)}  rule field of interest
*
* @returns  L7_TRUE     rule field is configured
* @returns  L7_FALSE    rule field not configured
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_BOOL aclMacIsRuleFieldConfigured(void *acl_ptr, L7_uint32 rulenum, L7_uint32 field);

/*********************************************************************
*
* @purpose  To create a new MAC access list.
*
* @param    aclIndex    @b{(input)} access list index
*
* @returns  L7_SUCCESS  access list created
* @returns  L7_FAILURE  invalid acl identifier, all other failures
* @returns  L7_ERROR    access list already exists
* @returns  L7_TABLE_IS_FULL  maximum number of ACLs already created
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacCreateACL(L7_uint32 aclIndex);

/*********************************************************************
*
* @purpose  To delete the specified MAC access list
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
*
* @returns  L7_SUCCESS  access list deleted
* @returns  L7_FAILURE  invalid access list index, all other failures
* @returns  L7_ERROR    access list does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacDeleteACL(void *acl_ptr);

/*********************************************************************
*
* @purpose  To add the name to this MAC access list.
*
* @param    *acl_ptr    @b{(input)} access list element pointer
* @param    *name       @b{(input)} access list name
*                                            
* @returns  L7_SUCCESS  name is added
* @returns  L7_FAILURE  invalid access list index, invalid name, or other failure
* @returns  L7_ERROR    access list does not exist
*
* @comments Does not check for name validity (e.g. alphanumeric string).
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpNameAdd(void *acl_ptr, L7_uchar8 *name);

/*********************************************************************
*
* @purpose  To get the name of this MAC access list.
*
* @param    *acl_ptr    @b{(input)}  access list element pointer
* @param    *name       @b{(output)} access list name
*                                            
* @returns  void
*
* @comments Only outputs a value if the acl_ptr and name parms are both non-null.
*
* @comments Assumes caller provides a name buffer of at least 
*           (L7_ACL_NAME_LEN_MAX+1) characters.
*
* @end
*
*********************************************************************/
void aclMacImpNameGet(void *acl_ptr, L7_uchar8 *name);

/*********************************************************************
*
* @purpose  To get the index of a MAC access list, given its name.
*
* @param    *name      @b{(input)} access list name
* @param    *aclIndex  @b{(output)} access list index
*                                            
* @returns  L7_SUCCESS  MAC access list index is retrieved
* @returns  L7_FAILURE  invalid parms, or other failure
* @returns  L7_ERROR    access list name does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpNameToIndexGet(L7_uchar8 *name, L7_uint32 *aclIndex);

/*********************************************************************
*
* @purpose  To get the name of a MAC access list, given its index
*
* @param    *aclIndex   @b{(input)}  access list index
* @param    *name       @b{(output)} access list name
*                                            
* @returns  L7_SUCCESS  name is retrieved
* @returns  L7_FAILURE  invalid parms, or other failure
* @returns  L7_ERROR    access list does not exist
*
* @comments Assumes caller provides a name buffer of at least 
*           (L7_ACL_NAME_LEN_MAX+1) characters.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpNameFromIndexGet(L7_uint32 aclIndex, L7_uchar8 *name);

/*********************************************************************
*
* @purpose  To check if a rule number exists in a MAC access list.
*
* @param    *headPtr    @b{(input)}  pointer to first ACL rule
* @param    rulenum     @b{(input)}  rule number of interest
* @param    **rulePtr   @b{(output)} pointer to found ACL rule
*
* @returns  L7_TRUE     rule number exists
* @returns  L7_FALSE    rule number does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL aclMacRuleNumExist(aclMacRuleParms_t *headPtr, L7_uint32 rulenum, 
                           aclMacRuleParms_t **rulePtr);

/*********************************************************************
*
* @purpose  To add an access option to a rule in an access list.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    action      @b{(input)} access action (permit/deny)
*
* @returns  L7_SUCCESS  access option is added
* @returns  L7_FAILURE  invalid rule number
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacActionAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 action);

/*********************************************************************
*
* @purpose  To remove a rule from an existing access list.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
*                                            
* @returns  L7_SUCCESS  rule is removed
* @returns  L7_FAILURE  invalid access list index or rule number
* @returns  L7_ERROR    access list index or rule number does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRemoveRule(void *acl_ptr, L7_uint32 rulenum);

/*********************************************************************
*
* @purpose  To add the assigned queue id for an access list rule.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    queueId     @b{(input)} assigned queue ID value
*
* @returns  L7_SUCCESS  assigned queue id added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacAssignQueueIdAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 queueId);

/*********************************************************************
*
* @purpose  To add the time range entry for ACL rule.
*
* @param    *acl_ptr         @b{(input)} access list element pointer
* @param    rulenum          @b{(input)} ACL rule number
* @param    *timeRangeEntry  @b{(input)} time range entry
*
* @returns  L7_SUCCESS  if time range entry is added for ACL rule
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacTimeRangeEntryAdd(void *acl_ptr, L7_uint32 rulenum, aclTRCorrEntry_t *timeRangeEntry);

/*********************************************************************
*
* @purpose  To add the redirect interface config id for an access list rule.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    *configId   @b{(input)} redirect interface config id
*
* @returns  L7_SUCCESS  redirect interface config id added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*                                            
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRedirectConfigIdAdd(void *acl_ptr, L7_uint32 rulenum, nimConfigID_t *configId);

/*********************************************************************
*
* @purpose  To add the mirror interface config id for an access list rule.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    *configId   @b{(input)} mirror interface config id
*
* @returns  L7_SUCCESS  mirror interface config id added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*                                            
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacMirrorConfigIdAdd(void *acl_ptr, L7_uint32 rulenum, nimConfigID_t *configId);

/*********************************************************************
*
* @purpose  To set the logging flag for a MAC access list rule.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    logFlag     @b{(input)} rule logging flag value
*
* @returns  L7_SUCCESS  logging flag is added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid flag value or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacLoggingAdd(void *acl_ptr, L7_uint32 rulenum, L7_BOOL logFlag);

/*********************************************************************
*
* @purpose  To add the class of service (cos) value.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    cos         @b{(input)} class of service (cos) value
*
* @returns  L7_SUCCESS  class of service value added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacCosAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 cos);

/*********************************************************************
*
* @purpose  To add the secondary class of service (cos2) value.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    cos         @b{(input)} secondary class of service (cos2) value
*
* @returns  L7_SUCCESS  secondary class of service value added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacCos2Add(void *acl_ptr, L7_uint32 rulenum, L7_uint32 cos2);

/*********************************************************************
*
* @purpose  To add the destination MAC address and mask.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    dstmac      @b{(input)} destination MAC address
* @param    dstmask     @b{(input)} destination MAC mask
*
* @returns  L7_SUCCESS  destination MAC added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacDstMacAdd(void *acl_ptr, L7_uint32 rulenum,
                        L7_uchar8 *dstmac, L7_uchar8 *dstmask);

/*********************************************************************
*
* @purpose  To add the Ethertype keyword identifier.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    keyid       @b{(input)} Ethertype keyword identifier
* @param    value       @b{(input)} Ethertype custom value
*
* @returns  L7_SUCCESS  Ethertype keyword identifier added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
*
* @comments The value parameter is only meaningful when keyid is set to 
*           L7_QOS_ETYPE_KEYID_CUSTOM.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacEtypeKeyAdd(void *acl_ptr, L7_uint32 rulenum, 
                          L7_QOS_ETYPE_KEYID_t keyid, L7_uint32 value);

/*********************************************************************
*
* @purpose  To add the 'match every' match condition.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    every       @b{(input)} 'match every' flag value (L7_TRUE/L7_FALSE)
*
* @returns  L7_SUCCESS  'match every' condition added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacEveryAdd(void *acl_ptr, L7_uint32 rulenum, L7_BOOL every);

/*********************************************************************
*
* @purpose  To add the source MAC address and mask.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    srcmac      @b{(input)} source MAC address
* @param    srcmask     @b{(input)} source MAC mask
*
* @returns  L7_SUCCESS  source MAC added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacSrcMacAdd(void *acl_ptr, L7_uint32 rulenum,
                        L7_uchar8 *srcmac, L7_uchar8 *srcmask);

/*********************************************************************
*
* @purpose  To add a single VLAN ID.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    vlan        @b{(input)} VLAN ID
*
* @returns  L7_SUCCESS  VLAN ID added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacVlanIdAdd(void *acl_ptr, L7_uint32 rulenum, L7_uint32 vlan);

/*********************************************************************
*
* @purpose  To add a VLAN ID range.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    startvlan   @b{(input)} starting VLAN ID
* @param    endvlan     @b{(input)} ending VLAN ID
*
* @returns  L7_SUCCESS  VLAN ID range added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacVlanIdRangeAdd(void *acl_ptr, L7_uint32 rulenum, 
                             L7_uint32 startvlan, L7_uint32 endvlan);

/*********************************************************************
*
* @purpose  To add a single Secondary VLAN ID.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    vlan2       @b{(input)} Secondary VLAN ID
*
* @returns  L7_SUCCESS  Secondary VLAN ID added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacVlanId2Add(void *acl_ptr, L7_uint32 rulenum, L7_uint32 vlan2);

/*********************************************************************
*
* @purpose  To add a Secondary VLAN ID range.
*
* @param    *acl_ptr    @b{(input)} access list element pointer   
* @param    rulenum     @b{(input)} ACL rule number
* @param    startvlan2  @b{(input)} starting Secondary VLAN ID
* @param    endvlan2    @b{(input)} ending Secondary VLAN ID
*
* @returns  L7_SUCCESS  Secondary VLAN ID range added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacVlanId2RangeAdd(void *acl_ptr, L7_uint32 rulenum, 
                              L7_uint32 startvlan2, L7_uint32 endvlan2);

/*********************************************************************
*
* @purpose  Get a aclStructure_t where the head is rule rulenum
*
* @param    aclIndex    @b{(input)}  access list index to begin search
* @param    rulenum     @b{(input)}  current rule number
* @param    *acl_rp     @b{(output)} rule parameter info pointer
*
* @returns  L7_SUCCESS  rulenum was found
* @returns  L7_ERROR    aclnum or rulenum does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleGet(L7_uint32 aclIndex, L7_uint32 rulenum, aclMacRuleParms_t **acl_rp);

/*********************************************************************
*
* @purpose  Sends a recently updated MAC ACL to the driver
*
* @param    aclIndex    @b{(input)} access list index
* @param    op          @b{(input)} operation (ACL_MODIFY)
*
* @returns  L7_SUCCESS  all interfaces updated
* @returns  L7_ERROR    TLV operation failed
* @returns  L7_FAILURE  other failure 
*
* @comments Will build a TLV and send the MAC ACL to the driver
*
* @end
*
*********************************************************************/
L7_RC_t aclMacUpdate(L7_uint32 aclIndex, L7_uint32 op);

/*********************************************************************
*
* @purpose  Sends a recently updated MAC ACL to the driver, except for
*           the specified interface
*
* @param    aclIndex    @b{(input)} access list index
* @param    op          @b{(input)} operation (ACL_MODIFY)
* @param    intIfNum    @b{(input)} internal interface number, or 0
*
* @returns  L7_SUCCESS  all interfaces updated
* @returns  L7_ERROR    TLV operation failed
* @returns  L7_FAILURE  other failure 
*
* @comments Will build a TLV and send the MAC ACL to the driver for each
*           interface to which it is attached EXCEPT for the specified
*           intIfNum.  If the intIfNum is 0, all of the ACL's interfaces
*           are updated.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacUpdateExceptIntf(L7_uint32 aclIndex, L7_uint32 op, L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Builds a set of MAC ACL Rule TLVs
*
* @param    aclIndex            @b{(input)}  access list index
* @param    *ruleCount          @b{(output)} number of rules written to TLV
* @param    *matchEveryFlags    @b{(output)} set if a 'match every' rule used
* @param    *pDirInfo           @b{(output)} Ptr to intf,dir information
* @param    vlan                @b{(input)}  VLAN ID the ACL is going to be applied to
* @param    tlvHandle           @b{(input)}  TLV block handle to use
*
* @returns  L7_SUCCESS  this ACL successfully processed
* @returns  L7_ERROR    problem with TLV creation
* @returns  L7_FAILURE  any other failure
*
* @comments Does not append an implicit 'deny all' rule to the TLV.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacTlvRuleDefBuild(L7_uint32 aclIndex, L7_uint32 *ruleCount,
                              aclTlvMatchEvery_t *matchEveryFlags,
                              aclIntfDirInfo_t *pDirInfo,
                              L7_uint32 vlan, L7_tlvHandle_t tlvHandle,
                              L7_uint8 *actionFlag);

/*********************************************************************
*
* @purpose  Builds a set of MAC ACL VLAN Rule TLVs
*
* @param    vlanNum             @b((input))  VLAN id
* @param    aclIndex            @b{(input)}  access list index
* @param    *ruleCount          @b{(output)} number of rules written to TLV
* @param    *matchEveryFlags    @b{(output)} set if a 'match every' rule used
*
* @returns  L7_SUCCESS  this ACL successfully processed
* @returns  L7_ERROR    problem with TLV creation
* @returns  L7_FAILURE  any other failure
*
* @comments Does not append an implicit 'deny all' rule to the TLV.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacTlvVlanRuleDefBuild( L7_uint32 vlanNum,
                                   L7_uint32 aclIndex,
                                   L7_uint32 *ruleCount,
                                   aclTlvMatchEvery_t *matchEveryFlags );
  
/*********************************************************************
*
* @purpose  Checks if the MAC ACL rule is effectively non-empty after
*           factoring in a mask value, if any
*
* @param    *aclIndex   @b{(input)} access list index   
* @param    rulenum     @b{(input)} ACL rule number
* @param    vlan        @b{(input)} VLAN ID to which the ACL is
*                                   to be applied, if any 
*                                   (vlan == 0 if TLV is being built 
*                                   for an interface)
*
* @returns  L7_TRUE     TLV rule non-empty
* @returns  L7_FALSE    TLV rule is empty (i.e., masked value is 0)
*
* @comments The reason for this check is to prevent maskable fields
*           whose mask value is 0 from appearing in the TLV issued to the
*           device.  It also examines the rule set when the ACL is
*           being applied to a VLAN ID to see if that results in a "match every"
*           result.
*           
*
* @end
*
*********************************************************************/
L7_BOOL aclMacIsTlvMaskedRuleValid(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 vlan);

/*********************************************************************
*
* @purpose  Translate the Ethertype keyid into a value  
*
* @param    keyId       @b{(input)}  Ethernet keyword identifier
* @param    custVal     @b{(input)}  Ethernet custom value
* @param    *value1     @b{(output)} first Ethertype value
* @param    *value2     @b{(output)} second Ethertype value (or 0 if unused)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Some Ethertype keywords translate into two Ethertype values. 
*           In this case, *value2 is output as a nonzero value.           
*
* @end
*
*********************************************************************/
L7_RC_t aclMacEtypeKeyIdToValue(L7_uint32 keyId, L7_uint32 custVal,
                                L7_uint32 *value1, L7_uint32 *value2);

/*********************************************************************
*
* @purpose  Translate the Ethertype value into a keyid
*
* @param    value       @b{(input)}  Ethernet value
* @param    *keyId      @b{(output)} Ethertype keyword identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An Ethertype value that is one of several values covered 
*           by a single keyid outputs that keyid.           
*
* @notes    Any value that does not match a specific keyid is output as   
*           the 'custom' keyid.           
*
* @end
*
*********************************************************************/
L7_RC_t aclMacEtypeValueToKeyId(L7_uint32 value, L7_uint32 *keyId);

/*********************************************************************
*
* @purpose  Check if Ethertype field is set to match a specific value  
*
* @param    *p          @b{(input)}  ACL MAC rule parms ptr
* @param    etypeVal    @b{(input)}  Ethertype value of interest
*
* @returns  L7_TRUE     rule contains desired Ethertype match field
* @returns  L7_FALSE    reule does not contain desired Ethertype match field
*
* @notes    Considers a match on either Ethertype keyword or custom value.
*
* @end
*
*********************************************************************/
L7_BOOL aclMacImpEtypeIsMatching(aclMacRuleParms_t *p, L7_uint32 etypeVal);

/*********************************************************************
*
* @purpose  Apply collateral config related to this interface 
*
* @param    intIfNum    @b{(input)}  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is typically used to update the device when config
*           such as for redirect and mirror interfaces come and go.
*
* @comments The intIfNum parm represents the potential target interface
*           for redirect or mirroring, not the current interface(s) to 
*           which an ACL containing those attributes is applied.
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpCollateralConfigApply(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Checks if specified MAC access list rule field is configured
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  current rule number
* @param    field       @b{(input)}  rule field of interest
*
* @returns  L7_TRUE     rule field is configured
* @returns  L7_FALSE    rule field not configured
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_BOOL aclMacImpIsFieldConfigured(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 field);

/*********************************************************************
*
* @purpose  Gets a list of interfaces to which a MAC ACL is assigned
*           in the specified direction
*
* @param    aclIndex    @b{(input)} access list index
* @param    aclDir      @b{(input)} interface direction (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    *intfList   @b{(output)} list of interfaces (L7_ACL_INTF_MAX_COUNT long)
*
* @returns  L7_SUCCESS  list of interfaces was built
* @returns  L7_FAILURE  invalid ACL index, or no interfaces exist
* @returns  L7_ERROR    access list is not assigned to any interface
*                       in this direction (no interface list was built)
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments An ACL can be shared/associated with multiple interfaces.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpAssignedIntfDirListGet(L7_uint32 aclIndex, L7_uint32 aclDir,
                                        L7_ACL_ASSIGNED_INTF_LIST_t *intfList);

/*********************************************************************
*
* @purpose  To add an access list to the specified interface and direction.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    aclDir      @b{(input)} interface direction to apply ACL (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    aclIndex    @b{(input)} access list index
* @param    seqNum      @b{(input)} ACL evaluation order sequence number
*
* @returns  L7_SUCCESS  ACL successfully added to intf, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
* @returns  L7_ERROR    ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED  intf, dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED  feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL  ACL assignment at capacity for intf, dir
* @returns  L7_REQUEST_DENIED Error applying ACL to hardware
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpInterfaceDirectionAdd(L7_uint32 intIfNum, L7_uint32 aclDir, 
                                       L7_uint32 aclIndex, L7_uint32 seqNum);

/*********************************************************************
*
* @purpose  To remove a MAC access list from the specified interface and direction.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    aclDir      @b{(input)} interface direction to apply ACL (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    aclIndex    @b{(input)} access list index
*
* @returns  L7_SUCCESS  intIfNum is removed from the access-list
* @returns  L7_FAILURE  invalid ACL index, or interface does not exist
* @returns  L7_ERROR    ACL does not exist for this index, or not assigned
*                         to the interface
* @returns  L7_REQUEST_DENIED error removing ACL from hardware
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpInterfaceDirectionRemove(L7_uint32 intIfNum, L7_uint32 aclDir,
                                          L7_uint32 aclIndex);

/*********************************************************************
*
* @purpose  Gets a list of VLANs to which a MAC ACL is assigned
*           in the specified direction
*
* @param    aclIndex    @b{(input)} access list index
* @param    aclDir      @b{(input)} VLAN direction (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    *intfList   @b{(output)} list of VLANs (L7_MAX_VLANS long)
*
* @returns  L7_SUCCESS  list of VLANs was built
* @returns  L7_FAILURE  invalid ACL index, or no VLANs exist
* @returns  L7_ERROR    access list is not assigned to any VLAN
*                       in this direction (no VLAN list was built)
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments An ACL can be shared/associated with multiple VLANs.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpAssignedVlanDirListGet( L7_uint32 aclIndex,
                                         L7_uint32 aclDir,
                                         L7_ACL_ASSIGNED_VLAN_LIST_t *vlanList );

/*********************************************************************
*
* @purpose  To add an access list to the specified VLAN and direction.
*
* @param    intIfNum    @b{(input)} internal VLAN number
* @param    aclDir      @b{(input)} VLAN direction to apply ACL (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    aclIndex    @b{(input)} access list index
* @param    seqNum      @b{(input)} ACL evaluation order sequence number
*
* @returns  L7_SUCCESS  ACL successfully added to VLAN, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
* @returns  L7_ERROR    ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED  VLAN, dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED  feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL  ACL assignment at capacity for VLAN, dir
* @returns  L7_REQUEST_DENIED Error applying ACL to hardware
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpVlanDirectionAdd( L7_uint32 vlanNum,
                                   L7_uint32 aclDir,
                                   L7_uint32 aclIndex,
                                   L7_uint32 seqNum );

/*********************************************************************
*
* @purpose  To remove a MAC access list from the specified VLAN and direction.
*
* @param    intIfNum    @b{(input)} internal VLAN number
* @param    aclDir      @b{(input)} VLAN direction to apply ACL (internal)
*                                   (ACL_INTF_DIR_IN or ACL_INTF_DIR_OUT)
* @param    aclIndex    @b{(input)} access list index
*
* @returns  L7_SUCCESS  vlanNum is removed from the access-list
* @returns  L7_FAILURE  invalid ACL index, or VLAN does not exist
* @returns  L7_ERROR    ACL does not exist for this index, or not assigned
*                         to the VLAN
* @returns  L7_REQUEST_DENIED error removing ACL from hardware
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpVlanDirectionRemove( L7_uint32 vlanNum,
                                      L7_uint32 aclDir,
                                      L7_uint32 aclIndex );

/*************************************************************************
* @purpose  Get the current number of MAC ACLs configured
*
* @param    pCurr  @b{(output)} Pointer to the current value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*********************************************************************/
L7_RC_t aclMacImpCurrNumGet(L7_uint32 *pCurr);

/*************************************************************************
* @purpose  Check if MAC ACL table is full
*
* @param    void
*
* @returns  L7_TRUE     MAC ACL table is full
* @returns  L7_FALSE    MAC ACL table not full
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*********************************************************************/
L7_BOOL aclMacImpIsTableFull(void);

/*********************************************************************
*
* @purpose  Outputs the MAC ACL type if it belongs to one of the supported 
*           index ranges
*
* @param    aclIndex    @b{(input)}  access list index to begin search
* @param    *acltype    @b{(output)} access list type
*
* @returns  L7_SUCCESS  access list index belongs to a supported MAC range
* @returns  L7_FAILURE  access list index is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpAclTypeGet(L7_uint32 aclIndex, L7_ACL_TYPE_t *acltype);

/*********************************************************************
*
* @purpose  Gets the current number of configured rules in a MAC ACL
*
* @param    aclIndex    @b{(input)}  access list index
* @param    *ruleCount  @b{(output)} number of rules in ACL
*
* @returns  L7_SUCCESS  ACL rule count retrieved
* @returns  L7_ERROR    access list does not exist
* @returns  L7_FAILURE  access list index is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The output ruleCount does not include the implicit 'deny all'
*           rule contained in every ACL definition.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacImpAclRuleCountGet(L7_uint32 aclIndex, L7_uint32 *ruleCount);

/*********************************************************************
* @purpose  Display the current MAC ACL Table contents
*
* @param    showRules   @b{(input)} Indicates if ACL rules are displayed
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void aclMacShow(L7_BOOL showRules);

/*********************************************************************
* @purpose  Display content of all rules for a specific MAC ACL
*
* @param    aclIndex    @b{(input)} MAC access list index
*
* @returns  void
*
* @comments Assumes caller handles any necessary semaphore protection.
*
* @end
*********************************************************************/
void aclMacRulesShow(L7_uint32 aclIndex);

/*********************************************************************
* @purpose  Display contents of a MAC ACL rule
*
* @param    *pRule      @b{(input)} ACL rule pointer
* @param    msgLvlReqd  @b{(input)} Desired output message level
*
* @returns  void
*
* @comments Assumes caller handles any necessary semaphore protection.
*
* @end
*********************************************************************/
void aclMacRuleDisplay(aclMacRuleParms_t *pRule, L7_uint32 msgLvlReqd);

/*********************************************************************
* @purpose  Display a MAC address or mask               
*
* @param    *p          @b{(input)} MAC address or mask pointer
* @param    msgLvlReqd  @b{(input)} Debug print message level
*
* @returns  void
*
* @comments Internal helper function.
*
* @end
*********************************************************************/
void aclMacAddrPrint(L7_uchar8 *p, L7_uint32 msgLvlReqd);

/****************************************************************************
* @purpose  call back routine that post the time range events to aclEventQueue
* @param    *name        @b{(input)} timeRangeName name of time range on                     
*                                    which there is a event
*                                    
* @param    *event       @b{(input)} time range events  as listed in
*                                    timeRangeEvent_t
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t aclTimeRangeEventCallback(L7_uchar8 *timeRangeName, timeRangeEvent_t  event);

/*********************************************************************
*
* @purpose  ACL Event Task
*
* @param    void
*
* @returns  void
*
* @comments Handles notifications form time range component and reinstalls a rule
* as activated/deactivated. Generic enough to be extended for future enhancements
*
* @comments
*
* @end
*
*********************************************************************/
void aclEventTask(void);

/*********************************************************************
*
* @purpose Initializes the Time Range Correlator Entry List to defaults
*
* @param   none
*
* @returns none
*
* @notes
*
* @end
*********************************************************************/
void aclTRCorrEntryListInitialize();

/*********************************************************************
*
* @purpose Gets a free correlaotr entry from Free List.
*
* @param   aclTRCorrEntryPtr    @b{ (output) } correlator pointer.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t aclTRCorrEntryGet(aclTRCorrEntry_t **aclTRCorrEntryPtr);

/*********************************************************************
*
* @purpose Sets back a used correlaotr entry to Free List.
*
* @param   aclTRCorrEntryPtr    @b{ (output) } correlator pointer.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t aclTRCorrEntryFree(aclTRCorrEntry_t *aclTRCorrEntry);

/*********************************************************************
*
* @purpose Add the correaltor for trId List
*
* @param   TRId         @b{ (input) } Time Range Id
* @param   corrEntry    @b{ (input) } node to be added
*
* @returns
*
* @notes   Adds new node to top of list
*
* @end
*********************************************************************/
void aclTRCorrEntryListAdd(L7_uint32 TRId, aclTRCorrEntry_t *corrEntry);

/*********************************************************************
*
* @purpose Delete the correaltor node from trId list
*
* @param   TRId         @b{ (input) } Time Range Id
* @param   corrEntry    @b{ (input) } node to be added
*
* @returns
*
* @notes   Delete the node from list
*
* @end
*********************************************************************/
void aclTRCorrEntryListDelete(L7_uint32 TRId,aclTRCorrEntry_t *corrEntry);

/*********************************************************************
*
* @purpose Initializes the Time Range Table to defaults
*
* @param   none
*
* @returns none
*
* @notes
*
* @end
*********************************************************************/
void aclTimeRangeTableInitialize();

/*********************************************************************
*
* @purpose Updates or creates Time Range entry for a given
*          Time Range Id.
*
* @param   timeRangeName @b{ (input) } Time Range Name
* @param   TRStatus      @b{ (input) } Time Range Status from Timezone component.
* @param   TRId          @b{ (output) } Time Range Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Creates or updates the existing entry in aclTimeRangeTable:
*
* @end
*********************************************************************/
L7_RC_t aclTimeRangeTableEntryUpdate(L7_uchar8 *timeRangeName,L7_uint32 status, L7_uint32 *TRId);

/*********************************************************************
*
* @purpose Get the status of Time Range Entry
*
* @param   TRId       @b{ (input) } Time Range Id
* @param   *status    @b{ (output) } status of the time range
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Get the time range status for associated with TrId entry in table
*
* @end
*********************************************************************/
L7_RC_t aclTimeRangeTableEntryStatusGet(L7_uint32 TRId, L7_uint32 *status);

/*********************************************************************
*
* @purpose Get the name of Time Range Entry
*
* @param   TRId              @b{ (input) } Time Range Id
* @param   *timeRangeName    @b{ (output) } Time Range Name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Get the time range name for associated with TrId entry in table
*
* @end
*********************************************************************/
L7_RC_t aclTimeRangeTableEntryTRNameGet(L7_uint32 TRId, L7_uchar8 *timeRangeName);

/*********************************************************************
*
* @purpose Updates or Delete Time Range information for a given
*          Time Range Id.
*
* @param   TRId       @b{ (input) } Time Range Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Reset the entry to defaults if refCount is zero
*          otherwise decrement the refCount
*
* @end
*********************************************************************/
L7_RC_t aclTimeRangeTableEntryDelete(L7_uint32 TRId);

/*********************************************************************
*
* @purpose Create a new Correlator node
*
* @param   TRId              @b{ (input) } Time Range Id
* @param   corrId            @b{ (input) } Correlator Id
* @param   aclTRCorrEntryPtr @b{ (output) } New node created
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  Create a  new correlator note and initialize to defaults
*
* @end
*********************************************************************/
L7_RC_t aclTimeTangeTableCorrEntryCreate(L7_uint32 TRId,L7_uint32 corrId, aclTRCorrEntry_t **aclTRCorrEntryPtr);

/*********************************************************************
*
* @purpose Update or Add the correaltor for Time range
*
* @param   TRId         @b{ (input) } Time Range Id
* @param   corrEntry    @b{ (input) } node to be added
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  if node is already added to the list increments
*         the refCount maintained in correlator node
*         Adds new node to top of list
*
* @end
*********************************************************************/
L7_RC_t aclTimeRangeTableCorrEntryAdd(L7_uint32 TRId, aclTRCorrEntry_t *corrEntry);

/*********************************************************************
*
* @purpose Delete the correlator node from trId list
*
* @param   TRId         @b{ (input) } Time Range Id
* @param   corrEntry    @b{ (input) } node to be added
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Decrement the refCount maintained in node, if refCount is
*          zero initialize the correlator to invalid value and remove from
*          the list
*
* @end
*********************************************************************/
L7_RC_t aclTimeRangeTableCorrEntryDelete(L7_uint32 TRId, aclTRCorrEntry_t *corrEntry);

/*********************************************************************
*
* @purpose Detach and attach correlator node new trId
*
* @param   prevTRId     @b{ (input) } Previous Time Range Id
* @param   currTRId     @b{ (input) } Current Time Range Id
* @param   corrEntry    @b{ (input) } Correlator node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  Detach node from list of prevTRId and attach to currTRId
*
*
* @end
*********************************************************************/
L7_RC_t aclTimeRangeTableCorrEntryRelocate(L7_uint32 prevTRId, L7_uint32 currTRId, aclTRCorrEntry_t *corrEntry);

/*********************************************************************
*
* @purpose  Displays contents of ACL aclTimeRangeNameTable table
*
* @param
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void aclTimeRangeTableShow();

#endif  /* ACL_H */

