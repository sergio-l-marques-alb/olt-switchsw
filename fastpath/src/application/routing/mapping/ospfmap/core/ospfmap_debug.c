/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  ospfmap_debug.c
*
* @purpose   OSPF Mapping Debug functions
*
* @component OSPF Mapping Layer
*
* @comments  none
*
* @create    06/21/2001
*
* @author    wjacobs
*
* @end
*
**********************************************************************/

#include <stdio.h>

#include "l7_ospfinclude.h"
#include "l7_ospf_api.h"
#include "ospf_cnfgr.h"
#include "ospf_vend_ctrl.h"
#include "os_xxcmn.h"
#include "osapi_support.h"
#include "bspapi.h"
#include "usmdb_util_api.h"

extern L7_int32            L7_ospf_task_id;
extern L7_ospfMapCfg_t    *pOspfMapCfgData;
extern ospfInfo_t         *pOspfInfo;
extern ospfAreaInfo_t     *pOspfAreaInfo;
extern ospfIntfInfo_t     *pOspfIntfInfo;
extern ospfMapCnfgrState_t ospfMapCnfgrState;
extern ospfNetworkAreaInfo_t  *networkAreaListHead_g;
extern ospfNetworkAreaInfo_t  *networkAreaListFree_g;
extern struct ospfMapCtrl_s  ospfMapCtrl_g;

/* Debug functions */
extern L7_RC_t usmDbInetAton(L7_uchar8 *ipAddr, L7_uint32 *val);
extern t_IFO * ifoPtrGet(L7_uint32 intIfNum);
extern L7_RC_t ifoAuthKeyCfgGet(L7_uint32 intIfNum, t_OspfAuthKey *p_Auth);
extern L7_RC_t virtIfAuthKeyCfgGet(t_Handle IFO_Id, t_OspfAuthKey *p_Auth);
extern void XX_DisplayQueue(t_Handle threadID, ulng queueId, ulng resolveNames, long maxDepth);
extern void osapiDebugStackTrace (L7_uint32 task_id, FILE *fp);


static void ospfRedistCfgBuildTestConfigData(ospfRedistCfg_t *redistCfg, L7_uint32 seed);
static void ospfDefaultRouteCfgBuildTestConfigData(ospfDefRouteCfg_t *defRouteCfg);
static void ospfAreaStubCfgDataShow(void);
static void ospfAreaNssaCfgDataShow(void);
static void ospfAreaNssaBuildTestConfigData(void);
static void ospfAreaRangeBuildTestConfigData(void);
static void ospfDefRouteCfgBuildTestConfigData(void);
static void ospfIntfBuildTestConfigData(ospfCfgCkt_t *pCfg);
void ospfConfigDataTestShow(void);
void ospfBuildTestConfigData(void);

L7_uchar8 *ospfMsgQueueNames[OSPF_NUM_QUEUES] = {"Event Queue", "Data Queue", 
  "ACK Queue", "Hello Queue"};

/*********************************************************************
* @purpose  Enable linking of OSPF Mapping layer debug functions
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ospfMapTraceLinkerAssist(void)
{
  L7_uint32 debug;

 ospfMapTraceModeSet(L7_ENABLE);
 debug = ospfMapTraceModeGet();
}

/*********************************************************************
* @purpose  Set OSPF Mapping Layer tracing mode
*
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void ospfMapTraceModeSet(L7_uint32 mode)
{
  pOspfMapCfgData->rtr.traceMode = mode;
  pOspfMapCfgData->cfgHdr.dataChanged = L7_TRUE;
}

/*********************************************************************
* @purpose  Get IP Mapping Layer tracing mode
*
* @param    void
*
* @param    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 ospfMapTraceModeGet(void)
{
  return(pOspfMapCfgData->rtr.traceMode);
}

/*********************************************************************
* @purpose  debug function to dump display the ospf vendor route table
*           on the screen
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ospfConfigStateShow(void)
{
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                  "\n\tOSPF configuration state:  %s\n",
                  ospfCfgPhaseNames[ospfMapCnfgrState]);
}

/*********************************************************************
* @purpose  debug function to dump display the ospf vendor route table
*           on the screen
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ospfNsfStateShow(void)
{
  printf("\nBackup unit elected:  %s",
         pOspfInfo->backupElected ? "true" : "false");
  printf("\nLast system restart was %s", 
         pOspfInfo->lastSystemRestartWarm ? "WARM" : "COLD");
  printf("\nIP MAP has%sannounced that startup is done",
         pOspfInfo->startupDone ? " " : " not ");
}

/*********************************************************************
* @purpose  Print the configuration for OSPF's origination of a default route.
*
* @param    void
*
* @notes
*
* @end
*********************************************************************/
void ospfMapDefaultOrigCfgDataShow()
{
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                   "\n\nConfiguration for Default Route Origination");
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                   "\n\nOriginate Default Route.....................%s",
                   pOspfMapCfgData->defRouteCfg.origDefRoute ? "true" : "false");
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                   "\nAlways......................................%s",
                   pOspfMapCfgData->defRouteCfg.always ? "true" : "false");
    if (pOspfMapCfgData->defRouteCfg.metric != FD_OSPF_ORIG_DEFAULT_ROUTE_METRIC) {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                       "\nMetric for Default Route....................%d",
                       pOspfMapCfgData->defRouteCfg.metric);
    }
    else {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                       "\nMetric for Default Route....................not configured");
    }
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                   "\nMetric Type for Default Route...............%s",
                   (pOspfMapCfgData->defRouteCfg.metType == L7_OSPF_METRIC_EXT_TYPE1) ?
                   "Type 1" : "Type 2");
}


/*********************************************************************
* @purpose  Print the route redistribution configuration for OSPF.
*
* @param    void
*
* @notes
*
* @end
*********************************************************************/
void ospfMapRedistCfgDataShow()
{
    L7_uint32 r;

    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                   "\n\nConfiguration for Route Redistribution");

    for (r = REDIST_RT_FIRST + 1; r < REDIST_RT_LAST; r++) {
        char *sourceName;
        if (r == REDIST_RT_OSPF) {
            continue;
        }
        switch (r)
        {
            case REDIST_RT_LOCAL: sourceName = "Connected"; break;
            case REDIST_RT_STATIC: sourceName = "Static"; break;
            case REDIST_RT_RIP: sourceName = "RIP"; break;
            case REDIST_RT_BGP: sourceName = "BGP"; break;
            default: sourceName = "Unknown";
        }

        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                       "\n\nRoute Source................................%s",
                       sourceName);
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                       "\nRedistribute................................%s",
                       pOspfMapCfgData->redistCfg[r].redistribute ? "true" : "false");
        if (pOspfMapCfgData->redistCfg[r].distList != FD_OSPF_DIST_LIST_OUT) {
            SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                           "\nDistribute List.............................%lu",
                           pOspfMapCfgData->redistCfg[r].distList);
        }
        else {
            SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                           "\nDistribute List.............................not configured");
        }
        if (pOspfMapCfgData->redistCfg[r].redistMetric != FD_OSPF_REDIST_METRIC) {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                       "\nRedistribution Metric.......................%d",
                       pOspfMapCfgData->redistCfg[r].redistMetric);
        }
        else {
            SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                           "\nRedistribution Metric.......................not configured");
        }
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                       "\nMetric Type.................................%s",
                       (pOspfMapCfgData->redistCfg[r].metType == L7_OSPF_METRIC_EXT_TYPE1) ?
                       "Type 1" : "Type 2");
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                       "\nTag.........................................%lu",
                       pOspfMapCfgData->redistCfg[r].tag);
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                       "\nRedistribute Subnets?.......................%s",
                       pOspfMapCfgData->redistCfg[r].subnets ? "true" : "false");
    }
}
/*********************************************************************
* @purpose  Dumps the ospf configuration information onto the
*           display screen.
*
* @param    void
*
* @notes
*
* @end
*********************************************************************/
void ospfMapCfgDataShow(void)
{
  L7_uchar8 debug_buf[30], c = 'n';
  L7_uint32 i, j, cfgIndex;
  nimUSP_t usp;
  ospfCfgCkt_t *pCfg;
  L7_uint32 intIfNum;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nDo you wish to see all interfaces? ");
  osapiFileRead(bspapiConsoleFdGet(), &c, sizeof(c));

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n+--------------------------------------------------------+");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n|              OSPF CONFIGURATION DATA                   |");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n+--------------------------------------------------------+");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\nHeader Information:");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nfilename...............................%s", pOspfMapCfgData->cfgHdr.filename);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nversion................................%d", pOspfMapCfgData->cfgHdr.version);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\ncomponentID............................%d", pOspfMapCfgData->cfgHdr.componentID);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\ntype...................................%d", pOspfMapCfgData->cfgHdr.type);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nlength.................................%d", pOspfMapCfgData->cfgHdr.length);

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\nOSPF Router Information:");
  
  osapiInetNtoa(pOspfMapCfgData->rtr.routerID, debug_buf);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_IPMAP,  "\nrouterID...............................%-15.15s", debug_buf);
  
  if (pOspfMapCfgData->rtr.ospfAdminMode == L7_ENABLE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nospfAdminMode..........................enable");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nospfAdminMode..........................disable");
  }

  if (pOspfMapCfgData->rtr.rfc1583Compatibility == L7_ENABLE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nrfc1583Compatibility...................enable");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nrfc1583Compatibility...................disable");
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nextLSALimit............................%d",
                 pOspfMapCfgData->rtr.extLSALimit);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nexitOverflowInterval...................%d",
                 pOspfMapCfgData->rtr.exitOverflowInterval);

  if (pOspfMapCfgData->rtr.opaqueLSAMode == L7_ENABLE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nopaqueLSAMode..........................enable");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nopaqueLSAMode..........................disable");
  }

  if (pOspfMapCfgData->rtr.traceMode == L7_ENABLE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\ntraceMode..............................enable");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\ntraceMode..............................disable");
  }

  if (pOspfMapCfgData->rtr.defaultPassiveMode == L7_TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nDefault Passive Status.................enable");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nDefault Passive Status.................disable");
  }

  if (pOspfMapCfgData->rtr.defMetConfigured) {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                     "\ndefaultMetric..........................%lu",
                     pOspfMapCfgData->rtr.defaultMetric);
  }
  else {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                     "\ndefaultMetric..........................not configured");
  }
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP, "\nmaximum paths..............................%u", 
                pOspfMapCfgData->rtr.maxPaths);

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\nOSPF Routing Interfaces:");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n-------------------------------------------------------------------------");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "---------------------------------------------------------------------------");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nslot. admin  area       pri-  hello    dead     rxmt     if     ");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  lsa      auth  auth  if     virt        virt    ifType  auth");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nport   mode   id        ority interval interval interval transmit");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  ack      type  keyid demand transmit     transmit         key");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n                                                                  delay  ");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  interval    mode   areaid      neighbor           ");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n-------------------------------------------------------------------------");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "---------------------------------------------------------------------------");

  for (cfgIndex = 1; cfgIndex < L7_OSPF_INTF_MAX_COUNT; cfgIndex++)
  {
    pCfg = &pOspfMapCfgData->ckt[cfgIndex];

    memset((void *)&usp, 0, sizeof(nimUSP_t));
    if (nimUspFromConfigIDGet(&pCfg->configId, &usp) != L7_SUCCESS)
      continue;
    if (nimGetIntIfNumFromUSP(&usp, &intIfNum) != L7_SUCCESS)
        continue;
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    if ((pCfg->adminMode == L7_ENABLE) || (c == 'y') || (c == 'Y'))
    {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n%s", ifName);
      if (pCfg->adminMode == L7_ENABLE)
      {
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "    E");
        }
        else
        {
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "    D");
        }
      osapiInetNtoa(pCfg->area, debug_buf);
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "    %-10.10s", debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  %d", pCfg->priority);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "       %d", pCfg->helloInterval);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "       %d", pCfg->deadInterval);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "       %d", pCfg->rxmtInterval);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "       %d", pCfg->ifTransitDelay);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "       %d", pCfg->lsaAckInterval);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "       %d", pCfg->authKeyLen);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "       %d", pCfg->metric);

      switch (pCfg->authType)
      {
        case L7_AUTH_TYPE_NONE:
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "     none");
          break;
        case L7_AUTH_TYPE_SIMPLE_PASSWORD:
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "     simple");
          break;
        case L7_AUTH_TYPE_MD5:
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "     md5");
          break;
        default:
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "     none");
          break;
        }

      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  %3u", (L7_uchar8)pCfg->authKeyId);

      if (pCfg->ifDemandMode == L7_ENABLE)
      {
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "    E");
        }
        else
        {
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "    D");
        }

      osapiInetNtoa(pCfg->virtTransitAreaID, debug_buf);
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "   %-10.10s", debug_buf);
      osapiInetNtoa(pCfg->virtIntfNeighbor, debug_buf);
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "   %-10.10s", debug_buf);

      switch (pCfg->ifType)
      {
        case L7_OSPF_INTF_BROADCAST:
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "     broadcast");
          break;
        case L7_OSPF_INTF_NBMA:
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "     nbma");
          break;
        case L7_OSPF_INTF_PTP:
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "     point to point");
          break;
        case L7_OSPF_INTF_VIRTUAL:
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "     virtual");
          break;
        case L7_OSPF_INTF_POINTTOMULTIPOINT:
          break;
        case L7_OSPF_INTF_TYPE_LOOPBACK:
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "     loopback");
          break;
        default:
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "     none");
          break;
        }

      memcpy(debug_buf, pCfg->authKey, (size_t)L7_AUTH_MAX_KEY_OSPF);
        *(debug_buf+L7_AUTH_MAX_KEY_OSPF) = L7_EOS;
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  %-16s", debug_buf);

      }
    }


  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nAdditional intf config:");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n");
  for (cfgIndex = 1; cfgIndex < L7_OSPF_INTF_MAX_COUNT; cfgIndex++)
  {
    pCfg = &pOspfMapCfgData->ckt[cfgIndex];

    memset((void *)&usp, 0, sizeof(nimUSP_t));
    if (nimUspFromConfigIDGet(&pCfg->configId, &usp) != L7_SUCCESS)
      continue;
    if (nimGetIntIfNumFromUSP(&usp, &intIfNum) != L7_SUCCESS)
        continue;


    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n usp = %d/%d/%d", usp.unit, usp.slot, usp.port );
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  intIfNum = %d", intIfNum );
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  mtuIgnore = %d", pCfg->mtuIgnore);
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  Passive status = %s",
                  ((pCfg->passiveMode == L7_OSPF_INTF_PASSIVE) ? "\"Passive\"" : "\"Not Passive\""));

    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  maxIpMtu = %d", pCfg->maxIpMtu);
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  authKeyLen = %d", pCfg->authKeyLen);
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  metric = %d", pCfg->metric);
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n");
  }
    


  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n-------------------------------------------------------------------------");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "--------------------------------------------------");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nKey:");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nE: Enable\tD: Disable");

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\nOSPF Area Information:");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n-----------------------------------------------");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\narea      extern      summary   aging      auth");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nid        routing               interval   type");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n          capability                           ");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n-----------------------------------------------");

  for (i = 0; i < L7_OSPF_MAX_AREAS; i++)
  {
    if (pOspfMapCfgData->area[i].inUse == L7_TRUE)
    {
      osapiInetNtoa(pOspfMapCfgData->area[i].area, debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n%-15.15s", debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\t%d", pOspfMapCfgData->area[i].extRoutingCapability);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\t%d", pOspfMapCfgData->area[i].summary);
    }
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\nOSPF Stub Area Information:");

  for (i = 0; i < L7_OSPF_MAX_AREAS; i++)
  {
    if (pOspfMapCfgData->area[i].inUse == L7_TRUE)
    {
      osapiInetNtoa(pOspfMapCfgData->area[i].area, debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n%-15.15s", debug_buf);

      for ( j = 0; j < L7_RTR_MAX_TOS_INDICES; j++)
      {
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\tStub Metric = %d", pOspfMapCfgData->area[i].stubCfg[j].stubMetric);
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\tStub Metric Type = %d", pOspfMapCfgData->area[i].stubCfg[j].stubMetricType);
      }
      
    }
  }


  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\nOSPF NSSA Information:");

  for (i = 0; i < L7_OSPF_MAX_AREAS; i++)
  {
    if (pOspfMapCfgData->area[i].inUse == L7_TRUE)
    {
      osapiInetNtoa(pOspfMapCfgData->area[i].area, debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n%-15.15s", debug_buf);

     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\ndefInfoOrig = %d", pOspfMapCfgData->area[i].nssaCfg.defInfoOrig);
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\ndefMetric = %d", pOspfMapCfgData->area[i].nssaCfg.defMetric);
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\ndefMetricType = %d", pOspfMapCfgData->area[i].nssaCfg.defMetricType);
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nimportSummaries = %d", pOspfMapCfgData->area[i].nssaCfg.importSummaries);
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nredistribute = %d", pOspfMapCfgData->area[i].nssaCfg.redistribute);
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\ntranslatorRole = %d", pOspfMapCfgData->area[i].nssaCfg.translatorRole);
     SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\ntranslatorStabilityInterval = %d", pOspfMapCfgData->area[i].nssaCfg.translatorStabilityInterval);
      
    }
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n----------------------------------------");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nKey:");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nextRoutingCapability:-\nL7_OSPF_AREA_IMPORT_EXT: 1\nL7_OSPF_AREA_IMPORT_NO_EXT: 2");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nL7_OSPF_AREA_IMPORT_NSSA: 3");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nsummary:-\nL7_OSPF_AREA_NO_SUMMARY: 1\nL7_OSPF_AREA_SEND_SUMMARY: 2");

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\nOSPF Area Range Information:");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n--------------------------------------------------");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\narea     lsdb   ip         subnet        advertise");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nid       type   addr       mask                   ");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n--------------------------------------------------");

  for (i = 0; i < L7_OSPF_MAX_AREA_RANGES; i++)
  {
    if (pOspfMapCfgData->areaRange[i].inUse == L7_TRUE)
    {
      osapiInetNtoa(pOspfMapCfgData->areaRange[i].area, debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n%-15.15s", debug_buf);

      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\t%d", pOspfMapCfgData->areaRange[i].lsdbType);

      osapiInetNtoa(pOspfMapCfgData->areaRange[i].ipAddr, debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  %-15.15s", debug_buf);

      osapiInetNtoa(pOspfMapCfgData->areaRange[i].subnetMask, debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\t%-15.15s", debug_buf);
      if (pOspfMapCfgData->areaRange[i].advertise == L7_TRUE)
      {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\ttrue");
      }
      else
      {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\tfalse");
      }
    }
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n--------------------------------------------------");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nKey:");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nlsdbType:-\nL7_OSPF_AREA_AGGREGATE_LSDBTYPE_SUMMARYLINK: 3");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nL7_OSPF_AREA_AGGREGATE_LSDBTYPE_NSSAEXTERNALLINK: 7");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n");

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\nOSPF Virtual Neighbor Information:");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n----------------------------------------------------------------");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "---------------------------------");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\narea             neighbor         intf  hello  dead  rxmt  trans");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  auth   auth  auth              ");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nid               router id        num   ival   ival  ival  delay");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  type   keyid key               ");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n----------------------------------------------------------------");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "---------------------------------");

  for (i = 0; i < L7_OSPF_MAX_VIRT_NBRS; i++)
  {
    osapiInetNtoa(pOspfMapCfgData->virtNbrs[i].virtTransitAreaID, debug_buf);
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n%-15.15s", debug_buf);

    osapiInetNtoa(pOspfMapCfgData->virtNbrs[i].virtIntfNeighbor, debug_buf);
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  %-15.15s", debug_buf);

    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  %4u", pOspfMapCfgData->virtNbrs[i].intIfNum);

    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  %4u ", pOspfMapCfgData->virtNbrs[i].helloInterval);
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  %4u", pOspfMapCfgData->virtNbrs[i].deadInterval);
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  %4u", pOspfMapCfgData->virtNbrs[i].rxmtInterval);
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  %4u ", pOspfMapCfgData->virtNbrs[i].ifTransitDelay);

    switch (pOspfMapCfgData->virtNbrs[i].authType)
    {
    case L7_AUTH_TYPE_NONE:
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  none  ");
      break;
    case L7_AUTH_TYPE_SIMPLE_PASSWORD:
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  simple");
      break;
    case L7_AUTH_TYPE_MD5:
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  md5   ");
      break;
    default:
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  ????  ");
      break;
    }

    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  %3u", (L7_uchar8)pOspfMapCfgData->virtNbrs[i].authKeyId);

    memcpy(debug_buf, pOspfMapCfgData->virtNbrs[i].authKey, (size_t)L7_AUTH_MAX_KEY_OSPF);
    *(debug_buf+L7_AUTH_MAX_KEY_OSPF) = L7_EOS;
    if (strlen(debug_buf) > 0)
    {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  \"%s\"", debug_buf);
    }
  }

  /* Route redistribution configuration for each source of routes */
  ospfMapRedistCfgDataShow();

  /* Default metric origination configuration */
  ospfMapDefaultOrigCfgDataShow();

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\npOspfInfo structure:");
  if (pOspfInfo->ospfInitialized == L7_TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tospfInitialized........true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tospfInitialized........false");
  }
  /* print the OSPF configuration state */
  ospfConfigStateShow();

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\toriginating default route........%s",
                 pOspfInfo->originatingDefaultRoute ? "true" : "false");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tdefault route exists.............%s",
                 pOspfInfo->defaultRouteExists ? "true" : "false");

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\npOspfAreaInfo structure:");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n---------------------------");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nareaId      ospfInitialized");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n---------------------------");
  for (i = 0; i < L7_OSPF_MAX_AREAS; i++)
  {
    osapiInetNtoa(pOspfAreaInfo[i].areaId, debug_buf);
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n%-15.15s", debug_buf);
    if (pOspfAreaInfo[i].ospfInitialized == L7_TRUE)
    {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\ttrue");
    }
    else
    {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\tfalse");
    }
  }
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n---------------------------\n");

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\npOspfIntfInfo structure:");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n-----------------------------");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                 "\nintIfNum   ospfInitialized   intfAcquired    ospfIfExistss");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n-----------------------------");
  for (i = 1; i < (L7_OSPF_INTF_MAX_COUNT + 1); i++)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                   "\n%u\t\t%s\t\t%s\t\t%s",
                   i,
                   (pOspfIntfInfo[i].ospfInitialized ? "true" : "false"),
                   (pOspfIntfInfo[i].intfAcquired ? "true" : "false"),
                   (pOspfIntfInfo[i].ospfIfExists ? "true" : "false"));
  }
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n-----------------------------\n");


  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,"Scaling Constants\n");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,"-----------------\n");
  

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,"L7_OSPF_INTF_MAX_COUNT - %d\n", L7_OSPF_INTF_MAX_COUNT);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,"L7_OSPF_MAX_AREAS - %d\n", L7_OSPF_MAX_AREAS);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,"L7_OSPF_MAX_AREA_RANGES - %d\n", L7_OSPF_MAX_AREA_RANGES);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,"L7_OSPF_MAX_VIRT_NBRS - %d\n", L7_OSPF_MAX_VIRT_NBRS);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,"REDIST_RT_LAST - %d\n", REDIST_RT_LAST);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,"L7_AUTH_MAX_KEY_OSPF - %d\n", L7_AUTH_MAX_KEY_OSPF);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,"L7_RTR_MAX_TOS_INDICES - %d\n", L7_RTR_MAX_TOS_INDICES);

}

/*********************************************************************
* @purpose  debug function to dump the specified IFO structure onto
*           the screen
*
* @param    slot          @b{(input)} interface specific slot
* @param    port          @b{(input)} interface specific port
* @param    virtFlag      @b{(input)} flag to display virtual IFO instead
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ospfMapExtenIFOShow(L7_uint32 unit, L7_uint32 slot, L7_uint32 port, 
                         L7_BOOL virtFlag)
{
  nimUSP_t      usp;
  L7_uint32     intIfNum, i, j;
  t_IFO_Cfg     ifoCfg;
  t_IFO         *p_IFO = NULL;
  L7_uchar8     debug_buf[30], authKey[OSPF_MAP_EXTEN_AUTH_MAX_KEY+1];
  t_OspfAuthKey authKeyCfg;
  L7_RC_t       rc;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

  usp.unit = unit;
  usp.slot = slot;
  usp.port = port;

  if (nimGetIntIfNumFromUSP(&usp, &intIfNum) == L7_SUCCESS)
  {
    if ((p_IFO = ifoPtrGet(intIfNum)) != NULL)
    {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP, "\n+--------------------------------------------------------+");

      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      if (virtFlag != L7_TRUE)
      {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                       "\n|              IFO DATA FOR INTERFACE %s              |", ifName);
      }
      else
      {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                       "\n|          VIRTUAL IFO DATA FOR INTERFACE %s          |", ifName);
      }
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP, "\n+--------------------------------------------------------+");
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP, "\nnext IFO pointer...........................%p", p_IFO->next);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP, "\nprevious IFO pointer.......................%p", p_IFO->prev);
      
      if (p_IFO->Taken == TRUE)
      {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nTaken......................................true");
      }
      else
      {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nTaken......................................false");
      }
      osapiInetNtoa((L7_uint32)p_IFO->IfoId, debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nIfoId......................................0x%x  (%-15.15s)", (L7_uint32)p_IFO->IfoId, debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nLowLayerId.................................0x%x", (L7_uint32)p_IFO->LowLayerId);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nARO_Id.....................................0x%x", (L7_uint32)p_IFO->ARO_Id);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nRTO_Id.....................................0x%x", (L7_uint32)p_IFO->RTO_Id);

      memset(&ifoCfg, 0, sizeof(t_IFO_Cfg));
      memcpy(&ifoCfg, &p_IFO->Cfg, sizeof(t_IFO_Cfg));
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\nIFO config Data:");

      osapiInetNtoa((L7_uint32)ifoCfg.IpAdr, debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tIpAdr........................%-15.15s", debug_buf);
	  for (j=1; j < IFO_MAX_IP_ADDRS; j++)
	  {
		  if (ifoCfg.addrs[j].ipAddr == 0)
		  {
			  continue;
		  }
		  osapiInetNtoa((L7_uint32)ifoCfg.addrs[j].ipAddr, debug_buf);
		  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\taddrs[%02d]....................%-15.15s", j, debug_buf);
	  }

      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tIfIndex......................%u", (L7_uint32)ifoCfg.IfIndex);

      osapiInetNtoa((L7_uint32)ifoCfg.IpMask, debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tIpMask.......................%-15.15s", debug_buf);

      osapiInetNtoa((L7_uint32)ifoCfg.AreaId, debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tAreaId.......................%-15.15s", debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tVpnCos.......................%u", (L7_uint32)ifoCfg.VpnCos);
      if (ifoCfg.IsVpnSupported == TRUE)
      {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tisVpnSupported...............true");
      }
      else
      {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tisVpnSupported...............false");
      }

      switch (ifoCfg.PhyType)
      {
      case OSPF_ETHERNET:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tPhyType......................Ethernet");
        break;
      case OSPF_FRAME_RELAY:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tPhyType......................FrameRelay");
        break;
      case OSPF_ATM:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tPhyType......................ATM");
        break;
      default:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tPhyType......................unknown");
        break;
      }

      osapiInetNtoa((L7_uint32)ifoCfg.VirtTransitAreaId, debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tVirtTransitAreaId............%-15.15s", debug_buf);

      osapiInetNtoa((L7_uint32)ifoCfg.VirtIfNeighbor, debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tVirtIfNeighbor...............%-15.15s", debug_buf);
      switch (ifoCfg.Type)
      {
      case IFO_ILLEGAL_TYPE:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tType.........................illegal");
        break;
      case IFO_BRDC:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tType.........................broadcast");
        break;
      case IFO_NBMA:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tType.........................nbma");
        break;
      case IFO_PTP:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tType.........................pointToPoint");
        break;
      case IFO_VRTL:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tType.........................virtual");
        break;
      case IFO_PTM:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tType.........................pointToMultipoint");
        break;
      case IFO_LOOPBACK:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tType.........................loopback");
        break;
      default:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tType.........................unknown");
        break;
      }
      if (ifoCfg.AdminStat == TRUE)
      {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tAdminStat....................true");
      }
      else
      {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tAdminStat....................false");
      }
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRouterPriority...............%u", (L7_uint32)ifoCfg.RouterPriority);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tInfTransDelay................%u", (L7_uint32)ifoCfg.InfTransDelay);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRxmtInterval.................%u", (L7_uint32)ifoCfg.RxmtInterval);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tHelloInterval................%u", (L7_uint32)ifoCfg.HelloInterval);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRouterDeadInterval...........%u", (L7_uint32)ifoCfg.RouterDeadInterval);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tNbmaPollInterval.............%u", (L7_uint32)ifoCfg.NbmaPollInterval);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tAckInterval..................%u", (L7_uint32)ifoCfg.AckInterval);
      switch (ifoCfg.State)
      {
      case IFO_DOWN:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tState........................down");
        break;
      case IFO_LOOP:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tState........................loopback");
        break;
      case IFO_WAIT:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tState........................waiting");
        break;
      case IFO_PTPST:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tState........................pointToPoint");
        break;
      case IFO_DR:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tState........................designatedRouter");
        break;
      case IFO_BACKUP:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tState........................backupDesignatedRouter");
        break;
      case IFO_DROTHER:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tState........................otherDesignatedRouter");
        break;
      default:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tState........................unknown");
        break;
      }

      osapiInetNtoa((L7_uint32)ifoCfg.DrIpAddr, debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tDrIpAddr.....................%-15.15s", debug_buf);

      osapiInetNtoa((L7_uint32)ifoCfg.BackupIpAddr, debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tBackupIpAddr.................%-15.15s", debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tEventsCounter................%u", (L7_uint32)ifoCfg.EventsCounter);
      switch (ifoCfg.AuType)
      {
      case IFO_NULL_AUTH:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tAuType.......................none");
        break;
      case IFO_PASSWORD:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tAuType.......................simplePassword");
        break;
      case IFO_CRYPT:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tAuType.......................md5");
        break;
      default:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tAuType.......................unknown");
        break;
      }
      sprintf(authKey,"%02X%02X%02X%02X%02X%02X%02X%02X",
             ifoCfg.AuKey[0],
             ifoCfg.AuKey[1],
             ifoCfg.AuKey[2],
             ifoCfg.AuKey[3],
             ifoCfg.AuKey[4],
             ifoCfg.AuKey[5],
             ifoCfg.AuKey[6],
             ifoCfg.AuKey[7]);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tAuKey (simple)...............%s", authKey);
      memcpy(authKey, ifoCfg.AuKey, (size_t)OSPF_MAP_EXTEN_AUTH_MAX_KEY_SIMPLE);
      *(authKey+OSPF_MAP_EXTEN_AUTH_MAX_KEY_SIMPLE) = L7_EOS;
      if (strlen(authKey) > 0)
      {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  \"%s\"", authKey);
      }

      switch (ifoCfg.IfStatus)
      {
      case ROW_ACTIVE:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tIfStatus.....................rowActive");
        break;
      case ROW_NOT_IN_SERVICE:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tIfStatus.....................rowNotInService");
        break;
      case ROW_NOT_READY:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tIfStatus.....................rowNotReady");
        break;
      case ROW_CREATE_AND_GO:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tIfStatus.....................rowCreateAndGo");
        break;
      case ROW_CREATE_AND_WAIT:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tIfStatus.....................rowCreateAndWait");
        break;
      case ROW_DESTROY:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tIfStatus.....................rowDestroy");
        break;
      case ROW_READ:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tIfStatus.....................rowRead");
        break;
      case ROW_READ_NEXT:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tIfStatus.....................rowReadNext");
        break;
      case ROW_CHANGE:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tIfStatus.....................rowChange");
        break;
      case ROW_MODIFY:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tIfStatus.....................rowModify");
        break;
      default:
        break;
      }
      if (ifoCfg.IfDemand == TRUE)
      {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tIfDemand.....................true");
      }
      else
      {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tIfDemand.....................false");
      }
      switch (ifoCfg.MulticastForwarding)
      {
      case IF_MLTCST_BLOCKED:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tMulticastForwarding..........blocked");
        break;
      case IF_MLTCST_MULTICAST:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tMulticastForwarding..........multicast");
        break;
      case IF_MLTCST_UNICAST:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tMulticastForwarding..........unicast");
        break;
      default:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tMulticastForwarding..........unknown");
        break;
      }
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tLinkLsaCount.................%u", (L7_uint32)ifoCfg.LinkLsaCount);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tLinkLsaCksum.................%u", (L7_uint32)ifoCfg.LinkLsaCksum);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tMaxIpMTUSize...................%u", (L7_uint32)ifoCfg.MaxIpMTUsize);
      if (ifoCfg.DisableFlooding == TRUE)
      {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tDisableFlooding..............true");
      }
      else
      {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tDisableFlooding..............false");
      }
#if L7_OSPF_TE
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tTeThreshold..................%u", (L7_uint32)ifoCfg.TeThreshold);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tTeMetric.....................%u", (L7_uint32)ifoCfg.TeMetric);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tTeMaxBandWidth...............%.2f", (double)ifoCfg.TeMaxBandWidth);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tTeMaxReserveBandWidth........%.2f", (double)ifoCfg.TeMaxReservBandWidth);
      switch (ifoCfg.LinkMuxCap)
      {
      case LKMUX_PSC_1:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tLinkMuxCap...................packetSwitchCapable-1");
        break;
      case LKMUX_PSC_2:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tLinkMuxCap...................packetSwitchCapable-2");
        break;
      case LKMUX_PSC_3:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tLinkMuxCap...................packetSwitchCapable-3");
        break;
      case LKMUX_PSC_4:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tLinkMuxCap...................packetSwitchCapable-4");
        break;
      case LKMUX_L2SC:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tLinkMuxCap...................Layer-2SwitchCapable");
        break;
      case LKMUX_TDM:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tLinkMuxCap...................TimeDivisionMultiplexCapable");
        break;
      case LKMUX_LSC:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tLinkMuxCap...................LambdaSwitchCapable");
        break;
      case LKMUX_FSC:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tLinkMuxCap...................FiberSwitchCapable");
        break;
      default:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tLinkMuxCap...................unknown");
        break;
      }
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tOutIfIndex...................%u", (L7_uint32)ifoCfg.OutIfIndex);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tIncIfIndex...................%u", (L7_uint32)ifoCfg.IncIfIndex);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tMaxLspBndw...................%.2f  %.2f  %.2f  %.2f  %.2f  %.2f  %.2f  %.2f",
             (double)ifoCfg.MaxLspBndw[0],
             (double)ifoCfg.MaxLspBndw[1],
             (double)ifoCfg.MaxLspBndw[2],
             (double)ifoCfg.MaxLspBndw[3],
             (double)ifoCfg.MaxLspBndw[4],
             (double)ifoCfg.MaxLspBndw[5],
             (double)ifoCfg.MaxLspBndw[6],
             (double)ifoCfg.MaxLspBndw[7]);

      switch (ifoCfg.ProtType)
      {
      case LKPROT_EXTRATRAFFIC:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tProtType.....................extra traffic");
        break;
      case LKPROT_UNPROTECT:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tProtType.....................unprotected");
        break;
      case LKPROT_SHARED:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tProtType.....................shared");
        break;
      case LKPROT_1_TO_1:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tProtType.....................dedicated 1:1");
        break;
      case LKPROT_1_PLUS_1:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tProtType.....................dedicated 1+1");
        break;
      case LKPROT_ENHANCED:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tProtType.....................enhanced");
        break;
      case LKPROT_SPARE1:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tProtType.....................reserved1");
        break;
      case LKPROT_SPARE2:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tProtType.....................reserved2");
        break;
      default:
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tProtType.....................unknown");
        break;
      }
      for (i = 0; i < 2; i++)
      {
        switch (ifoCfg.LinkDescr[i])
        {
        case LKDS_SONET_STAND:
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tLinkDescr(%d).................Standard SONET", i);
          break;
        case LKDS_SONET_ARBIT:
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tLinkDescr(%d).................Arbitrary SONET", i);
          break;
        case LKDS_SDH_STAND:
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tLinkDescr(%d).................Standard SDH", i);
          break;
        case LKDS_SDH_ARBIT:
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tLinkDescr(%d).................Arbitrary SDH", i);
          break;
        case LKDS_CLEAR:
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tLinkDescr(%d).................Clear", i);
          break;
        case LKDS_GIGE:
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tLinkDescr(%d).................GigE", i);
          break;
        case LKDS_10GIGE:
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tLinkDescr(%d).................10GigE", i);
          break;
        default:
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tLinkDescr(%d).................unknown", i);
          break;
        }
      }
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tLdPri........................%u  %u", (L7_uint32)ifoCfg.LdPri[0], (L7_uint32)ifoCfg.LdPri[1]);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tLdMinBndw....................%.2f  %.2f", (double)ifoCfg.LdMinBndw[0], (double)ifoCfg.LdMinBndw[1]);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tLdMaxBndw....................%.2f  %.2f", (double)ifoCfg.LdMaxBndw[0], (double)ifoCfg.LdMaxBndw[1]);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tSrlg.........................%u  %u  %u  %u",
             (L7_uint32)ifoCfg.Srlg[0],
             (L7_uint32)ifoCfg.Srlg[1],
             (L7_uint32)ifoCfg.Srlg[2],
             (L7_uint32)ifoCfg.Srlg[3]);

#endif /* L7_OSPF_TE */

      osapiInetNtoa((L7_uint32)p_IFO->DrId, debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\nDrId.......................................%-15.15s", debug_buf);

      osapiInetNtoa((L7_uint32)p_IFO->BackupId, debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nBackupId...................................%-15.15s", debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nTransitARO.................................0x%x", (L7_uint32)p_IFO->TransitARO);

      osapiInetNtoa((L7_uint32)p_IFO->VrtNgbIpAdr, debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nVrtNgbIpAdr................................%-15.15s", debug_buf);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nVirtIfIndex................................%u", (L7_uint32)p_IFO->VirtIfIndex);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nHelloTimer.................................0x%x", (L7_uint32)p_IFO->HelloTimer);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nWaitTimer..................................0x%x", (L7_uint32)p_IFO->WaitTimer);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nNboHl......................................0x%x", (L7_uint32)p_IFO->NboHl);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nLsAck......................................0x%x", (L7_uint32)p_IFO->LsAck);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nAckTimer...................................0x%x", (L7_uint32)p_IFO->AckTimer);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nMetricHl...................................0x%x", (L7_uint32)p_IFO->MetricHl);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nNbmaHl.....................................0x%x", (L7_uint32)p_IFO->NbmaHl);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nAuthKeyHl..................................0x%x", (L7_uint32)p_IFO->AuthKeyHl);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nTimeStamp..................................%u", (L7_uint32)p_IFO->TimeStamp);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nCfgMetric..................................%u", (L7_uint32)p_IFO->CfgMetric);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\nPacket Statistics:");
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRxPackets....................%u", (L7_uint32)p_IFO->PckStats.RxPackets);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tDiscardPackets...............%u", (L7_uint32)p_IFO->PckStats.DiscardPackets);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tBadVersion...................%u", (L7_uint32)p_IFO->PckStats.BadVersion);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tBadNetwork...................%u", (L7_uint32)p_IFO->PckStats.BadNetwork);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tBadVirtualLink...............%u", (L7_uint32)p_IFO->PckStats.BadVirtualLink);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tBadArea......................%u", (L7_uint32)p_IFO->PckStats.BadArea);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tBadDstAdr....................%u", (L7_uint32)p_IFO->PckStats.BadDstAdr);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tBadAuType....................%u", (L7_uint32)p_IFO->PckStats.BadAuType);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tBadAuthentication............%u", (L7_uint32)p_IFO->PckStats.BadAuthentication);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tBadNeighbor..................%u", (L7_uint32)p_IFO->PckStats.BadNeighbor);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tBadPckType...................%u", (L7_uint32)p_IFO->PckStats.BadPckType);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tTxPackets....................%u", (L7_uint32)p_IFO->PckStats.TxPackets);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRxHellos.....................%u", (L7_uint32)p_IFO->PckStats.RxHellos);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRxDbDescr....................%u", (L7_uint32)p_IFO->PckStats.RxDbDescr);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRxLsReq......................%u", (L7_uint32)p_IFO->PckStats.RxLsReq);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRxLsUpdate...................%u", (L7_uint32)p_IFO->PckStats.RxLsUpdate);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRxLsAck......................%u", (L7_uint32)p_IFO->PckStats.RxLsAck);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tTxHellos.....................%u", (L7_uint32)p_IFO->PckStats.TxHellos);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tTxDbDescr....................%u", (L7_uint32)p_IFO->PckStats.TxDbDescr);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tTxLsReq......................%u", (L7_uint32)p_IFO->PckStats.TxLsReq);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tTxLsUpdate...................%u", (L7_uint32)p_IFO->PckStats.TxLsUpdate);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tTxLsAck......................%u", (L7_uint32)p_IFO->PckStats.TxLsAck);

#if L7_OSPF_TE
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\nTraffic Engineering Last PDRs (TePdr):");
      for (i = 0; i < 4; i++)
      {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n");
        for (j = 0; j < 8; j++)
        {
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "   %.2f", (double)p_IFO->TePdr[i][j]);
        }
      }
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\nTeLinkId...................................%u", (L7_uint32)p_IFO->TeLinkId);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nTeCosMask..................................%u", (L7_uint32)p_IFO->TeCosMask);
#endif

      /* display MD5 authentication key (location depends on whether IFO or virtual IFO) */
      if (virtFlag != L7_TRUE)
        rc = ifoAuthKeyCfgGet(intIfNum, &authKeyCfg);
      else
        rc = virtIfAuthKeyCfgGet((t_Handle)p_IFO, &authKeyCfg);

      if (rc == L7_SUCCESS)
      {
        t_OspfAuthKey *p_Auth = &authKeyCfg;
        L7_uchar8 *pStr;

        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\nMD5 Authentication Key Info:");
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tKey ID.....................................%u", (L7_uint32)p_Auth->KeyId);
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tKey........................................");
        for (i = 0; i < OSPF_MAP_EXTEN_AUTH_MAX_KEY_MD5; i++)
        {
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "%02X", (L7_uchar8)p_Auth->KeyValue[i]);
        }
        memcpy(authKey, p_Auth->KeyValue, (size_t)OSPF_MAP_EXTEN_AUTH_MAX_KEY_MD5);
        *(authKey+OSPF_MAP_EXTEN_AUTH_MAX_KEY_MD5) = L7_EOS;
        if (strlen(authKey) > 0)
        {
          SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "  \"%s\"", authKey);
        }
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tStart Accept...............................0x%8.8x", (L7_uint32)p_Auth->KeyStartAccept);
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tStop Accept................................0x%8.8x", (L7_uint32)p_Auth->KeyStopAccept);
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tStart Generate.............................0x%8.8x", (L7_uint32)p_Auth->KeyStartGenerate);
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tStop Generate..............................0x%8.8x", (L7_uint32)p_Auth->KeyStopGenerate);
        switch (p_Auth->KeyStatus)
        {
        case ROW_ACTIVE:
          pStr = "rowActive";
          break;
        case ROW_NOT_IN_SERVICE:
          pStr = "rowNotInService";
          break;
        case ROW_NOT_READY:
          pStr = "rowNotReady";
          break;
        case ROW_CREATE_AND_GO:
          pStr = "rowCreateAndGo";
          break;
        case ROW_CREATE_AND_WAIT:
          pStr = "rowCreateAndWait";
          break;
        case ROW_DESTROY:
          pStr = "rowDestroy";
          break;
        default:
          pStr = "(other)";
          break;
        }
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRow Status.................................%s", pStr);
      }
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n");

    }
    else
    {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nNot an OSPF interface");
    }
  }
}

/*********************************************************************
* @purpose  debug function to dump the specified RTO structure onto
*           the screen
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ospfMapExtenRTOShow(void)
{
  t_RTO *p_RTO = NULL;
  t_S_RouterCfg rtrCfg;
  L7_uchar8 debug_buf[30];

  p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;

  if (p_RTO == NULL)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nCould not find RTO object");
    return;
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n+--------------------------------------------------------+");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n|              OSPF RTO DATA                             |");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n+--------------------------------------------------------+");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nRTO pointer................................%p", p_RTO);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nnext RTO pointer...........................%p", p_RTO->next);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nprevious RTO pointer.......................%p", p_RTO->prev);

  if (p_RTO->Taken == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nTaken......................................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nTaken......................................false");
  }
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nMngId......................................0x%x", (L7_uint32)p_RTO->MngId);

  memset(&rtrCfg, 0, sizeof(t_S_RouterCfg));
  memcpy(&rtrCfg, &p_RTO->Cfg, sizeof(t_S_RouterCfg));
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\nRTO config Data:");

  osapiInetNtoa((L7_uint32)rtrCfg.RouterId, debug_buf);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRouterId.........................%-15.15s", debug_buf);
  if (rtrCfg.AdminStat == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tAdminStat........................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tAdminStat........................false");
  }
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tVersionNumber....................%u", (L7_uint32)rtrCfg.VersionNumber);
  if (rtrCfg.AreaBdrRtrStatus == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tAreaBdrRtrStatus.................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tAreaBdrRtrStatus.................false");
  }
  if (rtrCfg.ASBdrRtrStatus == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tASBdrRtrStatus...................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tASBdrRtrStatus...................false");
  }
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tExternLsaCount...................%u", (L7_uint32)rtrCfg.ExternLsaCount);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tExternLsaCksumSum................%u", (L7_uint32)rtrCfg.ExternLsaCksumSum);
  if (rtrCfg.TOSSupport == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tTOSSupport.......................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tTOSSupport.......................false");
  }
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tOriginateNewLsas.................%u", (L7_uint32)rtrCfg.OriginateNewLsas);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRxNewLsas........................%u", (L7_uint32)rtrCfg.RxNewLsas);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tExtLsdbLimit.....................%d", (L7_int32)rtrCfg.ExtLsdbLimit);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tMaximumPaths.....................%u", (L7_int32)rtrCfg.MaximumPaths);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tExitOverflowInterval.............%u", (L7_uint32)rtrCfg.ExitOverflowInterval);
  if (rtrCfg.DemandExtensions == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tDemandExtensions.................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tDemandExtensions.................false");
  }
  if (rtrCfg.RFC1583Compatibility == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRFC1583Compatibility.............true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRFC1583Compatibility.............false");
  }
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tSilenceInterval..................%u", (L7_uint32)rtrCfg.SilenceInterval);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tStartUpSilenceInterval...........%u", (L7_uint32)rtrCfg.StartUpSilenceInterval);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tSPF HoldTime.....................%u", (L7_uint32)rtrCfg.Holdtime);
  
  if (rtrCfg.DelAreaOnLastIfDel == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tDelAreaOnLastIfDel...............true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tDelAreaOnLastIfDel...............false");
  }

  switch (rtrCfg.RouterStatus)
  {
  case ROW_ACTIVE:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRouterStatus.....................rowActive");
    break;
  case ROW_NOT_IN_SERVICE:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRouterStatus.....................rowNotInService");
    break;
  case ROW_NOT_READY:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRouterStatus.....................rowNotReady");
    break;
  case ROW_CREATE_AND_GO:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRouterStatus.....................rowCreateAndGo");
    break;
  case ROW_CREATE_AND_WAIT:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRouterStatus.....................rowCreateAndWait");
    break;
  case ROW_DESTROY:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRouterStatus.....................rowDestroy");
    break;
  case ROW_READ:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRouterStatus.....................rowRead");
    break;
  case ROW_READ_NEXT:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRouterStatus.....................rowReadNext");
    break;
  case ROW_CHANGE:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRouterStatus.....................rowChange");
    break;
  case ROW_MODIFY:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRouterStatus.....................rowModify");
    break;
  default:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tRouterStatus.....................unknown");
    break;
  }
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\nOspfSysLabel...............................0x%x", (L7_uint32)p_RTO->OspfSysLabel.threadHndle);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nOspfRtbThread..............................0x%x", (L7_uint32)p_RTO->OspfRtbThread.threadHndle);
  if (p_RTO->RtbOwnThread == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nRtbOwnThread...............................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nRtbOwnThread...............................false");
  }
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nTrapControl................................%u", (L7_uint32)p_RTO->TrapControl);
  if (p_RTO->IsOverflowed == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nIsOverflowed...............................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nIsOverflowed...............................false");
  }
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nOverflowTimer..............................0x%x", (L7_uint32)p_RTO->OverflowTimer);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nnonDefExtLsaCount..........................0x%x", (L7_uint32)p_RTO->NonDefExtLsaCount);
  
  if (p_RTO->LsdbOverload == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nLsdbOverload...............................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nLsdbOverload...............................false");
  }
  
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nAsExtRoutesHl..............................0x%x", (L7_uint32)p_RTO->AsExtRoutesHl);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nAsExternalLsaHl............................0x%x", (L7_uint32)p_RTO->AsExternalLsaHl);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nAsOpaqueLsaHl..............................0x%x", (L7_uint32)p_RTO->AsOpaqueLsaHl);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nRtbHl......................................0x%x", (L7_uint32)p_RTO->RtbHl);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nIfoIndexHl.................................0x%x", (L7_uint32)p_RTO->IfoIndexHl);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nFaIndexHl..................................0x%x", (L7_uint32)p_RTO->FaIndexHl);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nAroHl......................................0x%x", (L7_uint32)p_RTO->AroHl);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nAreasNum...................................%u", (L7_uint32)p_RTO->AreasNum);
}

/*********************************************************************
* @purpose  debug function to dump the list of prefixes in AsExtRoutesHl
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ospfDebugExtRoutesShow(void)
{
    t_RTO *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;
    t_S_AsExternalCfg *p_Entry;
    e_Err eIter;

    if (p_RTO == NULL)
    {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nCould not find RTO object");
        return;
    }

    eIter = HL_GetFirst(p_RTO->AsExtRoutesHl, (void *) &p_Entry);
    while (eIter == E_OK) 
    {
        L7_uchar8 pfxStr[OSAPI_INET_NTOA_BUF_SIZE];
        L7_uchar8 lsidStr[OSAPI_INET_NTOA_BUF_SIZE];
        osapiInetNtoa(p_Entry->DestNetIpAdr, pfxStr);
        osapiInetNtoa(p_Entry->LsId, lsidStr);
        printf("\n%16s/%d lsid %s", pfxStr, rtoMaskLength(p_Entry->DestNetIpMask), lsidStr);
        eIter = HL_GetNext(p_RTO->AsExtRoutesHl, (void*) &p_Entry, p_Entry);
    }
    return;
}

/*********************************************************************
* @purpose  debug function to dump the list of aggregates for a given area
*
* @param    areaId - Area ID of area whose ranges are to be shown
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ospfDebugAggrShow(L7_uint32 areaId)
{
    e_Err          er = E_OK;
    t_AgrEntry    *p_AdrRange;
    t_RTO *p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;
    t_ARO *p_ARO = NULL;
    word numRanges = 0;

    if (p_RTO == NULL)
    {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nCould not find RTO object");
        return;
    }

    /* Get the area object */
    if (HL_FindFirst(p_RTO->AroHl, (byte*)&areaId, (void**)&p_ARO) != E_OK)
    {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  
                       "\nCould not find area object for area %u", areaId);
        return;
    }

    HL_GetEntriesNmb(p_ARO->AdrRangeHl, &numRanges);
    if (numRanges == 0)
    {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  
                       "\nNo area ranges for area %u", areaId);
        return;
    }

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,  
                  "\n%-16s %-16s %4s %6s %6s %10s %10s", 
                  "Prefix", "Mask", "Type", "Effect", "Active", "Cost", "Num Routes");

    er = HL_GetFirst(p_ARO->AdrRangeHl, (void *)&p_AdrRange);
    while (er == E_OK)
   {
        L7_uchar8 pfxStr[OSAPI_INET_NTOA_BUF_SIZE];
        L7_uchar8 maskStr[OSAPI_INET_NTOA_BUF_SIZE];
        L7_uchar8 *effectStr = "A";
        word numRoutes;
        
        if (p_AdrRange->AggregateEffect == AGGREGATE_DO_NOT_ADVERTISE)
            effectStr = "S";

        HL_GetEntriesNmb(p_AdrRange->AgrRteHl, &numRoutes);
        osapiInetNtoa(p_AdrRange->NetIpAdr, pfxStr);
        osapiInetNtoa(p_AdrRange->SubnetMask, maskStr);

        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,  
                      "\n%-16s %-16s %4d %6s %6s %10ld %10d", 
                      pfxStr, maskStr, p_AdrRange->LsdbType, effectStr, 
                      p_AdrRange->alive ? "Y" : "N", p_AdrRange->LargestCost, numRoutes);

        /* List each contained route */
        if (numRoutes > 0)
        {
            t_RoutingTableEntry *p_Rte = NULLP;
            L7_uchar8 containedPfxStr[OSAPI_INET_NTOA_BUF_SIZE];
            L7_uchar8 containedMaskStr[OSAPI_INET_NTOA_BUF_SIZE];
            e_Err e;
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,  
                          "\n  Contained routes...");
            e = HL_GetFirst(p_AdrRange->AgrRteHl, (void *)&p_Rte);
            while((e == E_OK) && p_Rte)
            {

                osapiInetNtoa(p_Rte->DestinationId, containedPfxStr);
                osapiInetNtoa(p_Rte->IpMask, containedMaskStr);
                SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,  
                              "\n    %-16s %-16s ", containedPfxStr, containedMaskStr);
                e = HL_GetNext(p_AdrRange->AgrRteHl, (void **)&p_Rte, (void *)p_Rte);
            }
        }

      er = HL_GetNext(p_ARO->AdrRangeHl, (void *)&p_AdrRange, p_AdrRange);
   } 
}

/*********************************************************************
* @purpose  debug function to dump the specified ARO structure onto
*           the screen
*
* @param    areaId          @b{(input)} area id
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ospfMapExtenAROShow(L7_uint32 areaId)
{
  t_ARO         *p_ARO = NULL;
  e_Err         e;
  t_RTO         *p_RTO;
  L7_uchar8     debug_buf[30];

  p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;
  if (p_RTO == NULL)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nCould not find RTO object");
    return;
  }

  e = HL_FindFirst(p_RTO->AroHl, (byte*)&areaId, (void**)&p_ARO);
  if(e != E_OK || p_ARO == NULL)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nCould not find area %u", areaId);
    return;
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n+--------------------------------------------------------+");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n|              OSPF ARO DATA                             |");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n+--------------------------------------------------------+");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nnext ARO pointer.............................%#x", p_ARO->next);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nprevious ARO pointer.........................%#x", p_ARO->prev);

  if (p_ARO->Taken == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nTaken........................................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nTaken........................................false");
  }

  if (p_ARO->OperationState == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nOperationState...............................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nOperationState...............................false");
  }
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nRTO_Id.......................................0x%x", (L7_uint32)p_ARO->RTO_Id);

  osapiInetNtoa((L7_uint32)p_ARO->AreaId, debug_buf);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nAreaId.......................................%-15.15s", debug_buf);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nMngId........................................0x%x", (L7_uint32)p_ARO->MngId);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nIfoHl........................................0x%x", (L7_uint32)p_ARO->IfoHl);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nAdrRangeHl...................................0x%x", (L7_uint32)p_ARO->AdrRangeHl);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nHostHl.......................................0x%x", (L7_uint32)p_ARO->HostHl);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nRouterLsaHl..................................0x%x", (L7_uint32)p_ARO->RouterLsaHl);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nNetworkLsaHl.................................0x%x", (L7_uint32)p_ARO->NetworkLsaHl);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nNetSummaryLsaHl..............................0x%x", (L7_uint32)p_ARO->NetSummaryLsaHl);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nAsBoundSummaryLsaHl..........................0x%x", (L7_uint32)p_ARO->AsBoundSummaryLsaHl);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nNssaLsaHl....................................0x%x", (L7_uint32)p_ARO->NssaLsaHl);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nAreaOpaqueLsaHl..............................0x%x", (L7_uint32)p_ARO->AreaOpaqueLsaHl);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nSpfHl........................................0x%x", (L7_uint32)p_ARO->SpfHl);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nCandidateHl..................................0x%x", (L7_uint32)p_ARO->CandidateHl);

  if (p_ARO->TransitCapability == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nTransitCapability............................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nTransitCapability............................false");
  }

  switch (p_ARO->ExternalRoutingCapability)
  {
  case AREA_IMPORT_EXTERNAL:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nExternalRoutingCapability....................ImportExternal");
    break;
  case AREA_IMPORT_NO_EXTERNAL:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nExternalRoutingCapability....................ImportNoExternal");
    break;
  case AREA_IMPORT_NSSA:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nExternalRoutingCapability....................ImportNSSA");
    break;
  default:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nExternalRoutingCapability....................unknown");
    break;
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nStubDefaultCostHl............................0x%x", (L7_uint32)p_ARO->StubDefaultCostHl);

  switch (p_ARO->ImportSummaries)
  {
  case TRUE:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nImportSummaries..............................True");
    break;
  case FALSE:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nImportSummaries..............................False");
    break;
  default:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nImportSummaries..............................unknown");
    break;
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nExchOrLoadState..............................%u", (L7_uint32)p_ARO->ExchOrLoadState);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nSpfRuns......................................%u", (L7_uint32)p_ARO->SpfRuns);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nAreaBdrRtrCount..............................%u", (L7_uint32)p_ARO->AreaBdrRtrCount);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nAsBdrRtrCount................................%u", (L7_uint32)p_ARO->AsBdrRtrCount);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nAreaIntRtrCount..............................%u", (L7_uint32)p_ARO->AreaIntRtrCount);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nAreaLsaCount.................................%u", (L7_uint32)p_ARO->AreaLsaCount);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nAreaLsaCksumSum..............................%u", (L7_uint32)p_ARO->AreaLsaCksumSum);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nDelayedLsa...................................0x%x", (L7_uint32)p_ARO->DelayedLsa);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nDlyLsaTimer..................................0x%x", (L7_uint32)p_ARO->DlyLsaTimer);
  
#if 0  
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nMcSpfRuns....................................%u", (L7_uint32)p_ARO->McSpfRuns);

  switch (p_ARO->McInitCase)
  {
  case McSourceIntraArea:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nMcInitCase...................................McSourceIntraArea");
    break;
  case McSourceInterArea2:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nMcInitCase...................................McSourceInterArea2");
    break;
  case McSourceInterArea1:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nMcInitCase...................................McSourceInterArea1");
    break;
  case McSourceExternal:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nMcInitCase...................................McSourceExternal");
    break;
  case McSourceStubExternal:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nMcInitCase...................................McSourceStubExternal");
    break;
  default:
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nMcInitCase...................................unknown");
    break;
  }
#endif

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nOspfSysLabel.................................0x%x", (L7_uint32)p_ARO->OspfSysLabel.threadHndle);

  if (p_ARO->HasVirtLink == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nHasVirtLink..................................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nHasVirtLink..................................false");
  }

  if (p_ARO->IsAgrInit == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nIsAgrInit....................................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nIsAgrInit....................................false");
  }

  if (p_ARO->IsRtrLsaOrg == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nIsRtrLsaOrg..................................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nIsRtrLsaOrg..................................false");
  }

  if (p_ARO->IsNtwLsaOrg == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nIsNtwLsaOrg..................................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nIsNtwLsaOrg..................................false");
  }

#if 0
  if (p_ARO->TEsupport == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nTEsupport....................................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nTEsupport....................................false");
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nTeSpfRuns....................................%u", (L7_uint32)p_ARO->TeSpfRuns);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nTeLsaHl......................................0x%x", (L7_uint32)p_ARO->TeLsaHl);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nTeRtrHl......................................0x%x", (L7_uint32)p_ARO->TeRtrHl);
#endif  
  
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nDataBaseSize.................................%u", (L7_uint32)p_ARO->DataBaseSize);

  /* NSSA stuff */
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,   
                "\nNSSA Translator Role.........................");
  if (p_ARO->NSSATranslatorRole == NSSA_TRANSLATOR_ALWAYS)
  {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP, "ALWAYS");
  }
  else if (p_ARO->NSSATranslatorRole == NSSA_TRANSLATOR_CANDIDATE)
  {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP, "CANDIDATE");
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,   
                "\nNSSA Translator State........................");
  if (p_ARO->NSSATranslatorState == NSSA_TRANS_STATE_ENABLED)
  {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP, "ENABLED");
  }
  else if (p_ARO->NSSATranslatorState == NSSA_TRANS_STATE_ELECTED)
  {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP, "ELECTED");
  }
  else if (p_ARO->NSSATranslatorState == NSSA_TRANS_STATE_DISABLED)
  {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP, "DISABLED");
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,   
                "\nNSSA Translator Stability Interval...........%lu", 
                p_ARO->TranslatorStabilityInterval);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,   
                "\nNSSA Import Summaries........................%s",
                p_ARO->ImportSummaries ? "ENABLED" : "DISABLED");

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  
                 "\nNSSA T3 Default Cost........................%u", 
                 (L7_uint32)p_ARO->DefaultCost);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,   
                "\nNSSA Redistribute............................%s",
                p_ARO->NSSARedistribute ? "ENABLED" : "DISABLED");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,   
                "\nNSSA Default Information Originate...........%s",
                p_ARO->NSSADefInfoOrig ? "ENABLED" : "DISABLED");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,   
                "\nNSSA Default Metric..........................%lu", 
                p_ARO->NSSADefMetric);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,   
                "\nNSSA Default Metric Type.....................%s",
                (p_ARO->NSSADefMetricType == NSSA_COMPARABLE_COST) ? "Type 1" : "Type 2");
}

void ospfDebugNBOShowPtr(t_NBO * p_NBO)
{
  L7_uchar8     debug_buf[30];
  t_IFO *p_IFO;
  L7_uint32 nboUptime;
  L7_uint32 percentOfHellos = 100;

  if(!p_NBO)
  {
    sysapiPrintf("\r\nPlease provide a valid NBO pointer (use ospfDebugNBOPrint for a list)");
    return;
  }
  p_IFO = (t_IFO*) p_NBO->IFO_Id;

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n+--------------------------------------------------------+");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n|              OSPF NBO DATA for Neighbor %x              |", p_NBO);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n+--------------------------------------------------------+");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nnext NBO pointer...........................%p", p_NBO->next);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nprevious NBO pointer.......................%p", p_NBO->prev);

  if (p_NBO->Taken == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nTaken......................................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nTaken......................................false");
  }

  if (p_NBO->OperationState == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nOperationState.............................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nOperationState.............................false");
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nState......................................%u", (L7_uint32)p_NBO->State);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nIFO_Id.....................................0x%x", (L7_uint32)p_NBO->IFO_Id);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nRTO_Id.....................................0x%x", (L7_uint32)p_NBO->RTO_Id);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nARO_Id.....................................0x%x", (L7_uint32)p_NBO->ARO_Id);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nMngId......................................0x%x", (L7_uint32)p_NBO->MngId);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nInactivityTimer............................0x%x", (L7_uint32)p_NBO->InactivityTimer);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nInactivityInterval.........................%u", (L7_uint32)p_NBO->InactivityInterval);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nDdRxmtTimer................................0x%x", (L7_uint32)p_NBO->DdRxmtTimer);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nRqRxmtTimer................................0x%x", (L7_uint32)p_NBO->RqRxmtTimer);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nSlaveLastDdTimer...........................0x%x", (L7_uint32)p_NBO->SlaveLastDdTimer);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nRxmtInterval...............................%u", (L7_uint32)p_NBO->RxmtInterval);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nHloIntCount................................%u", (L7_uint32)p_NBO->HloIntCount);

  if (p_NBO->IsMaster == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nIsMaster...................................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nIsMaster...................................false");
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nDdSeqNum...................................%u", (L7_uint32)p_NBO->DdSeqNum);

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\nLastRxDd:");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tInterfaceMtu.................%u %u",
         (L7_ushort16)p_NBO->LastRxDd.InterfaceMtu[0],
         (L7_ushort16)p_NBO->LastRxDd.InterfaceMtu[1]);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tOptions......................%u", (L7_ushort16)p_NBO->LastRxDd.Options);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tFlags........................%u", (L7_ushort16)p_NBO->LastRxDd.Flags);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\tSeqNum.......................%u %u %u %u",
         (L7_ushort16)p_NBO->LastRxDd.SeqNum[0],
         (L7_ushort16)p_NBO->LastRxDd.SeqNum[1],
         (L7_ushort16)p_NBO->LastRxDd.SeqNum[2],
         (L7_ushort16)p_NBO->LastRxDd.SeqNum[3]);


  osapiInetNtoa((L7_uint32)p_NBO->RouterId, debug_buf);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\n\nRouterId...................................%-15.15s", debug_buf);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nRouterPriority.............................%u", (L7_ushort16)p_NBO->RouterPriority);

  osapiInetNtoa((L7_uint32)p_NBO->IpAdr, debug_buf);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nIpAdr......................................%-15.15s", debug_buf);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nOptions....................................%u", (L7_ushort16)p_NBO->Options);

  osapiInetNtoa((L7_uint32)p_NBO->DrId, debug_buf);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nDrId.......................................%-15.15s", debug_buf);

  osapiInetNtoa((L7_uint32)p_NBO->BackupId, debug_buf);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nBackupId...................................%-15.15s", debug_buf);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nLeReTx.....................................0x%x", (L7_uint32)p_NBO->LsReTx);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nDbSum......................................0x%x", (L7_uint32)p_NBO->DbSum);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nLsReq......................................0x%x", (L7_uint32)p_NBO->LsReq);

  if (p_NBO->ExchOrLoadState == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nExchOrLoadState............................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nExchOrLoadState............................false");
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nNbrEvents..................................%u", 
                 (L7_uint32)p_NBO->NbrEvents);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nDuplicateDDs...............................%u", 
                 (L7_uint32)p_NBO->dupDDs);

  nboUptime = TICKS_TO_SECONDS(TIMER_SysTime() - p_NBO->Uptime);
  if (nboUptime > p_IFO->Cfg.HelloInterval)
  {
    percentOfHellos = (100 * p_NBO->hellosRx) / (nboUptime / p_IFO->Cfg.HelloInterval);
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  
                 "\nHellosRx...................................%u (%u%% of expected)", 
                 (L7_uint32)p_NBO->hellosRx, percentOfHellos);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  
                 "\nlateHellos.................................%u", 
                 (L7_uint32)p_NBO->lateHellos);

  if (p_NBO->NbrHelloSuppressed == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nNbrHelloSuppressed.........................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nNbrHelloSuppressed.........................false");
  }

  if (p_NBO->InitDDSent == TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nInitDDSent.................................true");
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nInitDDSent.................................false");
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nOspfSysLabel...............................0x%x", (L7_uint32)p_NBO->OspfSysLabel.threadHndle);
}

/*********************************************************************
* @purpose  debug function to dump the specified NBO structure onto
*           the screen
*
* @param    slot          @b{(input)} interface specific slot
* @param    port          @b{(input)} interface specific port
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ospfMapExtenNBOShow(L7_uint32 unit, L7_uint32 slot, L7_uint32 port)
{
  nimUSP_t      usp;
  L7_uint32     intIfNum;
  t_IFO         *p_IFO = NULL;
  t_NBO         *p_NBO = NULL;
  e_Err e;

  usp.unit = unit;
  usp.slot = slot;
  usp.port = port;

  if (nimGetIntIfNumFromUSP(&usp, &intIfNum) == L7_SUCCESS)
  {
    if ((p_IFO = ifoPtrGet(intIfNum)) != NULL)
    {
      e = HL_GetFirst(p_IFO->NboHl, (void *)&p_NBO);
      while(e == E_OK && p_NBO)
      {
        ospfDebugNBOShowPtr(p_NBO);
        e = HL_GetNext(p_IFO->NboHl, (void *)&p_NBO, p_NBO);
      }
    }
    else
    {
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nNot an OSPF interface");
    }
  }
  else
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nInvalid interface");
  }
}

/*********************************************************************
* @purpose  debug function to dump display the ospf vendor route table
*           on the screen
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ospfDebugVendorRouteTableShow(void)
{
  t_RTO         *p_RTO;

  p_RTO = (t_RTO *) ospfMapCtrl_g.RTO_Id;
  if (p_RTO == NULL)
  {
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,  "\nCould not find RTO object");
    return;
  }

  RtbPrint(ospfMapCtrl_g.RTO_Id, 0);

  return;
}

/*********************************************************************
 * @purpose         Output reachability for a single ABR/ASBR
 *
 * @param p_RTO     none
 *
 * @returns         nothing
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static void
ShowBr(char *type, t_RoutingTableEntry *p_Rte, SP_IPADR area)
{
  char buf[OSPF_MAX_TRACE_STR_LEN];
  L7_uchar8 rtrId[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 nHop[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 areaId[OSAPI_INET_NTOA_BUF_SIZE];
   
  if (p_Rte->PathNum && p_Rte->PathPrm != NULL)
  {
    t_OspfPathPrm *Path;
    Path = p_Rte->PathPrm;
    osapiInetNtoa(p_Rte->DestinationId, rtrId);
    osapiInetNtoa(p_Rte->PathPrm->IpAdr, nHop);
    osapiInetNtoa(area, areaId);
    sprintf(buf, "%-15s via %-15s [%ld], %s, Area %s",
            rtrId, nHop, p_Rte->Cost, type, areaId);
    XX_Print(buf);

    if (p_Rte->PathNum > 1) 
    {
      Path = Path->next;
      while(Path != NULL)
      {
        sprintf(buf, "                via %-15s",
                osapiInet_ntoa(p_Rte->PathPrm->IpAdr));
        XX_Print(buf);
        Path = Path->next;
      }
    }
  }
}

/*********************************************************************
 * @purpose         Show reachability for all ABRs/ASBRs
 *
 * @param p_RTO     none
 *
 * @returns         nothing
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void
ospfBorderRoutersShow(void)
{
  /* assumes only one RTO */
  t_RTO *p_RTO = RTO_List;
  t_RTB *p_RTB;
  t_ARO *p_ARO;
  e_Err e;
  t_RoutingTableEntry *p_Rte;
  e_OspfDestType destAbr = 
    DEST_TYPE_IS_ROUTER | DEST_TYPE_IS_BORDER_ROUTER;
  e_OspfDestType destAsbr = 
    DEST_TYPE_IS_ROUTER | DEST_TYPE_IS_AS_BOUNDARY_ROUTER;
  e_OspfDestType destAbrAsbr = destAbr | destAsbr;

  if (osapiSemaTake(ospfMapCtrl_g.ospfSyncSema, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return;
  }
   
    if (p_RTO == NULL)
    {
      XX_Print("RTO is NULL\n");
      osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
      return;
    }

    sysapiPrintf("Intra-Area Best Paths To ABRs and ASBRs\n");
    /* Browse all areas */
    for (e = HL_GetFirst(p_RTO->AroHl, (void *)&p_ARO);
        e == E_OK;
        e = HL_GetNext(p_RTO->AroHl, (void *)&p_ARO, p_ARO))
    {
      /* iterate through all nodes in the shortest path tree */
      for (e = AVLH_GetFirst(p_ARO->SpfHl, (void *)&p_Rte);
          e == E_OK && p_Rte;
          e = AVLH_GetNext(p_ARO->SpfHl, (void *)&p_Rte, p_Rte))
      {
        if ((p_Rte->DestinationType & destAbrAsbr) == destAbr)
        {
          ShowBr("ABR", p_Rte, p_ARO->AreaId);
        }
        if ((p_Rte->DestinationType & destAbrAsbr) == destAsbr)
        {
          ShowBr("ASBR", p_Rte, p_ARO->AreaId);
        }
        if ((p_Rte->DestinationType & destAbrAsbr) == destAbrAsbr)
        {
          ShowBr("ABR+ASBR", p_Rte, p_ARO->AreaId);
        }
      }
    }

    sysapiPrintf("AS-External Best Paths To ASBRs\n");
    for (e = HL_GetFirst(p_RTO->RtbHl, (void *)&p_RTB);
        e == E_OK;
        e = HL_GetNext(p_RTO->RtbHl, (void *)&p_RTB, p_RTB))
    {
      e = o2RouteGetFirst(p_RTB, p_RTB->RtbRtBt, &p_Rte);
      while (e == E_OK)
      {
        if ((p_Rte->DestinationType & destAsbr) == destAsbr)
        {
          ShowBr("ASBR", p_Rte, p_Rte->AreaId);
        }
        else
        {
          ShowBr("????", p_Rte, p_Rte->AreaId);
        }
        e = o2RouteGetNext(p_RTB, p_RTB->RtbRtBt, &p_Rte);
      }
    }
    osapiSemaGive(ospfMapCtrl_g.ospfSyncSema);
}

/*********************************************************************
* @purpose  Display the "network area" operational data
*
* @param    None
*
* @returns  None
*
* @notes    None
*       
* @end
*********************************************************************/
void ospfMapNetworkAreaShow()
{
    L7_uchar8 debug_buf[30];
    ospfNetworkAreaInfo_t *p_NetworkArea = networkAreaListHead_g;

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                  "Network Area Commands Operational List\n");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                  "Head: %x, FreeList Head: %x\n", networkAreaListHead_g, networkAreaListFree_g);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                  "\n---------------------------------------------------------------");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                  "\nAddress    Network         WildCardMask    Area           Next");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                  "\n---------------------------------------------------------------");
    while(p_NetworkArea != L7_NULLPTR)
    {
      osapiInetNtoa(p_NetworkArea->ipAddr, debug_buf);
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,"\n%-11x%-16.15s", p_NetworkArea, debug_buf);
      osapiInetNtoa(p_NetworkArea->wildcardMask, debug_buf);
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,"%-16.15s", debug_buf);
      osapiInetNtoa(p_NetworkArea->areaId, debug_buf);
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP,"%-16.15s%-11x", debug_buf, p_NetworkArea->next);
      p_NetworkArea = p_NetworkArea->next;
    }
}

/*********************************************************************
* @purpose  Print the number of messages in the OSPF message queue
*
* @param    void
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void ospfMsgQLen(L7_uint32 detail)
{
  L7_int32 i, msgQLen = 0;
  OS_Thread *threadId = (OS_Thread*) ospfMapCtrl_g.ospfThread;

  printf("\nOSPF protocol thread ID:  %#x", (unsigned int) threadId);
  for(i = 0; i < OSPF_NUM_QUEUES; i++)
  {
    if (osapiMsgQueueGetNumMsgs((void*)threadId->QueueID[i], &msgQLen) == L7_SUCCESS)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP, 
                    "\nOSPF %s (Q%d) length:  %d", 
                    ospfMsgQueueNames[i], i, msgQLen);
      if (detail)
      {
        XX_DisplayQueue(threadId, i, L7_TRUE, 20);
      }
    }
    else
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_OSPFMAP, 
                    "\nOSPF message queue id %d Failed.", i);
    }
  }
}

/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/



/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    void
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void ospfBuildTestConfigData(void)
{

    L7_uint32 cfgIndex; /* cfgIndex               */
    L7_uint32 area;     /* area index             */
    L7_uint32 tosIndex; /* tos  index             */
    L7_uint32 vNbr;     /* virtual neighbor index */
    L7_uint32 i;        

  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/


    /* Ensure at least one entry in each array has a unique value to validate
       accurate migration */

    /* Generic OSPF Stack Cfg */
    pOspfMapCfgData->rtr.routerID             = 0xAA;
    pOspfMapCfgData->rtr.ospfAdminMode        = L7_DISABLE;
    pOspfMapCfgData->rtr.rfc1583Compatibility = L7_DISABLE;
    pOspfMapCfgData->rtr.exitOverflowInterval = 12;
    pOspfMapCfgData->rtr.extLSALimit          = 44;
    pOspfMapCfgData->rtr.opaqueLSAMode        = L7_ENABLE;
    pOspfMapCfgData->rtr.traceMode            = L7_DISABLE;
    /* There really isn't a default value for defaultMetric because its value is
     * irrelevant until defMetConfigured is L7_TRUE, but this value
     * is as good as any. 
     */
    pOspfMapCfgData->rtr.defaultMetric        = 77;
    pOspfMapCfgData->rtr.defMetConfigured     = L7_TRUE;
    pOspfMapCfgData->rtr.maxPaths             = 3;

    /* OSPF Router Interface Configuration parameters */
    for (cfgIndex = 1; cfgIndex < L7_OSPF_INTF_MAX_COUNT; cfgIndex++)
        ospfIntfBuildTestConfigData(&pOspfMapCfgData->ckt[cfgIndex]);

    /* OSPF Area Configuration parameters */

    /* Only the backbone area needs to be configured initially */
    area =  22;

    pOspfMapCfgData->area[area].inUse         = L7_TRUE;
    pOspfMapCfgData->area[area].area          = 14;
    pOspfMapCfgData->area[area].agingInterval = FD_OSPF_AREA_DEFAULT_AGING_INTERVAL;
    pOspfMapCfgData->area[area].summary       = L7_OSPF_AREA_NO_SUMMARY;

    /* Configure stub metrics */
    for (tosIndex = 0; tosIndex < L7_RTR_MAX_TOS_INDICES; tosIndex++)
    {
        pOspfMapCfgData->area[area].stubCfg[tosIndex].stubMetricType 
        = L7_OSPF_AREA_STUB_COMPARABLE_COST;

        pOspfMapCfgData->area[area].stubCfg[tosIndex].stubMetric = 4 + tosIndex;

        pOspfMapCfgData->area[area].stubCfg[tosIndex].inUse   = L7_TRUE;
    }  /*   tos < L7_MAX_TOS_INDICES */


    /* OSPF Area Range Configuration parameters:
       Nothing to be done.  Zeroeing out the configuration is enough */

    /* OSPF Virtual Neighbor Configuration Parameters */
    for (vNbr = 0; vNbr < L7_OSPF_MAX_VIRT_NBRS; vNbr++)
    {
        /* These settings are the default values for virtual neighbor entries when created */
        pOspfMapCfgData->virtNbrs[vNbr].virtTransitAreaID = vNbr+1;
        pOspfMapCfgData->virtNbrs[vNbr].virtIntfNeighbor = vNbr+2;
        pOspfMapCfgData->virtNbrs[vNbr].intIfNum = vNbr+3;
        pOspfMapCfgData->virtNbrs[vNbr].helloInterval = 22;
        pOspfMapCfgData->virtNbrs[vNbr].deadInterval = 28;
        pOspfMapCfgData->virtNbrs[vNbr].rxmtInterval = 32;
        pOspfMapCfgData->virtNbrs[vNbr].ifTransitDelay = 3;
        pOspfMapCfgData->virtNbrs[vNbr].authType = L7_AUTH_TYPE_SIMPLE_PASSWORD;
        memset((void *)pOspfMapCfgData->virtNbrs[vNbr].authKey, 4, (size_t)L7_AUTH_MAX_KEY_OSPF);  
        pOspfMapCfgData->virtNbrs[vNbr].authKeyLen = 2;
        pOspfMapCfgData->virtNbrs[vNbr].authKeyId = 4;

    } /* vNbr < L7_OSPF_MAX_VIRT_NBRS */

    /* Defaults for route redistribution */
    for (i = 0; i < REDIST_RT_LAST; i++) {
        ospfRedistCfgBuildTestConfigData(&pOspfMapCfgData->redistCfg[i], i);
    }

    /* Defaults for default route origination */
    ospfDefaultRouteCfgBuildTestConfigData(&pOspfMapCfgData->defRouteCfg);

    ospfAreaNssaBuildTestConfigData();
    ospfAreaRangeBuildTestConfigData();
    ospfDefRouteCfgBuildTestConfigData();


    /* End of OSPF Component's Factory Defaults */



   /* Force write of config file */
   pOspfMapCfgData->cfgHdr.dataChanged = L7_TRUE;  
   sysapiPrintf("Built test config data\n");


}



/*********************************************************************
* @purpose  Build non-default config of ospfRedistCfg_t.  
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    none 
*
* @end
*********************************************************************/
void ospfRedistCfgBuildTestConfigData(ospfRedistCfg_t *redistCfg, L7_uint32 seed)
{   
    redistCfg->redistribute = L7_TRUE;
    redistCfg->distList = 1 + seed ;
    redistCfg->redistMetric = 2;
    redistCfg->metType = L7_OSPF_METRIC_EXT_TYPE1;
    redistCfg->tag = 3 + seed;
    redistCfg->subnets = L7_TRUE;
    return;
}

/*********************************************************************
* @purpose  Build non-default config of ospfDefRouteCfg_t.  
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    none 
*
* @end
*********************************************************************/
void ospfDefaultRouteCfgBuildTestConfigData(ospfDefRouteCfg_t *defRouteCfg)
{
    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER);
    defRouteCfg->origDefRoute = L7_TRUE;
    defRouteCfg->always = L7_TRUE;
    defRouteCfg->metric = 2;
    defRouteCfg->metType = L7_OSPF_METRIC_EXT_TYPE1;
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return;
}




/*********************************************************************
* @purpose  Dump config for ospfAreaStubCfg_t
*
* @param    void
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void ospfAreaStubCfgDataShow(void)
{
    L7_uint32 i,j;

    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP, "\n\nAREA STUB CFG\n");
    for (i = 0; i < L7_OSPF_MAX_AREAS; i++)
    {

        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP, "\nAREA = %d", i);
        for (j = 0; j < L7_RTR_MAX_TOS_INDICES; j++)
        {

            SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP, "\nTOS = %d", j);
            SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                             "\ninUse = %d", pOspfMapCfgData->area[i].stubCfg[j].inUse);
            SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                             "\nstubMetric = %d", pOspfMapCfgData->area[i].stubCfg[j].stubMetric);
            SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                             "\nstubMetricType = %d", pOspfMapCfgData->area[i].stubCfg[j].stubMetricType);
        }

    }

}


/*********************************************************************
* @purpose  Dump config for ospfAreaNssaCfg_t
*
* @param    void
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void ospfAreaNssaCfgDataShow(void)
{
    L7_uint32 i;

    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP, "\n\nAREA NSSA CFG\n");
    for (i = 0; i < L7_OSPF_MAX_AREAS; i++)
    {

        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP, "\nAREA = %d", i);


        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                         "\ndefInfoOrig = %d", pOspfMapCfgData->area[i].nssaCfg.defInfoOrig);


        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                         "\ndefMetric = %d", pOspfMapCfgData->area[i].nssaCfg.defMetric);

        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                         "\ndefMetricType = %d", pOspfMapCfgData->area[i].nssaCfg.defMetricType);

        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                         "\nimportSummaries = %d", pOspfMapCfgData->area[i].nssaCfg.importSummaries);

        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                         "\nredistribute = %d", pOspfMapCfgData->area[i].nssaCfg.redistribute);

        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                         "\ntranslatorRole = %d", pOspfMapCfgData->area[i].nssaCfg.translatorRole);

        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_OSPFMAP,
                         "\ntranslatorStabilityInterval = %d", pOspfMapCfgData->area[i].nssaCfg.translatorStabilityInterval);
      
    }

}


/*********************************************************************
* @purpose  Build non-default config for ospfAreaNssaCfg_t
*
* @param    void
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void ospfAreaNssaBuildTestConfigData(void)
{
    L7_uint32 i;

    for (i = 0; i < L7_OSPF_MAX_AREAS; i++)
    {
      pOspfMapCfgData->area[i].nssaCfg.defInfoOrig      = 1;
      pOspfMapCfgData->area[i].nssaCfg.defMetric        = 2 + i ; 
      pOspfMapCfgData->area[i].nssaCfg.defMetricType    = 3;
      pOspfMapCfgData->area[i].nssaCfg.importSummaries  = 4;
      pOspfMapCfgData->area[i].nssaCfg.redistribute     = 5;
      pOspfMapCfgData->area[i].nssaCfg.translatorRole   = 6;
      pOspfMapCfgData->area[i].nssaCfg.translatorStabilityInterval = 7 + i;
    }

}


/*********************************************************************
* @purpose  Build non-default config for ospfAreaRangeCfg_t
*
* @param    void
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void ospfAreaRangeBuildTestConfigData(void)
{
    L7_uint32 i;

    for (i = 0; i < L7_OSPF_MAX_AREA_RANGES; i++)
    {
      pOspfMapCfgData->areaRange[i].advertise   = 1;
      pOspfMapCfgData->areaRange[i].area        = 12 + i;
      pOspfMapCfgData->areaRange[i].inUse       = L7_TRUE;
      pOspfMapCfgData->areaRange[i].ipAddr      = 0x0A0A0A01 +  i;
      pOspfMapCfgData->areaRange[i].lsdbType    = 2;
      pOspfMapCfgData->areaRange[i].subnetMask  = 0xFFFF0000;

    }

}



/*********************************************************************
* @purpose  Build non-default config for ospfDefRouteCfg_t
*
* @param    void
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void ospfDefRouteCfgBuildTestConfigData(void)
{
    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER);
    pOspfMapCfgData->defRouteCfg.always     = L7_TRUE;
    pOspfMapCfgData->defRouteCfg.metric     = 5;
    pOspfMapCfgData->defRouteCfg.metType    = L7_OSPF_METRIC_EXT_TYPE2;
    pOspfMapCfgData->defRouteCfg.origDefRoute   = L7_TRUE;
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
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
void ospfIntfBuildTestConfigData(ospfCfgCkt_t *pCfg)
{
    L7_uint32 intIfNum;

    /* Ensure at least one entry in each array has a unique value to validate
       accurate migration */

    if (nimIntIfFromConfigIDGet(&pCfg->configId, &intIfNum) != L7_SUCCESS)
        intIfNum = 0xFF;

    pCfg->adminMode      = L7_ENABLE;
    pCfg->area           = 1 + intIfNum;
    pCfg->priority       = 3;              
    pCfg->helloInterval  = 12;
    pCfg->deadInterval   = 60;         
    pCfg->rxmtInterval   = 12;         
    pCfg->nbmaPollInterval   = 130;    
    pCfg->ifTransitDelay     = 3;      
    pCfg->lsaAckInterval     = 4;      
    pCfg->authType           = L7_AUTH_TYPE_MD5;             
    memset(pCfg->authKey, 0x0A, (size_t)L7_AUTH_MAX_KEY_OSPF);  
    pCfg->authKeyLen         = 4;
    pCfg->authKeyId          = 7;
    pCfg->ifDemandMode       = L7_ENABLE;
    pCfg->virtTransitAreaID  = 9;
    pCfg->virtIntfNeighbor   = 16;  
    pCfg->ifType             = L7_OSPF_INTF_NBMA;
    /* by default, the metric will be computed based on port speed */
    pCfg->metric             = 4 + intIfNum; 
    pCfg->mtuIgnore          = L7_TRUE; 
    pCfg->maxIpMtu           = 1496;
}

/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void ospfConfigDataTestShow(void)
{

    L7_fileHdr_t  *pFileHdr;

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(pOspfMapCfgData->cfgHdr);

    sysapiCfgFileHeaderDump (pFileHdr);

   /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/

    ospfMapCfgDataShow();
    ospfAreaStubCfgDataShow();
    ospfAreaNssaCfgDataShow();


    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("pOspfMapCfgData->checkSum : %u\n", pOspfMapCfgData->checkSum);


}

void ospfOpaqueCapabilitySet(void)
{
    t_RTO *p_RTO = (t_RTO *)ospfMapCtrl_g.RTO_Id;
      p_RTO->Cfg.OpaqueCapability = L7_TRUE;
}

void ospfStackTrace(void)
{
  OS_Thread		*pThreadGlobal = (OS_Thread *)ospfMapCtrl_g.ospfThread;

	osapiDebugStackTrace((L7_int32) pThreadGlobal->TaskID, NULL);
}


/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/

