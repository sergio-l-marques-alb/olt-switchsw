
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename rip_map_migrate.h
*
* @purpose RIP Configuration Migration
*
* @component RIP
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#ifndef RIP_MAP_MIGRATE_H
#define RIP_MAP_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "l7_ripinclude.h"

#define L7_RIP_MAP_INTF_MAX_COUNT_REL_4_1 L7_IPMAP_INTF_MAX_COUNT_REL_4_1
#define L7_RIP_MAP_INTF_MAX_COUNT_REL_4_2 L7_IPMAP_INTF_MAX_COUNT_REL_4_2
#define L7_RIP_MAP_INTF_MAX_COUNT_REL_4_3 L7_IPMAP_INTF_MAX_COUNT_REL_4_3
#define L7_RIP_MAP_INTF_MAX_COUNT_REL_4_4 L7_IPMAP_INTF_MAX_COUNT_REL_4_4

typedef struct
{
  L7_uint32 ripAdminMode;       /* Enable RIP                 */
  L7_uint32 ripSplitHorizon;    /* Split Horizon mode         */
  L7_uint32 ripAutoSummarization;       /* Auto Summarization mode    */
  L7_uint32 ripHostRoutesAccept;        /* Host routes accept mode    */
  L7_uint32 ripDefaultMetric;   /* Default metric             */
  ripRtRedistCfg_t ripRtRedist[REDIST_RT_LAST]; /* Route Redistribution Config */
  ripDefRtAdvCfg_t ripDefRtAdv; /* Default Route Adv Config   */
}
ripCfgDataV3_t;

typedef struct
{
  L7_uint32 adminMode;          /* Enable RIP for interface   */
  L7_uint32 authType;           /* L7_AUTH_TYPES_t            */
  L7_char8 authKey[L7_AUTH_MAX_KEY_RIP];        /* authentication key     */
  L7_uint32 authKeyLen;         /* original length of auth key */
  L7_uint32 authKeyId;          /* auth key identifier (MD5)  */
  L7_uint32 verSend;            /* RIP version to send        */
  L7_uint32 verRecv;            /* RIP version to receive     */
  L7_uint32 intfCost;           /* interface cost (FUTURE USE) */
}
ripCfgCktV3_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  ripCfgDataV3_t rtr;
  ripCfgCktV3_t ckt[L7_MAX_SLOTS_PER_BOX_REL_4_0][L7_MAX_PORTS_PER_SLOT_REL_4_0 + 1];
  L7_uint32 checkSum;
}
ripMapCfgV3_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_uint32 ripAdminMode;       /* Enable RIP                 */
  L7_uint32 ripSplitHorizon;    /* Split Horizon mode         */
  L7_uint32 ripAutoSummarization;       /* Auto Summarization mode    */
  L7_uint32 ripHostRoutesAccept;        /* Host routes accept mode    */
  L7_uint32 ripDefaultMetric;   /* Default metric             */
  ripRtRedistCfg_t ripRtRedist[REDIST_RT_LAST_REL_4_1]; /* Route Redistribution Config */
  ripDefRtAdvCfg_t ripDefRtAdv; /* Default Route Adv Config   */

  ripMapCfgIntf_t intf[L7_RIP_MAP_INTF_MAX_COUNT_REL_4_1];
  L7_uint32 checkSum;

} ripMapCfgV4_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_uint32 ripAdminMode;             /* Enable RIP                 */
  L7_uint32 ripSplitHorizon;          /* Split Horizon mode         */
  L7_uint32 ripAutoSummarization;     /* Auto Summarization mode    */
  L7_uint32 ripHostRoutesAccept;      /* Host routes accept mode    */ 
  L7_uint32 ripDefaultMetric;         /* Default metric             */
  ripRtRedistCfg_t ripRtRedist[REDIST_RT_LAST_REL_4_3]; /* Route Redistribution Config*/
  ripDefRtAdvCfg_t ripDefRtAdv;       /* Default Route Adv Config   */

  ripMapCfgIntf_t intf[L7_RIP_MAP_INTF_MAX_COUNT_REL_4_3];
  L7_uint32 checkSum;

} ripMapCfgV5_t;

typedef struct
{
  L7_fileHdr_t     cfgHdr;
  L7_uint32        ripAdminMode;             /* Enable RIP                 */
  L7_uint32        ripSplitHorizon;          /* Split Horizon mode         */
  L7_uint32        ripAutoSummarization;     /* Auto Summarization mode    */
  L7_uint32        ripHostRoutesAccept;      /* Host routes accept mode    */ 
  L7_uint32           ripDefaultMetric;             /* Default metric             */
  ripRtRedistCfg_t    ripRtRedist[REDIST_RT_LAST_REL_4_4];  
                                             /* Route Redistribution Config*/
  ripDefRtAdvCfg_t    ripDefRtAdv;           /* Default Route Adv Config   */

  ripMapCfgIntf_t  intf[L7_RIP_MAP_INTF_MAX_COUNT_REL_4_4];
  L7_uint32        checkSum;

} ripMapCfgV6_t;


typedef ripMapCfg_t ripMapCfgV7_t;

#endif /* RIP_MAP_MIGRATE_H */
