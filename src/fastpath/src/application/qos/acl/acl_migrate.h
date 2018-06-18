/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename acl_migrate.h
*
* @purpose Access Control Lists' Configuration Migration
*
* @component Access Control List
*
* @comments none
*
* @create 08/24/2004
*
* @author Rama Sasthri, Kristipati
*
* @end
*
**********************************************************************/

#ifndef ACL_MIGRATE_H
#define ACL_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "l3_addrdefs.h"
#include "acl.h"

/* QSCAN: ADD -- Add _REL_5_0 definitions and use in V6 cfg arrays */

#define L7_ACL_INTF_MAX_COUNT_REL_4_1 (L7_MAX_PORT_COUNT_REL_4_1 +  L7_MAX_NUM_VLAN_INTF_REL_4_1 + 1)
#define L7_ACL_INTF_MAX_COUNT_REL_4_2 (L7_MAX_PORT_COUNT_REL_4_2 +  L7_MAX_NUM_VLAN_INTF_REL_4_2 + 1)
#define L7_ACL_INTF_MAX_COUNT_REL_4_3 (L7_MAX_PORT_COUNT_REL_4_3 +  L7_MAX_NUM_LAG_INTF_REL_4_3 + L7_MAX_NUM_VLAN_INTF_REL_4_3 + 1)
#define L7_ACL_INTF_MAX_COUNT_REL_4_4 (L7_MAX_PORT_COUNT_REL_4_4 +  L7_MAX_NUM_LAG_INTF_REL_4_4 + L7_MAX_NUM_VLAN_INTF_REL_4_4 + 1)
#define L7_ACL_INTF_MAX_COUNT_REL_5_0 (L7_MAX_PORT_COUNT_REL_5_0 +  L7_MAX_NUM_LAG_INTF_REL_5_0 + L7_MAX_NUM_VLAN_INTF_REL_5_0 + 1)

#define L7_ACL_MAX_LISTS_PER_VLAN_DIR_REL_5_0     L7_ACL_MAX_RULES_PER_MULTILIST_REL_5_0
#define L7_ACL_VLAN_MAX_COUNT_REL_5_0 0

#if 1 
/* For release 4.0.0.2 and before, these values of inbound and outbound ACL are 1 and 2, respectively.
For later 4.0.0 point releases, these values may be 0 or 1.  As these later releases are in limited
distribution, the values chosen to migrate are those equivalent to the 4.0.0.2 release. */
#define L7_INBOUND_ACL_REL_4_0  1
#define L7_OUTBOUND_ACL_REL_4_0 2

#else

#define L7_INBOUND_ACL_REL_4_0  0
#define L7_OUTBOUND_ACL_REL_4_0 1

#endif

#define L7_INBOUND_ACL_REL_4_1  1
#define L7_OUTBOUND_ACL_REL_4_1 2

#define L7_INBOUND_ACL_REL_4_2  0
#define L7_OUTBOUND_ACL_REL_4_2 1

#define L7_INBOUND_ACL_REL_4_3  0
#define L7_OUTBOUND_ACL_REL_4_3 1


typedef enum
{
  ACL_RULENUM_V3 = 1,
  ACL_ACTION_V3,
  ACL_EVERY_V3,
  ACL_PROTOCOL_V3,
  ACL_PROTOCOL_MASK_V3,
  ACL_SRCIP_V3,
  ACL_SRCIP_MASK_V3,
  ACL_SRCPORT_V3,
  ACL_SRCSTARTPORT_V3,
  ACL_SRCENDPORT_V3,
  ACL_DSTIP_V3,
  ACL_DSTIP_MASK_V3,
  ACL_DSTPORT_V3,
  ACL_DSTSTARTPORT_V3,
  ACL_DSTENDPORT_V3,
  ACL_TOSBYTE_V3,
  ACL_TOSMASK_V3,
  ACL_IPPREC_V3,
  ACL_IPTOS_V3,
  ACL_IPDSCP_V3
} ruleFieldsV3_t;

typedef struct aclRuleParmsV1_s
{
  L7_uint32 configMask;

  L7_uint32 ruleNum;            /* access list rule num               */
  L7_uint32 action;             /* action: permit/deny                */
  L7_BOOL every;

  L7_uchar8 protocol;           /* protocol number                    */
  L7_uchar8 protmask;           /* protocol mask value                */

  L7_uint32 srcIp;              /* source ip                          */
  L7_uint32 srcMask;            /* source mask                        */
  L7_ushort16 srcPort;          /* source port: keyword               */
  L7_ushort16 srcStartPort;     /* source port range: start port      */
  L7_ushort16 srcEndPort;       /* source port range: end port        */

  L7_uint32 dstIp;              /* destination ip                     */
  L7_uint32 dstMask;            /* destination mask                   */
  L7_ushort16 dstPort;          /* destination port: keyword          */
  L7_ushort16 dstStartPort;     /* destination port range: start port */
  L7_ushort16 dstEndPort;       /* destination port range: end port   */

  L7_uchar8 tosbyte;            /* 8 bit tos byte                     */
  L7_uchar8 tosmask;            /* tos mask                           */

  L7_BOOL last;                 /* check for last rule in an acl      */
  struct aclRuleParmsV1_s *next;        /* used by AVL - pointer to next rule */
} aclRuleParmsV1_t;

typedef struct
{
  L7_uint32 aclNum;
  aclRuleParmsV1_t ruleData[L7_ACL_MAX_RULE_NUM_REL_4_0 + 1];
} aclCfgDataV1_t;

typedef struct
{
  /* NOTE: aclData is indexed by list entry number, not directly by ACL ID.
   *       Entry 0 of this array is reserved, but unused (mainly for backwards
   *       compatibility with config files from earlier releases).
   */
  aclCfgDataV1_t aclData[L7_ACL_MAX_LISTS + 1];
  L7_uint32 intf_dir_Tb[L7_MAX_INTERFACE_COUNT_REL_4_0 + 1][ACL_INTF_DIR_MAX_REL_4_0 + 1];
  /* table to indicate the access-list application interface and direction */
  /* interfaces and directions are referenced as 1-based */
}
L7_aclCfgParmsV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_aclCfgParmsV1_t cfgParms;
  L7_uint32 checkSum;
}
aclCfgFileDataV1_t;

typedef struct
{
  L7_uint32 aclNum;
  aclRuleParmsV1_t ruleData[L7_ACL_MAX_RULE_NUM_REL_4_1 + 1];
} aclCfgDataV2_t;

typedef struct
{
  nimConfigID_t configId;       /* must be first in the struct */
  L7_uint32 intf_dir_Tb[ACL_MAX_DIRECTION_NUM_REL_4_1 + 1];
} aclIntfCfgDataV2_t;

typedef struct
{
  aclCfgDataV2_t aclData[L7_ACL_MAX_LISTS + 1];
  aclIntfCfgDataV2_t aclIntfCfgData[L7_ACL_INTF_MAX_COUNT_REL_4_1];
}
L7_aclCfgParmsV2_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_aclCfgParmsV2_t cfgParms;
  L7_uint32 checkSum;
}
aclCfgFileDataV2_t;

typedef struct
{
  nimConfigID_t configId;       /* must be first in the struct */
  L7_uint32 intf_dir_Tb[ACL_MAX_DIRECTION_NUM_REL_4_2];
} aclIntfCfgDataV3_t;

typedef struct
{
  aclCfgDataV2_t aclData[L7_ACL_MAX_LISTS + 1];
  aclIntfCfgDataV3_t aclIntfCfgData[L7_ACL_INTF_MAX_COUNT_REL_4_2];
}
L7_aclCfgParmsV3_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_aclCfgParmsV3_t cfgParms;
  L7_uint32 checkSum;
}
aclCfgFileDataV3_t;


/***** Start V4 definitions (Rel 4.3.x) *****/

typedef enum
{
  ACL_RULENUM_V4 = 1,
  ACL_ACTION_V4,

  ACL_ASSIGN_QUEUEID_V4,
  ACL_REDIRECT_INTF_V4,

  ACL_EVERY_V4,
  ACL_PROTOCOL_V4,
  ACL_PROTOCOL_MASK_V4,
  ACL_SRCIP_V4,
  ACL_SRCIP_MASK_V4,
  ACL_SRCPORT_V4,
  ACL_SRCSTARTPORT_V4,
  ACL_SRCENDPORT_V4,
  ACL_DSTIP_V4,
  ACL_DSTIP_MASK_V4,
  ACL_DSTPORT_V4,
  ACL_DSTSTARTPORT_V4,
  ACL_DSTENDPORT_V4,
  ACL_TOSMASK_V4,
  ACL_IPPREC_V4,
  ACL_IPTOS_V4,
  ACL_IPDSCP_V4,

  ACL_RULEFIELDS_TOTAL_V4               /* total number of IP ACL rule fields */
  
} ruleFieldsV4_t;

typedef enum
{
  ACL_MAC_RULENUM_V4 = 1,
  ACL_MAC_ACTION_V4,

  ACL_MAC_EVERY_V4,
  ACL_MAC_ASSIGN_QUEUEID_V4,
  ACL_MAC_REDIRECT_INTF_V4,

  ACL_MAC_COS_V4,
  ACL_MAC_COS2_V4,
  ACL_MAC_DSTMAC_V4,
  ACL_MAC_DSTMAC_MASK_V4,
  ACL_MAC_ETYPE_KEYID_V4,
  ACL_MAC_ETYPE_VALUE_V4,
  ACL_MAC_SRCMAC_V4,
  ACL_MAC_SRCMAC_MASK_V4,
  ACL_MAC_VLANID_V4,
  ACL_MAC_VLANID_START_V4,
  ACL_MAC_VLANID_END_V4,
  ACL_MAC_VLANID2_V4,
  ACL_MAC_VLANID2_START_V4,
  ACL_MAC_VLANID2_END_V4,

  ACL_MAC_RULEFIELDS_TOTAL_V4           /* total number of MAC ACL rule fields */

} macRuleFieldsV4_t;

/* L3 IP ACL classifier rule definition */
typedef struct aclRuleParmsV4_s
{
  L7_uint32       configMask;

  L7_uint32       ruleNum;                       /* access list rule num               */
  L7_uint32       action;                        /* action: permit/deny                */
  L7_BOOL         every;                         /* let every IP packet match this rule*/
  L7_uint32       assignQueueId;                 /* assigned queue id                  */
  nimConfigID_t   redirectConfigId;              /* redirect intf config id            */

  L7_uchar8       protocol;                      /* protocol number                    */
  L7_uchar8       protmask;                      /* protocol mask value                */

  L7_uint32       srcIp;                         /* source ip                          */
  L7_uint32       srcMask;                       /* source mask                        */
  L7_ushort16     srcPort;                       /* source port: keyword               */
  L7_ushort16     srcStartPort;                  /* source port range: start port      */
  L7_ushort16     srcEndPort;                    /* source port range: end port        */

  L7_uint32       dstIp;                         /* destination ip                     */
  L7_uint32       dstMask;                       /* destination mask                   */
  L7_ushort16     dstPort;                       /* destination port: keyword          */
  L7_ushort16     dstStartPort;                  /* destination port range: start port */
  L7_ushort16     dstEndPort;                    /* destination port range: end port   */

  L7_uchar8       tosbyte;                       /* 8 bit tos byte                     */
  L7_uchar8       tosmask;                       /* tos mask                           */

  L7_BOOL         last;                          /* check for last rule in an acl      */
  struct aclRuleParmsV4_s   *next;               /* used by AVL - pointer to next rule */
} aclRuleParmsV4_t;

/* L2 MAC ACL classifier rule definition */
typedef struct aclMacRuleParmsV4_s
{
  L7_uint32       configMask;

  L7_uint32       ruleNum;                       /* access list rule num               */
  L7_uint32       action;                        /* action: permit/deny                */
  L7_BOOL         every;                         /* let every MAC packet match this rule*/
  L7_uint32       assignQueueId;                 /* assigned queue id                  */
  nimConfigID_t   redirectConfigId;              /* redirect intf config id            */

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

  struct aclMacRuleParms_V4s   *next;            /* used by AVL - pointer to next rule */
} aclMacRuleParmsV4_t;

typedef union aclCfgRuleParmsV4_s
{
  aclRuleParmsV4_t          ip;                   /* IP ACL rules                      */
  aclMacRuleParmsV4_t       mac;                  /* MAC ACL rules                     */

} aclCfgRuleParmsV4_t;

typedef struct aclIdV4_s
{
  L7_ACL_TYPE_t     aclType;
  L7_uint32         aclId;                            /* IP: aclNum; MAC: aclIndex     */

} aclIdV4_t;

typedef struct
{
  aclIdV4_t           id;                                     /* ACL identification info       */
  L7_uchar8           aclName[L7_ACL_NAME_LEN_MAX+1];         /* ACL name (as needed)          */
  aclCfgRuleParmsV4_t ruleData[L7_ACL_MAX_RULE_NUM_REL_4_3+1];/* content depends on id.aclType */

} aclCfgDataV4_t;

typedef struct
{
  L7_BOOL           inUse;                            /* denotes entry is in use    */
  L7_uint32         seqNum;                           /* list order sequence number */
  aclIdV4_t         id;                               /* ACL identification info    */

} aclIntfParmsV4_t;

typedef struct
{
  aclIntfParmsV4_t    assignList[L7_ACL_MAX_LISTS_PER_INTF_DIR_REL_4_3];

} aclIntfListV4_t;

/* Struct to hold information about the ACLs configured for 
 * a given interface in each direction
 */
typedef struct
{
  nimConfigID_t     configId; /* must be first in the struct */
  aclIntfListV4_t   intf_dir_Tb[ACL_INTF_DIR_MAX];

} aclIntfCfgDataV4_t;

typedef struct
{
  /* NOTE: aclData is indexed by list entry number, not directly by ACL ID.
   *       Entry 0 of this array is reserved, but unused (mainly for backwards
   *       compatibility with config files from earlier releases).
   */
  aclCfgDataV4_t      aclData[L7_ACL_MAX_LISTS_REL_4_3+1];

  /* Array to hold information about the ACLs an interface is configured with */
  aclIntfCfgDataV4_t  aclIntfCfgData[L7_ACL_INTF_MAX_COUNT_REL_4_3];

  L7_uint32           aclMsgLvl;

} L7_aclCfgParmsV4_t;

typedef struct
{
  L7_fileHdr_t        cfgHdr;
  L7_aclCfgParmsV4_t  cfgParms;
  L7_uint32           checkSum;
  
} aclCfgFileDataV4_t;

/***** End V4 definitions (Rel 4.3.x) *****/


/***** Start V5 definitions (Rel 4.4.x) *****/

typedef enum
{
  ACL_RULENUM_V5 = 1,
  ACL_ACTION_V5,

  /* rule attributes */
  ACL_ASSIGN_QUEUEID_V5,
  ACL_REDIRECT_INTF_V5,
  ACL_MIRROR_INTF_V5,
  ACL_LOGGING_V5,

  /* match fields */
  ACL_EVERY_V5,
  ACL_PROTOCOL_V5,
  ACL_PROTOCOL_MASK_V5,
  ACL_SRCIP_V5,
  ACL_SRCIP_MASK_V5,
  ACL_SRCPORT_V5,
  ACL_SRCSTARTPORT_V5,
  ACL_SRCENDPORT_V5,
  ACL_DSTIP_V5,
  ACL_DSTIP_MASK_V5,
  ACL_DSTPORT_V5,
  ACL_DSTSTARTPORT_V5,
  ACL_DSTENDPORT_V5,
  ACL_TOSMASK_V5,
  ACL_IPPREC_V5,
  ACL_IPTOS_V5,
  ACL_IPDSCP_V5,

  ACL_RULEFIELDS_TOTAL_V5               /* total number of ACL rule fields */
  
} ruleFieldsV5_t;

typedef enum
{
  ACL_MAC_RULENUM_V5 = 1,
  ACL_MAC_ACTION_V5,

  /* rule attributes */
  ACL_MAC_ASSIGN_QUEUEID_V5,
  ACL_MAC_REDIRECT_INTF_V5,
  ACL_MAC_MIRROR_INTF_V5,
  ACL_MAC_LOGGING_V5,

  /* match fields */
  ACL_MAC_EVERY_V5,
  ACL_MAC_COS_V5,
  ACL_MAC_COS2_V5,
  ACL_MAC_DSTMAC_V5,
  ACL_MAC_DSTMAC_MASK_V5,
  ACL_MAC_ETYPE_KEYID_V5,
  ACL_MAC_ETYPE_VALUE_V5,
  ACL_MAC_SRCMAC_V5,
  ACL_MAC_SRCMAC_MASK_V5,
  ACL_MAC_VLANID_V5,
  ACL_MAC_VLANID_START_V5,
  ACL_MAC_VLANID_END_V5,
  ACL_MAC_VLANID2_V5,
  ACL_MAC_VLANID2_START_V5,
  ACL_MAC_VLANID2_END_V5,

  ACL_MAC_RULEFIELDS_TOTAL_V5           /* total number of ACL rule fields */

} macRuleFieldsV5_t;

/* L3 IP ACL classifier rule definition */
typedef struct aclRuleParmsV5_s
{
  L7_uint32       configMask;

  L7_uint32       ruleNum;                       /* access list rule num               */
  L7_uint32       action;                        /* action: permit/deny                */

  L7_uint32       assignQueueId;                 /* assigned queue id                  */
  nimConfigID_t   redirectConfigId;              /* redirect intf config id            */
  nimConfigID_t   mirrorConfigId;                /* mirror intf config id              */
  L7_BOOL         logging;                       /* denotes if rule hits are logged    */

  L7_BOOL         every;                         /* let every IP packet match this rule*/

  L7_uchar8       protocol;                      /* protocol number                    */
  L7_uchar8       protmask;                      /* protocol mask value                */

  L7_uint32       srcIp;                         /* source ip                          */
  L7_uint32       srcMask;                       /* source mask                        */
  L7_ushort16     srcPort;                       /* source port: keyword               */
  L7_ushort16     srcStartPort;                  /* source port range: start port      */
  L7_ushort16     srcEndPort;                    /* source port range: end port        */

  L7_uint32       dstIp;                         /* destination ip                     */
  L7_uint32       dstMask;                       /* destination mask                   */
  L7_ushort16     dstPort;                       /* destination port: keyword          */
  L7_ushort16     dstStartPort;                  /* destination port range: start port */
  L7_ushort16     dstEndPort;                    /* destination port range: end port   */

  L7_uchar8       tosbyte;                       /* 8 bit tos byte                     */
  L7_uchar8       tosmask;                       /* tos mask                           */

  L7_BOOL         last;                          /* check for last rule in an acl      */
  struct aclRuleParmsV5_s   *next;               /* used by AVL - pointer to next rule */
} aclRuleParmsV5_t;

/* L2 MAC ACL classifier rule definition */
typedef struct aclMacRuleParmsV5_s
{
  L7_uint32       configMask;

  L7_uint32       ruleNum;                       /* access list rule num               */
  L7_uint32       action;                        /* action: permit/deny                */

  L7_uint32       assignQueueId;                 /* assigned queue id                  */
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

  struct aclMacRuleParms_V5s   *next;            /* used by AVL - pointer to next rule */
} aclMacRuleParmsV5_t;

typedef union aclCfgRuleParmsV5_s
{
  aclRuleParmsV5_t          ip;                   /* IP ACL rules                      */
  aclMacRuleParmsV5_t       mac;                  /* MAC ACL rules                     */

} aclCfgRuleParmsV5_t;

typedef struct aclIdV5_s
{
  L7_ACL_TYPE_t     aclType;
  L7_uint32         aclId;                            /* IP: aclNum; MAC: aclIndex     */

} aclIdV5_t;

typedef struct
{
  aclIdV5_t           id;                                     /* ACL identification info       */
  L7_uchar8           aclName[L7_ACL_NAME_LEN_MAX+1];         /* ACL name (as needed)          */
  aclCfgRuleParmsV5_t ruleData[L7_ACL_MAX_RULE_NUM_REL_4_4+1];/* content depends on id.aclType */

} aclCfgDataV5_t;

typedef struct
{
  L7_BOOL           inUse;                            /* denotes entry is in use    */
  L7_uint32         seqNum;                           /* list order sequence number */
  aclIdV5_t         id;                               /* ACL identification info    */

} aclIntfParmsV5_t;

typedef struct
{
  aclIntfParmsV5_t    assignList[L7_ACL_MAX_LISTS_PER_INTF_DIR_REL_4_4];

} aclIntfListV5_t;

/* Struct to hold information about the ACLs configured for 
 * a given interface in each direction
 */
typedef struct
{
  nimConfigID_t     configId; /* must be first in the struct */
  aclIntfListV5_t   intf_dir_Tb[ACL_INTF_DIR_MAX];

} aclIntfCfgDataV5_t;

typedef struct
{
  /* NOTE: aclData is indexed by list entry number, not directly by ACL ID.
   *       Entry 0 of this array is reserved, but unused (mainly for backwards
   *       compatibility with config files from earlier releases).
   */
  aclCfgDataV5_t      aclData[L7_ACL_MAX_LISTS_REL_4_4+1];

  /* Array to hold information about the ACLs an interface is configured with */
  aclIntfCfgDataV5_t  aclIntfCfgData[L7_ACL_INTF_MAX_COUNT_REL_4_4];

  L7_uint32           aclMsgLvl;

} L7_aclCfgParmsV5_t;

typedef struct
{
  L7_fileHdr_t        cfgHdr;
  L7_aclCfgParmsV5_t  cfgParms;
  L7_uint32           checkSum;
  
} aclCfgFileDataV5_t;

/***** End V5 definitions (Rel 4.4.x) *****/


/***** Start V6 definitions (Rel 5.0.x) *****/

typedef enum
{
  ACL_RULENUM_V6 = 1,
  ACL_ACTION_V6,

  /* rule attributes */
  ACL_ASSIGN_QUEUEID_V6,
  ACL_REDIRECT_INTF_V6,
  ACL_MIRROR_INTF_V6,
  ACL_LOGGING_V6,

  /* match fields */
  ACL_EVERY_V6,
  ACL_PROTOCOL_V6,
  ACL_PROTOCOL_MASK_V6,
  ACL_SRCIP_V6,
  ACL_SRCIP_MASK_V6,
  ACL_SRCIPV6_V6,                       /* IPv6 address and prefix length */
  ACL_SRCPORT_V6,
  ACL_SRCSTARTPORT_V6,
  ACL_SRCENDPORT_V6,
  ACL_DSTIP_V6,
  ACL_DSTIP_MASK_V6,
  ACL_DSTIPV6_V6,                       /* IPv6 address and prefix length */
  ACL_DSTPORT_V6,
  ACL_DSTSTARTPORT_V6,
  ACL_DSTENDPORT_V6,
  ACL_TOSMASK_V6,
  ACL_IPPREC_V6,
  ACL_IPTOS_V6,
  ACL_IPDSCP_V6,
  ACL_FLOWLBLV6_V6,

  ACL_RULEFIELDS_TOTAL_V6               /* total number of ACL rule fields */
  
} ruleFieldsV6_t;

typedef enum
{
  ACL_MAC_RULENUM_V6 = 1,
  ACL_MAC_ACTION_V6,

  /* rule attributes */
  ACL_MAC_ASSIGN_QUEUEID_V6,
  ACL_MAC_REDIRECT_INTF_V6,
  ACL_MAC_MIRROR_INTF_V6,
  ACL_MAC_LOGGING_V6,

  /* match fields */
  ACL_MAC_EVERY_V6,
  ACL_MAC_COS_V6,
  ACL_MAC_COS2_V6,
  ACL_MAC_DSTMAC_V6,
  ACL_MAC_DSTMAC_MASK_V6,
  ACL_MAC_ETYPE_KEYID_V6,
  ACL_MAC_ETYPE_VALUE_V6,
  ACL_MAC_SRCMAC_V6,
  ACL_MAC_SRCMAC_MASK_V6,
  ACL_MAC_VLANID_V6,
  ACL_MAC_VLANID_START_V6,
  ACL_MAC_VLANID_END_V6,
  ACL_MAC_VLANID2_V6,
  ACL_MAC_VLANID2_START_V6,
  ACL_MAC_VLANID2_END_V6,

  ACL_MAC_RULEFIELDS_TOTAL_V6           /* total number of ACL rule fields */

} macRuleFieldsV6_t;

typedef union
{
  struct
  {
    L7_uint32     addr;                         /*  IPv4 address                       */
    L7_uint32     mask;                         /*  IPv4 netmask                       */
  } v4;

  L7_in6_prefix_t v6;

} aclIpAddrV6_t;

/* L3 IP ACL classifier rule definition */
typedef struct aclRuleParmsV6_s
{
  L7_uint32       configMask;

  L7_uint32       ruleNum;                       /* access list rule num               */
  L7_uint32       action;                        /* action: permit/deny                */

  L7_uint32       assignQueueId;                 /* assigned queue id                  */
  nimConfigID_t   redirectConfigId;              /* redirect intf config id            */
  nimConfigID_t   mirrorConfigId;                /* mirror intf config id              */
  L7_BOOL         logging;                       /* denotes if rule hits are logged    */

  L7_BOOL         every;                         /* let every IP packet match this rule*/

  L7_uchar8       protocol;                      /* protocol number                    */
  L7_uchar8       protmask;                      /* protocol mask value                */

  aclIpAddrV6_t   srcIp;                         /* combined v4/v6 addr, mask/prefix   */
  L7_ushort16     srcPort;                       /* source port: keyword               */
  L7_ushort16     srcStartPort;                  /* source port range: start port      */
  L7_ushort16     srcEndPort;                    /* source port range: end port        */

  aclIpAddrV6_t   dstIp;                         /* combined v4/v6 addr, mask/prefix   */
  L7_ushort16     dstPort;                       /* destination port: keyword          */
  L7_ushort16     dstStartPort;                  /* destination port range: start port */
  L7_ushort16     dstEndPort;                    /* destination port range: end port   */

  L7_uchar8       tosbyte;                       /* 8 bit tos byte                     */
  L7_uchar8       tosmask;                       /* tos mask                           */

  L7_uint32       flowlbl;                       /* ipv6 flow label (20-bit)           */

  L7_BOOL         last;                          /* check for last rule in an acl      */
  struct aclRuleParms_s   *next;                 /* used by AVL - pointer to next rule */
} aclRuleParmsV6_t;

/* L2 MAC ACL classifier rule definition */
typedef struct aclMacRuleParmsV6_s
{
  L7_uint32       configMask;

  L7_uint32       ruleNum;                       /* access list rule num               */
  L7_uint32       action;                        /* action: permit/deny                */

  L7_uint32       assignQueueId;                 /* assigned queue id                  */
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

  struct aclMacRuleParms_V6s   *next;            /* used by AVL - pointer to next rule */
} aclMacRuleParmsV6_t;

typedef union aclCfgRuleParmsV6_s
{
  aclRuleParmsV6_t          ip;                   /* IP ACL rules                      */
  aclMacRuleParmsV6_t       mac;                  /* MAC ACL rules                     */

} aclCfgRuleParmsV6_t;

typedef struct aclIdV6_s
{
  L7_ACL_TYPE_t     aclType;
  L7_uint32         aclId;                            /* IP: aclNum; MAC: aclIndex     */

} aclIdV6_t;

typedef struct
{
  aclIdV6_t           id;                                     /* ACL identification info       */
  L7_uchar8           aclName[L7_ACL_NAME_LEN_MAX+1];         /* ACL name (as needed)          */
  aclCfgRuleParmsV6_t ruleData[L7_ACL_MAX_RULE_NUM_REL_5_0+1];/* content depends on id.aclType */

} aclCfgDataV6_t;

typedef struct
{
  L7_BOOL           inUse;                            /* denotes entry is in use    */
  L7_uint32         seqNum;                           /* list order sequence number */
  aclIdV6_t         id;                               /* ACL identification info    */

} aclIntfParmsV6_t;

typedef struct
{
  aclIntfParmsV6_t    assignList[L7_ACL_MAX_LISTS_PER_INTF_DIR_REL_5_0];

} aclIntfListV6_t;

typedef struct
{
  aclIntfParmsV6_t    assignList[L7_ACL_MAX_LISTS_PER_VLAN_DIR_REL_5_0];

} aclVlanListV6_t;

/* Struct to hold information about the ACLs configured for 
 * a given interface in each direction
 */
typedef struct
{
  nimConfigID_t     configId; /* must be first in the struct */
  aclIntfListV6_t   intf_dir_Tb[ACL_INTF_DIR_MAX];

} aclIntfCfgDataV6_t;

/*
 * Struct to hold information about the ACLs configured for a given VLAN in
 * each direction.
 */
typedef struct
{
  L7_uint32       vlanId;
  L7_BOOL         inUse;
  aclVlanListV6_t vlan_dir_Tb[ACL_INTF_DIR_MAX];
} aclVlanCfgDataV6_t;

typedef struct
{
  /* NOTE: aclData is indexed by list entry number, not directly by ACL ID.
   *       Entry 0 of this array is reserved, but unused (mainly for backwards
   *       compatibility with config files from earlier releases).
   */
  aclCfgDataV6_t      aclData[L7_ACL_MAX_LISTS_REL_5_0+1];

  /* Array to hold information about the ACLs an interface is configured with */
  aclIntfCfgDataV6_t  aclIntfCfgData[L7_ACL_INTF_MAX_COUNT_REL_5_0];
  /*
   * Array to hold information about the ACLs with which a VLAN is configured.
   * The array is indexed via aclVlanMapTbl.  This index table is built when
   * the configuration is read and is an index both for this configuration and
   * for pAclVlanInfo.
   */
  aclVlanCfgDataV6_t  aclVlanCfgData[L7_ACL_VLAN_MAX_COUNT_REL_5_0+1];  /* index 0 not used */

  L7_uint32           aclMsgLvl;

} L7_aclCfgParmsV6_t;

typedef struct
{
  L7_fileHdr_t        cfgHdr;
  L7_aclCfgParmsV6_t  cfgParms;
  L7_uint32           checkSum;
  
} aclCfgFileDataV6_t;

/***** End V6 definitions (Rel 5.0.x) *****/


#endif /* ACL_MIGRATE_H */
