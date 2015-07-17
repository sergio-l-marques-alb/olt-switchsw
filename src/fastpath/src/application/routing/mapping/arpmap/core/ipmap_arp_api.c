/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    ipmap_arp_api.c
* @purpose     ARP API functions for the IP Mapping layer
* @component   IP Mapping Layer -- ARP
* @comments    Maps to the ARP object API.
*
*
* @comments    All API functions must be in this file and must take
*              the ARP component lock. Some (mostly static) utility
*              functions that are not called from external components
*              dont take the lock.
*
*
* @create      03/21/2001
* @author      gpaussa
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/


#include "l7_common.h"
#include "ipmap_arp.h"
#include "ip_config.h"
#include "dtlapi.h"
#include "bspapi.h"
#include "xx.ext"
#include "oll.ext"
#include "arp.ext"
#include "arp_debug.h"
#include "simapi.h"
#ifdef L7_NSF_PACKAGE
#include "ipmap_arp_ckpt.h"
#endif

extern L7_BOOL        ipMapRoutingStarted;
extern ipMapArpCtx_t  ipMapArpCtx_g;
extern ipMapArpIntf_t *ipMapArpIntfInUse_g[];
static ipMapArpIntf_t ipMapArpSystemIntf;   /* a dummy "system" interface */
L7_uchar8      ipMapArpBcastMac[L7_MAC_ADDR_LEN] = {0xff,0xff,0xff,0xff,0xff,0xff};
ipMapArpLockCtrl_t ipMapArpTableChangeMutex;
extern L7_uint32 ipMapTraceFlags;
extern L7_uint32 ipMapTraceIntf;

extern void arpMapCheckpointCallback(ARP_EVENT_TYPE_t arpEvent,
                                     L7_uint32 ipAddr,
                                     L7_uint32 intIfNum,
                                     L7_uchar8 *macAddr);

/* An ARP entry must be at least this old before we'll send an
 * ARP request triggered by software forwarding for L2 station learning. */
static const L7_uint32 TRIGGERED_ARP_INTERVAL = 30;   /* seconds */
/* Send out-of-cycle ARP request for every nth data packet software forwarded,
 * when ARP is already resolved. Software forwarding probably indicates the
 * next hop is not resolved to port and we need to trigger a unicast packet
 * from the neighbor to put him back in the L2 table. */
static const L7_uint32 ARP_SW_FWD_THRESH = 200;


/* local function prototypes */
static L7_BOOL ipMapArpIpAddrIsValid(L7_uint32 intIfNum, L7_uint32 ipAddr);


/*********************************************************************
* @purpose  Initializes the IP Mapping layer ARP component.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Causes ARP subsystem to be started if it is not already.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpInitPhase1Process(void)
{
  L7_uint32     i;
  ipMapArpIntf_t  *pIntf;
  t_ARPCfg arpCfg;
  t_ARPCfg *p_Prm = NULLP;
  e_Err         erc;
  L7_RC_t       rc;

  /* initialize component globals */
  ipMapArpCtxInit();
  for (i = 0; i < (L7_uint32)IPM_ARP_INTF_MAX; i++)
    ipMapArpIntfInUse_g[i] = L7_NULL;

  /* set up a dummy "system" interface structure to be used in certain cases
   * where a specific interface does not apply; assign to the unused interface
   * index 0 in the in-use array
   *
   * NOTE:  The key fields to set here are index=0 and intIfNum=0
   */
  pIntf = &ipMapArpSystemIntf;
  memset(pIntf, 0, sizeof(ipMapArpSystemIntf));
  pIntf->index = 0;
  pIntf->intIfNum = 0;
  ipMapArpIntfInUse_g[0] = pIntf;

  /* start up the ARP subsystem */
  if (ipMapArpL7Start() != L7_SUCCESS)
    return L7_FAILURE;

  /* initialize an ordered linked list for organizing interface info by
   * ascending IP address
   */
  if ((ipMapArpCtx_g.pIntfList =
      OLL_New(0, 2, offsetof(ipMapArpIntf_t, ipAddr), sizeof(L7_uint32),
      offsetof(ipMapArpIntf_t, intIfNum), sizeof(L7_uint32))) == 0)
    return L7_FAILURE;

  /* set up ARP configuration values (or use zero to keep code defaults) */
  /* NOTE:  Using factory defaults here to init ARP component.  Later, the
   *        current user config values are applied to override these defaults.
   */
  memset(&arpCfg, 0 ,sizeof(t_ARPCfg));
  arpCfg.arpAgeTime = (word)ipMapIpArpAgeTimeGet();
  arpCfg.arpRespTime = (word)ipMapIpArpRespTimeGet();
  arpCfg.arpCacheSize = (word)ipMapIpArpCacheSizeGet();
  arpCfg.retrNmb = (word)ipMapIpArpRetriesGet();
  arpCfg.dynamicRenew = (word)ipMapIpArpDynamicRenewGet();
  p_Prm = &arpCfg;

  /* create Layer 3 ARP manager object */
  if ((erc = ARP_Init(p_Prm, (t_Handle)NULLP, (t_Handle)NULLP, 0, &ipMapArpCtx_g.arpHandle))
      != E_OK)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "IPM_ARP: Failed to initialize L3 ARP Manager, rc=%d\n",
            erc);
    if (erc == E_NOMEMORY)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
              "Out of memory in routing heap.");
    }
    return L7_FAILURE;
  }

  /* set up a table for tracking gateway IP addresses of known routes
   */
  if ((rc = ipMapArpGwTableCreate()) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "IPM_ARP: Could not create gateway table, rc=%d\n", rc);
    return L7_FAILURE;
  }

  /* set up a table for searching ARP IP addresses in sequential order
   */
  if ((rc = ipMapArpIpTableCreate()) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "IPM_ARP: Could not create ARP IP lookup table, rc=%d\n", rc);
    return L7_FAILURE;
  }

  /* create the AVL component lock */
  ipMapArpCtx_g.arpLock.semId = osapiSemaBCreate (OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_FULL);
  if(ipMapArpCtx_g.arpLock.semId == 0)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "IPM_ARP: Could not create ARP component lock\n");
    return L7_FAILURE;
  }

#ifdef L7_NSF_PACKAGE
  if (arpMapCkptTableCreate() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_ARP_MAP_COMPONENT_ID,
           "IPM_ARP: Could not create ARP Checkpoint table");
    return L7_FAILURE;
  }
  ipMapArpCtx_g.ckptPendingSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if(!ipMapArpCtx_g.ckptPendingSema)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_ARP_MAP_COMPONENT_ID,
           "IPM_ARP: Error allocating ckptPendingSema");
    return L7_FAILURE;
  }
#endif

  if (ipMapArpStartTasks() != L7_SUCCESS)
    return L7_FAILURE;

  arpDebugCfgRead();
  arpApplyDebugConfigData();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initializes the IP Mapping layer ARP component.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipMapArpInitPhase2Process(void)
{
  L7_RC_t rc;

  LOG_INFO(LOG_CTX_STARTUP, "Going to register function 0x%08x to family DTL_FAMILY_ROUTING_ARP_MGMT (%u)",
           (L7_uint32) dtlIpv4ArpEntryCallback, DTL_FAMILY_ROUTING_ARP_MGMT);

  if ((rc = dtlCallbackRegistration(DTL_FAMILY_ROUTING_ARP_MGMT,
              dtlIpv4ArpEntryCallback)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "IPM_ARP: Could not register ARP Query callback with DTL, rc=%d\n",
            rc);
    return L7_FAILURE;
  }

#ifdef L7_NSF_PACKAGE
  if (arpMapCkptCallbacksRegister() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_ARP_MAP_COMPONENT_ID,
           "ARP MAP failed to register for checkpoint service callbacks.");
    return L7_FAILURE;
  }
#endif

  /*register for the debug function*/
  arpDebugRegister();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Fini's the IP Mapping layer ARP component Phase 1.
*
* @param    void
*
* @notes    cannot stop atic, if the atic system had failed
*           the box crashes
*
* @end
*********************************************************************/
void ipMapArpFiniPhase1Process(void)
{
  L7_RC_t rc;

  ipMapArpDeleteTasks();

  if ((rc = ipMapArpGwTableDestroy()) != L7_SUCCESS)
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "IPM_ARP: Could not destroy gateway table, rc=%d\n", rc);

  if ((rc = ipMapArpIpTableDestroy()) != L7_SUCCESS)
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "IPM_ARP: Could not destroy ARP IP lookup table, rc=%d\n", rc);

  (void) ARP_Delete(&ipMapArpCtx_g.arpHandle);

  osapiSemaDelete(ipMapArpCtx_g.arpLock.semId);
  ipMapArpCtx_g.arpLock.semId = 0;

  OLL_Delete(ipMapArpCtx_g.pIntfList);

  /* vendor routing subsystem cannot be destroyed once it has been initialized.
     The ipMapRoutingStarted flag will prevent us from re-starting routing subsystem */

}

/*********************************************************************
* @purpose  Fini's the IP Mapping layer ARP component Phase 2.
*
* @param    void
*
* @notes
*
* @end
*********************************************************************/
void ipMapArpFiniPhase2Process(void)
{
  dtlCallbackDeRegistration(DTL_FAMILY_ROUTING_ARP_MGMT);
}


/*********************************************************************
* @purpose  Route table change report callback function used for tracking
*           gateway addresses.
*
* @param    routeEntry  Contains one or more IP addresses that are
*                       now gateway addresses or are no longer gateway
*                       addresses
* @param    route_status RTO_ADD_ROUTE or RTO_DELETE_ROUTE
*                        (RTO_ROUTE_EVENT_t)
*
* @returns  void
*
* @notes    This function is called on the thread running the RTO code.
*
* @end
*********************************************************************/
void ipMapArpGwTableUpdate(L7_routeEntry_t *routeEntry, L7_uint32 route_status)
{
  L7_uint32 ipAddr;
  L7_uint32 intIfNum;
  L7_uint32 i;
  L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
  L7_BOOL comma = L7_FALSE;
  L7_BOOL staticRouteGw = L7_FALSE;

  /* skip local routes */
  if (routeEntry->protocol == RTO_LOCAL)
    return;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  if (ipMapTraceFlags & IPMAP_TRACE_ARP_GW)
  {
    sprintf(traceBuf, "ARP GW table %s:  ",
            (route_status == RTO_ADD_ROUTE) ? "ADD" : "DELETE");
  }

  for (i = 0; i < routeEntry->ecmpRoutes.numOfRoutes; i++)
  {
    ipAddr = routeEntry->ecmpRoutes.equalCostPath[i].arpEntry.ipAddr;
    intIfNum = routeEntry->ecmpRoutes.equalCostPath[i].arpEntry.intIfNum;
    if (ipAddr != 0)
    {
      if (ipMapTraceFlags & IPMAP_TRACE_ARP_GW)
      {
        L7_uchar8 nhAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
        osapiInetNtoa(ipAddr, nhAddrStr);
        if (comma)
          strcat(traceBuf, ", ");
        strncat(traceBuf, nhAddrStr, OSAPI_INET_NTOA_BUF_SIZE);
        comma = L7_TRUE;
      }
      switch (route_status)
      {
      case RTO_ADD_ROUTE:
        if (routeEntry->protocol == RTO_STATIC)
          staticRouteGw = L7_TRUE;
        if (ipMapArpGwTableInsert(ipAddr, intIfNum, staticRouteGw) != L7_SUCCESS)
        {
          L7_uchar8 ipStr[OSAPI_INET_NTOA_BUF_SIZE];
          osapiInetNtoa(ipAddr, ipStr);
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
                  "Unable to insert %s in ARP gateway table\n", (char *)ipStr);
        }
        break;

      case RTO_DELETE_ROUTE:
        if (ipMapArpGwTableRemove(ipAddr, intIfNum) != L7_SUCCESS)
        {
          L7_uchar8 ipStr[OSAPI_INET_NTOA_BUF_SIZE];
          osapiInetNtoa(ipAddr, ipStr);
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
                  "Unable to remove %s from ARP gateway table\n", (char *)ipStr);
        }
        break;

      default:
        break;
      }
    }
  }

  if (ipMapTraceFlags & IPMAP_TRACE_ARP_GW)
  {
    ipMapTraceWrite(traceBuf);
  }

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
}


/*********************************************************************
* @purpose  Clear the gateway table
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpGwTableClear(void)
{
  ipMapArpGwNode_t  *pNode;
  L7_uint32         count;
  ipMapArpGwNode_t  gw;

  if (ipMapTraceFlags & IPMAP_TRACE_ARP_GW)
  {
    ipMapTraceWrite("Clearing the ARP gateway table");
  }

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  count = avlTreeCount(&ipMapArpCtx_g.gwTbl.treeData);
  if (count == 0)
  {
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_SUCCESS;
  }

  /* Get the first node in the tree until the tree is empty */
  memset((L7_uchar8 *)&gw, 0, sizeof(gw));
  count = 0;
  while ((pNode = avlSearchLVL7(&ipMapArpCtx_g.gwTbl.treeData, &gw, AVL_NEXT)) != L7_NULL)
  {
    ipMapArpGwTableRemove(pNode->ipAddr, pNode->intIfNum);
  }
  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Registers a routing interface with the IP Mapping layer ARP
*           function to allow participation in ARP operation.
*
* @param    intIfNum    internal interface number
* @param    pParms      registration parms for this interface
*                         (@b{Input:} Note that the caller MUST fill in a
*                          structure of type ipMapArpIntfRegParms_t COMPLETELY
*                          before calling this function)
*
* @returns  L7_SUCCESS  router interface registration was successful
* @returns  L7_ERROR    maximum number of router interfaces already registered
* @returns  L7_FAILURE  program or usage error
*
* @notes    Each routing interface needs to register with IPM ARP in order
*           to be recognized for ARP-related operations, such as IP address
*           resolution.  The intent is to do this dynamically as routing
*           interfaces are enabled by the user, as opposed to statically
*           registering during system initialization.
*
* @notes    See ipMapArpIntfUnregister() to remove a routing interface from
*           the IPM ARP operation.  If a routing interface is re-configured
*           from one subnet to another, the old interface must first be
*           unregistered before the new interface definition is registered.
*
* @notes    The pParms->intfHandle is the interface context as it relates to
*           the caller (e.g., an END object ptr).  It is treated as an
*           opaque value in the IP Mapping Layer.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIntfRegister(L7_uint32 intIfNum,
                             ipMapArpIntfRegParms_t *pParms)
{
  L7_uint32         i;
  ipMapArpIntf_t    *pIntf;
  e_Err             erc;

  if (pParms == L7_NULL)
    return L7_FAILURE;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  /* find first available interface index (skip element 0) */
  for (i = 1; i < (L7_uint32)IPM_ARP_INTF_MAX; i++)
    if (ipMapArpIntfInUse_g[i] == L7_NULL)
      break;
  if (i == (L7_uint32)IPM_ARP_INTF_MAX)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "IPM_ARP: Out of resource for intf register (obj=0x%8.8x)\n",
            pParms->intfHandle);
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_ERROR;
  }
  /* use a non-zero value as a TEMPORARY marker to hold this spot in the list */
  ipMapArpIntfInUse_g[i] = (ipMapArpIntf_t *)(-1);

  /* build local interface instance reference and chain to IPM list */
  pIntf = (ipMapArpIntf_t *)XX_Malloc(sizeof(ipMapArpIntf_t));
  if (pIntf == NULL)
  {
    ipMapArpIntfInUse_g[i] = L7_NULL;
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "IPM_ARP: Unable to allocate local intf instance for obj=0x%8.8x\n",
            pParms->intfHandle);
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_ERROR;
  }
  memset(pIntf, 0, sizeof(ipMapArpIntf_t));
  pIntf->next = pIntf->prev = L7_NULL;  /* init list chain ptrs */
  pIntf->index = i;
  pIntf->intIfNum = intIfNum;
  memcpy(pIntf->l3Addr.lay3Addr, &pParms->ipAddr, L7_IP_ADDR_LEN);
  pIntf->l3Addr.protocolId = 0;        /* IPv4 */
  pIntf->ipAddr = pParms->ipAddr;
  pIntf->netMask = pParms->netMask;
  pIntf->unnumbered = pParms->unnumbered;
  pIntf->proxyArp = pParms->proxyArp;
  pIntf->localProxyArp = pParms->localProxyArp;
  pIntf->netId = pParms->ipAddr & pParms->netMask;
  memcpy(pIntf->macAddr, pParms->pMacAddr, L7_MAC_ADDR_LEN);
  pIntf->rawSendFn = pParms->rawSendFn;
  pIntf->sendBufSize = pParms->sendBufSize;
  pIntf->intfHandle = pParms->intfHandle;

  if ((erc = OLL_Insert(ipMapArpCtx_g.pIntfList, pIntf)) != E_OK)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "IPM_ARP: OLL_Insert failed for arp intf %u, rc=%d\n",
            pIntf->index, erc);
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_FAILURE;
  }
  ipMapArpIntfInUse_g[pIntf->index] = pIntf;

  /* add this interface to the ARP object */
  if ((erc = ARP_AddInterface(ipMapArpCtx_g.arpHandle, (F_ARPTransmit) ipMapArpSendCallback,
                               (t_Handle)pIntf, (t_LIH)pIntf->index)) != E_OK)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "IPM_ARP: ARP_AddInterface failed for arp intf %u, rc=%d\n",
            pIntf->index, erc);
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_FAILURE;
  }

  /* install the IP and MAC address of this interface as a local ARP entry */
  if ((erc = ARP_AddLocAddr(ipMapArpCtx_g.arpHandle, (t_IPAddr *)pIntf->l3Addr.lay3Addr,
                         (t_MACAddr *)(byte *)pIntf->macAddr, (word)pIntf->index)) != E_OK)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "IPM_ARP: ARP_AddLocalAddr failed for IP addr 0x%8.8x " \
            "(arp intf %u), rc=%d\n", pIntf->ipAddr, pIntf->index, erc);
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_FAILURE;
  }

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Unregisters a routing interface from the IP Mapping layer ARP
*           function.
*
* @param    intIfNum    internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    See ipMapArpIntfRegister() to register a routing interface with
*           the IPM ARP operation.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIntfUnregister(L7_uint32 intIfNum)
{
  ipMapArpIntf_t    *pIntf;
  L7_RC_t           rc = L7_SUCCESS;
  e_Err             erc;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  /* obtain interface instance */
  if (ipMapArpIntfInstanceGet(intIfNum, &pIntf) != L7_SUCCESS)
  {
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_FAILURE;
  }

  /* remove the IP and MAC address of this interface from the local ARP cache */
  if ((erc = ARP_DelLocAddr(ipMapArpCtx_g.arpHandle,
                            (t_IPAddr *)pIntf->l3Addr.lay3Addr,
                            (word)pIntf->index)) != E_OK)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "IPM_ARP: ARP_DelLocalAddr failed for IP addr 0x%8.8x " \
            "(arp intf %u), rc=%d\n", pIntf->ipAddr, pIntf->index, erc);
    rc = L7_FAILURE;
    /* keep going */
  }

  /* delete this interface from the ARP object */
  if ((erc = ARP_DelInterface(ipMapArpCtx_g.arpHandle, (t_LIH)pIntf->index))
      != E_OK)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "IPM_ARP: ARP_DelInterface failed for arp intf %u, rc=%d\n",
            pIntf->index, erc);
    rc = L7_FAILURE;
    /* keep going */
  }

  /* delete the local interface instance from the linked list and mark
   * its interface index as unused.
   */
  if ((erc = OLL_Extract(ipMapArpCtx_g.pIntfList, pIntf)) != E_OK)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "IPM_ARP: OLL_Extract failed for ARP intf %u, rc=%d\n",
            pIntf->index, erc);
    rc = L7_FAILURE;
    /* keep going */
  }
  ipMapArpIntfInUse_g[pIntf->index] = L7_NULL;

  XX_Free(pIntf);

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
  return rc;
}

/*********************************************************************
* @purpose  Attempts to resolve an IP address to an Ethernet MAC address.
*
* @param    intIfNum    internal interface number
* @param    ipAddr      layer 3 IP address to resolve
* @param    *pMacAddr   ptr to output buffer where MAC address is returned if
*                       ipAddr is already in the ARP cache
* @param    pCallbackFn address of callback function for asynchronous response
* @param    cbParm1     callback parameter 1
* @param    cbParm2     callback parameter 2
*
* @returns  L7_SUCCESS          address was resolved, MAC addr returned via pMacAddr
* @returns  L7_ASYNCH_RESPONSE  resolution in progress. will notify asynchronously
*                               through callback.
* @returns  L7_FAILURE          cannot resolve address
*
* @notes    An L7_ASYNCH_RESPONSE return code indicates the address was not found in
*           the ARP cache and is in the process of being resolved.  If the
*           pCallbackFn pointer is non-null, an asynchronous callback will be
*           made indicating the outcome of the resolution attempt.  If the
*           pCallbackFn is null, there will not be a notification should the
*           address get resolved.
*
* @notes    An L7_FAILURE can occur due to temporary resource constraints
*           (such as a full queue), or other programming/usage errors.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpAddrResolve(L7_uint32 intIfNum, L7_uint32 ipAddr,
                            L7_uchar8 *pMacAddr,
                            ipMapArpResCallback_ft pCallbackFn,
                            L7_uint32 cbParm1, L7_uint32 cbParm2)
{
  L7_RC_t rc;
  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);
  rc = ipMapArpAddrResolveInternal(intIfNum, ipAddr,
                                   pMacAddr, pCallbackFn,
                                   cbParm1, cbParm2);

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
  return rc;
}

/*********************************************************************
* @purpose  Called when an ARP entry is used for software forwarding.
*
* @param    ipAddr      IP address of neighbor
* @param    intIfNum    internal interface number of interface where packet
*                       is forwarded
*
* @notes    Increments a per ARP entry counter. If entry is older than
*           L2 table age, send an occasional ARP request to stimulate
*           station learning.
*
* @end
*********************************************************************/
void ipMapArpAddrUsed(L7_uint32 ipAddr, L7_uint32 intIfNum)
{
  t_ADR *arpEntry;
  t_ARP *p_A = (t_ARP*) ipMapArpCtx_g.arpHandle;
  ulng ageSecs;
  ipMapArpIntf_t *pIntf = NULL;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  /* Get the ARP interface */
  if (ipMapArpIntfInstanceGet(intIfNum, &pIntf) != L7_SUCCESS)
  {
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return;
  }
  if (ipMapArpExtenCacheFind(p_A->arpCacheId, (byte*)&ipAddr, pIntf->index,
                             &arpEntry) == E_OK)
  {
    arpEntry->swFwdCount++;
    /* If entry's age is more than the default L2 table age, send
     * an extra ARP request now and then to try to update the L2 table. */
    ipMapArpExtenEntryAgeCalc(arpEntry->timeStamp, &ageSecs);
    if ((ageSecs > TRIGGERED_ARP_INTERVAL) &&
        ((arpEntry->swFwdCount % ARP_SW_FWD_THRESH) == 0))
    {
      ARP_SendARPReqNoRxmt(ipMapArpCtx_g.arpHandle, (byte*)&ipAddr, pIntf->index);
    }
  }
  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
}

/*********************************************************************
* @purpose  Sends a gratuitous ARP on a specified interface.
*
* @param    intIfNum    internal interface number of interface where ARP
*                       is to be sent.
* @param    localIpAddr local IP address to be sent as the target IP
*
* @returns  L7_SUCCESS  gratuitous ARP successfully sent
* @returns  L7_FAILURE  failed to send gratuitous ARP
*
* @notes    The initial motivation for this is to advise our neighbors
*           of a new MAC address if the interface has bounced as a
*           result of a top-of-stack change. Sending a gratuitous ARP
*           can also identify if another station on the attached subnet
*           has been (mis)configured with the same IP address as is
*           configured on the local interface.
*
*           Unlike ipMapAddrArpResolve(), this function does not check the
*           ARP cache, since local IP addresses will be in the ARP cache. This
*           function does not accept a callback function, since we don't expect
*           to receive a response (and no action is pending on a response).
*           If we do receive a response, we will handle it in our normal
*           ARP receive processing.
*
* @notes    It is necessary to specify localIpAddr since an interface may have
*           multiple IP addresses configured. This function needs to be called
*           separately for each local address.
*
* @end
*********************************************************************/
L7_RC_t ipMapGratArpSend(L7_uint32 intIfNum, L7_uint32 localIpAddr)
{
  ipMapArpIntf_t  *pIntf;
  t_ARPResInfo  arpResInfo;
  e_Err         erc;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  /* obtain interface instance */
  if (ipMapArpIntfInstanceGet(intIfNum, &pIntf) != L7_SUCCESS)
  {
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_FAILURE;
  }

  /* set up user portion of ARP resolution info block */
  memset(&arpResInfo, 0, sizeof(t_ARPResInfo));
  memcpy(arpResInfo.destAddr.lay3Addr, &localIpAddr, L7_IP_ADDR_LEN);
  arpResInfo.destAddr.protocolId = 0;   /* IPv4 */
  arpResInfo.lanNmb = (t_LIH)pIntf->index;

  /* invoke the vendor ARP resolution function */
  erc = ARP_SendGratARP((t_ARP*) ipMapArpCtx_g.arpHandle,
                       arpResInfo.destAddr.lay3Addr, (word)arpResInfo.lanNmb);

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
  return ((erc == E_OK) ? L7_SUCCESS : L7_FAILURE);
}

/*********************************************************************
* @purpose  Receive an ARP packet and pass it to the ARP code.
*
* @param    intIfNum    internal interface number
* @param    *pArpData   ptr to flat data buffer containing the layer 3 ARP
*                         packet
*
* @returns  L7_SUCCESS  packet processed
* @returns  L7_ERROR    packet not processed
* @returns  L7_FAILURE  function failure
*
* @notes    The pData parm must point to the beginning of the layer 3
*           portion of the ARP packet, skipping over all layer 2 header
*           contents (including VLAN tags, etc.).
*
* @notes    This function assumes the packet is contained in a single,
*           contiguous data buffer.
*
* @notes    The packet contents are copied into an ARP frame object.
*           This function never assumes ownership of the original
*           packet buffer, so the caller must take care of its disposal.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpReceive(L7_uint32 intIfNum, L7_uchar8 *pArpData)
{
  ipMapArpIntf_t  *pIntf;
  void          *pFrame = L7_NULL;
  L7_uint32     copyLen;
  ipMapArpPkt_t *pArp;
  L7_uint32     arpSrcIp;
  L7_uint32 arpTgtIp;
  L7_uint32 targetIp;
  static L7_uint32 lastTgtIpLog = 0;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  /* obtain interface instance */
  if (ipMapArpIntfInstanceGet(intIfNum, &pIntf) != L7_SUCCESS)
  {
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_FAILURE;
  }

  /* check the ARP packet (request or reply) source IP address for
   * legitimacy
   */
  pArp = (ipMapArpPkt_t *)pArpData;
  memcpy(&arpSrcIp, pArp->srcIp, L7_IP_ADDR_LEN);

  if (ipMapArpIpAddrIsValid(intIfNum, osapiNtohl(arpSrcIp)) == L7_FALSE)
  {
    ipMapArpCtx_g.inSrcDiscard++;
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_SUCCESS;
  }

  /* Also drop ARP packets with bad target IP */
  memcpy(&arpTgtIp, pArp->dstIp, L7_IP_ADDR_LEN);
  targetIp = osapiNtohl(arpTgtIp);
  /* reject an address of 0 or class D, E, or beyond */
  if ((targetIp == 0) ||
      (targetIp >= (L7_uint32)L7_CLASS_D_ADDR_NETWORK) ||
      (((targetIp & IN_CLASSA_NET) >> IN_CLASSA_NSHIFT) == IN_LOOPBACKNET))
  {
    if (lastTgtIpLog != osapiUpTimeRaw())    /* rate limit log msgs to 1 per sec */
    {
      L7_uchar8 srcIpStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 tgtIpStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 srcMacStr[32];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      L7_uint32 i;
      L7_uchar8 macByte[8];
      L7_ushort16 msgType;      /* host byte order */
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      osapiInetNtoa(targetIp, tgtIpStr);
      osapiInetNtoa(osapiNtohl(arpSrcIp), srcIpStr);
      srcMacStr[0] = '\0';
      for (i = 0; i < L7_MAC_ADDR_LEN; i++)
      {
        if (i < (L7_MAC_ADDR_LEN - 1))
          sprintf(macByte, "%2.2x:", pArp->srcMac[i]);
        else
          sprintf(macByte, "%2.2x", pArp->srcMac[i]);
        strncat(srcMacStr, macByte, 3);
      }
      msgType = osapiNtohs(*(L7_ushort16*)pArp->opCode);
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_ARP_MAP_COMPONENT_ID,
                    "Received ARP %s on interface %s with bad target IP address %s. "
                    "Sender IP is %s, sender MAC is %s.",
                    (msgType == L7_ARPOP_REQUEST) ? "Request" : "Reply",
                    ifName, tgtIpStr, srcIpStr, srcMacStr);
      lastTgtIpLog = osapiUpTimeRaw();
    }

    ipMapArpCtx_g.inTgtDiscard++;
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_SUCCESS;
  }

  /* get a frame object from the vendor arp code */
  pFrame = F_New((void *)NULLP);
  if (pFrame == L7_NULL)
  {
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_ERROR;
  }

  /* copy ARP packet from caller's buffer to the frame object */
  copyLen = F_AddToEnd(pFrame, (byte *)pArpData, (word)IPM_ARP_PKT_LEN);
  if (copyLen != (L7_uint32)IPM_ARP_PKT_LEN)
  {
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_ERROR;
  }

  /* NOTE:  Not checking the opcode or anything here.  Different vendor ARP
   *        implementations may support different features (e.g., RARP), so
   *        let their code decide whether to process the packet or not.
   */

  /* invoke ARP code receive function */
   if (ARP_Receive(ipMapArpCtx_g.arpHandle, pFrame, intIfNum, pIntf->index) != E_OK)
   {
     IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
     return L7_ERROR;
   }

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
  return L7_SUCCESS;
}


/*-----------------------------------------------------------------------*/
/* GENERAL APIs:  Callable by any app, not just routing intf objects.    */
/*-----------------------------------------------------------------------*/

/*********************************************************************
* @purpose  Adds a static entry to the ARP table.
*
* @param    ipAddr      layer 3 IP address to resolve
* @param    intIfNum    Internal interface number.
* @param    *pMacAddr   ptr to buffer containing layer 2 MAC address
*
* @returns  L7_SUCCESS  entry added to the ARP table
* @returns  L7_NOT_EXIST the router does not have an interface in the same
*                        subnet as the target of the static entry
* @returns  L7_ERROR    entry could not be added
* @returns  L7_FAILURE  program malfunction or improper usage
*
* @notes    An L7_ERROR return code indicates the static entry could not
*           be added to the ARP table.  This could occur, for example,
*           because of internal resource limits (perhaps temporary).
*
* @notes    Do not use this function to add a net-directed broadcast
*           ARP entry for a local routing interface.  These are handled
*           by the ipMapArpNetDirBcastAdd() function.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpStaticEntryAdd(L7_uint32 ipAddr, L7_uint32 intIfNum,
                               L7_uchar8 *pMacAddr)
{
  t_ARPStatInfo     arpStatInfo;
  L7_uchar8         nullMac[L7_MAC_ADDR_LEN] = {0,0,0,0,0,0};
  ipMapArpIntf_t    *pIntf;
  ipMapArpIpNode_t  *pIp;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  /* reject IP address if it is one of the local ARP entries */
  if ((ipMapArpIpTableSearch(ipAddr, intIfNum,
                             (L7_uint32)L7_MATCH_EXACT, &pIp) == L7_SUCCESS) &&
      (pIp->arpEntry.entryType == ARP_ENTRY_TYPE_LOCAL))
  {
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_FAILURE;
  }

  /* never allow a null MAC address to be used
   * NOTE: checking (again) here to protect vendor ARP logic
   */
  if (memcmp(pMacAddr, nullMac, L7_MAC_ADDR_LEN) == 0)
  {
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_FAILURE;
  }

  /* A valid interface must have be passed as param */
  if ( intIfNum == L7_INVALID_INTF ||
      ipMapArpIntfInstanceGet(intIfNum, &pIntf) != L7_SUCCESS)
  {
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_FAILURE;
  }

  /* set up static ARP info block */
  memset(&arpStatInfo, 0, sizeof(t_ARPStatInfo));
  memcpy(arpStatInfo.l3Addr.lay3Addr, &ipAddr, L7_IP_ADDR_LEN);
  arpStatInfo.l3Addr.protocolId = 0;   /* IPv4 */
  arpStatInfo.lanNmb = (t_LIH)pIntf->index;
  memcpy(arpStatInfo.l2Addr, pMacAddr, L7_MAC_ADDR_LEN);

  /* check if ARP table already contains the maximum static entries */
  if (ipMapArpIpTableStaticMaxCheck() == L7_TRUE)
  {
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_ERROR;
  }

  /* call the ARP object to add this static entry */
  if (ARP_AddDstAddr(ipMapArpCtx_g.arpHandle,
                    (t_IPAddr *)arpStatInfo.l3Addr.lay3Addr,
                    (t_MACAddr *)arpStatInfo.l2Addr, (word)arpStatInfo.lanNmb)
      != E_OK)
  {
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_ERROR;
  }

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Deletes a static entry from the ARP table.
*
* @param    ipAddr      layer 3 IP address to resolve
* @param    intIfNum    Internal interface number of entry
*
* @returns  L7_SUCCESS  entry deleted from the ARP table
* @returns  L7_ERROR    entry could not be deleted
* @returns  L7_FAILURE  program malfunction or improper usage
*
* @notes    An L7_ERROR return code indicates the static entry could not
*           be deleted from the ARP table.  This could occur, for example,
*           because the entry was not found, or there is no routing
*           routing interface registered for a subnet to which this
*           address belongs.
*
* @notes    Do not use this function to delete a net-directed broadcast
*           ARP entry for a local routing interface.  These are handled
*           by the ipMapArpNetDirBcastDel() function.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpStaticEntryDel(L7_uint32 ipAddr, L7_uint32 intIfNum)
{
  t_ARPStatInfo arpStatInfo;
  ipMapArpIntf_t     *pIntf;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  /* set up static ARP info block */
  memset(&arpStatInfo, 0, sizeof(t_ARPStatInfo));
  memcpy(arpStatInfo.l3Addr.lay3Addr, &ipAddr, L7_IP_ADDR_LEN);

  /* obtain interface instance */
  if (ipMapArpIntfInstanceGet(intIfNum, &pIntf) != L7_SUCCESS)
  {
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_FAILURE;
  }

  /* call the ARP object to delete this static entry */
  if (ARP_DelDstAddr(ipMapArpCtx_g.arpHandle,
                    (t_IPAddr *)arpStatInfo.l3Addr.lay3Addr,
                     pIntf->index) != E_OK)
  {
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_ERROR;
  }

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Checks if an IP address is considered valid for entry into
*           the ARP table.
*
* @param    intIfNum    internal interface number
* @param    ipAddr      IP address
*
* @returns  L7_TRUE     address is acceptable
* @returns  L7_FALSE    address not acceptable for an ARP entry
*
* @notes    An internal interface number of 0 means the checking is not
*           associated with a specific interface.  The best that can be
*           done in that case is to use the natural network mask.  This
*           is the case when the user adds a static ARP entry, for example.

* @notes    The intent of this function is to qualify an IP address as
*           one that could be added to an ARP table.  It rejects addresses
*           such as class D, net-directed broadcast, etc., since these
*           should never be added by the network or a user.
*
* @notes    Assumes the caller locks the ARP component
*
* @end
*********************************************************************/
static L7_BOOL ipMapArpIpAddrIsValid(L7_uint32 intIfNum, L7_uint32 ipAddr)
{
  ipMapArpIntf_t     *pIntf;
  L7_uint32          netMask, netId, netDirBcast;
  L7_uint32 localAddr = 0;
  L7_uint32 localMask = 0;

  /* We need to respond to ARP probes that come with 0.0.0.0 senderIP
   * as per RFC 5227 section 2.1.1 */
  if (ipAddr == (L7_uint32)L7_NULL_IP_ADDR)
    return L7_TRUE;

  /* reject an address if class D, E, or beyond */
  if (ipAddr >= (L7_uint32)L7_CLASS_D_ADDR_NETWORK)
    return L7_FALSE;

  /* reject the loopback address */
  if (((ipAddr & IN_CLASSA_NET) >> IN_CLASSA_NSHIFT) == IN_LOOPBACKNET)
    return L7_FALSE;

  /* determine whether caller wants interface-based checking to occur */
  if (intIfNum == 0)
  {
    /* set up class-based check values */
    if (IN_CLASSA(ipAddr))
      netMask = IN_CLASSA_NET;
    else if (IN_CLASSB(ipAddr))
      netMask = IN_CLASSB_NET;
    else
      netMask = IN_CLASSC_NET;
    netId = ipAddr & netMask;
  }
  else
  {
    /* Ignore source address check for un-numbered interfaces */
    if (ipMapArpIntfInstanceGet(intIfNum, &pIntf) == L7_SUCCESS &&
        ipMapArpExtenIsUnnumbIntf(pIntf->index))
    {
      return L7_TRUE;
    }

    if (ipMapArpSubnetFind(ipAddr, &pIntf, &localAddr, &localMask)!=L7_SUCCESS)
    {
      /* address not in local subnet on this interface. Reject it. */
      return L7_FALSE;
    }

    /* If interface is specified, validate it against configured interface */
    if ( pIntf->intIfNum != intIfNum)
    {
      /* IP address part of subnet on different interface */
      return L7_FALSE;
    }

    netMask = localMask;
    netId = localAddr & localMask;
  }

  netDirBcast = netId | (~netMask);

  /* check for a subnet mismatch
   * NOTE:  For the intIfNum = 0 case, the IP addr always matches the subnet
   */
  if ((ipAddr & netMask) != netId)
    return L7_FALSE;

  /* reject an all-zero or all-ones host portion */
  if ((ipAddr == netId) || (ipAddr == netDirBcast))
    return L7_FALSE;

  return L7_TRUE;
}



/*********************************************************************
* @purpose  Gets ARP entry in the ARP table maintained in the
* @purpose  Application ARP table.
*
* @param    pArp->ipAddr @b{(input)} The ip address of the requested
*                        arp entry.
* @param    *pArp        @b{(output)} A reference to the ARP entry object
*                        containing information needed to add an entry to
*                        the ARP table.
*
*
* @returns  L7_SUCCESS  on a successful addition of the ARP entry
* @returns  L7_FAILURE  if the addition failed
* @returns  L7_ERROR    invalid parameter received.
*
* @notes    ARP table entries correlate an IP address to a MAC address and
*           Router Interface. The table is built as a result of static entries
*           and information obtained from ARP requests and replies. Dynamic
*           entries are periodically aged.
*
* @notes    The ARP information is obtained from the APPLICATION ARP TABLE
*           and NOT the DRIVER & BELOW as is normally the case with dtl
*           API.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpEntryGet(L7_arpEntry_t *pArp)
{
  ipMapArpResCallback_ft  pCallbackFnNull = L7_NULLPTR;
  ipMapArpIntf_t  *pIntf;
  L7_uint32 vlanId;
  L7_RC_t rc = L7_FAILURE;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  if ( pArp->intIfNum == L7_INVALID_INTF ||
      ipMapArpIntfInstanceGet(pArp->intIfNum, &pIntf) != L7_SUCCESS)
  {
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_FAILURE;
  }


  if (ipMapArpAddrResolveInternal(pIntf->intIfNum, pArp->ipAddr,
                                  pArp->macAddr.addr.enetAddr.addr,
                                  pCallbackFnNull, 0, 0) == L7_SUCCESS)
  {
    pArp->intIfNum = pIntf->intIfNum;
    if(ipMapVlanRtrIntIfNumToVlanId(pIntf->intIfNum, &vlanId) == L7_SUCCESS)
      pArp->vlanId = (L7_ushort16)vlanId;
    else
      pArp->vlanId = 0;

    rc = L7_SUCCESS;
  }

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
  return rc;
}

/*********************************************************************
* @purpose  Handle the ARP entry add collision event in the hardware
*           Winnow the arp cache whenever the ARP entries have
*           crossed a threshold or a h/w collision event occurs
*
* @param    none
*
* @returns  L7_SUCCESS  on a successful handling of the event
* @returns  L7_FAILURE  if the handling failed
*
* @notes    purge the arp entries in the cache and hardware whose
*           last-hit-time is more than L7_IP_ARP_CACHE_LAST_HIT_TIME
*
* @end
*********************************************************************/
L7_RC_t ipMapArpCollisionProcess(void)
{
  L7_uint32 startTime = osapiTimeMillisecondsGet();
  L7_uint32 initialCacheSize;
  L7_uint32 finalCacheSize;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  initialCacheSize = ipMapArpCtx_g.cacheCurrCt;

  ipMapArpExtenPurgeOldEntries();

  finalCacheSize = ipMapArpCtx_g.cacheCurrCt;

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);

  L7_LOGF(L7_LOG_SEVERITY_INFO, L7_ARP_MAP_COMPONENT_ID,
          "ARP hash collision. Reducing ARP cache from %u to %u entries in %u msec.",
          initialCacheSize, finalCacheSize, osapiTimeMillisecondsGetOffset(startTime));

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Purge a specific dynamic entry from the ARP cache.
*
* @param    ipAddr      IP address of entry to be purged (in host format)
* @param    intIfNum    Internal interface number of the entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The ARP entries for local router interfaces and static ARP
*           entries are not affected by this command.
*
* @notes    To delete a static ARP entry, use the ipMapArpStaticEntryDel()
*           API command.
*
* @notes    Deletion of a dynamic "gateway" ARP entry causes the entry to be
*           removed from the CPU ARP table; its removal from the NP FIB depends
*           on the conditions determined by the ARP change callback function.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpEntryPurge(L7_uint32 ipAddr, L7_uint32 intIfNum)
{
  t_ARP             *pArpObj = (t_ARP *)ipMapArpCtx_g.arpHandle;
  ipMapArpIpNode_t  *pIp;
  L7_uchar8         ipStr[OSAPI_INET_NTOA_BUF_SIZE];
  t_Lay3Addr        l3addr;
  ipMapArpIntf_t    *pIntf;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  if (ipMapArpIpTableSearch(ipAddr, intIfNum, (L7_uint32)L7_MATCH_EXACT, &pIp) != L7_SUCCESS)
  {
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_FAILURE;
  }

  if (pIp->arpEntry.permanent == TRUE)
  {
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_FAILURE;
  }

  /* set up print string before deleting the entry */
  osapiInetNtoa(ipAddr, ipStr);

  /* copy some info and release the ipTbl sema before calling down */
  memset(&l3addr, 0, sizeof(l3addr));
  memcpy(l3addr.lay3Addr, &ipAddr, L7_IP_ADDR_LEN);
  l3addr.protocolId = 0;                /* IPv4 */

  /* obtain interface instance */
  if (ipMapArpIntfInstanceGet(intIfNum, &pIntf) != L7_SUCCESS)
  {
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_FAILURE;
  }

  if (ARP_DelDstAddr(pArpObj, (t_IPAddr *)l3addr.lay3Addr, pIntf->index) != E_OK)
  {
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_FAILURE;
  }

  /* debug message */
  IPM_ARP_PRT(IPM_ARP_MSGLVL_MED, "IPM_ARP: purged %s\n", ipStr);

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clears the ARP table of dynamic (and gateway) entries.
*
* @param    intIfNum    internal interface number whose entries should be
*                         purged, or 0 for all interfaces
*
* @param    allowGw     denotes whether gateway entries should be cleared
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The ARP entries for local router interfaces and static ARP
*           entries are not affected by this command.
*
* @notes    If the allowGw parm is set to L7_TRUE, dynamic ARP entries labeled
*           as "gateway" entries are removed from the CPU ARP table; their
*           removal from the device FIB depends on the conditions determined by
*           the ARP change callback function.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpCacheClear(L7_uint32 intIfNum, L7_BOOL allowGw)
{
  ipMapArpIntf_t  *pIntf;
  t_ARP         *pArpObj = (t_ARP *)ipMapArpCtx_g.arpHandle;
#ifdef L7_NSF_PACKAGE
  L7_uchar8     macAddr[L7_MAC_ADDR_LEN];
#endif

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  if (ipMapArpIntfInstanceGet(intIfNum, &pIntf) != L7_SUCCESS)
  {
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_FAILURE;
  }


  /* debug message */
  IPM_ARP_PRT(IPM_ARP_MSGLVL_MED, "IPM_ARP: cache clear started\n");

#ifdef L7_NSF_PACKAGE
  if(IP_MAP_ARP_INTF_ALL == intIfNum)
  {
    memset(macAddr, 0, sizeof(macAddr));
    /* Calling the checkpointing code for NSF to flush all ARP entries */
    arpMapCheckpointCallback(ARP_ENTRY_FLUSH_ALL, 0, 0,
                             macAddr);
  }
#endif

  if (ARP_ClearCache(pArpObj, pIntf->index, (Bool)allowGw) != E_OK)
  {
    /* debug message */
    IPM_ARP_PRT(IPM_ARP_MSGLVL_MED, "IPM_ARP: cache clear finished with error\n");
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_FAILURE;
  }

  /* debug message */
  IPM_ARP_PRT(IPM_ARP_MSGLVL_MED, "IPM_ARP: cache clear finished\n");

  /* Try to reacquire gateway addresses for static routes. */
  ipMapArpGwTableStaticRenew();

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Retrieve various ARP cache statistics
*
* @param    *pStats     @b{(input)} pointer to ARP cache stats structure
*                                   to be filled in
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Primarily used for 'show' functions.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpCacheStatsGet(L7_arpCacheStats_t *pStats)
{
  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  if (pStats == L7_NULLPTR)
  {
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_FAILURE;
  }

  /* call the ARP object to obtain counts in a controlled manner */
  if (ARP_GetCacheStats(ipMapArpCtx_g.arpHandle, pStats) != E_OK)
  {
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_FAILURE;
  }

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Registers a secondary IP Address configured on a routing
*           interface with the IP Mapping layer ARP function to allow
*           participation in ARP operation.
*
* @param    intIfNum      Internal interface number
* @param    primaryIp     primary IP address on interface where secondary is added
* @param    secondaryIp   Secondary IP Address to be registered with ARP
* @param    secondaryMask Mask associated with the secondary IP Address
*
* @returns  L7_SUCCESS  router interface registration was successful
* @returns  L7_ERROR    could not find routing interface
* @returns  L7_FAILURE  program or usage error
*
* @notes    Assumes the routing table object (RTO) component was previously
*           initialized.
*           Uses primary IP address to identify the interface.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpSecondaryIpAddrRegister(L7_uint32 intIfNum,
                                        L7_uint32 primaryIp,
                                        L7_uint32 secondaryIp,
                                        L7_uint32 secondaryMask)
{
  ipMapArpIntf_t  *pIntf = L7_NULLPTR;
  e_Err           erc;
  t_Lay3Addr      l3Addr;
  L7_uchar8       mac[L7_MAC_ADDR_LEN];
  L7_uchar8       ipStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uint32 i;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  /* Clean up data structures */

  memset((L7_char8 *)&l3Addr, 0, sizeof(t_Lay3Addr));
  memset((L7_char8 *)mac, 0, L7_MAC_ADDR_LEN);
  memset((L7_char8 *)ipStr, 0, OSAPI_INET_NTOA_BUF_SIZE);

    pIntf = (ipMapArpIntf_t *)OLL_Find(ipMapArpCtx_g.pIntfList, &primaryIp, &intIfNum);

    /* Sanity checking */

    if ((pIntf == L7_NULLPTR) || (secondaryIp == L7_NULL_IP_ADDR))
    {
        IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_ERROR;
    }

  for (i = 0; i < L7_L3_NUM_SECONDARIES; i++)
  {
    if (pIntf->secondaries[i].ipAddr == 0)
    {
      pIntf->secondaries[i].ipAddr = secondaryIp;
      pIntf->secondaries[i].ipMask = secondaryMask;
      break;
    }
  }
  if (i == L7_L3_NUM_SECONDARIES)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_ARP_MAP_COMPONENT_ID,
            "Failed to add secondary address to ARP interface %s.",
            ifName);
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_ERROR;
  }

  memcpy((L7_char8 *)l3Addr.lay3Addr, (L7_char8 *)&secondaryIp, L7_IP_ADDR_LEN);
  l3Addr.protocolId = 0;

  /*
  Install the specified secondary IP and MAC address of this interface as a local
  ARP entry
  */
  if ((erc = ARP_AddLocAddr(ipMapArpCtx_g.arpHandle, (t_IPAddr *)l3Addr.lay3Addr,
                (t_MACAddr *)(byte *)pIntf->macAddr, (word)pIntf->index)) != E_OK)
  {
    osapiInetNtoa((L7_uint32)secondaryIp, ipStr);
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "IPM_ARP: ARP_AddLocalAddr failed for IP addr %-15.15s " \
        "(arp intf %u), rc=%d\n", ipStr, pIntf->index, erc);

    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_FAILURE;
  }

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Unregisters the specified secondary ip address with the
*           ARP component
*
* @param    intIfNum      Internal interface number
* @param    primaryIp     Primary IP address to on interface where secondary is removed
* @param    secondaryIp   Secondary IP Address to be de-registered with ARP
* @param    secondaryMask Mask associated with the secondary IP Address
*
* @returns  L7_SUCCESS  router interface de-registration was successful
* @returns  L7_ERROR    could not find routing interface
* @returns  L7_FAILURE  program or usage error
*
* @notes    Assumes the routing table object (RTO) component was previously
*           initialized.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpSecondaryIpAddrUnregister(L7_uint32 intIfNum,
                                          L7_uint32 primaryIp,
                                          L7_uint32 secondaryIp,
                                          L7_uint32 secondaryMask)
{
  ipMapArpIntf_t  *pIntf = L7_NULLPTR;
  e_Err           erc;
  t_Lay3Addr      l3Addr;
  L7_uchar8       mac[L7_MAC_ADDR_LEN];
  L7_uchar8       ipStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uint32 i;
  L7_RC_t rc = L7_SUCCESS;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  /* Clean up data structures */

  memset((L7_char8 *)&l3Addr, 0, sizeof(t_Lay3Addr));
  memset((L7_char8 *)mac, 0, L7_MAC_ADDR_LEN);
  memset((L7_char8 *)ipStr, 0, OSAPI_INET_NTOA_BUF_SIZE);

    pIntf = (ipMapArpIntf_t *)OLL_Find(ipMapArpCtx_g.pIntfList, &primaryIp, &intIfNum);

    if ((pIntf == L7_NULLPTR) || (secondaryIp == L7_NULL_IP_ADDR))
    {
        IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_ERROR;
    }

  for (i = 0; i < L7_L3_NUM_SECONDARIES; i++)
  {
    if ((pIntf->secondaries[i].ipAddr == secondaryIp) &&
        (pIntf->secondaries[i].ipMask == secondaryMask))
    {
      pIntf->secondaries[i].ipAddr = 0;
      pIntf->secondaries[i].ipMask = 0;
      break;
    }
  }
  if (i == L7_L3_NUM_SECONDARIES)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_ARP_MAP_COMPONENT_ID,
            "Failed to remove secondary address from ARP interface %s.",
            ifName);
    rc = L7_FAILURE;
    /* continue trying to clean up */
  }

  memcpy((L7_char8 *)l3Addr.lay3Addr, (L7_char8 *)&secondaryIp, L7_IP_ADDR_LEN);
  l3Addr.protocolId = 0;

  /* Delete the IP and MAC address of this interface from the local ARP table */
  erc = ARP_DelLocAddr(ipMapArpCtx_g.arpHandle, (t_IPAddr *)l3Addr.lay3Addr,
                       (word)pIntf->index);
  if (erc != E_OK)
  {
    osapiInetNtoa((L7_uint32)secondaryIp, ipStr);
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "IPM_ARP: ARP_DelLocalAddr failed for IP addr %-15.15s " \
        "(arp intf %u), rc=%d\n", ipStr, pIntf->index, erc);
    IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
    return L7_FAILURE;
  }

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);
  return rc;
}

/*********************************************************************
* @purpose  Registers for ARP Cache update events
*
* @param    pCallbackFn   Callback function
*
* @returns  L7_SUCCESS  Was able to successfully register the function
* @returns  L7_FAILURE  Too many registered functions
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpCacheRegister(ipMapArpCacheCallback_ft pCallbackFn)
{
  L7_uint32 i;
  L7_RC_t rc = L7_FAILURE;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  for (i = 0; i < IPM_ARP_CACHE_REG_MAX; i++)
  {
    if (ipMapArpCtx_g.arpCacheCallback[i] == L7_NULL)
    {
      ipMapArpCtx_g.arpCacheCallback[i] = pCallbackFn;
      rc = L7_SUCCESS;
      break;
    }
  }

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);

  return rc;
}

/*********************************************************************
* @purpose  Unregisters for ARP Cache update events
*
* @param    pCallbackFn   Callback function
*
* @returns  L7_SUCCESS  If function was registered
* @returns  L7_FAILURE  If function was not registered
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpCacheUnregister(ipMapArpCacheCallback_ft pCallbackFn)
{
  L7_uint32 i;
  L7_RC_t rc = L7_FAILURE;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  for (i = 0; i < IPM_ARP_CACHE_REG_MAX; i++)
  {
    if (ipMapArpCtx_g.arpCacheCallback[i] == pCallbackFn)
    {
      ipMapArpCtx_g.arpCacheCallback[i] = L7_NULL;
      rc = L7_SUCCESS;
      break;
    }
  }

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);

  return rc;
}

/*********************************************************************
* @purpose  Returns contents of the ARP entry for the specified IP address
*
* @param    ipAddr      IP address of the ARP entry
* @param    intIfNum    interface where ARP entry must be
* @param    *pArp       pointer to output location to store ARP entry info
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    could not find requested ARP entry
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpEntryByAddrGet(L7_IP_ADDR_t ipAddr, L7_uint32 intIfNum,
                               L7_arpEntry_t *pArp)
{
  L7_RC_t rc;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  rc = ipMapArpExtenEntryGet(ipAddr, intIfNum, pArp);

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);

  return rc;
}

/*********************************************************************
* @purpose  Returns contents of the ARP entry for the next sequential
*           IP address following the one specified
*
* @param    ipAddr      IP address of the ARP entry to start the search
* @param    intIfNum   @b{(input)} Internal interface number for entry
* @param    *pArp       pointer to output location to store ARP entry info
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no more ARP entries exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpEntryNext(L7_IP_ADDR_t ipAddr, L7_uint32 intIfNum,
                          L7_arpEntry_t *pArp)
{
  L7_RC_t rc;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  rc = ipMapArpExtenEntryNext(ipAddr, intIfNum, pArp);

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);

  return rc;
}

/*********************************************************************
* @purpose  Sets the ARP entry ageout time to a new value
*
* @param    secs        new ARP entry ageout time (in seconds)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpAgeTimeSet(L7_uint32 secs)
{
  L7_RC_t rc;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  rc = ipMapArpExtenAgeTimeSet(secs);

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);

  return rc;
}

/*********************************************************************
* @purpose  Sets the ARP resolution response timeout value
*
* @param    secs        new ARP response timeout (in seconds)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpRespTimeSet(L7_uint32 secs)
{
  L7_RC_t rc;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  rc = ipMapArpExtenRespTimeSet(secs);

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);

  return rc;
}

/*********************************************************************
* @purpose  Sets the number of retries for failed ARP resolution requests
*
* @param    numRetry    number of retry requests
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpRetriesSet(L7_uint32 numRetry)
{
  L7_RC_t rc;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  rc = ipMapArpExtenRetriesSet(numRetry);

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);

  return rc;
}

/*********************************************************************
* @purpose  Sets the ARP cache size
*
* @param    maxEnt      maximum number of entries allowed in ARP cache
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    If request ARP cache size is outside of range then return
*           an error.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpCacheSizeSet(L7_uint32 maxEnt)
{
  L7_RC_t rc;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  rc = ipMapArpExtenCacheSizeSet(maxEnt);

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);

  return rc;
}

/*********************************************************************
* @purpose  Sets the dynamic ARP entry renew mode
*
* @param    mode        dynamic renew mode (L7_ENABLE, L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpDynamicRenewSet(L7_uint32 mode)
{
  L7_RC_t rc;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  rc = ipMapArpExtenDynamicRenewSet(mode);

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);

  return rc;
}

/*********************************************************************
* @purpose  Sets the proxy ARP mode
*
* @param    mode        proxy ARP mode (L7_ENABLE, L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpProxyArpSet(L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_RC_t rc = L7_FAILURE;
  ipMapArpIntf_t *pIntf;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  if (ipMapArpIntfInstanceGet(intIfNum, &pIntf) == L7_SUCCESS)
  {
    if (mode == L7_ENABLE)
      pIntf->proxyArp = L7_TRUE;
    else
      pIntf->proxyArp = L7_FALSE;
    rc = L7_SUCCESS;
  }

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);

  return rc;
}

/*********************************************************************
* @purpose  Sets the local proxy ARP mode for an interface
*
* @param    mode        proxy ARP mode (L7_ENABLE, L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function should only be called by IP MAP.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpLocalProxyArpSet(L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_RC_t rc = L7_FAILURE;
  ipMapArpIntf_t *pIntf;

  IPM_ARP_SEMA_TAKE(&ipMapArpCtx_g.arpLock, L7_WAIT_FOREVER);

  if (ipMapArpIntfInstanceGet(intIfNum, &pIntf) == L7_SUCCESS)
  {
    if (mode == L7_ENABLE)
      pIntf->localProxyArp = L7_TRUE;
    else
      pIntf->localProxyArp = L7_FALSE;

    rc = L7_SUCCESS;
  }

  IPM_ARP_SEMA_GIVE(&ipMapArpCtx_g.arpLock);

  return rc;
}
