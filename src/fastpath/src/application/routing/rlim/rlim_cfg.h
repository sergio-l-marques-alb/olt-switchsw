/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename rlim.h
*
* @purpose Contains prototypes and Data Structures
*          for managing routing logical interfaces
*
* @component Routing Logical Interface Manager
*
* @comments
*
* @create 02/16/2005
*
* @author eberge
* @end
*
**********************************************************************/

#ifndef INCLUDE_RLIM_CFG_H
#define INCLUDE_RLIM_CFG_H

#include "l7_product.h"
#include "l3_compdefs.h"
#include "nimapi.h"

/*
 * Configuration defines and data structures
 */

#define RLIM_MAX_LOOPBACK_COUNT   L7_MAX_NUM_LOOPBACK_INTF
#define RLIM_MAX_LOOPBACK_ID      L7_LOOPBACKID_MAX

#define RLIM_MAX_TUNNEL_COUNT     L7_MAX_NUM_TUNNEL_INTF
#define RLIM_MAX_TUNNEL_ID        L7_TUNNELID_MAX

#if L7_LOOPBACKID_MIN != 0
#error assumption violated: min loopback ID is non-zero
#endif
#if L7_TUNNELID_MIN != 0
#error assumption violated: min tunnel ID is non-zero
#endif


typedef struct {
  L7_uchar8 addrType;
  union {
    L7_uint32 ip4addr;
    L7_in6_addr_t ip6addr;
    nimConfigID_t intfConfigId;
  } un;
} rlimAddr_t;

#define RLIM_LOOPBACK_INTF_INDICES \
          (RLIM_MAX_LOOPBACK_ID / (sizeof(L7_uchar8) * 8) + 1)

typedef struct
{
  L7_uchar8 value[RLIM_LOOPBACK_INTF_INDICES];
} rlimLoopbackMask_t;

#define RLIM_TUNNEL_INTF_INDICES \
          (RLIM_MAX_TUNNEL_ID / (sizeof(L7_uchar8) * 8) + 1)

typedef struct
{
  L7_uchar8 value[RLIM_TUNNEL_INTF_INDICES];
} rlimTunnelMask_t;

typedef struct
{
  L7_uchar8 tunnelMode;
  rlimAddr_t localAddr;
  rlimAddr_t remoteAddr;
} rlimTunnelCfgData_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  rlimLoopbackMask_t loopbackMask;
  rlimTunnelMask_t tunnelMask;
  rlimTunnelCfgData_t tunnelCfgData[RLIM_MAX_TUNNEL_COUNT];
  L7_uint32 checkSum; /* checksum of configfile: MUST BE LAST */
} rlimCfgData_t;

extern rlimCfgData_t *rlimCfgData;

#define RLIM_CFG_FILENAME     "rlim.cfg"
#define RLIM_CFG_VER_1        0x1
#define RLIM_CFG_VER_CURRENT  RLIM_CFG_VER_1

/*
 * Function prototypes
 */
void rlimBuildDefaultConfigData(L7_uint32 ver);
void rlimTunnelDefaultConfigSet(rlimTunnelCfgData_t *tcp);
L7_RC_t rlimApplyConfigData(void);

#endif /* !defined(INCLUDE_RLIM_CFG_H) */
