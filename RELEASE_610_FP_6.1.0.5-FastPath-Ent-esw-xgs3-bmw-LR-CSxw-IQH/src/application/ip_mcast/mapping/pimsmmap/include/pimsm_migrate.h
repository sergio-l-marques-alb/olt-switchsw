
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename pimsm_migrate.h
*
* @purpose pimsm Configuration Migration
*
* @component pimsm
*
* @comments none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#ifndef PIMSM_MIGRATE_H
#define PIMSM_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "l7_pimsminclude.h"
#include "pimsm_map_cfg.h"

#define L7_PIMSM_MAX_STATIC_RP_NUM_REL_4_0        5        
#define L7_PIMSM_MAX_STATIC_RP_NUM_REL_4_1        5        
#define L7_PIMSM_MAX_STATIC_RP_NUM_REL_4_2        5        
#define L7_PIMSM_MAX_STATIC_RP_NUM_REL_4_3        5
#define L7_PIMSM_MAX_STATIC_RP_NUM_REL_4_4        5
        

typedef struct
{
  L7_uint32 mode;               /* Interface Admin mode */
  L7_uint32 intfHelloInterval;  /* Hello interval per interface */
  L7_uint32 intfJoinPruneInterval;      /* Join/Prune interval per interface */
  L7_int32 intfCBSRPreference;  /* Candidate BSR Preference  */

  /* The following elements are present in base REL-G */
  /* L7_int32 intfCBSRHashMskLen; */  /* Candidate BSR Hash mask Length  */
  /* L7_int32 intfCRPPreference; */   /* Candidate CRP Preference  */
}
pimsmCfgCktV2_t;

typedef struct pimsmCfgStaticRP1_s
{
  L7_uint32       rpIpAddr;               /* Ip address of rp. */
  L7_uint32       rpGrpAddr;              /* Group address range 
                                          supported by RP. */
  L7_uint32       rpGrpMask;
}pimsmCfgStaticRP1_t;


typedef struct pimsmCfgDataV1_s
{
   L7_uint32   pimsmAdminMode;         /* Admin mode */
   L7_uint32   traceMode;              /* Not used in rel-L implementation  */   
   L7_uint32   joinPruneInterval;      /* Not used in rel-L implementation */
   L7_uint32   dataThresholdRate;      /* minimum rate in bits/sec **
                                       ** for the last-hop router  **
                                        to switch to shortest path */
   L7_uint32   regThresholdRate;       /* minimum rate in bits/sec for the
                                          RP to switch to shortest path */
   pimsmCfgStaticRP1_t   pimsmStaticRP[L7_MAX_STATIC_RP_NUM]; /*Added to make it consistent with rel-k */

} pimsmCfgDataV1_t;

typedef struct pimsmCfgCktV5_s
{
  nimConfigID_t configId;              /* interface configuration correlator */
  L7_uint32   mode;                    /* Interface Admin mode */
  L7_uint32   intfHelloInterval;       /* Hello interval per interface */
  L7_uint32   intfJoinPruneInterval;   /* Join/Prune interval per interface */
  L7_int32    intfCBSRPreference;      /* Not used in rel-L implementation  */
  L7_int32    intfCBSRHashMskLen;      /* Not used in rel-L implementation    */
  L7_int32    intfCRPPreference;       /* Not used in rel-L implementation    */
} pimsmCfgCktV5_t;


typedef struct
{
  L7_fileHdr_t cfgHdr;
  pimsmCfgDataV1_t rtr;
  pimsmCfgCktV2_t intf[L7_MAX_SLOTS_PER_BOX_REL_4_0][L7_MAX_PORTS_PER_SLOT_REL_4_0 + 1];
  L7_uint32 checksum;
}
L7_pimsmMapCfgV2_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  pimsmCfgDataV1_t rtr;
  pimsmCfgCktV5_t intf[L7_IPMAP_INTF_MAX_COUNT_REL_4_1];
  L7_uint32 checksum;
} L7_pimsmMapCfgV3_t;
                                            
typedef struct
{
  L7_fileHdr_t        cfgHdr;
  pimsmCfgDataV1_t      rtr;
  pimsmCfgCktV5_t       intf[L7_IPMAP_INTF_MAX_COUNT_REL_4_3];
  L7_uint32           checksum;
} L7_pimsmMapCfgV4_t;


typedef  struct
{
  L7_fileHdr_t        cfgHdr;
  pimsmCfgDataV1_t      rtr;
  pimsmCfgCktV5_t       intf[L7_IPMAP_INTF_MAX_COUNT_REL_4_4];
  L7_uint32           checksum;
} L7_pimsmMapCfgV5_t;

typedef L7_pimsmMapCfg_t L7_pimsmMapCfgV6_t; 

/*********************************************************************
* @purpose  Migrate old Pimsm config data format to new
*
* @param    oldVer      @b{(input)} version of old config definition
* @param    ver         @b{(input)} version of current config definition
* @param    newCfgBuf   @b{(input)} new config buffer location
*
* @returns  void
*
* @comments This is the callback function provided to the nvstore routine
*           to handle cases where the config file is of an older version.
*
* @comments Currently only handles back level configuration version 1.
*           In the event that the stored configuration file is older
*           than version 1, the configuration will revert back to factory
*           defaults.
*
* @end
*********************************************************************/
void pimsmMigrateConfigData(L7_uint32 oldVer, L7_uint32 ver, 
                            L7_char8 *newCfgBuf);
void pimsm6MigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, 
                            L7_char8 * pCfgBuffer);

#endif /* PIMSM_MIGRATE_H */
