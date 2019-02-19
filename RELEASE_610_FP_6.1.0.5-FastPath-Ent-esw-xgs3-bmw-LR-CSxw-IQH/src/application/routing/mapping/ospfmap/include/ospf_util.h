
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   ospf_util.h
*
* @purpose    OSPF Mapping layer internal function prototypes
*
* @component  OSPF Mapping Layer
*
* @comments   none
*
* @create     03/20/2001
*
* @author     asuthan
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_OSPF_MAP_UTIL_H
#define INCLUDE_OSPF_MAP_UTIL_H

#include "l7_common.h"
#include "ospf_config.h"
#include "rto_api.h"
#include "acl_api.h"

/*--------------------------------------*/
/*  OSPF MAP INFO STRUCTURES            */
/*--------------------------------------*/

typedef struct ospfInfo_s     
{
    L7_BOOL         ospfInitialized;

    /* TRUE if OSPF originates a default route */
    L7_BOOL         originatingDefaultRoute;

    /* TRUE if RTO contains a default route. OSPF uses this information
     * when it determines whether to originate a default route. We cache
     * this information from best route notifies rather than doing an RTO
     * lookup to avoid potential deadlocks on acquiring the RTO lock.
     */
    L7_BOOL         defaultRouteExists;

    /* Timer that tells OSPF to request best route changes from RTO. */
    osapiTimerDescr_t *bestRouteTimer;

    /* L7_TRUE if OSPF is registered with RTO for best route changes. */
    L7_BOOL  registeredWithRto;

} ospfInfo_t;
      
typedef struct ospfAreaInfo_s     
{
    L7_uint32       areaId;
    L7_BOOL         ospfInitialized;
} ospfAreaInfo_t;

typedef struct ospfIntfInfo_s     
{
    L7_BOOL         ospfInitialized;    
    L7_BOOL         ospfIfExists;   /* Has the interface been created? */
    L7_BOOL         intfAcquired;   /* Interface has been "acquired" by another component -
                                       It is not available for OSPF */
    AcquiredMask    acquiredList; /* Mask of components "acquiring" an interface */
    L7_uint32       adminMode;        /* effective ospf admin mode on an interface */
    L7_uint32       areaId;           /* effective areaId on an interface */
    L7_uint32       secondariesFlag;  /* effective advertising flags for secondaries */
} ospfIntfInfo_t;

typedef struct ospfAreaStubInfo_s     
{
    L7_uint32       stubTOS;
    L7_uint32       stubMetric;
    L7_uint32       stubMetricType;
} ospfAreaStubInfo_t;

/* This structure holds the "network area" operational data
 * corresponding to the configured "network area"
 * The "network area" commands are maintained as a list 
 * of ospfNetworkAreaInfo_t structures in the order that
 * user creates and maintains them
 */
typedef struct ospfNetworkAreaInfo_s
{
    L7_uint32                    ipAddr;
    L7_uint32                    wildcardMask;
    L7_uint32                    areaId;   
    struct ospfNetworkAreaInfo_s *next;
} ospfNetworkAreaInfo_t;

/*--------------------------------------*/
/*  OSPF MIB SUPPORT STRUCTURES         */
/*--------------------------------------*/

/* This structure is used merely to speed get and get-next searches for area ids */
typedef struct ospfmap_areaIdSearch_s
{
  L7_IP_ADDR_t                  areaId;
  struct ospfmap_areaIdSearch_s *next;
} ospfmap_areaIdSearch_t;


/* Structure to describe a route redistributed by OSPF.
 * The structure stores the attributes needed to determine if 
 * a best route change or a configuration change actually changes the
 * attributes of the redistributed route advertised by OSPF.
 * Instances of this structure are stored in the OSPF redistribution tree.
 * destAddr and destMask MUST be the first two fields in this structure
 * in order for the redistribution list code to work properly.
 */
typedef struct ospfRedistRoute_s
{
    /* destination address of the route. Must be first attribute. */
    L7_uint32 destAddr;

    /* destination mask of the route. Must be second attribute. */
    L7_uint32 destMask;

    /* OSPF metric for the route */
    L7_uint32 metric;

    /* OSPF metric type for the route */
    L7_OSPF_EXT_METRIC_TYPES_t metricType;

    /* OSPF tag to be advertised with the route */
    L7_uint32 tag;
    
    /* forwarding address of the OSPF route */
    L7_uint32 forwardingAddr;
    L7_uint32 origNexthop;

    /* Flag used to indicated whether a route should continue to be
     * redistributed. 
     */
    L7_BOOL used;

	/* This field is needed by the AVL Library */
	void         *avlNext;

} ospfRedistRoute_t;
       


/****************************************
*
*  DEBUGGING MACROS, FUNCTION PROTOTYPES
*
*****************************************/

/* Usage Note:
   To toggle between logging and printf modes, 
   comment or uncomment the following #define.
   
#define OSPFMAP_DEBUG_LOG 

*/   
   

#ifdef OSPFMAP_DEBUG_LOG


#define OSPFMAP_TRACE(format,args...)                       \
{                                                           \
   if ( ospfMapTraceModeGet() == L7_ENABLE)                 \
   {                                                        \
      LOG_MSG(format,##args);                               \
   }                                                        \
}
      
#define OSPFMAP_ERROR(format,args...)                       \
{                                                           \
   LOG_MSG(format,##args);                                  \
}

 
#else

#define OSPFMAP_TRACE(format,args...)                       \
{                                                           \
   if ( ospfMapTraceModeGet() == L7_ENABLE)                 \
   {                                                        \
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,format,##args);  \
   }                                                        \
}
      
#define OSPFMAP_ERROR(format,args...)                           \
{                                                               \
   SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP_ERROR,format,##args);   \
}

#endif
      

/* Debugging prototypes */
void ospfMapTraceLinkerAssist(void);

      


/*
**********************************************************************
*                    FUNCTION PROTOTYPES
**********************************************************************
*/

/* ospfmap.c */

L7_RC_t ospfStartTasks(void);
void ospfEndTasks(void);

L7_RC_t ospfSave(void);
L7_BOOL ospfHasDataChanged(void);
void ospfResetDataChanged(void);
void ospfBuildDefaultConfigData(L7_uint32 ver);
void ospfIntfBuildDefaultConfigData(nimConfigID_t *configId, ospfCfgCkt_t *pCfg);
L7_RC_t ospfDefaultIfType(L7_uint32 intIfNum, L7_uint32 *pIfType);
L7_RC_t ospfApplyConfigData(void);
L7_RC_t ospfApplyVirtualLinkConfigData(void);
L7_RC_t ospfResetCleanUp(void);
L7_RC_t ospfMapAppsInit(void);
void ospfMapAclCallback(L7_uint32 aclNum, aclRouteFilter_t event);
void ospfMapAclChangeProcess(L7_uint32 aclNum, aclRouteFilter_t event);
L7_BOOL ospfMapMapIntfIsConfigurable(L7_uint32 intIfNum, ospfCfgCkt_t **pCfg);
L7_BOOL ospfMapMapIntfConfigEntryGet(L7_uint32 intIfNum, ospfCfgCkt_t **pCfg);
L7_RC_t ospfMapIntfChangeCallback (L7_uint32 intIfNum, L7_uint32 event,NIM_CORRELATOR_t correlator);
void ospfMapExtenRouteCallback();
L7_RC_t ospfMapRoutingEventChangeCallBack (L7_uint32 intIfNum, 
                                          L7_uint32 event,
                                          void *pData, 
                                          ASYNC_EVENT_NOTIFY_INFO_t *response);
L7_RC_t ospfMapIpAddressDeleteCallBack (L7_uint32 intIfNum);
L7_RC_t ospfMapSecondaryIpAddrChangeCallback(L7_uint32 intIfNum);
SYSNET_PDU_RC_t ospfMapMcastIntercept(L7_uint32 hookId,
                                      L7_netBufHandle bufHandle,
                                      sysnet_pdu_info_t *pduInfo,
                                      L7_FUNCPTR_t contFuncPtr);

L7_RC_t ospfMapDefaultRouteMetricChange();
L7_RC_t ospfMapDefaultRouteApply();
L7_BOOL ospfMapDefaultRouteExists();
L7_RC_t ospfRedistCfgInit(ospfRedistCfg_t *redistCfg);
L7_RC_t ospfDefaultRouteCfgInit(ospfDefRouteCfg_t *defRouteCfg);
void ospfProcTask( void );
void ospfMapAdminModeProcess(L7_uint32 mode);
L7_RC_t ospfMapIntfAdminModeProcess(L7_uint32 intIfNum, L7_uint32 mode);

/* ospf_util.c */

L7_RC_t ospfMapOspfAdminModeEnable(void);
L7_RC_t ospfMapOspfAdminModeDisable(void);
L7_RC_t ospfMapRouterIDApply(L7_uint32 routerID);
L7_RC_t ospfMapAsbrStatusApply(L7_uint32 mode);
L7_BOOL ospfMapIsAsbr();
L7_RC_t ospfMapRfc1583CompatibilityApply(L7_uint32 mode);
L7_RC_t ospfMapIntfMtuIgnoreApply(L7_uint32 intIfNum, L7_BOOL val);
L7_RC_t ospfMapInftIpMtuApply(L7_uint32 intIfNum, L7_uint32 ipMtu);
L7_RC_t ospfMapIntfBandwidthApply(L7_uint32 intIfNum);
L7_RC_t ospfMapExitOverflowIntervalApply ( L7_int32 val );
L7_RC_t ospfMapSpfDelayApply( L7_int32 spfDelay );
L7_RC_t ospfMapSpfHoldtimeApply( L7_int32 spfHoldtime);
L7_RC_t ospfMapPassiveModeApply(L7_BOOL passiveMode);
L7_RC_t ospfMapExtLsdbLimitApply ( L7_int32 val );
L7_RC_t ospfMapMaxPathsApply(L7_uint32 maxPaths);
L7_RC_t ospfMapAutoCostRefBwApply(void);
L7_BOOL ospfMayEnableInterface(L7_uint32 intIfNum);
L7_RC_t ospfMapNetworkAreaGet (L7_uint32 ipAddr, L7_uint32 wildcardMask,
                               L7_int32 areaId);
L7_RC_t ospfMapNetworkAreaGetNext(L7_uint32 *ipAddr, L7_uint32 *wildcardMask,
                                  L7_int32 *areaId);
L7_RC_t ospfMapNetworkAreaUpdate (L7_uint32 ipAddr,
                                  L7_uint32 wildcardMask,
                                  L7_uint32 areaId);
L7_RC_t ospfMapNetworkAreaDelete (L7_uint32 ipAddr,
                                  L7_uint32 wildcardMask,
                                  L7_uint32 areaId);
void ospfMapMatchingNetworkFind(L7_uint32 ipAddr, L7_uint32 *matchingArea, L7_BOOL *matchFound);
void ospfMapIntfSecondariesFlagCompute(L7_uint32 intIfNum,
                                       L7_uint32 matchingArea,
                                       L7_uint32 *secondaries);
L7_RC_t ospfMapIntfModeAndAreaCompute(L7_uint32 intf);
void ospfMapNetworkAreaConfigRead(void);
L7_RC_t ospfMapIntfAdminModeEnable(L7_uint32 intIfNum);
L7_RC_t ospfMapIntfAdminModeDisable(L7_uint32 intIfNum);
L7_RC_t ospfMapIntfAreaIdApply(L7_uint32 intIfNum, L7_uint32 area);
L7_RC_t ospfMapIntfSecondariesFlagApply(L7_uint32 intIfNum, L7_uint32 secondaries);
L7_RC_t ospfMapIntfTypeApply(L7_uint32 intIfNum, L7_uint32 type);
L7_RC_t ospfMapIntfPriorityApply(L7_uint32 intIfNum, L7_uint32 priority);
L7_RC_t ospfMapIntfHelloIntervalApply(L7_uint32 intIfNum, L7_uint32 seconds);
L7_RC_t ospfMapIntfDeadIntervalApply(L7_uint32 intIfNum, L7_uint32 seconds);
L7_RC_t ospfMapIntfRxmtIntervalApply(L7_uint32 intIfNum, L7_uint32 seconds);
L7_RC_t ospfMapIntfNbmaPollIntervalApply(L7_uint32 intIfNum, L7_uint32 seconds);
L7_RC_t ospfMapIntfTransitDelayApply(L7_uint32 intIfNum, L7_uint32 seconds);
L7_RC_t ospfMapIntfLsaAckIntervalApply(L7_uint32 intIfNum, L7_uint32 seconds);
L7_RC_t ospfMapIntfAuthTypeApply(L7_uint32 intIfNum, L7_uint32 authType);
L7_RC_t ospfMapIntfAuthKeyApply(L7_uint32 intIfNum, L7_uchar8 *key);
L7_RC_t ospfMapIntfAuthKeyIdApply(L7_uint32 intIfNum, L7_uint32 keyId);
L7_RC_t ospfMapIntfDemandApply(L7_int32 intIfNum, L7_int32 mode);
L7_RC_t ospfMapIntfVirtTransitAreaIdApply(L7_uint32 intIfNum, L7_uint32 area);
L7_RC_t ospfMapIntfVirtIntfNeighborApply(L7_uint32 intIfNum, L7_uint32 areaId,
                                         L7_uint32 neighbor);
L7_RC_t ospfMapIntfMetricApply(L7_uint32 intIfNum, L7_uint32 metric);
L7_RC_t ospfMapIntfPassiveModeApply(L7_uint32 intIfNum, L7_BOOL value);
L7_RC_t ospfMapAreaIsStubAreaApply(L7_uint32 areaId);
L7_RC_t ospfMapStubMetricCfgApply ( L7_uint32 areaId, L7_uint32 TOS, 
                                    L7_uint32 metric, L7_uint32 metricType );
L7_RC_t ospfMapAreaImportAsExternApply(L7_uint32 areaId, L7_int32 prevConf,
                                       L7_int32 val);
L7_RC_t ospfMapAreaSummaryApply(L7_uint32 areaId, L7_int32 val);
L7_RC_t ospfMapAreaDefaultMetricApply(L7_uint32 areaId, L7_uint32 metric);
L7_RC_t ospfMapAreaDefaultMetricTypeApply(L7_uint32 areaId, L7_uint32 metric);

L7_RC_t ospfMapAreaFreeApply( L7_uint32 area);
L7_RC_t ospfMapAreaAggregateRangeApply ( L7_uint32 areaId, L7_uint32 lsdbType, 
                                L7_uint32 ipAddr, L7_uint32 netMask, 
                                L7_uint32 advertise );
L7_RC_t ospfMapAreaAggregateRangeDeleteApply ( L7_uint32 areaId, L7_uint32 lsdbType,
                                      L7_uint32 ipAddr, L7_uint32 netMask );
L7_BOOL ospfMapIsValidSourceProto(L7_REDIST_RT_INDICES_t sourceProto);
ospfRedistCfg_t* ospfMapRedistCfgGet(L7_REDIST_RT_INDICES_t sourceProto);
L7_BOOL ospfMapIsSubnet(L7_uint32 prefix, L7_uint32 mask);
L7_RC_t ospfMapLocalRedistSuppress(L7_uint32 intIfNum);
L7_BOOL ospfIsRegisteredWithRto(void);

/* ospf_redist_list.c */
L7_RC_t ospfMapRedistListCreate(void);
L7_RC_t ospfMapRedistListDestroy(void);
L7_RC_t ospfMapRedistListAdd(ospfRedistRoute_t *redistRoute);
L7_RC_t ospfMapRedistListModify(ospfRedistRoute_t *redistRoute);
L7_RC_t ospfMapRedistListDelete(L7_uint32 destAddr, L7_uint32 destMask);
L7_RC_t ospfMapRedistListPurge(void);
L7_RC_t ospfMapRedistListPurgeUnused(void);
L7_RC_t ospfMapRedistListFind(ospfRedistRoute_t *redistRoute);
L7_RC_t ospfMapRedistListMarkUsed(L7_uint32 destAddr, L7_uint32 destMask);
L7_RC_t ospfMapRedistListMarkAllUnused(void);
L7_BOOL ospfMapRedistRouteChanged(ospfRedistRoute_t *redistRoute);
L7_RC_t ospfMapRedistListGetFirst(ospfRedistRoute_t *redistRoute);
L7_RC_t ospfMapRedistListGetNext(L7_uint32 destAddr, 
                                 L7_uint32 destMask,
                                 ospfRedistRoute_t *redistRoute);

void ospfMapBestRouteTimerCb(void);
L7_RC_t ospfMapBestRoutesGet(void);

/* ospf_mib_support.c */
L7_RC_t ospfMapAreaIdMibTblInsert(L7_IP_ADDR_t areaId);
L7_RC_t ospfMapAreaIdMibTblDelete(L7_IP_ADDR_t areaId);
L7_RC_t ospfMapAreaIdMibTblFind(L7_IP_ADDR_t *pAreaId, L7_uint32 matchType, 
                                ospfmap_areaIdSearch_t *pData);


L7_RC_t ospfMapMibSupportInit();
L7_RC_t ospfMapMibSupportFini();
L7_BOOL ospfMapAreaAggregateRangeConflictExists(L7_uint32 areaId,
                                                L7_uint32 lsdbType,
                                                L7_uint32 ipAddr,
                                                L7_uint32 netMask);

L7_RC_t ospfMapAreaAggregateRangeCfgIndexFind ( L7_uint32 areaId,
                                                       L7_uint32 lsdbType,
                                                       L7_uint32 ipAddr,
                                                       L7_uint32 netMask,
                                                       L7_uint32 matchType,
                                                       L7_uint32 *index);

/* ospf_port.c */

/* ospf_outcalls.c */
L7_BOOL ospfMapIsAclPresent(void);
L7_RC_t ospfMapAclCallbackRegister(ACL_ROUTE_FILTER_FUNCPTR_t funcPtr, L7_uchar8 *funcNameStr);
L7_RC_t ospfMapAclCallbackUnregister(ACL_ROUTE_FILTER_FUNCPTR_t funcPtr);
L7_BOOL ospfMapAclCallbackIsRegistered(ACL_ROUTE_FILTER_FUNCPTR_t funcPtr);
L7_RC_t ospfMapAclRouteFilter(L7_uint32 aclnum,
                              L7_uint32 routePrefix,
                              L7_uint32 routeMask,
                              FILTER_ACTION_t *action);



/* ospf_migrate.c */
void ospfMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);

/* ospfmap_debug.c */
void ospfMapCfgDataShow(void);
void ospfMapExtenRTOShow(void);
void ospfMapExtenIFOShow(L7_uint32 unit, L7_uint32 slot, L7_uint32 port, 
                         L7_BOOL virtFlag);
void ospfMapExtenAROShow(L7_uint32 areaId);
void ospfMapExtenNBOShow(L7_uint32 unit, L7_uint32 slot, L7_uint32 port);

#endif /* INCLUDE_OSPF_MAP_UTIL_H */

