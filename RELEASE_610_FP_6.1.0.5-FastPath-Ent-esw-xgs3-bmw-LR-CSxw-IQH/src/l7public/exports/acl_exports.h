/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename acl_exports.h
*
* @purpose  Defines constants and feature definitions that are shared by Management and the application 
*
* @component 
*
* @comments 
*           
*
* @Notes   
*
* @created 03/26/2008
*
* @author akulkarn    
* @end
*
**********************************************************************/

#ifndef __ACL_EXPORTS_H_
#define __ACL_EXPORTS_H_
/* ACL Component Feature List */
typedef enum
{
  /* ACL supported */
  L7_ACL_FEATURE_SUPPORTED = 0,            /* general support statement */

  /* individual ACL type support */
  L7_ACL_TYPE_MAC_FEATURE_ID,              /* L2 MAC ACL support statement */
  L7_ACL_TYPE_IPV6_FEATURE_ID,             /* IPV6 ACL support statement */

  /* common match criteria */
  L7_ACL_RULE_MATCH_EVERY_FEATURE_ID,

  /* IPV4 rule match criteria */
  L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID,
  L7_ACL_RULE_MATCH_DSTL4PORT_FEATURE_ID,
  L7_ACL_RULE_MATCH_DSTL4PORT_RANGE_FEATURE_ID,
  L7_ACL_RULE_MATCH_IPDSCP_FEATURE_ID,
  L7_ACL_RULE_MATCH_IPPRECEDENCE_FEATURE_ID,
  L7_ACL_RULE_MATCH_IPTOS_FEATURE_ID,
  L7_ACL_RULE_MATCH_PROTOCOL_FEATURE_ID,
  L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID,
  L7_ACL_RULE_MATCH_SRCL4PORT_FEATURE_ID,
  L7_ACL_RULE_MATCH_SRCL4PORT_RANGE_FEATURE_ID,
  L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID, /* general IP masking supp*/

  /* IPV6 rule match criteria */
  L7_ACL_RULE_MATCH_IPV6_DSTIP_FEATURE_ID,
  L7_ACL_RULE_MATCH_IPV6_DSTL4PORT_FEATURE_ID,
  L7_ACL_RULE_MATCH_IPV6_DSTL4PORT_RANGE_FEATURE_ID,
  L7_ACL_RULE_MATCH_IPV6_FLOWLBL_FEATURE_ID,
  L7_ACL_RULE_MATCH_IPV6_IPDSCP_FEATURE_ID,
  L7_ACL_RULE_MATCH_IPV6_PROTOCOL_FEATURE_ID,
  L7_ACL_RULE_MATCH_IPV6_SRCIP_FEATURE_ID,
  L7_ACL_RULE_MATCH_IPV6_SRCL4PORT_FEATURE_ID,
  L7_ACL_RULE_MATCH_IPV6_SRCL4PORT_RANGE_FEATURE_ID,

  /* MAC rule match criteria */
  L7_ACL_RULE_MATCH_COS_FEATURE_ID,
  L7_ACL_RULE_MATCH_COS2_FEATURE_ID,
  L7_ACL_RULE_MATCH_DSTMAC_FEATURE_ID,
  L7_ACL_RULE_MATCH_DSTMAC_MASK_FEATURE_ID,
  L7_ACL_RULE_MATCH_ETYPE_FEATURE_ID,
  L7_ACL_RULE_MATCH_SRCMAC_FEATURE_ID,
  L7_ACL_RULE_MATCH_SRCMAC_MASK_FEATURE_ID,
  L7_ACL_RULE_MATCH_VLANID_FEATURE_ID,
  L7_ACL_RULE_MATCH_VLANID_RANGE_FEATURE_ID,
  L7_ACL_RULE_MATCH_VLANID2_FEATURE_ID,
  L7_ACL_RULE_MATCH_VLANID2_RANGE_FEATURE_ID,

  /* rule option support */
  L7_ACL_ASSIGN_QUEUE_FEATURE_ID,
  L7_ACL_REDIRECT_FEATURE_ID,
  L7_ACL_MIRROR_FEATURE_ID,
  L7_ACL_LOG_DENY_FEATURE_ID,
  L7_ACL_LOG_PERMIT_FEATURE_ID,

  /*  interface support */
  L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID,
  L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID,
  L7_ACL_INTF_DIFFSERV_COEXIST_FEATURE_ID,
  L7_ACL_INTF_LAG_SUPPORT_FEATURE_ID,
  L7_ACL_INTF_MULTIPLE_ACL_FEATURE_ID,
  L7_ACL_INTF_MIXED_INBOUND_TYPE_ACL_FEATURE_ID,   /* use for IPv4 vs. L2 MAC combo */
  L7_ACL_INTF_MIXED_INBOUND_IPV6_MAC_FEATURE_ID,   /* use for IPv6 vs. L2 MAC combo */
  L7_ACL_INTF_MIXED_INBOUND_IPV6_IPV4_FEATURE_ID,  /* use for IPv6 vs. IPv4 combo   */
  L7_ACL_INTF_MIXED_OUTBOUND_TYPE_ACL_FEATURE_ID,   /* use for IPv4 vs. L2 MAC combo */
  L7_ACL_INTF_MIXED_OUTBOUND_IPV6_MAC_FEATURE_ID,   /* use for IPv6 vs. L2 MAC combo */
  L7_ACL_INTF_MIXED_OUTBOUND_IPV6_IPV4_FEATURE_ID,  /* use for IPv6 vs. IPv4 combo   */

  L7_ACL_INTF_VLAN_SUPPORT_FEATURE_ID,     /* allow binding of ACLs to VLAN IDs */
  L7_ACL_VLAN_DIRECTION_INBOUND_FEATURE_ID,
  L7_ACL_VLAN_DIRECTION_OUTBOUND_FEATURE_ID,
  L7_ACL_VLAN_MULTIPLE_ACL_FEATURE_ID,
  L7_ACL_VLAN_MIXED_INBOUND_TYPE_ACL_FEATURE_ID,   /* use for IPv4 vs. L2 MAC combo */
  L7_ACL_VLAN_MIXED_INBOUND_IPV6_MAC_FEATURE_ID,   /* use for IPv6 vs. L2 MAC combo */
  L7_ACL_VLAN_MIXED_INBOUND_IPV6_IPV4_FEATURE_ID,  /* use for IPv6 vs. IPv4 combo   */
  L7_ACL_VLAN_MIXED_OUTBOUND_TYPE_ACL_FEATURE_ID,   /* use for IPv4 vs. L2 MAC combo */
  L7_ACL_VLAN_MIXED_OUTBOUND_IPV6_MAC_FEATURE_ID,   /* use for IPv6 vs. L2 MAC combo */
  L7_ACL_VLAN_MIXED_OUTBOUND_IPV6_IPV4_FEATURE_ID,  /* use for IPv6 vs. IPv4 combo   */
  L7_FEAT_EGRESS_ACL_ON_IFP_ID,                     /* use IFP for Egress ACLs */

  L7_ACL_FEATURE_ID_TOTAL                  /* total number of enum values */
} L7_ACL_FEATURE_IDS_t;



/*--------------------------------------*/
/*  ACL Constants                       */
/*--------------------------------------*/

typedef enum
{
  L7_ACL_TYPE_NONE = 0,                 /* undefined    */
  L7_ACL_TYPE_IP,                       /* IP           */
  L7_ACL_TYPE_MAC,                      /* MAC          */
  L7_ACL_TYPE_IPV6                      /* IPv6         */

} L7_ACL_TYPE_t;

/* maximum ACL name, used for named access lists */
#define L7_ACL_NAME_LEN_MIN             1   /* does not include trailing '\0' */
#define L7_ACL_NAME_LEN_MAX             31  /* does not include trailing '\0' */

/* define allowed standard and extended ACL ID ranges */
#define L7_ACL_MIN_STD1_ID              1
#define L7_ACL_MAX_STD1_ID              99
#define L7_ACL_MIN_EXT1_ID              100
#define L7_ACL_MAX_EXT1_ID              199

/* these min/max ACL IDs are for IP standard and extended list numbers only */
#define L7_MIN_ACL_ID                   L7_ACL_MIN_STD1_ID
#define L7_MAX_ACL_ID                   L7_ACL_MAX_EXT1_ID

/* callers to ACL API functions use these values for interface direction */
typedef enum
{
  L7_INBOUND_ACL = 0,
  L7_OUTBOUND_ACL,

  L7_ACL_DIRECTION_TOTAL                /* total number of ACL directions defined */

} L7_ACL_DIRECTION_t;

/* ACL interface sequence number range definitions */
#define L7_ACL_AUTO_INTF_SEQ_NUM        (L7_ACL_MIN_INTF_SEQ_NUM-1)
#define L7_ACL_MIN_INTF_SEQ_NUM         1
#define L7_ACL_MAX_INTF_SEQ_NUM         4294967295U
/* ACL VLAN sequence number range definitions */
#define L7_ACL_MIN_VLAN_SEQ_NUM         1
#define L7_ACL_MAX_VLAN_SEQ_NUM         4294967295U

/* ACL auto-increment interface sequence number value */
#define L7_ACL_AUTO_INCR_INTF_SEQ_NUM   0

typedef enum
{
  L7_ACL_PERMIT = 0,
  L7_ACL_DENY,

  L7_ACL_ACTION_TOTAL                   /* total number of ACL actions defined */

} L7_ACL_ACTION_t;

/* ACL layer 4 port flag (source and destination) definitions */
typedef enum
{
  L7_ACL_NONE_L4_PORT = 0,
  L7_ACL_SRC_L4_PORT = 1,
  L7_ACL_DST_L4_PORT = 2
} L7_ACT_L4_PORTFLAG_t;

/* ACL dscp range definitions */
#define L7_ACL_MIN_DSCP                                   0
#define L7_ACL_MAX_DSCP                                   63

/* ACL precedence range definitions */
#define L7_ACL_MIN_PRECEDENCE                             0
#define L7_ACL_MAX_PRECEDENCE                             7

/* ACL tos bits' range definitions */
#define L7_ACL_MIN_TOS_BIT                                0
#define L7_ACL_MAX_TOS_BIT                                255

/* ACL IPv6 prefix length range definitions */
#define L7_ACL_MIN_IPV6_PREFIX_LEN                        0
#define L7_ACL_MAX_IPV6_PREFIX_LEN                        128

/* ACL IPv6 flow label range definitions */
#define L7_ACL_MIN_FLOWLBL                                0
#define L7_ACL_MAX_FLOWLBL                                1048575

/* ACL l4port number definitions */
#define L7_ACL_MIN_L4PORT_NUM                             0
#define L7_ACL_MAX_L4PORT_NUM                             65535

/* ACL protocol definitions */
#define L7_ACL_MIN_PROTOCOL_NUM                           0
#define L7_ACL_MAX_PROTOCOL_NUM                           255

/* ACL l4port keyword definitions*/
#define L7_ACL_L4PORT_DOMAIN                              53
#define L7_ACL_L4PORT_ECHO                                7
#define L7_ACL_L4PORT_FTP                                 21
#define L7_ACL_L4PORT_FTPDATA                             20
#define L7_ACL_L4PORT_HTTP                                80
#define L7_ACL_L4PORT_SMTP                                25
#define L7_ACL_L4PORT_SNMP                                161
#define L7_ACL_L4PORT_TELNET                              23
#define L7_ACL_L4PORT_TFTP                                69
#define L7_ACL_L4PORT_WWW                                 80

/* ACL protocol keyword definitions*/
#define L7_ACL_PROTOCOL_ICMP                              1
#define L7_ACL_PROTOCOL_ICMPV6                            58
#define L7_ACL_PROTOCOL_IGMP                              2
#define L7_ACL_PROTOCOL_IP                                255  /* use reserved value */
#define L7_ACL_PROTOCOL_TCP                               6
#define L7_ACL_PROTOCOL_UDP                               17

/* ACL ICMP message type/code definitions */
#define L7_ACL_ICMPV6_MSG_TYPE_ND_NA                      136  /* neighbor advertisement */
#define L7_ACL_ICMPV6_MSG_TYPE_ND_NS                      135  /* neighbor solicitation  */
#define L7_ACL_ICMPV6_MSG_CODE_ND_NA_NONE                 0
#define L7_ACL_ICMPV6_MSG_CODE_ND_NS_NONE                 0

/* ACL cos, cos2 allowed value range */
#define L7_ACL_MIN_COS                                    0
#define L7_ACL_MAX_COS                                    7

/* ACL vlanid, vlanid2 allowed value range */
#define L7_ACL_MIN_VLAN_ID                                0
#define L7_ACL_MAX_VLAN_ID                                4095

/* ACL VLAN sequence number range definitions */
#define L7_ACL_MIN_VLAN_SEQ_NUM         1
#define L7_ACL_MAX_VLAN_SEQ_NUM         4294967295U

#define L7_ACL_AUTO_INCR_VLAN_SEQ_NUM   0

/******************** conditional Override *****************************/

#ifdef INCLUDE_ACL_EXPORTS_OVERRIDES
#include "acl_exports_overrides.h"
#endif

#endif /* __ACL_EXPORTS_H_*/
