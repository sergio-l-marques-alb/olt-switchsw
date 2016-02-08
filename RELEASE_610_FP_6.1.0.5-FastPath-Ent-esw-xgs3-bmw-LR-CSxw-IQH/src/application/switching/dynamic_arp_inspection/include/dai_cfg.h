
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
* @filename  dai_cfg.h
*
* @purpose   Dynamic ARP Inspection configuration
*
* @component Dynamic ARP Inspection
*
* @create 8/28/2007
*
* @author Kiran Kumar Kella
*
* @end
*             
**********************************************************************/
#ifndef DAI_CFG_H
#define DAI_CFG_H


#include "comm_mask.h"
#include "comm_structs.h"
#include "l7_common.h"

#define DAI_MAX_INTF_COUNT   L7_MAX_INTERFACE_COUNT
#define DAI_MAX_VLAN_COUNT   L7_PLATFORM_MAX_VLAN_ID
#define DAI_MAX_TRACE_LEN    512
#define ARP_ACL_NULL          ""

typedef enum daiTraceFlags_e
{
  DAI_TRACE_INIT = 0x1,
  DAI_TRACE_VLAN_EVENTS = 0x2,
  DAI_TRACE_PORT_STATE = 0x4,
  DAI_TRACE_FRAME_TX = 0x8,
  DAI_TRACE_FRAME_TX_DETAIL = 0x10,
  DAI_TRACE_FRAME_RX = 0x20,
  DAI_TRACE_FRAME_RX_DETAIL = 0x40

} daiTraceFlags_t;

typedef L7_uchar8 aclName_t[L7_ARP_ACL_NAME_LEN_MAX+1];

/* ARP ACL Rule structure */
typedef struct arpAclRule_s
{
  L7_IP_ADDR_t    senderHostIp;
  L7_uchar8       senderHostMac[L7_ENET_MAC_ADDR_LEN];
} arpAclRule_t;

/* ARP ACL command structure */
typedef struct arpAclCfg_s
{
  aclName_t     aclName;

  arpAclRule_t  rules[L7_ARP_ACL_RULES_MAX];

} arpAclCfg_t;

/* Rate limit config structure */
typedef struct rateLimitCfg_s
{
  /* rate limit parameters */
  /* value of -1 means rate limit is set to none */
  L7_int32              rate_limit;
  L7_uint32             burst_interval;
} rateLimitCfg_t;

/* Global configuration data for Dynamic ARP Inspection */
typedef struct daiCfgData_s
{
  /* Set of VLANs where DAI is enabled */
  L7_VLAN_MASK_t        daiVlans;

  /* DAI trust config for each port */
  L7_INTF_MASK_t        daiTrust;

  /* source MAC verify */
  L7_BOOL               daiVerifySMac;

  /* destination MAC verify */
  L7_BOOL               daiVerifyDMac;

  /* IP address verify */
  L7_BOOL               daiVerifyIP;

  /* Static Flag setting per VLAN */
  L7_VLAN_MASK_t        daiStaticFlag;

  /* Enable/disable logging of DAI errors per VLAN */
  L7_VLAN_MASK_t        daiLogInvalid;

  /* Event tracing */
  daiTraceFlags_t       daiTraceFlags;

  /* rate limit configuration of all interfaces */
  rateLimitCfg_t        intfCfg[DAI_MAX_INTF_COUNT];

  /* ARP ACL Name settings per VLAN */
  aclName_t             aclName[DAI_MAX_VLAN_COUNT+1];

  /* ARP ACL commands data */
  arpAclCfg_t           arpAclCfg[L7_ARP_ACL_CMDS_MAX];

} daiCfgData_t;


#endif /* DAI_CFG_H */
