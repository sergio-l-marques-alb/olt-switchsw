/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   pimsm_config.h
*
* @purpose    PIM-SM Mapping layer structures and configuration data
*
* @component  PIM-SM Mapping Layer
*
* @comments   none
*
* @create     03/07/2002
*
* @author     Ratnakar
* @end
*
**********************************************************************/



#ifndef _PIMSM_OLD_CONFIG_VER_2_H_
#define _PIMSM_OLD_CONFIG_VER_2_H_

#include "l7_common.h"
#include "l7_pimsminclude.h"


/****************************************
*
*  PIM-SM Global Configuration Data
*
*****************************************/

typedef struct pimsmCfgStaticRPV2_s
{
  L7_IP_ADDR_t rpIpAddr;               /* Ip address of rp. */
  L7_IP_ADDR_t rpGrpAddr;              /* Group address range supported by RP. */
  L7_IP_MASK_t rpGrpMask;
}pimsmCfgStaticRPV2_t;

typedef struct pimsmCfgDataV2_s
{
   L7_uint32   pimsmAdminMode;         /* Admin mode */
   L7_uint32   traceMode;              /* enable tracing of events */
   L7_uint32   joinPruneInterval;      /* PIM-SM join/prune interval */
   L7_uint32   dataThresholdRate;      /* minimum rate in bits/sec for the
                                          last-hop router to switch to shortest path */
   L7_uint32   regThresholdRate;       /* minimum rate in bits/sec for the
                                          RP to switch to shortest path */
   pimsmCfgStaticRPV2_t  pimsmStaticRP[L7_MAX_STATIC_RP_NUM];
                                       /* Static rp config info. */
} pimsmCfgDataV2_t;


/****************************************
*
*  PIM-SM Circuit Configuraton Data
*
*****************************************/

typedef struct pimsmCfgCktV2_s
{
  L7_uint32   mode;                    /* Interface Admin mode */
  L7_uint32   intfHelloInterval;       /* Hello interval per interface */
  L7_uint32   intfJoinPruneInterval;   /* Join/Prune interval per interface */
  L7_uint32   intfCBSRPreference;      /* Candidate BSR Preference  */
} pimsmCfgCktV2_t;



/****************************************
*
*  PIM-SM Configuration File
*
*****************************************/

typedef struct L7_pimsmMapCfgV2_s
{
  L7_fileHdr_t          cfgHdr;
  pimsmCfgDataV2_t      rtr;
  pimsmCfgCktV2_t       intf[L7_MAX_SLOTS_PER_BOX][L7_MAX_PORTS_PER_SLOT+1];
  L7_uint32             checksum;
} L7_pimsmMapCfgV2_t;

#endif /* _PIMSM_OLD_CONFIG_VER_2_H_ */

