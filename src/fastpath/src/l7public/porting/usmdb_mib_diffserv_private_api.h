/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   usmdb_mib_private_diffserv_api.h
*
* @purpose    USMDB support for DiffServ MIB
*                  
* @component  unitmgr
*
* @comments   Common header file for DiffServ private MIB USMDB APIs
*
* @create     03/12/2002
*
* @author     gpaussa
* @end
*
**********************************************************************/
#ifndef USMDB_MIB_PRIVATE_DIFFSERV_API_H
#define USMDB_MIB_PRIVATE_DIFFSERV_API_H


#include "usmdb_mib_diffserv_common.h"
#include "acl_exports.h"
#include "diffserv_exports.h"
#include "l7_cos_api.h"


#define L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX    31
#define L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MIN    1

#define L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX   31
#define L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MIN   1

#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_REFCLASS_NONE   0

/* NOTE: This definition must be the same as L7_ACL_NAME_LEN_MAX */
#define L7_USMDB_MIB_DIFFSERV_CLASS_ACL_NAME_MAX         31

/*
**********************************************************************
*   USER CONFIG PARAMETER MIN/MAX RANGES
**********************************************************************
*/

/* Class Rule Table objects */
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_COS_MIN                0
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_COS_MAX                7
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPEKEYID_MIN         L7_QOS_ETYPE_KEYID_MIN
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPEKEYID_MAX         L7_QOS_ETYPE_KEYID_MAX
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPE_MIN              0x00000600  /* 1536  */
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPE_MAX              0x0000FFFF  /* 65535 */
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPADDR_MIN             0
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPADDR_MAX             4294967295UL
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MIN             0
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MAX             65535
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_MACADDR_MIN            0   /*per byte*/
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_MACADDR_MAX            255 /*per byte*/
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_FLOWLBLV6_MIN          0
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_FLOWLBLV6_MAX          0x000FFFFF  /* 20 bit field */
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPDSCP_MIN             0
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPDSCP_MAX             63   
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPPRECEDENCE_MIN       0
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPPRECEDENCE_MAX       7
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPTOS_MIN              0
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPTOS_MAX              255   
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_PROTOCOL_MIN           0
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_PROTOCOL_MAX           255   
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN             0
#define L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX             4095

/* Policy Attribute Table objects */
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_MIN      L7_QOS_COS_QUEUE_ID_MIN
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_MAX      L7_QOS_COS_QUEUE_ID_MAX
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CRATE_KBPS_MIN        1
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CRATE_KBPS_MAX        4294967295UL

#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CRATE_KBPS_ROBO_MIN     64
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CRATE_KBPS_ROBO_STEP    64

#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_PRATE_KBPS_MIN        1
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_PRATE_KBPS_MAX        4294967295UL
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CBURST_MIN            1
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CBURST_MAX            128 
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_EBURST_MIN            1
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_EBURST_MAX            128  
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_PBURST_MIN            1
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_PBURST_MAX            128  
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN               0
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX               7
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN              0
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX              7
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN            0
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX            63
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN      0
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX      7
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_REDIRECT_INTF_MIN     1
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_REDIRECT_INTF_MAX     (L7_MAX_INTERFACE_COUNT-1)
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_MIRROR_INTF_MIN       1
#define L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_MIRROR_INTF_MAX       (L7_MAX_INTERFACE_COUNT-1)

/* User Keyword Translations for Layer 4 Protocols */
#define L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_ICMP       1
#define L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_IGMP       2
#define L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_IP         255
#define L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_TCP        6
#define L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_UDP        17
                                                                      
/* User Keyword Translations for Layer 4 Port Numbers */
#define L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_DOMAIN     53
#define L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_ECHO       7
#define L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_FTP        21
#define L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_FTPDATA    20
#define L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_HTTP       80
#define L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_SMTP       25
#define L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_SNMP       161
#define L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_TELNET     23
#define L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_TFTP       69
#define L7_USMDB_MIB_DIFFSERV_LAYER_4_PORT_NUM_WWW        80
  
/* User Keyword Translations for IP DSCP Values */
#define L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF11                10 
#define L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF12                12 
#define L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF13                14 
#define L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF21                18 
#define L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF22                20 
#define L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF23                22 
#define L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF31                26 
#define L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF32                28 
#define L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF33                30 
#define L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF41                34 
#define L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF42                36 
#define L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF43                38 
#define L7_USMDB_MIB_DIFFSERV_IP_DSCP_BE                  0  
#define L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS0                 0  
#define L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS1                 8  
#define L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS2                 16 
#define L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS3                 24 
#define L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS4                 32 
#define L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS5                 40 
#define L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS6                 48 
#define L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS7                 56 
#define L7_USMDB_MIB_DIFFSERV_IP_DSCP_EF                  46 
  
/*
**********************************************************************
*   MIB DEFINED ENUMERATIONS
**********************************************************************
*/

/* Values for DiffServ Class type */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_NONE = 0,
  L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ALL,
  L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ANY,
  L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL,

  L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_TOTAL        /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_t;

/* Values for DiffServ Class Layer3 Protocol type */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_NONE = 0,
  L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4,
  L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6,

  L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_TOTAL  /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_t;

/* Values for DiffServ Class ACL type */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_NONE = 0,
  L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_IP,
  L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_MAC,

  L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_TOTAL     /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_t;

/* Values for DiffServ Class Rule types supported as match conditions */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE = 0,

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2,

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_TOTAL   /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t;

/* Values for DiffServ Policy type */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_NONE = 0,
  L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN,
  L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT,

  L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_TOTAL       /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t;

/* Values for DiffServ Policy Attribute statement type */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE = 0,

  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_BANDWIDTH,      /* deprecated */
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_EXPEDITE,       /* deprecated */
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_RANDOMDROP,     /* deprecated */
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_SHAPE_AVERAGE,  /* deprecated */
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_SHAPE_PEAK,     /* deprecated */

  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_TOTAL  /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t;

/* Values for DiffServ Bandwidth units type */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_BW_UNITS_NONE = 0,
  L7_USMDB_MIB_DIFFSERV_BW_UNITS_KBPS,
  L7_USMDB_MIB_DIFFSERV_BW_UNITS_PERCENT

} L7_USMDB_MIB_DIFFSERV_BW_UNITS_t;

/* Values for DiffServ Police Action type */
/* NOTE: Update DiffServ defaults in defaultconfig.h when changing this enum. */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_NONE = 0,

  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP,
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS,
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2,
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2,
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP,
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC,
  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND,

  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_TOTAL        /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_POLICE_ACT_t;

#define L7_USMDB_MIB_DIFFSERV_POLICE_ACT_IS_MARKING(_act)   \
  ((((_act) >= L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS) && \
    ((_act) <= L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC))  \
   ? L7_TRUE : L7_FALSE)

/* Values for DiffServ Police Color Level type */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_LEVEL_NONE = 0,

  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_LEVEL_CONFORM,
  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_LEVEL_EXCEED,

  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_LEVEL_TOTAL  /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_LEVEL_t;

/* Values for DiffServ Police Color Aware type */
/* NOTE: Update DiffServ defaults in defaultconfig.h when changing this enum. */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_NONE = 0,

  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND,     /* color-blind mode           */
  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS,       /* aware COS (802.1p)         */
  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2,      /* aware COS2 (802.1p)        */
  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP,    /* aware IP DSCP              */
  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC,    /* aware IP Prec              */
  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_UNUSED,    /* exceed color not specified */

  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_TOTAL      /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_t;

#define L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_MODE_IS_AWARE(_mode) \
  ((((_mode) >= L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS) &&      \
    ((_mode) <= L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC))     \
   ? L7_TRUE : L7_FALSE)


/*
**********************************************************************
*   MIB DEFINED SET-TEST OBJECT ID ENUMERATIONS
**********************************************************************
*/

/* Enumerated values for Class Table set-test object ids */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_NAME = 1,
  L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_TYPE,
  L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_L3_PROTO,
  L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_ACL_TYPE,
  L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_ACL_NUMBER,
  L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_STORAGE_TYPE,
  L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_ROW_STATUS,

  L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_TOTAL /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_TYPE_t;

/* Enumerated values for Class Rule Table set-test object ids */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_NONE = 0,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_ENTRY_TYPE,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_COS,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_COS2,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTIP_ADDR,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTIP_MASK,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTIPV6_ADDR,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTIPV6_ADDR_PLEN,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTL4PORT_START,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTL4PORT_END,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTMAC_ADDR,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTMAC_MASK,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_ETYPE_KEYID,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_ETYPE_VALUE,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_FLOWLBLV6,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_IPDSCP,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_IPPRECEDENCE,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_IPTOS_BITS,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_IPTOS_MASK,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_PROTOCOL_NUM,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_REFCLASS_INDEX,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCIP_ADDR,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCIP_MASK,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCIPV6_ADDR,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCIPV6_ADDR_PLEN,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCL4PORT_START,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCL4PORT_END,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCMAC_ADDR,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCMAC_MASK,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_VLANID_START,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_VLANID_END,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_VLANID2_START,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_VLANID2_END,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_EXCLUDE_FLAG,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_STORAGE_TYPE,
  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_ROW_STATUS,

  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_TOTAL /* number of enum entries*/

} L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_TYPE_t;

/* Enumerated values for Policy Table set-test object ids */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_NAME = 1,
  L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_TYPE,
  L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_STORAGE_TYPE,
  L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_ROW_STATUS,

  L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_TOTAL /* number of enum entries*/

} L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_TYPE_t;

/* Enumerated values for Policy-Class Instance Table set-test object ids */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_CLASS_INDEX = 1,
  L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_STORAGE_TYPE,
  L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_ROW_STATUS,

  L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_TOTAL /*number of enum entries*/

} L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_TYPE_t;

/* Enumerated values for Policy Attribute Table set-test object ids */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_ENTRY_TYPE = 1,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_ASSIGN_QUEUE_QID,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_BANDWIDTH_CRATE,          /* deprecated */
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_BANDWIDTH_CRATE_UNITS,    /* deprecated */
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_EXPEDITE_CRATE,           /* deprecated */
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_EXPEDITE_CRATE_UNITS,     /* deprecated */
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_EXPEDITE_CBURST,          /* deprecated */
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_COSVAL,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_COS2VAL,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_COS_AS_COS2,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_IPDSCPVAL,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_IPPRECEDENCEVAL,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MIRROR_INTF,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_CRATE,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_CBURST,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_CONFORM_ACT,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_CONFORM_VAL,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_NONCONFORM_ACT,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_NONCONFORM_VAL,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_COLOR_CONFORM_INDEX,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_COLOR_CONFORM_MODE,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_COLOR_CONFORM_VAL,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_CRATE,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_CBURST,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_EBURST,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_CONFORM_ACT,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_CONFORM_VAL,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_EXCEED_ACT,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_EXCEED_VAL,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_NONCONFORM_ACT,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_NONCONFORM_VAL,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_CONFORM_INDEX,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_CONFORM_MODE,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_CONFORM_VAL,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_EXCEED_INDEX,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_EXCEED_MODE,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_EXCEED_VAL,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_CRATE,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_CBURST,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_PRATE,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_PBURST,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_CONFORM_ACT,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_CONFORM_VAL,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_EXCEED_ACT,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_EXCEED_VAL,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_NONCONFORM_ACT,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_NONCONFORM_VAL,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_CONFORM_INDEX,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_CONFORM_MODE,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_CONFORM_VAL,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_EXCEED_INDEX,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_EXCEED_MODE,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_EXCEED_VAL,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_RANDOMDROP_MIN_THRESH,                /* deprecated */
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_RANDOMDROP_MAX_THRESH,                /* deprecated */
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_RANDOMDROP_MAX_DROP_PROB,             /* deprecated */
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_RANDOMDROP_SAMPLING_RATE,             /* deprecated */ 
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_RANDOMDROP_DECAY_EXPONENT,            /* deprecated */
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_REDIRECT_INTF,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_SHAPE_AVERAGE_CRATE,                  /* deprecated */
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_SHAPE_PEAK_CRATE,                     /* deprecated */
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_SHAPE_PEAK_PRATE,                     /* deprecated */
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_STORAGE_TYPE,
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_ROW_STATUS,

  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_TOTAL /*number of enum entries*/

} L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_TYPE_t;

/* Enumerated values for Service Table set-test object ids */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_POLICY_INDEX = 1,
  L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_STORAGE_TYPE,
  L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_ROW_STATUS,

  L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_TOTAL /* number of enum entries*/

} L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_TYPE_t;

/*
==============================
==============================
==============================

   GENERAL STATUS GROUP API

==============================
==============================
==============================
*/
 
/*************************************************************************
* @purpose  Get the value of the DiffServ administrative mode
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    pMode       @b{(output)} Pointer to mode value      
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServGenAdminModeGet(L7_uint32 UnitIndex, L7_uint32* pMode);

/*************************************************************************
* @purpose  Set the value of the DiffServ administrative mode
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    mode        @b{(input)} Mode value (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServGenAdminModeSet(L7_uint32 UnitIndex, L7_uint32 mode);

/*************************************************************************
* @purpose  Get the current size of the Class Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    pSize       @b{(output)} Pointer to table size value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServGenClassTableSizeGet(L7_uint32 UnitIndex, 
                                          L7_uint32 *pSize);

/*************************************************************************
* @purpose  Get the maximum size of the Class Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    pMax        @b{(output)} Pointer to table maximum value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServGenClassTableMaxGet(L7_uint32 UnitIndex, 
                                         L7_uint32 *pMax);

/*************************************************************************
* @purpose  Get the current size of the Class Rule Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    pSize       @b{(output)} Pointer to table size value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServGenClassRuleTableSizeGet(L7_uint32 UnitIndex, 
                                              L7_uint32 *pSize);

/*************************************************************************
* @purpose  Get the maximum size of the Class Rule Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    pMax        @b{(output)} Pointer to table maximum value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServGenClassRuleTableMaxGet(L7_uint32 UnitIndex, 
                                             L7_uint32 *pMax);

/*************************************************************************
* @purpose  Get the current size of the Policy Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    pSize       @b{(output)} Pointer to table size value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServGenPolicyTableSizeGet(L7_uint32 UnitIndex, 
                                           L7_uint32 *pSize);

/*************************************************************************
* @purpose  Get the maximum size of the Policy Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    pMax        @b{(output)} Pointer to table maximum value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServGenPolicyTableMaxGet(L7_uint32 UnitIndex, 
                                          L7_uint32 *pMax);

/*************************************************************************
* @purpose  Get the current size of the Policy-Class Instance Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    pSize       @b{(output)} Pointer to table size value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServGenPolicyInstTableSizeGet(L7_uint32 UnitIndex, 
                                               L7_uint32 *pSize);

/*************************************************************************
* @purpose  Get the maximum size of the Policy-Class Instance Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    pMax        @b{(output)} Pointer to table maximum value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServGenPolicyInstTableMaxGet(L7_uint32 UnitIndex, 
                                              L7_uint32 *pMax);

/*************************************************************************
* @purpose  Get the current size of the Policy Attribute Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    pSize       @b{(output)} Pointer to table size value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServGenPolicyAttrTableSizeGet(L7_uint32 UnitIndex, 
                                               L7_uint32 *pSize);

/*************************************************************************
* @purpose  Get the maximum size of the Policy Attribute Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    pMax        @b{(output)} Pointer to table maximum value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServGenPolicyAttrTableMaxGet(L7_uint32 UnitIndex, 
                                              L7_uint32 *pMax);

/*************************************************************************
* @purpose  Get the current size of the Service Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    pSize       @b{(output)} Pointer to table size value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServGenServiceTableSizeGet(L7_uint32 UnitIndex, 
                                            L7_uint32 *pSize);

/*************************************************************************
* @purpose  Get the maximum size of the Service Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    pMax        @b{(output)} Pointer to table maximum value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServGenServiceTableMaxGet(L7_uint32 UnitIndex, 
                                           L7_uint32 *pMax);


/*
=====================
=====================
=====================

   CLASS TABLE API

=====================
=====================
=====================
*/

/*************************************************************************
* @purpose  Obtain the next available index value to be used when creating  
*           a new row in the Class Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    pClassIndex @b{(output)} Pointer to class index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An L7_SUCCESS return with an output *pClassIndex value of 0
*           means the table is full.
*
* @notes    The index value remains free until used in a subsequent create
*           request.  When two or more requestors get the same index value,
*           only the first one to successfully create a new row in this table
*           gets to keep it.  All other requestors' create requests will
*           fail due to the index being in use, and they must call this
*           function again to get a new value.
*
* @notes    Although an index value is considered in use once a
*           successful row create occurs, the next index value is not
*           revealed until the row is either completed or deleted.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassIndexNext(L7_uint32 UnitIndex, 
                                    L7_uint32 *pClassIndex);

/*************************************************************************
* @purpose  Retrieve the maximum class index value allowed for the 
*           Class Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    pIndexMax   @b{(output)} Pointer to index max output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is purely informational.  The corresponding table 'IndexNext'
*           function must be used to obtain the current index value.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassIndexMaxGet(L7_uint32 UnitIndex, 
                                      L7_uint32 *pIndexMax);

/*************************************************************************
* @purpose  Create a new row in the Class Table for the specified index
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    activateRow @b{(input)} Activate row when ready flag
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR               Index value in use, row not complete
* @returns  L7_ALREADY_CONFIGURED  Index value in use, row complete
* @returns  L7_REQUEST_DENIED      Config change not approved
* @returns  L7_FAILURE
*
* @notes    An L7_ERROR response means the class index value is currently 
*           in use, but the row is not complete.  This is typically
*           caused when multiple requestors obtain the same "index next"
*           value and race to create the new row.  The caller should
*           get a new index value and re-attempt to create the row.
*
* @notes    An L7_ALREADY_CONFIGURED response means the class index value
*           is currently in use for a completed row.  The caller should
*           get a new index value and re-attempt to create the row.
*
* @notes    An L7_REQUEST_DENIED response means another application   
*           has denied approval of this configuration change.
*
* @notes    The row status is initially set to 'not ready' and remains that 
*           way until every object in the row contains a valid value.  If the
*           activateRow parameter is set to L7_TRUE, the row status will then
*           be automatically updated to 'active'.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassCreate(L7_uint32 UnitIndex, L7_uint32 classIndex,
                                 L7_BOOL activateRow);

/*************************************************************************
* @purpose  Delete a row from the Class Table for the specified index
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR               Row does not exist for the specified index, or
*                                    is currently being referenced
* @returns  L7_REQUEST_DENIED      Config change not approved
* @returns  L7_FAILURE
*
* @notes    A class cannot be deleted if it is currently referenced by one
*           or more policies.
*
* @notes    A class cannot be deleted if it is referenced by another class.
*
* @notes    An L7_REQUEST_DENIED response means another application   
*           has denied approval of this configuration change.
*
* @notes    Deletes all class rules defined for this class.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassDelete(L7_uint32 UnitIndex, L7_uint32 classIndex);

/*********************************************************************
* @purpose  Verify that a Class Table row exists for the specified index
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassGet(L7_uint32 UnitIndex, L7_uint32 classIndex);

/*********************************************************************
* @purpose  Determine next sequential row in the Class Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    prevClassIndex  @b{(input)} Class index to begin search
* @param    pClassIndex @b{(output)} Pointer to next sequential class index
*                                      value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Reached end of table
* @returns  L7_FAILURE  
*
* @notes    A prevClassIndex value of 0 is used to find the first entry in
*           the table.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassGetNext(L7_uint32 UnitIndex, 
                                  L7_uint32 prevClassIndex, 
                                  L7_uint32 *pClassIndex);

/*********************************************************************
* @purpose  Populates an array with the class indexes for all entries in 
*           Class Table sorted such that any class referring to another
*           class appears after that class.
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    orderedClassList  @b{(output)} an array to receive the sorted class
*                                          indexes
* @param    pClassIndex @b{(output)} the number of entries in the output array
*
* @returns  L7_SUCCESS  at least one entry in output array
* @returns  L7_FAILURE  no entries found
*
* @notes    The array for the list must contain at least L7_DIFFSERV_CLASS_LIM
*           entries.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassOrderedListGet(L7_uint32 UnitIndex, 
                                         L7_uint32 *orderedClassList, 
                                         L7_uint32 *classCount);

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Class Table 
*           to determine its legitimacy
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    objectId    @b{(input)} Object identifier
* @param    pValue      @b{(input)} Pointer to proposed value to be set
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Proposed value invalid
* @returns  L7_FAILURE  
*
* @notes    Only tests objects with access types READ-CREATE or READ-WRITE;
*           all other access types return L7_FAILURE.
*
* @notes    Caller must use one of the _SETTEST_OID_TYPE object identifiers
*           only (i.e., do not use the _TABLE_OBJECT type object names here).
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassSetTest(L7_uint32 UnitIndex, L7_uint32 classIndex, 
    L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_TYPE_t objectId, void *pValue);

/*********************************************************************
* @purpose  Get the class name
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    pStringPtr  @b{(output)} Pointer to a class name string
* @param    pLen        @b{(input/output)} Pointer to length of the string  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established or buffer length is 
*                       too short
* @returns  L7_FAILURE  
*
* @notes    `pStringPtr` points to a char buffer of minimum length equals to
*            L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassNameGet(L7_uint32 UnitIndex, L7_uint32 classIndex,
                                  L7_uchar8 *pStringPtr, L7_uint32 *pLen);

/*********************************************************************
* @purpose  Set the class name
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    stringPtr   @b{(input)} Class name string pointer 
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassNameSet(L7_uint32 UnitIndex, L7_uint32 classIndex,
                                  L7_uchar8 *stringPtr);

/*********************************************************************
* @purpose  Translate a class name into its associated Class Table index
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    stringPtr   @b{(input)} Class name string pointer
* @param    pClassIndex @b{(output)} Pointer to class index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is a supplemental function provided to assist the user
*           interface code with name-to-index translation.  Use the
*           name 'get' function to translate in the reverse direction.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassNameToIndex(L7_uint32 UnitIndex, 
                                      L7_uchar8 *stringPtr, 
                                      L7_uint32 *pClassIndex);

/*********************************************************************
* @purpose  Get the type of class definition
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    pClassType  @b{(output)} Pointer to class type value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    The class type indicates how the various class match conditions
*           are evaluated.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassTypeGet(L7_uint32 UnitIndex, L7_uint32 classIndex,
                              L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_t *pClassType);

/*********************************************************************
* @purpose  Set the type of class definition
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classType   @b{(input)} Class type value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    The class type indicates how the various class match conditions
*           are evaluated.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassTypeSet(L7_uint32 UnitIndex, L7_uint32 classIndex,
                              L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_t classType);


/*********************************************************************
* @purpose  Get the L3 protocol of class definition
*
* @param    UnitIndex      @b{(input)} System unit number
* @param    classIndex     @b{(input)} Class index
* @param    pClassL3Proto  @b{(output)} Pointer to class L3 protocol value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    The class L3 protocol indicates how the various class match conditions
*           are evaluated.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassL3ProtoGet(L7_uint32 UnitIndex, L7_uint32 classIndex,
                                     L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_t *pClassL3Proto);

/*********************************************************************
* @purpose  Set the L3 protocol of class definition
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classL3Proto   @b{(input)} Class L3 protocol value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    The class L3 protocol indicates how the various class match conditions
*           are evaluated.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassL3ProtoSet(L7_uint32 UnitIndex, L7_uint32 classIndex,
                                     L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_t classL3Proto);

/*********************************************************************
* @purpose  Get the ACL type used to define the class match conditions
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    pAclType    @b{(output)} Pointer to ACL type value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Class type not 'acl'
* @returns  L7_FAILURE  
*
* @notes    This function is only meaningful for a class type of 'acl';
*           an L7_ERROR is returned for classes of any other type.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassAclTypeGet(L7_uint32 UnitIndex, L7_uint32 classIndex,
                                    L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_t *pAclType);

/*********************************************************************
* @purpose  Set the ACL type used to define the class match conditions
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    aclType     @b{(input)} ACL type value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Class type not 'acl'
* @returns  L7_FAILURE  
*
* @notes    This function is only meaningful for a class type of 'acl';
*           an L7_ERROR is returned for classes of any other type.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassAclTypeSet(L7_uint32 UnitIndex, L7_uint32 classIndex,
                                    L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_t aclType);

/*********************************************************************
* @purpose  Get the ACL number used to define the class match conditions
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    pAclNum     @b{(output)} Pointer to ACL number value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Class type not 'acl'
* @returns  L7_FAILURE  
*
* @notes    This function is only meaningful for a class type of 'acl';
*           an L7_ERROR is returned for classes of any other type.
*
* @notes    The ACL number is only meaningful in accordance with the ACL type.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassAclNumGet(L7_uint32 UnitIndex, L7_uint32 classIndex,
                                    L7_uint32 *pAclNum);

/*********************************************************************
* @purpose  Set the ACL number used to define the class match conditions
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    aclNum      @b{(input)} ACL number value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Class type not 'acl'
* @returns  L7_FAILURE  
*
* @notes    This function is only meaningful for a class type of 'acl';
*           an L7_ERROR is returned for classes of any other type.
*
* @notes    The ACL number is only meaningful in accordance with the ACL type.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassAclNumSet(L7_uint32 UnitIndex, L7_uint32 classIndex,
                                    L7_uint32 aclNum);

/*********************************************************************
* @purpose  Get the storage type for the specified Class Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    pValue      @b{(output)} Pointer to storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassStorageTypeGet(L7_uint32 UnitIndex,
                                         L7_uint32 classIndex,
                              L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t *pValue);

/*********************************************************************
* @purpose  Set the storage type for the specified Class Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    value       @b{(input)} Storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassStorageTypeSet(L7_uint32 UnitIndex,
                                         L7_uint32 classIndex,
                              L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t value);

/*********************************************************************
* @purpose  Get the current status of the specified Class Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    pValue      @b{(output)} Pointer to row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRowStatusGet(L7_uint32 UnitIndex,
                                       L7_uint32 classIndex,
                              L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t *pValue);

/*********************************************************************
* @purpose  Set the current status of the specified Class Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    value       @b{(input)} Row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRowStatusSet(L7_uint32 UnitIndex,
                                       L7_uint32 classIndex,
                              L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t value);

/*********************************************************************
* @purpose  Provide the reference class index, if any, used by the specified
*           class
*
* @param    UnitIndex       @b{(input)}  System unit number
* @param    classIndex      @b{(input)}  Class index
* @param    pRefClassIndex  @b{(output)} Pointer to reference class index value
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Class does not contain a reference class
* @returns  L7_FAILURE
*
* @notes    This is a supplemental function provided to assist the user
*           interface code with obtaining the reference class used by 
*           a particular class.  This simplifies the API interaction
*           for this translation, since a class can reference at most
*           one other class.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassToRefClass(L7_uint32 UnitIndex,
                                     L7_uint32 classIndex,
                                     L7_uint32 *pRefClassIndex);

/*********************************************************************
* @purpose  Check if class definition is suitable for inclusion by specified
*           policy
*
* @param    UnitIndex       @b{(input)}  System unit number
* @param    classIndex      @b{(input)}  Class index
* @param    policyIndex     @b{(input)}  Policy index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is a supplemental function provided to assist the user
*           interface code with determining if a class can be assigned
*           to a given policy.  Some platforms impose restrictions on
*           the classifiers that can be used in a certain policy direction
*           (in/out).
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassValidForPolicyCheck(L7_uint32 UnitIndex,
                                              L7_uint32 classIndex,
                                              L7_uint32 policyIndex);


/*
==========================
==========================
==========================

   CLASS RULE TABLE API

==========================
==========================
==========================
*/

/*************************************************************************
* @purpose  Obtain the next available index value to be used when creating  
*           a new row in the Class Rule Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    pClassRuleIndex @b{(output)} Pointer to index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An L7_SUCCESS return with an output *pClassRuleIndex value
*           of 0 means the table is full.
*
* @notes    The index value remains free until used in a subsequent create
*           request.  When two or more requestors get the same index value,
*           only the first one to successfully create a new row in this table
*           gets to keep it.  All other requestors' create requests will
*           fail due to the index being in use, and they must call this
*           function again to get a new value.
*
* @notes    Although an index value is considered in use once a
*           successful row create occurs, the next index value is not
*           revealed until the row is either completed or deleted.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleIndexNext(L7_uint32 UnitIndex, 
                                        L7_uint32 classIndex,
                                        L7_uint32 *pClassRuleIndex);

/*************************************************************************
* @purpose  Retrieve the maximum class rule index value allowed for the 
*           Class Rule Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    pIndexMax   @b{(output)} Pointer to index max output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is purely informational.  The corresponding table 'IndexNext'
*           function must be used to obtain the current index value.
*
* @notes    There is a difference between this maximum index and the 
*           maximum table size.  The latter encompasses independent
*           sets of index maximums.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleIndexMaxGet(L7_uint32 UnitIndex, 
                                          L7_uint32 *pIndexMax);

/*************************************************************************
* @purpose  Create a new row in the Class Rule Table for the specified indexes
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    activateRow @b{(input)} Activate row when ready flag
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR               Rule index value in use, row not complete
* @returns  L7_ALREADY_CONFIGURED  Rule index value in use, row complete
* @returns  L7_FAILURE
*
* @notes    An L7_ERROR response means the class rule index value is currently
*           in use, but the row is not complete.  This is typically
*           caused when multiple requestors obtain the same "index next"
*           value and race to create the new row.  The caller should
*           get a new index value and re-attempt to create the row.
*
* @notes    An L7_ALREADY_CONFIGURED response means the class rule index
*           value is currently in use for a completed row.  The caller
*           should get a new index value and re-attempt to create the row.
*
* @notes    The row status is initially set to 'not ready' and remains that 
*           way until every object in the row contains a valid value.  If the
*           activateRow parameter is set to L7_TRUE, the row status will then
*           be automatically updated to 'active'.
*
* @notes    An attempt to create a class rule for a class type 'acl' is
*           rejected with an L7_FAILURE response.  Class type 'acl' rules are
*           automatically created when the class is created.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleCreate(L7_uint32 UnitIndex, L7_uint32 classIndex,
                                     L7_uint32 classRuleIndex,
                                     L7_BOOL activateRow);

/*************************************************************************
* @purpose  Delete a row from the Class Rule Table for the specified index
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE
*
* @notes    An attempt to delete a class rule for a class type 'acl' is
*           rejected with an L7_FAILURE response.  Class type 'acl' rules are
*           automatically deleted when the class is deleted.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleDelete(L7_uint32 UnitIndex, L7_uint32 classIndex,
                                     L7_uint32 classRuleIndex);

/*********************************************************************
* @purpose  Verify that a Class Rule Table row exists for the specified index
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleGet(L7_uint32 UnitIndex, L7_uint32 classIndex,
                                  L7_uint32 classRuleIndex);

/*********************************************************************
* @purpose  Determine next sequential row in the Class Rule Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    prevClassIndex  @b{(input)} Class index to begin search
* @param    prevClassRuleIndex  @b{(input)} Class rule index to begin search
* @param    pClassIndex @b{(output)} Pointer to next sequential class index
*                                      value
* @param    pClassRuleIndex @b{(output)} Pointer to next sequential class rule
*                                          index value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Reached end of table
* @returns  L7_FAILURE  
*
* @notes    A prevClassIndex of 0 is used to find the first class in the table.
*
* @notes    A prevClassRuleIndex of 0 is used to find the first rule relative
*           to the specified prevClassIndex.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleGetNext(L7_uint32 UnitIndex, 
                                      L7_uint32 prevClassIndex, 
                                      L7_uint32 prevClassRuleIndex,
                                      L7_uint32 *pClassIndex,
                                      L7_uint32 *pClassRuleIndex);

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Class Rule  
*           Table to determine its legitimacy
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    objectId    @b{(input)} Object identifier
* @param    pValue      @b{(input)} Pointer to proposed value to be set
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Proposed value invalid
* @returns  L7_FAILURE  
*
* @notes    Only tests objects with access types READ-CREATE or READ-WRITE;
*           all other access types return L7_FAILURE.
*
* @notes    Caller must use one of the _SETTEST_OID_TYPE object identifiers
*           only (i.e., do not use the _TABLE_OBJECT type object names here).
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleSetTest(L7_uint32 UnitIndex, L7_uint32 classIndex,
                                      L7_uint32 classRuleIndex,
    L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_TYPE_t objectId, void *pValue);

/*********************************************************************
* @purpose  Get the Class Rule match entry type
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pEntryType  @b{(output)} Pointer to entry type value  
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    The entry type is essential to determine which one of the
*           individual match conditions is defined for this rule.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchEntryTypeGet(L7_uint32 UnitIndex, 
                                                L7_uint32 classIndex,
                                                L7_uint32 classRuleIndex,
                        L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t *pEntryType);

/*********************************************************************
* @purpose  Set the Class Rule match entry type
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    entryType   @b{(input)} Entry type value  
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    The entry type MUST be set before any of the individual match
*           match conditions can be set for this rule.  The entry type value
*           determines which match get/set commands are valid for this rule.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchEntryTypeSet(L7_uint32 UnitIndex, 
                                                L7_uint32 classIndex,
                                                L7_uint32 classRuleIndex,
                        L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t entryType);

/*********************************************************************
* @purpose  Get the Class of Service (COS) match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pCosVal     @b{(output)} Pointer to COS value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _COS.
*
* @notes    The Class of Service field is defined as the three-bit user
*           priority field in the first/outer 802.1Q tag header of a tagged
*           Ethernet frame.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchCosGet(L7_uint32 UnitIndex, 
                                          L7_uint32 classIndex,
                                          L7_uint32 classRuleIndex,
                                          L7_uint32 *pCosVal);

/*********************************************************************
* @purpose  Set the Class of Service (COS) match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    cosVal      @b{(input)} COS value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _COS.
*
* @notes    The Class of Service field is defined as the three-bit user
*           priority field in the first/outer 802.1Q tag header of a tagged
*           Ethernet frame.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchCosSet(L7_uint32 UnitIndex, 
                                          L7_uint32 classIndex,
                                          L7_uint32 classRuleIndex,
                                          L7_uint32 cosVal);

/*********************************************************************
* @purpose  Get the Secondary Class of Service (COS2) match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pCosVal     @b{(output)} Pointer to COS2 value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _COS2.
*
* @notes    The Secondary Class of Service field is defined as the three-bit 
*           priority field in the second/inner 802.1Q tag header of a double
*           VLAN tagged Ethernet frame.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchCos2Get(L7_uint32 UnitIndex, 
                                           L7_uint32 classIndex,
                                           L7_uint32 classRuleIndex,
                                           L7_uint32 *pCosVal);

/*********************************************************************
* @purpose  Set the Secondary Class of Service (COS2) match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    cosVal      @b{(input)} COS2 value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _COS2.
*
* @notes    The Secondary Class of Service field is defined as the three-bit 
*           priority field in the second/inner 802.1Q tag header of a double
*           VLAN tagged Ethernet frame.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchCos2Set(L7_uint32 UnitIndex, 
                                           L7_uint32 classIndex,
                                           L7_uint32 classRuleIndex,
                                           L7_uint32 cosVal);

/*********************************************************************
* @purpose  Get the destination IP address match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pIpAddr     @b{(output)} Pointer to IP address value  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _DSTIP.
*
* @notes    A valid IP address match condition requires both the address
*           and mask values.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchDstIpAddrGet(L7_uint32 UnitIndex, 
                                                L7_uint32 classIndex,
                                                L7_uint32 classRuleIndex,
                                                L7_uint32 *pIpAddr);

/*********************************************************************
* @purpose  Set the destination IP address match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    ipAddr      @b{(input)} IP address value  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _DSTIP.
*
* @notes    A valid IP address match condition requires both the address
*           and mask values.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchDstIpAddrSet(L7_uint32 UnitIndex, 
                                                L7_uint32 classIndex,
                                                L7_uint32 classRuleIndex,
                                                L7_uint32 ipAddr);

/*********************************************************************
* @purpose  Get the destination IP address mask match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pIpMask     @b{(output)} Pointer to IP mask value  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _DSTIP.
*
* @notes    A valid IP address match condition requires both the address
*           and mask values.
* 
* @notes    A non-contiguous mask value is permitted (all b'0' are
*           interpreted as 'dont care' bits).
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchDstIpMaskGet(L7_uint32 UnitIndex, 
                                                L7_uint32 classIndex,
                                                L7_uint32 classRuleIndex,
                                                L7_uint32 *pIpMask);

/*********************************************************************
* @purpose  Set the destination IP address mask match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    ipMask      @b{(input)} IP address mask value  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _DSTIP.
*
* @notes    A valid IP address match condition requires both the address
*           and mask values.
* 
* @notes    A non-contiguous mask value is permitted (all b'0' are
*           interpreted as 'dont care' bits).
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchDstIpMaskSet(L7_uint32 UnitIndex, 
                                                L7_uint32 classIndex,
                                                L7_uint32 classRuleIndex,
                                                L7_uint32 ipMask);

/*********************************************************************
* @purpose  Get the destination IPv6 address match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pIpv6Addr     @b{(output)} Pointer to IPv6 address value  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _DSTIPV6.
*
* @notes    A valid IPv6 address match condition requires both the address
*           and prefix length values.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchDstIpv6AddrGet(L7_uint32 UnitIndex, 
                                                  L7_uint32 classIndex,
                                                  L7_uint32 classRuleIndex,
                                                  L7_in6_addr_t *pIpv6Addr);

/*********************************************************************
* @purpose  Set the destination IPv6 address match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    ipAddr      @b{(input)} IP address value  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _DSTIPV6.
*
* @notes    A valid IPv6 address match condition requires both the address
*           and prefix length values.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchDstIpv6AddrSet(L7_uint32 UnitIndex, 
                                                  L7_uint32 classIndex,
                                                  L7_uint32 classRuleIndex,
                                                  L7_in6_addr_t *ipv6Addr);

/*********************************************************************
* @purpose  Get the destination IPv6 prefix length value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pIpv6PrefLength @b{(output)} Pointer to IPv6 prefix length  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _DSTIPV6_PLEN.
*
* @notes    A valid IPv6 address match condition requires both the address
*           and prefix length values.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchDstIpv6PrefLenGet(L7_uint32 UnitIndex, 
                                                     L7_uint32 classIndex,
                                                     L7_uint32 classRuleIndex,
                                                     L7_uint32 *pIpv6PrefLength);

/*********************************************************************
* @purpose  Set the destination IPv6 prefix length value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    ipv6PrefLength  @b{(input)} IPv6 address value  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _DSTIPV6_PLEN.
*
* @notes    A valid IPv6 address match condition requires both the address
*           and mask values.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchDstIpv6PrefLenSet(L7_uint32 UnitIndex, 
                                                     L7_uint32 classIndex,
                                                     L7_uint32 classRuleIndex,
                                                     L7_uint32 ipv6PrefLength);

/*********************************************************************
* @purpose  Get the destination layer 4 port range start match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pPortNum    @b{(output)} Pointer to port number value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _DSTL4PORT.
*
* @notes    A valid layer 4 port match condition requires both the start
*           and end port numbers, with start <= end.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchDstL4PortStartGet(L7_uint32 UnitIndex, 
                                                     L7_uint32 classIndex,
                                                     L7_uint32 classRuleIndex,
                                                     L7_uint32 *pPortNum);

/*********************************************************************
* @purpose  Set the destination layer 4 port range start match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    portNum     @b{(input)} Port number value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _DSTL4PORT.
*
* @notes    A valid layer 4 port match condition requires both the start
*           and end port numbers, with start <= end.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchDstL4PortStartSet(L7_uint32 UnitIndex, 
                                                     L7_uint32 classIndex,
                                                     L7_uint32 classRuleIndex,
                                                     L7_uint32 portNum);

/*********************************************************************
* @purpose  Get the destination layer 4 port range end match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pPortNum    @b{(output)} Pointer to port number value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _DSTL4PORT.
*
* @notes    A valid layer 4 port match condition requires both the start
*           and end port numbers, with start <= end.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchDstL4PortEndGet(L7_uint32 UnitIndex, 
                                                   L7_uint32 classIndex,
                                                   L7_uint32 classRuleIndex,
                                                   L7_uint32 *pPortNum);

/*********************************************************************
* @purpose  Set the destination layer 4 port range end match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    portNum     @b{(input)} Port number value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _DSTL4PORT.
*
* @notes    A valid layer 4 port match condition requires both the start
*           and end port numbers, with start <= end.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchDstL4PortEndSet(L7_uint32 UnitIndex, 
                                                   L7_uint32 classIndex,
                                                   L7_uint32 classRuleIndex,
                                                   L7_uint32 portNum);

/*********************************************************************
* @purpose  Get the destination MAC address match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pMacAddrPtr @b{(output)} Pointer to MAC address 
* @param    pLen        @b{(input/output)} Pointer to length of the MAC address
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type or buffer 
*                       length is too short
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _DSTMAC.
*
* @notes    A valid MAC address match condition requires both the address
*           and mask values.
* 
* @notes    `pMacAddrPtr` points to a char buffer of minimum length equals to
*            L7_MAC_ADDR_LEN
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchDstMacAddrGet(L7_uint32 UnitIndex, 
                                                 L7_uint32 classIndex,
                                                 L7_uint32 classRuleIndex,
                                                 L7_uchar8 *pMacAddrPtr,
                                                 L7_uint32 *pLen);

/*********************************************************************
* @purpose  Set the destination MAC address match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    macAddrPtr  @b{(input)} MAC address pointer  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _DSTMAC.
*
* @notes    A valid MAC address match condition requires both the address
*           and mask values.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchDstMacAddrSet(L7_uint32 UnitIndex, 
                                                 L7_uint32 classIndex,
                                                 L7_uint32 classRuleIndex,
                                                 L7_uchar8 *macAddrPtr);

/*********************************************************************
* @purpose  Get the destination MAC address mask match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pMacMaskPtr @b{(output)} Pointer to MAC mask 
* @param    pLen        @b{(input/output)} Pointer to length of the MAC mask
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type or buffer 
*                       length is too short
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _DSTMAC.
*
* @notes    A valid IP address match condition requires both the address
*           and mask values.
* 
* @notes    A non-contiguous mask value is permitted (all b'0' are
*           interpreted as 'dont care' bits).
* 
* @notes    `pMacMaskPtr` points to a char buffer of minimum length equals to
*            L7_MAC_ADDR_LEN
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchDstMacMaskGet(L7_uint32 UnitIndex, 
                                                 L7_uint32 classIndex,
                                                 L7_uint32 classRuleIndex,
                                                 L7_uchar8 *pMacMaskPtr,
                                                 L7_uint32 *pLen);

/*********************************************************************
* @purpose  Set the destination MAC address mask match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    macMaskPtr  @b{(input)} MAC mask pointer
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _DSTMAC.
*
* @notes    A valid IP address match condition requires both the address
*           and mask values.
* 
* @notes    A non-contiguous mask value is permitted (all b'0' are
*           interpreted as 'dont care' bits).
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchDstMacMaskSet(L7_uint32 UnitIndex, 
                                                 L7_uint32 classIndex,
                                                 L7_uint32 classRuleIndex,
                                                 L7_uchar8 *macMaskPtr);

/*********************************************************************
* @purpose  Get the Ethertype keyword match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pEtypeKey   @b{(output)} Pointer to Ethertype keyword
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _ETYPE.
*
* @notes    All Ethertype match rules have a keyword.  A keyword of    
*           'custom' id defined by the Ethertype value and mask objects.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchEtypeKeyGet(L7_uint32 UnitIndex, 
                                               L7_uint32 classIndex,
                                               L7_uint32 classRuleIndex,
                                               L7_QOS_ETYPE_KEYID_t *pEtypeKey);

/*********************************************************************
* @purpose  Set the Ethertype keyword match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    etypeKey    @b{(input)} Ethertype keyword
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _ETYPE.
*
* @notes    All Ethertype match rules have a keyword.  Setting a keyword      
*           to 'custom' also requires setting the Ethertype value and mask
*           objects.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchEtypeKeySet(L7_uint32 UnitIndex, 
                                               L7_uint32 classIndex,
                                               L7_uint32 classRuleIndex,
                                               L7_QOS_ETYPE_KEYID_t etypeKey);

/*********************************************************************
* @purpose  Get the Ethertype custom match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pEtypeVal   @b{(output)} Pointer to Ethertype value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _ETYPE.
*
* @notes    This object is only valid for an Ethertype keyword of 'custom'.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchEtypeValueGet(L7_uint32 UnitIndex, 
                                                 L7_uint32 classIndex,
                                                 L7_uint32 classRuleIndex,
                                                 L7_uint32 *pEtypeVal);

/*********************************************************************
* @purpose  Set the Ethertype custom match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    etypeVal    @b{(input)} Ethertype value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _ETYPE.
*
* @notes    This object is only valid for an Ethertype keyword of 'custom'. 
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchEtypeValueSet(L7_uint32 UnitIndex, 
                                                 L7_uint32 classIndex,
                                                 L7_uint32 classRuleIndex,
                                                 L7_uint32 etypeVal);

/*********************************************************************
* @purpose  Get the 'match every' flag value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pEveryFlag  @b{(output)} Pointer to match every flag value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    This flag indicates that the class rule is defined to match 
*           on every packet, regardless of content.
*
* @notes    There is no corresponding 'set' function for the 'match every'
*           flag; this flag is read-only and is updated based on the setting
*           of the match entry type.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchEveryFlagGet(L7_uint32 UnitIndex, 
                                                L7_uint32 classIndex,
                                                L7_uint32 classRuleIndex,
                            L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_t *pEveryFlag);

/*********************************************************************
* @purpose  Get the IPv6 Flow Label match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pFlowLabelVal    @b{(output)} Pointer to IPv6 Flow Label value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _IP6_FLOW_LABEL.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchIp6FlowLabelGet(L7_uint32 UnitIndex, 
                                                   L7_uint32 classIndex,
                                                   L7_uint32 classRuleIndex,
                                                   L7_uint32 *pFlowLabelVal);

/*********************************************************************
* @purpose  Set the IPv6 Flow Label match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pFlowLabelVal    @b{(output)} Pointer to IPv6 Flow Label value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _IP6_FLOW_LABEL.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchIp6FlowLabelSet(L7_uint32 UnitIndex, 
                                                   L7_uint32 classIndex,
                                                   L7_uint32 classRuleIndex,
                                                   L7_uint32 flowLabelVal);

/*********************************************************************
* @purpose  Get the IP DiffServ Code Point (DSCP) match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pDscpVal    @b{(output)} Pointer to IP DSCP value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _IPDSCP.
*
* @notes    The IP DSCP, IP Precedence, and IP TOS match conditions are 
*           three different ways of indicating a match value for the same
*           field, namely the SERVICE_TYPE octet in the IP header.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchIpDscpGet(L7_uint32 UnitIndex, 
                                             L7_uint32 classIndex,
                                             L7_uint32 classRuleIndex,
                                             L7_uint32 *pDscpVal);

/*********************************************************************
* @purpose  Set the IP DiffServ Code Point (DSCP) match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    dscpVal     @b{(input)} IP DSCP value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _IPDSCP.
*
* @notes    The IP DSCP, IP Precedence, and IP TOS match conditions are 
*           three different ways of indicating a match value for the same
*           field, namely the SERVICE_TYPE octet in the IP header.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchIpDscpSet(L7_uint32 UnitIndex, 
                                             L7_uint32 classIndex,
                                             L7_uint32 classRuleIndex,
                                             L7_uint32 dscpVal);

/*********************************************************************
* @purpose  Get the IP Precedence match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pPrecedenceVal  @b{(output)} Pointer to IP Precedence value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _IPPRECEDENCE.
*
* @notes    The IP DSCP, IP Precedence, and IP TOS match conditions are 
*           three different ways of indicating a match value for the same
*           field, namely the SERVICE_TYPE octet in the IP header.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchIpPrecedenceGet(L7_uint32 UnitIndex, 
                                                   L7_uint32 classIndex,
                                                   L7_uint32 classRuleIndex,
                                                   L7_uint32 *pPrecedenceVal);

/*********************************************************************
* @purpose  Set the IP Precedence match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    precedenceVal   @b{(input)} IP Precedence value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _IPPRECEDENCE.
*
* @notes    The IP DSCP, IP Precedence, and IP TOS match conditions are 
*           three different ways of indicating a match value for the same
*           field, namely the SERVICE_TYPE octet in the IP header.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchIpPrecedenceSet(L7_uint32 UnitIndex, 
                                                   L7_uint32 classIndex,
                                                   L7_uint32 classRuleIndex,
                                                   L7_uint32 precedenceVal);

/*********************************************************************
* @purpose  Get the IP TOS bits match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pTosBits    @b{(output)} Pointer to IP TOS bits value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _IPTOS.
*
* @notes    The IP DSCP, IP Precedence, and IP TOS match conditions are 
*           three different ways of indicating a match value for the same
*           field, namely the SERVICE_TYPE octet in the IP header.
* 
* @notes    A valid IP TOS match condition requires both the TOS bits 
*           and mask values.  This can be used as a "free form" TOS
*           match specification.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchIpTosBitsGet(L7_uint32 UnitIndex, 
                                                L7_uint32 classIndex,
                                                L7_uint32 classRuleIndex,
                                                L7_uchar8 *pTosBits);

/*********************************************************************
* @purpose  Set the IP TOS bits match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    tosBits     @b{(input)} IP TOS bits value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _IPTOS.
*
* @notes    The IP DSCP, IP Precedence, and IP TOS match conditions are 
*           three different ways of indicating a match value for the same
*           field, namely the SERVICE_TYPE octet in the IP header.
* 
* @notes    A valid IP TOS match condition requires both the TOS bits 
*           and mask values.  This can be used as a "free form" TOS
*           match specification.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchIpTosBitsSet(L7_uint32 UnitIndex, 
                                                L7_uint32 classIndex,
                                                L7_uint32 classRuleIndex,
                                                L7_uchar8 tosBits);

/*********************************************************************
* @purpose  Get the IP TOS bits mask match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pTosMask    @b{(output)} Pointer to IP TOS bits mask value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _IPTOS.
*
* @notes    The IP DSCP, IP Precedence, and IP TOS match conditions are 
*           three different ways of indicating a match value for the same
*           field, namely the SERVICE_TYPE octet in the IP header.
* 
* @notes    A valid IP TOS match condition requires both the TOS bits 
*           and mask values.  This can be used as a "free form" TOS
*           match specification.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchIpTosMaskGet(L7_uint32 UnitIndex, 
                                                L7_uint32 classIndex,
                                                L7_uint32 classRuleIndex,
                                                L7_uchar8 *pTosMask);

/*********************************************************************
* @purpose  Set the IP TOS bits mask match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    tosMask     @b{(input)} IP TOS bits mask value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _IPTOS.
*
* @notes    The IP DSCP, IP Precedence, and IP TOS match conditions are 
*           three different ways of indicating a match value for the same
*           field, namely the SERVICE_TYPE octet in the IP header.
* 
* @notes    A valid IP TOS match condition requires both the TOS bits 
*           and mask values.  This can be used as a "free form" TOS
*           match specification.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchIpTosMaskSet(L7_uint32 UnitIndex, 
                                                L7_uint32 classIndex,
                                                L7_uint32 classRuleIndex,
                                                L7_uchar8 tosMask);

/*********************************************************************
* @purpose  Get the protocol number match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pProtocolNum  @b{(output)} Pointer to protocol number value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _PROTOCOL.
*
* @notes    Protocol numbers are assigned by IANA.  This function does not
*           validate the legitimacy of the protocol number value used.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchProtocolNumGet(L7_uint32 UnitIndex, 
                                                  L7_uint32 classIndex,
                                                  L7_uint32 classRuleIndex,
                                                  L7_uint32 *pProtocolNum);

/*********************************************************************
* @purpose  Set the protocol number match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    protocolNum @b{(input)} Protocol number value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _PROTOCOL.
*
* @notes    Protocol numbers are assigned by IANA.  This function does not
*           validate the legitimacy of the protocol number value used.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchProtocolNumSet(L7_uint32 UnitIndex, 
                                                  L7_uint32 classIndex,
                                                  L7_uint32 classRuleIndex,
                                                  L7_uint32 protocolNum);

/*********************************************************************
* @purpose  Get the referenced class index value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pRefClassIndex  @b{(output)} Pointer to reference class index value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _REFCLASS.
*
* @notes    A reference class match condition is different from most others
*           in that the actual rule match conditions are obtained from the 
*           definition of the referenced class.  This happens when the 
*           class information is presented to the low-level interface.  The
*           index value of the referenced class is what's used here.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchRefClassIndexGet(L7_uint32 UnitIndex, 
                                                    L7_uint32 classIndex,
                                                    L7_uint32 classRuleIndex,
                                                    L7_uint32 *pRefClassIndex);

/*********************************************************************
* @purpose  Set the referenced class index value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    refClassIndex   @b{(input)} Reference class index value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _REFCLASS.
*
* @notes    A reference class match condition is different from most others
*           in that the actual rule match conditions are obtained from the 
*           definition of the referenced class.  This happens when the 
*           class information is presented to the low-level interface.  The
*           index value of the referenced class is what's used here.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchRefClassIndexSet(L7_uint32 UnitIndex, 
                                                    L7_uint32 classIndex,
                                                    L7_uint32 classRuleIndex,
                                                    L7_uint32 refClassIndex);

/*********************************************************************
* @purpose  Get rule index of the Reference Class Rule for the
*           specified Class
*
* @param    UnitIndex        @b{(input)} System unit number
* @param    classIndex       @b{(input)} Class index
* @param    pClassRuleIndex  @b{(output)} Pointer to Class Rule index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  Specified Class has no Reference Class attached
*
* @notes    The class rule index can only be provided if the request
*           matches the class rule entry type of:  _REFCLASS.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchRefClassRuleIndexFind(L7_uint32 UnitIndex,
                                                         L7_uint32 classIndex,
                                                         L7_uint32 *pClassRuleIndex);

/*********************************************************************
* @purpose  Remove the referenced class index value from the specified
*           class
*
* @param    UnitIndex       @b{(input)} System unit number
* @param    classIndex      @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR  Not allowed for current class rule entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _REFCLASS.
*
* @notes    Reference class rule index (classRuleIndex) for the
*           specified class is obtained by calling
*           usmDbDiffServClassRuleMatchRefClassRuleIndexFind() API.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchRefClassIndexRemove(L7_uint32 UnitIndex,
                                                       L7_uint32 classIndex,
                                                       L7_uint32 classRuleIndex);

/*********************************************************************
* @purpose  Get the source IP address match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pIpAddr     @b{(output)} Pointer to IP address value  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _SRCIP.
*
* @notes    A valid IP address match condition requires both the address
*           and mask values.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchSrcIpAddrGet(L7_uint32 UnitIndex, 
                                                L7_uint32 classIndex,
                                                L7_uint32 classRuleIndex,
                                                L7_uint32 *pIpAddr);

/*********************************************************************
* @purpose  Set the source IP address match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    ipAddr      @b{(input)} IP address value  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _SRCIP.
*
* @notes    A valid IP address match condition requires both the address
*           and mask values.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchSrcIpAddrSet(L7_uint32 UnitIndex, 
                                                L7_uint32 classIndex,
                                                L7_uint32 classRuleIndex,
                                                L7_uint32 ipAddr);

/*********************************************************************
* @purpose  Get the source IP address mask match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pIpMask     @b{(output)} Pointer to IP mask value  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _SRCIP.
*
* @notes    A valid IP address match condition requires both the address
*           and mask values.
* 
* @notes    A non-contiguous mask value is permitted (all b'0' are
*           interpreted as 'dont care' bits).
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchSrcIpMaskGet(L7_uint32 UnitIndex, 
                                                L7_uint32 classIndex,
                                                L7_uint32 classRuleIndex,
                                                L7_uint32 *pIpMask);

/*********************************************************************
* @purpose  Set the source IP address mask match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    ipMask      @b{(input)} IP address mask value  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _SRCIP.
*
* @notes    A valid IP address match condition requires both the address
*           and mask values.
* 
* @notes    A non-contiguous mask value is permitted (all b'0' are
*           interpreted as 'dont care' bits).
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchSrcIpMaskSet(L7_uint32 UnitIndex, 
                                                L7_uint32 classIndex,
                                                L7_uint32 classRuleIndex,
                                                L7_uint32 ipMask);

/*********************************************************************
* @purpose  Get the source IPv6 address match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pIpv6Addr     @b{(output)} Pointer to IPv6 address value  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _SRCIPV6.
*
* @notes    A valid IPv6 address match condition requires both the address
*           and prefix length values.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchSrcIpv6AddrGet(L7_uint32 UnitIndex, 
                                                  L7_uint32 classIndex,
                                                  L7_uint32 classRuleIndex,
                                                  L7_in6_addr_t *pIpv6Addr);

/*********************************************************************
* @purpose  Set the source IPv6 address match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    ipv6Addr      @b{(input)} IPv6 address value  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _SRCIPV6.
*
* @notes    A valid IPv6 address match condition requires both the address
*           and prefix length values.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchSrcIpv6AddrSet(L7_uint32 UnitIndex, 
                                                  L7_uint32 classIndex,
                                                  L7_uint32 classRuleIndex,
                                                  L7_in6_addr_t *ipv6Addr);

/*********************************************************************
* @purpose  Get the source IPv6 prefix length value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pIpv6PrefLength @b{(output)} Pointer to IPv6 prefix length  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _SRCIPV6_PLEN.
*
* @notes    A valid IPv6 address match condition requires both the address
*           and prefix length values.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchSrcIpv6PrefLenGet(L7_uint32 UnitIndex, 
                                                     L7_uint32 classIndex,
                                                     L7_uint32 classRuleIndex,
                                                     L7_uint32 *pIpv6PrefLength);

/*********************************************************************
* @purpose  Set the source IPv6 prefix length value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    ipv6PrefLength  @b{(input)} IPv6 address value  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _SRCIPV6_PLEN.
*
* @notes    A valid IPv6 address match condition requires both the address
*           and mask values.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchSrcIpv6PrefLenSet(L7_uint32 UnitIndex, 
                                                     L7_uint32 classIndex,
                                                     L7_uint32 classRuleIndex,
                                                     L7_uint32 ipv6PrefLength);
/*********************************************************************
* @purpose  Get the source layer 4 port range start match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pPortNum    @b{(output)} Pointer to port number value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _SRCL4PORT.
*
* @notes    A valid layer 4 port match condition requires both the start
*           and end port numbers, with start <= end.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchSrcL4PortStartGet(L7_uint32 UnitIndex, 
                                                     L7_uint32 classIndex,
                                                     L7_uint32 classRuleIndex,
                                                     L7_uint32 *pPortNum);

/*********************************************************************
* @purpose  Set the source layer 4 port range start match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    portNum     @b{(input)} Port number value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _SRCL4PORT.
*
* @notes    A valid layer 4 port match condition requires both the start
*           and end port numbers, with start <= end.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchSrcL4PortStartSet(L7_uint32 UnitIndex, 
                                                     L7_uint32 classIndex,
                                                     L7_uint32 classRuleIndex,
                                                     L7_uint32 portNum);

/*********************************************************************
* @purpose  Get the source layer 4 port range end match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pPortNum    @b{(output)} Pointer to port number value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _SRCL4PORT.
*
* @notes    A valid layer 4 port match condition requires both the start
*           and end port numbers, with start <= end.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchSrcL4PortEndGet(L7_uint32 UnitIndex, 
                                                   L7_uint32 classIndex,
                                                   L7_uint32 classRuleIndex,
                                                   L7_uint32 *pPortNum);

/*********************************************************************
* @purpose  Set the source layer 4 port range end match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    portNum     @b{(input)} Port number value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _SRCL4PORT.
*
* @notes    A valid layer 4 port match condition requires both the start
*           and end port numbers, with start <= end.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchSrcL4PortEndSet(L7_uint32 UnitIndex, 
                                                   L7_uint32 classIndex,
                                                   L7_uint32 classRuleIndex,
                                                   L7_uint32 portNum);

/*********************************************************************
* @purpose  Get the source MAC address match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pMacAddrPtr @b{(output)} Pointer to MAC address 
* @param    pLen        @b{(input/output)} Pointer to length of the MAC addr
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type or buffer 
*                       length is too short
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _SRCMAC.
*
* @notes    A valid MAC address match condition requires both the address
*           and mask values.
* 
* @notes    `pMacAddrPtr` points to a char buffer of minimum length equals to
*            L7_MAC_ADDR_LEN
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchSrcMacAddrGet(L7_uint32 UnitIndex, 
                                                 L7_uint32 classIndex,
                                                 L7_uint32 classRuleIndex,
                                                 L7_uchar8 *pMacAddrPtr,
                                                 L7_uint32 *pLen);

/*********************************************************************
* @purpose  Set the source MAC address match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    macAddrPtr  @b{(input)} MAC address pointer  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _SRCMAC.
*
* @notes    A valid MAC address match condition requires both the address
*           and mask values.
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchSrcMacAddrSet(L7_uint32 UnitIndex, 
                                                 L7_uint32 classIndex,
                                                 L7_uint32 classRuleIndex,
                                                 L7_uchar8 *macAddrPtr);

/*********************************************************************
* @purpose  Get the source MAC address mask match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pMacMaskPtr @b{(output)} Pointer to MAC mask
* @param    pLen        @b{(input/output)} Pointer to length of the MAC Mask  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type or buffer 
*                       length is too short
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _SRCMAC.
*
* @notes    A valid IP address match condition requires both the address
*           and mask values.
* 
* @notes    A non-contiguous mask value is permitted (all b'0' are
*           interpreted as 'dont care' bits).
*
* @notes    `pMacMaskPtr` points to a char buffer of minimum length equals to
*            L7_MAC_ADDR_LEN
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchSrcMacMaskGet(L7_uint32 UnitIndex, 
                                                 L7_uint32 classIndex,
                                                 L7_uint32 classRuleIndex,
                                                 L7_uchar8 *pMacMaskPtr,
                                                 L7_uint32 *pLen);

/*********************************************************************
* @purpose  Set the source MAC address mask match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    macMaskPtr  @b{(input)} MAC mask pointer
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _SRCMAC.
*
* @notes    A valid IP address match condition requires both the address
*           and mask values.
* 
* @notes    A non-contiguous mask value is permitted (all b'0' are
*           interpreted as 'dont care' bits).
* 
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchSrcMacMaskSet(L7_uint32 UnitIndex, 
                                                 L7_uint32 classIndex,
                                                 L7_uint32 classRuleIndex,
                                                 L7_uchar8 *macMaskPtr);

/*********************************************************************
* @purpose  Get the VLAN ID range start match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pVlanId     @b{(output)} Pointer to VLAN ID value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _VLAN.
*
* @notes    The VLAN ID field is defined as the 12-bit VLAN identifier
*           in the first/outer 802.1Q header of a tagged Ethernet frame.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchVlanIdStartGet(L7_uint32 UnitIndex, 
                                                  L7_uint32 classIndex,
                                                  L7_uint32 classRuleIndex,
                                                  L7_uint32 *pVlanId);

/*********************************************************************
* @purpose  Set the VLAN ID range start match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    vlanId      @b{(input)} VLAN ID value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _VLAN.
*
* @notes    The VLAN ID field is defined as the 12-bit VLAN identifier
*           in the first/outer 802.1Q header of a tagged Ethernet frame.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchVlanIdStartSet(L7_uint32 UnitIndex, 
                                                  L7_uint32 classIndex,
                                                  L7_uint32 classRuleIndex,
                                                  L7_uint32 vlanId);

/*********************************************************************
* @purpose  Get the VLAN ID range end match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pVlanId     @b{(output)} Pointer to VLAN ID value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _VLAN.
*
* @notes    The VLAN ID field is defined as the 12-bit VLAN identifier
*           in the first/outer 802.1Q header of a tagged Ethernet frame.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchVlanIdEndGet(L7_uint32 UnitIndex, 
                                                L7_uint32 classIndex,
                                                L7_uint32 classRuleIndex,
                                                L7_uint32 *pVlanId);

/*********************************************************************
* @purpose  Set the VLAN ID range end match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    vlanId      @b{(input)} VLAN ID value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _VLAN.
*
* @notes    The VLAN ID field is defined as the 12-bit VLAN identifier
*           in the first/outer 802.1Q header of a tagged Ethernet frame.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchVlanIdEndSet(L7_uint32 UnitIndex, 
                                                L7_uint32 classIndex,
                                                L7_uint32 classRuleIndex,
                                                L7_uint32 vlanId);

/*********************************************************************
* @purpose  Get the VLAN ID2 range start match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pVlanId     @b{(output)} Pointer to VLAN ID2 value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _VLANID2.
*
* @notes    The Secondary VLAN ID field is defined as the 12-bit VLAN
*           identifier in the second/inner 802.1Q tag header of a double
*           VLAN tagged Ethernet frame.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchVlanId2StartGet(L7_uint32 UnitIndex, 
                                                   L7_uint32 classIndex,
                                                   L7_uint32 classRuleIndex,
                                                   L7_uint32 *pVlanId);

/*********************************************************************
* @purpose  Set the VLAN ID2 range start match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    vlanId      @b{(input)} VLAN ID2 value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _VLANID2.
*
* @notes    The Secondary VLAN ID field is defined as the 12-bit VLAN
*           identifier in the second/inner 802.1Q tag header of a double
*           VLAN tagged Ethernet frame.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchVlanId2StartSet(L7_uint32 UnitIndex, 
                                                   L7_uint32 classIndex,
                                                   L7_uint32 classRuleIndex,
                                                   L7_uint32 vlanId);

/*********************************************************************
* @purpose  Get the VLAN ID2 range end match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pVlanId     @b{(output)} Pointer to VLAN ID2 value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _VLANID2.
*
* @notes    The Secondary VLAN ID field is defined as the 12-bit VLAN
*           identifier in the second/inner 802.1Q tag header of a double
*           VLAN tagged Ethernet frame.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchVlanId2EndGet(L7_uint32 UnitIndex, 
                                                 L7_uint32 classIndex,
                                                 L7_uint32 classRuleIndex,
                                                 L7_uint32 *pVlanId);

/*********************************************************************
* @purpose  Set the VLAN ID2 range end match value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    vlanId      @b{(input)} VLAN ID2 value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _VLANID2.
*
* @notes    The Secondary VLAN ID field is defined as the 12-bit VLAN
*           identifier in the second/inner 802.1Q tag header of a double
*           VLAN tagged Ethernet frame.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchVlanId2EndSet(L7_uint32 UnitIndex, 
                                                 L7_uint32 classIndex,
                                                 L7_uint32 classRuleIndex,
                                                 L7_uint32 vlanId);

/*********************************************************************
* @purpose  Get the Class Rule exclude flag
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pExcludeFlag  @b{(output)} Pointer to exclude flag value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    The effect of the exclude flag is to negate this class rule
*           match condition, namely cause a match on everything EXCEPT
*           the stated condition.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchExcludeFlagGet(L7_uint32 UnitIndex, 
                                                  L7_uint32 classIndex,
                                                  L7_uint32 classRuleIndex,
                            L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_t *pExcludeFlag);

/*********************************************************************
* @purpose  Set the Class Rule exclude flag
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    excludeFlag @b{(input)} Exclude flag value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for this match entry type
* @returns  L7_FAILURE  
*
* @notes    The effect of the exclude flag is to negate this class rule
*           match condition, namely cause a match on everything EXCEPT
*           the stated condition.
*
* @notes    The exclude flag must be set after the Class Rule entry type,
*           but before any other match object in the row.
*
* @notes    Only a set value of L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE
*           is allowed for a match entry type of:  _REFCLASS.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleMatchExcludeFlagSet(L7_uint32 UnitIndex, 
                                                  L7_uint32 classIndex,
                                                  L7_uint32 classRuleIndex,
                            L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_t excludeFlag);

/*********************************************************************
* @purpose  Get the storage type for the specified Class Rule Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pValue      @b{(output)} Pointer to storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleStorageTypeGet(L7_uint32 UnitIndex,
                                             L7_uint32 classIndex,
                                             L7_uint32 classRuleIndex,
                              L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t *pValue);

/*********************************************************************
* @purpose  Set the storage type for the specified Class Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    value       @b{(input)} Storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleStorageTypeSet(L7_uint32 UnitIndex,
                                             L7_uint32 classIndex,
                                             L7_uint32 classRuleIndex,
                              L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t value);

/*********************************************************************
* @purpose  Get the current status of the specified Class Rule Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    pValue      @b{(output)} Pointer to row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleRowStatusGet(L7_uint32 UnitIndex,
                                           L7_uint32 classIndex,
                                           L7_uint32 classRuleIndex,
                              L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t *pValue);

/*********************************************************************
* @purpose  Set the current status of the specified Class Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    value       @b{(input)} Row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleRowStatusSet(L7_uint32 UnitIndex,
                                           L7_uint32 classIndex,
                                           L7_uint32 classRuleIndex,
                              L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t value);

/*********************************************************************
* @purpose  Get the ACL rule identifier, if any, for the specified entry
*           in the Class Rule Table
*
* @param    UnitIndex       @b{(input)}  System unit number
* @param    classIndex      @b{(input)}  Class index
* @param    classRuleIndex  @b{(input)}  Class rule index
* @param    pAclRuleId      @b{(output)} Pointer to ACL rule ID output value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not an 'acl' class rule
* @returns  L7_FAILURE  Improper usage or other malfunction
*
* @notes    This is a supplemental function provided to assist the user
*           interface code with correlating the specified class rule to 
*           the access list rule from which it was derived.  This is only
*           meaningful for a class type 'acl'.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServClassRuleAclRuleIdGet(L7_uint32 UnitIndex,
                                           L7_uint32 classIndex, 
                                           L7_uint32 classRuleIndex,
                                           L7_uint32 *pAclRuleId);


/*
======================
======================
======================
 
   POLICY TABLE API

======================
======================
======================
*/

/*************************************************************************
* @purpose  Obtain the next available index value to be used when creating  
*           a new row in the Policy Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    pPolicyIndex @b{(output)} Pointer to policy index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An L7_SUCCESS return with an output *pPolicyIndex value of 0
*           means the table is full.
*
* @notes    The index value remains free until used in a subsequent create
*           request.  When two or more requestors get the same index value,
*           only the first one to successfully create a new row in this table
*           gets to keep it.  All other requestors' create requests will
*           fail due to the index being in use, and they must call this
*           function again to get a new value.
*
* @notes    Although an index value is considered in use once a
*           successful row create occurs, the next index value is not
*           revealed until the row is either completed or deleted.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyIndexNext(L7_uint32 UnitIndex, 
                                     L7_uint32 *pPolicyIndex);

/*************************************************************************
* @purpose  Retrieve the maximum policy index value allowed for the 
*           Policy Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    pIndexMax   @b{(output)} Pointer to index max output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is purely informational.  The corresponding table 'IndexNext'
*           function must be used to obtain the current index value.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyIndexMaxGet(L7_uint32 UnitIndex, 
                                       L7_uint32 *pIndexMax);

/*************************************************************************
* @purpose  Create a new row in the Policy Table for the specified index
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    activateRow @b{(input)} Activate row when ready flag
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR               Index value in use, row not complete
* @returns  L7_ALREADY_CONFIGURED  Index value in use, row complete
* @returns  L7_REQUEST_DENIED      Config change not approved
* @returns  L7_FAILURE
*
* @notes    An L7_ERROR response means the policy index value is currently 
*           in use, but the row is not complete.  This is typically
*           caused when multiple requestors obtain the same "index next"
*           value and race to create the new row.  The caller should
*           get a new index value and re-attempt to create the row.
*
* @notes    An L7_ALREADY_CONFIGURED response means the policy index value
*           is currently in use for a completed row.  The caller should
*           get a new index value and re-attempt to create the row.
*
* @notes    An L7_REQUEST_DENIED response means another application   
*           has denied approval of this configuration change.
*
* @notes    The row status is initially set to 'not ready' and remains that 
*           way until every object in the row contains a valid value.  If the
*           activateRow parameter is set to L7_TRUE, the row status will then
*           be automatically updated to 'active'.
*
* @end
**********************************************************************/
L7_RC_t usmDbDiffServPolicyCreate(L7_uint32 UnitIndex, L7_uint32 policyIndex,
                                  L7_BOOL activateRow);

/*************************************************************************
* @purpose  Delete a row from the Policy Table for the specified index
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR               Row does not exist for the specified index, or is
*                                    referenced by one or more services (interfaces)
* @returns  L7_REQUEST_DENIED      Config change not approved
* @returns  L7_FAILURE
*
* @notes    A policy cannot be deleted if it is currently assigned to an    
*           interface.
*
* @notes    An L7_REQUEST_DENIED response means another application   
*           has denied approval of this configuration change.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyDelete(L7_uint32 UnitIndex, L7_uint32 policyIndex);

/*********************************************************************
* @purpose  Verify that a Policy Table row exists for the specified index
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyGet(L7_uint32 UnitIndex, L7_uint32 policyIndex);

/*********************************************************************
* @purpose  Determine next sequential row in the Policy Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    prevPolicyIndex  @b{(input)} Policy index to begin search
* @param    pPolicyIndex  @b{(output)} Pointer to next sequential policy index
*                                      value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Reached end of table
* @returns  L7_FAILURE  
*
* @notes    A prevPolicyIndex value of 0 is used to find the first entry in
*           the table.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyGetNext(L7_uint32 UnitIndex, 
                                   L7_uint32 prevPolicyIndex, 
                                   L7_uint32 *pPolicyIndex);

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Policy Table 
*           to determine its legitimacy
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    objectId    @b{(input)} Object identifier
* @param    pValue      @b{(input)} Pointer to proposed value to be set
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Proposed value invalid
* @returns  L7_FAILURE  
*
* @notes    Only tests objects with access types READ-CREATE or READ-WRITE;
*           all other access types return L7_FAILURE.
*
* @notes    Caller must use one of the _SETTEST_OID_TYPE object identifiers
*           only (i.e., do not use the _TABLE_OBJECT type object names here).
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicySetTest(L7_uint32 UnitIndex, L7_uint32 policyIndex, 
    L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_TYPE_t objectId, void *pValue);

/*********************************************************************
* @purpose  Get the policy name
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    pStringPtr  @b{(output)} Pointer to a policy name string
* @param    pLen        @b{(input/output)} Pointer to length of the string  
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyNameGet(L7_uint32 UnitIndex, L7_uint32 policyIndex,
                                   L7_uchar8 *pStringPtr, L7_uint32 *pLen);

/*********************************************************************
* @purpose  Set the class name
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    stringPtr   @b{(input)} Class name string pointer 
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyNameSet(L7_uint32 UnitIndex, L7_uint32 policyIndex,
                                   L7_uchar8 *stringPtr);

/*********************************************************************
* @purpose  Translate a policy name into its associated Policy Table index
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    stringPtr   @b{(input)} Policy name string pointer
* @param    pPolicyIndex @b{(output)} Pointer to policy index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is a supplemental function provided to assist the user
*           interface code with name-to-index translation.  Use the
*           name 'get' function to translate in the reverse direction.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyNameToIndex(L7_uint32 UnitIndex, 
                                       L7_uchar8 *stringPtr, 
                                       L7_uint32 *pPolicyIndex);

/*********************************************************************
* @purpose  Get the type of policy definition
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    pPolicyType @b{(output)} Pointer to policy type value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    The policy type dictates whether inbound or outbound policy
*           attribute statements are used in the policy definition.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyTypeGet(L7_uint32 UnitIndex, L7_uint32 policyIndex,
                              L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t *pPolicyType);

/*********************************************************************
* @purpose  Set the type of policy definition
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyType  @b{(input)} Policy type value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    The policy type dictates whether inbound or outbound policy
*           attribute statements are used in the policy definition.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyTypeSet(L7_uint32 UnitIndex, L7_uint32 policyIndex,
                              L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t policyType);

/*********************************************************************
* @purpose  Get the storage type for the specified Policy Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    pValue      @b{(output)} Pointer to storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyStorageTypeGet(L7_uint32 UnitIndex,
                                          L7_uint32 policyIndex,
                              L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t *pValue);

/*********************************************************************
* @purpose  Set the storage type for the specified Policy Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    value       @b{(input)} Storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyStorageTypeSet(L7_uint32 UnitIndex,
                                          L7_uint32 policyIndex,
                              L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t value);

/*********************************************************************
* @purpose  Get the current status of the specified Policy Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    pValue      @b{(output)} Pointer to row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyRowStatusGet(L7_uint32 UnitIndex,
                                        L7_uint32 policyIndex,
                              L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t *pValue);

/*********************************************************************
* @purpose  Set the current status of the specified Policy Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    value       @b{(input)} Row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyRowStatusSet(L7_uint32 UnitIndex,
                                        L7_uint32 policyIndex,
                              L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t value);


/*
=====================================
=====================================
=====================================
 
   POLICY-CLASS INSTANCE TABLE API

=====================================
=====================================
=====================================
*/

/*************************************************************************
* @purpose  Obtain the next available index value to be used when creating  
*           a new row in the Policy-Class Instance Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    pPolicyInstIndex @b{(output)} Pointer to instance index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An L7_SUCCESS return with an output *pPolicyInstIndex value
*           of 0 means the table is full.
*
* @notes    The index value remains free until used in a subsequent create
*           request.  When two or more requestors get the same index value,
*           only the first one to successfully create a new row in this table
*           gets to keep it.  All other requestors' create requests will
*           fail due to the index being in use, and they must call this
*           function again to get a new value.
*
* @notes    Although an index value is considered in use once a
*           successful row create occurs, the next index value is not
*           revealed until the row is either completed or deleted.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyInstIndexNext(L7_uint32 UnitIndex, 
                                         L7_uint32 policyIndex,
                                         L7_uint32 *pPolicyInstIndex);

/*************************************************************************
* @purpose  Retrieve the maximum policy instance index value allowed for the 
*           Policy Instance Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    pIndexMax   @b{(output)} Pointer to index max output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is purely informational.  The corresponding table 'IndexNext'
*           function must be used to obtain the current index value.
*
* @notes    There is a difference between this maximum index and the 
*           maximum table size.  The latter encompasses independent
*           sets of index maximums.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyInstIndexMaxGet(L7_uint32 UnitIndex, 
                                           L7_uint32 *pIndexMax);

/*********************************************************************
* @purpose  Find the policy instance index from the policy and class indexes
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    classIndex  @b{(input)} Class index
* @param    pPolicyInstIndex @b{(output)} Pointer to policy instance index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is a supplemental function provided to assist the user
*           interface code by searching through the Policy-Class Instance
*           Table for the policy instance index whose row matches the 
*           specified class index for the policy.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyInstIndexFromClass(L7_uint32 UnitIndex, 
                                              L7_uint32 policyIndex, 
                                              L7_uint32 classIndex,
                                              L7_uint32 *pPolicyInstIndex);

/*************************************************************************
* @purpose  Create a new row in the Policy-Class Instance Table for the 
*           specified index
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    activateRow @b{(input)} Activate row when ready flag
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR               Index value in use, row not complete
* @returns  L7_ALREADY_CONFIGURED  Index value in use, row complete
* @returns  L7_REQUEST_DENIED      Config change not approved
* @returns  L7_FAILURE
*
* @notes    An L7_ERROR response means the policy instance index value is
*           currently in use, but the row is not complete.  This is typically
*           caused when multiple requestors obtain the same "index next"
*           value and race to create the new row.  The caller should
*           get a new index value and re-attempt to create the row.
*
* @notes    An L7_ALREADY_CONFIGURED response means the policy instance index
*           value is currently in use for a completed row.  The caller should
*           get a new index value and re-attempt to create the row.
*
* @notes    An L7_REQUEST_DENIED response means another application   
*           has denied approval of this configuration change.
*
* @notes    The row status is initially set to 'not ready' and remains that 
*           way until every object in the row contains a valid value.  If the
*           activateRow parameter is set to L7_TRUE, the row status will then
*           be automatically updated to 'active'.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyInstCreate(L7_uint32 UnitIndex, 
                                      L7_uint32 policyIndex,
                                      L7_uint32 policyInstIndex,
                                      L7_BOOL activateRow);

/*************************************************************************
* @purpose  Delete a row from the Policy-Class Instance Table for the 
*           specified index
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR               Row does not exist for the specified index
* @returns  L7_REQUEST_DENIED      Config change not approved
* @returns  L7_FAILURE
*
* @notes    An L7_REQUEST_DENIED response means another application   
*           has denied approval of this configuration change.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyInstDelete(L7_uint32 UnitIndex, 
                                      L7_uint32 policyIndex,
                                      L7_uint32 policyInstIndex);

/*********************************************************************
* @purpose  Verify that a Policy-Class Instance Table row exists for the 
*           specified index
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyInstGet(L7_uint32 UnitIndex, 
                                   L7_uint32 policyIndex,
                                   L7_uint32 policyInstIndex);

/*********************************************************************
* @purpose  Determine next sequential row in the Policy-Class Instance Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    prevPolicyIndex  @b{(input)} Policy index to begin search
* @param    prevPolicyInstIndex  @b{(input)} Policy instance index to begin 
*                                              search
* @param    pPolicyIndex  @b{(output)} Pointer to next sequential policy index
*                                      value
* @param    pPolicyInstIndex  @b{(output)} Pointer to next sequential policy
*                                            instance index value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Reached end of table
* @returns  L7_FAILURE  
*
* @notes    A prevPolicyIndex value of 0 is used to find the first policy
*           entry in the table.
*
* @notes    A prevPolicyInstIndex value of 0 is used to find the first class
*           instance relative to the specified prevPolicyIndex.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyInstGetNext(L7_uint32 UnitIndex, 
                                       L7_uint32 prevPolicyIndex, 
                                       L7_uint32 prevPolicyInstIndex, 
                                       L7_uint32 *pPolicyIndex,
                                       L7_uint32 *pPolicyInstIndex);

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Policy-Class
*           Instance Table to determine its legitimacy
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    objectId    @b{(input)} Object identifier
* @param    pValue      @b{(input)} Pointer to proposed value to be set
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR               Proposed value invalid
* @returns  L7_FAILURE  
*
* @notes    Only tests objects with access types READ-CREATE or READ-WRITE;
*           all other access types return L7_FAILURE.
*
* @notes    Caller must use one of the _SETTEST_OID_TYPE object identifiers
*           only (i.e., do not use the _TABLE_OBJECT type object names here).
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyInstSetTest(L7_uint32 UnitIndex, 
                                       L7_uint32 policyIndex, 
                                       L7_uint32 policyInstIndex, 
                L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_TYPE_t objectId, 
                                       void *pValue);

/*********************************************************************
* @purpose  Get the class index referenced as the policy class instance
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    pClassIndex @b{(output)} Pointer to class index value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    The class index identifies the classifier definition used by 
*           the policy to establish an instance of that class (to which
*           policy attributes can be attached).  This is what connects a 
*           class (instance) to a policy.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyInstClassIndexGet(L7_uint32 UnitIndex, 
                                             L7_uint32 policyIndex,
                                             L7_uint32 policyInstIndex,
                                             L7_uint32 *pClassIndex);

/*********************************************************************
* @purpose  Set the class index referenced as the policy class instance
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    classIndex  @b{(input)} Class index value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    The class index identifies the classifier definition used by 
*           the policy to establish an instance of that class (to which
*           policy attributes can be attached).  This is what connects a 
*           class (instance) to a policy.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyInstClassIndexSet(L7_uint32 UnitIndex, 
                                             L7_uint32 policyIndex,
                                             L7_uint32 policyInstIndex,
                                             L7_uint32 classIndex);

/*********************************************************************
* @purpose  Get the storage type for the specified Policy-Class Instance 
*           Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    pValue      @b{(output)} Pointer to storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyInstStorageTypeGet(L7_uint32 UnitIndex,
                                              L7_uint32 policyIndex,
                                              L7_uint32 policyInstIndex,
                              L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t *pValue);

/*********************************************************************
* @purpose  Set the storage type for the specified Policy-Class Instance 
*           Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    value       @b{(input)} Storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyInstStorageTypeSet(L7_uint32 UnitIndex,
                                              L7_uint32 policyIndex,
                                              L7_uint32 policyInstIndex,
                              L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t value);

/*********************************************************************
* @purpose  Get the current status of the specified Policy-Class
*           Instance Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    pValue      @b{(output)} Pointer to row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyInstRowStatusGet(L7_uint32 UnitIndex,
                                            L7_uint32 policyIndex,
                                            L7_uint32 policyInstIndex,
                              L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t *pValue);

/*********************************************************************
* @purpose  Set the current status of the specified Policy-Class 
*           Instance Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    value       @b{(input)} Row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyInstRowStatusSet(L7_uint32 UnitIndex,
                                            L7_uint32 policyIndex,
                                            L7_uint32 policyInstIndex,
                              L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t value);


/*
================================
================================
================================

   POLICY ATTRIBUTE TABLE API

================================
================================
================================
*/

/*************************************************************************
* @purpose  Obtain the next available index value to be used when creating  
*           a new row in the Policy Attribute Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    pPolicyAttrIndex @b{(output)} Pointer to policy attribute index 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An L7_SUCCESS return with an output *pPolicyAttrIndex value
*           of 0 means the table is full.
*
* @notes    The index value remains free until used in a subsequent create
*           request.  When two or more requestors get the same index value,
*           only the first one to successfully create a new row in this table
*           gets to keep it.  All other requestors' create requests will
*           fail due to the index being in use, and they must call this
*           function again to get a new value.
*
* @notes    Although an index value is considered in use once a
*           successful row create occurs, the next index value is not
*           revealed until the row is either completed or deleted.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrIndexNext(L7_uint32 UnitIndex, 
                                         L7_uint32 policyIndex,
                                         L7_uint32 policyInstIndex,
                                         L7_uint32 *pPolicyAttrIndex);

/*************************************************************************
* @purpose  Retrieve the maximum policy attribute index value allowed for the 
*           Policy Attribute Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    pIndexMax   @b{(output)} Pointer to index max output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is purely informational.  The corresponding table 'IndexNext'
*           function must be used to obtain the current index value.
*
* @notes    There is a difference between this maximum index and the 
*           maximum table size.  The latter encompasses independent
*           sets of index maximums.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrIndexMaxGet(L7_uint32 UnitIndex, 
                                           L7_uint32 *pIndexMax);

/*************************************************************************
* @purpose  Create a new row in the Policy ATtribute Table for the 
*           specified index
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    activateRow @b{(input)} Activate row when ready flag
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR               Index value in use, row not complete
* @returns  L7_ALREADY_CONFIGURED  Index value in use, row complete
* @returns  L7_FAILURE
*
* @notes    An L7_ERROR response means the policy attribute index value is
*           currently in use, but the row is not complete.  This is typically
*           caused when multiple requestors obtain the same "index next"
*           value and race to create the new row.  The caller should
*           get a new index value and re-attempt to create the row.
*
* @notes    An L7_ALREADY_CONFIGURED response means the policy attribute index
*           value is currently in use for a completed row.  The caller should
*           get a new index value and re-attempt to create the row.
*
* @notes    The row status is initially set to 'not ready' and remains that 
*           way until every object in the row contains a valid value.  If the
*           activateRow parameter is set to L7_TRUE, the row status will then
*           be automatically updated to 'active'.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrCreate(L7_uint32 UnitIndex, 
                                      L7_uint32 policyIndex,
                                      L7_uint32 policyInstIndex,
                                      L7_uint32 policyAttrIndex,
                                      L7_BOOL activateRow);

/*************************************************************************
* @purpose  Delete a row from the Policy Attribute Table for the 
*           specified index
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrDelete(L7_uint32 UnitIndex, 
                                      L7_uint32 policyIndex,
                                      L7_uint32 policyInstIndex, 
                                      L7_uint32 policyAttrIndex);

/*********************************************************************
* @purpose  Verify that a Policy Attribute Table row exists for the 
*           specified index
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrGet(L7_uint32 UnitIndex, 
                                   L7_uint32 policyIndex,
                                   L7_uint32 policyInstIndex,
                                   L7_uint32 policyAttrIndex);

/*********************************************************************
* @purpose  Determine next sequential row in the Policy Attribute Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    prevPolicyIndex  @b{(input)} Policy index to begin search
* @param    prevPolicyInstIndex  @b{(input)} Policy instance index to begin 
*                                              search
* @param    prevPolicyAttrIndex  @b{(input)} Policy attribute index to begin 
*                                              search
* @param    pPolicyIndex  @b{(output)} Pointer to next sequential policy index
*                                      value
* @param    pPolicyInstIndex  @b{(output)} Pointer to next sequential policy
*                                            instance index value
* @param    pPolicyAttrIndex  @b{(output)} Pointer to next sequential policy
*                                            attribute index value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Reached end of table
* @returns  L7_FAILURE  
*
* @notes    A prevPolicyIndex value of 0 is used to find the first policy
*           entry in the table.
*
* @notes    A prevPolicyInstIndex value of 0 is used to find the first class
*           instance relative to the specified prevPolicyIndex.
*
* @notes    A prevPolicyAttrIndex value of 0 is used to find the first policy
*           attribute relative to the specified prevPolicyIndex, 
*           prevPolicyInstIndex.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrGetNext(L7_uint32 UnitIndex, 
                                       L7_uint32 prevPolicyIndex, 
                                       L7_uint32 prevPolicyInstIndex, 
                                       L7_uint32 prevPolicyAttrIndex, 
                                       L7_uint32 *pPolicyIndex,
                                       L7_uint32 *pPolicyInstIndex,
                                       L7_uint32 *pPolicyAttrIndex);

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Policy
*           Attribute Table to determine its legitimacy
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    objectId    @b{(input)} Object identifier
* @param    pValue      @b{(input)} Pointer to proposed value to be set
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Proposed value invalid
* @returns  L7_FAILURE  
*
* @notes    Only tests objects with access types READ-CREATE or READ-WRITE;
*           all other access types return L7_FAILURE.
*
* @notes    Caller must use one of the _SETTEST_OID_TYPE object identifiers
*           only (i.e., do not use the _TABLE_OBJECT type object names here).
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrSetTest(L7_uint32 UnitIndex, 
                                       L7_uint32 policyIndex, 
                                       L7_uint32 policyInstIndex, 
                                       L7_uint32 policyAttrIndex, 
                L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_TYPE_t objectId, 
                                       void *pValue);

/*********************************************************************
* @purpose  Get the Policy Attribute statement entry type
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pEntryType  @b{(output)} Pointer to entry type value  
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    The entry type is essential to determine which one of the
*           individual statements is defined for this policy attribute.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtEntryTypeGet(L7_uint32 UnitIndex, 
                                                L7_uint32 policyIndex, 
                                                L7_uint32 policyInstIndex, 
                                                L7_uint32 policyAttrIndex, 
                        L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t *pEntryType);

/*********************************************************************
* @purpose  Set the Policy Attribute statement entry type
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    entryType   @b{(input)} Entry type value  
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    The entry type MUST be set before any of the individual        
*           statements can be set for this policy attribute.  The entry type 
*           value determines which statement get/set commands are valid for 
*           this attribute.
*
* @notes    For a given policy-class instance, one or more attribute
*           statements may be defined.  However, not all statement entry
*           types are allowed to co-exist within the same policy-class
*           instance.  The following lists for each statement entry types 
*           any other types that conflict:
*              _ASSIGN_QUEUE      :  _DROP
*              _BANDWIDTH         :  _EXPEDITE
*              _DROP              :  _ASSIGN_QUEUE, _MARK_(any), _POLICE_(any), 
*                                        _REDIRECT
*              _EXPEDITE          :  _BANDWIDTH, _SHAPE_PEAK
*              _MARK_COSVAL       :  _DROP, _MARK_COS2VAL, _MARK_IPDSCPVAL,
*                                        _MARK_IPPPRECEDENCEVAL, _POLICE_(any)
*              _MARK_COS2VAL      :  _DROP, _MARK_COSVAL, _MARK_IPDSCPVAL,
*                                        _MARK_IPPPRECEDENCEVAL, _POLICE_(any)
*              _MARK_IPDSCPVAL    :  _DROP, _MARK_COSVAL, _MARK_COS2VAL,
*                                        _MARK_IPPPRECEDENCEVAL, _POLICE_(any)
*           _MARK_IPPRECEDENCEVAL :  _DROP, _MARK_COSVAL, _MARK_COS2VAL,
*                                        _MARK_IPPDSCPVAL, _POLICE_(any)
*              _MIRROR            :  _DROP, _REDIRECT
*              _POLICE_SIMPLE     :  _DROP, _MARK_(any), _POLICE_SINGLERATE,
*                                       _POLICE_TWORATE
*              _POLICE_SINGLERATE :  _DROP, _MARK_(any), _POLICE_SIMPLE,
*                                       _POLICE_TWORATE
*              _POLICE_TWORATE    :  _DROP, _MARK_(any), _POLICE_SIMPLE,
*                                       _POLICE_SINGLERATE
*              _RANDOMDROP        :  (no conflicts)
*              _REDIRECT          :  _DROP, _MIRROR
*              _SHAPE_AVERAGE     :  _SHAPE_PEAK
*              _SHAPE_PEAK        :  _EXPEDITE, _SHAPE_AVERAGE
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtEntryTypeSet(L7_uint32 UnitIndex, 
                                                L7_uint32 policyIndex, 
                                                L7_uint32 policyInstIndex, 
                                                L7_uint32 policyAttrIndex, 
                        L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType);

/*********************************************************************
* @purpose  Get the assigned queue id attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pQueueId    @b{(output)} Pointer to assigned queue id
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _ASSIGN_QUEUE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtAssignQueueIdGet(L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 *pQueueId);

/*********************************************************************
* @purpose  Set the assigned queue id attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    queueId     @b{(input)} Queue id value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _ASSIGN_QUEUE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtAssignQueueIdSet(L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 queueId);

/*********************************************************************
* @purpose  Get the 'drop' attribute statement flag value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pDropFlag   @b{(output)} Pointer to drop attribute flag value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _DROP.
*
* @notes    This flag indicates that the policy attribute is defined to  
*           drop every inbound packet.
*
* @notes    There is no corresponding 'set' function for the 'drop'
*           flag; this flag is read-only and is updated based on the setting
*           of the policy attribute entry type.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtDropFlagGet(L7_uint32 UnitIndex, 
                                               L7_uint32 policyIndex, 
                                               L7_uint32 policyInstIndex, 
                                               L7_uint32 policyAttrIndex, 
                              L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_t *pDropFlag);

/*********************************************************************
* @purpose  Get the mark COS attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pCosVal     @b{(output)} Pointer to COS value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _MARK_COSVAL.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtMarkCosValGet(L7_uint32 UnitIndex, 
                                                 L7_uint32 policyIndex, 
                                                 L7_uint32 policyInstIndex, 
                                                 L7_uint32 policyAttrIndex, 
                                                 L7_uint32 *pCosVal);

/*********************************************************************
* @purpose  Set the mark COS attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    cosVal      @b{(input)} COS value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _MARK_COSVAL.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtMarkCosValSet(L7_uint32 UnitIndex, 
                                                 L7_uint32 policyIndex, 
                                                 L7_uint32 policyInstIndex, 
                                                 L7_uint32 policyAttrIndex, 
                                                 L7_uint32 cosVal);

/*********************************************************************
* @purpose  Set the mark "COS as Secondary Cos" attribute statement value
*
* @param    UnitIndex    @b{(input)} System unit number
* @param    policyIndex  @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    policyAttrIndex @b{(input)} Policy attribute index
* @param    cosVal       @b{(input)} COS value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _MARK_COS_AS_COS2.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtMarkCosAsCos2Set(L7_uint32 UnitIndex, 
                                                 L7_uint32 policyIndex, 
                                                 L7_uint32 policyInstIndex, 
                                                 L7_uint32 policyAttrIndex, 
                                                 L7_uint32 cosVal);

/*********************************************************************
* @purpose  Get the mark "COS as Secondary Cos" attribute statement value
*
* @param    UnitIndex    @b{(input)} System unit number
* @param    policyIndex  @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    policyAttrIndex @b{(input)} Policy attribute index
* @param    pCosVal      @b{(output)} Pointer to COS value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _MARK_COS_AS_COS2.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtMarkCosAsCos2Get(L7_uint32 UnitIndex, 
                                                 L7_uint32 policyIndex, 
                                                 L7_uint32 policyInstIndex, 
                                                 L7_uint32 policyAttrIndex, 
                                                 L7_uint32 *pCosVal);

/*********************************************************************
* @purpose  Get the mark Secondary COS (COS2) attribute statement value
*
* @param    UnitIndex       @b{(input)} System unit number
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    policyAttrIndex @b{(input)} Policy attribute index
* @param    pCosVal         @b{(output)} Pointer to COS2 value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _MARK_COS2VAL.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtMarkCos2ValGet(L7_uint32 UnitIndex, 
                                                  L7_uint32 policyIndex, 
                                                  L7_uint32 policyInstIndex, 
                                                  L7_uint32 policyAttrIndex, 
                                                  L7_uint32 *pCosVal);

/*********************************************************************
* @purpose  Set the mark Secondary COS attribute statement value
*
* @param    UnitIndex       @b{(input)} System unit number
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    policyAttrIndex @b{(input)} Policy attribute index
* @param    cosVal          @b{(input)} COS2 value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _MARK_COS2VAL.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtMarkCos2ValSet(L7_uint32 UnitIndex, 
                                                  L7_uint32 policyIndex, 
                                                  L7_uint32 policyInstIndex, 
                                                  L7_uint32 policyAttrIndex, 
                                                  L7_uint32 cosVal);

/*********************************************************************
* @purpose  Get the marking IP DSCP attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pDscpVal    @b{(output)} Pointer to DSCP value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _MARK_IPDSCPVAL.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtMarkIpDscpValGet(L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 *pDscpVal);

/*********************************************************************
* @purpose  Set the marking IP DSCP attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    dscpVal     @b{(input)} DSCP value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _MARK_IPDSCPVAL.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtMarkIpDscpValSet(L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 dscpVal);

/*********************************************************************
* @purpose  Get the marking IP Precedence attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pPrecVal    @b{(output)} Pointer to IP Precedence value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _MARK_IPPRECEDENCEVAL.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtMarkIpPrecedenceValGet(L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 *pPrecVal);

/*********************************************************************
* @purpose  Set the marking IP Precedence attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    precVal     @b{(input)} IP Precedence value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _MARK_IPPRECEDENCEVAL.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtMarkIpPrecedenceValSet(L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 precVal);

/*********************************************************************
* @purpose  Get the mirror interface attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pIntIfNum   @b{(output)} Pointer to mirror-to interface
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE
*
* @notes    Usage of this function is only allowed for an attribute
*           statement entry type of:  _MIRROR.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtMirrorIntfGet(L7_uint32 UnitIndex,
                                                 L7_uint32 policyIndex,
                                                 L7_uint32 policyInstIndex,
                                                 L7_uint32 policyAttrIndex,
                                                 L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose  Set the mirror interface attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    intIfNum    @b{(input)} Mirror-to interface
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE
*
* @notes    Usage of this function is only allowed for an attribute
*           statement entry type of:  _MIRROR.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtMirrorIntfSet(L7_uint32 UnitIndex,
                                                 L7_uint32 policyIndex,
                                                 L7_uint32 policyInstIndex,
                                                 L7_uint32 policyAttrIndex,
                                                 L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the policing conform action attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pAction     @b{(output)} Pointer to police action value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SIMPLE, _POLICE_SINGLERATE,
*           or _POLICE_TWORATE.
*
* @notes    A policing action is one of _DROP, _MARKxxxx, or _SEND.
*           When _MARKxxxx is used, a police conform value must 
*           also be specified.  
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceConformActGet(L7_uint32 UnitIndex, 
                                                     L7_uint32 policyIndex, 
                                                     L7_uint32 policyInstIndex, 
                                                     L7_uint32 policyAttrIndex, 
                              L7_USMDB_MIB_DIFFSERV_POLICE_ACT_t *pAction);

/*********************************************************************
* @purpose  Set the policing conform action attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    action      @b{(input)} Police action value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SIMPLE, _POLICE_SINGLERATE,
*           or _POLICE_TWORATE.
*
* @notes    A policing action is one of _DROP, _MARKxxxx, or _SEND.
*           When _MARKxxxx is used, a police conform value must 
*           also be specified.  
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceConformActSet(L7_uint32 UnitIndex, 
                                                     L7_uint32 policyIndex, 
                                                     L7_uint32 policyInstIndex, 
                                                     L7_uint32 policyAttrIndex, 
                              L7_USMDB_MIB_DIFFSERV_POLICE_ACT_t action);

/*********************************************************************
* @purpose  Get the policing conform value attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pValue      @b{(output)} Pointer to police conform value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SIMPLE, _POLICE_SINGLERATE,
*           or _POLICE_TWORATE.
*
* @notes    This value is required when a police conform action of _MARKxxxx
*           is used.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceConformValGet(L7_uint32 UnitIndex, 
                                                       L7_uint32 policyIndex, 
                                                       L7_uint32 policyInstIndex, 
                                                       L7_uint32 policyAttrIndex, 
                                                       L7_uint32 *pValue);

/*********************************************************************
* @purpose  Set the policing conform value attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    value     @b{(input)} Police conform value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SIMPLE, _POLICE_SINGLERATE,
*           or _POLICE_TWORATE.
*
* @notes    This value is required when a police conform action of _MARKxxxx
*           is used.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceConformValSet(L7_uint32 UnitIndex, 
                                                     L7_uint32 policyIndex, 
                                                     L7_uint32 policyInstIndex, 
                                                     L7_uint32 policyAttrIndex, 
                                                     L7_uint32 value);

/*********************************************************************
* @purpose  Get the policing exceed action attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pAction     @b{(output)} Pointer to police action value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SINGLERATE or _POLICE_TWORATE.
*
* @notes    A policing action is one of _DROP, _MARKxxxx, or _SEND.
*           When _MARKxxxx is used, a police exceed value must 
*           also be specified.  
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceExceedActGet(L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                              L7_USMDB_MIB_DIFFSERV_POLICE_ACT_t *pAction);

/*********************************************************************
* @purpose  Set the policing exceed action attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    action      @b{(input)} Police action value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SINGLERATE or _POLICE_TWORATE.
*
* @notes    A policing action is one of _DROP, _MARKxxxx, or _SEND.
*           When _MARKxxxx is used, a police exceed value must 
*           also be specified.  
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceExceedActSet(L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                              L7_USMDB_MIB_DIFFSERV_POLICE_ACT_t action);

/*********************************************************************
* @purpose  Get the policing exceed value attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pValue    @b{(output)} Pointer to police exceed value
*              
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SINGLERATE or _POLICE_TWORATE.
*
* @notes    This value is required when a police exceed action of _MARKxxxx
*           is used.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceExceedValGet(L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 *pValue);

/*********************************************************************
* @purpose  Set the policing exceed value attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    value       @b{(input)} Police exceed value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SINGLERATE or _POLICE_TWORATE.
*
* @notes    This value is required when a police exceed action of _MARKxxxx
*           is used.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceExceedValSet(L7_uint32 UnitIndex, 
                                                      L7_uint32 policyIndex, 
                                                      L7_uint32 policyInstIndex, 
                                                      L7_uint32 policyAttrIndex, 
                                                      L7_uint32 value);

/*********************************************************************
* @purpose  Get the policing nonconform action attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pAction     @b{(output)} Pointer to police action value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SIMPLE, _POLICE_SINGLERATE,
*           or _POLICE_TWORATE.
*
* @notes    A policing action is one of _DROP, _MARKxxxx, or _SEND.
*           When _MARKxxxx is used, a police nonconform value must 
*           also be specified.  
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceNonconformActGet(L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                              L7_USMDB_MIB_DIFFSERV_POLICE_ACT_t *pAction);

/*********************************************************************
* @purpose  Set the policing nonconform action attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    action      @b{(input)} Police action value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SIMPLE, _POLICE_SINGLERATE,
*           or _POLICE_TWORATE.
*
* @notes    A policing action is one of _DROP, _MARKxxxx, or _SEND.
*           When _MARKxxxx is used, a police nonconform value must 
*           also be specified.  
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceNonconformActSet(L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                              L7_USMDB_MIB_DIFFSERV_POLICE_ACT_t action);

/*********************************************************************
* @purpose  Get the policing nonconform DSCP value attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pDscpVal    @b{(output)} Pointer to police nonconform value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SIMPLE, _POLICE_SINGLERATE,
*           or _POLICE_TWORATE.
*
* @notes    This value is required when a police nonconform action of _MARKxxxx
*           is used.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceNonconformValGet(L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 *pValue);

/*********************************************************************
* @purpose  Set the policing nonconform value attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    value       @b{(input)} Police noncofrm value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SIMPLE, _POLICE_SINGLERATE,
*           or _POLICE_TWORATE.
*
* @notes    This value is required when a police nonconform action of _MARKxxxx
*           is used.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceNonconformValSet(L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 value);

/*********************************************************************
* @purpose  Get the policing color conform attribute class index value
*
* @param    UnitIndex       @b{(input)} System unit number
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    policyAttrIndex @b{(input)} Policy attribute index
* @param    pClassIndex     @b{(output)} Pointer to class index output value  
*
* @returns  L7_SUCCESS  
* @returns  L7_NOT_EXIST  Value is not configured
* @returns  L7_FAILURE    Improper usage or other malfunction
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceColorConformIndexGet(
                                                     L7_uint32 UnitIndex, 
                                                     L7_uint32 policyIndex, 
                                                     L7_uint32 policyInstIndex, 
                                                     L7_uint32 policyAttrIndex, 
                                                     L7_uint32 *pClassIndex);

/*********************************************************************
* @purpose  Set the policing color conform attribute class index value
*
* @param    UnitIndex       @b{(input)} System unit number
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    policyAttrIndex @b{(input)} Policy attribute index
* @param    classIndex      @b{(input)} Class index value to be set
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR      Invalid class definition for color-aware use
* @returns  L7_REQUEST_DENIED Exceed color-aware parms same as Conform.
* @returns  L7_FAILURE    Improper usage or other malfunction
*
* @notes    A classIndex of 0 sets the conform mode to 'color blind'.
*
* @notes    In color-aware mode, the color conform mode and value must
*           not be the same as the color exceed mode and value.  The
*           L7_REQUEST_DENIED return value is specifically used to 
*           indicate this error condition.
*
* @notes    Usage of this function is only meaningful for an attribute 
*           statement entry type of:  _POLICE_SIMPLE, _POLICE_SINGLERATE,
*           or _POLICE_TWORATE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceColorConformIndexSet(
                                                       L7_uint32 UnitIndex, 
                                                       L7_uint32 policyIndex, 
                                                       L7_uint32 policyInstIndex, 
                                                       L7_uint32 policyAttrIndex, 
                                                       L7_uint32 classIndex);

/*********************************************************************
* @purpose  Get the policing color conform mode attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pMode       @b{(output)} Pointer to police color mode
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SIMPLE, _POLICE_SINGLERATE,
*           or _POLICE_TWORATE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceColorConformModeGet(L7_uint32 UnitIndex, 
                                                     L7_uint32 policyIndex, 
                                                     L7_uint32 policyInstIndex, 
                                                     L7_uint32 policyAttrIndex, 
                              L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_t *pMode);

/*********************************************************************
* @purpose  Get the policing color conform value attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pVal        @b{(output)} Pointer to police color value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SIMPLE, _POLICE_SINGLERATE,
*           or _POLICE_TWORATE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceColorConformValGet(L7_uint32 UnitIndex, 
                                                     L7_uint32 policyIndex, 
                                                     L7_uint32 policyInstIndex, 
                                                     L7_uint32 policyAttrIndex, 
                                                     L7_uint32 *pVal);

/*********************************************************************
* @purpose  Get the policing color exceed attribute class index value
*
* @param    UnitIndex       @b{(input)} System unit number
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    policyAttrIndex @b{(input)} Policy attribute index
* @param    pClassIndex     @b{(output)} Pointer to class index output value  
*
* @returns  L7_SUCCESS  
* @returns  L7_NOT_EXIST  Value is not configured
* @returns  L7_FAILURE    Improper usage or other malfunction
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceColorExceedIndexGet(
                                                     L7_uint32 UnitIndex, 
                                                     L7_uint32 policyIndex, 
                                                     L7_uint32 policyInstIndex, 
                                                     L7_uint32 policyAttrIndex, 
                                                     L7_uint32 *pClassIndex);

/*********************************************************************
* @purpose  Set the policing color exceed attribute class index value
*
* @param    UnitIndex       @b{(input)} System unit number
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    policyAttrIndex @b{(input)} Policy attribute index
* @param    classIndex      @b{(input)} Class index value to be set
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR      Invalid class definition for color-aware use
* @returns  L7_REQUEST_DENIED Exceed color-aware parms same as Conform.
* @returns  L7_FAILURE    Improper usage or other malfunction
*
* @notes    A classIndex of 0 sets the exceed mode to 'color blind'.
*
* @notes    In color-aware mode, the color exceed mode and value must
*           not be the same as the color conform mode and value.  The
*           L7_REQUEST_DENIED return value is specifically used to 
*           indicate this error condition.
*
* @notes    Usage of this function is only meaningful for an attribute 
*           statement entry type of:  _POLICE_SINGLERATE or _POLICE_TWORATE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceColorExceedIndexSet(
                                                       L7_uint32 UnitIndex, 
                                                       L7_uint32 policyIndex, 
                                                       L7_uint32 policyInstIndex, 
                                                       L7_uint32 policyAttrIndex, 
                                                       L7_uint32 classIndex);

/*********************************************************************
* @purpose  Get the policing color exceed mode attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pMode       @b{(output)} Pointer to police color mode
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SINGLERATE or _POLICE_TWORATE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceColorExceedModeGet(L7_uint32 UnitIndex, 
                                                     L7_uint32 policyIndex, 
                                                     L7_uint32 policyInstIndex, 
                                                     L7_uint32 policyAttrIndex, 
                              L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_t *pMode);

/*********************************************************************
* @purpose  Get the policing color exceed value attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pVal        @b{(output)} Pointer to police color value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SINGLERATE or _POLICE_TWORATE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceColorExceedValGet(L7_uint32 UnitIndex, 
                                                     L7_uint32 policyIndex, 
                                                     L7_uint32 policyInstIndex, 
                                                     L7_uint32 policyAttrIndex, 
                                                     L7_uint32 *pVal);

/*********************************************************************
* @purpose  Get the policing color aware mode and value from the specified
*           class index
*
* @param    UnitIndex       @b{(input)}  System unit number
* @param    classIndex      @b{(input)}  Class index
* @param    pColorMode      @b{(output)} Pointer to color mode output location  
* @param    pColorValue     @b{(output)} Pointer to color value output location  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR      Invalid class definition for color-aware use
* @returns  L7_FAILURE    Improper usage or other malfunction
*
* @notes    The class definition used for policing color awareness is only
*           allowed to contain a single, non-excluded class match condition
*           identifying one of the supported comparison fields: COS,
*           IP DSCP, IP Precedence, or Secondary COS.  Anything else results
*           in an L7_ERROR return code.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceColorAwareClassInfoGet(
                                                 L7_uint32 UnitIndex,
                                                 L7_uint32 classIndex, 
                                                 L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_t *pColorMode,
                                                 L7_uint32 *pColorValue);

/*********************************************************************
* @purpose  Determine if a DiffServ class is valid for use as a policing
*           color aware designation
*
* @param    UnitIndex       @b{(input)}  System unit number
* @param    policyIndex     @b{(input)}  Policy index
* @param    policyInstIndex @b{(input)}  Policy instance index
* @param    classIndex      @b{(input)}  Class index value to be set
*
* @returns  L7_SUCCESS    Class is valid for color-aware use
* @returns  L7_ERROR      Invalid class definition for color-aware use
* @returns  L7_FAILURE    Improper usage or other malfunction
*
* @notes    The class definition used for policing color awareness is only
*           allowed to contain a single, non-excluded class match condition
*           identifying one of the supported comparison fields: COS,
*           IP DSCP, IP Precedence, or Secondary COS.  Also, the field
*           used in this class must not conflict with the classifier
*           match conditions for this policy-class instance.  Anything else
*           results in an L7_ERROR return code.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceColorAwareClassValidate(
                                                       L7_uint32 UnitIndex, 
                                                       L7_uint32 policyIndex, 
                                                       L7_uint32 policyInstIndex, 
                                                       L7_uint32 classIndex);

/*********************************************************************
* @purpose  Determine if color aware policing definitions are equivalent 
*           for two DiffServ classes 
*
* @param    UnitIndex     @b{(input)}  System unit number
* @param    classIndex1   @b{(input)}  First class index to compare
* @param    classIndex2   @b{(input)}  Second class index to compare
*
* @returns  L7_TRUE       Classes are color-aware with equivalent mode, val
* @returns  L7_FALSE      Classes not both color-aware, or not equivalent 
*
* @notes    The intent of this API is to compare two color-aware class 
*           definitions to determine if they match on the same color 
*           field (mode) and value.  This is typically used when comparing
*           the conform and exceed color class contents.
*
* @end
*********************************************************************/
L7_BOOL usmDbDiffServPolicyAttrStmtPoliceColorAwareClassesAreEquivalent(
                                                       L7_uint32 UnitIndex, 
                                                       L7_uint32 classIndex1,
                                                       L7_uint32 classIndex2);

/*********************************************************************
* @purpose  Get the simple policing committed rate attribute statement
*           value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pRate       @b{(output)} Pointer to rate value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SIMPLE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceSimpleCrateGet(L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 *pRate);

/*********************************************************************
* @purpose  Set the simple policing committed rate attribute statement
*           value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    rate        @b{(input)} Rate value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SIMPLE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceSimpleCrateSet(L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 rate);

/*********************************************************************
* @purpose  Get the simple policing committed burst size attribute statement
*           value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pBurst      @b{(output)} Pointer to burst size value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SIMPLE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceSimpleCburstGet(L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 *pBurst);

/*********************************************************************
* @purpose  Set the simple policing committed burst size attribute statement
*           value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    burst       @b{(input)} Burst size value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SIMPLE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceSimpleCburstSet(
                                                    L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 burst);

/*********************************************************************
* @purpose  Get the single-rate policing committed rate attribute
*           statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pRate       @b{(output)} Pointer to rate value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SINGLERATE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceSinglerateCrateGet(
                                                    L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 *pRate);

/*********************************************************************
* @purpose  Set the single-rate policing committed rate attribute
*           statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    rate        @b{(output)} Rate value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SINGLERATE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceSinglerateCrateSet(L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 rate);

/*********************************************************************
* @purpose  Get the single-rate policing committed burst size attribute
*           statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pBurst      @b{(output)} Pointer to burst size value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SINGLERATE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceSinglerateCburstGet(
                                                    L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 *pBurst);

/*********************************************************************
* @purpose  Set the single-rate policing committed burst size attribute
*           statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    burst       @b{(input)} Burst size value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SINGLERATE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceSinglerateCburstSet(
                                                    L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 burst);

/*********************************************************************
* @purpose  Get the single-rate policing excess burst size attribute
*           statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pBurst      @b{(output)} Pointer to burst size value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SINGLERATE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceSinglerateEburstGet(
                                                    L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 *pBurst);

/*********************************************************************
* @purpose  Set the single-rate policing excess burst size attribute
*           statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    burst       @b{(input)} Burst size value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_SINGLERATE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceSinglerateEburstSet(
                                                    L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 burst);

/*********************************************************************
* @purpose  Get the two-rate policing committed rate attribute
*           statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pRate       @b{(output)} Pointer to rate value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_TWORATE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceTworateCrateGet(
                                                    L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 *pRate);

/*********************************************************************
* @purpose  Set the two-rate policing committed rate attribute
*           statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    rate        @b{(output)} Rate value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_TWORATE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceTworateCrateSet(L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 rate);

/*********************************************************************
* @purpose  Get the two-rate policing committed burst size attribute
*           statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pBurst      @b{(output)} Pointer to burst size value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_TWORATE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceTworateCburstGet(
                                                    L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 *pBurst);

/*********************************************************************
* @purpose  Set the two-rate policing committed burst size attribute
*           statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    burst       @b{(input)} Burst size value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_TWORATE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceTworateCburstSet(
                                                    L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 burst);

/*********************************************************************
* @purpose  Get the two-rate policing peak rate attribute statement
*           value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pRate       @b{(output)} Pointer to rate value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_TWORATE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceTworatePrateGet(
                                                    L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 *pRate);

/*********************************************************************
* @purpose  Set the two-rate policing peak rate attribute statement
*           value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    rate        @b{(output)} Rate value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_TWORATE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceTworatePrateSet(L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 rate);

/*********************************************************************
* @purpose  Get the two-rate policing peak burst size attribute statement
*           value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pBurst      @b{(output)} Pointer to burst size value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_TWORATE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceTworatePburstGet(
                                                    L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 *pBurst);

/*********************************************************************
* @purpose  Set the two-rate policing peak burst size attribute statement
*           value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    burst       @b{(input)} Burst size value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _POLICE_TWORATE.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtPoliceTworatePburstSet(
                                                    L7_uint32 UnitIndex, 
                                                    L7_uint32 policyIndex, 
                                                    L7_uint32 policyInstIndex, 
                                                    L7_uint32 policyAttrIndex, 
                                                    L7_uint32 burst);

/*********************************************************************
* @purpose  Get the redirect interface attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pIntIfNum   @b{(output)} Pointer to redirection interface
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _REDIRECT.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtRedirectIntfGet(L7_uint32 UnitIndex, 
                                                   L7_uint32 policyIndex, 
                                                   L7_uint32 policyInstIndex, 
                                                   L7_uint32 policyAttrIndex, 
                                                   L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose  Set the redirect interface attribute statement value
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    intIfNum    @b{(input)} Redirection interface
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current match entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for an attribute 
*           statement entry type of:  _REDIRECT.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStmtRedirectIntfSet(L7_uint32 UnitIndex, 
                                                   L7_uint32 policyIndex, 
                                                   L7_uint32 policyInstIndex, 
                                                   L7_uint32 policyAttrIndex, 
                                                   L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the storage type for the specified Policy Attribute 
*           Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pValue      @b{(output)} Pointer to storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStorageTypeGet(L7_uint32 UnitIndex,
                                              L7_uint32 policyIndex,
                                              L7_uint32 policyInstIndex,
                                              L7_uint32 policyAttrIndex, 
                              L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t *pValue);

/*********************************************************************
* @purpose  Set the storage type for the specified Policy Attribute 
*           Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    value       @b{(input)} Storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrStorageTypeSet(L7_uint32 UnitIndex,
                                              L7_uint32 policyIndex,
                                              L7_uint32 policyInstIndex,
                                              L7_uint32 policyAttrIndex, 
                              L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t value);

/*********************************************************************
* @purpose  Get the current status of the specified Policy Attribute
*           Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    pValue      @b{(output)} Pointer to row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrRowStatusGet(L7_uint32 UnitIndex,
                                            L7_uint32 policyIndex,
                                            L7_uint32 policyInstIndex,
                                            L7_uint32 policyAttrIndex, 
                              L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t *pValue);

/*********************************************************************
* @purpose  Set the current status of the specified Policy Attribute
*           Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index
* @param    value       @b{(input)} Row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyAttrRowStatusSet(L7_uint32 UnitIndex,
                                            L7_uint32 policyIndex,
                                            L7_uint32 policyInstIndex,
                                            L7_uint32 policyAttrIndex, 
                              L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t value);


/*
================================================
================================================
================================================
 
   POLICY-CLASS INBOUND PERFORMANCE TABLE API

================================================
================================================
================================================
*/

/*********************************************************************
* @purpose  Verify that a Policy-Class In Perf Table row exists for the
*           specified indexes
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    intIfNum    @b{(input)} Internal interface number
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyPerfInGet(L7_uint32 UnitIndex, 
                                     L7_uint32 policyIndex,
                                     L7_uint32 policyInstIndex,
                                     L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine next sequential row in the Policy-Class In Perf Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    prevPolicyIndex  @b{(input)} Policy index to begin search
* @param    prevPolicyInstIndex  @b{(input)} Policy instance index to begin 
*                                              search
* @param    prevIntIfNum     @b{(input)} Internal interface index to begin
*                                          search
* @param    pPolicyIndex  @b{(output)} Pointer to next sequential policy index
*                                      value
* @param    pPolicyInstIndex  @b{(output)} Pointer to next sequential policy
*                                            instance index value
* @param    pIntIfNum     @b{(output)} Pointer to next sequential internal     
*                                        interface number
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Reached end of table
* @returns  L7_FAILURE  
*
* @notes    A prevPolicyIndex value of 0 is used to find the first policy
*           entry in the table.
*
* @notes    A prevPolicyInstIndex value of 0 is used to find the first class
*           instance relative to the specified prevPolicyIndex.
*
* @notes    A prevIntIfNum value of 0 is used to find the first interface
*           relative to the specified prevPolicyIndex and prevPolicyInstIndex.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyPerfInGetNext(L7_uint32 UnitIndex, 
                                         L7_uint32 prevPolicyIndex, 
                                         L7_uint32 prevPolicyInstIndex, 
                                         L7_uint32 prevIntIfNum,
                                         L7_uint32 *pPolicyIndex,
                                         L7_uint32 *pPolicyInstIndex,
                                         L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose  Get the inbound offered octets count for the specified
*           policy class instance
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    intIfNum    @b{(input)} Internal interface number
* @param    policyIndex @b{(input)} Policy index
* @param    pValueHi    @b{(output)} Pointer to counter value (upper 32-bits)
* @param    pValueLo    @b{(output)} Pointer to counter value (lower 32-bits)
*
* @returns  L7_SUCCESS  
* @returns  L7_NOT_SUPPORTED  Specified counter not supported  
* @returns  L7_FAILURE  
*
* @notes    This one API function is used to retrieve either or both
*           32-bit portions of this 64-bit high-capacity counter.  The
*           caller indicates which portion(s) are desired by providing
*           a non-null pointer value(s) for pValueHi, pValueLo.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyPerfInOfferedOctetsGet(L7_uint32 UnitIndex,
                                                  L7_uint32 policyIndex,
                                                  L7_uint32 policyInstIndex,
                                                  L7_uint32 intIfNum,
                                                  L7_uint32 *pValueHi,
                                                  L7_uint32 *pValueLo);
                                              
/*********************************************************************
* @purpose  Get the inbound offered packets count for the specified
*           policy class instance
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    intIfNum    @b{(input)} Internal interface number
* @param    policyIndex @b{(input)} Policy index
* @param    pValueHi    @b{(output)} Pointer to counter value (upper 32-bits)
* @param    pValueLo    @b{(output)} Pointer to counter value (lower 32-bits)
*
* @returns  L7_SUCCESS  
* @returns  L7_NOT_SUPPORTED  Specified counter not supported  
* @returns  L7_FAILURE  
*
* @notes    This one API function is used to retrieve either or both
*           32-bit portions of this 64-bit high-capacity counter.  The
*           caller indicates which portion(s) are desired by providing
*           a non-null pointer value(s) for pValueHi, pValueLo.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyPerfInOfferedPacketsGet(L7_uint32 UnitIndex,
                                                   L7_uint32 policyIndex,
                                                   L7_uint32 policyInstIndex,
                                                   L7_uint32 intIfNum,
                                                   L7_uint32 *pValueHi,
                                                   L7_uint32 *pValueLo);
                                              
/*********************************************************************
* @purpose  Get the inbound discarded octets count for the specified
*           policy class instance
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    intIfNum    @b{(input)} Internal interface number
* @param    policyIndex @b{(input)} Policy index
* @param    pValueHi    @b{(output)} Pointer to counter value (upper 32-bits)
* @param    pValueLo    @b{(output)} Pointer to counter value (lower 32-bits)
*
* @returns  L7_SUCCESS  
* @returns  L7_NOT_SUPPORTED  Specified counter not supported  
* @returns  L7_FAILURE  
*
* @notes    This one API function is used to retrieve either or both
*           32-bit portions of this 64-bit high-capacity counter.  The
*           caller indicates which portion(s) are desired by providing
*           a non-null pointer value(s) for pValueHi, pValueLo.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyPerfInDiscardedOctetsGet(L7_uint32 UnitIndex,
                                                    L7_uint32 policyIndex,
                                                    L7_uint32 policyInstIndex,
                                                    L7_uint32 intIfNum,
                                                    L7_uint32 *pValueHi,
                                                    L7_uint32 *pValueLo);
                                              
/*********************************************************************
* @purpose  Get the inbound discarded packets count for the specified
*           policy class instance
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    intIfNum    @b{(input)} Internal interface number
* @param    policyIndex @b{(input)} Policy index
* @param    pValueHi    @b{(output)} Pointer to counter value (upper 32-bits)
* @param    pValueLo    @b{(output)} Pointer to counter value (lower 32-bits)
*
* @returns  L7_SUCCESS  
* @returns  L7_NOT_SUPPORTED  Specified counter not supported  
* @returns  L7_FAILURE  
*
* @notes    This one API function is used to retrieve either or both
*           32-bit portions of this 64-bit high-capacity counter.  The
*           caller indicates which portion(s) are desired by providing
*           a non-null pointer value(s) for pValueHi, pValueLo.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyPerfInDiscardedPacketsGet(L7_uint32 UnitIndex,
                                                     L7_uint32 policyIndex,
                                                     L7_uint32 policyInstIndex,
                                                     L7_uint32 intIfNum,
                                                     L7_uint32 *pValueHi,
                                                     L7_uint32 *pValueLo);
                                              
/*********************************************************************
* @purpose  Get the storage type for the specified Policy-Class In Perf 
*           Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    intIfNum    @b{(input)} Internal interface number
* @param    pValue      @b{(output)} Pointer to storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyPerfInStorageTypeGet(L7_uint32 UnitIndex,
                                                L7_uint32 policyIndex,
                                                L7_uint32 policyInstIndex,
                                                L7_uint32 intIfNum,
                              L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t *pValue);

/*********************************************************************
* @purpose  Get the current status of the specified Policy-Class In Perf
*           Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    intIfNum    @b{(input)} Internal interface number
* @param    pValue      @b{(output)} Pointer to row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyPerfInRowStatusGet(L7_uint32 UnitIndex,
                                              L7_uint32 policyIndex,
                                              L7_uint32 policyInstIndex,
                                              L7_uint32 intIfNum,
                              L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t *pValue);

/*
=================================================
=================================================
=================================================
 
   POLICY-CLASS OUTBOUND PERFORMANCE TABLE API

=================================================
=================================================
=================================================
*/

/*********************************************************************
* @purpose  Verify that a Policy-Class Out Perf Table row exists for the
*           specified indexes
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    intIfNum    @b{(input)} Internal interface number
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyPerfOutGet(L7_uint32 UnitIndex, 
                                      L7_uint32 policyIndex,
                                      L7_uint32 policyInstIndex,
                                      L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine next sequential row in the Policy-Class Out Perf Table
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    prevPolicyIndex  @b{(input)} Policy index to begin search
* @param    prevPolicyInstIndex  @b{(input)} Policy instance index to begin 
*                                              search
* @param    prevIntIfNum     @b{(input)} Internal interface index to begin
*                                          search
* @param    pPolicyIndex  @b{(output)} Pointer to next sequential policy index
*                                      value
* @param    pPolicyInstIndex  @b{(output)} Pointer to next sequential policy
*                                            instance index value
* @param    pIntIfNum     @b{(output)} Pointer to next sequential internal     
*                                        interface number
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Reached end of table
* @returns  L7_FAILURE  
*
* @notes    A prevPolicyIndex value of 0 is used to find the first policy
*           entry in the table.
*
* @notes    A prevPolicyInstIndex value of 0 is used to find the first class
*           instance relative to the specified prevPolicyIndex.
*
* @notes    A prevIntIfNum value of 0 is used to find the first interface
*           relative to the specified prevPolicyIndex and prevPolicyInstIndex.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyPerfOutGetNext(L7_uint32 UnitIndex, 
                                          L7_uint32 prevPolicyIndex, 
                                          L7_uint32 prevPolicyInstIndex, 
                                          L7_uint32 prevIntIfNum,
                                          L7_uint32 *pPolicyIndex,
                                          L7_uint32 *pPolicyInstIndex,
                                          L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose  Get the outbound offered octets count for the specified
*           policy class instance
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    intIfNum    @b{(input)} Internal interface number
* @param    policyIndex @b{(input)} Policy index
* @param    pValueHi    @b{(output)} Pointer to counter value (upper 32-bits)
* @param    pValueLo    @b{(output)} Pointer to counter value (lower 32-bits)
*
* @returns  L7_SUCCESS  
* @returns  L7_NOT_SUPPORTED  Specified counter not supported  
* @returns  L7_FAILURE  
*
* @notes    This one API function is used to retrieve either or both
*           32-bit portions of this 64-bit high-capacity counter.  The
*           caller indicates which portion(s) are desired by providing
*           a non-null pointer value(s) for pValueHi, pValueLo.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyPerfOutOfferedOctetsGet(L7_uint32 UnitIndex,
                                                   L7_uint32 policyIndex,
                                                   L7_uint32 policyInstIndex,
                                                   L7_uint32 intIfNum,
                                                   L7_uint32 *pValueHi,
                                                   L7_uint32 *pValueLo);

/*********************************************************************
* @purpose  Get the outbound offered packets count for the specified
*           policy class instance
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    intIfNum    @b{(input)} Internal interface number
* @param    policyIndex @b{(input)} Policy index
* @param    pValueHi    @b{(output)} Pointer to counter value (upper 32-bits)
* @param    pValueLo    @b{(output)} Pointer to counter value (lower 32-bits)
*
* @returns  L7_SUCCESS  
* @returns  L7_NOT_SUPPORTED  Specified counter not supported  
* @returns  L7_FAILURE  
*
* @notes    This one API function is used to retrieve either or both
*           32-bit portions of this 64-bit high-capacity counter.  The
*           caller indicates which portion(s) are desired by providing
*           a non-null pointer value(s) for pValueHi, pValueLo.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyPerfOutOfferedPacketsGet(L7_uint32 UnitIndex,
                                                    L7_uint32 policyIndex,
                                                    L7_uint32 policyInstIndex,
                                                    L7_uint32 intIfNum,
                                                    L7_uint32 *pValueHi,
                                                    L7_uint32 *pValueLo);

/*********************************************************************
* @purpose  Get the outbound discarded octets count for the specified
*           policy class instance
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    intIfNum    @b{(input)} Internal interface number
* @param    policyIndex @b{(input)} Policy index
* @param    pValueHi    @b{(output)} Pointer to counter value (upper 32-bits)
* @param    pValueLo    @b{(output)} Pointer to counter value (lower 32-bits)
*
* @returns  L7_SUCCESS  
* @returns  L7_NOT_SUPPORTED  Specified counter not supported  
* @returns  L7_FAILURE  
*
* @notes    This one API function is used to retrieve either or both
*           32-bit portions of this 64-bit high-capacity counter.  The
*           caller indicates which portion(s) are desired by providing
*           a non-null pointer value(s) for pValueHi, pValueLo.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyPerfOutDiscardedOctetsGet(L7_uint32 UnitIndex,
                                                     L7_uint32 policyIndex,
                                                     L7_uint32 policyInstIndex,
                                                     L7_uint32 intIfNum,
                                                     L7_uint32 *pValueHi,
                                                     L7_uint32 *pValueLo);

/*********************************************************************
* @purpose  Get the outbound discarded packets count for the specified
*           policy class instance
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    intIfNum    @b{(input)} Internal interface number
* @param    policyIndex @b{(input)} Policy index
* @param    pValueHi    @b{(output)} Pointer to counter value (upper 32-bits)
* @param    pValueLo    @b{(output)} Pointer to counter value (lower 32-bits)
*
* @returns  L7_SUCCESS  
* @returns  L7_NOT_SUPPORTED  Specified counter not supported  
* @returns  L7_FAILURE  
*
* @notes    This one API function is used to retrieve either or both
*           32-bit portions of this 64-bit high-capacity counter.  The
*           caller indicates which portion(s) are desired by providing
*           a non-null pointer value(s) for pValueHi, pValueLo.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyPerfOutDiscardedPacketsGet(L7_uint32 UnitIndex,
                                                      L7_uint32 policyIndex,
                                                      L7_uint32 policyInstIndex,
                                                      L7_uint32 intIfNum,
                                                      L7_uint32 *pValueHi,
                                                      L7_uint32 *pValueLo);

/*********************************************************************
* @purpose  Get the storage type for the specified Policy-Class Out Perf 
*           Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    intIfNum    @b{(input)} Internal interface number
* @param    pValue      @b{(output)} Pointer to storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyPerfOutStorageTypeGet(L7_uint32 UnitIndex,
                                                 L7_uint32 policyIndex,
                                                 L7_uint32 policyInstIndex,
                                                 L7_uint32 intIfNum,
                              L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t *pValue);

/*********************************************************************
* @purpose  Get the current status of the specified Policy-Class Out Perf
*           Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    policyIndex @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    intIfNum    @b{(input)} Internal interface number
* @param    pValue      @b{(output)} Pointer to row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServPolicyPerfOutRowStatusGet(L7_uint32 UnitIndex,
                                               L7_uint32 policyIndex,
                                               L7_uint32 policyInstIndex,
                                               L7_uint32 intIfNum,
                              L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t *pValue);

/*
=======================
=======================
=======================
 
   SERVICE TABLE API

=======================
=======================
=======================
*/

/*************************************************************************
* @purpose  Retrieve the maximum number of DiffServ service interfaces
*           supported by the system
*
* @param    UnitIndex   @b{(input)}  System unit number
* @param    pMaxNumIntf @b{(output)} Pointer to max number of service intf
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This value represents the total number of interfaces that
*           can be configured for DiffServ.  On some platforms, this 
*           number is less than the total number of interfaces (ports)
*           in the system.  Note that this is not the value of the 
*           highest intIfNum supported.
*           
* @notes    Each service interface is defined as having an inbound and 
*           outbound data path.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServServiceIntfMaxGet(L7_uint32 UnitIndex,
                                       L7_uint32 *pMaxNumIntf);

/*************************************************************************
* @purpose  Create a new row in the Service Table for the specified 
*           interface and direction
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    activateRow @b{(input)} Activate row when ready flag
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR               Index values in use, row not complete
* @returns  L7_ALREADY_CONFIGURED  Index values in use, row complete
* @returns  L7_FAILURE
*
* @notes    An L7_ERROR response means the service index values are
*           currently in use, but the row is not complete.  This is
*           typically caused when multiple requestors use the same
*           index values and race to create the new row.  The caller
*           should use new index values and re-attempt to create the row.
*
* @notes    An L7_ALREADY_CONFIGURED response means the service index values
*           are currently in use for a completed row.  The caller should
*           use new index values and re-attempt to create the row.
*
* @notes    The row status is initially set to 'not ready' and remains that 
*           way until every object in the row contains a valid value.  If the
*           activateRow parameter is set to L7_TRUE, the row status will then
*           be automatically updated to 'active'.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServServiceCreate(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                        L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                                   L7_BOOL activateRow);

/*************************************************************************
* @purpose  Delete a row from the Service Table for the specified
*           interface and direction
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServServiceDelete(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                        L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection);

/*********************************************************************
* @purpose  Verify that a Service Table row exists for the specified 
*           interface and direction
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServServiceGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                        L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection);

/*********************************************************************
* @purpose  Determine next sequential row in the Service Table
*
* @param    UnitIndex   @b{(input)} System unit number
*                                                 search
* @param    prevIntIfNum  @b{(input)} Internal interface number to begin search
* @param    prevIfDirection @b{(input)} Interface direction to begin search
* @param    pIntIfNum   @b{(output)} Pointer to next sequential internal 
*                                      interface number value
* @param    pIfDirection @b{(output)} Pointer to next sequential
*                                       interface direction value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Reached end of table
* @returns  L7_FAILURE  
*
* @notes    A prevIntIfNum value of 0 is used to find the first interface
*           entry the table.
*
* @notes    A prevIfDirection value of 0 is used to find the first
*           interface direction entry relative to the specified prevIntIfNum.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServServiceGetNext(L7_uint32 UnitIndex, 
                                    L7_uint32 prevIntIfNum, 
                        L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t prevIfDirection,
                                    L7_uint32 *pIntIfNum,
                        L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t *pIfDirection);

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Service Table 
*           to determine its legitimacy
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    objectId    @b{(input)} Object identifier
* @param    pValue      @b{(input)} Pointer to proposed value to be set
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Proposed value invalid
* @returns  L7_FAILURE  
*
* @notes    Only tests objects with access types READ-CREATE or READ-WRITE;
*           all other access types return L7_FAILURE.
*
* @notes    Caller must use one of the _SETTEST_OID_TYPE object identifiers
*           only (i.e., do not use the _TABLE_OBJECT type object names here).
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServServiceSetTest(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
                        L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
    L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_TYPE_t objectId, void *pValue);

/*********************************************************************
* @purpose  Get the policy index attached to the specified interface
*           and direction
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    pPolicyIndex  @b{(output)} Pointer to a policy index value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServServicePolicyIndexGet(L7_uint32 UnitIndex, 
                                           L7_uint32 intIfNum,
                        L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                                           L7_uint32 *pPolicyIndex);

/*********************************************************************
* @purpose  Set the policy index attached to the specified interface
*           and direction
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    policyIndex @b{(input)} Policy index
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServServicePolicyIndexSet(L7_uint32 UnitIndex, 
                                           L7_uint32 intIfNum,
                        L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                                           L7_uint32 policyIndex);

/*********************************************************************
* @purpose  Get the service interface operational status for the specified
*           Service Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    pIfOperStatus @b{(output)} Pointer to an intf oper status value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServServiceIfOperStatusGet(L7_uint32 UnitIndex, 
                                            L7_uint32 intIfNum,
                        L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                        L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_t *pIfOperStatus);

/*********************************************************************
* @purpose  Get the storage type for the specified Service Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    pValue      @b{(output)} Pointer to storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServServiceStorageTypeGet(L7_uint32 UnitIndex,
                                           L7_uint32 intIfNum,
                        L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                    L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t *pValue);

/*********************************************************************
* @purpose  Set the storage type for the specified Service Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    value       @b{(input)} Storage type value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServServiceStorageTypeSet(L7_uint32 UnitIndex,
                                           L7_uint32 intIfNum,
                        L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                    L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t value);

/*********************************************************************
* @purpose  Get the current status for the specified Service Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    pValue      @b{(output)} Pointer to row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServServiceRowStatusGet(L7_uint32 UnitIndex,
                                         L7_uint32 intIfNum,
                        L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                    L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t *pValue);

/*********************************************************************
* @purpose  Set the current status for the specified Service Table row
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    value       @b{(input)} Row status value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServServiceRowStatusSet(L7_uint32 UnitIndex,
                                         L7_uint32 intIfNum,
                        L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                    L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t value);

/*********************************************************************
* @purpose  Verifies the current status for the specified Service Table row(s)
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    value       @b{(input)} Row status expected value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    This is a helper function used for checking the row status 
*           against a desired value.  It is useful for handling the case
*           where the intIfNum value can be either an individual interface
*           number or L7_ALL_INTERFACES.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServServiceRowStatusVerify(L7_uint32 UnitIndex,
                                            L7_uint32 intIfNum,
                 L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                   L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t value);

/*
===================================
===================================
===================================
 
   SERVICE PERFORMANCE TABLE API

===================================
===================================
===================================
*/

/*********************************************************************
* @purpose  Get the offered octets count for the specified service 
*           interface and direction
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    pValueHi    @b{(output)} Pointer to counter value (upper 32-bits)
* @param    pValueLo    @b{(output)} Pointer to counter value (lower 32-bits)
*
* @returns  L7_SUCCESS  
* @returns  L7_NOT_SUPPORTED  Specified counter not supported  
* @returns  L7_FAILURE  
*
* @notes    This one API function is used to retrieve either or both
*           32-bit portions of this 64-bit high-capacity counter.  The
*           caller indicates which portion(s) are desired by providing
*           a non-null pointer value(s) for pValueHi, pValueLo.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServServicePerfOfferedOctetsGet(L7_uint32 UnitIndex,
                                                    L7_uint32 intIfNum,
                    L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                                                    L7_uint32 *pValueHi,
                                                    L7_uint32 *pValueLo);
                                              
/*********************************************************************
* @purpose  Get the offered packets count for the specified service 
*           interface and direction
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    pValueHi    @b{(output)} Pointer to counter value (upper 32-bits)
* @param    pValueLo    @b{(output)} Pointer to counter value (lower 32-bits)
*
* @returns  L7_SUCCESS  
* @returns  L7_NOT_SUPPORTED  Specified counter not supported  
* @returns  L7_FAILURE  
*
* @notes    This one API function is used to retrieve either or both
*           32-bit portions of this 64-bit high-capacity counter.  The
*           caller indicates which portion(s) are desired by providing
*           a non-null pointer value(s) for pValueHi, pValueLo.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServServicePerfOfferedPacketsGet(L7_uint32 UnitIndex,
                                                  L7_uint32 intIfNum,
                    L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                                                  L7_uint32 *pValueHi,
                                                  L7_uint32 *pValueLo);
                                              
/*********************************************************************
* @purpose  Get the discarded octets count for the specified service 
*           interface and direction
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    pValueHi    @b{(output)} Pointer to counter value (upper 32-bits)
* @param    pValueLo    @b{(output)} Pointer to counter value (lower 32-bits)
*
* @returns  L7_SUCCESS  
* @returns  L7_NOT_SUPPORTED  Specified counter not supported  
* @returns  L7_FAILURE  
*
* @notes    This one API function is used to retrieve either or both
*           32-bit portions of this 64-bit high-capacity counter.  The
*           caller indicates which portion(s) are desired by providing
*           a non-null pointer value(s) for pValueHi, pValueLo.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServServicePerfDiscardedOctetsGet(L7_uint32 UnitIndex,
                                                   L7_uint32 intIfNum,
                    L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                                                   L7_uint32 *pValueHi,
                                                   L7_uint32 *pValueLo);
                                              
/*********************************************************************
* @purpose  Get the discarded packets count for the specified service 
*           interface and direction
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    pValueHi    @b{(output)} Pointer to counter value (upper 32-bits)
* @param    pValueLo    @b{(output)} Pointer to counter value (lower 32-bits)
*
* @returns  L7_SUCCESS  
* @returns  L7_NOT_SUPPORTED  Specified counter not supported  
* @returns  L7_FAILURE  
*
* @notes    This one API function is used to retrieve either or both
*           32-bit portions of this 64-bit high-capacity counter.  The
*           caller indicates which portion(s) are desired by providing
*           a non-null pointer value(s) for pValueHi, pValueLo.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServServicePerfDiscardedPacketsGet(L7_uint32 UnitIndex,
                                                    L7_uint32 intIfNum,
                    L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                                                    L7_uint32 *pValueHi,
                                                    L7_uint32 *pValueLo);
                                              
/*********************************************************************
* @purpose  Get the sent octets count for the specified service 
*           interface and direction
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    pValueHi    @b{(output)} Pointer to counter value (upper 32-bits)
* @param    pValueLo    @b{(output)} Pointer to counter value (lower 32-bits)
*
* @returns  L7_SUCCESS  
* @returns  L7_NOT_SUPPORTED  Specified counter not supported  
* @returns  L7_FAILURE  
*
* @notes    This one API function is used to retrieve either or both
*           32-bit portions of this 64-bit high-capacity counter.  The
*           caller indicates which portion(s) are desired by providing
*           a non-null pointer value(s) for pValueHi, pValueLo.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServServicePerfSentOctetsGet(L7_uint32 UnitIndex,
                                              L7_uint32 intIfNum,
                    L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                                              L7_uint32 *pValueHi,
                                              L7_uint32 *pValueLo);
                                              
/*********************************************************************
* @purpose  Get the sent packets count for the specified service 
*           interface and direction
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    pValueHi    @b{(output)} Pointer to counter value (upper 32-bits)
* @param    pValueLo    @b{(output)} Pointer to counter value (lower 32-bits)
*
* @returns  L7_SUCCESS  
* @returns  L7_NOT_SUPPORTED  Specified counter not supported  
* @returns  L7_FAILURE  
*
* @notes    This one API function is used to retrieve either or both
*           32-bit portions of this 64-bit high-capacity counter.  The
*           caller indicates which portion(s) are desired by providing
*           a non-null pointer value(s) for pValueHi, pValueLo.
*
* @end
*********************************************************************/
L7_RC_t usmDbDiffServServicePerfSentPacketsGet(L7_uint32 UnitIndex,
                                               L7_uint32 intIfNum,
                    L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                                               L7_uint32 *pValueHi,
                                               L7_uint32 *pValueLo);
                                              
/*********************************************************************
* @purpose  Helper routine to check if the system supports the
*           specified policy or service performance counter
*
* @param    UnitIndex   @b{(input)} System unit number
* @param    tid         @b{(input)} Table Id
* @param    ctr         @b{(input)} Policy/Service Performance Counter
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    This routine gets called from all the usmdb performance
*           counter *get* functions.
*
* @end
*********************************************************************/
L7_BOOL usmDbDiffServIsPerfCounterSupported(L7_uint32 UnitIndex, 
                                            L7_USMDB_MIB_DIFFSERV_TABLE_ID_t tid,
                                            L7_uint32 ctr);

#endif /* USMDB_MIB_PRIVATE_DIFFSERV_API_H */
