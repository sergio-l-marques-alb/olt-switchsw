
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   pimsm_old_config_ver1.h
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



#ifndef _PIMSM_OLD_CONFIG_VER_1_H_
#define _PIMSM_OLD_CONFIG_VER_1_H_

#include "l7_common.h"
#include "l7_pimsminclude.h"


/****************************************
*
*  PIM-SM Global Configuration Data
*
*****************************************/


typedef struct pimsmCfgDataV1_s
{
   L7_uint32   pimsmAdminMode;         /* Admin mode */
   L7_uint32   traceMode;              /* enable tracing of events */
   L7_uint32   joinPruneInterval;      /* PIM-SM join/prune interval */
   L7_uint32   dataThresholdRate;      /* minimum rate in bits/sec for the
                                          last-hop router to switch to shortest path */
   L7_uint32   regThresholdRate;       /* minimum rate in bits/sec for the
                                          RP to switch to shortest path */
} pimsmCfgDataV1_t;


/****************************************
*
*  PIM-SM Circuit Configuraton Data
*
*****************************************/

typedef struct pimsmCfgCktV1_s
{
  L7_uint32   mode;                    /* Interface Admin mode */
  L7_uint32   intfHelloInterval;       /* Hello interval per interface */
  L7_uint32   intfJoinPruneInterval;   /* Join/Prune interval per interface */
  L7_uint32   intfCBSRPreference;      /* Candidate BSR Preference  */
} pimsmCfgCktV1_t;



/****************************************
*
*  PIM-SM Configuration File
*
*****************************************/

typedef struct L7_pimsmMapCfgV1_s
{
  L7_fileHdr_t        cfgHdr;
  pimsmCfgDataV1_t    rtr;
  pimsmCfgCktV1_t     intf[L7_MAX_SLOTS_PER_UNIT][L7_MAX_PORTS_PER_SLOT+1];
  L7_uint32           checksum;
} L7_pimsmMapCfgV1_t;

#endif /* _PIMSM_OLD_CONFIG_VER_1_H_ */
