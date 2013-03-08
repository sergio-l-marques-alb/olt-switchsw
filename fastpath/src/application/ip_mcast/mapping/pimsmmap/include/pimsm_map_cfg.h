/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   pimsm_map_cfg.h
*
* @purpose    PIM-SM Mapping layer internal function prototypes
*
* @component  PIM-SM Mapping Layer
*
* @comments   none
*
* @create     13/10/2006
*
* @author    gkiran/dsatyanarayana
* @end
*
**********************************************************************/


#ifndef _PIMSM_MAP_CFG_H_
#define _PIMSM_MAP_CFG_H_
#include "l3_default_cnfgr.h"
#include "async_event_api.h"
#include "mfc_api.h"
#include "l7_mgmd_api.h"
#include "pimsmuictrl.h"

#define L7_MAX_STATIC_RP_NUM            L7_PIMSM_MAX_STATIC_RP_NUM
#define L7_MAX_SSM_RANGE                L7_PIMSM_MAX_SSM_RANGE
#define L7_MAX_CAND_RP_NUM              L7_PIMSM_MAX_CAND_RP_NUM



#define L7_PIMSM_CFG_FILENAME   "pimsmMap.cfg"
#define L7_PIMSM6_CFG_FILENAME   "pimsmMap6.cfg"
#define L7_PIMSM_CFG_VER_1       0x1
#define L7_PIMSM_CFG_VER_2       0x2
#define L7_PIMSM_CFG_VER_3       0x3
#define L7_PIMSM_CFG_VER_4       0x4
#define L7_PIMSM_CFG_VER_5       0x5
#define L7_PIMSM_CFG_VER_6       0x6
#define L7_PIMSM_CFG_VER_CURRENT L7_PIMSM_CFG_VER_6

/****************************************
*
*  PIM-SM Global Configuration Data
*
*****************************************/
typedef struct pimsmCfgStaticRP_s
{
  L7_inet_addr_t rpIpAddr;               /* Ip address of rp. */
  L7_inet_addr_t rpGrpAddr;              /* Group address range 
                                          supported by RP. */
  L7_inet_addr_t rpGrpMask;
  L7_BOOL        overRide;               /* overRide */
}pimsmCfgStaticRP_t;

typedef struct pimsmCfgSsmRange_s
{
  L7_inet_addr_t groupAddr;               /* SSM Group Address. */
  L7_uchar8      prefixLen;               /* Prefix Length      */
  L7_uint32      ssmMode;                 /* ssm Mode           */
}pimsmCfgSsmRange_t;

typedef struct pimsmCfgCandBSRdata_s {
   L7_uint32 intIfNum;
   L7_uint32         candBSRPriority;       
   L7_uchar8         candBSRHashMaskLength;
   L7_uint32      candBSRScope; 
} pimsmCfgCandBSRdata_t;

typedef struct pimsmCfgCandRPdata_s {
   L7_uint32 intIfNum;
   L7_inet_addr_t grpAddress;
   L7_uint32      grpPrefixLength;
} pimsmCfgCandRPdata_t;

typedef struct pimsmCfgData_s
{
   L7_uint32   pimsmAdminMode;         /* Admin mode */
   L7_uint32   traceMode;              /* Not used in rel-L implementation  */   
   L7_uint32   joinPruneInterval;      /* Not used in rel-L implementation */
   L7_uint32   dataThresholdRate;      /* minimum rate in bits/sec **
                                       ** for the last-hop router  **
                                        to switch to shortest path */
   L7_uint32   regThresholdRate;       /* minimum rate in bits/sec for the
                                          RP to switch to shortest path */
   pimsmCfgStaticRP_t     pimsmStaticRP[L7_MAX_STATIC_RP_NUM];
                                       /* Static rp config info. */
   pimsmCfgSsmRange_t     pimsmSsmRange[L7_MAX_SSM_RANGE];
                                       /* Ssm Range  Table*/
   pimsmCfgCandBSRdata_t  candidateBsr;   /* Candidate Bsr Table */
   pimsmCfgCandRPdata_t   pimsmCandidateRp[L7_MAX_CAND_RP_NUM];
                                       /* Cand-RP Table */
   L7_uint32   familyType;   
} pimsmCfgData_t;

/****************************************
*
*  PIM-SM Circuit Configuraton Data
*
*****************************************/

typedef struct pimsmCfgCkt_s
{
  nimConfigID_t configId;              /* interface configuration correlator */
  L7_uint32   mode;                    /* Interface Admin mode */
  L7_uint32   intfHelloInterval;       /* Hello interval per interface */
  L7_uint32   intfJoinPruneInterval;   /* Join/Prune interval per interface */
  L7_int32    intfCBSRPreference;      /* Not used in rel-L implementation  */
  L7_int32    intfCBSRHashMskLen;      /* Not used in rel-L implementation    */
  L7_int32    intfCRPPreference;       /* Not used in rel-L implementation    */
  L7_uint32   bsrBorderMode;           /* BSR Border Mode */
  L7_uint32   drPriority;              /* DR Priority. */
} pimsmCfgCkt_t;

/****************************************
*
*  PIM-SM Configuration File
*
*****************************************/

typedef struct L7_pimsmMapCfg_s
{
  L7_fileHdr_t        cfgHdr;
  pimsmCfgData_t      rtr;
  pimsmCfgCkt_t       intf[L7_IPMAP_INTF_MAX_COUNT];
  L7_uint32           checksum;
} L7_pimsmMapCfg_t;

#endif /* _PIMSM_MAP_CFG_H_ */
