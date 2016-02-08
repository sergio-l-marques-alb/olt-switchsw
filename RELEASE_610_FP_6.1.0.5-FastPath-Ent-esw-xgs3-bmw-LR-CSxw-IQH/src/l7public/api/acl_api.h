/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename acl_api.h
*
* @purpose Contains LVL7 Access Control List offerings 
*
* @component Access Control List
*
* @comments 
*
* @create 02/21/2002
*
* @author rjindal
*
* @end
*
**********************************************************************/


#ifndef INCLUDE_ACL_API_H
#define INCLUDE_ACL_API_H


#include "l7_common.h"
#include "l7_cos_api.h"
#include "acl_exports.h"
#include "diffserv_exports.h"
#include "acl_exports.h"
#include "diffserv_exports.h"
#include "nimapi.h"
#include "l3_commdefs.h"
#include "l3_addrdefs.h"
#include "defaultconfig.h"
#include "default_cnfgr.h"
#include "tlv_api.h"

/* the following values are established here if not already defined in platform.h */
#ifndef L7_ACL_MAX_LISTS
  #define L7_ACL_MAX_LISTS                  FD_QOS_ACL_MAX_LISTS
#endif

#ifndef L7_ACL_MAX_RULES_PER_LIST
  #define L7_ACL_MAX_RULES_PER_LIST         FD_QOS_ACL_MAX_RULES_PER_LIST
#endif

#ifndef L7_ACL_MAX_RULES_PER_MULTILIST
  #define L7_ACL_MAX_RULES_PER_MULTILIST    L7_ACL_MAX_RULES_PER_LIST
#endif

/* number of unique VLAN IDs that ACLs can be associated with */
#ifndef L7_ACL_VLAN_MAX_COUNT  
  #define L7_ACL_VLAN_MAX_COUNT               24
#endif

/* define the maximum number of ACLs that can be configured per directional
 * interface (this is used when storing or displaying the ACL interface config)
 */
#if (L7_ACL_MAX_RULES_PER_MULTILIST == 0)
  #define L7_ACL_MAX_LISTS_PER_VLAN_DIR     L7_ACL_MAX_LISTS
#else
  #define L7_ACL_MAX_LISTS_PER_VLAN_DIR     L7_ACL_MAX_RULES_PER_MULTILIST
#endif
#if (L7_ACL_MAX_RULES_PER_MULTILIST == 0)
  #define L7_ACL_MAX_LISTS_PER_INTF_DIR     L7_ACL_MAX_LISTS
#else
  #define L7_ACL_MAX_LISTS_PER_INTF_DIR     L7_ACL_MAX_RULES_PER_MULTILIST
#endif  

#ifndef L7_MAX_CFG_QUEUES_PER_PORT
  #define L7_MAX_CFG_QUEUES_PER_PORT        1
#endif

/* define maximum number of applied ACL rules that can have logging enabled */
#ifndef L7_ACL_LOG_RULE_LIMIT
  #define L7_ACL_LOG_RULE_LIMIT             FD_QOS_ACL_LOG_RULE_LIMIT
#endif

#ifndef L7_ACL_LOG_RULE_PER_INTF_DIR_LIMIT
  #define L7_ACL_LOG_RULE_PER_INTF_DIR_LIMIT    min(L7_ACL_LOG_RULE_LIMIT, L7_ACL_MAX_RULES_PER_MULTILIST)
#endif

/* assign queue id limits */
#define L7_ACL_ASSIGN_QUEUE_ID_MIN          0
#define L7_ACL_ASSIGN_QUEUE_ID_MAX          (L7_MAX_CFG_QUEUES_PER_PORT-1)

/* range of access list rule numbers */
#define L7_ACL_MIN_RULE_NUM                 1
#define L7_ACL_MAX_RULE_NUM                 L7_ACL_MAX_RULES_PER_LIST
#define L7_ACL_DEFAULT_RULE_NUM             (L7_ACL_MAX_RULE_NUM+1)

/* define allowed IPV6 ACL named list index ranges (as perceived externally) */
#define L7_ACL_NAMED_IPV4_MIN_INDEX               1000
#define L7_ACL_NAMED_IPV4_MAX_INDEX               (L7_ACL_NAMED_IPV4_MIN_INDEX + L7_ACL_MAX_LISTS - 1)

/* define allowed MAC ACL named list index ranges (as perceived externally) */
#define L7_ACL_MAC_MIN_INDEX                10000
#define L7_ACL_MAC_MAX_INDEX                (L7_ACL_MAC_MIN_INDEX + L7_ACL_MAX_LISTS - 1)

/* define allowed IPV6 ACL named list index ranges (as perceived externally) */
#define L7_ACL_IPV6_MIN_INDEX               20000
#define L7_ACL_IPV6_MAX_INDEX               (L7_ACL_IPV6_MIN_INDEX + L7_ACL_MAX_LISTS - 1)

/*** TLV definitions for ACL ***/
#define L7_QOS_ACL_TLV_MAX_SIZE                 65536 /* 64 KB is TLV util limit */
                                                      
#define L7_QOS_ACL_TLV_LIST_TYPE                0x0020
#define L7_QOS_ACL_TLV_INST_DEL_LIST_TYPE       0x0021
                                                      
#define L7_QOS_ACL_TLV_RULE_DEF                 0x0610
#define L7_QOS_ACL_TLV_MATCH_COS_TYPE           0x0611
#define L7_QOS_ACL_TLV_MATCH_COS2_TYPE          0x0612
#define L7_QOS_ACL_TLV_MATCH_DSTIP_TYPE         0x0613
#define L7_QOS_ACL_TLV_MATCH_DSTIPV6_TYPE       0x0614
#define L7_QOS_ACL_TLV_MATCH_DSTL4PORT_TYPE     0x0615
#define L7_QOS_ACL_TLV_MATCH_DSTMAC_TYPE        0x0616
#define L7_QOS_ACL_TLV_MATCH_ETYPE_TYPE         0x0617
#define L7_QOS_ACL_TLV_MATCH_EVERY_TYPE         0x0618
#define L7_QOS_ACL_TLV_MATCH_FLOWLBLV6_TYPE     0x0619
#define L7_QOS_ACL_TLV_MATCH_ICMPMSG_TYPE       0x061A
#define L7_QOS_ACL_TLV_MATCH_IPTOS_TYPE         0x061B
#define L7_QOS_ACL_TLV_MATCH_PROTOCOL_TYPE      0x061C
#define L7_QOS_ACL_TLV_MATCH_SRCIP_TYPE         0x061D
#define L7_QOS_ACL_TLV_MATCH_SRCIPV6_TYPE       0x061E
#define L7_QOS_ACL_TLV_MATCH_SRCL4PORT_TYPE     0x061F
#define L7_QOS_ACL_TLV_MATCH_SRCMAC_TYPE        0x0620
#define L7_QOS_ACL_TLV_MATCH_VLANID_TYPE        0x0621
#define L7_QOS_ACL_TLV_MATCH_VLANID2_TYPE       0x0622

/* these values parallel their DiffServ counterparts */                                                      
#define L7_QOS_ACL_TLV_ATTR_ASSIGN_QUEUE_TYPE   0x0711
#define L7_QOS_ACL_TLV_ATTR_MIRROR_TYPE         0x0718
#define L7_QOS_ACL_TLV_ATTR_REDIRECT_TYPE       0x071E

/* ACL List Instance */
typedef struct
{
  L7_uint32     instanceKey; /* aclnum */
} aclTlvListType_t;
#define L7_QOS_ACL_TLV_LIST_TYPE_LEN (L7_uint32)sizeof(aclTlvListType_t)

/* ACL Deletion List TLV */
typedef struct
{
  L7_uint32     keyCount;
  L7_uint32     instanceKey; /* may extend this later for a list */
} aclTlvInstDelList_t;
#define L7_QOS_ACL_TLV_INST_DEL_LIST_LEN (L7_uint32)sizeof(aclTlvInstDelList_t)
/* special definition for minimum native length (useful for building, parsing)*/
#define L7_QOS_ACL_TLV_INST_DEL_LIST_MIN_LEN (L7_uint32)sizeof(L7_uint32)


/* ACL Rule Definition TLV */
#define L7_QOS_ACL_TLV_RULE_DEF_PERMIT 0
#define L7_QOS_ACL_TLV_RULE_DEF_DENY   1 
#define L7_QOS_ACL_TLV_RULE_CORR_NULL  0        /* 0 means no rule logging    */
typedef struct
{
  L7_uchar8     denyFlag;                       
  L7_uchar8     rsvd1[3];                       /* reserved -- for alignment  */
  L7_uint32     logCorrelator;                  /* rule identifier for logging*/
} aclTlvRuleDef_t;
#define L7_QOS_ACL_TLV_RULE_DEF_LEN (L7_uint32)sizeof(aclTlvRuleDef_t)


/* Class of Service (CoS) match criterion TLV */
typedef struct
{
  L7_uchar8     cosValue;                       /* COS value (0-7)            */
  L7_uchar8     rsvd1[3];                       /* reserved -- for alignment  */
} aclTlvMatchCos_t;
#define L7_QOS_ACL_TLV_MATCH_COS_LEN ((L7_uint32)sizeof(aclTlvMatchCos_t))


/* Secondary Class of Service (CoS2) match criterion TLV */
typedef struct
{
  L7_uchar8     cosValue;                       /* COS value (0-7)            */
  L7_uchar8     rsvd1[3];                       /* reserved -- for alignment  */
} aclTlvMatchCos2_t;
#define L7_QOS_ACL_TLV_MATCH_COS2_LEN ((L7_uint32)sizeof(aclTlvMatchCos2_t))


/* Destination IP Address match criterion TLV */
typedef struct
{
  L7_uint32     ipAddr;
  L7_uint32     ipMask;
} aclTlvMatchDstIp_t;
#define L7_QOS_ACL_TLV_MATCH_DSTIP_LEN (L7_uint32)sizeof(aclTlvMatchDstIp_t)


/* Destination IPv6 Address match criterion TLV */
typedef struct
{
  L7_uchar8     ipv6Addr[L7_IP6_ADDR_LEN];
  L7_uchar8     ipv6Mask[L7_IP6_ADDR_LEN];
} aclTlvMatchDstIpv6_t;
#define L7_QOS_ACL_TLV_MATCH_DSTIPV6_LEN (L7_uint32)sizeof(aclTlvMatchDstIpv6_t)


/* Destination Layer-4 Port Range match criterion TLV */
#define L7_QOS_ACL_TLV_MATCH_DSTL4PORT_MASK  0xFFFF
#define L7_QOS_ACL_TLV_MATCH_DSTL4PORT_SHIFT 0
typedef struct
{
  L7_ushort16   portStart;
  L7_ushort16   portEnd;
  L7_ushort16   portMask;
  L7_ushort16   rsvd1;                          /* reserved -- for alignment  */
} aclTlvMatchDstL4Port_t;
#define L7_QOS_ACL_TLV_MATCH_DSTL4PORT_LEN (L7_uint32)sizeof(aclTlvMatchDstL4Port_t)


/* Destination MAC Address match criterion TLV */
typedef struct
{
  L7_uchar8     macAddr[L7_MAC_ADDR_LEN];
  L7_uchar8     macMask[L7_MAC_ADDR_LEN];
} aclTlvMatchDstMac_t;
#define L7_QOS_ACL_TLV_MATCH_DSTMAC_LEN ((L7_uint32)sizeof(aclTlvMatchDstMac_t))


/* Ethertype match criterion TLV */
typedef struct
{
  L7_ushort16   etypeValue1;
  L7_ushort16   etypeValue2;
  L7_uchar8     checkStdHdrFlag;                /* qualify using protocol hdr */
  L7_uchar8     rsvd1[3];                       /* reserved -- for alignment  */
} aclTlvMatchEtype_t;
#define L7_QOS_ACL_TLV_MATCH_ETYPE_LEN ((L7_uint32)sizeof(aclTlvMatchEtype_t))


/* "Match Every" match criterion TLV */
#define L7_QOS_ACL_TLV_MATCH_EVERY_LEN       0


/* IPv6 Flow Label match criterion TLV */
#define L7_QOS_ACL_TLV_MATCH_FLOWLBLV6_MASK   0xFFFFF
#define L7_QOS_ACL_TLV_MATCH_FLOWLBLV6_SHIFT  0
typedef struct
{
  L7_uint32     flowLabel;                      /* defined as 20-bit value    */
  L7_uint32     flowLabelMask;                  /* in case masking allowed    */
} aclTlvMatchFlowlblv6_t;
#define L7_QOS_ACL_TLV_MATCH_FLOWLBLV6_LEN (L7_uint32)sizeof(aclTlvMatchFlowlblv6_t)


/* ICMP Message match criterion TLV */
/* NOTE: Currently used for implicit IPv6 deny all only (not user configurable) */
#define L7_QOS_ACL_TLV_MATCH_ICMPMSG_MASK_OFF    0x00
#define L7_QOS_ACL_TLV_MATCH_ICMPMSG_MASK_ON     0xFF
typedef struct
{
  L7_uchar8     msgType; 
  L7_uchar8     msgTypeMask;
  L7_uchar8     msgCode; 
  L7_uchar8     msgCodeMask;
} aclTlvMatchIcmpMsg_t;
#define L7_QOS_ACL_TLV_MATCH_ICMPMSG_LEN (L7_uint32)sizeof(aclTlvMatchIcmpMsg_t)


/* IP Type of Service (ToS) match criterion TLV */
#define L7_QOS_ACL_TLV_MATCH_IPDSCP_MASK         0xFC
#define L7_QOS_ACL_TLV_MATCH_IPDSCP_SHIFT        2    
#define L7_QOS_ACL_TLV_MATCH_IPPRECEDENCE_MASK   0xE0
#define L7_QOS_ACL_TLV_MATCH_IPPRECEDENCE_SHIFT  5
typedef struct
{
  L7_uchar8     tosValue;
  L7_uchar8     tosMask;
  L7_uchar8     rsvd1[2];                       /* reserved -- for alignment  */
} aclTlvMatchIpTos_t;
#define L7_QOS_ACL_TLV_MATCH_IPTOS_LEN (L7_uint32)sizeof(aclTlvMatchIpTos_t)


/* Protocol Number match criterion TLV */
#define L7_QOS_ACL_TLV_MATCH_PROTOCOL_MASK    0xFF
#define L7_QOS_ACL_TLV_MATCH_PROTOCOL_IP_MASK 0x00  /* 'match all' protocols */
#define L7_QOS_ACL_TLV_MATCH_PROTOCOL_SHIFT   0
typedef struct
{
  L7_uchar8     protoNumValue;
  L7_uchar8     protoNumMask;
  L7_uchar8     rsvd1[2];                       /* reserved -- for alignment  */
} aclTlvMatchProtocol_t;
#define L7_QOS_ACL_TLV_MATCH_PROTOCOL_LEN (L7_uint32)sizeof(aclTlvMatchProtocol_t)


/* Source IP Address match criterion TLV */
typedef struct
{
  L7_uint32     ipAddr;
  L7_uint32     ipMask;
} aclTlvMatchSrcIp_t;
#define L7_QOS_ACL_TLV_MATCH_SRCIP_LEN (L7_uint32)sizeof(aclTlvMatchSrcIp_t)


/* Source IPv6 Address match criterion TLV */
typedef struct
{
  L7_uchar8     ipv6Addr[L7_IP6_ADDR_LEN];
  L7_uchar8     ipv6Mask[L7_IP6_ADDR_LEN];
} aclTlvMatchSrcIpv6_t;
#define L7_QOS_ACL_TLV_MATCH_SRCIPV6_LEN (L7_uint32)sizeof(aclTlvMatchSrcIpv6_t)


/* Source Layer-4 Port Range match criterion TLV */
#define L7_QOS_ACL_TLV_MATCH_SRCL4PORT_MASK  0xFFFF
#define L7_QOS_ACL_TLV_MATCH_SRCL4PORT_SHIFT 0
typedef struct
{
  L7_ushort16   portStart;
  L7_ushort16   portEnd;
  L7_ushort16   portMask;
  L7_ushort16   rsvd1;                          /* reserved -- for alignment  */
} aclTlvMatchSrcL4Port_t;
#define L7_QOS_ACL_TLV_MATCH_SRCL4PORT_LEN (L7_uint32)sizeof(aclTlvMatchSrcL4Port_t)


/* Source MAC Address match criterion TLV */
typedef struct
{
  L7_uchar8     macAddr[L7_MAC_ADDR_LEN];
  L7_uchar8     macMask[L7_MAC_ADDR_LEN];
} aclTlvMatchSrcMac_t;
#define L7_QOS_ACL_TLV_MATCH_SRCMAC_LEN ((L7_uint32)sizeof(aclTlvMatchSrcMac_t))


/* VLAN ID match criterion TLV */
typedef struct
{
  L7_ushort16   vidStart;                       /* VLAN ID start              */
  L7_ushort16   vidEnd;                         /* VLAN ID end                */
} aclTlvMatchVlanId_t;
#define L7_QOS_ACL_TLV_MATCH_VLANID_LEN ((L7_uint32)sizeof(aclTlvMatchVlanId_t))


/* Secondary VLAN ID (VLAN ID2) match criterion TLV */
typedef struct
{
  L7_ushort16   vidStart;                       /* Secondary ID start         */
  L7_ushort16   vidEnd;                         /* Secondary ID end           */
} aclTlvMatchVlanId2_t;
#define L7_QOS_ACL_TLV_MATCH_VLANID2_LEN ((L7_uint32)sizeof(aclTlvMatchVlanId2_t))


/* Assign Queue rule attribute TLV */
typedef struct
{
  L7_uchar8     qid;                            /* queue id                   */
  L7_uchar8     rsvd1[3];                       /* reserved -- for alignment  */
} aclTlvAttrAssignQueue_t;
#define L7_QOS_ACL_TLV_ATTR_ASSIGN_QUEUE_LEN ((L7_uint32)sizeof(aclTlvAttrAssignQueue_t))


/* Redirect rule attribute TLV */
typedef struct
{
  L7_uint32     intfUnit;                       /* interface unit number      */
  L7_uint32     intfSlot;                       /* interface slot number      */
  L7_uint32     intfPort;                       /*interface port num (0-based)*/
} aclTlvAttrRedirect_t;               
#define L7_QOS_ACL_TLV_ATTR_REDIRECT_LEN ((L7_uint32)sizeof(aclTlvAttrRedirect_t))

/* Mirror rule attribute TLV */
typedef struct
{
  L7_uint32     intfUnit;                       /* interface unit number      */
  L7_uint32     intfSlot;                       /* interface slot number      */
  L7_uint32     intfPort;                       /*interface port num (0-based)*/
} aclTlvAttrMirror_t;               
#define L7_QOS_ACL_TLV_ATTR_MIRROR_LEN ((L7_uint32)sizeof(aclTlvAttrMirror_t))


typedef enum
{
  ACL_RULENUM = 1,
  ACL_ACTION,

  /* rule attributes */
  ACL_ASSIGN_QUEUEID,
  ACL_REDIRECT_INTF,
  ACL_MIRROR_INTF,
  ACL_LOGGING,

  /* match fields */
  ACL_EVERY,
  ACL_PROTOCOL,
  ACL_PROTOCOL_MASK,
  ACL_SRCIP,
  ACL_SRCIP_MASK,
  ACL_SRCIPV6,                          /* IPv6 address and prefix length */
  ACL_SRCPORT,
  ACL_SRCSTARTPORT,
  ACL_SRCENDPORT,
  ACL_DSTIP,
  ACL_DSTIP_MASK,
  ACL_DSTIPV6,                          /* IPv6 address and prefix length */
  ACL_DSTPORT,
  ACL_DSTSTARTPORT,
  ACL_DSTENDPORT,
  ACL_TOSMASK,
  ACL_IPPREC,
  ACL_IPTOS,
  ACL_IPDSCP,
  ACL_FLOWLBLV6,

  ACL_RULEFIELDS_TOTAL                  /* total number of ACL rule fields */
  
} ruleFields_t;

typedef enum
{
  ACL_MAC_RULENUM = 1,
  ACL_MAC_ACTION,

  /* rule attributes */
  ACL_MAC_ASSIGN_QUEUEID,
  ACL_MAC_REDIRECT_INTF,
  ACL_MAC_MIRROR_INTF,
  ACL_MAC_LOGGING,

  /* match fields */
  ACL_MAC_EVERY,
  ACL_MAC_COS,
  ACL_MAC_COS2,
  ACL_MAC_DSTMAC,
  ACL_MAC_DSTMAC_MASK,
  ACL_MAC_ETYPE_KEYID,
  ACL_MAC_ETYPE_VALUE,
  ACL_MAC_SRCMAC,
  ACL_MAC_SRCMAC_MASK,
  ACL_MAC_VLANID,
  ACL_MAC_VLANID_START,
  ACL_MAC_VLANID_END,
  ACL_MAC_VLANID2,
  ACL_MAC_VLANID2_START,
  ACL_MAC_VLANID2_END,

  ACL_MAC_RULEFIELDS_TOTAL              /* total number of ACL rule fields */

} macRuleFields_t;

/* ACL route filter types */
typedef enum
{
  L7_ACL_ROUTE_FILTER_CREATE = 1,
  L7_ACL_ROUTE_FILTER_MODIFY,
  L7_ACL_ROUTE_FILTER_DELETE
} aclRouteFilter_t;

/* ACL notify event types */
typedef enum
{
  L7_ACL_NOTIFY_EVENT_CREATE = 1,
  L7_ACL_NOTIFY_EVENT_MODIFY,
  L7_ACL_NOTIFY_EVENT_ATTR_ONLY_MODIFY, /* only changed a rule attribute (assign-queue, etc.) */
  L7_ACL_NOTIFY_EVENT_DELETE,
  L7_ACL_NOTIFY_EVENT_RENAME            /* ACL name was changed */

} aclNotifyEvent_t;

/*********************************************************************
* @purpose  Callback function signature for ACL route filter events from QOS package.
*
* @param    aclnum      @b{(input)} ACL index value (can be ACL number or index)
* @param    event       @b{(input)} ACL route filter event
*
* @returns  void
*
* @comments This function is invoked by the QOS ACL component based on component
*           callback registration.
*
* @end
*********************************************************************/
typedef void (*ACL_ROUTE_FILTER_FUNCPTR_t) (L7_uint32 aclnum, 
                                            aclRouteFilter_t event);

/*********************************************************************
* @purpose  Callback function signature for ACL notify events from QOS package.
*
* @param    aclEvent    @b{(input)} ACL event
* @param    aclType     @b{(input)} Type of ACL
* @param    aclIndex    @b{(input)} ACL index value (can be ACL number or index)
* @param    aclName     @b{(input)} Name of this ACL
* @param    aclOldName  @b{(input)} Old name of ACL (name change only), or L7_NULLPTR
*
* @returns  void
*
* @comments This function is invoked by the QOS ACL component based on component
*           callback registration.
*
* @end
*********************************************************************/
typedef void (*ACL_NOTIFY_EVENT_FUNCPTR_t) (aclNotifyEvent_t aclEvent,
                                            L7_ACL_TYPE_t aclType,
                                            L7_uint32 aclIndex, 
                                            L7_uchar8 *aclName,
                                            L7_uchar8 *aclOldName);

/*********************************************************************
* @purpose  Callback function signature to approve a request to delete an ACL.
*
* @param    aclType     @b{(input)} Type of ACL
* @param    aclIndex    @b{(input)} ACL index value (can be ACL number or index)
* @param    aclName     @b{(input)} Name of this ACL
*
* @returns  L7_SUCCESS  OK to delete ACL
* @returns  L7_FAILURE  Do not delete ACL (it is currently in use)
*
* @comments This function is invoked by the QOS ACL component based on component
*           callback registration.
*
* @comments IMPORTANT: The callback function MUST NOT BLOCK during its processing   
*           and must RESPOND SYNCHRONOUSLY to this request.
*
* @end
*********************************************************************/
typedef L7_RC_t (*ACL_DELETE_APPROVAL_FUNCPTR_t) (L7_ACL_TYPE_t aclType,
                                                  L7_uint32 aclIndex, 
                                                  L7_uchar8 *aclName);

/* interface assigned list ACL information */
typedef struct
{
  L7_uint32         seqNum;             /* ACL evaluation order sequence number */
  L7_ACL_TYPE_t     aclType;            /* type of ACL                          */
  L7_uint32         aclId;              /* ACL identifying number (per ACL type)*/

} L7_ACL_INTF_LIST_INFO_t;

typedef struct
{
  L7_uint32               count;        /* number of valid listEntry entries         */
  L7_ACL_INTF_LIST_INFO_t listEntry[L7_ACL_MAX_LISTS_PER_INTF_DIR];  /* assigned list info */

} L7_ACL_INTF_DIR_LIST_t;

/* ACL interface list */
typedef struct
{
  L7_uint32         count;              /* number of valid intfList entries                     */
  L7_uint32         intIfNum[L7_ACL_INTF_MAX_COUNT];  /* intIfNum list to which ACL is assigned */

} L7_ACL_ASSIGNED_INTF_LIST_t;

/* VLAN assigned list ACL information */
typedef struct
{
  L7_uint32         seqNum;             /* ACL evaluation order sequence number */
  L7_ACL_TYPE_t     aclType;            /* type of ACL                          */
  L7_uint32         aclId;              /* ACL identifying number (per ACL type)*/

} L7_ACL_VLAN_LIST_INFO_t;

typedef struct
{
  L7_uint32               count;        /* number of valid listEntry entries         */
  L7_ACL_VLAN_LIST_INFO_t listEntry[L7_ACL_MAX_LISTS_PER_VLAN_DIR];  /* assigned list info */

} L7_ACL_VLAN_DIR_LIST_t;

/* ACL interface list */
typedef struct
{
  L7_uint32         count;                           /* number of valid vlanList entries      */
  L7_uint32         vlanNum[L7_MAX_NUM_VLAN_INTF];  /* vlanNum list to which ACL is assigned */

} L7_ACL_ASSIGNED_VLAN_LIST_t;

/*** IP ACL prototypes ***/

/*************************************************************************
* @purpose  Obtain the next available index value to be used when creating  
*           a new named ACL in the access list table
*
* @param    aclType     @b{(input)}  access list type
* @param    *next       @b{(output)} access list index next value
*
* @returns  L7_SUCCESS
* @returns  L7_TABLE_IS_FULL  ACL table is currently full
* @returns  L7_FAILURE        all other failures
*
* @comments The index value remains free until used in a subsequent ACL create
*           request.  When two or more requestors get the same index value,
*           only the first one to successfully create a new ACL
*           gets to keep it.  All other requestors' create requests will
*           fail due to the index being in use, so they must call this
*           function again to get a new value.
*
* @comments Although an index value is considered in use once a
*           successful row create occurs, the next index value is not
*           revealed until the row is either completed or deleted.
*
* @end
*********************************************************************/
L7_RC_t aclNamedIndexNextFree(L7_ACL_TYPE_t aclType, L7_uint32 *next);

/*************************************************************************
*
* @purpose  Retrieve the min/max index values allowed for the named ACL table 
*
* @param    aclType     @b{(input)}  access list type
* @param    *pMin       @b{(output)} Pointer to index min output location
* @param    *pMax       @b{(output)} Pointer to index max output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is purely informational.  The aclNamedIndexNext function
*           must be used to obtain the current index value.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclNamedIndexMinMaxGet(L7_ACL_TYPE_t aclType, L7_uint32 *pMin, L7_uint32 *pMax);

/*********************************************************************
*
* @purpose  Get the first named access list index created in the system.
*
* @param    aclType     @b{(input)}  access list type
* @param    *pAclIndex  @b{(output)} pointer to ACL index value
*
* @returns  L7_SUCCESS        First named access list index was found
* @returns  L7_NOT_SUPPORTED  Invalid aclType parameter specified
* @returns  L7_FAILURE  
* @returns  L7_ERROR          No access lists have been created yet
*
* @comments The only valid aclType value is L7_ACL_TYPE_IPV6.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclNamedIndexGetFirst(L7_ACL_TYPE_t aclType, L7_uint32 *pAclIndex);

/*********************************************************************
*
* @purpose  Get the first named access list index created in the system.
*
* @param    aclType     @b{(input)}  access list type
* @param    aclIndex    @b{(input)}  current ACL index value
* @param    *pAclIndex  @b{(output)} pointer to next ACL index value
*
* @returns  L7_SUCCESS        Next named access list index was found
* @returns  L7_NOT_SUPPORTED  Invalid aclType parameter specified
* @returns  L7_ERROR          No subsequent ACL index was found
* @returns  L7_FAILURE        All other failures
*
* @comments This function is also used for a 'getFirst' search by 
*           specifying an aclIndex value of 0.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclNamedIndexGetNext(L7_ACL_TYPE_t aclType, L7_uint32 aclIndex, 
                                    L7_uint32 *pAclIndex);

/*********************************************************************
*
* @purpose  Checks if aclIndex is a valid, configured named access list
*
* @param    aclType     @b{(input)}  access list type
* @param    aclIndex    @b{(input)}  ACL index value
*
* @returns  L7_SUCCESS        Access list index was found
* @returns  L7_NOT_SUPPORTED  Invalid aclType parameter specified
* @returns  L7_FAILURE        Access list index not found
*
* @comments The only valid aclType values are L7_ACL_TYPE_IPV6 and 
*           L7_ACL_TYPE_IP.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclNamedIndexCheckValid(L7_ACL_TYPE_t aclType, L7_uint32 aclIndex);

/*********************************************************************
*
* @purpose  Checks if the aclIndex is within proper range
*
* @param    aclType     @b{(input)}  access list type
* @param    aclIndex    @b{(input)}  ACL index value
*
* @returns  L7_SUCCESS        Access list index is in range
* @returns  L7_NOT_SUPPORTED  Invalid aclType parameter specified
* @returns  L7_FAILURE        Access list index is out of range
*
* @comments The only valid aclType value is L7_ACL_TYPE_IPV6.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclNamedIndexRangeCheck(L7_ACL_TYPE_t aclType, L7_uint32 aclIndex);

/*********************************************************************
*
* @purpose  Get the first access list ID created on the system.
*
* @param    L7_uint32  *aclnum   the access-list identifying number
*
* @returns  L7_SUCCESS, if the first access list ID was found
* @returns  L7_ERROR,   if the first access list ID was not found
* @returns  L7_FAILURE, if any other failure
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclNumGetFirst(L7_uint32 *aclnum);

/*********************************************************************
*
* @purpose  Given aclnum, get the next access list ID
*
* @param    L7_uint32   aclnum   the current access-list ID
* @param    L7_uint32  *next     the next ACL ID
*
* @returns  L7_SUCCESS, if the next access list ID was found
* @returns  L7_ERROR,   if no subsequent access list ID was found
* @returns  L7_FAILURE, if any other failure
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclNumGetNext(L7_uint32 aclnum, L7_uint32 *next);

/*********************************************************************
*
* @purpose  Checks if aclnum is a valid, configured access list
*
* @param    L7_uint32  aclnum   the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list was found
* @returns  L7_FAILURE, if the access list does not exist 
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclNumCheckValid(L7_uint32 aclnum);

/*********************************************************************
*
* @purpose  Checks if the ACL ID belongs to one of the supported number ranges
*
* @param    L7_uint32   aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list ID belongs to a supported range
* @returns  L7_FAILURE, if the access list ID is out of range
*
* @comments This function does not distinguish between standard and extended
*           IP ACL numbers, but considers whether the ACL ID belongs to any
*           ACL number range that is supported.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclNumRangeCheck(L7_uint32 aclnum);

/*********************************************************************
*
* @purpose  To create a new access list.
*
* @param    L7_uint32  aclnum  the access-list identifying number
*
* @returns  L7_SUCCESS, if the access-list is added
* @returns  L7_FAILURE, if invalid identifying number
* @returns  L7_ERROR, if the access-list already exists
* @returns  L7_TABLE_IS_FULL, if maximum number of ACLs already created
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclCreate(L7_uint32 aclnum);

/*********************************************************************
*
* @purpose  To delete an existing access list.
*
* @param    L7_uint32  aclnum  the access-list identifying number
* @param    L7_BOOL    force   force deletion regardless of whether in use
*
* @returns  L7_SUCCESS, if the access-list is deleted
* @returns  L7_FAILURE, if invalid identifying number
* @returns  L7_ERROR, if the access-list does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclDelete(L7_uint32 aclnum, L7_BOOL force);

/*********************************************************************
*
* @purpose  To add the name to this named access list.
*
* @param    aclnum      @b{(input)} access list identifying number
* @param    *name       @b{(input)} access list name (1 to L7_ACL_NAME_LEN_MAX chars)
*                                            
* @returns  L7_SUCCESS  name is added
* @returns  L7_FAILURE  invalid access list index, invalid name, or other failure
* @returns  L7_ERROR    access list does not exist
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t aclNameAdd(L7_uint32 aclnum, L7_uchar8 *name);

/*********************************************************************
*
* @purpose  To change the name of an existing named access list.
*
* @param    aclnum      @b{(input)} access list identifying number
* @param    *newname    @b{(input)} new access list name (1 to L7_ACL_NAME_LEN_MAX chars)
*                                            
* @returns  L7_SUCCESS  name is added
* @returns  L7_FAILURE  invalid access list index, invalid name, or other failure
* @returns  L7_ERROR    access list does not exist
* @returns  L7_ALREAD_CONFIGURED  name is in use by another ACL
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t aclNameChange(L7_uint32 aclnum, L7_uchar8 *newname);

/*********************************************************************
*
* @purpose  To get the name of this access list.
*
* @param    aclnum      @b{(input)}  access list identifying number
* @param    *name       @b{(output)} access list name (1 to L7_ACL_NAME_LEN_MAX chars)
*                                            
* @returns  L7_SUCCESS  name is retrieved
* @returns  L7_FAILURE  invalid access list number, name ptr, or other failure
* @returns  L7_ERROR    access list does not exist
*
* @comments Caller must provide a name buffer of at least 
*           (L7_ACL_NAME_LEN_MAX+1) characters.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclNameGet(L7_uint32 aclnum, L7_uchar8 *name);

/*********************************************************************
*
* @purpose  Checks if the named ACL name is a valid string
*
* @param    *name       @b{(input)} access list name string
*
* @returns  L7_SUCCESS  valid named ACL name string
* @returns  L7_FAILURE  invalid named ACL name string
*
* @comments This function only checks the name string syntax for a named ACL. 
*           It does not check if any named ACL currently contains this name.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclNameStringCheck(L7_uchar8 *name);

/*********************************************************************
*
* @purpose  To get the number of a named access list, given its name.
*
* @param    *name      @b{(input)} access list name
* @param    *aclnum    @b{(output)} access list identifying number
*                                            
* @returns  L7_SUCCESS  named access list number is retrieved
* @returns  L7_FAILURE  invalid parms, or other failure
* @returns  L7_ERROR    access list name does not exist
*
* @comments
*
* @end
*
*********************************************************************/
extern L7_RC_t aclNameToIndex(L7_uchar8 *name, L7_uint32 *aclnum);

/*********************************************************************
*
* @purpose  Get the index number of an access list, given its ACL type and name.
*
* @param    aclType     @b{(input)}  access list type
* @param    *pName      @b{(input)}  access list name pointer
* @param    *aclNum     @b{(output)} access list identifying number
*                                            
* @returns  L7_SUCCESS
* @returns  L7_ERROR    ACL does not exist
* @returns  L7_FAILURE
*
* @comments Uses the ACL type and name to output the list index
*           number.  For IPV4 numbered access lists, the ACL name
*           is its number represented in string notation, e.g. "101".
*
* @end
*
*********************************************************************/
extern L7_RC_t aclCommonNameToIndex(L7_ACL_TYPE_t aclType, L7_uchar8 *pName,
                                    L7_uint32 *pAclNum);

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
extern L7_RC_t aclCommonNameStringGet(L7_uint32 aclnum, L7_ACL_TYPE_t *aclType,
                                      L7_uchar8 *name);

/*********************************************************************
*
* @purpose  To add an action to a new access list.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  action   action type (permit/deny)
*
* @returns  L7_SUCCESS, if the access option is added
* @returns  L7_FAILURE, if invalid identifying number
* @returns  L7_ERROR, if the access-list does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleActionAdd(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 action);

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
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleRemove(L7_uint32 aclnum, L7_uint32 rulenum);

/*********************************************************************
*
* @purpose  To set the assigned queue id for an access list rule.
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  queueId   the queue id value              
*
* @returns  L7_SUCCESS, if assigned queue id is added
* @returns  L7_FAILURE, if invalid queue id or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleAssignQueueIdAdd(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 queueId);

/*********************************************************************
*
* @purpose  To set the redirect interface config id for an access list rule.
*
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *configId  the redirect interface config id
*                                            
* @returns  L7_SUCCESS, if redirect interface is added
* @returns  L7_FAILURE, if invalid redirect interface or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleRedirectConfigIdAdd(L7_uint32 aclnum, L7_uint32 rulenum, nimConfigID_t *configId);

/*********************************************************************
*
* @purpose  To set the mirror interface config id for an access list rule.
*
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *configId  the mirror interface config id
*                                            
* @returns  L7_SUCCESS, if mirror interface is added
* @returns  L7_FAILURE, if invalid mirror interface or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleMirrorConfigIdAdd(L7_uint32 aclnum, L7_uint32 rulenum, nimConfigID_t *configId);

/*********************************************************************
*
* @purpose  To set the logging flag for an access list rule.
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  logFlag   rule logging flag value         
*
* @returns  L7_SUCCESS, if logging flag is added
* @returns  L7_FAILURE, if invalid flag value or other failure
* @returns  L7_ERROR, if the access-list or rule does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleLoggingAdd(L7_uint32 aclnum, L7_uint32 rulenum, L7_BOOL logFlag);

/*********************************************************************
*
* @purpose  To add a protocol to an existing access list entry.
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  protocol  the protocol number
*
* @returns  L7_SUCCESS, if the protocol is added
* @returns  L7_FAILURE, if invalid list number, rulenum, or protocol
* @returns  L7_ERROR, if the access-list or rulenum does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleProtocolAdd(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 protocol);

/*********************************************************************
*
* @purpose  To add the source ip address and net mask.
*
* @param    L7_uint32  val      the access-list identifying number
* @param    L7_uint32  entryId  the access-list entry id
* @param    L7_uint32  ip       the ip address
* @param    L7_uint32  mask     the net mask
*
* @returns  L7_SUCCESS, if source ip address and net mask are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleSrcIpMaskAdd(L7_uint32 aclnum, L7_uint32 rulenum, 
                                   L7_uint32 ipAddr, L7_uint32 mask);

/*********************************************************************
*
* @purpose  To add the source IPv6 address and prefix length.
*
* @param    L7_uint32       aclnum   the access-list identifying number
* @param    L7_uint32       rulenum  the acl rule number
* @param    L7_in6_prefix_t *addr6   ptr to ipv6 address and prefix length
*
* @returns  L7_SUCCESS, if source ipv6 address and prefix length are added
* @returns  L7_FAILURE, if invalid list number, entry id, or other
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleSrcIpv6AddrAdd(L7_uint32 aclnum, L7_uint32 rulenum, 
                                     L7_in6_prefix_t *addr6);

/*********************************************************************
*
* @purpose  To add the source port.
*
* @param    L7_uint32  val      the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  port     the ip address
*
* @returns  L7_SUCCESS, if source ip address and net mask are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleSrcL4PortAdd(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 port);

/*********************************************************************
*
* @purpose  To add the range of the source layer 4 ports to a rule
*           in an existing access list.
*
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  startport  the start port number
* @param    L7_uint32  endport    the end port number
*
* @returns  L7_SUCCESS, if the port range is added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleSrcL4PortRangeAdd(L7_uint32 aclnum, L7_uint32 rulenum, 
                                        L7_uint32 startport, L7_uint32 endport);

/*********************************************************************
*
* @purpose  To add the destination ip address and mask.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  ipAddr   the ip address
* @param    L7_uint32  mask     the net mask
*
* @returns  L7_SUCCESS, if source ip address and net mask are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleDstIpMaskAdd(L7_uint32 aclnum, L7_uint32 rulenum, 
                                   L7_uint32 ipAddr, L7_uint32 mask);

/*********************************************************************
*
* @purpose  To add the destination IPv6 address and prefix length.
*
* @param    L7_uint32       aclnum   the access-list identifying number
* @param    L7_uint32       rulenum  the acl rule number
* @param    L7_in6_prefix_t *addr6   ptr to ipv6 address and prefix length
*
* @returns  L7_SUCCESS, if destination ipv6 address and prefix length are added
* @returns  L7_FAILURE, if invalid list number, entry id, or other
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleDstIpv6AddrAdd(L7_uint32 aclnum, L7_uint32 rulenum, 
                                     L7_in6_prefix_t *addr6);

/*********************************************************************
*
* @purpose  To add the destination port.
*
* @param    L7_uint32  val      the access-list identifying number
* @param    L7_uint32  entryId  the access-list entry id
* @param    L7_uint32  port     the ip address
*
* @returns  L7_SUCCESS, if destination ip address, net mask are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleDstL4PortAdd(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 port);

/*********************************************************************
*
* @purpose  To add the range of the source layer 4 ports to a rule
*           in an existing access list.
*
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  startport  the start port number
* @param    L7_uint32  endport    the end port number
*
* @returns  L7_SUCCESS, if the port range is added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleDstL4PortRangeAdd(L7_uint32 aclnum, L7_uint32 rulenum, 
                                        L7_uint32 startport, L7_uint32 endport);

/*********************************************************************
*
* @purpose  Set the match condition whereby all packets match for an
*           ACL rule
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the rule number
* @param    L7_BOOL    match    the match condition (L7_TRUE/L7_FALSE)
*
* @returns  L7_SUCCESS, if
* @returns  L7_FAILURE, if invalid list or rule number
* @returns  L7_ERROR, if the access-list or rule does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments This routine sets the range of all the filtering criteria
*           in a rule to the maximum, in an existing access list.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleEveryAdd(L7_uint32 aclnum, L7_uint32 rulenum,
                               L7_BOOL match);

/*********************************************************************
*
* @purpose  To add the differentiated services code point (dscp) value
*           to a rule in an existing access list.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  dscpval  the diffserv code point value
*
* @returns  L7_SUCCESS, if
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleIPDscpAdd(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 dscpval);

/*********************************************************************
*
* @purpose  To add the precedence value to a rule in an existing
*           access list.
*
* @param    L7_uint32  aclnum         access-list identifying number
* @param    L7_uint32  rulenum        the acl rule number
* @param    L7_uint32  precedenceval  the precedence value
*
* @returns  L7_SUCCESS, if the precedence value is added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleIPPrecedenceAdd(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 precedenceval);

/*********************************************************************
*
* @purpose  To add the type of service bits and mask.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  tosbits  tos bits
* @param    L7_uint32  tosmask  tos mask
*
* @returns  L7_SUCCESS, if the service type parameters are added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleIPTosAdd(L7_uint32 aclnum, L7_uint32 rulenum, 
                               L7_uint32 tosbits, L7_uint32 tosmask);

/*********************************************************************
*
* @purpose  To add the IPv6 flow label.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  flowlbl  flow label value
*
* @returns  L7_SUCCESS, if the value is added
* @returns  L7_FAILURE, if invalid list number, entry id, or protocol
* @returns  L7_ERROR, if the access-list or entry id does not exist
* @returns  L7_REQUEST_DENIED, if hardware update failed
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleIpv6FlowLabelAdd(L7_uint32 aclnum, L7_uint32 rulenum, 
                                       L7_uint32 flowlbl);

/*********************************************************************
*
* @purpose  To add an access list to the specified interface and direction.
*
* @param    L7_uint32  intIfNum   the internal interface number
* @param    L7_uint32  direction  the direction of application of ACL
*                                 (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
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
extern L7_RC_t aclInterfaceDirectionAdd(L7_uint32 intIfNum, L7_uint32 direction, 
                                        L7_uint32 aclnum, L7_uint32 seqNum);

/*********************************************************************
*
* @purpose  To remove an access list from the specified interface and direction.
*
* @param    L7_uint32  intIfNum   the internal interface number
* @param    L7_uint32  direction  the direction of application of ACL
*                                 (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    L7_uint32  aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if ACL successfully removed from intf,dir
* @returns  L7_FAILURE, if invalid parameter, or other failure
* @returns  L7_ERROR, if ACL does not exist for this aclnum
* @returns  L7_REQUEST_DENIED, if error removing ACL from hardware
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclInterfaceDirectionRemove(L7_uint32 intIfNum, L7_uint32 direction, L7_uint32 aclnum);


/*********************************************************************
*
* @purpose  Check if the specified interface and direction is valid
*           for use with ACL
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    direction   @b{(input)}  interface direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
*
* @returns  L7_SUCCESS  intf,dir is valid for ACL
* @returns  L7_ERROR    intf,dir not valid for ACL
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments The interface type and direction supported for ACL use   
*           depends on the capabilities of the platform.
*
* @comments Note that this API only indicates whether the interface
*           and direction are usable for ACLs.  It does not indicate
*           whether any ACLs are currently assigned to the interface
*           and direction (see aclIsInterfaceInUse).
*
* @end
*
*********************************************************************/
extern L7_RC_t aclIntfDirGet(L7_uint32 intIfNum, L7_uint32 direction);

/*********************************************************************
*
* @purpose  Get next sequential interface and direction that is valid
*           for use with ACL
*
* @param    intIfNum        @b{(input)}  internal interface number
* @param    direction       @b{(input)}  interface direction
*                                        (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    *nextIntIfNum   @b{(output)} next internal interface number
* @param    *nextDirection  @b{(output)} next interface direction
*                                        (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
*
* @returns  L7_SUCCESS  next intf,dir for ACL was obtained
* @returns  L7_ERROR    no more valid intf,dir for ACL
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments The interface type and direction supported for ACL use   
*           depends on the capabilities of the platform.
*
* @comments Note that this API only provides an interface and direction
*           that is usable for ACLs.  It does not indicate whether
*           any ACLs are currently assigned to the interface
*           and direction (see aclIsInterfaceInUse).
*
* @end
*
*********************************************************************/
extern L7_RC_t aclIntfDirGetNext(L7_uint32 intIfNum, L7_uint32 direction,
                                 L7_uint32 *nextIntIfNum, L7_uint32 *nextDirection);

/*********************************************************************
*
* @purpose  Verifies that the ACL sequence number is in use for the  
*           specified interface and direction
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    direction   @b{(input)}  interface direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    seqNum      @b{(input)}  ACL interface sequence number
*
* @returns  L7_SUCCESS  sequence number exists for intf,dir
* @returns  L7_ERROR    sequence number does not exist for intf,dir
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments A sequence number value may be used only once for a given
*           interface and direction pair.  
*
* @end
*
*********************************************************************/
extern L7_RC_t aclIntfDirSequenceGet(L7_uint32 intIfNum, L7_uint32 direction, 
                                     L7_uint32 seqNum);

/*********************************************************************
*
* @purpose  Get next sequential ACL interface sequence number in use 
*           for the specified interface and direction.
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    direction   @b{(input)}  interface direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    seqNum      @b{(input)}  ACL interface sequence number
* @param    *nextSeqNum @b{(output)} next ACL interface sequence number
*
* @returns  L7_SUCCESS  next sequence number was obtained
* @returns  L7_ERROR    no more sequence numbers for intf,dir
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments Use a seqNum value of 0 to find the first ACL sequence   
*           number for this interface and direction, if it exists.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclIntfDirSequenceGetNext(L7_uint32 intIfNum, L7_uint32 direction, 
                                         L7_uint32 seqNum, L7_uint32 *nextSeqNum);

/*********************************************************************
*
* @purpose  Get the first access list rule given an ACL ID
*
* @param    L7_uint32   aclnum   the access-list identifying number
* @param    L7_uint32   *next    the next rule ID
*
* @returns  L7_SUCCESS, if the first rule for this ACL ID was found
* @returns  L7_FAILURE, 
* @returns  L7_ERROR,   if no rules have been created for this ACL
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleGetFirst(L7_uint32 aclnum, L7_uint32 *next);

/*********************************************************************
*
* @purpose  Given aclnum, get the next access list ID
*
* @param    L7_uint32   aclnum   the access-list ID
* @param    L7_uint32   rule     the current rule ID
* @param    L7_uint32  *next     the next rule ID
*
* @returns  L7_SUCCESS, if the next rule ID was found
* @returns  L7_FAILURE, if rule is the last valid rule ID for this ACL
* @returns  L7_ERROR,   if aclnum or rule does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleGetNext(L7_uint32 aclnum, L7_uint32 rule, L7_uint32 *next);

/*********************************************************************
*
* @purpose  Checks if aclnum is a valid, configured access list
*
* @param    L7_uint32  aclnum   the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list was found
* @returns  L7_FAILURE, if the access list does not exist 
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleCheckValid(L7_uint32 aclnum, L7_uint32 rulenum);

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
* @comments The aclnum parameter is required by this function in case there 
*           is ever a need to support separate rule number ranges based
*           on the type of ACL (i.e., standard vs. extended).
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleNumRangeCheck(L7_uint32 aclnum, L7_uint32 rulenum);

/*********************************************************************
* @purpose  Determine if the interface type is valid for ACL
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
extern L7_BOOL aclIsValidIntfType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Determine if the interface is valid for ACL
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
extern L7_BOOL aclIsValidIntf(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Checks if aclnum is currently assigned to this intf,dir pair
*
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  intf       the interface number
* @param    L7_uint32  direction  the interface direction
*
* @returns  L7_SUCCESS, if the ACL is assigned to this intf,dir
* @returns  L7_FAILURE, if the ACL is not assigned to this intf,dir
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclIntfDirCheckValid(L7_uint32 aclnum, L7_uint32 intf, L7_uint32 direction);

/*********************************************************************
*
* @purpose Check to see if vlanNum is a valid VLAN ID for the purpose
*          of binding ACLs to it.
*
* @param L7_uint32 vlan       @b((input)) VLAN Number
*
* @returns L7_TRUE     If valid VLAN
* @returns L7_FALSE    If not valid VLAN
*
* @notes none
*
* @end
*
*********************************************************************/
L7_BOOL aclIsValidVlan( L7_uint32 vlanNum );

/*********************************************************************
*
* @purpose  Checks if aclnum is currently assigned to this vlan,dir pair
*
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  vlan       the VLAN number
* @param    L7_uint32  direction  the traffic direction
*
* @returns  L7_SUCCESS, if the ACL is assigned to this vlan,dir
* @returns  L7_FAILURE, if the ACL is not assigned to this vlan,dir
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanDirCheckValid( L7_uint32 aclnum,
                              L7_uint32 vlan,
                              L7_uint32 direction );

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
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_BOOL aclIsFieldConfigured(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 field);

/*********************************************************************
*
* @purpose  To get the action for a rule.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  *action  action type (permit/deny) (aclActionIndex_t)
*
* @returns  L7_SUCCESS, if the access option is retrieved
* @returns  L7_FAILURE, if there is no action set
* @returns  L7_ERROR,   if the access-list or rule does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleActionGet(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 *action);

/*********************************************************************
*
* @purpose  To get the assigned queue id from an existing access list rule entry.
*
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *queueId   the queue id         
*
* @returns  L7_SUCCESS, if the queue id is retrieved
* @returns  L7_FAILURE, if invalid list number or rulenum
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleAssignQueueIdGet(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 *queueId);

/*********************************************************************
*
* @purpose  To get the redirect interface config id from an existing 
*           access list rule entry.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    nimConfigID_t *configId the redirect interface config id
*
* @returns  L7_SUCCESS, if the interface number is retrieved
* @returns  L7_FAILURE, if invalid list number or rulenum
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleRedirectConfigIdGet(L7_uint32 aclnum, L7_uint32 rulenum, nimConfigID_t *configId);

/*********************************************************************
*
* @purpose  To get the mirror interface config id from an existing 
*           access list rule entry.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    nimConfigID_t *configId the mirror interface config id
*
* @returns  L7_SUCCESS, if the interface number is retrieved
* @returns  L7_FAILURE, if invalid list number or rulenum
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleMirrorConfigIdGet(L7_uint32 aclnum, L7_uint32 rulenum, nimConfigID_t *configId);

/*********************************************************************
*
* @purpose  To get the logging flag from an existing access list rule entry.
*
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *logFlag   the rule logging flag value
*
* @returns  L7_SUCCESS, if the logging flag is retrieved
* @returns  L7_FAILURE, if invalid list number or rulenum
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleLoggingGet(L7_uint32 aclnum, L7_uint32 rulenum, L7_BOOL *logFlag);

/*********************************************************************
*
* @purpose  To get a protocol from an existing access list entry.
*
* @param    L7_uint32  aclnum     the access-list identifying number
* @param    L7_uint32  rulenum    the acl rule number
* @param    L7_uint32  *protocol  the protocol number
*
* @returns  L7_SUCCESS, if the protocol is retrieved
* @returns  L7_FAILURE, if invalid list number or rulenum
* @returns  L7_ERROR, if the access-list or rulenum does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleProtocolGet(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 *protocol);

/*********************************************************************
*
* @purpose  To get the source ip address and mask.
*
* @param    L7_uint32  val      the access-list identifying number
* @param    L7_uint32  aclnum   the access-list entry id
* @param    L7_uint32  *ipAddr  the ip address
* @param    L7_uint32  *mask    the ip address mask
*
* @returns  L7_SUCCESS, if source ip address and mask are added
* @returns  L7_FAILURE, if invalid list number or entry id
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleSrcIpMaskGet(L7_uint32 aclnum, L7_uint32 rulenum, 
                                   L7_uint32 *ipAddr, L7_uint32 *mask);

/*********************************************************************
*
* @purpose  To get the source IPv6 address and prefix length.
*
* @param    L7_uint32       aclnum   the access-list identifying number
* @param    L7_uint32       rulenum  the acl rule number
* @param    L7_in6_prefix_t *addr6   ptr to ipv6 address and prefix length
*
* @returns  L7_SUCCESS, if source ipv6 address and prefix len are retrieved
* @returns  L7_FAILURE, if invalid list number or entry id
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleSrcIpv6AddrGet(L7_uint32 aclnum, L7_uint32 rulenum, 
                                     L7_in6_prefix_t *addr6);

/*********************************************************************
*
* @purpose  To get the L4 source port.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  *port    L4 port number
*
* @returns  L7_SUCCESS, if L4 source port is retrieved
* @returns  L7_FAILURE, if invalid list number or entry id 
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleSrcL4PortGet(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 *port);

/*********************************************************************
*
* @purpose  To get the range of the source layer 4 ports of a rule
*           in an existing access list.
*
* @param    L7_uint32  aclnum      the access-list identifying number
* @param    L7_uint32  rulenum     the acl rule number
* @param    L7_uint32  *startport  the start port number
* @param    L7_uint32  *endport    the end port number
*
* @returns  L7_SUCCESS, if the port range is retrieved
* @returns  L7_FAILURE, if invalid list number or entry id
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleSrcL4PortRangeGet(L7_uint32 aclnum, L7_uint32 rulenum, 
                                        L7_uint32 *startport, L7_uint32 *endport);

/*********************************************************************
*
* @purpose  Get the destination ip address and mask.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  *ipAddr  the ip address
* @param    L7_uint32  *mask    the net mask
*
* @returns  L7_SUCCESS, if source ip address and net mask are retrieved
* @returns  L7_FAILURE, if invalid list number, entry id, or port
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleDstIpMaskGet(L7_uint32 aclnum, L7_uint32 rulenum, 
                                   L7_uint32 *ipAddr, L7_uint32 *mask);

/*********************************************************************
*
* @purpose  To get the destination IPv6 address and prefix length.
*
* @param    L7_uint32       aclnum   the access-list identifying number
* @param    L7_uint32       rulenum  the acl rule number
* @param    L7_in6_prefix_t *addr6   ptr to ipv6 address and prefix length
*
* @returns  L7_SUCCESS, if destination ipv6 address and prefix len are retrieved
* @returns  L7_FAILURE, if invalid list number or entry id
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleDstIpv6AddrGet(L7_uint32 aclnum, L7_uint32 rulenum, 
                                     L7_in6_prefix_t *addr6);

/*********************************************************************
*
* @purpose  Get the L4 destination port.
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  *port    the L4 port number
*
* @returns  L7_SUCCESS, if destination ip address, net mask are added
* @returns  L7_FAILURE, if invalid list number, entry id, or ip
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleDstL4PortGet(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 *port);

/*********************************************************************
*
* @purpose  Get the range of the destination layer 4 ports for a rule
*           in an existing access list.
*
* @param    L7_uint32  aclnum      the access-list identifying number
* @param    L7_uint32  rulenum     the acl rule number
* @param    L7_uint32  *startport  the start port number
* @param    L7_uint32  *endport    the end port number
*
* @returns  L7_SUCCESS, if the port range is retrieved
* @returns  L7_FAILURE, if invalid list number, entry id, or port
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleDstL4PortRangeGet(L7_uint32 aclnum, L7_uint32 rulenum, 
                                        L7_uint32 *startport, L7_uint32 *endport);

/*********************************************************************
*
* @purpose  Gets the match-all or 'every' status for a rule
*
* @param    L7_uint32  aclnum   the access-list identifying number
* @param    L7_uint32  rulenum  the acl rule number
* @param    L7_uint32  every    true or false, match every packet
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
extern L7_RC_t aclRuleEveryGet(L7_uint32 aclnum, L7_uint32 rulenum, L7_BOOL *every);

/*********************************************************************
*
* @purpose  To get the differentiated services code point (dscp) value
*           for a rule in an existing access list.
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  *dscpval  the diffserv code point value
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
extern L7_RC_t aclRuleIPDscpGet(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 *dscpval);

/*********************************************************************
*
* @purpose  To get the precedence value for a rule in an existing
*           access list.
*
* @param    L7_uint32  aclnum          access-list identifying number
* @param    L7_uint32  rulenum         the acl rule number
* @param    L7_uint32  *precedenceval  the precedence value
*
* @returns  L7_SUCCESS, if the precedence value is retrieved
* @returns  L7_FAILURE, if invalid list number, entry id
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleIPPrecedenceGet(L7_uint32 aclnum, L7_uint32 rulenum, L7_uint32 *precedenceval);

/*********************************************************************
*
* @purpose  To get the type of service bits and mask.
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  *tosbits  tos bits
* @param    L7_uint32  *tosmask  tos mask
*
* @returns  L7_SUCCESS, if the service type parameters are added
* @returns  L7_FAILURE, if invalid list number, entry id
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleIPTosGet(L7_uint32 aclnum, L7_uint32 rulenum, 
                               L7_uint32 *tosbits, L7_uint32 *tosmask);

/*********************************************************************
*
* @purpose  To get the IPv6 flow label.
*
* @param    L7_uint32  aclnum    the access-list identifying number
* @param    L7_uint32  rulenum   the acl rule number
* @param    L7_uint32  *flowlbl  flow label value
*
* @returns  L7_SUCCESS, if the value is retrieved
* @returns  L7_FAILURE, if invalid list number, entry id
* @returns  L7_ERROR, if the access-list or entry id does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclRuleIpv6FlowLabelGet(L7_uint32 aclnum, L7_uint32 rulenum, 
                                       L7_uint32 *flowlbl);

/*********************************************************************
*
* @purpose  To get the interface and the access list application
*           direction for an existing access list.
*
* @param    L7_uint32  aclnum      the access-list identifying number
* @param    L7_uint32  *direction  the direction of application of acl,
*                                  see aclBoundTypes_t for the list
*
* @returns  L7_SUCCESS, if intIfNum is added to the access-list
* @returns  L7_FAILURE, if invalid identifying number or 
*                       internal interface number does not exist
* @returns  L7_ERROR, if intIfNum is in use by another access-list
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t aclInterfaceDirectionGet(L7_uint32 aclnum, L7_uint32 *direction);

/*********************************************************************
*
* @purpose  Gets the list of ACLs currently assigned to the specified
*           interface and direction
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    direction   @b{(input)}  interface direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    *listInfo   @b{(output)} list of assigned ACLs
*
* @returns  L7_SUCCESS  list of ACLs was built
* @returns  L7_FAILURE  invalid interface, direction, or other failure
* @returns  L7_ERROR    no access list assigned to the interface in this 
*                       direction
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
extern L7_RC_t aclIntfDirAclListGet(L7_uint32 intIfNum, L7_uint32 direction,
                                    L7_ACL_INTF_DIR_LIST_t *listInfo);

/*********************************************************************
*
* @purpose  Gets a list of interfaces to which an ACL is assigned in the
*           specified direction
*
* @param    L7_uint32  aclnum      the access-list identifying number
* @param    L7_uint32  direction   the direction of application of ACL
*                                   (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
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
extern L7_RC_t aclAssignedIntfDirListGet(L7_uint32 aclnum, L7_uint32 direction,
                                         L7_ACL_ASSIGNED_INTF_LIST_t *intfList);

/*********************************************************************
*
* @purpose  To check if an interface and direction is in use by any access list.
*
* @param    L7_uint32  intIfNum   the internal interface number
* @param    L7_uint32  direction  the direction of application of ACL
*                                 (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
*
* @returns  L7_TRUE, if the intf,dir has at least one ACL attached
* @returns  L7_FALSE, if the intf,dir has no ACL attached
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_BOOL aclIsInterfaceInUse(L7_uint32 intIfNum, L7_uint32 direction);

/*********************************************************************
*
* @purpose  To add an access list to the specified vlan and direction.
*
* @param    L7_uint32  vlanNum    the internal vlan number
* @param    L7_uint32  direction  the direction of application of ACL
*                                 (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
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
L7_RC_t aclVlanDirectionAdd( L7_uint32 vlanNum,
                             L7_uint32 direction,
                             L7_uint32 aclnum,
                             L7_uint32 seqNum );

/*********************************************************************
*
* @purpose  To remove an vlan and the access list application
*           direction to an existing access list.
*
* @param    L7_uint32  vlanNum    the internal vlan number
* @param    L7_uint32  direction  the direction of application of ACL
*                                 (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    L7_uint32  aclnum     the access-list identifying number
*
* @returns  L7_SUCCESS, if ACL successfully removed from vlan,dir
* @returns  L7_FAILURE, if invalid parameter, or other failure
* @returns  L7_ERROR, if ACL does not exist for this aclnum
* @returns  L7_REQUEST_DENIED, if error removing ACL from hardware
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanDirectionRemove( L7_uint32 vlanNum,
                                L7_uint32 direction,
                                L7_uint32 aclnum );

/*********************************************************************
*
* @purpose  Verifies that the specified vlan and direction is valid
*           for use with ACL
*
* @param    vlanNum    @b{(input)}   interal vlan number
* @param    direction   @b{(input)}  vlan direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
*
* @returns  L7_SUCCESS  vlan,dir is valid for ACL
* @returns  L7_ERROR    vlan,dir not valid for ACL
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments The vlan type and direction supported for ACL use
*           depends on the capabilities of the platform.
*
* @comments Note that this API only indicates whether the vlan
*           and direction are usable for ACLs.  It does not indicate
*           whether any ACLs are currently assigned to the vlan
*           and direction (see usmDbQosAclIsVlanInUse).
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanDirGet( L7_uint32 vlanNum,
                       L7_uint32 direction );

/*********************************************************************
*
* @purpose  Get next sequential vlan and direction that is valid
*           for use with ACL
*
* @param    vlanNum         @b{(input)}  interal vlan number
* @param    direction       @b{(input)}  vlan direction
*                                        (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    *nextVlanNum    @b{(output)} next interal vlan number
* @param    *nextDirection  @b{(output)} next vlan direction (internal)
*
* @returns  L7_SUCCESS  next vlan,dir for ACL was obtained
* @returns  L7_ERROR    no more valid vlan,dir for ACL
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments The vlan type and direction supported for ACL use
*           depends on the capabilities of the platform.
*
* @comments Note that this API only provides an vlan and direction
*           that is usable for ACLs.  It does not indicate whether
*           any ACLs are currently assigned to the vlan
*           and direction (see usmDbQosAclIsVlanInUse).
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanDirGetNext( L7_uint32 vlanNum,
                           L7_uint32 direction,
                           L7_uint32 *nextVlanNum,
                           L7_uint32 *nextDirection );

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
L7_RC_t aclVlanIdGetNext(L7_uint32 vlanNum, L7_uint32 *nextVlanNum);

/*********************************************************************
*
* @purpose  Verifies that the ACL sequence number is in use for the
*           specified vlan and direction.
*
* @param    vlanNum    @b{(input)}  interal vlan number
* @param    direction   @b{(input)}  vlan direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    seqNum      @b{(input)}  ACL vlan sequence number
*
* @returns  L7_SUCCESS  sequence number exists for vlan,dir
* @returns  L7_ERROR    sequence number does not exist for vlan,dir
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments A sequence number value may be used only once for a given
*           vlan and direction pair.
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanDirSequenceGet( L7_uint32 vlanNum,
                               L7_uint32 direction,
                               L7_uint32 seqNum );

/*********************************************************************
*
* @purpose  Get next sequential ACL vlan sequence number in use
*           for the specified vlan and direction.
*
* @param    vlanNum     @b{(input)}  interal vlan number
* @param    direction   @b{(input)}  vlan direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    seqNum      @b{(input)}  ACL vlan sequence number
* @param    *nextSeqNum @b{(output)} next ACL vlan sequence number
*
* @returns  L7_SUCCESS  next sequence number was obtained
* @returns  L7_ERROR    no more sequence numbers for vlan,dir
* @returns  L7_FAILURE  invalid parameter or other failure
*
* @comments Use a seqNum value of 0 to find the first ACL sequence
*           number for this vlan and direction, if it exists.
*
* @end
*
*********************************************************************/
L7_RC_t aclVlanDirSequenceGetNext( L7_uint32 vlanNum,
                                   L7_uint32 direction,
                                   L7_uint32 seqNum,
                                   L7_uint32 *nextSeqNum );

/*********************************************************************
*
* @purpose  Gets the list of ACLs currently assigned to the specified
*           vlan and direction
*
* @param    vlanNum     @b{(input)}  interal vlan number
* @param    direction   @b{(input)}  vlan direction
*                                    (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    *listInfo   @b{(output)} list of assigned ACLs
*
* @returns  L7_SUCCESS  list of ACLs was built
* @returns  L7_FAILURE  invalid vlan, direction, or other failure
* @returns  L7_ERROR    no access list assigned to the vlan in this
*                       direction
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
L7_RC_t aclVlanDirAclListGet( L7_uint32 vlanNum,
                              L7_uint32 direction,
                              L7_ACL_VLAN_DIR_LIST_t *listInfo );

/*********************************************************************
*
* @purpose  Gets a list of vlans to which an ACL is assigned in the
*           specified direction
*
* @param    L7_uint32  aclnum      the access-list identifying number
* @param    L7_uint32  direction   the direction of application of ACL
*                                   (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
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
L7_RC_t aclAssignedVlanDirListGet( L7_uint32 aclnum,
                                   L7_uint32 direction,
                                   L7_ACL_ASSIGNED_VLAN_LIST_t *vlanList );

/*********************************************************************
*
* @purpose  To check if an vlan and direction is in use by any access list.
*
* @param    L7_uint32  vlanNum    the internal vlan number
* @param    L7_uint32  direction  the direction of application of ACL
*                                 (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
*
* @returns  L7_TRUE, if the vlan,dir has at least one ACL attached
* @returns  L7_FALSE, if the vlan,dir has no ACL attached
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_BOOL aclIsVlanInUse( L7_uint32 vlanNum,
                        L7_uint32 direction );

/*********************************************************************
* @purpose  Check if the specified Internal Interface Number is in
*           use by the DiffServ component
*
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    direction  @b{(input)} Interface direction
*                                  (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments None
*       
* @end
*********************************************************************/
extern L7_BOOL aclIsDiffServIntfInUse(L7_uint32 intIfNum, L7_uint32 direction);

/*************************************************************************
* @purpose  Get the maximum number of ACLs that can be configured
*
* @param    pMax  @b{(output)} Pointer to the maximum value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This value encompasses all ACL types.
*
* @end
*********************************************************************/
extern L7_RC_t aclMaxNumGet(L7_uint32 *pMax);

/*************************************************************************
* @purpose  Get the current number of ACLs configured
*
* @param    pCurr  @b{(output)} Pointer to the current value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This value encompasses all ACL types.
*
* @end
*********************************************************************/
extern L7_RC_t aclCurrNumGet(L7_uint32 *pCurr);

/*********************************************************************
*
* @purpose  Creates a single TLV containing active rules of the specified
*           ACL within it and return the data in TLV block storage
*           provided by the caller.
*
* @param    aclnum      @b{(input)}  access-list identifying number
* @param    tlvHandle   @b{(input)}  TLV block handle (supplied by caller)
*
* @returns  L7_SUCCESS  TLV successfully built
* @returns  L7_ERROR    Access list does not exist
* @returns  L7_FAILURE  All other failures
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t aclTlvGet(L7_uint32 aclnum, L7_tlvHandle_t tlvHandle);

/*************************************************************************
* @purpose  Register a route filter callback function to be called when there is a 
*           configuration change to an ACL. 
*
* @param    funcPtr     @b{(input)} pointer to the callback function
* @param    compId      @b{(input)} component ID
* @param    displayStr  @b{(input)} display string (for debugging), or L7_NULLPTR
*
* @returns  L7_SUCCESS          registration succeeded
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE          function pointer bad, or maximum callbacks already registered
*
* @comments The displayStr parm can be used to specify information to be used
*           for debugging, such as the name of the callback function being registered
*           by the caller.  This is optional and is set to L7_NULLPTR if not specified.
*
* @end
*********************************************************************/
L7_RC_t aclRouteFilterCallbackRegister(ACL_ROUTE_FILTER_FUNCPTR_t funcPtr,
                                       L7_COMPONENT_IDS_t compId,
                                       L7_uchar8 *displayStr);

/*************************************************************************
* @purpose  Remove a route filter callback function from the list of functions to
*           be called when an ACL changes. 
*
* @param    funcPtr  @b{(input)} pointer to callback function to unregister
*
* @returns  L7_SUCCESS          registration succeeded
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE          function pointer NULL or not registered
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t aclRouteFilterCallbackUnregister(ACL_ROUTE_FILTER_FUNCPTR_t funcPtr);

/*************************************************************************
* @purpose  Indicate whether a route filter callback function is already registered
*           for ACL changes.
*
* @param    funcPtr     @b{(input)} pointer to the callback function
*
* @returns  L7_TRUE     already registered
* @returns  L7_FALSE    not registered
*
* @comments 
*
* @end
*********************************************************************/
L7_BOOL aclRouteFilterCallbackIsRegistered(ACL_ROUTE_FILTER_FUNCPTR_t funcPtr);

/*************************************************************************
* @purpose  Display the contents of the route filter callback table.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t aclRouteFilterCallbackTableShow(void);

/*************************************************************************
* @purpose  Register a notify event callback function to be called 
*           whenever there is a configuration change to an ACL. 
*
* @param    funcPtr     @b{(input)} pointer to the callback function
* @param    compId      @b{(input)} component ID
* @param    displayStr  @b{(input)} display string (for debugging), or L7_NULLPTR
*
* @returns  L7_SUCCESS          registration succeeded
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE          function pointer bad, or maximum callbacks already registered
*
* @comments The displayStr parm can be used to specify information to be used
*           for debugging, such as the name of the callback function being registered
*           by the caller.  This is optional and is set to L7_NULLPTR if not specified.
*
*
* @end
*********************************************************************/
L7_RC_t aclNotifyEventCallbackRegister(ACL_NOTIFY_EVENT_FUNCPTR_t funcPtr,
                                       L7_COMPONENT_IDS_t compId,
                                       L7_uchar8 *displayStr);

/*************************************************************************
* @purpose  Remove a notify event callback function from the registered list.
*
* @param    funcPtr  @b{(input)} pointer to callback function to unregister
*
* @returns  L7_SUCCESS          registration succeeded
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE          function pointer NULL or not registered
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t aclNotifyEventCallbackUnregister(ACL_NOTIFY_EVENT_FUNCPTR_t funcPtr);

/*************************************************************************
* @purpose  Indicate whether a notify event callback function is already 
*           registered for ACL changes.
*
* @param    funcPtr     @b{(input)} pointer to the callback function
*
* @returns  L7_TRUE     already registered
* @returns  L7_FALSE    not registered
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t aclNotifyEventCallbackIsRegistered(ACL_NOTIFY_EVENT_FUNCPTR_t funcPtr);

/*************************************************************************
* @purpose  Display the contents of the notify event callback table.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t aclNotifyEventCallbackTableShow(void);

/*************************************************************************
* @purpose  Register a delete approval callback function to be called 
*           prior to deleting an ACL.
*
* @param    funcPtr     @b{(input)} pointer to the callback function
* @param    compId      @b{(input)} component ID
* @param    displayStr  @b{(input)} display string (for debugging), or L7_NULLPTR
*
* @returns  L7_SUCCESS          registration succeeded
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE          function pointer bad, or maximum callbacks already registered
*
* @comments The displayStr parm can be used to specify information to be used
*           for debugging, such as the name of the callback function being registered
*           by the caller.  This is optional and is set to L7_NULLPTR if not specified.
*
*
* @end
*********************************************************************/
L7_RC_t aclDeleteApprovalCallbackRegister(ACL_DELETE_APPROVAL_FUNCPTR_t funcPtr,
                                          L7_COMPONENT_IDS_t compId,
                                          L7_uchar8 *displayStr);

/*************************************************************************
* @purpose  Remove a delete approval callback function from the registered list.
*
* @param    funcPtr  @b{(input)} pointer to callback function to unregister
*
* @returns  L7_SUCCESS          registration succeeded
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE          function pointer NULL or not registered
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t aclDeleteApprovalCallbackUnregister(ACL_DELETE_APPROVAL_FUNCPTR_t funcPtr);

/*************************************************************************
* @purpose  Indicate whether a delete approval callback function is already 
*           registered for ACL changes.
*
* @param    funcPtr     @b{(input)} pointer to the callback function
*
* @returns  L7_TRUE     already registered
* @returns  L7_FALSE    not registered
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t aclDeleteApprovalCallbackIsRegistered(ACL_DELETE_APPROVAL_FUNCPTR_t funcPtr);

/*************************************************************************
* @purpose  Display the contents of the delete approval callback table.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t aclDeleteApprovalCallbackTableShow(void);

/*************************************************************************
* @purpose  Display the contents of all supported ACL callback tables.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t aclCallbackTableShow(void);

/*************************************************************************
* @purpose  Apply an ACL as a route filter. 
*
* @param    aclnum  @b{(input)} number of access list to filter route
* @param    routePrefix  @b{(input)} Destination prefix of route
* @param    routeMask  @b{(input)} Destination mask of route
* @param    action  @b{(output)} FILTER_ACTION_PERMIT if the route passes the filter
*                                FILTER_ACTION_DENY if the route does not pass
*
* @returns  L7_SUCCESS
*
* @comments If no ACL exists with the given aclnum, action is set to permit.
*           Rules that do not have a srcIp and srcMask field configured are
*           skipped. If a rule has destIp and dstMask configured, those fields
*           are applied in addition to srcIp and srcMask. All other fields are
*           ignored. The action is set to that of the first matching rule. 
*           If no rules are configured, the action is set to deny.
*
* @end
*********************************************************************/
extern L7_RC_t aclRouteFilter(L7_uint32 aclnum,
                              L7_uint32 routePrefix,
                              L7_uint32 routeMask,
                              FILTER_ACTION_t *action);

/*********************************************************************
*
* @purpose  Callback from DTL informing about an IP Deny Rule match
*
* @param    aclnum @{(input)} ACL index
* @param    rulenum @{(input)} rule index
* @param    count @{(input)} number of matches on ACL IP rule
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Driver will not call back unless a minute or more has elasped 
* @notes    since last rule match.
*
* @end
*********************************************************************/
L7_RC_t aclIpRuleDenyTrapCallBack(L7_uint32 aclnum,
                                  L7_uint32 rulenum,
                                  L7_ulong64 count);


/*** MAC ACL prototypes ***/

/*************************************************************************
* @purpose  Obtain the next available index value to be used when creating  
*           a new MAC ACL in the access list table
*
* @param    *next       @b{(output)} access list index next value
*
* @returns  L7_SUCCESS
* @returns  L7_TABLE_IS_FULL  ACL table is currently full
* @returns  L7_FAILURE        all other failures
*
* @comments The index value remains free until used in a subsequent ACL create
*           request.  When two or more requestors get the same index value,
*           only the first one to successfully create a new ACL
*           gets to keep it.  All other requestors' create requests will
*           fail due to the index being in use, so they must call this
*           function again to get a new value.
*
* @comments Although an index value is considered in use once a
*           successful row create occurs, the next index value is not
*           revealed until the row is either completed or deleted.
*
* @end
*********************************************************************/
L7_RC_t aclMacIndexNext(L7_uint32 *next);

/*************************************************************************
*
* @purpose  Retrieve the min/max index values allowed for the MAC ACL table 
*
* @param    *pMin       @b{(output)} Pointer to index min output location
* @param    *pMax       @b{(output)} Pointer to index max output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is purely informational.  The aclMacIndexNext function
*           must be used to obtain the current index value.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacIndexMinMaxGet(L7_uint32 *pMin, L7_uint32 *pMax);

/*********************************************************************
*
* @purpose  To create a new MAC access list.
*
* @param    aclIndex    @b{(input)} access list index
*
* @returns  L7_SUCCESS        access list created
* @returns  L7_FAILURE        invalid acl identifier, all other failures
* @returns  L7_ERROR          access list already exists
* @returns  L7_TABLE_IS_FULL  maximum number of ACLs already created
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacCreate(L7_uint32 aclIndex);

/*********************************************************************
*
* @purpose  To delete an existing MAC access list.
*
* @param    aclIndex    @b{(input)} access list index
* @param    force       @b{(input)} force deletion regardless of whether in use
*
* @returns  L7_SUCCESS  access list deleted
* @returns  L7_ERROR    access list does not exist
* @returns  L7_FAILURE  invalid access list index, all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacDelete(L7_uint32 aclIndex, L7_BOOL force);

/*********************************************************************
*
* @purpose  To add the name to this MAC access list.
*
* @param    aclIndex    @b{(input)} access list index
* @param    *name       @b{(input)} access list name (1 to L7_ACL_NAME_LEN_MAX chars)
*                                            
* @returns  L7_SUCCESS  name is added
* @returns  L7_FAILURE  invalid access list index, invalid name, or other failure
* @returns  L7_ERROR    access list does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacNameAdd(L7_uint32 aclIndex, L7_uchar8 *name);

/*********************************************************************
*
* @purpose  To change the name of an existing MAC access list.
*
* @param    aclIndex    @b{(input)} access list index
* @param    *newname    @b{(input)} new access list name (1 to L7_ACL_NAME_LEN_MAX chars)
*                                            
* @returns  L7_SUCCESS  name is added
* @returns  L7_FAILURE  invalid access list index, invalid name, or other failure
* @returns  L7_ERROR    access list does not exist
* @returns  L7_ALREAD_CONFIGURED  name is in use by another ACL
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacNameChange(L7_uint32 aclIndex, L7_uchar8 *newname);

/*********************************************************************
*
* @purpose  To get the name of this MAC access list.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    *name       @b{(output)} access list name (1 to L7_ACL_NAME_LEN_MAX chars)
*                                            
* @returns  L7_SUCCESS  name is retrieved
* @returns  L7_FAILURE  invalid access list index, name ptr, or other failure
* @returns  L7_ERROR    access list does not exist
*
* @comments Caller must provide a name buffer of at least 
*           (L7_ACL_NAME_LEN_MAX+1) characters.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacNameGet(L7_uint32 aclIndex, L7_uchar8 *name);

/*********************************************************************
*
* @purpose  Checks if the MAC ACL name is a valid string
*
* @param    *name       @b{(input)} MAC access-list name string
*
* @returns  L7_SUCCESS  valid MAC ACL name string
* @returns  L7_FAILURE  invalid MAC ACL name string
*
* @comments This function only checks the name string syntax for a MAC ACL. 
*           It does not check if an MAC ACL currently contains this name.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacNameStringCheck(L7_uchar8 *name);

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
L7_RC_t aclMacNameToIndex(L7_uchar8 *name, L7_uint32 *aclIndex);

/*********************************************************************
*
* @purpose  To add an action to a MAC access list rule.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    action      @b{(input)} action type (permit/deny)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  invalid acl identifier, all other failures
* @returns  L7_ERROR    access-list does not exist
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleActionAdd(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 action);

/*********************************************************************
*
* @purpose  To remove a rule from an existing access list.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
*                                            
* @returns  L7_SUCCESS
* @returns  L7_ERROR    access-list or rule number does not exist
* @returns  L7_FAILURE  invalid access-list index or rule number
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleRemove(L7_uint32 aclIndex, L7_uint32 rulenum);

/*********************************************************************
*
* @purpose  To add the assigned queue id for an access list rule.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    queueId     @b{(input)} assigned queue ID value
*
* @returns  L7_SUCCESS  assigned queue id added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleAssignQueueIdAdd(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 queueId);

/*********************************************************************
*
* @purpose  To add the redirect interface config id for an access list rule.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    *configId   @b{(input)} redirected interface config id
*                                            
* @returns  L7_SUCCESS  redirect interface config id added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  invalid redirect interface or other failure
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleRedirectConfigIdAdd(L7_uint32 aclIndex, L7_uint32 rulenum, nimConfigID_t *configId);

/*********************************************************************
*
* @purpose  To add the mirror interface config id for an access list rule.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    *configId   @b{(input)} mirrored interface config id
*                                            
* @returns  L7_SUCCESS  mirror interface config id added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  invalid mirror interface or other failure
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleMirrorConfigIdAdd(L7_uint32 aclIndex, L7_uint32 rulenum, nimConfigID_t *configId);

/*********************************************************************
*
* @purpose  To add the logging flag for an access list rule.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    logFlag     @b{(input)} rule logging flag value
*
* @returns  L7_SUCCESS  logging flag is added
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid flag value or other failure
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleLoggingAdd(L7_uint32 aclIndex, L7_uint32 rulenum, L7_BOOL logFlag);

/*********************************************************************
*
* @purpose  To add the class of service (cos) value.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    cos         @b{(input)} class of service (cos) value
*
* @returns  L7_SUCCESS  class of service value added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleCosAdd(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 cos);

/*********************************************************************
*
* @purpose  To add the secondary class of service (cos2) value.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    cos         @b{(input)} secondary class of service (cos2) value
*
* @returns  L7_SUCCESS  secondary class of service value added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleCos2Add(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 cos2);

/*********************************************************************
*
* @purpose  To add the destination MAC address and mask.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    dstmac      @b{(input)} destination MAC address
* @param    dstmask     @b{(input)} destination MAC mask
*
* @returns  L7_SUCCESS  destination MAC added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleDstMacAdd(L7_uint32 aclIndex, L7_uint32 rulenum,
                            L7_uchar8 *dstmac, L7_uchar8 *dstmask);

/*********************************************************************
*
* @purpose  To add the Ethertype keyword identifier.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    keyid       @b{(input)} Ethertype keyword identifier
* @param    value       @b{(input)} Ethertype custom value
*
* @returns  L7_SUCCESS  Ethertype keyword identifier added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments The value parameter is only meaningful when keyid is set to 
*           L7_QOS_ETYPE_KEYID_CUSTOM.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleEtypeKeyAdd(L7_uint32 aclIndex, L7_uint32 rulenum, 
                              L7_QOS_ETYPE_KEYID_t keyid, L7_uint32 value);

/*********************************************************************
*
* @purpose  Set the match condition whereby all packets match for a
*           MAC ACL rule
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    every       @b{(input)} 'match every' flag value (L7_TRUE/L7_FALSE)
*
* @returns  L7_SUCCESS  'match every' condition added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleEveryAdd(L7_uint32 aclIndex, L7_uint32 rulenum, L7_BOOL every);

/*********************************************************************
*
* @purpose  To add the source MAC address and mask.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    srcmac      @b{(input)} source MAC address
* @param    srcmask     @b{(input)} source MAC mask
*
* @returns  L7_SUCCESS  source MAC added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleSrcMacAdd(L7_uint32 aclIndex, L7_uint32 rulenum,
                            L7_uchar8 *srcmac, L7_uchar8 *srcmask);

/*********************************************************************
*
* @purpose  To add a single VLAN ID.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    vlan        @b{(input)} VLAN ID
*
* @returns  L7_SUCCESS  VLAN ID added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleVlanIdAdd(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 vlan);

/*********************************************************************
*
* @purpose  To add a VLAN ID range.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    startvlan   @b{(input)} starting VLAN ID
* @param    endvlan     @b{(input)} ending VLAN ID
*
* @returns  L7_SUCCESS  VLAN ID range added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleVlanIdRangeAdd(L7_uint32 aclIndex, L7_uint32 rulenum, 
                                 L7_uint32 startvlan, L7_uint32 endvlan);

/*********************************************************************
*
* @purpose  To add a single Secondary VLAN ID (vlan2).
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    vlan2       @b{(input)} Secondary VLAN ID
*
* @returns  L7_SUCCESS  Secondary VLAN ID added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleVlanId2Add(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 vlan2);

/*********************************************************************
*
* @purpose  To add a Secondary VLAN ID (vlan2) range.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    startvlan2  @b{(input)} starting Secondary VLAN ID
* @param    endvlan2    @b{(input)} ending Secondary VLAN ID
*
* @returns  L7_SUCCESS  Secondary VLAN ID range added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
* @returns  L7_REQUEST_DENIED hardware update failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleVlanId2RangeAdd(L7_uint32 aclIndex, L7_uint32 rulenum, 
                                  L7_uint32 startvlan2, L7_uint32 endvlan2);

/*********************************************************************
*
* @purpose  To add a MAC access list to the specified interface and direction.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    direction   @b{(input)} interface direction to apply ACL
*                                   (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    aclIndex    @b{(input)} access list index
* @param    seqNum      @b{(input)} ACL evaluation order sequence number
*
* @returns  L7_SUCCESS  ACL successfully added to intf, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
* @returns  L7_ERROR    ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED  intf, dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED  feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL  ACL assignment at capacity for intf, dir
* @returns  L7_REQUEST_DENIED error applying ACL to hardware
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacInterfaceDirectionAdd(L7_uint32 intIfNum, L7_uint32 direction, 
                                    L7_uint32 aclIndex, L7_uint32 seqNum);

/*********************************************************************
*
* @purpose  To remove a MAC access list from the specified interface and direction.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    direction   @b{(input)} interface direction to apply ACL
*                                   (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    aclIndex    @b{(input)} access list index
*
* @returns  L7_SUCCESS  intIfNum is removed from the access-list
* @returns  L7_FAILURE  invalid ACL index, or interface does not exist
* @returns  L7_ERROR    ACL does not exist for this index
* @returns  L7_REQUEST_DENIED error removing ACL from hardware
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacInterfaceDirectionRemove(L7_uint32 intIfNum, L7_uint32 direction, 
                                       L7_uint32 aclIndex);

/*********************************************************************
*
* @purpose  To add a MAC access list to the specified vlan and direction.
*
* @param    vlanNum     @b{(input)} internal vlan number
* @param    direction   @b{(input)} vlan direction to apply ACL
*                                   (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    aclIndex    @b{(input)} access list index
* @param    seqNum      @b{(input)} ACL evaluation order sequence number
*
* @returns  L7_SUCCESS  ACL successfully added to vlan, dir
* @returns  L7_FAILURE  invalid parameter, or other failure
* @returns  L7_ERROR    ACL assignment sequence number already at limit
* @returns  L7_ALREADY_CONFIGURED  vlan, dir already assigned a single ACL
* @returns  L7_NOT_SUPPORTED  feature not supported for attempted action
* @returns  L7_TABLE_IS_FULL  ACL assignment at capacity for vlan, dir
* @returns  L7_REQUEST_DENIED error applying ACL to hardware
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacVlanDirectionAdd( L7_uint32 vlanNum,
                                L7_uint32 direction,
                                L7_uint32 aclIndex,
                                L7_uint32 seqNum );

/*********************************************************************
*
* @purpose  To remove a MAC access list from the specified vlan and direction.
*
* @param    L7_uint32  vlanNum    the internal vlan number
* @param    L7_uint32  direction  the direction of application of ACL
*                                 (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    L7_uint32  aclIndex   the access-list identifying index
*
* @returns  L7_SUCCESS  vlanNum is removed from the access-list
* @returns  L7_FAILURE  invalid ACL index, or vlan does not exist
* @returns  L7_ERROR    ACL does not exist for this index
* @returns  L7_REQUEST_DENIED     error removing ACL from hardware
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacVlanDirectionRemove( L7_uint32 vlanNum,
                                   L7_uint32 direction,
                                   L7_uint32 aclIndex );

/*********************************************************************
*
* @purpose  Get the first MAC access list created in the system.
*
* @param    *aclIndex   @b{(output)} first access list index value
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no MAC access lists exist
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacIndexGetFirst(L7_uint32 *aclIndex);

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
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacIndexGetNext(L7_uint32 aclIndex, L7_uint32 *next);

/*********************************************************************
*
* @purpose  Checks if aclIndex is a valid, configured MAC access list
*
* @param    aclIndex    @b{(input)}  access list index
*
* @returns  L7_SUCCESS  MAC access list is valid
* @returns  L7_FAILURE  MAC access list not valid
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacIndexCheckValid(L7_uint32 aclIndex);

/*********************************************************************
*
* @purpose  Checks if the MAC ACL index is within proper range
*
* @param    aclIndex    @b{(input)}  access list index to begin search
*
* @returns  L7_SUCCESS  MAC access list index is in range
* @returns  L7_FAILURE  MAC access list index out of range
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacIndexRangeCheck(L7_uint32 aclIndex);

/*********************************************************************
*
* @purpose  Get the first access list rule given a MAC ACL index
*
* @param    aclIndex    @b{(input)}  access list index
* @param    *rulenum    @b{(output)} first configured rule number
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no rules exist for this ACL
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleGetFirst(L7_uint32 aclIndex, L7_uint32 *rulenum);

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
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleGetNext(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 *next);

/*********************************************************************
*
* @purpose  Checks if rulenum is valid, configured for this MAC access list
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  current rule number
*
* @returns  L7_SUCCESS  rule is valid
* @returns  L7_ERROR    rule not valid
* @returns  L7_FAILURE  access list does not exist, or other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleCheckValid(L7_uint32 aclIndex, L7_uint32 rulenum);

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
* @comments The aclIndex parameter is required by this function in case there 
*           is ever a need to support separate rule number ranges based
*           on the type of ACL (i.e., standard vs. extended).
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleNumRangeCheck(L7_uint32 aclIndex, L7_uint32 rulenum);

/*********************************************************************
*
* @purpose  Checks if aclIndex is currently assigned to this intf,dir pair
*
* @param    aclIndex    @b{(input)}  access list index
* @param    intIfNum    @b{(input)}  internal interface number
* @param    direction   @b{(input)}  interface direction
*
* @returns  L7_SUCCESS, if the MAC ACL is assigned to this intf,dir
* @returns  L7_FAILURE, if the MAC ACL is not assigned to this intf,dir
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacIntfDirCheckValid(L7_uint32 aclIndex, L7_uint32 intIfNum, L7_uint32 direction);

/*********************************************************************
*
* @purpose  Checks if aclIndex is currently assigned to this vlan,dir pair
*
* @param    L7_uint32  aclIndex   the access-list identifying index
* @param    L7_uint32  vlan       the VLAN number
* @param    L7_uint32  direction  the VLAN direction
*
* @returns  L7_SUCCESS, if the MAC ACL is assigned to this intf,dir
* @returns  L7_FAILURE, if the MAC ACL is not assigned to this intf,dir
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacVlanDirCheckValid( L7_uint32 aclIndex,
                                 L7_uint32 vlan,
                                 L7_uint32 direction );

/*********************************************************************
*
* @purpose  Checks if the MAC ACL name is a valid string
*
* @param    *name       @b{(input)} MAC access-list name string
*
* @returns  L7_SUCCESS  valid MAC ACL name string
* @returns  L7_FAILURE  invalid MAC ACL name string
*
* @comments This function only checks the name string syntax for a MAC ACL. 
*           It does not check if an MAC ACL currently contains this name.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacNameStringCheck(L7_uchar8 *name);

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
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL aclMacIsFieldConfigured(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 field);

/*********************************************************************
*
* @purpose  To get an action fom a MAC access list rule.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *action     @b{(output)} action type (permit/deny)
*
* @returns  L7_SUCCESS  rule action retrieved
* @returns  L7_FAILURE  invalid acl identifier, all other failures
* @returns  L7_ERROR    access-list does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleActionGet(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 *action);

/*********************************************************************
*
* @purpose  To get the assigned queue id from an access list rule.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *queueId    @b{(output)} assigned queue ID value
*
* @returns  L7_SUCCESS  assigned queue id retrieved
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid queue ID or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleAssignQueueIdGet(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 *queueId);

/*********************************************************************
*
* @purpose  To get the redirect interface config id from an access list rule.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    *configId   @b{(output)} redirected interface config id
*                                            
* @returns  L7_SUCCESS  redirect interface config id retrieved
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  invalid redirect interface or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleRedirectConfigIdGet(L7_uint32 aclIndex, L7_uint32 rulenum, nimConfigID_t *configId);

/*********************************************************************
*
* @purpose  To get the mirror interface config id from an access list rule.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    *configId   @b{(output)} mirrored interface config id
*                                            
* @returns  L7_SUCCESS  mirror interface config id retrieved
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  invalid mirror interface or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleMirrorConfigIdGet(L7_uint32 aclIndex, L7_uint32 rulenum, nimConfigID_t *configId);

/*********************************************************************
*
* @purpose  To get the logging flag from an access list rule.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *logFlag    @b{(output)} rule logging flag value
*
* @returns  L7_SUCCESS  logging flag is retrieved
* @returns  L7_ERROR    access list or rulenum does not exist
* @returns  L7_FAILURE  invalid log flag or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleLoggingGet(L7_uint32 aclIndex, L7_uint32 rulenum, L7_BOOL *logFlag);

/*********************************************************************
*
* @purpose  To get the class of service (cos) value from an access list rule.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *cos        @b{(output)} class of service (cos) value
*
* @returns  L7_SUCCESS  class of service value retrieved
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleCosGet(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 *cos);

/*********************************************************************
*
* @purpose  To get the secondary class of service (cos2) value from 
*           an access list rule.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *cos2       @b{(output)} secondary class of service (cos) value
*
* @returns  L7_SUCCESS  secondary class of service value retrieved
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleCos2Get(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 *cos2);

/*********************************************************************
*
* @purpose  To get the destination MAC address and mask from
*           an access list rule
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *dstmac     @b{(output)} destination MAC address
* @param    *dstmask    @b{(output)} destination MAC mask
*
* @returns  L7_SUCCESS  destination MAC added
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleDstMacGet(L7_uint32 aclIndex, L7_uint32 rulenum,
                            L7_uchar8 *dstmac, L7_uchar8 *dstmask);

/*********************************************************************
*
* @purpose  To get the Ethertype keyword identifier from an access list rule.
*
* @param    aclIndex    @b{(input)} access list index
* @param    rulenum     @b{(input)} ACL rule number
* @param    *keyid      @b{(output)} Ethertype keyword identifier
* @param    *value      @b{(output)} Ethertype custom value       
*
* @returns  L7_SUCCESS  Ethertype keyword identifier retrieved
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
*
* @comments The *value output is only meaningful when the *keyid 
*           reads L7_QOS_ETYPE_KEYID_CUSTOM.  It is set to zero
*           otherwise.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleEtypeKeyGet(L7_uint32 aclIndex, L7_uint32 rulenum, 
                              L7_QOS_ETYPE_KEYID_t *keyid, L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the match condition whereby all packets match for a
*           MAC ACL rule
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *every      @b{(output)} 'match every' flag value (L7_TRUE/L7_FALSE)
*
* @returns  L7_SUCCESS  'match every' condition retrieved
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleEveryGet(L7_uint32 aclIndex, L7_uint32 rulenum, L7_BOOL *every);

/*********************************************************************
*
* @purpose  To get the source MAC address and mask from an access list rule.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *srcmac     @b{(output)} source MAC address
* @param    *srcmask    @b{(output)} source MAC mask
*
* @returns  L7_SUCCESS  source MAC retrieved
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleSrcMacGet(L7_uint32 aclIndex, L7_uint32 rulenum,
                            L7_uchar8 *srcmac, L7_uchar8 *srcmask);

/*********************************************************************
*
* @purpose  To get a single VLAN ID from an access list rule.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *vlan       @b{(output)} VLAN ID
*
* @returns  L7_SUCCESS  VLAN ID retrieved
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleVlanIdGet(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 *vlan);

/*********************************************************************
*
* @purpose  To get a VLAN ID range from an access list rule.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *startvlan  @b{(output)} starting VLAN ID
* @param    *endvlan    @b{(output)} ending VLAN ID
*
* @returns  L7_SUCCESS  VLAN ID range retrieved
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleVlanIdRangeGet(L7_uint32 aclIndex, L7_uint32 rulenum, 
                                 L7_uint32 *startvlan, L7_uint32 *endvlan);

/*********************************************************************
*
* @purpose  To get a single Secondary VLAN ID (vlan2) from an access list rule.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *vlan2      @b{(output)} Secondary VLAN ID
*
* @returns  L7_SUCCESS  Secondary VLAN ID retrieved
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleVlanId2Get(L7_uint32 aclIndex, L7_uint32 rulenum, L7_uint32 *vlan2);

/*********************************************************************
*
* @purpose  To get a Secondary VLAN ID (vlan2) range from an access list rule.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    rulenum     @b{(input)}  ACL rule number
* @param    *startvlan2 @b{(output)} starting Secondary VLAN ID
* @param    *endvlan2   @b{(output)} ending Secondary VLAN ID
*
* @returns  L7_SUCCESS  Secondary VLAN ID range retrieved
* @returns  L7_ERROR    access-list or rulenum does not exist
* @returns  L7_FAILURE  all other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t aclMacRuleVlanId2RangeGet(L7_uint32 aclIndex, L7_uint32 rulenum, 
                                  L7_uint32 *startvlan2, L7_uint32 *endvlan2);

/*********************************************************************
*
* @purpose  Gets a list of interfaces to which an ACL is assigned in the
*           specified direction
*
* @param    aclIndex    @b{(input)}  access list index
* @param    direction   @b{(input)}  interface direction of interest
*                                      (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
* @param    *intfList   @b{(output)} list of interfaces (L7_ACL_INTF_MAX_COUNT long)
*
* @returns  L7_SUCCESS  list of interfaces was built
* @returns  L7_FAILURE  invalid access list index, or interface does not exist
* @returns  L7_ERROR    access list not assigned to any interface
*                         in this direction (no interface list was built)
*
* @comments An ACL can be shared/associated with multiple interfaces.
*
* @end
*
*********************************************************************/
L7_RC_t aclMacAssignedIntfDirListGet(L7_uint32 aclIndex, L7_uint32 direction,
                                     L7_ACL_ASSIGNED_INTF_LIST_t *intfList);

/*********************************************************************
*
* @purpose  Gets a list of vlan a MAC ACL is associated with for the
*           specified direction
*
* @param    L7_uint32  aclIndex    the access-list identifying index
* @param    L7_uint32  direction   the direction of application of ACL
*                                   (L7_INBOUND_ACL or L7_OUTBOUND_ACL)
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
L7_RC_t aclMacAssignedVlanDirListGet( L7_uint32 aclIndex,
                                      L7_uint32 direction,
                                      L7_ACL_ASSIGNED_VLAN_LIST_t *vlanList );

/*************************************************************************
* @purpose  Get the maximum number of ACLs that can be configured
*
* @param    pMax  @b{(output)} Pointer to the maximum value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t aclMacMaxNumGet(L7_uint32 *pMax);

/*************************************************************************
* @purpose  Get the current number of MAC ACLs configured
*
* @param    pCurr  @b{(output)} Pointer to the current value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t aclMacCurrNumGet(L7_uint32 *pCurr);

/*********************************************************************
*
* @purpose  Creates a single TLV containing active rules of the specified
*           MAC ACL within it and return the data in TLV block storage
*           provided by the caller.
*
* @param    aclIndex    @b{(input)}  access list index
* @param    tlvHandle   @b{(input)}  TLV block handle (supplied by caller)
*
* @returns  L7_SUCCESS  TLV successfully built
* @returns  L7_ERROR    Access list does not exist
* @returns  L7_FAILURE  All other failures
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t aclMacTlvGet(L7_uint32 aclIndex, L7_tlvHandle_t tlvHandle);

/*********************************************************************
* @purpose  Indicates if ACL cluster config receive operation currently in progress
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  
*
* @end
*********************************************************************/
L7_BOOL aclClusterIsRxInProgress(void);

/*********************************************************************
*
* @purpose  Checks if config push aclnum exists
*
* @param    L7_uint32  aclnum   the access-list identifying number
*
* @returns  L7_SUCCESS, if the access list was found
* @returns  L7_FAILURE, if the access list does not exist 
*
* @comments Searches the received config push data for a matching 
*           ACL number.
*
* @end
*
*********************************************************************/
L7_RC_t aclClusterNumCheckValid(L7_uint32 aclnum);

/*********************************************************************
*
* @purpose  To get the number of a config push named access list, given its name.
*
* @param    *name      @b{(input)} access list name
* @param    *aclnum    @b{(output)} access list identifying number
*                                            
* @returns  L7_SUCCESS  named access list number is retrieved
* @returns  L7_FAILURE  invalid parms, or other failure
* @returns  L7_ERROR    access list name does not exist
*
* @comments Searches the received config push data for a matching 
*           ACL number.
*
* @end
*
*********************************************************************/
L7_RC_t aclClusterNameToIndex(L7_uchar8 *name, L7_uint32 *aclnum);

/*********************************************************************
*
* @purpose  To get the index of a config push MAC access list, given its name.
*
* @param    *name       @b{(input)} access list name
* @param    *aclIndex   @b{(output)} access list index
*                                            
* @returns  L7_SUCCESS  MAC access list index is retrieved
* @returns  L7_FAILURE  invalid parms, or other failure
* @returns  L7_ERROR    access list name does not exist
*
* @comments Searches the received config push data for a matching 
*           ACL number.
*
* @end
*
*********************************************************************/
L7_RC_t aclClusterMacNameToIndex(L7_uchar8 *name, L7_uint32 *aclIndex);

L7_RC_t aclConfigSyncWithEasyACL();
#endif  /* ACL_API_H */
