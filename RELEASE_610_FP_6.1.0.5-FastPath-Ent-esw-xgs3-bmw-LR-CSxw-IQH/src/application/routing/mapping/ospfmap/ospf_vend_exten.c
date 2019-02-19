/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  ospf_vend_exten.c
*
* @purpose   OSPF Routing vendor-specific functions
*
* @component Ospf Mapping Layer
*
* @comments  none
*
* @create    04/12/2001
*
* @author    anayar
*
* @end
*
**********************************************************************/

#include "ospf_vend_ctrl.h"
#include "ospf_util.h"
#include "ospf_cnfgr.h"
#include "l3_commdefs.h"
#include "log.h"
#include "nimapi.h"
#include "simapi.h"
#include "usmdb_util_api.h"
#include "rfc1443.h"		/* Needed for enums of _true and _false */
#include "os_xxcmn.h"

/* When OSPF redistributes from any protocol other than BGP, if
    no redistribution metric and no default metric is configured, then
    this value is used as the metric of the redistributed route. */
#define OSPF_EXT_ROUTE_DEFAULT_METRIC   20

/* Same as above, for routes learned from BGP. */
#define OSPF_EXT_ROUTE_DEFAULT_METRIC_BGP   1

/* When OSPF originates a default route, if no metric is configured,
 * and no default metric is configured, OSPF advertises with this metric.
 */
#define OSPF_DEFAULT_METRIC_10  10

/* Mapping from L7_RTO_PROTOCOL_INDICES_t to L7_REDIST_RT_INDICES_t */
/* Defined in rto.c. */
extern L7_REDIST_RT_INDICES_t protocolMapping[RTO_LAST_PROTOCOL];

extern L7_ospfMapCfg_t     *pOspfMapCfgData;
extern ospfInfo_t         *pOspfInfo;
extern ospfVlinkInfo_t    *pOspfVlinkInfo;
extern ospfIntfInfo_t     *pOspfIntfInfo;

extern struct ospfMapCtrl_s  ospfMapCtrl_g;
extern struct ospfMapMibCtrl_s  ospfMapMibCtrl_g;
extern void *Ospf_Redist_Queue;
extern void *ospfMsgQSema;

rtoRouteChange_t *ospfRouteChangeBuf;

/* Wrapper function in Routing code */
extern void L7_RteRemoved(t_RTB *p_RTB, t_RoutingTableEntry *p_Rte);

/* private function prototypes */
static L7_RC_t ospfMapExtenRTOConfigSet(t_S_RouterCfg *p_rtoCfg);
static L7_RC_t ospfMapExtenIFOConfigSet(t_Handle IFO_Id, t_IFO_Cfg *p_ifoCfg);
static L7_RC_t virtIfAuthKeyCfgSet(t_Handle IFO_Id, t_OspfAuthKey *p_Auth);
static L7_RC_t rtoCfgGet(t_Handle RTO_Id, t_S_RouterCfg *p_Cfg);
static L7_RC_t aroCfgGet(L7_uint32 areaId, t_S_AreaCfg *p_Cfg);
static L7_RC_t ifoCfgGet(L7_uint32 intIfNum, t_IFO_Cfg *p_Cfg);
static L7_RC_t ifoMetricCfgGet(L7_uint32 intIfNum, L7_int32 TOS, t_S_IfMetric *p_Metric);
static L7_RC_t ifoMetricCfgSet(L7_uint32 intIfNum, L7_int32 TOS, t_S_IfMetric *p_Metric);
static L7_RC_t ifoAuthKeyCfgSet(L7_uint32 intIfNum, t_OspfAuthKey *p_Auth);
static L7_RC_t aroLSADbGet(L7_uint32 areaId, t_S_LsDbEntry *p_DbEntry);
static L7_RC_t nboCfgGet(L7_uint32 intIfNum, L7_uint32 routerId, t_S_NeighborCfg *p_Cfg);

static L7_RC_t nbrCfgSet(L7_uint32 IpAddr, L7_uint32 intIfNum, t_S_NeighborCfg *p_Cfg);
static L7_RC_t nbrCfgGet(L7_uint32 IpAddr, L7_uint32 intIfNum, t_S_NeighborCfg *p_Cfg);
static L7_RC_t stubCfgSet(L7_uint32 AreaId, L7_int32 TOS, t_S_StubAreaEntry *p_stubCfg);
static L7_RC_t areaAggregateGet(L7_uint32 AreaID, L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, t_S_AreaAggregateEntry *p_agrEntry);
static L7_RC_t areaAggregateSet(L7_uint32 AreaID, L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, t_S_AreaAggregateEntry *p_agrEntry);
static L7_RC_t areaRangeGet(L7_uint32 AreaID, L7_int32 Net, t_S_AreaAggregateEntry *p_agrEntry);
static L7_RC_t areaRangeSet(L7_uint32 AreaID, L7_int32 Net, t_S_AreaAggregateEntry *p_agrEntry);
static L7_uint32 ospfMapRedistMetric(L7_routeEntry_t *routeEntry);
static L7_OSPF_EXT_METRIC_TYPES_t ospfMapRedistMetricType(L7_routeEntry_t *routeEntry);
static L7_uint32 ospfMapRedistRouteTag(L7_routeEntry_t *routeEntry);
static L7_RC_t ospfMapRedistRouteCreate(L7_routeEntry_t *routeEntry, t_S_AsExternalCfg *asExtCfg);
static L7_BOOL ospfMapMayRedistribute(L7_routeEntry_t *routeEntry);
static L7_RC_t ospfMapExtenRedistFromExt(t_S_AsExternalCfg *asExtCfg,
                                         ospfRedistRoute_t *redistRoute);
static void ospfMapBestRouteEventPost(void);
static L7_RC_t ospfBestRouteChange(L7_routeEntry_t *routeEntry,
                                   RTO_ROUTE_EVENT_t route_status);
L7_BOOL ospfMapPreviouslyRedistributed(L7_uint32 destAddr, L7_uint32 destMask);

extern e_Err LsaOrgSumBulk(t_Handle Id);
extern t_IFO * ifoPtrGet(L7_uint32 intIfNum);
L7_RC_t ifoAuthKeyCfgGet(L7_uint32 intIfNum, t_OspfAuthKey *p_Auth);
L7_RC_t virtIfAuthKeyCfgGet(t_Handle IFO_Id, t_OspfAuthKey *p_Auth);

/* Start of thread-safe api */
/*-----------------------------------------------------------------------------------*/
/* All call to Routing API must be enqueued to the ospf thread.  The following   */
/* Pack & Unpack functions enqueue all user-interface requests to the thread  */
/* and only these functions MUST be used to interface with Routing ospf code!         */
/*-----------------------------------------------------------------------------------*/
e_Err RTO_Config_Pack(t_Handle RTO_Id, t_S_RouterCfg *p_Cfg);
e_Err IFO_Config_Pack(t_Handle IFO_Id, t_IFO_Cfg *p_Cfg);
e_Err ARO_Config_Pack(t_Handle ARO_Id, t_S_AreaCfg *p_Cfg);
e_Err aroLSADbGet_Pack(L7_uint32 areaId, t_S_LsDbEntry *p_DbEntry);
e_Err ospfASOpaqueCksumSum_Get_Pack(t_Handle RTO_Id, L7_uint32 *opaqueCksumSum);
e_Err ospfIsStubRtr_Get_Pack(t_Handle RTO_Id, L7_BOOL *isStubRtr);
e_Err ospfExtLsdbOverflow_Get_Pack(t_Handle RTO_Id, L7_BOOL *extLsdbOverflow);
e_Err ospfGlobalStatus_Pack(t_Handle RTO_Id, L7_ospfStatus_t *status);
e_Err ospfNeighborClear_Pack(t_Handle RTO_Id, t_IFO *p_IFO, L7_uint32 routerId);
e_Err ospfCountersClear_Pack(t_Handle RTO_Id);
static e_Err ospfASOpaqueCksumSum_Get_UnPack(void *p_Info);
static e_Err ospfIsStubRtr_Get_Unpack(void *p_Info);
static e_Err ospfExtLsdbOverflow_Get_Unpack(void *p_Info);
e_Err ospfLinkOpaqueLsdbTable_Lookup_Pack(L7_uint32 ipAddr, L7_uint32 ifIndex,
    L7_uint32 LsdbType, L7_uint32 LsdbId,
    L7_uint32 LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa);
e_Err ospfLinkOpaqueLsdbTable_NextLookup_Pack(L7_uint32 *ipAddr,
    L7_uint32 *ifIndex,
    L7_int32  *LsdbType,
    L7_uint32 *LsdbId,
    L7_uint32 *LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa);
e_Err ospfLinkOpaqueAdvertisement_Lookup_Pack(L7_uint32 ipAddr, L7_uint32 ifIndex,
    L7_uint32 LsdbType, L7_uint32 LsdbId,
    L7_uint32 LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa,
    L7_char8 **p_LsdbAdvertisement);
e_Err ospfAsOpaqueLsdbTable_Lookup_Pack(L7_uint32 LsdbType, L7_uint32 LsdbId,
    L7_uint32 LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa);
e_Err ospfAsOpaqueLsdbTable_NextLookup_Pack(L7_int32  *LsdbType,
    L7_uint32 *LsdbId,
    L7_uint32 *LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa);
e_Err ospfAsOpaqueAdvertisement_Lookup_Pack(L7_uint32 LsdbType,L7_uint32 LsdbId,
    L7_uint32 LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa,
    L7_char8 **p_LsdbAdvertisement);
e_Err ospfAreaOpaqueLsdbTable_Lookup_Pack(L7_uint32 LsdbAreaId,
    L7_uint32 LsdbType, L7_uint32 LsdbId,
    L7_uint32 LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa);

e_Err ospfLsdbTable_Lookup_Pack(L7_uint32 LsdbAreaId,
                                L7_uint32 LsdbType, L7_uint32 LsdbId,
                                L7_uint32 LsdbRouterId,
                                L7_ospfLsdbEntry_t *p_Lsa);

e_Err ospfLsdbAdvertisement_Lookup_Pack(L7_uint32 LsdbAreaId,
                                        L7_uint32 LsdbType, L7_uint32 LsdbId,
                                        L7_uint32 LsdbRouterId,
                                        L7_ospfLsdbEntry_t *p_Lsa,
                                        L7_char8 **p_LsdbAdvertisement);

e_Err ospfLsdbTable_GetNext_Pack(L7_uint32 *areaId,
                                 L7_uint32 *Type, L7_uint32 *Lsid,
                                 L7_uint32 *RouterId,
                                 L7_ospfLsdbEntry_t *p_Lsa);

e_Err ospfExtLsdbTable_Lookup_Pack(L7_uint32 LsdbType, L7_uint32 LsdbId,
                                   L7_uint32 LsdbRouterId,
                                   L7_ospfLsdbEntry_t *p_Lsa);

e_Err ospfExtLsdbAdvertisement_Lookup_Pack(L7_uint32 LsdbType, L7_uint32 LsdbId,
                                           L7_uint32 LsdbRouterId,
                                           L7_ospfLsdbEntry_t *p_Lsa,
                                           L7_char8 **p_LsdbAdvertisement);

e_Err ospfExtLsdbTable_GetNext_Pack(L7_uint32 *Type, L7_uint32 *Lsid,
                                    L7_uint32 *RouterId,
                                    L7_ospfLsdbEntry_t *p_Lsa);

e_Err ospfAbrEntry_Lookup_Pack(L7_uint32 destinationIp,
                               L7_RtbEntryInfo_t *p_rtbEntry);

e_Err ospfAsbrEntry_Lookup_Pack(L7_uint32 destinationIp,
                                L7_RtbEntryInfo_t *p_rtbEntry);

e_Err ospfAbrEntryNext_Pack(L7_uint32 *destinationIp,
                            L7_RtbEntryInfo_t *p_rtbEntry);

e_Err ospfAsbrEntryNext_Pack(L7_uint32 *destinationIp,
                             L7_RtbEntryInfo_t *p_rtbEntry);

e_Err NBO_Config_Pack(t_Handle NBO_Id, t_S_NeighborCfg *p_Cfg);
e_Err RTO_AsExternalCfg_Pack(t_Handle RTO_Id, t_S_AsExternalCfg *p_Cfg);
e_Err RTO_PurgeExternalLsas_Pack(t_Handle RTO_Id);
e_Err ARO_StubMetric_Pack(t_Handle ARO_Id, t_S_StubAreaEntry *p_Cfg);
e_Err ARO_AddressRange_Pack(t_S_AreaAggregateEntry *p_Cfg);
e_Err ARO_HostConfig_Pack(t_Handle ARO_Id, t_S_HostRouteCfg *p_Cfg);
e_Err ARO_Delete_Pack(t_Handle ARO_Id, ulng flag);
e_Err IFO_Delete_Pack(t_Handle IFO_Id, ulng flag, L7_uint32 intIfNum);
e_Err IFO_Up_Pack(t_Handle IFO_Id);
e_Err IFO_Down_Pack(t_Handle IFO_Id);
e_Err IFO_MetricConfig_Pack(t_Handle IFO_Id, t_S_IfMetric *p_Cfg);
e_Err IFO_AuthKeyConfig_Pack(t_Handle IFO_Id, t_OspfAuthKey *p_Cfg);
e_Err NSSA_Config_Pack(L7_uint32 areaId, t_S_NssaCfg *p_Cfg);
e_Err IFO_Stats_Pack(t_IFO *p_IFO, t_PCK_Stats *intfStats);
extern e_Err RTO_OpaqueCapability_Get(t_Handle RTO_Id, L7_uint32 *opaqueEnabled);
extern e_Err RTO_OpaqueCapability_Set(t_Handle RTO_Id, L7_uint32 opaqueEnabled);
extern e_Err ARO_OpaqueLsdbEntry(t_Handle ARO_Id, L7_ospfOpaqueLsdbEntry_t *p_Lsa);
extern e_Err ARO_OpaqueLsdbNextEntry(t_Handle ARO_Id, L7_ospfOpaqueLsdbEntry_t *p_Lsa);
extern e_Err ARO_OpaqueAdvertisement(t_Handle ARO_Id,
                                  L7_ospfOpaqueLsdbEntry_t *p_Lsa,
                                                                L7_char8 **p_LsdbAdvertisement);
extern e_Err RTO_ASOpaqueCksumSum_Get(t_Handle RTO_Id, L7_uint32 *opaqueCksumSum);
extern e_Err RTO_IsStubRtr_Get(t_Handle RTO_Id, L7_BOOL *isStubRtr);
extern e_Err RTO_ExtLsdbOverflow_Get(t_Handle RTO_Id, L7_BOOL *extLsdbOverflow);


/* The following static fuctions may ONLY be called by the corresponding
   Pack functions. */
static e_Err RTO_OpaqueCapability_Get_UnPack(void *p_Info);
static e_Err RTO_OpaqueCapability_Set_UnPack(void *p_Info);

static e_Err ospfMapExtenIntfAreaIdSet(void *p_Info);
static e_Err RTO_Config_Unpack(void *p_Info);
static e_Err IFO_Config_Unpack(void *p_Info);
static e_Err ARO_Config_Unpack(void *p_Info);
static e_Err aroLSADbGet_Unpack(void *p_Info);
static e_Err ospfLsdbTable_Lookup_Unpack(void *p_Info);
static e_Err ospfLsdbAdvertisement_Lookup_Unpack(void *p_Info);
static e_Err ospfLsdbTable_GetNext_Unpack(void *p_Info);
static e_Err ospfExtLsdbTable_Lookup_Unpack(void *p_Info);
static e_Err ospfExtLsdbAdvertisement_Lookup_Unpack(void *p_Info);
static e_Err ospfExtLsdbTable_GetNext_Unpack(void *p_Info);
static e_Err ospfAbrEntry_Lookup_Unpack(void *p_Info);
static e_Err ospfAsbrEntry_Lookup_Unpack(void *p_Info);
static e_Err ospfAbrEntryNext_UnPack(void *p_Info);
static e_Err ospfAsbrEntryNext_UnPack(void *p_Info);
static e_Err NBO_Config_Unpack(void *p_Info);
static e_Err RTO_AsExternalCfg_Unpack(void *p_Info);
static e_Err RTO_PurgeExternalLsas_Unpack(void *p_Info);
static e_Err ARO_StubMetric_Unpack(void *p_Info);
static e_Err ARO_AddressRange_Unpack(void *p_Info);
static e_Err ARO_HostConfig_Unpack(void *p_Info);
static e_Err ARO_Delete_Unpack(void *p_Info);
static e_Err IFO_Delete_Unpack(void *p_Info);
static e_Err IFO_Up_Unpack(void *p_Info);
static e_Err IFO_Down_Unpack(void *p_Info);
static e_Err IFO_MetricConfig_Unpack(void *p_Info);
static e_Err IFO_AuthKeyConfig_Unpack(void *p_Info);
static e_Err IFO_Stats_Unpack(void *p_Info);
static e_Err ospfGlobalStatus_Unpack(void *p_Info);
static e_Err ospfNeighborClear_Unpack(void *p_Info);
static e_Err ospfCountersClear_Unpack(void *p_Info);
/* End of thread-safe api */

/* NSSA functions */
L7_RC_t ospfMapExtenNSSAConfigurationGet(L7_uint32 areaId, t_S_NssaCfg *cfg);

/*
**********************************************************************
*                    API FUNCTIONS  -  GLOBAL CONFIG
**********************************************************************
*/

/*********************************************************************
* @purpose  Sets the Ospf Admin mode
*
* @param    mode  @b{(input)}   L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenOspfAdminModeSet(L7_uint32 mode)
{
  t_RTO *p_RTO = NULL;
  e_Err erc;
  t_S_RouterCfg rtrCfg;

  static const char *routine_name = "ospfMapExtenOspfAdminModeSet()";

  p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;

  memset(&rtrCfg, 0, sizeof(rtrCfg));

  memcpy(&rtrCfg, &p_RTO->Cfg, sizeof(t_S_RouterCfg));
  rtrCfg.RouterStatus = (mode == L7_ENABLE) ? ROW_ACTIVE : ROW_NOT_IN_SERVICE;

  if ((erc = RTO_Config_Pack(ospfMapCtrl_g.RTO_Id, &rtrCfg)) != E_OK)
  {
    OSPFMAP_ERROR("%s %d: %s: ERROR: OSPF_MAP: Unable to configure OSPF router object, rc=%d\n",
                    __FILE__, __LINE__, routine_name, erc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Ospf Router ID
*
* @param    routerID   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenRouterIDSet(L7_uint32 routerID)
{
  t_RTO *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;
  t_S_RouterCfg rtrCfg;

  /* If the router id stored in the vendor code is the same as what
     the user is trying to apply return gracefully */
  if(p_RTO->Cfg.RouterId == routerID)
    return L7_SUCCESS;

  memcpy(&rtrCfg, &p_RTO->Cfg, sizeof(t_S_RouterCfg));
  rtrCfg.RouterId =  (SP_IPADR) routerID;

  return ospfMapExtenRTOConfigSet(&rtrCfg);
}

/*********************************************************************
* @purpose  Handle a change in whether the router is an
*           ASBR. The router becomes an ASBR when it is configured to
*           redistribute from any source. We need to tell the protocol
*           engine of the change so that it can correctly set the E bit
*           in its router LSAs.
*
* @param    mode   @b{(input)}     L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAsbrStatusSet(L7_uint32 mode)
{
  t_RTO *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;
  t_S_RouterCfg rtrCfg;
  t_S_AsExternalCfg asExtCfg;

  bzero((L7_char8 *)&asExtCfg, sizeof(t_S_AsExternalCfg));
  memcpy(&rtrCfg, &p_RTO->Cfg, sizeof(t_S_RouterCfg));
  rtrCfg.ASBdrRtrStatus = (mode == L7_ENABLE) ? TRUE : FALSE;
  return ospfMapExtenRTOConfigSet(&rtrCfg);
}

/*********************************************************************
* @purpose  Purge all external LSAs.
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @comments This causes the engine to clear its list of destinations
*           it's advertising as externals, and to reoriginate external
*           LSAs for those destinations, with age set to max age.
*           Since the default route is advertised as an external route,
*           this also purges the default route, if one is being
*           advertised.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenPurgeExternalLsas()
{
    /* delete all routes in the redistribution list */
    ospfMapRedistListPurge();

    pOspfInfo->originatingDefaultRoute = L7_FALSE;

    /* purge external LSAs from the network */
    RTO_PurgeExternalLsas_Pack((t_RTO *) ospfMapCtrl_g.RTO_Id);

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Given a description of an OSPF external route as type
*           t_S_AsExternalCfg, create an equivalent ospfRedistRoute_t.
*
* @param    asExtCfg   @b{(input)}  OSPF external route description
* @param    redistRoute @b{(output)} Corresponding route for
*                                    redistribution tree.
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t ospfMapExtenRedistFromExt(t_S_AsExternalCfg *asExtCfg,
                                         ospfRedistRoute_t *redistRoute)
{
    redistRoute->destAddr = asExtCfg->DestNetIpAdr;
    redistRoute->destMask = asExtCfg->DestNetIpMask;
    redistRoute->metric = asExtCfg->MetricValue;

    if (asExtCfg->IsMetricType2) {
        redistRoute->metricType = L7_OSPF_METRIC_EXT_TYPE2;
    }
    else {
        redistRoute->metricType = L7_OSPF_METRIC_EXT_TYPE1;
    }
    redistRoute->tag = asExtCfg->ExtRouteTag;
    redistRoute->forwardingAddr = DefaultDestination;
    redistRoute->origNexthop = asExtCfg->Nexthop;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Iterates through all routes in the OSPF redistributeion tree
*           and deletes those routes which are no longer used. For each
*           deleted route, notifies the protocol engine to stop
*           advertising the route.
*
* @param    none
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE otherwise
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenUnusedRoutesDelete()
{
    t_S_AsExternalCfg asExtCfg;
    ospfRedistRoute_t redistRoute;
    L7_RC_t rc = L7_SUCCESS;

    if (ospfMapRedistListGetFirst(&redistRoute) == L7_NOT_EXIST) {
        return L7_SUCCESS;
    }

    while (rc == L7_SUCCESS) {
        if (redistRoute.used == L7_FALSE) {
            /* Tell engine to stop advertising */
            asExtCfg.DestNetIpAdr = redistRoute.destAddr;
            asExtCfg.DestNetIpMask = redistRoute.destMask;
            asExtCfg.AsExtStatus = ROW_DESTROY;
            RTO_AsExternalCfg_Pack((t_RTO *) ospfMapCtrl_g.RTO_Id, &asExtCfg);
        }
        rc = ospfMapRedistListGetNext(redistRoute.destAddr, redistRoute.destMask,
                                      &redistRoute);
    }

    /* Remove all unused routes from redistribution tree. */
    ospfMapRedistListPurgeUnused();
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function should be called whenever there is a configuration
*           change that affects which routes OSPF should redistribute or
*           the attributes of redistributed routes. When a configuration
*           change occurs, we apply the new configuration to every best
*           route in the routing table and inform the OSPF protocol engine
*           of the changes.
*
* @param    none
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE otherwise
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenRedistReevaluate()
{
    t_S_AsExternalCfg asExtCfg;
    L7_routeEntry_t routeEntry;
    ospfRedistRoute_t redistRoute;

    if (ospfMapOspfInitialized() == L7_FALSE) {
        return L7_SUCCESS;
    }

    /* set all routes in the redist list inactive. */
    ospfMapRedistListMarkAllUnused();

    bzero((L7_char8 *)&asExtCfg, sizeof(t_S_AsExternalCfg));
    bzero((L7_char8 *)&routeEntry, sizeof(L7_routeEntry_t));

    /* iterate through best routes */
    while (rtoNextRouteGet(&routeEntry, L7_TRUE) == L7_SUCCESS) {
        
        /* Determine whether route passes new redistribution configuration */
        if (ospfMapMayRedistribute(&routeEntry)) {

            /* Figure out if OSPF redistributed this route under the previous
            configuration. */
            if (ospfMapPreviouslyRedistributed(routeEntry.ipAddr,
                                               routeEntry.subnetMask)) {

                ospfMapRedistListMarkUsed(routeEntry.ipAddr,
                                          routeEntry.subnetMask);

                /* Create a change notification */
                /* Only update the engine if the route actually changes */
                ospfMapRedistRouteCreate(&routeEntry, &asExtCfg);
                asExtCfg.AsExtStatus = ROW_CHANGE;

                ospfMapExtenRedistFromExt(&asExtCfg, &redistRoute);
                if (ospfMapRedistRouteChanged(&redistRoute)) {

                    /* Update the entry on the mapping layer's list of routes
                     that OSPF is currently redistributing. */
                    ospfMapRedistListModify(&redistRoute);

                    /* Send change route to protocol engine */
                    RTO_AsExternalCfg_Pack((t_RTO *) ospfMapCtrl_g.RTO_Id, &asExtCfg);
                }
            }
            else{

                /* Route not previously redistributed, but should be now. */
                /* Determine the attributes of the route to be redistributed */
                ospfMapRedistRouteCreate(&routeEntry, &asExtCfg);
                asExtCfg.AsExtStatus = ROW_CREATE_AND_GO;

                /* Add the new route to the mapping layer's tree of routes
                    that OSPF is currently redistributing. */
                ospfMapExtenRedistFromExt(&asExtCfg, &redistRoute);
                redistRoute.used = L7_TRUE;
                ospfMapRedistListAdd(&redistRoute);

                /* send the new route to the protocol engine for advertisement */
                RTO_AsExternalCfg_Pack((t_RTO *) ospfMapCtrl_g.RTO_Id, &asExtCfg);
            }
        }  /* end may redistribute */
    }      /* end while next best route */

    /* Delete each route no longer redistributed. */
    ospfMapExtenUnusedRoutesDelete();
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Configures if Ospf is compatible with RFC 1583
*
* @param    mode   @b{(input)}  L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    If enabled, preference rules remain those as defined by
* @notes    RFC 1583.  If disabled, preference rules are as stated
* @notes    in Section 16.4.1 of the OSPFv2 standard. When disabled,
* @notes    greater protection is provided against routing loops.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenRfc1583CompatibilitySet(L7_uint32 mode)
{
  t_RTO *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;
  t_S_RouterCfg rtrCfg;

  memcpy(&rtrCfg, &p_RTO->Cfg, sizeof(t_S_RouterCfg));
  rtrCfg.RFC1583Compatibility = (mode == L7_ENABLE) ? TRUE : FALSE;

  return ospfMapExtenRTOConfigSet(&rtrCfg);
}

/*********************************************************************
* @purpose  Configures the value of MtuIgnore flag
*
* @param    intIfNum   @b{(input)}
* @param    val        @b{(input)}  L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    If TRUE, MTU value is ignored while forming adjacencies.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfMtuIgnoreSet(L7_uint32 intIfNum, L7_BOOL val)
{
  t_IFO         *p_IFO = NULL;
  t_NBO         *p_NBO;
  t_IFO_Cfg     ifoCfg;
  e_Err         e = E_FAILED;
  L7_RC_t       rc = L7_FAILURE;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));
  ifoCfg.MtuIgnore = val;

  rc = ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg);

  if(rc == L7_SUCCESS)
  {
    e = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
    while(e == E_OK)
    {
      /* As we have changed the mtu ignore setting, we should try to form the adjacencies again,
       * Change the state to INIT, As per RFC, this will clear all the LSA's
       * and bring the state to INIT, and will try to form the adjacencies again.
       */
      DoNboTransition(p_NBO, NBO_1WAY_RX, 0);

      /* get next neighbour */
      e = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
    }
  }

  /* Returning the value of rc because, whatever the result of recycling adjacencies
   * the return from the function should be based on whether mtu ignore was set or not
   */
  return rc;
}

/*********************************************************************
* @purpose  Configures the value of MaxIpMTUsize
*
* @param    ipMtu   @b{(input)} L7_uint32
* @param    intIfNum   @b{(input)} L7_uint32
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This MTU value is used in DD packets
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfIpMtuSet(L7_uint32 intIfNum, L7_uint32 ipMtu)
{
  t_IFO         *p_IFO = NULL;
  t_IFO_Cfg     ifoCfg;
  L7_RC_t       rc = L7_FAILURE;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));
  ifoCfg.MaxIpMTUsize = ipMtu;

  rc = ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg);

  /* Bounce all adjacencies on this interface. See if they come back
   * up with new IP MTU. */
  ospfMapExtenNeighborClear(intIfNum, 0);

  /* Returning the value of rc because, whatever the result of recycling adjacencies
   * the return from the function should be based on whether ip mtu was set or not
   */
  return rc;

}

/*********************************************************************
* @purpose  Allows a user to enable or disable traps based on the
*           specified mode.
*
* @param    mode      L7_ENABLE/ L7_DISABLE
* @param    trapType  ospf trap type specified in L7_OSPF_TRAP_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapVendExtenTrapModeSet(L7_uint32 trapType)
{
  return ((RTO_TrapControl((t_RTO *)ospfMapCtrl_g.RTO_Id, trapType) == E_OK?
           L7_SUCCESS : L7_FAILURE));
}

/*********************************************************************
* @purpose  Set the maximum number of non-default AS-external-LSAs entries
*           that can be stored in the link-state database.
*
* @param    val         number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The default value is -1. If the value is -1, then there
*           is no limit. When the number of non-default AS-external-LSAs
*           in a router's link-state database reaches ExtLsdbLimit, the
*           router enters overflow state. The router never holds more than
*           ExtLsdbLimit non-default AS-external-LSAs in it database
*           ExtLsdbLimit MUST be set identically in all routers attached
*           to the OSPF backbone and/or any regular OSPF area. (that is,
*           OSPF stub areas and NSSAs are excluded.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExtLsdbLimitSet ( L7_int32 val )
{
  t_RTO *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;
  t_S_RouterCfg rtrCfg;

  memcpy(&rtrCfg, &p_RTO->Cfg, sizeof(t_S_RouterCfg));
  rtrCfg.ExtLsdbLimit = val;

  return ospfMapExtenRTOConfigSet(&rtrCfg);
}

/*********************************************************************
* @purpose  Report to the OSPF protocol a change to maximum paths.
*
* @param    maxPaths         
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenMaxPathsSet ( L7_uint32 maxPaths)
{
  t_RTO *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;
  t_S_RouterCfg rtrCfg;

  memcpy(&rtrCfg, &p_RTO->Cfg, sizeof(t_S_RouterCfg));
  rtrCfg.MaximumPaths = maxPaths;

  return ospfMapExtenRTOConfigSet(&rtrCfg);
}

/*********************************************************************
* @purpose  Set a Bit Mask indicating whether the router is
*           forwarding IP multicast (Class D) datagrams.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *val        bit mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This is based on algorithms defined in Multicast Extensions to OSPF rfc1850.
*           Bit 0, indicates that router can forward IP multicast datagrams in the router's
*           directly attached areas (called intra-area multicast routing).
*
*           Bit 1, indicates that router can forward IP multicast datagrams between OSPF
*           areas (called inter-area multicast routing).
*
*           Bit 2, indicates that router can forward IP multicast datagrams between Auto-
*           nomous Systems (called inter-AS multicast routing).
*
*           Only certain combinations of bit settings are allowed:
*           0 (the default) - multicast forwarding is not enabled.
*           1 - intra-area multicasting only.
*           3 - intra-area and inter-area multicasting.
*           5 - intra-area and inter-AS multicasting.
*           7 - multicasting to all areas.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenMulticastExtensionsSet ( L7_int32 val )
{
  t_RTO *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;
  t_S_RouterCfg rtrCfg;

  memcpy(&rtrCfg, &p_RTO->Cfg, sizeof(t_S_RouterCfg));
  rtrCfg.MulticastExtensions = val;

  return ospfMapExtenRTOConfigSet(&rtrCfg);
}

/*********************************************************************
* @purpose  Set the number of seconds, that after entering overflow
*           state a router waits before attempting to leave overflow
*           state.
*
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This allows the router to again originate non-default
*           AS-external LSAs.
*           The default value is 0 - the router will not leave
*           overflow state until it is restarted.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExitOverflowIntervalSet ( L7_int32 val )
{
  t_RTO *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;
  t_S_RouterCfg rtrCfg;

  memcpy(&rtrCfg, &p_RTO->Cfg, sizeof(t_S_RouterCfg));
  rtrCfg.ExitOverflowInterval = val;

  return ospfMapExtenRTOConfigSet(&rtrCfg);
}

/*********************************************************************
* @purpose  Sets the delay time between when OSPF receives a topology
*           change and when it starts an SPF calculation
*
* @param    spfDelay    @b{(input)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The length of time in seconds between when OSPF receives
*             a topology change and when it starts an SPF calculation."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenSpfDelaySet ( L7_int32 spfDelay )
{
  t_RTO *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;
  t_S_RouterCfg rtrCfg;

  memcpy(&rtrCfg, &p_RTO->Cfg, sizeof(t_S_RouterCfg));
  rtrCfg.SilenceInterval = spfDelay;

  return ospfMapExtenRTOConfigSet(&rtrCfg);
}

/*********************************************************************
* @purpose  Sets the minimum time (in seconds) between two consecutive
*           SPF calculations
*
* @param    spfHoldtime @b{(input)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The length of time in seconds between two consecutive
*             SPF calculations."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenSpfHoldtimeSet ( L7_int32 spfHoldtime )
{

  t_RTO *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;
  t_S_RouterCfg rtrCfg;

  memcpy(&rtrCfg, &p_RTO->Cfg, sizeof(t_S_RouterCfg));
  rtrCfg.Holdtime = spfHoldtime;

  return ospfMapExtenRTOConfigSet(&rtrCfg);
}

/*********************************************************************
* @purpose  Set the router's support for demand routing.
*
* @param    *val        pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    L7_TRUE: The router supports demand routing
*           L7_FALSE: The router does not support demand routing
*
*           Note: OSPF++ does not support demand routing
*           extensions, this field should always be set to FALSE
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenDemandExtensionsSet ( L7_int32 val )
{
  /*t_RTO *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;
  t_S_RouterCfg rtrCfg; */

  return L7_NOT_SUPPORTED;
  /*

  memcpy(&rtrCfg, &p_RTO->Cfg, sizeof(t_S_RouterCfg));
  rtrCfg.DemandExtensions = FALSE;

  return ospfMapExtenRTOConfigSet(&rtrCfg);

  */
}


/*********************************************************************
* @purpose  Set the router's support for TOS routing.
*
* @param    *val        pass L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    L7_TRUE: The router supports TOS routing
*           L7_FALSE: The router does not support TOS routing
*
* @end
*********************************************************************/
L7_RC_t ospfMapTOSSupportSet ( L7_int32 val )
{
  t_RTO *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;
  t_S_RouterCfg rtrCfg;

  if (ospfMapOspfInitialized() != L7_TRUE)
  {
      return L7_SUCCESS;
  }
  if (ospfMapCtrl_g.RTO_Id == L7_NULLPTR)
  {
    return L7_SUCCESS;
  }
  memcpy(&rtrCfg, &p_RTO->Cfg, sizeof(t_S_RouterCfg));
  if (val == L7_ENABLE)
    rtrCfg.TOSSupport = TRUE;
  else
    rtrCfg.TOSSupport = FALSE;

  return ospfMapExtenRTOConfigSet(&rtrCfg);
}

/*********************************************************************
* @purpose  Get the current version number of the OSPF protocol.
*
* @param    *val        version number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Currently this is set to 2
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVersionNumberGet ( L7_int32 *val )
{
  t_S_RouterCfg cfg;

  if (rtoCfgGet(ospfMapCtrl_g.RTO_Id, &cfg) != L7_SUCCESS)
    return L7_FAILURE;

  *val = cfg.VersionNumber;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get a Bit Mask indicating whether the router is
*           forwarding IP multicast (Class D) datagrams.
*
* @param    *val        bit mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This is based on algorithms defined in Multicast Extensions to OSPF rfc1850.
*           Bit 0, indicates that router can forward IP multicast datagrams in the router's
*           directly attached areas (called intra-area multicast routing).
*
*           Bit 1, indicates that router can forward IP multicast datagrams between OSPF
*           areas (called inter-area multicast routing).
*
*           Bit 2, indicates that router can forward IP multicast datagrams between Auto-
*           nomous Systems (called inter-AS multicast routing).
*
*           Only certain combinations of bit settings are allowed:
*           0 (the default) - multicast forwarding is not enabled.
*           1 - intra-area multicasting only.
*           3 - intra-area and inter-area multicasting.
*           5 - intra-area and inter-AS multicasting.
*           7 - multicasting to all areas.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenMulticastExtensionsGet ( L7_int32 *val )
{
  t_S_RouterCfg cfg;

  if (rtoCfgGet(ospfMapCtrl_g.RTO_Id, &cfg) != L7_SUCCESS)
    return L7_FAILURE;

  *val = cfg.MulticastExtensions;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the router's support for demand routing.
*
* @param    *val        pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    L7_TRUE: The router supports demand routing
*           L7_FALSE: The router does not support demand routing
*
*           Note: OSPF++ does not support demand routing
*           extensions, this field should always be set to FALSE
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenDemandExtensionsGet ( L7_int32 *val )
{
  t_S_RouterCfg cfg;

  if (rtoCfgGet(ospfMapCtrl_g.RTO_Id, &cfg) != L7_SUCCESS)
    return L7_FAILURE;

  *val = (cfg.DemandExtensions == TRUE) ? L7_TRUE : L7_FALSE;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Get the self originated external LSA count value
*
* @param    *val        pointer to output location
*                         @b{(output) count value}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenSelfOrigExternalLSACountGet(L7_uint32 *val)
{
  t_S_RouterCfg cfg;

  if (rtoCfgGet(ospfMapCtrl_g.RTO_Id, &cfg) != L7_SUCCESS)
    return L7_FAILURE;

  *val = cfg.SelfOrigExternLsaCount;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the opaque AS LSA count value
*
* @param    *val        pointer to output location
*                         @b{(output) count value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenOpaqueASLSACountGet(L7_uint32 *val)
{
  t_S_RouterCfg cfg;

  if (rtoCfgGet(ospfMapCtrl_g.RTO_Id, &cfg) != L7_SUCCESS)
    return L7_FAILURE;

  *val = cfg.OpaqueASLsaCount;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the external LSA count value
*
* @param    *val        pointer to output location
*                         @b{(output) external LSA count value}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExternalLSACountGet(L7_uint32 *val)
{
  t_S_RouterCfg cfg;

  if (rtoCfgGet(ospfMapCtrl_g.RTO_Id, &cfg) != L7_SUCCESS)
    return L7_FAILURE;

  *val = cfg.ExternLsaCount;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the external LSA checksum value
*
* @param    *val        pointer to output location
*                         @b{(output) external LSA checksum value}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExternalLSAChecksumGet(L7_uint32 *val)
{
  t_S_RouterCfg cfg;

  if (rtoCfgGet(ospfMapCtrl_g.RTO_Id, &cfg) != L7_SUCCESS)
    return L7_FAILURE;

  *val = cfg.ExternLsaCksumSum;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of new link-state advertisements that have
*           been originated
*
* @param    *val        pointer to output location
*                         @b{(output) new LSAs originated count value}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNewLSAOrigGet(L7_uint32 *val)
{
  t_S_RouterCfg cfg;

  if (rtoCfgGet(ospfMapCtrl_g.RTO_Id, &cfg) != L7_SUCCESS)
    return L7_FAILURE;

  *val = cfg.OriginateNewLsas;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of link-state advertisements received for
*           new instantiations
*
* @param    *val        pointer to output location
*                         @b{(output) LSAs received count value}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNumLSAReceivedGet(L7_uint32 *val)
{
  t_S_RouterCfg cfg;

  if (rtoCfgGet(ospfMapCtrl_g.RTO_Id, &cfg) != L7_SUCCESS)
    return L7_FAILURE;

  *val = cfg.RxNewLsas;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check if the Router is a OSPF Area Border Router
*
* @param    *mode        pointer to output location
*                         @b{(output) L7_TRUE or L7_FALSE}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAbrStatusGet(L7_uint32 *mode)
{
  t_S_RouterCfg cfg;

  if (rtoCfgGet(ospfMapCtrl_g.RTO_Id, &cfg) != L7_SUCCESS)
    return L7_FAILURE;

  *mode = (cfg.AreaBdrRtrStatus == TRUE) ? L7_TRUE : L7_FALSE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check if the capability of Storing Opaque LSAs
*
* @param    *mode        pointer to output location
*                         @b{(output) L7_TRUE or L7_FALSE}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenOpaqueLSAModeGet(L7_uint32 *mode)
{
  t_S_RouterCfg cfg;

  if (rtoCfgGet(ospfMapCtrl_g.RTO_Id, &cfg) != L7_SUCCESS)
    return L7_FAILURE;

  *mode = (cfg.OpaqueCapability == TRUE) ? L7_TRUE : L7_FALSE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Exit Overflow Interval value
*
* @param    *val        pointer to output location
*                         @b{(output) Exit Overflow Interval value }
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExitOverflowIntervalGet(L7_uint32 *val)
{
  t_S_RouterCfg cfg;

  if (rtoCfgGet(ospfMapCtrl_g.RTO_Id, &cfg) != L7_SUCCESS)
    return L7_FAILURE;

  *val = cfg.ExitOverflowInterval;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the External LSA Limit
*
* @param    *val        pointer to output location
*                         @b{(output) External LSA Limit }
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExternalLSALimitGet(L7_int32 *val)
{
  t_S_RouterCfg cfg;

  if (rtoCfgGet(ospfMapCtrl_g.RTO_Id, &cfg) != L7_SUCCESS)
    return L7_FAILURE;

  *val = cfg.ExtLsdbLimit;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the AdminStat of the router
*
* @param    *val        pointer to output location
*                         @b{(output) AdminStat }
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAdminStatGet(L7_uint32 *val)
{
  t_S_RouterCfg cfg;

  if (rtoCfgGet(ospfMapCtrl_g.RTO_Id, &cfg) != L7_SUCCESS)
    return L7_FAILURE;

  *val = (cfg.AdminStat == TRUE)? L7_ENABLE: L7_DISABLE;

  return L7_SUCCESS;
}

/*
**********************************************************************
*                    API FUNCTIONS  -  INTERFACE CONFIG
**********************************************************************
*/

/*********************************************************************
* @purpose  Tests if the Ospf Admin mode for the specified interface
*           is settable.
*
* @param    ipAddr   @b{(input)} ip address of the interface
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    mode     @b{(input)}  L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The OSPF interface's administrative status.  The value
*             formed on the interface, and the interface will be
*             advertised as an internal route to some area.  The
*             value 'disabled' denotes that the interface is
*             external to OSPF."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfAdminModeSetTest(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                         L7_uint32 mode)
{
  t_Handle ptr;
  L7_uint32 tmpMode;

  tmpMode = (mode == L7_ENABLE)? ROW_ACTIVE: ROW_NOT_IN_SERVICE;

  if ((ptr = ospfIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, ipAddr,
                                intIfNum)) != NULLP)
  {
    if (ospfIfTable_SetTest(ospfIfTableAdminStatusSubId, tmpMode, ptr) == E_OK)
    {
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Admin mode for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    mode   @b{(input)}  L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfAdminModeSet(L7_uint32 intIfNum, L7_uint32 mode)
{

  static const char *routine_name = "ospfMapExtenIntfAdminModeSet()";
  t_IFO         *p_IFO = NULL;
  ospfMapLocalMgr_t *lm;
  e_Err         e, erc;
  t_IFO_Cfg     ifoCfg;
  t_MetricEntry *p_M = NULL;
  t_S_IfMetric mCfg;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL) {
    return L7_ERROR;
  }

  if (ospfMapLmLookup(intIfNum, &lm) != L7_SUCCESS) {
      return L7_FAILURE;
  }

  memset(&ifoCfg, 0, sizeof(ifoCfg));
  memset(&mCfg, 0, sizeof(mCfg));

  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));
  ifoCfg.AdminStat = (mode == L7_ENABLE) ? TRUE : FALSE;

  if (ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg) != L7_SUCCESS)
    return L7_FAILURE;

  e = HL_GetFirst(p_IFO->MetricHl, (void *)&p_M);
  if (e == E_OK && p_M != NULL)
  {
    mCfg.IfIndex = p_IFO->Cfg.IfIndex;
    mCfg.MetricTOS = p_M->MetricTOS;
    mCfg.Metric = p_M->Metric;
    mCfg.MetricStatus = (mode == L7_ENABLE) ? ROW_ACTIVE : ROW_NOT_IN_SERVICE;

    if ((erc = IFO_MetricConfig_Pack((t_Handle)p_IFO, &mCfg)) != E_OK)
    {
      OSPFMAP_ERROR("%s %d: %s: ERROR: OSPF_MAP: Error setting interface metrics, rc=%d\n",
                    __FILE__, __LINE__, routine_name, erc);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Test if the area id is settable.
*
* @param    IPAddress   Ip address
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    area   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " A 32-bit integer uniquely identifying the area to
*             which the interface connects.  Area ID 0.0.0.0 is
*             used for the OSPF backbone."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfAreaIdSetTest ( L7_uint32 ipAddress, L7_int32 intIfNum,
                                        L7_uint32 val )
{
  t_Handle ptr;

  if ((ptr = ospfIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, ipAddress,
                                intIfNum)) != NULLP)
  {
    if (ospfIfTable_SetTest(ospfIfTableAreaId, val, ptr) == E_OK)
    {
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the OSPF Area ID for the specified interface
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes    Processing has four stages:
*            1. sanity checks
*            2. configuration changes
*            3. readvertisement
*            4. schedule routing table update (which may trigger further
*               readvertisement.
*
* @end
*********************************************************************/
static e_Err ospfMapExtenIntfAreaIdSet(void *p_Info)
{
  t_XXCallInfo *xxCallInfo = (t_XXCallInfo *)p_Info;
  L7_uint32    intIfNum = (L7_uint32)PACKET_GET(xxCallInfo, 0);
  L7_uint32    newAreaId = (L7_uint32)PACKET_GET(xxCallInfo, 1);
  ulng			giveSem = PACKET_GET(xxCallInfo, 2);
  t_IFO         *p_IFO = NULL;
  e_Err         e;
  t_RTO         *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;
  t_ARO         *p_ARO = NULL;
  t_ARO         *p_oldARO = NULL;
  ospfMapLocalMgr_t *p_lm = NULL;
  L7_uchar8 areaIdStr[OSAPI_INET_NTOA_BUF_SIZE];
  word remainingIntfs = 0;

  /* Get the interface name for error reporting */
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  /* Get the area ID for error reporting */
  osapiInetNtoa(newAreaId, areaIdStr);

  /* 1. Sanity checks. */

  if (ospfMapLmLookup(intIfNum, &p_lm) != L7_SUCCESS)
  {
	  if (giveSem != 0)
	  {
		  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	  }
    return E_BADPARM;
  }

  if ((p_IFO = (t_IFO *) p_lm->IFO_Id) == NULL)
  {
	  if (giveSem != 0)
	  {
		  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	  }
    return E_BADPARM;
  }

  p_oldARO = (t_ARO *) p_IFO->ARO_Id;

  /* If the interface already belongs to the same area exit successfully */
  if (p_oldARO && (p_oldARO->AreaId == newAreaId))
  {
	  if (giveSem != 0)
	  {
		  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	  }
    return E_OK;
  }

  /* Before making configuration changes, set the interface down. 
   * This drops any adjacencies on the interface. */
  IFO_Down(p_IFO);    

  /* Stage 2:  configuration changes */
  /* Update the IFO config */
  p_IFO->Cfg.AreaId = (SP_IPADR) newAreaId;

  /* Find the new area object. Create if it doesn't yet exist. */
  e = HL_FindFirst(p_RTO->AroHl, (byte*)&newAreaId, (void**)&p_ARO);
  if (e != E_OK || p_ARO == NULL)
  {
    if (ARO_Init(p_IFO, (void *)&p_ARO) != E_OK)
    {
        LOG_MSG("Failed to initialize area %s while setting the OSPF area ID on interface %s.", 
                areaIdStr, ifName);
        p_IFO->Cfg.IfStatus = ROW_NOT_IN_SERVICE;
		if (giveSem != 0)
		{
			osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
		}
        return E_FAILED;
    }
  }
  else
  {
      /* Area already existed. Sanity check that interface was not already
       * in new area's list of interfaces. */
      if (HL_FindFirst(p_ARO->IfoHl, (byte*)&p_IFO->IfoId, NULL) == E_OK)
      {
          /* Report problem, but continue. */
          LOG_MSG("Interface %s already in list of interfaces for area %s",
                  ifName, areaIdStr);
      }
  }

  p_IFO->ARO_Id = p_ARO;
  
  /* Remove interface from old area's list of interfaces */
  if (p_oldARO != NULL)
  {
    if (HL_Delete(p_oldARO->IfoHl, (byte*)&p_IFO->IfoId, p_IFO) != E_OK)
    {
      p_IFO->Cfg.IfStatus = ROW_NOT_IN_SERVICE;
      if (giveSem != 0)
      {
        osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
      }
      return E_FAILED;
    }
    /* if no more interfaces in this area and not backbone, delete the area */
    if ((p_RTO->Cfg.DelAreaOnLastIfDel) &&
        (HL_GetEntriesNmb(p_oldARO->IfoHl, &remainingIntfs) == E_OK) &&
        (remainingIntfs == 0))
    {
      ARO_Delete(p_oldARO, 1);
    }
  }

  /* Insert interface into new area's list of interfaces */
  if (HL_Insert(p_ARO->IfoHl, (byte*)&p_IFO->IfoId, p_IFO) != E_OK)
  {
      p_IFO->Cfg.IfStatus = ROW_NOT_IN_SERVICE;
	  if (giveSem != 0)
	  {
		  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	  }
      return E_FAILED;
  }

  IFO_Up(p_IFO);


  /* 3. Readvertise router LSAs for old and new areas to reflect change. 
   * Also originate all summaries into new area. Other readvertisement of 
   * other summary LSAs, for example summarizing destinations in the new 
   * area for other areas, will happen as a result of recomputing
   * the routing tables. */

  LsaReOriginate[S_ROUTER_LSA](p_ARO, 0);
  if(p_oldARO)
     LsaReOriginate[S_ROUTER_LSA](p_oldARO, 0);

  if (routerIsBorder(p_RTO))
      LsaOrgSumBulk((t_Handle)p_ARO);

  /* 4. Recompute all routes. */
  RTO_RecomputeAllRoutes(p_RTO);

  if (giveSem != 0)
  {
	  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  }
  return E_OK;
}

/*********************************************************************
* @purpose  ospfMapExtenIntfAreaIdPack is used to enqueue a request for
*           configuring the areaid of an ospf interface.
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    area   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfAreaIdPack(L7_uint32 intIfNum, L7_uint32 area)
{
	t_XXCallInfo	*xxci;      /* XX_Call info */
	e_Err			e;
	L7_int32		currThread = osapiTaskIdSelf();
	OS_Thread		*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

	PACKET_INIT(xxci, ospfMapExtenIntfAreaIdSet, 0, 0, 3, (ulng)intIfNum);
	PACKET_PUT(xxci, (ulng)area);

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}

		PACKET_PUT(xxci, (ulng) 1);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call ospfMapExtenIntfAreadIdSet on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}
	}
	else
	{
		PACKET_PUT(xxci, (ulng) 0);
		e = ospfMapExtenIntfAreaIdSet(xxci);
		XX_Free(xxci);
	}

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

	return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Test if the priority of this interface is settable.
*
* @param    IPAddress   Ip address
* @param    intIfNum    internal interface number
* @param    val         Designated Router Priority
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The priority of this interface.  Used in multi-access
*             networks, this field is used in the designated router
*             election algorithm.  The value 0 signifies that the
*             router is not eligible to become the designated router
*             on this particular network.  In the event of a tie in
*             this value, router will use their Router ID as a tie
*             breaker."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfPrioritySetTest ( L7_uint32 ipAddr, L7_int32 intIfNum,
                                          L7_int32 val )
{
  /* This is a read-create parameter and can only be changed at conceptual
     interface creation, i.e. before the corresponding interface row
     status becomes active */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Priority for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    priority   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfPrioritySet(L7_uint32 intIfNum, L7_uint32 priority)
{
  t_IFO         *p_IFO = NULL;
  t_IFO_Cfg     ifoCfg;
  ospfMapLocalMgr_t *p_lm = NULL;

  if (ospfMapLmLookup(intIfNum, &p_lm) != L7_SUCCESS)
    return L7_ERROR;

  if ((p_IFO = (t_IFO *) p_lm->IFO_Id) == NULL)
    return L7_ERROR;


  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));
  ifoCfg.RouterPriority = priority;

  if (ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg) != L7_SUCCESS){
      return L7_FAILURE;
  }

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Test if the Ospf Hello Interval for the specified interface
*           is settable.
*
* @param    ipAddr   @b{(input)} ip Address of the interface
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds  @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The length of time, in seconds between the Hello
*             packets that the router sends on the interface.
*             This value must be the same for all router attached
*             to a common network."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfHelloIntervalSetTest (L7_uint32 ipAddr, L7_uint32 intIfNum,
                                              L7_uint32 seconds)
{
  /* This is a read-create parameter and can only be changed at conceptual
     interface creation, i.e. before the corresponding interface row
     status becomes active */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Hello Interval for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfHelloIntervalSet(L7_uint32 intIfNum, L7_uint32 seconds)
{
  t_IFO         *p_IFO = NULL;
  t_IFO_Cfg     ifoCfg;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));
  ifoCfg.HelloInterval = (word) seconds;

  return ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg);
}

/*********************************************************************
* @purpose  Test if the Ospf Dead Interval for the specified interface
*           is settable.
*
* @param    ipAddr   @b{(input)} ip address of the interface
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds  @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The number of seconds that a router's Hello packets
*             have not been seen before it's neighbors declare
*             the router down.  This should be some multiple of the
*             Hello interval.  This value must be the same for all
*             routers attached to a common network."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfDeadIntervalSetTest(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                            L7_uint32 seconds)
{
  /* This is a read-create parameter and can only be changed at conceptual
     interface creation, i.e. before the corresponding interface row
     status becomes active */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Dead Interval for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfDeadIntervalSet(L7_uint32 intIfNum, L7_uint32 seconds)
{
  t_IFO         *p_IFO = NULL;
  t_IFO_Cfg     ifoCfg;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));
  ifoCfg.RouterDeadInterval = (word) seconds;

  return ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg);
}

/*********************************************************************
* @purpose  Tests if the Ospf Retransmit Interval for the specified interface
*           is settable.
*
* @param    IPAddress   Ip address
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The number of seconds between link-state advertisement
*             retransmissions, for adjacencies belonging to this
*             interface.  This value is also used when retransmitting
*             database description and link-state request packets."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfRxmtIntervalSetTest(L7_uint32 ipAddr,
                                            L7_uint32 intIfNum,
                                            L7_uint32 seconds)
{
  /* This is a read-create parameter and can only be changed at conceptual
   interface creation, i.e. before the corresponding interface row
   status becomes active */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Retransmit Interval for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfRxmtIntervalSet(L7_uint32 intIfNum, L7_uint32 seconds)
{
  t_IFO         *p_IFO = NULL;
  t_IFO_Cfg     ifoCfg;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));
  ifoCfg.RxmtInterval = (word) seconds;

  return ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg);
}

/*********************************************************************
* @purpose  Test if the ospf demand value is settable.
*
* @param    ipAddr      Ip Address of the interface
* @param    intIfNum    internal interface number
* @param    val         pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " Indicates whether Demand OSPF procedures (hello
*             supression to FULL neighbors and setting the DoNotAge
*             flag on propogated LSAs) should be performed on
*             this interface."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfDemandAgingSetTest( L7_uint32 ipAddr,
                                            L7_int32 intIfNum,
                                            L7_int32 val )
{
  t_Handle ptr;
  L7_uint32 demandAging;

  demandAging = (val == L7_TRUE) ? TRUE : FALSE;

  if ((ptr = ospfIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, ipAddr,
                                intIfNum)) != NULLP)
  {
    if (ospfIfTable_SetTest(ospfIfTableIfDemandSubId, demandAging, ptr) == E_OK)
    {
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Demand Aging for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val   @b{(input)}  (TRUE/FALSE)
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfDemandAgingSet(L7_uint32 intIfNum, L7_uint32 val)
{
  t_IFO         *p_IFO = NULL;
  t_IFO_Cfg     ifoCfg;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));
  ifoCfg.IfDemand = (val == L7_TRUE) ? TRUE : FALSE;

  return ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg);
}

/*********************************************************************
* @purpose  Test if the Ospf Nbma Poll Interval for the specified interface
*           is settable.
*
* @param    ipAddr    @b{(input)} ip address of the interface
* @param    intIfNum  @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The larger time interval, in seconds, between the
*             Hello packets sent to an inactive non-broadcast multi-
*             access neighbor."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfNbmaPollIntervalSetTest(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                                L7_uint32 seconds)
{
  /* This is a read-create parameter and can only be changed at conceptual
   interface creation, i.e. before the corresponding interface row
   status becomes active */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Nbma Poll Interval for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfNbmaPollIntervalSet(L7_uint32 intIfNum, L7_uint32 seconds)
{
  t_IFO         *p_IFO = NULL;
  t_IFO_Cfg     ifoCfg;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));
  ifoCfg.NbmaPollInterval = (word) seconds;

  return ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg);
}

/*********************************************************************
* @purpose  Tests if the Ospf Transit Delay for the specified interface
*           is settable.
*
* @param    IPAddress   Ip address
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The estimated number of seconds it takes to transmit
*             a link state update packet over this interface."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfTransitDelaySetTest (L7_uint32 ipAddr,
                                             L7_uint32 intIfNum,
                                             L7_uint32 seconds)
{
  /* This is a read-create parameter and can only be changed at conceptual
   interface creation, i.e. before the corresponding interface row
   status becomes active */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Transit Delay for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfTransitDelaySet(L7_uint32 intIfNum, L7_uint32 seconds)
{
  t_IFO         *p_IFO = NULL;
  t_IFO_Cfg     ifoCfg;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));
  ifoCfg.InfTransDelay = (word) seconds;

  return ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg);
}



/*********************************************************************
* @purpose  Sets the LSA Ack Interval for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The length of time, in seconds, between LSA Acknowledgement
*             packet transmissions. The value must be less than
*             retransmit interval."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfLsaAckIntervalSet(L7_uint32 intIfNum, L7_uint32 seconds)
{

    t_IFO         *p_IFO = NULL;
    t_IFO_Cfg     ifoCfg;

    if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
      return L7_ERROR;

    memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));
    ifoCfg.AckInterval = (word) seconds;

    return ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg);


}

/*********************************************************************
* @purpose  Test if the OSPF interface type is settable
*
* @param    intIfNum    internal interface number
* @param    val         integer
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The OSPF interface type.  By way of default, this field
*             may be intuited from the corresponding value of ifType
*             Broadcast LANs, such as Ethernet and IEEE 802.5, take
*             the value 'broadcast', X.25 and similar technologies
*             take the value 'nbma', and links that are definitively
*             point to point take the value 'pointToPoint'."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfTypeSetTest ( L7_int32 intIfNum, L7_int32 val )
{
  /* This is a read-create parameter and can only be changed at conceptual
     interface creation, i.e. before the corresponding interface row
     status becomes active */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete the OSPF interface type.
*
* @param    intIfNum    internal interface number
* @param    val         integer
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes   needs intIfNum only.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfTypeSet(L7_uint32 intIfNum, L7_int32 val)
{
  t_IFO         *p_IFO = NULL;
  t_IFO_Cfg     ifoCfg;
  L7_RC_t       rc;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));

  if (val == L7_OSPF_INTF_DEFAULT)
  {
    (void) ospfDefaultIfType(intIfNum, &val);
  }
  
  if (ospfMapVendIfTypeGet(val, &ifoCfg.Type) != L7_SUCCESS)
  {
    return L7_ERROR;
  }

  IFO_Down_Pack(p_IFO);
  rc = ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg);
  IFO_Up_Pack(p_IFO);

  return rc;
}

/*********************************************************************
* @purpose  Convert a LVL7 Cfg ifType to a Vendor ifType
*
* @param    cfgIfType   LVL7 Cfg ifType
* @param    pVendIfType return value for vendor ifType
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if given invalid parameter
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapVendIfTypeGet(L7_uint32 cfgIfType, L7_uint32 *pVendIfType)
{
  switch (cfgIfType)
  {
    case L7_OSPF_INTF_DEFAULT:
    case L7_OSPF_INTF_BROADCAST:
      *pVendIfType = IFO_BRDC;
      break;
    case L7_OSPF_INTF_NBMA:
      *pVendIfType = IFO_NBMA;
      break;
    case L7_OSPF_INTF_PTP:
      *pVendIfType = IFO_PTP;
      break;
    case L7_OSPF_INTF_VIRTUAL:
      *pVendIfType = IFO_VRTL;
      break;
    case L7_OSPF_INTF_POINTTOMULTIPOINT:
      *pVendIfType = IFO_PTM;
      break;
    case L7_OSPF_INTF_TYPE_LOOPBACK:
      *pVendIfType = IFO_LOOPBACK;
      break;
    default:
      return L7_ERROR;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Test if the entry status is settable.
*
* @param    ipAddr      Ip Address of the interface
* @param    intIfNum    internal interface number
* @param    val        status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the status of the entry.  Setting
*             it to 'invalid' has the effect of rendering it
*             inoperative.  The internal effect (row removal) is
*             implementation dependent."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfStatusSetTest ( L7_uint32 ipAddr, L7_int32 intIfNum,
                                      L7_int32 val )
{
  t_Handle ptr;
  e_A_RowStatus status;

  switch (val)
  {
  case L7_OSPF_ROW_ACTIVE:
    status = ROW_ACTIVE;
    break;
  case L7_OSPF_ROW_NOT_IN_SERVICE:
    status = ROW_NOT_IN_SERVICE;
    break;
  case L7_OSPF_ROW_NOT_READY:
    status = ROW_NOT_READY;
    break;
  case L7_OSPF_ROW_CREATE_AND_GO:
    status = ROW_CREATE_AND_GO;
    break;
  case L7_OSPF_ROW_CREATE_AND_WAIT:
    status = ROW_CREATE_AND_WAIT;
    break;
  case L7_OSPF_ROW_DESTROY:
    status = ROW_DESTROY;
    break;
  default:
    return L7_FAILURE;
    break;
  }

  if ((ptr = ospfIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, ipAddr,
                                intIfNum)) != NULLP)
  {
    if (ospfIfTable_SetTest(ospfIfTableRowStatusSubId, status, ptr) == E_OK)
    {
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the entry status.
*
* @param    intIfNum   interface
* @param    val        status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative. needs intIfNum only.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfStatusSet(L7_uint32 intIfNum, L7_int32 val)
{
	t_IFO				*p_IFO = NULL;
	t_IFO_Cfg			ifoCfg;
  t_IFO_Addrs    prevAddrs[IFO_MAX_IP_ADDRS];
  e_A_RowStatus	parm;
  L7_uint32 i=0, j=0, ipNet1, ipNet2, secondariesFlag;
	L7_rtrIntfIpAddr_t	addrs[L7_L3_NUM_IP_ADDRS];

  /* In this function, we check for the secondaryFlags that
   * was set due to "network area" or "ip ospf area secondaries none"
   * and compile only the enabled addresses on the interface
   * and inform the vendor code.
   */
	if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
	{
		return L7_ERROR;
	}

	memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));
  memcpy((L7_char8 *)prevAddrs, (L7_char8 *)ifoCfg.addrs,
         L7_L3_NUM_IP_ADDRS*sizeof(L7_rtrIntfIpAddr_t));

	if (ipMapRtrIntfIpAddrListGet(intIfNum, addrs) == L7_SUCCESS)
	{
    ifoCfg.DupNet[0] = 0;
    memset((L7_char8 *)ifoCfg.addrs, 0,
			   L7_L3_NUM_IP_ADDRS*(sizeof(L7_rtrIntfIpAddr_t)));

    memcpy((L7_char8 *)(&ifoCfg.addrs[0]), (L7_char8 *)(&addrs[0]),
           sizeof(L7_rtrIntfIpAddr_t));

    ospfMapIntfEffectiveSecondariesFlagGet(intIfNum, &secondariesFlag);
    for (i = 1; i < L7_L3_NUM_IP_ADDRS; i++)
		{
			ifoCfg.DupNet[i] = 0;
      if(secondariesFlag & (1<<i))
      {
        j++;
        memcpy((L7_char8 *)(&ifoCfg.addrs[j]), (L7_char8 *)(&addrs[i]),
               sizeof(L7_rtrIntfIpAddr_t));
      }
		}

		for (i = 0; i < L7_L3_NUM_IP_ADDRS - 1; i++)
		{
			if (addrs[i].ipAddr == 0)
			{
				continue;
			}

			ipNet1 = addrs[i].ipAddr & addrs[i].ipMask;
			for (j = i+1; j < L7_L3_NUM_IP_ADDRS; j++)
			{
				if (addrs[j].ipAddr == 0)
				{
					continue;
				}

				ipNet2 = addrs[j].ipAddr & addrs[j].ipMask;

				if (ipNet2 == ipNet1)
				{
					ifoCfg.DupNet[j] = 1;
				}
			}
		}
	}

  /* There is no change in the set of addresses 
   * to be informed to the vendor code
   */
  if(memcmp((void *)prevAddrs, (void *)ifoCfg.addrs,
     L7_L3_NUM_IP_ADDRS*sizeof(L7_rtrIntfIpAddr_t)) == 0)
    return L7_SUCCESS;

	switch (val)
	{
		case L7_OSPF_ROW_ACTIVE:
			parm = ROW_ACTIVE;
			break;

		case L7_OSPF_ROW_NOT_IN_SERVICE:
			parm = ROW_NOT_IN_SERVICE;
			break;

		case L7_OSPF_ROW_NOT_READY:
			parm = ROW_NOT_READY;
			break;

		case L7_OSPF_ROW_CREATE_AND_GO:
			parm = ROW_CREATE_AND_GO;
			break;

		case L7_OSPF_ROW_CREATE_AND_WAIT:
			parm = ROW_CREATE_AND_WAIT;
			break;

		case L7_OSPF_ROW_DESTROY:
			parm = ROW_DESTROY;
			break;

		case L7_OSPF_ROW_CHANGE:
			parm = ROW_CHANGE;
			break;

		default:
			return L7_FAILURE;
			break;
	}

	ifoCfg.IfStatus = parm;

	if (IFO_Config_Pack((t_Handle)p_IFO, &ifoCfg) != E_OK)
	{
		return L7_FAILURE;
	}

	return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Test if Ospf multicast forwarding for the specified
*           interface is settable.
*
* @param    ipAddr      Ip Address of the interface
* @param    intIfNum    internal interface number
* @param    val         multicast forwarding
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The way multicasts should be forwarded on this interface;
*             not forwarded, forwarded as data link multicasts, or
*             forwarded as data link unicasts.  Data link multicasting
*             is not meaningful on point to point and NBMA interfaces,
*             and setting ospfMulticastForwarding to 0 effectively
*             disables all mutlicast forwarding.
*
*             Possible values:
*               blocked (1)
*               multicast (2)
*               unicast (3) "
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfMulticastForwardingSetTest ( L7_uint32 ipAddr,
                                                   L7_int32 intIfNum,
                                                   L7_int32 val )
{
  t_Handle ptr;
  e_MulticastForwConf mcastfwd;

  switch (val)
  {
  case L7_OSPF_MULTICAST_FORWARDING_BLOCKED:
    mcastfwd = IF_MLTCST_BLOCKED;
    break;
  case L7_OSPF_MULTICAST_FORWARDING_MULTICAST:
    mcastfwd = IF_MLTCST_MULTICAST;
    break;
  case L7_OSPF_MULTICAST_FORWARDING_UNICAST:
    mcastfwd = IF_MLTCST_UNICAST;
    break;
  default:
    return L7_FAILURE;
    break;
  }

  if ((ptr = ospfIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, ipAddr,
                                intIfNum)) != NULLP)
  {
    if (ospfIfTable_SetTest(ospfIfTableMltCastFrwrdSubId, mcastfwd, ptr) == E_OK)
    {
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete the way multicasts should be forwarded.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    LessIf      interface
* @param    val         multicast forwarding
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting ospfMulticastForwarding to 0 disables all multicast forwarding.needs intIfNum only.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfMulticastForwardingSet(L7_uint32 intIfNum, L7_int32 val)
{
  t_IFO         *p_IFO = NULL;
  t_IFO_Cfg     ifoCfg;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));
  ifoCfg.MulticastForwarding = val;

  return ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg);
}

/*********************************************************************
* @purpose  Test if the Ospf Authentication Type for the specified interface
*           is settable.
*
* @param    ipAddr   @b{(input)} IP adress of the interface
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    authType @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_ERROR, if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The authentication type specified for an interface.
*             Additional authentication types may be assigned
*             locally."
*
*             Authentication Types: See L7_AUTH_TYPES_t
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfAuthTypeSetTest (L7_uint32 ipAddr, L7_uint32 intIfNum,
                                         L7_uint32 authType)
{
  t_Handle ptr;
  e_IFO_AuTypes vendAuthType;

  switch (authType)
  {
  case L7_AUTH_TYPE_NONE:
    vendAuthType = IFO_NULL_AUTH;
    break;
  case L7_AUTH_TYPE_SIMPLE_PASSWORD:
    vendAuthType = IFO_PASSWORD;
    break;
  case L7_AUTH_TYPE_MD5:
    vendAuthType = IFO_CRYPT;
    break;
  default:
    return L7_FAILURE;
    break;
  }

  if ((ptr = ospfIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, ipAddr,
                                intIfNum)) != NULLP)
  {
    if (ospfIfTable_SetTest(ospfIfTableAuthTypeSubId, vendAuthType, ptr) == E_OK)
    {
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Authentication Type for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    authType   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfAuthTypeSet(L7_uint32 intIfNum, L7_uint32 authType)
{
  t_IFO         *p_IFO = NULL;
  t_IFO_Cfg     ifoCfg;
  e_IFO_AuTypes vendAuthType;
  L7_RC_t       rc;
  L7_uchar8     blankKey[OSPF_MAP_EXTEN_AUTH_MAX_KEY_MD5] = { 0 };

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  /* Convert from L7 auth types to vendor auth types */
  switch (authType)
  {
  case L7_AUTH_TYPE_NONE:
    vendAuthType = IFO_NULL_AUTH;
    break;
  case L7_AUTH_TYPE_SIMPLE_PASSWORD:
    vendAuthType = IFO_PASSWORD;
    break;
  case L7_AUTH_TYPE_MD5:
    vendAuthType = IFO_CRYPT;
    break;
  default:
    return L7_FAILURE;
    break;
  }

  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));
  ifoCfg.AuType = vendAuthType;

  if ((rc = ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg)) != L7_SUCCESS)
    return L7_FAILURE;

  /* now that the auth type was set, set the auth key to a 'blank' value */
  return ospfMapExtenIntfAuthKeySet(intIfNum, blankKey);
}

/*********************************************************************
* @purpose  Test if the Ospf Authentication Key for the specified interface
*           is settable.
*
* @param    ipAddr   @b{(input)} ip address of the interface
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    key      @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The Authentication Key.  If the Area's Authorization
*             Type is simplePassword, and the key length is
*             shorter than 8 octets, the agent will left adjust
*             and zero fill to 8 octets.
*
*             Note that unauthenticated interfaces need no
*             authentication key, and simple password authentication
*             cannot use a key of more than 8 octets.  Larger keys
*             are useful only with authetication mechanisms not
*             specified in this document.
*
*             When read, ospfIfAuthKey always returns an Octet
*             String of length zero.
*
*             Default value: 0.0.0.0.0.0.0.0 "
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfAuthKeySetTest(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                       L7_uchar8 *key)
{
  /* This is a read-create parameter and can only be changed at conceptual
   interface creation, i.e. before the corresponding interface row
   status becomes active */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Authentication Key for the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    key         @b{(input)} authentication key
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    The incoming key value is left-adjusted and zero-padded
*           to the maximum length per the current auth type.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfAuthKeySet(L7_uint32 intIfNum, L7_uchar8 *key)
{
  t_IFO         *p_IFO = NULL;
  t_IFO_Cfg     ifoCfg;
  t_OspfAuthKey authKeyCfg;
  byte          blankKey[OSPF_MAP_EXTEN_AUTH_MAX_KEY_MD5] = { 0 };
  byte          *pKey;
  L7_RC_t       rc1, rc2;

  /* get IFO config info for this interface */
  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  /* get auth key config info for this interface */
  if (ifoAuthKeyCfgGet(intIfNum, &authKeyCfg) != L7_SUCCESS)
    return L7_FAILURE;

  /* NOTE:  Always updating BOTH the simple and MD5 key fields here.
   *        Depending on the current AuType value, one or both of these
   *        fields are set to a blank (all 0's) key.
   *          - If AuType is IFO_PASSWORD, the incoming key value
   *              is used to set the simple password AuKey field.
   *          - If AuType is IFO_CRYPT, the incoming key is used
   *              to set the AuthKeyHL element's KeyValue field.
   *          - If AuType is IFO_NULL_AUTH, the incoming key
   *              value is ignored (both key fields set to blank key).
   */

  /* retrieve current IFO configuration data */
  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));

  /* update simple key value to input/blank key per current auth type */
  pKey = (ifoCfg.AuType == IFO_PASSWORD) ? (byte *)key : blankKey;
  memcpy(ifoCfg.AuKey, pKey, (size_t)OSPF_MAP_EXTEN_AUTH_MAX_KEY_SIMPLE);
  rc1 = ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg);

  /* also update MD5 key value to input/blank key per current auth type */
  pKey = (ifoCfg.AuType == IFO_CRYPT) ? (byte *)key : blankKey;
  memcpy(authKeyCfg.KeyValue, pKey, (size_t)OSPF_MAP_EXTEN_AUTH_MAX_KEY_MD5);
  authKeyCfg.KeyStatus = ROW_CHANGE;
  rc2 = ifoAuthKeyCfgSet(intIfNum, &authKeyCfg);

  return (rc1 != L7_SUCCESS) ? rc1 : rc2;
}

/*********************************************************************
* @purpose  Test if the Ospf Authentication Key Id for the specified interface
*           is settable.
*
* @param    ipAddr   @b{(input)} ip address of the interface
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    keyId    @b{(input)} key identifier
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfAuthKeyIdSetTest(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                         L7_uint32 keyId)
{
  t_OspfAuthKey authKeyCfg;

  if (ifoPtrGet(intIfNum) == NULL)
    return L7_ERROR;

  if (ifoAuthKeyCfgGet(intIfNum, &authKeyCfg) != L7_SUCCESS)
    return L7_FAILURE;

  if ((keyId < L7_OSPF_AUTH_KEY_ID_MIN) || (keyId > L7_OSPF_AUTH_KEY_ID_MAX))
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Ospf Authentication Key Id for the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    keyId       @b{(input)} key identifier
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfAuthKeyIdSet(L7_uint32 intIfNum, L7_uint32 keyId)
{
  t_OspfAuthKey authKeyCfg;

  /* get IFO config info for this interface */
  if (ifoPtrGet(intIfNum) == NULL)
    return L7_ERROR;

  if (ifoAuthKeyCfgGet(intIfNum, &authKeyCfg) != L7_SUCCESS)
    return L7_FAILURE;

  if ((keyId < L7_OSPF_AUTH_KEY_ID_MIN) || (keyId > L7_OSPF_AUTH_KEY_ID_MAX))
    return L7_FAILURE;

  /* use the current key id to delete the existing AuthKeyHL elem, then
   * create a new elem with the new key id
   */

  authKeyCfg.KeyStatus = ROW_DESTROY;
  if (ifoAuthKeyCfgSet(intIfNum, &authKeyCfg) != L7_SUCCESS)
    return L7_FAILURE;

  authKeyCfg.KeyId = (byte)keyId;
  authKeyCfg.KeyStatus = ROW_CREATE_AND_GO;

  return ifoAuthKeyCfgSet(intIfNum, &authKeyCfg);
}

/*********************************************************************
* @purpose  Sets the Ospf Virtual Transit Area Id for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    area   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfVirtTransitAreaIdSet(L7_uint32 intIfNum, L7_uint32 area)
{
  t_IFO         *p_IFO = NULL;
  t_IFO_Cfg     ifoCfg;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));
  ifoCfg.VirtTransitAreaId = area;

  return ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg);

}

/*********************************************************************
* @purpose  Sets the Ospf Virtual Neighbor for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    neighbor   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfVirtIntfNeighborSet(L7_uint32 intIfNum, L7_uint32 areaId,
                                            L7_uint32 neighbor)
{
  t_IFO         *p_IFO = NULL;
  t_IFO_Cfg     ifoCfg;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));
  ifoCfg.VirtIfNeighbor = neighbor;
  ifoCfg.VirtTransitAreaId = areaId;

  return ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg);

}

/*********************************************************************
* @purpose  Gets the Ospf LSA Acknowledgement Interval for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    * seconds   @b{(output)} Number of seconds
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfLSAAckIntervalGet(L7_uint32 intIfNum, L7_uint32 *seconds)
{
  t_IFO_Cfg     ifoCfg;

  if (ifoCfgGet(intIfNum, &ifoCfg) != L7_SUCCESS)
    return L7_FAILURE;

  *seconds = ifoCfg.AckInterval;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Ospf State for the specified interface
*
* @param    ipAddr @b{(input)}   ip Address
* @param    intIfNum @b{(input)} internal interface number
* @param    *val   @b{(output)} State val
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfStateGet(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                 L7_uint32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, ipAddr,
                                intIfNum)) != NULLP)
  {
    *val = OSPFIF_STATE(ptr);
    /* Convert value back to Routing defines.  This is because in IFO_Config
      Routing converts the existing state to conform with SNMP defines. */
    *val = *val - 1;

    switch (*val)
    {
    case IFO_DOWN:
      *val = L7_OSPF_INTF_DOWN;
      break;
    case IFO_LOOP:
      *val = L7_OSPF_INTF_LOOPBACK;
      break;
    case IFO_WAIT:
      *val = L7_OSPF_INTF_WAITING;
      break;
    case IFO_PTPST:
      *val = L7_OSPF_INTF_POINTTOPOINT;
      break;
    case IFO_DR:
      *val = L7_OSPF_INTF_DESIGNATEDROUTER;
      break;
    case IFO_BACKUP:
      *val = L7_OSPF_INTF_BACKUPDESIGNATEDROUTER;
      break;
    case IFO_DROTHER:
      *val = L7_OSPF_INTF_OTHERDESIGNATEDROUTER;
      break;
    default:
      return L7_FAILURE;
    }

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the Ospf Designated Router Id for the specified interface
*
* @param    ipAddr   @b{(input)} ip address of the interface
* @param    intIfNum @b{(input)} internal interface number
* @param    * val   @b{(output)} Designated Router Id
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfDrIdGet(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                L7_uint32 *val)
{
  t_IFO         *p_IFO = NULL;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_FAILURE;

  *val = p_IFO->DrId;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the Ospf Backup Designated Router Id for the
*           specified interface
*
* @param    ipAddr @b{(input)}   Ip Address of the Interface
* @param    intIfNum @b{(input)} internal interface number
* @param    * val   @b{(output)} Backup Designated Router Id
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfBackupDrIdGet(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                      L7_uint32 *val)
{
  t_IFO         *p_IFO = NULL;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_FAILURE;

  *val = p_IFO->BackupId;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the number of times this  OSPF  interface  has
*            changed its state
*
* @param    ipAddr   @b{(input)} ip address of the interface
* @param    intIfNum @b{(input)} internal interface number
* @param    *val   @b{(output)} Link Events Counter
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfLinkEventsCounterGet(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                             L7_uint32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, ipAddr,
                                intIfNum)) != NULLP)
  {
    *val = OSPFIF_EVENTS(ptr);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the Ospf Local Link Opaque LSAs for the
*           specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    * val   @b{(output)} Local Link Opaque LSA Count
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfLocalLinkOpaqueLSAsGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  t_IFO_Cfg     ifoCfg;

  if (ifoCfgGet(intIfNum, &ifoCfg) != L7_SUCCESS)
    return L7_FAILURE;

  *val = ifoCfg.LinkLsaCount;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Gets the Ospf Local Link Opaque LSA Checksum for the
*           specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    * val   @b{(output)} Local Link Opaque LSA Checksum
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfLocalLinkOpaqueLSACksumGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  t_IFO_Cfg     ifoCfg;

  if (ifoCfgGet(intIfNum, &ifoCfg) != L7_SUCCESS)
    return L7_FAILURE;

  *val = ifoCfg.LinkLsaCksum;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Gets the Ospf IP Address for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    * val   @b{(output)} IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfIPAddrGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  t_IFO_Cfg     ifoCfg;

  if (ifoCfgGet(intIfNum, &ifoCfg) != L7_SUCCESS)
    return L7_FAILURE;

  *val = ifoCfg.IpAdr;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Gets the Ospf Subnet Mask for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    * val   @b{(output)} Subnet Mask
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfSubnetMaskGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  t_IFO_Cfg     ifoCfg;

  if (ifoCfgGet(intIfNum, &ifoCfg) != L7_SUCCESS)
    return L7_FAILURE;

  *val = ifoCfg.IpMask;

  return L7_SUCCESS;

}

/* ospfIf */
/*********************************************************************
* @purpose  Get the Interface Entry.
*
* @param    ipAddr      Ip address
* @param    intf        interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfEntryGet(L7_uint32 ipAddr, L7_uint32 intf)
{
  t_Handle ptr;

  if ((ptr = ospfIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, ipAddr,
                                intf)) != NULLP)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next Interface Entry.
*
* @param    *ipAddr    Ip address
* @param    *intf  interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfEntryNext(L7_uint32 *ipAddr, L7_uint32 *intf)
{
  t_Handle ptr;

  if ((ptr = ospfIfTable_GetNext(ospfMapMibCtrl_g.p_ospfMibObj, *ipAddr,
                                 *intf, (ulng *)ipAddr, (ulng *)intf)) != NULLP)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the priority of this interface.
*
* @param    IPAddress   Ip address
* @param    rtrIfNum    routing interface
* @param    *val        Designated Router Priority
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The value 0 signifies that router is not eligible to become
*           the designated router on this particular  network.needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfRtrPriorityGet(L7_uint32 ipAddr, L7_uint32 rtrIfNum, L7_int32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, ipAddr,
                                rtrIfNum)) != NULLP)
  {
    *val = OSPFIF_RTRPRIORITY(ptr);

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the number of seconds it takes to transmit a link state
*           update packet over this interface.
*
* @param    ipAddr      ip address
* @param    rtrIfNum    routing interface
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfTransitDelayGet(L7_uint32 ipAddr, L7_uint32 rtrIfNum,
                                      L7_int32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, ipAddr,
                                rtrIfNum)) != NULLP)
  {
    *val = OSPFIF_TRANSITDELAY(ptr);

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the number of seconds between link-state advertisement retransmissions,
*           for  adjacencies belonging to this interface.
*
* @param    ipAddr   Ip address
* @param    rtrIfNum routing interface
* @param    *val     seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfRetransIntervalGet(L7_uint32 ipAddr, L7_uint32 rtrIfNum,
                                         L7_int32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, ipAddr,
                                rtrIfNum)) != NULLP)
  {
    *val = OSPFIF_RETRANSINTERVAL(ptr);

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the time in seconds between Hello packets that router
*           sends on the interface.
*
* @param    ipAddr      Ip address
* @param    rtrIfNum    routing interface
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfHelloIntervalGet(L7_uint32 ipAddr, L7_uint32 rtrIfNum,
                                       L7_int32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, ipAddr,
                                rtrIfNum)) != NULLP)
  {
    *val = OSPFIF_HELLOINTERVAL(ptr);

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the number of seconds that a router's Hello packets have
*           not been seen before it's neighbors declare the router down.
*
* @param    ipAddr      Ip address
* @param    rtrIfNum    routing interface
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This should be some multiple of Hello interval. This value
*           must be the same for all routers attached to a common network.needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfRtrDeadIntervalGet(L7_uint32 ipAddr, L7_uint32 rtrIfNum,
                                         L7_int32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, ipAddr,
                                rtrIfNum)) != NULLP)
  {
    *val = OSPFIF_RTRDEADINTERVAL(ptr);

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the larger time interval, in seconds, between the Hello packets
*           sent to an inactive nonbroadcast multiaccess neighbor.
*
* @param    ipAddr      Ip address
* @param    rtrIfNum    routing interface
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfPollIntervalGet(L7_uint32 ipAddr, L7_uint32 rtrIfNum,
                                      L7_int32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, ipAddr,
                                rtrIfNum)) != NULLP)
  {
    *val = OSPFIF_POLLINTERVAL(ptr);

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    IPAddr      Ip address
* @param    intIfNum    internal interface number
* @param    *val        status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfStatusGet(L7_uint32 ipAddr, L7_uint32 intIfNum,
                                L7_int32 *val)
{
  t_IFO_Cfg     ifoCfg;

  if (ifoCfgGet(intIfNum, &ifoCfg) != L7_SUCCESS)
    return L7_FAILURE;

  switch (ifoCfg.IfStatus)
  {
  case ROW_ACTIVE:
    *val = L7_OSPF_ROW_ACTIVE;
    break;
  case ROW_NOT_IN_SERVICE:
    *val = L7_OSPF_ROW_NOT_IN_SERVICE;
    break;
  case ROW_NOT_READY:
    *val = L7_OSPF_ROW_NOT_READY;
    break;
  case ROW_CREATE_AND_GO:
    *val = L7_OSPF_ROW_CREATE_AND_GO;
    break;
  case ROW_CREATE_AND_WAIT:
    *val = L7_OSPF_ROW_CREATE_AND_WAIT;
    break;
  case ROW_DESTROY:
    *val = L7_OSPF_ROW_DESTROY;
    break;
  default:
    return L7_FAILURE;
    break;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the way multicasts should be forwarded on this interface.
*
* @param    IPAddr   Ip address
* @param    intIfNum internal interface number
* @param    *val     multicast forwarding
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfMulticastForwardingGet(L7_uint32 ipAddr,
                                             L7_uint32 intIfNum,
                                             L7_int32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, ipAddr,
                                intIfNum)) != NULLP)
  {
    *val =  OSPFIF_MTCASTFORWARDING(ptr);

    switch (*val)
    {
    case IF_MLTCST_BLOCKED:
      *val = L7_OSPF_MULTICAST_FORWARDING_BLOCKED;
      break;
    case IF_MLTCST_MULTICAST:
      *val = L7_OSPF_MULTICAST_FORWARDING_MULTICAST;
      break;
    case IF_MLTCST_UNICAST:
      *val = L7_OSPF_MULTICAST_FORWARDING_UNICAST;
      break;
    default:
      return L7_FAILURE;
      break;
    }
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To indicate whether Demand OSPF procedures should be performed on this interface.
*
* @param    IPAddr      Ip address
* @param    rtrIfNum    routing interface
* @param    *val        pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Demand OSPF procedures include hello supression to FULL neighbors
*           and setting the DoNotAge flag on proogated LSAs.needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfDemandGet(L7_uint32 ipAddr, L7_uint32 rtrIfNum, L7_int32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, ipAddr,
                                rtrIfNum)) != NULLP)
  {
    *val = OSPFIF_DEMAND(ptr);
    switch (*val)
    {
    case TRUE:
      *val = L7_TRUE;
      break;
    case FALSE:
      *val = L7_FALSE;
      break;
    default:
      return L7_FAILURE;
      break;
    }

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the Ospf Area Id for the specified interface
*
* @param    ipAddr      Ip address
* @param    rtrIfNum    routing interface number
* @param    *area       area id
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " A 32-bit integer uniquely identifying the area to
*             which the interface connects.  Area ID 0.0.0.0 is
*             used for the OSPF backbone."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfAreaIdGet(L7_uint32 ipAddr, L7_uint32 rtrIfNum,
                                L7_uint32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, ipAddr,
                                rtrIfNum)) != NULLP)
  {
    *val = OSPFIF_AREAID(ptr);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the Ospf Interface Type
*
* @param    IPAddress   Ip address
* @param    rtrIfNum    routing interface number
* @param    *val        integer
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The OSPF interface type.  By way of default, this field
*             may be intuited from the corresponding value of ifType
*             Broadcast LANs, such as Ethernet and IEEE 802.5, take
*             the value 'broadcast', X.25 and similar technologies
*             take the value 'nbma', and links that are definitively
*             point to point take the value 'pointToPoint'."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfTypeGet(L7_uint32 ipAddr, L7_uint32 rtrIfNum,
                              L7_int32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, ipAddr,
                                rtrIfNum)) != NULLP)
  {
    *val = OSPFIF_IFTYPE(ptr);

    switch (*val)
    {
    case IFO_BRDC:
      *val = L7_OSPF_INTF_BROADCAST;
      break;
    case IFO_NBMA:
      *val = L7_OSPF_INTF_NBMA;
      break;
    case IFO_PTP:
    case IFO_LOOPBACK:	/* Note: Loopback i/f is shown as p-t-p i/f to external world */
      *val = L7_OSPF_INTF_PTP;
      break;
    case IFO_VRTL:
      *val = L7_OSPF_INTF_VIRTUAL;
      break;
    case IFO_PTM:
      *val = L7_OSPF_INTF_POINTTOMULTIPOINT;
      break;
    default:
      return L7_FAILURE;
    }
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the OSPF interface's administrative status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    LessIf      interface
* @param    *val        status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfAdminStatGet(L7_uint32 intIfNum, L7_int32 *val)
{
  t_IFO_Cfg     ifoCfg;

  if (ifoCfgGet(intIfNum, &ifoCfg) != L7_SUCCESS)
    return L7_FAILURE;

  *val = ifoCfg.AdminStat;

  return L7_SUCCESS;

}


/* ospfIf */
/*********************************************************************
* @purpose  Get a particular TOS metric for a non-virtual interface.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    LessIf      interface
* @param    TOS         TOS metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfMetricEntryGet(L7_uint32 ipAddress, L7_uint32 intIfNum, L7_int32 TOS)
{
  t_Handle ospfIfMetricTablePtr = NULLP;

  ospfIfMetricTablePtr = ospfIfMetricTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                    ipAddress, intIfNum, TOS);

  if (ospfIfMetricTablePtr != NULLP)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next TOS metric for a non-virtual interface.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *IPAddress  Ip address
* @param    *LessIf     interface
* @param    *TOS        TOS metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfMetricEntryNext(L7_uint32 *ipAddress, L7_uint32 *intIfNum, L7_int32 *TOS)
{
  t_Handle ospfIfMetricTablePtr = L7_NULLPTR;
  L7_uint32 prevIfIndex, ifIndex;
  L7_uint32 prevIpAddress, prevIntIfNum;
  L7_int32 prevTOS;

  prevIpAddress = *ipAddress;
  prevIntIfNum = *intIfNum;
  prevTOS = *TOS;
  prevIfIndex = *intIfNum;
  ifIndex = *intIfNum;

  ospfIfMetricTablePtr = ospfIfMetricTable_GetNext(ospfMapMibCtrl_g.p_ospfMibObj,
                                                   prevIpAddress, prevIfIndex,
                                                   prevTOS, (ulng *)ipAddress,
                                                   (ulng *)&ifIndex, (ulng *)TOS);
  *intIfNum = ifIndex;

  if (ospfIfMetricTablePtr != L7_NULLPTR)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the OSPF interface metric value.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    LessIf      interface
* @param    TOS         TOS metric
* @param    *val        metric value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The default value of the TOS 0 Metric is 10^8 / ifSpeed.needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfMetricValueGet(L7_uint32 ipAddress, L7_uint32 intIfNum, L7_int32 TOS, L7_int32 *val)
{
  t_Handle ospfIfMetricTablePtr = NULLP;

  if ((ospfIfMetricTablePtr = ospfIfMetricTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                    ipAddress, intIfNum, TOS)) != NULLP)
  {
    *val = OSPFIFMETRIC_VALUE(ospfIfMetricTablePtr);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the OSPF interface metric value status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    LessIf      interface
* @param    TOS         TOS metric
* @param    *val        row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfMetricStatusGet(L7_uint32 ipAddress, L7_uint32 intIfNum, L7_int32 TOS, L7_int32 *val)
{
  t_Handle ospfIfMetricTablePtr = NULLP;

  if ((ospfIfMetricTablePtr = ospfIfMetricTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                    ipAddress, intIfNum, TOS)) != NULLP)
  {
    *val = OSPFIFMETRIC_ROWSTATUS(ospfIfMetricTablePtr);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Test to set the metric value.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    LessIf      interface
* @param    TOS         TOS metric
* @param    val         metric value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfMetricValueSetTest(L7_uint32 ipAddress, L7_uint32 intIfNum, L7_int32 TOS, L7_int32 val)
{
  /* This field does not have a OID in the ospf mib. So it cannot
     be tested.
  */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the metric value.
*
* @param    intIfNum    interface
* @param    TOS         TOS metric
* @param    val         metric value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfMetricValueSet( L7_uint32 intIfNum, L7_int32 TOS, L7_int32 val)
{
  t_S_IfMetric     ifoMetric;

  if (ifoMetricCfgGet(intIfNum, TOS, &ifoMetric) != L7_SUCCESS)
    return L7_FAILURE;

  ifoMetric.Metric = val;

  if (ifoMetricCfgSet(intIfNum, TOS, &ifoMetric) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Set the passive mode of an interface
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    val         @b{(input)} passive mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfPassiveModeSet(L7_uint32 intIfNum, L7_BOOL val)
{
  t_IFO         *p_IFO = NULL;
  t_IFO_Cfg     ifoCfg;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));
  ifoCfg.passiveMode = val;

  return ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg);
}

/*********************************************************************
* @purpose  Test to set the entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    LessIf      interface
* @param    TOS         TOS metric
* @param    val         row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative.needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfMetricStatusSetTest(L7_uint32 ipAddress, L7_uint32 intIfNum, L7_int32 TOS, L7_int32 val)
{
  t_Handle ospfIfMetricTablePtr = NULLP;
  e_Err errRet;

  ospfIfMetricTablePtr = ospfIfMetricTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                              ipAddress, intIfNum, TOS);
  if (ospfIfMetricTablePtr != NULLP)
  {
    errRet = ospfIfMetricTable_SetTest(ospfIfMetricTableRowStatusSubId, val,
                                       ospfIfMetricTablePtr);
    if (errRet == E_OK)
      return L7_SUCCESS;
    else
      return L7_FAILURE;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete the entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    LessIf      interface
* @param    TOS         TOS metric
* @param    val         row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative.needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIfMetricStatusSet(L7_uint32 ipAddress, L7_uint32 intIfNum, L7_int32 TOS, L7_int32 val)
{
  t_S_IfMetric     ifoMetric;
  e_A_RowStatus parm;

  switch (val)
  {
  case L7_OSPF_ROW_ACTIVE:
    parm = ROW_ACTIVE;
    break;
  case L7_OSPF_ROW_NOT_IN_SERVICE:
    parm = ROW_NOT_IN_SERVICE;
    break;
  case L7_OSPF_ROW_NOT_READY:
    parm = ROW_NOT_READY;
    break;
  case L7_OSPF_ROW_CREATE_AND_GO:
    parm = ROW_CREATE_AND_GO;
    break;
  case L7_OSPF_ROW_CREATE_AND_WAIT:
    parm = ROW_CREATE_AND_WAIT;
    break;
  case L7_OSPF_ROW_DESTROY:
    parm = ROW_DESTROY;
    break;
  default:
    return L7_FAILURE;
    break;
  }

  if (ifoMetricCfgGet(intIfNum, TOS, &ifoMetric) != L7_SUCCESS)
    return L7_FAILURE;

  ifoMetric.MetricStatus = parm;

  if (ifoMetricCfgSet(intIfNum, TOS, &ifoMetric) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;

}

/* ospfVirtIf */
/*********************************************************************
* @purpose  Determines if the specified interface is a virtual interface
*
* @param    intIfNum    Internal interface number
*
* @returns  L7_TRUE     if the specified interface exists & is a virtual
*                       interface.
* @returns  L7_FALSE    if the specified interface does not exist or
*                       is not a virtual interface
*
* @notes    none.
*
* @end
*********************************************************************/
L7_BOOL ospfMapIsVirtIfGet(L7_uint32 intIfNum)
{
  t_IFO         *p_IFO = NULL;
  t_IFO_Cfg     ifoCfg;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_FALSE;

  if (IFO_Config_Pack((t_Handle)p_IFO, &ifoCfg) == E_OK)
    if (ifoCfg.Type == IFO_VRTL)
      return L7_TRUE;

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Get a single Virtual Interface entry information.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      Id
* @param    Neighbor    entry info
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfEntryGet(L7_uint32 AreaId, L7_uint32 Neighbor)
{
  t_Handle ptr;

  if ((ptr = ospfVirtIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, AreaId,
                                    Neighbor)) != NULLP)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next Virtual Interface entry information.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *AreaId     Id
* @param    *Neighbor   entry info
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfEntryNext(L7_uint32 *AreaId, L7_uint32 *Neighbor)
{
  t_Handle ptr;

  if ((ptr = ospfVirtIfTable_GetNext(ospfMapMibCtrl_g.p_ospfMibObj, *AreaId,
                                    *Neighbor, (ulng *)AreaId,
                                     (ulng *)Neighbor)) != NULLP)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the number of seconds it takes to transmit a link-state
*           update packet over this interface.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfTransitDelayGet(L7_uint32 AreaId, L7_uint32 Neighbor,
                                          L7_int32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfVirtIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, AreaId,
                                    Neighbor)) != NULLP)
  {
    *val = OSPFVIRTIF_TRANSITDELAY(ptr);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the number of seconds between link-state retransmissions,
*           for  adjacencies belonging to this interface.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfRetransIntervalGet(L7_uint32 AreaId, L7_uint32 Neighbor,
                                             L7_int32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfVirtIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, AreaId,
                                    Neighbor)) != NULLP)
  {
    *val = OSPFVIRTIF_RETRANSINTERVAL(ptr);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the time, in seconds, between the Hello packets that
*           the router sends on the interface.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This value must be the same for the virtual neighbor.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfHelloIntervalGet(L7_uint32 AreaId, L7_uint32 Neighbor, L7_int32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfVirtIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, AreaId,
                                    Neighbor)) != NULLP)
  {
    *val = OSPFVIRTIF_HELLOINTERVAL(ptr);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the number of seconds that a router's Hello packets
*           have not been seen before it's neighbors declare the router down.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This value must be the same for the virtual neighbor.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfRtrDeadIntervalGet(L7_uint32 AreaId, L7_uint32 Neighbor, L7_int32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfVirtIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, AreaId,
                                    Neighbor)) != NULLP)
  {
    *val = OSPFVIRTIF_RTRDEADINTERVAL(ptr);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the OSPF virtual interface state.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        interface state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfStateGet(L7_uint32 AreaId, L7_uint32 Neighbor, L7_int32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfVirtIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, AreaId,
                                    Neighbor)) != NULLP)
  {
    *val = OSPFVIRTIF_STATE(ptr);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the OSPF virtual interface metric.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    ifId        IFO_Id of virtual interface
* @param    *metric     metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfMetricGet(void *ifId, L7_uint32 *metric)
{
    t_S_IfMetric mCfg;
    e_Err erc;
    t_Handle IFO_Id = (t_Handle) ifId;

    memset(&mCfg, 0, sizeof(mCfg));
    mCfg.MetricStatus = ROW_READ;
    if ((erc = IFO_MetricConfig_Pack(IFO_Id, &mCfg)) != E_OK)
    {
      OSPFMAP_ERROR("%s %d: ERROR: OSPF_MAP: Error getting virtual link metric, rc=%d\n",
                    __FILE__, __LINE__, erc);
      return L7_FAILURE;
    }

    *metric = mCfg.Metric;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of state changes or error events on this Virtual Link.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        number of state changes
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfEventsGet(L7_uint32 AreaId, L7_uint32 Neighbor, L7_int32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfVirtIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, AreaId,
                                    Neighbor)) != NULLP)
  {
    *val = OSPFVIRTIF_EVENTS(ptr);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *val        row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfStatusGet(L7_uint32 AreaId, L7_uint32 Neighbor, L7_int32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfVirtIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, AreaId,
                                    Neighbor)) != NULLP)
  {
    *val = OSPFVIRTIF_ROWSTATUS(ptr);

    switch (*val)
    {
    case ROW_ACTIVE:
      *val = L7_OSPF_ROW_ACTIVE;
      break;
    case ROW_NOT_IN_SERVICE:
      *val = L7_OSPF_ROW_NOT_IN_SERVICE;
      break;
    case ROW_NOT_READY:
      *val = L7_OSPF_ROW_NOT_READY;
      break;
    case ROW_CREATE_AND_GO:
      *val = L7_OSPF_ROW_CREATE_AND_GO;
      break;
    case ROW_CREATE_AND_WAIT:
      *val = L7_OSPF_ROW_CREATE_AND_WAIT;
      break;
    case ROW_DESTROY:
      *val = L7_OSPF_ROW_DESTROY;
      break;
    default:
      return L7_FAILURE;
      break;
    }
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Tests if the number of seconds it takes to transmit a link-state
*           update packet over this interface is settable.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The estimated number of seconds it takes to transmit
*             a link-state update packet over this interface."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfTransitDelaySetTest(L7_uint32 AreaId,
                                              L7_uint32 Neighbor,
                                              L7_int32 val)
{
  /* This is a read-create parameter and can only be changed at conceptual
   interface creation, i.e. before the corresponding interface row
   status becomes active */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the interface transit delay
*
* @param    index       index into the pOspfVlinkInfo struct to access
*                       the vlink params.
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfTransitDelaySet(L7_uint32 index, L7_int32 val)
{
  t_IFO_Cfg   ifoCfg ;
  t_IFO       *p_IFO;

  if(index >= L7_OSPF_MAX_VIRT_NBRS)
    return L7_FAILURE;

  p_IFO = (t_IFO *)pOspfVlinkInfo[index].IFO_Id;

  if(!p_IFO)
    return L7_FAILURE;

  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));

  ifoCfg.InfTransDelay = val;
  ifoCfg.IfStatus = ROW_CHANGE;

  return ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg);
}

/*********************************************************************
* @purpose  Tests if the number of seconds between link-state retransmissions,
*           for  adjacencies belonging to this interface is settable.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of seconds between link-state advertisement
*             retransmissions, for adjacencies belonging to this
*             interface.  This value is also used when retransmitting
*             database description and link-state request packets.
*             This value should be well over the expected routnd
*             trip time."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfRetransIntervalSetTest(L7_uint32 AreaId,
                                                 L7_uint32 Neighbor,
                                                 L7_int32 val)
{
  /* This is a read-create parameter and can only be changed at conceptual
   interface creation, i.e. before the corresponding interface row
   status becomes active */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete the number of seconds.
*
* @param    index       index into the pOspfVlinkInfo struct to access
*                       the vlink params.
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfRetransIntervalSet(L7_uint32 index, L7_int32 val)
{
  t_IFO_Cfg   ifoCfg ;
  t_IFO       *p_IFO = NULLP;

  if(index >= L7_OSPF_MAX_VIRT_NBRS)
    return L7_FAILURE;

  p_IFO = (t_IFO *)pOspfVlinkInfo[index].IFO_Id;

  if(!p_IFO)
    return L7_FAILURE;

  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));

  ifoCfg.RxmtInterval = val;
  ifoCfg.IfStatus = ROW_CHANGE;

  return ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg);
}

/*********************************************************************
* @purpose  Tests if the time, in seconds, between the Hello packets that
*           the router sends on the interface is settable.
*
* @param    intIfNum    internal interface number
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The length of time, in seconds, between the Hello
*             packets that the router sends on the interface.
*             This value must be the same for the virtual
*             neighbor."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfHelloIntervalSetTest( L7_uint32 AreaId,
                                                L7_uint32 Neighbor,
                                                L7_int32 val)
{
  /* This is a read-create parameter and can only be changed at conceptual
   interface creation, i.e. before the corresponding interface row
   status becomes active */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete the time.
*
* @param    index       index into the pOspfVlinkInfo struct to access
*                       the vlink params.
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfHelloIntervalSet(L7_uint32 index, L7_int32 val)
{
  t_IFO_Cfg   ifoCfg ;
  t_IFO       *p_IFO = NULLP;

  if(index >= L7_OSPF_MAX_VIRT_NBRS)
    return L7_FAILURE;

  p_IFO = (t_IFO *)pOspfVlinkInfo[index].IFO_Id;

  if(!p_IFO)
    return L7_FAILURE;

  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));

  ifoCfg.HelloInterval = val;
  ifoCfg.IfStatus = ROW_CHANGE;

  return ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg);
}

/*********************************************************************
* @purpose  Tests if the number of seconds that a router's Hello packets
*           have not been seen before it's neighbors declare the router down
*           is settable.
*
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of seconds that a router's Hello packets
*             have not been seen before it's neighbors declare
*             the router down.  This should be some multiple of
*             the Hello interval.  This value must be the same
*             for the virtual neighbor."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfRtrDeadIntervalSetTest( L7_uint32 AreaId,
                                                  L7_uint32 Neighbor,
                                                  L7_int32 val)
{
  /* This is a read-create parameter and can only be changed at conceptual
   interface creation, i.e. before the corresponding interface row
   status becomes active */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete the number of seconds.
*
* @param    index       index into the pOspfVlinkInfo struct to access
*                       the vlink params.
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfRtrDeadIntervalSet(L7_uint32 index, L7_int32 val)
{
  t_IFO_Cfg   ifoCfg ;
  t_IFO       *p_IFO = NULLP;

  if(index >= L7_OSPF_MAX_VIRT_NBRS)
    return L7_FAILURE;

  p_IFO = (t_IFO *)pOspfVlinkInfo[index].IFO_Id;

  if(!p_IFO)
    return L7_FAILURE;

  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));

  ifoCfg.RouterDeadInterval = val;
  ifoCfg.IfStatus = ROW_CHANGE;

  return ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg);
}

/*********************************************************************
* @purpose  Test if the entry status is settable.
*
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the status of the entry.  Setting
*             it to 'invalid' has the effect of rendering it inoperative
*             The internal effect (row removal) is implementation
*             dependent."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfStatusSetTest(L7_uint32 AreaId, L7_uint32 Neighbor,
                                        L7_int32 val)
{
  t_Handle ptr;
  e_A_RowStatus status;

  switch (val)
  {
  case L7_OSPF_ROW_ACTIVE:
    status = ROW_ACTIVE;
    break;
  case L7_OSPF_ROW_NOT_IN_SERVICE:
    status = ROW_NOT_IN_SERVICE;
    break;
  case L7_OSPF_ROW_NOT_READY:
    status = ROW_NOT_READY;
    break;
  case L7_OSPF_ROW_CREATE_AND_GO:
    status = ROW_CREATE_AND_GO;
    break;
  case L7_OSPF_ROW_CREATE_AND_WAIT:
    status = ROW_CREATE_AND_WAIT;
    break;
  case L7_OSPF_ROW_DESTROY:
    status = ROW_DESTROY;
    break;
  default:
    return L7_FAILURE;
    break;
  }

  if ((ptr = ospfVirtIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, AreaId,
                                    Neighbor)) != NULLP)
  {
    if (ospfVirtIfTable_SetTest(ospfVirtIfTableRowStatusSubId, status, ptr) == E_OK)
    {
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete the entry status.
*
* @param    index       index into the pOspfVlinkInfo struct to access
*                       the vlink params.
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfStatusSet(L7_uint32 index, L7_int32 val)
{
  t_IFO_Cfg   ifoCfg ;
  t_IFO       *p_IFO = NULLP;

  if(index >= L7_OSPF_MAX_VIRT_NBRS)
    return L7_FAILURE;

  p_IFO = (t_IFO *)pOspfVlinkInfo[index].IFO_Id;

  if(!p_IFO)
    return L7_FAILURE;

  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));

  ifoCfg.IfStatus = val;

  return ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg);
}

/*********************************************************************
* @purpose  Test if the authentication type is settable.
*
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    val         authentication type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The authentication type specified for an interface.
*             Additional authentication types may be assigned
*             locally.
*
*             Authentication Types:
*               none (0)
*               simplePassword (1)
*               md5 (2)
*               reserved for specification by IANA (>2) "
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfAuthTypeSetTest(L7_uint32 AreaId, L7_uint32 Neighbor,
                                          L7_int32 val)
{
  t_Handle ptr;
  e_IFO_AuTypes vendAuthType;

  switch (val)
  {
  case L7_AUTH_TYPE_NONE:
    vendAuthType = IFO_NULL_AUTH;
    break;
  case L7_AUTH_TYPE_SIMPLE_PASSWORD:
    vendAuthType = IFO_PASSWORD;
    break;
  case L7_AUTH_TYPE_MD5:
    vendAuthType = IFO_CRYPT;
    break;
  default:
    return L7_FAILURE;
    break;
  }

  if ((ptr = ospfVirtIfTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, AreaId,
                                    Neighbor)) != NULLP)
  {
    if (ospfVirtIfTable_SetTest(ospfVirtIfTableAuthTypeSubId, vendAuthType, ptr) == E_OK)
    {
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the authentication type.
*
* @param    index       index into the pOspfVlinkInfo struct to access
*                       the vlink params.
* @param    val         authentication type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfAuthTypeSet(L7_uint32 index, L7_int32 val)
{
  t_IFO_Cfg     ifoCfg ;
  t_IFO         *p_IFO = NULLP;
  L7_uchar8     blankKey[OSPF_MAP_EXTEN_AUTH_MAX_KEY_MD5] = { 0 };

  if(index >= L7_OSPF_MAX_VIRT_NBRS)
    return L7_FAILURE;

  p_IFO = (t_IFO *)pOspfVlinkInfo[index].IFO_Id;

  if(!p_IFO)
    return L7_FAILURE;

  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));

  ifoCfg.AuType = val;
  ifoCfg.IfStatus = ROW_CHANGE;

  if(ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg) != L7_SUCCESS)
    return L7_FAILURE;

  /* now that the auth type was set, set the auth key to a 'blank' value */
  return ospfMapExtenVirtIfAuthKeySet(index, blankKey);
}

/*********************************************************************
* @purpose  Tests if the Authentication Key is settable.
*
* @param    AreaId      Id
* @param    Neighbor    entry info
* @param    *buf        authentication key
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The Authentication Key.  If the Area's Authorization
*             Type is simplePassword, and the key length is
*             shorter than 8 octets, the agent will left adjust
*             and zero fill to 8 octets.
*
*             Note that unauthenticated interfaces need no
*             authentication key, and simple password authentication
*             cannot use a key of more than 8 octets.  Larger keys
*             are useful only with authetication mechanisms not
*             specified in this document.
*
*             When read, ospfIfAuthKey always returns an Octet
*             String of length zero.
*
*             Default value: 0.0.0.0.0.0.0.0 "
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfAuthKeySetTest(L7_uint32 AreaId, L7_uint32 Neighbor,
                                         L7_uchar8 *buf)
{
  /* This is a read-create parameter and can only be changed at conceptual
   interface creation, i.e. before the corresponding interface row
   status becomes active */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the Authentication Key.
*
* @param    index       index into the pOspfVlinkInfo struct to access
*                       the vlink params.
* @param    *buf        authentication key
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfAuthKeySet(L7_uint32 index, L7_uchar8 *buf)
{
  t_IFO_Cfg     ifoCfg ;
  t_OspfAuthKey authKeyCfg;
  byte          blankKey[OSPF_MAP_EXTEN_AUTH_MAX_KEY_MD5] = { 0 };
  byte          *pKey;
  L7_RC_t       rc1, rc2;
  t_IFO         *p_IFO = NULLP;

  if(index >= L7_OSPF_MAX_VIRT_NBRS)
    return L7_FAILURE;

  p_IFO = (t_IFO *)pOspfVlinkInfo[index].IFO_Id;

  if(!p_IFO)
    return L7_FAILURE;

  /* get IFO config info for this virtual interface */
  memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));

  /* get auth key config info for this virtual interface */
  if(virtIfAuthKeyCfgGet((t_Handle)p_IFO, &authKeyCfg) != L7_SUCCESS)
    return L7_FAILURE;

  /* NOTE:  Always updating BOTH the simple and MD5 key fields here.
   *        Depending on the current AuType value, one or both of these
   *        fields are set to a blank (all 0's) key.
   *          - If AuType is IFO_PASSWORD, the incoming key value
   *              is used to set the simple password AuKey field.
   *          - If AuType is IFO_CRYPT, the incoming key is used
   *              to set the AuthKeyHL element's KeyValue field.
   *          - If AuType is IFO_NULL_AUTH, the incoming key
   *              value is ignored (both key fields set to blank key).
   */

  /* update simple key value to input/blank key per current auth type */
  pKey = (ifoCfg.AuType == IFO_PASSWORD) ? (byte *)buf : blankKey;
  memcpy(ifoCfg.AuKey, pKey, (size_t)OSPF_MAP_EXTEN_AUTH_MAX_KEY_SIMPLE);
  ifoCfg.IfStatus = ROW_CHANGE;
  rc1 = ospfMapExtenIFOConfigSet((t_Handle)p_IFO, &ifoCfg);

  /* also update MD5 key value to input/blank key per current auth type */
  pKey = (ifoCfg.AuType == IFO_CRYPT) ? (byte *)buf : blankKey;
  memcpy(authKeyCfg.KeyValue, pKey, (size_t)OSPF_MAP_EXTEN_AUTH_MAX_KEY_MD5);
  authKeyCfg.KeyStatus = ROW_CHANGE;
  rc2 = virtIfAuthKeyCfgSet((t_Handle)p_IFO, &authKeyCfg);

  return (rc1 != L7_SUCCESS) ? rc1 : rc2;
}

/*********************************************************************
* @purpose  Tests if the Authentication Key Id is settable.
*
* @param    index       index into the pOspfVlinkInfo struct to access
*                       the vlink params.
* @param    keyId       key identifier
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfAuthKeyIdSetTest(L7_uint32 index, L7_uint32 keyId)
{
  t_OspfAuthKey authKeyCfg;
  t_IFO         *p_IFO = NULLP;

  if(index >= L7_OSPF_MAX_VIRT_NBRS)
    return L7_FAILURE;

  p_IFO = (t_IFO *)pOspfVlinkInfo[index].IFO_Id;

  if(!p_IFO)
    return L7_FAILURE;

  /* get auth key config info for this virtual interface */
  if(virtIfAuthKeyCfgGet((t_Handle)p_IFO, &authKeyCfg) != L7_SUCCESS)
      return L7_FAILURE;

  if ((keyId < L7_OSPF_AUTH_KEY_ID_MIN) || (keyId > L7_OSPF_AUTH_KEY_ID_MAX))
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the Authentication Key Id.
*
* @param    index       index into the pOspfVlinkInfo struct to access
*                       the vlink params.
* @param    keyId       key identifier
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtIfAuthKeyIdSet(L7_uint32 index, L7_uint32 keyId)
{
  t_OspfAuthKey authKeyCfg;
  t_IFO         *p_IFO = NULLP;

  if(index >= L7_OSPF_MAX_VIRT_NBRS)
    return L7_FAILURE;

  p_IFO = (t_IFO *)pOspfVlinkInfo[index].IFO_Id;

  if(!p_IFO)
    return L7_FAILURE;

  /* get auth key config info for this virtual interface */
  if(virtIfAuthKeyCfgGet((t_Handle)p_IFO, &authKeyCfg) != L7_SUCCESS)
      return L7_FAILURE;

  if ((keyId < L7_OSPF_AUTH_KEY_ID_MIN) || (keyId > L7_OSPF_AUTH_KEY_ID_MAX))
    return L7_FAILURE;

  /* use the current key id to delete the existing AuthKeyHL elem, then
   * create a new elem with the new key id
   */
  authKeyCfg.KeyStatus = ROW_DESTROY;
  if (virtIfAuthKeyCfgSet((t_Handle)p_IFO, &authKeyCfg) != L7_SUCCESS)
    return L7_FAILURE;

  authKeyCfg.KeyId = (byte)keyId;
  authKeyCfg.KeyStatus = ROW_CREATE_AND_GO;

  return virtIfAuthKeyCfgSet((t_Handle)p_IFO, &authKeyCfg);
}

/*********************************************************************
* @purpose  Get the Ospf Router Id for the first neighbour of the
*           specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    ipAddress        L7_uint32 ipAddress of the neighbor
* @param    * p_routerId   @b{(output)} router Id of the Neighbour
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrRouterIdGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *p_routerId)
{
  t_Handle ospfNbrTablePtr = L7_NULLPTR;


  ospfNbrTablePtr = ospfNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                    ipAddress, intIfNum);

  if (ospfNbrTablePtr != L7_NULLPTR)
  {
    *p_routerId = OSPFNBR_RTRID(ospfNbrTablePtr);
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;


}

/*********************************************************************
* @purpose  Get the Ospf Router Id for the next neighbour, after the
*           specified router Id of the neighbour, of the
*           specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *p_routerId      L7_uint32 next router Id
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrRouterIdGetNext(L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *p_routerId)
{
  t_IFO         *p_IFO = NULL;
  t_NBO         *p_NBO = NULL;
  e_Err         e;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  e = HL_FindFirst(p_IFO->NboHl,(byte*)&routerId, (void**)&p_NBO);

  if ((e != E_OK) || (p_NBO == NULL))
    return L7_FAILURE;

  e = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);

  if ((e != E_OK) || (p_NBO == NULL))
    return L7_FAILURE;

  *p_routerId = p_NBO->RouterId;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Get the Ospf IP Address of the specified neighbour router Id
*           of the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 IP Address of the neighbour
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrIPAddrGet(L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val)
{
  t_S_NeighborCfg     nboCfg;

  if (nboCfgGet(intIfNum, routerId, &nboCfg) != L7_SUCCESS)
    return L7_FAILURE;

  *val = nboCfg.IpAdr;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Get the Ospf Interface index of the specified neighbour
*           router Id of the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 interface index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrIfIndexGet(L7_uint32 intIfNum, L7_uint32 routerId, L7_uint32 *val)
{
  t_Handle ospfNbrTablePtr = L7_NULLPTR;

  ospfNbrTablePtr = ospfNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                    routerId, intIfNum);

  if (ospfNbrTablePtr != L7_NULLPTR)
  {
    *val = OSPFNBR_GETIFINDEX(ospfNbrTablePtr);
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;

}

/*********************************************************************
* @purpose  Get the Ospf Options of the specified neighbour
*           router Id of the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 options
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrOptionsGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val)
{
  t_Handle ospfNbrTablePtr = L7_NULLPTR;


  ospfNbrTablePtr = ospfNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                    ipAddress, intIfNum);

  if (ospfNbrTablePtr != L7_NULLPTR)
  {
    *val = OSPFNBR_OPTIONS(ospfNbrTablePtr);
    if((*val & OSPF_OPT_Q_BIT) == OSPF_OPT_Q_BIT)
    {
        *val = 0;
        *val |= L7_OSPF_OPT_Q_BIT;
        return L7_SUCCESS;
    }
    if((*val & OSPF_OPT_E_BIT) == OSPF_OPT_E_BIT)
    {
        *val = 0;
        *val |= L7_OSPF_OPT_E_BIT;
        return L7_SUCCESS;
    }
    if((*val & OSPF_OPT_MC_BIT) == OSPF_OPT_MC_BIT)
    {
        *val = 0;
        *val |= L7_OSPF_OPT_MC_BIT;
        return L7_SUCCESS;
    }
    if((*val & OSPF_OPT_O_BIT) == OSPF_OPT_O_BIT)
    {
        *val = 0;
        *val |= L7_OSPF_OPT_O_BIT;
        return L7_SUCCESS;
    }
    if((*val & VPN_OPTION_BIT) == VPN_OPTION_BIT)
    {
        *val = 0;
        *val |= L7_VPN_OPTION_BIT;
        return L7_SUCCESS;
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;

}

/*********************************************************************
* @purpose  Get the Ospf Area of the specified neighbour
*           router Id of the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 area
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrAreaGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val)
{
  t_Handle ospfNbrTablePtr = L7_NULLPTR;


  ospfNbrTablePtr = ospfNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                    ipAddress, intIfNum);

  if (ospfNbrTablePtr != L7_NULLPTR)
  {
    *val = OSPFNBR_GETAREA(ospfNbrTablePtr);
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;

}

/*********************************************************************
* @purpose  Get the Ospf dead timer remining of the specified neighbour
*           router Id of the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 time in secs
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrDeadTimerRemainingGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val)
{
  t_Handle ospfNbrTablePtr = L7_NULLPTR;


  ospfNbrTablePtr = ospfNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                    ipAddress, intIfNum);

  if (ospfNbrTablePtr != L7_NULLPTR)
  {
    *val = OSPFNBR_GETDEADTIMERREMAINING(ospfNbrTablePtr);
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;

}

/*********************************************************************
* @purpose  How long adjacency for a given neighbor on given interface 
*           has been in Full state.
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 time in secs
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrUptimeGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val)
{
  t_Handle ospfNbrTablePtr = L7_NULLPTR;

  ospfNbrTablePtr = ospfNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                        ipAddress, intIfNum);

  if (ospfNbrTablePtr != L7_NULLPTR)
  {
    *val = OSPFNBR_GETUPTIME(ospfNbrTablePtr);
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Ospf Priority of the specified neighbour
*           router Id of the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 priority
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrPriorityGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val)
{
  t_Handle ospfNbrTablePtr = L7_NULLPTR;


  ospfNbrTablePtr = ospfNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                    ipAddress, intIfNum);

  if (ospfNbrTablePtr != L7_NULLPTR)
  {
    *val = OSPFNBR_GETPRIORITY(ospfNbrTablePtr);
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;

}
/*********************************************************************
* @purpose  Get the Ospf State of the specified neighbour
*           router Id of the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 state
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrStateGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val)
{
  t_Handle ospfNbrTablePtr = L7_NULLPTR;


  ospfNbrTablePtr = ospfNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                    ipAddress, intIfNum);

  if (ospfNbrTablePtr != L7_NULLPTR)
  {
    *val = OSPFNBR_STATE(ospfNbrTablePtr);
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;

}

/*********************************************************************
* @purpose  Get the Ospf Interface State of the specified neighbour
*           router Id of the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 state
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrIntfStateGet(L7_uint32 intIfNum, 
                                    L7_uint32 routerId, L7_uint32 *val)
{
  L7_uint32     state;
  t_IFO         *p_IFO = NULL;

  if ((p_IFO = ifoPtrGet(intIfNum)) != NULL)
  {
    state = p_IFO->Cfg.State;
    switch (state)
    {
    case IFO_DOWN:
      *val = L7_OSPF_INTF_DOWN;
      break;
    case IFO_LOOP:
      *val = L7_OSPF_INTF_LOOPBACK;
      break;
    case IFO_WAIT:
      *val = L7_OSPF_INTF_WAITING;
      break;
    case IFO_PTPST:
      *val = L7_OSPF_INTF_POINTTOPOINT;
      break;
    case IFO_DR:
    case IFO_BACKUP:
    case IFO_DROTHER:
      if (routerId == p_IFO->DrId)
      {
        *val = L7_OSPF_INTF_DESIGNATEDROUTER;
      }
      else if (routerId == p_IFO->BackupId)
      {
        *val = L7_OSPF_INTF_BACKUPDESIGNATEDROUTER;
      }
      else
      {
        *val = L7_OSPF_INTF_OTHERDESIGNATEDROUTER;
      }
      break;
    default:
      return L7_FAILURE;
    }

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Ospf Events Counter of the specified neighbour
*           router Id of the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 events counter
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrEventsCounterGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val)
{
  t_Handle ospfNbrTablePtr = L7_NULLPTR;

  ospfNbrTablePtr = ospfNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                    ipAddress, intIfNum);

  if (ospfNbrTablePtr != L7_NULLPTR)
  {
    *val = OSPFNBR_EVENTS(ospfNbrTablePtr);
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;

}

/*********************************************************************
* @purpose  Check if Ospf Hellos are suppressed to the specified neighbour
*           router Id of the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 Hellos Suppressed ? L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrHellosSuppressedGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val)
{
  t_Handle ospfNbrTablePtr = L7_NULLPTR;
  L7_uint32 retValue = 0;

  ospfNbrTablePtr = ospfNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                    ipAddress, intIfNum);

  if (ospfNbrTablePtr != L7_NULLPTR)
  {
    retValue = OSPFNBR_HELLOSUPPRESSED(ospfNbrTablePtr);
    *val = retValue == _true ? L7_TRUE : retValue == _false ? L7_FALSE : L7_ERROR;
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;

}

/*********************************************************************
* @purpose  Get the current length of the retransmission queue of the
*           specified neighbour router Id of the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    routerId L7_uint32 the specified router Id of the neighbour
* @param    *val      L7_uint32 LS Retransmission Queue Length
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrLSRetransQLenGet(L7_uint32 intIfNum, L7_uint32 ipAddress, L7_uint32 *val)
{
  t_Handle ospfNbrTablePtr = L7_NULLPTR;


  ospfNbrTablePtr = ospfNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                    ipAddress, intIfNum);

  if (ospfNbrTablePtr != L7_NULLPTR)
  {
    *val = OSPFNBR_LSRETRANSQLEN(ospfNbrTablePtr);
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;

}

/* ospfNbr */
/*********************************************************************
* @purpose  Get a single neighbor entry information.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrEntryGet(L7_uint32 ipAddress, L7_uint32 intIfNum)
{
  t_Handle ospfNbrTablePtr = L7_NULLPTR;


  ospfNbrTablePtr = ospfNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                    ipAddress, intIfNum);

  if (ospfNbrTablePtr != L7_NULLPTR)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next neighbor entry information.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    *IpAddr             Ip address
* @param    *intIfNum           internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrEntryNext(L7_uint32 *ipAddress, L7_uint32 *intIfNum)
{
  t_Handle ospfNbrTablePtr = L7_NULLPTR;
  L7_uint32 prevIpAddress, prevIntIfNum;
  L7_uint32 prevIfIndex, ifIndex;

  prevIpAddress = *ipAddress;
  prevIntIfNum = *intIfNum;
  prevIfIndex = *intIfNum;
  ifIndex = *intIfNum;
  ospfNbrTablePtr = ospfNbrTable_GetNext(ospfMapMibCtrl_g.p_ospfMibObj,
                                         prevIpAddress, prevIfIndex,
                                         (ulng *)ipAddress, (ulng *)&ifIndex);
  *intIfNum = ifIndex;

  if (ospfNbrTablePtr != L7_NULLPTR)
    return L7_SUCCESS;
  else
    return L7_FAILURE;

}

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
* @param    *val                entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbmaNbrStatusGet(L7_uint32 ipAddress, L7_uint32 intIfNum, L7_int32 *val)
{
  t_Handle ospfNbrTablePtr = L7_NULLPTR;


  ospfNbrTablePtr = ospfNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                    ipAddress, intIfNum);

  if (ospfNbrTablePtr != L7_NULLPTR)
  {
    *val = OSPFNBR_ROWSTATUS(ospfNbrTablePtr);
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
* @param    *val                entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    'dynamic' and 'permanent' refer to how the neighbor became known.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbmaNbrPermanenceGet(L7_uint32 ipAddress, L7_uint32 intIfNum, L7_int32 *val)
{
  t_Handle ospfNbrTablePtr = L7_NULLPTR;


  ospfNbrTablePtr = ospfNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                    ipAddress, intIfNum);

  if (ospfNbrTablePtr != L7_NULLPTR)
  {
    *val = OSPFNBR_PERMANENCE(ospfNbrTablePtr);
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Test to set the priority of this neighbor.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
* @param    val                 router priority
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The value 0 signifies that neighbor is not eligible to become
*           the designated router on this particular network.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrPrioritySetTest(L7_uint32 ipAddress, L7_uint32 intIfNum, L7_int32 val)
{
  t_Handle ospfNbrTablePtr = L7_NULLPTR;
  e_Err errRet;

  ospfNbrTablePtr = ospfNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                              ipAddress, intIfNum);
  if (ospfNbrTablePtr != L7_NULLPTR)
  {
    errRet = ospfNbrTable_SetTest(ospfNbrTableNbrPriorSubId, val,
                                       ospfNbrTablePtr);
    if (errRet == E_OK)
      return L7_SUCCESS;
    else
      return L7_FAILURE;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete the priority of this neighbor.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
* @param    val                 router priority
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The value 0 signifies that neighbor is not eligible to become
*           the designated router on this particular network.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbrPrioritySet(L7_uint32 IpAddr, L7_uint32 intIfNum, L7_int32 val)
{
  t_S_NeighborCfg     nboCfg;
  if(nbrCfgGet(IpAddr, intIfNum, &nboCfg) != L7_SUCCESS)
      return L7_FAILURE;

  nboCfg.NbrPriority = val;
  nboCfg.NbmaNbrStatus = ROW_CHANGE;
  if(nbrCfgSet(IpAddr, intIfNum, &nboCfg) != L7_SUCCESS)
      return L7_FAILURE;
  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Test to set the entry status.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
* @param    val                 entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbmaNbrStatusSetTest(L7_uint32 ipAddress, L7_uint32 intIfNum, L7_int32 val)
{
  t_Handle ospfNbrTablePtr = L7_NULLPTR;
  e_Err errRet;

  ospfNbrTablePtr = ospfNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                              ipAddress, intIfNum);
  if (ospfNbrTablePtr != L7_NULLPTR)
  {
    errRet = ospfNbrTable_SetTest(ospfNbrTableRowStatusSubId, val,
                                       ospfNbrTablePtr);
    if (errRet == E_OK)
      return L7_SUCCESS;
    else
      return L7_FAILURE;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete the entry status.
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    IpAddr              Ip address
* @param    intIfNum            internal interface number
* @param    val                 entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNbmaNbrStatusSet(L7_uint32 IpAddr, L7_uint32 intIfNum, L7_int32 val)
{
  t_S_NeighborCfg     nboCfg;
  e_A_RowStatus parm;

  switch (val)
  {
  case L7_OSPF_ROW_ACTIVE:
    parm = ROW_ACTIVE;
    break;
  case L7_OSPF_ROW_NOT_IN_SERVICE:
    parm = ROW_NOT_IN_SERVICE;
    break;
  case L7_OSPF_ROW_NOT_READY:
    parm = ROW_NOT_READY;
    break;
  case L7_OSPF_ROW_CREATE_AND_GO:
    parm = ROW_CREATE_AND_GO;
    break;
  case L7_OSPF_ROW_CREATE_AND_WAIT:
    parm = ROW_CREATE_AND_WAIT;
    break;
  case L7_OSPF_ROW_DESTROY:
    parm = ROW_DESTROY;
    break;
  default:
    return L7_FAILURE;
    break;
  }

  if(nbrCfgGet(IpAddr, intIfNum, &nboCfg) != L7_SUCCESS)
      return L7_FAILURE;

  nboCfg.NbmaNbrStatus = parm;
  if(nbrCfgSet(IpAddr, intIfNum, &nboCfg) != L7_SUCCESS)
      return L7_FAILURE;
  return L7_SUCCESS;

}
/*ospfVirtNbr */
/*********************************************************************
* @purpose  Get the Virtual neighbor entry information.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Area        area id
* @param    RtrId       router id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtNbrEntryGet(L7_uint32 Area, L7_uint32 RtrId)
{
  t_Handle ptr;

  if ((ptr = ospfVirtNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, Area,
                                     RtrId)) != NULLP)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Virtual neighbor next entry information.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *Area       area id
* @param    *RtrId      router id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtNbrEntryNext(L7_uint32 *Area, L7_uint32 *RtrId)
{
  t_Handle ptr;

  if ((ptr = ospfVirtNbrTable_GetNext(ospfMapMibCtrl_g.p_ospfMibObj, *Area,
                                *RtrId, (ulng *)Area, (ulng *)RtrId)) != NULLP)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the IP address this Virtual Neighbor is using.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Area        area id
* @param    RtrId       router id
* @param    *val        Ip address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtNbrIpAddrGet(L7_uint32 Area, L7_uint32 RtrId, L7_uint32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfVirtNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, Area,
                                     RtrId)) != NULLP)
  {
    *val = OSPFVIRTNBR_IPADDR(ptr);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get a Bit Mask corresponding to the neighbor's options field.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Area        area id
* @param    RtrId       router id
* @param    *val        bit mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Bit 1, indicates that system will operate on Type of Service metrics other than
*               TOS 0. If zero, neighbor will ignore all metrics except the TOS 0 metric.
*           Bit 2, indicates that system implements OSPF Multicast Routing.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtNbrOptionsGet(L7_uint32 Area, L7_uint32 RtrId, L7_uint32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfVirtNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, Area,
                                     RtrId)) != NULLP)
  {
    *val = OSPFVIRTNBR_OPTIONS(ptr);

    if((*val & OSPF_OPT_Q_BIT) == OSPF_OPT_Q_BIT)
    {
        *val = 0;
        *val |= L7_OSPF_OPT_Q_BIT;
        return L7_SUCCESS;
    }
    if((*val & OSPF_OPT_E_BIT) == OSPF_OPT_E_BIT)
    {
        *val = 0;
        *val |= L7_OSPF_OPT_E_BIT;
        return L7_SUCCESS;
    }
    if((*val & OSPF_OPT_MC_BIT) == OSPF_OPT_MC_BIT)
    {
        *val = 0;
        *val |= L7_OSPF_OPT_MC_BIT;
        return L7_SUCCESS;
    }
    if((*val & OSPF_OPT_O_BIT) == OSPF_OPT_O_BIT)
    {
        *val = 0;
        *val |= L7_OSPF_OPT_O_BIT;
        return L7_SUCCESS;
    }
    if((*val & VPN_OPTION_BIT) == VPN_OPTION_BIT)
    {
        *val = 0;
        *val |= L7_VPN_OPTION_BIT;
        return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Virtual Neighbor Relationship state.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Area        area id
* @param    RtrId       router id
* @param    *val        state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtNbrStateGet(L7_uint32 Area, L7_uint32 RtrId, L7_uint32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfVirtNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, Area,
                                     RtrId)) != NULLP)
  {
    *val = OSPFVIRTNBR_STATE(ptr);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the number of times this virtual link has changed its state,
*           or an error has occurred.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Area        area id
* @param    RtrId       router id
* @param    *val        number of state changes
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtNbrEventsGet(L7_uint32 Area, L7_uint32 RtrId, L7_uint32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfVirtNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, Area,
                                     RtrId)) != NULLP)
  {
    *val = OSPFVIRTNBR_EVENTS(ptr);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}


/*********************************************************************
* @purpose  Get the current length of the retransmission queue.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Area        area id
* @param    RtrId       router id
* @param    *val        length
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtNbrLsRetransQLenGet(L7_uint32 Area, L7_uint32 RtrId, L7_uint32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfVirtNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, Area,
                                     RtrId)) != NULLP)
  {
    *val = OSPFVIRTNBR_LSRETRANSQLEN(ptr);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Indicate whether Hellos are being suppressed to the neighbor.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Area        area id
* @param    RtrId       router id
* @param    *val        pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVirtNbrHelloSuppressedGet(L7_uint32 Area, L7_uint32 RtrId, L7_uint32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfVirtNbrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj, Area,
                                     RtrId)) != NULLP)
  {
    *val = OSPFVIRTNBR_HELLOSUPPRESSED(ptr);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*
**********************************************************************
*                    API FUNCTIONS  -  AREA CONFIG
**********************************************************************
*/


/*********************************************************************
* @purpose  Check if the area exists in the "active" OSPF stack
*
* @param    areaID  @b{(input)}
*
* @returns  L7_SUCCESS, if area has been configured
* @returns  L7_FAILURE, otherwise
*
* @notes    The core OSPF stack  may not allow for pre-configuration of
*           parameters.  This check tests if the area currently exists
*           in the core stack.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaExists(L7_uint32 areaId)
{
  t_ARO         *p_ARO = NULL;
  e_Err         e;
  t_RTO         *p_RTO;

  p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;

  e = HL_FindFirst(p_RTO->AroHl, (byte*)&areaId, (void**)&p_ARO);
  if(e != E_OK || p_ARO == NULL)
      return L7_FAILURE;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Free the area in the "active" OSPF stack
*
* @param    areaID  @b{(input)}
*
* @returns  L7_SUCCESS, if area has not been configured
* @returns  L7_FAILURE, otherwise
*
* @notes    The current stack does not allow for individual removal
*           of an area. An instance of an area is automatically created
*           when it becomes associated with an interface.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaFree(L7_uint32 areaId)
{
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Sets the specified Area Range
*
* @param    areaID  @b{(input)}
* @param    Area Range Params   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if area has not been configured
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeSet(L7_uint32 areaId, struct L7_ospfAreaAddrRangeInfo_s *areaAddrRange)
{
  t_S_AreaAggregateEntry adrRange;
  t_RTO         *p_RTO;
  L7_uint32 val;

  p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;

  if(areaAddrRange == NULL)
      return L7_FAILURE;

  memset(&adrRange, 0, sizeof(adrRange));

  if (areaAddrRange->descr.lsdbType == L7_OSPF_AREA_AGGREGATE_LSDBTYPE_SUMMARYLINK)
      adrRange.LsdbType = AGGREGATE_SUMMARY_LINK;
  else
      adrRange.LsdbType = AGGREGATE_NSSA_EXTERNAL_LINK;

  adrRange.NetIpAdr = areaAddrRange->descr.ipAddr;
  adrRange.SubnetMask = areaAddrRange->descr.netMask;
  adrRange.AggregateEffect = (areaAddrRange->advertiseMode == L7_TRUE) ? AGGREGATE_ADVERTISE : AGGREGATE_DO_NOT_ADVERTISE;
  adrRange.AreaId = areaId;


  /* Determine if the area range exists in the vendor implementation */
  if (ospfMapExtenAreaAggrStatusGet(areaId,
                                    areaAddrRange->descr.lsdbType,
                                    areaAddrRange->descr.ipAddr,
                                    areaAddrRange->descr.netMask,
                                    &val) == L7_SUCCESS)
      adrRange.AggregateStatus = ROW_CHANGE;
  else
      adrRange.AggregateStatus = ROW_CREATE_AND_GO;


  /* Apply the configuration */
  if(ARO_AddressRange_Pack(&adrRange) != E_OK){
      return L7_FAILURE;
  }

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Deletes the specified Area Range
*
* @param    areaID  @b{(input)}
* @param    Area Range Params   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if area has not been configured
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeDelete(L7_uint32 areaId, struct L7_ospfAreaAddrRangeInfo_s *areaAddrRange)
{
  e_Err         e;
  t_S_AreaAggregateEntry adrRange;
  t_RTO         *p_RTO;
  t_ARO         *p_ARO = NULLP;

  p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;

  e = HL_FindFirst(p_RTO->AroHl, (byte*)&areaId, (void**)&p_ARO);
  if(e != E_OK || !p_ARO)
      return L7_ERROR;

  if(areaAddrRange == NULL)
      return L7_FAILURE;

  memset(&adrRange, 0, sizeof(adrRange));

  adrRange.LsdbType = areaAddrRange->descr.lsdbType;
  adrRange.NetIpAdr = areaAddrRange->descr.ipAddr;
  adrRange.SubnetMask = areaAddrRange->descr.netMask;
  adrRange.AggregateStatus = ROW_DESTROY;
  adrRange.AreaId = areaId;

  if(ARO_AddressRange_Pack(&adrRange) != E_OK){
      return L7_FAILURE;
  }

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Get the number times the intra-area route table has been
*           calculated
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) SPF calculation count value}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNumSPFRunsGet(L7_uint32 areaId, L7_uint32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfAreaTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,areaId)) != NULLP)
  {
    *val = OSPFAREA_AUTHSPFRUNS(ptr);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the total number of reachable area border routers
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) area border router count value}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaBorderCountGet(L7_uint32 areaId, L7_uint32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfAreaTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,areaId)) != NULLP)
  {
    *val = OSPFAREA_BDRRTRCOUNT(ptr);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the total number link-state advertisements in this
*           area's link-state database (excludes external LSA's)
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) area LSA count value}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSACountGet(L7_uint32 areaId, L7_uint32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfAreaTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,areaId)) != NULLP)
  {
    *val = OSPFAREA_LSACOUNT(ptr);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the total number link-state advertisements of a
*           particular type in this area's link-state database 
*           (except external LSA's)
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        @b{(output) LSA statistics 
*                         
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSAStatGet(L7_uint32 areaId, L7_OspfAreaDbStats_t *p_AreaStats)
{
  t_ARO         *p_ARO = NULL;
  e_Err         e;

  if (p_AreaStats == NULL)
  {
    return L7_FAILURE;
  }

  if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_MSG("Failed to take OSPF sync semaphore\n");
    return L7_FAILURE;
  }

  e = HL_FindFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
                   (byte*)&areaId, (void**)&p_ARO);
  if ((e != E_OK) || (p_ARO == NULL))
  {
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
    return L7_FAILURE;
  }
    

  e = ARO_LsaStatsReport(p_ARO, p_AreaStats);
  if (e != E_OK)
  {
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
    return L7_FAILURE;
  }

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the 32-bit unsigned checksum of the link-state advertisements
*           LS checksums in this contained in this area's link-state database
*           (excludes external LSA's)
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) area LSA checksum value}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSAChecksumGet(L7_uint32 areaId, L7_uint32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfAreaTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,areaId)) != NULLP)
  {
    *val = OSPFAREA_LSACHKSUM(ptr);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the area summary.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    *val        variable
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The variable ospfAreaSummary controls the import of
*           summary LSAs into stub areas. It has no effect on other areas.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaSummaryGet(L7_uint32 areaId, L7_uint32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfAreaTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,areaId)) != NULLP)
  {
    *val = OSPFAREA_SUMMARY(ptr);

    switch (*val)
    {
    case AREA_NO_SUMMARY:
      *val = L7_OSPF_AREA_NO_SUMMARY;
      break;
    case AREA_SEND_SUMMARY:
      *val = L7_OSPF_AREA_SEND_SUMMARY;
      break;
    default:
      return L7_FAILURE;
      break;
    }
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the total number of Autonomous System border routers
*           reachable  within  this area.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    *val        number of routers
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    This is initially zero, and is calculated in each SPF Pass.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAsBdrRtrCountGet(L7_uint32 areaId, L7_uint32 *val)
{
  t_S_AreaCfg cfg;

  if (aroCfgGet(areaId, &cfg) != L7_SUCCESS)
    return L7_FAILURE;

  *val = cfg.AsBdrRtrCount;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the OSPF area entry status.
*
* @param    areaId      type
* @param    *val        entry status
*
* @returns  L7_SUCCESS  if the area is currently actie
* @returns  L7_FAILURE  otherwise
*
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaStatusGet(L7_uint32 areaId, L7_uint32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfAreaTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,areaId)) != NULLP)
  {
    *val = OSPFAREA_ROWSTATUS(ptr);

    switch (*val)
    {
    case ROW_ACTIVE:
      *val = L7_OSPF_ROW_ACTIVE;
      break;
    case ROW_NOT_IN_SERVICE:
      *val = L7_OSPF_ROW_NOT_IN_SERVICE;
      break;
    case ROW_NOT_READY:
      *val = L7_OSPF_ROW_NOT_READY;
      break;
    case ROW_CREATE_AND_GO:
      *val = L7_OSPF_ROW_CREATE_AND_GO;
      break;
    case ROW_CREATE_AND_WAIT:
      *val = L7_OSPF_ROW_CREATE_AND_WAIT;
      break;
    case ROW_DESTROY:
      *val = L7_OSPF_ROW_DESTROY;
      break;
    default:
      return L7_FAILURE;
      break;
    }
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get new OSPF area entry.
*
* @param    AreaId      new entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    entry information describes the configured parameters and
*           cumulative statistics of one of the router's attached areas.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaEntryGet(L7_uint32 areaId)
{
  t_Handle ptr;

  if ((ptr = ospfAreaTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,areaId)) != NULLP)
    return L7_SUCCESS;

  return L7_FAILURE;

}

/*********************************************************************
* @purpose  Get next OSPF area entry.
*
* @param    AreaId      new entry
* @param    nextAreaId  next area id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    entry information describes the configured parameters and
*           cumulative statistics of one of the router's attached areas.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaEntryGetNext(L7_uint32 areaId, L7_uint32 *nextAreaId)
{
  t_Handle ptr;

  if ((ptr = ospfAreaTable_GetNext(ospfMapMibCtrl_g.p_ospfMibObj,
                                   areaId, (ulng *)nextAreaId)) != NULLP)
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the area's support for importing AS external link-state advertisements.
*
* @param    AreaId      type
* @param    *val        value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenImportAsExternGet(L7_uint32 areaId, L7_uint32 *val)
{
  t_Handle ptr;

  if ((ptr = ospfAreaTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,areaId)) != NULLP)
  {
    *val = OSPFAREA_IMPORTASEXT(ptr);

    switch (*val)
    {
    case AREA_IMPORT_EXTERNAL:
      *val = L7_OSPF_AREA_IMPORT_EXT;
      break;
    case AREA_IMPORT_NO_EXTERNAL:
      *val = L7_OSPF_AREA_IMPORT_NO_EXT;
      break;
    case AREA_IMPORT_NSSA:
      *val = L7_OSPF_AREA_IMPORT_NSSA;
      break;
    default:
      return L7_FAILURE;
      break;
    }
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Test if the area's support for importing AS external link-state
*           advertisements is settable
*
* @param    areaId      type
* @param    val        value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenImportAsExternSetTest(L7_uint32 areaId, L7_uint32 val)
{
  t_Handle      ptr;

  /* Find the corresponding area entry */
  if ((ptr = ospfAreaTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,areaId)) != NULLP)
  {
    /* check if the value is within the correct settable range */
    if (ospfAreaTable_SetTest(ospfAreaTableImpAsExtSubId, val, ptr) == E_OK)
    {
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the area's support for importing AS external link-state
*           advertisements.
*
* @param    AreaId      type
* @param    val         value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenImportAsExternSet(L7_uint32 areaId, L7_uint32 val)
{
  t_S_AreaCfg cfg;
  t_ARO         *p_ARO = NULL;
  e_Err         e;
  t_Handle      ptr;
  e_AreaImportExternalCfg parm;
  L7_RC_t       rc = L7_FAILURE;


  e = HL_FindFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
                   (byte*)&areaId, (void**)&p_ARO);
  if ((e != E_OK) || (p_ARO == NULL))
    return L7_FAILURE;

  memset(&cfg, 0, sizeof(t_S_AreaCfg));
  cfg.AreaStatus = ROW_READ;

  if (ARO_Config_Pack(p_ARO, &cfg) != E_OK)
    return L7_FAILURE;

  /* map to vendor types */
  switch (val)
  {
  case L7_OSPF_AREA_IMPORT_EXT:
    parm = AREA_IMPORT_EXTERNAL;
    break;
  case L7_OSPF_AREA_IMPORT_NO_EXT:
    parm = AREA_IMPORT_NO_EXTERNAL;
    break;
  case L7_OSPF_AREA_IMPORT_NSSA:
    parm = AREA_IMPORT_NSSA ;
    break;
  default:
    return L7_FAILURE;
    break;
  }

  cfg.ExternalRoutingCapability = parm;
  cfg.AreaStatus = ROW_CHANGE;

  /* Find the corresponding area entry */
  if ((ptr = ospfAreaTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,areaId)) != NULLP)
  {
    /* set the row status in the ospf mib table to disable */
    if (ospfAreaTable_SetRowStatus(ospfMapMibCtrl_g.p_ospfMibObj, areaId,
                                   ROW_NOT_IN_SERVICE) == E_OK)
    {
      if ((e = ARO_Config_Pack(p_ARO, &cfg)) == E_OK)
        rc = L7_SUCCESS;

      if (ospfAreaTable_SetRowStatus(ospfMapMibCtrl_g.p_ospfMibObj, areaId,
                                     ROW_ACTIVE) == E_OK)
      {
        return rc;
      }
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Test if the area summary is settable
*
* @param    areaId      type
* @param    *val        variable
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The variable ospfAreaSummary controls the import of
*             summary LSAs into stub areas. It has no effect on other areas.
*
*             Valid outputs:
*               noAreaSummary (1)
*               sendAreaSummary (2)
*
*             If it is noAreaSummary, the router will neither originate
*             nor propagate summary LSAs into the stub area.  It will
*             rely entirely on its default route.
*
*             If it is sendAreaSummary, the router will both summarize
*             and propagate summary LSAs."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaSummarySetTest ( L7_uint32 areaId, L7_int32 val )
{
  t_Handle      ptr;

  /* Find the corresponding area entry */
  if ((ptr = ospfAreaTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,areaId)) != NULLP)
  {
    /* check if the value is within the correct settable range */
    if (ospfAreaTable_SetTest(ospfAreaTableAreaSummarySubId, val, ptr) == E_OK)
    {
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete the area summary.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    val         variable
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    If it is noAreaSummary, router will neither originate nor propagate
*           summary LSAs into stub area. It will rely on its default route.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaSummarySet(L7_uint32 areaId, L7_uint32 val)
{
  t_S_AreaCfg cfg;
  t_ARO         *p_ARO = NULL;
  e_Err         e;
  t_Handle      ptr;
  e_AreaSummaryCfg parm;
  L7_RC_t       rc = L7_FAILURE;

  e = HL_FindFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
                   (byte*)&areaId, (void**)&p_ARO);
  if ((e != E_OK) || (p_ARO == NULL))
    return L7_FAILURE;

  memset(&cfg, 0, sizeof(t_S_AreaCfg));
  cfg.AreaStatus = ROW_READ;

  if (ARO_Config_Pack(p_ARO, &cfg) != E_OK)
    return L7_FAILURE;

  /* map to vendor types */
  switch (val)
  {
  case L7_OSPF_AREA_NO_SUMMARY:
    parm = AREA_NO_SUMMARY;
    break;
  case L7_OSPF_AREA_SEND_SUMMARY:
    parm = AREA_SEND_SUMMARY;
    break;
  default:
    return L7_FAILURE;
    break;
  }

  cfg.AreaSummary = parm;
  cfg.AreaStatus = ROW_CHANGE;

  /* Find the corresponding area entry */
  if ((ptr = ospfAreaTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,areaId)) != NULLP)
  {
    /* set the row status in the ospf mib table to disable */
    if (ospfAreaTable_SetRowStatus(ospfMapMibCtrl_g.p_ospfMibObj, areaId,
                                   ROW_NOT_IN_SERVICE) == E_OK)
    {
      if ((e = ARO_Config_Pack(p_ARO, &cfg)) == E_OK)
        rc = L7_SUCCESS;

      /* reset the row status back to active in the ospf mib table */
      if (ospfAreaTable_SetRowStatus(ospfMapMibCtrl_g.p_ospfMibObj, areaId,
                                     ROW_ACTIVE) == E_OK)
      {
        return rc;
      }
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Test if the OSPF area entry status is settable.
*
* @param    areaId      type
* @param    val         entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaStatusSetTest ( L7_uint32 areaId, L7_int32 val )
{
  t_Handle      ptr;

  /* Find the corresponding area entry */
  if ((ptr = ospfAreaTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,areaId)) != NULLP)
  {
    /* check if the value is within the correct settable range */
    if (ospfAreaTable_SetTest(ospfAreaTableRowStatusSubId, val, ptr) == E_OK)
    {
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete the OSPF area entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    val         entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaStatusSet(L7_uint32 areaId, L7_uint32 val)
{
  t_S_AreaCfg cfg;
  t_ARO         *p_ARO = NULL;
  e_Err         e;
  e_A_RowStatus parm;

  e = HL_FindFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
                   (byte*)&areaId, (void**)&p_ARO);
  if ((e != E_OK) || (p_ARO == NULL))
    return L7_FAILURE;

  memset(&cfg, 0, sizeof(t_S_AreaCfg));
  cfg.AreaStatus = ROW_READ;

  if (ARO_Config_Pack(p_ARO, &cfg) != E_OK)
    return L7_FAILURE;

  switch (val)
  {
  case L7_OSPF_ROW_ACTIVE:
    parm = ROW_ACTIVE;
    break;
  case L7_OSPF_ROW_NOT_IN_SERVICE:
    parm = ROW_NOT_IN_SERVICE;
    break;
  case L7_OSPF_ROW_NOT_READY:
    parm = ROW_NOT_READY;
    break;
  case L7_OSPF_ROW_CREATE_AND_GO:
    parm = ROW_CREATE_AND_GO;
    break;
  case L7_OSPF_ROW_CREATE_AND_WAIT:
    parm = ROW_CREATE_AND_WAIT;
    break;
  case L7_OSPF_ROW_DESTROY:
    parm = ROW_DESTROY;
    break;
  default:
    return L7_FAILURE;
    break;
  }

  cfg.AreaStatus = parm;

  if (ARO_Config_Pack(p_ARO, &cfg) != E_OK)
    return L7_FAILURE;


  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Check the External Routing Capability for the specified Area
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) external routing capability val}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaExternalRoutingCapabilityGet(L7_uint32 areaId, L7_uint32 *val)
{
  t_S_AreaCfg cfg;

  if (aroCfgGet(areaId, &cfg) != L7_SUCCESS)
    return L7_FAILURE;

  *val = cfg.ExternalRoutingCapability;

  return L7_SUCCESS;
}

/* ospfStub */
/*********************************************************************
* @purpose  Get the area entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    TOS         entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenStubAreaEntryGet(L7_uint32 AreaId, L7_int32 TOS)
{
  t_Handle ospfStubAreaTablePtr = L7_NULLPTR;

  ospfStubAreaTablePtr = ospfStubAreaTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                  AreaId, TOS);
  if (ospfStubAreaTablePtr != L7_NULLPTR)
    return L7_SUCCESS;
  else
    return L7_FAILURE;

}

/*********************************************************************
* @purpose  Get the next area entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *AreaId     next area id
* @param    *TOS        next TOS
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenStubAreaEntryNext(L7_uint32 *AreaId, L7_int32 *TOS)
{
  t_Handle ospfStubAreaTablePtr = L7_NULLPTR;
  L7_uint32 prevAreaId;
  L7_int32  prevTOS;

  prevAreaId = *AreaId;
  prevTOS = *TOS;
  ospfStubAreaTablePtr = ospfStubAreaTable_GetNext(ospfMapMibCtrl_g.p_ospfMibObj,
                                                   prevAreaId, prevTOS,
                                                   (ulng *)AreaId, (ulng *)TOS);
  if (ospfStubAreaTablePtr != L7_NULLPTR)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the metric value.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    TOS         entry
* @param    *val        value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    By default, this equals the least metric at the type of
*           service among the interfaces to other areas.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenStubAreaMetricGet(L7_uint32 AreaId, L7_int32 TOS, L7_int32 *val)
{
  t_Handle ospfStubAreaTablePtr = L7_NULLPTR;

  ospfStubAreaTablePtr = ospfStubAreaTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                  AreaId, TOS);
  if (ospfStubAreaTablePtr != L7_NULLPTR)
  {
    *val = OSPFSTUBAREA_METRIC(ospfStubAreaTablePtr);
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Test to set the the metric value.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    TOS         entry
* @param    metric      value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    By default, this equals the least metric at the type of
*           service among the interfaces to other areas.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenStubAreaMetricSetTest(L7_uint32 AreaId, L7_int32 TOS, L7_uint32 metric)
{
  /* This field does not have a corresponding oid value assigned to it
     so it cannot be tested
  */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the status of entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *AreaId     type
* @param    TOS         entry
* @param    *val        entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenStubAreaStatusGet(L7_uint32 AreaId, L7_int32 TOS, L7_int32 *val)
{
  t_Handle ospfStubAreaTablePtr = L7_NULLPTR;

  ospfStubAreaTablePtr = ospfStubAreaTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                  AreaId, TOS);
  if (ospfStubAreaTablePtr != L7_NULLPTR)
  {
    *val = OSPFSTUBAREA_ROWSTATUS(ospfStubAreaTablePtr);
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;

}

/*********************************************************************
* @purpose  Test to set the status of entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *AreaId     type
* @param    TOS         entry
* @param    *val        entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenStubAreaStatusSetTest(L7_uint32 AreaId, L7_int32 TOS, L7_int32 val)
{
  t_Handle ospfStubAreaTablePtr = L7_NULLPTR;
  e_Err errRet;

  ospfStubAreaTablePtr = ospfStubAreaTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                  AreaId, TOS);
  if (ospfStubAreaTablePtr != L7_NULLPTR)
  {
    errRet = ospfStubAreaTable_SetTest(ospfStubAreaTableRowStatusSubId, val,
                                       ospfStubAreaTablePtr);
    if (errRet == E_OK)
      return L7_SUCCESS;
    else
      return L7_FAILURE;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  set the status of entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId     type
* @param    TOS         entry
* @param    val        entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenStubAreaStatusSet(L7_uint32 AreaId, L7_int32 TOS, L7_int32 val)
{
  t_S_StubAreaEntry cfg;
  t_ARO         *p_ARO = NULL;
  e_Err         e;
  e_A_RowStatus parm;

  e = HL_FindFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
                   (byte*)&AreaId, (void**)&p_ARO);
  if ((e != E_OK) || (p_ARO == NULL))
    return L7_FAILURE;

  memset(&cfg, 0, sizeof(t_S_StubAreaEntry));
  cfg.StubStatus = ROW_READ;

  if (ARO_StubMetric_Pack((t_Handle)p_ARO, &cfg) != E_OK)
    return L7_FAILURE;

  switch (val)
  {
  case L7_OSPF_ROW_ACTIVE:
    parm = ROW_ACTIVE;
    break;
  case L7_OSPF_ROW_NOT_IN_SERVICE:
    parm = ROW_NOT_IN_SERVICE;
    break;
  case L7_OSPF_ROW_NOT_READY:
    parm = ROW_NOT_READY;
    break;
  case L7_OSPF_ROW_CREATE_AND_GO:
    parm = ROW_CREATE_AND_GO;
    break;
  case L7_OSPF_ROW_CREATE_AND_WAIT:
    parm = ROW_CREATE_AND_WAIT;
    break;
  case L7_OSPF_ROW_DESTROY:
    parm = ROW_DESTROY;
    break;
  default:
    return L7_FAILURE;
    break;
  }

  cfg.StubStatus = parm;

  if (ARO_StubMetric_Pack((t_Handle)p_ARO, &cfg) != E_OK)
    return L7_FAILURE;


  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the type of metric set as a default route.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    TOS         entry
* @param    *val        metric type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenStubAreaMetricTypeGet(L7_uint32 AreaId, L7_int32 TOS, L7_int32 *val)
{
  t_Handle ospfStubAreaTablePtr = L7_NULLPTR;

  ospfStubAreaTablePtr = ospfStubAreaTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                  AreaId, TOS);
  if (ospfStubAreaTablePtr != L7_NULLPTR)
  {
    *val = OSPFSTUBAREA_METRICTYPE(ospfStubAreaTablePtr);
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Test to Set the stub metric type.
*
* @param    areaId      areaID
* @param    TOS         TOS         (L7_TOSTYPES)
* @param    metricType metric type (L7_OSPF_STUB_METRIC_TYPE_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the type of metric advertised as
*             a default route."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenStubAreaMetricTypeSetTest ( L7_uint32 areaId, L7_uint32 TOS, L7_uint32 metricType )
{
  t_Handle ospfStubAreaTablePtr = L7_NULLPTR;
  e_Err errRet;

  ospfStubAreaTablePtr = ospfStubAreaTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                  areaId, TOS);
  if (ospfStubAreaTablePtr != L7_NULLPTR)
  {
    errRet = ospfStubAreaTable_SetTest(ospfStubAreaTableMetricTypeSubId, metricType,
                                       ospfStubAreaTablePtr);
    if (errRet == E_OK)
      return L7_SUCCESS;
    else
      return L7_FAILURE;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Set the stub metric configuration for the areaID and TOS
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    TOS         entry
* @param    metric      metric
* @param    metricType  metric type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenStubAreaMetricCfgSet(L7_uint32 AreaId, L7_int32 TOS,
                                     L7_uint32 metric, L7_uint32 metricType)
{
  t_S_StubAreaEntry stubCfg;

  bzero((L7_uchar8 *)&stubCfg,sizeof(stubCfg));

  if (metricType == L7_OSPF_AREA_STUB_METRIC)
      stubCfg.StubMetricType = STUB_OSPF_METRIC;
  else
  if (metricType == L7_OSPF_AREA_STUB_COMPARABLE_COST)
      stubCfg.StubMetricType = STUB_COMPARABLE_COST;
  else
  if (metricType == L7_OSPF_AREA_STUB_NON_COMPARABLE_COST)
      stubCfg.StubMetricType = STUB_NON_COMPARABLE;
  else
      return L7_FAILURE;


  stubCfg.AreaId        = AreaId;
  stubCfg.StubMetric    = metric;
  stubCfg.StubTOS       = TOS;

  if (ospfMapExtenStubAreaEntryGet(AreaId,TOS) == L7_SUCCESS)
      stubCfg.StubStatus    = ROW_CHANGE;
  else
      stubCfg.StubStatus    = ROW_CREATE_AND_GO;

  if(stubCfgSet(AreaId, TOS, &stubCfg) != L7_SUCCESS)
      return L7_FAILURE;
  return L7_SUCCESS;

}


/*********************************************************************
* @purpose  Configures if the specified Area is a stub area
*
* @param    areaID  @b{(input)}
* @param    stubArea   @b{(input)}   L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if area has not been configured
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaIsStubAreaSet(L7_uint32 areaID, L7_uint32 stubArea)
{
  t_S_AreaCfg cfg;
  t_ARO         *p_ARO = NULL;
  e_Err         e;

  if(!areaID)
      return L7_FAILURE;

  e = HL_FindFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
                   (byte*)&areaID, (void**)&p_ARO);
  if ((e != E_OK) || (p_ARO == NULL))
    return L7_ERROR;

  memset(&cfg, 0, sizeof(t_S_AreaCfg));
  cfg.AreaStatus = ROW_READ;

  if (ARO_Config_Pack(p_ARO, &cfg) != E_OK)
    return L7_FAILURE;


  cfg.ExternalRoutingCapability = (stubArea == L7_TRUE) ? AREA_IMPORT_NO_EXTERNAL : AREA_IMPORT_EXTERNAL;
  cfg.AreaStatus = ROW_CHANGE;
  if (ARO_Config_Pack(p_ARO, &cfg) != E_OK)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the specified Stub Area
*
* @param    areaID  @b{(input)}
* @param    Stub Area Params   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if area has not been configured
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaStubAreaSet(L7_uint32 areaID, struct ospfAreaStubInfo_s *stubInfo)
{
  t_ARO         *p_ARO = NULL;
  e_Err         e;
  t_S_StubAreaEntry stubCfg;

  if(!areaID)
      return L7_FAILURE;

  e = HL_FindFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
                   (byte*)&areaID, (void**)&p_ARO);
  if ((e != E_OK) || (p_ARO == NULL))
    return L7_ERROR;

  memset(&stubCfg, 0, sizeof(stubCfg));
  stubCfg.StubStatus = ROW_CREATE_AND_GO;
  stubCfg.StubMetric = stubInfo->stubMetric;
  stubCfg.StubTOS = stubInfo->stubTOS;
  stubCfg.AreaId = areaID;
  stubCfg.StubMetricType = stubInfo->stubMetricType;

  if(ARO_StubMetric_Pack((t_Handle)p_ARO, &stubCfg) != E_OK)
      return L7_FAILURE;

  return L7_SUCCESS;
}

/* Queries for Routing Table Entries for "Border Routers" */

/*********************************************************************
* @purpose  ospfAbrEntry_Lookup_Pack is used to enqueue a 
*           request for getting a routing entry, to the ospf thread.
*
* @param    destinationIp      Router Id of the border router
* @param    p_rtbEntry         pointer to routing table entry
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err ospfAbrEntry_Lookup_Pack(L7_uint32 destinationIp,
                               L7_RtbEntryInfo_t *p_rtbEntry)
{
  t_XXCallInfo  *xxci;      /* XX_Call info */
  e_Err         e;
  L7_RC_t       rc = L7_FAILURE;
  L7_int32      currThread = osapiTaskIdSelf();
  OS_Thread     *pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

  if(p_rtbEntry == NULL)
  {
    return E_FAILED;
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }

    PACKET_INIT(xxci, ospfAbrEntry_Lookup_Unpack, 0, 0, 3, (L7_uint32)destinationIp);
    PACKET_PUT(xxci, (L7_RtbEntryInfo_t *)p_rtbEntry);
    PACKET_PUT(xxci, (L7_RC_t *)&rc);
    e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

    if (e != E_OK)
    {
      LOG_MSG("Failed to call ospfAbrEntry_Lookup_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
      return E_FAILED;
    }

    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }
  }
  else
  {
    rc = ospfAbrEntry_Lookup(destinationIp, p_rtbEntry);
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  }

  return (rc == L7_SUCCESS)? E_OK: E_FAILED;
}

/*********************************************************************
* @purpose  ospfAsbrEntry_Lookup_Pack is used to enqueue a 
*           request for getting a routing entry, to the ospf thread.
*
* @param    destinationIp      Router Id of the border router
* @param    p_rtbEntry         pointer to routing table entry
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err ospfAsbrEntry_Lookup_Pack(L7_uint32 destinationIp,
                                L7_RtbEntryInfo_t *p_rtbEntry)
{
  t_XXCallInfo  *xxci;      /* XX_Call info */
  e_Err         e;
  L7_RC_t       rc = L7_FAILURE;
  L7_int32      currThread = osapiTaskIdSelf();
  OS_Thread     *pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

  if(p_rtbEntry == NULL)
  {
    return E_FAILED;
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }

    PACKET_INIT(xxci, ospfAsbrEntry_Lookup_Unpack, 0, 0, 3, (L7_uint32)destinationIp);
    PACKET_PUT(xxci, (L7_RtbEntryInfo_t *)p_rtbEntry);
    PACKET_PUT(xxci, (L7_RC_t *)&rc);
    e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

    if (e != E_OK)
    {
      LOG_MSG("Failed to call ospfAsbrEntry_Lookup_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
      return E_FAILED;
    }

    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }
  }
  else
  {
    rc = ospfAsbrEntry_Lookup(destinationIp, p_rtbEntry);
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  }

  return (rc == L7_SUCCESS)? E_OK: E_FAILED;
}

/*********************************************************************
* @purpose  ospfAbrEntry_Lookup_Unpack is the callback function
*           to be called by the ospf thread for getting border router entry.
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err ospfAbrEntry_Lookup_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  ulng destinationIp = (ulng)PACKET_GET(xxCallInfo, 0);
  L7_RtbEntryInfo_t *p_rtbEntry = (L7_RtbEntryInfo_t *)PACKET_GET(xxCallInfo, 1);
  L7_RC_t *rc = (L7_RC_t *)PACKET_GET(xxCallInfo, 2);

  *rc = ospfAbrEntry_Lookup(destinationIp, p_rtbEntry);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return ((*rc == L7_SUCCESS)? E_OK: E_FAILED);
}

/*********************************************************************
* @purpose  ospfAsbrEntry_Lookup_Unpack is the callback function
*           to be called by the ospf thread for getting border router entry.
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err ospfAsbrEntry_Lookup_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  ulng destinationIp = (ulng)PACKET_GET(xxCallInfo, 0);
  L7_RtbEntryInfo_t *p_rtbEntry = (L7_RtbEntryInfo_t *)PACKET_GET(xxCallInfo, 1);
  L7_RC_t *rc = (L7_RC_t *)PACKET_GET(xxCallInfo, 2);

  *rc = ospfAsbrEntry_Lookup(destinationIp, p_rtbEntry);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return ((*rc == L7_SUCCESS)? E_OK: E_FAILED);
}

/*********************************************************************
* @purpose  Get the ABR Entry corresponding to destIp and
*           destIpMask
*
* @param    destinationIp      Router Id of the border router
* @param    p_rtbEntry         pointer to routing table entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfAbrEntry_Lookup(L7_uint32 destinationIp,
                            L7_RtbEntryInfo_t *p_rtbEntry)
{
  e_Err   e;
  t_RTO   *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;

  if((p_RTO == NULL) || (p_rtbEntry == NULL))
      return L7_FAILURE;

  if((e = RTO_AbrEntryGet(p_RTO, destinationIp,
                          p_rtbEntry)) != E_OK)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the ASBR Entry corresponding to destIp and
*           destIpMask
*
* @param    destinationIp      Router Id of the border router
* @param    p_rtbEntry         pointer to routing table entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfAsbrEntry_Lookup(L7_uint32 destinationIp,
                             L7_RtbEntryInfo_t *p_rtbEntry)
{
  e_Err   e;
  t_RTO   *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;

  if((p_RTO == NULL) || (p_rtbEntry == NULL))
      return L7_FAILURE;

  if((e = RTO_AsbrEntryGet(p_RTO, destinationIp,
                           p_rtbEntry)) != E_OK)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the ABR Entry corresponding to destIp and
*           destIpMask
*
* @param    destinationIp      Router Id of the ABR
* @param    p_rtbEntry         pointer to routing table entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAbrEntryGet ( L7_uint32 destinationIp,
                                  L7_RtbEntryInfo_t *p_rtbEntry )
{
  e_Err errRet;

  if ((errRet = ospfAbrEntry_Lookup_Pack(destinationIp,
                                         p_rtbEntry)) == E_OK)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the ASBR Entry corresponding to destIp and
*           destIpMask
*
* @param    destinationIp      Router Id of the ASBR
* @param    p_rtbEntry         pointer to routing table entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAsbrEntryGet ( L7_uint32 destinationIp,
                                   L7_RtbEntryInfo_t *p_rtbEntry )
{
  e_Err errRet;

  if ((errRet = ospfAsbrEntry_Lookup_Pack(destinationIp,
                                          p_rtbEntry)) == E_OK)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  ospfAbrEntryNext_Pack is used to enqueue a 
*           request for getting a routing entry, to the ospf thread.
*
* @param    destinationIp      Router Id of the border router
* @param    p_rtbEntry         pointer to routing table entry
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err ospfAbrEntryNext_Pack(L7_uint32 *destinationIp,
                            L7_RtbEntryInfo_t *p_rtbEntry)
{
  t_XXCallInfo  *xxci;      /* XX_Call info */
  e_Err         e;
  L7_RC_t       rc = L7_FAILURE;
  L7_int32      currThread = osapiTaskIdSelf();
  OS_Thread     *pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

  if(p_rtbEntry == NULL)
  {
    return E_FAILED;
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }

    PACKET_INIT(xxci, ospfAbrEntryNext_UnPack, 0, 0, 3, (L7_uint32)destinationIp);
    PACKET_PUT(xxci, (L7_RtbEntryInfo_t *)p_rtbEntry);
    PACKET_PUT(xxci, (L7_RC_t *)&rc);
    e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

    if (e != E_OK)
    {
      LOG_MSG("Failed to call ospfAbrEntryNext_UnPack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
      return E_FAILED;
    }

    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }
  }
  else
  {
    rc = ospfAbrEntryNext(destinationIp, p_rtbEntry);
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  }

  return (rc == L7_SUCCESS)? E_OK: E_FAILED;
}

/*********************************************************************
* @purpose  ospfAsbrEntryNext_Pack is used to enqueue a 
*           request for getting a routing entry, to the ospf thread.
*
* @param    destinationIp      Router Id of the border router
* @param    p_rtbEntry         pointer to routing table entry
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err ospfAsbrEntryNext_Pack(L7_uint32 *destinationIp,
                             L7_RtbEntryInfo_t *p_rtbEntry)
{
  t_XXCallInfo  *xxci;      /* XX_Call info */
  e_Err         e;
  L7_RC_t       rc = L7_FAILURE;
  L7_int32      currThread = osapiTaskIdSelf();
  OS_Thread     *pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

  if(p_rtbEntry == NULL)
  {
    return E_FAILED;
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }

    PACKET_INIT(xxci, ospfAsbrEntryNext_UnPack, 0, 0, 3, (L7_uint32)destinationIp);
    PACKET_PUT(xxci, (L7_RtbEntryInfo_t *)p_rtbEntry);
    PACKET_PUT(xxci, (L7_RC_t *)&rc);
    e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

    if (e != E_OK)
    {
      LOG_MSG("Failed to call ospfAsbrEntryNext_UnPack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
      return E_FAILED;
    }

    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }
  }
  else
  {
    rc = ospfAsbrEntryNext(destinationIp, p_rtbEntry);
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  }

  return (rc == L7_SUCCESS)? E_OK: E_FAILED;
}

/*********************************************************************
* @purpose  ospfAbrEntry_Lookup_Unpack is the callback function
*           to be called by the ospf thread for getting border router entry.
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err ospfAbrEntryNext_UnPack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  L7_uint32 *destinationIp = (L7_uint32 *)PACKET_GET(xxCallInfo, 0);
  L7_RtbEntryInfo_t *p_rtbEntry = (L7_RtbEntryInfo_t *)PACKET_GET(xxCallInfo, 1);
  L7_RC_t *rc = (L7_RC_t *)PACKET_GET(xxCallInfo, 2);

  *rc = ospfAbrEntryNext(destinationIp, p_rtbEntry);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return ((*rc == L7_SUCCESS)? E_OK: E_FAILED);
}

/*********************************************************************
* @purpose  ospfAsbrEntry_Lookup_Unpack is the callback function
*           to be called by the ospf thread for getting border router entry.
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err ospfAsbrEntryNext_UnPack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  L7_uint32 *destinationIp = (L7_uint32 *)PACKET_GET(xxCallInfo, 0);
  L7_RtbEntryInfo_t *p_rtbEntry = (L7_RtbEntryInfo_t *)PACKET_GET(xxCallInfo, 1);
  L7_RC_t *rc = (L7_RC_t *)PACKET_GET(xxCallInfo, 2);

  *rc = ospfAsbrEntryNext(destinationIp, p_rtbEntry);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return ((*rc == L7_SUCCESS)? E_OK: E_FAILED);
}

/*********************************************************************
* @purpose  Get the Next ABR Entry corresponding to destIp and
*           destIpMask
*
* @param    destinationIp      Router Id of the border router
* @param    p_rtbEntry         pointer to routing table entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfAbrEntryNext(L7_uint32 *destinationIp,
                         L7_RtbEntryInfo_t *p_rtbEntry)
{
  e_Err   e;
  t_RTO   *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;

  if((p_RTO == NULL) || (p_rtbEntry == NULL))
      return L7_FAILURE;
  if((e = RTO_AbrEntryNext(p_RTO, destinationIp,
                           p_rtbEntry)) != E_OK)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Next ASBR Entry corresponding to destIp and
*           destIpMask
*
* @param    destinationIp      Router Id of the border router
* @param    p_rtbEntry         pointer to routing table entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfAsbrEntryNext(L7_uint32 *destinationIp,
                          L7_RtbEntryInfo_t *p_rtbEntry)
{
  e_Err   e;
  t_RTO   *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;

  if((p_RTO == NULL) || (p_rtbEntry == NULL))
      return L7_FAILURE;
  if((e = RTO_AsbrEntryNext(p_RTO, destinationIp,
                            p_rtbEntry)) != E_OK)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the next ABR Entry corresponding to destIp and
*           destIpMask
*
* @param    destinationIp      Router Id of the ABR
* @param    p_rtbEntry         pointer to routing table entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAbrEntryNext ( L7_uint32 *destinationIp,
                                   L7_RtbEntryInfo_t *p_rtbEntry )
{
  e_Err errRet;

  if ((errRet = ospfAbrEntryNext_Pack(destinationIp,
                                      p_rtbEntry)) == E_OK)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next ASBR Entry corresponding to destIp and
*           destIpMask
*
* @param    destinationIp      Router Id of the ASBR
* @param    p_rtbEntry         pointer to routing table entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAsbrEntryNext ( L7_uint32 *destinationIp,
                                    L7_RtbEntryInfo_t *p_rtbEntry )
{
  e_Err errRet;

  if ((errRet = ospfAsbrEntryNext_Pack(destinationIp,
                                       p_rtbEntry)) == E_OK)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Ospf lsa-database LSA type for the specified Area
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) LSA Type}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSADbLSATypeGet(L7_uint32 areaId, L7_uint32 *val)
{
  t_S_LsDbEntry lsDbEntry;

  memset(&lsDbEntry, 0, sizeof(t_S_LsDbEntry));
  if (aroLSADbGet_Pack(areaId, &lsDbEntry) != L7_SUCCESS)
    return L7_FAILURE;

  *val = lsDbEntry.LsdbType;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Ospf lsa-database LSA Age for the specified Area
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) LSA Age}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSADbLSAAgeGet(L7_uint32 areaId, L7_int32 Type,
                                       L7_uint32 Lsid, L7_uint32 RouterId,
                                       L7_uint32 *val)
{
  e_Err errRet;
  L7_uint32 tmpType;
  L7_ospfLsdbEntry_t Lsa;

  switch (Type)
  {
  case L7_S_ILLEGAL_LSA:
    tmpType = S_ILLEGAL_LSA;
    break;

  case L7_S_ROUTER_LSA:
    tmpType = S_ROUTER_LSA;
    break;

  case L7_S_NETWORK_LSA:
    tmpType = S_NETWORK_LSA;
    break;

  case L7_S_IPNET_SUMMARY_LSA:
    tmpType = S_IPNET_SUMMARY_LSA;
    break;

  case L7_S_ASBR_SUMMARY_LSA:
    tmpType = S_ASBR_SUMMARY_LSA;
    break;

  case L7_S_AS_EXTERNAL_LSA:
    tmpType = S_AS_EXTERNAL_LSA;
    break;

  case L7_S_GROUP_MEMBER_LSA:
    tmpType = S_GROUP_MEMBER_LSA;
    break;

  case L7_S_NSSA_LSA:
    tmpType = S_NSSA_LSA;
    break;

  case L7_S_TMP2_LSA:
    tmpType = S_TMP2_LSA;
    break;

  case L7_S_LINK_OPAQUE_LSA:
    tmpType = S_LINK_OPAQUE_LSA;
    break;

  case L7_S_AREA_OPAQUE_LSA:
    tmpType = S_AREA_OPAQUE_LSA;
    break;

  case L7_S_AS_OPAQUE_LSA:
    tmpType = S_AS_OPAQUE_LSA;
    break;
  default:
    return L7_FAILURE;
  }

  if ((errRet = ospfLsdbTable_Lookup_Pack(areaId, tmpType, Lsid, RouterId,
                                     &Lsa)) == E_OK)
  {
    *val = OSPFLSDB_AGE(&Lsa);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Ospf lsa-database LSA LS Id for the specified Area
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) LSA LSId}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSADbLSALSIdGet(L7_uint32 areaId, L7_uint32 *val)
{
  t_S_LsDbEntry lsDbEntry;

  memset(&lsDbEntry, 0, sizeof(t_S_LsDbEntry));
  if (aroLSADbGet_Pack(areaId, &lsDbEntry) != L7_SUCCESS)
    return L7_FAILURE;

  *val = lsDbEntry.LsdbLsid;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Ospf lsa-database LSA Router Id for the specified
*           Area
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) LSA RouterId}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSADbLSARouterIdGet(L7_uint32 areaId, L7_uint32 *val)
{
  t_S_LsDbEntry lsDbEntry;

  memset(&lsDbEntry, 0, sizeof(t_S_LsDbEntry));
  if (aroLSADbGet_Pack(areaId, &lsDbEntry) != L7_SUCCESS)
    return L7_FAILURE;

  *val = lsDbEntry.LsdbRouterId;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Ospf lsa-database LSA Sequence for the specified
*           Area
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) LSA Sequence}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSADbLSASequenceGet(L7_uint32 areaId, L7_uint32 *val)
{
  t_S_LsDbEntry lsDbEntry;

  memset(&lsDbEntry, 0, sizeof(t_S_LsDbEntry));
  if (aroLSADbGet_Pack(areaId, &lsDbEntry) != L7_SUCCESS)
    return L7_FAILURE;

  *val = lsDbEntry.LsdbSequence;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Ospf lsa-database LSA Options for the specified
*           Area
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) LSA Options}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSADbLSAOptionsGet(L7_uint32 areaId, L7_int32 Type,
                                           L7_uint32 Lsid, L7_uint32 RouterId,
                                           L7_uchar8 *val)
{
  e_Err errRet;
  L7_ospfLsdbEntry_t Lsa;

  if ((errRet = ospfLsdbTable_Lookup_Pack(areaId, Type,
                                          Lsid, RouterId,
                                          &Lsa)) == E_OK)
  {
    *val = Lsa.LsdbOptions;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the flags field for a router LSA
*
* @param    areaId  @b{(input)}  area ID where router LSA is flooded
* @param    routerId @b{(input)} router ID of originating router
* @param    rtrLsaFlags  @b{(output)} flags field in router LSA
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenRouterLsaFlagsGet(L7_uint32 areaId, L7_uint32 routerId, 
                                      L7_uchar8 *rtrLsaFlags)
{
    e_Err errRet;
    L7_ospfLsdbEntry_t Lsa;

    if ((errRet = ospfLsdbTable_Lookup_Pack(areaId, S_ROUTER_LSA,
                                            routerId, routerId,
                                            &Lsa)) == E_OK)
    {
	*rtrLsaFlags = Lsa.rtrLsaFlags;
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Ospf lsa-database LSA Checksum for the specified Area
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) LSA Checksum}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSADbLSACksumGet(L7_uint32 areaId, L7_int32 Type,
                                         L7_uint32 Lsid, L7_uint32 RouterId,
                                         L7_uint32 *val)
{
  e_Err errRet;
  L7_uint32 tmpType;
  L7_ospfLsdbEntry_t Lsa;

  switch (Type)
  {
  case L7_S_ILLEGAL_LSA:
    tmpType = S_ILLEGAL_LSA;
    break;

  case L7_S_ROUTER_LSA:
    tmpType = S_ROUTER_LSA;
    break;

  case L7_S_NETWORK_LSA:
    tmpType = S_NETWORK_LSA;
    break;

  case L7_S_IPNET_SUMMARY_LSA:
    tmpType = S_IPNET_SUMMARY_LSA;
    break;

  case L7_S_ASBR_SUMMARY_LSA:
    tmpType = S_ASBR_SUMMARY_LSA;
    break;

  case L7_S_AS_EXTERNAL_LSA:
    tmpType = S_AS_EXTERNAL_LSA;
    break;

  case L7_S_GROUP_MEMBER_LSA:
    tmpType = S_GROUP_MEMBER_LSA;
    break;

  case L7_S_NSSA_LSA:
    tmpType = S_NSSA_LSA;
    break;

  case L7_S_TMP2_LSA:
    tmpType = S_TMP2_LSA;
    break;

  case L7_S_LINK_OPAQUE_LSA:
    tmpType = S_LINK_OPAQUE_LSA;
    break;

  case L7_S_AREA_OPAQUE_LSA:
    tmpType = S_AREA_OPAQUE_LSA;
    break;

  case L7_S_AS_OPAQUE_LSA:
    tmpType = S_AS_OPAQUE_LSA;
    break;
  default:
    return L7_FAILURE;
  }

  if ((errRet = ospfLsdbTable_Lookup_Pack(areaId, tmpType,
                                          Lsid, RouterId,
                                          &Lsa)) == E_OK)
  {
    *val = OSPFLSDB_CHKSUM(&Lsa);
    return L7_SUCCESS;
  }

  return L7_FAILURE;

}

/*********************************************************************
* @purpose  Get the Ospf lsa-database LSA Advertisement packet for the
*           specified Area
*
* @param    areaId      @b{(input)} area identifier
* @param    *val        pointer to output location
*                         @b{(output) LSA Advertisement Packet}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaLSADbLSAAdvertiseGet(L7_uint32 areaId, L7_int32 Type,
                                             L7_uint32 Lsid, L7_uint32 RouterId,
                                             L7_char8 **buf, L7_uint32 *buf_len)
{
  e_Err errRet;
  L7_uint32 tmpType;
  L7_ospfLsdbEntry_t Lsa;

  switch (Type)
  {
  case L7_S_ILLEGAL_LSA:
    tmpType = S_ILLEGAL_LSA;
    break;

  case L7_S_ROUTER_LSA:
    tmpType = S_ROUTER_LSA;
    break;

  case L7_S_NETWORK_LSA:
    tmpType = S_NETWORK_LSA;
    break;

  case L7_S_IPNET_SUMMARY_LSA:
    tmpType = S_IPNET_SUMMARY_LSA;
    break;

  case L7_S_ASBR_SUMMARY_LSA:
    tmpType = S_ASBR_SUMMARY_LSA;
    break;

  case L7_S_AS_EXTERNAL_LSA:
    tmpType = S_AS_EXTERNAL_LSA;
    break;

  case L7_S_GROUP_MEMBER_LSA:
    tmpType = S_GROUP_MEMBER_LSA;
    break;

  case L7_S_NSSA_LSA:
    tmpType = S_NSSA_LSA;
    break;

  case L7_S_TMP2_LSA:
    tmpType = S_TMP2_LSA;
    break;

  case L7_S_LINK_OPAQUE_LSA:
    tmpType = S_LINK_OPAQUE_LSA;
    break;

  case L7_S_AREA_OPAQUE_LSA:
    tmpType = S_AREA_OPAQUE_LSA;
    break;

  case L7_S_AS_OPAQUE_LSA:
    tmpType = S_AS_OPAQUE_LSA;
    break;
  default:
    return L7_FAILURE;
  }

  if ((errRet = ospfLsdbAdvertisement_Lookup_Pack(areaId, tmpType,
                                                  Lsid, RouterId,
                                                  &Lsa,
                                                  buf)) == E_OK)
  {
    *buf_len = OSPFLSDB_ADVERT_LEN(&Lsa);

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/* ospfLsdb */
/*********************************************************************
* @purpose  Get the process's Link State Database entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    Type        entry
* @param    Lsid        Link State id
* @param    RouterId    Database entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenLsdbEntryGet(L7_uint32 areaId, L7_uint32 Type,
                                 L7_uint32 Lsid, L7_uint32 RouterId,
                                 L7_ospfLsdbEntry_t *p_Lsa)
{
  e_Err errRet;

  if ((errRet = ospfLsdbTable_Lookup_Pack(areaId, Type, Lsid, RouterId,
                                          p_Lsa)) == E_OK)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next External Link State Database entry.
*
* @param    AreaId      Area Id
* @param    Type        Lsdb Type
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    p_Lsa	Lsdb Entry given to the external world
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfExtLsdbTable_GetNext(L7_uint32 *Type, L7_uint32 *Lsid,
                                 L7_uint32 *RouterId,
                                 L7_ospfLsdbEntry_t *p_Lsa)
{
  e_Err         e;
  L7_ospfLsdbEntry_t temp_Lsa;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  p_Lsa->LsdbLsid = *Lsid;
  p_Lsa->LsdbType = (L7_uchar8)S_AS_EXTERNAL_LSA; /* LS Type for External LSAs */
  p_Lsa->LsdbRouterId = *RouterId;

  /* In the same area, check for the next LSA entry */
  if((e = ARO_ExtLsdbNextEntry((t_RTO *)ospfMapCtrl_g.RTO_Id, p_Lsa)) != E_OK)
  {
    return L7_FAILURE;
  }

  /* Update the function parameters for the next LSA */
  *Type = p_Lsa->LsdbType;
  *Lsid = p_Lsa->LsdbLsid;
  *RouterId = p_Lsa->LsdbRouterId;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the next Link State Database entry.
*
* @param    AreaId      type
* @param    Type        entry
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    p_Lsa	Lsdb Entry given to the external world
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfLsdbTable_GetNext(L7_uint32 *areaId, L7_uint32 *Type,
                              L7_uint32 *Lsid, L7_uint32 *RouterId,
                              L7_ospfLsdbEntry_t *p_Lsa)
{
  t_RTO *p_RTO = (t_RTO *)ospfMapCtrl_g.RTO_Id;
  t_ARO         *p_ARO = NULL;
  e_Err         e;
  L7_ospfLsdbEntry_t temp_Lsa;
  t_Handle nextArea;
  L7_uint32 nextAreaId;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  /* First find the ARO object corresponding to the areaId */
  e = HL_FindFirst(p_RTO->AroHl, (byte*)areaId, (void**)&p_ARO);
  if ((e != E_OK) || (p_ARO == NULL))
  {
    /* Get the next area */
    nextArea = ospfAreaTable_GetNext(ospfMapMibCtrl_g.p_ospfMibObj,
                                     *areaId, (ulng *)&nextAreaId);
    if (nextArea == NULLP)
      return L7_FAILURE;
    e = HL_FindFirst(p_RTO->AroHl, (byte*)&nextAreaId, (void**)&p_ARO);
    if ((e != E_OK) || (p_ARO == NULL))
      return L7_FAILURE;
  }

  p_Lsa->LsdbLsid = *Lsid;
  p_Lsa->LsdbRouterId = *RouterId;
  p_Lsa->LsdbType = (L7_uchar8)(*Type);

  /* In the same area, check for the next LSA entry */
  if((e = ARO_LsdbNextEntry(p_ARO, p_Lsa)) != E_OK)
  {
    /* No more LSAs in the above area, so browse the
     * other Areas */
    while(1)
    {
      /* Get the Next Area */
      e = HL_GetNext(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
                     (void **)&p_ARO, (void*)p_ARO);
      if ((e != E_OK) || (p_ARO == NULL))
      {
        return L7_FAILURE;
      }

      /* Return the first LSA in this area */
      p_Lsa->LsdbLsid = p_Lsa->LsdbRouterId = p_Lsa->LsdbType = 0;

      if((e = ARO_LsdbNextEntry(p_ARO, p_Lsa)) == E_OK)
      {
        break;
      }
    }
  }

  /* Update the function parameters for the next LSA */
  *areaId = p_Lsa->AreaId;
  *Type = p_Lsa->LsdbType;
  *Lsid = p_Lsa->LsdbLsid;
  *RouterId = p_Lsa->LsdbRouterId;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the External Link State Database entry.
*
* @param    AreaId      type
* @param    Type        entry
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    p_Lsa	Lsdb Entry given to the external world
* @param    p_LsdbAdvertisement Lsdb Advertisement
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfExtLsdbAdvertisement_Lookup(L7_uint32 LsdbType, L7_uint32 LsdbId,
                                        L7_uint32 LsdbRouterId,
                                        L7_ospfLsdbEntry_t *p_Lsa,
                                        L7_char8 **p_LsdbAdvertisement)
{
  e_Err         e;
  L7_ospfLsdbEntry_t temp_Lsa;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  p_Lsa->LsdbLsid = LsdbId;
  p_Lsa->LsdbType = (L7_uchar8)S_AS_EXTERNAL_LSA; /* LS Type for External LSAs */
  p_Lsa->LsdbRouterId = LsdbRouterId;

  /* Find the ext Lsa corresponding to LsdbId, LsdbRouterId */
  if((e = ARO_ExtLsdbAdvertisement((t_RTO *)ospfMapCtrl_g.RTO_Id,
                                   p_Lsa,
                                   p_LsdbAdvertisement)) != E_OK)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the External Link State Database entry.
*
* @param    AreaId      type
* @param    Type        entry
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    p_Lsa	Lsdb Entry given to the external world
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfExtLsdbTable_Lookup(L7_uint32 LsdbType, L7_uint32 LsdbId,
                                L7_uint32 LsdbRouterId,
                                L7_ospfLsdbEntry_t *p_Lsa)
{
  e_Err         e;
  L7_ospfLsdbEntry_t temp_Lsa;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  p_Lsa->LsdbLsid = LsdbId;
  p_Lsa->LsdbType = (L7_uchar8)S_AS_EXTERNAL_LSA; /* LS Type for External LSAs */
  p_Lsa->LsdbRouterId = LsdbRouterId;

  /* Find the ext Lsa corresponding to LsdbId, LsdbRouterId */
  if((e = ARO_ExtLsdbEntry((t_RTO *)ospfMapCtrl_g.RTO_Id, p_Lsa)) != E_OK)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Link State Database entry.
*
* @param    AreaId      Area Id
* @param    Type        Lsdb Type
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    p_Lsa	Lsdb Entry given to the external world
* @param    p_LsdbAdvertisement  Lsdb Advertisement
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfLsdbAdvertisement_Lookup(L7_uint32 LsdbAreaId, L7_uint32 LsdbType,
                                    L7_uint32 LsdbId, L7_uint32 LsdbRouterId,
                                    L7_ospfLsdbEntry_t *p_Lsa,
                                    L7_char8 **p_LsdbAdvertisement)
{
  t_ARO         *p_ARO = NULL;
  e_Err         e;
  L7_ospfLsdbEntry_t temp_Lsa;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  if ((LsdbType != S_AS_EXTERNAL_LSA) && (LsdbType != S_AS_OPAQUE_LSA))
  {
    /* First find the ARO object corresponding to the LsdbAreaId */
    e = HL_FindFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
                     (byte*)&LsdbAreaId, (void**)&p_ARO);
    if ((e != E_OK) || (p_ARO == NULL))
    {
      return L7_FAILURE;
    }
  }

  p_Lsa->LsdbLsid = LsdbId;
  p_Lsa->LsdbType = (L7_uchar8)LsdbType;
  p_Lsa->LsdbRouterId = LsdbRouterId;

  /* Find the Lsa corresponding to LsdbType, LsdbId, LsdbRouterId */
  if((e = ARO_LsdbAdvertisement(p_ARO, p_Lsa, p_LsdbAdvertisement)) != E_OK)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Link State Database entry.
*
* @param    AreaId      Area Id
* @param    Type        Lsdb Type
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    p_Lsa	Lsdb Entry given to the external world
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfLsdbTable_Lookup(L7_uint32 LsdbAreaId, L7_uint32 LsdbType,
                                L7_uint32 LsdbId, L7_uint32 LsdbRouterId,
                                L7_ospfLsdbEntry_t *p_Lsa)
{
  t_ARO         *p_ARO = NULL;
  e_Err         e;
  L7_ospfLsdbEntry_t temp_Lsa;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  /* First find the ARO object corresponding to the LsdbAreaId */
  e = HL_FindFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
                   (byte*)&LsdbAreaId, (void**)&p_ARO);
  if ((e != E_OK) || (p_ARO == NULL))
  {
    return L7_FAILURE;
  }

  p_Lsa->LsdbLsid = LsdbId;
  p_Lsa->LsdbType = (L7_uchar8)LsdbType;
  p_Lsa->LsdbRouterId = LsdbRouterId;

  /* Find the Lsa corresponding to LsdbType, LsdbId, LsdbRouterId */
  if((e = ARO_LsdbEntry(p_ARO, p_Lsa)) != E_OK)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the next Link State Database entry.
*
* @param    *AreaId     Area Id
* @param    *Type       Lsdb Type
* @param    *Lsid       Link State id
* @param    *RouterId   Database entry
* @param    *p_Lsa	Lsdb Entry given to the external world
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenLsdbEntryNext(L7_uint32 *areaId, L7_uint32 *Type,
                                  L7_uint32 *Lsid, L7_uint32 *RouterId,
                                  L7_ospfLsdbEntry_t *p_Lsa)
{
  e_Err errRet;

  if ((errRet = ospfLsdbTable_GetNext_Pack(areaId, Type,
                                           Lsid, RouterId,
                                           p_Lsa)) == E_OK)
  {
    switch (*Type)
    {
    case S_ILLEGAL_LSA:
      *Type = L7_S_ILLEGAL_LSA;
      break;

    case S_ROUTER_LSA:
      *Type = L7_S_ROUTER_LSA;
      break;

    case S_NETWORK_LSA:
      *Type = L7_S_NETWORK_LSA;
      break;

    case S_IPNET_SUMMARY_LSA:
      *Type = L7_S_IPNET_SUMMARY_LSA;
      break;

    case S_ASBR_SUMMARY_LSA:
      *Type = L7_S_ASBR_SUMMARY_LSA;
      break;

    case S_AS_EXTERNAL_LSA:
      *Type = L7_S_AS_EXTERNAL_LSA;
      break;

    case S_GROUP_MEMBER_LSA:
      *Type = L7_S_GROUP_MEMBER_LSA;
      break;

    case S_NSSA_LSA:
      *Type = L7_S_NSSA_LSA;
      break;

    case S_TMP2_LSA:
      *Type = L7_S_TMP2_LSA;
      break;

    case S_LINK_OPAQUE_LSA:
      *Type = L7_S_LINK_OPAQUE_LSA;
      break;

    case S_AREA_OPAQUE_LSA:
      *Type = L7_S_AREA_OPAQUE_LSA;
      break;

    case S_AS_OPAQUE_LSA:
      *Type = L7_S_AS_OPAQUE_LSA;
      break;
    default:
      return L7_FAILURE;
    }

    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the sequence number.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    Type        entry
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    *val        sequence number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    sequence number field is a signed 32-bit integer.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenLsdbSequenceGet(L7_uint32 areaId, L7_int32 Type,
                                    L7_uint32 Lsid, L7_uint32 RouterId,
                                    L7_uint32 *val)
{
  e_Err errRet;
  L7_uint32 tmpType;
  L7_ospfLsdbEntry_t Lsa;

  switch (Type)
  {
  case L7_S_ILLEGAL_LSA:
    tmpType = S_ILLEGAL_LSA;
    break;

  case L7_S_ROUTER_LSA:
    tmpType = S_ROUTER_LSA;
    break;

  case L7_S_NETWORK_LSA:
    tmpType = S_NETWORK_LSA;
    break;

  case L7_S_IPNET_SUMMARY_LSA:
    tmpType = S_IPNET_SUMMARY_LSA;
    break;

  case L7_S_ASBR_SUMMARY_LSA:
    tmpType = S_ASBR_SUMMARY_LSA;
    break;

  case L7_S_AS_EXTERNAL_LSA:
    tmpType = S_AS_EXTERNAL_LSA;
    break;

  case L7_S_GROUP_MEMBER_LSA:
    tmpType = S_GROUP_MEMBER_LSA;
    break;

  case L7_S_NSSA_LSA:
    tmpType = S_NSSA_LSA;
    break;

  case L7_S_TMP2_LSA:
    tmpType = S_TMP2_LSA;
    break;

  case L7_S_LINK_OPAQUE_LSA:
    tmpType = S_LINK_OPAQUE_LSA;
    break;

  case L7_S_AREA_OPAQUE_LSA:
    tmpType = S_AREA_OPAQUE_LSA;
    break;

  case L7_S_AS_OPAQUE_LSA:
    tmpType = S_AS_OPAQUE_LSA;
    break;
  default:
    return L7_FAILURE;
  }

  if ((errRet = ospfLsdbTable_Lookup_Pack(areaId, tmpType, Lsid, RouterId,
                                          &Lsa)) == E_OK)
  {
    *val = OSPFLSDB_SEQUENCE(&Lsa);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/* ospfExtLsdb */
/*********************************************************************
* @purpose  Get a single Link State entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExtLsdbEntryGet(L7_uint32 Type, L7_uint32 Lsid,
                                    L7_uint32 RouterId,
                                    L7_ospfLsdbEntry_t *p_Lsa)
{
  e_Err errRet;

  if ((errRet = ospfExtLsdbTable_Lookup_Pack(Type, Lsid,
                                             RouterId,
                                             p_Lsa)) == E_OK)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next Link State entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *Type       link state type
* @param    *Lsid       link state id
* @param    *RouterId   router id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExtLsdbEntryNext(L7_uint32 *Type, L7_uint32 *Lsid,
                                     L7_uint32 *RouterId,
                                     L7_ospfLsdbEntry_t *p_Lsa)
{
  e_Err errRet;

  if ((errRet = ospfExtLsdbTable_GetNext_Pack(Type, Lsid, RouterId,
                                              p_Lsa)) == E_OK)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the sequence number field.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
* @param    *val        sequence number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    It is used to detect old and duplicate link state advertisements.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExtLsdbSequenceGet(L7_int32 Type, L7_uint32 Lsid,
                                       L7_uint32 RouterId, L7_uint32 *val)
{
  e_Err errRet;
  L7_ospfLsdbEntry_t Lsa;

  if ((errRet = ospfExtLsdbTable_Lookup_Pack(Type, Lsid, RouterId,
                                             &Lsa)) == E_OK)
  {
    *val = OSPFLSDB_SEQUENCE(&Lsa);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the age of the link state advertisement in seconds.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
* @param    *val        age in seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExtLsdbAgeGet(L7_int32 Type, L7_uint32 Lsid, L7_uint32 RouterId, L7_int32 *val)
{
  e_Err errRet;
  L7_ospfLsdbEntry_t Lsa;

  if ((errRet = ospfExtLsdbTable_Lookup_Pack(Type, Lsid, RouterId,
                                             &Lsa)) == E_OK)
  {
    *val = OSPFLSDB_AGE(&Lsa);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the checksum of the complete contents of the advertisement,
*           excepting the age field.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
* @param    *val        checksum
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExtLsdbChecksumGet(L7_int32 Type, L7_uint32 Lsid, L7_uint32 RouterId, L7_int32 *val)
{
  e_Err errRet;
  L7_ospfLsdbEntry_t Lsa;

  if ((errRet = ospfExtLsdbTable_Lookup_Pack(Type, Lsid, RouterId,
                                             &Lsa)) == E_OK)
  {
    *val = OSPFLSDB_CHKSUM(&Lsa);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the entire Link State Advertisement, including its header.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
* @param    *buf        link state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExtLsdbAdvertisementGet(L7_int32 Type, L7_uint32 Lsid, L7_uint32 RouterId, L7_char8 **buf, L7_uint32 *len)
{
  e_Err errRet;
  L7_ospfLsdbEntry_t Lsa;

  if ((errRet = ospfExtLsdbAdvertisement_Lookup_Pack(Type, Lsid, RouterId,
                                                     &Lsa,
                                                     buf)) == E_OK)
  {
    *len = OSPFLSDB_ADVERT_LEN(&Lsa);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the length of the link state advertisement.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
* @param    *val        length
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenExtLsdbAdvertisementLengthGet(L7_int32 Type, L7_uint32 Lsid, L7_uint32 RouterId, L7_uint32 *val)
{
  e_Err errRet;
  L7_ospfLsdbEntry_t Lsa;

  if ((errRet = ospfExtLsdbTable_Lookup_Pack(Type, Lsid, RouterId,
                                             &Lsa)) == E_OK)
  {
    *val = OSPFLSDB_ADVERT_LEN(&Lsa);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/* ospfAreaAggregate */
/*********************************************************************
* @purpose  Get the entry in range of IP addresses specified by an IP address/IP network mask pair.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaID      area id
* @param    Type        link state type
* @param    Net         network
* @param    Mask        mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaAggrEntryGet(L7_uint32 AreaID, L7_uint32 Type, L7_int32 Net, L7_uint32 Mask)
{
  t_Handle ospfAreaAggrTablePtr = L7_NULLPTR;
  ospfAreaAggrTablePtr = ospfAreaAggrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                  AreaID, Type, Net, Mask);

  if (ospfAreaAggrTablePtr != L7_NULLPTR)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}


/*********************************************************************
* @purpose  Get the next entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *AreaID     area id
* @param    *Type       link state type
* @param    *Net        network
* @param    *Mask       mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaAggrEntryNext(L7_uint32 *AreaID, L7_uint32 *Type, L7_int32 *Net, L7_uint32 *Mask)
{
  t_Handle ospfAreaAggrTablePtr = L7_NULLPTR;
  L7_uint32 prevAreaId, prevType, prevNet, prevMask;

  prevAreaId = *AreaID;
  prevType = *Type;
  prevNet = *Net;
  prevMask = *Mask;
  ospfAreaAggrTablePtr = ospfAreaAggrTable_GetNext(ospfMapMibCtrl_g.p_ospfMibObj,
                                                   prevAreaId, prevType, prevNet,
                                                   prevMask, (ulng *)AreaID,
                                                   (ulng *)Type, (ulng *)Net,
                                                   (ulng *)Mask);

  if (ospfAreaAggrTablePtr != L7_NULLPTR)
    return L7_SUCCESS;
  else
    return L7_FAILURE;

}

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaID      area id
* @param    Type        link state type
* @param    Net         network
* @param    Mask        mask
* @param    *val        entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaAggrStatusGet(L7_uint32 AreaID, L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, L7_int32 *val)
{
  t_Handle ospfAreaAggrTablePtr = L7_NULLPTR;
  ospfAreaAggrTablePtr = ospfAreaAggrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                  AreaID, Type, Net, Mask);
  if (ospfAreaAggrTablePtr != L7_NULLPTR)
  {
    *val = OSPFAREAAGGR_ROWSTATUS( ospfAreaAggrTablePtr );
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the subnet effect.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaID      area id
* @param    Type        link state type
* @param    Net         network
* @param    Mask        mask
* @param    *val        subnet effect
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Subnets subsumed by ranges either trigger the advertisement
*           of indicated aggregate, or result in subnet's not being
*           advertised at all outside the area.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaAggrEffectGet(L7_uint32 AreaID, L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, L7_int32 *val)
{
  t_S_AreaAggregateEntry agrEntry;

  if(areaAggregateGet(AreaID, Type, Net, Mask, &agrEntry) != L7_SUCCESS)
    return L7_FAILURE;

  *val = agrEntry.AggregateEffect;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Test to set the entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaID      area id
* @param    Type        link state type
* @param    Net         network
* @param    Mask        mask
* @param    val         entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaAggrStatusSetTest(L7_uint32 AreaID, L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, L7_int32 val)
{
  t_Handle ospfAreaAggrTablePtr = L7_NULLPTR;
  e_Err errRet;

  ospfAreaAggrTablePtr = ospfAreaAggrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                              AreaID, Type, Net, Mask);
  if (ospfAreaAggrTablePtr != L7_NULLPTR)
  {
    errRet = ospfAreaAggrTable_SetTest(ospfAreaAggrTableRowStatusSubId, val,
                                       ospfAreaAggrTablePtr);
    if (errRet == E_OK)
      return L7_SUCCESS;
    else
      return L7_FAILURE;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete the entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaID      area id
* @param    Type        link state type
* @param    Net         network
* @param    Mask        mask
* @param    val         entry status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaAggrStatusSet(L7_uint32 AreaID, L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, L7_int32 val)
{
  t_S_AreaAggregateEntry agrEntry;
  e_A_RowStatus parm;

  switch (val)
  {
  case L7_OSPF_ROW_ACTIVE:
    parm = ROW_ACTIVE;
    break;
  case L7_OSPF_ROW_NOT_IN_SERVICE:
    parm = ROW_NOT_IN_SERVICE;
    break;
  case L7_OSPF_ROW_NOT_READY:
    parm = ROW_NOT_READY;
    break;
  case L7_OSPF_ROW_CREATE_AND_GO:
    parm = ROW_CREATE_AND_GO;
    break;
  case L7_OSPF_ROW_CREATE_AND_WAIT:
    parm = ROW_CREATE_AND_WAIT;
    break;
  case L7_OSPF_ROW_DESTROY:
    parm = ROW_DESTROY;
    break;
  default:
    return L7_FAILURE;
    break;
  }

  if(areaAggregateGet(AreaID, Type, Net, Mask, &agrEntry) != L7_SUCCESS)
    return L7_FAILURE;

  agrEntry.AggregateStatus = parm;
  if(areaAggregateSet(AreaID, Type, Net, Mask, &agrEntry) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Test to set the subnet effect.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaID      area id
* @param    Type        link state type
* @param    Net         network
* @param    Mask        mask
* @param    val         subnet effect
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Subnets subsumed by ranges either trigger the advertisement
*           of indicated aggregate, or result in subnet's not being
*           advertised at all outside the area.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaAggrEffectSetTest(L7_uint32 AreaID, L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, L7_int32 val)
{
  t_Handle ospfAreaAggrTablePtr = L7_NULLPTR;
  e_Err errRet;

  ospfAreaAggrTablePtr = ospfAreaAggrTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                              AreaID, Type, Net, Mask);
  if (ospfAreaAggrTablePtr != L7_NULLPTR)
  {
    errRet = ospfAreaAggrTable_SetTest(ospfAreaAggrTableEffectSubId, val,
                                       ospfAreaAggrTablePtr);
    if (errRet == E_OK)
      return L7_SUCCESS;
    else
      return L7_FAILURE;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete the subnet effect.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaID      area id
* @param    Type        link state type
* @param    Net         network
* @param    Mask        mask
* @param    val         subnet effect
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Subnets subsumed by ranges either trigger the advertisement
*           of indicated aggregate, or result in subnet's not being
*           advertised at all outside the area.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaAggrEffectSet(L7_uint32 AreaID, L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, L7_int32 val)
{
  t_S_AreaAggregateEntry agrEntry;

  if(areaAggregateGet(AreaID, Type, Net, Mask, &agrEntry) != L7_SUCCESS)
    return L7_FAILURE;

  agrEntry.AggregateStatus = ROW_CHANGE;
  agrEntry.AggregateEffect = val;
  if(areaAggregateSet(AreaID, Type, Net, Mask, &agrEntry) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Get the active/inactive status of the area aggregate range
*
* @param    areaId      type
* @param    lsdbType    LSDB Type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    ipAddr      ip address
* @param    netMask     mask to specify area range
*
* @returns  L7_SUCCESS  if active
* @returns  L7_FAILURE  otherwise
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeEntryStatusGet ( L7_uint32 areaId,
                                              L7_uint32 lsdbType,
                                              L7_int32 ipAddr,
                                              L7_uint32 netMask)
{
  t_S_AreaAggregateEntry agrEntry;
  t_ARO         *p_ARO = NULL;
  e_Err         e;

  e = HL_FindFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
                   (byte*)&areaId, (void**)&p_ARO);
  if ((e != E_OK) || (p_ARO == NULL))
    return L7_FALSE;

  memset(&agrEntry, 0, sizeof(t_S_AreaAggregateEntry));

  agrEntry.AreaId        = areaId;
  agrEntry.LsdbType      = lsdbType;
  agrEntry.NetIpAdr      = ipAddr;
  agrEntry.SubnetMask    = netMask;
  agrEntry.AggregateStatus = ROW_READ;

  e =  ARO_AddressRange_Pack(&agrEntry);
  if(e == E_OK){
         return L7_SUCCESS;
  }

  return L7_FAILURE;
}


/*********************************************************************
* @purpose  Get the area range entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    RangeNet    range entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeEntryGet(L7_uint32 AreaId, L7_uint32 RangeNet)
{
  t_S_AreaAggregateEntry agrEntry;

  if(areaRangeGet(AreaId, RangeNet, &agrEntry) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the next area range entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *AreaId     type
* @param    *RangeNet   range entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if there are no more entries in the table
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeEntryNext(L7_uint32 *AreaID, L7_uint32 *RangeNet)
{
  t_S_AreaAggregateEntry agrEntry;
  t_ARO         *p_ARO = NULL;
  e_Err         e;

  e = HL_FindFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
                   (byte*)AreaID, (void**)&p_ARO);
  if ((e != E_OK) || (p_ARO == NULL))
    return L7_ERROR;

  memset(&agrEntry, 0, sizeof(t_S_AreaAggregateEntry));

  /* Iterate thorough all area range entries for the specified area id */
  while(e == E_OK)
  {
     agrEntry.AggregateStatus = ROW_READ_NEXT;
     if ((e =  ARO_AddressRange_Pack(&agrEntry)) == E_OK)
     {
       /* If range net == 0, assume that the calling function is trying to
          get the first area range entry for the given area id */
       if (*RangeNet == 0)
       {
         *AreaID = agrEntry.AreaId;
         *RangeNet = agrEntry.NetIpAdr;
         return L7_SUCCESS;
       }

       /* If we have found an exact match, try getting the next entry */
       if((agrEntry.AreaId == *AreaID) && (agrEntry.NetIpAdr == *RangeNet))
       {
         if (ARO_AddressRange_Pack(&agrEntry) == E_OK)
         {
           *AreaID = agrEntry.AreaId;
           *RangeNet = agrEntry.NetIpAdr;
           return L7_SUCCESS;
         }
       }
     }
  }

  /* If we could not find an exact match or if the range net == 0 but the
     specified area does not have any area range entries return failure */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Subnet Mask that pertains to the Net or Subnet.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    RangeNet    range entry
* @param    *val        IpAddress
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeMaskGet(L7_uint32 AreaId, L7_uint32 RangeNet, L7_uint32 *val)
{
  t_S_AreaAggregateEntry agrEntry;

  if(areaRangeGet(AreaId, RangeNet, &agrEntry) != L7_SUCCESS)
    return L7_FAILURE;

  *val = agrEntry.SubnetMask;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    RangeNet    range entry
* @param    *val        row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeStatusGet(L7_uint32 AreaId, L7_uint32 RangeNet, L7_int32 *val)
{
  t_S_AreaAggregateEntry agrEntry;

  if(areaRangeGet(AreaId, RangeNet, &agrEntry) != L7_SUCCESS)
    return L7_FAILURE;

  *val = agrEntry.AggregateStatus;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the area range effect.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    RangeNet    range entry
* @param    *val        effect
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Subnets subsumed by ranges either trigger the advertisement
*           of the indicated summary, or result in the subnet's not
*           being advertised at all outside the area.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeEffectGet(L7_uint32 AreaId, L7_uint32 RangeNet, L7_int32 *val)
{
  t_S_AreaAggregateEntry agrEntry;

  if(areaRangeGet(AreaId, RangeNet, &agrEntry) != L7_SUCCESS)
    return L7_FAILURE;

  *val = agrEntry.AggregateEffect;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete the Subnet Mask that pertains to the Net or Subnet.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    RangeNet    range entry
* @param    val         IpAddress
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeMaskSet(L7_uint32 AreaId, L7_uint32 RangeNet, L7_uint32 val)
{
  t_S_AreaAggregateEntry agrEntry;

  if(areaRangeGet(AreaId, RangeNet, &agrEntry) != L7_SUCCESS)
    return L7_FAILURE;

  agrEntry.SubnetMask = val;
  agrEntry.AggregateStatus = ROW_CHANGE;
  if(areaRangeSet(AreaId, RangeNet, &agrEntry) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete the entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    RangeNet    range entry
* @param    val         row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeStatusSet(L7_uint32 AreaId, L7_uint32 RangeNet, L7_int32 val)
{
  t_S_AreaAggregateEntry agrEntry;

  if(areaRangeGet(AreaId, RangeNet, &agrEntry) != L7_SUCCESS)
    return L7_FAILURE;

  agrEntry.AggregateStatus = val;
  if(areaRangeSet(AreaId, RangeNet, &agrEntry) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete the area range effect.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    AreaId      type
* @param    RangeNet    range entry
* @param    *val        effect
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaRangeEffectSet(L7_uint32 AreaId, L7_uint32 RangeNet, L7_int32 val)
{
  t_S_AreaAggregateEntry agrEntry;

  if(areaRangeGet(AreaId, RangeNet, &agrEntry) != L7_SUCCESS)
    return L7_FAILURE;

  agrEntry.AggregateEffect = val;
  agrEntry.AggregateStatus = ROW_CHANGE;
  if(areaRangeSet(AreaId, RangeNet, &agrEntry) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}
/* ospfHost */
/*********************************************************************
* @purpose  Get the host entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    TOS         host entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/

L7_RC_t ospfMapExtenHostEntryGet(L7_uint32 IpAddress, L7_int32 TOS )
{
  t_Handle ospfHostTablePtr = L7_NULLPTR;

  ospfHostTablePtr = ospfHostTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                  IpAddress, TOS);
  if (ospfHostTablePtr != L7_NULLPTR)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next host entry.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    *IPAddress  Ip address
* @param    *TOS        host entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    needs AreaId too.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenHostEntryNext(L7_uint32 *IpAddress, L7_int32 *TOS )
{
  t_Handle ospfHostTablePtr = L7_NULLPTR;
  L7_uint32 prevIpAddress;
  L7_int32  prevTOS;

  prevIpAddress = *IpAddress;
  prevTOS = *TOS;
  ospfHostTablePtr = ospfHostTable_GetNext(ospfMapMibCtrl_g.p_ospfMibObj,
                                           prevIpAddress, prevTOS,
                                           (ulng *)IpAddress, (ulng *)TOS);
  if (ospfHostTablePtr != L7_NULLPTR)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}


/*********************************************************************
* @purpose  Get the Metric to be advertised.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    *val        metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenHostMetricGet(L7_uint32 IpAddress, L7_int32 TOS, L7_int32 *val )
{
  t_Handle ospfHostTablePtr = L7_NULLPTR;

  ospfHostTablePtr = ospfHostTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                  IpAddress, TOS);
  if (ospfHostTablePtr != L7_NULLPTR)
  {
    *val = OSPFHOST_METRIC(ospfHostTablePtr);
  }

  if (ospfHostTablePtr != L7_NULLPTR)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    *val        row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenHostStatusGet(L7_uint32 IpAddress, L7_int32 TOS, L7_int32 *val )
{
  t_Handle ospfHostTablePtr = L7_NULLPTR;

  ospfHostTablePtr = ospfHostTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                  IpAddress, TOS);
  if (ospfHostTablePtr != L7_NULLPTR)
  {
    *val = OSPFHOST_ROWSTATUS(ospfHostTablePtr);
  }

  if (ospfHostTablePtr != L7_NULLPTR)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}


/*********************************************************************
* @purpose  Test to set the Metric.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    val         metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenHostMetricSetTest( L7_uint32 IpAddress, L7_int32 TOS, L7_int32 val )
{
  /* The OID for metric field in host table is not provided in the code.
     So cannot be set.
  */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete the Metric.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    val         metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenHostMetricSet( L7_uint32 IpAddress, L7_int32 TOS, L7_int32 val )
{
  t_S_HostRouteCfg Hosts;

  t_ARO         *p_ARO = NULL;
  e_Err         e;

  e = HL_GetFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl, (void**)&p_ARO);
  while(e == E_OK)
  {
     memset(&Hosts, 0, sizeof(t_S_HostRouteCfg));


     while(e == E_OK)
     {
        Hosts.HostStatus = ROW_READ_NEXT;
        e =  ARO_HostConfig_Pack(p_ARO, &Hosts);
        if(e == E_OK){
           if((Hosts.HostIpAdr == IpAddress) && (Hosts.HostTOS == TOS) )
           {
             Hosts.HostMetric = val;
             Hosts.HostStatus = ROW_CHANGE;
             if(ARO_HostConfig_Pack(p_ARO, &Hosts) != E_OK)
                return L7_FAILURE;
             return L7_SUCCESS;
           }
        }


     }
     e = HL_GetNext(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl, (void *)&p_ARO, (void**)&p_ARO);
  }
  return L7_FAILURE;


}

/*********************************************************************
* @purpose  Test to set the entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    val         row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenHostStatusSetTest(L7_uint32 IpAddress, L7_int32 TOS, L7_int32 val )
{
  t_Handle ospfHostTablePtr = L7_NULLPTR;
  e_Err errRet;

  ospfHostTablePtr = ospfHostTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                  IpAddress, TOS);
  if (ospfHostTablePtr != L7_NULLPTR)
  {
    errRet = ospfHostTable_SetTest(ospfHostTableRowStatusSubId, val,
                                       ospfHostTablePtr);
    if (errRet == E_OK)
      return L7_SUCCESS;
    else
      return L7_FAILURE;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete the entry status.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    val         row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Setting it to 'invalid' makes it inoperative.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenHostStatusSet(L7_uint32 IpAddress, L7_int32 TOS, L7_int32 val )
{
  t_S_HostRouteCfg Hosts;

  t_ARO         *p_ARO = NULL;
  e_Err         e;
  e_A_RowStatus parm;

  switch (val)
  {
  case L7_OSPF_ROW_ACTIVE:
    parm = ROW_ACTIVE;
    break;
  case L7_OSPF_ROW_NOT_IN_SERVICE:
    parm = ROW_NOT_IN_SERVICE;
    break;
  case L7_OSPF_ROW_NOT_READY:
    parm = ROW_NOT_READY;
    break;
  case L7_OSPF_ROW_CREATE_AND_GO:
    parm = ROW_CREATE_AND_GO;
    break;
  case L7_OSPF_ROW_CREATE_AND_WAIT:
    parm = ROW_CREATE_AND_WAIT;
    break;
  case L7_OSPF_ROW_DESTROY:
    parm = ROW_DESTROY;
    break;
  default:
    return L7_FAILURE;
    break;
  }

  e = HL_GetFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl, (void**)&p_ARO);
  while(e == E_OK)
  {
     memset(&Hosts, 0, sizeof(t_S_HostRouteCfg));


     while(e == E_OK)
     {
        Hosts.HostStatus = ROW_READ_NEXT;
        e =  ARO_HostConfig_Pack(p_ARO, &Hosts);
        if(e == E_OK){
           if((Hosts.HostIpAdr == IpAddress) && (Hosts.HostTOS == TOS) )
           {
             Hosts.HostStatus = parm;
             if(ARO_HostConfig_Pack(p_ARO, &Hosts) != E_OK)
                return L7_FAILURE;
             return L7_SUCCESS;
           }
        }


     }
     e = HL_GetNext(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl, (void *)&p_ARO, (void**)&p_ARO);
  }
  return L7_FAILURE;


}

/*********************************************************************
* @purpose  Get the Area the Host Entry is to be found within.
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    IPAddress   Ip address
* @param    TOS         TOS type
* @param    *val        row status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    By default, the area that a subsuming OSPF interface is in, or 0.0.0.0.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenHostAreaIDGet(L7_uint32 IpAddress, L7_int32 TOS, L7_int32 *val )
{
  t_Handle ospfHostTablePtr = L7_NULLPTR;

  ospfHostTablePtr = ospfHostTable_Lookup(ospfMapMibCtrl_g.p_ospfMibObj,
                                                  IpAddress, TOS);
  if (ospfHostTablePtr != L7_NULLPTR)
  {
    *val = OSPFHOST_AREAID(ospfHostTablePtr);
  }

  if (ospfHostTablePtr != L7_NULLPTR)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*
**********************************************************************
*                    API FUNCTIONS  -  NSSA CONFIG  (RFC 3101)
**********************************************************************
*/

/*********************************************************************
* @purpose  Get the information associated with NSSA configuration
*
* @param    areaId       area id
* @param    *metric      pointer to stub area metric
* @param    *metricType  pointer to stub area metric type
*
* @returns  L7_SUCCESS  if NSSA configuration is found
* @returns  L7_FAILURE  otherwise
*
* @notes    Placeholder for private MIB support for OSPF NSSAs
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSAEntryGet ( L7_uint32 areaId, L7_uint32 *metric,
                                   L7_uint32 *metricType)
{
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Given a NSSA area specification, return the area ID
*           of the next NSSA area
*
* @param    areaId      area id
*
* @returns  L7_SUCCESS  if NSSA configuration is found
* @returns  L7_FAILURE  otherwise
*
* @notes    Placeholder for private MIB support for OSPF NSSAs
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSAEntryNext ( L7_uint32 *areaId, L7_uint32 *TOS)
{
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Get the information associated with the specified NSSA
*
* @param    areaId       area id
* @param    cfg       structure to store NSSA information
*
* @returns  L7_SUCCESS  if NSSA configuration is found
* @returns  L7_FAILURE  otherwise
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSAConfigurationGet(L7_uint32 areaId, t_S_NssaCfg *cfg)
{
  L7_RC_t     cfgRc = L7_SUCCESS;
  L7_uint32   cfgData, defInfoOrg, metric;
  L7_OSPF_STUB_METRIC_TYPE_t metricType;

  /* Read user configuration information */
  cfg->AreaId = areaId;

  cfgRc |= ospfMapNSSATranslatorRoleGet(areaId, &cfgData);
  cfg->NSSATranslatorRole = (cfgData == L7_OSPF_NSSA_TRANSLATOR_ALWAYS) ?
    NSSA_TRANSLATOR_ALWAYS : NSSA_TRANSLATOR_CANDIDATE;

  cfgRc |= ospfMapNSSATranslatorStabilityIntervalGet(areaId, &cfgData);
  cfg->TranslatorStabilityInterval = (ulng)((cfgData == L7_OSPF_NOT_CONFIGURED) ?
    L7_OSPF_NSSA_TRANS_STABILITY_INT_DEF : cfgData);

  cfgRc |= ospfMapNSSAImportSummariesGet(areaId, &cfgData);
  cfg->ImportSummaries = (cfgData == L7_ENABLE)? TRUE : FALSE;

  cfgRc |= ospfMapStubMetricGet(areaId, 0, &cfgData);
  cfg->DefaultCost = cfgData;

  cfgRc |= ospfMapNSSARedistributeGet(areaId, &cfgData);
  cfg->NSSARedistribute = (cfgData == L7_ENABLE)? TRUE : FALSE;

  cfgRc |= ospfMapNSSADefaultInfoGet(areaId, &defInfoOrg, &metric, &metricType);
  cfg->NSSADefInfoOrig = (defInfoOrg == L7_TRUE)? TRUE : FALSE;
  cfg->NSSADefMetric   = (ulng)((metric == L7_OSPF_NOT_CONFIGURED)? 10: cfgData);

  cfg->NSSADefMetricType = (metricType == L7_OSPF_AREA_STUB_COMPARABLE_COST)?
    NSSA_COMPARABLE_COST : NSSA_COMPARABLE_COST;

  return cfgRc;
}

/*********************************************************************
* @purpose  Set the entry status.
*
* @param    areaId  area identifier
* @param    status  used to create and delete nssa configurations
*                   (L7_OSPF_ROW_STATUS_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the status of the entry.  Setting
*             it to 'invalid' has the effect of rendering it inoperable
*             The internal effect (row removal) is implementation
*             dependent."
*             This funtion is primarily used to create/delete NSSAs.
*             In order to change specific config parms individual
*             functions have been provided.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSAStatusSet (L7_uint32 areaId, L7_int32 status)
{
  t_S_NssaCfg nssaCfg;

  memset(&nssaCfg, 0, sizeof(t_S_NssaCfg));

  /* If the area does not exist in the vendor code or we cannot successfully get
  ** the configuration information from the LVL7 config file then quit
  */
  if((ospfMapExtenAreaEntryGet(areaId) != L7_SUCCESS) ||
      /* shall get the nssa configuration only if the action is not NSSA Delete */
     ((status != L7_OSPF_ROW_DESTROY) && 
      (ospfMapExtenNSSAConfigurationGet(areaId, &nssaCfg) != L7_SUCCESS)))
    return L7_FAILURE;

  switch(status)
  {
  case L7_OSPF_ROW_ACTIVE:
    nssaCfg.NSSAStatus = ROW_ACTIVE;
    break;
  case L7_OSPF_ROW_NOT_IN_SERVICE:
    nssaCfg.NSSAStatus = ROW_ACTIVE;
    break;
  case L7_OSPF_ROW_NOT_READY:
    nssaCfg.NSSAStatus = ROW_NOT_READY;
    break;
  case L7_OSPF_ROW_CREATE_AND_GO:
    nssaCfg.NSSAStatus = ROW_CREATE_AND_GO;
    break;
  case L7_OSPF_ROW_CREATE_AND_WAIT:
    nssaCfg.NSSAStatus = ROW_CREATE_AND_WAIT;
    break;
  case L7_OSPF_ROW_DESTROY:
    nssaCfg.NSSAStatus = ROW_DESTROY;
    break;
  default:
    return L7_FAILURE;
    break;
  }

  return NSSA_Config_Pack(areaId, &nssaCfg);
}


/*********************************************************************
* @purpose  Set the NSSA Translator Role of the specified NSSA
*
* @param    areaId    areaID
* @param    nssaTR   NSSA translator role (L7_OSPF_NSSA_TRANSLATOR_ROLE_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes " This variable Specifies whether or not an NSSA border router will
*        unconditionally translate Type-7 LSAs into Type-5 LSAs.  When
*        it is set to Always, an NSSA border router always translates
*        Type-7 LSAs into Type-5 LSAs regardless of the translator state
*        of other NSSA border routers.  When it is set to Candidate, an
*        NSSA border router participates in the translator election
*        process described in Section 3.1.  The default setting is
*        Candidate"
*
*        This function assumes that the area has already been configured
*        to act as a NSSA
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSATranslatorRoleSet (L7_uint32 areaId, L7_uint32 nssaTR)
{
  t_S_NssaCfg nssaCfg;

  memset(&nssaCfg, 0, sizeof(t_S_NssaCfg));

  /* If we cannot successfully get the configuration information
  ** from the LVL7 config file then quit
  */
  if(ospfMapExtenNSSAConfigurationGet(areaId, &nssaCfg) != L7_SUCCESS)
    return L7_FAILURE;

  switch(nssaTR)
  {
  case L7_OSPF_NSSA_TRANSLATOR_ALWAYS:
    nssaCfg.NSSATranslatorRole = NSSA_TRANSLATOR_ALWAYS;
    break;
  case L7_OSPF_NSSA_TRANSLATOR_CANDIDATE:
    nssaCfg.NSSATranslatorRole = NSSA_TRANSLATOR_CANDIDATE;
    break;
  default:
    return L7_FAILURE;
    break;
  }

  nssaCfg.NSSAStatus = ROW_CHANGE;
  return NSSA_Config_Pack(areaId, &nssaCfg);
}

/*********************************************************************
* @purpose  Get the NSSA Translator State of the specified NSSA
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    areaId    areaID
* @param    *nssaTR   NSSA translator state (L7_OSPF_NSSA_TRANSLATOR_STATE_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes " This variable specifies the translator state of a NSSA border
*        router.  The translator state is determined by translator
*        election taking into account the user configured NSSA Translator
*        Role and is a read-only parm."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSATranslatorStateGet (L7_uint32 areaId,
                                            L7_uint32 *nssaTRState)
{
  L7_uint32 externRouting;
  t_S_NssaCfg nssaCfg;

  /* Set the value to disabled by default */
  *nssaTRState = L7_OSPF_NSSA_TRANS_STATE_DISABLED;

  /* If we could not get the configuration information send the default
  ** value back to the user
  */
  if((ospfMapExtenAreaExternalRoutingCapabilityGet(areaId, &externRouting) !=
      L7_SUCCESS) || (externRouting != AREA_IMPORT_NSSA))
    return L7_SUCCESS;

  memset(&nssaCfg, 0, sizeof(t_S_NssaCfg));
  if(ospfMapExtenNSSAConfigurationGet(areaId, &nssaCfg) != L7_SUCCESS)
    return L7_SUCCESS;

  nssaCfg.NSSAStatus = ROW_READ;
  if(NSSA_Config_Pack(areaId, &nssaCfg) == L7_SUCCESS)
  {
    switch(nssaCfg.NSSATranslatorState)
    {
    case NSSA_TRANS_STATE_ENABLED:
      *nssaTRState = L7_OSPF_NSSA_TRANS_STATE_ENABLED;
      break;
    case NSSA_TRANS_STATE_ELECTED:
      *nssaTRState = L7_OSPF_NSSA_TRANS_STATE_ELECTED;
      break;
    case NSSA_TRANS_STATE_DISABLED:
      *nssaTRState = L7_OSPF_NSSA_TRANS_STATE_DISABLED;
      break;
    default:
      break;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the Translator Stability Interval of the specified NSSA
*
* @param    areaId     areaID
* @param    *stabInt   TranslatorStabilityInterval
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes " This variable defines the length of time an elected Type-7
*        translator will continue to perform its translator duties once
*        it has determined that its translator status has been deposed by
*        another NSSA border router translator as described in Section
*        3.1 and 3.3.  The default setting is 40 seconds."
*
*        This function assumes that the area has already been configured
*        to act as a NSSA
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSATranslatorStabilityIntervalSet (L7_uint32 areaId,
                                                        L7_uint32 stabInt)
{
  t_S_NssaCfg nssaCfg;

  memset(&nssaCfg, 0, sizeof(t_S_NssaCfg));

  /* If we cannot successfully get the configuration information
  ** from the LVL7 config file then quit
  */
  if(ospfMapExtenNSSAConfigurationGet(areaId, &nssaCfg) != L7_SUCCESS)
    return L7_FAILURE;

  nssaCfg.TranslatorStabilityInterval = stabInt;
  nssaCfg.NSSAStatus = ROW_CHANGE;

  return NSSA_Config_Pack(areaId, &nssaCfg);
}

/*********************************************************************
* @purpose  Set the import summary configuration for the specified NSSA
*
* @param    areaId     areaID
* @param    impSum    L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes " When set to enabled, OSPF's summary routes are imported into
*        the NSSA as Type-3 summary-LSAs.  When set to disabled, summary
*        routes are not imported into the NSSA.  The default setting is
*        enabled."
*
*        This function assumes that the area has already been configured
*        to act as a NSSA
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSAImportSummariesSet (L7_uint32 areaId, L7_uint32 impSum)
{
  t_S_NssaCfg nssaCfg;

  memset(&nssaCfg, 0, sizeof(t_S_NssaCfg));

  /* If we cannot successfully get the configuration information
  ** from the LVL7 config file then quit
  */
  if(ospfMapExtenNSSAConfigurationGet(areaId, &nssaCfg) != L7_SUCCESS)
    return L7_FAILURE;

  nssaCfg.ImportSummaries = (impSum == L7_ENABLE)? TRUE : FALSE;
  nssaCfg.NSSAStatus = ROW_CHANGE;

  return NSSA_Config_Pack(areaId, &nssaCfg);
}

/*********************************************************************
* @purpose  Set the T3 default cost for the specified NSSA
*
* @param    areaId     areaID
* @param    impSum    L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSADefaultCostSet (L7_uint32 areaId, L7_uint32 defCost)
{
  t_S_NssaCfg nssaCfg;

  memset(&nssaCfg, 0, sizeof(t_S_NssaCfg));

  /* If we cannot successfully get the configuration information
  ** from the LVL7 config file then quit
  */
  if(ospfMapExtenNSSAConfigurationGet(areaId, &nssaCfg) != L7_SUCCESS)
    return L7_FAILURE;

  nssaCfg.DefaultCost = defCost;
  nssaCfg.NSSAStatus = ROW_CHANGE;

  return NSSA_Config_Pack(areaId, &nssaCfg);
}

/*********************************************************************
* @purpose  Set the route redistribution configuration for the specified NSSA
*
* @param    areaId     areaID
* @param    redist    L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes " L7_ENABLE/ L7_DISABLE (Used when the router is a NSSA ABR
*          and you want the redistribute command to import routes only
*          into the normal areas, but not into the NSSA area)."
*
*        This function assumes that the area has already been configured
*        to act as a NSSA
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSARedistributeSet (L7_uint32 areaId, L7_uint32 redist)
{
  t_S_NssaCfg nssaCfg;

  memset(&nssaCfg, 0, sizeof(t_S_NssaCfg));

  /* If we cannot successfully get the configuration information
  ** from the LVL7 config file then quit
  */
  if(ospfMapExtenNSSAConfigurationGet(areaId, &nssaCfg) != L7_SUCCESS)
    return L7_FAILURE;

  nssaCfg.NSSARedistribute = (redist == L7_ENABLE)? TRUE : FALSE;
  nssaCfg.NSSAStatus = ROW_CHANGE;

  return NSSA_Config_Pack(areaId, &nssaCfg);
}

/*********************************************************************
* @purpose  Set the default information origination configuration for
*           the specified NSSA
*
* @param    areaId      areaID
* @param    defInfoOrg L7_TRUE/ L7_FALSE
* @param    metricType metric type (L7_OSPF_STUB_METRIC_TYPE_t)
* @param    metric     metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes   This function assumes that the area has already been configured
*          to act as a NSSA
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSADefaultInfoSet(L7_uint32 areaId, L7_uint32 defInfoOrg,
                                       L7_uint32 metricType, L7_uint32 metric)
{
  t_S_NssaCfg nssaCfg;

  memset(&nssaCfg, 0, sizeof(t_S_NssaCfg));

  /* If we cannot successfully get the configuration information
  ** from the LVL7 config file then quit
  */
  if(ospfMapExtenNSSAConfigurationGet(areaId, &nssaCfg) != L7_SUCCESS)
    return L7_FAILURE;

  nssaCfg.NSSADefInfoOrig   = (defInfoOrg == L7_TRUE)? TRUE : FALSE;
  nssaCfg.NSSADefMetric     = (ulng)metric;
  nssaCfg.NSSADefMetricType = (metricType == L7_OSPF_AREA_STUB_COMPARABLE_COST)?
                                  NSSA_COMPARABLE_COST: NSSA_NON_COMPARABLE_COST;
  nssaCfg.NSSAStatus = ROW_CHANGE;

  return NSSA_Config_Pack(areaId, &nssaCfg);
}

/*********************************************************************
* @purpose  Set the default information origination configuration for
*           the specified NSSA
*
* @param    areaId        areaID
* @param    defInfoOrg  L7_TRUE/ L7_FALSE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes " L7_TRUE/ L7_FALSE (Used to generate a Type 7 default into
*          the NSSA area. This argument takes effect only on a NSSA ABR)."
* @notes   This function assumes that the area has already been configured
*          to act as a NSSA
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSADefaultInfoOriginateSet (L7_uint32 areaId, L7_uint32 defInfoOrg)
{
  t_S_NssaCfg nssaCfg;

  memset(&nssaCfg, 0, sizeof(t_S_NssaCfg));

  /* If we cannot successfully get the configuration information
  ** from the LVL7 config file then quit
  */
  if(ospfMapExtenNSSAConfigurationGet(areaId, &nssaCfg) != L7_SUCCESS)
    return L7_FAILURE;

  nssaCfg.NSSADefInfoOrig = (defInfoOrg == L7_TRUE)? TRUE : FALSE;
  nssaCfg.NSSAStatus      = ROW_CHANGE;

  return NSSA_Config_Pack(areaId, &nssaCfg);
}

/*********************************************************************
* @purpose  Set the metric type of the default route for the NSSA.
*
* @param    areaId      areaID
* @param    metricType metric type (L7_OSPF_STUB_METRIC_TYPE_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the type of metric advertised as
*             a default route."
* @notes   This function assumes that the area has already been configured
*          to act as a NSSA
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSADefaultMetricTypeSet ( L7_uint32 areaId, L7_uint32 metricType )
{
  t_S_NssaCfg nssaCfg;

  memset(&nssaCfg, 0, sizeof(t_S_NssaCfg));

  /* If we cannot successfully get the configuration information
  ** from the LVL7 config file then quit
  */
  if(ospfMapExtenNSSAConfigurationGet(areaId, &nssaCfg) != L7_SUCCESS)
    return L7_FAILURE;

  nssaCfg.NSSADefMetricType = (metricType == L7_OSPF_AREA_STUB_COMPARABLE_COST)?
                                  NSSA_COMPARABLE_COST: NSSA_NON_COMPARABLE_COST;
  nssaCfg.NSSAStatus        = ROW_CHANGE;

  return NSSA_Config_Pack(areaId, &nssaCfg);
}

/*********************************************************************
* @purpose  Set the value of the default metric for the given NSSA
*
* @param    areaId      areaID
* @param    metric     metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes   This function assumes that the area has already been configured
*          to act as a NSSA
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNSSADefaultMetricSet ( L7_uint32 areaId, L7_uint32 metric )
{
  t_S_NssaCfg nssaCfg;

  memset(&nssaCfg, 0, sizeof(t_S_NssaCfg));

  /* If we cannot successfully get the configuration information
  ** from the LVL7 config file then quit
  */
  if(ospfMapExtenNSSAConfigurationGet(areaId, &nssaCfg) != L7_SUCCESS)
    return L7_FAILURE;

  nssaCfg.NSSADefMetric = (ulng)metric;
  nssaCfg.NSSAStatus    = ROW_CHANGE;

  return NSSA_Config_Pack(areaId, &nssaCfg);
}

/*********************************************************************
* @purpose  Get the Ospf Area Id for the first area of the Ospf Router
*
* @param    *p_areaId        pointer to output location
*                         @b{(output) area Id}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaIdGet(L7_uint32 *p_areaId)
{
  t_ARO         *p_ARO = NULL;
  e_Err         e;
  t_RTO *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;

  if(p_RTO == NULL)
      return L7_FAILURE;

  e = HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO);

  if ((e != E_OK) || (p_ARO == NULL))
    return L7_FAILURE;

  *p_areaId = p_ARO->AreaId;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Ospf Area Id for the next area, of the Ospf Router ,
*           after the area Id specified
*
* @param    areaId      @b{(input)} area identifier
* @param    *p_areaId        pointer to output location
*                         @b{(output) area Id}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenAreaIdGetNext(L7_uint32 areaId, L7_uint32 *p_areaId)
{
  t_ARO         *p_ARO = NULL;
  e_Err         e;
  t_RTO *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;

  if(p_RTO == NULL)
      return L7_FAILURE;

  e = HL_FindFirst(p_RTO->AroHl,
                   (byte*)&areaId, (void**)&p_ARO);

  if ((e != E_OK) || (p_ARO == NULL))
    return L7_FAILURE;

  e = HL_GetNext(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl, (void *)&p_ARO, p_ARO);

  if ((e != E_OK) || (p_ARO == NULL))
    return L7_FAILURE;

  *p_areaId = p_ARO->AreaId;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Convert Vendor trap type to Lvl7 Trap type.
*
* @param    vendTrap    vendor specific ospf trap
* @param    l7Trap      Ospf trap type.
*                       This value is specified by the enum L7_OSPF_TRAP_t
*                       defined in l3_commdefs.h as follows:
*
*                       typedef enum
*                       {
*                         L7_OSPF_TRAP_RESERVED = 0,
*                         L7_OSPF_TRAP_VIRT_IF_STATE_CHANGE,
*                         L7_OSPF_TRAP_NBR_STATE_CHANGE,
*                         L7_OSPF_TRAP_VIRT_NBR_STATE_CHANGE,
*                         L7_OSPF_TRAP_IF_CONFIG_ERROR,
*                         L7_OSPF_TRAP_VIRT_IF_CONFIG_ERROR,
*                         L7_OSPF_TRAP_IF_AUTH_FAILURE,
*                         L7_OSPF_TRAP_VIRT_IF_AUTH_FAILURE,
*                         L7_OSPF_TRAP_RX_BAD_PACKET,
*                         L7_OSPF_TRAP_VIRT_IF_RX_BAD_PACKET,
*                         L7_OSPF_TRAP_TX_RETRANSMIT,
*                         L7_OSPF_TRAP_VIRT_IF_TX_RETRANSMIT,
*                         L7_OSPF_TRAP_ORIGINATE_LSA,
*                         L7_OSPF_TRAP_MAX_AGE_LSA,
*                         L7_OSPF_TRAP_LS_DB_OVERFLOW,
*                         L7_OSPF_TRAP_LS_DB_APPROACHING_OVERFLOW,
*                         L7_OSPF_TRAP_IF_STATE_CHANGE,
*                         L7_OSPF_TRAP_IF_RX_PACKET,
*                         L7_OSPF_TRAP_RTB_ENTRY_INFO,
*                         L7_OSPF_TRAP_LAST
*
*                       }  L7_OSPF_TRAP_t;
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenVendorTrapToL7Trap(L7_uint32 vendTrap, L7_uint32 *l7Trap)
{
  switch(vendTrap)
  {
  case VIRT_IF_STATE_CHANGE:
    *l7Trap = L7_OSPF_TRAP_VIRT_IF_STATE_CHANGE;
    break;
  case NBR_STATE_CHANGE:
    *l7Trap = L7_OSPF_TRAP_NBR_STATE_CHANGE;
    break;
  case VIRT_NBR_STATE_CHANGE:
    *l7Trap = L7_OSPF_TRAP_VIRT_NBR_STATE_CHANGE;
    break;
  case IF_CONFIG_ERROR:
    *l7Trap = L7_OSPF_TRAP_IF_CONFIG_ERROR;
    break;
  case VIRT_IF_CONFIG_ERROR:
    *l7Trap = L7_OSPF_TRAP_VIRT_IF_CONFIG_ERROR;
    break;
  case IF_AUTH_FAILURE:
    *l7Trap = L7_OSPF_TRAP_IF_AUTH_FAILURE;
    break;
  case VIRT_IF_AUTH_FAILURE:
    *l7Trap = L7_OSPF_TRAP_VIRT_IF_AUTH_FAILURE;
    break;
  case RX_BAD_PACKET:
    *l7Trap = L7_OSPF_TRAP_RX_BAD_PACKET;
    break;
  case VIRT_IF_RX_BAD_PACKET:
    *l7Trap = L7_OSPF_TRAP_VIRT_IF_RX_BAD_PACKET;
    break;
  case TX_RETRANSMIT:
    *l7Trap = L7_OSPF_TRAP_TX_RETRANSMIT;
    break;
  case VIRT_IF_TX_RETRANSMIT:
    *l7Trap = L7_OSPF_TRAP_VIRT_IF_TX_RETRANSMIT;
    break;
  case ORIGINATE_LSA:
    *l7Trap = L7_OSPF_TRAP_ORIGINATE_LSA;
    break;
  case MAX_AGE_LSA:
    *l7Trap = L7_OSPF_TRAP_MAX_AGE_LSA;
    break;
  case LS_DB_OVERFLOW:
    *l7Trap = L7_OSPF_TRAP_LS_DB_OVERFLOW;
    break;
  case LS_DB_APPROACHING_OVERFLOW:
    *l7Trap = L7_OSPF_TRAP_LS_DB_APPROACHING_OVERFLOW;
    break;
  case IF_STATE_CHANGE:
    *l7Trap = L7_OSPF_TRAP_IF_STATE_CHANGE;
    break;
  case IF_RX_PACKET:
    *l7Trap = L7_OSPF_TRAP_IF_RX_PACKET;
    break;
  case RTB_ENTRY_INFO:
    *l7Trap = L7_OSPF_TRAP_RTB_ENTRY_INFO;
    break;
  default:
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*
**********************************************************************
*                        PRIVATE HELPER FUNCTIONS
**********************************************************************
*/

/*********************************************************************
* @purpose  common function to set the Router Config parameters
*
* @param    *p_rtoCfg     pointer to output location
*                         @b{(output)  router config entry}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t ospfMapExtenRTOConfigSet(t_S_RouterCfg *p_rtoCfg)
{
  e_Err erc;
  static const char *routine_name = "ospfMapExtenRTOConfigSet()";

  p_rtoCfg->RouterStatus = ROW_CHANGE;

  if ((erc = RTO_Config_Pack(ospfMapCtrl_g.RTO_Id, p_rtoCfg)) != E_OK)
  {
    OSPFMAP_ERROR("%s %d: %s: ERROR: OSPF_MAP: Unable to configure OSPF router object, rc=%d\n",
                    __FILE__, __LINE__, routine_name, erc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  common function to set the Interface Config parameters
*
* @param    IFO_Id        @b{(input)} interface identifier
* @param    *p_ifoCfg     pointer to output location
*                         @b{(output)  interface config entry}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t ospfMapExtenIFOConfigSet(t_Handle IFO_Id, t_IFO_Cfg *p_ifoCfg)
{
  e_Err erc;

  static const char *routine_name = "ospfMapExtenIFOConfigSet()";

  p_ifoCfg->IfStatus = ROW_CHANGE;

  if ((erc = IFO_Config_Pack(IFO_Id, p_ifoCfg)) != E_OK)
  {
    OSPFMAP_ERROR("%s %d: %s: ERROR: OSPF_MAP: Unable to configure OSPF Interface object, rc=%d\n",
                    __FILE__, __LINE__, routine_name, erc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  common function to retrieve the Virtual Interface Authentication Key
*           Config parameters
*
* @param    IFO_Id     Virtual IFO handle
* @param    *p_Auth    pointer to output location
*                      @b{(output)  virtual interface auth key entry}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t virtIfAuthKeyCfgGet(t_Handle IFO_Id, t_OspfAuthKey *p_Auth)
{
  t_IFO         *p_IFO = (t_IFO *)IFO_Id;

  if(!p_IFO)
    return L7_FAILURE;

  memset(p_Auth, 0, (size_t)sizeof(t_OspfAuthKey));
  /* always read the first (and only) entry in the list, regardless of key id */
  p_Auth->PrevEntry = NULL;
  p_Auth->KeyStatus = ROW_READ_NEXT;

  return ((IFO_AuthKeyConfig_Pack((t_Handle)p_IFO, p_Auth) == E_OK) ?
          L7_SUCCESS : L7_FAILURE);
}

/*********************************************************************
* @purpose  common function to set the Virtual Interface Authentication Key
*           Config parameters
*
* @param    IFO_Id        @b{(input)} Virtual IFO handle
* @param    *p_Cfg        pointer to output location
*                         @b{(output)  virtual interface config entry}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t virtIfAuthKeyCfgSet(t_Handle IFO_Id, t_OspfAuthKey *p_Auth)
{
  t_IFO         *p_IFO = (t_IFO *)IFO_Id;

  if(!p_IFO)
    return L7_FAILURE;

  return ((IFO_AuthKeyConfig_Pack((t_Handle)p_IFO, p_Auth) == E_OK) ?
          L7_SUCCESS : L7_FAILURE);
}

/*********************************************************************
* @purpose  common function to retrieve the Router Config parameters
*
* @param    RTO_Id        @b{(input)} router identifier
* @param    *p_Cfg        pointer to output location
*                         @b{(output)  router config entry}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t rtoCfgGet(t_Handle RTO_Id, t_S_RouterCfg *p_Cfg)
{
  memset(p_Cfg, 0, sizeof(t_S_RouterCfg));
  p_Cfg->RouterStatus = ROW_READ;

  if (RTO_Config_Pack(RTO_Id, p_Cfg) != E_OK)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  common function to retrieve the Area Config parameters
*
* @param    areaId        @b{(input)} area identifier
* @param    *p_Cfg        pointer to output location
*                         @b{(output)  area config entry}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t aroCfgGet(L7_uint32 areaId, t_S_AreaCfg *p_Cfg)
{
  t_ARO         *p_ARO = NULL;
  e_Err         e;

  e = HL_FindFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
                   (byte*)&areaId, (void**)&p_ARO);
  if ((e != E_OK) || (p_ARO == NULL))
    return L7_FAILURE;

  memset(p_Cfg, 0, sizeof(t_S_AreaCfg));
  p_Cfg->AreaStatus = ROW_READ;

  if (ARO_Config_Pack(p_ARO, p_Cfg) != E_OK)
    return L7_FAILURE;

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  common function to retrieve the Interface Config parameters
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    *p_Cfg        pointer to output location
*                         @b{(output)  interface config entry}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t ifoCfgGet(L7_uint32 intIfNum, t_IFO_Cfg *p_Cfg)
{
  t_IFO         *p_IFO = NULL;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  memset(p_Cfg, 0, sizeof(t_IFO_Cfg));
  p_Cfg->IfStatus = ROW_READ;

  if (IFO_Config_Pack((t_Handle)p_IFO, p_Cfg) != E_OK)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  common function to retrieve the Interface Metric Config parameters
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    TOS           @b{(input)} Type of Service
* @param    *p_Metric     pointer to output location
*                         @b{(output)  interface metric config entry}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t ifoMetricCfgGet(L7_uint32 intIfNum, L7_int32 TOS, t_S_IfMetric *p_Metric)
{
  t_IFO         *p_IFO = NULL;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  memset(p_Metric, 0, sizeof(t_S_IfMetric));
  p_Metric->MetricStatus = ROW_READ;
  p_Metric->MetricTOS = TOS;

  if (IFO_MetricConfig_Pack((t_Handle)p_IFO, p_Metric) != E_OK)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  common function to set the Interface Metric Config parameters
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    TOS           @b{(input)} Type of Service
* @param    *p_Metric     pointer to output location
*                         @b{(output)  interface metric config entry}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t ifoMetricCfgSet(L7_uint32 intIfNum, L7_int32 TOS, t_S_IfMetric *p_Metric)
{
  t_IFO         *p_IFO = NULL;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  p_Metric->MetricStatus = ROW_CHANGE;
  p_Metric->MetricTOS = TOS;

  if (IFO_MetricConfig_Pack((t_Handle)p_IFO, p_Metric) != E_OK)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  common function to retrieve the Interface Authentication Key parameters
*
* @param    intIfNum      @b{(input)}  internal interface number
* @param    *p_Auth       @b{(output)} interface auth key entry output location
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      interface object does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ifoAuthKeyCfgGet(L7_uint32 intIfNum, t_OspfAuthKey *p_Auth)
{
  t_IFO         *p_IFO = NULL;
  e_Err         e;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  memset(p_Auth, 0, sizeof(t_OspfAuthKey));
  /* always read the first (and only) entry in the list, regardless of key id */
  p_Auth->PrevEntry = NULL;
  p_Auth->KeyStatus = ROW_READ_NEXT;

  e = IFO_AuthKeyConfig_Pack((t_Handle)p_IFO, p_Auth);
  if (e != E_OK)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  common function to set the Interface Authentication Key parameters
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    *p_Auth       @b{(input)} interface auth key entry info
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      interface object does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t ifoAuthKeyCfgSet(L7_uint32 intIfNum, t_OspfAuthKey *p_Auth)
{
  t_IFO         *p_IFO = NULL;
  e_Err         e;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  e = IFO_AuthKeyConfig_Pack((t_Handle)p_IFO, p_Auth);
  if (e != E_OK)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  common function to retrieve the LSA database parameters
*
* @param    areaId        @b{(input)} Area Identifier
* @param    *p_DbEntry    pointer to output location
*                         @b{(output)  lsa database entry}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t aroLSADbGet(L7_uint32 areaId, t_S_LsDbEntry *p_DbEntry)
{
  t_ARO         *p_ARO = NULL;
  e_Err         e;

  e = HL_FindFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
                   (byte*)&areaId, (void**)&p_ARO);

  if ((e != E_OK) || (p_ARO == NULL))
    return L7_FAILURE;



  if (ARO_LsaDatabase(p_ARO, p_DbEntry) != E_OK)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  common function to retrieve the Neighbor Config parameters
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    routerId      @b{(input)} Router Identifier
* @param    *p_Cfg        pointer to output location
*                         @b{(output)  neighbor entry}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t nboCfgGet(L7_uint32 intIfNum, L7_uint32 routerId, t_S_NeighborCfg *p_Cfg)
{
  t_IFO         *p_IFO = NULL;
  t_NBO         *p_NBO = NULL;
  e_Err         e;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  e = HL_FindFirst(p_IFO->NboHl, (byte*)&routerId, (void**)&p_NBO);

  if ((e != E_OK) || (p_NBO == NULL))
    return L7_FAILURE;

  memset(p_Cfg, 0, sizeof(t_S_NeighborCfg));
  p_Cfg->NbmaNbrStatus = ROW_READ;

  if (NBO_Config_Pack((t_Handle)p_NBO, p_Cfg) != E_OK)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  common function to retrieve the Neighbor Config parameters
*           using different set of parametrs
*
* @param    IpAddr        @b{(input)} IP Address
* @param    intIfNum      @b{(input)} internal interface number
* @param    *p_Cfg        pointer to output location
*                         @b{(output)  neighbor entry}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t nbrCfgGet(L7_uint32 IpAddr, L7_uint32 intIfNum, t_S_NeighborCfg *p_Cfg)
{
  t_IFO         *p_IFO = NULL;
  t_NBO         *p_NBO = NULL;
  e_Err         e;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  e = HL_GetFirst(p_IFO->NboHl,  (void**)&p_NBO);
  if(e != E_OK)
      return L7_ERROR;

  while(e == E_OK)
  {
    memset(p_Cfg, 0, sizeof(t_S_NeighborCfg));
    p_Cfg->NbmaNbrStatus = ROW_READ;

    e = NBO_Config_Pack((t_Handle)p_NBO, p_Cfg) ;
    if(e == E_OK){
        if((p_Cfg->IpAdr == IpAddr) )
            return L7_SUCCESS;
        else
            e = HL_GetNext(p_IFO->NboHl,  (void**)&p_NBO, p_NBO);
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;

}

/*********************************************************************
* @purpose  common function to set the Neighbor Config parameters
*
* @param    IpAddr        @b{(input)} IP Address
* @param    intIfNum      @b{(input)} internal interface number
* @param    *p_Cfg        pointer to output location
*                         @b{(output)  neighbor entry}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t nbrCfgSet(L7_uint32 IpAddr, L7_uint32 intIfNum, t_S_NeighborCfg *p_Cfg)
{
  t_IFO         *p_IFO = NULL;
  t_NBO         *p_NBO = NULL;
  e_Err         e;
  t_S_NeighborCfg     nboCfg;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_ERROR;

  e = HL_GetFirst(p_IFO->NboHl,  (void**)&p_NBO);
  if(e != E_OK)
      return L7_ERROR;

  while(e == E_OK)
  {
    memset(&nboCfg, 0, sizeof(t_S_NeighborCfg));
    nboCfg.NbmaNbrStatus = ROW_READ;

    e = NBO_Config_Pack((t_Handle)p_NBO, &nboCfg) ;
    if(e == E_OK){
        if((nboCfg.IpAdr == IpAddr) )
        {
           if(NBO_Config_Pack((t_Handle)p_NBO, p_Cfg) != E_OK)
               return L7_FAILURE;
           return L7_SUCCESS;
        }

        else
            e = HL_GetNext(p_IFO->NboHl,  (void**)&p_NBO, p_NBO);
    }
  }

  return L7_FAILURE;

}

/*********************************************************************
* @purpose  common function to set the Stub Area Config parameters
*
* @param    AreaId        @b{(input)} Area Identifier
* @param    TOS           @b{(input)} Type of Service
* @param    *p_stubCfg    pointer to output location
*                         @b{(output) stub area entry}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t stubCfgSet(L7_uint32 AreaId, L7_int32 TOS, t_S_StubAreaEntry *p_stubCfg)
{
  t_ARO         *p_ARO = NULL;
  e_Err         e;

  e = HL_FindFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
                   (byte*)&AreaId, (void**)&p_ARO);
  if ((e != E_OK) || (p_ARO == NULL))
    return L7_FAILURE;

  if(ARO_StubMetric_Pack((t_Handle)p_ARO, p_stubCfg) != E_OK)
      return L7_FAILURE;
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  common function to retrieve the Area Aggregate Config parameters
*
* @param    AreaID        @b{(input)} Area Identifier
* @param    Type          @b{(input)} Type of entry
* @param    Net           @b{(input)} IP Address
* @param    Mask           @b{(input)} Subnet Mask
* @param    *p_agrEntry   pointer to output location
*                         @b{(output) area aggregate entry}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t areaAggregateGet(L7_uint32 AreaID, L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, t_S_AreaAggregateEntry *p_agrEntry)
{
  t_ARO         *p_ARO = NULL;
  e_Err         e;

  e = HL_FindFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
                   (byte*)&AreaID, (void**)&p_ARO);
  if ((e != E_OK) || (p_ARO == NULL))
    return L7_FAILURE;

  memset(p_agrEntry, 0, sizeof(t_S_AreaAggregateEntry));

  while(e == E_OK)
  {
     p_agrEntry->AggregateStatus = ROW_READ_NEXT;
     e =  ARO_AddressRange_Pack(p_agrEntry);
     if(e == E_OK){
        if((p_agrEntry->AreaId == AreaID) && (p_agrEntry->LsdbType == Type)
            && (p_agrEntry->NetIpAdr == Net) && (p_agrEntry->SubnetMask == Mask))
            return L7_SUCCESS;
     }


  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  common function to set the Area Aggregate Config parameters
*
* @param    AreaID        @b{(input)} Area Identifier
* @param    Type          @b{(input)} Type of entry
* @param    Net           @b{(input)} IP Address
* @param    Mask           @b{(input)} Subnet Mask
* @param    *p_agrEntry   pointer to output location
*                         @b{(output) area aggregate entry}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t areaAggregateSet(L7_uint32 AreaID, L7_uint32 Type, L7_int32 Net, L7_uint32 Mask, t_S_AreaAggregateEntry *p_agrEntry)
{
  t_ARO         *p_ARO = NULL;
  e_Err         e;
  t_S_AreaAggregateEntry agrEntry;

  e = HL_FindFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
                   (byte*)&AreaID, (void**)&p_ARO);
  if ((e != E_OK) || (p_ARO == NULL))
    return L7_FAILURE;

  memset(&agrEntry, 0, sizeof(t_S_AreaAggregateEntry));

  while(e == E_OK)
  {
     agrEntry.AggregateStatus = ROW_READ_NEXT;
     e =  ARO_AddressRange_Pack(&agrEntry);
     if(e == E_OK){
        if((agrEntry.AreaId == AreaID) && (agrEntry.LsdbType == Type)
            && (agrEntry.NetIpAdr == Net) && (agrEntry.SubnetMask == Mask))
        {
            if(ARO_AddressRange_Pack(p_agrEntry) != E_OK)
                return L7_FAILURE;
            return L7_SUCCESS;
        }

     }


  }
  return L7_FAILURE;
}


/*********************************************************************
* @purpose  common function to retrieve the Area Range Config parameters
*
* @param    AreaID        @b{(input)} Area Identifier
* @param    Net           @b{(input)} IP address
* @param    *p_agrEntry   pointer to output location
*                         @b{(output) area range entry}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t areaRangeGet(L7_uint32 AreaID, L7_int32 Net, t_S_AreaAggregateEntry *p_agrEntry)
{
  t_ARO         *p_ARO = NULL;
  e_Err         e;

  e = HL_FindFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
                   (byte*)&AreaID, (void**)&p_ARO);
  if ((e != E_OK) || (p_ARO == NULL))
    return L7_FAILURE;

  memset(p_agrEntry, 0, sizeof(t_S_AreaAggregateEntry));

  while(e == E_OK)
  {
     p_agrEntry->AggregateStatus = ROW_READ_NEXT;
     e =  ARO_AddressRange_Pack(p_agrEntry);
     if(e == E_OK){
        if((p_agrEntry->AreaId == AreaID) && (p_agrEntry->NetIpAdr == Net) )
            return L7_SUCCESS;
     }


  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  common function to set the Area Range Config parameters
*
* @param    AreaID        @b{(input)} Area Identifier
* @param    Net           @b{(input)} IP Address
* @param    *p_agrEntry   pointer to output location
*                         @b{(output) area range entry}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t areaRangeSet(L7_uint32 AreaID, L7_int32 Net, t_S_AreaAggregateEntry *p_agrEntry)
{
  t_ARO         *p_ARO = NULL;
  e_Err         e;
  t_S_AreaAggregateEntry agrEntry;

  e = HL_FindFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
                   (byte*)&AreaID, (void**)&p_ARO);
  if ((e != E_OK) || (p_ARO == NULL))
    return L7_FAILURE;

  memset(&agrEntry, 0, sizeof(t_S_AreaAggregateEntry));

  while(e == E_OK)
  {
     agrEntry.AggregateStatus = ROW_READ_NEXT;
     e =  ARO_AddressRange_Pack(&agrEntry);
     if(e == E_OK){
        if((agrEntry.AreaId == AreaID) && (agrEntry.NetIpAdr == Net) )
        {
            if(ARO_AddressRange_Pack(p_agrEntry) != E_OK)
                return L7_FAILURE;
            return L7_SUCCESS;
        }

     }


  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Tell the OSPF engine to originate a default route.
*
* @param    none
*
* @returns  L7_SUCCESS  if success
*
* @notes    A default route not originated if no default exists in
*           the forwarding table, unless OSPF is configured to
*           originate unconditionally.
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenDefRouteOrig ()
{
    t_S_AsExternalCfg asExtCfg;
    L7_BOOL alwaysOriginate = pOspfMapCfgData->defRouteCfg.always;
    L7_uint32 ospfMode;

    if ((ospfMapOspfAdminModeGet(&ospfMode) != L7_SUCCESS) ||
        (ospfMode != L7_ENABLE)) {
        return L7_SUCCESS;
    }

    if (ospfMapOspfInitialized() != L7_TRUE) {
        return L7_SUCCESS;
    }

    if (alwaysOriginate || ospfMapDefaultRouteExists()) {
        /* First, make sure router is considered an ASBR. */
        if (ospfMapIsAsbr() == L7_FALSE) {
            ospfMapExtenAsbrStatusSet(L7_ENABLE);
        }

        /* OK to originate a default route */
        bzero((L7_char8 *)&asExtCfg, sizeof(t_S_AsExternalCfg));
        asExtCfg.DestNetIpAdr  = 0;
        asExtCfg.DestNetIpMask = 0;
        asExtCfg.PhyType       = OSPF_ETHERNET;
        if (pOspfMapCfgData->defRouteCfg.metType == L7_OSPF_METRIC_EXT_TYPE2) {
            asExtCfg.IsMetricType2 = TRUE;
        }
        else {
            asExtCfg.IsMetricType2 = FALSE;
        }
        if (pOspfMapCfgData->defRouteCfg.metric != FD_OSPF_ORIG_DEFAULT_ROUTE_METRIC) {
            asExtCfg.MetricValue = pOspfMapCfgData->defRouteCfg.metric;
        }
        else if (pOspfMapCfgData->rtr.defMetConfigured) {
            asExtCfg.MetricValue = pOspfMapCfgData->rtr.defaultMetric;
        }
        else {
            asExtCfg.MetricValue = OSPF_DEFAULT_METRIC_10;
        }

        /* Always set the forwarding address to 0 */
        asExtCfg.ForwardingAdr = 0;

        if (ospfMapOriginatingDefaultRoute()) {
            asExtCfg.AsExtStatus = ROW_CHANGE;
        }
        else {
            pOspfInfo->originatingDefaultRoute = L7_TRUE;
            asExtCfg.AsExtStatus = ROW_CREATE_AND_GO;
        }

        RTO_AsExternalCfg_Pack((t_RTO *) ospfMapCtrl_g.RTO_Id, &asExtCfg);
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Tell the OSPF engine not to originate a default route.
*
* @param    none
*
* @returns  L7_SUCCESS  if successful
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenDefRouteNoOrig ()
{
    t_S_AsExternalCfg asExtCfg;

    if (ospfMapOriginatingDefaultRoute()) {

        bzero((L7_char8 *)&asExtCfg, sizeof(t_S_AsExternalCfg));
        asExtCfg.DestNetIpAdr  = 0;
        asExtCfg.DestNetIpMask = 0;
        asExtCfg.AsExtStatus = ROW_DESTROY;
        RTO_AsExternalCfg_Pack((t_RTO *) ospfMapCtrl_g.RTO_Id, &asExtCfg);
        pOspfInfo->originatingDefaultRoute = L7_FALSE;

        /* If router was considered an ASBR only because it originated a
         * default route (it doesn't redistribute), then clear the ASBR status.
         */
        if (ospfMapIsAsbr() == L7_FALSE) {
            ospfMapExtenAsbrStatusSet(L7_DISABLE);
        }
    }
    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Given a route from a source other than OSPF, determine the OSPF
*          metric that should be advertised with the route.
*
* @param routeEntry @b{(input)} route from another source
*
* @returns the OSPF metric to be advertised
*
* @comments We first check if the user has configured a redistribution
*           metric specific to the source. If not, we check if the user
*           has configured a default metric for OSPF. If not, we use
*           1 if BGP is the source and 20 otherwise.
*
* @end
*
*********************************************************************/
static L7_uint32 ospfMapRedistMetric(L7_routeEntry_t *routeEntry)
{
    L7_REDIST_RT_INDICES_t srcProto = protocolMapping[routeEntry->protocol];
    ospfRedistCfg_t *redistCfg = ospfMapRedistCfgGet(srcProto);
    if (redistCfg) {
        if (redistCfg->redistMetric != FD_OSPF_REDIST_METRIC) {
            /* user has configured a redistribution metric */
            return (L7_uint32) redistCfg->redistMetric;
        }

        /* Next, see if the user has configured a default metric for OSPF. */
        if (pOspfMapCfgData->rtr.defMetConfigured) {
            return pOspfMapCfgData->rtr.defaultMetric;
        }
    }

    /* Fall back on defaults. Different if source is BGP. */
    if (srcProto == REDIST_RT_BGP) {
        return OSPF_EXT_ROUTE_DEFAULT_METRIC_BGP;
    }
    else {
        return OSPF_EXT_ROUTE_DEFAULT_METRIC;
    }
}

/*********************************************************************
*
* @purpose Given a route from a source other than OSPF, determine the OSPF
*  metric type that should be advertised with the route.
*
* @param routeEntry @b{(input)} route from another source
*
* @returns the OSPF metric type to be advertised
*
* @comments Defaults to external type 2 if use hasn't configured otherwise.
*
* @end
*
*********************************************************************/
static L7_OSPF_EXT_METRIC_TYPES_t
ospfMapRedistMetricType(L7_routeEntry_t *routeEntry)
{
    L7_REDIST_RT_INDICES_t srcProto = protocolMapping[routeEntry->protocol];
    ospfRedistCfg_t *redistCfg = ospfMapRedistCfgGet(srcProto);
    if (redistCfg) {
        return redistCfg->metType;
    }
    else {
        LOG_MSG("SOFTWARE ERROR: Unable to get redistribution configuration\nin ospfMapRedistMetricType().\n");
        return L7_OSPF_METRIC_EXT_TYPE2;
    }
}

/*********************************************************************
*
* @purpose Given a route from a source other than OSPF, determine the OSPF
*  tag that should be advertised with the route.
*
* @param routeEntry @b{(input)} route from another source
*
* @returns the OSPF tag to be advertised
*
* @comments Defaults to 0 if use hasn't configured otherwise.
*
* @end
*
*********************************************************************/
static L7_uint32
ospfMapRedistRouteTag(L7_routeEntry_t *routeEntry)
{
    L7_REDIST_RT_INDICES_t srcProto = protocolMapping[routeEntry->protocol];
    ospfRedistCfg_t *redistCfg = ospfMapRedistCfgGet(srcProto);
    if (redistCfg) {
        return redistCfg->tag;
    }
    else {
        LOG_MSG("SOFTWARE ERROR: Unable to get redistribution configuration\nin ospfMapRedistRouteTag().\n");
        return FD_OSPF_ASBR_EXT_ROUTE_DEFAULT_ROUTE_TAG;
    }
}

/*********************************************************************
*
* @purpose  Stop redistributing a given destination network.
*
* @param ipAddr @b{(input)} destination address
* @param netMask @b{(input)} destination network mask
*
* @returns L7_SUCCESS
*
* @comments  Removes the route from the redistribution list and
*            withdraws the external LSA.
*
* @end
*
*********************************************************************/
L7_RC_t ospfMapWithdrawRedistRoute(L7_uint32 ipAddr, L7_uint32 netMask)
{
    t_S_AsExternalCfg asExtCfg;     /* describes an external route */

    /* create the ExtCfg */
    asExtCfg.DestNetIpAdr  = ipAddr;
    asExtCfg.DestNetIpMask = netMask;
    asExtCfg.AsExtStatus = ROW_DESTROY;

    /* Remove the route from the redistribution list */
    ospfMapRedistListDelete(ipAddr, netMask);

    /* Withdraw advertisement */
    RTO_AsExternalCfg_Pack((t_RTO *) ospfMapCtrl_g.RTO_Id, &asExtCfg);

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Given a route from a source other than OSPF, determine the OSPF
*  attributes that should be advertised with the route.
*
* @param routeEntry @b{(input)} route from another source
* @param isPreviouslyRedist @b{(input)} has the route been redistributed previously?
* @param asExtCfg @b{(output)} description of the OSPF route derived
*                             from routeEntry
*
* @returns L7_SUCCESS
*
* @comments
*  
*           - If the route was previously redistributed and is being (potentially)
*             changed. Then use the LsId of the existing entry. 
*
*           - Else if we have redistributed a route with the same network number, 
*             create a unique LsId,
*
*           - Else use the network address as the LsId.
*
* @end
*
*********************************************************************/
static L7_RC_t
ospfMapRedistRouteCreate(L7_routeEntry_t *routeEntry, t_S_AsExternalCfg *asExtCfg)
{
    L7_arpEntry_t *nextHop;    /* next hop info */
    L7_OSPF_EXT_METRIC_TYPES_t metType;

    asExtCfg->DestNetIpAdr  = routeEntry->ipAddr;
    asExtCfg->DestNetIpMask = routeEntry->subnetMask;
    asExtCfg->PhyType       = OSPF_ETHERNET;

    /* If next hop of source route is on an interface where OSPF is
     * configured, retain the next hop address as the forwarding address.
     * Otherwise, set the forwarding address to 0.
     */
    nextHop = &routeEntry->ecmpRoutes.equalCostPath[0].arpEntry;
    asExtCfg->Nexthop = nextHop->ipAddr;
    asExtCfg->ForwardingAdr = DefaultDestination;

    /* Determine the appropriate metric and metric type */
    asExtCfg->MetricValue = ospfMapRedistMetric(routeEntry);
    metType = ospfMapRedistMetricType(routeEntry);
    if (metType == L7_OSPF_METRIC_EXT_TYPE1) {
        asExtCfg->IsMetricType2 = L7_FALSE;
    }
    else {
        asExtCfg->IsMetricType2 = L7_TRUE;
    }

    /* Set the OSPF tag */
    asExtCfg->ExtRouteTag = ospfMapRedistRouteTag(routeEntry);

    return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Given a route from a source other than OSPF, determine whether OSPF
*  should redistribute the route.
*
* @param routeEntry @b{(input)} route from another source
*
* @returns L7_SUCCESS
*
* @comments OSPF should redistribute if the user has configured OSPF to
*           redistribute routes from the source of this route. The route
*           must pass additional conditions, including an optional route
*           filter.
*
* @end
*
*********************************************************************/
static L7_BOOL ospfMapMayRedistribute(L7_routeEntry_t *routeEntry)
{
    FILTER_ACTION_t action;
    L7_REDIST_RT_INDICES_t srcProto = protocolMapping[routeEntry->protocol];
    ospfRedistCfg_t *redistCfg = ospfMapRedistCfgGet(srcProto);
    if (redistCfg == NULL) {
        /* This will happen if routeEntry is an OSPF route. */
        return L7_FALSE;
    }
    if (redistCfg->redistribute == L7_FALSE) {
        return L7_FALSE;
    }

    /* Always suppress the default route */
    if ((routeEntry->ipAddr == 0) && (routeEntry->subnetMask == 0)) {
        return L7_FALSE;
    }

    /* OSPF never handles REJECT routes */
    if (routeEntry->flags & L7_RTF_REJECT) {
        return L7_FALSE;
    }

    /* OSPF never redistributes iBGP routes */
    if (routeEntry->protocol == RTO_IBGP) {
        return L7_FALSE;
    }

    /* If routeEntry is a local route, verify that OSPF is not enabled
     * on the interface. If OSPF is on the interface, the destination is
     * advertised already as an internal route.
     */
    if (routeEntry->protocol == RTO_LOCAL) {
        /* Since routeEntry is local, we know there's only one split. */
        L7_uint32 intIfNum = routeEntry->ecmpRoutes.equalCostPath[0].arpEntry.intIfNum;
        L7_uint32 ospfIntfAdminMode;
        if ((ospfMapIntfEffectiveAdminModeGet(intIfNum, &ospfIntfAdminMode) == L7_SUCCESS) &&
            (ospfIntfAdminMode == L7_ENABLE))
        {
            return L7_FALSE;
        }
    }

    /* if a distribute list is configured, filter route through it */
    if (redistCfg->distList != FD_OSPF_DIST_LIST_OUT) {
        if (ospfMapAclRouteFilter(redistCfg->distList, routeEntry->ipAddr,
                                  routeEntry->subnetMask, &action) == L7_SUCCESS) {
            if (action == FILTER_ACTION_DENY) {
                return L7_FALSE;
            }
        }
    }

    /* Make sure destination is a valid unicast address. */
    if (!rtoValidDestAddr(routeEntry->ipAddr, routeEntry->subnetMask)) {
        return L7_FALSE;
    }

    /* if destination of routeEntry is a subnet, only redistribute if
    OSPF is configured to redistribute subnets. */
    if (redistCfg->subnets == L7_FALSE) {
        if (ospfMapIsSubnet(routeEntry->ipAddr, routeEntry->subnetMask)) {
            return L7_FALSE;
        }
    }
    return L7_TRUE;
}

/*********************************************************************
* @purpose  Callback function for notification from RTO of best route
*           changes.
*
* @param    void
*
* @returns  void
*
* @notes    Set a timer. When the timer expires, ask RTO for a set of changes.
*
* @end
*********************************************************************/
void ospfMapExtenRouteCallback()
{
  if (!pOspfInfo->bestRouteTimer)
    /* schedule an event to request changes from RTO */
    osapiTimerAdd((void*)ospfMapBestRouteTimerCb, L7_NULL, L7_NULL, 
                  L7_OSPF_BEST_ROUTE_DELAY, &pOspfInfo->bestRouteTimer);

  return; 
}

/*********************************************************************
*
* @purpose Timer callback to trigger OSPF to get best routes from RTO.
*
* @param void
*
* @returns void
*
* @comments  This callback tells OSPF that best route changes are pending
*            in RTO. OSPF has to go back to RTO and ask for the changes.
*
* @end
*
*********************************************************************/
void ospfMapBestRouteTimerCb(void)
{
  pOspfInfo->bestRouteTimer = NULL;
  ospfMapBestRouteEventPost();
}

/*********************************************************************
*
* @purpose Put an event on the OSPF map thread message queue to 
*          retrieve best route changes from RTO.
*
* @param void
*
* @returns void
*
* @comments  
*
* @end
*
*********************************************************************/
static void ospfMapBestRouteEventPost(void)
{
    ospfMapMsg_t msg;
    L7_RC_t rc;

    if (Ospf_Redist_Queue == L7_NULLPTR) {
        return;
    }

    /* OSPF could have deregistered after getting notified of best route 
     * change. Don't bother RTO if this is the case. */
    if (!ospfIsRegisteredWithRto())
      return;

    memset((void*) &msg, 0, sizeof(ospfMapMsg_t));
    msg.msg = OSPF_RTO_EVENT;

    rc = osapiMessageSend(Ospf_Redist_Queue, &msg, sizeof(ospfMapMsg_t), 
                          L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

    if (rc == L7_SUCCESS)
    {
        osapiSemaGive(ospfMsgQSema);
    }
    else
    {
        OSPFMAP_ERROR("Failed to send best route notification message to OSPF mapping thread.");
    }
    return;
}

/*********************************************************************
*
* @purpose Request a set of best route changes from RTO. If more changes
*          remain pending, generate another event to ask for more.
*
* @param void
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @comments  
*
* @end
*
*********************************************************************/
L7_RC_t ospfMapBestRoutesGet(void)
{
  L7_uint32 i;
  L7_uint32 numChanges = 0;
  L7_BOOL moreChanges = L7_FALSE;
  rtoRouteChange_t *routeChange;

  /* OSPF could have deregistered after getting notified of best route 
   * change. Don't bother RTO if this is the case. */
  if (!ospfIsRegisteredWithRto())
    return L7_SUCCESS;

  if (rtoRouteChangeRequest(ospfMapExtenRouteCallback, L7_OSPF_MAX_BEST_ROUTE_CHANGES,
                            &numChanges, &moreChanges, 
                            ospfRouteChangeBuf) != L7_SUCCESS)
  {
    /* Don't schedule another try. */
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
            "Failed to retrieve best route changes from RTO.");
    return L7_FAILURE;
  }

  /* process each best route change */
  for (i = 0; i < numChanges; i++)
  {
    routeChange = &ospfRouteChangeBuf[i];
    ospfBestRouteChange(&routeChange->routeEntry, routeChange->changeType);
  }

  if (moreChanges)
  {
    /* post another event to the OSPF message queue to request more 
     * changes from RTO */
    ospfMapBestRouteEventPost();
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process a best route change event from RTO.
*
* @param    routeEntry @b{(input)} Route that was added/ deleted/ modified
* @param    route_status @b{(input)} One of RTO_ADD_ROUTE, RTO_DELETE_ROUTE, 
*                                    RTO_CHANGE_ROUTE
*
* @returns  L7_SUCCESS
*
* @notes    Processing of RTO best route changes is done on the
*           OSPF MAP thread.
*       
* @end
*********************************************************************/
static L7_RC_t ospfBestRouteChange(L7_routeEntry_t *routeEntry,
                                  RTO_ROUTE_EVENT_t route_status)
{
    t_S_AsExternalCfg asExtCfg;     /* describes an external route */
    ospfRedistRoute_t redistRoute;  /* Corresponding route in redistribution tree */
    L7_uint32 mode;                 /* OSPF admin mode */

    bzero((L7_char8 *)&asExtCfg, sizeof(t_S_AsExternalCfg));

    if (ospfMapOspfInitialized() != L7_TRUE) {
        return L7_SUCCESS;
    }

    if ((ospfMapOspfAdminModeGet(&mode) != L7_SUCCESS) || (mode != L7_ENABLE)) {
        return L7_SUCCESS;
    }

    switch (route_status)
    {
    case RTO_ADD_ROUTE:
        if (ospfMapMayRedistribute(routeEntry)) {

            /* Determine the attributes of the route to be redistributed */
            ospfMapRedistRouteCreate(routeEntry, &asExtCfg);
            asExtCfg.AsExtStatus = ROW_CREATE_AND_GO;

            /* Add the new route to the mapping layer's tree of routes
                that OSPF is currently redistributing. */
            ospfMapExtenRedistFromExt(&asExtCfg, &redistRoute);
            ospfMapRedistListAdd(&redistRoute);

            /* send the new route to the protocol engine for advertisement */
            RTO_AsExternalCfg_Pack((t_RTO *) ospfMapCtrl_g.RTO_Id, &asExtCfg);
        }

        /* If RTO is adding a default route, and OSPF's origination of a default
         * route depends on the presence of a default route in the routing table,
         * then see if OSPF should now originate.
         */
        if ((routeEntry->ipAddr == 0) && (routeEntry->subnetMask == 0)) {
            pOspfInfo->defaultRouteExists = L7_TRUE;
            if ((pOspfMapCfgData->defRouteCfg.always == L7_FALSE) &&
                (ospfMapOriginatingDefaultRoute() == L7_FALSE)) {
                if (pOspfMapCfgData->defRouteCfg.origDefRoute) {
                    ospfMapExtenDefRouteOrig();
                }
            }
        }
        break;

    case RTO_CHANGE_ROUTE:
        if (ospfMapMayRedistribute(routeEntry)) {

            /* Figure out if OSPF previously redistributed this route */
            if (ospfMapPreviouslyRedistributed(routeEntry->ipAddr,
                                               routeEntry->subnetMask)) {

                /* Create a change notification */
                /* Only update the engine if the route actually changes */
                ospfMapRedistRouteCreate(routeEntry, &asExtCfg);
                ospfMapExtenRedistFromExt(&asExtCfg, &redistRoute);
                asExtCfg.AsExtStatus = ROW_CHANGE;
                if (ospfMapRedistRouteChanged(&redistRoute)) 
                {
                  /* Update the entry on the mapping layer's tree of routes
                   that OSPF is currently redistributing. */
                  ospfMapRedistListModify(&redistRoute);

                  /* Send change route to protocol engine */
                  RTO_AsExternalCfg_Pack((t_RTO *) ospfMapCtrl_g.RTO_Id, &asExtCfg);
                }
            }
            else {
                /* route wasn't redistributed before, but because of change, it
                    should be now. So add it. */
                ospfMapRedistRouteCreate(routeEntry, &asExtCfg);                
                asExtCfg.AsExtStatus = ROW_CREATE_AND_GO;
                ospfMapExtenRedistFromExt(&asExtCfg, &redistRoute);
                ospfMapRedistListAdd(&redistRoute);
                RTO_AsExternalCfg_Pack((t_RTO *) ospfMapCtrl_g.RTO_Id, &asExtCfg);
            }
        } /* end may redistribute */
        else {
            /* route may have been redistributed before change. If so, delete it. */
            if (ospfMapPreviouslyRedistributed(routeEntry->ipAddr,
                                               routeEntry->subnetMask)) {
                ospfMapWithdrawRedistRoute(routeEntry->ipAddr, routeEntry->subnetMask);
            }
        }
        break;

    case RTO_DELETE_ROUTE:
        if (ospfMapPreviouslyRedistributed(routeEntry->ipAddr,
                                           routeEntry->subnetMask)) {
            ospfMapWithdrawRedistRoute(routeEntry->ipAddr,
                                       routeEntry->subnetMask);
        }

        /* If RTO is deleting the default route, and OSPF's origination of a
         * default route was conditioned on the presence of a default route in
         * the routing table, then stop originating default.
         */
        if ((routeEntry->ipAddr == 0) && (routeEntry->subnetMask == 0)) {
            pOspfInfo->defaultRouteExists = L7_FALSE;
            if ((pOspfMapCfgData->defRouteCfg.always == L7_FALSE) &&
                (pOspfInfo->originatingDefaultRoute)) {
                ospfMapExtenDefRouteNoOrig();
            }
        }
        break;

    default:
        /* RTO does not use RTO_xxx_ECMP_ROUTE at this time. */
        LOG_MSG("ERROR:  Invalid route_status %d.\n", route_status);
    }

    return L7_SUCCESS;
}


/*********************************************************************
* @purpose  RTO_Config_Unpack is the callback function to be called
*           by the ospf thread to process the user configuration request
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err RTO_Config_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_Handle      RTO_Id = (t_Handle)PACKET_GET(xxCallInfo, 0);
  t_S_RouterCfg *p_rtoCfg = (t_S_RouterCfg *)PACKET_GET(xxCallInfo, 1);
  e_Err         e;

  e = RTO_Config(RTO_Id, p_rtoCfg);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return e;
}

/*********************************************************************
* @purpose  RTO_Config_Pack is used to enqueue a request for configuring
*           an ospf router object in the ospf thread.
*
* @param    RTO_Id     RTO object Id
* @param    p_Cfg      RTO configuration (see spcfg.h)
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err RTO_Config_Pack(t_Handle RTO_Id, t_S_RouterCfg *p_Cfg)
{
	t_S_RouterCfg	*p_rtoCfg;  /* ospf router object configuration info */
	t_XXCallInfo	*xxci;      /* XX_Call info */
	e_Err			e;
	void			*p_info;
	int				numOfMsgs = 0, i;
	L7_int32		currThread = osapiTaskIdSelf();
	OS_Thread		*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

	/* Fill in the router configuration info */
	p_rtoCfg = XX_Malloc(sizeof(t_S_RouterCfg));
	if (!p_rtoCfg)
	{
		return E_NOMEMORY;
	}

	/* copy user supplied configuration info */
	memcpy(p_rtoCfg, p_Cfg, sizeof(t_S_RouterCfg));

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			XX_Free(p_rtoCfg);
			return E_FAILED;
		}

		PACKET_INIT(xxci, RTO_Config_Unpack, 0, 0, 2, (ulng)RTO_Id);
		PACKET_PUT(xxci, (ulng)p_rtoCfg);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call RTO_Config_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			XX_Free(p_rtoCfg);
			return E_FAILED;
		}
	}
	else
	{
		e = RTO_Config(RTO_Id, p_rtoCfg);
	}

  if ((p_Cfg->RouterStatus == ROW_NOT_IN_SERVICE) ||
      (p_Cfg->RouterStatus == ROW_DESTROY))
  {
    /* read out all the messages in the OSPF queues as they are now stale */
    L7_uint32 q;
    for (q = 0; q < OSPF_NUM_QUEUES; q++)
    {
      osapiMsgQueueGetNumMsgs((void *)(pThreadGlobal->QueueID[q]), &numOfMsgs);
      for (i = 0; i < numOfMsgs; i++)
      {
        if (osapiMessageReceive((void *)(pThreadGlobal->QueueID[q]),
                                (char *)&p_info, OS_MSG_SIZE, L7_NO_WAIT) == L7_SUCCESS)
        {
          /* free the XX_Call parameter block */
          XX_Free(p_info);
        }
      }
    }
  }

	memcpy(p_Cfg, p_rtoCfg, sizeof(t_S_RouterCfg));
	XX_Free(p_rtoCfg);

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

	return E_OK;
}


/*********************************************************************
* @purpose  IFO_Config_Unpack is the callback function to be called
*           by the ospf thread to process the user configuration request
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err IFO_Config_Unpack(void *p_Info)
{
  t_XXCallInfo *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_Handle     IFO_Id = (t_Handle)PACKET_GET(xxCallInfo, 0);
  t_IFO_Cfg    *p_ifoCfg = (t_IFO_Cfg *)PACKET_GET(xxCallInfo, 1);
  L7_uint32    intIfNum;
  e_Err        e = E_FAILED;

  /* If we found the intIfNum in the LM table and this interface still exists 
   * (has not been destroyed) OR this is a virtual interface, then send the 
   * configuration.  Do not need to do the extra check for virtual interfaces 
   * since they do not use ospfProcTask to send create/config/delete commands 
   * to the ospf mapping code.  Therefore the virtual interface usmdb calls to 
   * create/config/delete will be single threaded. 
   */
  if (p_ifoCfg->Type == IFO_VRTL ||
      ((intIfNum = ospfMapLmIfoIntIfNumGet(IFO_Id)) > 0 && 
       pOspfIntfInfo[intIfNum].ospfIfExists == L7_TRUE))
  {
    e = IFO_Config(IFO_Id, p_ifoCfg);
  }


  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return e;
}

/*********************************************************************
* @purpose  IFO_Config_Pack is used to enqueue a request for configuring
*           an ospf interface in the ospf thread.
*
* @param    IFO_Id     IFO object Id
* @param    p_Cfg      IFO configuration (see spcfg.h)
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err IFO_Config_Pack(t_Handle IFO_Id, t_IFO_Cfg *p_Cfg)
{
	t_IFO_Cfg		*p_ifoCfg;  /* interface configuration info */
	t_XXCallInfo	*xxci;      /* XX_Call info */
	e_Err			e;
	L7_int32		currThread = osapiTaskIdSelf();
	OS_Thread		*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

	/* Fill in the interface configuration info */
	p_ifoCfg = XX_Malloc(sizeof(t_IFO_Cfg));

	if (!p_ifoCfg)
	{
		return E_NOMEMORY;
	}

    /* copy user supplied configuration info */
	memcpy(p_ifoCfg, p_Cfg, sizeof(t_IFO_Cfg));

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			XX_Free(p_ifoCfg);
			return E_FAILED;
		}

		PACKET_INIT(xxci, IFO_Config_Unpack, 0, 0, 2, (ulng)IFO_Id);
		PACKET_PUT(xxci, (ulng)p_ifoCfg);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call IFO_Config_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			XX_Free(p_ifoCfg);
			return E_FAILED;
		}
	}
	else
	{
		e = IFO_Config(IFO_Id, p_ifoCfg);
	}

	memcpy(p_Cfg, p_ifoCfg, sizeof(t_IFO_Cfg));
	XX_Free(p_ifoCfg);

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

	return E_OK;
}

/*********************************************************************
* @purpose  ARO_Config_Unpack is the callback function to be called
*           by the ospf thread to process the user configuration request
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err ARO_Config_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_Handle      ARO_Id = (t_Handle)PACKET_GET(xxCallInfo, 0);
  t_S_AreaCfg   *p_aroCfg = (t_S_AreaCfg *)PACKET_GET(xxCallInfo, 1);
  e_Err         e;

  e = ARO_Config(ARO_Id, p_aroCfg);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return e;
}

/*********************************************************************
* @purpose  ARO_Config_Pack is used to enqueue a request for configuring
*           an ospf router object in the ospf thread.
*
* @param    ARO_Id     ARO object Id
* @param    p_Cfg      ARO configuration (see spcfg.h)
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err ARO_Config_Pack(t_Handle ARO_Id, t_S_AreaCfg *p_Cfg)
{
	t_S_AreaCfg		*p_aroCfg;  /* ospf area configuration info */
	t_XXCallInfo	*xxci;      /* XX_Call info */
	e_Err			e;
	L7_int32		currThread = osapiTaskIdSelf();
	OS_Thread		*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

	/* Fill in the area configuration info */
	p_aroCfg = XX_Malloc(sizeof(t_S_AreaCfg));
	if (!p_aroCfg)
	{
		return E_NOMEMORY;
	}
    
	/* copy user supplied configuration info */
	memcpy(p_aroCfg, p_Cfg, sizeof(t_S_AreaCfg));

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			XX_Free(p_aroCfg);
			return E_FAILED;
		}

		PACKET_INIT(xxci, ARO_Config_Unpack, 0, 0, 2, (ulng)ARO_Id);
		PACKET_PUT(xxci, (ulng)p_aroCfg);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call ARO_Config_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			XX_Free(p_aroCfg);
			return E_FAILED;
		}
	}
	else
	{
		e = ARO_Config(ARO_Id, p_aroCfg);
	}

	memcpy(p_Cfg, p_aroCfg, sizeof(t_S_AreaCfg));
	XX_Free(p_aroCfg);

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

	return E_OK;
}

/*********************************************************************
* @purpose  aroLSADbGet_Unpack is the callback function to be called
*           by the ospf thread to process the user configuration request
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err aroLSADbGet_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  L7_uint32     areaId = (L7_uint32)PACKET_GET(xxCallInfo, 0);
  t_S_LsDbEntry *p_DbEntry = (t_S_LsDbEntry *)PACKET_GET(xxCallInfo, 1);
  L7_RC_t       *p_rc = (L7_RC_t *)PACKET_GET(xxCallInfo, 2);
  e_Err         e = E_FAILED;

  *p_rc = aroLSADbGet(areaId, p_DbEntry);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return e;
}

/*********************************************************************
* @purpose  aroLSADbGet_Pack is used to enqueue a request for configuring
*           an ospf router object in the ospf thread.
*
* @param    ARO_Id     ARO object Id
* @param    p_Cfg      ARO configuration (see spcfg.h)
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err aroLSADbGet_Pack(L7_uint32 areaId, t_S_LsDbEntry *p_DbEntry)
{
	t_XXCallInfo	*xxci;      /* XX_Call info */
	e_Err			e;
	L7_RC_t			rc = L7_FAILURE;
	L7_int32		currThread = osapiTaskIdSelf();
	OS_Thread		*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}

		PACKET_INIT(xxci, aroLSADbGet_Unpack, 0, 0, 3, (L7_uint32)areaId);
		PACKET_PUT(xxci, (t_S_LsDbEntry *)p_DbEntry);
		PACKET_PUT(xxci, (L7_RC_t *)&rc);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call aroLSADbGet_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}
	}
	else
	{
		rc = aroLSADbGet(areaId, p_DbEntry);
	}

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

    return (rc == L7_SUCCESS)? E_OK: E_FAILED;
}

/*********************************************************************
* @purpose  ospfExtLsdbAdvertisement_Lookup_Unpack is the callback function
*           to be called by the ospf thread to process the user 
*           configuration request
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err ospfExtLsdbAdvertisement_Lookup_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  ulng Type = (ulng)PACKET_GET(xxCallInfo, 0);
  ulng Lsid = (ulng)PACKET_GET(xxCallInfo, 1);
  ulng RouterId = (ulng)PACKET_GET(xxCallInfo, 2);
  L7_ospfLsdbEntry_t *p_Lsa = (L7_ospfLsdbEntry_t *)PACKET_GET(xxCallInfo, 3);
  L7_char8 **p_LsdbAdvertisement = (L7_char8 **)PACKET_GET(xxCallInfo, 4);
  L7_RC_t *rc = (L7_RC_t *)PACKET_GET(xxCallInfo, 5);

  *rc = ospfExtLsdbAdvertisement_Lookup(Type, Lsid, RouterId,
                                        p_Lsa,
                                        p_LsdbAdvertisement);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return ((*rc == L7_SUCCESS)? E_OK: E_FAILED);
}

/*********************************************************************
* @purpose  ospfLsdbAdvertisement_Lookup_Unpack is the callback function
*           to be called by the ospf thread to process the user 
*           configuration request
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err ospfLsdbAdvertisement_Lookup_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  ulng areaId = (ulng)PACKET_GET(xxCallInfo, 0);
  ulng Type = (ulng)PACKET_GET(xxCallInfo, 1);
  ulng Lsid = (ulng)PACKET_GET(xxCallInfo, 2);
  ulng RouterId = (ulng)PACKET_GET(xxCallInfo, 3);
  L7_ospfLsdbEntry_t *p_Lsa = (L7_ospfLsdbEntry_t *)PACKET_GET(xxCallInfo, 4);
  L7_char8 **p_LsdbAdvertisement = (L7_char8 **)PACKET_GET(xxCallInfo, 5);
  L7_RC_t *rc = (L7_RC_t *)PACKET_GET(xxCallInfo, 6);

  *rc = ospfLsdbAdvertisement_Lookup(areaId, Type, Lsid, RouterId,
                                     p_Lsa,
                                     p_LsdbAdvertisement);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return ((*rc == L7_SUCCESS)? E_OK: E_FAILED);
}

/*********************************************************************
* @purpose  ospfLsdbTable_Lookup_Unpack is the callback function to be called
*           by the ospf thread to process the user configuration request
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err ospfLsdbTable_Lookup_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  ulng areaId = (ulng)PACKET_GET(xxCallInfo, 0);
  ulng Type = (ulng)PACKET_GET(xxCallInfo, 1);
  ulng Lsid = (ulng)PACKET_GET(xxCallInfo, 2);
  ulng RouterId = (ulng)PACKET_GET(xxCallInfo, 3);
  L7_ospfLsdbEntry_t *p_Lsa = (L7_ospfLsdbEntry_t *)PACKET_GET(xxCallInfo, 4);
  L7_RC_t *rc = (L7_RC_t *)PACKET_GET(xxCallInfo, 5);

  *rc = ospfLsdbTable_Lookup(areaId, Type, Lsid, RouterId, p_Lsa);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return ((*rc == L7_SUCCESS)? E_OK: E_FAILED);
}

/*********************************************************************
* @purpose  ospfLsdbAdvertisement_Lookup_Pack is used to enqueue a 
*           request for configuring an ospf router object in the ospf thread.
*
* @param    LsdbAreaId     Area Id
* @param    LsdbType       Lsid Type
* @param    LsdbId         Link State Id
* @param    LsdbRouterId   Router Id
* @param    p_Lsa          Lsdb Entry given to external world
* @param    p_LsdbAdvertisement pointer to address where LSDB Advertisement is
*                               copied by the OSPF code
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err ospfLsdbAdvertisement_Lookup_Pack(L7_uint32 LsdbAreaId,
                                        L7_uint32 LsdbType, L7_uint32 LsdbId,
                                        L7_uint32 LsdbRouterId,
                                        L7_ospfLsdbEntry_t *p_Lsa,
                                        L7_char8 **p_LsdbAdvertisement)
{
	t_XXCallInfo	*xxci;      /* XX_Call info */
	e_Err			e;
	L7_RC_t			rc = L7_FAILURE;
	L7_int32		currThread = osapiTaskIdSelf();
	OS_Thread		*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;
	L7_ospfLsdbEntry_t	temp_Lsa;

	if(p_Lsa == NULL)
	{
		p_Lsa = &temp_Lsa;
	}

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}

		PACKET_INIT(xxci, ospfLsdbAdvertisement_Lookup_Unpack, 0, 0, 7,
					(L7_uint32)LsdbAreaId);
		PACKET_PUT(xxci, (L7_uint32)LsdbType);
		PACKET_PUT(xxci, (L7_uint32)LsdbId);
		PACKET_PUT(xxci, (L7_uint32)LsdbRouterId);
		PACKET_PUT(xxci, (L7_ospfLsdbEntry_t *)p_Lsa);
		PACKET_PUT(xxci, (L7_char8 **)p_LsdbAdvertisement);
		PACKET_PUT(xxci, (L7_RC_t *)&rc);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call ospfLsdbAdvertisement_Lookup_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}
	}
	else
	{
		rc = ospfLsdbAdvertisement_Lookup(LsdbAreaId, LsdbType, 
						  LsdbId, LsdbRouterId,
						  p_Lsa,
						  p_LsdbAdvertisement);
	}

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

	return (rc == L7_SUCCESS)? E_OK: E_FAILED;
}

/*********************************************************************
* @purpose  ospfLsdbTable_Lookup_Pack is used to enqueue a request for configuring
*           an ospf router object in the ospf thread.
*
* @param    LsdbAreaId     Area Id
* @param    LsdbType       Lsid Type
* @param    LsdbId         Link State Id
* @param    LsdbRouterId   Router Id
* @param    p_Lsa          Lsdb Entry given to external world
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err ospfLsdbTable_Lookup_Pack(L7_uint32 LsdbAreaId,
                                L7_uint32 LsdbType, L7_uint32 LsdbId,
                                L7_uint32 LsdbRouterId,
                                L7_ospfLsdbEntry_t *p_Lsa)
{
	t_XXCallInfo	*xxci;      /* XX_Call info */
	e_Err			e;
	L7_RC_t			rc = L7_FAILURE;
	L7_int32		currThread = osapiTaskIdSelf();
	OS_Thread		*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;
	L7_ospfLsdbEntry_t	temp_Lsa;

	if(p_Lsa == NULL)
	{
		p_Lsa = &temp_Lsa;
	}

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}

		PACKET_INIT(xxci, ospfLsdbTable_Lookup_Unpack, 0, 0, 6, (L7_uint32)LsdbAreaId);
		PACKET_PUT(xxci, (L7_uint32)LsdbType);
		PACKET_PUT(xxci, (L7_uint32)LsdbId);
		PACKET_PUT(xxci, (L7_uint32)LsdbRouterId);
		PACKET_PUT(xxci, (L7_ospfLsdbEntry_t *)p_Lsa);
		PACKET_PUT(xxci, (L7_RC_t *)&rc);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call ospfLsdbTable_Lookup_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}
	}
	else
	{
		rc = ospfLsdbTable_Lookup(LsdbAreaId, LsdbType, LsdbId, LsdbRouterId, p_Lsa);
	}

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

	return (rc == L7_SUCCESS)? E_OK: E_FAILED;
}

/*********************************************************************
* @purpose  ospfLsdbTable_GetNext_Unpack is the callback function to be called
*           by the ospf thread to process the user configuration request
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err ospfLsdbTable_GetNext_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  L7_uint32 *p_areaId = (L7_uint32 *)PACKET_GET(xxCallInfo, 0);
  L7_uint32 *p_Type = (L7_uint32 *)PACKET_GET(xxCallInfo, 1);
  L7_uint32 *p_Lsid = (L7_uint32 *)PACKET_GET(xxCallInfo, 2);
  L7_uint32 *p_RouterId = (L7_uint32 *)PACKET_GET(xxCallInfo, 3);
  L7_ospfLsdbEntry_t *p_Lsa = (L7_ospfLsdbEntry_t *)PACKET_GET(xxCallInfo, 4);
  L7_RC_t *rc = (L7_RC_t *)PACKET_GET(xxCallInfo, 5);

  *rc = ospfLsdbTable_GetNext(p_areaId, p_Type, p_Lsid, p_RouterId, p_Lsa);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return ((*rc == L7_SUCCESS)? E_OK: E_FAILED);
}

/*********************************************************************
* @purpose  ospfLsdbTable_GetNext_Pack is used to enqueue a request for configuring
*           an ospf router object in the ospf thread.
*
* @param    *areaId        Area Id
* @param    *Type          Lsid Type
* @param    *Lsid          Link State Id
* @param    *RouterId      Router Id
* @param    p_Lsa          Lsdb Entry given to external world
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err ospfLsdbTable_GetNext_Pack(L7_uint32 *areaId,
                                 L7_uint32 *Type, L7_uint32 *Lsid,
                                 L7_uint32 *RouterId,
                                 L7_ospfLsdbEntry_t *p_Lsa)
{
	t_XXCallInfo	*xxci;      /* XX_Call info */
	e_Err			e;
	L7_RC_t         rc = L7_FAILURE;
	L7_int32		currThread = osapiTaskIdSelf();
	OS_Thread		*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;
	L7_ospfLsdbEntry_t	temp_Lsa;

	if(p_Lsa == NULL)
	{
		p_Lsa = &temp_Lsa;
	}

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}

		PACKET_INIT(xxci, ospfLsdbTable_GetNext_Unpack, 0, 0, 6, (L7_uint32)areaId);
		PACKET_PUT(xxci, (L7_uint32 *)Type);
		PACKET_PUT(xxci, (L7_uint32 *)Lsid);
		PACKET_PUT(xxci, (L7_uint32 *)RouterId);
		PACKET_PUT(xxci, (L7_ospfLsdbEntry_t *)p_Lsa);
		PACKET_PUT(xxci, (L7_RC_t *)&rc);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call ospfLsdbTable_GetNext_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}
	}
	else
	{
		rc = ospfLsdbTable_GetNext(areaId, Type, Lsid, RouterId, p_Lsa);
	}

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

	return (rc == L7_SUCCESS)? E_OK: E_FAILED;
}

/*********************************************************************
 * @purpose  ospfExtLsdbTable_Lookup_Unpack is the callback function
 *           to be called by the ospf thread to process the user 
 *           configuration request
 *
 * @param    p_Info         pointer to the message containing
 *                          the required parameters
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
static e_Err ospfExtLsdbTable_Lookup_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  ulng Type = (ulng)PACKET_GET(xxCallInfo, 0);
  ulng Lsid = (ulng)PACKET_GET(xxCallInfo, 1);
  ulng RouterId = (ulng)PACKET_GET(xxCallInfo, 2);
  L7_ospfLsdbEntry_t *p_Lsa = (L7_ospfLsdbEntry_t *)PACKET_GET(xxCallInfo, 3);
  L7_RC_t *rc = (L7_RC_t *)PACKET_GET(xxCallInfo, 4);

  *rc = ospfExtLsdbTable_Lookup(Type, Lsid, RouterId, p_Lsa);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return ((*rc == L7_SUCCESS)? E_OK: E_FAILED);
}

/*********************************************************************
* @purpose  ospfExtLsdbAdvertisement_Lookup_Pack is used to enqueue a request
*           for configuring an ospf router object in the ospf thread.
*
* @param    LsdbType       Lsid Type
* @param    LsdbId         Link State Id
* @param    LsdbRouterId   Router Id
* @param    p_Lsa          Lsdb Entry given to external world
* @param    p_LsdbAdvertisement pointer to address where LSDB Advertisement is
*                               copied by the OSPF code
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*
* @notes
*
* @end
*********************************************************************/
e_Err ospfExtLsdbAdvertisement_Lookup_Pack(L7_uint32 LsdbType, L7_uint32 LsdbId,
                                           L7_uint32 LsdbRouterId,
                                           L7_ospfLsdbEntry_t *p_Lsa,
                                           L7_char8 **p_LsdbAdvertisement)
{
	t_XXCallInfo	*xxci;      /* XX_Call info */
	e_Err			e;
	L7_RC_t			rc = L7_FAILURE;
	L7_int32		currThread = osapiTaskIdSelf();
	OS_Thread		*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;
	L7_ospfLsdbEntry_t	temp_Lsa;

	if(p_Lsa == NULL)
	{
		p_Lsa = &temp_Lsa;
	}

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}

		PACKET_INIT(xxci, ospfExtLsdbAdvertisement_Lookup_Unpack, 0, 0, 6,
			       				(L7_uint32)LsdbType);
		PACKET_PUT(xxci, (L7_uint32)LsdbId);
		PACKET_PUT(xxci, (L7_uint32)LsdbRouterId);
		PACKET_PUT(xxci, (L7_ospfLsdbEntry_t *)p_Lsa);
		PACKET_PUT(xxci, (L7_char8 **)p_LsdbAdvertisement);
		PACKET_PUT(xxci, (L7_RC_t *)&rc);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call ospfExtLsdbAdvertisement_Lookup_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}
	}
	else
	{
		rc = ospfExtLsdbAdvertisement_Lookup(LsdbType, LsdbId, LsdbRouterId,
			       			     p_Lsa,
						     p_LsdbAdvertisement);
	}

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

	return (rc == L7_SUCCESS)? E_OK: E_FAILED;
}
/*********************************************************************
* @purpose  ospfExtLsdbTable_Lookup_Pack is used to enqueue a request
*           for configuring an ospf router object in the ospf thread.
*
* @param    LsdbType       Lsid Type
* @param    LsdbId         Link State Id
* @param    LsdbRouterId   Router Id
* @param    p_Lsa          Lsdb Entry given to external world
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*
* @notes
*
* @end
*********************************************************************/
e_Err ospfExtLsdbTable_Lookup_Pack(L7_uint32 LsdbType, L7_uint32 LsdbId,
                                   L7_uint32 LsdbRouterId,
                                   L7_ospfLsdbEntry_t *p_Lsa)
{
	t_XXCallInfo	*xxci;      /* XX_Call info */
	e_Err			e;
	L7_RC_t			rc = L7_FAILURE;
	L7_int32		currThread = osapiTaskIdSelf();
	OS_Thread		*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;
	L7_ospfLsdbEntry_t	temp_Lsa;

	if(p_Lsa == NULL)
	{
		p_Lsa = &temp_Lsa;
	}

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}

		PACKET_INIT(xxci, ospfExtLsdbTable_Lookup_Unpack, 0, 0, 5, (L7_uint32)LsdbType);
		PACKET_PUT(xxci, (L7_uint32)LsdbId);
		PACKET_PUT(xxci, (L7_uint32)LsdbRouterId);
		PACKET_PUT(xxci, (L7_ospfLsdbEntry_t *)p_Lsa);
		PACKET_PUT(xxci, (L7_RC_t *)&rc);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call ospfExtLsdbTable_Lookup_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}
	}
	else
	{
		rc = ospfExtLsdbTable_Lookup(LsdbType, LsdbId, LsdbRouterId, p_Lsa);
	}

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

	return (rc == L7_SUCCESS)? E_OK: E_FAILED;
}

/*********************************************************************
* @purpose  ospfExtLsdbTable_GetNext_Unpack is the callback function
*           to be called by the ospf thread to process the user
*           configuration request
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err ospfExtLsdbTable_GetNext_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  L7_uint32 *p_Type = (L7_uint32 *)PACKET_GET(xxCallInfo, 0);
  L7_uint32 *p_Lsid = (L7_uint32 *)PACKET_GET(xxCallInfo, 1);
  L7_uint32 *p_RouterId = (L7_uint32 *)PACKET_GET(xxCallInfo, 2);
  L7_ospfLsdbEntry_t *p_Lsa = (L7_ospfLsdbEntry_t *)PACKET_GET(xxCallInfo, 3);
  L7_RC_t *rc = (L7_RC_t *)PACKET_GET(xxCallInfo, 4);

  *rc = ospfExtLsdbTable_GetNext(p_Type, p_Lsid, p_RouterId, p_Lsa);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return ((*rc == L7_SUCCESS)? E_OK: E_FAILED);
}

/*********************************************************************
* @purpose  ospfExtLsdbTable_GetNext_Pack is used to enqueue a request
*           for configuring an ospf router object in the ospf thread.
*
* @param    *Type          Lsid Type
* @param    *Lsid          Link State Id
* @param    *RouterId      Router Id
* @param    p_Lsa          Lsdb Entry given to external world
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err ospfExtLsdbTable_GetNext_Pack(L7_uint32 *Type, L7_uint32 *Lsid,
                                    L7_uint32 *RouterId,
                                    L7_ospfLsdbEntry_t *p_Lsa)
{
	t_XXCallInfo	*xxci;      /* XX_Call info */
	e_Err			e;
	L7_RC_t         rc = L7_FAILURE;
	L7_int32		currThread = osapiTaskIdSelf();
	OS_Thread		*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;
	L7_ospfLsdbEntry_t	temp_Lsa;

	if(! p_Lsa)
	{
		p_Lsa = &temp_Lsa;
	}

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}

		PACKET_INIT(xxci, ospfExtLsdbTable_GetNext_Unpack, 0, 0, 5, (L7_uint32)Type);
		PACKET_PUT(xxci, (L7_uint32 *)Lsid);
		PACKET_PUT(xxci, (L7_uint32 *)RouterId);
		PACKET_PUT(xxci, (L7_ospfLsdbEntry_t *)p_Lsa);
		PACKET_PUT(xxci, (L7_RC_t *)&rc);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call ospfExtLsdbTable_GetNext_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}
	}
	else
	{
		rc = ospfExtLsdbTable_GetNext(Type, Lsid, RouterId, p_Lsa);
	}

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

	return (rc == L7_SUCCESS)? E_OK: E_FAILED;
}

/*********************************************************************
* @purpose  NBO_Config_Unpack is the callback function to be called
*           by the ospf thread to process the user configuration request
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err NBO_Config_Unpack(void *p_Info)
{
  t_XXCallInfo    *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_Handle        NBO_Id = (t_Handle)PACKET_GET(xxCallInfo, 0);
  t_S_NeighborCfg *p_nboCfg = (t_S_NeighborCfg *)PACKET_GET(xxCallInfo, 1);
  e_Err           e;

  e = NBO_Config(NBO_Id, p_nboCfg);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return e;
}

/*********************************************************************
* @purpose  NBO_Config_Pack is used to enqueue a request for configuring
*           an ospf router object in the ospf thread.
*
* @param    NBO_Id     NBO object Id
* @param    p_Cfg      NBO configuration (see spcfg.h)
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err NBO_Config_Pack(t_Handle NBO_Id, t_S_NeighborCfg *p_Cfg)
{
	t_S_NeighborCfg	*p_nboCfg;  /* ospf neighbor configuration info */
	t_XXCallInfo	*xxci;      /* XX_Call info */
	e_Err			e;
	L7_int32		currThread = osapiTaskIdSelf();
	OS_Thread		*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

	/* Fill in the neighbor configuration info */
	p_nboCfg = XX_Malloc(sizeof(t_S_NeighborCfg));
	if (!p_nboCfg)
	{
		return E_NOMEMORY;
	}
    
	/* copy user supplied configuration info */
	memcpy(p_nboCfg, p_Cfg, sizeof(t_S_NeighborCfg));

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			XX_Free(p_nboCfg);
			return E_FAILED;
		}

		PACKET_INIT(xxci, NBO_Config_Unpack, 0, 0, 2, (ulng)NBO_Id);
		PACKET_PUT(xxci, (ulng)p_nboCfg);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call NBO_Config_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			XX_Free(p_nboCfg);
			return E_FAILED;
		}
	}
	else
	{
		e = NBO_Config(NBO_Id, p_nboCfg);
	}

	memcpy(p_Cfg, p_nboCfg, sizeof(t_S_NeighborCfg));
	XX_Free(p_nboCfg);

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

	return E_OK;
}

/*********************************************************************
* @purpose  RTO_AsExternalCfg_Unpack is the callback function to be called
*           by the ospf thread to process the user configuration request
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err RTO_AsExternalCfg_Unpack(void *p_Info)
{
  t_XXCallInfo      *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_Handle          RTO_Id = (t_Handle)PACKET_GET(xxCallInfo, 0);
  t_S_AsExternalCfg *p_asExtCfg = (t_S_AsExternalCfg *)PACKET_GET(xxCallInfo, 1);
  e_Err         e;

  e = RTO_AsExternalCfg(RTO_Id, p_asExtCfg);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return e;
}

/*********************************************************************
* @purpose  RTO_AsExternalCfg_Pack is used to enqueue a request for configuring
*           an ospf router object in the ospf thread.
*
* @param    RTO_Id     RTO object Id
* @param    p_Cfg      RTO configuration (see spcfg.h)
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err RTO_AsExternalCfg_Pack(t_Handle RTO_Id, t_S_AsExternalCfg *p_Cfg)
{
  t_S_AsExternalCfg *p_asExtCfg;  /* ospf router object configuration info */
  t_XXCallInfo      *xxci;        /* XX_Call info */
  e_Err             e;
  L7_int32			currThread = osapiTaskIdSelf();
  OS_Thread			*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

  /* Fill in the router configuration info */
  p_asExtCfg = XX_Malloc(sizeof(t_S_AsExternalCfg));
  if(!p_asExtCfg)
    return E_NOMEMORY;

  /* copy user supplied configuration info */
  memcpy(p_asExtCfg, p_Cfg, sizeof(t_S_AsExternalCfg));

  /*
  Due to a callback for RTO, this function can be called
  on the main OSPF thread.  If this happens, the sync
  semaphore should not be used, AND the call to
  RTO_AsExternalCfg should happen inline
  */

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    if(osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG( "Failed to take OSPF sync semaphore\n");
          XX_Free(p_asExtCfg);
      return E_FAILED;
    }

    PACKET_INIT(xxci, RTO_AsExternalCfg_Unpack, 0, 0, 2, (ulng)RTO_Id);
    PACKET_PUT(xxci, (ulng)p_asExtCfg);
    e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);
    if (e != E_OK)
		{
			LOG_MSG("Failed to call RTO_AsExternalCfg_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

    if(osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG( "Failed to take OSPF sync semaphore\n");
          XX_Free(p_asExtCfg);
      return E_FAILED;
    }
  }
  else
  {
    e = RTO_AsExternalCfg(RTO_Id, p_asExtCfg);
  }

  memcpy(p_Cfg, p_asExtCfg, sizeof(t_S_AsExternalCfg));
  XX_Free(p_asExtCfg);

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
	  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  }

  return E_OK;
}

/*********************************************************************
* @purpose  RTO_PurgeExternalLsas_Unpack is the callback function
*           to be called by the ospf thread to purge all external LSAs.
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err RTO_PurgeExternalLsas_Unpack(void *p_Info)
{
  t_XXCallInfo      *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_Handle          RTO_Id = (t_Handle)PACKET_GET(xxCallInfo, 0);
  e_Err         e;

  e = RTO_PurgeExternalLsas(RTO_Id);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return e;
}

/*********************************************************************
* @purpose  RTO_PurgeExternalLsas_Pack is used to enqueue a request to
*           purge all external LSAs in the ospf thread.
*
* @param    RTO_Id     RTO object Id
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err RTO_PurgeExternalLsas_Pack(t_Handle RTO_Id)
{
	t_XXCallInfo      *xxci;        /* XX_Call info */
	e_Err             e;
	L7_int32		currThread = osapiTaskIdSelf();
	OS_Thread		*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}

		PACKET_INIT(xxci, RTO_PurgeExternalLsas_Unpack, 0, 0, 1, (ulng)RTO_Id);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call RTO_PurgeExternalLsas_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}
	}
	else
	{
		e = RTO_PurgeExternalLsas(RTO_Id);
	}

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

	return E_OK;
}

/*********************************************************************
* @purpose  ARO_StubMetric_Unpack is the callback function to be called
*           by the ospf thread to process the user configuration request
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err ARO_StubMetric_Unpack(void *p_Info)
{
  t_XXCallInfo        *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_Handle            ARO_Id = (t_Handle)PACKET_GET(xxCallInfo, 0);
  t_S_StubAreaEntry   *p_stubCfg = (t_S_StubAreaEntry *)PACKET_GET(xxCallInfo, 1);
  e_Err         e;

  e = ARO_StubMetric(ARO_Id, p_stubCfg);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return e;
}

/*********************************************************************
* @purpose  ARO_StubMetric_Pack is used to enqueue a request for configuring
*           an ospf router object in the ospf thread.
*
* @param    ARO_Id     ARO object Id
* @param    p_Cfg      ARO configuration (see spcfg.h)
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err ARO_StubMetric_Pack(t_Handle ARO_Id, t_S_StubAreaEntry *p_Cfg)
{
	t_S_StubAreaEntry   *p_stubCfg;  /* ospf area configuration info */
	t_XXCallInfo        *xxci;      /* XX_Call info */
	e_Err               e;
	L7_int32			currThread = osapiTaskIdSelf();
	OS_Thread			*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

	/* Fill in the area configuration info */
	p_stubCfg = XX_Malloc(sizeof(t_S_StubAreaEntry));
	if (!p_stubCfg)
	{
		return E_NOMEMORY;
	}
    
	/* copy user supplied configuration info */
	memcpy(p_stubCfg, p_Cfg, sizeof(t_S_StubAreaEntry));

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			XX_Free(p_stubCfg);
			return E_FAILED;
		}

		PACKET_INIT(xxci, ARO_StubMetric_Unpack, 0, 0, 2, (ulng)ARO_Id);
		PACKET_PUT(xxci, (ulng)p_stubCfg);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call ARO_StubMetric_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			XX_Free(p_stubCfg);
			return E_FAILED;
		}
	}
	else
	{
		e = ARO_StubMetric(ARO_Id, p_stubCfg);
	}

	memcpy(p_Cfg, p_stubCfg, sizeof(t_S_StubAreaEntry));
	XX_Free(p_stubCfg);

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

	return E_OK;
}

/*********************************************************************
* @purpose  ARO_AddressRange_Unpack is the callback function to be called
*           by the ospf thread to process the user configuration request
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err ARO_AddressRange_Unpack(void *p_Info)
{
  t_XXCallInfo           *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_S_AreaAggregateEntry *p_adrRange = (t_S_AreaAggregateEntry *)PACKET_GET(xxCallInfo, 0);
  ulng					 giveSem = PACKET_GET(xxCallInfo, 1);
  e_Err                  e;
  t_ARO                  *p_ARO = NULLP;
  t_RTO                  *p_RTO = (t_RTO *)ospfMapCtrl_g.RTO_Id;
  t_IFO                  ifo;

  bzero((char *)&ifo, sizeof(t_IFO));

  /* Fill in the required information for the ifo */
  ifo.RTO_Id = ospfMapCtrl_g.RTO_Id;
  ifo.Cfg.AreaId = p_adrRange->AreaId;

  /* If the area does not exist in the RTOs hash list, create it.  The address
  ** range configuration will be automatically applied in the user callback
  ** function
  */
  if(HL_FindFirst(p_RTO->AroHl, (byte*)&(p_adrRange->AreaId), (void *)&p_ARO) != E_OK)
  {
    e = ARO_Init(&ifo, (void *)&p_ARO);
    if (e != E_OK)
    {
      if (giveSem != 0)
      {
        osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
      }
      return e;
    }
  }
  
  e = ARO_AddressRange((t_Handle)p_ARO, p_adrRange);

  if (giveSem != 0)
  {
	  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  }

  return e;
}

/*********************************************************************
* @purpose  ARO_AddressRange_Pack is used to enqueue a request for configuring
*           an ospf router object in the ospf thread.
*
* @param    p_Cfg       ARO configuration (see spcfg.h)
*
* @returns  E_OK        config successfuly setup or read
*           E_FAILED    couldn't setup or read
*           E_BADPARM   bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err ARO_AddressRange_Pack(t_S_AreaAggregateEntry *p_Cfg)
{
	t_S_AreaAggregateEntry  *p_adrRange; /* ospf area configuration info */
	t_XXCallInfo            *xxci;       /* XX_Call info */
	e_Err                   e;
	L7_int32				currThread = osapiTaskIdSelf();
	OS_Thread				*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

	/* Fill in the area configuration info */
	p_adrRange = XX_Malloc(sizeof(t_S_AreaAggregateEntry));
	if (!p_adrRange)
	{
		return E_NOMEMORY;
	}
    
	/* copy user supplied configuration info */
	memcpy(p_adrRange, p_Cfg, sizeof(t_S_AreaAggregateEntry));
	PACKET_INIT(xxci, ARO_AddressRange_Unpack, 0, 0, 2, (ulng)p_adrRange);

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			XX_Free(p_adrRange);
			return E_FAILED;
		}

		PACKET_PUT(xxci, (ulng) 1);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call ARO_AddressRange_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			XX_Free(p_adrRange);
			return E_FAILED;
		}
	}
	else
	{
		PACKET_PUT(xxci, (ulng) 0);
		e = ARO_AddressRange_Unpack(xxci);
		XX_Free(xxci);
	}

	memcpy(p_Cfg, p_adrRange, sizeof(t_S_AreaAggregateEntry));
	XX_Free(p_adrRange);

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

	return E_OK;
}

/*********************************************************************
* @purpose  ARO_HostConfig_Unpack is the callback function to be called
*           by the ospf thread to process the user configuration request
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err ARO_HostConfig_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_Handle      ARO_Id = (t_Handle)PACKET_GET(xxCallInfo, 0);
  t_S_HostRouteCfg   *p_hosts = (t_S_HostRouteCfg *)PACKET_GET(xxCallInfo, 1);
  e_Err         e;

  e = ARO_HostConfig(ARO_Id, p_hosts);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return e;
}

/*********************************************************************
* @purpose  ARO_HostConfig_Pack is used to enqueue a request for configuring
*           an ospf router object in the ospf thread.
*
* @param    ARO_Id     ARO object Id
* @param    p_Cfg      ARO configuration (see spcfg.h)
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err ARO_HostConfig_Pack(t_Handle ARO_Id, t_S_HostRouteCfg *p_Cfg)
{
	t_S_HostRouteCfg	*p_hosts;  /* ospf area configuration info */
	t_XXCallInfo		*xxci;      /* XX_Call info */
	e_Err				e;
	L7_int32			currThread = osapiTaskIdSelf();
	OS_Thread			*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

	/* Fill in the area configuration info */
	p_hosts = XX_Malloc(sizeof(t_S_HostRouteCfg));
	if (!p_hosts)
	{
		return E_NOMEMORY;
	}
    
	/* copy user supplied configuration info */
	memcpy(p_hosts, p_Cfg, sizeof(t_S_HostRouteCfg));

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			XX_Free(p_hosts);
			return E_FAILED;
		}

		PACKET_INIT(xxci, ARO_HostConfig_Unpack, 0, 0, 2, (ulng)ARO_Id);
		PACKET_PUT(xxci, (ulng)p_hosts);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call ARO_HostConfig_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			XX_Free(p_hosts);
			return E_FAILED;
		}
	}
	else
	{
		e = ARO_HostConfig(ARO_Id, p_hosts);
	}

	memcpy(p_Cfg, p_hosts, sizeof(t_S_HostRouteCfg));
	XX_Free(p_hosts);

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

	return E_OK;
}

/*********************************************************************
* @purpose  ARO_Delete_Unpack is the callback function to be called
*           by the ospf thread to process the user configuration request
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err ARO_Delete_Unpack(void *p_Info)
{
  t_XXCallInfo *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_Handle     ARO_Id = (t_Handle)PACKET_GET(xxCallInfo, 0);
  ulng         flag = (ulng)PACKET_GET(xxCallInfo, 1);
  e_Err        e;

  e = ARO_Delete(ARO_Id, flag);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return e;
}

/*********************************************************************
* @purpose  ARO_Delete_Pack is used to enqueue a request for configuring
*           an ospf interface in the ospf thread.
*
* @param    ARO_Id     IFO object Id
* @param    flag       set to TRUE/FALSE specified by the calling function
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err ARO_Delete_Pack(t_Handle ARO_Id, ulng flag)
{
	t_XXCallInfo	*xxci;      /* XX_Call info */
	e_Err			e;
	L7_int32		currThread = osapiTaskIdSelf();
	OS_Thread		*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}

		PACKET_INIT(xxci, ARO_Delete_Unpack, 0, 0, 2, (ulng)ARO_Id);
		PACKET_PUT(xxci, (ulng)flag);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call ARO_Delete_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}
	}
	else
	{
		e = ARO_Delete(ARO_Id, flag);
	}

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

	return E_OK;
}

/*********************************************************************
* @purpose  IFO_Delete_Unpack is the callback function to be called
*           by the ospf thread to process the user configuration request
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err IFO_Delete_Unpack(void *p_Info)
{
  t_XXCallInfo *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_Handle     IFO_Id = (t_Handle)PACKET_GET(xxCallInfo, 0);
  ulng         flag = (ulng)PACKET_GET(xxCallInfo, 1);
  ulng         intIfNum = (ulng)PACKET_GET(xxCallInfo, 2);
  e_Err        e;

  e = IFO_Delete(IFO_Id, flag);
  if (intIfNum)
  {
      pOspfIntfInfo[intIfNum].ospfIfExists = L7_FALSE;
  }

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return e;
}

/*********************************************************************
* @purpose  IFO_Delete_Pack is used to enqueue a request for configuring
*           an ospf interface in the ospf thread.
*
* @param    IFO_Id     IFO object Id
* @param    flag       set to TRUE/FALSE specified by the calling function
* @param    intIfNum   internal interface number for the IFO object or 0 
*                      if a virtual interface.
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err IFO_Delete_Pack(t_Handle IFO_Id, ulng flag, L7_uint32 intIfNum)
{
	t_XXCallInfo	*xxci;      /* XX_Call info */
	e_Err			e;
	L7_int32		currThread = osapiTaskIdSelf();
	OS_Thread		*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}

		PACKET_INIT(xxci, IFO_Delete_Unpack, 0, 0, 3, (ulng)IFO_Id);
		PACKET_PUT(xxci, (ulng)flag);
		PACKET_PUT(xxci, (ulng)intIfNum);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call IFO_Delete_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}
	}
	else
	{
		e = IFO_Delete(IFO_Id, flag);
	}

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

	return E_OK;
}

/*********************************************************************
* @purpose  IFO_Up_Unpack is the callback function to be called
*           by the ospf thread to process the user configuration request
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err IFO_Up_Unpack(void *p_Info)
{
  t_XXCallInfo *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_Handle     IFO_Id = (t_Handle)PACKET_GET(xxCallInfo, 0);
  e_Err        e;

  e = IFO_Up(IFO_Id);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return e;
}

/*********************************************************************
* @purpose  IFO_Up_Pack is used to enqueue a request for configuring
*           an ospf interface in the ospf thread.
*
* @param    IFO_Id     IFO object Id
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err IFO_Up_Pack(t_Handle IFO_Id)
{
	t_XXCallInfo	*xxci;      /* XX_Call info */
	e_Err			e;
	L7_int32		currThread = osapiTaskIdSelf();
	OS_Thread		*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}

		PACKET_INIT(xxci, IFO_Up_Unpack, 0, 0, 1, (ulng)IFO_Id);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call IFO_Up_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}
	}
	else
	{
		e = IFO_Up(IFO_Id);
	}

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

	return E_OK;
}

/*********************************************************************
* @purpose  IFO_Down_Unpack is the callback function to be called
*           by the ospf thread to process the user configuration request
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err IFO_Down_Unpack(void *p_Info)
{
  t_XXCallInfo *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_Handle     IFO_Id = (t_Handle)PACKET_GET(xxCallInfo, 0);
  e_Err        e;

  e = IFO_Down(IFO_Id);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return e;
}

/*********************************************************************
* @purpose  IFO_Down_Pack is used to enqueue a request for configuring
*           an ospf interface in the ospf thread.
*
* @param    IFO_Id     IFO object Id
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err IFO_Down_Pack(t_Handle IFO_Id)
{
	t_XXCallInfo	*xxci;      /* XX_Call info */
	e_Err			e;
	L7_int32		currThread = osapiTaskIdSelf();
	OS_Thread		*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}

		PACKET_INIT(xxci, IFO_Down_Unpack, 0, 0, 1, (ulng)IFO_Id);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call IFO_Down_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			return E_FAILED;
		}
	}
	else
	{
		e = IFO_Down(IFO_Id);
	}

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

	return E_OK;
}

/*********************************************************************
* @purpose  IFO_MetricConfig_Unpack is the callback function to be called
*           by the ospf thread to process the user configuration request
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err IFO_MetricConfig_Unpack(void *p_Info)
{
  t_XXCallInfo *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_Handle     IFO_Id = (t_Handle)PACKET_GET(xxCallInfo, 0);
  t_S_IfMetric *p_metric = (t_S_IfMetric *)PACKET_GET(xxCallInfo, 1);
  e_Err        e;

  e = IFO_MetricConfig(IFO_Id, p_metric);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return e;
}

/*********************************************************************
* @purpose  IFO_MetricConfig_Pack is used to enqueue a request for configuring
*           an ospf interface in the ospf thread.
*
* @param    IFO_Id     IFO object Id
* @param    p_Cfg      IFO configuration (see spcfg.h)
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err IFO_MetricConfig_Pack(t_Handle IFO_Id, t_S_IfMetric *p_Cfg)
{
	t_S_IfMetric	*p_metric;  /* interface configuration info */
	t_XXCallInfo	*xxci;      /* XX_Call info */
	e_Err			e;
	L7_int32		currThread = osapiTaskIdSelf();
	OS_Thread		*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

	/* Fill in the interface configuration info */
	p_metric = XX_Malloc(sizeof(t_S_IfMetric));
	if (!p_metric)
	{
		return E_NOMEMORY;
	}
    
	/* copy user supplied configuration info */
	memcpy(p_metric, p_Cfg, sizeof(t_S_IfMetric));

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			XX_Free(p_metric);
			return E_FAILED;
		}

		PACKET_INIT(xxci, IFO_MetricConfig_Unpack, 0, 0, 2, (ulng)IFO_Id);
		PACKET_PUT(xxci, (ulng)p_metric);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call IFO_MetricConfig_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			XX_Free(p_metric);
			return E_FAILED;
		}
	}
	else
	{
		e = IFO_MetricConfig(IFO_Id, p_metric);
	}

	memcpy(p_Cfg, p_metric, sizeof(t_S_IfMetric));
	XX_Free(p_metric);

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

	return E_OK;
}

/*********************************************************************
* @purpose  IFO_AuthKeyConfig_Unpack is the callback function to be called
*           by the ospf thread to process the user configuration request
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err IFO_AuthKeyConfig_Unpack(void *p_Info)
{
  t_XXCallInfo *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_Handle     IFO_Id = (t_Handle)PACKET_GET(xxCallInfo, 0);
  t_OspfAuthKey *p_authKeyCfg = (t_OspfAuthKey *)PACKET_GET(xxCallInfo, 1);
  e_Err         e;

  e = IFO_AuthKeyConfig(IFO_Id, p_authKeyCfg);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return e;
}

/*********************************************************************
* @purpose  IFO_AuthKeyConfig_Pack is used to enqueue a request for configuring
*           an ospf interface in the ospf thread.
*
* @param    IFO_Id     IFO object Id
* @param    p_Cfg      Auth Key configuration (see spcfg.h)
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err IFO_AuthKeyConfig_Pack(t_Handle IFO_Id, t_OspfAuthKey *p_Cfg)
{
	t_OspfAuthKey	*p_authKeyCfg;  /* auth key configuration info */
	t_XXCallInfo	*xxci;      /* XX_Call info */
	e_Err			e;
	L7_int32		currThread = osapiTaskIdSelf();
	OS_Thread		*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

	/* Fill in the auth key configuration info */
	p_authKeyCfg = XX_Malloc(sizeof(t_OspfAuthKey));
	if (!p_authKeyCfg)
	{
		return E_NOMEMORY;
	}
    
	/* copy user supplied configuration info */
	memcpy(p_authKeyCfg, p_Cfg, sizeof(t_OspfAuthKey));

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			XX_Free(p_authKeyCfg);
			return E_FAILED;
		}

		PACKET_INIT(xxci, IFO_AuthKeyConfig_Unpack, 0, 0, 2, (ulng)IFO_Id);
		PACKET_PUT(xxci, (ulng)p_authKeyCfg);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call IFO_AuthKeyConfig_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			XX_Free(p_authKeyCfg);
			return E_FAILED;
		}
	}
	else
	{
		e = IFO_AuthKeyConfig(IFO_Id, p_authKeyCfg);
	}

	memcpy(p_Cfg, p_authKeyCfg, sizeof(t_OspfAuthKey));
	XX_Free(p_authKeyCfg);

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

	return E_OK;
}

/*********************************************************************
* @purpose  NSSA_Config_Unpack is the callback function to be called
*           by the ospf thread to process the user configuration request
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err NSSA_Config_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  ulng          areaId     = PACKET_GET(xxCallInfo, 0);
  t_S_NssaCfg   *p_nssaCfg = (t_S_NssaCfg *)PACKET_GET(xxCallInfo, 1);
  ulng			giveSem = PACKET_GET(xxCallInfo, 2);
  e_Err         e;
  t_ARO         *p_ARO;

  e = HL_FindFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
                   (byte*)&areaId, (void**)&p_ARO);

  if ((e != E_OK) || (p_ARO == NULL))
  {
    if (giveSem != 0)
    {
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
    }
    return L7_FAILURE;
  }

  if(p_ARO->ExternalRoutingCapability != AREA_IMPORT_NSSA)
    p_nssaCfg->NSSAStatus = ROW_CREATE_AND_GO;

  e = NSSA_Config((t_Handle)p_ARO, p_nssaCfg);

  if (giveSem != 0)
  {
	  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  }

  return e;
}

/*********************************************************************
* @purpose  NSSA_Config_Pack is used to enqueue a request for configuring
*           an ospf router object in the ospf thread.
*
* @param    areaId     area object Id
* @param    p_Cfg      NSSA configuration (see spcfg.h)
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err NSSA_Config_Pack(L7_uint32 areaId, t_S_NssaCfg *p_Cfg)
{
	t_S_NssaCfg		*p_nssaCfg;  /* ospf area configuration info */
	t_XXCallInfo	*xxci;      /* XX_Call info */
	e_Err			e;
	L7_int32		currThread = osapiTaskIdSelf();
	OS_Thread		*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

	/* Fill in the NSSA configuration info */
	p_nssaCfg = XX_Malloc(sizeof(t_S_NssaCfg));
	if (!p_nssaCfg)
	{
		return E_NOMEMORY;
	}
    
	/* copy user supplied configuration info */
	memcpy(p_nssaCfg, p_Cfg, sizeof(t_S_NssaCfg));
	PACKET_INIT(xxci, NSSA_Config_Unpack, 0, 0, 3, (ulng)areaId);
	PACKET_PUT(xxci, (ulng)p_nssaCfg);

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG("Failed to take OSPF sync semaphore\n");
			XX_Free(p_nssaCfg);
			return E_FAILED;
		}

		/*
		The semaphore is taken, so the other end should give it
		*/

		PACKET_PUT(xxci, (ulng) 1);
		e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

		if (e != E_OK)
		{
			LOG_MSG("Failed to call NSSA_Config_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
			return E_FAILED;
		}

		if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
		{
			LOG_MSG( "Failed to take OSPF sync semaphore\n");
			XX_Free(p_nssaCfg);
			return E_FAILED;
		}
	}
	else
	{
		/*
		Don't give the semaphore when unpacking
		*/

		PACKET_PUT(xxci, (ulng) 0);
		e = NSSA_Config_Unpack(xxci);
		XX_Free(xxci);
	}

	memcpy(p_Cfg, p_nssaCfg, sizeof(t_S_NssaCfg));
	XX_Free(p_nssaCfg);

	if (currThread != (L7_int32) pThreadGlobal->TaskID)
	{
		osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
	}

	return E_OK;
}

/*********************************************************************
* @purpose  Get SPF stats
*
* @param    maxStats     Maximum number of stats to be reported
* @param    spfStats     Output buffer. Large enough for maxStats.
*
* @returns  Number of stats actually reported. Number available could
*           be less than number requested.
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 ospfMapExtenSpfStatsGet(L7_uint32 maxStats, L7_OspfSpfStats_t *spfStats)
{
  L7_uint32     numReported;

  if (spfStats == NULL)
  {
    return 0;
  }

  if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_MSG("Failed to take OSPF sync semaphore\n");
    return 0;
  }

  numReported = RTB_SpfStatsReport(ospfMapCtrl_g.RTO_Id, maxStats, spfStats);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return numReported;
}

/*********************************************************************
* @purpose  Get OSPFv2 interface statistics 
*
* @param    intIfNum  @b{(input)}    interface whose stats are to be retrieved
* @param    intfStats @b{(output)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfMapExtenIntfStatsGet(L7_uint32 intIfNum, L7_OspfIntfStats_t *intfStats)
{
  t_PCK_Stats pktStats;
  t_IFO *p_IFO = NULL;

  if ((p_IFO = ifoPtrGet(intIfNum)) == NULL)
    return L7_FAILURE;

  if (IFO_Stats_Pack(p_IFO, &pktStats) == E_OK)
  {
    intfStats->RxPackets = pktStats.RxPackets;
    intfStats->DiscardPackets = pktStats.DiscardPackets;
    intfStats->BadVersion = pktStats.BadVersion;
    intfStats->BadNetwork = pktStats.BadNetwork;
    intfStats->BadVirtualLink = pktStats.BadVirtualLink;
    intfStats->BadArea = pktStats.BadArea;
    intfStats->BadDstAdr = pktStats.BadDstAdr;
    intfStats->BadAuType = pktStats.BadAuType;
    intfStats->BadAuthentication = pktStats.BadAuthentication;
    intfStats->BadNeighbor = pktStats.BadNeighbor;
    intfStats->BadPckType = pktStats.BadPckType;
    intfStats->TxPackets = pktStats.TxPackets;
    intfStats->RxHellos = pktStats.RxHellos;
    intfStats->RxDbDescr = pktStats.RxDbDescr;
    intfStats->RxLsReq = pktStats.RxLsReq;
    intfStats->RxLsUpdate = pktStats.RxLsUpdate;
    intfStats->RxLsAck = pktStats.RxLsAck;
    intfStats->HellosIgnored = pktStats.HellosIgnored;
    intfStats->TxHellos = pktStats.TxHellos;
    intfStats->TxDbDescr = pktStats.TxDbDescr;
    intfStats->TxLsReq = pktStats.TxLsReq;
    intfStats->TxLsUpdate = pktStats.TxLsUpdate;
    intfStats->TxLsAck = pktStats.TxLsAck;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  NSSA_Config_Unpack is the callback function to be called
*           by the ospf thread to process the user configuration request
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err IFO_Stats_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_IFO *p_IFO  = (t_IFO*) PACKET_GET(xxCallInfo, 0);
  t_PCK_Stats   *pktStats = (t_PCK_Stats *)PACKET_GET(xxCallInfo, 1);

  memcpy(pktStats, &p_IFO->PckStats, sizeof(t_PCK_Stats));
  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  return E_OK;
}

/*********************************************************************
* @purpose  Get OSPFv2 interface statistics 
*
* @param    intIfNum  @b{(input)}    interface whose stats are to be retrieved
* @param    intfStats @b{(output)}
*
* @returns  E_OK
*           E_FAILED
*
* @notes    none 
*       
* @end
*********************************************************************/
e_Err IFO_Stats_Pack(t_IFO *p_IFO, t_PCK_Stats *intfStats)
{
  t_XXCallInfo	*xxci;      /* XX_Call info */
	e_Err			e;
	L7_int32		currThread = osapiTaskIdSelf();
	OS_Thread		*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

  /* Should never be called on OSPF protocol thread */
	if (currThread == (L7_int32) pThreadGlobal->TaskID)
    return E_FAILED;
  
  if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_MSG("Failed to take OSPF sync semaphore\n");
    return E_FAILED;
  }

  PACKET_INIT(xxci, IFO_Stats_Unpack, 0, 0, 2, (L7_uint32)p_IFO);
  PACKET_PUT(xxci, intfStats);
  e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

  if (e != E_OK)
  {
    LOG_MSG("Failed to call IFO_Stats_Unpack() on OSPF thread\n");
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
    return E_FAILED;
  }

  if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_MSG("Failed to take OSPF sync semaphore\n");
    return E_FAILED;
  }

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema); 

  return E_OK;
}

/*********************************************************************
*
* @purpose Determine whether OSPF is currently redistributing a route
*          to a given destination.
*
* @param @b{(input)} destAddr - destination IP address
* @param @b{(input)} destMask - destination network mask
*
* @returns L7_TRUE if OSPF is currently redistributing this prefix.
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL ospfMapPreviouslyRedistributed(L7_uint32 destAddr, L7_uint32 destMask)
{
    ospfRedistRoute_t redistRoute;
    redistRoute.destAddr = destAddr;
    redistRoute.destMask = destMask;
    if (ospfMapRedistListFind(&redistRoute) == L7_SUCCESS) {
        return L7_TRUE;
    }
    else {
        return L7_FALSE;
    }
}

/*********************************************************************
 * @purpose  RTO_OpaqueCapability_Get_UnPack is the callback function to
 *           be called by the ospf thread to process the user 
 *           configuration request
 *
 * @param    p_Info         pointer to the message containing
 *                          the required information
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
static e_Err RTO_OpaqueCapability_Get_UnPack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_Handle      RTO_Id = (t_Handle)PACKET_GET(xxCallInfo, 0);
  L7_uint32     *opaqueEnabled = (L7_uint32 *)PACKET_GET(xxCallInfo, 1);
  e_Err         e;

  e = RTO_OpaqueCapability_Get(RTO_Id, opaqueEnabled);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return e;
}

/*********************************************************************
 * @purpose  RTO_OpaqueCapability_Get_Pack is used to enqueue a request
 *           for getting an ospf router object in the ospf thread.
 *
 * @param    RTO_Id     RTO object Id
 * @param    opaqueEnabled OSPF OPAQUE CAPABILITY SUPPORT
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
e_Err RTO_OpaqueCapability_Get_Pack(t_Handle RTO_Id, L7_uint32 *opaqueEnabled)
{
  t_XXCallInfo    *xxci;      /* XX_Call info */
  e_Err           e;
  L7_int32        currThread = osapiTaskIdSelf();
  OS_Thread       *pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }

    PACKET_INIT(xxci, RTO_OpaqueCapability_Get_UnPack, 0, 0, 2, (ulng)RTO_Id);
    PACKET_PUT(xxci, (ulng)opaqueEnabled);
    e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

    if (e != E_OK)
    {
      LOG_MSG("Failed to call RTO_OpaqueCapability_Get_UnPack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
      return E_FAILED;
    }

    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }
  }
  else
  {
    e = RTO_OpaqueCapability_Get(RTO_Id, opaqueEnabled);
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  }

  return e;
}

/*********************************************************************
 * @purpose  Get opaque capability support.
 *
 * @param    opaqueEnabled   @b{(input)}
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenOspfOpaqueCapabilityGet(L7_uint32 *opaqueEnabled)
{
  e_Err erc;
  static const char *routine_name = "ospfMapExtenOspfOpaqueCapabilityGet()";

  if ((erc = RTO_OpaqueCapability_Get_Pack(ospfMapCtrl_g.RTO_Id, opaqueEnabled)) != E_OK)
  {
    OSPFMAP_ERROR("%s %d: %s: ERROR: OSPF_MAP: Unable to get opaque support for OSPF, rc=%d\n",
        __FILE__, __LINE__, routine_name, erc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  RTO_OpaqueCapability_Set_UnPack is the callback function to
 *           be called by the ospf thread to process the user 
 *           configuration request
 *
 * @param    p_Info         pointer to the message containing
 *                          the required information
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
static e_Err RTO_OpaqueCapability_Set_UnPack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_Handle      RTO_Id = (t_Handle)PACKET_GET(xxCallInfo, 0);
  L7_uint32     opaqueEnabled = (L7_uint32 )PACKET_GET(xxCallInfo, 1);
  e_Err         e;

  e = RTO_OpaqueCapability_Set(RTO_Id, opaqueEnabled);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return e;
}

/*********************************************************************
 * @purpose  RTO_OpaqueCapability_Set_Pack is used to enqueue a request
 *           for getting an ospf router object in the ospf thread.
 *
 * @param    RTO_Id     RTO object Id
 * @param    opaqueEnabled OSPF OPAQUE CAPABILITY SUPPORT
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
e_Err RTO_OpaqueCapability_Set_Pack(t_Handle RTO_Id, L7_uint32 opaqueEnabled)
{
  t_XXCallInfo    *xxci;      /* XX_Call info */
  e_Err           e;
  L7_int32        currThread = osapiTaskIdSelf();
  OS_Thread       *pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }

    PACKET_INIT(xxci, RTO_OpaqueCapability_Set_UnPack, 0, 0, 2, (ulng)RTO_Id);
    PACKET_PUT(xxci, (ulng)opaqueEnabled);
    e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

    if (e != E_OK)
    {
      LOG_MSG("Failed to call RTO_OpaqueCapability_Set_UnPack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
      return E_FAILED;
    }

    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }
  }
  else
  {
    e = RTO_OpaqueCapability_Set(RTO_Id, opaqueEnabled);
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  }

  return E_OK;
}

/*********************************************************************
 * @purpose  Set opaque capability support.
 *
 * @param    opaqueEnabled   @b{(input)}
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenOspfOpaqueCapabilitySet(L7_uint32 opaqueEnabled)
{
  e_Err erc;
  static const char *routine_name = "ospfMapExtenRTOOpaqueCapabilityGet()";

  if ((erc = RTO_OpaqueCapability_Set_Pack(ospfMapCtrl_g.RTO_Id, opaqueEnabled)) != E_OK)
  {
    OSPFMAP_ERROR("%s %d: %s: ERROR: OSPF_MAP: Unable to get opaque support for OSPF, rc=%d\n",
        __FILE__, __LINE__, routine_name, erc);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Get the AreaOpaque Link State Database entry.
 *
 * @param    AreaId      Area Id
 * @param    Type        Lsdb Type
 * @param    Lsid        Link State id
 * @param    RouterId    Database entry
 * @param    p_Lsa       Lsdb Entry given to the external world
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none.
 *
 * @end
 *********************************************************************/
L7_RC_t ospfAreaOpaqueLsdbTable_Lookup(L7_uint32 lsdbAreaId, L7_uint32 lsdbType,
    L7_uint32 lsdbId, L7_uint32 lsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  t_ARO         *p_ARO = NULL;
  e_Err         e;
  L7_ospfOpaqueLsdbEntry_t temp_Lsa;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  /* First find the ARO object corresponding to the LsdbAreaId */
  e = HL_FindFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
      (byte*)&lsdbAreaId, (void**)&p_ARO);
  if ((e != E_OK) || (p_ARO == NULL))
  {
    return L7_FAILURE;
  }

  p_Lsa->LsdbLsid = lsdbId;
  p_Lsa->LsdbType = (L7_uchar8)lsdbType;
  p_Lsa->LsdbRouterId = lsdbRouterId;

  /* Find the Lsa corresponding to LsdbType, LsdbId, LsdbRouterId */
  if((e = ARO_OpaqueLsdbEntry(p_ARO, p_Lsa)) != E_OK)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  ospfAreaOpaqueLsdbTable_Lookup_Unpack is the callback 
 *           function to be called by the ospf thread to process the
 *           user configuration request
 *
 * @param    p_Info         pointer to the message containing
 *                          the required parameters
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
static e_Err ospfAreaOpaqueLsdbTable_Lookup_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  L7_uint32 areaId = (L7_uint32)PACKET_GET(xxCallInfo, 0);
  L7_uint32 Type = (L7_uint32)PACKET_GET(xxCallInfo, 1);
  L7_uint32 Lsid = (L7_uint32)PACKET_GET(xxCallInfo, 2);
  L7_uint32 RouterId = (L7_uint32)PACKET_GET(xxCallInfo, 3);
  L7_ospfOpaqueLsdbEntry_t *p_Lsa = 
    (L7_ospfOpaqueLsdbEntry_t *)PACKET_GET(xxCallInfo, 4);
  L7_RC_t *rc = (L7_RC_t *)PACKET_GET(xxCallInfo, 5);

  *rc = ospfAreaOpaqueLsdbTable_Lookup(areaId, Type, Lsid, RouterId, p_Lsa);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return ((*rc == L7_SUCCESS)? E_OK: E_FAILED);
}

/*********************************************************************
 * @purpose  ospfAreaOpaqueLsdbTable_Lookup_Pack is used to enqueue a 
 *           request for configuring an ospf router object in the ospf thread.
 *
 * @param    LsdbAreaId     Area Id
 * @param    LsdbType       Lsid Type
 * @param    LsdbId         Link State Id
 * @param    LsdbRouterId   Router Id
 * @param    p_Lsa          Lsdb Entry given to external world
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
e_Err ospfAreaOpaqueLsdbTable_Lookup_Pack(L7_uint32 lsdbAreaId,
    L7_uint32 lsdbType, L7_uint32 lsdbId,
    L7_uint32 lsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  t_XXCallInfo    *xxci;      /* XX_Call info */
  e_Err                   e;
  L7_RC_t                 rc;
  L7_int32                currThread = osapiTaskIdSelf();
  OS_Thread               *pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;
  L7_ospfOpaqueLsdbEntry_t  temp_Lsa;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }

    PACKET_INIT(xxci, ospfAreaOpaqueLsdbTable_Lookup_Unpack, 0, 0, 6, (L7_uint32)lsdbAreaId);
    PACKET_PUT(xxci, (L7_uint32)lsdbType);
    PACKET_PUT(xxci, (L7_uint32)lsdbId);
    PACKET_PUT(xxci, (L7_uint32)lsdbRouterId);
    PACKET_PUT(xxci, (L7_ospfOpaqueLsdbEntry_t *)p_Lsa);
    PACKET_PUT(xxci, (L7_RC_t *)&rc);
    e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

    if (e != E_OK)
    {
      LOG_MSG("Failed to call ospfaAreaOpaqueLsdbTable_Lookup_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
      return E_FAILED;
    }

    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }
  }
  else
  {
    rc = ospfAreaOpaqueLsdbTable_Lookup(lsdbAreaId, lsdbType, lsdbId, lsdbRouterId, p_Lsa);
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  }

  return (rc == L7_SUCCESS)? E_OK: E_FAILED;
}

/*********************************************************************
 * @purpose  Get the process's AreaOpaque Link State Database entry.
 *
 * @param    AreaId      area
 * @param    Type        type
 * @param    Lsid        Link State id
 * @param    RouterId    Advertising Router
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none.
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenAreaOpaqueLsdbEntryGet(L7_uint32 areaId, L7_uint32 type,
    L7_uint32 lsid, L7_uint32 routerId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  e_Err errRet;

  if ((errRet = ospfAreaOpaqueLsdbTable_Lookup_Pack(areaId,type,lsid,routerId,
          p_Lsa)) == E_OK)
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get the AreaOpaque Link State Database entry.
 *
 * @param    AreaId      Area Id
 * @param    Type        Lsdb Type
 * @param    Lsid        Link State id
 * @param    RouterId    Database entry
 * @param    p_Lsa       Lsdb Entry given to the external world
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none.
 *
 * @end
 *********************************************************************/
L7_RC_t ospfAreaOpaqueLsdbTable_NextLookup(L7_uint32 *LsdbAreaId, 
    L7_int32 *LsdbType,
    L7_uint32 *LsdbId, 
    L7_uint32 *LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  t_ARO         *p_ARO = NULL;
  e_Err         e;
  L7_ospfOpaqueLsdbEntry_t temp_Lsa;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  /* First find the ARO object corresponding to the LsdbAreaId */
  e = HL_FindFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
      (byte*)LsdbAreaId, (void**)&p_ARO);
  if ((e != E_OK) || (p_ARO == NULL))
  {
    return L7_FAILURE;
  }

  p_Lsa->LsdbLsid = *LsdbId;
  p_Lsa->LsdbType = (L7_uchar8)*LsdbType;
  p_Lsa->LsdbRouterId = *LsdbRouterId;
  if(*LsdbType == L7_NULL)
    p_Lsa->LsdbType = S_AREA_OPAQUE_LSA;

  /* Find the Lsa corresponding to LsdbType, LsdbId, LsdbRouterId */
  if((e = ARO_OpaqueLsdbNextEntry(p_ARO, p_Lsa)) != E_OK)
  {
    return L7_FAILURE;
  }

  *LsdbAreaId = p_Lsa->AreaId;
  *LsdbType = p_Lsa->LsdbType;
  *LsdbId = p_Lsa->LsdbLsid;
  *LsdbRouterId = p_Lsa->LsdbRouterId;

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  ospfAreaOpaqueLsdbTable_Lookup_Unpack is the callback 
 *           function to be called by the ospf thread to process the
 *           user configuration request
 *
 * @param    p_Info         pointer to the message containing
 *                          the required parameters
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
static e_Err ospfAreaOpaqueLsdbTable_NextLookup_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  L7_uint32 *areaId = (L7_uint32*)PACKET_GET(xxCallInfo, 0);
  L7_int32 *Type = (L7_int32*)PACKET_GET(xxCallInfo, 1);
  L7_uint32 *Lsid = (L7_uint32*)PACKET_GET(xxCallInfo, 2);
  L7_uint32 *RouterId = (L7_uint32*)PACKET_GET(xxCallInfo, 3);
  L7_ospfOpaqueLsdbEntry_t *p_Lsa = 
    (L7_ospfOpaqueLsdbEntry_t *)PACKET_GET(xxCallInfo, 4);
  L7_RC_t *rc = (L7_RC_t *)PACKET_GET(xxCallInfo, 5);

  *rc = ospfAreaOpaqueLsdbTable_NextLookup(areaId, Type, Lsid, RouterId, p_Lsa);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return ((*rc == L7_SUCCESS)? E_OK: E_FAILED);
}

/*********************************************************************
 * @purpose  ospfAreaOpaqueLsdbTable_NextLookup_Pack is used to enqueue a 
 *           request for configuring an ospf router object in the ospf thread.
 *
 * @param    LsdbAreaId     Area Id
 * @param    LsdbType       Lsid Type
 * @param    LsdbId         Link State Id
 * @param    LsdbRouterId   Router Id
 * @param    p_Lsa          Lsdb Entry given to external world
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
e_Err ospfAreaOpaqueLsdbTable_NextLookup_Pack(L7_uint32 *LsdbAreaId,
    L7_int32 *LsdbType, L7_uint32 *LsdbId,
    L7_uint32 *LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  t_XXCallInfo    *xxci;      /* XX_Call info */
  e_Err                   e;
  L7_RC_t                 rc;
  L7_int32                currThread = osapiTaskIdSelf();
  OS_Thread               *pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;
  L7_ospfOpaqueLsdbEntry_t  temp_Lsa;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }

    PACKET_INIT(xxci, ospfAreaOpaqueLsdbTable_NextLookup_Unpack, 0, 0, 6, (L7_uint32)LsdbAreaId);
    PACKET_PUT(xxci, (L7_int32*)LsdbType);
    PACKET_PUT(xxci, (L7_uint32*)LsdbId);
    PACKET_PUT(xxci, (L7_uint32*)LsdbRouterId);
    PACKET_PUT(xxci, (L7_ospfOpaqueLsdbEntry_t *)p_Lsa);
    PACKET_PUT(xxci, (L7_RC_t *)&rc);
    e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

    if (e != E_OK)
    {
      LOG_MSG("Failed to call ospfAreaOpaqueLsdbTable_NextLookup_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
      return E_FAILED;
    }

    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }
  }
  else
  {
    rc = ospfAreaOpaqueLsdbTable_NextLookup(LsdbAreaId, LsdbType, LsdbId, LsdbRouterId, p_Lsa);
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  }

  return (rc == L7_SUCCESS)? E_OK: E_FAILED;
}

/*********************************************************************
 * @purpose  Get the process's AreaOpaque Next Link State Database entry.
 *
 * @param    AreaId      area
 * @param    Type        type
 * @param    Lsid        Link State id
 * @param    RouterId    Advertising Router
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none.
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenAreaOpaqueLsdbEntryNext(L7_uint32 *areaId, L7_int32 *type,
    L7_uint32 *lsid, L7_uint32 *routerId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  e_Err errRet;

  if ((errRet = ospfAreaOpaqueLsdbTable_NextLookup_Pack(areaId,type,lsid,routerId,
          p_Lsa)) == E_OK)
  {
    switch (*type)
    {
      case S_ILLEGAL_LSA:
      case S_AREA_OPAQUE_LSA:
        *type = L7_S_AREA_OPAQUE_LSA;
        break;
      default:
        return L7_FAILURE;
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get the sequence number.
 *
 * @param    AreaId      Area
 * @param    Type        Type
 * @param    Lsid        Link State id
 @param    RouterId    Database entry
 * @param    *val        sequence number
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    sequence number field is a signed 32-bit integer.
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenType10LsaSequenceGet(L7_uint32 areaId, L7_int32 type,
    L7_uint32 lsid, L7_uint32 routerId,
    L7_uint32 *val)
{
  e_Err errRet;
  L7_uint32 tmpType;
  L7_ospfOpaqueLsdbEntry_t lsa;

  switch (type)
  {
    case L7_S_AREA_OPAQUE_LSA:
      tmpType = S_AREA_OPAQUE_LSA;
      break;

    default:
      return L7_FAILURE;
  }

  if ((errRet = ospfAreaOpaqueLsdbTable_Lookup_Pack(areaId, tmpType, lsid,
          routerId, &lsa)) == E_OK)
  {
    *val = lsa.LsdbSequence;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get the Ospf Opaquee LSA Age for the specified Area
 *
 * @param    AreaId      Area
 * @param    Type        Type
 * @param    Lsid        Link State id
 * @param    RouterId    Database entry
 * @param    *val        sequence number
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenType10LsaAgeGet(L7_uint32 areaId, L7_int32 type,
    L7_uint32 lsid, L7_uint32 routerId,
    L7_uint32 *val)
{
  e_Err errRet;
  L7_uint32 tmpType;
  L7_ospfOpaqueLsdbEntry_t lsa;

  switch (type)
  {
    case L7_S_AREA_OPAQUE_LSA:
      tmpType = S_AREA_OPAQUE_LSA;
      break;

    default:
      return L7_FAILURE;
  }

  if ((errRet = ospfAreaOpaqueLsdbTable_Lookup_Pack(areaId, tmpType, lsid,
          routerId, &lsa)) == E_OK)
  {
    *val = lsa.LsdbAge;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get the checksum of the type-10 LSA.
 *
 * @param    AreaId      Area
 * @param    Type        Type
 * @param    Lsid        Link State id
 * @param    RouterId    Database entry
 * @param    *val        sequence number
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenType10LsaChecksumGet(L7_uint32 areaId, L7_int32 type,
    L7_uint32 lsid, L7_uint32 routerId,
    L7_uint32 *val)
{
  e_Err errRet;
  L7_uint32 tmpType;
  L7_ospfOpaqueLsdbEntry_t lsa;

  switch (type)
  {
    case L7_S_AREA_OPAQUE_LSA:
      tmpType = S_AREA_OPAQUE_LSA;
      break;

    default:
      return L7_FAILURE;
  }

  if ((errRet = ospfAreaOpaqueLsdbTable_Lookup_Pack(areaId, tmpType, lsid,
          routerId, &lsa)) == E_OK)
  {
    *val = lsa.LsdbChecksum;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get the Link State Database entry.
 *
 * @param    AreaId      Area Id
 * @param    Type        Lsdb Type
 * @param    Lsid        Link State id
 * @param    RouterId    Database entry
 * @param    p_Lsa       Lsdb Entry given to the external world
 * @param    p_LsdbAdvertisement  Lsdb Advertisement
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none.
 *
 * @end
 *********************************************************************/
L7_RC_t ospfType10Advertisement_Lookup(L7_uint32 LsdbAreaId, L7_uint32 LsdbType,
    L7_uint32 LsdbId, L7_uint32 LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa,
    L7_char8 **p_LsdbAdvertisement)
{
  t_ARO         *p_ARO = NULL;
  e_Err         e;
  L7_ospfOpaqueLsdbEntry_t temp_Lsa;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  /* First find the ARO object corresponding to the LsdbAreaId */
  e = HL_FindFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl,
      (byte*)&LsdbAreaId, (void**)&p_ARO);
  if ((e != E_OK) || (p_ARO == NULL))
  {
    return L7_FAILURE;
  }

  p_Lsa->LsdbLsid = LsdbId;
  p_Lsa->LsdbType = (L7_uchar8)LsdbType;
  p_Lsa->LsdbRouterId = LsdbRouterId;

  /* Find the Lsa corresponding to LsdbType, LsdbId, LsdbRouterId */
  if((e = ARO_OpaqueAdvertisement(p_ARO, p_Lsa, p_LsdbAdvertisement)) != E_OK)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  ospfLsdbAdvertisement_Lookup_Unpack is the callback function
 *           to be called by the ospf thread to process the user
 *           configuration request
 *
 * @param    p_Info         pointer to the message containing
 *                          the required parameters
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
static e_Err ospfAreaOpaqueAdvertisement_Lookup_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  ulng areaId = (ulng)PACKET_GET(xxCallInfo, 0);
  ulng Type = (ulng)PACKET_GET(xxCallInfo, 1);
  ulng Lsid = (ulng)PACKET_GET(xxCallInfo, 2);
  ulng RouterId = (ulng)PACKET_GET(xxCallInfo, 3);
  L7_ospfOpaqueLsdbEntry_t *p_Lsa = (L7_ospfOpaqueLsdbEntry_t *)PACKET_GET(xxCallInfo, 4);
  L7_char8 **p_LsdbAdvertisement = (L7_char8 **)PACKET_GET(xxCallInfo, 5);
  L7_RC_t *rc = (L7_RC_t *)PACKET_GET(xxCallInfo, 6);

  *rc = ospfType10Advertisement_Lookup(areaId, Type, Lsid, RouterId,
      p_Lsa,
      p_LsdbAdvertisement);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return ((*rc == L7_SUCCESS)? E_OK: E_FAILED);
}

/*********************************************************************
 * @purpose  ospfAreaOpaqueAdvertisement_Lookup_Pack is used to enqueue a
 *           request for configuring an ospf router object in the ospf thread.
 *
 * @param    LsdbAreaId     Area Id
 * @param    LsdbType       Lsid Type
 * @param    LsdbId         Link State Id
 * @param    LsdbRouterId   Router Id
 * @param    p_Lsa          Lsdb Entry given to external world
 * @param    p_LsdbAdvertisement pointer to address where LSDB Advertisement is
 *                               copied by the OSPF code
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
e_Err ospfAreaOpaqueAdvertisement_Lookup_Pack(L7_uint32 LsdbAreaId,
    L7_uint32 LsdbType, L7_uint32 LsdbId,
    L7_uint32 LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa,
    L7_char8 **p_LsdbAdvertisement)
{
  t_XXCallInfo    *xxci;      /* XX_Call info */
  e_Err                   e;
  L7_RC_t                 rc;
  L7_int32                currThread = osapiTaskIdSelf();
  OS_Thread               *pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;
  L7_ospfOpaqueLsdbEntry_t      temp_Lsa;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }

    PACKET_INIT(xxci, ospfAreaOpaqueAdvertisement_Lookup_Unpack, 0, 0, 7,
        (L7_uint32)LsdbAreaId);
    PACKET_PUT(xxci, (L7_uint32)LsdbType);
    PACKET_PUT(xxci, (L7_uint32)LsdbId);
    PACKET_PUT(xxci, (L7_uint32)LsdbRouterId);
    PACKET_PUT(xxci, (L7_ospfOpaqueLsdbEntry_t *)p_Lsa);
    PACKET_PUT(xxci, (L7_char8 **)p_LsdbAdvertisement);
    PACKET_PUT(xxci, (L7_RC_t *)&rc);
    e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

    if (e != E_OK)
    {
      LOG_MSG("Failed to call ospfAreaOpaqueAdvertisement_Lookup_Pack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
      return E_FAILED;
    }

    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }
  }
  else
  {
    rc = ospfType10Advertisement_Lookup(LsdbAreaId, LsdbType,
        LsdbId, LsdbRouterId,
        p_Lsa,
        p_LsdbAdvertisement);
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  }

  return (rc == L7_SUCCESS)? E_OK: E_FAILED;
}

/*********************************************************************
 * @purpose  Get the type-10 advertisement of the LSA.
 *
 * @param    AreaId      Area
 * @param    Type        Type
 * @param    Lsid        Link State id
 * @param    RouterId    Database entry
 * @param    *val        sequence number
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenType10LsaAdvertisementGet(L7_uint32 areaId, L7_int32 type,
    L7_uint32 lsid, L7_uint32 routerId,
    L7_char8 **buf, L7_uint32 *len)
{
  e_Err errRet;
  L7_uint32 tmpType;
  L7_ospfOpaqueLsdbEntry_t lsa;

  switch (type)
  {
    case L7_S_AREA_OPAQUE_LSA:
      tmpType = S_AREA_OPAQUE_LSA;
      break;

    default:
      return L7_FAILURE;
  }

  if ((errRet = ospfAreaOpaqueAdvertisement_Lookup_Pack(areaId, tmpType, lsid,
          routerId, &lsa, buf)) == E_OK)
  {
    *len = lsa.LsdbAdvertLength;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get the LinkOpaque Link State Database entry.
 *
 * @param    ipAddr      IpAddress
 * @param    ifIndex     Interface Index
 * @param    Type        Lsdb Type
 * @param    Lsid        Link State id
 * @param    RouterId    Database entry
 * @param    p_Lsa       Lsdb Entry given to the external world
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none.
 *
 * @end
 *********************************************************************/
L7_RC_t ospfLinkOpaqueLsdbTable_Lookup(L7_uint32 ipAddr, L7_uint32 ifIndex,
    L7_uint32 LsdbType, L7_uint32 LsdbId, 
    L7_uint32 LsdbRouterId, 
    L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  t_ARO         *p_ARO = NULL;
  t_IFO         *p_IFO = NULL;
  L7_BOOL       found = L7_FALSE;
  L7_ospfOpaqueLsdbEntry_t temp_Lsa;
  e_Err         e = E_FAILED;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  /* First find the ARO object corresponding to the ipAddr */
  e = HL_GetFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl, (void *)&p_ARO);
  while(e == E_OK)
  {
    e = HL_GetFirst(p_ARO->IfoHl, (void *)&p_IFO);
    while(e == E_OK)
    {
      if(ipAddr == p_IFO->Cfg.IpAdr)
      {
        found = L7_TRUE;
        break;
      }
      e = HL_GetNext(p_ARO->IfoHl, (void *)&p_IFO, p_IFO);
    }
    if(found == L7_TRUE)
      break;
    e = HL_GetNext(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl, (void *)&p_ARO, p_ARO);
  }

  if (found != L7_TRUE)
  {
    return L7_FAILURE;
  }

  p_Lsa->IpAdr = ipAddr;
  p_Lsa->IfIndex = ifIndex;
  p_Lsa->LsdbLsid = LsdbId;
  p_Lsa->LsdbType = (L7_uchar8)LsdbType;
  p_Lsa->LsdbRouterId = LsdbRouterId;

  /* Find the Lsa corresponding to LsdbType, LsdbId, LsdbRouterId */
  if((e = ARO_OpaqueLsdbEntry(p_ARO, p_Lsa)) != E_OK)
  {
    return L7_FAILURE;
  }
  p_Lsa->IpAdr = p_IFO->Cfg.IpAdr;
  p_Lsa->IfIndex = p_IFO->Cfg.IfIndex;

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  ospfLinkOpaqueLsdbTable_Lookup_Unpack is the callback 
 *           function to be called by the ospf thread to process the
 *           user configuration request
 *
 * @param    p_Info         pointer to the message containing
 *                          the required parameters
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
static e_Err ospfLinkOpaqueLsdbTable_Lookup_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  L7_uint32 ipAddr = (L7_uint32)PACKET_GET(xxCallInfo, 0);
  L7_uint32 ifIndex = (L7_uint32)PACKET_GET(xxCallInfo, 1);
  L7_uint32 Type = (L7_uint32)PACKET_GET(xxCallInfo, 2);
  L7_uint32 Lsid = (L7_uint32)PACKET_GET(xxCallInfo, 3);
  L7_uint32 RouterId = (L7_uint32)PACKET_GET(xxCallInfo, 4);
  L7_ospfOpaqueLsdbEntry_t *p_Lsa = 
    (L7_ospfOpaqueLsdbEntry_t *)PACKET_GET(xxCallInfo, 5);
  L7_RC_t *rc = (L7_RC_t *)PACKET_GET(xxCallInfo, 6);

  *rc = ospfLinkOpaqueLsdbTable_Lookup(ipAddr,ifIndex,Type,Lsid,RouterId,p_Lsa);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return ((*rc == L7_SUCCESS)? E_OK: E_FAILED);
}

/*********************************************************************
 * @purpose  ospfLinkOpaqueLsdbTable_Lookup_Pack is used to enqueue a 
 *           request for configuring an ospf router object in the ospf thread.
 *
 * @param    ipAddr      IpAddress
 * @param    ifIndex     Interface Index
 * @param    LsdbType       Lsid Type
 * @param    LsdbId         Link State Id
 * @param    LsdbRouterId   Router Id
 * @param    p_Lsa          Lsdb Entry given to external world
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
e_Err ospfLinkOpaqueLsdbTable_Lookup_Pack(L7_uint32 ipAddr, L7_uint32 ifIndex,
    L7_uint32 LsdbType, L7_uint32 LsdbId,
    L7_uint32 LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  t_XXCallInfo    *xxci;      /* XX_Call info */
  e_Err                   e;
  L7_RC_t                 rc;
  L7_int32                currThread = osapiTaskIdSelf();
  OS_Thread               *pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;
  L7_ospfOpaqueLsdbEntry_t  temp_Lsa;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }

    PACKET_INIT(xxci,ospfLinkOpaqueLsdbTable_Lookup_Unpack,0,0,7,(L7_uint32)ipAddr);
    PACKET_PUT(xxci, (L7_uint32)ifIndex);
    PACKET_PUT(xxci, (L7_uint32)LsdbType);
    PACKET_PUT(xxci, (L7_uint32)LsdbId);
    PACKET_PUT(xxci, (L7_uint32)LsdbRouterId);
    PACKET_PUT(xxci, (L7_ospfOpaqueLsdbEntry_t *)p_Lsa);
    PACKET_PUT(xxci, (L7_RC_t *)&rc);
    e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

    if (e != E_OK)
    {
      LOG_MSG("Failed to call ospfLinkOpaqueLsdbTable_Lookup_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
      return E_FAILED;
    }

    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }
  }
  else
  {
    rc = ospfLinkOpaqueLsdbTable_Lookup(ipAddr,ifIndex,LsdbType,
        LsdbId,LsdbRouterId,p_Lsa);
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  }

  return (rc == L7_SUCCESS)? E_OK: E_FAILED;
}

/*********************************************************************
 * @purpose  Get the process's LinkOpaque Link State Database entry.
 *
 * @param    ipAddr      IpAddress
 * @param    ifIndex     Interface Index
 * @param    Type        type
 * @param    Lsid        Link State id
 * @param    RouterId    Advertising Router
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none.
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenLinkOpaqueLsdbEntryGet(L7_uint32 ipAddr, L7_uint32 ifIndex,
    L7_uint32 LsdbType, L7_uint32 LsdbId,
    L7_uint32 LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  e_Err errRet;

  if ((errRet = ospfLinkOpaqueLsdbTable_Lookup_Pack(ipAddr,ifIndex,LsdbType,
          LsdbId,LsdbRouterId,p_Lsa)) == E_OK)
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get the LinkOpaque Link State Database entry.
 *
 * @param    ipAddr      IpAddress
 * @param    ifIndex     Interface Index
 * @param    Type        Lsdb Type
 * @param    Lsid        Link State id
 * @param    RouterId    Database entry
 * @param    p_Lsa       Lsdb Entry given to the external world
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none.
 *
 * @end
 *********************************************************************/
L7_RC_t ospfLinkOpaqueLsdbTable_NextLookup(L7_uint32 *ipAddr, 
    L7_uint32 *ifIndex,
    L7_int32  *LsdbType,
    L7_uint32 *LsdbId, 
    L7_uint32 *LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  t_ARO         *p_ARO = NULL;
  t_IFO         *p_IFO = NULL;
  L7_BOOL       found = L7_FALSE;
  e_Err         e = E_FAILED;
  L7_ospfOpaqueLsdbEntry_t temp_Lsa;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  /* First find the ARO object corresponding to the ipAddr */
  e = HL_GetFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl, (void *)&p_ARO);
  while(e == E_OK)
  {
    e = HL_GetFirst(p_ARO->IfoHl, (void *)&p_IFO);
    while(e == E_OK)
    {
      if(*ipAddr == p_IFO->Cfg.IpAdr)
      {
        found = L7_TRUE;
        break;
      }
      e = HL_GetNext(p_ARO->IfoHl, (void *)&p_IFO, p_IFO);
    }
    if(found == L7_TRUE)
      break;
    e = HL_GetNext(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl, (void *)&p_ARO, p_ARO);
  }

  if ((found != L7_TRUE)&&(*ipAddr != L7_NULL)&&(*LsdbType != L7_NULL))
  {
    return L7_FAILURE;
  }

  p_Lsa->IpAdr = *ipAddr;
  p_Lsa->IfIndex = *ifIndex;
  p_Lsa->LsdbLsid = *LsdbId;
  p_Lsa->LsdbType = (L7_uchar8)*LsdbType;
  p_Lsa->LsdbRouterId = *LsdbRouterId;
  if(*LsdbType == L7_NULL)
    p_Lsa->LsdbType = S_LINK_OPAQUE_LSA;

  /* Find the Lsa corresponding to LsdbType, LsdbId, LsdbRouterId */
  if((e = ARO_OpaqueLsdbNextEntry(p_ARO, p_Lsa)) != E_OK)
  {
    return L7_FAILURE;
  }
  p_Lsa->IpAdr = p_IFO->Cfg.IpAdr;
  p_Lsa->IfIndex = p_IFO->Cfg.IfIndex;

  *ipAddr = p_IFO->Cfg.IpAdr;
  *ifIndex = p_IFO->Cfg.IfIndex;
  *LsdbType = p_Lsa->LsdbType;
  *LsdbId = p_Lsa->LsdbLsid;
  *LsdbRouterId = p_Lsa->LsdbRouterId;

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  ospfLinkOpaqueLsdbTable_Lookup_Unpack is the callback 
 *           function to be called by the ospf thread to process the
 *           user configuration request
 *
 * @param    p_Info         pointer to the message containing
 *                          the required parameters
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
static e_Err ospfLinkOpaqueLsdbTable_NextLookup_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  L7_uint32 *ipAddr = (L7_uint32*)PACKET_GET(xxCallInfo, 0);
  L7_uint32 *ifIndex = (L7_uint32*)PACKET_GET(xxCallInfo, 1);
  L7_int32  *Type = (L7_int32*)PACKET_GET(xxCallInfo, 2);
  L7_uint32 *Lsid = (L7_uint32*)PACKET_GET(xxCallInfo, 3);
  L7_uint32 *RouterId = (L7_uint32*)PACKET_GET(xxCallInfo, 4);
  L7_ospfOpaqueLsdbEntry_t *p_Lsa = 
    (L7_ospfOpaqueLsdbEntry_t *)PACKET_GET(xxCallInfo, 5);
  L7_RC_t *rc = (L7_RC_t *)PACKET_GET(xxCallInfo, 6);

  *rc = ospfLinkOpaqueLsdbTable_NextLookup(ipAddr,ifIndex,Type,Lsid,RouterId,p_Lsa);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return ((*rc == L7_SUCCESS)? E_OK: E_FAILED);
}

/*********************************************************************
 * @purpose  ospfLinkOpaqueLsdbTable_NextLookup_Pack is used to enqueue a 
 *           request for configuring an ospf router object in the ospf thread.
 *
 * @param    ipAddr      IpAddress
 * @param    ifIndex     Interface Index
 * @param    LsdbType       Lsid Type
 * @param    LsdbId         Link State Id
 * @param    LsdbRouterId   Router Id
 * @param    p_Lsa          Lsdb Entry given to external world
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
e_Err ospfLinkOpaqueLsdbTable_NextLookup_Pack(L7_uint32 *ipAddr,
    L7_uint32 *ifIndex,
    L7_int32  *LsdbType,
    L7_uint32 *LsdbId,
    L7_uint32 *LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  t_XXCallInfo    *xxci;      /* XX_Call info */
  e_Err                   e;
  L7_RC_t                 rc;
  L7_int32                currThread = osapiTaskIdSelf();
  OS_Thread               *pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;
  L7_ospfOpaqueLsdbEntry_t  temp_Lsa;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }

    PACKET_INIT(xxci, ospfLinkOpaqueLsdbTable_NextLookup_Unpack, 0, 0, 7, (L7_uint32)ipAddr);
    PACKET_PUT(xxci, (L7_int32*)ifIndex);
    PACKET_PUT(xxci, (L7_int32*)LsdbType);
    PACKET_PUT(xxci, (L7_uint32*)LsdbId);
    PACKET_PUT(xxci, (L7_uint32*)LsdbRouterId);
    PACKET_PUT(xxci, (L7_ospfOpaqueLsdbEntry_t *)p_Lsa);
    PACKET_PUT(xxci, (L7_RC_t *)&rc);
    e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

    if (e != E_OK)
    {
      LOG_MSG("Failed to call ospfLinkOpaqueLsdbTable_NextLookup_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
      return E_FAILED;
    }

    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }
  }
  else
  {
    rc = ospfLinkOpaqueLsdbTable_NextLookup(ipAddr,ifIndex,LsdbType,
        LsdbId,LsdbRouterId,p_Lsa);
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  }

  return (rc == L7_SUCCESS)? E_OK: E_FAILED;
}

/*********************************************************************
 * @purpose  Get the process's LinkOpaque Next Link State Database entry.
 *
 * @param    ipAddr      IpAddress
 * @param    ifIndex     Interface Index
 * @param    Type        type
 * @param    Lsid        Link State id
 * @param    RouterId    Advertising Router
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none.
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenLinkOpaqueLsdbEntryNext(L7_uint32 *ipAddr, 
    L7_uint32 *ifIndex,
    L7_int32  *Type,
    L7_uint32 *Lsid, 
    L7_uint32 *RouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  e_Err errRet;

  if ((errRet = ospfLinkOpaqueLsdbTable_NextLookup_Pack(ipAddr,ifIndex,Type,
          Lsid,RouterId,p_Lsa)) == E_OK)
  {
    switch (*Type)
    {
      case S_ILLEGAL_LSA:
      case S_LINK_OPAQUE_LSA:
        *Type = L7_S_LINK_OPAQUE_LSA;
        break;
      default:
        return L7_FAILURE;
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get the sequence number.
 *
 * @param    ipAddr      IpAddress
 * @param    ifIndex     Interface Index
 * @param    Type        Type
 * @param    Lsid        Link State id
 * @param    RouterId    Database entry
 * @param    *val        sequence number
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    sequence number field is a signed 32-bit integer.
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenType9LsaSequenceGet(L7_uint32 ipAddr,
    L7_uint32 ifIndex, 
    L7_int32  Type,
    L7_uint32 Lsid, 
    L7_uint32 RouterId,
    L7_uint32 *val)
{
  e_Err errRet;
  L7_uint32 tmpType;
  L7_ospfOpaqueLsdbEntry_t Lsa;

  switch (Type)
  {
    case L7_S_LINK_OPAQUE_LSA:
      tmpType = S_LINK_OPAQUE_LSA;
      break;
    default:
      return L7_FAILURE;
  }

  if ((errRet = ospfLinkOpaqueLsdbTable_Lookup_Pack(ipAddr,ifIndex,tmpType,
          Lsid,RouterId,&Lsa)) == E_OK)
  {
    *val = Lsa.LsdbSequence;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get the Ospf Link Opaque LSA Age 
 *
 * @param    ipAddr      IpAddress
 * @param    ifIndex     Interface Index
 * @param    Type        Type
 * @param    Lsid        Link State id
 * @param    RouterId    Database entry
 * @param    *val        sequence number
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenType9LsaAgeGet(L7_uint32 ipAddr,
    L7_uint32 ifIndex,
    L7_int32  Type,
    L7_uint32 Lsid,
    L7_uint32 RouterId,
    L7_uint32 *val)
{
  e_Err errRet;
  L7_uint32 tmpType;
  L7_ospfOpaqueLsdbEntry_t Lsa;

  switch (Type)
  {
    case L7_S_LINK_OPAQUE_LSA:
      tmpType = S_LINK_OPAQUE_LSA;
      break;
    default:
      return L7_FAILURE;
  }

  if ((errRet = ospfLinkOpaqueLsdbTable_Lookup_Pack(ipAddr,ifIndex,
          tmpType,Lsid,RouterId,&Lsa)) == E_OK)
  {
    *val = Lsa.LsdbAge;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get the checksum of the type-9 LSA.
 *
 * @param    ipAddr      IpAddress
 * @param    ifIndex     Interface Index
 * @param    Type        Type
 * @param    Lsid        Link State id
 * @param    RouterId    Database entry
 * @param    *val        sequence number
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenType9LsaChecksumGet(L7_uint32 ipAddr,
    L7_uint32 ifIndex,
    L7_int32  Type,
    L7_uint32 Lsid,
    L7_uint32 RouterId,
    L7_uint32 *val)
{
  e_Err errRet;
  L7_uint32 tmpType;
  L7_ospfOpaqueLsdbEntry_t Lsa;

  switch (Type)
  {
    case L7_S_LINK_OPAQUE_LSA:
      tmpType = S_LINK_OPAQUE_LSA;
      break;
    default:
      return L7_FAILURE;
  }

  if ((errRet = ospfLinkOpaqueLsdbTable_Lookup_Pack(ipAddr,ifIndex,
          tmpType,Lsid,RouterId, &Lsa)) == E_OK)
  {
    *val = Lsa.LsdbChecksum;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get the Link State Database advertisement.
 *
 * @param    ipAddr      IpAddress
 * @param    ifIndex     Interface Index
 * @param    Type        Lsdb Type
 * @param    Lsid        Link State id
 * @param    RouterId    Database entry
 * @param    p_Lsa       Lsdb Entry given to the external world
 * @param    p_LsdbAdvertisement  Lsdb Advertisement
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none.
 *
 * @end
 *********************************************************************/
L7_RC_t ospfType9Advertisement_Lookup(L7_uint32 ipAddr,
    L7_uint32 ifIndex,
    L7_uint32 LsdbType,
    L7_uint32 LsdbId,
    L7_uint32 LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa,
    L7_char8 **p_LsdbAdvertisement)
{
  t_ARO         *p_ARO = NULL;
  t_IFO         *p_IFO = NULL;
  L7_BOOL        found = L7_FALSE;
  e_Err          e = E_FAILED;
  L7_ospfOpaqueLsdbEntry_t temp_Lsa;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  /* First find the ARO object corresponding to the ipAddr */
  e = HL_GetFirst(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl, (void *)&p_ARO);
  while(e == E_OK)
  {
    e = HL_GetFirst(p_ARO->IfoHl, (void *)&p_IFO);
    while(e == E_OK)
    {
      if(ipAddr == p_IFO->Cfg.IpAdr)
      {
        found = L7_TRUE;
        break;
      }
      e = HL_GetNext(p_ARO->IfoHl, (void *)&p_IFO, p_IFO);
    }
    if(found == L7_TRUE)
      break;
    e = HL_GetNext(((t_RTO *)ospfMapCtrl_g.RTO_Id)->AroHl, (void *)&p_ARO, p_ARO);
  }

  if (found != L7_TRUE)
  {
    return L7_FAILURE;
  }

  p_Lsa->IpAdr = ipAddr;
  p_Lsa->IfIndex = ifIndex;
  p_Lsa->LsdbLsid = LsdbId;
  p_Lsa->LsdbType = (L7_uchar8)LsdbType;
  p_Lsa->LsdbRouterId = LsdbRouterId;

  /* Find the Lsa corresponding to LsdbType, LsdbId, LsdbRouterId */
  if((e = ARO_OpaqueAdvertisement(p_ARO, p_Lsa, p_LsdbAdvertisement)) != E_OK)
  {
    return L7_FAILURE;
  }
  p_Lsa->IpAdr = p_IFO->Cfg.IpAdr;
  p_Lsa->IfIndex = p_IFO->Cfg.IfIndex;

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  ospfLinkOpaqueAdvertisement_Lookup_Unpack is the callback function
 *           to be called by the ospf thread to process the user
 *           configuration request
 *
 * @param    p_Info         pointer to the message containing
 *                          the required parameters
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
static e_Err ospfLinkOpaqueAdvertisement_Lookup_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  ulng ipAddr = (ulng)PACKET_GET(xxCallInfo, 0);
  ulng ifIndex = (ulng)PACKET_GET(xxCallInfo, 1);
  ulng Type = (ulng)PACKET_GET(xxCallInfo, 2);
  ulng Lsid = (ulng)PACKET_GET(xxCallInfo, 3);
  ulng RouterId = (ulng)PACKET_GET(xxCallInfo, 4);
  L7_ospfOpaqueLsdbEntry_t *p_Lsa = (L7_ospfOpaqueLsdbEntry_t *)PACKET_GET(xxCallInfo, 5);
  L7_char8 **p_LsdbAdvertisement = (L7_char8 **)PACKET_GET(xxCallInfo, 6);
  L7_RC_t *rc = (L7_RC_t *)PACKET_GET(xxCallInfo, 7);

  *rc = ospfType9Advertisement_Lookup(ipAddr,ifIndex, Type, Lsid, RouterId,
      p_Lsa, p_LsdbAdvertisement);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return ((*rc == L7_SUCCESS)? E_OK: E_FAILED);
}

/*********************************************************************
 * @purpose  ospfLinkOpaqueAdvertisement_Lookup_Pack is used to enqueue a
 *           request for configuring an ospf router object in the ospf thread.
 *
 * @param    ipAddr      IpAddress
 * @param    ifIndex     Interface Index
 * @param    LsdbType       Lsid Type
 * @param    LsdbId         Link State Id
 * @param    LsdbRouterId   Router Id
 * @param    p_Lsa          Lsdb Entry given to external world
 * @param    p_LsdbAdvertisement pointer to address where LSDB Advertisement is
 *                               copied by the OSPF code
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
e_Err ospfLinkOpaqueAdvertisement_Lookup_Pack(L7_uint32 ipAddr, L7_uint32 ifIndex,
    L7_uint32 LsdbType, L7_uint32 LsdbId,
    L7_uint32 LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa,
    L7_char8 **p_LsdbAdvertisement)
{
  t_XXCallInfo    *xxci;      /* XX_Call info */
  e_Err                   e;
  L7_RC_t                 rc;
  L7_int32                currThread = osapiTaskIdSelf();
  OS_Thread               *pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;
  L7_ospfOpaqueLsdbEntry_t      temp_Lsa;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }

    PACKET_INIT(xxci, ospfLinkOpaqueAdvertisement_Lookup_Unpack, 0, 0, 8,
        (L7_uint32)ipAddr);
    PACKET_PUT(xxci, (L7_uint32)ifIndex);
    PACKET_PUT(xxci, (L7_uint32)LsdbType);
    PACKET_PUT(xxci, (L7_uint32)LsdbId);
    PACKET_PUT(xxci, (L7_uint32)LsdbRouterId);
    PACKET_PUT(xxci, (L7_ospfOpaqueLsdbEntry_t *)p_Lsa);
    PACKET_PUT(xxci, (L7_char8 **)p_LsdbAdvertisement);
    PACKET_PUT(xxci, (L7_RC_t *)&rc);
    e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

    if (e != E_OK)
    {
      LOG_MSG("Failed to call ospfLinkOpaqueAdvertisement_Lookup_Pack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
      return E_FAILED;
    }

    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }
  }
  else
  {
    rc = ospfType9Advertisement_Lookup(ipAddr,ifIndex,LsdbType,LsdbId,
        LsdbRouterId,p_Lsa,p_LsdbAdvertisement);
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  }

  return (rc == L7_SUCCESS)? E_OK: E_FAILED;
}

/*********************************************************************
 * @purpose  Get the type-9 advertisement of the LSA.
 *
 * @param    ipAddr      IpAddress
 * @param    ifIndex     Interface Index
 * @param    Type        Type
 * @param    Lsid        Link State id
 * @param    RouterId    Database entry
 * @param    *val        sequence number
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenType9LsaAdvertisementGet(L7_uint32 ipAddr,
    L7_int32  ifIndex,
    L7_int32  Type,
    L7_uint32 Lsid, 
    L7_uint32 RouterId,
    L7_char8 **buf, L7_uint32 *len)
{
  e_Err errRet;
  L7_uint32 tmpType;
  L7_ospfOpaqueLsdbEntry_t Lsa;

  switch (Type)
  {
    case L7_S_LINK_OPAQUE_LSA:
      tmpType = S_LINK_OPAQUE_LSA;
      break;
    default:
      return L7_FAILURE;
  }

  if ((errRet = ospfLinkOpaqueAdvertisement_Lookup_Pack(ipAddr,ifIndex,tmpType,
          Lsid, RouterId, &Lsa, buf)) == E_OK)
  {
    *len = Lsa.LsdbAdvertLength;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get the As Link State Database entry.
 *
 * @param    Type        Lsdb Type
 * @param    Lsid        Link State id
 * @param    RouterId    Database entry
 * @param    p_Lsa       Lsdb Entry given to the external world
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none.
 *
 * @end
 *********************************************************************/
L7_RC_t ospfAsOpaqueLsdbTable_Lookup(L7_uint32 LsdbType, L7_uint32 LsdbId, 
    L7_uint32 LsdbRouterId, 
    L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  t_RTO         *p_RTO = ospfMapCtrl_g.RTO_Id;
  e_Err         e = E_FAILED;
  L7_ospfOpaqueLsdbEntry_t temp_Lsa;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  p_Lsa->LsdbLsid = LsdbId;
  p_Lsa->LsdbType = (L7_uchar8)LsdbType;
  p_Lsa->LsdbRouterId = LsdbRouterId;

  /* Find the Lsa corresponding to LsdbType, LsdbId, LsdbRouterId */
  if((e = ARO_OpaqueLsdbEntry(p_RTO, p_Lsa)) != E_OK)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  ospfAsOpaqueLsdbTable_Lookup_Unpack is the callback 
 *           function to be called by the ospf thread to process the
 *           user configuration request
 *
 * @param    p_Info         pointer to the message containing
 *                          the required parameters
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
static e_Err ospfAsOpaqueLsdbTable_Lookup_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  L7_uint32 Type = (L7_uint32)PACKET_GET(xxCallInfo, 0);
  L7_uint32 Lsid = (L7_uint32)PACKET_GET(xxCallInfo, 1);
  L7_uint32 RouterId = (L7_uint32)PACKET_GET(xxCallInfo, 2);
  L7_ospfOpaqueLsdbEntry_t *p_Lsa = 
    (L7_ospfOpaqueLsdbEntry_t *)PACKET_GET(xxCallInfo, 3);
  L7_RC_t *rc = (L7_RC_t *)PACKET_GET(xxCallInfo, 4);

  *rc = ospfAsOpaqueLsdbTable_Lookup(Type,Lsid,RouterId,p_Lsa);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return ((*rc == L7_SUCCESS)? E_OK: E_FAILED);
}

/*********************************************************************
 * @purpose  ospfAsOpaqueLsdbTable_Lookup_Pack is used to enqueue a 
 *           request for configuring an ospf router object in the ospf thread.
 *
 * @param    LsdbType       Lsid Type
 * @param    LsdbId         Link State Id
 * @param    LsdbRouterId   Router Id
 * @param    p_Lsa          Lsdb Entry given to external world
 *
 * @returns  E_OK           config successfuly setup or read
 E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
e_Err ospfAsOpaqueLsdbTable_Lookup_Pack(L7_uint32 LsdbType, L7_uint32 LsdbId,
    L7_uint32 LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  t_XXCallInfo    *xxci;      /* XX_Call info */
  e_Err                   e;
  L7_RC_t                 rc;
  L7_int32                currThread = osapiTaskIdSelf();
  OS_Thread               *pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;
  L7_ospfOpaqueLsdbEntry_t  temp_Lsa;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }

    PACKET_INIT(xxci,ospfAsOpaqueLsdbTable_Lookup_Unpack,0,0,5,(L7_uint32)LsdbType);
    PACKET_PUT(xxci, (L7_uint32)LsdbId);
    PACKET_PUT(xxci, (L7_uint32)LsdbRouterId);
    PACKET_PUT(xxci, (L7_ospfOpaqueLsdbEntry_t *)p_Lsa);
    PACKET_PUT(xxci, (L7_RC_t *)&rc);
    e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

    if (e != E_OK)
    {
      LOG_MSG("Failed to call ospfAsOpaqueLsdbTable_Lookup_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
      return E_FAILED;
    }

    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }
  }
  else
  {
    rc = ospfAsOpaqueLsdbTable_Lookup(LsdbType,LsdbId,LsdbRouterId,p_Lsa);
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  }

  return (rc == L7_SUCCESS)? E_OK: E_FAILED;
}

/*********************************************************************
 * @purpose  Get the process's AsOpaque Link State Database entry.
 *
 * @param    Type        type
 * @param    Lsid        Link State id
 * @param    RouterId    Advertising Router
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none.
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenAsOpaqueLsdbEntryGet(L7_uint32 LsdbType, L7_uint32 LsdbId,
    L7_uint32 LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  e_Err errRet;

  if ((errRet = ospfAsOpaqueLsdbTable_Lookup_Pack(LsdbType,
          LsdbId,LsdbRouterId,p_Lsa)) == E_OK)
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get the AsOpaque Link State Database entry.
 *
 * @param    Type        Lsdb Type
 * @param    Lsid        Link State id
 * @param    RouterId    Database entry
 * @param    p_Lsa       Lsdb Entry given to the external world
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none.
 *
 * @end
 *********************************************************************/
L7_RC_t ospfAsOpaqueLsdbTable_NextLookup(L7_int32  *LsdbType,
    L7_uint32 *LsdbId, 
    L7_uint32 *LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  t_RTO         *p_RTO = ospfMapCtrl_g.RTO_Id;
  e_Err         e      = E_FAILED;
  L7_ospfOpaqueLsdbEntry_t temp_Lsa;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  p_Lsa->LsdbLsid = *LsdbId;
  p_Lsa->LsdbRouterId = *LsdbRouterId;
  p_Lsa->LsdbType = (L7_uchar8)*LsdbType;
  if(*LsdbType == L7_NULL)
    p_Lsa->LsdbType = S_AS_OPAQUE_LSA;

  /* Find the Lsa corresponding to LsdbType, LsdbId, LsdbRouterId */
  if((e = ARO_OpaqueLsdbNextEntry(p_RTO, p_Lsa)) != E_OK)
  {
    return L7_FAILURE;
  }

  *LsdbType = p_Lsa->LsdbType;
  *LsdbId = p_Lsa->LsdbLsid;
  *LsdbRouterId = p_Lsa->LsdbRouterId;

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  ospfAsOpaqueLsdbTable_Lookup_Unpack is the callback 
 *           function to be called by the ospf thread to process the
 *           user configuration request
 *
 * @param    p_Info         pointer to the message containing
 *                          the required parameters
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
static e_Err ospfAsOpaqueLsdbTable_NextLookup_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  L7_int32  *Type = (L7_int32*)PACKET_GET(xxCallInfo, 0);
  L7_uint32 *Lsid = (L7_uint32*)PACKET_GET(xxCallInfo, 1);
  L7_uint32 *RouterId = (L7_uint32*)PACKET_GET(xxCallInfo, 2);
  L7_ospfOpaqueLsdbEntry_t *p_Lsa = 
    (L7_ospfOpaqueLsdbEntry_t *)PACKET_GET(xxCallInfo, 3);
  L7_RC_t *rc = (L7_RC_t *)PACKET_GET(xxCallInfo, 4);

  *rc = ospfAsOpaqueLsdbTable_NextLookup(Type,Lsid,RouterId,p_Lsa);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return ((*rc == L7_SUCCESS)? E_OK: E_FAILED);
}

/*********************************************************************
 * @purpose  ospfAsOpaqueLsdbTable_NextLookup_Pack is used to enqueue a 
 *           request for configuring an ospf router object in the ospf thread.
 *
 * @param    LsdbType       Lsid Type
 * @param    LsdbId         Link State Id
 * @param    LsdbRouterId   Router Id
 * @param    p_Lsa          Lsdb Entry given to external world
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
e_Err ospfAsOpaqueLsdbTable_NextLookup_Pack(L7_int32  *LsdbType,
    L7_uint32 *LsdbId,
    L7_uint32 *LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  t_XXCallInfo    *xxci;      /* XX_Call info */
  e_Err                   e;
  L7_RC_t                 rc;
  L7_int32                currThread = osapiTaskIdSelf();
  OS_Thread               *pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;
  L7_ospfOpaqueLsdbEntry_t  temp_Lsa;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }

    PACKET_INIT(xxci, ospfAsOpaqueLsdbTable_NextLookup_Unpack, 0, 0, 5, (L7_uint32)LsdbType);
    PACKET_PUT(xxci, (L7_uint32*)LsdbId);
    PACKET_PUT(xxci, (L7_uint32*)LsdbRouterId);
    PACKET_PUT(xxci, (L7_ospfOpaqueLsdbEntry_t *)p_Lsa);
    PACKET_PUT(xxci, (L7_RC_t *)&rc);
    e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

    if (e != E_OK)
    {
      LOG_MSG("Failed to call ospfAsOpaqueLsdbTable_NextLookup_Unpack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
      return E_FAILED;
    }

    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }
  }
  else
  {
    rc = ospfAsOpaqueLsdbTable_NextLookup(LsdbType,LsdbId,LsdbRouterId,p_Lsa);
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  }

  return (rc == L7_SUCCESS)? E_OK: E_FAILED;
}

/*********************************************************************
 * @purpose  Get the process's AsOpaque Next Link State Database entry.
 *
 * @param    Type        type
 * @param    Lsid        Link State id
 * @param    RouterId    Advertising Router
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none.
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenAsOpaqueLsdbEntryNext(L7_int32  *Type,
    L7_uint32 *Lsid, 
    L7_uint32 *RouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa)
{
  e_Err errRet;

  if ((errRet = ospfAsOpaqueLsdbTable_NextLookup_Pack(Type,
          Lsid,RouterId,p_Lsa)) == E_OK)
  {
    switch (*Type)
    {
      case S_ILLEGAL_LSA:
      case S_AS_OPAQUE_LSA:
        *Type = L7_S_AS_OPAQUE_LSA;
        break;
      default:
        return L7_FAILURE;
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get the sequence number.
 *
 * @param    Type        Type
 * @param    Lsid        Link State id
 * @param    RouterId    Database entry
 * @param    *val        sequence number
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    sequence number field is a signed 32-bit integer.
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenType11LsaSequenceGet(L7_int32  Type,
    L7_uint32 Lsid, 
    L7_uint32 RouterId,
    L7_uint32 *val)
{
  e_Err errRet;
  L7_uint32 tmpType;
  L7_ospfOpaqueLsdbEntry_t Lsa;

  switch (Type)
  {
    case L7_S_AS_OPAQUE_LSA:
      tmpType = S_AS_OPAQUE_LSA;
      break;
    default:
      return L7_FAILURE;
  }

  if ((errRet = ospfAsOpaqueLsdbTable_Lookup_Pack(tmpType,
          Lsid,RouterId,&Lsa)) == E_OK)
  {
    *val = Lsa.LsdbSequence;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get the Ospf As Opaque LSA Age 
 *
 * @param    Type        Type
 * @param    Lsid        Link State id
 * @param    RouterId    Database entry
 * @param    *val        sequence number
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenType11LsaAgeGet(L7_int32  Type,
    L7_uint32 Lsid,
    L7_uint32 RouterId,
    L7_uint32 *val)
{
  e_Err errRet;
  L7_uint32 tmpType;
  L7_ospfOpaqueLsdbEntry_t Lsa;

  switch (Type)
  {
    case L7_S_AS_OPAQUE_LSA:
      tmpType = S_AS_OPAQUE_LSA;
      break;
    default:
      return L7_FAILURE;
  }

  if ((errRet = ospfAsOpaqueLsdbTable_Lookup_Pack(tmpType,Lsid,
          RouterId,&Lsa)) == E_OK)
  {
    *val = Lsa.LsdbAge;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get the checksum of the type-11 LSA.
 *
 * @param    Type        Type
 * @param    Lsid        Link State id
 * @param    RouterId    Database entry
 * @param    *val        sequence number
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenType11LsaChecksumGet(L7_int32  Type,
    L7_uint32 Lsid,
    L7_uint32 RouterId,
    L7_uint32 *val)
{
  e_Err errRet;
  L7_uint32 tmpType;
  L7_ospfOpaqueLsdbEntry_t Lsa;

  switch (Type)
  {
    case L7_S_AS_OPAQUE_LSA:
      tmpType = S_AS_OPAQUE_LSA;
      break;
    default:
      return L7_FAILURE;
  }

  if ((errRet = ospfAsOpaqueLsdbTable_Lookup_Pack(tmpType,Lsid,
          RouterId, &Lsa)) == E_OK)
  {
    *val = Lsa.LsdbChecksum;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Get the Link State Database advertisement.
 *
 * @param    Type        Lsdb Type
 * @param    Lsid        Link State id
 * @param    RouterId    Database entry
 * @param    p_Lsa       Lsdb Entry given to the external world
 * @param    p_LsdbAdvertisement  Lsdb Advertisement
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none.
 *
 * @end
 *********************************************************************/
L7_RC_t ospfType11Advertisement_Lookup(L7_uint32 LsdbType,
    L7_uint32 LsdbId,
    L7_uint32 LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa,
    L7_char8 **p_LsdbAdvertisement)
{
  t_Handle      *p_RTO = ospfMapCtrl_g.RTO_Id;
  e_Err          e = E_FAILED;
  L7_ospfOpaqueLsdbEntry_t temp_Lsa;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  p_Lsa->LsdbLsid = LsdbId;
  p_Lsa->LsdbType = (L7_uchar8)LsdbType;
  p_Lsa->LsdbRouterId = LsdbRouterId;

  /* Find the Lsa corresponding to LsdbType, LsdbId, LsdbRouterId */
  if((e = ARO_OpaqueAdvertisement(p_RTO, p_Lsa, p_LsdbAdvertisement)) != E_OK)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  ospfAsOpaqueAdvertisement_Lookup_Unpack is the callback function
 *           to be called by the ospf thread to process the user
 *           configuration request
 *
 * @param    p_Info         pointer to the message containing
 *                          the required parameters
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
static e_Err ospfAsOpaqueAdvertisement_Lookup_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  ulng Type = (ulng)PACKET_GET(xxCallInfo, 0);
  ulng Lsid = (ulng)PACKET_GET(xxCallInfo, 1);
  ulng RouterId = (ulng)PACKET_GET(xxCallInfo, 2);
  L7_ospfOpaqueLsdbEntry_t *p_Lsa = (L7_ospfOpaqueLsdbEntry_t *)PACKET_GET(xxCallInfo, 3);
  L7_char8 **p_LsdbAdvertisement = (L7_char8 **)PACKET_GET(xxCallInfo, 4);
  L7_RC_t *rc = (L7_RC_t *)PACKET_GET(xxCallInfo, 5);

  *rc = ospfType11Advertisement_Lookup(Type, Lsid, RouterId,
      p_Lsa, p_LsdbAdvertisement);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return ((*rc == L7_SUCCESS)? E_OK: E_FAILED);
}

/*********************************************************************
 * @purpose  ospfAsOpaqueAdvertisement_Lookup_Pack is used to enqueue a
 *           request for configuring an ospf router object in the ospf thread.
 *
 * @param    LsdbType       Lsid Type
 * @param    LsdbId         Link State Id
 * @param    LsdbRouterId   Router Id
 * @param    p_Lsa          Lsdb Entry given to external world
 * @param    p_LsdbAdvertisement pointer to address where LSDB Advertisement is
 *                               copied by the OSPF code
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
e_Err ospfAsOpaqueAdvertisement_Lookup_Pack(L7_uint32 LsdbType,L7_uint32 LsdbId,
    L7_uint32 LsdbRouterId,
    L7_ospfOpaqueLsdbEntry_t *p_Lsa,
    L7_char8 **p_LsdbAdvertisement)
{
  t_XXCallInfo    *xxci;      /* XX_Call info */
  e_Err                   e;
  L7_RC_t                 rc;
  L7_int32                currThread = osapiTaskIdSelf();
  OS_Thread               *pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;
  L7_ospfOpaqueLsdbEntry_t      temp_Lsa;

  if(p_Lsa == NULL)
  {
    p_Lsa = &temp_Lsa;
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }

    PACKET_INIT(xxci, ospfAsOpaqueAdvertisement_Lookup_Unpack, 0, 0, 6,
        (L7_uint32)LsdbType);
    PACKET_PUT(xxci, (L7_uint32)LsdbId);
    PACKET_PUT(xxci, (L7_uint32)LsdbRouterId);
    PACKET_PUT(xxci, (L7_ospfOpaqueLsdbEntry_t *)p_Lsa);
    PACKET_PUT(xxci, (L7_char8 **)p_LsdbAdvertisement);
    PACKET_PUT(xxci, (L7_RC_t *)&rc);
    e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

    if (e != E_OK)
    {
      LOG_MSG("Failed to call ospfAsOpaqueAdvertisement_Lookup_Pack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
      return E_FAILED;
    }

    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }
  }
  else
  {
    rc = ospfType11Advertisement_Lookup(LsdbType,LsdbId,
        LsdbRouterId,p_Lsa,p_LsdbAdvertisement);
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  }

  return (rc == L7_SUCCESS)? E_OK: E_FAILED;
}

/*********************************************************************
 * @purpose  Get the type-11 advertisement of the LSA.
 *
 * @param    Type        Type
 * @param    Lsid        Link State id
 * @param    RouterId    Database entry
 * @param    *val        sequence number
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes    
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenType11LsaAdvertisementGet(L7_int32  Type,
    L7_uint32 Lsid, 
    L7_uint32 RouterId,
    L7_char8 **buf, L7_uint32 *len)
{
  e_Err errRet;
  L7_uint32 tmpType;
  L7_ospfOpaqueLsdbEntry_t Lsa;

  switch (Type)
  {
    case L7_S_AS_OPAQUE_LSA:
      tmpType = S_AS_OPAQUE_LSA;
      break;

    default:
      return L7_FAILURE;
  }

  if ((errRet = ospfAsOpaqueAdvertisement_Lookup_Pack(tmpType,
          Lsid, RouterId, &Lsa, buf)) == E_OK)
  {
    *len = Lsa.LsdbAdvertLength;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  ospfASOpaqueCksumSum_Get_UnPack is the callback function to
 *           be called by the ospf thread to process the user 
 *           configuration request
 *
 * @param    p_Info         pointer to the message containing
 *                          the required information
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
static e_Err ospfASOpaqueCksumSum_Get_UnPack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_Handle      RTO_Id = (t_Handle)PACKET_GET(xxCallInfo, 0);
  L7_uint32     *opaqueCksumSum = (L7_uint32 *)PACKET_GET(xxCallInfo, 1);
  e_Err         e;

  e = RTO_ASOpaqueCksumSum_Get(RTO_Id, opaqueCksumSum);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return e;
}
/*********************************************************************
 * @purpose  ospfASOpaqueCksumSum_Get_Pack is used to enqueue a request
 *           for getting an ospf router object in the ospf thread.
 *
 * @param    RTO_Id     RTO object Id
 * @param    opaqueCksumSum TYPE-11 OPAQUE CHECKSUM SUM
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
e_Err ospfASOpaqueCksumSum_Get_Pack(t_Handle RTO_Id, L7_uint32 *opaqueCksumSum)
{
  t_XXCallInfo    *xxci;      /* XX_Call info */
  e_Err           e;
  L7_int32        currThread = osapiTaskIdSelf();
  OS_Thread       *pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }

    PACKET_INIT(xxci, ospfASOpaqueCksumSum_Get_UnPack, 0, 0, 2, (ulng)RTO_Id);
    PACKET_PUT(xxci, (ulng)opaqueCksumSum);
    e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

    if (e != E_OK)
    {
      LOG_MSG("Failed to call RTO_ASOpaqueCksumSum_Get_UnPack on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
      return E_FAILED;
    }

    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }
  }
  else
  {
    e = RTO_ASOpaqueCksumSum_Get(RTO_Id, opaqueCksumSum);
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  }

  return e;
}

/*********************************************************************
 * @purpose  Get the checksum Sum of the type-11 LSA.
 *
 * @param    *val        checksumSum
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenASOpaqueLSAChecksumSumGet(L7_uint32 *val)
{
  e_Err errRet;
  t_Handle      *RTO_Id = ospfMapCtrl_g.RTO_Id;

  if ((errRet = ospfASOpaqueCksumSum_Get_Pack(RTO_Id, val)) == E_OK)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Is OSPF in stub router advertisement mode?
 *
 * @param    p_Info         pointer to the message containing
 *                          the required information
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
static e_Err ospfIsStubRtr_Get_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_Handle      RTO_Id = (t_Handle)PACKET_GET(xxCallInfo, 0);
  L7_uint32     *isStubRtr = (L7_uint32 *)PACKET_GET(xxCallInfo, 1);
  e_Err         e;

  e = RTO_IsStubRtr_Get(RTO_Id, isStubRtr);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return e;
}

/*********************************************************************
 * @purpose  is router in stub advertisement mode
 *
 * @param    RTO_Id     RTO object Id
 * @param    isStubRtr  output
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
e_Err ospfIsStubRtr_Get_Pack(t_Handle RTO_Id, L7_BOOL *isStubRtr)
{
  t_XXCallInfo    *xxci;      /* XX_Call info */
  e_Err           e;
  L7_int32        currThread = osapiTaskIdSelf();
  OS_Thread       *pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }

    PACKET_INIT(xxci, ospfIsStubRtr_Get_Unpack, 0, 0, 2, (ulng)RTO_Id);
    PACKET_PUT(xxci, (ulng)isStubRtr);
    e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

    if (e != E_OK)
    {
      LOG_MSG("Failed to call ospfIsStubRtr_Get_Unpack() on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
      return E_FAILED;
    }

    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }
  }
  else
  {
    e = RTO_IsStubRtr_Get(RTO_Id, isStubRtr);
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  }

  return e;
}

/*********************************************************************
 * @purpose  Is OSPF in stub router mode?
 *
 * @param    *isStubRtr        output value
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenIsStubRtrGet(L7_BOOL *isStubRtr)
{
  e_Err errRet;
  t_Handle      *RTO_Id = ospfMapCtrl_g.RTO_Id;

  if ((errRet = ospfIsStubRtr_Get_Pack(RTO_Id, isStubRtr)) == E_OK)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}


/*********************************************************************
 * @purpose  Is OSPF in external LSDB overflow?
 *
 * @param    p_Info         pointer to the message containing
 *                          the required information
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
static e_Err ospfExtLsdbOverflow_Get_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_Handle      RTO_Id = (t_Handle)PACKET_GET(xxCallInfo, 0);
  L7_uint32     *extLsdbOverflow = (L7_uint32 *)PACKET_GET(xxCallInfo, 1);
  e_Err         e;

  e = RTO_ExtLsdbOverflow_Get(RTO_Id, extLsdbOverflow);

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);

  return e;
}

/*********************************************************************
 * @purpose  is router in external LSDB overflow?
 *
 * @param    RTO_Id     RTO object Id
 * @param    extLsdbOverflow  output
 *
 * @returns  E_OK           config successfuly setup or read
 *           E_FAILED       couldn't setup or read
 *           E_BADPARM      bad configuration parameters
 *
 * @notes
 *
 * @end
 *********************************************************************/
e_Err ospfExtLsdbOverflow_Get_Pack(t_Handle RTO_Id, L7_BOOL *extLsdbOverflow)
{
  t_XXCallInfo    *xxci;      /* XX_Call info */
  e_Err           e;
  L7_int32        currThread = osapiTaskIdSelf();
  OS_Thread       *pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }

    PACKET_INIT(xxci, ospfExtLsdbOverflow_Get_Unpack, 0, 0, 2, (ulng)RTO_Id);
    PACKET_PUT(xxci, (ulng)extLsdbOverflow);
    e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);

    if (e != E_OK)
    {
      LOG_MSG("Failed to call ospfExtLsdbOverflow_Get_Unpack() on OSPF thread\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
      return E_FAILED;
    }

    if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_MSG("Failed to take OSPF sync semaphore\n");
      return E_FAILED;
    }
  }
  else
  {
    e = RTO_ExtLsdbOverflow_Get(RTO_Id, extLsdbOverflow);
  }

  if (currThread != (L7_int32) pThreadGlobal->TaskID)
  {
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  }

  return e;
}

/*********************************************************************
 * @purpose  Is OSPF in external LSDB overflow?
 *
 * @param    *extLsdbOverflow        output value
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapExtenExtLsdbOverflowGet(L7_BOOL *extLsdbOverflow)
{
  e_Err errRet;
  t_Handle      *RTO_Id = ospfMapCtrl_g.RTO_Id;

  if ((errRet = ospfExtLsdbOverflow_Get_Pack(RTO_Id, extLsdbOverflow)) == E_OK)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Callback function
*           to be called by the ospf thread for general status information.
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err ospfGlobalStatus_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_Handle      RTO_Id = (t_Handle)PACKET_GET(xxCallInfo, 0);
  L7_ospfStatus_t  *ospfStatus = (L7_ospfStatus_t*) PACKET_GET(xxCallInfo, 1);
  e_Err         e;

  e = RTO_GlobalStatusGet(RTO_Id, ospfStatus);
  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  return e;
}

/*********************************************************************
* @purpose  Request global stats
*
* @param    areaId     area object Id
* @param    status     output structure ptr
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err ospfGlobalStatus_Pack(t_Handle RTO_Id, L7_ospfStatus_t *status)
{
  t_XXCallInfo    *xxci;      /* XX_Call info */
  e_Err           e;
  L7_int32        currThread = osapiTaskIdSelf();
  OS_Thread       *pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

  if (currThread == (L7_int32) pThreadGlobal->TaskID)
  {
    /* This is for UI to get OSPFv2 status. Should never be called 
     * on OSPFv2 protocol thread. */
    return E_FAILED;
  }

  PACKET_INIT(xxci, ospfGlobalStatus_Unpack, 0, 0, 2, (ulng)RTO_Id);
  PACKET_PUT(xxci, (ulng) status);
  if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_MSG("Failed to take OSPF sync semaphore\n");
    return E_FAILED;
  }

  e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);
  if (e != E_OK)
  {
    /* No need to log a message. UI just won't get status. */
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
    return E_FAILED;
  }

  /* Block until protocol thread has handled request. */
  if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, 
                    L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_MSG( "Failed to take OSPF sync semaphore\n");
    return E_FAILED;
  }

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  return E_OK;
}

/*********************************************************************
* @purpose  Get general OSPFv3 status information
*
* @param    *status   @b{(output)     pointer to output location                       
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenStatusGet(L7_ospfStatus_t *status)
{
  t_Handle *RTO_Id = ospfMapCtrl_g.RTO_Id;
  if (ospfGlobalStatus_Pack(RTO_Id, status) == E_OK)
    return L7_SUCCESS;
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Tell OSPF to clear neighbors.
*
* @param    RTO_Id - OSPF instance
*           p_IFO - if non-NULL, restrict clear to this interface
*           routerId - If non-zero, restrict clear to neighbors with this router ID
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err ospfNeighborClear_Pack(t_Handle RTO_Id, t_IFO *p_IFO, L7_uint32 routerId)
{
  t_XXCallInfo    *xxci;      /* XX_Call info */
  e_Err           e;
  L7_int32        currThread = osapiTaskIdSelf();
  OS_Thread       *pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

  if (currThread == (L7_int32) pThreadGlobal->TaskID)
  {
    /* This is for UI to get OSPFv2 status. Should never be called 
     * on OSPFv2 protocol thread. */
    return E_FAILED;
  }

  PACKET_INIT(xxci, ospfNeighborClear_Unpack, 0, 0, 3, (ulng)RTO_Id);
  PACKET_PUT(xxci, (ulng) p_IFO);
  PACKET_PUT(xxci, (ulng) routerId);
  if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_MSG("Failed to take OSPF sync semaphore\n");
    return E_FAILED;
  }

  e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);
  if (e != E_OK)
  {
    /* No need to log a message. UI just won't get status. */
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
    return E_FAILED;
  }

  /* Block until protocol thread has handled request. */
  if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, 
                    L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_MSG( "Failed to take OSPF sync semaphore\n");
    return E_FAILED;
  }

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  return E_OK;
}

/*********************************************************************
* @purpose  Callback function
*           to be called by the ospf thread to clear neighbors
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err ospfNeighborClear_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_Handle      RTO_Id = (t_Handle)PACKET_GET(xxCallInfo, 0);
  t_Handle      IFO_Id = (t_Handle)PACKET_GET(xxCallInfo, 1);
  ulng          routerId = PACKET_GET(xxCallInfo, 2);
  e_Err         e;

  e = RTO_NeighborClear(RTO_Id, IFO_Id, routerId);
  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  return e;
}


/*********************************************************************
* @purpose  Clear OSPF neighbors.
*
* @param    intIfNum - If non-zero, restrict clear to neighbors on this interface.
* @param    routerId   If non-zero, restrict clear to neighbors with this router ID. 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenNeighborClear(L7_uint32 intIfNum, L7_uint32 routerId)
{
  t_Handle *RTO_Id = ospfMapCtrl_g.RTO_Id;
  t_IFO *p_IFO = NULL;

  if (intIfNum)
  {
    p_IFO = ifoPtrGet(intIfNum);
    if (p_IFO == NULL)
      return L7_ERROR;
  }

  if (ospfNeighborClear_Pack(RTO_Id, p_IFO, routerId) == E_OK)
    return L7_SUCCESS;

  return L7_FAILURE;
}


/*********************************************************************
* @purpose  Tell OSPF to clear statistics.
*
* @param    RTO_Id - OSPF instance
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
e_Err ospfCountersClear_Pack(t_Handle RTO_Id)
{
  t_XXCallInfo    *xxci;      /* XX_Call info */
  e_Err           e;
  L7_int32        currThread = osapiTaskIdSelf();
  OS_Thread       *pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

  if (currThread == (L7_int32) pThreadGlobal->TaskID)
  {
    /* Should never be called on OSPFv2 protocol thread. */
    return E_FAILED;
  }

  PACKET_INIT(xxci, ospfCountersClear_Unpack, 0, 0, 3, (ulng)RTO_Id);
  if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_MSG("Failed to take OSPF sync semaphore\n");
    return E_FAILED;
  }

  e = XX_Call(ospfMapCtrl_g.ospfThread, xxci);
  if (e != E_OK)
  {
    /* No need to log a message. UI just won't get status. */
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
    return E_FAILED;
  }

  /* Block until protocol thread has handled request. */
  if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, 
                    L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_MSG( "Failed to take OSPF sync semaphore\n");
    return E_FAILED;
  }

  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  return E_OK;
}

/*********************************************************************
* @purpose  Callback function
*           to be called by the ospf thread to clear counters
*
* @param    p_Info         pointer to the message containing
*                          the required parameters
*
* @returns  E_OK           config successfuly setup or read
*           E_FAILED       couldn't setup or read
*           E_BADPARM      bad configuration parameters
*
* @notes
*
* @end
*********************************************************************/
static e_Err ospfCountersClear_Unpack(void *p_Info)
{
  t_XXCallInfo  *xxCallInfo = (t_XXCallInfo *)p_Info;
  t_Handle      RTO_Id = (t_Handle)PACKET_GET(xxCallInfo, 0);
  e_Err         e;

  e = RTO_CountersClear(RTO_Id);
  osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
  return e;
}


/*********************************************************************
* @purpose  Clear OSPF statistics.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtenCountersClear(void)
{
  t_Handle *RTO_Id = ospfMapCtrl_g.RTO_Id;

  if (ospfCountersClear_Pack(RTO_Id) == E_OK)
    return L7_SUCCESS;

  return L7_FAILURE;
}


