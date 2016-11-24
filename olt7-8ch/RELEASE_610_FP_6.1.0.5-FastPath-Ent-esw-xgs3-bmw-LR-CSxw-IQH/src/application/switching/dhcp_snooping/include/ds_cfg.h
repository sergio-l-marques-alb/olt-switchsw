/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  ds_cfg.h
*
* @purpose   DHCP Snooping configuration
*
* @component DHCP Snooping
*
* @create 3/15/2007
*
* @author Rob Rice
*
* @end
*             
**********************************************************************/
#ifndef DS_CFG_H
#define DS_CFG_H


#include "comm_mask.h"
#include "l7_common.h"
#include "dhcp_snooping_api.h"


#define DS_MAX_TRACE_LEN    512

#define L7_DHCP_SNOOPING_CFG_FILENAME       "dsDb.cfg"
#define L7_DHCP_SNOOPING_CFG_VER_1          0x1
#define L7_DHCP_SNOOPING_VER_CURRENT        L7_DHCP_SNOOPING_CFG_VER_1


typedef enum dsTraceFlags_e
{
  DS_TRACE_INIT = 0x1,
  DS_TRACE_VLAN_EVENTS = 0x2,
  DS_TRACE_PORT_STATE = 0x4,
  DS_TRACE_FRAME_TX = 0x8,
  DS_TRACE_FRAME_RX = 0x10,
  DS_TRACE_FRAME_RX_DETAIL = 0x20,
  DS_TRACE_IPSG_PORT = 0x40,
  DS_TRACE_BINDING = 0x80,

  #ifdef L7_DHCP_L2_RELAY_PACKAGE
  /* L2 Relay specific traces, though all above traces are equally 
     applicable to L2 relay except DS_TRACE_IPSG_PORT and DS_TRACE_BINDING. */
  DS_TRACE_OPTION82_SERVER = 0x100,      /* Option-82 checks and validations for server packets */
  DS_TRACE_OPTION82_CLIENT = 0x200,      /* Option-82 checks and validations for client packets */
  DS_TRACE_OPTION82_CONFIG = 0x400,      /* Option-82 configuration checks */
  DS_TRACE_OPTION82_EXTERNAL_CALLS = 0x800       /* Option-82 external calls/status s*/
  #endif


} dsTraceFlags_t;

#define DHCPS_MESSAGE_TYPE_ACK             5
#define DHCPS_MESSAGE_TYPE_NACK            6

#ifdef L7_IPSG_PACKAGE
/* Interface configuration for IP Source Guard */
typedef struct ipsgIntfCfg_s
{
  /* Whether to check the source IP address of incoming packets. */
  L7_BOOL verifyIp;

  /* Whether to check the source MAC address of incoming packets. */
  L7_BOOL verifyMac;

} ipsgIntfCfg_t;
#endif

/* Rate limit config structure */
typedef struct rateLimitCfg_s
{
  /* rate limit parameters */
  /* value of -1 means rate limit is set to none */
  L7_int32              rate_limit;
  L7_uint32             burst_interval;
} rateLimitCfg_t;

/* Interface configuration structure */
typedef struct dsIntfCfg_s
{
  dsL2RelaySubscriptionCfg_t  dsL2RelaySubscrptionCfg[L7_DOT1AD_SERVICES_MAX_PER_INTF];
  rateLimitCfg_t   rateLimitCfg;
} dsIntfCfg_t;


typedef struct dsCfgData_s
{
  /* Whether DHCP snooping is enabled globally */
  L7_uint32 dsGlobalAdminMode;   
  /* Whether to drop incoming frames whose source MAC address is 
   * not in the DHCP snooping bindings database. */
  L7_BOOL dsVerifyMac;

  /* Set of VLANs where DHCP snooping is enabled */
  L7_VLAN_MASK_t dsVlans;

#ifdef L7_DHCP_L2_RELAY_PACKAGE
  /* Whether DHCP L2 Relay is enbaled globally */
  L7_uint32 dsL2RelayAdminMode;
  /* DHCP L2Relay config for each port. */
  L7_INTF_MASK_t dsL2RelayIntfMode;

  /* DHCP L2 Relay trust config for each port. */
  L7_INTF_MASK_t dsL2RelayTrust;

  /* DHCP L2Relay config for each vlan. */
  L7_VLAN_MASK_t dsL2RelayVlanMode;

  /* DHCP L2Relay circuit-identifier for each vlan. */
  L7_VLAN_MASK_t dsL2RelayCircuitIdMode;

  /* DHCP L2Relay remote-identifier string for each vlan. */
  L7_uchar8 dsL2RelayRemoteId[L7_PLATFORM_MAX_VLAN_ID+1][DS_MAX_REMOTE_ID_STRING];
#endif
  
  /* DHCP Snooping trust config for each port. */
  L7_INTF_MASK_t dsTrust;

  /* Enable/disable logging of DHCP snooping errors */
  L7_INTF_MASK_t dsLogInvalid;

  /* Remote file Name to store & restore the DS Bindings */
   L7_uchar8 dsDbFileName[L7_CLI_MAX_STRING_LENGTH];

  /* Remote IP addr to store & restore the DS Bindings */
  L7_uint32 dsDbIpAddr;  
   
  /* Write delay interval to store the DHCP SNOOPING bindings */
  L7_uint32 dsDbStoreInterval; 

  /* Event tracing. */
  dsTraceFlags_t dsTraceFlags;
  
  /*  configuration of all interfaces */
  dsIntfCfg_t      dsIntfCfg[DS_MAX_INTF_COUNT+1];

#ifdef L7_IPSG_PACKAGE
  ipsgIntfCfg_t ipsgIntfCfg[DS_MAX_INTF_COUNT];
#endif

} dsCfgData_t;


#endif /* DS_CFG_H */
