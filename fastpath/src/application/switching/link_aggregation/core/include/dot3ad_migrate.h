
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename dot3ad_migrate.h
*
* @purpose dot3ad Configuration Migration
*
* @component dot3ad
*
* @comments none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#ifndef DOT3AD_MIGRATE_H
#define DOT3AD_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "dot3ad_include.h"

typedef struct
{
  struct dot3ad_agg_s     *next;
  struct dot3ad_agg_s     *prev;

  L7_enetMacAddr_t    aggMacAddr;       /* MAC address associated with this logical interface */
  L7_uint32           aggId;            /* intIfNum of this logical interface */
  L7_BOOL             individualAgg;    /* L7_TRUE: default, aggregator represents an aggregate */
  L7_uint32           actorAdminAggKey; /* intIfNum of this logical interface */
  L7_uint32           actorOperAggKey;  /* intIfNum of this logical interface */

  L7_enetMacAddr_t    partnerSys;
  L7_uint32           partnerSysPri;
  L7_uint32           partnerOperAggKey;

  L7_uint32           collectorMaxDelay;
  L7_uint32           rxState;          /* enabled - disabled */
  L7_uint32           txState;

  /*ports actively participating in aggregation*/
  L7_uint32           aggActivePortList[L7_MAX_MEMBERS_PER_LAG];
  L7_uint32           activeNumMembers;
  /* ports attached to this aggregator */
  L7_uint32           aggPortList[L7_MAX_MEMBERS_PER_LAG];
  L7_uint32           currNumMembers;

  /* ports that have selected this aggregator */
  L7_uint32           aggSelectedPortList[L7_MAX_MEMBERS_PER_LAG];
  L7_uint32           currNumSelectedMembers;

  /* ports waiting to meet all selection requirements */
  L7_uint32           aggWaitSelectedPortList[L7_MAX_MEMBERS_PER_LAG];
  L7_uint32           currNumWaitSelectedMembers;
  dot3ad_usp_t        aggPortListUsp[L7_MAX_MEMBERS_PER_LAG];

  L7_BOOL             ready;

  L7_uchar8           name[L7_DOT3AD_MAX_NAME];
  L7_BOOL             inuse;  /* aggregator has been assigned */
  L7_uint32           adminMode; /*L7_ENABLE enabled, L7_DISABLE disabled*/
  L7_uint32           unused;  /*Was linktrapstate. Since that value was always with nim. removed to avoid confusion*/
  L7_uint32           stpMode;
  L7_BOOL             isStatic;  /* This LAG is static, i.e. no protocol partner
                                  * Using default values to aggregate  */

} dot3ad_aggV4_t;

typedef struct
{
  dot3ad_port_t dot3adPort[L7_MAX_PORT_COUNT_REL_4_0];  /* holds per port into       */
  dot3ad_aggV4_t dot3adAgg[L7_MAX_NUM_LAG_INTF_REL_4_0];  /* holds per aggregator info */
  dot3ad_system_t dot3adSystem; /* holds system info         */
  L7_uint32 dot3adAggIdx[L7_MAX_INTERFACE_COUNT_REL_4_0];       /* maps an aggregator interface
                                                                   number to a dot3adAgg index */
  L7_uint32 dot3adPortIdx[L7_MAX_PORT_COUNT_REL_4_0 + 1];       /* maps a port interface number
                                                                   to a dot3adPort index       */
}
dot3adCfgDataV1_t;

typedef struct
{
  L7_fileHdr_t hdr;
  dot3adCfgDataV1_t cfg;
  L7_uint32 checkSum;
}
dot3adCfgV1_t;

typedef struct
{
  dot3ad_port_t dot3adPort[L7_MAX_PORT_COUNT_REL_4_1 + 1];      /* holds per port into       */
  dot3ad_aggV4_t dot3adAgg[L7_MAX_NUM_LAG_INTF_REL_4_1];  /* holds per aggregator info */
  dot3ad_system_t dot3adSystem; /* holds system info         */
  L7_uint32 dot3adAggIdx[L7_MAX_INTERFACE_COUNT_REL_4_1];       /* maps an aggregator interface
                                                                   number to a dot3adAgg index */
  L7_uint32 dot3adPortIdx[L7_MAX_PORT_COUNT_REL_4_1 + 1];       /* maps a port interface number
                                                                   to a dot3adPort index       */
} dot3adCfgDataV2_t;

typedef struct
{
  L7_fileHdr_t hdr;
  dot3adCfgDataV2_t cfg;
  L7_uint32 checkSum;
} dot3adCfgV2_t;

typedef struct
{
  dot3ad_port_t dot3adPort[L7_MAX_PORT_COUNT_REL_4_3 + 1];
  dot3ad_aggV4_t dot3adAgg[L7_MAX_NUM_LAG_INTF_REL_4_3];
  dot3ad_system_t dot3adSystem;
  L7_uint32 dot3adAggIdx[L7_MAX_INTERFACE_COUNT_REL_4_3];
  L7_uint32 dot3adPortIdx[L7_MAX_PORT_COUNT_REL_4_3+1];
} dot3adCfgDataV3_t;

typedef struct
{
  L7_fileHdr_t hdr;
  dot3adCfgDataV3_t cfg;
  L7_uint32 checkSum;
} dot3adCfgV3_t;

typedef struct
{
  dot3ad_port_t     dot3adPort[L7_MAX_PORT_COUNT_REL_4_4 + 1];      /* holds per port into          */
  dot3ad_aggV4_t    dot3adAgg[L7_MAX_NUM_LAG_INTF_REL_4_4];         /* holds per aggregator info    */
  dot3ad_system_t   dot3adSystem;                           /* holds system info            */
  L7_uint32         dot3adAggIdx[L7_MAX_INTERFACE_COUNT_REL_4_4];   /* maps an aggregator interface
                                                               number to a dot3adAgg index  */
  L7_uint32         dot3adPortIdx[L7_MAX_PORT_COUNT_REL_4_4+1];     /* maps a port interface number
                                                               to a dot3adPort index        */ 
} dot3adCfgDataV4_t;

typedef struct
{
  L7_fileHdr_t      hdr;
  dot3adCfgDataV4_t cfg;
  L7_uint32         checkSum;
} dot3adCfgV4_t;

typedef dot3adCfg_t dot3adCfgV5_t;

/*********************************************************************
* @purpose  Build default dot3ad data for a lag interface 
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
* @param    lagId     @b{(input)}   lagId of the lag
* @param    ver       @b{(input)}   Software version of Config Data
* @param    agg       @b{(output)}  The agg port pointer to be modified
* @param    idx       @b{(output)}  The agg idx pointer to be modified
*
* @returns  L7_SUCCESS
*
* @notes    Since the runtime and config structures are the same, this routine
*           can be used to set either structure to the defaults   
*
* @end
*********************************************************************/
L7_RC_t dot3adLagIntfDefaultBuildV4(L7_uint32 intIfNum, L7_uint32 lagId, L7_uint32 ver, 
                                    dot3ad_aggV4_t *agg, L7_uint32 *idx);

#endif /* DOT3AD_MIGRATE_H */
