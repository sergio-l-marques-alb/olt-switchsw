/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  ipmap.c
*
* @purpose   Ip Mapping layer Initialization Routines
*
* @component Ip Mapping Layer
*
* @comments  none
*
* @create    03/12/2001
*
* @author    asuthan
*
* @end
*
**********************************************************************/

/*************************************************************
Some notes on event handling in IP MAP....

IP MAP handles two types of events:

1. interface events (e.g., link up, link down, attach, detach)
2. configuration events that change the state of the router or of a routing
interface. These include IP address configuration changes and configuration
of the routing admin status on the router or an interface.

An interface can only be a routing interface if a number of conditions are
met. See ipMapMayEnableInterface() for details. Both interface events and
configuration events can change whether an interface is an active routing
interface.

IP MAP informs other applications (those that register using
ipMapRegisterRoutingEventChange()) of several "router events." These include

a. enabling or disabling the router
b. enabling or disabling an interface for routing
c. adding or deleting an IP address on an interface

These events are not independent. For example, if routing is enabled on the
router, then that might bring up routing on several interfaces, and IP MAP
might need to inform registrants of the IP address on those interfaces. So
one change might cause IP MAP to send several router events.

IP MAP also informs the driver and the operating system of changes associated
with routing events.

The current overall system design requires that IP MAP keep track of the
registrants' responses to router events. One reason for this is that interface
events from NIM can cause router events (for example, a link up activates a
routing interface), and NIM requires its registrants to respond when they have
completed processing an event. IP MAP doesn't respond to NIM until all apps
registered with IP MAP have responded to the router events triggered by NIM.
Also, IP MAP has to coordinate the changes it sends
to applications with changes it sends to the driver and the operating system.
An example often given is that IP MAP should not remove a routing interface
from the operating system before all the apps have been informed that the
routing interface is gone, lest an app try to use the interface (e.g, send a
packet on a socket bound to the interface) and cause a problem within the
operating system.

Since IP MAP's registrants may take some time processing routing events
(a 20 second wait has been observed, for example, when many routes
have been learned), there is a desire to
avoid having IP MAP block waiting for registrants to respond. Thus, a
system of "asynchronous notification" has been put in place. When IP MAP
sends a routing event, an application can respond immediately (on the IP MAP
thread), or can respond at a later time on its own thread. IP MAP tallies the
responses, and considers the event done when the last registrant has
responded. If one or more registrants do not respond within a certain time,
the event times out (and bad things probably happen).


*************************************************************/


#define L7_MAP_ROUTE_PREFERENCES 1

#include "l7_ipinclude.h"
#include "ip_util.h"

#ifdef L7_NSF_PACKAGE
#include "ipmap_ckpt.h"
#include "ipmap_arp_ckpt.h"
#endif

#include "l3end_api.h"
#include "dtlapi.h"
#include "rtrdisc.h"
#include "rtrdisc_config.h"
#include "dot1q_api.h"
#include "dot1x_api.h"
#include "rtrdisc_api.h"
#include "rtrdisc_util.h"
#include "nimapi.h"
#include "rtiprecv.h"
#include "osapi_support.h"
#include "simapi.h"
#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"
#include "ipmap_sid.h"
#include "ipmap_arp_exten.h"
#include "xx.ext"
#include "l7_vrrp_api.h"
#include "usmdb_ip_api.h"
#include "dot1q_api.h"
#include "l7_ip_api.h"
#include "cli_txt_cfg_api.h"

#ifdef L7_QOS_FLEX_PACKAGE_ACL
#include "support_api.h"
#endif


L7_int32                ipMapForwardingTaskId = L7_NULL;
L7_int32                ipMapProcessingTaskId = L7_NULL;
ipMapInfo_t                 *pIpMapInfo                                 = L7_NULL;
void *ipMapMsgQSema = L7_NULL;
void *ipMapForwardQueue[L7_L3_FORWARD_QUEUE_COUNT];
L7_uint32 ipMapForwardQueueHWMark[L7_L3_FORWARD_QUEUE_COUNT];
void *ipMapProcess_Queue = L7_NULL;
pIpRoutingEventNotifyList_t  pIpIntfStateNotifyList;
L7_rtrIntfMap_t    rtrIntfMap[L7_RTR_MAX_RTR_INTERFACES+1];
L7_uint32          intIfNumToRtrIntf[L7_MAX_INTERFACE_COUNT+1];
L7_uint32 *intIfToCfgIndex;
ipMapIntfInfo_t     *ipMapIntfInfo;
rtoRouteChange_t *ipMapRouteChangeBuf;
extern void ipStrClearRtrPortRoutingFlags(void);
extern ipMapCnfgrState_t    ipMapCnfgrState;

/*---------------------------------------*/
/* Global Configuration Data Structures  */
/*---------------------------------------*/
L7_ipMapCfg_t           *ipMapCfg                                       = L7_NULL;
L7_rtrStaticRouteCfg_t  *route                                          = L7_NULL;
L7_rtrStaticArpCfg_t    *arp                                            = L7_NULL;

#ifdef L7_QOS_FLEX_PACKAGE_ACL
ipMapDebugCfg_t ipMapDebugCfg;
extern void ipMapDebugCfgUpdate(void);
#endif

/*--------------------*/
/* Static variables.  */
/*--------------------*/
/* Buffer pool used by the IP mapping code */
static L7_uint32            ipMapAsyncNotifyPoolId                         = 0;

static L7_uint32 ipMapQHighWaterMark = 0;

static void ipMapBestRouteEventPost(void);
static L7_RC_t ipMapLocalRoutesReserve(void);
static L7_RC_t ipMapLocalRoutesUnreserve(void);

/* bit mask indicating the items of interest for debug tracing */
extern L7_uint32 ipMapTraceFlags;
extern L7_uint32 ipMapTraceIntf;
extern L7_uchar8 *ipMapProcMessageNames[IPMAP_LAST_MSG];
extern L7_uchar8 *rtoProtoNames[RTO_LAST_PROTOCOL];

/* statistics counters for software forwarding */
static L7_uint32 ipForwardPktsDropNotExe = 0;
L7_uint32 ipForwardPktsIn0 = 0;
L7_uint32 ipForwardPktsIn1 = 0;
L7_uint32 ipForwardArpPktsIn = 0;
L7_uint32 ipForwardPktsDropped = 0;
L7_uint32 ipForwardNotRtrIntf = 0;
L7_uint32 ipForwardRtrIntfInvalid = 0;
L7_uint32 ipForwardConsumed = 0;
L7_uint32 ipForwardConsumedMcast = 0;
L7_uint32 ipForwardBufAllocFail = 0;
L7_uint32 ipForwardQueueFull = 0;
L7_uint32 ipForwardPktsOut = 0;
L7_uint32 ipForwardIpPktsToEnd = 0;
L7_uint32 ipForwardArpToEnd = 0;
L7_uint32 ipForwardPktsToStack = 0;

/* Forwarding stats for IPv6. Placed here so that end driver doesn't
 * depend on v6 flex package. */
L7_uint32 ip6ForwardPktsIn = 0;        /* number of IPv6 packets to CPU */
L7_uint32 ip6ForwardNotRtrIntf = 0;    /* packets received on non-routing interface */
L7_uint32 ip6ForwardRxIfDown = 0;      /* packets received on down interface */
L7_uint32 ip6ForwardPktsToStack = 0;   /* packets to IP stack */

extern void l3BestRouteReport (L7_routeEntry_t *routeEntry,
                               L7_uint32 route_status,
                               void *passthrough);

/* debugging */
void ipMapQLenShow(void);
void ipMapQLenReset(void);

void ipForwardStatsReset(void)
{
  ipForwardPktsIn0 = 0;
  ipForwardArpPktsIn = 0;
  ipForwardPktsIn1 = 0;
  ipForwardPktsDropped = 0;
  ipForwardBufAllocFail = 0;
  ipForwardNotRtrIntf = 0;
  ipForwardConsumed = 0;
  ipForwardConsumedMcast = 0;
  ipForwardRtrIntfInvalid = 0;
  ipForwardPktsDropNotExe = 0;
  ipForwardQueueFull = 0;
  ipForwardPktsOut = 0;
  ipForwardPktsToStack = 0;
  ipForwardIpPktsToEnd = 0;
  ipForwardArpToEnd = 0;
}

void ipForwardStatsShow(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                "\nIP Packets to CPU: %u", ipForwardPktsIn0);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                "\nARP Packets to CPU: %u", ipForwardArpPktsIn);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                "\nPackets to IP forwarding task: %u", ipForwardPktsIn1);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                "\nIP packets dropped. mbuf alloc failure: %u",
                ipForwardPktsDropped);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                "\nIP packets dropped. buffer pool alloc failure: %u",
                ipForwardBufAllocFail);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                "\nIP packets dropped. ingress interface not router interface: %u",
                ipForwardNotRtrIntf);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                "\nIP packets consumed by intercepting application: %u",
                ipForwardConsumed);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                "\nIP multicast packets consumed by intercepting application: %u",
                ipForwardConsumedMcast);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                "\nIP packets dropped. ingress interface not valid: %u",
                ipForwardRtrIntfInvalid);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                "\nIP packets dropped while not in Execute state: %u",
                ipForwardPktsDropNotExe);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                "\nIP packets dropped because IP forwarding queue is full: %u",
                ipForwardQueueFull);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                "\nIP packets forwarded: %u", ipForwardPktsOut);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                "\nIP packets to IP stack: %u", ipForwardPktsToStack);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                "\nIP packets to end driver: %u", ipForwardIpPktsToEnd);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                "\nARP packets to end driver: %u", ipForwardArpToEnd);
}


/*********************************************************************
* @purpose  Initialize ip task
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipStartTasks(void)
{
    static const char *routine_name = "ipStartTasks()";

    IPMAP_TRACE("%s %d: %s : \n", __FILE__, __LINE__, routine_name);

    /* Create processing Task */

    ipMapProcessingTaskId = osapiTaskCreate("ipMapProcessingTask", ipMapProcessingTask, 0, 0,
                                            ipMapProcessingSidDefaultStackSize(),
                                            ipMapProcessingSidDefaultTaskPriority(),
                                            ipMapProcessingSidDefaultTaskSlice());

    if (ipMapProcessingTaskId == L7_ERROR)
    {
        ipMapProcessingTaskId = L7_NULL;
        IPMAP_ERROR_SEVERE("%s %d: %s : Failed to Create Ip Forwarding Task\n",
                           __FILE__, __LINE__, routine_name);
        return(L7_FAILURE);
    }

    if (osapiWaitForTaskInit(L7_IPMAP_PROCESSING_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
        IPMAP_ERROR_SEVERE("%s %d: %s: Unable to initialize IP Processing task()\n",
                           __FILE__, __LINE__, routine_name);
        return(L7_FAILURE);
    }

    /* Create forwarding task and queue semaphore*/

    ipMapMsgQSema = osapiSemaCCreate (OSAPI_SEM_Q_FIFO, 0);

    if (ipMapMsgQSema == L7_NULLPTR)
    {
        L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_IP_MAP_COMPONENT_ID,
               "Failure creating IpMap message queue semaphore.");
        return L7_FAILURE;
    }

    ipMapForwardingTaskId = osapiTaskCreate("ipMapForwardingTask", ipMapForwardingTask, 0, 0,
                                            ipMapForwardingSidDefaultStackSize(),
                                            ipMapForwardingSidDefaultTaskPriority(),
                                            ipMapForwardingSidDefaultTaskSlice());


    if (ipMapForwardingTaskId == L7_ERROR)
    {
        ipMapForwardingTaskId = L7_NULL;
        IPMAP_ERROR_SEVERE("%s %d: %s : Failed to Create Ip Forwarding Task\n",
                           __FILE__, __LINE__, routine_name);
        return(L7_FAILURE);
    }

    if (osapiWaitForTaskInit (L7_IPMAP_FORWARDING_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
        IPMAP_ERROR_SEVERE("%s %d: %s: Unable to initialize IP Forwarding task()\n",
                           __FILE__, __LINE__, routine_name);
        return(L7_FAILURE);
    }

    return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Reset data changed flags.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    All products that include this component use text based
*           configuration persistent storage.
*
* @end
*********************************************************************/
L7_RC_t ipSave(void)
{
  ipMapCfg->cfgHdr.dataChanged = L7_FALSE;
  route->cfgHdr.dataChanged = L7_FALSE;
  arp->cfgHdr.dataChanged = L7_FALSE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Restores IP user config file to factory defaults
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
void ipMapRestoreProcess(void)
{
  static const char *routine_name = "ipMapRestoreProcess()";
  L7_uint32 i;
  nimConfigID_t nullIntfConfigId;
  L7_uint32 cfgIntIfNum;

  IPMAP_TRACE("%s %d: %s : \n", __FILE__, __LINE__, routine_name);

  memset(&nullIntfConfigId, 0, sizeof(nullIntfConfigId));
  /*If a default route was configured, delete it from VxWorks route table
    and then inform service port to use its own gateway. Since VxWorks only
    uses one next hop, we don't know which gateway it might be using,
    so inform VxWorks of them all.
  */
  for (i = 0; i < L7_RTR_MAX_STATIC_ROUTES; i++)
  {
      if ((route->rtrStaticRouteCfgData[i].inUse == L7_TRUE) &&
          (route->rtrStaticRouteCfgData[i].ipAddr == 0) &&
          (route->rtrStaticRouteCfgData[i].ipMask == 0))
      {
          L7_uint32 nh;
          for (nh = 0; (nh < platRtrRouteMaxEqualCostEntriesGet()) &&
                (route->rtrStaticRouteCfgData[i].nextHops[nh].nextHopRtr != 0); nh++)
          {
              if (NIM_CONFIG_ID_IS_EQUAL(
                    &route->rtrStaticRouteCfgData[i].nextHops[nh].intfConfigId,
                    &nullIntfConfigId) == L7_TRUE)
              {

                  if ( _ipMapRouterIfResolve(
                      route->rtrStaticRouteCfgData[i].nextHops[nh].nextHopRtr,
                      &cfgIntIfNum) != L7_SUCCESS)
                  {
                    continue;
                  }
              }
              else
              {
                  if (nimIntIfFromConfigIDGet(
                     &route->rtrStaticRouteCfgData[i].nextHops[nh].intfConfigId,
                     &cfgIntIfNum) != L7_SUCCESS)
                  {
                    continue;
                  }
              }
              osapiDelGateway(route->rtrStaticRouteCfgData[i].nextHops[nh].nextHopRtr, cfgIntIfNum);
          }
      }
  }

  osapiServPortDefaultRouteEnableSet(L7_TRUE);

  if (ipMapCfg->rtr.rtrAdminMode == L7_ENABLE)
  {
      ipMapCfg->rtr.rtrAdminMode = L7_DISABLE;
      (void)ipMapRtrAdminModeDisable();
  }

  (void)ipStrClearRtrPortRoutingFlags();
  return;
}

/*********************************************************************
* @purpose  Build default ip config data
*
* @param    ver   @b{(input)} Software version of Config Data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ipBuildDefaultConfigData(L7_uint32 ver)
{
  static const char *routine_name = "ipBuildDefaultConfigData()";
  L7_uint32 cfgIndex;
  L7_rtrCfgCkt_t *pCfg;
  nimConfigID_t configId[L7_IPMAP_INTF_MAX_COUNT];

  /* Before we wipe the config, unreserve all local routes */
  ipMapLocalRoutesUnreserve();

  IPMAP_TRACE("%s %d: %s : \n", __FILE__, __LINE__, routine_name);

  /* Build Config File Header */

  memset(&configId[0], 0, sizeof(nimConfigID_t) * L7_IPMAP_INTF_MAX_COUNT);

  for (cfgIndex = 1; cfgIndex < L7_IPMAP_INTF_MAX_COUNT; cfgIndex++)
    NIM_CONFIG_ID_COPY(&configId[cfgIndex], &ipMapCfg->ckt[cfgIndex].configId);

  memset(ipMapCfg, 0, sizeof( L7_ipMapCfg_t));

  for (cfgIndex = 1; cfgIndex < L7_IPMAP_INTF_MAX_COUNT; cfgIndex++)
    NIM_CONFIG_ID_COPY(&ipMapCfg->ckt[cfgIndex].configId, &configId[cfgIndex]);

  ipMapCfg->cfgHdr.version       = ver;
  ipMapCfg->cfgHdr.componentID   = L7_IP_MAP_COMPONENT_ID;
  ipMapCfg->cfgHdr.type          = L7_CFG_DATA;
  ipMapCfg->cfgHdr.length        = sizeof(L7_ipMapCfg_t);
  ipMapCfg->cfgHdr.dataChanged   = L7_FALSE;
  ipMapCfg->cfgHdr.version       = ver;

  /*---------------------------*/
  /* Build Default Config Data */
  /*---------------------------*/

  /* Generic IP Stack Cfg */
  ipMapCfg->ip.ipSourceAddrCheck     =  FD_IP_DEFAULT_IP_SRC_ADDR_CHECKING;
  ipMapCfg->ip.ipSpoofingCheck       =  FD_IP_DEFAULT_IP_SPOOFING;
  ipMapCfg->ip.ipDefaultTTL          =  FD_IP_DEFAULT_TTL;

  /* ARP Configuration parameters */
  ipMapCfg->ip.ipArpAgeTime          =  FD_IP_DEFAULT_ARP_AGE_TIME;
  ipMapCfg->ip.ipArpRespTime         =  FD_IP_DEFAULT_ARP_RESP_TIME;
  ipMapCfg->ip.ipArpRetries          =  FD_IP_DEFAULT_ARP_RETRIES;
  ipMapCfg->ip.ipArpCacheSize        =  platRtrArpMaxEntriesGet();
  ipMapCfg->ip.ipArpDynamicRenew     =  FD_IP_DEFAULT_ARP_DYNAMIC_RENEW;

  memcpy((L7_uint32 *)&ipMapCfg->ip.route_preferences, (L7_uint32 *)&FD_rtr_rto_preferences,
         (sizeof (L7_uint32) * L7_ROUTE_MAX_PREFERENCES));

  /* Router Configuration parameters */
  ipMapCfg->rtr.rtrAdminMode             = FD_RTR_DEFAULT_ADMIN_MODE;
  ipMapCfg->rtr.rtrTOSForwarding         = FD_RTR_DEFAULT_TOS_FORWARDING;
  ipMapCfg->rtr.rtrICMPRedirectMode      = FD_RTR_DEFAULT_ICMP_REDIRECT_MODE;
  ipMapCfg->rtr.rtrTraceMode             = FD_RTR_DEFAULT_TRACE_MODE;
  ipMapCfg->rtr.rtrICMPEchoReplyMode     = FD_RTR_DEFAULT_ICMP_ECHO_REPLY_MODE;
  ipMapCfg->rtr.rtrICMPRatelimitBurstSize = FD_RTR_DEFAULT_ICMP_RATE_LIMIT_SIZE;
  ipMapCfg->rtr.rtrICMPRatelimitInterval = FD_RTR_DEFAULT_ICMP_RATE_LIMIT_INTERVAL;
  (void) ipMapRtrICMPRatelimitApply (ipMapCfg->rtr.rtrICMPRatelimitBurstSize, ipMapCfg->rtr.rtrICMPRatelimitInterval);
  ipMapRtrICMPEchoReplyModeApply(FD_RTR_DEFAULT_ICMP_ECHO_REPLY_MODE);
  /* Router Configuration parameters */
  for (cfgIndex = 1; cfgIndex < L7_IPMAP_INTF_MAX_COUNT; cfgIndex++)
  {
    pCfg = &ipMapCfg->ckt[cfgIndex];
    ipIntfBuildDefaultConfigData(&pCfg->configId, pCfg);
  }
}

/*********************************************************************
* @purpose  Build default ip config data for the intf
*
* @param    slot                slot number
* @param    port                port number
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ipIntfBuildDefaultConfigData(nimConfigID_t *configId, L7_rtrCfgCkt_t *pCfg)
{
  memset(pCfg, 0, sizeof(L7_rtrCfgCkt_t));

  NIM_CONFIG_ID_COPY(&pCfg->configId, configId);

  pCfg->icmpUnreachablesMode = FD_RTR_DEFAULT_ICMP_UNREACHABLES_MODE;

  pCfg->ipMtu = FD_IP_DEFAULT_IP_MTU;

  pCfg->bandwidth = FD_IP_DEFAULT_BW;

  pCfg->icmpRedirectsMode  = FD_RTR_DEFAULT_ICMP_REDIRECT_MODE;

  memset(&pCfg->numberedIfc, 0, sizeof(nimConfigID_t));

  if (FD_RTR_DEFAULT_INTF_ADMIN_MODE == L7_ENABLE)
  {
    pCfg->flags  |=  L7_RTR_INTF_ADMIN_MODE_ENABLE;
  }

  if (FD_RTR_DEFAULT_FWD_MULTICASTS == L7_ENABLE)
  {
    pCfg->flags  |=  L7_RTR_INTF_MCAST_FWD_MODE_ENABLE;
  }

  if (FD_RTR_DEFAULT_FWD_NET_DIRECT_BCAST == L7_ENABLE)
  {
    pCfg->flags  |=  L7_RTR_INTF_NETDIR_BCAST_MODE_ENABLE;
  }

  if (cnfgrIsFeaturePresent(L7_IP_MAP_COMPONENT_ID,
                            L7_IPMAP_FEATURE_PROXY_ARP) == L7_TRUE)
  {
    if (FD_PROXY_ARP_ADMIN_MODE == L7_ENABLE)
    {
      pCfg->flags  |=  L7_RTR_INTF_PROXY_ARP;
    }

    if (FD_LOCAL_PROXY_ARP_ADMIN_MODE == L7_ENABLE)
    {
      pCfg->flags  |=  L7_RTR_INTF_LOCAL_PROXY_ARP;
    }
  }

  if (FD_RTR_DEFAULT_GRAT_ARP == L7_ENABLE)
  {
    pCfg->flags  |=  L7_RTR_INTF_GRAT_ARP;
  }

  if (FD_RTR_INTF_UNNUMBERED == L7_TRUE)
  {
    pCfg->flags |= L7_RTR_INTF_UNNUMBERED;
  }

  pCfg->flags &= ~L7_RTR_INTF_ADDR_METHOD_DHCP;

  return;
}

/*********************************************************************
* @purpose  Build default configuration data for static routes
*
* @param    ver   @b{(input)} Software version of Config Data
*
* @returns  void
*
* @notes    This component always uses text based config persistent storage.
*
* @end
*********************************************************************/
void ipStaticRtsBuildDefaultConfigData(L7_uint32 ver)
{
  /* Static Routes have no default configuration */
  memset(route, 0, sizeof( L7_rtrStaticRouteCfg_t));

  route->cfgHdr.version       = ver;
  route->cfgHdr.componentID   = L7_IP_MAP_COMPONENT_ID;
  route->cfgHdr.type          = L7_CFG_DATA;
  route->cfgHdr.length        = sizeof(L7_rtrStaticRouteCfg_t);
  route->cfgHdr.dataChanged   = L7_FALSE;

  return;
}

/*********************************************************************
* @purpose  Build default configuration data for static arps
*
* @param    ver   @b{(input)} Software version of Config Data
*
* @returns  void
*
* @notes    This component always uses text based configuration
*           persistent storage.
*
* @end
*********************************************************************/
void ipStaticArpBuildDefaultConfigData(L7_uint32 ver)
{
  /* Static arp entries have no default configuration */

  /* NOTE: MUST use factory default max value definition here */
  memset(( void * )arp, 0, sizeof(L7_rtrStaticArpCfgData_t) * FD_RTR_MAX_STATIC_ARP_ENTRIES);
  arp->cfgHdr.version       = ver;
  arp->cfgHdr.componentID   = L7_IP_MAP_COMPONENT_ID;
  arp->cfgHdr.type          = L7_CFG_DATA;
  arp->cfgHdr.length        = sizeof(L7_rtrStaticArpCfg_t);
  arp->cfgHdr.dataChanged   = L7_FALSE;

  return;
}

/*********************************************************************
* @purpose  Reserve space in RTO for a local route for each IPv4 address.
*
* @param    void
*
* @returns  L7_SUCCESS if a local route reserved for every address
*           L7_FAILURE otherwise
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t ipMapLocalRoutesReserve(void)
{
  L7_uint32 cfgIndex;
  L7_rtrCfgCkt_t *pCfg = NULL;
  L7_uint32 j;
  L7_uint32 rc = L7_SUCCESS;

  for (cfgIndex = 1; cfgIndex < L7_IPMAP_INTF_MAX_COUNT; cfgIndex++)
  {
    pCfg = &ipMapCfg->ckt[cfgIndex];
    for (j=0; j < L7_L3_NUM_IP_ADDRS; j++)
    {
      if (pCfg->addrs[j].ipAddr)
      {
        if (rtoRouteReserve() != L7_SUCCESS)
        {
          rc = L7_FAILURE;
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                  "RTO does not have enough space for local routes."
                  "For each IP address configured on a local interface, "
                  "RTO reserves space for a local route. If an address is"
                  " added when the routing table is already full, RTO is "
                  "unable to reserve space for the new local route and logs this message.");
        }
      }
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Unreserve space in RTO for a local route for each IPv4 address.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t ipMapLocalRoutesUnreserve(void)
{
  L7_uint32 cfgIndex;
  L7_rtrCfgCkt_t *pCfg = NULL;
  L7_uint32 j;

  for (cfgIndex = 1; cfgIndex < L7_IPMAP_INTF_MAX_COUNT; cfgIndex++)
  {
    pCfg = &ipMapCfg->ckt[cfgIndex];
    for (j=0; j < L7_L3_NUM_IP_ADDRS; j++)
    {
      if (pCfg->addrs[j].ipAddr)
      {
        rtoRouteUnReserve();
      }
    }
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Applies ip config data
*
* @param    void
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @notes    Interface configuration data is cannot be applied here, because
*           when this is called during initialization, the interfaces have
*           not yet been created and therefore IP MAP is unable to get the
*           configuration for an interface. Interface config data is applied
*           in response to interface events from NIM.
*
* @end
*********************************************************************/
L7_RC_t ipMapApplyConfigData(void)
{
  static const char *routine_name = "ipMapApplyConfigData()";

  nimConfigID_t configIdNull;
  L7_uint32 intIfNum;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  IPMAP_TRACE("%s %d: %s : \n", __FILE__, __LINE__, routine_name);

  /* Clear rtr information arrays before applying the new configuration */
  memset((void *)rtrIntfMap, 0, sizeof(L7_rtrIntfMap_t)*(L7_RTR_MAX_RTR_INTERFACES + 1));
  memset((void *)intIfNumToRtrIntf, 0, sizeof(L7_uint32)*(L7_MAX_INTERFACE_COUNT + 1));
  memset((void *)&pIpMapInfo->operRtr, 0, sizeof(ipMapOper_t));
  memset((void *)&pIpMapInfo->operIntf, 0,
         sizeof(ipMapOperRtrIntf_t) * (L7_MAX_INTERFACE_COUNT));
  /* Treat IPv4 routing as disabled initially */
  pIpMapInfo->operRtr.lastNotify = L7_RTR_DISABLE;

  for (intIfNum = 1; intIfNum < L7_MAX_INTERFACE_COUNT; intIfNum++)
  {
    pIpMapInfo->operIntf[intIfNum].lastNotify = L7_RTR_INTF_DISABLE;
  }

/*-------------------------------------*/
/*   Configure IP specific parameters  */
/*-------------------------------------*/
  /* Generic IP Stack Cfg */
  ipMapIpSourceCheckingApply(ipMapCfg->ip.ipSourceAddrCheck);
  ipMapIpSpoofingModeApply(ipMapCfg->ip.ipSpoofingCheck);
  ipMapIpDefaultTTLApply(ipMapCfg->ip.ipDefaultTTL);

  /* ARP Configuration parameters */
  ipMapIpArpAgeTimeApply(ipMapCfg->ip.ipArpAgeTime);
  ipMapIpArpRespTimeApply(ipMapCfg->ip.ipArpRespTime);
  ipMapIpArpRetriesApply(ipMapCfg->ip.ipArpRetries);
  ipMapIpArpCacheSizeApply(ipMapCfg->ip.ipArpCacheSize);
  ipMapIpArpDynamicRenewApply(ipMapCfg->ip.ipArpDynamicRenew);


/*-----------------------------------------*/
/*   Configure Router specific parameters  */
/*-----------------------------------------*/
  if (ipMapCfg->rtr.rtrAdminMode == L7_ENABLE)
  {
    ipMapRtrAdminModeEnable();
  }
  else if (ipMapCfg->rtr.rtrAdminMode == L7_DISABLE)
  {
    ipMapRtrAdminModeDisable();
  }
  else
  {
    IPMAP_ERROR("%s %d: %s:  ERROR: Cannot decipher router admin mode configuration\n",
                __FILE__, __LINE__, routine_name);
    L7_assert(1);
    return(L7_FAILURE);
  }

  ipMapRtrICMPRedirectModeApply(ipMapCfg->rtr.rtrICMPRedirectMode);

  /* Reserve space in RTO for a local route for each IPv4 address */
  ipMapLocalRoutesReserve();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Transfer configuration complete event to IP MAP processing thread
*
* @param    event  - one of txtCfgApplyEvent_t
*
* @returns  L7_SUCCESS
*
* @notes    It turned out that IP MAP did not have a use for this notification.
*           So IP MAP does not register for this event. Function left here for
*           future use.
*
* @end
*********************************************************************/
L7_RC_t ipMapConfigCompleteCallback(L7_uint32 event)
{
  ipMapMsg_t   Message;
  L7_RC_t rc;

  if (ipMapTraceFlags & IPMAP_TRACE_NSF)
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX,
                  "IP MAP got configuration complete event %u", event);
    ipMapTraceWrite(traceBuf);
  }

  /* Ignore events types we don't care about.  */
  if ((event != TXT_CFG_APPLY_FAILURE) &&
      (event != TXT_CFG_APPLY_SUCCESS))
  {
    return L7_SUCCESS;
  }

  Message.msgId = IPMAP_CFG_COMPLETE;
  Message.type.cfgCompleteEvent.event = event;

  rc = osapiMessageSend(ipMapProcess_Queue, &Message, sizeof(ipMapMsg_t),
                        L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_IP_MAP_COMPONENT_ID,
            "Failed to send configuration complete event %u to IP MAP thread",
            event);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process configuration complete event
*
* @param    event  - one of txtCfgApplyEvent_t
*
* @returns  L7_SUCCESS
*
* @notes    Originally thought IP MAP would respond to configurator for
*           L2/config hw reconciliation phase as soon as config push was
*           done. This really needs to be deferred until the routing
*           interfaces are up. Leaving empty function here for future use.
*
* @end
*********************************************************************/
L7_RC_t ipMapCfgComplete(L7_uint32 event)
{
  return L7_SUCCESS;
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
L7_RC_t ipMapIntfCreate(L7_uint32 intIfNum, L7_uint32 *eventCompleted)
{
  nimConfigID_t configId;
  L7_rtrCfgCkt_t *pCfg = L7_NULLPTR;

  if (ipMapIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  memset((void*) &pIpMapInfo->operIntf[intIfNum], 0, sizeof(ipMapOperRtrIntf_t));

  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
    return L7_FAILURE;

  /* Should not be an interface structure for this interface yet. If one exists,
   * holler. */
  if (intIfToCfgIndex[intIfNum] != 0)
    {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "Interface structure already exists for interface %s when L7_CREATE event received",
            ifName);
    *eventCompleted = L7_TRUE;
    return L7_SUCCESS;
  }

    if (ipMapMapIntfConfigEntryGet(intIfNum, &pCfg) != L7_TRUE)
    {
        /* failed to create configuration structure for circuit */
    *eventCompleted = L7_TRUE;
        return L7_FAILURE;
    }

    /* Set the config id in the new the configuration structure. */
    ipIntfBuildDefaultConfigData(&configId, pCfg);

  /* Check if interface ready to be enabled */
  ipMapIntfUpdate(intIfNum, eventCompleted);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_DELETE
*
* @param    intIfNum  internal interface number
* @param    eventCompleted  ({output}) whether an async event is in progress
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t ipMapIntfDelete(L7_uint32 intIfNum, L7_uint32 *eventCompleted)
{
  L7_rtrCfgCkt_t *pCfg = L7_NULLPTR;
  L7_uint32 i;

  if (!ipMapMapIntfIsConfigurable(intIfNum, &pCfg))
    return L7_FAILURE;

  /* Set admin mode down so that router interface will be removed. */
  pCfg->flags &= ~L7_RTR_INTF_ADMIN_MODE_ENABLE;

  /* Unreserve space in RTO for the local routes corresponding to each
   * IPv4 address configured on this interface */
  for (i=0; i < L7_L3_NUM_IP_ADDRS; i++)
  {
    if (pCfg->addrs[i].ipAddr)
    {
      rtoRouteUnReserve();
    }
  }

  ipMapIntfUpdate(intIfNum, eventCompleted);

    memset((void *)&pCfg->configId, 0, sizeof(nimConfigID_t));

  /* Release the ip address (primary and secondary) associated with this interface */
  for (i=0; i < L7_L3_NUM_IP_ADDRS; i++)
  {
    if (pCfg->addrs[i].ipAddr)
    {
      pCfg->addrs[i].ipAddr = L7_NULL_IP_ADDR;
      pCfg->addrs[i].ipMask = L7_NULL_IP_ADDR;
    }
  }

  memset((void *)&intIfToCfgIndex[intIfNum], 0, sizeof(L7_uint32));
    ipMapCfg->cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  If NSF protocol unexpectedly does not report completion
*           of its routing table update, flush tentative routes when
*           this timer expires.
*
* @param    none used
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ipMapStaleRouteTimerExp(L7_uint32 unused1, L7_uint32 unused2)
{
  ipMapMsg_t   Message;
  L7_RC_t rc;

  memset(&Message, 0, sizeof(ipMapMsg_t));
  Message.msgId = IPMAP_STALE_ROUTE_TIMER;

  rc = osapiMessageSend(ipMapProcess_Queue, &Message, sizeof(ipMapMsg_t),
                        L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_IP_MAP_COMPONENT_ID,
            "Failed to send stale route timer expire event to IP MAP thread");
  }
}

/*********************************************************************
* @purpose  Handle NIM startup callback
*
* @param    startupPhase     create or activate
*
* @returns  void
*
* @notes    Transfer to IP MAP thread
*
* @end
*********************************************************************/
void ipMapIntfStartupCallback(NIM_STARTUP_PHASE_t startupPhase)
{
  ipMapMsg_t   Message;
  L7_RC_t rc;

  if (ipMapTraceFlags & IPMAP_TRACE_NIM_EVENTS)
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX,
                  "Received NIM %s startup event",
                  startupPhase == NIM_INTERFACE_CREATE_STARTUP ? "CREATE" : "ACTIVATE");
    ipMapTraceWrite(traceBuf);
  }

  if (ipMapCnfgrState != IPMAP_PHASE_EXECUTE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "Received NIM startup %d callback while in init state %s",
            startupPhase, ipMapCnfgrStateNames[ipMapCnfgrState]);
    return;
  }

  memset(&Message, 0, sizeof(ipMapMsg_t));
  Message.msgId = IPMAP_NIM_STARTUP;
  Message.type.nimStartupEvent.startupPhase = startupPhase;

  rc = osapiMessageSend(ipMapProcess_Queue, &Message, sizeof(ipMapMsg_t),
                        L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_IP_MAP_COMPONENT_ID,
            "Failed to send NIM startup phase %d to IP MAP thread",
            startupPhase);
  }
}

/*********************************************************************
* @purpose  Do NIM create startup
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    IP MAP has no configuration at this point.
*
* @end
*********************************************************************/
L7_RC_t ipMapNimCreateStartup(void)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;
  L7_uint32 eventCompleted;

  rc = nimFirstValidIntfNumber(&intIfNum);
  while (rc == L7_SUCCESS)
  {
    if (ipMapIsValidIntf(intIfNum))
    {
      if (ipMapIntfCreate(intIfNum, &eventCompleted) != L7_SUCCESS)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                "Failed to create interface %s with intIfNum %u", ifName, intIfNum);
      }
      /* Tell router event clients about every NIM interface create. We can't know at
       * this point whether routing is configured on an interface. So we can't filter
       * these events to just the routing interfaces. We can't defer the creates
       * because router event clients have to receive these before the configuration
       * push. This event requires a response, because we can't let the system
       * move on the config push until all components have processed all the interface
       * creates. */
      ipMapRoutingEventChangeNotify(intIfNum, L7_RTR_INTF_CREATE, L7_TRUE, L7_NULL);
      pIpMapInfo->operIntf[intIfNum].lastNotify = L7_RTR_INTF_CREATE;
      pIpMapInfo->operIntf[intIfNum].asyncPending = L7_TRUE;
    }
    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  /* Register with NIM to receive port CREATE and DELETE events */
  PORTEVENT_SETMASKBIT(pIpMapInfo->nimEventMask, L7_CREATE);
  PORTEVENT_SETMASKBIT(pIpMapInfo->nimEventMask, L7_DELETE);
  nimRegisterIntfEvents(L7_IP_MAP_COMPONENT_ID, pIpMapInfo->nimEventMask);
  /* nimStartupEventDone() call is deferred until all router event clients
   * have reported completion of the interface create. */

  pIpMapInfo->createStartupDone = L7_TRUE;

  if (ipMapTraceFlags & IPMAP_TRACE_NIM_EVENTS)
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX,
                  "IP MAP has sent all create startup events...waiting for responses");
    ipMapTraceWrite(traceBuf);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Do NIM activate startup
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    Only consider interfaces where routing is configured.
*
* @end
*********************************************************************/
L7_RC_t ipMapNimActivateStartup(void)
{
  L7_uint32 rtrIfNum;
  L7_uint32 eventCompleted;

  /* flip to L7_TRUE if at least one routing interface is up */
  L7_BOOL oneIntfUp = L7_FALSE;

  if (ipMapTraceFlags & IPMAP_TRACE_NIM_EVENTS)
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX,
                  "IP MAP beginning NIM activate startup");
    ipMapTraceWrite(traceBuf);
  }

  for (rtrIfNum = 1; rtrIfNum <= L7_RTR_MAX_RTR_INTERFACES; rtrIfNum++)
  {
    if (rtrIntfMap[rtrIfNum].present)
    {
      ipMapIntfUpdate(rtrIntfMap[rtrIfNum].intIfNum, &eventCompleted);
      if (!oneIntfUp && ipMapIntfIsUp(rtrIntfMap[rtrIfNum].intIfNum))
      {
        oneIntfUp = L7_TRUE;
      }
    }
  }

  /* Routing interfaces need to be created before USL does the L2/config
   * hw reconciliation. Should be ready for that here. */
  if (ipMapTraceFlags & IPMAP_TRACE_NSF)
  {
    ipMapTraceWrite("IP MAP reporting completion of L2 hw updates.");
  }
  cnfgrApiComponentHwUpdateDone(L7_IP_MAP_COMPONENT_ID, L7_CNFGR_HW_APPLY_CONFIG);

  /* Now ask NIM to send any future changes for these event types */
  PORTEVENT_SETMASKBIT(pIpMapInfo->nimEventMask, L7_ATTACH);
  PORTEVENT_SETMASKBIT(pIpMapInfo->nimEventMask, L7_DETACH);
  PORTEVENT_SETMASKBIT(pIpMapInfo->nimEventMask, L7_ACTIVE);
  PORTEVENT_SETMASKBIT(pIpMapInfo->nimEventMask, L7_INACTIVE);
  PORTEVENT_SETMASKBIT(pIpMapInfo->nimEventMask, L7_LAG_ACQUIRE);
  PORTEVENT_SETMASKBIT(pIpMapInfo->nimEventMask, L7_PROBE_SETUP);
  PORTEVENT_SETMASKBIT(pIpMapInfo->nimEventMask, L7_LAG_RELEASE);
  PORTEVENT_SETMASKBIT(pIpMapInfo->nimEventMask, L7_PROBE_TEARDOWN);
  PORTEVENT_SETMASKBIT(pIpMapInfo->nimEventMask, L7_SPEED_CHANGE);
  PORTEVENT_SETMASKBIT(pIpMapInfo->nimEventMask, L7_SET_MTU_SIZE);
  nimRegisterIntfEvents(L7_IP_MAP_COMPONENT_ID, pIpMapInfo->nimEventMask);

  nimStartupEventDone(L7_IP_MAP_COMPONENT_ID);

  if (ipMapTraceFlags & IPMAP_TRACE_NSF)
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX,
                  "IP MAP completed NIM activate startup");
    ipMapTraceWrite(traceBuf);
  }

  /* If this is a warm restart, apply checkpointed data. */
  if (pIpMapInfo->warmRestart)
  {
#ifdef L7_NSF_PACKAGE
    ipMapCheckpointDataApply();

    /* Apply ARP checkpointed data */
    arpMapCheckpointDataApply();

    /* If we are not expecting any protocol to add any NSF routes,
     * then flush any stale checkpointed routes now. Otherwise, defer
     * flush until last NSF protocol has updated RTO. */
    if (pIpMapInfo->nsfRoutesPending == 0)
    {
      ipMapStaleRoutesFlush();
    }

    /* All saved configuration has been applied by now, as text configuration has
       completely been applied prior to this routine being invoked.
       Flush any remaining checkpointed data */
    ipMapCheckpointRtrIntfDataFlush();

    pIpMapInfo->warmRestartCompleted = L7_TRUE;
#endif

    /* Now checkpointed routes have been cleaned up and driver is ready to
     * accept route updates. Register with RTO for best route changes. */
    if ((rtoBestRouteClientRegister("IP MAP", ipMapRouteCallback)) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_IP_MAP_COMPONENT_ID,
              "Unable to register best route callback with RTO");
      return L7_ERROR;
    }
  }
  else
  {
    /* ARP is not direct NIM client */
    /* So update configurator that the hardware update by ARP component is done
     * during cold restart case */
    cnfgrApiComponentHwUpdateDone(L7_ARP_MAP_COMPONENT_ID, L7_CNFGR_HW_APPLY_L3);
  }

  /* If routing is disabled, or if no routing interfaces are up, then tell
   * Configurator that IP MAP is finished updating its L3 hw tables. Otherwise,
   * wait until IP MAP processes a complete set of best routes in
   * ipMapBestRoutesGet(). */
  if ((ipMapCfg->rtr.rtrAdminMode != L7_ENABLE) || !oneIntfUp)
  {
    if (ipMapTraceFlags & IPMAP_TRACE_NSF)
    {
      ipMapTraceWrite("No IPv4 routes. IP MAP reporting L3 hardware updates done.");
    }
    cnfgrApiComponentHwUpdateDone(L7_IP_MAP_COMPONENT_ID, L7_CNFGR_HW_APPLY_L3);
    pIpMapInfo->l3HwUpdateComplete = L7_TRUE;
  }

  /* Since interfaces can only transition from disabled to enabled here,
   * all router events are synchronous. Therefore, we are guaranteed that
   * IP MAP has sent all the router events required given the configuration
   * and current NIM interface state. So tell router event clients that
   * startup router events are complete. */
  ipMapRoutingEventChangeNotify(0, L7_RTR_STARTUP_DONE, L7_FALSE, L7_NULL);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Test if warm restart has completed for IP MAP
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL ipMapWarmRestartCompleted(void)
{
  /* We are in the midst of a warm restart if the last boot was a warm restart,
     but warm restart has not completed*/

  L7_BOOL warmRestartCompleted;

  warmRestartCompleted = L7_FALSE;

  if (pIpMapInfo->warmRestart == L7_TRUE)
  {
      if (pIpMapInfo->warmRestartCompleted == L7_TRUE)
      {
          warmRestartCompleted = L7_TRUE;
      }
  }
  else
  {
      /* There was no warm restart, so indicate completion */
      warmRestartCompleted = L7_TRUE;
  }
  return warmRestartCompleted;
}

/*********************************************************************
* @purpose  Test if backup is elected
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL ipMapBackupElected(void)
{
  return pIpMapInfo->backupElected;
}

/*********************************************************************
* @purpose  Process link state changes for the IP Mapping Layer
*
* @param    intIfNum        Internal Interface Number
* @param    event           event, defined by L7_PORT_EVENTS_t
* @param    correlator      correlator for the event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    L7_PORT_ROUTING_ENABLED and L7_PORT_ROUTING_DISABLED events
* @notes    are not handled by this routine, as these events originate
* @notes    from inside the IP Mapping layer, as opposed to the IP Mapping
* @notes    layer responding to these events.
*
* @end
*********************************************************************/
L7_RC_t ipMapIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{
  ipMapMsg_t   Message;
  L7_RC_t rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t status;

  /* If this is an event we are not interested in, perform an early return*/
  if (event != L7_CREATE &&
      event != L7_ATTACH &&
      event != L7_DETACH &&
      event != L7_DELETE &&
      event != L7_ACTIVE &&
      event != L7_INACTIVE &&
      event != L7_LAG_ACQUIRE &&
      event != L7_PROBE_SETUP &&
      event != L7_LAG_RELEASE &&
      event != L7_PROBE_TEARDOWN &&
      event != L7_SPEED_CHANGE &&
      event != L7_SET_MTU_SIZE)
  {
    status.intIfNum     = intIfNum;
    status.component    = L7_IP_MAP_COMPONENT_ID;
    status.response.rc  = L7_SUCCESS;
    status.response.reason = NIM_ERR_RC_UNUSED;
    status.event        = event;
    status.correlator   = correlator;
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }

  Message.msgId = IPMAP_INTF_CHANGE;
  Message.type.intfChangeEvent.intIfNum = intIfNum;
  Message.type.intfChangeEvent.event = event;
  Message.type.intfChangeEvent.correlator = correlator;

  rc = osapiMessageSend(ipMapProcess_Queue, &Message, sizeof(ipMapMsg_t),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

  return rc;
}

/*********************************************************************
* @purpose  Process link state changes for the IP Mapping Layer
*
* @param    intIfNum        Internal Interface Number
* @param    event           event, defined by L7_PORT_EVENTS_t
* @param    correlator      correlator for the event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    L7_PORT_ROUTING_ENABLED and L7_PORT_ROUTING_DISABLED events
* @notes    are not handled by this routine, as these events originate
* @notes    from inside the IP Mapping layer, as opposed to the IP Mapping
* @notes    layer responding to these events.
*
* @end
*********************************************************************/
L7_RC_t ipMapIntfChangeProcess (L7_uint32 intIfNum, L7_uint32 event,
                                NIM_CORRELATOR_t correlator)
{
  L7_BOOL          event_completed;
  NIM_EVENT_COMPLETE_INFO_t status;
  ipMapIntfChangeParms_t *change_parms;
  L7_RC_t   rc = L7_SUCCESS;

  L7_uint32 ipMtu;
  L7_uint32 linkMtu;
  L7_rtrCfgCkt_t *pCfg;     /* interface config for intIfNum */

  /*------------------------------------*/
  /* Initialize the NIM status callback */
  /*------------------------------------*/

  status.intIfNum     = intIfNum;
  status.component    = L7_IP_MAP_COMPONENT_ID;
  status.response.rc  = L7_SUCCESS;
  status.response.reason = NIM_ERR_RC_UNUSED;
  status.event        = event;
  status.correlator   = correlator;


  /* Return if IP map does not care about the interface */
  if (ipMapIsValidIntf(intIfNum) != L7_TRUE)
  {
    if ((ipMapTraceFlags & IPMAP_TRACE_NIM_EVENTS) &&
        (!ipMapTraceIntf || (ipMapTraceIntf == intIfNum)))
    {
       L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
       L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
       nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
       osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX,
                     "IP MAP ignoring NIM event %s for invalid IP MAP interface %s.",
                     nimGetIntfEvent(event), ifName);
       ipMapTraceWrite(traceBuf);
    }
    nimEventStatusCallback(status);
    return rc;
  }


  /* Return if there is already an outstanding NIM event for the interface
     and the interface is currently a router interface */
  if (pIpMapInfo->operIntf[intIfNum].nimParms.correlator != L7_NULL)
      {
          /* We cannot handle two asynchronous callbacks on the same interface
             at this time from NIM */
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "More than one outstanding NIM callback on interface %s", ifName);
    rc = L7_FAILURE;
    status.response.rc  = rc;
    if ((ipMapTraceFlags & IPMAP_TRACE_NIM_EVENTS) &&
        (!ipMapTraceIntf || (ipMapTraceIntf == intIfNum)))
    {
      L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
      osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX, "IP MAP ignoring 2nd NIM event %s for interface %s.",
                    nimGetIntfEvent(event), ifName);
              ipMapTraceWrite(traceBuf);
          }
          nimEventStatusCallback(status);
          return L7_FAILURE;
  }

  event_completed   = L7_TRUE;

  if (IPMAP_IS_READY)
  {
      if ((ipMapTraceFlags & IPMAP_TRACE_NIM_EVENTS) &&
          (!ipMapTraceIntf || (ipMapTraceIntf == intIfNum)))
      {
          L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
          osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX,
                        "IP MAP processing NIM event %s for interface %s.",
                        nimGetIntfEvent(event), ifName);
          ipMapTraceWrite(traceBuf);
      }
    switch (event)
    {
      case (L7_CREATE):
        rc = ipMapIntfCreate(intIfNum, &event_completed);

        /* Tell router event clients about interface create */
        event_completed = L7_FALSE;
        ipMapRoutingEventChangeNotify(intIfNum, L7_RTR_INTF_CREATE, L7_TRUE, L7_NULL);
        pIpMapInfo->operIntf[intIfNum].lastNotify = L7_RTR_INTF_CREATE;
        pIpMapInfo->operIntf[intIfNum].asyncPending = L7_TRUE;
        break;

      case (L7_DELETE):
        rc = ipMapIntfDelete(intIfNum, &event_completed);

        /* Tell router event clients about interface delete */
        event_completed = L7_FALSE;
        ipMapRoutingEventChangeNotify(intIfNum, L7_RTR_INTF_DELETE, L7_TRUE, L7_NULL);
        pIpMapInfo->operIntf[intIfNum].lastNotify = L7_RTR_INTF_DELETE;
        pIpMapInfo->operIntf[intIfNum].asyncPending = L7_TRUE;
        break;

    case (L7_ATTACH):
    case (L7_DETACH):
      case (L7_ACTIVE):
//    case (L7_INACTIVE):
      case (L7_LAG_ACQUIRE):
      case (L7_PROBE_SETUP):
      case (L7_LAG_RELEASE):
      case (L7_PROBE_TEARDOWN):
        ipMapIntfUpdate(intIfNum, &event_completed);
          break;

      case (L7_SPEED_CHANGE):
        /* propagate event to router event clients. Don't see any need to make
         * this event asynchronous. Important not to set lastNotify here. */
        ipMapRoutingEventChangeNotify(intIfNum, L7_RTR_INTF_SPEED_CHANGE, L7_FALSE, L7_NULL);
        break;

      case (L7_SET_MTU_SIZE):
          if ((ipMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE) &&
              (nimGetIntfMtuSize(intIfNum, &linkMtu) == L7_SUCCESS))
          {
            /* If IP MTU not configured, or if user has configured an IP MTU that
             * is now invalid because the link MTU has gotten smaller, apply the
             * default IP MTU. Retain configured value in configuration though. */
            ipMtu = pCfg->ipMtu;
            if ((ipMtu == FD_IP_DEFAULT_IP_MTU) || (ipMtu > linkMtu))
            {
              L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
              nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
              /* note intentionally 0, not link mtu */
              /* If this fails, don't report failure to NIM (and reset the box!).
               * Linux 2.4 doesn't accept an IP MTU > 1500. */
              if (ipMapIntfIpMtuApply(intIfNum, FD_IP_DEFAULT_IP_MTU) == L7_SUCCESS)
              {
                L7_LOGF(L7_LOG_SEVERITY_INFO, L7_IP_MAP_COMPONENT_ID,
                        "On interface %s, setting IPv4 MTU to %u because of a change to the link MTU."
                        " If the IPv4 MTU is not configured on an interface, it is automatically adjusted"
                        " based on the link MTU of the interface.",
                        ifName, linkMtu);
              }
              else
              {
                L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                        "On interface %s, attempted to set IPv4 MTU to %u because of a change "
                        "to the link MTU. IPv4 MTU change failed.",
                        ifName, linkMtu);
              }
            }
          }
        break;

      default:
        rc = L7_SUCCESS;
    }
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_IP_MAP_COMPONENT_ID,
            "Received interface change while in phase %s",
            ipMapCnfgrStateNames[ipMapCnfgrState]);
  }


  /*-------------------*/
  /* Event completion? */
  /*-------------------*/

  if (event_completed == L7_TRUE)
  {
     /* Return event completion status  */
      status.response.rc  = rc;
      memset(&pIpMapInfo->operIntf[intIfNum].nimParms, 0,
             sizeof(ipMapIntfChangeParms_t));

      if ((ipMapTraceFlags & IPMAP_TRACE_NIM_EVENTS) &&
          (!ipMapTraceIntf || (ipMapTraceIntf == intIfNum)))
      {
          L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
          osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX,
                        "IP MAP responding to NIM event %s intf %s.",
                        nimGetIntfEvent(event), ifName);
          ipMapTraceWrite(traceBuf);
      }
      nimEventStatusCallback(status);
  }
  else
  {
    /* save for later nimEventStatusCallback */
      change_parms = &pIpMapInfo->operIntf[intIfNum].nimParms;
      change_parms->intIfNum = intIfNum;
      change_parms->event = event;
      change_parms->correlator = correlator;

      if ((ipMapTraceFlags & IPMAP_TRACE_NIM_EVENTS) &&
          (!ipMapTraceIntf || (ipMapTraceIntf == intIfNum)))
      {
          L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
          osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX,
                        "IP MAP deferring response to NIM event %u intf %s.",
                  event, ifName);
          ipMapTraceWrite(traceBuf);
      }
  }

  return rc;
}

/*********************************************************************
* @purpose  Add a route to the hardware's routing table.
*
* @param    pRouteEntry     Pointer to a route entry structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapRouteEntryAdd(L7_routeEntry_t *pRouteEntry)
{
    L7_RC_t       rc;
    L7_uchar8     ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8     netMaskStr[OSAPI_INET_NTOA_BUF_SIZE];

    /* Don't Add the Host Interface routes to the Hardware */
    if (ipMapCfg->rtr.rtrAdminMode != L7_ENABLE)
    {
      return L7_SUCCESS;
    }
    if (ipMapTraceFlags & IPMAP_TRACE_ROUTES)
    {
        L7_uint32 nh;
        /* each next hop can contribute 17 characters */
        #define IPMAPROUTEENTRYADD_TRACE_LEN (50 + platRtrRouteMaxEqualCostEntriesGet() * (OSAPI_INET_NTOA_BUF_SIZE + 2))
        L7_uchar8 traceBuf[IPMAPROUTEENTRYADD_TRACE_LEN];
        L7_uchar8 nhStr[OSAPI_INET_NTOA_BUF_SIZE];
        osapiInetNtoa((L7_uint32)pRouteEntry->ipAddr, ipAddrStr);
        osapiInetNtoa((L7_uint32)pRouteEntry->subnetMask, netMaskStr);
        osapiSnprintf(traceBuf, IPMAPROUTEENTRYADD_TRACE_LEN, "IP MAP to DTL: add %s route to %s %s via ",
                rtoProtoNames[pRouteEntry->protocol], ipAddrStr, netMaskStr);
        for (nh = 0; nh < pRouteEntry->ecmpRoutes.numOfRoutes; nh++)
        {
            osapiInetNtoa((L7_uint32)pRouteEntry->ecmpRoutes.equalCostPath[nh].arpEntry.ipAddr,
                          nhStr);
            strcat(traceBuf, nhStr);
            if (nh + 1 < pRouteEntry->ecmpRoutes.numOfRoutes)
                strcat(traceBuf, ", ");
        }
        ipMapTraceWrite(traceBuf);
    }

    rc = dtlIpv4RouteEntryAdd(pRouteEntry);

    if (rc == L7_TABLE_IS_FULL)
    {
        osapiInetNtoa((L7_uint32)pRouteEntry->ipAddr, ipAddrStr);
        osapiInetNtoa((L7_uint32)pRouteEntry->subnetMask, netMaskStr);
        IPMAP_ERROR("Unable to add %s %s to the hardware routing table. Table full.\n",
                  ipAddrStr, netMaskStr);
        return L7_TABLE_IS_FULL;
    }

    if (rc != L7_SUCCESS)
    {
        osapiInetNtoa((L7_uint32)pRouteEntry->ipAddr, ipAddrStr);
        osapiInetNtoa((L7_uint32)pRouteEntry->subnetMask, netMaskStr);
        IPMAP_ERROR("Failed to add %s %s to the hardware routing table.\n",
                  ipAddrStr, netMaskStr);
        return L7_FAILURE;
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Modify a route in the system cache
*
* @param    pRouteEntry     Pointer to a route entry structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapRouteEntryModify(L7_routeEntry_t *pRouteEntry)
{
    L7_uchar8     ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8     netMaskStr[OSAPI_INET_NTOA_BUF_SIZE];

    /* Don't Modify the Host Interface routes in the Hardware */
    if (ipMapCfg->rtr.rtrAdminMode != L7_ENABLE)
    {
      return L7_SUCCESS;
    }
  if (ipMapTraceFlags & IPMAP_TRACE_ROUTES)
  {
      L7_uint32 nh;
      /* each next hop can contribute 17 characters */
      #define IPMAPROUTEENTRYMODIFY_TRACE_LEN (50 + platRtrRouteMaxEqualCostEntriesGet() * (OSAPI_INET_NTOA_BUF_SIZE + 2))
      L7_uchar8 traceBuf[IPMAPROUTEENTRYMODIFY_TRACE_LEN];
      L7_uchar8 nhStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa((L7_uint32)pRouteEntry->ipAddr, ipAddrStr);
      osapiInetNtoa((L7_uint32)pRouteEntry->subnetMask, netMaskStr);
      osapiSnprintf(traceBuf, IPMAPROUTEENTRYMODIFY_TRACE_LEN, "IP MAP to DTL: modify %s route to %s %s via ",
              rtoProtoNames[pRouteEntry->protocol], ipAddrStr, netMaskStr);
      for (nh = 0; nh < pRouteEntry->ecmpRoutes.numOfRoutes; nh++)
      {
          osapiInetNtoa((L7_uint32)pRouteEntry->ecmpRoutes.equalCostPath[nh].arpEntry.ipAddr,
                        nhStr);
          strcat(traceBuf, nhStr);
          if (nh + 1 < pRouteEntry->ecmpRoutes.numOfRoutes)
              strcat(traceBuf, ", ");
      }
      ipMapTraceWrite(traceBuf);
  }

  if (dtlIpv4RouteEntryModify(pRouteEntry) != L7_SUCCESS)
  {
      osapiInetNtoa((L7_uint32)pRouteEntry->ipAddr, ipAddrStr);
      osapiInetNtoa((L7_uint32)pRouteEntry->subnetMask, netMaskStr);
      IPMAP_ERROR("Failed to modify route to %s %s in the hardware routing table. \n",
                  ipAddrStr, netMaskStr);
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete a route from the system cache
*
* @param    pRouteEntry     Pointer to a route entry structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapRouteEntryDelete(L7_routeEntry_t *pRouteEntry)
{
    L7_uchar8     ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8     netMaskStr[OSAPI_INET_NTOA_BUF_SIZE];

  /* Don't Delete the Host Interface routes from the Hardware */
  if (ipMapCfg->rtr.rtrAdminMode != L7_ENABLE)
  {
    return L7_SUCCESS;
  }
  if (ipMapTraceFlags & IPMAP_TRACE_ROUTES)
  {
      L7_uint32 nh;
      /* each next hop can contribute 17 characters */
      #define IPMAPROUTEENTRYDELETE_TRACE_LEN (50 + platRtrRouteMaxEqualCostEntriesGet() * (OSAPI_INET_NTOA_BUF_SIZE + 2))
      L7_uchar8 traceBuf[IPMAPROUTEENTRYDELETE_TRACE_LEN];
      L7_uchar8 nhStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa((L7_uint32)pRouteEntry->ipAddr, ipAddrStr);
      osapiInetNtoa((L7_uint32)pRouteEntry->subnetMask, netMaskStr);
      osapiSnprintf(traceBuf, IPMAPROUTEENTRYDELETE_TRACE_LEN, "IP MAP to DTL: delete %s route to %s %s via ",
              rtoProtoNames[pRouteEntry->protocol], ipAddrStr, netMaskStr);
      for (nh = 0; nh < pRouteEntry->ecmpRoutes.numOfRoutes; nh++)
      {
          osapiInetNtoa((L7_uint32)pRouteEntry->ecmpRoutes.equalCostPath[nh].arpEntry.ipAddr,
                        nhStr);
          strcat(traceBuf, nhStr);
          if (nh + 1 < pRouteEntry->ecmpRoutes.numOfRoutes)
              strcat(traceBuf, ", ");
      }
      ipMapTraceWrite(traceBuf);
  }

  if (dtlIpv4RouteEntryDelete(pRouteEntry) != L7_SUCCESS)
  {
      osapiInetNtoa((L7_uint32)pRouteEntry->ipAddr, ipAddrStr);
      osapiInetNtoa((L7_uint32)pRouteEntry->subnetMask, netMaskStr);
      IPMAP_ERROR("Failed to delete route to %s %s from the hardware routing table.\n",
                  ipAddrStr, netMaskStr);
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add an ARP entry to the system cache
*
* @param    pArpEntry       Pointer to an ARP entry structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpEntryAdd(L7_arpEntry_t *pArpEntry)
{
  /* Don't Add the Host Interface entries to the Hardware */
  if (ipMapCfg->rtr.rtrAdminMode != L7_ENABLE)
  {
    return L7_SUCCESS;
  }
  if (dtlIpv4ArpEntryAdd(pArpEntry) != L7_SUCCESS)
  {
    /* NOTE: Not showing an error message here, since some devices cannot
     *       accept every ARP entry, even when there is room available in
     *       their ARP cache.
     */
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Modify an ARP entry in the system cache
*
* @param    pArpEntry       Pointer to an ARP entry structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpEntryModify(L7_arpEntry_t *pArpEntry)
{

  static const char *routine_name = "ipMapArpEntryModify()";
  L7_uchar8     ipStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(pArpEntry->intIfNum, L7_SYSNAME, ifName);

  /* Don't Modify the Host Interface entries in the Hardware */
  if (ipMapCfg->rtr.rtrAdminMode != L7_ENABLE)
  {
    return L7_SUCCESS;
  }
  IPMAP_TRACE("%s : IP Address %#x and intf %d, %s\n",
              routine_name, pArpEntry->ipAddr, pArpEntry->intIfNum, ifName);

  if (dtlIpv4ArpEntryModify(pArpEntry) != L7_SUCCESS)
  {
    osapiInetNtoa((L7_uint32)pArpEntry->ipAddr, ipStr);
    IPMAP_ERROR("%s %d: %s: ARP modification failed for IP address %s\n",
                __FILE__, __LINE__, routine_name, ipStr);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete an ARP entry from the system cache
*
* @param    pArpEntry       Pointer to an ARP entry structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpEntryDelete(L7_arpEntry_t *pArpEntry)
{

  static const char *routine_name = "ipMapArpEntryDelete()";
  L7_uchar8     ipStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(pArpEntry->intIfNum, L7_SYSNAME, ifName);

  /* Don't Delete the Host Interface entries from the Hardware */
  if (ipMapCfg->rtr.rtrAdminMode != L7_ENABLE)
  {
    return L7_SUCCESS;
  }
  IPMAP_TRACE("%s : IP Address %#x and intf %d, %s\n",
              routine_name, pArpEntry->ipAddr, pArpEntry->intIfNum, ifName);


  if (dtlIpv4ArpEntryDelete(pArpEntry) != L7_SUCCESS)
  {
    osapiInetNtoa((L7_uint32)pArpEntry->ipAddr, ipStr);
    IPMAP_ERROR("%s %d: %s: ARP deletion failed for IP address %s\n",
                __FILE__, __LINE__, routine_name, ipStr);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/*===================== CIRCUIT CONFIGURATION =====================*/
/*===================== CIRCUIT CONFIGURATION =====================*/
/*===================== CIRCUIT CONFIGURATION =====================*/
/*===================== CIRCUIT CONFIGURATION =====================*/
/*===================== CIRCUIT CONFIGURATION =====================*/
/*===================== CIRCUIT CONFIGURATION =====================*/


/*======================= IP INIT  ================================*/

/* ipInit - Must be called after ipStartTasks */

/*********************************************************************
* @purpose  starts the ip forwarding task
*
* @param    none
*
* @returns  void
*
* @notes    The forwarding task reads from the queue used for data
*           packets. If the queue contains no packet, the task
*           sleeps waiting on receive.  It also sleeps for a short
*           while after processing FD_CNFGR_IP_MAX_MSGS_SERVICED
*           number of packets.
*
* @end
*********************************************************************/
void ipMapForwardingTask(void)
{
    static const char   *routine_name = "ipMapForwardingTask()";

    ipMapMsg_t          Message;
    L7_RC_t             status;
    L7_uint32           numTries;
    static L7_uint32    currentQ = 0;
    L7_uint32           ipMap_service_count = 0;  /* Maximum number messages to service
                                                    per invocation of task */

    osapiTaskInitDone(L7_IPMAP_FORWARDING_TASK_SYNC);

    /* Main body of task */

    for (;;)
    {
        /*
        Use a semaphore to regulate access to the various queues
        */

        if (osapiSemaTake(ipMapMsgQSema, L7_WAIT_FOREVER) != L7_SUCCESS)
        {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                   "Failure taking ipMap message queue semaphore.");
            continue;
        }

        /* Zero the message */

        memset(&Message, 0x00, sizeof(ipMapMsg_t));

        /*
        Get a message from the current queue

        Currently, the algorithm for the queue checking will
        be round-robin.  It can be modified later if some type
        of priority checking will be used.
        */

        status = L7_ERROR;
        numTries = 0;

        while ((status != L7_SUCCESS) && (numTries < L7_L3_FORWARD_QUEUE_COUNT))
        {
            L7_uint32 ForwardQLen;

            status = osapiMessageReceive(ipMapForwardQueue[currentQ],
                                         &Message, sizeof(ipMapMsg_t),
                                         L7_NO_WAIT);

            if (osapiMsgQueueGetNumMsgs(ipMapForwardQueue[currentQ], &ForwardQLen) == L7_SUCCESS)
            {
                /*
                Account for current message just received
                */

                if (ForwardQLen+1 > ipMapForwardQueueHWMark[currentQ])
                    ipMapForwardQueueHWMark[currentQ] = ForwardQLen+1;
            }

            currentQ++;
            currentQ %= L7_L3_FORWARD_QUEUE_COUNT;
            numTries++;
        }

        if (status == L7_SUCCESS)
        {
            switch (Message.msgId)
            {
                case IPMAP_PDU:
                    /*
                    PDU received

                    discard all frames while an IP Map config restore is in progress
                    */

                    if (ipMapCnfgrState == IPMAP_PHASE_EXECUTE)
                    {
                        ipForwardPktsIn1++;
                        ipMapPduRcv((ipMapMsg_t *)&Message);
                    }
                    else
                    {
                        ipForwardPktsDropNotExe++;
                        SYSAPI_NET_MBUF_FREE(Message.type.pdu.bufHandle);
                    }
                    break;

                default:
                    break;
            } /* endswitch */
        }
        else
        {
            IPMAP_ERROR("%s %d: %s: Bad status %08x\n",
                        __FILE__, __LINE__, routine_name, status);
        }

        /* Let other tasks run, if required.                */

        ipMap_service_count++;

        if (ipMap_service_count == FD_CNFGR_IP_MAX_MSGS_SERVICED)
        {
            ipMap_service_count = 0;
            (void) osapiTaskYield();
        }

    } /* for (;;) */

} /* ipMapForwardingTask */


void ipMapQLenShow(void)
{
  L7_int32 ipMapQLen;
  if (osapiMsgQueueGetNumMsgs(ipMapProcess_Queue, &ipMapQLen) == L7_SUCCESS)
  {
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                "\nIP MAP processing task queue...\n  current:  %u\n  high water:  %u\n  max: %d)",
                ipMapQLen, ipMapQHighWaterMark, IPMAP_PROC_MSG_COUNT);
}
}

void ipMapQLenReset(void)
{
  ipMapQHighWaterMark = 0;
}

/*********************************************************************
* @purpose  starts the ip processing task
*
* @param    none
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapProcessingTask(void)
{
  static const char *routine_name = "ipMapProcessingTask()";

  ipMapMsg_t   Message;
  L7_uint32    status;
  L7_uint32    ipMap_service_count = 0;  /* Maximum number messages to service
                                                    per invocation of task */

  osapiTaskInitDone(L7_IPMAP_PROCESSING_TASK_SYNC);

  /* Main body of task */
  for (;;)
  {
    /* zero the message */
    memset(&Message,0x00,sizeof(ipMapMsg_t));

    /* Don't try process anything unless the Queue has been created */
    if (ipMapProcess_Queue != L7_NULL)
    {
      /*  Get a Message From The Queue */
      status = osapiMessageReceive(ipMapProcess_Queue,
                                   &Message,
                                   sizeof(ipMapMsg_t),
                                   L7_WAIT_FOREVER);

      if (status == L7_SUCCESS)
      {
          if (ipMapTraceFlags & IPMAP_TRACE_PROC_TASK)
          {
              L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
              osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX, "IP MAP processing %s message.",
                      ipMapProcMessageNames[Message.msgId]);
              ipMapTraceWrite(traceBuf);
          }
          {
          L7_int32 ipMapQLen;
          if (osapiMsgQueueGetNumMsgs(ipMapProcess_Queue, &ipMapQLen) == L7_SUCCESS)
          {
            if (ipMapQLen > ipMapQHighWaterMark)
              ipMapQHighWaterMark = ipMapQLen;
          }
          }
        switch (Message.msgId)
        {
          case IPMAP_CNFGR:
            ipMapCnfgrParse(&Message.type.CmdData);
            break;

          case IPMAP_RESTORE_EVENT:
            if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
              break;
            ipMapRestoreProcess();
            ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
            break;

          case IPMAP_CFG_COMPLETE:
            if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
              break;
            ipMapCfgComplete(Message.type.cfgCompleteEvent.event);
            ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
            break;

          case IPMAP_STALE_ROUTE_TIMER:
            if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
              break;
            L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_IP_MAP_COMPONENT_ID,
                    "%s failed to report completion of routing table update",
                    ipMapNsfRoutesPendingString());
            pIpMapInfo->nsfRoutesPending = 0;
            ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
#ifdef L7_NSF_PACKAGE
            ipMapStaleRoutesFlush();
#endif
            break;

          case IPMAP_NIM_STARTUP:
            if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
              break;
            if (Message.type.nimStartupEvent.startupPhase == NIM_INTERFACE_CREATE_STARTUP)
              ipMapNimCreateStartup();
            else if (Message.type.nimStartupEvent.startupPhase == NIM_INTERFACE_ACTIVATE_STARTUP)
              ipMapNimActivateStartup();
            else
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                      "Unknown NIM startup event %d", Message.type.nimStartupEvent.startupPhase);
            ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
            break;

          case IPMAP_INTF_CHANGE:
            if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
              break;
            ipMapIntfChangeProcess(Message.type.intfChangeEvent.intIfNum,
                                   Message.type.intfChangeEvent.event,
                                   Message.type.intfChangeEvent.correlator);
            ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
            break;

          case IPMAP_ASYNC_EVENT_COMPLETE:
              if (ipMapTraceFlags & IPMAP_TRACE_PROC_TASK)
              {
                  L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
                  osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX, "IP MAP async event intf %d, event %d",
                          Message.type.asyncEvent.type.rtrInfo.intIfNum,
                          Message.type.asyncEvent.type.rtrInfo.event);
                  ipMapTraceWrite(traceBuf);
              }
            if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
              break;
             ipMapAsyncEventCompleteProcess(&Message.type.asyncEvent);
            ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);
          break;

          case IPMAP_RTO_EVENT:
          if (ipMapLockTake (IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__)
                          == L7_SUCCESS)
          {
            ipMapBestRoutesGet();
            ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);
          }
          break;

          default:
            break;
        } /* endswitch */
      }
      else
      {
        IPMAP_ERROR("%s %d: %s: L7_ip_processing_task: Bad status %08x\n",
                    __FILE__, __LINE__, routine_name, status);
      }
    }
    /* Let other tasks run, if required.                */
    ipMap_service_count++;
    if (ipMap_service_count == FD_CNFGR_IP_MAX_MSGS_SERVICED)
    {
      ipMap_service_count = 0;
      (void) osapiTaskYield();
    }

  } /* for (;,;,;) */

} /* L7_ip_processing_task */

/*********************************************************************
* @purpose  Process an received IP message
*
* @param    ipMapMsg_t  Message received on the queue
*
* @returns  L7_SUCCESS  If the frame has been consumed successfully
* @returns  L7_FAILURE  If the frame has not been consumed successfully
*
* @notes   This function is called from L7_ip_forwarding_task() on the
* @notes   IpFwdTask task.  The sysapi network buffer is passed in via
* @notes   the ipMapMsg_t message structure.   If the frame is consumed
* @notes   successfully, the buffer should be freed by the consumer,
* @notes   otherwise it is freed here.
*
* @end
*********************************************************************/
L7_RC_t ipMapPduRcv(ipMapMsg_t *msg)
{
  /* Send message to the IP forwarder*/
  /* IP Forwarder processes message in one of several ways:

      1. sends it to the local IP Management stack (i.e. ARP or IP packet),
         which sends it to the appropriate enabled routing protocol,  OR
      2. forwards the frame out of the appropriate interface, OR
      3. discards the frame.

  */
  L7_RC_t rc;
  sysnet_pdu_info_t pduInfo;
  L7_char8    *data;
  L7_netBufHandle bufHandle;
  L7_ushort16 *pEtype;
  L7_ushort16 protocol_type;
  L7_uint32 offset;
  L7_uint32 destIpAddr;
  L7_ulong32 tmp;
  L7_uchar8  *tmpip;


  bufHandle = msg->type.pdu.bufHandle;
  SYSAPI_NET_MBUF_SET_LOC(bufHandle, MBUF_LOC_IP4_FWD);
  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);

  /*Get the offset where the actual data start devoid of L2 headers*/
  offset = sysNetDataOffsetGet(data);

  /*Determine the protocol type*/
  pEtype = (L7_ushort16 *)(data + offset - 2);
  protocol_type = osapiNtohs(*pEtype);

  bzero((char *)&pduInfo, sizeof(sysnet_pdu_info_t));
  pduInfo.intIfNum = msg->type.pdu.intf;
  pduInfo.rxPort = msg->type.pdu.dtlIntf;
  pduInfo.vlanId = msg->type.pdu.vlanId;

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  if (protocol_type == L7_ETYPE_IP)
  {
    L7_ipHeader_t *ipHdr = (L7_ipHeader_t *)(data + offset);
    void *layer4data = (void *)(data + offset + L7_IP_HDR_LEN);
    ipMapDebugPacketRxTrace(pduInfo.intIfNum, pduInfo.vlanId,ipHdr, layer4data);
  }
#endif /*L7_QOS_FLEX_PACKAGE_ACL*/
  if ((protocol_type == L7_ETYPE_IP) &&
      (ipMapTraceFlags & IPMAP_TRACE_FORWARDING) &&
      (!ipMapTraceIntf || (ipMapTraceIntf == msg->type.pdu.intf)))
  {
    L7_ipHeader_t *ipHdr = (L7_ipHeader_t *)(data + offset);
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    L7_uchar8 srcStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    if (nimGetIntfName(msg->type.pdu.intf, L7_SYSNAME, ifName) != L7_SUCCESS)
      strcpy(ifName, "invalid interface");
    osapiInetNtoa(osapiNtohl(ipHdr->iph_src), srcStr);
    osapiInetNtoa(osapiNtohl(ipHdr->iph_dst), destStr);
    osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX, "Received IP packet on %s vlan %d: ver/hlen %#4.2x, tos %d, len %d, id %d, "
            "flags/offset %#4.2x, ttl %d, proto %d, src %s, dst %s.\n",
            ifName, msg->type.pdu.vlanId, ipHdr->iph_versLen, ipHdr->iph_tos,
            osapiNtohs(ipHdr->iph_len), osapiNtohs(ipHdr->iph_ident),
            ipHdr->iph_flags_frag, ipHdr->iph_ttl,
            ipHdr->iph_prot, srcStr, destStr);
    ipMapTraceWrite(traceBuf);
  }

  /* For builds without routing support the ipmRouterIfFrameProcess always
  ** returns an error.
  */
  rc = ipmRouterIfFrameProcess (msg->type.pdu.bufHandle, &pduInfo);
  if (rc != L7_SUCCESS)
  {
    /* If packet arrived on mgmt vlan on port that's a member of the
     * mgmt vlan, send it to the end driver. */
    if ((pduInfo.vlanId == simMgmtVlanIdGet()) &&
        dot1qIntfIsVlanMember(pduInfo.vlanId, pduInfo.rxPort))
    {
      /*Check if it is IP or ARP packet*/
      if (protocol_type == L7_ETYPE_IP)
      {
        L7_ipHeader_t *ip_header;

        /* If the packet is destined for the system IP address,
         * change to the interface that the packet originally
         * came in on (was saved by DTL when it converted to the
         * VLAN routing interface).
         */
        ip_header = (L7_ipHeader_t *)(data + offset);
        destIpAddr = (L7_uint32)osapiNtohl((L7_ulong32)ip_header->iph_dst);
        if (destIpAddr == simGetSystemIPAddr())
          pduInfo.intIfNum = msg->type.pdu.dtlIntf;

        /*It's an IP packet */
        ipForwardIpPktsToEnd++;
        rc = dtlIPProtoRecv(msg->type.pdu.bufHandle, &pduInfo);
        if (rc != L7_SUCCESS)
          SYSAPI_NET_MBUF_FREE(msg->type.pdu.bufHandle);
      }
      else if (protocol_type == L7_ETYPE_ARP)
      {
        L7_ether_arp_t *arp_header;

        /* If the packet is destined for the system IP address,
         * change to the interface that the packet originally
         * came in on (was saved by DTL when it converted to the
         * VLAN routing interface).
         */
        arp_header = (L7_ether_arp_t *)(data + offset);
          tmpip = arp_header->arp_tpa;
          tmp = (tmpip[0] << 24) | (tmpip[1] << 16) | (tmpip[2] << 8) | (tmpip[3] ) ;
          destIpAddr = (L7_uint32) osapiNtohl(tmp);

        if (destIpAddr == simGetSystemIPAddr())
          pduInfo.intIfNum = msg->type.pdu.dtlIntf;

        /*It's an ARP packet */
        ipForwardArpToEnd++;
        rc = dtlARPProtoRecv(msg->type.pdu.bufHandle, &pduInfo);
        if (rc != L7_SUCCESS)
          SYSAPI_NET_MBUF_FREE(msg->type.pdu.bufHandle);
      }
      else
      {
        rc = L7_FAILURE;
        SYSAPI_NET_MBUF_FREE(msg->type.pdu.bufHandle);
      }
    }
    else
    {
      rc = L7_FAILURE;
      SYSAPI_NET_MBUF_FREE(msg->type.pdu.bufHandle);
    }
  }
  return rc;
}



/*********************************************************************
* @purpose  Process IP admin mode events
*
* @param    mode        L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapAdminModeProcess(L7_uint32 mode)
{
  if (mode == L7_ENABLE)
  {
    (void) ipMapRtrAdminModeEnable();
  }
  else if (mode == L7_DISABLE)
  {
    (void) ipMapRtrAdminModeDisable();
  }
}

/*********************************************************************
* @purpose  Process the configuration of an IP address on an interface.
*
* @param    intIfNum    Internal Interface Number
* @param    newIpAddr   IP Address
* @param    newIpMask   IP Mask
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfIpAddressAddProcess(L7_uint32 intIfNum,
                                     L7_uint32 newIpAddr, L7_uint32 newIpMask)
{
  L7_RC_t rc = ipMapIntfUpdate(intIfNum, NULL);
  return rc;
}

/*********************************************************************
* @purpose  Take actions required when the user deletes the primary IP address
*           from an interface.
*
* @param    intIfNum    Internal Interface Number
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfIpAddressRemoveProcess(L7_uint32 intIfNum)
{
  return ipMapRtrIntfIpAddressRemoveApply(intIfNum);
}

/*********************************************************************
* @purpose  Process an asynchronous event completion
*
* @param    pAsyncEvent pointer to an ipAsyncNotifyData_t structure
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapAsyncEventCompleteProcess(ipAsyncNotifyData_t *pAsyncEvent)
{
    L7_uint32  intIfNum;
    L7_uint32 intf;
    L7_RC_t irc;
    ipMapIntfChangeParms_t *change_parms;
    NIM_EVENT_COMPLETE_INFO_t status;

    intIfNum =  pAsyncEvent->type.rtrInfo.intIfNum;

    switch (pAsyncEvent->type.rtrInfo.event)
    {
    case (L7_RTR_DISABLE_PENDING):
      ipMapRoutingEventChangeNotify(intIfNum, L7_RTR_DISABLE, L7_TRUE, L7_NULL);
      pIpMapInfo->operRtr.lastNotify = L7_RTR_DISABLE;

      /* Disable routing on all interfaces */
      for( irc = nimFirstValidIntfNumber(&intIfNum); irc == L7_SUCCESS;
           irc = nimNextValidIntfNumber(intIfNum, &intIfNum))
      {
        if(pIpMapInfo->operIntf[intIfNum].lastNotify == L7_RTR_INTF_DISABLE_PENDING)
        {
          pIpMapInfo->operIntf[intIfNum].lastNotify = L7_RTR_INTF_DISABLE;
          ipMapIntfUpdate(intIfNum, L7_NULL);
        }
      }
      break;

    case (L7_RTR_DISABLE):
      pIpMapInfo->operRtr.asyncPending = L7_FALSE;

      /* rtr disable implies disable of each interface. so if we get
         a nim intf down-type event during this time, nim response is deferred.
         look for deferred nim responses.
      */
      for( irc = nimFirstValidIntfNumber(&intIfNum); irc == L7_SUCCESS;
           irc = nimNextValidIntfNumber(intIfNum,&intIfNum))
      {
        if (pIpMapInfo->operIntf[intIfNum].nimParms.correlator != L7_NULL)
        {
          change_parms = &pIpMapInfo->operIntf[intIfNum].nimParms;
          status.intIfNum     = intIfNum;
          status.component    = L7_IP_MAP_COMPONENT_ID;
          status.response.rc  = L7_SUCCESS;
          status.response.reason  = 0;
          status.event        = change_parms->event;
          status.correlator   = change_parms->correlator;
          memset( change_parms, 0, sizeof(ipMapIntfChangeParms_t));
          nimEventStatusCallback(status);
        }
        /* Check if we need to enable as a Host Interface */
        if (ipMapMayEnableInterface (intIfNum) == IPMAP_RTR_INTF_ENABLE_HOST)
        {
          ipMapIntfUpdate(intIfNum, L7_NULL);
        }
      }

      /* may have been reenabled during async event period, check it */
      ipMapAdminModeProcess(ipMapCfg->rtr.rtrAdminMode);
        break;

      case (L7_RTR_INTF_DISABLE):
      pIpMapInfo->operIntf[intIfNum].asyncPending = L7_FALSE;

      /* respond to NIM if necessary */
      if (pIpMapInfo->operIntf[intIfNum].nimParms.correlator != L7_NULL)
      {
        change_parms = &pIpMapInfo->operIntf[intIfNum].nimParms;
        status.intIfNum     = intIfNum;
        status.component    = L7_IP_MAP_COMPONENT_ID;
        status.response.rc  = L7_SUCCESS;
        status.response.reason  = 0;
        status.event        = change_parms->event;
        status.correlator   = change_parms->correlator;
        memset(change_parms, 0, sizeof(ipMapIntfChangeParms_t));
        nimEventStatusCallback(status);
      }

      /* may have been reenabled during async event period, check */
      ipMapIntfUpdate(intIfNum, L7_NULL);
          break;

      case (L7_RTR_INTF_DISABLE_PENDING):
        ipMapRoutingEventChangeNotify(intIfNum, L7_RTR_INTF_DISABLE, L7_TRUE, L7_NULL);
        pIpMapInfo->operIntf[intIfNum].lastNotify = L7_RTR_INTF_DISABLE;
        ipMapIntfUpdate(intIfNum, L7_NULL);
          break;

      case (L7_RTR_INTF_CREATE):
        pIpMapInfo->operIntf[intIfNum].asyncPending = L7_FALSE;
        if (pIpMapInfo->operIntf[intIfNum].nimParms.correlator != L7_NULL)
        {
          /* IP MAP generated the RTR_INTF_CREATE event in response to
           * a NIM L7_CREATE event. NIM waiting for a response from IP MAP. */
          change_parms = &pIpMapInfo->operIntf[intIfNum].nimParms;
          status.intIfNum     = intIfNum;
          status.component    = L7_IP_MAP_COMPONENT_ID;
          status.response.rc  = L7_SUCCESS;
          status.response.reason  = 0;
          status.event        = change_parms->event;
          status.correlator   = change_parms->correlator;
          memset( change_parms, 0, sizeof(ipMapIntfChangeParms_t));
          nimEventStatusCallback(status);
        }
        else if (pIpMapInfo->createStartupDone)
        {
          /* IP MAP sent RTR_INTF_CREATE during NIM create startup. */
          /* See if all interface creates are finished */
          for (irc = nimFirstValidIntfNumber(&intf); irc == L7_SUCCESS;
           irc = nimNextValidIntfNumber(intf, &intf))
          {
            if ((pIpMapInfo->operIntf[intf].lastNotify == L7_RTR_INTF_CREATE) &&
                pIpMapInfo->operIntf[intf].asyncPending)
            {
              /* at least one create event is still pending */
              break;
            }
          }
          if (irc != L7_SUCCESS)
          {
            /* No interface creates still pending */
            if (ipMapTraceFlags & IPMAP_TRACE_NSF)
            {
              L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
              osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX,
                            "IP MAP completed NIM create startup");
              ipMapTraceWrite(traceBuf);
            }
            nimStartupEventDone(L7_IP_MAP_COMPONENT_ID);   /* respond to NIM for interface creates */
          }
        }
        break;

      case (L7_RTR_INTF_DELETE):
        if (pIpMapInfo->operIntf[intIfNum].nimParms.correlator != L7_NULL)
        {
          change_parms = &pIpMapInfo->operIntf[intIfNum].nimParms;
          status.intIfNum     = intIfNum;
          status.component    = L7_IP_MAP_COMPONENT_ID;
          status.response.rc  = L7_SUCCESS;
          status.response.reason  = 0;
          status.event        = change_parms->event;
          status.correlator   = change_parms->correlator;
          memset( change_parms, 0, sizeof(ipMapIntfChangeParms_t));
          nimEventStatusCallback(status);
        }
          break;

      default:
          break;
    }

    return;
}

/*********************************************************************
* @purpose  Process an RTO best route change on the IP MAP thread.
*
* @param    routeEntry  @b{(input)}  Route that was added/deleted/modified
* @param    route_status  @b{(input)}  one of RTO_ROUTE_EVENT_t
*
* @returns  L7_SUCCESS
*
* @notes    If the route includes a next hop IP address that has not yet
*           been resolved to a MAC address, IP MAP will ask ARP to send
*           an ARP request to resolve the next hop. But if the route contains
*           one or more
*           unresolved next hops, IP MAP will still send all next hops to
*           the driver. These will be marked with an unresolved flag. If
*           the next hop becomes resolved later, the driver should change
*           the status of the next hop from unresolved to resolved when
*           the ARP entry is added.
*
* @end
*********************************************************************/
L7_RC_t ipMapRtoRouteChangeProcess(L7_routeEntry_t *routeEntry,
                                   RTO_ROUTE_EVENT_t route_status)
{
  /* Entry in ARP table for next hop IP address */
  L7_arpEntry_t arpEntry;

  /* ARP entry structure within routeEntry specifying next hop */
  L7_arpEntry_t *nhArpEntry;

  L7_char8 gwmac[L7_ENET_MAC_ADDR_LEN];
  L7_uint32 i;

  /* Update the IP stack's routing table. */
  l3BestRouteReport(routeEntry, route_status, NULL);

  /* Is this the default route? */
  if (routeEntry->ipAddr == 0)
    routeEntry->flags |= L7_RTF_DEFAULT_ROUTE;

  /* Is this a local route? */
  if (routeEntry->protocol == RTO_LOCAL)
    routeEntry->flags |= L7_RTF_DIRECT_ATTACH;

  /* If this is a route delete event we only need to provide the destination
     network to be deleted.  */
  if (route_status == RTO_DELETE_ROUTE)
  {
    ipMapRouteEntryDelete(routeEntry);
    return L7_SUCCESS;
  }

  /* For add or modify, check whether each next hop is resolved to MAC. */
  for (i = 0; i < routeEntry->ecmpRoutes.numOfRoutes; i++)
  {
    routeEntry->ecmpRoutes.equalCostPath[i].flags |= L7_RT_EQ_PATH_VALID;
      nhArpEntry = &routeEntry->ecmpRoutes.equalCostPath[i].arpEntry;
    bzero((char *)gwmac, L7_ENET_MAC_ADDR_LEN);
    memcpy(nhArpEntry->macAddr.addr.enetAddr.addr, gwmac, L7_ENET_MAC_ADDR_LEN);
    nhArpEntry->flags &= ~L7_ARP_RESOLVED;
    nhArpEntry->macAddr.type = L7_LL_ETHERNET;
    nhArpEntry->macAddr.len = L7_ENET_MAC_ADDR_LEN;
    nhArpEntry->vlanId = 0;
    nhArpEntry->flags |= L7_ARP_GATEWAY;

    if (routeEntry->protocol == RTO_LOCAL)
    {
      nhArpEntry->flags |= L7_ARP_RESOLVED;
    }
    else
    {
      if (ipMapArpEntryByAddrGet((L7_IP_ADDR_t)nhArpEntry->ipAddr,
                                 nhArpEntry->intIfNum, &arpEntry) == L7_SUCCESS)
      {
        memcpy(nhArpEntry->macAddr.addr.enetAddr.addr,
               arpEntry.macAddr.addr.enetAddr.addr, L7_ENET_MAC_ADDR_LEN);
        nhArpEntry->flags |= L7_ARP_RESOLVED;
      }
      else
      {
        /* No need for callback when resolution completes. */
        if (ipMapArpAddrResolve(nhArpEntry->intIfNum, nhArpEntry->ipAddr,
                                gwmac, L7_NULLPTR, 0, 0) == L7_SUCCESS)
        {
          memcpy(nhArpEntry->macAddr.addr.enetAddr.addr,
                   gwmac, L7_ENET_MAC_ADDR_LEN);
          nhArpEntry->flags |= L7_ARP_RESOLVED;
        }
      }
    }   /* end not local route */
  } /* end for each next hop */

  if (route_status == RTO_ADD_ROUTE)
    ipMapRouteEntryAdd(routeEntry);
  else if (route_status == RTO_CHANGE_ROUTE)
    ipMapRouteEntryModify(routeEntry);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  RTO calls this function when a best route is added/deleted/changed.
*           IP MAP reports the route to the driver. Also handles update
*           of ARP gateway table and update of IP stack routing table.
*
* @param    routeEntry  @b{(input)}  Route that was added/deleted/modified
* @param    route_status  @b{(input)}  one of RTO_ROUTE_EVENT_t
* @param    passthrough  @b{(input)}  unused
*
* @returns  void
*
* @notes    If the route includes a next hop IP address that has not yet
*           been resolved to a MAC address, IP MAP will ask ARP to send
*           an ARP request to resolve the next hop. IP MAP will hold off
*           reporting the route to the driver until the resolution attempt
*           has completed. But if the route still contains one or more
*           unresolved next hops, IP MAP will still send all next hops to
*           the driver. These will be marked with an unresolved flag. If
*           the next hop becomes resolved later, the driver should change
*           the status of the next hop from unresolved to resolved when
*           the ARP entry is added.
*
* @end
*********************************************************************/
void ipMapRouteCallback()
{
  if (ipMapTraceFlags & IPMAP_TRACE_ROUTE_UPDATE)
  {
    ipMapTraceWrite("Best routes have changed.");
  }

  if (!pIpMapInfo->bestRouteTimer)
    /* schedule an event to request changes from RTO */
    osapiTimerAdd((void*)ipMapBestRouteTimerCb, L7_NULL, L7_NULL,
                  L7_IPMAP_BEST_ROUTE_DELAY, &pIpMapInfo->bestRouteTimer);

  return;
}

/*********************************************************************
*
* @purpose Timer callback to trigger IP MAP to get best routes from RTO.
*
* @param void
*
* @returns void
*
* @comments  This callback tells IP MAP that best route changes are pending
*            in RTO. IP MAP has to go back to RTO and ask for the changes.
*
* @end
*
*********************************************************************/
void ipMapBestRouteTimerCb(void)
{
  pIpMapInfo->bestRouteTimer = NULL;
  ipMapBestRouteEventPost();
}

/*********************************************************************
*
* @purpose Trigger IP MAP to request best route changes from RTO.
*
* @param void
*
* @returns void
*
* @comments  Called either when timer expires to retrieve initial set
*            of changes after a notification from RTO or when more
*            changes are pending after getting a set of changes.
*
* @end
*
*********************************************************************/
static void ipMapBestRouteEventPost(void)
{
    /* Put event on IP MAP processing thread. */
    L7_RC_t rc;
    ipMapMsg_t msg;

    if (ipMapProcess_Queue == L7_NULLPTR)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                "ipMapProcess_Queue is NULL");
      return;
    }

    memset( (void *)&msg, 0, sizeof(ipMapMsg_t) );
    msg.msgId = IPMAP_RTO_EVENT;

    rc = osapiMessageSend(ipMapProcess_Queue, &msg, sizeof(ipMapMsg_t),
                          L7_NO_WAIT, L7_MSG_PRIORITY_NORM );
    if (rc != L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
                "osapiMessageSend() failed with rc = %u", rc);
    }

    return;
}

/*********************************************************************
*
* @purpose  Ask RTO for a set of best route changes. If more changes
*           remain, queue up another event to ask for more.
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
L7_RC_t ipMapBestRoutesGet(void)
{
  L7_uint32 i;
  L7_uint32 numChanges = 0;
  L7_BOOL moreChanges = L7_FALSE;
  rtoRouteChange_t *routeChange;
  static L7_uint32 numRoutes = 0;

  if (rtoRouteChangeRequest(ipMapRouteCallback, L7_IPMAP_MAX_BEST_ROUTE_CHANGES,
                            &numChanges, &moreChanges,
                            ipMapRouteChangeBuf) != L7_SUCCESS)
  {
    /* Don't schedule another try. */
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_IP_MAP_COMPONENT_ID,
            "Failed to retrieve best route changes from RTO.");
    return L7_FAILURE;
  }

  /* process each best route change */
  for (i = 0; i < numChanges; i++)
  {
    routeChange = &ipMapRouteChangeBuf[i];
    ipMapRtoRouteChangeProcess(&routeChange->routeEntry, routeChange->changeType);
  }

  numRoutes += numChanges;

  if (moreChanges)
  {
    /* post another event to the IP MAP message queue to request more
     * changes from RTO */
    ipMapBestRouteEventPost();
  }
  else
  {
    if (ipMapTraceFlags & IPMAP_TRACE_ROUTE_UPDATE)
    {
      L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
      osapiSnprintf(traceBuf, IPMAP_TRACE_LEN_MAX,
                    "Completed update of %u routes.", numRoutes);
      ipMapTraceWrite(traceBuf);
    }
    numRoutes = 0;

    /* if we haven't yet told Configurator that our initial route updates
     * are complete, do so now.  */
    if (!pIpMapInfo->l3HwUpdateComplete)
    {
      if (ipMapTraceFlags & IPMAP_TRACE_NSF)
      {
        ipMapTraceWrite("IP MAP reporting completion of L3 hw updates.");
      }
      cnfgrApiComponentHwUpdateDone(L7_IP_MAP_COMPONENT_ID, L7_CNFGR_HW_APPLY_L3);
      pIpMapInfo->l3HwUpdateComplete = L7_TRUE;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initialize Buffer pool for the IP mapping component.
*
* @param    None.
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t ipMapBufferPoolCreate()
{
  /* IP Buffer pool variables */

  if ( bufferPoolInit ( L7_IPMAP_MAX_ASYNC_EVENTS, sizeof(ipAsyncNotifyData_t),
                          "IP Map Layered Async Notify",  &ipMapAsyncNotifyPoolId) != L7_SUCCESS)
  {
      IPMAP_ERROR("Error allocating memory for IP Async Notify buffer pool\n");
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Initialize Buffer pool for the IP mapping component.
*
* @param    None.
*
* @notes    None.
*
* @end
*********************************************************************/
void ipMapBufferPoolDelete()
{
  (void)bufferPoolTerminate(ipMapAsyncNotifyPoolId);
}


/*********************************************************************
* @purpose  Return IP async notif buffer pool ID
*
*
* @returns  buffer pool ID.
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 ipMapAsyncNotifyPoolIdGet (void)
{
  return ipMapAsyncNotifyPoolId;
}

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr to interface config structure
*
* @returns  L7_TRUE if pCfg is set
* @returns  L7_FALSE otherwise
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @end
*********************************************************************/
L7_BOOL ipMapMapIntfIsConfigurable(L7_uint32 intIfNum, L7_rtrCfgCkt_t **pCfg)
{
  L7_uint32 index;
  nimConfigID_t configId;

  if (!(IPMAP_IS_READY))
    return L7_FALSE;

  /* Check boundary conditions */
  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
    return L7_FALSE;

  index = intIfToCfgIndex[intIfNum];

  if (index == 0)
    return L7_FALSE;

  /* verify that the configId in the config data table entry matches the configId that NIM maps to
   ** the intIfNum we are considering
   */
  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId, &(ipMapCfg->ckt[index].configId)) == L7_FALSE)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      /* if we get here, either we have a table management error between ipMapCfg and ipMapMapTbl or
      ** there is synchronization issue between NIM and components w.r.t. interface creation/deletion
      */
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
              "Error accessing IPMAP config data for interface %s in ipMapMapIntfIsConfigurable.\n", ifName);
      return L7_FALSE;
    }
  }

  *pCfg = &ipMapCfg->ckt[index];

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr to interface config structure
*
* @returns  L7_TRUE if there is an available config entry and pCfg is set
* @returns  L7_FALSE otherwise
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @end
*********************************************************************/
L7_BOOL ipMapMapIntfConfigEntryGet(L7_uint32 intIfNum, L7_rtrCfgCkt_t **pCfg)
{
  L7_uint32 i;
  nimConfigID_t configIdNull;

  /* Avoid N^2 processing when interfaces created at startup */
  static L7_uint32 nextIndex = 1;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  if (!(IPMAP_IS_READY))
    return L7_FALSE;

  if (nextIndex < L7_IPMAP_INTF_MAX_COUNT)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&ipMapCfg->ckt[nextIndex].configId, &configIdNull))
    {
      intIfToCfgIndex[intIfNum] = nextIndex;
      *pCfg = &ipMapCfg->ckt[nextIndex];
      nextIndex++;
      return L7_TRUE;
    }
  }

  /* cached nextIndex is in use. Resort to search from beginning. */
    for (i = 1; i < L7_IPMAP_INTF_MAX_COUNT; i++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&ipMapCfg->ckt[i].configId, &configIdNull))
      {
        intIfToCfgIndex[intIfNum] = i;
        *pCfg = &ipMapCfg->ckt[i];
      nextIndex = i + 1;
        return L7_TRUE;
      }
    }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Process secondary IP address add request
*
* @param    intIfNum  Internal Interface Number
* @param    ipAddr    Secondary IP Address to be added
* @param    ipMask    Subnet mask associated with the IP Address
* @param    index     Index position for the new secondary address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfSecondaryIpAddrAddProcess(L7_uint32 intIfNum, L7_uint32 ipAddr,
                                           L7_uint32 ipMask, L7_uint32 index)
{
  if (ipMapIntfIsUp(intIfNum))
  {
    /* Apply new address */
    if (ipMapRtrIntfSecondaryIpAddressApply(intIfNum, ipAddr, ipMask, index) == L7_SUCCESS)
    {
      /* Notify router event clients of new address. A non-asynchronous event. */
      ipMapRoutingEventChangeNotify(intIfNum, L7_RTR_INTF_SECONDARY_IP_ADDR_ADD,
                                    L7_FALSE, L7_NULLPTR);
    }
    else
    {
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process Secondary IP address remove request
*
* @param    intIfNum  Internal Interface Number
* @param    ipAddr    Secondary IP Address to be removed
* @param    ipMask    Subnet mask associated with the IP Address
* @param    index     Index to addrs[] array on interface
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfSecondaryIpAddrRemoveProcess(L7_uint32 intIfNum, L7_uint32 ipAddr,
                                                 L7_uint32 ipMask, L7_uint32 index)
{
  /* If the interface is down, then return successfully.  This configuration
   * change will be applied when the interface comes up. */

  if (!ipMapIntfIsUp(intIfNum))
  {
    return L7_SUCCESS;
  }

  return ipMapRtrIntfSecondaryIpAddressRemoveApply(intIfNum, ipAddr, ipMask, index);
}

void ipMapForwardQueueInfoShow(void)
{
    L7_uint32 i;
    L7_uint32 ForwardQLen;

    for (i = 0; i < L7_L3_FORWARD_QUEUE_COUNT; i++)
    {
        if (osapiMsgQueueGetNumMsgs(ipMapForwardQueue[i], &ForwardQLen) == L7_SUCCESS)
        {
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                          "\tIP forward queue[%d] current count:    %d\n",
                          i, ForwardQLen);
        }

        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,
                      "\tIP forward queue[%d] high water mark:  %d (limit: %d)\n",
                      i, ipMapForwardQueueHWMark[i], L7_MAX_NETWORK_BUFF_PER_BOX);
    }
}

#ifdef L7_QOS_FLEX_PACKAGE_ACL
/*********************************************************************
* @purpose  Saves IPMAP debug configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ipMapDebugCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t ipMapDebugSave(void)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Copy the operational states into the config file */
  ipMapDebugCfgUpdate();

  if (ipMapDebugHasDataChanged() == L7_TRUE)
  {
    ipMapDebugCfg.hdr.dataChanged = L7_FALSE;
    ipMapDebugCfg.checkSum = nvStoreCrc32((L7_uchar8 *)&ipMapDebugCfg,
        (L7_uint32)(sizeof(ipMapDebugCfg) - sizeof(ipMapDebugCfg.checkSum)));
    /* call save NVStore routine */
    if ((rc = sysapiSupportCfgFileWrite(L7_IP_MAP_COMPONENT_ID, IPMAP_DEBUG_CFG_FILENAME ,
            (L7_char8 *)&ipMapDebugCfg, (L7_uint32)sizeof(ipMapDebugCfg_t))) == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
              "Error on call to osapiFsWrite routine on config file %s\n",IPMAP_DEBUG_CFG_FILENAME);
    }
  }

  return(rc);
}

/*********************************************************************
 * @purpose  Restores ip debug configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    ipMapDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t ipMapDebugRestore(void)
{
  L7_RC_t rc;

  ipMapDebugBuildDefaultConfigData(IPMAP_DEBUG_CFG_VER_CURRENT);

    ipMapDebugCfg.hdr.dataChanged = L7_TRUE;

      rc = ipMapRestoreDebugConfigData();

        return rc;
}
/*********************************************************************
* @purpose  Checks if ip debug config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL ipMapDebugHasDataChanged(void)
{
  return ipMapDebugCfg.hdr.dataChanged;
}

/*********************************************************************
* @purpose  Build default ipMap debug config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ipMapDebugBuildDefaultConfigData(L7_uint32 ver)
{

  /* setup file header */
  ipMapDebugCfg.hdr.version = ver;
  ipMapDebugCfg.hdr.componentID = L7_IP_MAP_COMPONENT_ID;
  ipMapDebugCfg.hdr.type = L7_CFG_DATA;
  ipMapDebugCfg.hdr.length = (L7_uint32)sizeof(ipMapDebugCfg);
  strcpy((L7_char8 *)ipMapDebugCfg.hdr.filename, IPMAP_DEBUG_CFG_FILENAME);
  ipMapDebugCfg.hdr.dataChanged = L7_FALSE;

  /* set all flags to L7_FALSE */
 memset(&ipMapDebugCfg.cfg, 0, sizeof(ipMapDebugCfg.cfg));
}

/*********************************************************************
* @purpose  Apply ip debug config data
*
* @param    void
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    Called after a default config is built
*
* @end
*********************************************************************/
L7_RC_t ipMapApplyDebugConfigData(void)
{
  L7_RC_t rc;
  L7_uint32 i=0;

  for (i = 0; i < L7_MAX_ACL_ID; i++)
  {
    if (ipMapDebugCfg.cfg.aclNumTrace[i] != 0)
    {
     rc = ipMapDebugPacketTraceFlagSet(ipMapDebugCfg.cfg.aclNumTrace[i], L7_TRUE);
    }
    else
    {
     rc = ipMapDebugPacketTraceFlagSet(ipMapDebugCfg.cfg.aclNumTrace[i], L7_FALSE);
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Apply ip debug config data
*
* @param    void
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    Called after a default config is built
*
* @end
*********************************************************************/
L7_RC_t ipMapRestoreDebugConfigData(void)
{
  L7_RC_t rc;
  L7_uint32 i=0;

  for (i=L7_MIN_ACL_ID; i<=L7_MAX_ACL_ID; i++)
  {
     rc = ipMapDebugPacketTraceFlagSet(i, L7_FALSE);
  }
  return L7_SUCCESS;
}
#endif

/*********************************************************************
* @purpose  Removing Timer When Rate limit is disabled.
*
* @param    void
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
void ipMapIcmpRatelimitTimerStop(void)
{

  if (pIpMapInfo->icmpRateLimitTimer)
  {
    /* Free this timer when Ratelimit is Disabled. */
    osapiTimerFree(pIpMapInfo->icmpRateLimitTimer);
    pIpMapInfo->icmpRateLimitTimer = NULL;
  }

  return;
}

/*********************************************************************
* @purpose  Assigning tokens when the Timer expires.
*
* @param    void
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
void ipMapIcmpRatelimitTimerStart(void)
{
  L7_uint32 rateInterval;
  rateInterval = ipMapCfg->rtr.rtrICMPRatelimitInterval;

  if (rateInterval!= 0)
  {
     if (!pIpMapInfo->icmpRateLimitTimer)
     {
      /* schedule an event to fill the token bucket */
      osapiTimerAdd((void*)ipMapIcmpRatelimitTimerCb, L7_NULL, L7_NULL,
                  rateInterval, &pIpMapInfo->icmpRateLimitTimer);
     }
  }

  return;
}

/*********************************************************************
* @purpose  Assigning tokens when the Timer expires.
*
* @param    void
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL isRateLimitTimerNull( void)
{

  if (!pIpMapInfo->icmpRateLimitTimer)
  {
     return L7_TRUE;
  }
  else
  {
     return L7_FALSE;
  }

}


/*********************************************************************
*
* @purpose Timer callback for ICMP RateLimit
*
* @param void
*
* @returns void
*
*
* @end
*
*********************************************************************/
void ipMapIcmpRatelimitTimerCb(void)
{
  L7_uint32 rateInterval, burst_size;
  pIpMapInfo->icmpRateLimitTimer = NULL;

  burst_size = ipMapCfg->rtr.rtrICMPRatelimitBurstSize;
  rateInterval = ipMapCfg->rtr.rtrICMPRatelimitInterval;

   if (rateInterval!= 0)
  {
    pIpMapInfo->send_count = burst_size;
  }
  if (rateInterval == 0)
  {
    pIpMapInfo->send_count = ~0u;
  }
}

/*********************************************************************
*
* @purpose Get ICMP RateLimit Send Count
*
* @param void
*
* @returns void
*
*
* @end
*
*********************************************************************/
L7_uint32 ipMapIcmpRatelimitSendCountGet(void)
{
   return pIpMapInfo->send_count;
}

/*********************************************************************
*
* @purpose Set ICMP RateLimit Send Count
*
* @param void
*
* @returns void
*
*
* @end
*
*********************************************************************/
void ipMapIcmpRatelimitSendCountDec(void)
{
  L7_uint32 rateInterval, burst_size;

  if ((ipMapRtrICMPRatelimitGet (&burst_size, &rateInterval) == L7_SUCCESS) &&
      (rateInterval!= 0))
  {
    if (pIpMapInfo->send_count != 0)
    {
      pIpMapInfo->send_count = pIpMapInfo->send_count-1;
      return;
    }
  }
  return;
}

void ipMapDebugIcmpRatelimitSendCount(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_IPMAP,"Send Count %d", pIpMapInfo->send_count);
}

/*********************************************************************
* @purpose  Registers for IP address conflict events
*
* @param    cbFunc  @b{(input)} Callback function
*
* @returns  L7_SUCCESS  Was able to successfully register the function
* @returns  L7_FAILURE  Too many registered functions
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapAddrConflictRegister(ipAddrConflictCB_t cbFunc)
{
  L7_uint32 i;
  L7_RC_t rc = L7_FAILURE;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  for (i = 0; i < IPMAP_IP_ADDR_CONFLICT_CB_MAX; i++)
  {
    if (pIpMapInfo->ipAddrConflictCallback[i] == L7_NULL)
    {
      pIpMapInfo->ipAddrConflictCallback[i] = cbFunc;
      rc = L7_SUCCESS;
      break;
    }
  }

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);

  return rc;
}

/*********************************************************************
* @purpose  Unregisters for IP address callback events
*
* @param    cbFunc   @b{(input)} Callback function
*
* @returns  L7_SUCCESS  If function was unregistered
* @returns  L7_FAILURE  If function was not unregistered
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapAddrConflictUnregister(ipAddrConflictCB_t cbFunc)
{
  L7_uint32 i;
  L7_RC_t rc = L7_FAILURE;

  if (ipMapLockTake(IPMAP_WRITE_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  for (i = 0; i < IPMAP_IP_ADDR_CONFLICT_CB_MAX; i++)
  {
    if (pIpMapInfo->ipAddrConflictCallback[i] == cbFunc)
    {
      pIpMapInfo->ipAddrConflictCallback[i] = L7_NULL;
      rc = L7_SUCCESS;
      break;
    }
  }

  ipMapLockGive(IPMAP_WRITE_LOCK, __FUNCTION__);

  return rc;
}

/*********************************************************************
* @purpose  Notify registrants of IP address conflict event
*
* @param    intIfNum  @{(input)} internal interface number
* @param    ipAddr    @{(input)} Conflicting IP on this interface
* @param    macAddr   @{(input)} Conflicting host's MAC address
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapAddrConflictNotify(L7_uint32 intIfNum,
                             L7_uint32 ipAddr,
                             L7_uchar8 *macAddr)
{
  L7_uint32 i;
  ipAddrConflictCB_t func;

  for (i = 0; i < IPMAP_IP_ADDR_CONFLICT_CB_MAX; i++)
  {
    func = pIpMapInfo->ipAddrConflictCallback[i];
    if (func != L7_NULL)
    {
      (*func)(intIfNum, ipAddr, macAddr);
    }
  }
}

/*********************************************************************
* @purpose  Apply address conflict detect mode
*
* @param    none
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapAddrConflictDetectApply()
{
  L7_uint32 rtrIfNum;
  L7_uint32 intIfNum;
  L7_int32 j;
  L7_rtrCfgCkt_t *pCfg;

    if (ipMapLockTake(IPMAP_READ_LOCK, IPMAP_LOCK_WAIT, __FUNCTION__) != L7_SUCCESS)
    {
      return;
    }

    /* Iterate through all active routing interfaces
     * and send gratuitous ARP packets on their addresses */
    for (rtrIfNum = 1; rtrIfNum <= L7_RTR_MAX_RTR_INTERFACES; rtrIfNum++)
    {
      if (ipMapCheckIfNumber(rtrIfNum) == L7_SUCCESS)
      {
        intIfNum = rtrIntfMap[rtrIfNum].intIfNum;
        if ((ipMapIntfIsUp(intIfNum)) &&
            (ipMapMapIntfIsConfigurable(intIfNum, &pCfg)))
        {
          /* The reason to iterate from top index to zero is that 
           * they are stacked up in the timer library in that order
           * and thereby are fired in reverse order to send the grat arp packets */
          for (j = (L7_L3_NUM_IP_ADDRS-1); j >= 0; j--)
          {
            if (pCfg->addrs[j].ipAddr)
            {
              ipMapGratArpSend(intIfNum, pCfg->addrs[j].ipAddr);
            }
          }
        }
      }
    }
    ipMapLockGive(IPMAP_READ_LOCK, __FUNCTION__);

    /* Send gratuitous ARPs for VR addresses in the VR groups
     * where the VR is master */
    if(vrrpFuncTable.L7_vrrpMasterGratARPSend)
    {
      vrrpFuncTable.L7_vrrpMasterGratARPSend();
    }
}

/*********************************************************************
* @purpose  Reset the statistics of the address conflicts detected
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapAddrConflictStatsReset()
{
  ipMapArpExtenAddrConflictStatsReset();
}
