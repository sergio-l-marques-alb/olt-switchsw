/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  ospfmap.c
*
* @purpose   Ospf utility functions
*
* @component Ospf Mapping Layer
*
* @comments  none
*
* @create    03/19/2001
*
* @author    asuthan
*
* @end
*             
**********************************************************************/

#include "l7_ospfinclude.h"
#include "l7_ospf_api.h"
#include "l3end_api.h"
#include "ospf_cnfgr.h"
#include "osapi_support.h"
#include "ospf_debug_api.h"
#include "support_api.h"
#include "acl_api.h"

#ifdef L7_NSF_PACKAGE
#include "ospf_ckpt.h"
#endif

L7_uint32 tosIndexToTos[L7_RTR_MAX_TOS_INDICES] = 
{
    L7_TOS_NORMAL_SERVICE,
    L7_TOS_MINIMIZE_COST,
    L7_TOS_MAXIMIZE_RELIABILITY,
    L7_TOS_MAXIMIZE_THROUGHPUT,
    L7_TOS_MINIMIZE_DELAY  
};


L7_uint32 tosToTosIndex[L7_RTR_MAX_TOS] = 
{
    L7_TOS_NORMAL_SERVICE_INDEX,0,L7_TOS_MINIMIZE_COST_INDEX,0,
    L7_TOS_MAXIMIZE_RELIABILITY_INDEX,0,0,0,
    L7_TOS_MAXIMIZE_THROUGHPUT_INDEX,0,0,0,
    0,0,0,0,
    L7_TOS_MINIMIZE_DELAY_INDEX,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0
};


/* Declarations */

L7_int32            L7_ospf_task_id;
L7_ospfMapCfg_t     *pOspfMapCfgData = L7_NULLPTR;
ospfInfo_t          *pOspfInfo = L7_NULLPTR;
ospfAreaInfo_t      *pOspfAreaInfo = L7_NULLPTR;
ospfVlinkInfo_t     *pOspfVlinkInfo = L7_NULLPTR;

ospfNetworkAreaInfo_t  *pOspfNetworkAreaInfo = L7_NULLPTR;
ospfNetworkAreaInfo_t  *networkAreaListHead_g = L7_NULLPTR;
ospfNetworkAreaInfo_t  *networkAreaListFree_g = L7_NULLPTR;

/* This points to the first element in an array. The number of 
 * elements in the array is the maximum number of interfaces
 * (L7_OSPF_INTF_MAX_COUNT). The array is indexed by internal
 * interface numbers.
 */
ospfIntfInfo_t      *pOspfIntfInfo = L7_NULLPTR;

L7_uint32           *ospfMapMapTbl = L7_NULLPTR;

/* Max messages in the OSPF redistribution queue */
static L7_int32 redistQHighWaterMark = 0;

#define OSPF_LAST_DATA_CNG_LEN 64
static L7_uchar8 ospfLastDataChanged[OSPF_LAST_DATA_CNG_LEN + 1];
static L7_BOOL ospfTraceDataChange = L7_FALSE;

/* Externs */

extern ospfMapCnfgrState_t ospfMapCnfgrState;
ospfMapDeregister_t ospfMapDeregister = {L7_FALSE, L7_FALSE, L7_FALSE};
extern void *ospfMsgQSema;
extern void *Ospf_Proc_Queue;
extern void *Ospf_Redist_Queue;


static L7_RC_t ospfMapRoutingEventProcess (L7_uint32 intIfNum, 
                                           L7_uint32 event,
                                           void *pData, 
                                           ASYNC_EVENT_NOTIFY_INFO_t *response);
ospfMapDebugCfg_t ospfMapDebugCfg;    /* Debug Configuration File Overlay */
extern void ospfMapDebugCfgUpdate(void);
static L7_uint32 ospfIfTypeFromNimIfType(L7_INTF_TYPES_t nimIfType);

/*********************************************************************
* @purpose  Initialize OSPF tasks
*
* @param    void
*
* @returns  L7_SUCCESS if tasks successfully started
* @returns  L7_FAILURE otherwise
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfStartTasks(void)
{
    static const char *routine_name = "ospfStartTasks()";

    OSPFMAP_TRACE("%s : \n", routine_name);

    /* Create and initialize the OSPF protocol engine */
    if (ospfMapOspfTaskCreate() != L7_SUCCESS)
    {
        OSPFMAP_ERROR("%s: ERROR: Unable to create OSPF task\n", routine_name);
        return L7_FAILURE;
    }

    return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Initialize ospf task
*
* @param    void
*
* @returns  void
*
* @notes    none
*       
* @end
*********************************************************************/
void ospfEndTasks(void)
{
    ospfMapOspfTaskDelete();
}

/*********************************************************************
* @purpose  Checks if ospf user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL ospfHasDataChanged(void)
{

    if (ospfMapDeregister.ospfMapHasDataChanged == L7_TRUE)
        return L7_FALSE;

    return pOspfMapCfgData->cfgHdr.dataChanged;
}

/*********************************************************************
* @purpose  Reset OSPF's data changed fla
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
void ospfResetDataChanged(void)
{
  pOspfMapCfgData->cfgHdr.dataChanged = L7_FALSE;
  return;
}

/*********************************************************************
* @purpose  Record that there has been a change to OSPF configuration data
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
void ospfDataChangedSet(const L7_uchar8 *caller)
{
  if (ospfTraceDataChange)
    printf("\n%s setting data changed flag", caller);
  pOspfMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  strncpy(ospfLastDataChanged, caller, OSPF_LAST_DATA_CNG_LEN);
}

/*********************************************************************
* @purpose  Record that there has been a change to OSPF configuration data
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
void ospfLastDataChangedPrint(void)
{
  printf("\nLast function to set the OSPF data changed flag:  %s.",
         ospfLastDataChanged);
}

/*********************************************************************
* @purpose  Record that there has been a change to OSPF configuration data
*
* @param    void
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
void ospfDataChangedTraceSet(L7_BOOL on)
{
  ospfTraceDataChange = on;
}

/*********************************************************************
* @purpose  Saves Ospf user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfSave(void)
{
    L7_uint32 i=0;
    static const char *routine_name = "ospfSave()";
    ospfNetworkAreaInfo_t *p_NetworkArea = networkAreaListHead_g;

    OSPFMAP_TRACE("%s %d: %s : \n", __FILE__, __LINE__, routine_name);

    if (ospfMapDeregister.ospfMapSave == L7_TRUE)
        return L7_FAILURE;

    /*-------------------------------------------*/
    /* Save the "network area" operational data  */
    /* to the configuration                      */
    /*-------------------------------------------*/
    memset(pOspfMapCfgData->networkArea, 0, sizeof(pOspfMapCfgData->networkArea));
    while(p_NetworkArea != L7_NULLPTR)
    {
      pOspfMapCfgData->networkArea[i].ipAddress = p_NetworkArea->ipAddr;
      pOspfMapCfgData->networkArea[i].wildcardMask = p_NetworkArea->wildcardMask;
      pOspfMapCfgData->networkArea[i].areaId = p_NetworkArea->areaId;

      p_NetworkArea = p_NetworkArea->next;
      i++;
    }

    if (pOspfMapCfgData->cfgHdr.dataChanged == L7_TRUE)
    {
        pOspfMapCfgData->cfgHdr.dataChanged = L7_FALSE;
        pOspfMapCfgData->checkSum = nvStoreCrc32((L7_char8 *)pOspfMapCfgData,
                                                 sizeof(L7_ospfMapCfg_t) - sizeof(pOspfMapCfgData->checkSum));                      

        /* call save NVStore routine */        
        if (sysapiCfgFileWrite(L7_OSPF_MAP_COMPONENT_ID, L7_OSPF_CFG_FILENAME, 
                               (L7_char8 *)pOspfMapCfgData, 
                               sizeof (L7_ospfMapCfg_t)) == L7_ERROR)
        {
            OSPFMAP_ERROR("%s %d: %s: ERROR: Error on call to osapiFsWrite routine on config file %s\n",
                          __FILE__, __LINE__, routine_name, L7_OSPF_CFG_FILENAME);
        }
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Restore OSPF configuration to factory defaults. 
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE if the request could not be sent to the mapping thread.
*
* @notes    Sends event to the OSPF mapping thread for processing. 
*       
* @end
*********************************************************************/
L7_RC_t ospfRestore(void)
{
    L7_RC_t rc;
    ospfMapMsg_t msg;

    if (Ospf_Proc_Queue == L7_NULLPTR)
    {
        return L7_FAILURE;
    }

    memset( (void *)&msg, 0, sizeof(ospfMapMsg_t) );
    msg.msg = OSPF_RESTORE_EVENT;

    rc = osapiMessageSend( Ospf_Proc_Queue, &msg, sizeof(ospfMapMsg_t), 
                           L7_NO_WAIT, L7_MSG_PRIORITY_NORM );

    if (rc == L7_SUCCESS)
    {
        osapiSemaGive(ospfMsgQSema);
    }
    else
    {
        OSPFMAP_ERROR("ospfRestore: osapiMessageSend() failed\n");
    }

    return rc;
}

/*********************************************************************
* @purpose  Restores OSPF configuration to factory defaults. 
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ospfRestoreProcess(void)
{
    ospfResetCleanUp();

    if (ospfMapOspfInitialized() == L7_TRUE) {
        /* Uninitialize the OSPF protocol engine */
        ospfMapOspfVendorUninit();
        pOspfInfo->ospfInitialized = L7_FALSE;
    }

    ospfBuildDefaultConfigData(L7_OSPF_CFG_VER_CURRENT);
    ospfDataChangedSet(__FUNCTION__);

    /* reset "operational" version of this. */
    ospfMapNetworkAreaConfigRead();

    if (ospfMayEnableOspf())
    {
      ospfMapOspfEnable(L7_FALSE);
    }

    return L7_SUCCESS;  
}

/*********************************************************************
* @purpose  Initialize a new instance of ospfRedistCfg_t.  
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t ospfRedistCfgInit(ospfRedistCfg_t *redistCfg)
{   
    redistCfg->redistribute = FD_OSPF_REDISTRIBUTE;
    redistCfg->distList = FD_OSPF_DIST_LIST_OUT;
    redistCfg->redistMetric = FD_OSPF_REDIST_METRIC;
    redistCfg->metType = FD_OSPF_ASBR_EXT_ROUTE_DEFAULT_METRIC_TYPE;
    redistCfg->tag = FD_OSPF_ASBR_EXT_ROUTE_DEFAULT_ROUTE_TAG;
    redistCfg->subnets = FD_OSPF_REDIST_SUBNETS;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initialize a new instance of ospfDefRouteCfg_t.  
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t ospfDefaultRouteCfgInit(ospfDefRouteCfg_t *defRouteCfg)
{
    defRouteCfg->origDefRoute = FD_OSPF_ORIG_DEFAULT_ROUTE;
    defRouteCfg->always = FD_OSPF_ORIG_DEFAULT_ROUTE_ALWAYS;
    defRouteCfg->metric = FD_OSPF_ORIG_DEFAULT_ROUTE_METRIC;
    defRouteCfg->metType = FD_OSPF_ORIG_DEFAULT_ROUTE_TYPE;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build default ospf config data  
*
* @param    ver   @b{(input)} Software version of Config Data
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void ospfBuildDefaultConfigData(L7_uint32 ver)
{

    L7_uint32 cfgIndex; /* cfgIndex               */
    L7_uint32 vNbr;     /* virtual neighbor index */
    L7_uint32 i;        
    nimConfigID_t configId[L7_OSPF_INTF_MAX_COUNT];

    static const char *routine_name = "ospfBuildDefaultConfigData()";

    OSPFMAP_TRACE("%s %d: %s : \n", __FILE__, __LINE__, routine_name);

    for (cfgIndex = 1; cfgIndex < L7_OSPF_INTF_MAX_COUNT; cfgIndex++)
        NIM_CONFIG_ID_COPY(&configId[cfgIndex], &pOspfMapCfgData->ckt[cfgIndex]);

    memset(( void * )pOspfMapCfgData, 0, sizeof( L7_ospfMapCfg_t));

    /* Build Config File Header */
    strcpy(pOspfMapCfgData->cfgHdr.filename, L7_OSPF_CFG_FILENAME);
    pOspfMapCfgData->cfgHdr.version       = ver;
    pOspfMapCfgData->cfgHdr.componentID   = L7_OSPF_MAP_COMPONENT_ID;
    pOspfMapCfgData->cfgHdr.type          = L7_CFG_DATA;
    pOspfMapCfgData->cfgHdr.length        = sizeof(L7_ospfMapCfg_t);
    pOspfMapCfgData->cfgHdr.dataChanged   = L7_FALSE;


    /*---------------------------*/
    /* Build Default Config Data */
    /*---------------------------*/

    /* Generic OSPF Stack Cfg */
    pOspfMapCfgData->rtr.routerID             = FD_OSPF_DEFAULT_ROUTER_ID;
    pOspfMapCfgData->rtr.ospfAdminMode        = FD_OSPF_DEFAULT_ADMIN_MODE;
    pOspfMapCfgData->rtr.rfc1583Compatibility = FD_OSPF_DEFAULT_RFC_1583_COMPATIBILITY;
    pOspfMapCfgData->rtr.exitOverflowInterval = FD_OSPF_DEFAULT_EXIT_OVERFLOW_INTERVAL;
    pOspfMapCfgData->rtr.spfDelay             = FD_OSPF_DEFAULT_SPF_DELAY;
    pOspfMapCfgData->rtr.spfHoldtime          = FD_OSPF_DEFAULT_SPF_HOLDTIME;
    pOspfMapCfgData->rtr.extLSALimit          = FD_OSPF_DEFAULT_EXT_LSDB_LIMIT;
    pOspfMapCfgData->rtr.opaqueLSAMode        = FD_OSPF_DEFAULT_OPAQUE_LSA_MODE;
    pOspfMapCfgData->rtr.traceMode            = FD_OSPF_DEFAULT_TRACE_MODE;
    /* There really isn't a default value for defaultMetric because its value is
     * irrelevant until defMetConfigured is L7_TRUE, but this value
     * is as good as any. 
     */
    pOspfMapCfgData->rtr.defaultMetric        = FD_OSPF_DEFAULT_METRIC;
    pOspfMapCfgData->rtr.defMetConfigured     = L7_FALSE;
    pOspfMapCfgData->rtr.defaultPassiveMode   = FD_OSPF_DEFAULT_PASSIVE_MODE;
    pOspfMapCfgData->rtr.maxPaths             = FD_OSPF_MAXIMUM_PATHS;
    pOspfMapCfgData->rtr.autoCostRefBw        = FD_OSPF_DEFAULT_REFER_BANDWIDTH;
    pOspfMapCfgData->rtr.helperSupport        = FD_OSPF_HELPER_SUPPORT;
    pOspfMapCfgData->rtr.strictLsaChecking    = FD_OSPF_STRICT_LSA_CHECKING;
    pOspfMapCfgData->rtr.nsfSupport           = FD_OSPF_NSF_SUPPORT;
    pOspfMapCfgData->rtr.restartInterval      = FD_OSPF_RESTART_INTERVAL;
    pOspfMapCfgData->rtr.stubRtrMode          = FD_OSPF_STUB_ROUTER;

    /* Intentionally not using an FD value here. There is no default value. If 
     * stubRtrMode is OSPF_STUB_ROUTER_CFG_STARTUP, then user must configure 
     * the startup time. */
    pOspfMapCfgData->rtr.stubRtrStartupTime   = 0;
    pOspfMapCfgData->rtr.overrideSummaryMetric = FD_OSPF_STUB_ROUTER_OVERRIDE_SUMMARY_METRIC;
    pOspfMapCfgData->rtr.stubRtrSummaryMetric = FD_OSPF_STUB_ROUTER_SUMMARY_METRIC;

    /* OSPF Router Interface Configuration parameters */
    for (cfgIndex = 1; cfgIndex < L7_OSPF_INTF_MAX_COUNT; cfgIndex++)
        ospfIntfBuildDefaultConfigData(&configId[cfgIndex], &pOspfMapCfgData->ckt[cfgIndex]);

    /* OSPF Area Range Configuration parameters:
       Nothing to be done.  Zeroeing out the configuration is enough */

    /* OSPF Virtual Neighbor Configuration Parameters */
    for (vNbr = 0; vNbr < L7_OSPF_MAX_VIRT_NBRS; vNbr++)
    {
        /* These settings are the default values for virtual neighbor entries when created */
        pOspfMapCfgData->virtNbrs[vNbr].virtTransitAreaID = FD_OSPF_INTF_DEFAULT_VIRT_TRANSIT_AREA_ID;
        pOspfMapCfgData->virtNbrs[vNbr].virtIntfNeighbor = FD_OSPF_INTF_DEFAULT_VIRT_INTF_NEIGHBOR;
        pOspfMapCfgData->virtNbrs[vNbr].intIfNum = FD_OSPF_INTF_DEFAULT_VIRT_INTIFNUM;
        pOspfMapCfgData->virtNbrs[vNbr].helloInterval = FD_OSPF_INTF_DEFAULT_HELLO_INTERVAL;
        pOspfMapCfgData->virtNbrs[vNbr].deadInterval = FD_OSPF_INTF_DEFAULT_DEAD_INTERVAL;
        pOspfMapCfgData->virtNbrs[vNbr].rxmtInterval = FD_OSPF_INTF_DEFAULT_RXMT_INTERVAL;
        pOspfMapCfgData->virtNbrs[vNbr].ifTransitDelay = FD_OSPF_INTF_DEFAULT_IF_TRANSIT_DELAY;
        pOspfMapCfgData->virtNbrs[vNbr].authType = FD_OSPF_INTF_DEFAULT_AUTH_TYPE;
        memset((void *)pOspfMapCfgData->virtNbrs[vNbr].authKey, FD_OSPF_INTF_DEFAULT_AUTH_KEY_CHAR, (size_t)L7_AUTH_MAX_KEY_OSPF);  
        pOspfMapCfgData->virtNbrs[vNbr].authKeyLen = FD_OSPF_INTF_DEFAULT_AUTH_KEY_LEN;
        pOspfMapCfgData->virtNbrs[vNbr].authKeyId = FD_OSPF_INTF_DEFAULT_AUTH_KEY_ID;

    } /* vNbr < L7_OSPF_MAX_VIRT_NBRS */

    /* Defaults for route redistribution */
    for (i = 0; i < REDIST_RT_LAST; i++) {
        ospfRedistCfgInit(&pOspfMapCfgData->redistCfg[i]);
    }

    /* Defaults for default route origination */
    ospfDefaultRouteCfgInit(&pOspfMapCfgData->defRouteCfg);

    /* End of OSPF Component's Factory Defaults */
}

/*********************************************************************
* @purpose  Build default OSPF config data for an intf 
*
* @param    slot        slot number of the interface
* @param    port        port number of the interface
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void ospfIntfBuildDefaultConfigData(nimConfigID_t *configId, ospfCfgCkt_t *pCfg)
{
    L7_uint32 intIfNum;

    NIM_CONFIG_ID_COPY(&pCfg->configId, configId);
    pCfg->adminMode      = FD_OSPF_INTF_DEFAULT_ADMIN_MODE;
    pCfg->area           = FD_OSPF_INTF_DEFAULT_AREA;
    pCfg->advertiseSecondaries = FD_OSPF_INTF_DEFAULT_SECONDARIES_FLAG;
    pCfg->priority       = FD_OSPF_INTF_DEFAULT_PRIORITY;              
    pCfg->helloInterval  = FD_OSPF_INTF_DEFAULT_HELLO_INTERVAL;
    pCfg->deadInterval   = FD_OSPF_INTF_DEFAULT_DEAD_INTERVAL;         
    pCfg->rxmtInterval   = FD_OSPF_INTF_DEFAULT_RXMT_INTERVAL;         
    pCfg->nbmaPollInterval   = FD_OSPF_INTF_DEFAULT_NBMA_POLL_INTERVAL;    
    pCfg->ifTransitDelay     = FD_OSPF_INTF_DEFAULT_IF_TRANSIT_DELAY;      
    pCfg->lsaAckInterval     = FD_OSPF_INTF_DEFAULT_LSA_ACK_INTERVAL;      
    pCfg->authType           = FD_OSPF_INTF_DEFAULT_AUTH_TYPE;             
    memset(pCfg->authKey, FD_OSPF_INTF_DEFAULT_AUTH_KEY_CHAR, (size_t)L7_AUTH_MAX_KEY_OSPF);  
    pCfg->authKeyLen         = FD_OSPF_INTF_DEFAULT_AUTH_KEY_LEN;
    pCfg->authKeyId          = FD_OSPF_INTF_DEFAULT_AUTH_KEY_ID;
    pCfg->ifDemandMode       = FD_OSPF_INTF_DEFAULT_IF_DEMAND_MODE;
    pCfg->virtTransitAreaID  = FD_OSPF_INTF_DEFAULT_VIRT_TRANSIT_AREA_ID;
    pCfg->virtIntfNeighbor   = FD_OSPF_INTF_DEFAULT_VIRT_INTF_NEIGHBOR;  
    /* by default, the metric will be computed based on port speed */
    pCfg->metric             = L7_OSPF_INTF_METRIC_NOT_CONFIGURED; 
    pCfg->mtuIgnore          = FD_OSPF_DEFAULT_MTU_IGNORE; 
    pCfg->maxIpMtu           = FD_IP_DEFAULT_IP_MTU;
    pCfg->passiveMode        =  pOspfMapCfgData->rtr.defaultPassiveMode;

    pCfg->ifType = ospfIfTypeFromNimIfType(configId->type);

    if(nimIntIfFromConfigIDGet(configId, &intIfNum) == L7_SUCCESS)
    {
      /* set the default operational data on the interface */
      pOspfIntfInfo[intIfNum].adminMode       = pCfg->adminMode;
      pOspfIntfInfo[intIfNum].areaId          = pCfg->area;
      pOspfIntfInfo[intIfNum].secondariesFlag = pCfg->advertiseSecondaries;
    }
}

/*********************************************************************
* @purpose  Map a NIM interface type to a default OSPF interface type.
*
* @param    nimIfType  NIM interface type
*
* @returns  OSPF interface type
*
* @notes    none
*
* @end
*********************************************************************/
static L7_uint32 ospfIfTypeFromNimIfType(L7_INTF_TYPES_t nimIfType)
{
  L7_uint32 ospfIfType;

  switch (nimIfType)
  {
    case L7_PHYSICAL_INTF:
    case L7_LOGICAL_VLAN_INTF:
      ospfIfType = L7_OSPF_INTF_BROADCAST;
      break;

    case L7_TUNNEL_INTF:
      ospfIfType = L7_OSPF_INTF_PTP;
      break;

    case L7_LOOPBACK_INTF:
      ospfIfType = L7_OSPF_INTF_TYPE_LOOPBACK;
      break;

    default:
      /* we hit the default case for every config index 
       * that's not a configured interface. */
      ospfIfType = FD_OSPF_INTF_DEFAULT_TYPE;
      break;
  }
  return ospfIfType;
}

/*********************************************************************
* @purpose  Return the default ospf interface type of the given interface
*
* @param    intIfNum  internal interface ID
* @param    pIfType   return value for interface type
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfDefaultIfType(L7_uint32 intIfNum, L7_uint32 *pIfType)
{
    L7_INTF_TYPES_t nimIfType;

    if (nimGetIntfType(intIfNum, &nimIfType) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      OSPFMAP_ERROR("%s %d: ERROR: invalid internal interface ID %d, %s\n",
                      __FILE__, __LINE__, intIfNum, ifName);
      return L7_FAILURE;
    }

    *pIfType = ospfIfTypeFromNimIfType(nimIfType);

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Applies ospf config data
*
* @param    void 
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t ospfApplyConfigData(void)
{
    L7_uint32 i;

    /* At startup, the OSPF config state is INIT_2 when this function is 
     * called, so don't call OSPFMAP_IS_READY().
     */

    if (ospfMapOspfInitialized() != L7_TRUE)
        return L7_SUCCESS;

    /* read the "network area" configuration on the interfaces */
    ospfMapNetworkAreaConfigRead();

    /*--------------------------------------*/
    /*     Configure General Routing Parms  */
    /*--------------------------------------*/
    ospfMapExtenRouterIDSet(pOspfMapCfgData->rtr.routerID);
    ospfMapRfc1583CompatibilityApply( pOspfMapCfgData->rtr.rfc1583Compatibility);
    ospfMapExtLsdbLimitApply(pOspfMapCfgData->rtr.extLSALimit);
    ospfMapExitOverflowIntervalApply(pOspfMapCfgData->rtr.exitOverflowInterval);
    ospfMapMaxPathsApply(pOspfMapCfgData->rtr.maxPaths);
    ospfMapAutoCostRefBwApply();
    (void) ospfMapExtenOspfOpaqueCapabilitySet(pOspfMapCfgData->rtr.opaqueLSAMode);
    ospfMapStubRtrSummaryMetricApply(pOspfMapCfgData->rtr.overrideSummaryMetric,
                                     pOspfMapCfgData->rtr.stubRtrSummaryMetric);
    ospfMapStubRtrModeApply(pOspfMapCfgData->rtr.stubRtrMode, 
                          pOspfMapCfgData->rtr.stubRtrStartupTime);
    
    if (pOspfMapCfgData->rtr.spfDelay != FD_OSPF_DEFAULT_SPF_DELAY)
    {
      ospfMapSpfDelayApply(pOspfMapCfgData->rtr.spfDelay);
    }
    if (pOspfMapCfgData->rtr.spfHoldtime != FD_OSPF_DEFAULT_SPF_HOLDTIME)
    {
      ospfMapSpfHoldtimeApply(pOspfMapCfgData->rtr.spfHoldtime);
    }

    /*********************************************************************************
    *                                                                                *
    *  Configure Areas:                                                              *
    *  Area configuration is read and applied when interfaces are enabled (via       *
    *  ospfMapAreaConfigure() call). This ensures that the core OSPF implementation  *
    *  is ready to receive the area configuration commands.                          *
    *                                                                                *
    *  For ordered access to the list of area IDs stored in the configuration file,  *
    *  initialize the area IDs in the MIB table.                                     *
    *********************************************************************************/
    for (i=0;  i < L7_OSPF_MAX_AREAS; i++)
    {
        if ( pOspfMapCfgData->area[i].inUse == L7_TRUE)
        {
            if (ospfMapAreaIdMibTblInsert((L7_IP_ADDR_t)pOspfMapCfgData->area[i].area) 
                != L7_SUCCESS)
            {
                OSPFMAP_ERROR("%s %d: %s: ERROR: Inserting area %x in the MIB table\n",
                              __FILE__, __LINE__, __FUNCTION__, pOspfMapCfgData->area[i].area);
            }
        }
    } /*  i < L7_MAX_OSPF_AREAS */             

    /* area range data automatically applied when areas applied */        


    /* If ASBR, register with RTO */
    if (ospfMapIsAsbr())
      ospfMapAsbrStatusApply(L7_ENABLE);

    /* Originate default route if configured to do so */
    ospfMapDefaultRouteApply();

    ospfApplyVirtualLinkConfigData();

#ifdef L7_NSF_PACKAGE
    ospfGrConfigDataApply();
#endif

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Applies ospf virtual link config data
*
* @param    void 
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t ospfApplyVirtualLinkConfigData(void)
{
    L7_uint32 i;

    /* Virtual link configuration */
    for (i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
    {
        if ((pOspfVlinkInfo[i].vlinkCreated == L7_FALSE) && 
            (pOspfMapCfgData->virtNbrs[i].virtTransitAreaID != L7_NULL_IP_ADDR) &&
            (pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor != L7_NULL_IP_ADDR))
        {
           if (ospfMapOspfVirtIntfCreate(i, pOspfMapCfgData->virtNbrs[i].virtTransitAreaID,
                                              pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor) == L7_SUCCESS)
           {
               pOspfVlinkInfo[i].areaId = pOspfMapCfgData->virtNbrs[i].virtTransitAreaID;
               pOspfVlinkInfo[i].neighbor = pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor;
               pOspfVlinkInfo[i].vlinkCreated = L7_TRUE;
           }
        }
    } /* i < L7_OSPF_MAX_VIRT_NBRS */

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clean up OSPF as part of restoring to default configuration.
*
* @param    void 
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @notes    This routine performs a partial clean up of the OSPF stack.   
*           It removes optional configuration parameters such as areas,
*           aggregate ranges, stub area configuration, etc.
*
* @notes    Deletes all OSPF interfaces.
*           Cleanup occurs even if OSPF protocol engine has not been initialized.
*           Does not uninitialize OSPF protocol engine.
*
* @end
*********************************************************************/
L7_RC_t ospfResetCleanUp(void)
{
    L7_uint32 i;
    L7_RC_t rc;
    L7_uint32 intIfNum;

    static const char *routine_name = "ospfResetCleanUp()";

    OSPFMAP_TRACE("%s %d: %s : \n", __FILE__, __LINE__, routine_name);

    /*-------------------------------------------*/
    /* Clean up area range configuration, if any */
    /*-------------------------------------------*/
    for (i=0; i < L7_OSPF_MAX_AREA_RANGES; i++)
    {
        if (pOspfMapCfgData->areaRange[i].inUse == L7_TRUE)
        {
            ospfMapAreaAggregateRangeDelete (pOspfMapCfgData->areaRange[i].area,
                                             pOspfMapCfgData->areaRange[i].lsdbType,
                                             pOspfMapCfgData->areaRange[i].ipAddr,
                                             pOspfMapCfgData->areaRange[i].subnetMask);
        }
    }  /*    for (i=0; i < L7_OSPF_MAX_AREA_RANGES; i++) */


    /*-------------------------------------*/
    /* Clean up area configuration, if any */
    /*-------------------------------------*/
    for (i=0; i < L7_OSPF_MAX_AREAS; i++)
    {
        if (pOspfMapCfgData->area[i].inUse == L7_TRUE)
        {
            ospfMapAreaFree(pOspfMapCfgData->area[i].area);
        }
    }  /*    for (i=0; i < L7_OSPF_MAX_AREAS; i++) */

    /* delete all OSPF interfaces */
    if (nimPhaseStatusCheck() == L7_TRUE) 
    {
        rc = nimFirstValidIntfNumber(&intIfNum);   
        while (rc == L7_SUCCESS)
        {
            if (ospfMapOspfIntfInitialized(intIfNum))
            {
                ospfMapOspfIntfDelete(intIfNum);
            }
            rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
        }  
    }

    /*---------------------------------------*/
    /*  Clean up virtual link configuration  */
    /*---------------------------------------*/
    for (i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
    {
        /* Free all configured virtual links */
        if (pOspfVlinkInfo[i].vlinkCreated == L7_TRUE)
        {
            ospfMapVirtIfDelete(pOspfMapCfgData->virtNbrs[i].virtTransitAreaID,
                                pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor);
        }
    } /* i < L7_OSPF_MAX_VIRT_NBRS */

    /* Deregister with RTO if OSPF was ASBR */
    if (ospfMapIsAsbr())
      ospfMapAsbrStatusApply(L7_DISABLE);

    return ospfMapOspfAdminModeDisable();
}

/*********************************************************************
* @purpose  Initialize the OSPF layer
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes      
*       
* @end
*********************************************************************/
L7_RC_t ospfMapAppsInit(void)
{
  /* Initialize the OSPF protocol engine */
  if (ospfMapOspfVendorInit(pOspfInfo->p3InitTime) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_OSPF_MAP_COMPONENT_ID,
            "Failed to initialize OSPFv2 protocol engine");
    L7_assert(1);
    return L7_FAILURE;
  }
  pOspfInfo->ospfInitialized = L7_TRUE;
  pOspfInfo->originatingDefaultRoute = L7_FALSE;
  pOspfInfo->defaultRouteExists = L7_FALSE;

  /* Apply OSPF configuration data. */
  ospfApplyConfigData();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Callback function to process ACL changes. OSPF is interested in
*           ACL changes if it is configured to use an ACL to filter routes
*           for redistribution.
*
* @param    aclNum ACL number of ACL that has changed
* @param    event  add, modify, or delete ACL
*
* @returns  void
*
* @notes    Transfer to OSPF MAP thread. 
*       
* @end
*********************************************************************/
void ospfMapAclCallback(L7_uint32 aclNum, aclRouteFilter_t event)
{
  ospfMapMsg_t msg;
  L7_RC_t rc;

  /* send the event to the OSPF mapping thread for processing */
  if (Ospf_Proc_Queue == L7_NULLPTR)
  {
    return;
  }

  memset((void*) &msg, 0, sizeof(ospfMapMsg_t));
  msg.msg = OSPF_ACL_EVENT;
  msg.type.aclEvent.aclNum = aclNum;
  msg.type.aclEvent.event = event;

  rc = osapiMessageSend(Ospf_Proc_Queue, &msg, sizeof(ospfMapMsg_t),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

  if (rc == L7_SUCCESS)
  {
    osapiSemaGive(ospfMsgQSema);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
           "Failed to send ACL change message to OSPF mapping thread.");
  }
}

/*********************************************************************
* @purpose  Process an ACL change. OSPF is interested in
*           ACL changes if it is configured to use an ACL to filter routes
*           for redistribution.
*
* @param    aclNum ACL number of ACL that has changed
* @param    event  add, modify, or delete ACL
*
* @returns  void
*
* @notes    For now, we reevaluate all best routes, regardless of the 
*           type of event. 
*       
* @end
*********************************************************************/
void ospfMapAclChangeProcess(L7_uint32 aclNum, aclRouteFilter_t event)
{
    /* See if this ACL is used to filter routes from any source. */
    L7_uint32 i;
    L7_BOOL change = L7_FALSE;

  for (i = REDIST_RT_FIRST + 1; i < REDIST_RT_LAST; i++)
  {
    if (i == REDIST_RT_OSPF)
    {
            continue;
        }
        if (pOspfMapCfgData->redistCfg[i].redistribute && 
        (pOspfMapCfgData->redistCfg[i].distList == aclNum))
    {

            change = L7_TRUE;
            break;
        }
    }
  if (change)
  {
        ospfMapExtenRedistReevaluate();
    }
}

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to policy interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL ospfMapMapIntfIsConfigurable(L7_uint32 intIfNum, ospfCfgCkt_t **pCfg)
{
    L7_uint32 index;
    nimConfigID_t configId;

    if (ospfMapReadyForIntfEvents() == L7_FALSE)
        return L7_FALSE;

    /* Check boundary conditions */
    if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
        return L7_FALSE;

    index = ospfMapMapTbl[intIfNum];

    if (index == 0)
        return L7_FALSE;

    /* verify that the configId in the config data table entry matches the 
     ** configId that NIM maps to the intIfNum we are considering
     */
    if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
    {
        if (NIM_CONFIG_ID_IS_EQUAL(&configId, &(pOspfMapCfgData->ckt[index].configId)) == L7_FALSE)
        {
            L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
            nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

            /* if we get here, either we have a table management error between 
            ** ospfMapCfg and ospfMapMapTbl or there is synchronization issue 
            ** between NIM and components w.r.t. interface creation/deletion.
            */
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
                    "Error accessing OSPFMAP config data for interface %d, %s in ospfMapMapIntfIsConfigurable.\n", intIfNum, ifName);
            return L7_FALSE;
        }
    }

    *pCfg = &pOspfMapCfgData->ckt[index];

    return L7_TRUE;
}

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to policy interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL ospfMapMapIntfConfigEntryGet(L7_uint32 intIfNum, ospfCfgCkt_t **pCfg)
{
    L7_uint32 i;
    nimConfigID_t configIdNull;
    static L7_uint32 nextIndex = 1;

    memset(&configIdNull, 0, sizeof(nimConfigID_t));

    if (ospfMapReadyForIntfEvents() == L7_FALSE)
        return L7_FALSE;

    if (nextIndex < L7_OSPF_INTF_MAX_COUNT)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&pOspfMapCfgData->ckt[nextIndex].configId, &configIdNull))
      {
          ospfMapMapTbl[intIfNum] = nextIndex;
          *pCfg = &pOspfMapCfgData->ckt[nextIndex];
          nextIndex++;
          return L7_TRUE;
      }
    }

    for (i = 1; i < L7_OSPF_INTF_MAX_COUNT; i++)
    {
        if (NIM_CONFIG_ID_IS_EQUAL(&pOspfMapCfgData->ckt[i].configId, &configIdNull))
        {
            ospfMapMapTbl[intIfNum] = i;
            *pCfg = &pOspfMapCfgData->ckt[i];
            nextIndex = i + 1;
            return L7_TRUE;
        }
    }
    return L7_FALSE;
}


/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t ospfMapIntfCreate(L7_uint32 intIfNum) 
{
    nimConfigID_t configId;
    ospfCfgCkt_t *pCfg = L7_NULL;

    if (ospfMapReadyForIntfEvents() == L7_FALSE)
        return L7_FAILURE;
    if (ospfMapIsValidIntf(intIfNum) != L7_TRUE)
        return L7_FAILURE;
    if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
        return L7_FAILURE;

    if (ospfMapMapTbl[intIfNum] != 0)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
              "Interface structure already exists for interface %s when L7_RTR_INTF_CREATE event received",
              ifName);
      return L7_SUCCESS;
    }

    /* If an interface configuration entry is not already assigned to the interface,
       assign one */
    if (ospfMapMapIntfConfigEntryGet(intIfNum, &pCfg) != L7_TRUE)
      return L7_FAILURE;

    /* Update the configuration structure with the config id */
    if (pCfg == L7_NULL)
    {
      return L7_FAILURE;
    }

    ospfIntfBuildDefaultConfigData(&configId, pCfg);
    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_DELETE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t ospfMapIntfDelete(L7_uint32 intIfNum) 
{
    ospfCfgCkt_t *pCfg;

    if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
    {
        memset((void *)&pCfg->configId, 0, sizeof(nimConfigID_t));
        memset((void *)&ospfMapMapTbl[intIfNum], 0, sizeof(L7_uint32));
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This is the callback function OSPF registers with IP MAP
*           for notification of routing events.   
*
* @param    intIfnum @b{(input)}  internal interface number
* @param    event    @b{(input)}  an event listed in L7_RTR_EVENT_CHANGE_t
* @param    pData    @b{(input)}  unused
* @param    response @b{(input)}  provides parameters used to acknowledge
*                                 processing of the event
*
* @returns  L7_SUCCESS or L7_FALIURE
*       
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapRoutingEventChangeCallBack (L7_uint32 intIfNum, 
                                           L7_uint32 event,
                                           void *pData, 
                                           ASYNC_EVENT_NOTIFY_INFO_t *response)
{
    ospfMapMsg_t msg;
    L7_RC_t rc;

    static const char *routine_name = "ospfMapRoutingEventChangeCallBack()";
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    OSPFMAP_TRACE("%s %d: %s : intf = %d, %s, state = %d \n", __FILE__,
                  __LINE__, routine_name, intIfNum, ifName, event);

    /* send the event to the OSPF mapping thread for processing */
    if (Ospf_Proc_Queue == L7_NULLPTR) {
        return L7_FAILURE;
    }

    memset((void*) &msg, 0, sizeof(ospfMapMsg_t));
    msg.msg                       = OSPF_RTR_EVENT;
    msg.type.rtrEvent.intIfNum    = intIfNum;
    msg.type.rtrEvent.event       = event;
    msg.type.rtrEvent.pData       = pData;  
    if (response != L7_NULLPTR)
    {
        memcpy(&(msg.type.rtrEvent.response), response, 
               sizeof(ASYNC_EVENT_NOTIFY_INFO_t) );
    }

    rc = osapiMessageSend(Ospf_Proc_Queue, &msg, sizeof(ospfMapMsg_t),
                          L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

    if (rc == L7_SUCCESS) 
    {
        osapiSemaGive(ospfMsgQSema);
    }
    else
    {
        OSPFMAP_ERROR("Failed to send message to OSPF mapping thread in %s\n",
                      routine_name);
    }
    return rc;
}

/*********************************************************************
* @purpose  Process routing events.
*
* @param    intIfnum @b{(input)}  internal interface number
* @param    event    @b{(input)}  an event in L7_RTR_EVENT_CHANGE_t
* @param    pData    @b{(input)}  unused
* @param    response @b{(input)}  provides parameters used to acknowledge
*                                 processing of the event
*
* @returns  L7_SUCCESS or L7_FAILURE
*       
* @notes    This function is called on the OSPF mapping thread.
*           We are required to send to IP MAP an asynchronous response 
*           saying that OSPF has completely processed the event. We 
*           always send that response from this function. When this
*           function completes, we are guaranteed that all processing
*           of the event has been done both on the mapping thread and
*           on the protocol engine thread. This is because the mapping
*           thread blocks while the protocol engine thread does any
*           work required of it.
*       
* @end
*********************************************************************/
static L7_RC_t ospfMapRoutingEventProcess (L7_uint32 intIfNum, 
                                           L7_uint32 event,
                                           void *pData, 
                                           ASYNC_EVENT_NOTIFY_INFO_t *response)
{
  L7_uint32 rtrIfNum;
  L7_uint32 ipMtu;
  L7_uint32 ifMetric;

  /* whether OSPF is enabled */
  L7_uint32 ospfAdminMode;
  L7_BOOL gracefulRestart = L7_FALSE;

  ospfCfgCkt_t *pCfg;

  /* used to send acknowledgement data back to IP MAP */
  ASYNC_EVENT_COMPLETE_INFO_t event_completion;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
  memset(&event_completion, 0, sizeof( ASYNC_EVENT_COMPLETE_INFO_t) );

  OSPFMAP_TRACE("%s %d: %s : intf = %d, %s, event = %d\n",
                __FILE__, __LINE__, "ospfMapRoutingEventProcess", 
                intIfNum, ifName, event);

  /* For a number of events we need to know whether OSPF is enabled. 
   * In these cases, if OSPF is disabled, we take no action here. The 
   * change will be applied when OSPF becomes enabled. 
   */
  ospfMapOspfAdminModeGet(&ospfAdminMode);

  switch (event)
  {
    case L7_RTR_ENABLE: 
      /* If the administrator has enabled OSPF and if system startup is done, 
       * enable OSPF. */
      if (ospfMayEnableOspf())
      {
        ospfMapOspfEnable(L7_FALSE);
      }
      break;


    case L7_RTR_DISABLE_PENDING:
      if (ospfAdminMode == L7_ENABLE)
      {
        ospfMapOspfAdminModeDisable();
      }
      break;

    case L7_RTR_DISABLE:
      break;   /* did the work in L7_RTR_DISABLE_PENDING */

    case L7_RTR_INTF_CREATE:
      ospfMapIntfCreate(intIfNum);
      break;

    case L7_RTR_INTF_ENABLE: 
      ospfMapIntfModeAndAreaCompute(intIfNum);
      if (ospfMayEnableInterface(intIfNum) == L7_TRUE)
      {
        ospfMapIntfAdminModeEnable(intIfNum);
      }
      break;

    case L7_RTR_INTF_DELETE:
      ospfMapIntfDelete(intIfNum);
      break;

    case L7_RTR_INTF_DISABLE_PENDING:
      if (ospfAdminMode == L7_ENABLE)
      {
        if (ospfMapOspfIntfExists(intIfNum) == L7_TRUE)
        {
          if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
          {
            ospfMapRoutingIntfDisabled(intIfNum);
          }
        }
      }
      break;

    case L7_RTR_INTF_DISABLE:
      /* Nothing to do. We cleaned up on the disable pending */
      break;

    case L7_RTR_INTF_SECONDARY_IP_ADDR_ADD:
    case L7_RTR_INTF_SECONDARY_IP_ADDR_DELETE:
      if (ospfAdminMode == L7_ENABLE)
        ospfMapSecondaryIpAddrChangeCallback(intIfNum);
      break;


    case L7_RTR_INTF_MTU:
      if (ipMapIntfEffectiveIpMtuGet(intIfNum, &ipMtu) == L7_SUCCESS)
      {
        if (ospfMapIntfIpMtuSet(intIfNum, ipMtu) != L7_SUCCESS)
        {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
                  "OSFPv2 failed to set IPv4 MTU of %u on interface %s.", 
                  ipMtu, ifName);
        }
      }
      break;

    case L7_RTR_INTF_BW_CHANGE:
      /* Based on the "bandwidth" command on an interface */
      ospfMapIntfBandwidthApply(intIfNum);
      break;

    case L7_RTR_INTF_SPEED_CHANGE:
      /* Based on actual interface speed */
      if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
      {
        /* We need to apply a speed change only if the user has NOT specifically 
           configured a speed via the UI.  This can be ascertained by checking if
           the stored configuration is == L7_OSPF_INTF_METRIC_NOT_CONFIGURED */
        if (pCfg->metric == L7_OSPF_INTF_METRIC_NOT_CONFIGURED)
        {
          if (ospfMapIntfMetricCalculate(intIfNum, &ifMetric) == L7_SUCCESS)
          {
            (void) ospfMapIntfMetricApply(intIfNum, ifMetric);
          }
        }
      }
      break;

    case L7_RTR_STARTUP_DONE:
#ifdef L7_NSF_PACKAGE
      gracefulRestart = pOspfInfo->lastSystemRestartWarm && !o2CkptTableEmpty();
#endif
      pOspfInfo->startupDone = L7_TRUE;   /* set before enabling OSPF */
      if (ospfMayEnableOspf())
      {
        ospfMapOspfEnable(gracefulRestart);
      }
      else
      {
        ipMapInitialRoutesDone(NSF_PROTOCOL_OSPF);
      }
      break;

    default:
      /* No action necessary for events OSPF doesn't care about. */
      break;
  }

  if ((response != L7_NULLPTR) && 
      (response->handlerId != 0) && 
      (response->correlator != 0))
  {
    /* acknowledge OSPF's processing of the event */
    /* Purposely overloading componentID with L7_IPMAP_REGISTRANTS_t */
    event_completion.componentId = L7_IPRT_OSPF;
    event_completion.handlerId = response->handlerId;
    event_completion.correlator = response->correlator;
    event_completion.async_rc.rc = L7_SUCCESS;
    event_completion.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;    
    asyncEventCompleteTally(&event_completion);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This routine is called when a routing interface is disabled
*
* @param    intIfnum    internal interface number
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t ospfMapRoutingIntfDisabled(L7_uint32 intIfNum)
{
    L7_uint32 rtrIfNum, mode;

    if (ospfMapOspfInitialized() == L7_FALSE)
        return L7_SUCCESS;

    /* Perform ospf operations only if ospf is enabled on the switch */
    if (ospfMapOspfAdminModeGet(&mode) == L7_SUCCESS &&
        mode == L7_ENABLE)
    {
        /* set the "effective admin mode" to disable, to help in the
         * area/mode computation, when the ospfMapIntfModeAndAreaCompute()
         * is called later on address addition
         */
        ospfMapIntfEffectiveAdminModeSet(intIfNum, L7_DISABLE);

        /* Determine the router interface for the specified internal interface */
        if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
        {
            /* Delete the interface if it exists */
            if (ospfMapOspfIntfInitialized(intIfNum) == L7_TRUE)
            {
                /* Set to interface to "down" state to cause a interface state transition */
                ospfMapOspfIntfDown(intIfNum);

                if (ospfMapOspfIntfDelete(intIfNum) == L7_SUCCESS)
                {
                    return L7_SUCCESS;
                }
            }
        }
    }

    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Issues a secondary IP Address change notification to the
*           OSPF protocol
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapSecondaryIpAddrChangeCallback(L7_uint32 intIfNum)
{
  L7_uint32 rtrIfNum, cfgAdminMode;

  /*
  Do nothing if the ospf interface has not been previously created.
  The secondary address becomes relevant only if it is configured
  on an active OSPF interface
  */

  if (ospfMayEnableInterface(intIfNum) != L7_TRUE)
  {
    return L7_SUCCESS;
  }

  /* Verify if the interface is configured for routing */

  if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    return L7_SUCCESS;
  }

  if (ospfMapOspfIntfExists(intIfNum) == L7_FALSE)
  {
    return L7_SUCCESS;
  }

  /*
  If we get here this is an OSPF routing interface and we need to
  notify the vendor code of the changed secondary network
  configuration so that the appropriate advertisements are sent
  out
  */
  if (ospfMapIntfAdminModeGet(intIfNum, &cfgAdminMode) == L7_SUCCESS)
  {
    if (cfgAdminMode == L7_DISABLE)
    {
      /* If we get here and the config admin mode is disabled,
       * it means the effective admin mode is enabled due to a
       * matching "network area" commmand.
       * So, call ospfMapIntfModeAndAreaCompute() on this interface
       * which would find any matching networks for the newly
       * added secondary address
       */
      return ospfMapIntfModeAndAreaCompute(intIfNum);
    }
    else
    {
      /* If we get here, call ospfMapExtenIfStatusSet() directly */
      return ospfMapExtenIfStatusSet(intIfNum, L7_OSPF_ROW_CHANGE);
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check for replicated OSPF multicast control packets
*
* @param    hookId      @b{(input)} hook location
* @param    bufHandle   @b{(input)} frame buffer handle
* @param    *pduInfo    @b{(input)} ptr to frame info
* @param    contFuncPtr @b{(input)} optional continuation function ptr
*
* @returns  SYSNET_PDU_RC_DISCARD frame to be discarded (stop further processing)
* @returns  SYSNET_PDU_RC_IGNORED frame has been ignored (continue processing it)
*
* @comments The sockets interface is designed to present a received multicast
*           packet to all sockets registered for that multicast address, 
*           regardless of which interface actually received it.  Since each 
*           OSPF routing interface registers for the 224.0.0.5 and 224.0.0.6
*           multicast addresses, all sockets process every such received packet
*           as if it came in on their own interface.  This can confuse the
*           OSPF protocol.  Without a suitable socket option to prevent this,
*           this intercept is used to discard any OSPF multicast packet 
*           received from an interface whose subnet does not match the 
*           source IP subnet before it reaches the socket.
*       
* @end
*********************************************************************/
SYSNET_PDU_RC_t ospfMapMcastIntercept(L7_uint32 hookId,
                                      L7_netBufHandle bufHandle,
                                      sysnet_pdu_info_t *pduInfo,
                                      L7_FUNCPTR_t contFuncPtr)
{
    L7_uchar8     *data;
    L7_ipHeader_t *ipHdr;
    L7_uint32     ifIpAddr, ifNetMask;
	L7_uint32     tmpDst;

    SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
    ipHdr = (L7_ipHeader_t *)(data + sysNetDataOffsetGet(data));

    /* must be an OSPF protocol packet destined to either the "all OSPF routers"
     * or "all OSPF designated routers" multicast IP address
     */
	tmpDst = osapiNtohl(ipHdr->iph_dst);
    if ( (ipHdr->iph_prot == IP_PROT_OSPFIGP) &&
         ((tmpDst == L7_IP_ALL_OSPF_RTRS_ADDR) ||
          (tmpDst == L7_IP_ALL_OSPF_DESIG_RTRS_ADDR)) )
    {
        /* discard packet if sender belongs to different subnet than received
         * interface subnet
         */
    if (ipMapRtrIntfIpAddressGet(pduInfo->intIfNum, &ifIpAddr, 
                                 &ifNetMask) != L7_SUCCESS)
    {
      return SYSNET_PDU_RC_DISCARD;
    }

    if ((ifIpAddr != L7_NULL_IP_ADDR) && (ifNetMask != L7_NULL_IP_MASK))
        {
            if (((ifIpAddr ^ osapiNtohl(ipHdr->iph_src)) & ifNetMask) != 0)
                return SYSNET_PDU_RC_DISCARD;
        }
  }


    return SYSNET_PDU_RC_IGNORED;
}

/*********************************************************************
* @purpose  This function should be called whenever there is a configuration
*           change that affects the metric of the default route 
*           originated by this router.
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
L7_RC_t ospfMapDefaultRouteMetricChange()
{
    ospfMapDefaultRouteApply();
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply changes to the configuration for origination of 
*           a default route.
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
L7_RC_t ospfMapDefaultRouteApply()
{
    L7_BOOL alwaysOriginate = pOspfMapCfgData->defRouteCfg.always;

    if (ospfMapOspfInitialized() != L7_TRUE)
    {
        return L7_SUCCESS;
    }
    if (pOspfMapCfgData->defRouteCfg.origDefRoute && 
        (alwaysOriginate || ospfMapDefaultRouteExists())) {
        return ospfMapExtenDefRouteOrig();
    }
    else {
        return ospfMapExtenDefRouteNoOrig();
    }
}

/*********************************************************************
* @purpose  Returns true if the forwarding table contains a default route.
*
* @param    none 
*
* @returns  L7_TRUE if default route exists
* @returns  L7_FALSE otherwise
*
* @comments 
*       
* @end
*********************************************************************/
L7_BOOL ospfMapDefaultRouteExists()
{
    return pOspfInfo->defaultRouteExists;
}

/*********************************************************************
* @purpose  The OSPF mapping thread uses this function to 
*           process messages from the message queue, performing the tasks
*           specified in each message. The initial motivation for this 
*           task is to offload initialization processing from the 
*           configurator.
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ospfProcTask(void)
{
    L7_uint32 status;
    ospfMapMsg_t Message;

    /* Loop forever, processing incoming messages */
    while (L7_TRUE)
    {
        /* Since we are reading from multiple queues, we cannot wait forever 
         * on the message receive from each queue. Rather than sleep between
         * queue reads, use a semaphore to indicate whether any queue has
         * data. Wait until data is available. */
        if (osapiSemaTake(ospfMsgQSema, L7_WAIT_FOREVER) != L7_SUCCESS)
        {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
                   "Failure taking OSPF message queue semaphore.");
            continue;
        }
        memset(&Message, 0x00, sizeof(ospfMapMsg_t));

        /* Always try to read from the main OSPF queue first. This queue contains 
         * all events except redistribution events. */
        status = osapiMessageReceive(Ospf_Proc_Queue, &Message,
                                     sizeof(ospfMapMsg_t), L7_NO_WAIT);

        if (status != L7_SUCCESS)
        {
            /* If the OSPF queue was empty, see if a redistribution event is
             * in the redistribution queue. */
            status = osapiMessageReceive(Ospf_Redist_Queue, &Message,
                                         sizeof(ospfMapMsg_t), L7_NO_WAIT);
        }

        if (status == L7_SUCCESS)
        {
          /* Configuration actions are all done on the UI thread. 
           * Synchronize that processing with event processing by 
           * taking the same semaphore here. */
          switch (Message.msg)
            {
            case OSPF_RTR_EVENT:
                osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 
                ospfMapRoutingEventProcess(Message.type.rtrEvent.intIfNum,
                                           Message.type.rtrEvent.event,
                                           Message.type.rtrEvent.pData,
                                           &Message.type.rtrEvent.response);
                osapiSemaGive(ospfMapCtrl_g.cfgSema);
                break;
            case OSPF_CNFGR_INIT:
                osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 
                ospfCnfgrHandle(&Message.type.cmdData); 
                osapiSemaGive(ospfMapCtrl_g.cfgSema);
                break;

            case OSPF_RESTORE_EVENT:
                osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 
                ospfRestoreProcess();
                osapiSemaGive(ospfMapCtrl_g.cfgSema);
                break;

            case OSPF_RTO_EVENT:
            {
                L7_int32 redistQLen;
                osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 
                if (osapiMsgQueueGetNumMsgs(Ospf_Redist_Queue, &redistQLen) == L7_SUCCESS) 
                {
                    if (redistQLen > redistQHighWaterMark)
                        redistQHighWaterMark = redistQLen;                   
                }   
                ospfMapBestRoutesGet();
                osapiSemaGive(ospfMapCtrl_g.cfgSema);
            }      
            break;

            case OSPF_ACL_EVENT:
                osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 
                ospfMapAclChangeProcess(Message.type.aclEvent.aclNum,
                                        Message.type.aclEvent.event);
                osapiSemaGive(ospfMapCtrl_g.cfgSema);
                break;

            default:
                L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
                       "Unknown message type received in the OSPF processing task.");
                break;
            }  
        }
        else /* status != L7_SUCCESS */
        {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID, 
                    "Bad status from OSPF message queue read: %d.", status);     
        }
    } /* end while true */

    return;
}

void ospfMapQueueInfoShow(void)
{
    L7_uint32 procQLen = 0;
    L7_uint32 redistQLen = 0;

    if (osapiMsgQueueGetNumMsgs(Ospf_Proc_Queue, &procQLen) == L7_SUCCESS)
    {
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                      "\tOSPF processing queue current count:  %d\n",
                      procQLen);
    }

    if (osapiMsgQueueGetNumMsgs(Ospf_Redist_Queue, &redistQLen) == L7_SUCCESS)
    {
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                      "\tOSPF redist queue current count:  %d\n",
                      redistQLen);
    }
}

void ospfRedistQLenShow(void)
{
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                  "\n\tOSPF redistribution queue high water mark:  %d (limit: %d)\n",
                  redistQHighWaterMark, OSPF_REDIST_MSG_COUNT);
}

/*********************************************************************
* @purpose  Process OSPF interface admin mode events
*
* @param    intIfNum    internal interface number
* @param    mode        L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ospfMapIntfAdminModeProcess(L7_uint32 intIfNum, L7_uint32 mode)
{
  ospfMapIntfModeAndAreaCompute(intIfNum);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Saves ospfMap debug configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ospfMapDebugCfg is the overlay
*       
* @end
*********************************************************************/
L7_RC_t ospfMapDebugSave(void)
{
  L7_RC_t rc;

  rc = L7_SUCCESS;

  if (ospfMapDebugHasDataChanged() == L7_TRUE)
  {
    /* Copy the operational states into the config file */
    ospfMapDebugCfgUpdate();

    /* Store the config file */
    ospfMapDebugCfg.hdr.dataChanged = L7_FALSE;
    ospfMapDebugCfg.checkSum = nvStoreCrc32((L7_uchar8 *)&ospfMapDebugCfg,
                                      (L7_uint32)(sizeof(ospfMapDebugCfg) - sizeof(ospfMapDebugCfg.checkSum)));
        /* call save NVStore routine */
    if ((rc = sysapiSupportCfgFileWrite(L7_OSPF_MAP_COMPONENT_ID, OSPFMAP_DEBUG_CFG_FILENAME , 
                            (L7_char8 *)&ospfMapDebugCfg, (L7_uint32)sizeof(ospfMapDebugCfg_t))) == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
              "Error on call to sysapiSupportCfgFileWrite routine on config file %s\n",OSPFMAP_DEBUG_CFG_FILENAME);
    }
  }

  return(rc);
}

/*********************************************************************
* @purpose  Restores ospfMap debug configuration
*
* @param    void     
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ospfMapDebugCfg is the overlay
*       
* @end
*********************************************************************/
L7_RC_t ospfMapDebugRestore(void)
{
  L7_RC_t rc;
  ospfMapDebugBuildDefaultConfigData(OSPFMAP_DEBUG_CFG_VER_CURRENT);

  ospfMapDebugCfg.hdr.dataChanged = L7_TRUE;

  rc = ospfMapApplyDebugConfigData();

  return rc;
}

/*********************************************************************
* @purpose  Checks if ospfMap debug config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL ospfMapDebugHasDataChanged(void)
{
  return ospfMapDebugCfg.hdr.dataChanged;
}

/*********************************************************************
* @purpose  Build default ospf config data  
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void ospfMapDebugBuildDefaultConfigData(L7_uint32 ver)
{
  
  /* setup file header */
  ospfMapDebugCfg.hdr.version = ver;
  ospfMapDebugCfg.hdr.componentID = L7_OSPF_MAP_COMPONENT_ID;
  ospfMapDebugCfg.hdr.type = L7_CFG_DATA;
  ospfMapDebugCfg.hdr.length = (L7_uint32)sizeof(ospfMapDebugCfg);
  strcpy((L7_char8 *)ospfMapDebugCfg.hdr.filename, OSPFMAP_DEBUG_CFG_FILENAME);
  ospfMapDebugCfg.hdr.dataChanged = L7_FALSE;

  /* set all flags to L7_FALSE */
  memset(&ospfMapDebugCfg.cfg, 0, sizeof(ospfMapDebugCfg.cfg));
}                     

/*********************************************************************
* @purpose  Apply ospf debug config data  
*
* @param    void
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    Called after a default config is built
*
* @end
*********************************************************************/
L7_RC_t ospfMapApplyDebugConfigData(void)
{
  L7_RC_t rc;

  rc = ospfMapDebugPacketTraceFlagSet(ospfMapDebugCfg.cfg.ospfMapDebugPacketTraceFlag);
  
  return rc;
}
