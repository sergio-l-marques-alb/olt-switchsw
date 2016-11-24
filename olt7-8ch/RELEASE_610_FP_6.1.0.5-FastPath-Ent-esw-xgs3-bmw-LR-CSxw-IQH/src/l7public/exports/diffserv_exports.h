/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename snooping_exports.h
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

#ifndef __DIFFSERV_EXPORTS_H_
#define __DIFFSERV_EXPORTS_H_

/* QOS DiffServ Component Feature List
 *
 * NOTE: Remember to update the feature string list in diffserv_util.c whenever
 *       a change is made to this list
 */
typedef enum
{
  /* DiffServ support */
  L7_DIFFSERV_FEATURE_SUPPORTED = 0,            /* general support statement */
  /* class type */
  L7_DIFFSERV_CLASS_TYPE_ALL_FEATURE_ID,
  L7_DIFFSERV_CLASS_TYPE_ANY_FEATURE_ID,
  L7_DIFFSERV_CLASS_TYPE_ACL_FEATURE_ID,
  /* class l3 protocol */
  L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID,
  L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID,
  /* class match criteria */
  L7_DIFFSERV_CLASS_MATCH_COS_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_COS2_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_DSTIP_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_DSTL4PORT_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_DSTMAC_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_ETYPE_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_EVERY_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_IPV6_DSTIP_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_IPV6_DSTL4PORT_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_IPV6_DSTL4PORT_RANGE_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_IPV6_FLOWLBL_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_IPV6_IPDSCP_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_IPV6_PROTOCOL_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_IPV6_SRCIP_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_IPV6_SRCL4PORT_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_IPV6_SRCL4PORT_RANGE_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_IPV6_MAC_FEATURE_ID,              /* supports IPv6 and MAC (L2) fields in same class */
  L7_DIFFSERV_CLASS_MATCH_IPDSCP_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_IPPRECEDENCE_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_IPTOS_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_PROTOCOL_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_REFCLASS_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_SRCIP_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_SRCL4PORT_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_SRCMAC_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_VLANID_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_VLANID2_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID,
  L7_DIFFSERV_CLASS_MATCH_SUPPORTS_MASKING_FEATURE_ID, /* general masking supp*/
  L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID,  /* general range supp  */
  /* policy */
  L7_DIFFSERV_POLICY_OUT_CLASS_UNRESTRICTED_FEATURE_ID, /* not just DSCP/Prec */
  L7_DIFFSERV_POLICY_MIXED_IPV6_FEATURE_ID,   /* allow both IPv6 and IPv4 classes in same policy */
  /* policy attributes -- inbound */
  L7_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_FEATURE_ID,
  L7_DIFFSERV_POLICY_ATTR_DROP_FEATURE_ID,
  L7_DIFFSERV_POLICY_ATTR_MARK_COSVAL_FEATURE_ID,
  L7_DIFFSERV_POLICY_ATTR_MARK_COS2VAL_FEATURE_ID,
  L7_DIFFSERV_POLICY_ATTR_MARK_COSASCOS2_FEATURE_ID,
  L7_DIFFSERV_POLICY_ATTR_MARK_IPDSCPVAL_FEATURE_ID,
  L7_DIFFSERV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_FEATURE_ID,
  L7_DIFFSERV_POLICY_ATTR_MIRROR_FEATURE_ID,
  L7_DIFFSERV_POLICY_ATTR_POLICE_SIMPLE_FEATURE_ID,
  L7_DIFFSERV_POLICY_ATTR_POLICE_SINGLERATE_FEATURE_ID,
  L7_DIFFSERV_POLICY_ATTR_POLICE_TWORATE_FEATURE_ID,
  L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID,
  L7_DIFFSERV_POLICY_ATTR_REDIRECT_FEATURE_ID,
  L7_DIFFSERV_POLICY_ATTR_MARK_COS_AND_ASSIGN_QUEUE_FEATURE_ID,
  /* service interface */
  L7_DIFFSERV_SERVICE_IN_SLOTPORT_FEATURE_ID,
  L7_DIFFSERV_SERVICE_IN_ALLPORTS_FEATURE_ID,
  L7_DIFFSERV_SERVICE_OUT_SLOTPORT_FEATURE_ID,
  L7_DIFFSERV_SERVICE_OUT_ALLPORTS_FEATURE_ID,
  L7_DIFFSERV_SERVICE_SUPPORTS_LAG_INTF_FEATURE_ID,
  L7_DIFFSERV_SERVICE_SUPPORTS_TUNNEL_INTF_FEATURE_ID,
  L7_DIFFSERV_SERVICE_ACL_COEXIST_FEATURE_ID,
  /* PHB support */
  L7_DIFFSERV_PHB_EF_FEATURE_ID,
  L7_DIFFSERV_PHB_AF4X_FEATURE_ID,
  L7_DIFFSERV_PHB_AF3X_FEATURE_ID,
  L7_DIFFSERV_PHB_AF2X_FEATURE_ID,
  L7_DIFFSERV_PHB_AF1X_FEATURE_ID,
  L7_DIFFSERV_PHB_CS_FEATURE_ID,
  L7_DIFFSERV_PHB_OTHER_FEATURE_ID,     /* i.e., any other DSCP value */

  L7_DIFFSERV_FEATURE_ID_TOTAL          /* total number of enum values */
} L7_DIFFSERV_FEATURE_IDS_t;


/* QOS Ethertype keyword list */
typedef enum
{
  L7_QOS_ETYPE_KEYID_NONE = 0,

  L7_QOS_ETYPE_KEYID_CUSTOM,                              /* user value     */

  L7_QOS_ETYPE_KEYID_APPLETALK,                           /* Appletalk      */
  L7_QOS_ETYPE_KEYID_ARP,                                 /* ARP            */
  L7_QOS_ETYPE_KEYID_IBMSNA,                              /* IBM SNA        */
  L7_QOS_ETYPE_KEYID_IPV4,                                /* IPv4           */
  L7_QOS_ETYPE_KEYID_IPV6,                                /* IPv6           */
  L7_QOS_ETYPE_KEYID_IPX,                                 /* IPX            */
  L7_QOS_ETYPE_KEYID_MPLSMCAST,                           /* MPLS multicast */
  L7_QOS_ETYPE_KEYID_MPLSUCAST,                           /* MPLS unicast   */
  L7_QOS_ETYPE_KEYID_NETBIOS,                             /* NetBIOS        */
  L7_QOS_ETYPE_KEYID_NOVELL,                              /* Novell         */
  L7_QOS_ETYPE_KEYID_PPPOE,                               /* PPPoE          */
  L7_QOS_ETYPE_KEYID_RARP,                                /* Reverse ARP    */

  L7_QOS_ETYPE_KEYID_TOTAL                      /* number of enum entries */

} L7_QOS_ETYPE_KEYID_t;

#define L7_QOS_ETYPE_KEYID_MIN                  (L7_QOS_ETYPE_KEYID_NONE+1)
#define L7_QOS_ETYPE_KEYID_MAX                  (L7_QOS_ETYPE_KEYID_TOTAL-1)

#define L7_QOS_ETYPE_CUSTOM_VAL_MIN             0x0600
#define L7_QOS_ETYPE_CUSTOM_VAL_MAX             0xFFFF

#define L7_QOS_ETYPE_VALUE_UNUSED               0x0000

/* QOS User Keyword Translations for Ethertype Hex Values */
#define L7_QOS_ETYPE_ID_CUSTOM                  0x0000  /* user-supplied */
#define L7_QOS_ETYPE_ID_APPLETALK               0x809B
#define L7_QOS_ETYPE_ID_ARP                     0x0806
#define L7_QOS_ETYPE_ID_IBMSNA                  0x80D5
#define L7_QOS_ETYPE_ID_IPV4                    0x0800
#define L7_QOS_ETYPE_ID_IPV6                    0x86DD
#define L7_QOS_ETYPE_ID_IPX                     0x8037
#define L7_QOS_ETYPE_ID_MPLSMCAST               0x8848
#define L7_QOS_ETYPE_ID_MPLSUCAST               0x8847
#define L7_QOS_ETYPE_ID_NETBIOS                 0x8191
#define L7_QOS_ETYPE_ID_NOVELL_ID1              0x8137   /* 1st ID */
#define L7_QOS_ETYPE_ID_NOVELL_ID2              0x8138   /* 2nd ID */
#define L7_QOS_ETYPE_ID_PPPOE_ID1               0x8863   /* 1st ID */
#define L7_QOS_ETYPE_ID_PPPOE_ID2               0x8864   /* 2nd ID */
#define L7_QOS_ETYPE_ID_RARP                    0x8035

/* User Keyword Ethertype Name Strings */
#define L7_QOS_ETYPE_STR_CUSTOM                 "custom"  /* value */
#define L7_QOS_ETYPE_STR_APPLETALK              "appletalk"
#define L7_QOS_ETYPE_STR_ARP                    "arp"
#define L7_QOS_ETYPE_STR_IBMSNA                 "ibmsna"
#define L7_QOS_ETYPE_STR_IPV4                   "ipv4"
#define L7_QOS_ETYPE_STR_IPV6                   "ipv6"
#define L7_QOS_ETYPE_STR_IPX                    "ipx"
#define L7_QOS_ETYPE_STR_MPLSMCAST              "mplsmcast"
#define L7_QOS_ETYPE_STR_MPLSUCAST              "mplsucast"
#define L7_QOS_ETYPE_STR_NETBIOS                "netbios"
#define L7_QOS_ETYPE_STR_NOVELL                 "novell"
#define L7_QOS_ETYPE_STR_PPPOE                  "pppoe"
#define L7_QOS_ETYPE_STR_RARP                   "rarp"




/******************** conditional Override *****************************/

#ifdef INCLUDE_DIFFSERV_EXPORTS_OVERRIDES
#include "diffserv_exports_overrides.h"
#endif

#endif /* __DIFFSERV_EXPORTS_H_*/
