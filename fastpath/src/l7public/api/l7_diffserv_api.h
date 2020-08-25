/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_diffserv_api.h
*
* @purpose    DiffServ component API functions
*
* @component  diffserv
*
* @comments   none
*
* @create     03/20/2002
*
* @author     gpaussa
* @end
*             
**********************************************************************/
#ifndef L7_DIFFSERV_API_H
#define L7_DIFFSERV_API_H

#include "l7_common.h"
#include "acl_exports.h"
#include "diffserv_exports.h"
#include "acl_exports.h"
#include "statsapi.h"
#include "usmdb_qos_diffserv_api.h"
#include "tlv_api.h"

/*
=========================================
=========================================
=========================================

   ENUMS FOR PRIVATE MIB TABLE OBJECTS

=========================================
=========================================
=========================================
*/

/* Enumerated values for General Status Group objects */
typedef enum
{
  L7_DIFFSERV_GEN_STATUS_TABLE_NONE = 0,

  /* L7_DIFFSERV_GEN_STATUS_ADMIN_MODE is accessed separately */
  L7_DIFFSERV_GEN_STATUS_TABLE_CLASS_SIZE,
  L7_DIFFSERV_GEN_STATUS_TABLE_CLASS_MAX,
  L7_DIFFSERV_GEN_STATUS_TABLE_CLASS_RULE_SIZE,
  L7_DIFFSERV_GEN_STATUS_TABLE_CLASS_RULE_MAX,
  L7_DIFFSERV_GEN_STATUS_TABLE_POLICY_SIZE,
  L7_DIFFSERV_GEN_STATUS_TABLE_POLICY_MAX,
  L7_DIFFSERV_GEN_STATUS_TABLE_POLICY_INST_SIZE,
  L7_DIFFSERV_GEN_STATUS_TABLE_POLICY_INST_MAX,
  L7_DIFFSERV_GEN_STATUS_TABLE_POLICY_ATTR_SIZE,
  L7_DIFFSERV_GEN_STATUS_TABLE_POLICY_ATTR_MAX,
  L7_DIFFSERV_GEN_STATUS_TABLE_SERVICE_SIZE,
  L7_DIFFSERV_GEN_STATUS_TABLE_SERVICE_MAX,

  L7_DIFFSERV_GEN_STATUS_TABLE_TOTAL            /* number of enum values */

} L7_DIFFSERV_GEN_STATUS_GROUP_OBJECT_t;

/* Enumerated values for Class Table objects */
typedef enum
{
  L7_DIFFSERV_CLASS_NAME = 1,
  L7_DIFFSERV_CLASS_TYPE,
  L7_DIFFSERV_CLASS_L3_PROTO,
  L7_DIFFSERV_CLASS_ACLTYPE,
  L7_DIFFSERV_CLASS_ACLNUM,
  L7_DIFFSERV_CLASS_STORAGE_TYPE,
  L7_DIFFSERV_CLASS_ROW_STATUS

} L7_DIFFSERV_CLASS_TABLE_OBJECT_t;

/* Enumerated values for Class Rule Table objects */
typedef enum
{
  L7_DIFFSERV_CLASS_RULE_MATCH_ENTRY_TYPE = 1,
  L7_DIFFSERV_CLASS_RULE_MATCH_COS,
  L7_DIFFSERV_CLASS_RULE_MATCH_COS2,
  L7_DIFFSERV_CLASS_RULE_MATCH_DSTIP_ADDR,
  L7_DIFFSERV_CLASS_RULE_MATCH_DSTIP_MASK,
  L7_DIFFSERV_CLASS_RULE_MATCH_DSTIPV6_ADDR,
  L7_DIFFSERV_CLASS_RULE_MATCH_DSTIPV6_PLEN,
  L7_DIFFSERV_CLASS_RULE_MATCH_DSTL4PORT_START,
  L7_DIFFSERV_CLASS_RULE_MATCH_DSTL4PORT_END,
  L7_DIFFSERV_CLASS_RULE_MATCH_DSTMAC_ADDR,
  L7_DIFFSERV_CLASS_RULE_MATCH_DSTMAC_MASK,
  L7_DIFFSERV_CLASS_RULE_MATCH_ETYPE_KEYID,
  L7_DIFFSERV_CLASS_RULE_MATCH_ETYPE_VALUE,
  L7_DIFFSERV_CLASS_RULE_MATCH_EVERY,
  L7_DIFFSERV_CLASS_RULE_MATCH_IPDSCP,
  L7_DIFFSERV_CLASS_RULE_MATCH_IPPRECEDENCE,
  L7_DIFFSERV_CLASS_RULE_MATCH_IPTOS_BITS,
  L7_DIFFSERV_CLASS_RULE_MATCH_IPTOS_MASK,
  L7_DIFFSERV_CLASS_RULE_MATCH_IPV6_FLOWLBL,
  L7_DIFFSERV_CLASS_RULE_MATCH_PROTOCOL_NUM,
  L7_DIFFSERV_CLASS_RULE_MATCH_REFCLASS_INDEX,
  L7_DIFFSERV_CLASS_RULE_MATCH_SRCIP_ADDR,
  L7_DIFFSERV_CLASS_RULE_MATCH_SRCIP_MASK,
  L7_DIFFSERV_CLASS_RULE_MATCH_SRCIPV6_ADDR,
  L7_DIFFSERV_CLASS_RULE_MATCH_SRCIPV6_PLEN,
  L7_DIFFSERV_CLASS_RULE_MATCH_SRCL4PORT_START,
  L7_DIFFSERV_CLASS_RULE_MATCH_SRCL4PORT_END,
  L7_DIFFSERV_CLASS_RULE_MATCH_SRCMAC_ADDR,
  L7_DIFFSERV_CLASS_RULE_MATCH_SRCMAC_MASK,
  L7_DIFFSERV_CLASS_RULE_MATCH_VLANID_START,
  L7_DIFFSERV_CLASS_RULE_MATCH_VLANID_END,
  L7_DIFFSERV_CLASS_RULE_MATCH_VLANID2_START,
  L7_DIFFSERV_CLASS_RULE_MATCH_VLANID2_END,
  L7_DIFFSERV_CLASS_RULE_MATCH_EXCLUDE_FLAG,
  L7_DIFFSERV_CLASS_RULE_STORAGE_TYPE,
  L7_DIFFSERV_CLASS_RULE_ROW_STATUS 

} L7_DIFFSERV_CLASS_RULE_TABLE_OBJECT_t;


/* Enumerated values for Policy Table Objects */
typedef enum
{
  L7_DIFFSERV_POLICY_NAME = 1,
  L7_DIFFSERV_POLICY_TYPE,
  L7_DIFFSERV_POLICY_STORAGE_TYPE,
  L7_DIFFSERV_POLICY_ROW_STATUS

} L7_DIFFSERV_POLICY_TABLE_OBJECT_t;

/* Enumerated values for Policy-Class Instance Table Objects */
typedef enum
{
  L7_DIFFSERV_POLICY_INST_CLASS_INDEX = 1,
  L7_DIFFSERV_POLICY_INST_STORAGE_TYPE,
  L7_DIFFSERV_POLICY_INST_ROW_STATUS

} L7_DIFFSERV_POLICY_INST_TABLE_OBJECT_t;

/* Enumerated values for Policy Attribute Table Objects */
typedef enum
{
  L7_DIFFSERV_POLICY_ATTR_STMT_ENTRY_TYPE = 1,
  L7_DIFFSERV_POLICY_ATTR_STMT_ASSIGN_QUEUE_QID,
  L7_DIFFSERV_POLICY_ATTR_STMT_DROP,
  L7_DIFFSERV_POLICY_ATTR_STMT_MARK_COSVAL,
  L7_DIFFSERV_POLICY_ATTR_STMT_MARK_COS2VAL,
  L7_DIFFSERV_POLICY_ATTR_STMT_MARK_COS_AS_COS2,
  L7_DIFFSERV_POLICY_ATTR_STMT_MARK_IPDSCPVAL,
  L7_DIFFSERV_POLICY_ATTR_STMT_MARK_IPPRECEDENCEVAL,
  L7_DIFFSERV_POLICY_ATTR_STMT_MIRROR_INTF,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_CRATE,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_CBURST,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_CONFORM_ACT,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_CONFORM_VAL,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_NONCONFORM_ACT,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_NONCONFORM_VAL,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_COLOR_CONFORM_INDEX,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_COLOR_CONFORM_MODE,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_COLOR_CONFORM_VAL,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_CRATE,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_CBURST,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_EBURST,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_CONFORM_ACT,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_CONFORM_VAL,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_EXCEED_ACT,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_EXCEED_VAL,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_NONCONFORM_ACT,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_NONCONFORM_VAL,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_CONFORM_INDEX,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_CONFORM_MODE,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_CONFORM_VAL,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_EXCEED_INDEX,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_EXCEED_MODE,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_EXCEED_VAL,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_CRATE,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_CBURST,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_PRATE,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_PBURST,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_CONFORM_ACT,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_CONFORM_VAL,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_EXCEED_ACT,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_EXCEED_VAL,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_NONCONFORM_ACT,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_NONCONFORM_VAL,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_CONFORM_INDEX,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_CONFORM_MODE,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_CONFORM_VAL,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_EXCEED_INDEX,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_EXCEED_MODE,
  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_EXCEED_VAL,
  L7_DIFFSERV_POLICY_ATTR_STMT_REDIRECT_INTF,
  L7_DIFFSERV_POLICY_ATTR_STORAGE_TYPE,
  L7_DIFFSERV_POLICY_ATTR_ROW_STATUS

} L7_DIFFSERV_POLICY_ATTR_TABLE_OBJECT_t;

/* Enumerated values for Policy-Class Inbound Performance Table Objects
 * (NOTE:  Private MIB component always provides 64-bit counters)
 */
typedef enum
{
  L7_DIFFSERV_POLICY_PERF_IN_HC_OFFERED_OCTETS = 1,
  L7_DIFFSERV_POLICY_PERF_IN_HC_OFFERED_PACKETS,
  L7_DIFFSERV_POLICY_PERF_IN_HC_DISCARDED_OCTETS,
  L7_DIFFSERV_POLICY_PERF_IN_HC_DISCARDED_PACKETS,
  L7_DIFFSERV_POLICY_PERF_IN_STORAGE_TYPE,
  L7_DIFFSERV_POLICY_PERF_IN_ROW_STATUS
                             
} L7_DIFFSERV_POLICY_PERF_IN_TABLE_OBJECT_t;

/* Enumerated values for Policy-Class Outbound Performance Table Objects
 * (NOTE:  Private MIB component always provides 64-bit counters)
 */
typedef enum
{
  L7_DIFFSERV_POLICY_PERF_OUT_HC_OFFERED_OCTETS = 1,
  L7_DIFFSERV_POLICY_PERF_OUT_HC_OFFERED_PACKETS,
  L7_DIFFSERV_POLICY_PERF_OUT_HC_DISCARDED_OCTETS,
  L7_DIFFSERV_POLICY_PERF_OUT_HC_DISCARDED_PACKETS,
  L7_DIFFSERV_POLICY_PERF_OUT_STORAGE_TYPE,
  L7_DIFFSERV_POLICY_PERF_OUT_ROW_STATUS
                          
} L7_DIFFSERV_POLICY_PERF_OUT_TABLE_OBJECT_t;

/* Enumerated values for Service Table Objects */
typedef enum
{
  L7_DIFFSERV_SERVICE_POLICY_INDEX = 1,
  L7_DIFFSERV_SERVICE_IF_OPER_STATUS,
  L7_DIFFSERV_SERVICE_STORAGE_TYPE,
  L7_DIFFSERV_SERVICE_ROW_STATUS

} L7_DIFFSERV_SERVICE_TABLE_OBJECT_t;

/* Enumerated values for Service Performance Table Objects
 * (NOTE:  Private MIB component always provides 64-bit counters)
 */
typedef enum
{
  L7_DIFFSERV_SERVICE_PERF_HC_OFFERED_OCTETS = 1,
  L7_DIFFSERV_SERVICE_PERF_HC_OFFERED_PACKETS,
  L7_DIFFSERV_SERVICE_PERF_HC_DISCARDED_OCTETS,
  L7_DIFFSERV_SERVICE_PERF_HC_DISCARDED_PACKETS,
  L7_DIFFSERV_SERVICE_PERF_HC_SENT_OCTETS,
  L7_DIFFSERV_SERVICE_PERF_HC_SENT_PACKETS

} L7_DIFFSERV_SERVICE_PERF_TABLE_OBJECT_t;


/*
==========================================
==========================================
==========================================

   ENUMS FOR STANDARD MIB TABLE OBJECTS

==========================================
==========================================
==========================================
*/

/* Enumerated values for Data Path Table Objects */
typedef enum
{
  L7_DIFFSERV_DATA_PATH_START = 1,
  L7_DIFFSERV_DATA_PATH_STORAGE,
  L7_DIFFSERV_DATA_PATH_ROW_STATUS

} L7_DIFFSERV_DATA_PATH_TABLE_OBJECT_t;

/* Enumerated values for Classifier Table Objects */
typedef enum
{
  L7_DIFFSERV_CLFR_STORAGE = 1,
  L7_DIFFSERV_CLFR_ROW_STATUS

} L7_DIFFSERV_CLFR_TABLE_OBJECT_t;

/* Enumerated values for Classifier Element Table Objects */
typedef enum
{
  L7_DIFFSERV_CLFR_ELEMENT_PRECEDENCE = 1,
  L7_DIFFSERV_CLFR_ELEMENT_NEXT,
  L7_DIFFSERV_CLFR_ELEMENT_SPECIFIC,
  L7_DIFFSERV_CLFR_ELEMENT_STORAGE,
  L7_DIFFSERV_CLFR_ELEMENT_ROW_STATUS

} L7_DIFFSERV_CLFR_ELEMENT_TABLE_OBJECT_t;

/* Enumerated values for IP Multi-field Classification Table Objects */
typedef enum
{
  L7_DIFFSERV_MULTI_FIELD_CLFR_ADDR_TYPE = 1,
  L7_DIFFSERV_MULTI_FIELD_CLFR_DST_ADDR,
  L7_DIFFSERV_MULTI_FIELD_CLFR_DST_PREFIX_LENGTH,
  L7_DIFFSERV_MULTI_FIELD_CLFR_SRC_ADDR,
  L7_DIFFSERV_MULTI_FIELD_CLFR_SRC_PREFIX_LENGTH,
  L7_DIFFSERV_MULTI_FIELD_CLFR_DSCP,
  L7_DIFFSERV_MULTI_FIELD_CLFR_FLOW_ID,
  L7_DIFFSERV_MULTI_FIELD_CLFR_PROTOCOL,
  L7_DIFFSERV_MULTI_FIELD_CLFR_DSTL4PORT_MIN,
  L7_DIFFSERV_MULTI_FIELD_CLFR_DSTL4PORT_MAX,
  L7_DIFFSERV_MULTI_FIELD_CLFR_SRCL4PORT_MIN,
  L7_DIFFSERV_MULTI_FIELD_CLFR_SRCL4PORT_MAX,
  L7_DIFFSERV_MULTI_FIELD_CLFR_STORAGE,
  L7_DIFFSERV_MULTI_FIELD_CLFR_ROW_STATUS

} L7_DIFFSERV_MULTI_FIELD_CLFR_TABLE_OBJECT_t;

/* Enumerated values for Auxiliary Multi-field Classification Table Objects */
typedef enum
{
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_DST_ADDR = 1,
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_DST_MASK,
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_SRC_ADDR,
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_SRC_MASK,
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_PROTOCOL,
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_DSTL4PORT_MIN,
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_DSTL4PORT_MAX,
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_SRCL4PORT_MIN,
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_SRCL4PORT_MAX,
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_COS,
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_COS2,
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_ETYPE_VAL1,        /* 0 means not used     */
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_ETYPE_VAL2,        /* 0 means not used     */
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_TOS,
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_TOS_MASK,
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_DSTMAC,
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_DSTMAC_MASK,
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_SRCMAC,
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_SRCMAC_MASK,
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_VLANID_MIN,
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_VLANID_MAX,
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_VLANID2_MIN,
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_VLANID2_MAX,
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_STORAGE,
  L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_ROW_STATUS

} L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_TABLE_OBJECT_t;


/* Enumerated values for Meter Table Objects */
typedef enum
{
  L7_DIFFSERV_METER_SUCCEED_NEXT = 1,
  L7_DIFFSERV_METER_FAIL_NEXT,
  L7_DIFFSERV_METER_SPECIFIC,
  L7_DIFFSERV_METER_STORAGE,
  L7_DIFFSERV_METER_ROW_STATUS

} L7_DIFFSERV_METER_TABLE_OBJECT_t;


/* Enumerated values for Token Bucket Parameter Table Objects */
typedef enum
{
  L7_DIFFSERV_TB_PARAM_TYPE = 1,
  L7_DIFFSERV_TB_PARAM_RATE,
  L7_DIFFSERV_TB_PARAM_BURST_SIZE,
  L7_DIFFSERV_TB_PARAM_INTERVAL,
  L7_DIFFSERV_TB_PARAM_STORAGE,
  L7_DIFFSERV_TB_PARAM_ROW_STATUS

} L7_DIFFSERV_TB_PARAM_TABLE_OBJECT_t;

/* Enumerated values for Action Table Objects */
typedef enum
{
  L7_DIFFSERV_ACTION_INTERFACE = 1,
  L7_DIFFSERV_ACTION_NEXT,
  L7_DIFFSERV_ACTION_SPECIFIC,
  L7_DIFFSERV_ACTION_STORAGE,
  L7_DIFFSERV_ACTION_ROW_STATUS

} L7_DIFFSERV_ACTION_TABLE_OBJECT_t;

/* Enumerated values for COS Mark Action Table Objects */
typedef enum
{
  L7_DIFFSERV_COS_MARK_ACT_COS = 1
} L7_DIFFSERV_COS_MARK_ACT_TABLE_OBJECT_t;

/* Enumerated values for Secondary COS (COS2) Mark Action Table Objects */
typedef enum
{
  L7_DIFFSERV_COS2_MARK_ACT_COS = 1
} L7_DIFFSERV_COS2_MARK_ACT_TABLE_OBJECT_t;

/* Enumerated values for DSCP Mark Action Table Objects */
typedef enum
{
  L7_DIFFSERV_DSCP_MARK_ACT_DSCP = 1
} L7_DIFFSERV_DSCP_MARK_ACT_TABLE_OBJECT_t;

/* Enumerated values for IP Precedence Mark Action Table Objects */
typedef enum
{
  L7_DIFFSERV_IPPREC_MARK_ACT_IPPRECEDENCE = 1
} L7_DIFFSERV_IPPREC_MARK_ACT_TABLE_OBJECT_t;


/* Enumerated values for Count Action Table Objects */
typedef enum
{
  L7_DIFFSERV_COUNT_ACT_OCTETS = 1,
  L7_DIFFSERV_COUNT_ACT_PKTS,
  L7_DIFFSERV_COUNT_ACT_STORAGE,
  L7_DIFFSERV_COUNT_ACT_ROW_STATUS

} L7_DIFFSERV_COUNT_ACT_TABLE_OBJECT_t;

/* Enumerated values for Assign Queue Table Objects */
typedef enum
{
  L7_DIFFSERV_ASSIGN_QUEUE_QNUM = 1,
  L7_DIFFSERV_ASSIGN_QUEUE_STORAGE,
  L7_DIFFSERV_ASSIGN_QUEUE_ROW_STATUS

} L7_DIFFSERV_ASSIGN_QUEUE_TABLE_OBJECT_t;

/* Enumerated values for Color Aware Table Objects */
typedef enum
{
  L7_DIFFSERV_COLOR_AWARE_LEVEL = 1,
  L7_DIFFSERV_COLOR_AWARE_MODE,
  L7_DIFFSERV_COLOR_AWARE_VALUE

} L7_DIFFSERV_COLOR_AWARE_TABLE_OBJECT_t;

/* Enumerated values for Redirect Table Objects */
typedef enum
{
  L7_DIFFSERV_REDIRECT_INTF = 1,
  L7_DIFFSERV_REDIRECT_STORAGE,
  L7_DIFFSERV_REDIRECT_ROW_STATUS

} L7_DIFFSERV_REDIRECT_TABLE_OBJECT_t;

/* Enumerated values for Mirror Table Objects */
typedef enum
{
  L7_DIFFSERV_MIRROR_INTF = 1,
  L7_DIFFSERV_MIRROR_STORAGE,
  L7_DIFFSERV_MIRROR_ROW_STATUS

} L7_DIFFSERV_MIRROR_TABLE_OBJECT_t;

/* Enumerated values for Algorithmic Drop Table Objects */
typedef enum
{
  L7_DIFFSERV_ALG_DROP_TYPE = 1,
  L7_DIFFSERV_ALG_DROP_NEXT,
  L7_DIFFSERV_ALG_DROP_QMEASURE,
  L7_DIFFSERV_ALG_DROP_QTHRESHOLD,
  L7_DIFFSERV_ALG_DROP_SPECIFIC,
  L7_DIFFSERV_ALG_DROP_OCTETS,
  L7_DIFFSERV_ALG_DROP_PKTS,
  L7_DIFFSERV_ALG_RANDOMDROP_OCTETS,
  L7_DIFFSERV_ALG_RANDOMDROP_PKTS,
  L7_DIFFSERV_ALG_DROP_STORAGE,
  L7_DIFFSERV_ALG_DROP_ROW_STATUS

} L7_DIFFSERV_ALG_DROP_TABLE_OBJECT_t;


/*
===================================
===================================
===================================

   DIFFSERV TLV TYPE IDENTIFIERS

===================================
===================================
===================================
*/

/* QOS DiffServ Functional Category TLVs */
#define L7_QOS_DIFFSERV_TLV_RESERVED                              0x0000
#define L7_QOS_DIFFSERV_TLV_CLSF_INST_TYPE                        0x0030
#define L7_QOS_DIFFSERV_TLV_INST_DEL_LIST_TYPE                    0x0031
#define L7_QOS_DIFFSERV_TLV_POLICY_ADD_TYPE                       0x0032

/* Class Rule Specification TLVs */
#define L7_QOS_DIFFSERV_TLV_CLASS_DEF_TYPE                        0x0400
#define L7_QOS_DIFFSERV_TLV_CLASS_RULE_DEF_TYPE                   0x0410
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_COS_TYPE                  0x0411
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_COS2_TYPE                 0x0412
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTIP_TYPE                0x0413
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTIPV6_TYPE              0x0414
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTL4PORT_TYPE            0x0415
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTMAC_TYPE               0x0416
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_ETYPE_TYPE                0x0417
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_EVERY_TYPE                0x0418
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPV6_FLOWLBL_TYPE         0x0419
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPTOS_TYPE                0x041A
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_PROTOCOL_TYPE             0x041B
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCIP_TYPE                0x041C
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCIPV6_TYPE              0x041D
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCL4PORT_TYPE            0x041E
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCMAC_TYPE               0x041F
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_VLANID_TYPE               0x0420
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_VLANID2_TYPE              0x0421

/* Policy Attribute Specification TLVs */
#define L7_QOS_DIFFSERV_TLV_POLICY_DEF_TYPE                       0x0500
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_ASSIGN_QUEUE_TYPE         0x0511
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_BW_ALLOC_TYPE             0x0512
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_DROP_TYPE                 0x0513
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_COSVAL_TYPE          0x0514
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_COS2VAL_TYPE         0x0515
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_IPDSCPVAL_TYPE       0x0516
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_TYPE 0x0517
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MIRROR_TYPE               0x0518
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_DEF_TYPE           0x0519
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_CONFORM_TYPE       0x051A
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_EXCEED_TYPE        0x051B
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_NONCONFORM_TYPE    0x051C
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_RANDOMDROP_TYPE           0x051D   /* deprecated */
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_REDIRECT_TYPE             0x051E
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_SHAPING_TYPE              0x051F   /* deprecated */
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_COSVAL_AS_COS2_TYPE  0x0521

/* Class Rule Definition TLV -- 'deny' flag values */
#define L7_QOS_DIFFSERV_TLV_CLASS_RULE_DEF_PERMIT 0
#define L7_QOS_DIFFSERV_TLV_CLASS_RULE_DEF_DENY   1 /* use any non-zero value */

/* Bandwidth Allocation policy attribute TLV -- min/max BW unspecified */
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_BANDWIDTH_MIN_NONE  0 /* no mimimum   */
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_BANDWIDTH_MAX_UNLIM 0 /* unlimited    */

/* Bandwidth Allocation policy attribute TLV -- scheduler type values */
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_SCHEDULER_SP        1 /* strict pri   */
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_SCHEDULER_WRR_WFQ   2 /* weighted     */
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_SCHEDULER_BE_HI     3 /*best-eff(high)*/
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_SCHEDULER_BE_LO     4 /*best-eff(low) */

/* Policing Policy definition TLV -- policing style values */
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_STYLE_SIMPLE     1
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_STYLE_SINGLERATE 2
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_STYLE_TWORATE    3

/* Policing Policy definition TLV -- color mode values */
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORMODE_BLIND  1 /* color-blind */
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORMODE_AWARE  2 /* color-aware */

/* Policing Policy definition TLV -- color aware match field type */
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_COS      1
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_COS2     2
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_IPDSCP   3
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_IPPREC   4
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_UNUSED   5

/* Police Conform/Exceed/Nonconform policy attribute TLV -- action flag values*/
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_DROP         1 /* discard all  */
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_MARKCOS      2 /* mark w/COS   */
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_MARKCOS2     3 /* mark w/COS2  */
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_MARKDSCP     4 /* mark w/DSCP  */
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_MARKPREC     5 /* mark w/Preced*/
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_SEND         6 /* no marking   */
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_MARKCOS_AS_COS2      7 /* mark w/COS using ineer Tag */


/*------------------------------------------------*/
/* DiffServ TLV native data definition and length */
/*------------------------------------------------*/

/* NOTE:  ALL TLV STRUCTURE DATA FIELDS ARE REPRESENTED IN NETWORK BYTE ORDER!
 *                                                         ==================
 */

/* QOS DiffServ Classifier Instance TLV */
typedef struct
{
  L7_uint32     instanceKey;
} diffServTlvClsfInst_t;
#define L7_QOS_DIFFSERV_TLV_CLSF_INST_LEN \
                ((L7_uint32)sizeof(diffServTlvClsfInst_t))

/* QOS DiffServ Instance Deletion List TLV */
typedef struct
{
  L7_uint32     keyCount;
  L7_uint32     instanceKey[L7_DIFFSERV_INST_PER_POLICY_LIM]; /* (no +1 here) */
} diffServTlvInstDelList_t;
#define L7_QOS_DIFFSERV_TLV_INST_DEL_LIST_LEN \
                ((L7_uint32)sizeof(diffServTlvInstDelList_t))
/* special definition for minimum native length (useful for building, parsing)*/
#define L7_QOS_DIFFSERV_TLV_INST_DEL_LIST_MIN_LEN \
                ((L7_uint32)sizeof(L7_uint32))
                                       
/* QOS DiffServ Policy Add TLV */
typedef struct
{
  L7_uint32     instanceCount;
} diffServTlvPolicyAdd_t;
#define L7_QOS_DIFFSERV_TLV_POLICY_ADD_LEN \
                ((L7_uint32)sizeof(diffServTlvPolicyAdd_t))
                                       

/*-------------------------------*/
/* Class Rule Specification TLVs */
/*-------------------------------*/

/* Class Definition TLV */
#define L7_QOS_DIFFSERV_TLV_CLASS_DEF_LEN \
                ((L7_uint32)0)

/* Class Rule Definition TLV */
typedef struct
{
  L7_uchar8     denyFlag;                       /* bypass DiffServ (exclude)  */
  L7_uchar8     rsvd1[3];                       /* reserved -- for alignment  */
} diffServTlvClassRuleDef_t;
#define L7_QOS_DIFFSERV_TLV_CLASS_RULE_DEF_LEN \
                ((L7_uint32)sizeof(diffServTlvClassRuleDef_t))

/* Class of Service (CoS) match criterion TLV */
typedef struct
{
  L7_uchar8     cosValue;                       /* COS value (0-7)            */
  L7_uchar8     rsvd1[3];                       /* reserved -- for alignment  */
} diffServTlvMatchCos_t;
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_COS_LEN \
                ((L7_uint32)sizeof(diffServTlvMatchCos_t))

/* Secondary Class of Service (CoS2) match criterion TLV */
typedef struct
{
  L7_uchar8     cosValue;                       /* COS value (0-7)            */
  L7_uchar8     rsvd1[3];                       /* reserved -- for alignment  */
} diffServTlvMatchCos2_t;
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_COS2_LEN \
                ((L7_uint32)sizeof(diffServTlvMatchCos2_t))

/* Destination IP Address match criterion TLV */
typedef struct
{
  L7_uint32     ipAddr;
  L7_uint32     ipMask;
} diffServTlvMatchDstIp_t;
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTIP_LEN \
                ((L7_uint32)sizeof(diffServTlvMatchDstIp_t))

/* Destination IPv6 match criterion TLV */
typedef struct
{
  L7_uchar8     ipv6Addr[L7_IP6_ADDR_LEN];
  L7_uchar8     ipv6Mask[L7_IP6_ADDR_LEN];
} diffServTlvMatchDstIpv6_t;
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTIPV6_LEN \
                ((L7_uint32)sizeof(diffServTlvMatchDstIpv6_t))

/* Destination Layer-4 Port Range match criterion TLV */
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTL4PORT_MASK  0xFFFF
typedef struct
{
  L7_ushort16   portStart;
  L7_ushort16   portEnd;
  L7_ushort16   portMask;
  L7_ushort16   rsvd1;                          /* reserved -- for alignment  */
} diffServTlvMatchDstL4Port_t;
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTL4PORT_LEN \
                ((L7_uint32)sizeof(diffServTlvMatchDstL4Port_t))

/* Destination MAC Address match criterion TLV */
typedef struct
{
  L7_uchar8     macAddr[L7_MAC_ADDR_LEN];
  L7_uchar8     macMask[L7_MAC_ADDR_LEN];
} diffServTlvMatchDstMac_t;
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTMAC_LEN \
                ((L7_uint32)sizeof(diffServTlvMatchDstMac_t))

/* Ethertype match criterion TLV */
typedef struct
{
  L7_ushort16   etypeValue1;
  L7_ushort16   etypeValue2;
  L7_uchar8     checkStdHdrFlag;                /* qualify using protocol hdr */
  L7_uchar8     rsvd1[3];                       /* reserved -- for alignment  */
} diffServTlvMatchEtype_t;
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_ETYPE_LEN \
                ((L7_uint32)sizeof(diffServTlvMatchEtype_t))

/* "Match Every" match criterion TLV */
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_EVERY_LEN \
                ((L7_uint32)0)

/* IPv6 Flow Label match criterion TLV */
#define L7_QOS_DIFFSERV_TLV_MATCH_IPV6FLOWLBL_MASK   0xFFFFF
#define L7_QOS_DIFFSERV_TLV_MATCH_IPV6FLOWLBL_SHIFT  0
typedef struct
{
  L7_uint32     ipv6FlowLabel;                      /* defined as 20-bit value    */
  L7_uint32     ipv6FlowLabelMask;                  /* in case masking allowed    */
} diffServTlvMatchIpv6FlowLabel_t;
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPV6_FLOWLBL_LEN \
                ((L7_uint32)sizeof(diffServTlvMatchIpv6FlowLabel_t))

/* IP Type of Service (ToS) match criterion TLV */
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPDSCP_SHIFT        2    
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPDSCP_MASK         0xFC
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPPRECEDENCE_SHIFT  5
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPPRECEDENCE_MASK   0xE0
typedef struct
{
  L7_uchar8     tosValue;
  L7_uchar8     tosMask;
  L7_uchar8     rsvd1[2];                       /* reserved -- for alignment  */
} diffServTlvMatchIpTos_t;
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPTOS_LEN \
                ((L7_uint32)sizeof(diffServTlvMatchIpTos_t))

/* Protocol Number match criterion TLV */
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_PROTOCOL_MASK     0xFF
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_PROTOCOL_IP_MASK  0x00  /* 'match all' protocols */
typedef struct
{
  L7_uchar8     protoNumValue;
  L7_uchar8     protoNumMask;
  L7_uchar8     rsvd1[2];                       /* reserved -- for alignment  */
} diffServTlvMatchProtocol_t;
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_PROTOCOL_LEN \
                ((L7_uint32)sizeof(diffServTlvMatchProtocol_t))

/* Source IP Address match criterion TLV */
typedef struct
{
  L7_uint32     ipAddr;
  L7_uint32     ipMask;
} diffServTlvMatchSrcIp_t;
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCIP_LEN \
                ((L7_uint32)sizeof(diffServTlvMatchSrcIp_t))

/* Source IPv6 match criterion TLV */
typedef struct
{
  L7_uchar8     ipv6Addr[L7_IP6_ADDR_LEN];
  L7_uchar8     ipv6Mask[L7_IP6_ADDR_LEN];
} diffServTlvMatchSrcIpv6_t;
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCIPV6_LEN \
                ((L7_uint32)sizeof(diffServTlvMatchSrcIpv6_t))

/* Source Layer-4 Port Range match criterion TLV */
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCL4PORT_MASK  0xFFFF
typedef struct
{
  L7_ushort16   portStart;
  L7_ushort16   portEnd;
  L7_ushort16   portMask;
  L7_ushort16   rsvd1;                          /* reserved -- for alignment  */
} diffServTlvMatchSrcL4Port_t;
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCL4PORT_LEN \
                ((L7_uint32)sizeof(diffServTlvMatchSrcL4Port_t))

/* Source MAC Address match criterion TLV */
typedef struct
{
  L7_uchar8     macAddr[L7_MAC_ADDR_LEN];
  L7_uchar8     macMask[L7_MAC_ADDR_LEN];
} diffServTlvMatchSrcMac_t;
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCMAC_LEN \
                ((L7_uint32)sizeof(diffServTlvMatchSrcMac_t))

/* VLAN ID match criterion TLV */
typedef struct
{
  L7_ushort16   vidStart;                       /* VLAN ID start              */
  L7_ushort16   vidEnd;                         /* VLAN ID end                */
} diffServTlvMatchVlanId_t;
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_VLANID_LEN \
                ((L7_uint32)sizeof(diffServTlvMatchVlanId_t))

/* Secondary VLAN ID (VLAN ID2) match criterion TLV */
typedef struct
{
  L7_ushort16   vidStart;                       /* Secondary ID start         */
  L7_ushort16   vidEnd;                         /* Secondary ID end           */
} diffServTlvMatchVlanId2_t;
#define L7_QOS_DIFFSERV_TLV_CLASS_MATCH_VLANID2_LEN \
                ((L7_uint32)sizeof(diffServTlvMatchVlanId2_t))


/*-------------------------------------*/
/* Policy Attribute Specification TLVs */
/*-------------------------------------*/

/* Policy Definition TLV */
#define L7_QOS_DIFFSERV_TLV_POLICY_DEF_LEN \
                ((L7_uint32)0)

/* Assign Queue policy attribute TLV */
typedef struct
{
  L7_uchar8     qid;                            /* queue id                   */
  L7_uchar8     rsvd1[3];                       /* reserved -- for alignment  */
} diffServTlvAttrAssignQueue_t;
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_ASSIGN_QUEUE_LEN \
                ((L7_uint32)sizeof(diffServTlvAttrAssignQueue_t))

/* Bandwidth Allocation policy attribute TLV */
/* NOTE: this definition covers both 'bandwidth' and 'expedite' attributes */
typedef struct
{
  L7_uint32     minBw;                          /*minimum guaranteed bandwidth*/
  L7_uint32     maxBw;                          /* maximum allowed bandwidth  */
  L7_uint32     maxBurst;                       /* maximum burst size         */
  L7_uchar8     schedType;                      /* scheduler type             */
  L7_uchar8     rsvd1[3];                       /* reserved -- for alignment  */
} diffServTlvAttrBwAlloc_t;        
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_BW_ALLOC_LEN \
                ((L7_uint32)sizeof(diffServTlvAttrBwAlloc_t))

/* Drop policy attribute TLV */
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_DROP_LEN \
                ((L7_uint32)0)

/* Mark Class of Service (CoS) policy attribute TLV */
typedef struct
{
  L7_uchar8     cosMark;                        /* COS value (0-7)            */
  L7_uchar8     rsvd1[3];                       /* reserved -- for alignment  */
} diffServTlvAttrMarkCosVal_t;
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_COSVAL_LEN \
                ((L7_uint32)sizeof(diffServTlvAttrMarkCosVal_t))

/* Mark Secondary Class of Service (CoS2) policy attribute TLV */
typedef struct
{
  L7_uchar8     cosMark;                        /* COS value (0-7)            */
  L7_uchar8     rsvd1[3];                       /* reserved -- for alignment  */
} diffServTlvAttrMarkCos2Val_t;
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_COS2VAL_LEN \
                ((L7_uint32)sizeof(diffServTlvAttrMarkCos2Val_t))

/* Mark Class of Service (CoS) as Secondary Cos (Cos2) policy attribute TLV */
typedef struct
{
  L7_BOOL       cosMarkAsCos2;            /* whether to use secondary cos (cos2) value or not */
} diffServTlvAttrMarkCosValAsCos2_t;
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_COSVAL_AS_COS2_LEN \
                ((L7_uint32)sizeof(diffServTlvAttrMarkCosValAsCos2_t))

/* Mark IP DSCP policy attribute TLV */
typedef struct
{
  L7_uchar8     dscpMark;                       /* IP DSCP value (0-63)       */
  L7_uchar8     rsvd1[3];                       /* reserved -- for alignment  */
} diffServTlvAttrMarkIpDscpVal_t;
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_IPDSCPVAL_LEN \
                ((L7_uint32)sizeof(diffServTlvAttrMarkIpDscpVal_t))

/* Mark IP Precedence policy attribute TLV */
typedef struct
{
  L7_uchar8     ipPrecMark;                     /* IP Precedence value (0-7)  */
  L7_uchar8     rsvd1[3];                       /* reserved -- for alignment  */
} diffServTlvAttrMarkIpPrecedenceVal_t;
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_LEN \
                ((L7_uint32)sizeof(diffServTlvAttrMarkIpPrecedenceVal_t))

/* Mirror policy attribute TLV */
typedef struct
{
  L7_uint32     intfUnit;                       /* interface unit number      */
  L7_uint32     intfSlot;                       /* interface slot number      */
  L7_uint32     intfPort;                       /*interface port num (0-based)*/
} diffServTlvAttrMirror_t;
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MIRROR_LEN \
                ( (L7_uint32)sizeof( diffServTlvAttrMirror_t ) )

/* Policing Policy Definition TLV */
typedef struct
{
  L7_uchar8     style;                          /* style of policing          */
  L7_uchar8     colorMode;                      /* color blind or aware       */
  L7_uchar8     colorConformType;               /* aware conform field type   */
  L7_uchar8     colorConformValue;              /* aware conform field value  */
  L7_uchar8     colorExceedType;                /* aware exceed field type    */
  L7_uchar8     colorExceedValue;               /* aware exceed field value   */
  L7_uchar8     rsvd1[2];                       /* reserved -- for alignment  */
} diffServTlvAttrPoliceDef_t;      
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_DEF_LEN \
                ((L7_uint32)sizeof(diffServTlvAttrPoliceDef_t))

/* Police Conform policy attribute TLV */
typedef struct
{
  L7_uint32     dataRate;                       /* CIR                        */
  L7_uint32     burstSize;                      /* CBS                        */
  L7_uchar8     actionFlag;                     /* police action              */
  L7_uchar8     markValue;                      /* marking action value       */
  L7_uchar8     rsvd1[2];                       /* reserved -- for alignment  */
} diffServTlvAttrPoliceConform_t;      
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_CONFORM_LEN \
                ((L7_uint32)sizeof(diffServTlvAttrPoliceConform_t))

/* Police Exceed policy attribute TLV */
/* (use same police action definitions as Police Conform) */
typedef struct
{
  L7_uint32     dataRate;                       /* CIR or PIR                 */
  L7_uint32     burstSize;                      /* EBS or PBS                 */
  L7_uchar8     actionFlag;                     /* police action              */
  L7_uchar8     markValue;                      /* marking action value       */
  L7_uchar8     rsvd1[2];                       /* reserved -- for alignment  */
} diffServTlvAttrPoliceExceed_t;      
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_EXCEED_LEN \
                ((L7_uint32)sizeof(diffServTlvAttrPoliceExceed_t))

/* Police Non-Conform policy attribute TLV */
/* (use same police action definitions as Police Conform) */
typedef struct
{
  L7_uchar8     actionFlag;                     /* police action              */
  L7_uchar8     markValue;                      /* marking action value       */
  L7_uchar8     rsvd1[2];                       /* reserved -- for alignment  */
} diffServTlvAttrPoliceNonconform_t;      
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_NONCONFORM_LEN \
                ((L7_uint32)sizeof(diffServTlvAttrPoliceNonconform_t))

/* Random Drop policy attribute TLV */
typedef struct
{
  L7_uint32     minThresh;                      /* beginning threhsold (bytes)*/
  L7_uint32     maxThresh;                      /* ending threshold (bytes)   */
  L7_uint32     maxDropProb;                    /* maximum drop probability   */
  L7_uint32     samplingRate;                   /* queue sampling rate        */
  L7_uint32     decayExponent;                  /* decay factor exponent      */
} diffServTlvAttrRandomDrop_t;            
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_RANDOMDROP_LEN \
                ((L7_uint32)sizeof(diffServTlvAttrRandomDrop_t))

/* Redirect policy attribute TLV */
typedef struct
{
  L7_uint32     intfUnit;                       /* interface unit number      */
  L7_uint32     intfSlot;                       /* interface slot number      */
  L7_uint32     intfPort;                       /*interface port num (0-based)*/
} diffServTlvAttrRedirect_t;               
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_REDIRECT_LEN \
                ((L7_uint32)sizeof(diffServTlvAttrRedirect_t))

/* Shaping policy attribute TLV */
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_SHAPING_RATE_MAX_NONE  0  /* unlimited*/
typedef struct
{
  L7_uint32     commitRate;                     /* committed send rate        */
  L7_uint32     maxRate;                        /* maximum send rate          */
  L7_uint32     maxBurst;                       /* maximum burst size         */
} diffServTlvAttrShaping_t;               
#define L7_QOS_DIFFSERV_TLV_POLICY_ATTR_SHAPING_LEN \
                ((L7_uint32)sizeof(diffServTlvAttrShaping_t))


/*
========================
========================
========================

   DIFFSERV UTILITIES

========================
========================
========================
*/

/* DiffServ notify event types */
typedef enum
{
  L7_DIFFSERV_NOTIFY_EVENT_CREATE = 1,
  L7_DIFFSERV_NOTIFY_EVENT_MODIFY,
  L7_DIFFSERV_NOTIFY_EVENT_DELETE,
  L7_DIFFSERV_NOTIFY_EVENT_RENAME               /* DiffServ policy name was changed */

} L7_DIFFSERV_NOTIFY_EVENT_t;

/* DiffServ change approval types */
typedef enum
{
  L7_DIFFSERV_CHANGE_APPROVAL_CLASS_CREATE = 1,
  L7_DIFFSERV_CHANGE_APPROVAL_CLASS_DELETE,
  L7_DIFFSERV_CHANGE_APPROVAL_POLICY_CREATE,
  L7_DIFFSERV_CHANGE_APPROVAL_POLICY_DELETE,
  L7_DIFFSERV_CHANGE_APPROVAL_POLICY_INST_CREATE,
  L7_DIFFSERV_CHANGE_APPROVAL_POLICY_INST_DELETE

} L7_DIFFSERV_CHANGE_APPROVAL_t;

/*********************************************************************
* @purpose  Callback function signature for DiffServ policy notify events from QOS package.
*
* @param    policyEvent     @b{(input)} DiffServ policy event
* @param    policyType      @b{(input)} Type of policy
* @param    policyIndex     @b{(input)} Policy index value
* @param    policyName      @b{(input)} Name of this DiffServ policy
* @param    policyOldName   @b{(input)} Old name of policy (name change only), or L7_NULLPTR
*
* @returns  void
*
* @comments This function is invoked by the QOS DiffServ component based on component
*           callback registration.
*
* @end
*********************************************************************/
typedef void (*L7_DIFFSERV_NOTIFY_EVENT_FUNCPTR_t) (L7_DIFFSERV_NOTIFY_EVENT_t policyEvent,
                                                    L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t policyType,
                                                    L7_uint32 policyIndex, 
                                                    L7_uchar8 *policyName,
                                                    L7_uchar8 *policyOldName);

/*********************************************************************
* @purpose  Callback function signature to approve a request to delete a DiffServ policy.
*
* @param    policyChange    @b{(input)} DiffServ policy change ID
* @param    policyType      @b{(input)} Type of policy
* @param    policyIndex     @b{(input)} Policy index value
* @param    policyName      @b{(input)} Name of this DiffServ policy
* @param    pData           @b{(input)} Ptr to request-specific data, or L7_NULLPTR
*
* @returns  L7_SUCCESS  Allow DiffServ policy change
* @returns  L7_FAILURE  Do not allow DiffServ policy change
*
* @comments This function is invoked by the QOS DiffServ component based on component
*           callback registration.
*
* @comments IMPORTANT: The callback function MUST NOT BLOCK during its processing   
*           and must RESPOND SYNCHRONOUSLY to this request.
*
* @end
*********************************************************************/
typedef L7_RC_t (*L7_DIFFSERV_CHANGE_APPROVAL_FUNCPTR_t) (L7_DIFFSERV_CHANGE_APPROVAL_t policyChange,
                                                          L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t policyType,
                                                          L7_uint32 policyIndex, 
                                                          L7_uchar8 *policyName,
                                                          void *pData);

/*********************************************************************
* @purpose  Determine if the DiffServ component has been initialized
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Indicates diffServSysInit() has allocated and initialized
*           its component data structures, which is the minimum acceptable
*           condition for any DiffServ operation.
*       
* @end
*********************************************************************/
L7_BOOL diffServIsInitialized(void);

/*********************************************************************
* @purpose  Determine if the DiffServ component is ready for operation
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Indicates all system conditions have been met for DiffServ
*           component operation.  This is the next sequential state of
*           readiness beyond component initialization, and covers any
*           dependencies on other parts of the system.
*
* @notes    To allow component preconfiguration, DO NOT check this 
*           condition when doing configuration object 'get' and 'set'
*           functions.  This checking is needed, however, for the 
*           various 'apply' functions.
*       
* @end
*********************************************************************/
L7_BOOL diffServIsReady(void);

/*************************************************************************
* @purpose  Register a notify event callback function to be called 
*           whenever there is a configuration change to a DiffServ policy. 
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
L7_RC_t diffServNotifyEventCallbackRegister(L7_DIFFSERV_NOTIFY_EVENT_FUNCPTR_t funcPtr,
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
L7_RC_t diffServNotifyEventCallbackUnregister(L7_DIFFSERV_NOTIFY_EVENT_FUNCPTR_t funcPtr);

/*************************************************************************
* @purpose  Indicate whether a notify event callback function is already 
*           registered for DiffServ policy changes.
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
L7_RC_t diffServNotifyEventCallbackIsRegistered(L7_DIFFSERV_NOTIFY_EVENT_FUNCPTR_t funcPtr);

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
L7_RC_t diffServNotifyEventCallbackTableShow(void);

/*************************************************************************
* @purpose  Register a change approval callback function to be called 
*           prior to deleting a DiffServ policy.
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
L7_RC_t diffServChangeApprovalCallbackRegister(L7_DIFFSERV_CHANGE_APPROVAL_FUNCPTR_t funcPtr,
                                               L7_COMPONENT_IDS_t compId,
                                               L7_uchar8 *displayStr);

/*************************************************************************
* @purpose  Remove a change approval callback function from the registered list.
*
* @param    funcPtr  @b{(input)} pointer to callback function to unregister
*
* @returns  L7_SUCCESS          de-registration succeeded
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE          function pointer NULL or not registered
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t diffServChangeApprovalCallbackUnregister(L7_DIFFSERV_CHANGE_APPROVAL_FUNCPTR_t funcPtr);

/*************************************************************************
* @purpose  Indicate whether a change approval callback function is already 
*           registered for DiffServ policy changes.
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
L7_RC_t diffServChangeApprovalCallbackIsRegistered(L7_DIFFSERV_CHANGE_APPROVAL_FUNCPTR_t funcPtr);

/*************************************************************************
* @purpose  Display the contents of the change approval callback table.
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
L7_RC_t diffServChangeApprovalCallbackTableShow(void);

/*************************************************************************
* @purpose  Display the contents of all supported DiffServ callback tables.
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
L7_RC_t diffServCallbackTableShow(void);

/*********************************************************************
* @purpose  Get DiffServ component tracing mode
*
* @param    void
*
* @returns  L7_ENABLE
* @returns  L7_DISABLE
*
* @notes    
*       
* @end
*********************************************************************/
L7_uint32 diffServTraceModeGet(void);

/*********************************************************************
* @purpose  Set DiffServ component tracing mode
*
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  none
*
* @notes    
*       
* @end
*********************************************************************/
void diffServTraceModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Get DiffServ debug message level
*
* @param    msgLvl      @b{(input)} Message level (0 = off, >0 = on)
*
* @returns  none
*
* @notes    Messages are defined according to interest level.  Higher
*           values generally display more debug messages.
*       
* @end
*********************************************************************/
L7_uint32 diffServMsgLvlGet(void);

/*********************************************************************
* @purpose  Set DiffServ debug message level
*
* @param    msgLvl      @b{(input)} Message level (0 = off, >0 = on)
*
* @returns  none
*
* @notes    Messages are defined according to interest level.  Higher
*           values generally display more debug messages.
*       
* @end
*********************************************************************/
void diffServMsgLvlSet(L7_uint32 msgLvl);


/*
==========================
==========================
==========================

   GENERAL STATUS GROUP

==========================
==========================
==========================
*/

/*********************************************************************
* @purpose  Get the value of the DiffServ administrative mode  
*
* @param    pMode       @{(output)} Pointer to mode value
*
* @returns  void
*
* @notes    The DiffServ admin mode is maintained in the General Status Group.
*
* @notes    Only outputs a value if the pMode parm is non-null.
*
* @end
*********************************************************************/
void diffServAdminModeGet(L7_uint32* pMode);

/*************************************************************************
* @purpose  Set the value of the DiffServ administrative mode
*
* @param    mode        @b{(input)} Mode value      
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The DiffServ admin mode is maintained in the General Status Group.
*
* @end
*********************************************************************/
L7_RC_t diffServAdminModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Get the specified object from the General Status Group 
*
* @param    oid         @b{(input)}  Object ID
* @param    pValue      @b{(output)} Pointer to the object output value
*  
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServGenObjectGet(L7_DIFFSERV_GEN_STATUS_GROUP_OBJECT_t oid,
                             L7_uint32 *pValue);


/*
=================
=================
=================

   CLASS TABLE

=================
=================
=================
*/

/*************************************************************************
* @purpose  Obtain the next available index value to be used when creating  
*           a new row in the Class Table
*
* @param    pClassIndex @b{(output)} Pointer to class index value
*
* @returns  void
*
* @notes    Only outputs a value if the pClassIndex parm is non-null.
*
* @notes    A *pClassIndex output value of 0 means the table is full.
*
* @notes    The index value remains free until used in a subsequent create
*           request.  When two or more requestors get the same index value,
*           only the first one to successfully create a new row in this table
*           gets to keep it.  All other requestors' create requests will
*           fail due to the index being in use, so they must call this
*           function again to get a new value.
*
* @notes    Although an index value is considered in use once a
*           successful row create occurs, the next index value is not
*           revealed until the row is either completed or deleted.
*
* @end
*********************************************************************/
void diffServClassIndexNext(L7_uint32 *pClassIndex);

/*************************************************************************
* @purpose  Retrieve the maximum class index value allowed for the 
*           Class Table
*
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
L7_RC_t diffServClassIndexMaxGet(L7_uint32 *pIndexMax);

/*********************************************************************
* @purpose  Create a new row in the Class Table for the specified index
*
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
* @notes    The row status is initially represented as 'not ready' and remains 
*           that way until every object in the row contains a valid value. 
*           If the activateRow parameter is set to L7_TRUE, the row status 
*           will then be automatically updated to 'active', otherwise it will
*           change to 'not in service'.
*
* @end
*********************************************************************/
L7_RC_t diffServClassCreate(L7_uint32 classIndex, 
                            L7_BOOL activateRow);

/*************************************************************************
* @purpose  Delete a row from the Class Table for the specified index
*
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
L7_RC_t diffServClassDelete(L7_uint32 classIndex);

/*********************************************************************
* @purpose  Verify that a Class Table row exists for the specified index
*
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
L7_RC_t diffServClassGet(L7_uint32 classIndex);

/*********************************************************************
* @purpose  Determine next sequential row in the Class Table
*
* @param    prevClassIndex  @b{(input)}  Class index to begin search
* @param    pClassIndex     @b{(output)} Pointer to next sequential class
*                                          index value
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
L7_RC_t diffServClassGetNext(L7_uint32 prevClassIndex, 
                             L7_uint32 *pClassIndex);

/*********************************************************************
* @purpose  Populates an array with the class indexes for all entries in 
*           Class Table sorted such that any class referring to another
*           class appears after that class.
*
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
L7_RC_t diffServClassOrderedListGet(L7_uint32 *orderedClassList, 
                                    L7_uint32 *count);

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Class Table 
*           to determine its legitimacy
*
* @param    classIndex  @b{(input)} Class index
* @param    oid         @b{(input)} Object identifier
* @param    pValue      @b{(input)} Pointer to proposed value to be set
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Proposed value invalid
* @returns  L7_FAILURE  
*
* @notes    Only tests objects with max access types READ-CREATE or READ-WRITE,
*           thus all other access types return L7_FAILURE.
*
* @notes    Caller must use one of the _SETTEST_OID_TYPE object identifiers
*           only (i.e., do not use the _TABLE_OBJECT type object names here).
*
* @notes    A *pointer* to the test value is used here to allow for objects
*           of different types.  This means all character array/string objects
*           are passed in as L7_uchar8**.
*
* @end
*********************************************************************/
L7_RC_t diffServClassSetTest(L7_uint32 classIndex, 
                             L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_TYPE_t oid, 
                             void *pValue);

/*********************************************************************
* @purpose  Get the specified object from the Class Table
*
* @param    classIndex  @b{(input)} Class index
* @param    oid         @b{(input)} Object identifier 
* @param    pValue      @b{(output)} Pointer to object output value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value not yet established
* @returns  L7_FAILURE  Improper usage or other malfunction
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServClassObjectGet(L7_uint32 classIndex,
                               L7_DIFFSERV_CLASS_TABLE_OBJECT_t oid,
                               void *pValue);

/*********************************************************************
* @purpose  Set the specified object in the Class Table
*
* @param    classIndex  @b{(input)} Class index
* @param    oid         @b{(input)} Object identifier 
* @param    pValue      @b{(output)} Pointer to object set value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A *pointer* to the set value is used here to allow for objects
*           of different types.  This means all character array/string objects
*           are passed in as L7_uchar8**.
*
* @end
*********************************************************************/
L7_RC_t diffServClassObjectSet(L7_uint32 classIndex,
                               L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_TYPE_t oid, 
                               void *pValue);

/*********************************************************************
* @purpose  Translate a class name into its associated Class Table index
*
* @param    namePtr     @b{(input)} Class name string pointer
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
L7_RC_t diffServClassNameToIndex(L7_uchar8 *namePtr, 
                                 L7_uint32 *pClassIndex);

/*********************************************************************
* @purpose  Provide the reference class index, if any, used by the specified
*           class
*
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
L7_RC_t diffServClassToRefClass(L7_uint32 classIndex,
                                L7_uint32 *pRefClassIndex);

/*********************************************************************
* @purpose  Check if class definition is suitable for inclusion by specified
*           policy
*
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
L7_RC_t diffServClassValidForPolicyCheck(L7_uint32 classIndex,
                                         L7_uint32 policyIndex);

/*********************************************************************
* @purpose  Get the next policy index that references this class
*
* @param    classIndex      @b{(input)}   Class index
* @param    prevPolicyIndex @b{(input)}   Policy index to begin search
* @param    pPolicyIndex    @b{(output)}  Ptr to next sequential policy index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR        Reached end of table
* @returns  L7_FAILURE
*
* @notes    A prevPolicyIndex value of 0 is used to find the first policy
*           that references this class.
*
* @notes    A policy is considered to reference a class if any of the 
*           policy class instances specify the class either 
*           directly, or through the class's reference chain.
*
* @end
*********************************************************************/
L7_RC_t diffServClassRefByPolicyGetNext(L7_uint32 classIndex,
                                        L7_uint32 prevPolicyIndex,
                                        L7_uint32 *pPolicyIndex);

/*
==========================
==========================
==========================

   CLASS RULE TABLE API

==========================
==========================
==========================
*/

/*********************************************************************
* @purpose  Obtain the next available index value to be used when creating  
*           a new row in the Class Rule Table
*
* @param    classIndex      @b{(input)}  Class index
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
* @end
*********************************************************************/
L7_RC_t diffServClassRuleIndexNext(L7_uint32 classIndex,
                                   L7_uint32 *pClassRuleIndex);

/*************************************************************************
* @purpose  Retrieve the maximum class rule index value allowed for the 
*           Class Rule Table
*
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
L7_RC_t diffServClassRuleIndexMaxGet(L7_uint32 *pIndexMax);

/*************************************************************************
* @purpose  Create a new row in the Class Rule Table for the specified indexes
*
* @param    classIndex      @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    activateRow     @b{(input)} Activate row when ready flag
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
* @end
*********************************************************************/
L7_RC_t diffServClassRuleCreate(L7_uint32 classIndex, 
                                L7_uint32 classRuleIndex,
                                L7_BOOL activateRow);

/*********************************************************************
* @purpose  Delete a row from the Class Rule Table for the specified index
*
* @param    classIndex      @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE
*
* @notes    In general, class rules cannot be deleted from the user interface.
*           Instead, deletion normally occurs due to the deletion of a class
*           row, which in turn deletes each class rule within it.
*
* @end
*********************************************************************/
L7_RC_t diffServClassRuleDelete(L7_uint32 classIndex, 
                                L7_uint32 classRuleIndex);

/*************************************************************************
* @purpose  Delete all rows from the Class Rule Table for the specified class
*
* @param    classIndex      @b{(input)} Class index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    In general, class rules cannot be deleted from the user interface.
*           Instead, deletion normally occurs due to the deletion of a class
*           row, which in turn deletes each class rule within it.
*
* @end
*********************************************************************/
L7_RC_t diffServClassRuleDeleteAll(L7_uint32 classIndex);

/*********************************************************************
* @purpose  Verify that a Class Rule Table row exists for the specified indexes
*
* @param    classIndex      @b{(input)} Class index
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
L7_RC_t diffServClassRuleGet(L7_uint32 classIndex, 
                             L7_uint32 classRuleIndex);

/*********************************************************************
* @purpose  Determine next sequential row in the Class Rule Table
*
* @param    prevClassIndex      @b{(input)}  Class index to begin search
* @param    prevClassRuleIndex  @b{(input)}  Class rule index to begin search
* @param    pClassIndex         @b{(output)} Pointer to next sequential class
*                                              index value
* @param    pClassRuleIndex     @b{(output)} Pointer to next sequential class
*                                              rule index value
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
L7_RC_t diffServClassRuleGetNext(L7_uint32 prevClassIndex, 
                                 L7_uint32 prevClassRuleIndex,
                                 L7_uint32 *pClassIndex,
                                 L7_uint32 *pClassRuleIndex);

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Class Rule  
*           Table to determine its legitimacy
*
* @param    classIndex  @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    oid         @b{(input)} Object identifier
* @param    pValue      @b{(input)} Pointer to proposed value to be set
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR          Proposed value invalid
* @returns  L7_NOT_SUPPORTED  Feature is not supported on this platform
* @returns  L7_FAILURE  
*
* @notes    Only tests objects with access types READ-CREATE or READ-WRITE;
*           all other access types return L7_FAILURE.
*
* @notes    Caller must use one of the _SETTEST_OID_TYPE object identifiers
*           only (i.e., do not use the _TABLE_OBJECT type object names here).
*
* @notes    A *pointer* to the test value is used here to allow for objects
*           of different types.  This means all character array/string objects
*           are passed in as L7_uchar8**.
*
* @notes    The required set ordering for a row in this table is:  1) entry
*           type, 2) exclude flag, 3) individual match object(s).
*
* @end
*********************************************************************/
L7_RC_t diffServClassRuleSetTest(L7_uint32 classIndex,
                                 L7_uint32 classRuleIndex,
                                 L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_TYPE_t oid,
                                 void *pValue);

/*********************************************************************
* @purpose  Get the specified object from the Class Rule Table
*
* @param    classIndex      @b{(input)}  Class index
* @param    classRuleIndex  @b{(input)}  Class rule index
* @param    oid             @b{(input)}  Object identifier
* @param    pValue          @b{(output)} Pointer to the object output value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value not yet established
* @returns  L7_FAILURE  Improper usage or other malfunction
*
* @notes    An object can only be provided if the request matches the
*           class rule entry type and if the object's value has been
*           instantiated.
*
* @end
*********************************************************************/
L7_RC_t diffServClassRuleObjectGet(L7_uint32 classIndex, 
                                   L7_uint32 classRuleIndex,
                                   L7_DIFFSERV_CLASS_RULE_TABLE_OBJECT_t oid,
                                   void *pValue);

/*********************************************************************
* @purpose  Set the specified object in the Class Rule Table
*
* @param    classIndex      @b{(input)}  Class index
* @param    classRuleIndex  @b{(input)}  Class rule index
* @param    oid             @b{(input)}  Object identifier
* @param    pValue          @b{(output)} Pointer to the object set value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current entry type
* @returns  L7_FAILURE  
*
* @notes    A *pointer* to the set value is used here to allow for objects
*           of different types.  This means all character array/string objects
*           are passed in as L7_uchar8**.
*
* @notes    The required set ordering for a row in this table is:  1) entry
*           type, 2) exclude flag, 3) individual match object(s).
*
* @end
*********************************************************************/
L7_RC_t diffServClassRuleObjectSet(L7_uint32 classIndex, 
                                   L7_uint32 classRuleIndex,
                                   L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_TYPE_t oid,
                                   void *pValue);

/*********************************************************************
* @purpose  Get the ACL rule identifier, if any, for the specified entry
*           in the Class Rule Table
*
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
L7_RC_t diffServClassRuleAclRuleIdGet(L7_uint32 classIndex, 
                                      L7_uint32 classRuleIndex,
                                      L7_uint32 *pAclRuleId);

/*********************************************************************
* @purpose  Get the Class Rule index of the Reference Class Rule for
*           the specified Class
*
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
L7_RC_t diffServClassRuleRefClassRuleIndexFind(L7_uint32 classIndex,
                                               L7_uint32 *pClassRuleIndex);

/*********************************************************************
* @purpose  Remove the referenced class index value from the specified
*           class
*
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
* @notes    The actual class rule row will not be deleted, but the
*           reference class rule index value will be set to 0 (zero)
*           so that the user knows the rule is not available for re-use.
*           An exception to this is if the reference class rule is the
*           most recently created in the class, it is deleted so that
*           it may be reused.
*
* @end
*********************************************************************/
L7_RC_t diffServClassRuleRefClassIndexRemove(L7_uint32 classIndex,
                                             L7_uint32 classRuleIndex);

/*********************************************************************
* @purpose  Determine if the class rule is the most recently created  
*           rule in the class (i.e., is last one in current list)  
*
* @param    classIndex      @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
*
* @returns  L7_TRUE     Class rule is most recent
* @returns  L7_FALSE    Class rule not most recent
*
* @notes
*       
* @end
*********************************************************************/
L7_BOOL diffServClassRuleIsMostRecent(L7_uint32 classIndex, 
                                      L7_uint32 classRuleIndex);


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
* @param    pPolicyIndex @b{(output)} Pointer to policy index value
*
* @returns  void
*
* @notes    Only outputs a value if the pPolicyIndex parm is non-null.
*
* @notes    A *pPolicyIndex output value of 0 means the table is full.
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
void diffServPolicyIndexNext(L7_uint32 *pPolicyIndex);

/*************************************************************************
* @purpose  Retrieve the maximum policy index value allowed for the 
*           Policy Table
*
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
L7_RC_t diffServPolicyIndexMaxGet(L7_uint32 *pIndexMax);

/*************************************************************************
* @purpose  Create a new row in the Policy Table for the specified index
*
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
*********************************************************************/
L7_RC_t diffServPolicyCreate(L7_uint32 policyIndex, 
                             L7_BOOL activateRow);

/*************************************************************************
* @purpose  Delete a row from the Policy Table for the specified index
*
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
L7_RC_t diffServPolicyDelete(L7_uint32 policyIndex);

/*********************************************************************
* @purpose  Verify that a Policy Table row exists for the specified index
*
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
L7_RC_t diffServPolicyGet(L7_uint32 policyIndex);

/*********************************************************************
* @purpose  Determine next sequential row in the Policy Table
*
* @param    prevPolicyIndex  @b{(input)}  Policy index to begin search
* @param    pPolicyIndex     @b{(output)} Pointer to next sequential policy
*                                           index value
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
L7_RC_t diffServPolicyGetNext(L7_uint32 prevPolicyIndex, 
                              L7_uint32 *pPolicyIndex);

/*********************************************************************
* @purpose  Reports whether a Policy is being used in any Service
*
* @param    policyIndex      @b{(input)}  Policy index to check
*
* @returns  L7_TRUE     policy is being referenced in at least one service
* @returns  L7_FALSE    policy is not being referenced any services
*
* @end
*********************************************************************/
L7_BOOL diffServIsPolicyReferenced(L7_uint32 policyIndex);

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Policy Table 
*           to determine its legitimacy
*
* @param    policyIndex @b{(input)} Policy index
* @param    oid         @b{(input)} Object identifier
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
* @notes    A *pointer* to the test value is used here to allow for objects
*           of different types.  This means all character array/string objects
*           are passed in as L7_uchar8**.
* @end
*********************************************************************/
L7_RC_t diffServPolicySetTest(L7_uint32 policyIndex, 
                              L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_TYPE_t oid, 
                              void *pValue);

/*********************************************************************
* @purpose  Get the specified object from the Policy Table
*
* @param    policyIndex @b{(input)}  Policy index
* @param    oid         @b{(input)}  Object identifier
* @param    pValue      @b{(output)} Pointer to the object output value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value not yet established
* @returns  L7_FAILURE  Improper usage or other malfunction
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyObjectGet(L7_uint32 policyIndex, 
                                L7_DIFFSERV_POLICY_TABLE_OBJECT_t oid,
                                void *pValue);

/*********************************************************************
* @purpose  Set the specified object in the Policy Table
*
* @param    policyIndex @b{(input)}  Policy index
* @param    oid         @b{(input)}  Object identifier
* @param    pValue      @b{(output)} Pointer to the object set value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A *pointer* to the set value is used here to allow for objects
*           of different types.  This means all character array/string objects
*           are passed in as L7_uchar8**.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyObjectSet(L7_uint32 policyIndex, 
                                L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_TYPE_t oid, 
                                void *pValue);

/*********************************************************************
* @purpose  Translate a policy name into its associated Policy Table index
*
* @param    stringPtr     @b{(input)}  Policy name string pointer
* @param    pPolicyIndex  @b{(output)} Pointer to policy index value
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
L7_RC_t diffServPolicyNameToIndex(L7_uchar8 *stringPtr, 
                                  L7_uint32 *pPolicyIndex);

/*********************************************************************
* @purpose  Get current number of instances defined for the specified policy.
*
* @param    policyIndex @b{(input)}  Policy index
* @param    *pCount     @b{(output)} Pointer to instance count output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyInstCountGet(L7_uint32 policyIndex,
                                   L7_uint32 *pCount);

/*********************************************************************
* @purpose  Create a single TLV containing all service policy
*           instances within it and return the data in TLV block 
*           storage provided by the caller.
*
* @param    policyIndex @{(input)} Policy index
* @param    intIfNum    @{(input)} Internal interface number   
* @param    ifDirection @{(input)} Interface direction         
* @param    tlvHandle   @{(input)} handle of the TLV block_to be
*                                  populated with policy information         
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Problem with TLV
* @returns  L7_FAILURE
*
* @notes    The information distilled into the TLV represents a snapshot of the data in
*           the policy database at time of invocation.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyTlvGet(L7_uint32 policyIndex, L7_uint32 intIfNum,
                             L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection, L7_tlvHandle_t tlvHandle);


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
* @param    policyIndex      @b{(input)}  Policy index
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
* @end
*********************************************************************/
L7_RC_t diffServPolicyInstIndexNext(L7_uint32 policyIndex,
                                    L7_uint32 *pPolicyInstIndex);

/*************************************************************************
* @purpose  Retrieve the maximum policy instance index value allowed for the 
*           Policy Instance Table
*
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
L7_RC_t diffServPolicyInstIndexMaxGet(L7_uint32 *pIndexMax);

/*********************************************************************
* @purpose  Create a new row in the Policy-Class Instance Table for the 
*           specified indexes
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    activateRow     @b{(input)} Activate row when ready flag
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
L7_RC_t diffServPolicyInstCreate(L7_uint32 policyIndex, 
                                 L7_uint32 policyInstIndex,
                                 L7_BOOL activateRow);

/*************************************************************************
* @purpose  Delete a row from the Policy-Class Instance Table for the 
*           specified indexes
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
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
L7_RC_t diffServPolicyInstDelete(L7_uint32 policyIndex, 
                                 L7_uint32 policyInstIndex);

/*************************************************************************
* @purpose  Delete all rows from the Policy-Class Instance Table for the 
*           specified policy
*
* @param    policyIndex     @b{(input)} Policy index
*
* @returns  L7_SUCCESS
* @returns  L7_REQUEST_DENIED      Config change not approved
* @returns  L7_FAILURE
*
* @notes    An L7_REQUEST_DENIED response means another application   
*           has denied approval of this configuration change.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyInstDeleteAll(L7_uint32 policyIndex);

/*********************************************************************
* @purpose  Verify that a Policy-Class Instance Table row exists for the 
*           specified indexes
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyInstGet(L7_uint32 policyIndex, 
                              L7_uint32 policyInstIndex);

/*********************************************************************
* @purpose  Determine next sequential row in the Policy-Class Instance Table
*
* @param    prevPolicyIndex     @b{(input)}  Policy index to begin search
* @param    prevPolicyInstIndex @b{(input)}  Policy instance index to begin 
*                                              search
* @param    pPolicyIndex        @b{(output)} Pointer to next sequential policy
*                                              index value
* @param    pPolicyInstIndex    @b{(output)} Pointer to next sequential policy
*                                              instance index value
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
L7_RC_t diffServPolicyInstGetNext(L7_uint32 prevPolicyIndex, 
                                  L7_uint32 prevPolicyInstIndex, 
                                  L7_uint32 *pPolicyIndex,
                                  L7_uint32 *pPolicyInstIndex);

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Policy-Class
*           Instance Table to determine its legitimacy
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    oid             @b{(input)} Object identifier
* @param    pValue          @b{(input)} Pointer to proposed value to be set
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
* @notes    A *pointer* to the test value is used here to allow for objects
*           of different types.  This means all character array/string objects
*           are passed in as L7_uchar8**.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyInstSetTest(L7_uint32 policyIndex, 
                                  L7_uint32 policyInstIndex, 
                                  L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_TYPE_t oid,
                                  void *pValue);

/*********************************************************************
* @purpose  Get the specified object from the Policy-Class Instance Table
*
* @param    policyIndex      @b{(input)}  Policy index
* @param    policyInstIndex  @b{(input)}  Policy instance index
* @param    oid              @b{(input)}  Object identifier
* @param    pValue           @b{(output)} Pointer to the object output value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value not yet established
* @returns  L7_FAILURE  Improper usage or other malfunction
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyInstObjectGet(L7_uint32 policyIndex,
                                    L7_uint32 policyInstIndex,
                                    L7_DIFFSERV_POLICY_INST_TABLE_OBJECT_t oid,
                                    void *pValue);

/*********************************************************************
* @purpose  Set the specified object in the Policy-Class Instance Table
*
* @param    policyIndex      @b{(input)}  Policy index
* @param    policyInstIndex  @b{(input)}  Policy instance index
* @param    oid              @b{(input)}  Object identifier
* @param    pValue           @b{(output)} Pointer to the object set value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current entry type
* @returns  L7_FAILURE  
*
* @notes    A *pointer* to the set value is used here to allow for objects
*           of different types.  This means all character array/string objects
*           are passed in as L7_uchar8**.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyInstObjectSet(L7_uint32 policyIndex,
                                    L7_uint32 policyInstIndex,
                                    L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_TYPE_t oid,
                                    void *pValue);

/*********************************************************************
* @purpose  Find the policy instance index from the policy and class indexes
*
* @param    policyIndex       @b{(input)}  Policy index
* @param    classIndex        @b{(input)}  Class index
* @param    pPolicyInstIndex  @b{(output)} Pointer to policy instance index
*                                            value
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
L7_RC_t diffServPolicyInstIndexFromClass(L7_uint32 policyIndex, 
                                         L7_uint32 classIndex,
                                         L7_uint32 *pPolicyInstIndex);

/*********************************************************************
* @purpose  Determine if the policy instance is the most recently created  
*           instance in the policy (i.e., is last one in current list)  
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
*
* @returns  L7_TRUE     Policy instance is most recent
* @returns  L7_FALSE    Policy instance not most recent
*
* @notes
*       
* @end
*********************************************************************/
L7_BOOL diffServPolicyInstIsMostRecent(L7_uint32 policyIndex, 
                                       L7_uint32 policyInstIndex);

                                      
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
* @param    policyIndex      @b{(input)}  Policy index
* @param    policyInstIndex  @b{(input)}  Policy instance index
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
* @end
*********************************************************************/
L7_RC_t diffServPolicyAttrIndexNext(L7_uint32 policyIndex,
                                    L7_uint32 policyInstIndex,
                                    L7_uint32 *pPolicyAttrIndex);

/*************************************************************************
* @purpose  Retrieve the maximum policy attribute index value allowed for the 
*           Policy Attribute Table
*
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
L7_RC_t diffServPolicyAttrIndexMaxGet(L7_uint32 *pIndexMax);

/*************************************************************************
* @purpose  Create a new row in the Policy Attribute Table for the 
*           specified indexes
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    policyAttrIndex @b{(input)} Policy attribute index
* @param    activateRow     @b{(input)} Activate row when ready flag
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
L7_RC_t diffServPolicyAttrCreate(L7_uint32 policyIndex,
                                 L7_uint32 policyInstIndex,
                                 L7_uint32 policyAttrIndex,
                                 L7_BOOL activateRow);

/*************************************************************************
* @purpose  Delete a row from the Policy Attribute Table for the 
*           specified indexes
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    policyAttrIndex @b{(input)} Policy attribute index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyAttrDelete(L7_uint32 policyIndex,
                                 L7_uint32 policyInstIndex, 
                                 L7_uint32 policyAttrIndex);

/*************************************************************************
* @purpose  Delete all rows from the Policy Attribute Table for the 
*           specified policy instance
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyAttrDeleteAll(L7_uint32 policyIndex, 
                                    L7_uint32 policyInstIndex);

/*********************************************************************
* @purpose  Verify that a Policy Attribute Table row exists for the 
*           specified index
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    policyAttrIndex @b{(input)} Policy attribute index
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyAttrGet(L7_uint32 policyIndex,
                              L7_uint32 policyInstIndex,
                              L7_uint32 policyAttrIndex);

/*********************************************************************
* @purpose  Determine next sequential row in the Policy Attribute Table
*
* @param    prevPolicyIndex     @b{(input)}  Policy index to begin search
* @param    prevPolicyInstIndex @b{(input)}  Policy instance index to begin 
*                                              search
* @param    prevPolicyAttrIndex @b{(input)}  Policy attribute index to begin 
*                                              search
* @param    pPolicyIndex        @b{(output)} Pointer to next sequential policy 
*                                              index value
* @param    pPolicyInstIndex    @b{(output)} Pointer to next sequential policy
*                                              instance index value
* @param    pPolicyAttrIndex    @b{(output)} Pointer to next sequential policy
*                                              attribute index value
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
L7_RC_t diffServPolicyAttrGetNext(L7_uint32 prevPolicyIndex, 
                                  L7_uint32 prevPolicyInstIndex, 
                                  L7_uint32 prevPolicyAttrIndex, 
                                  L7_uint32 *pPolicyIndex,
                                  L7_uint32 *pPolicyInstIndex,
                                  L7_uint32 *pPolicyAttrIndex);

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Policy
*           Attribute Table to determine its legitimacy
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    policyAttrIndex @b{(input)} Policy attribute index
* @param    oid             @b{(input)} Object identifier
* @param    pValue          @b{(input)} Pointer to proposed value to be set
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
* @notes    A *pointer* to the test value is used here to allow for objects
*           of different types.  This means all character array/string objects
*           are passed in as L7_uchar8**.
*
* @notes    THE _BANDWIDTH AND _EXPEDITE OBJECTS MUST BE SET IN THE 
*           FOLLOWING ORDER:  1) _CRATE_UNITS, 2) _CRATE.
*
* @notes    THE _POLICE_XXX ACTION OBJECTS MUST BE SET IN THE FOLLOWING
*           ORDER:  1) _YYY_ACT, 2) YYY_VAL.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyAttrSetTest(L7_uint32 policyIndex, 
                                  L7_uint32 policyInstIndex, 
                                  L7_uint32 policyAttrIndex, 
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_TYPE_t oid, 
                                  void *pValue);

/*********************************************************************
* @purpose  Get the specified object from the Policy Attribute Table
*
* @param    policyIndex     @b{(input)}  Policy index
* @param    policyInstIndex @b{(input)}  Policy instance index
* @param    policyAttrIndex @b{(input)}  Policy attribute index
* @param    oid             @b{(input)}  Object identifier
* @param    pValue          @b{(output)} Pointer to the object output value  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value not yet established
* @returns  L7_FAILURE  Improper usage or other malfunction
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyAttrObjectGet(L7_uint32 policyIndex, 
                                    L7_uint32 policyInstIndex, 
                                    L7_uint32 policyAttrIndex, 
                                    L7_DIFFSERV_POLICY_ATTR_TABLE_OBJECT_t oid,
                                    void *pValue);

/*********************************************************************
* @purpose  Set the specified object in the Policy Attribute Table
*
* @param    policyIndex      @b{(input)}  Policy index
* @param    policyInstIndex  @b{(input)}  Policy instance index
* @param    policyAttrIndex  @b{(input)}  Policy attribute index
* @param    oid              @b{(input)}  Object identifier
* @param    pValue           @b{(output)} Pointer to the object set value  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current entry type
* @returns  L7_FAILURE  
*
* @notes    A *pointer* to the set value is used here to allow for objects
*           of different types.  This means all character array/string objects
*           are passed in as L7_uchar8**.
*
* @notes    THE _BANDWIDTH AND _EXPEDITE OBJECTS MUST BE SET IN THE 
*           FOLLOWING ORDER:  1) _CRATE_UNITS, 2) _CRATE.
*
* @notes    THE _POLICE_XXX ACTION OBJECTS MUST BE SET IN THE FOLLOWING
*           ORDER:  1) _YYY_ACT, 2) YYY_VAL.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyAttrObjectSet(L7_uint32 policyIndex, 
                                    L7_uint32 policyInstIndex, 
                                    L7_uint32 policyAttrIndex, 
                                    L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_TYPE_t oid, 
                                    void *pValue);

/*********************************************************************
* @purpose  Get the policing color aware mode and value from the specified
*           class index
*
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
L7_RC_t diffServPolicyAttrColorAwareClassInfoGet(L7_uint32 classIndex, 
                                                 L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_t *pColorMode,
                                                 L7_uint32 *pColorValue);

/*********************************************************************
* @purpose  Determine if a DiffServ class is valid for use as a policing
*           color aware designation
*
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
L7_RC_t diffServPolicyAttrColorAwareClassValidate(L7_uint32 policyIndex, 
                                                  L7_uint32 policyInstIndex, 
                                                  L7_uint32 classIndex);

/*********************************************************************
* @purpose  Determine if color aware policing definitions are equivalent 
*           for two DiffServ classes 
*
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
L7_BOOL diffServPolicyAttrColorAwareClassesAreEquivalent(L7_uint32 classIndex1,
                                                         L7_uint32 classIndex2);

/*********************************************************************
* @purpose  Check if the mirror/redirect target interface can be used
*
* @param    policyIndex     @b{(input)}  Policy index
* @param    policyInstIndex @b{(input)}  Policy instance index
* @param    policyAttrIndex @b{(input)}  Policy attribute index
* @param    oid             @b{(input)}  Object identifier
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    By "usable" here, we mean that the interface is a supported
*           DiffServ interface that is in a state of either L7_INTF_ATTACHING
*           or L7_INTF_ATTACHED.
*
* @notes    The only object IDs supported are for the mirror and redirect
*           interface attributes.
*
* @end
*********************************************************************/
L7_BOOL diffServPolicyAttrTargetIntfIsUsable(L7_uint32 policyIndex, 
                                             L7_uint32 policyInstIndex, 
                                             L7_uint32 policyAttrIndex, 
                                             L7_DIFFSERV_POLICY_ATTR_TABLE_OBJECT_t oid);

/*********************************************************************
* @purpose  Determine if the policy attribute is the most recently created  
*           one in this policy instance (i.e., is last one in current list)  
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    policyAttrIndex @b{(input)} Policy attribute index
*
* @returns  L7_TRUE     Policy instance is most recent
* @returns  L7_FALSE    Policy instance not most recent
*
* @notes
*       
* @end
*********************************************************************/
L7_BOOL diffServPolicyAttrIsMostRecent(L7_uint32 policyIndex, 
                                       L7_uint32 policyInstIndex,
                                       L7_uint32 policyAttrIndex);

/*
===============================================
===============================================
===============================================

   POLICY-CLASS INBOUND PERFORMANCE TABLE API

===============================================
===============================================
===============================================
*/

/*********************************************************************
* @purpose  Verify that a Policy-Class In Perf Table row exists for the
*           specified indexes
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    intIfNum        @b{(input)} Internal interface number
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyPerfInGet(L7_uint32 policyIndex,
                                L7_uint32 policyInstIndex,
                                L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine next sequential row in the Policy-Class In Perf Table
*
* @param    prevPolicyIndex     @b{(input)}  Policy index to begin search
* @param    prevPolicyInstIndex @b{(input)}  Policy instance index to begin 
*                                               search
* @param    prevIntIfNum        @b{(input)}  Internal interface index to begin
*                                              search
* @param    pPolicyIndex        @b{(output)} Pointer to next sequential policy
*                                              index value
* @param    pPolicyInstIndex    @b{(output)} Pointer to next sequential policy
*                                              instance index value
* @param    pIntIfNum           @b{(output)} Pointer to next sequential internal     
*                                              interface number
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
L7_RC_t diffServPolicyPerfInGetNext(L7_uint32 prevPolicyIndex, 
                                    L7_uint32 prevPolicyInstIndex, 
                                    L7_uint32 prevIntIfNum,
                                    L7_uint32 *pPolicyIndex,
                                    L7_uint32 *pPolicyInstIndex,
                                    L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose  Get the specified object from the Policy-Class In Perf Table
*
* @param    policyIndex     @b{(input)}  Policy index
* @param    policyInstIndex @b{(input)}  Policy instance index
* @param    intIfNum        @b{(input)}  Internal interface number
* @param    oid             @b{(input)}  Object identifier
* @param    pValue          @b{(output)} Pointer to object output value 
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    When accessing any counter object, the caller is required to set 
*           the pValue parameter to point to an output variable of type
*           L7_ulong64.  For a 32-bit counter, the pValue->high portion is 
*           always output as 0.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyPerfInObjectGet(L7_uint32 policyIndex, 
                                      L7_uint32 policyInstIndex, 
                                      L7_uint32 intIfNum, 
                                      L7_DIFFSERV_POLICY_PERF_IN_TABLE_OBJECT_t oid,
                                      void *pValue);


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
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    intIfNum        @b{(input)} Internal interface number
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyPerfOutGet(L7_uint32 policyIndex,
                                 L7_uint32 policyInstIndex,
                                 L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine next sequential row in the Policy-Class Out Perf Table
*
* @param    prevPolicyIndex     @b{(input)}  Policy index to begin search
* @param    prevPolicyInstIndex @b{(input)}  Policy instance index to begin 
*                                               search
* @param    prevIntIfNum        @b{(input)}  Internal interface index to begin
*                                              search
* @param    pPolicyIndex        @b{(output)} Pointer to next sequential policy
*                                              index value
* @param    pPolicyInstIndex    @b{(output)} Pointer to next sequential policy
*                                              instance index value
* @param    pIntIfNum           @b{(output)} Pointer to next sequential internal     
*                                              interface number
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
L7_RC_t diffServPolicyPerfOutGetNext(L7_uint32 prevPolicyIndex, 
                                     L7_uint32 prevPolicyInstIndex, 
                                     L7_uint32 prevIntIfNum,
                                     L7_uint32 *pPolicyIndex,
                                     L7_uint32 *pPolicyInstIndex,
                                     L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose  Get the specified object from the Policy-Class Out Perf Table
*
* @param    policyIndex     @b{(input)}  Policy index
* @param    policyInstIndex @b{(input)}  Policy instance index
* @param    intIfNum        @b{(input)}  Internal interface number
* @param    oid             @b{(input)}  Object identifier
* @param    pValue          @b{(output)} Pointer to the object output value  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    When accessing any counter object, the caller is required to set 
*           the pValue parameter to point to an output variable of type
*           L7_ulong64.  For a 32-bit counter, the pValue->high portion is 
*           always output as 0.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyPerfOutObjectGet(L7_uint32 policyIndex, 
                                       L7_uint32 policyInstIndex, 
                                       L7_uint32 intIfNum, 
                                       L7_DIFFSERV_POLICY_PERF_OUT_TABLE_OBJECT_t oid,
                                       void *pValue);


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
* @param    pMaxNumIntf @b{(output)} Pointer to max number of service intf
*
* @returns  void
*
* @notes    Only outputs a value if the pMaxNumIntf parm is non-null.
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
void diffServServiceIntfMaxGet(L7_uint32 *pMaxNumIntf);

/*************************************************************************
* @purpose  Create a new row in the Service Table for the specified 
*           interface and direction
*
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
L7_RC_t diffServServiceCreate(L7_uint32 intIfNum,
                              L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                              L7_BOOL activateRow);

/*************************************************************************
* @purpose  Delete a row from the Service Table for the specified
*           interface and direction
*
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t diffServServiceDelete(L7_uint32 intIfNum,
                              L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection);

/*********************************************************************
* @purpose  Verify that a Service Table row exists for the specified 
*           interface and direction
*
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
L7_RC_t diffServServiceGet(L7_uint32 intIfNum,
                           L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection);

/*********************************************************************
* @purpose  Determine next sequential row in the Service Table
*
* @param    prevIntIfNum    @b{(input)}  Internal interface number to begin
*                                          search
* @param    prevIfDirection @b{(input)}  Interface direction to begin search
* @param    pIntIfNum       @b{(output)} Pointer to next sequential internal 
*                                          interface number value
* @param    pIfDirection    @b{(output)} Pointer to next sequential
*                                          interface direction value
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
L7_RC_t diffServServiceGetNext(L7_uint32 prevIntIfNum, 
                               L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t prevIfDirection,
                               L7_uint32 *pIntIfNum,
                               L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t *pIfDirection);

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Service Table 
*           to determine its legitimacy
*
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    oid             @b{(input)} Object identifier
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
* @notes    A *pointer* to the test value is used here to allow for objects
*           of different types.  This means all character array/string objects
*           are passed in as L7_uchar8**.
*
* @end
*********************************************************************/
L7_RC_t diffServServiceSetTest(L7_uint32 intIfNum, 
                               L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                               L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_TYPE_t oid, 
                               void *pValue);

/*********************************************************************
* @purpose  Get the specified object in the Service Table
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    ifDirection @b{(input)}  Interface direction
* @param    oid         @b{(input)}  Object identifier
* @param    pValue      @b{(output)} Pointer to object output value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    
* @notes    When accessing any counter object, the caller is required to set 
*           the pValue parameter to point to an output variable of type
*           L7_ulong64.  For a 32-bit counter, the pValue->high portion is 
*           always output as 0.
*
*
* @end
*********************************************************************/
L7_RC_t diffServServiceObjectGet(L7_uint32 intIfNum,
                                 L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                                 L7_DIFFSERV_SERVICE_TABLE_OBJECT_t oid,
                                 void  *pValue);

/*********************************************************************
* @purpose  Set the specified object in the Service Table
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    ifDirection @b{(input)}  Interface direction
* @param    oid         @b{(input)}  Object identifier
* @param    pValue      @b{(output)} Pointer to object set value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    A *pointer* to the set value is used here to allow for objects
*           of different types.  This means all character array/string objects
*           are passed in as L7_uchar8**.
*
* @end
*********************************************************************/
L7_RC_t diffServServiceObjectSet(L7_uint32 intIfNum,
                                 L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                                 L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_TYPE_t oid, 
                                 void  *pValue);


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
* @purpose  Get the specified object from the Service Performance Table
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    ifDirection @b{(input)}  Interface direction
* @param    oid         @b{(input)}  Object identifier
* @param    pValue      @b{(output)} Pointer to object output value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    When accessing any counter object, the caller is required to set 
*           the pValue parameter to point to an output variable of type
*           L7_ulong64.  For a 32-bit counter, the pValue->high portion is 
*           always output as 0.
*
* @end
*********************************************************************/
L7_RC_t diffServServicePerfObjectGet(L7_uint32 intIfNum,
                                     L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                                     L7_DIFFSERV_SERVICE_PERF_TABLE_OBJECT_t oid,
                                     void *pValue);


/*
=========================
=========================
=========================

   DATA PATH TABLE API

=========================
=========================
=========================
*/
 
/*********************************************************************
* @purpose  Verify that a Data Path Table row exists for the specified 
*           interface and direction
*
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServDataPathGet(L7_uint32 intIfNum,
                            L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection);

/*********************************************************************
* @purpose  Determine next sequential row in the Data Path Table
*
* @param    prevIntIfNum    @b{(input)}  Internal interface number to 
                                         begin search
* @param    prevIfDirection @b{(input)}  Interface direction to begin search
* @param    pIntIfNum       @b{(output)} Pointer to next sequential internal 
*                                        interface number value
* @param    pIfDirection    @b{(output)} Pointer to next sequential
*                                        interface direction value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevIntIfNum value of 0 is used to find the first interface
*           entry in the table.
*
* @notes    A prevIfDirection value of 0 is used to find the first
*           interface direction entry relative to the specified prevIntIfNum.
*
* @end
*********************************************************************/
L7_RC_t diffServDataPathGetNext(L7_uint32 prevIntIfNum, 
                        L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t prevIfDirection,
                                L7_uint32 *pIntIfNum,
                        L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t *pIfDirection);

/*********************************************************************
* @purpose  Get the value of an object in the Data Path Table based on 
*           Object Id
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    ifDirection @b{(input)}  Interface direction
* @param    objectId    @b{(input)}  Object Id
* @param    pValue      @b{(output)} Pointer to the value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServDataPathObjectGet(L7_uint32 intIfNum,
                              L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                              L7_DIFFSERV_DATA_PATH_TABLE_OBJECT_t objectId,
                                       void *pValue);

/*
==========================
==========================
==========================

   CLASSIFIER TABLE API

==========================
==========================
==========================
*/

/*************************************************************************
* @purpose  Obtain the current value of the classifier index next variable
*
* @param    pClfrIndex  @b{(output)} Pointer to classifier index value
*
* @returns  void
*
* @notes    Only outputs a value if the pClfrIndex parm is non-null.
*
* @notes    A *pClfrIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServClfrIndexNext(L7_uint32 *pClfrIndex);

/*********************************************************************
* @purpose  Verify that a Classifeir Table row exists for the specified 
*           classifier Id.
*
* @param    clfrId      @b{(input)} Classifier Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServClfrGet(L7_uint32 clfrId);

/*********************************************************************
* @purpose  Determine next sequential row in the Classifier Table
*
* @param    prevClfrId  @b{(input)}  Classifier Id to begin search
* @param    pClfrId     @b{(output)} Pointer to next sequential classifier Id.
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevClfrId value of 0 is used to find the first classifier
*           entry in the table.
*
* @end
*********************************************************************/
L7_RC_t diffServClfrGetNext(L7_uint32 prevClfrId, L7_uint32 *pClfrId);

/*********************************************************************
* @purpose  Get the value of an object in the Classifier Table based on
*           Object Id
*
* @param    clfrId      @b{(input)}  Classifier Id
* @param    pValue      @b{(output)} Pointer to the value
* @param    objectId    @b{(input)}  Object Id
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServClfrObjectGet(L7_uint32 clfrId, 
                              L7_DIFFSERV_CLFR_TABLE_OBJECT_t objectId,
                              void *pValue);


/*
==================================
==================================
==================================

   CLASSIFIER ELEMENT TABLE API

==================================
==================================
==================================
*/

/*************************************************************************
* @purpose  Obtain the current value of the classifier element index
*           next variable
*
* @param    pClfrElemIndex  @b{(output)} Pointer to classifier element
*                                        index value
*
* @returns  void
*
* @notes    Only outputs a value if the pClfrElemIndex parm is non-null.
*
* @notes    A *pClfrElemIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServClfrElemIndexNext(L7_uint32 *pClfrElemIndex);

/*********************************************************************
* @purpose  Verify that a Classifier Element Table row exists for the 
*           specified Classifier Element Id.
*
* @param    clfrId      @b{(input)} Classifier Id
* @param    clfrElemId  @b{(input)} Classifier Element Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServClfrElemGet(L7_uint32 clfrId, L7_uint32 clfrElemId);

/*********************************************************************
* @purpose  Determine next sequential row in the Classifier Element Table
*
* @param    prevClfrId      @b{(input)}  Classifier Id   to begin search
* @param    prevClfrElemId  @b{(input)}  Classifier Element Id to begin search
* @param    pClfrId         @b{(input)}  Pointer to next sequential classifier 
*                                        Id
* @param    pClfrElemId     @b{(output)} Pointer to next sequential classifier 
*                                        Element Id.
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevClfrElemId value of 0 is used to find the first classifier
*           element entry in the table.
*
* @end
*********************************************************************/
L7_RC_t diffServClfrElemGetNext(L7_uint32 prevClfrId, L7_uint32 prevClfrElemId, 
                                L7_uint32 *pClfrId, L7_uint32 *pClfrElemId);

/*********************************************************************
* @purpose  Get the value of an object in the Classifier Element Table
*           based on Object Id
*
* @param    clfrId      @b{(input)}  Classifier Id
* @param    clfrElemId  @b{(input)}  Classifier Element Id
* @param    objectId    @b{(input)}  Object Id
* @param    pValue      @b{(output)} Pointer to the value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServClfrElemObjectGet(L7_uint32 clfrId, L7_uint32 clfrElemId, 
                                  L7_DIFFSERV_CLFR_ELEMENT_TABLE_OBJECT_t objectId,
                                  void *pValue);


/*
=========================================
=========================================
=========================================

   IP MULTI-FIELD CLASSIFIER TABLE API

=========================================
=========================================
=========================================
*/
 
/*************************************************************************
* @purpose  Obtain the current value of the multifield classifier index
*           next variable
*
* @param    pMFClfrIndex  @b{(output)} Pointer to multifield classifier
*                                      index value
*
* @returns  void
*
* @notes    Only outputs a value if the pMFClfrIndex parm is non-null.
*
* @notes    A *pMFClfrIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServMFClfrIndexNext(L7_uint32 *pMFClfrIndex);

/*********************************************************************
* @purpose  Verify that a IP Multi Field Classifier Table row exists for the 
*           specified Multi Field Clfr Id.
*
* @param    mfClfrId    @b{(input)} Multi field Classifier Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServMFClfrGet(L7_uint32 mfClfrId);

/*********************************************************************
* @purpose  Determine next sequential row in the IP Multi Field Classifier 
*           Table
*
* @param    prevMfClfrId  @b{(input)}  Multi field clfr id to begin search
* @param    pMfClfrId     @b{(output)} Pointer to next sequential multi field
*                                      clfr id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A pMfClfrId value of 0 is used to find the first multi field 
*           classifier entry in the table.
*
* @end
*********************************************************************/
L7_RC_t diffServMFClfrGetNext(L7_uint32 prevMfClfrId, L7_uint32 *pMfClfrId);

/*********************************************************************
* @purpose  Get the value of an object in the Multi Field Classifier
*           Table based on Object Id
*
* @param    mfClfrId    @b{(input)}  Multi field Classifier Id
* @param    pValue      @b{(output)} Pointer to the value
* @param    oBjectId    @b{(input)}  Object Id
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServMFClfrObjectGet(L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_TABLE_OBJECT_t objectId,
                                L7_uint32 mfClfrId,
                                void *pValue);


/*
==============================================
==============================================
==============================================

   AUXILIARY MULTI-FIELD CLASSIFIER TABLE API

==============================================
==============================================
==============================================
*/
 
/*************************************************************************
* @purpose  Obtain the current value of the Auxiliary Multifield 
*           Classifier index next variable
*
* @param    pAuxMFClfrIndex  @b{(output)} Pointer to Auxiliary Multifield
*                                         Classifier index value
*
* @returns  void
*
* @notes    Only outputs a value if the pAuxMFClfrIndex parm is non-null.
*
* @notes    A *pAuxMFClfrIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServAuxMFClfrIndexNext(L7_uint32 *pAuxMFClfrIndex);

/*********************************************************************
* @purpose  Verify that an Auxiliary Multi Field Classifier Table row exists 
*           for the specified Id.
*
* @param    auxMFClfrId @b{(input)} Auxiliary Multi field Classifier Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServAuxMFClfrGet(L7_uint32 auxMFClfrId);

/*********************************************************************
* @purpose  Determine next sequential row in the Auxiliary Multi Field
*           Classifier Table
*
* @param    prevAuxMfClfrId @b{(input)}  Auxiliary Multi field clfr id to 
*                                        begin search
* @param    pAuxMfClfrId    @b{(output)} Pointer to next sequential aux multi 
*                                        field clfr id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A pAuxMfClfrId value of 0 is used to find the first auxiliary multi 
*           field classifier entry in the table.
*
* @end
*********************************************************************/
L7_RC_t diffServAuxMFClfrGetNext(L7_uint32 prevAuxMfClfrId,
                                 L7_uint32 *pAuxMfClfrId);

/*********************************************************************
* @purpose  Get the value of an object in the Auxiliary Multi Field 
*           Classifier Table based on Object Id
*
* @param    auxMFClfrId @b{(input)}  Auxiliary Multi field Classifier Id
* @param    objectId    @b{(input)}  Object Id
* @param    pvalue     @b{(output)} Pointer to the value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value never been established
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServAuxMFClfrObjectGet(L7_uint32 auxMFClfrId, 
                      L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_TABLE_OBJECT_t objectId,
                                   void *pValue);
/*
=====================
=====================
=====================

   METER TABLE API

=====================
=====================
=====================
*/

/*************************************************************************
* @purpose  Obtain the current value of the meter index next variable
*
* @param    pMeterIndex  @b{(output)} Pointer to meter index value
*
* @returns  void
*
* @notes    Only outputs a value if the pMeterIndex parm is non-null.
*
* @notes    A *pMeterIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServMeterIndexNext(L7_uint32 *pMeterIndex);

/*********************************************************************
* @purpose  Verify that a Meter Table row exists for the specified 
*           Meter Id
*
* @param    meterId     @b{(input)} Meter Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServMeterGet(L7_uint32 meterId);

/*********************************************************************
* @purpose  Determine next sequential row in the Meter Table
*
* @param    prevMeterId @b{(input)}  Meter Id to begin search
* @param    pMeterId    @b{(output)} Pointer to next sequential
*                                    Meter Id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevMeterId value of 0 is used to find the first Meter 
*           Entry in the table
*
* @end
*********************************************************************/
L7_RC_t diffServMeterGetNext(L7_uint32 prevMeterId, L7_uint32 *pMeterId);

/*********************************************************************
* @purpose  Get the value of an object in the Meter Table based on Object Id
*
* @param    meterId    @b{(input)}  Meter Id
* @param    objectId    @b{(input)}  Object Id
* @param    pValue     @b{(output)} Pointer to the value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServMeterObjectGet(L7_uint32 meterId,
                               L7_DIFFSERV_METER_TABLE_OBJECT_t objectId,
                               void *pValue);


/*
======================================
======================================
======================================

   TOKEN BUCKET PARAMETER TABLE API

======================================
======================================
======================================
*/

/*************************************************************************
* @purpose  Obtain the current value of the TB param index next variable
*
* @param    pTBParamIndex  @b{(output)} Pointer to TB param index value
*
* @returns  void
*
* @notes    Only outputs a value if the pTBParamIndex parm is non-null.
*
* @notes    A *pTBParamIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServTBParamIndexNext(L7_uint32 *pTBParamIndex);

/*********************************************************************
* @purpose  Verify that a Token Bucket Parameter Table row exists for the 
*           specified TB Parameter Id
*
* @param    tbParamId   @b{(input)} TB Parameter Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServTBParamGet(L7_uint32 tbParamId);

/*********************************************************************
* @purpose  Determine next sequential row in the Token Bucket Parameter Table 
*
* @param    prevTbParamId @b{(input)}  TB Parameter Id to begin search
* @param    pTbParamId    @b{(output)} Pointer to next sequential
*                                      TB Parameter Id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevTbParamId value of 0 is used to find the first TB parameter 
*           Entry in the table
*
* @end
*********************************************************************/
L7_RC_t diffServTBParamGetNext(L7_uint32 prevTbParamId, L7_uint32 *pTbParamId);

/*********************************************************************
* @purpose  Get the value of an object in the Token Bucket Parameter Table
*           based on Object Id
*
* @param    tbParamId    @b{(input)}  TB Parameter Id
* @param    pValue       @b{(output)} Pointer to the value
* @param    objectId     @b{(input)}  Object Id
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServTBParamObjectGet(L7_uint32 tbParamId,
                                 L7_DIFFSERV_TB_PARAM_TABLE_OBJECT_t objectId,
                                 void *pValue);


/*
=============================
=============================
=============================

   COLOR AWARE TABLE APIs

=============================
=============================
=============================
*/

/*********************************************************************
* @purpose  Get the value of an object in the Color Aware Table based
*           on Object Id
*
* @param    meterId       @b{(input)}  Meter Id
* @param    objectId      @b{(input)}  Object Id
* @param    pValue        @b{(output)} Pointer to object value 
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    Augments the Meter Table.
*
* @end
*********************************************************************/
L7_RC_t diffServColorAwareObjectGet(L7_uint32 meterId,
                                    L7_DIFFSERV_COLOR_AWARE_TABLE_OBJECT_t objectId,
                                    void *pValue);


/*
======================
======================
======================

   ACTION TABLE API

======================
======================
======================
*/

/*************************************************************************
* @purpose  Obtain the current value of the action index next variable
*
* @param    pActionIndex  @b{(output)} Pointer to action index value
*
* @returns  void
*
* @notes    Only outputs a value if the pActionIndex parm is non-null.
*
* @notes    An L7_SUCCESS return with an output *pActionIndex value of 0
*           means the table is full.
*
* @end
*********************************************************************/
void diffServActionIndexNext(L7_uint32 *pActionIndex);

/*********************************************************************
* @purpose  Verify that an Action Table row exists for the specified Action id
*
* @param    actionId    @b{(input)} Action Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServActionGet(L7_uint32 actionId);

/*********************************************************************
* @purpose  Determine next sequential row in the Action Table
*
* @param    prevActionId  @b{(input)}  Action id to begin search
* @param    pActionId     @b{(output)} Pointer to next sequential
*                                      Action id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevActionId value of 0 is used to find the first Action 
*           Entry in the table
*
* @end
*********************************************************************/
L7_RC_t diffServActionGetNext(L7_uint32 prevActionId, L7_uint32 *pActionId);

/*********************************************************************
* @purpose  Get the value of an object in the Action Table based on Object Id
*
* @param    actionId    @b{(input)}  Action Id
* @param    objectId    @b{(input)}  Object Id
* @param    pValue      @b{(output)} Pointer to the value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServActionObjectGet(L7_uint32 actionId, 
                                L7_DIFFSERV_ACTION_TABLE_OBJECT_t objectId,
                                void *pValue);
                                                                            

/*
================================
================================
================================

   COS MARK ACTION TABLE API

================================
================================
================================
*/

/*********************************************************************
* @purpose  Verify that a COS Mark Action Table row exists for the 
*           specified COS value
*
* @param    cos  @b{(input)} COS value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    must use this function to determine if cos 0 row exists 
*
* @end
*********************************************************************/
L7_RC_t diffServCosMarkActGet(L7_uint32 cos);

/*********************************************************************
* @purpose  Determine next sequential row in the COS Mark Action Table
*
* @param    prevCos   @b{(input)}  COS value to begin search
* @param    pCos      @b{(output)} Pointer to next sequential COS value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevCos of 0 yields first non-0 row in the table
*
* @end
*********************************************************************/
L7_RC_t diffServCosMarkActGetNext(L7_uint32 prevCos, L7_uint32 *pCos);

/*********************************************************************
* @purpose  Get COS value for the specified COS mark action table row index
*
* @param    cos       @b{(input)}  Index COS value
* @param    objectId  @b{(input)}  Object Id
* @param    pCos      @b{(output)} Pointer to the output COS value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServCosMarkActObjectGet(L7_uint32 cos,
                                    L7_DIFFSERV_COS_MARK_ACT_TABLE_OBJECT_t objectId,
                                    L7_uint32 *pCos);

/*
=================================================
=================================================
=================================================

   SECONDARY COS (COS2) MARK ACTION TABLE APIs

=================================================
=================================================
=================================================
*/

/*********************************************************************
* @purpose  Verify that a Secondary COS Mark Action Table row exists 
*           for the specified COS value
*
* @param    cos         @b{(input)} Secondary COS value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    must use this function to determine if secondary cos 0 row exists 
*
* @end
*********************************************************************/
L7_RC_t diffServCos2MarkActGet(L7_uint32 cos);

/*********************************************************************
* @purpose  Determine next sequential row in the Secondary COS Mark Action Table
*
* @param    prevCos     @b{(input)}  Secondary COS value to begin search
* @param    pCos        @b{(output)} Pointer to next sequential Secondary COS value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevCos of 0 yields first non-0 row in the table
*
* @end
*********************************************************************/
L7_RC_t diffServCos2MarkActGetNext(L7_uint32 prevCos, L7_uint32 *pCos);

/*********************************************************************
* @purpose  Get the COS value for the specified Secondary COS mark action
*           table row index
*
* @param    cos       @b{(input)}  Index Secondary COS value
* @param    objectId  @b{(input)}  Object Id
* @param    pCos      @b{(output)} Pointer to the output Secondary COS value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServCos2MarkActObjectGet(L7_uint32 cos,
                                     L7_DIFFSERV_COS2_MARK_ACT_TABLE_OBJECT_t objectId,
                                     L7_uint32 *pCos);

/*
================================
================================
================================

   DSCP MARK ACTION TABLE API

================================
================================
================================
*/
 
/*********************************************************************
* @purpose  Verify that a DSCP Mark Action Table row exists for the 
*           specified DSCP value
*
* @param    dscp @b{(input)} DSCP value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    must use this function to determine if dscpMarkAct 0 row exists 
*
* @end
*********************************************************************/
L7_RC_t diffServDscpMarkActGet(L7_uint32 dscp);

/*********************************************************************
* @purpose  Determine next sequential row in the DSCP Mark Action Table
*
* @param    prevDscp  @b{(input)}  DSCP value to begin search
* @param    pDscp     @b{(output)} Pointer to next sequential DSCP value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevDscpMarkAct of 0 yields first non-0 row in the table
*
* @end
*********************************************************************/
L7_RC_t diffServDscpMarkActGetNext(L7_uint32 prevDscp, L7_uint32 *pDscp);

/*********************************************************************
* @purpose  Get DSCP value for the specified DSCP Mark Action Table row index
*
* @param    dscp         @b{(input)}  Index DSCP value 
* @param    objectId     @b{(input)}  Object Id
* @param    pDscp        @b{(output)} Pointer to the ouput DSCP value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServDscpMarkActObjectGet(L7_uint32 dscp,
                                     L7_DIFFSERV_DSCP_MARK_ACT_TABLE_OBJECT_t objectId,
                                     L7_uint32 *pDscp);

/*
=======================================
=======================================
=======================-===============

   IP PRECEDENCE  MARK ACTION TABLE API

=======================================
=======================================
=======================================
*/

/*********************************************************************
* @purpose  Verify that an IP Precedence Mark Action Table row exists for the 
*           specified IP Precedence value
*
* @param    ipPrecedence @b{(input)} IP Precedence value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    must use this function to determine if ipPrecedence 0 row exists 
*
* @end
*********************************************************************/
L7_RC_t diffServIpPrecMarkActGet(L7_uint32 ipPrecedence);

/*********************************************************************
* @purpose  Determine next sequential row in the IP Precedence Mark Action 
*           Table
*
* @param    prevIpPrecedence @b{(input)}  Ip Precedence value to begin search
* @param    pIpPrecedence    @b{(output)} Pointer to next sequential
*                                         IP Precedence value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevIpPrecedence of 0 yields first non-0 row in the table
*
* @end
*********************************************************************/
L7_RC_t diffServIpPrecMarkActGetNext(L7_uint32 prevIpPrecedence,
                                     L7_uint32 *pIpPrecedence);

/*********************************************************************
* @purpose  Get IP Precedence value for the specified IP Precedence Mark 
*           Action Table row index
*
* @param    ipPrecedence  @b{(input)}  Index Ip Precedence value 
* @param    objectId      @b{(input)}  Object Id
* @param    pIpPrecedence @b{(output)} Pointer to the output IP Precedence value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServIpPrecMarkActObjectGet(L7_uint32 ipPrecedence,
                           L7_DIFFSERV_IPPREC_MARK_ACT_TABLE_OBJECT_t objectId,
                                       L7_uint32 *pIpPrecedence);

/*
============================
============================
============================

   COUNT ACTION TABLE API

============================
============================
============================
*/
 
/*************************************************************************
* @purpose  Obtain the current value of count action index next variable
*
* @param    pCountActIndex  @b{(output)} Pointer to count action index value
*
* @returns  void
*
* @notes    Only outputs a value if the pCountActIndex parm is non-null.
*
* @notes    A *pCountActIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServCountActIndexNext(L7_uint32 *pCountActIndex);

/*********************************************************************
* @purpose  Verify that a Count Action Table row exists for the 
*           specified Count Action Id
*
* @param    countActId  @b{(input)} Count Action Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServCountActGet(L7_uint32 countActId);

/*********************************************************************
* @purpose  Determine next sequential row in the Count Action Table
*
* @param    prevCountActId @b{(input)}  Count Action Id to begin search
* @param    pCountActId    @b{(output)} Pointer to next sequential
*                                       Count Action Id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevCountActId value of 0 is used to find the first Count 
*           Action Entry in the table
*
* @end
*********************************************************************/
L7_RC_t diffServCountActGetNext(L7_uint32 prevCountActId,
                                L7_uint32 *pCountActId);

/*********************************************************************
* @purpose  Get the value of an object in the Count Action Table based on
*           Object Id
*
* @param    countActId  @b{(input)}  Count Action Id
* @param    objectId    @b{(input)}  Object Id
* @param    pValue      @b{(output)} Pointer to counter value 
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServCountActObjectGet(L7_uint32 countActId,
                                  L7_DIFFSERV_COUNT_ACT_TABLE_OBJECT_t objectId,
                                  void *pValue);


/*
=============================
=============================
=============================

   ASSIGN QUEUE TABLE APIs

=============================
=============================
=============================
*/

/*************************************************************************
* @purpose  Obtain the current value of assign queue index next variable
*
* @param    pAssignIndex  @b{(output)} Pointer to assign queue index value
*
* @returns  void
*
* @notes    Only outputs a value if the pAssignIndex parm is non-null.
*
* @notes    A *pAssignIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServAssignQueueIndexNext(L7_uint32 *pAssignIndex);

/*********************************************************************
* @purpose  Verify that an Assign Queue Table row exists for the 
*           specified Assign Queue Id
*
* @param    assignId  @b{(input)} Assign Queue Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServAssignQueueGet(L7_uint32 assignId);

/*********************************************************************
* @purpose  Determine next sequential row in the Assign Queue Table
*
* @param    prevAssignId  @b{(input)}  Assign Queue Id to begin
*                                        search
* @param    pAssignId     @b{(output)} Pointer to next sequential
*                                        Assign Queue Id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevAssignId value of 0 is used to find the first
*           Assign Queue Entry in the table
*
* @end
*********************************************************************/
L7_RC_t diffServAssignQueueGetNext(L7_uint32 prevAssignId,
                                   L7_uint32 *pAssignId);

/*********************************************************************
* @purpose  Get the value of an object in the Assign Queue Table based
*           on Object Id
*
* @param    assignId    @b{(input)}  Assign Queue Id
* @param    objectId    @b{(input)}  Object Id
* @param    pValue      @b{(output)} Pointer to counter value 
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServAssignQueueObjectGet(L7_uint32 assignId,
                        L7_DIFFSERV_ASSIGN_QUEUE_TABLE_OBJECT_t objectId,
                                     void *pValue);


/*
=============================
=============================
=============================

     REDIRECT TABLE APIs

=============================
=============================
=============================
*/

/*************************************************************************
* @purpose  Obtain the current value of redirect index next variable
*
* @param    pRedirectIndex  @b{(output)} Pointer to redirect index value
*
* @returns  void
*
* @notes    Only outputs a value if the pRedirectIndex parm is non-null.
*
* @notes    A *pRedirectIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServRedirectIndexNext(L7_uint32 *pRedirectIndex);

/*********************************************************************
* @purpose  Verify that a Redirect Table row exists for the 
*           specified Redirect Id
*
* @param    redirectId  @b{(input)} Redirect Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServRedirectGet(L7_uint32 redirectId);

/*********************************************************************
* @purpose  Determine next sequential row in the Redirect Table
*
* @param    prevRedirectId  @b{(input)}  Redirect Id to begin search
* @param    pRedirectId     @b{(output)} Pointer to next sequential
*                                          Redirect Id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevRedirectId value of 0 is used to find the first
*           Redirect Entry in the table
*
* @end
*********************************************************************/
L7_RC_t diffServRedirectGetNext(L7_uint32 prevRedirectId,
                                L7_uint32 *pRedirectId);

/*********************************************************************
* @purpose  Get the value of an object in the Redirect Table based
*           on Object Id
*
* @param    redirectId  @b{(input)}  Redirect Id
* @param    objectId    @b{(input)}  Object Id
* @param    pValue      @b{(output)} Pointer to object value 
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServRedirectObjectGet(L7_uint32 redirectId,
                                  L7_DIFFSERV_REDIRECT_TABLE_OBJECT_t objectId,
                                  void *pValue);


/*
===========================
===========================
===========================

     MIRROR TABLE APIs

===========================
===========================
===========================
*/

/*************************************************************************
* @purpose  Obtain the current value of mirror index next variable
*
* @param    pMirrorIndex    @b{(output)} Pointer to mirror index value
*
* @returns  void
*
* @notes    Only outputs a value if the pMirrorIndex parm is non-null.
*
* @notes    A *pMirrorIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServMirrorIndexNext(L7_uint32 *pMirrorIndex);

/*********************************************************************
* @purpose  Verify that a Mirror Table row exists for the 
*           specified Mirror Id
*
* @param    mirrorId    @b{(input)} Mirror Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServMirrorGet(L7_uint32 mirrorId);

/*********************************************************************
* @purpose  Determine next sequential row in the Mirror Table
*
* @param    prevMirrorId    @b{(input)}  Mirror Id to begin search
* @param    pMirrorId       @b{(output)} Pointer to next sequential
*                                          Mirror Id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevMirrorId value of 0 is used to find the first
*           Mirror Entry in the table
*
* @end
*********************************************************************/
L7_RC_t diffServMirrorGetNext(L7_uint32 prevMirrorId,
                              L7_uint32 *pMirrorId);

/*********************************************************************
* @purpose  Get the value of an object in the Mirror Table based
*           on Object Id
*
* @param    mirrorId    @b{(input)}  Mirror Id
* @param    objectId    @b{(input)}  Object Id
* @param    pValue      @b{(output)} Pointer to object value 
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServMirrorObjectGet(L7_uint32 mirrorId,
                                L7_DIFFSERV_MIRROR_TABLE_OBJECT_t objectId,
                                void *pValue);


/*
================================
================================
================================

   ALGORITHMIC DROP TABLE API

================================
================================
================================
*/
 
/*************************************************************************
* @purpose  Obtain the current value of the algorithmic drop index next
*           variable
*
* @param    UnitIndex      @b{(input)}  System unit number
* @param    pAlgDropIndex  @b{(output)} Pointer to algorithmic drop
*                                       index value
*
* @returns  void
*
* @notes    Only outputs a value if the pAlgDropIndex parm is non-null.
*
* @notes    A *pAlgDropIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServAlgDropIndexNext(L7_uint32 *pAlgDropIndex);

/*********************************************************************
* @purpose  Verify that an Algorithmic Drop Table row exists for the 
*           specified Algorithmic Drop Id
*
* @param    algDropId   @b{(input)} Algorithmic Drop Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServAlgDropGet(L7_uint32 algDropId);

/*********************************************************************
* @purpose  Determine next sequential row in the Algorithmic Drop Table
*
* @param    prevAlgDropId  @b{(input)}  Algorithmic Drop Id to begin search
* @param    pAlgDropId     @b{(output)} Pointer to next sequential
*                                       Algorithmic Drop Id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevAlgDropId value of 0 is used to find the first Algorithmic
*           Drop Entry in the table
*
* @end
*********************************************************************/
L7_RC_t diffServAlgDropGetNext(L7_uint32 prevAlgDropId, L7_uint32 *pAlgDropId);

/*********************************************************************
* @purpose  Get the value of an object in the Algorithmic Drop Table based 
*           on Object Id
*
* @param    algDropId    @b{(input)}  Algorithmic Drop Id
* @param    pAlgDropType @b{(output)} Pointer to algorithm type value 
* @param    ObjectId     @b{(input)}  Object Id
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServAlgDropObjectGet(L7_DIFFSERV_ALG_DROP_TABLE_OBJECT_t objectId,
                                 L7_uint32 algDropId, void *pValue);

/*
=========================
=========================
=========================

   INTERFACE USAGE API

=========================
=========================
=========================
*/

/*********************************************************************
* @purpose  Check if the specified Internal Interface Number is in
*           use by the ACL component
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    ifDirection @b{(input)} Interface Direction
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*       
* @end
*********************************************************************/
L7_BOOL diffServIsAclIntfInUse(L7_uint32 intIfNum, 
                               L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection);

/*********************************************************************
* @purpose  Determine if the interface is valid in diffServ
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
L7_BOOL diffServIsValidIntfType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Determine if the interface is valid in diffServ
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
L7_BOOL diffServIsValidIntf(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine if interface is in an attached state
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    *pIntfState @b{(output)} ptr to output location, or L7_NULLPTR
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments An 'attached' state by this definition is one of:
*           L7_INTF_ATTACHING, L7_INTF_ATTACHED, or L7_INTF_DETACHING.
*
* @comments Pass a non-null pointer for the pIntfState parameter
*           to retrieve the NIM interface state value used here.
*       
* @end
*********************************************************************/
L7_BOOL diffServIsIntfAttached(L7_uint32 intIfNum, L7_INTF_STATES_t *pIntfState);


/*
===============
===============
===============

   OTHER API   

===============
===============
===============
*/

/*********************************************************************
* @purpose  Get the MAC Access List name for the specified ACL number
*
* @param    aclNum          @b{(input)} MAC Access List number
* @param    pName           @b{(output)} MAC ACL name pointer
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR        ACL does not exist
* @returns  L7_FAILURE
*
* @notes    Caller must provide a name buffer of at least 
*           (L7_USMDB_MIB_DIFFSERV_CLASS_ACL_NAME_MAX+1) characters.
*
* @end
*********************************************************************/
L7_RC_t diffServAclMacNameGet(L7_uint32 aclNum, L7_uchar8 *pName);

/*********************************************************************
* @purpose  Query if the specified ACL number matches a configured
*           named IPv4 Access List
*
* @param    aclNum          @b{(input)} IP Access List number
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR        ACL does not exist
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t diffServIsAclNamedIpAcl(L7_uint32 aclNum);

/*********************************************************************
* @purpose  Get the IP Access List name for the specified ACL number
*
* @param    aclNum          @b{(input)} IP Access List number
* @param    pName           @b{(output)} IP ACL name pointer
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR        ACL does not exist
* @returns  L7_FAILURE
*
* @notes    Caller must provide a name buffer of at least 
*           (L7_USMDB_MIB_DIFFSERV_CLASS_ACL_NAME_MAX+1) characters.
*
* @end
*********************************************************************/
L7_RC_t diffServAclIpNameGet(L7_uint32 aclNum, L7_uchar8 *pName);

/*********************************************************************
*
* @purpose  Get the index number of a MAC access list, given its name.
*
* @param    pName      @b{(input)} access list name pointer
* @param    *aclIndex  @b{(output)} access list index pointer
*                                            
* @returns  L7_SUCCESS
* @returns  L7_ERROR        ACL does not exist
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*
*********************************************************************/
L7_RC_t diffServAclMacNameToIndex(L7_uchar8 *pName, L7_uint32 *pAclNum);

/*********************************************************************
*
* @purpose  Get the index number of an access list, given its ACL type and name.
*
* @param    aclType     @b{(input)} access list type
* @param    *pName      @b{(input)} access list name pointer
* @param    *aclNum     @b{(output)} access list identifying number
*                                            
* @returns  L7_SUCCESS
* @returns  L7_ERROR    ACL does not exist
* @returns  L7_FAILURE
*
* @notes    Uses the ACL type and name to output the list index
*           number.  For IPV4 numbered access lists, the ACL name
*           is its number represented in string notation, e.g. "101".
*
* @end
*
*********************************************************************/
L7_RC_t diffServAclCommonNameToIndex(L7_ACL_TYPE_t aclType, L7_uchar8 *pName,
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
* @notes    Assumes caller provides a name buffer of at least 
*           (L7_ACL_NAME_LEN_MAX+1) characters.
*
* @notes    Determines the ACL type from its index number and outputs
*           the list name.  For IPV4 numbered access lists, the 
*           ACL number is output in string notation, e.g. "101".
*
* @end
*
*********************************************************************/
L7_RC_t diffServAclCommonNameStringGet(L7_uint32 aclnum, L7_ACL_TYPE_t *aclType,
                                       L7_uchar8 *name);

/*********************************************************************
* @purpose  Indicates if DiffServ cluster config receive operation currently in progress
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
L7_BOOL diffServClusterIsRxInProgress(void);

/*********************************************************************
* @purpose  Translate a config push policy name into its associated Policy Table index
*
* @param    stringPtr     @b{(input)}  Policy name string pointer
* @param    pPolicyIndex  @b{(output)} Pointer to policy index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Searches the received config push data for a matching
*           policy name.
*
* @end
*********************************************************************/
L7_RC_t diffServClusterPolicyNameToIndex(L7_uchar8 *stringPtr, 
                                         L7_uint32 *pPolicyIndex);

/*********************************************************************
* @purpose  Get current number of instances in a config push policy definition
*
* @param    policyIndex   @b{(input)}  Policy index
* @param    pCount        @b{(output)} Pointer to instance count output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServClusterPolicyInstCountGet(L7_uint32 policyIndex, 
                                          L7_uint32 *pCount);

#endif /* L7_DIFFSERV_API_H */
