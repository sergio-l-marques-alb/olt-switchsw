
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   ospf_config.h
*
* @purpose    OSPF Mapping layer structures and config data
*
* @component  OSPF Mapping Layer
*
* @comments   none
*
* @create     03/18/2001
*
* @author     asuthan/wjacobs
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/

#ifndef INCLUDE_OSPF_MAP_CONFIG_H
#define INCLUDE_OSPF_MAP_CONFIG_H

#include "l7_common.h"
#include "l3_commdefs.h"
#include "l3_defaultconfig.h"
#include "l3_default_cnfgr.h"

/* Enumeration to specify the OSPF configuration version. */
typedef enum
{
    L7_OSPF_CFG_VER_1 = 1,
    L7_OSPF_CFG_VER_2,
    L7_OSPF_CFG_VER_3,
    L7_OSPF_CFG_VER_4,     /* release G */
    L7_OSPF_CFG_VER_5,
    L7_OSPF_CFG_VER_6,
    L7_OSPF_CFG_VER_7,     /* release I */
    L7_OSPF_CFG_VER_8,     /* release K */
    L7_OSPF_CFG_VER_9      /* release L */
} L7_ospf_version_t;
                                                                                                                                                             
#define L7_OSPF_CFG_FILENAME      "ospfMap.cfg"
#define L7_OSPF_CFG_VER_CURRENT   L7_OSPF_CFG_VER_9
#define NUM_NETWORK_AREA_CMDS     L7_RTR_MAX_RTR_INTERFACES * 2

/****************************************
*
*  OSPF Global Configuration Data                
*
*****************************************/

typedef struct ospfCfgData_s
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

  /* If true, ospf enabled interface is passive by default */
  L7_BOOL    defaultPassiveMode;
  
  /* The maximum number of paths that OSPF can report for a given dest */
  L7_uint32  maxPaths;
  
  /* The auto cost reference bandwitdh to provide flexible control over OSPF link costs */
  L7_uint32  autoCostRefBw;           /* Mbps */

} ospfCfgData_t;     


/****************************************
*
*  OSPF Circuit Configuration Data         
*
*****************************************/

typedef struct ospfCfgCkt_s
{
  nimConfigID_t       configId;
  L7_uint32           adminMode;                /* OSPF adminMode configured on the interface */
  L7_uint32           area;                     /* Area ID Configured on the interface */
  L7_uint32           priority;                 /* Priority of the interface    */
  L7_uint32           maxIpMtu;                 /* no longer used. Get IP MTU from IP MAP. */
  L7_BOOL             mtuIgnore;                /* mtu ignore flag for this interface*/
  L7_uint32           advertiseSecondaries;     /* flag for advertising the secondary addresses */

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
  L7_uint32           passiveMode;              /* if the interface is passive or not */

} ospfCfgCkt_t;

/********************************************
* 
* OSPF Virtual Neighbor Configuration Data
*
********************************************/

typedef struct ospfVirtNbrCfg_s
{   
  L7_uint32           virtTransitAreaID;        /* Transit area for a virt. link*/
  L7_uint32           virtIntfNeighbor;         /* Router ID of virt. neighbor  */
  L7_uint32           intIfNum;                 /* Associated interface number */
  L7_uint32           helloInterval;
  L7_uint32           deadInterval;
  L7_uint32           rxmtInterval;
  L7_uint32           ifTransitDelay;
  L7_uint32           authType;                 /* authentication type        */
  L7_uchar8           authKey[L7_AUTH_MAX_KEY_OSPF];
  L7_uint32           authKeyLen;               /* auth key length (no padding) */
  L7_uint32           authKeyId;                /* auth key identifier (for MD5)*/

} ospfVirtNbrCfg_t;

/****************************************
*
*  OSPF Area Configuration Data         
*
*****************************************/

typedef struct ospfAreaRangeCfg_s
{
  L7_uint32           area;                     /* Area ID                      */
  L7_uint32           lsdbType;                 /* L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t */
  L7_uint32           ipAddr;                   /* IP Address                   */
  L7_uint32           subnetMask;               /* Subnet Mask                  */
  L7_uint32           advertise;                /* L7_TRUE, if advertise        */
  L7_BOOL             inUse;                  

} ospfAreaRangeCfg_t;


typedef struct ospfAreaStubCfg_s
{
  L7_BOOL             inUse;                  
  /* Stub area configuration parms */
  L7_uint32           stubMetric;            /* metric                         */
  L7_uint32           stubMetricType;        /* L7_OSPF_STUB_METRIC_TYPE_t   */

} ospfAreaStubCfg_t;


/* NSSA area configuration parms */
typedef struct ospfAreaNssaCfg_s
{
  L7_uint32           translatorRole;              /* NSSA Translator Role (L7_OSPF_NSSA_TRANSLATOR_ROLE_t) */
  L7_uint32           translatorStabilityInterval; /* Defines the length of time an elected Type-7 translator will
                                                   ** continue to perform its translator duties once it has
                                                   ** determined that its translator status has been deposed by
                                                   ** another NSSA border router translator as described in Section
                                                   ** 3.1 and 3.3.  The default setting is 40 seconds.
                                                   */
  L7_uint32           importSummaries;        /* L7_ENABLE/ L7_DISABLE  (When set to enabled, OSPF's summary 
                                              ** routes are imported into the NSSA as Type-3 summary-LSAs.  
                                              ** When set to disabled, summary routes are not imported 
                                              ** into the NSSA.  The default setting is enabled)
                                              */
  L7_uint32           redistribute;           /* L7_ENABLE/ L7_DISABLE (Used when the router is a NSSA ABR 
                                              ** and you want the redistribute command to import routes only 
                                              ** into the normal areas, but not into the NSSA area)
                                              */
  L7_BOOL             defInfoOrig;            /* L7_TRUE/ L7_FALSE (Used to generate a Type 7 default into 
                                              ** the NSSA area. This argument takes effect only on a NSSA ABR)
                                              */
  L7_uint32           defMetric;              /* Metric value for the default route */
  L7_OSPF_STUB_METRIC_TYPE_t  defMetricType;  /* Metric type for the default route */

} ospfAreaNssaCfg_t;

typedef struct ospfAreaCfg_s
{

  L7_BOOL             inUse;  

  L7_uint32           area;                     /* Area ID                      */
  L7_uint32           extRoutingCapability;     /* L7_OSPF_AREA_EXT_ROUTING_t   */
  L7_uint32           summary;                  /* L7_OSPF_AREA_SUMMARY_t       */

  /* agingInterval is no longer used. Leaving to avoid config migration work. */
  L7_uint32           agingInterval;            /* deprecated  */

  L7_uint32           authType;                 /* Area authentication type for */
                                                /*     RFC 1583 support         */
                                                
  ospfAreaStubCfg_t   stubCfg[L7_RTR_MAX_TOS_INDICES];
  ospfAreaNssaCfg_t   nssaCfg;
 
} ospfAreaCfg_t;

/* Instances of this structure describe the redistribution 
    configuration from a specific source protocol. The source
    protocol is determined by the index for this instance in OSPF's
    redistCfg array. */
typedef struct ospfRedistCfg_s
{
    /* true if this entry in the redistCfg[] array is active. */
    L7_BOOL redistribute;

    /* Access list number identifying distribute list used to filter
        routes OSPF redistributes from other protocols. 
        0 indicates no distribute list is configured. */
    L7_uint32 distList;

    /* Routes OSPF redistributes from this protocol will be advertised
        with this metric. -1 indicates no metric is configured. 
        If configured, range is [0, 16,777,215]. */
    L7_int32 redistMetric;

    /* Routes OSPF redistributes from this protocol will be advertised
        with this metric type. External type 2 by default. */
    L7_OSPF_EXT_METRIC_TYPES_t metType;

    /* Routes OSPF redistributes from this protocol will be advertised
        with this tag. Defaults to 0. Range is [0, 2^32 - 1]. */
    L7_uint32 tag;

    /* If true, OSPF will redistribute subnetted routes from this 
        protocol. False by default. */
    L7_BOOL subnets;

} ospfRedistCfg_t;

/* This structure stores configuration parameters related to 
    origination of a default route. */
typedef struct ospfDefRouteCfg_s
{
    /* If true, OSPF is configured to originate a default route. */
    L7_BOOL origDefRoute;

    /* If false, OSPF only originates a default route if a default 
        route is in the forwarding table. */
    L7_BOOL always;

    /* The metric for the default route. -1 if the user has 
        not configured. If configured, range is [1, 16,777,215]. */
    L7_int32 metric;

    /* The metric type for the default route. */
    L7_OSPF_EXT_METRIC_TYPES_t metType;

} ospfDefRouteCfg_t;

/****************************************
*
*  OSPF Network Area Configuration Data         
*
*****************************************/

/* This structure stores configuration parameters related to
   network and its ospf area */
typedef struct ospfNetworkCfg_s
{
    L7_uint32       ipAddress;      /* IP Address    */
    L7_uint32       wildcardMask;   /* IP-address mask that includes "don't care" bits */
    L7_uint32       areaId;         /* Area Id for this network */
} ospfNetworkCfg_t;

/****************************************
*
*  OSPF Configuration File                
*
*****************************************/

typedef struct L7_ospfMapCfg_s 
{
  L7_fileHdr_t        cfgHdr;
  ospfCfgData_t       rtr;
  ospfCfgCkt_t        ckt[L7_OSPF_INTF_MAX_COUNT];
  ospfAreaCfg_t       area[L7_OSPF_MAX_AREAS];
  ospfAreaRangeCfg_t  areaRange[L7_OSPF_MAX_AREA_RANGES];
  /* we support maximum of L7_RTR_MAX_RTR_INTERFACES * 2
   * "network" commands on the router.
   */
  ospfNetworkCfg_t    networkArea[NUM_NETWORK_AREA_CMDS];
  ospfVirtNbrCfg_t    virtNbrs[L7_OSPF_MAX_VIRT_NBRS];

  /* Redistribution configuration for each protocol from which OSPF
    redistributes. */
  ospfRedistCfg_t     redistCfg[REDIST_RT_LAST];

  /* Configuration for OSPF's origination of the default route. */
  ospfDefRouteCfg_t   defRouteCfg;

  L7_uint32           checkSum;

}L7_ospfMapCfg_t;

#define OSPFMAP_DEBUG_CFG_FILENAME "ospf_debug.cfg"
#define OSPFMAP_DEBUG_CFG_VER_1          0x1
#define OSPFMAP_DEBUG_CFG_VER_CURRENT    OSPFMAP_DEBUG_CFG_VER_1

typedef struct ospfMapDebugCfgData_s 
{
  L7_BOOL ospfMapDebugPacketTraceFlag;
} ospfMapDebugCfgData_t;

typedef struct ospfMapDebugCfg_s
{
  L7_fileHdr_t          hdr;
  ospfMapDebugCfgData_t  cfg;
  L7_uint32             checkSum;
} ospfMapDebugCfg_t;

L7_RC_t ospfMapDebugSave(void);
L7_RC_t ospfMapDebugRestore(void);
L7_BOOL ospfMapDebugHasDataChanged(void);
void ospfMapDebugBuildDefaultConfigData(L7_uint32 ver);
L7_RC_t ospfMapApplyDebugConfigData(void);

#endif /* INCLUDE_OSPF_MAP_CONFIG_H */
