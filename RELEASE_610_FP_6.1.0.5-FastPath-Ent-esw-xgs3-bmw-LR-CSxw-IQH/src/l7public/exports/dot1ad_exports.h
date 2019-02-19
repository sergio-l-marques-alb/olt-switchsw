/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename dot1ad_exports.h
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
* @created 
*
* @author 
* @end
*
**********************************************************************/

#ifndef __DOT1AD_EXPORTS_H_
#define __DOT1AD_EXPORTS_H_


typedef enum
{
  L7_DOT1AD_FEATURE_ID = 0,
  L7_FLEX_METRO_DOT1AD_VLAN_BASED_PROTOCOL_TUNNEL_FEATURE_ID,
  L7_FLEX_METRO_DOT1AD_INTF_BASED_PROTOCOL_TUNNEL_FEATURE_ID,
  L7_FLEX_METRO_DOT1AD_DATA_TUNNEL_FEATURE_ID,
  L7_FLEX_METRO_DOT1AD_CPE_FEATURE_ID,
  L7_FLEX_METRO_DOT1AD_PB_FEATURE_ID,
  L7_FLEX_METRO_DOT1AD_SVLAN_FEATURE_ID,
  L7_FLEX_METRO_DOT1AD_ELINE_ON_LAG_FEATURE_ID,
  L7_FLEX_METRO_DOT1AD_SUPPORTED_FEATURE_ID,
  L7_FLEX_METRO_DOT1AD_VLAN_CROSSCONNECT_ELINE_FEATURE_ID,
  L7_DOT1AD_FEATURE_ID_TOTAL,
}L7_DOT1AD_FEATURE_IDS_t;


/*-------------------------------------------------------------------*/
/*----------------Common dot1ad constants -----------------------------*/
/*------------------------------------------------------------------ */

#define L7_DOT1AD_RESERVED_MAC_MASK_LEN 5
#define L7_DOT1AD_INTF_RESERVEDMAC_NUM_MIN 0
#define L7_DOT1AD_INTF_RESERVEDMAC_NUM_MAX 15
#define L7_DOT1AD_L2_PROTOCOL_ID_MAX \
                L7_DOT1AD_INTF_RESERVEDMAC_NUM_MAX

typedef enum
{
  DOT1AD_TERMINATE=0,
  DOT1AD_TUNNEL,
  DOT1AD_DISCARD,
  DOT1AD_DISCARD_SHUTDOWN,
  DOT1AD_DETUNNEL
} DOT1AD_TUNNEL_ACTION_t;

typedef enum
{
   DOT1AD_INTFERFACE_TYPE_UNI=0, /* UNI port */
   DOT1AD_INTFERFACE_TYPE_UNI_P, /* Port based UNI port */
   DOT1AD_INTFERFACE_TYPE_UNI_S, /* Service based UNI port */
   DOT1AD_INTFERFACE_TYPE_NNI
}DOT1AD_INTFERFACE_TYPE_t;
/* Defines for Dot1ad */
/***************************************************/
/*          DOT1AD constants                       */
/***************************************************/
/* maximum service instance name length */
#define L7_DOT1AD_SERVICE_NAME_LEN_MIN             1   /* does not include trailing '\0' */
#define L7_DOT1AD_SERVICE_NAME_LEN_MAX             31  /* does not include trailing '\0' */

#define L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MIN             1   /* does not include trailing '\0' */
#define L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX             31  /* does not include trailing '\0' */

#define L7_DOT1AD_MAX_NNI_INTFS_PER_SERVICE              (L7_MAX_PHYSICAL_PORTS_PER_UNIT/6)
#define L7_DOT1AD_MAX_NNI_INTFS_PER_SUBSCRIPTION    (L7_MAX_PHYSICAL_PORTS_PER_UNIT/6)

#define L7_DOT1AD_TUNNELING_MAC_MIN "01:80:c2:00:00:00"
#define L7_DOT1AD_TUNNELING_MAC_MAX "01:80:c2:00:00:3f"

#define L7_DOT1AD_TUNNELING_PROTOCOL_ID_MIN "0x0001"
#define L7_DOT1AD_TUNNELING_PROTOCOL_ID_MAX "0xffff"

#define L7_DOT1AD_SERVICE_CONFORM_DATA_RATE_MIN 64
#define L7_DOT1AD_SERVICE_CONFORM_DATA_RATE_MAX 4294967295U

/* AutoInstall defines .*/
#define FD_AUTO_INSTALL_UNICAST_RETRY_COUNT_MIN      1
#define FD_AUTO_INSTALL_UNICAST_RETRY_COUNT_MAX      6

typedef enum
{
  L7_DOT1AD_SVCTYPE_ELINE = 1,
  L7_DOT1AD_SVCTYPE_ELAN,
  L7_DOT1AD_SVCTYPE_ETREE,
  L7_DOT1AD_SVCTYPE_TLS,
  L7_DOT1AD_SVCTYPE_TOTAL                   
}L7_DOT1AD_SVCTYPE_t;
 
typedef enum
{
  L7_DOT1AD_PKTTYPE_NONE = 0,
  L7_DOT1AD_PKTTYPE_UNTAGGED,
  L7_DOT1AD_PKTTYPE_PRIOTAGGED,
  L7_DOT1AD_PKTTYPE_TAGGED,
  L7_DOT1AD_PKTTYPE_NO_STAG, /*To match packets without service tag*/
  L7_DOT1AD_PKTTYPE_NO_CTAG,
  L7_DOT1AD_PKTTYPE_TOTAL
}L7_DOT1AD_PKTTYPE_t;

/* Vlan 4093 and 4094 are reserved for E-Tree and hence the max CVID and SVID is set to 4092 */

#define L7_DOT1AD_SVID_MIN                                      1
#define L7_DOT1AD_SVID_MAX                                      4092

#define L7_DOT1AD_CVID_MIN                                      1
#define L7_DOT1AD_CVID_MAX                                      4092

#define L7_DOT1AD_INTF_MAX_COUNT                                L7_MAX_PORT_COUNT + L7_MAX_NUM_LAG_INTF + 1 
#define L7_DOT1AD_SERVICES_MAX_PER_INTF                         8
#define L7_DOT1AD_SUBSCRIPTIONS_MAX                             4096

#define L7_DOT1AD_ETREE_RESERVED_INNER_VLAN                     4093

typedef enum
{
  DOT1AD_SERVICE = 1,
  DOT1AD_PKTTYPE,
  DOT1AD_SVID,
  DOT1AD_MATCH_CVID,
  DOT1AD_MATCH_SVID,
  DOT1AD_MATCH_PRIORITY,
  DOT1AD_ASSIGN_CVID ,
  DOT1AD_REMARK_CVID,
  DOT1AD_REMOVE_CTAG,
  DOT1AD_NNI_INTERFACE_LIST,
  DOT1AD_SERVICEFIELDS_TOTAL                  /* total number of dot1ad fields */
} Dot1adRuleFields_t;

/******************** conditional Override *****************************/

#ifdef INCLUDE_DOT1AD_EXPORTS_OVERRIDES
#include "dot1ad_exports_overrides.h"
#endif

#endif /* __DOT1AD_EXPORTS_H_*/
