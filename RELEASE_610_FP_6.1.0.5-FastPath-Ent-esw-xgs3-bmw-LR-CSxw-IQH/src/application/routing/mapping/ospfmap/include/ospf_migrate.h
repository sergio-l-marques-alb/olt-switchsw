
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename ospf_migrate.h
*
* @purpose OSPF Configuration Migration
*
* @component OSPF
*
* @comments none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#ifndef OSPF_MIGRATE_H
#define OSPF_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "l7_ospfinclude.h"

#define L7_OSPF_INTF_MAX_COUNT_REL_4_1 L7_IPMAP_INTF_MAX_COUNT_REL_4_1
#define L7_OSPF_INTF_MAX_COUNT_REL_4_2 L7_IPMAP_INTF_MAX_COUNT_REL_4_2
#define L7_OSPF_INTF_MAX_COUNT_REL_4_3 L7_IPMAP_INTF_MAX_COUNT_REL_4_3
#define L7_OSPF_INTF_MAX_COUNT_REL_4_4 L7_IPMAP_INTF_MAX_COUNT_REL_4_4

typedef struct
{
  L7_uint32 ospfAdminMode;      /* Enable OSPF */
  L7_uint32 rfc1583Compatibility;       /* if enabled, RFC 1583 governs */
  L7_int32 extLSALimit;         /* ExtLsdbLimit */
  L7_uint32 exitOverflowInterval;       /* seconds */
  L7_uint32 opaqueLSAMode;      /* Enable opaque LSA options  FUTURE_FUNC */
  L7_uint32 traceMode;          /* Enable tracing of events  */

  /* The default metric for OSPF. Used for redistributed routes
   * and a locally originated default route. Only a valid value if
   * defMetConfigured is true. Range is [0, 16,777,215]. */
  L7_uint32 defaultMetric;

  /* If true, a default metric has been configured. */
  L7_BOOL defMetConfigured;

}
ospfCfgDataV4_t;

typedef struct
{
  L7_uint32 adminMode;          /* Enable OSPF                  */
  L7_uint32 area;               /* Area ID                      */
  L7_uint32 priority;           /* Priority of the interface    */

  L7_uint32 helloInterval;      /* seconds                      */
  L7_uint32 deadInterval;       /* seconds                      */
  L7_uint32 rxmtInterval;       /* seconds                      */
  L7_uint32 nbmaPollInterval;   /* seconds                      */
  L7_uint32 ifTransitDelay;     /* seconds                      */

  L7_uint32 lsaAckInterval;     /* seconds                      */

  L7_uint32 authType;           /* authentication type        */
  L7_uchar8 authKey[L7_AUTH_MAX_KEY_OSPF];      /* L7_AUTH_TYPES_t         */
  L7_uint32 authKeyLen;         /* auth key length (no padding) */
  L7_uint32 authKeyId;          /* auth key identifier (for MD5) */

  L7_uint32 ifDemandMode;       /* Enable use of demand procs.  */

  L7_uint32 virtTransitAreaID;  /* Transit area for a virt. link */
  L7_uint32 virtIntfNeighbor;   /* Router ID of virt. neighbor  */
  L7_uint32 ifType;             /* Interface type (L7_OSPF_IFTYPE_t) */
  L7_uint32 metric;             /* corresponding to a speed of 10M   */
}
ospfCfgCktV4_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  ospfCfgDataV4_t rtr;
  ospfCfgCktV4_t ckt[L7_MAX_SLOTS_PER_BOX_REL_4_0][L7_MAX_PORTS_PER_SLOT_REL_4_0 + 1];
  ospfAreaCfg_t area[L7_OSPF_MAX_AREAS];
  ospfAreaRangeCfg_t areaRange[L7_OSPF_MAX_AREA_RANGES];
  ospfVirtNbrCfg_t virtNbrs[L7_OSPF_MAX_VIRT_NBRS];

  /* Redistribution configuration for each protocol from which OSPF redistributes. */
  ospfRedistCfg_t redistCfg[REDIST_RT_LAST];

  /* Configuration for OSPF's origination of the default route. */
  ospfDefRouteCfg_t defRouteCfg;

  L7_uint32 checkSum;
}
L7_ospfMapCfgV4_t;

typedef struct
{
  L7_uint32 routerID;           /* OSPF Router ID */
  L7_uint32 ospfAdminMode;      /* Enable OSPF */
  L7_uint32 rfc1583Compatibility;       /* if enabled, RFC 1583 governs */
  L7_int32 extLSALimit;         /* ExtLsdbLimit */
  L7_uint32 exitOverflowInterval;       /* seconds */
  L7_uint32 opaqueLSAMode;      /* Enable opaque LSA options  FUTURE_FUNC */
  L7_uint32 traceMode;          /* Enable tracing of events  */

  /* The default metric for OSPF. Used for redistributed routes
     and a locally originated default route. Only a valid value if
     defMetConfigured is true. Range is [0, 16,777,215]. */
  L7_uint32 defaultMetric;

  /* If true, a default metric has been configured. */
  L7_BOOL defMetConfigured;

} ospfCfgDataV5_t;

typedef struct
{
  nimConfigID_t configId;
  L7_uint32 adminMode;          /* Enable OSPF                  */
  L7_uint32 area;               /* Area ID                      */
  L7_uint32 priority;           /* Priority of the interface    */

  L7_uint32 helloInterval;      /* seconds                      */
  L7_uint32 deadInterval;       /* seconds                      */
  L7_uint32 rxmtInterval;       /* seconds                      */
  L7_uint32 nbmaPollInterval;   /* seconds                      */
  L7_uint32 ifTransitDelay;     /* seconds                      */

  L7_uint32 lsaAckInterval;     /* seconds                      */

  L7_uint32 authType;           /* authentication type        */
  L7_uchar8 authKey[L7_AUTH_MAX_KEY_OSPF];      /* L7_AUTH_TYPES_t         */
  L7_uint32 authKeyLen;         /* auth key length (no padding) */
  L7_uint32 authKeyId;          /* auth key identifier (for MD5) */

  L7_uint32 ifDemandMode;       /* Enable use of demand procs.  */

  L7_uint32 virtTransitAreaID;  /* Transit area for a virt. link */
  L7_uint32 virtIntfNeighbor;   /* Router ID of virt. neighbor  */

  /* Values in l3_commdefs.h:  L7_OSPF_INTF_BROADCAST, L7_OSPF_INTF_NBMA,
   * L7_OSPF_INTF_PTP, L7_OSPF_INTF_VIRTUAL, L7_OSPF_INTF_POINTTOMULTIPOINT.
   */
  L7_uint32 ifType;

  L7_uint32 metric;             /* corresponding to a speed of 10M   */

} ospfCfgCktV5_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  ospfCfgDataV5_t rtr;
  ospfCfgCktV5_t ckt[L7_OSPF_INTF_MAX_COUNT_REL_4_1];
  ospfAreaCfg_t area[L7_OSPF_MAX_AREAS_REL_4_1];
  ospfAreaRangeCfg_t areaRange[L7_OSPF_MAX_AREA_RANGES_REL_4_1];
  ospfVirtNbrCfg_t virtNbrs[L7_OSPF_MAX_VIRT_NBRS_REL_4_1];

  /* Redistribution configuration for each protocol from which OSPF
     redistributes. */
  ospfRedistCfg_t redistCfg[L7_OSPF_REDIST_RT_LAST_REL_4_1];

  /* Configuration for OSPF's origination of the default route. */
  ospfDefRouteCfg_t defRouteCfg;

  L7_uint32 checkSum;

} L7_ospfMapCfgV5_t;

typedef struct
{
  L7_uint32 routerID;           /* OSPF Router ID */
  L7_uint32 ospfAdminMode;      /* Enable OSPF */
  L7_uint32 rfc1583Compatibility;       /* if enabled, RFC 1583 governs */
  L7_int32 extLSALimit;         /* ExtLsdbLimit */
  L7_uint32 exitOverflowInterval;       /* seconds */
  L7_uint32 opaqueLSAMode;      /* Enable opaque LSA options  FUTURE_FUNC */
  L7_uint32 traceMode;          /* Enable tracing of events  */

  /* The default metric for OSPF. Used for redistributed routes
     and a locally originated default route. Only a valid value if
     defMetConfigured is true. Range is [0, 16,777,215]. */
  L7_uint32 defaultMetric;

  /* If true, a default metric has been configured. */
  L7_BOOL defMetConfigured;

  /* The maximum number of paths that OSPF can report for a given dest */
  L7_uint32 maxPaths;

} ospfCfgDataV6_t;

typedef struct
{
  L7_BOOL inUse;

  L7_uint32 area;               /* Area ID                      */
  L7_uint32 extRoutingCapability;       /* L7_OSPF_AREA_EXT_ROUTING_t   */
  L7_uint32 summary;            /* L7_OSPF_AREA_SUMMARY_t       */
  L7_uint32 agingInterval;      /* seconds - Read only          */

  ospfAreaStubCfg_t stubCfg[L7_RTR_MAX_TOS_INDICES];
  ospfAreaNssaCfg_t nssaCfg;

} ospfAreaCfgV6_t;

typedef struct
{
  nimConfigID_t       configId;
  L7_uint32           adminMode;                /* OSPF adminMode configured on the interface */
  L7_uint32           area;                     /* Area ID Configured on the interface */
  L7_uint32           priority;                 /* Priority of the interface    */
  L7_uint32           maxIpMtu;                 /* Max ip mtu for this interface*/
  L7_BOOL             mtuIgnore;                /* mtu ignore flag for this interface*/

  L7_uint32           helloInterval;            /* seconds                      */
  L7_uint32           deadInterval;             /* seconds                      */
  L7_uint32           rxmtInterval;             /* seconds                      */
  L7_uint32           nbmaPollInterval;         /* seconds                      */
  L7_uint32           ifTransitDelay;           /* seconds                      */

  L7_uint32           lsaAckInterval;           /* seconds                      */

  L7_uint32           authType;                 /* authentication type        */
  L7_uchar8           authKey[L7_AUTH_MAX_KEY_OSPF]; /* L7_AUTH_TYPES_t         */
  L7_uint32           authKeyLen;               /* auth key length (no padding) */
  L7_uint32           authKeyId;                /* auth key identifier (for MD5)*/

  L7_uint32           ifDemandMode;             /* Enable use of demand procs.  */

  L7_uint32           virtTransitAreaID;        /* Transit area for a virt. link*/
  L7_uint32           virtIntfNeighbor;         /* Router ID of virt. neighbor  */

  /* Values in l3_commdefs.h:  L7_OSPF_INTF_BROADCAST, L7_OSPF_INTF_NBMA,
   * L7_OSPF_INTF_PTP, L7_OSPF_INTF_VIRTUAL, L7_OSPF_INTF_POINTTOMULTIPOINT. 
   */  
  L7_uint32           ifType;                   
  
  L7_uint32           metric;                   /* corresponding to a speed of 10M   */
} ospfCfgCktV6_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  ospfCfgDataV6_t rtr;
  ospfCfgCktV6_t ckt[L7_OSPF_INTF_MAX_COUNT_REL_4_1];
  ospfAreaCfgV6_t area[L7_OSPF_MAX_AREAS];
  ospfAreaRangeCfg_t areaRange[L7_OSPF_MAX_AREA_RANGES];
  ospfVirtNbrCfg_t virtNbrs[L7_OSPF_MAX_VIRT_NBRS];

  /* Redistribution configuration for each protocol from which OSPF
     redistributes. */
  ospfRedistCfg_t redistCfg[REDIST_RT_LAST];

  /* Configuration for OSPF's origination of the default route. */
  ospfDefRouteCfg_t defRouteCfg;

  L7_uint32 checkSum;

} L7_ospfMapCfgV6_t;

typedef struct
{
  nimConfigID_t       configId;
  L7_uint32           adminMode;                /* OSPF adminMode configured on the interface */
  L7_uint32           area;                     /* Area ID Configured on the interface */
  L7_uint32           priority;                 /* Priority of the interface    */
  L7_uint32           maxIpMtu;                 /* Max ip mtu for this interface*/
  L7_BOOL             mtuIgnore;                /* mtu ignore flag for this interface*/

  L7_uint32           helloInterval;            /* seconds                      */
  L7_uint32           deadInterval;             /* seconds                      */
  L7_uint32           rxmtInterval;             /* seconds                      */
  L7_uint32           nbmaPollInterval;         /* seconds                      */
  L7_uint32           ifTransitDelay;           /* seconds                      */

  L7_uint32           lsaAckInterval;           /* seconds                      */

  L7_uint32           authType;                 /* authentication type        */
  L7_uchar8           authKey[L7_AUTH_MAX_KEY_OSPF]; /* L7_AUTH_TYPES_t         */
  L7_uint32           authKeyLen;               /* auth key length (no padding) */
  L7_uint32           authKeyId;                /* auth key identifier (for MD5)*/

  L7_uint32           ifDemandMode;             /* Enable use of demand procs.  */

  L7_uint32           virtTransitAreaID;        /* Transit area for a virt. link*/
  L7_uint32           virtIntfNeighbor;         /* Router ID of virt. neighbor  */

  /* Values in l3_commdefs.h:  L7_OSPF_INTF_BROADCAST, L7_OSPF_INTF_NBMA,
   * L7_OSPF_INTF_PTP, L7_OSPF_INTF_VIRTUAL, L7_OSPF_INTF_POINTTOMULTIPOINT. 
   */  
  L7_uint32           ifType;                   
  
  L7_uint32           metric;                   /* corresponding to a speed of 10M   */
} ospfCfgCktV7_t;

typedef struct ospfCfgDataV7_s
{
  L7_uint32  routerID;                 /* OSPF Router ID */
  L7_uint32  ospfAdminMode;            /* Enable OSPF */
  L7_uint32  rfc1583Compatibility;     /* if enabled, RFC 1583 governs */
  L7_int32   extLSALimit;              /* ExtLsdbLimit */
  L7_uint32  exitOverflowInterval;     /* seconds */
  L7_uint32  opaqueLSAMode;            /* Enable opaque LSA options  FUTURE_FUNC */
  L7_uint32  traceMode;                /* Enable tracing of events  */        

  /* The default metric for OSPF. Used for redistributed routes
    and a locally originated default route. Only a valid value if
    defMetConfigured is true. Range is [0, 16,777,215]. */
  L7_uint32  defaultMetric;

  /* If true, a default metric has been configured. */
  L7_BOOL    defMetConfigured;

  /* The maximum number of paths that OSPF can report for a given dest */
  L7_uint32  maxPaths;

} ospfCfgDataV7_t;     

/* Before addition of REDIST_RT_OSPFV3 */
#define REDIST_RT_LAST_V7 6

typedef struct L7_ospfMapCfgV7_s 
{
  L7_fileHdr_t        cfgHdr;
  ospfCfgDataV7_t     rtr;
  ospfCfgCktV7_t      ckt[L7_OSPF_INTF_MAX_COUNT_REL_4_3];
  ospfAreaCfg_t       area[L7_OSPF_MAX_AREAS];
  ospfAreaRangeCfg_t  areaRange[L7_OSPF_MAX_AREA_RANGES];
  ospfVirtNbrCfg_t    virtNbrs[L7_OSPF_MAX_VIRT_NBRS];

  /* Redistribution configuration for each protocol from which OSPF
    redistributes. */
  ospfRedistCfg_t     redistCfg[REDIST_RT_LAST_V7];

  /* Configuration for OSPF's origination of the default route. */
  ospfDefRouteCfg_t   defRouteCfg;

  L7_uint32           checkSum;

} L7_ospfMapCfgV7_t;

typedef struct
{
  nimConfigID_t       configId;
  L7_uint32           adminMode;                /* OSPF adminMode configured on the interface */
  L7_uint32           area;                     /* Area ID Configured on the interface */
  L7_uint32           priority;                 /* Priority of the interface    */
  L7_uint32           maxIpMtu;                 /* Max ip mtu for this interface*/
  L7_BOOL             mtuIgnore;                /* mtu ignore flag for this interface*/

  L7_uint32           helloInterval;            /* seconds                      */
  L7_uint32           deadInterval;             /* seconds                      */
  L7_uint32           rxmtInterval;             /* seconds                      */
  L7_uint32           nbmaPollInterval;         /* seconds                      */
  L7_uint32           ifTransitDelay;           /* seconds                      */

  L7_uint32           lsaAckInterval;           /* seconds                      */

  L7_uint32           authType;                 /* authentication type        */
  L7_uchar8           authKey[L7_AUTH_MAX_KEY_OSPF]; /* L7_AUTH_TYPES_t         */
  L7_uint32           authKeyLen;               /* auth key length (no padding) */
  L7_uint32           authKeyId;                /* auth key identifier (for MD5)*/

  L7_uint32           ifDemandMode;             /* Enable use of demand procs.  */

  L7_uint32           virtTransitAreaID;        /* Transit area for a virt. link*/
  L7_uint32           virtIntfNeighbor;         /* Router ID of virt. neighbor  */

  /* Values in l3_commdefs.h:  L7_OSPF_INTF_BROADCAST, L7_OSPF_INTF_NBMA,
   * L7_OSPF_INTF_PTP, L7_OSPF_INTF_VIRTUAL, L7_OSPF_INTF_POINTTOMULTIPOINT. 
   */  
  L7_uint32           ifType;                   
  
  L7_uint32           metric;                   /* corresponding to a speed of 10M   */
} ospfCfgCktV8_t;

typedef struct ospfCfgDataV8_s
{
  L7_uint32  routerID;                 /* OSPF Router ID */
  L7_uint32  ospfAdminMode;            /* Enable OSPF */
  L7_uint32  rfc1583Compatibility;     /* if enabled, RFC 1583 governs */
  L7_int32   extLSALimit;              /* ExtLsdbLimit */
  L7_uint32  exitOverflowInterval;     /* seconds */
  L7_uint32  spfDelay;                 /* seconds */
  L7_uint32  spfHoldtime;              /* seconds */
  L7_uint32  opaqueLSAMode;            /* Enable opaque LSA options  FUTURE_FUNC */
  L7_uint32  traceMode;                /* Enable tracing of events  */

  /* The default metric for OSPF. Used for redistributed routes
     and a locally originated default route. Only a valid value if
     defMetConfigured is true. Range is [0, 16,777,215]. */
  L7_uint32  defaultMetric;

  /* If true, a default metric has been configured. */
  L7_BOOL    defMetConfigured;

  /* The maximum number of paths that OSPF can report for a given dest */
  L7_uint32  maxPaths;
  

} ospfCfgDataV8_t;

typedef struct L7_ospfMapCfgV8_s
{
  L7_fileHdr_t        cfgHdr;
  ospfCfgDataV8_t     rtr;
  ospfCfgCktV8_t      ckt[L7_OSPF_INTF_MAX_COUNT_REL_4_4];
  ospfAreaCfg_t       area[L7_OSPF_MAX_AREAS];
  ospfAreaRangeCfg_t  areaRange[L7_OSPF_MAX_AREA_RANGES];
  ospfVirtNbrCfg_t    virtNbrs[L7_OSPF_MAX_VIRT_NBRS];

  /* Redistribution configuration for each protocol from which OSPF
     redistributes. */
  ospfRedistCfg_t     redistCfg[REDIST_RT_LAST_REL_4_4];

  /* Configuration for OSPF's origination of the default route. */
  ospfDefRouteCfg_t   defRouteCfg;

  L7_uint32           checkSum;

} L7_ospfMapCfgV8_t;


typedef L7_ospfMapCfg_t L7_ospfMapCfgV9_t;

#endif /* OSPF_MIGRATE_H */
