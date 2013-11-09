/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
********************************************************************************
*
* @filename  broad_l3.c
*
* @purpose   This file contains external HAPI interface for configuring Layer3.
*            All the DAPI command handlers, Init functions, L2 callbacks are
*            defined.
*
* @component HAPI
*
* @comments  XGS2 devices are not supported.
*
* @create    10/1/2007
*
* @author    sdoke
*
* @end
*
*******************************************************************************/
#define L7_MAC_ENET_VRRP

#include "bcm/l2.h"
#include "bcmx/port.h"
#include "bcmx/vlan.h"
#include "bcmx/switch.h"

#include "l7_common.h"
#include "l7_packet.h"
#include "osapi.h"
#include "sysbrds.h"
#include "broad_l3_int.h"
#include "broad_l2_vlan.h"
#include "broad_l2_std.h"
#include "l7_usl_bcmx_l2.h"
#include "l7_usl_bcmx_l3.h"
#include "l7_usl_bcmx_port.h"

#include "dot1s_exports.h"
#include "ibde.h"


#ifdef L7_WIRELESS_PACKAGE
#include "broad_l2_tunnel.h"
#endif
/* AVL trees */

/* Next hop AVL tree */
avlTree_t hapiBroadL3NhopTree;
static avlTreeTables_t *hapiBroadL3NhopTreeHeap;
static BROAD_L3_NH_ENTRY_t *hapiBroadL3NhopDataHeap;

/* ECMP AVL tree */
avlTree_t hapiBroadL3EcmpTree;
static avlTreeTables_t *hapiBroadL3EcmpTreeHeap;
static BROAD_L3_ECMP_ENTRY_t *hapiBroadL3EcmpDataHeap;

/* Host AVL tree */
avlTree_t hapiBroadL3HostTree;
static avlTreeTables_t *hapiBroadL3HostTreeHeap;
static BROAD_L3_HOST_ENTRY_t *hapiBroadL3HostDataHeap;

/* MAC AVL tree */
avlTree_t hapiBroadL3MacTree;
static avlTreeTables_t *hapiBroadL3MacTreeHeap;
static BROAD_L3_MAC_KEY_t *hapiBroadL3MacDataHeap;

/* Route AVL tree */
avlTree_t hapiBroadL3RouteTree;
static avlTreeTables_t *hapiBroadL3RouteTreeHeap;
static BROAD_L3_ROUTE_ENTRY_t *hapiBroadL3RouteDataHeap;

/* Tunnel AVL tree - for all types of tunnels (4o4, 6o4, 6to4) */
avlTree_t hapiBroadL3TunnelTree;
static avlTreeTables_t *hapiBroadL3TunnelTreeHeap;
static BROAD_L3_TUNNEL_ENTRY_t *hapiBroadL3TunnelDataHeap;

/* Work list head and tail*/
BROAD_L3_NH_ENTRY_t *hapiBroadL3NhopListHead = L7_NULLPTR;
BROAD_L3_NH_ENTRY_t *hapiBroadL3NhopListTail = L7_NULLPTR;

BROAD_L3_ECMP_ENTRY_t *hapiBroadL3EcmpListHead = L7_NULLPTR;
BROAD_L3_ECMP_ENTRY_t *hapiBroadL3EcmpListTail = L7_NULLPTR;

BROAD_L3_HOST_ENTRY_t *hapiBroadL3HostListHead = L7_NULLPTR;
BROAD_L3_HOST_ENTRY_t *hapiBroadL3HostListTail = L7_NULLPTR;

BROAD_L3_ROUTE_ENTRY_t *hapiBroadL3RouteListHead = L7_NULLPTR;
BROAD_L3_ROUTE_ENTRY_t *hapiBroadL3RouteListTail = L7_NULLPTR;

BROAD_L3_TUNNEL_ENTRY_t *hapiBroadL3TunnelListHead = L7_NULLPTR;
BROAD_L3_TUNNEL_ENTRY_t *hapiBroadL3TunnelListTail = L7_NULLPTR;


/* Semaphore/locks */
void *hapiBroadL3Sema;    /* For all AVL trees/worklists */

/* Wake up signal for Async task */
void *hapiBroadL3WakeUpQueue;

/* Queue to wait for ASYNC task to finish up work list processing */
void *hapiBroadL3WlistWaitQueue;

/* Timer to retry failed L3 entries */
osapiTimerDescr_t *pBroadL3RetryTimer = L7_NULL;

/* Flag to indicate router card is inserted and init'ed */
volatile L7_BOOL hapiBroadL3RouterCardInited = L7_FALSE;

/* CPU target interface definitions */
bcm_mac_t hapiBroadL3CpuMac;
bcm_if_t  hapiBroadL3CpuIntfId = HAPI_BROAD_INVALID_L3_INTF_ID;
bcm_if_t  hapiBroadL3CpuEgrNhopId = HAPI_BROAD_INVALID_L3_INTF_ID;

L7_BOOL hapiBroadL3ProcessZeroCountNhops = L7_FALSE;

/* 128-bit mask for IPv6 routes */
L7_uint32 hapiBroadL3Ipv6Mask128[] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                                      0xFFFFFFFF};
/* 48-bit mask for 6to4 addresses of automatic tunnels */
L7_uint32 hapiBroadL3Ipv6Mask6to4[] = {0xFFFFFFFF, 0xFFFF0000, 0x0, 0x0};

/* Mask of the vlans that are enabled for routing */
L7_VLAN_MASK_t routedVlanMask;

/* DAPI Command handler prototypes */
typedef L7_RC_t BROAD_L3_DAPI_CMD_HANDLER_t (DAPI_USP_t *usp, DAPI_CMD_t cmd,
                                             void *data, DAPI_t *dapi_g);

static BROAD_L3_DAPI_CMD_HANDLER_t hapiBroadL3IntfCreate;
static BROAD_L3_DAPI_CMD_HANDLER_t hapiBroadL3IntfModify;
static BROAD_L3_DAPI_CMD_HANDLER_t hapiBroadL3IntfDelete;
static BROAD_L3_DAPI_CMD_HANDLER_t hapiBroadL3IntfIpv4AddrSet;
static BROAD_L3_DAPI_CMD_HANDLER_t hapiBroadL3IntfAddrAdd;
static BROAD_L3_DAPI_CMD_HANDLER_t hapiBroadL3IntfAddrDelete;
static BROAD_L3_DAPI_CMD_HANDLER_t hapiBroadL3RouteEntryModify;
static BROAD_L3_DAPI_CMD_HANDLER_t hapiBroadL3HostEntryModify;
static BROAD_L3_DAPI_CMD_HANDLER_t hapiBroadL3HostEntryQuery;
static BROAD_L3_DAPI_CMD_HANDLER_t hapiBroadL3TunnelCreate;
static BROAD_L3_DAPI_CMD_HANDLER_t hapiBroadL3TunnelDelete;
static BROAD_L3_DAPI_CMD_HANDLER_t hapiBroadL3TunnelMacAddressAdd;
static BROAD_L3_DAPI_CMD_HANDLER_t hapiBroadL3TunnelMacAddressDelete;
static BROAD_L3_DAPI_CMD_HANDLER_t hapiBroadL3TunnelNextHopSet;
static BROAD_L3_DAPI_CMD_HANDLER_t hapiBroadL3IntfVRIDAdd;
static BROAD_L3_DAPI_CMD_HANDLER_t hapiBroadL3IntfVRIDDelete;
static BROAD_L3_DAPI_CMD_HANDLER_t hapiBroadL3NoOp;
static BROAD_L3_DAPI_CMD_HANDLER_t hapiBroadL3IntfStatsGet;

static L7_RC_t hapiBroadL3RouteNhopProcess(BROAD_L3_ROUTE_ENTRY_t  *pRouteEntry,
                                           DAPI_ROUTING_ROUTE_ENTRY_t *pRtNhop,
                                           DAPI_t *dapi_g);

static BROAD_L3_NH_ENTRY_t *hapiBroadL3NhopEntryAdd(BROAD_L3_NH_KEY_t *pNhopKey,
                                                    DAPI_t *dapi_g);

static BROAD_L3_MAC_ENTRY_t * hapiBroadL3MacEntryAdd(DAPI_USP_t *usp,
                                                     L7_enetMacAddr_t *pMacAddr,
                                                     DAPI_t *dapi_g);

static L7_RC_t hapiBroadL3MacNhopLink (BROAD_L3_NH_ENTRY_t *pNhopEntry,
                                       BROAD_L3_MAC_ENTRY_t  *pMacEntry);

static L7_RC_t hapiBroadL3MacEntryResolve (BROAD_L3_MAC_ENTRY_t  *pMac,
                                           DAPI_t *dapi_g);

static BROAD_L3_ECMP_ENTRY_t * hapiBroadL3EcmpEntryAdd(BROAD_L3_NH_ENTRY_t **pNhops,
                                                       L7_uint8 numNextHops);

static void hapiBroadL3EcmpResolve(BROAD_L3_NH_ENTRY_t *pNhop);

static void hapiBroadL3TunnelResolve(BROAD_L3_NH_ENTRY_t *pNhop);

static L7_RC_t hapiBroadL3TunnelNhopProcess(DAPI_USP_t *usp,
                                            BROAD_L3_TUNNEL_ENTRY_t *pTunEntry,
                                            DAPI_t *dapi_g);

static BROAD_L3_NH_ENTRY_t *hapiBroadL3TunnelNhopGet(BROAD_L3_NH_KEY_t *pNhopKey,
                                                     DAPI_t *dapi_g);

static void hapiBroadL3Tunnel6to4RelayUpdate (DAPI_USP_t *tnlUsp,
                                              BROAD_L3_ROUTE_ENTRY_t *pRouteEntry,
                                              BROAD_L3_NH_ENTRY_t * pV6Nhop,
                                              DAPI_t * dapi_g);

static L7_RC_t hapiBroadL3TunnelHostMacLink (BROAD_L3_HOST_ENTRY_t *pHostEntry,
                                               BROAD_L3_MAC_ENTRY_t  *pMacEntry);

static L7_RC_t hapiBroadL3TunnelHostMacUnlink (BROAD_L3_HOST_ENTRY_t *pHostEntry,
                                               BROAD_L3_MAC_ENTRY_t  *pMacEntry);

static L7_RC_t hapiBroadL3Clear(void);

L7_RC_t hapiBroadL3IcmpRedirVlanConfig(L7_ushort16 vlanId, L7_int32 enable);

/*******************************************************************************
*
* @purpose Initializes L3 code
*
* @param   *dapi_g system information
*
* @returns L7_SUCCESS
*
* @notes   Any failure during initialization is treated as fatal error.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3Init(DAPI_t *dapi_g)
{
  /* Allocate memory for the AVL trees maintained - nh, host, route, and mac. */
  hapiBroadL3NhopTreeHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                         HAPI_BROAD_L3_NEXT_HOP_TREE_HEAP_SIZE);
  hapiBroadL3NhopDataHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                         HAPI_BROAD_L3_NEXT_HOP_DATA_HEAP_SIZE);

  hapiBroadL3EcmpTreeHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                         HAPI_BROAD_L3_ECMP_TREE_HEAP_SIZE);
  hapiBroadL3EcmpDataHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                         HAPI_BROAD_L3_ECMP_DATA_HEAP_SIZE);

  hapiBroadL3HostTreeHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                        HAPI_BROAD_L3_HOST_TREE_HEAP_SIZE);
  hapiBroadL3HostDataHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                        HAPI_BROAD_L3_HOST_DATA_HEAP_SIZE);

  hapiBroadL3MacTreeHeap = osapiMalloc (L7_DRIVER_COMPONENT_ID,
                                        HAPI_BROAD_L3_MAC_TREE_HEAP_SIZE);
  hapiBroadL3MacDataHeap = osapiMalloc (L7_DRIVER_COMPONENT_ID,
                                        HAPI_BROAD_L3_MAC_DATA_HEAP_SIZE);

  hapiBroadL3RouteTreeHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                         HAPI_BROAD_L3_ROUTE_TREE_HEAP_SIZE);
  hapiBroadL3RouteDataHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                         HAPI_BROAD_L3_ROUTE_DATA_HEAP_SIZE);

  hapiBroadL3TunnelTreeHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                          HAPI_BROAD_L3_TUNNEL_TREE_HEAP_SIZE);
  hapiBroadL3TunnelDataHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                          HAPI_BROAD_L3_TUNNEL_DATA_HEAP_SIZE);

  /* Check for allocation failures. Current implementaion of osapiMalloc treats
   * alloc failures as fatal, throwing log error anyway.
   */

  if ((L7_NULLPTR == hapiBroadL3NhopTreeHeap) ||
      (L7_NULLPTR == hapiBroadL3NhopDataHeap) ||
      (L7_NULLPTR == hapiBroadL3EcmpTreeHeap) ||
      (L7_NULLPTR == hapiBroadL3EcmpDataHeap) ||
      (L7_NULLPTR == hapiBroadL3HostTreeHeap) ||
      (L7_NULLPTR == hapiBroadL3HostDataHeap) ||
      (L7_NULLPTR == hapiBroadL3MacTreeHeap) ||
      (L7_NULLPTR == hapiBroadL3MacDataHeap) ||
      (L7_NULLPTR == hapiBroadL3RouteTreeHeap) ||
      (L7_NULLPTR == hapiBroadL3RouteDataHeap) ||
      (L7_NULLPTR == hapiBroadL3TunnelTreeHeap) ||
      (L7_NULLPTR == hapiBroadL3TunnelDataHeap))
  {
    HAPI_BROAD_L3_LOG_ERROR(0);
  }

  /* Create a single lock for all the work lists and trees*/
  HAPI_BROAD_L3_SEMA_CREATE(hapiBroadL3Sema,
                            OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

  /* Debug semaphore for tracing */
  HAPI_BROAD_L3_SEMA_CREATE(broadL3DebugSema,
                            OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

  /* Create a queue to wake up ASYNC task */
  hapiBroadL3WakeUpQueue = (void *)osapiMsgQueueCreate("hapiL3WakeQ",
                                                        1,
                                                        sizeof(L7_uint32));

  if (hapiBroadL3WakeUpQueue == L7_NULL)
  {
    HAPI_BROAD_L3_LOG_ERROR(0);
  }

  /* Init the L3 cache */
  if (hapiBroadL3CacheInit() != L7_SUCCESS)
  {
    HAPI_BROAD_L3_LOG_ERROR(0);
  }

  /* Create a queue to wait for ASYNC task to finish up work list processing
   * Mainly used to synchronize card removal with async processing
   */

  hapiBroadL3WlistWaitQueue = (void *)osapiMsgQueueCreate("hapiL3WaitQ",
                                                          16,
                                                          sizeof(void *));

  /* Finally, create L3 Async task for processing the work lists */
  if (osapiTaskCreate("hapiL3AsyncTask", hapiBroadL3AsyncTask,
                      1, dapi_g, L7_DEFAULT_STACK_SIZE,
                      L7_DEFAULT_TASK_PRIORITY,
                      L7_DEFAULT_TASK_SLICE) == L7_ERROR)
  {
    HAPI_BROAD_L3_LOG_ERROR(0);
  }

  return L7_SUCCESS;
}


/*******************************************************************************
*
* @purpose Re-initialize the L3 code.
*
* @param   none
*
* @returns none
*
* @notes   Called from hpcHardwareDriverReset().
*
* @end
*
*******************************************************************************/
void hapiBroadL3ReInit(void)
{
  /* What to do here exactly */
}

/*******************************************************************************
*
* @function hapiBroadL3PortInit
*
* @purpose  Hook in DAPI L3 command handlers for the given port
*
* @param    DAPI_PORT_t* dapiPortPtr - generic port instance
*
* @returns  L7_RC_t
*
* @notes    none
*
* @end
*
*******************************************************************************/

L7_RC_t hapiBroadL3PortInit(DAPI_PORT_t *dapiPortPtr)
{
  L7_RC_t result = L7_SUCCESS;

  /* Global or system wide commands */
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_ROUTE_FORWARDING_CONFIG] =
    (HAPICTLFUNCPTR_t)hapiBroadL3RouteFwdConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_IPV6_ROUTE_FORWARDING_CONFIG] =
    (HAPICTLFUNCPTR_t)hapiBroadL3RouteFwdConfig;


  /* Interface commands */
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_ADD] =
    (HAPICTLFUNCPTR_t)hapiBroadL3IntfCreate;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_MODIFY] =
    (HAPICTLFUNCPTR_t)hapiBroadL3IntfModify;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_DELETE] =
    (HAPICTLFUNCPTR_t)hapiBroadL3IntfDelete;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_IPV4_ADDR_SET] =
    (HAPICTLFUNCPTR_t)hapiBroadL3IntfIpv4AddrSet;
  dapiPortPtr->cmdTable[DAPI_CMD_IPV6_INTF_ADDR_ADD] =
    (HAPICTLFUNCPTR_t)hapiBroadL3IntfAddrAdd;
  dapiPortPtr->cmdTable[DAPI_CMD_IPV6_INTF_ADDR_DELETE] =
    (HAPICTLFUNCPTR_t)hapiBroadL3IntfAddrDelete;

  /* Note: DAPI_CMD_ROUTING_INTF_ADDR_ADD and DAPI_CMD_ROUTING_INTF_ADDR_DELETE
   * are not being used by application. Hence removed.
   */

  /* Route commands */
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_ROUTE_ENTRY_ADD] =
    (HAPICTLFUNCPTR_t)hapiBroadL3RouteEntryAdd;
  dapiPortPtr->cmdTable[DAPI_CMD_IPV6_ROUTE_ENTRY_ADD] =
    (HAPICTLFUNCPTR_t)hapiBroadL3RouteEntryAdd;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_ROUTE_ENTRY_MODIFY] =
    (HAPICTLFUNCPTR_t)hapiBroadL3RouteEntryModify;
  dapiPortPtr->cmdTable[DAPI_CMD_IPV6_ROUTE_ENTRY_MODIFY] =
    (HAPICTLFUNCPTR_t)hapiBroadL3RouteEntryModify;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE] =
    (HAPICTLFUNCPTR_t)hapiBroadL3RouteEntryDelete;
  dapiPortPtr->cmdTable[DAPI_CMD_IPV6_ROUTE_ENTRY_DELETE] =
    (HAPICTLFUNCPTR_t)hapiBroadL3RouteEntryDelete;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE_ALL] =
    (HAPICTLFUNCPTR_t)hapiBroadL3RouteEntryDeleteAll;


  /* Host commands */
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_ARP_ENTRY_ADD] =
    (HAPICTLFUNCPTR_t)hapiBroadL3HostEntryAdd;
  dapiPortPtr->cmdTable[DAPI_CMD_IPV6_NEIGH_ENTRY_ADD] =
    (HAPICTLFUNCPTR_t)hapiBroadL3HostEntryAdd;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_ARP_ENTRY_MODIFY] =
    (HAPICTLFUNCPTR_t)hapiBroadL3HostEntryModify;
  dapiPortPtr->cmdTable[DAPI_CMD_IPV6_NEIGH_ENTRY_MODIFY] =
    (HAPICTLFUNCPTR_t)hapiBroadL3HostEntryModify;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_ARP_ENTRY_DELETE] =
    (HAPICTLFUNCPTR_t)hapiBroadL3HostEntryDelete;
  dapiPortPtr->cmdTable[DAPI_CMD_IPV6_NEIGH_ENTRY_DELETE] =
    (HAPICTLFUNCPTR_t)hapiBroadL3HostEntryDelete;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_ARP_ENTRY_DELETE_ALL] =
    (HAPICTLFUNCPTR_t)hapiBroadL3HostEntryDeleteAll;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_ARP_ENTRY_QUERY] =
    (HAPICTLFUNCPTR_t)hapiBroadL3HostEntryQuery;
  dapiPortPtr->cmdTable[DAPI_CMD_IPV6_NEIGH_ENTRY_QUERY] =
    (HAPICTLFUNCPTR_t)hapiBroadL3HostEntryQuery;


  /* L3 Tunneling commands */
  /* Note: These commands are for 4o4/Wireless tunnels only. For 6o4 tunnels
   * the DAPI commands are different. See hapiBroadL3TunnelCardInsert()
   * Ideally, there should be one set of DAPI Tunnel commands.
   */
  dapiPortPtr->cmdTable[DAPI_CMD_L3_TUNNEL_ADD] =
    (HAPICTLFUNCPTR_t)hapiBroadL3TunnelCreate;
  dapiPortPtr->cmdTable[DAPI_CMD_L3_TUNNEL_DELETE] =
    (HAPICTLFUNCPTR_t)hapiBroadL3TunnelDelete;
  dapiPortPtr->cmdTable[DAPI_CMD_L3_TUNNEL_MAC_ADDRESS_ADD] =
    (HAPICTLFUNCPTR_t)hapiBroadL3TunnelMacAddressAdd;
  dapiPortPtr->cmdTable[DAPI_CMD_L3_TUNNEL_MAC_ADDRESS_DELETE] =
    (HAPICTLFUNCPTR_t)hapiBroadL3TunnelMacAddressDelete;


  /* VRID commands */
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_VRID_ADD] =
    (HAPICTLFUNCPTR_t)hapiBroadL3IntfVRIDAdd;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_VRID_DELETE] =
    (HAPICTLFUNCPTR_t)hapiBroadL3IntfVRIDDelete;

  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_ROUTE_ICMP_REDIRECTS_CONFIG] =
    (HAPICTLFUNCPTR_t)hapiBroadL3IcmpRedirConfig;

  /* Misc commands with No-Op handler - legacy? */
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_LOCAL_MCASTADD] =
    (HAPICTLFUNCPTR_t)hapiBroadL3NoOp;
  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_INTF_LOCAL_MCASTDELETE] =
    (HAPICTLFUNCPTR_t)hapiBroadL3NoOp;

  dapiPortPtr->cmdTable[DAPI_CMD_ROUTING_ROUTE_BOOTP_DHCP_CONFIG] =
    (HAPICTLFUNCPTR_t)hapiBroadL3NoOp;


  /* Stats commands */
  dapiPortPtr->cmdTable[DAPI_CMD_IPV6_INTF_STATS_GET] =
    (HAPICTLFUNCPTR_t)hapiBroadL3IntfStatsGet;

  return result;
}


/*******************************************************************************
*
* @purpose Initialize routing for all ports on this card
*
* @param   L7_ushort16  unitNum - unit number for this card
* @param   L7_ushort16  slotNum - slot number for this card
* @param   DAPI_t *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   Called from hapiBroadPhysicalCardInsert() for physical cards and
*          from hapiBroadL3RouterCardInsert() for logical cards and from
*          hapiBroadCpuCardInsert() for CPU logical card.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3CardInit(L7_ushort16 unitNum,
                            L7_ushort16 slotNum,
                            DAPI_t *dapi_g)
{
  L7_int32                     rv;
  L7_RC_t                      result;
  DAPI_USP_t                   usp;
  DAPI_PORT_t                 *dapiPortPtr;
  BROAD_PORT_t                *hapiPortPtr;
  usl_bcm_l3_intf_t            cpuIntfInfo;
  usl_bcm_l3_egress_t          bcmEgrObj;
  L7_uint32                    egrFlags;

  usp.unit = unitNum;
  usp.slot = slotNum;

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3CardInit: Unit %d, slot %d\n",
                      unitNum, slotNum);

  for (usp.port=0;
       usp.port< dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot;
       usp.port++)
  {
    dapiPortPtr = DAPI_PORT_GET(&usp, dapi_g);
    hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);

    hapiPortPtr->l3_interface_id = HAPI_BROAD_INVALID_L3_INTF_ID;

    if ((L7_TRUE == IS_PORT_TYPE_PHYSICAL(dapiPortPtr)) ||
        (L7_TRUE == IS_PORT_TYPE_LOGICAL_VLAN(dapiPortPtr)))
    {
      /* Reset the L3 and VRRP interface IDs */
      hapiPortPtr->vrrp_interface_id =
        (L7_uint32 *)osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                 (L7_PLATFORM_ROUTING_VRRP_MAX_VRID + 1) *
                                 sizeof(L7_uint32));

      if (hapiPortPtr->vrrp_interface_id == L7_NULLPTR)
      {
        /* Treat alloc failure as fatal. osapiMalloc log errors on failure too */
        HAPI_BROAD_L3_LOG_ERROR(0);
      }
    }
    else if (L7_TRUE == IS_PORT_TYPE_CPU(dapiPortPtr))
    {
      if ((hapiBroadL3CpuIntfId == HAPI_BROAD_INVALID_L3_INTF_ID) &&
          (hapiBroadL3CpuEgrNhopId == HAPI_BROAD_INVALID_L3_INTF_ID))
      {
        /* First time around create a special interface where all CPU targeted
         * packets will go,
         */

        /* Get the router MAC address - strictly speaking this MAC addr doesn't
         * matter, as CPU interface is configured to L2 switch packets and not to
         * route them. So, no L3 modifications will be applied.
         */

        result = sysapiHpcIfaceMacGet(L7_LOGICAL_VLAN_INTF, L7_VLAN_SLOT_NUM, 0,
                                      L7_NULLPTR, (L7_uchar8 *)hapiBroadL3CpuMac);
        if (result != L7_SUCCESS)
        {
          HAPI_BROAD_L3_LOG_ERROR (result);
        }

        usl_bcm_l3_intf_t_init(&cpuIntfInfo);
        memcpy(cpuIntfInfo.bcm_data.l3a_mac_addr, hapiBroadL3CpuMac, sizeof(bcm_mac_t));
        cpuIntfInfo.bcm_data.l3a_vid = HPC_STACKING_VLAN_ID;
        cpuIntfInfo.bcm_data.l3a_mtu = L7_MAX_FRAME_SIZE-4;
        cpuIntfInfo.bcm_data.l3a_flags |= (BCM_L3_ADD_TO_ARL | BCM_L3_L2ONLY);

        rv = usl_bcmx_l3_intf_create(&cpuIntfInfo);
        HAPI_BROAD_L3_BCMX_DBG(rv, "hapiBroadL3CardInit: CPU intf created (%s)\n",
                               bcm_errmsg(rv));
        if (rv < 0)
        {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
                  "Failed to create Cpu L3 intf, error code %d"
                  "L3 intf params: vid %d mac %x:%x:%x:%x:%x:%x\n",
                  rv, cpuIntfInfo.bcm_data.l3a_vid,
                  hapiBroadL3CpuMac[0], hapiBroadL3CpuMac[1],
                  hapiBroadL3CpuMac[2], hapiBroadL3CpuMac[3],
                  hapiBroadL3CpuMac[4], hapiBroadL3CpuMac[5]);
          HAPI_BROAD_L3_LOG_ERROR (rv);
        }
        else
        {
          hapiBroadL3CpuIntfId = cpuIntfInfo.bcm_data.l3a_intf_id;
          hapiPortPtr->l3_interface_id = cpuIntfInfo.bcm_data.l3a_intf_id;
          /* Create a CPU next hop egress object
           * CPU next hop is only used for routes with no next hop information.
           * For example, reject routes, link locals etc.
           */
          usl_bcm_l3_egress_t_init(&bcmEgrObj);
          egrFlags = 0;

          /* We use reserved 127.0.0.1 address to represent
          ** cpu next-hop egress object
          */
          bcmEgrObj.key.addr.l3a_ip_addr = HAPI_BROAD_L3_CPU_EGR_NHOP_IP;

          bcmEgrObj.bcm_data.vlan = HPC_STACKING_VLAN_ID;

          if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
          {
            bcmEgrObj.bcm_data.flags |= BCM_L3_TGID;
            bcmEgrObj.bcm_data.trunk = hapiPortPtr->hapiModeparm.lag.tgid;
          }
          else
          {
            bcmEgrObj.bcm_data.module = BCM_GPORT_MODPORT_MODID_GET(hapiPortPtr->bcmx_lport);
            bcmEgrObj.bcm_data.port   = BCM_GPORT_MODPORT_PORT_GET(hapiPortPtr->bcmx_lport);
            if ((bcmEgrObj.bcm_data.module == HAPI_BROAD_INVALID_MODID) ||
                (bcmEgrObj.bcm_data.port   == HAPI_BROAD_INVALID_MODPORT))
            {
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
                      "Failed to get modid/port for lport %x\n",
                      hapiPortPtr->bcmx_lport);
              HAPI_BROAD_L3_LOG_ERROR (0);
            }
          }

          memcpy(bcmEgrObj.bcm_data.mac_addr, hapiBroadL3CpuMac, sizeof(bcm_mac_t));
          bcmEgrObj.bcm_data.intf = hapiBroadL3CpuIntfId;


          result = usl_bcmx_l3_egress_create(&egrFlags, &bcmEgrObj, 1,
                                             &hapiBroadL3CpuEgrNhopId, &rv);

          HAPI_BROAD_L3_BCMX_DBG(rv, "hapiBroadL3CardInit: CPU nhop created (%s)\n",
                                 bcm_errmsg(rv));
          if ((result == L7_FAILURE) || (rv != BCM_E_NONE))
          {
            HAPI_BROAD_L3_LOG_ERROR (rv);
          }
        }

        HAPI_BROAD_L3_DEBUG(broadL3Debug,
                          "hapiBroadL3CardInit - CPU intf_id = %d, egrId %d\n",
                          hapiBroadL3CpuIntfId, hapiBroadL3CpuEgrNhopId);
      }
    }
  }

  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Initializes logical router cards
*
* @param  dapiUsp         USP for the logical router card
* @param  cmd             DAPI cmd for inserting card (not used)
* @param  data            Data associated with the DAPI cmd (not used)
* @param *handle          DAPI driver object handle
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3RouterCardInsert(DAPI_USP_t *dapiUsp,
                                    DAPI_CMD_t cmd,
                                    void *data,
                                    void *handle)
{
  L7_RC_t             result = L7_SUCCESS;
  L7_ushort16         numOfPortsInSlot;
  DAPI_USP_t          usp;
  BROAD_PORT_t       *hapiPortPtr;
  DAPI_t             *dapi_g = (DAPI_t*)handle;

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3RouterCardInsert: Unit %d "
                      "Slot %d\n", dapiUsp->unit, dapiUsp->slot);

  usp.unit = dapiUsp->unit;
  usp.slot = dapiUsp->slot;

  numOfPortsInSlot = dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot;

  for (usp.port=0; usp.port < numOfPortsInSlot; usp.port++)
  {
    dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port]->hapiPort =
      (void *)osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(BROAD_PORT_t));

    hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);
    memset((L7_char8*)hapiPortPtr, 0, sizeof(BROAD_PORT_t));
  }

  if (hapiBroadSlotCtlInit(usp.unit, usp.slot, dapi_g) == L7_FAILURE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: In %s call to 'hapiBroadSlotCtlInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  if (hapiBroadL3CardInit(usp.unit, usp.slot, dapi_g) == L7_FAILURE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: In %s call to 'hapiBroadL3CardInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  /* Initialize the storage for all the AVL trees */
  memset (&hapiBroadL3NhopTree, 0, sizeof (hapiBroadL3NhopTree));
  memset (hapiBroadL3NhopTreeHeap, 0, HAPI_BROAD_L3_NEXT_HOP_TREE_HEAP_SIZE);
  memset (hapiBroadL3NhopDataHeap, 0, HAPI_BROAD_L3_NEXT_HOP_DATA_HEAP_SIZE);

  memset (&hapiBroadL3EcmpTree, 0, sizeof (hapiBroadL3EcmpTree));
  memset (hapiBroadL3EcmpTreeHeap, 0, HAPI_BROAD_L3_ECMP_TREE_HEAP_SIZE);
  memset (hapiBroadL3EcmpDataHeap, 0, HAPI_BROAD_L3_ECMP_DATA_HEAP_SIZE);

  memset (&hapiBroadL3HostTree, 0, sizeof (hapiBroadL3HostTree));
  memset (hapiBroadL3HostTreeHeap, 0, HAPI_BROAD_L3_HOST_TREE_HEAP_SIZE);
  memset (hapiBroadL3HostDataHeap, 0, HAPI_BROAD_L3_HOST_DATA_HEAP_SIZE);

  memset (&hapiBroadL3MacTree, 0, sizeof (hapiBroadL3MacTree));
  memset (hapiBroadL3MacTreeHeap, 0, HAPI_BROAD_L3_MAC_TREE_HEAP_SIZE);
  memset (hapiBroadL3MacDataHeap, 0, HAPI_BROAD_L3_MAC_DATA_HEAP_SIZE);

  memset (&hapiBroadL3RouteTree, 0, sizeof (hapiBroadL3RouteTree));
  memset (hapiBroadL3RouteTreeHeap, 0, HAPI_BROAD_L3_ROUTE_TREE_HEAP_SIZE);
  memset (hapiBroadL3RouteDataHeap, 0, HAPI_BROAD_L3_ROUTE_DATA_HEAP_SIZE);

  memset (&hapiBroadL3TunnelTree, 0, sizeof (hapiBroadL3TunnelTree));
  memset (hapiBroadL3TunnelTreeHeap, 0, HAPI_BROAD_L3_TUNNEL_TREE_HEAP_SIZE);
  memset (hapiBroadL3TunnelDataHeap, 0, HAPI_BROAD_L3_TUNNEL_DATA_HEAP_SIZE);

  /* Create the Next hop AVL tree. Note that the AVL create API
   * returns void.
   */
  avlCreateAvlTree(&hapiBroadL3NhopTree,
                   hapiBroadL3NhopTreeHeap,
                   hapiBroadL3NhopDataHeap,
                   HAPI_BROAD_L3_NH_TBL_SIZE,
                   sizeof(BROAD_L3_NH_ENTRY_t), 0,
                   sizeof(BROAD_L3_NH_KEY_t));

  /* Create ECMP AVL tree */
  avlCreateAvlTree(&hapiBroadL3EcmpTree,
                   hapiBroadL3EcmpTreeHeap,
                   hapiBroadL3EcmpDataHeap,
                   HAPI_BROAD_L3_MAX_ECMP_GROUPS,
                   sizeof(BROAD_L3_ECMP_ENTRY_t), 0,
                   sizeof(BROAD_L3_ECMP_KEY_t));

  /* Create the Host AVL tree */
  avlCreateAvlTree(&hapiBroadL3HostTree,
                   hapiBroadL3HostTreeHeap,
                   hapiBroadL3HostDataHeap,
                   HAPI_BROAD_L3_HOST_TBL_SIZE,
                   sizeof(BROAD_L3_HOST_ENTRY_t), 0,
                   sizeof(BROAD_L3_HOST_KEY_t));

  /* Create the Mac AVL tree */
  avlCreateAvlTree(&hapiBroadL3MacTree,
                   hapiBroadL3MacTreeHeap,
                   hapiBroadL3MacDataHeap,
                   HAPI_BROAD_L3_MAC_TBL_SIZE,
                   sizeof(BROAD_L3_MAC_ENTRY_t), 0,
                   sizeof(BROAD_L3_MAC_KEY_t));

  /* Create the Route AVL tree */
  avlCreateAvlTree(&hapiBroadL3RouteTree,
                   hapiBroadL3RouteTreeHeap,
                   hapiBroadL3RouteDataHeap,
                   HAPI_BROAD_L3_ROUTE_TBL_SIZE,
                   sizeof(BROAD_L3_ROUTE_ENTRY_t), 0,
                   sizeof(BROAD_L3_ROUTE_KEY_t));

  /* Create the Route AVL tree */
  avlCreateAvlTree(&hapiBroadL3TunnelTree,
                   hapiBroadL3TunnelTreeHeap,
                   hapiBroadL3TunnelDataHeap,
                   HAPI_BROAD_L3_MAX_TUNNELS,
                   sizeof(BROAD_L3_TUNNEL_ENTRY_t), 0,
                   sizeof(BROAD_L3_TUNNEL_KEY_t));

  /* Start a timer to retry failed entries (if any) */
  if (pBroadL3RetryTimer == L7_NULL)
  {
    osapiTimerAdd((void*)hapiBroadL3RetryTimer, 0, 0,
                  HAPI_BROAD_L3_RETRY_INTERVAL, &pBroadL3RetryTimer );
  }

  memset(&routedVlanMask, 0, sizeof(routedVlanMask));

  hapiBroadL3RouterCardInited = L7_TRUE;

  return result;
}


/******************************************************************************
*
* @purpose Removes logical card
*
* @param  dapiUsp         USP for the logical router card
* @param  cmd             DAPI cmd for removing card (not used)
* @param  data            Data associated with the DAPI cmd (not used)
* @param *handle          DAPI driver object handle
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
******************************************************************************/
L7_RC_t hapiBroadL3RouterCardRemove(DAPI_USP_t *usp,
                                     DAPI_CMD_t cmd,
                                     void *data,
                                     DAPI_t *dapi_g)
{
  L7_RC_t result;

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3RouterCardRemove: Unit %d "
                      "Slot %d\n", usp->unit, usp->slot);

  hapiBroadL3DebugTaskIdSet();

  /* Ideally, all routes/hosts should have been deleted/queued for deletion.
   * Clear any entries left in the AVL tree (for whatever reason).
   */
  hapiBroadL3Clear();

  /* Wait for Async task to finish processing */
  hapiBroadL3AsyncWait(usp, cmd);

  /* Delete the special CPU L3 interface */
  if (hapiBroadL3CpuIntfId != HAPI_BROAD_INVALID_L3_INTF_ID)
  {
    L7_int32 rv;
    usl_bcm_l3_intf_t   cpuIntfInfo;
    usl_bcm_l3_egress_t bcmEgrObj;

    usl_bcm_l3_egress_t_init(&bcmEgrObj);

    /* We use reserved 127.0.0.1 address to represent
    ** cpu next-hop egress object
    */
    bcmEgrObj.key.addr.l3a_ip_addr = HAPI_BROAD_L3_CPU_EGR_NHOP_IP;

    result = usl_bcmx_l3_egress_destroy(&bcmEgrObj, &hapiBroadL3CpuEgrNhopId, 1, &rv);

    HAPI_BROAD_L3_BCMX_DBG(rv, "hapiBroadL3RouterCardRemove: CPU nexthop"
                           " deleted (%s)\n", bcm_errmsg(rv));

    if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNIT))
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
              "hapiBroadL3RouterCardRemove: Failed deleting CPU nexthop %d\n", rv);
    }

    hapiBroadL3CpuEgrNhopId = HAPI_BROAD_INVALID_L3_INTF_ID;

    usl_bcm_l3_intf_t_init(&cpuIntfInfo);
    cpuIntfInfo.bcm_data.l3a_intf_id = hapiBroadL3CpuIntfId;
    hapiBroadL3CpuIntfId = HAPI_BROAD_INVALID_L3_INTF_ID;

    rv = usl_bcmx_l3_intf_delete(&cpuIntfInfo);
    /* Ignore BCM_E_UNIT error as it is possible that we may try to remove
     * logical cards after bcmx_unit has detached. The hardware L3 tables
     * would be cleared by bcm_clear anyways.
     */
    HAPI_BROAD_L3_BCMX_DBG(rv, "hapiBroadL3RouterCardRemove: CPU interface"
                           " deleted (%s)\n", bcm_errmsg(rv));
    if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNIT))
    {
      HAPI_BROAD_L3_LOG_ERROR(rv);
    }

  }

  /* Delete the AVL trees. Yes, fatal error if delete fails */
  result = avlDeleteAvlTree(&hapiBroadL3NhopTree);
  if (L7_SUCCESS != result)
  {
    HAPI_BROAD_L3_LOG_ERROR(0);
  }

  result = avlDeleteAvlTree(&hapiBroadL3EcmpTree);
  if (L7_SUCCESS != result)
  {
    HAPI_BROAD_L3_LOG_ERROR(0);
  }

  result = avlDeleteAvlTree(&hapiBroadL3HostTree);
  if (L7_SUCCESS != result)
  {
    HAPI_BROAD_L3_LOG_ERROR(0);
  }

  result = avlDeleteAvlTree(&hapiBroadL3MacTree);
  if (L7_SUCCESS != result)
  {
    HAPI_BROAD_L3_LOG_ERROR(0);
  }

  result = avlDeleteAvlTree(&hapiBroadL3RouteTree);
  if (L7_SUCCESS != result)
  {
    HAPI_BROAD_L3_LOG_ERROR(0);
  }

  result = avlDeleteAvlTree(&hapiBroadL3TunnelTree);
  if (L7_SUCCESS != result)
  {
    HAPI_BROAD_L3_LOG_ERROR(0);
  }

  if (pBroadL3RetryTimer != L7_NULL)
  {
    osapiTimerFree(pBroadL3RetryTimer);
    pBroadL3RetryTimer = L7_NULL;
  }

  memset(&routedVlanMask, 0, sizeof(routedVlanMask));

  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose Clear any internal AVL entries submitting them for deletion.
*
* @returns L7_RC_t result
*
* @notes   This function is only intended for router card remove operation.
*
* @end
*
******************************************************************************/
static L7_RC_t hapiBroadL3Clear(void)
{
  L7_ushort16 count = 0;
  BROAD_L3_ROUTE_KEY_t rtKey;
  BROAD_L3_ROUTE_ENTRY_t *pRouteEntry;
  BROAD_L3_HOST_KEY_t hostKey;
  BROAD_L3_HOST_ENTRY_t *pHostEntry;
  BROAD_L3_NH_KEY_t nhopKey;
  BROAD_L3_NH_ENTRY_t *pNhopEntry;
  BROAD_L3_ECMP_KEY_t ecmpKey;
  BROAD_L3_ECMP_ENTRY_t *pEcmpEntry;

  memset(&rtKey, 0, sizeof(rtKey));
  memset(&hostKey, 0, sizeof(hostKey));
  memset(&nhopKey, 0, sizeof(nhopKey));
  memset(&ecmpKey, 0, sizeof(ecmpKey));

  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);
  /* Check if any routes are pending, if yes, loop through the submit for
   * deletion
   */
  if (hapiBroadL3RouteTree.count > 0)
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3Clear: Pending routes %d\n",
                        hapiBroadL3RouteTree.count);

    pRouteEntry = avlSearchLVL7(&hapiBroadL3RouteTree, &rtKey, AVL_EXACT);
    if (pRouteEntry == L7_NULL)
    {
      pRouteEntry = avlSearchLVL7(&hapiBroadL3RouteTree, &rtKey, AVL_NEXT);
    }
    while(pRouteEntry != L7_NULL)
    {
      if (pRouteEntry->wl.cmd == BROAD_L3_ROUTE_CMD_NO_OP)
      {
        pRouteEntry->wl.cmd = BROAD_L3_ROUTE_CMD_DELETE;
        HAPI_BROAD_L3_ROUTE_WLIST_ADD(pRouteEntry);
        count++;
      }
      else
      {
        pRouteEntry->wl.cmd = BROAD_L3_ROUTE_CMD_DELETE;
      }
      pRouteEntry = avlSearchLVL7(&hapiBroadL3RouteTree, pRouteEntry, AVL_NEXT);
    }

    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3Clear: Queued %d routes for "
                        "for deletion\n", count);
  }

  /* Check if any hosts are pending, if yes, loop through the submit for
   * deletion
   */

  if (hapiBroadL3HostTree.count > 0)
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3Clear: Pending hosts %d\n",
                        hapiBroadL3HostTree.count);

    pHostEntry = avlSearchLVL7(&hapiBroadL3HostTree, &hostKey, AVL_EXACT);
    if (pHostEntry == L7_NULL)
    {
      pHostEntry = avlSearchLVL7(&hapiBroadL3HostTree, &hostKey, AVL_NEXT);
    }

    count = 0;

    while(pHostEntry != L7_NULL)
    {
      if (pHostEntry->wl.cmd == BROAD_L3_HOST_CMD_NO_OP)
      {
        pHostEntry->wl.cmd = BROAD_L3_HOST_CMD_DELETE;
        HAPI_BROAD_L3_HOST_WLIST_ADD(pHostEntry);
        count++;
      }
      else
      {
        pHostEntry->wl.cmd = BROAD_L3_HOST_CMD_DELETE;
      }
      pHostEntry = avlSearchLVL7(&hapiBroadL3HostTree, pHostEntry, AVL_NEXT);
    }

    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3Clear: Queued %d hosts for "
                        "for deletion\n", count);
  }

  /* Check if any nexthops are pending, if yes, loop through the submit for
   * deletion
   */
  if (hapiBroadL3EcmpTree.count > 0)
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3Clear: Pending ECMP %d\n",
                        hapiBroadL3EcmpTree.count);

    pEcmpEntry = avlSearchLVL7(&hapiBroadL3EcmpTree, &ecmpKey,  AVL_EXACT);

    if (pEcmpEntry == L7_NULL)
    {
      pEcmpEntry = avlSearchLVL7(&hapiBroadL3EcmpTree, &ecmpKey, AVL_NEXT);
    }

    count = 0;
    while(pEcmpEntry != L7_NULL)
    {
      /* Just flag and do not queue for deletion yet. */
      pEcmpEntry->flags  |= BROAD_L3_ECMP_ZERO_COUNT;
      hapiBroadL3ProcessZeroCountNhops = L7_TRUE;
      count++;

      pEcmpEntry = avlSearchLVL7(&hapiBroadL3EcmpTree, pEcmpEntry, AVL_NEXT);
    }

    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3Clear: Queued %d ECMP for "
                        "for deletion\n", count);
  }

  if (hapiBroadL3NhopTree.count > 0)
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3Clear: Pending Nexthops %d\n",
                        hapiBroadL3NhopTree.count);

    pNhopEntry = avlSearchLVL7(&hapiBroadL3NhopTree, &nhopKey,  AVL_EXACT);
    if (pNhopEntry == L7_NULL)
    {
      pNhopEntry = avlSearchLVL7(&hapiBroadL3NhopTree, &nhopKey, AVL_NEXT);
    }

    count = 0;
    while (pNhopEntry != L7_NULL)
    {
      /* Just flag and do not queue for deletion yet. */
      pNhopEntry->flags |= BROAD_L3_NH_ZERO_COUNT;
      hapiBroadL3ProcessZeroCountNhops = L7_TRUE;
      count++;

      pNhopEntry = avlSearchLVL7(&hapiBroadL3NhopTree, pNhopEntry, AVL_NEXT);
    }

    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3Clear: Queued %d Nexthops for "
                        "for deletion\n", count);
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose Initializes logical tunnel card
*
* @param  dapiUsp         USP for the logical tunnel card
* @param  cmd             DAPI cmd for inserting card (not used)
* @param  data            Data associated with the DAPI cmd (not used)
* @param *handle          DAPI driver object handle
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
******************************************************************************/
L7_RC_t hapiBroadL3TunnelCardInsert(DAPI_USP_t *dapiUsp,
                                    DAPI_CMD_t cmd,
                                    void *data,
                                    void *handle)
{
  DAPI_USP_t usp;
  DAPI_t *dapi_g = (DAPI_t*)handle;
  BROAD_PORT_t *hapiPortPtr;
  DAPI_PORT_t *dapiPortPtr;
  L7_RC_t rc;

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3TunnelCardInsert: Unit %d "
                      "Slot %d\n", dapiUsp->unit, dapiUsp->slot);

  /*
   * Leverage existing "generic" card function registrations for
   * logical interfaces.  But override the ones that we explicitly
   * handle in hardware below.
   */
  rc = dapiGenericCardInsert(dapiUsp, cmd, data, handle);
  if (rc != L7_SUCCESS)
  {
    return rc;
  }

  usp.unit = dapiUsp->unit;
  usp.slot = dapiUsp->slot;

  for (usp.port = 0;
       usp.port < dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot;
       usp.port++)
  {
    dapiPortPtr = DAPI_PORT_GET(&usp, dapi_g);

    hapiPortPtr = (BROAD_PORT_t*)osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                             sizeof(BROAD_PORT_t));
    dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port]->hapiPort = (void *)hapiPortPtr;
    bzero((L7_char8*)hapiPortPtr, sizeof(BROAD_PORT_t));
    hapiPortPtr->l3_interface_id = HAPI_BROAD_INVALID_L3_INTF_ID;
    hapiPortPtr->hapiModeparm.pTunnel = L7_NULL;

    /*
     * Override hardware-supported operations
     */
    dapiPortPtr->cmdTable[DAPI_CMD_TUNNEL_CREATE] =
          (HAPICTLFUNCPTR_t)hapiBroadL3TunnelCreate;
    dapiPortPtr->cmdTable[DAPI_CMD_TUNNEL_DELETE] =
          (HAPICTLFUNCPTR_t)hapiBroadL3TunnelDelete;
    dapiPortPtr->cmdTable[DAPI_CMD_TUNNEL_NEXT_HOP_SET] =
          (HAPICTLFUNCPTR_t)hapiBroadL3TunnelNextHopSet;
    dapiPortPtr->cmdTable[DAPI_CMD_IPV6_INTF_ADDR_ADD] =
          (HAPICTLFUNCPTR_t)hapiBroadL3IntfAddrAdd;
    dapiPortPtr->cmdTable[DAPI_CMD_IPV6_INTF_ADDR_DELETE] =
          (HAPICTLFUNCPTR_t)hapiBroadL3IntfAddrDelete;
  }
  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Removes logical tunnel card
*
* @param  dapiUsp         USP for the logical tunnel card
* @param  cmd             DAPI cmd for removing card (not used)
* @param  data            Data associated with the DAPI cmd (not used)
* @param *handle          DAPI driver object handle
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
******************************************************************************/
L7_RC_t hapiBroadL3TunnelCardRemove(DAPI_USP_t *dapiUsp,
                                    DAPI_CMD_t cmd,
                                    void *data,
                                    void *handle)
{
  DAPI_USP_t usp;
  DAPI_t *dapi_g = (DAPI_t*)handle;
  BROAD_PORT_t *hapiPortPtr;

  usp.unit = dapiUsp->unit;
  usp.slot = dapiUsp->slot;

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3TunnelCardRemove: Unit %d "
                      "Slot %d\n", dapiUsp->unit, dapiUsp->slot);

  for (usp.port = 0;
       usp.port < dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot;
       usp.port++)
  {
    hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);

    osapiFree(L7_DRIVER_COMPONENT_ID, hapiPortPtr);
  }
  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Replay global L3 config
*
* @param *dapi_g          DAPI driver object handle
*
* @returns L7_RC_t result
*
* @notes  Only configuration that is system based is applied/replayed
*
* @end
*
******************************************************************************/
L7_RC_t hapiBroadL3ApplyConfig(DAPI_t *dapi_g)
{
  L7_RC_t result;
  DAPI_ROUTING_MGMT_CMD_t cmd;
  DAPI_USP_t              usp = {-1,-1,-1};  /* NA for these commands */
  DAPI_SYSTEM_t *dapiSystemPtr = dapi_g->system;

  memset(&cmd, 0, sizeof(DAPI_ROUTING_MGMT_CMD_t));
  cmd.cmdData.forwardingConfig.getOrSet = DAPI_CMD_SET;
  cmd.cmdData.forwardingConfig.enable = dapiSystemPtr->routeForwardConfigEnable;

  /* Replay global v4 routing */
  result = hapiBroadL3RouteFwdConfig (&usp,
                                      DAPI_CMD_ROUTING_ROUTE_FORWARDING_CONFIG,
                                      &cmd,
                                      dapi_g);

  if (result != L7_SUCCESS)
  {
    return result;
  }

  memset(&cmd, 0, sizeof(DAPI_ROUTING_MGMT_CMD_t));
  cmd.cmdData.forwardingConfig.getOrSet = DAPI_CMD_SET;
  cmd.cmdData.forwardingConfig.enable = dapiSystemPtr->ipv6ForwardConfigEnable;

  /* Replay global v6 routing */
  result = hapiBroadL3RouteFwdConfig (&usp,
                                      DAPI_CMD_IPV6_ROUTE_FORWARDING_CONFIG,
                                      &cmd,
                                      dapi_g);

  if (result != L7_SUCCESS)
  {
    return result;
  }

  /* Replay global ICMP redirect config */
  memset(&cmd, 0, sizeof(DAPI_ROUTING_MGMT_CMD_t));
  cmd.cmdData.icmpRedirectsConfig.enable = dapiSystemPtr->icmpRedirEnable;
  cmd.cmdData.icmpRedirectsConfig.getOrSet = DAPI_CMD_SET;

  result = hapiBroadL3IcmpRedirConfig (&usp,
                                       DAPI_CMD_ROUTING_ROUTE_ICMP_REDIRECTS_CONFIG,
                                       &cmd,
                                       dapi_g);

  return result;
}



/******************************************************************************
*                            DAPI_CMD HANDLERS
*******************************************************************************/


/******************************************************************************
*
* @purpose Enables/Disables routing globally.
*
* @param  usp         unit/slot/port
* @param  cmd         DAPI_CMD_ROUTING_ROUTE_FORWARDING_CONFIG or
*                     DAPI_CMD_IPV6_ROUTE_FORWARDING_CONFIG
* @param  data        Data associated with the DAPI cmd
* @param *dapi_g      DAPI driver object
*
* @returns L7_RC_t result
*
* @notes This function is called as part of hapiBroadApplyConfig() to re-apply
*        global config when new units/slots join the stack.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3RouteFwdConfig(DAPI_USP_t *usp,
                                  DAPI_CMD_t cmd,
                                  void *data,
                                  DAPI_t *dapi_g)
{
  DAPI_ROUTING_MGMT_CMD_t *dapiCmd = (DAPI_ROUTING_MGMT_CMD_t*)data;
  DAPI_SYSTEM_t *dapiSystemPtr = dapi_g->system;
  L7_BOOL ipv6_enable = dapiSystemPtr->ipv6ForwardConfigEnable;
  L7_BOOL ipv4_enable = dapiSystemPtr->routeForwardConfigEnable;
  L7_BOOL enable = L7_FALSE;
  L7_RC_t result = L7_SUCCESS;


  switch (cmd)
  {
    /* Do not skip the command based on global state. Always replay
     * the command so that new units joining the stack have correct
     * routing state.
     */
    case DAPI_CMD_ROUTING_ROUTE_FORWARDING_CONFIG:
      ipv4_enable = dapiCmd->cmdData.forwardingConfig.enable;
      dapiSystemPtr->routeForwardConfigEnable = ipv4_enable;

      HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3RouteFwdConfig: IPv4 %d\n",
                          ipv4_enable);
      break;

    case DAPI_CMD_IPV6_ROUTE_FORWARDING_CONFIG:
      ipv6_enable = dapiCmd->cmdData.forwardingConfig.enable;
      dapiSystemPtr->ipv6ForwardConfigEnable = ipv6_enable;

      HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3RouteFwdConfig: IPv6 %d\n",
                          ipv6_enable);
      break;

    default:
      return L7_FAILURE;
      break;
  }


  /* Enable/Disable both v4 & v6. Individual controls are available,
   * bcmPortControlIP4 & bcmPortControlIP6, but are per port. Use global
   * version instead.
   */
  enable = (((ipv4_enable == L7_TRUE) || (ipv6_enable == L7_TRUE))
            ? L7_TRUE : L7_FALSE);

  /* L3 is always enabled now regardless of presence of L3 package to support
  **  wireless switching builds
  */

  if (DAPI_CMD_IPV6_ROUTE_FORWARDING_CONFIG == cmd)
  {
    /* Install or remove IPv6 Link local routes. */
    if (L7_TRUE == ipv6_enable)
    {
      hapiBroadL3v6LinkLocalRouteAddDel(L7_TRUE, dapi_g);
    }
    else
    {
      hapiBroadL3v6LinkLocalRouteAddDel(L7_FALSE, dapi_g);
    }
  }

  return result;
}


/******************************************************************************
*
* @purpose Creates a routing interface
*
* @param  usp       unit/slot/port of routing interface (port/VLAN)
* @param  cmd       DAPI_CMD_ROUTING_INTF_ADD
* @param  *data     command data
* @param  *dapi_g   DAPI driver object
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
static L7_RC_t hapiBroadL3IntfCreate(DAPI_USP_t *usp,
                                     DAPI_CMD_t cmd,
                                     void *data,
                                     DAPI_t *dapi_g)
{
  L7_RC_t result = L7_SUCCESS;
  DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd = (DAPI_ROUTING_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t    *dapiPortPtr;
  DAPI_SYSTEM_t  *dapiSystemPtr = dapi_g->system;
  BROAD_PORT_t   *hapiPortPtr;
  bcm_mac_t       mac_addr;
  bcm_vlan_t      vid;
  usl_bcm_l3_intf_t  intfInfo;
  L7_int32        rv;
  usl_bcm_port_learn_mode_t learnMode;
  bcm_port_discard_t discardMode;
  usl_bcm_port_vlan_t portVlanMask;

  memset(&portVlanMask, 0, sizeof(portVlanMask));

  hapiBroadL3DebugTaskIdSet();

  dapiPortPtr = DAPI_PORT_GET(usp,dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp,dapi_g);

  memcpy(mac_addr, dapiCmd->cmdData.rtrIntfAdd.macAddr.addr, sizeof(bcm_mac_t));

  vid = dapiCmd->cmdData.rtrIntfAdd.vlanID;

  HAPI_BROAD_L3_DEBUG(broadL3Debug,
                      "hapiBroadL3IntfCreate - "
                      "usp = %u.%u.%u, vid = %d, "
                      "mac = %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
                      usp->unit, usp->slot, usp->port, vid,
                      mac_addr[0], mac_addr[1], mac_addr[2],
                      mac_addr[3], mac_addr[4], mac_addr[5]);

  if (L7_TRUE == IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
  {
    /* Remove the port from VLANs. The VLAN membership will be restored when
     * routing is disabled on the port.
     */
    hapiBroadAddRemovePortFromVlans(usp, 0, dapi_g);

    if (0 == vid)
    {
      result = L7_FAILURE;
      HAPI_BROAD_L3_LOG_ERROR(result);
    }

    /* Add the CPU and this port to the assigned VLAN */
    rv = usl_bcmx_vlan_create(vid);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      HAPI_BROAD_L3_LOG_ERROR(rv);
    }

    L7_VLAN_SETMASKBIT(portVlanMask.vlan_membership_mask, vid);

    rv = usl_bcmx_port_vlan_member_add(hapiPortPtr->bcmx_lport, &portVlanMask);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      HAPI_BROAD_L3_LOG_ERROR (vid);
    }

    /* Set STP state to forwarding for all instances */
    hapiBroadDot1sPortAllGroupsStateSet(usp, BCM_STG_STP_FORWARD, dapi_g);

    /* Disable learning on this port. As the VLAN is reserved, MAC addresses
     * are not learnt by USL. Packets can only be routed on this port, unlike
     * VLAN routing interfaces. If learning is not disabled, then hardware
     * will learn entries which are ignored by USL/HAPI. This causes stale
     * entries in hardware. Also, the port can only be part routing VLAN.
     */
    learnMode = BCM_PORT_LEARN_FWD;
    rv = usl_bcmx_port_learn_set(hapiPortPtr->bcmx_lport, learnMode);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      HAPI_BROAD_L3_LOG_ERROR(rv);
    }

    /* Set PVID to this VLAN */
    rv = usl_bcmx_port_untagged_vlan_set(hapiPortPtr->bcmx_lport, vid);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      HAPI_BROAD_L3_LOG_ERROR(rv);
    }

    /* Set default priority */
    rv = usl_bcmx_port_untagged_priority_set(hapiPortPtr->bcmx_lport, 0);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      HAPI_BROAD_L3_LOG_ERROR(rv);
    }

    /* Enable ingress filtering */
    result = hapiBroadVlanIngressFilterSet(hapiPortPtr->bcmx_lport, L7_TRUE);

    if (result != L7_SUCCESS)
    {
      HAPI_BROAD_L3_LOG_ERROR(hapiPortPtr->bcmx_lport);
    }

    /* Set port to accept both untagged and tagged frames */
    discardMode = BCM_PORT_DISCARD_NONE;
    rv = usl_bcmx_port_discard_set(hapiPortPtr->bcmx_lport, discardMode);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      HAPI_BROAD_L3_LOG_ERROR(rv);
    }

    hapiPortPtr->port_based_routing_vlanid = vid;
    dapiPortPtr->modeparm.physical.routerIntfEnabled = L7_TRUE;

    /* The v4 address/mask fields in DAPI_CMD_ROUTING_INTF_ADD command are not
     * really used. The actual IP addr/mask of routing interface are set using
     * DAPI_CMD_ROUTING_INTF_IPV4_ADDR_SET command. Although, HAPI L3 doesn't
     * really use the addr/mask in dapiPortPtr->modeparm.
     */

    memcpy(dapiPortPtr->modeparm.physical.macAddr,
           dapiCmd->cmdData.rtrIntfAdd.macAddr.addr,
           sizeof(dapiPortPtr->modeparm.physical.macAddr));
    dapiPortPtr->modeparm.physical.mtu = dapiCmd->cmdData.rtrIntfAdd.mtu;
    dapiPortPtr->modeparm.physical.encapsType =
      dapiCmd->cmdData.rtrIntfAdd.encapsType;
    dapiPortPtr->modeparm.physical.bCastCapable =
      dapiCmd->cmdData.rtrIntfAdd.broadcastCapable;

    /* Update pbvlans */
    hapiBroadL3ProtocolVlanRemove(usp, dapi_g);
  }
  else if (IS_PORT_TYPE_LOGICAL_VLAN(dapiPortPtr) == L7_TRUE)
  {
    /* The v4 address/mask fields in DAPI_CMD_ROUTING_INTF_ADD command are not
     * really used. The actual IP addr/mask of routing interface are set using
     * DAPI_CMD_ROUTING_INTF_IPV4_ADDR_SET command. Although, HAPI L3 doesn't
     * really use the addr/mask in dapiPortPtr->modeparm.
     */

    memcpy(dapiPortPtr->modeparm.router.macAddr,
           dapiCmd->cmdData.rtrIntfAdd.macAddr.addr,
           sizeof(dapiPortPtr->modeparm.router.macAddr));
    dapiPortPtr->modeparm.router.mtu = dapiCmd->cmdData.rtrIntfAdd.mtu;
    dapiPortPtr->modeparm.router.encapsType =
      dapiCmd->cmdData.rtrIntfAdd.encapsType;
    dapiPortPtr->modeparm.router.bCastCapable =
      dapiCmd->cmdData.rtrIntfAdd.broadcastCapable;
    dapiPortPtr->modeparm.router.vlanID = dapiCmd->cmdData.rtrIntfAdd.vlanID;
    L7_VLAN_SETMASKBIT(routedVlanMask, dapiCmd->cmdData.rtrIntfAdd.vlanID);
  }
  else
  {
    /* Invalid port type. Neither port based nor VLAN based */
    HAPI_BROAD_L3_LOG_ERROR(L7_FAILURE);
  }

  /* Create the L3 interface in the hardware */
  usl_bcm_l3_intf_t_init(&intfInfo);
  intfInfo.bcm_data.l3a_intf_id = HAPI_BROAD_INVALID_L3_INTF_ID;
  memcpy(intfInfo.bcm_data.l3a_mac_addr, mac_addr, sizeof(bcm_mac_t));
  intfInfo.bcm_data.l3a_vid = vid;
  intfInfo.bcm_data.l3a_flags |= BCM_L3_ADD_TO_ARL;
  /* Set the L3 MTU for the interface (enforced for devices which support
   * per L3-interface MTU).
   */
  intfInfo.bcm_data.l3a_mtu = dapiCmd->cmdData.rtrIntfAdd.mtu;

  rv = usl_bcmx_l3_intf_create(&intfInfo);
  if ((L7_BCMX_OK(rv) != L7_TRUE) ||
      (intfInfo.bcm_data.l3a_intf_id == HAPI_BROAD_INVALID_L3_INTF_ID))
  {
    HAPI_BROAD_L3_LOG_ERROR(rv);
  }
  else
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug,
                        "hapiBroadL3IntfCreate - intf create succeeded "
                        "intf_id = %d\n", intfInfo.bcm_data.l3a_intf_id);
    hapiPortPtr->l3_interface_id = intfInfo.bcm_data.l3a_intf_id;

    /* We could use soc_feature_egr_l3_mtu to check for L3 MTU support, but it
     * breaks for mixed stacking. So, update IP MTU in both ways always
     * (per-port and per-l3-intf, only one takes affect on a given device).
     */

    /* Enforce the L3 MTU for devices that support per-port L3 MTU */
    hapiBroadL3IntfMtuSet(usp, dapiCmd->cmdData.rtrIntfAdd.mtu, dapi_g);

    /* Install Host policy, to capture unicast ARP packets */
    hapiBroadL3HostPolicyInstall (usp, dapiCmd->cmdData.rtrIntfAdd.macAddr.addr, dapi_g);

    dapiPortPtr->modeparm.router.routerIntfEnabled = L7_TRUE;

    /* Check the global knob for ICMP redirect control */
    if (dapiSystemPtr->icmpRedirEnable == L7_TRUE)
    {
      hapiBroadL3IcmpRedirVlanConfig(vid, L7_TRUE);
    }
  }

  return result;
}


/******************************************************************************
*
* @purpose Modifies a routing interface
*
* @param  usp       unit/slot/port of routing interface (port/VLAN)
* @param  cmd       DAPI_CMD_ROUTING_INTF_MODIFY
* @param  *data     command data
* @param  *dapi_g   DAPI driver object
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
******************************************************************************/
static L7_RC_t hapiBroadL3IntfModify(DAPI_USP_t *usp,
                                     DAPI_CMD_t cmd,
                                     void *data,
                                     DAPI_t *dapi_g)
{
  DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd = (DAPI_ROUTING_INTF_MGMT_CMD_t*)data;
  DAPI_ROUTING_INTF_MGMT_CMD_t  deleteRoutingIntf;
  DAPI_ROUTING_INTF_MGMT_CMD_t  createRoutingIntf;

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "****hapiBroadL3IntfModify****\n");

  /* Implement interface modify as Intf Delete followed by Intf Create */
  deleteRoutingIntf.cmdData.rtrIntfDelete.ipAddr =
    dapiCmd->cmdData.rtrIntfModify.ipAddr;
  deleteRoutingIntf.cmdData.rtrIntfDelete.ipMask =
    dapiCmd->cmdData.rtrIntfModify.ipMask;
  deleteRoutingIntf.cmdData.rtrIntfDelete.vlanID =
    dapiCmd->cmdData.rtrIntfModify.vlanID;

  if (hapiBroadL3IntfDelete(usp,
                            DAPI_CMD_ROUTING_INTF_DELETE,
                            &deleteRoutingIntf,
                            dapi_g) == L7_FAILURE)
  {
    return L7_FAILURE;
  }

  createRoutingIntf.cmdData.rtrIntfAdd.ipAddr =
    dapiCmd->cmdData.rtrIntfModify.ipAddr;
  createRoutingIntf.cmdData.rtrIntfAdd.ipMask =
    dapiCmd->cmdData.rtrIntfModify.ipMask;
  createRoutingIntf.cmdData.rtrIntfAdd.vlanID =
    dapiCmd->cmdData.rtrIntfModify.vlanID;
  createRoutingIntf.cmdData.rtrIntfAdd.getOrSet =
    dapiCmd->cmdData.rtrIntfModify.getOrSet;
  createRoutingIntf.cmdData.rtrIntfAdd.broadcastCapable =
    dapiCmd->cmdData.rtrIntfModify.broadcastCapable;
  createRoutingIntf.cmdData.rtrIntfAdd.encapsType =
    dapiCmd->cmdData.rtrIntfModify.encapsType;
  memcpy(createRoutingIntf.cmdData.rtrIntfAdd.macAddr.addr,
         dapiCmd->cmdData.rtrIntfModify.macAddr.addr,
         sizeof(createRoutingIntf.cmdData.rtrIntfAdd.macAddr.addr));
  createRoutingIntf.cmdData.rtrIntfAdd.mtu =
    dapiCmd->cmdData.rtrIntfModify.mtu;

  if (hapiBroadL3IntfCreate(usp,
                            DAPI_CMD_ROUTING_INTF_ADD,
                            &createRoutingIntf,
                            dapi_g) == L7_FAILURE)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Deletes a routing interface
*
* @param  usp       unit/slot/port of routing interface (port/VLAN)
* @param  cmd       DAPI_CMD_ROUTING_INTF_DELETE
* @param  *data     command data
* @param  *dapi_g   DAPI driver object
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
static L7_RC_t hapiBroadL3IntfDelete(DAPI_USP_t *usp,
                                     DAPI_CMD_t cmd,
                                     void *data,
                                     DAPI_t *dapi_g)
{
  L7_RC_t result = L7_SUCCESS;
  DAPI_PORT_t *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  bcm_vlan_t vid;
  usl_bcm_l3_intf_t intfInfo;
  int rv;
  L7_uint32 ip_addr = 0;
  usl_bcm_port_learn_mode_t learnMode;

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3IntfDelete: USP %d:%d:%d\n",
                      usp->unit, usp->slot, usp->port);

  hapiBroadL3DebugTaskIdSet();

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  /* Wait until all pending ARP/Route delete operations are done. */
  /* The reason to wait is to prevent application from adding/deleting intf
   * repeatedly, which results in continuous host/route adds/deletes.
   */
  hapiBroadL3AsyncWait(usp, cmd);

#ifdef L7_MCAST_PACKAGE
  /* Wait for the IP Multicast task to finish any pending group
   * add/delete actions.
   */
  hapiBroadL3McastWait ();
#endif

  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
  {
    ip_addr = dapiPortPtr->modeparm.physical.ipAddr;

    vid = hapiPortPtr->port_based_routing_vlanid;

    /* delete the vlan. this also takes care of clearing ICMP redir config for
     * this vlan
     */
    rv = usl_bcmx_vlan_destroy(vid);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      HAPI_BROAD_L3_LOG_ERROR(rv);
    }

    hapiPortPtr->port_based_routing_vlanid = 0;
    dapiPortPtr->modeparm.physical.routerIntfEnabled = L7_FALSE;
    dapiPortPtr->modeparm.physical.ipAddr = 0;
    dapiPortPtr->modeparm.physical.ipMask = 0;
    hapiBroadL3HostPolicyRemove (usp, dapiPortPtr->modeparm.physical.macAddr, dapi_g);
    memset(dapiPortPtr->modeparm.physical.macAddr,
           0,
           sizeof(dapiPortPtr->modeparm.physical.macAddr));
    dapiPortPtr->modeparm.physical.mtu = 0;
    dapiPortPtr->modeparm.physical.encapsType = 0;
    dapiPortPtr->modeparm.physical.bCastCapable = 0;

    /* Restore VLAN membership of the port. */
    hapiBroadAddRemovePortFromVlans (usp, 1, dapi_g);

    /* Restore pbvlans */
    hapiBroadL3ProtocolVlanAdd(usp, dapi_g);

    /* Pvid, acceptableFrameType and ingress filtering are not restored
     * here, they are restored by the application. This is becoz hapi
     * port structure might not have any valid values for these attributes.
     * This happens when the box is booted with a saved port-based
     * routing configuration.
     */

    /* Restore learning mode on this port */
    if (hpcSoftwareLearningEnabled () == L7_TRUE)
    {
      learnMode = (BCM_PORT_LEARN_CPU | BCM_PORT_LEARN_FWD);
    }
    else
    {
      learnMode = (BCM_PORT_LEARN_ARL |  BCM_PORT_LEARN_FWD | BCM_PORT_LEARN_PENDING);    /* PTin modified: MAC learn */
    }

    rv = usl_bcmx_port_learn_set(hapiPortPtr->bcmx_lport, learnMode);

    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      HAPI_BROAD_L3_LOG_ERROR(rv);
    }

    /* Restore STP state for port. */
    if (L7_BRIDGE_SPEC_SUPPORTED == L7_BRIDGE_SPEC_802DOT1D)
    {
      rv = bcmx_port_stp_set(hapiPortPtr->bcmx_lport,
                             hapiPortPtr->hw_stp_state);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        HAPI_BROAD_L3_LOG_ERROR(hapiPortPtr->bcmx_lport);
      }
    }
    else
    {
      hapiBroadDot1sPortStateCopy(usp, usp, dapi_g);
    }
  }
  else if (IS_PORT_TYPE_LOGICAL_VLAN(dapiPortPtr) == L7_TRUE)
  {
    ip_addr = dapiPortPtr->modeparm.router.ipAddr;

    dapiPortPtr->modeparm.router.ipAddr = 0;
    dapiPortPtr->modeparm.router.ipMask = 0;

    /* Remove the Host policy if not being used by other interfaces */
    hapiBroadL3HostPolicyRemove(usp, dapiPortPtr->modeparm.router.macAddr, dapi_g);

    L7_VLAN_CLRMASKBIT(routedVlanMask, dapiPortPtr->modeparm.router.vlanID);

    memset(dapiPortPtr->modeparm.router.macAddr,
           0,
           sizeof(dapiPortPtr->modeparm.router.macAddr));
    dapiPortPtr->modeparm.router.mtu = 0;
    dapiPortPtr->modeparm.router.encapsType = 0;
    dapiPortPtr->modeparm.router.bCastCapable = 0;
    dapiPortPtr->modeparm.router.vlanID = 0;
  }
  else
  {
    HAPI_BROAD_L3_LOG_ERROR(L7_FAILURE);
  }

  /* For devices which support per-port L3 MTU, restore the port's
   * L3 MTU to max(default). Driver adds 4 bytes, so adjust here.
   */
  hapiBroadL3IntfMtuSet(usp, (L7_MAX_FRAME_SIZE-4), dapi_g);

  usl_bcm_l3_intf_t_init(&intfInfo);
  intfInfo.bcm_data.l3a_intf_id = hapiPortPtr->l3_interface_id;

  /* Before calling intf_delete, make the l3_interface_id invalid
   * so that others cannot use the same intf to update it. TBD
   */
  hapiPortPtr->l3_interface_id = HAPI_BROAD_INVALID_L3_INTF_ID;

  rv = usl_bcmx_l3_intf_delete(&intfInfo);

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3IntfDelete: Deleting %d (%s)\n",
                      intfInfo.bcm_data.l3a_intf_id, bcm_errmsg(rv));
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    HAPI_BROAD_L3_LOG_ERROR(rv);
  }
  else
  {
    hapiPortPtr->multicast_ttl_limit = 0;
  }

  dapiPortPtr->modeparm.router.routerIntfEnabled = L7_FALSE;

  return result;
}


/******************************************************************************
*
* @purpose Sets IPv4 address of routing interface
*
* @param  usp       unit/slot/port of routing interface (port/VLAN based)
* @param  cmd       DAPI_CMD_ROUTING_INTF_IPV4_ADDR_SET
* @param  *data     Command data
* @param  *dapi_g   DAPI driver object
*
* @returns L7_RC_t result
*
* @notes Typically used by IPMAP to set IP addr to NULL incase of failures.
*
* @end
*
******************************************************************************/
static L7_RC_t hapiBroadL3IntfIpv4AddrSet(DAPI_USP_t *usp,
                                          DAPI_CMD_t cmd,
                                          void *data,
                                          DAPI_t *dapi_g)
{
  DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd = (DAPI_ROUTING_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t                  *dapiPortPtr;

  hapiBroadL3DebugDapiCmdPrint(__FUNCTION__, usp, cmd, data);

  dapiPortPtr = DAPI_PORT_GET(usp,dapi_g);

  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
  {
    dapiPortPtr->modeparm.physical.ipAddr  = dapiCmd->cmdData.rtrIntfModify.ipAddr;
    dapiPortPtr->modeparm.physical.ipMask  = dapiCmd->cmdData.rtrIntfModify.ipMask;
  }
  else if (IS_PORT_TYPE_LOGICAL_VLAN(dapiPortPtr) == L7_TRUE)
  {
    dapiPortPtr->modeparm.router.ipAddr = dapiCmd->cmdData.rtrIntfModify.ipAddr;
    dapiPortPtr->modeparm.router.ipMask = dapiCmd->cmdData.rtrIntfModify.ipMask;
  }
  else
  {
    HAPI_BROAD_L3_LOG_ERROR(L7_FAILURE);
  }

  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Adds an IP(v6 only) address to a routing interface
*
* @param  usp    unit/slot/port of routing interface
* @param  cmd    DAPI_CMD_IPV6_INTF_ADDR_ADD
* @param  data   Command data
* @param *dapi_g DAPI driver object
*
* @returns L7_RC_t result
*
* @notes Valid for tunnel interfaces only. DAPI_CMD_ROUTING_INTF_ADDR_ADD
*        has been removed as application doesn't seem to use it.
*
* @end
*
*******************************************************************************/
static L7_RC_t hapiBroadL3IntfAddrAdd(DAPI_USP_t *usp,
                                          DAPI_CMD_t cmd,
                                          void *data,
                                          DAPI_t *dapi_g)
{
  L7_in6_addr_t *ip6Addr;
  DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd = (DAPI_ROUTING_INTF_MGMT_CMD_t *)data;
  DAPI_ROUTING_ARP_CMD_t        hostCmd;
  DAPI_USP_t                    cpuUsp;

  hapiBroadL3DebugDapiCmdPrint(__FUNCTION__, usp, cmd, data);

  memset(&hostCmd, 0, sizeof(hostCmd));

  switch (cmd)
  {
    case DAPI_CMD_IPV6_INTF_ADDR_ADD:

      /* Link local IPv6 addr is set using DAPI_CMD_IPV6_INTF_ADDR_ADD cmd */

      if (IS_USP_TYPE_TUNNEL(usp))
      {
        ip6Addr = &dapiCmd->cmdData.rtrIntfIpAddrAddDelete.ip6Addr;
        if (L7_IP6_IS_ADDR_LINK_LOCAL (ip6Addr) == L7_TRUE)
        {
          /* Ignore link local addresses on tunnels */
          return L7_SUCCESS;
        }

        /* For local IPv6 addresses of tunnels, add host entry pointing to CPU */
        hostCmd.cmdData.arpAdd.getOrSet = DAPI_CMD_SET;
        memcpy (&hostCmd.cmdData.arpAdd.ip6Addr, ip6Addr, sizeof(L7_in6_addr_t));

        /* To add a host entry, we need a MAC address and USP. Use CPU MAC
         * address and USP. Tunnel USP cannot be used, as it doesn't map to
         * to a phyical port
         */
        if (CPU_USP_GET(&cpuUsp) != L7_SUCCESS)
        {
          HAPI_BROAD_L3_LOG_ERROR(0);
        }

        memcpy(&(hostCmd.cmdData.arpAdd.macAddr), hapiBroadL3CpuMac,
               sizeof(bcm_mac_t));

        hostCmd.cmdData.arpAdd.flags = DAPI_ROUTING_ARP_DEST_KNOWN;

        hapiBroadL3HostEntryAdd(&cpuUsp, DAPI_CMD_IPV6_NEIGH_ENTRY_ADD,
                                &hostCmd, dapi_g);

      }
      return L7_SUCCESS;

    /* Note: For v4 interfaces, DAPI_CMD_ROUTING_INTF_IPV4_ADDR_SET is used */
    default:
      HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3IntfAddrAdd:"
                          "Invalid cmd\n");
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Deletes an IP address assigned to L3 interface
*
* @param  usp    unit/slot/port of routing interface
* @param  cmd    DAPI_CMD_INTF_IPV6_ADDR_DELETE
* @param  data   Data associated with the cmd
* @param *dapi_g DAPI driver object
*
* @returns L7_RC_t result
*
* @notes DAPI_CMD_ROUTING_INTF_ADDR_DELETE has been removed as application
*        doesn't seem to use it.
*
* @end
*
******************************************************************************/
static L7_RC_t hapiBroadL3IntfAddrDelete(DAPI_USP_t *usp,
                                             DAPI_CMD_t cmd,
                                             void *data,
                                             DAPI_t *dapi_g)
{
  L7_in6_addr_t *ip6Addr;
  DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd = (DAPI_ROUTING_INTF_MGMT_CMD_t *)data;
  DAPI_ROUTING_ARP_CMD_t        hostCmd;
  DAPI_USP_t                    cpuUsp;

  hapiBroadL3DebugDapiCmdPrint(__FUNCTION__, usp, cmd, data);

  switch (cmd)
  {
    case DAPI_CMD_IPV6_INTF_ADDR_DELETE:
      if (IS_USP_TYPE_TUNNEL(usp))
      {
        ip6Addr = &dapiCmd->cmdData.rtrIntfIpAddrAddDelete.ip6Addr;
        if (L7_IP6_IS_ADDR_LINK_LOCAL (ip6Addr) == L7_TRUE)
        {
          /* Ignore link local addresses on tunnels */
          return L7_SUCCESS;
        }

        memset(&hostCmd, 0 , sizeof (DAPI_ROUTING_ARP_CMD_t));
        hostCmd.cmdData.arpDelete.getOrSet = DAPI_CMD_SET;
        memcpy (&hostCmd.cmdData.arpDelete.ip6Addr, ip6Addr, sizeof(L7_in6_addr_t));

        if (CPU_USP_GET(&cpuUsp) != L7_SUCCESS)
        {
          HAPI_BROAD_L3_LOG_ERROR(0);
        }

        memcpy(&(hostCmd.cmdData.arpDelete.macAddr), hapiBroadL3CpuMac,
               sizeof(bcm_mac_t));

        hapiBroadL3HostEntryDelete(&cpuUsp, DAPI_CMD_IPV6_NEIGH_ENTRY_DELETE,
                                   &hostCmd, dapi_g);
      }
      return L7_SUCCESS;

    /* Note: For v4 interfaces, DAPI_CMD_ROUTING_INTF_IPV4_ADDR_SET is used */
    default:
      HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3IntfAddrDelete:"
                          "Invalid cmd\n");
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Adds a route to the work list and signals ASYNC task
*
* @param  usp    unit/slot/port (not used for this command)
* @param  cmd    DAPI_CMD_ROUTING_ROUTE_ENTRY_ADD or
*                DAPI_CMD_IPV6_ROUTE_ENTRY_ADD
* @param  data   Data associated with the command
* @param *dapi_g DAPI driver object
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3RouteEntryAdd(DAPI_USP_t *usp,
                                 DAPI_CMD_t cmd,
                                 void *data,
                                 DAPI_t *dapi_g)
{
  BROAD_L3_ROUTE_ENTRY_t   route;
  BROAD_L3_ROUTE_ENTRY_t  *pRouteEntry;
  DAPI_ROUTING_MGMT_CMD_t *dapiCmd = (DAPI_ROUTING_MGMT_CMD_t*)data;
  DAPI_ROUTING_ROUTE_ENTRY_t *pRtNhop   = &dapiCmd->cmdData.routeAdd.route;

  hapiBroadL3DebugTaskIdSet();

  hapiBroadL3DebugDapiCmdPrint(__FUNCTION__, usp, cmd, data);

  dapiCmd->cmdData.routeAdd.count++; /* Why is this req / TBD? */

  if (dapiCmd->cmdData.routeAdd.flags & DAPI_ROUTING_ROUTE_DISCARD)
  {
    /* Indicate to app that this doesn't require h/w entry */
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3RouteEntryAdd: route discard\n");
    return L7_ALREADY_CONFIGURED;
  }

  memset(&route, 0, sizeof(BROAD_L3_ROUTE_ENTRY_t));

  switch (cmd)
  {
    case DAPI_CMD_ROUTING_ROUTE_ENTRY_ADD:

      /* Fill the key route information */
      route.key.family = L7_AF_INET;
      route.key.ipAddr.ipv4 = dapiCmd->cmdData.routeAdd.ipAddr;
      route.key.ipMask.ipv4 = dapiCmd->cmdData.routeAdd.ipMask;

      break;

    case DAPI_CMD_IPV6_ROUTE_ENTRY_ADD:
      /*
       * Link local addresses are not added to the route table, they
       * are picked up by the FE80::0/10 route that is installed when
       * IPv6 is enabled.  Just return success here if the caller
       * supplied a link-local address.
       */
      if (L7_IP6_IS_ADDR_LINK_LOCAL(&dapiCmd->cmdData.routeAdd.ip6Addr))
      {
        HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3RouteEntryAdd: Ignoring "
                            "IPv6 Link local route");
        return L7_SUCCESS;
      }

      /*
       * Check if this Ipv6 route has prefix len in the range /65 through /127.
       * Some XGS3 devices cannot handle these prefix lengths. Log the event and
       * and don't let the route in. For 128-bit prefix lenghts, we use host
       * entries to workaround the h/w limitation. See below.
       */

      if ((memcmp(&dapiCmd->cmdData.routeAdd.ip6Mask,
                  hapiBroadL3Ipv6Mask128,
                  sizeof(hapiBroadL3Ipv6Mask128)) != 0) &&
          ((dapiCmd->cmdData.routeAdd.ip6Mask.in6.addr32[2] != 0) ||
           (dapiCmd->cmdData.routeAdd.ip6Mask.in6.addr32[3] != 0)))
      {
        if (hapiBroadL3Ipv6Prefix128Supported() == L7_FALSE)
        {
          HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3RouteEntryAdd: Ignoring "
                             " IPv6 route with /65 through /127 prefix len\n");

          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                  "hapiBroadL3RouteEntryAdd: Ignoring IPv6 route with prefix len"
                " /65 through /127\n");
          return L7_SUCCESS;
        }
      }

      /*
       * Local 128 bit network routes for tunnels are already covered
       * by the tunnel in-to-me host entry that is created when the
       * tunnel is assigned it's local address.  Hence, if this is a
       * local 128 bit route to a tunnel USP (i.e. there is 1 nexthop,
       * the nexthop USP is a tunnel USP, the route has a 128 bit
       * prefix, and the destination addr is the same as that nexthop
       * addr), just return success.
       */
      if ((dapiCmd->cmdData.routeAdd.count == 1) &&
          (IS_USP_TYPE_TUNNEL(&dapiCmd->cmdData.routeAdd.route.equalCostRoute[0].usp)
           == L7_TRUE) &&
          (memcmp(&dapiCmd->cmdData.routeAdd.ip6Mask,
                  hapiBroadL3Ipv6Mask128,
                  sizeof(dapiCmd->cmdData.routeAdd.ip6Mask)) == 0) &&
          (memcmp(&dapiCmd->cmdData.routeAdd.route.equalCostRoute[0].ip6Addr,
                  &dapiCmd->cmdData.routeAdd.ip6Addr,
                  sizeof(dapiCmd->cmdData.routeAdd.ip6Addr)) == 0))
      {
        HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3RouteEntryAdd: Ignoring "
                            "128-bit tunnel connected IPv6 route");
        return L7_SUCCESS;
      }

      /* Fill the key route information */
      route.key.family = L7_AF_INET6;
      memcpy(&route.key.ipAddr, &dapiCmd->cmdData.routeAdd.ip6Addr,
             sizeof(bcm_ip6_t));
      memcpy(&route.key.ipMask, &dapiCmd->cmdData.routeAdd.ip6Mask,
             sizeof(bcm_ip6_t));

      break;

    default:
      HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3RouteEntryAdd: invalid cmd");
      return L7_FAILURE;
  }

  /* Pre-check the table full condition. It is possible that async task hasn't
   * got a chance to process the deletes on the work list
   */
  if (hapiBroadL3RouteTableFull(BROAD_L3_ROUTE_CMD_ADD,
                                route.key.family,
                                pRtNhop, dapi_g) == L7_TRUE)
  {
    hapiBroadL3AsyncWait(usp, cmd);

    /* If there is room, continue. Else bail out */
    if (hapiBroadL3RouteTableFull(BROAD_L3_ROUTE_CMD_ADD,
                                  route.key.family,
                                  pRtNhop, dapi_g) == L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID,
              "hapiBroadL3RouteEntryAdd: Route/Nexthop/ECMP table full.");
      return L7_FAILURE;
    }
  }

  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  /* Check if the route already exists */
  pRouteEntry = avlSearchLVL7(&hapiBroadL3RouteTree, &route, AVL_EXACT);

  if (pRouteEntry != L7_NULL)
  {
     /* Duplicate route already exists */
     HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3RouteEntryAdd: Route already"
                         "exists\n");
     /* This should be the case only when a delete is pending */
     if (pRouteEntry->wl.cmd == BROAD_L3_ROUTE_CMD_DELETE)
     {
       pRouteEntry->wl.cmd = BROAD_L3_ROUTE_CMD_MODIFY; /* Treat this as modify */
       hapiBroadL3RouteNhopProcess(pRouteEntry, pRtNhop, dapi_g);
     }
     else /* Route add is invalid for other cases */
     {
       HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
       return L7_FAILURE;
     }
  }
  else
  {
    /* Insert the route in to AVL tree and Work list */
    pRouteEntry = hapiBroadL3AvlInsert(&hapiBroadL3RouteTree, &route);

    if (pRouteEntry != L7_NULL)
    {
      pRouteEntry->numNextHops = 0;
      pRouteEntry->rv = 0;
      hapiBroadL3RouteNhopProcess(pRouteEntry, pRtNhop, dapi_g);
      pRouteEntry->wl.cmd = BROAD_L3_ROUTE_CMD_ADD;
      HAPI_BROAD_L3_ROUTE_WLIST_ADD(pRouteEntry);
    }
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

  /* Wake up Async task to process the work lists */
  HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;

  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose Process the next hops associated with the given route. This function
*          is called from DAPI_CMD handler for route add/modify.
*
* @param   pRouteEntry - Route information
* @param   pRtNhop     - Next hop information in the DAPI_CMD
*
* @notes   Caller must take hapiBroadL3Sema before invoking this function.
*
* @end
*
*******************************************************************************/
static L7_RC_t hapiBroadL3RouteNhopProcess(BROAD_L3_ROUTE_ENTRY_t  *pRouteEntry,
                                           DAPI_ROUTING_ROUTE_ENTRY_t *pRtNhop,
                                           DAPI_t *dapi_g)
{
  L7_uint8 i;
  L7_uint8 numNextHops = 0;
  BROAD_L3_NH_ENTRY_t     *pNhopEntry = L7_NULLPTR;
  BROAD_L3_NH_ENTRY_t     *pNhops[L7_RT_MAX_EQUAL_COST_ROUTES] = {0};
  BROAD_L3_ECMP_ENTRY_t   *pEcmpEntry = L7_NULLPTR;
  BROAD_L3_NH_KEY_t       nhKey;

  /* Check how many next hops are associated with this route */
  for (i=0; i < L7_RT_MAX_EQUAL_COST_ROUTES; i++)
  {
    if (L7_TRUE == pRtNhop->equalCostRoute[i].valid)
    {
      if (isValidUsp(&(pRtNhop->equalCostRoute[i].usp), dapi_g) != L7_TRUE)
      {
        HAPI_BROAD_L3_LOG_ERROR(0);
      }
      numNextHops++;
    }
  }

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3RouteNhopProcess: Number of "
                      "next hops %d\n", numNextHops);

  if ((pRouteEntry->key.family == L7_AF_INET6) &&
      (memcmp(&(pRouteEntry->key.ipMask), hapiBroadL3Ipv6Mask128,
              sizeof(hapiBroadL3Ipv6Mask128)) == 0))
  {
    if ((hapiBroadL3Ipv6Prefix128Supported() == L7_FALSE) && (numNextHops > 1))
    {
      /* Only allow one next hop if the route is a 128 bit prefix route.
       * If h/w doesn't support >64 prefix len, we use host entries to get
       * around the 64 bit prefix length limitation, and host entries
       * cannot be ECMP.
       */
       numNextHops = 1;
    }
  }


  /* Loop through the current nhops and decrement the ref count. If the route
   * requires the nhop, it will be prevented from deletion below.
   *
   * Note: At this point, the nhops cannot be queued to worklist for deletion.
   * The route has to be modified on async side before the nexthop is deleted.
   * In other words, a next hop cannot be deleted if a route entry uses it.
   * So, the order of L3 hardware operations matters.
   */

  if (BROAD_L3_IS_ROUTE_NON_ECMP(pRouteEntry))
  {
    /* For non-ECMP, decrement the ref count for the single nhop */
    if (pRouteEntry->pNhop != L7_NULL)
    {
      /* Just flag and do not queue for deletion yet */
      pRouteEntry->pNhop->ref_count--;
      pRouteEntry->pNhop->flags |= BROAD_L3_NH_ZERO_COUNT;
    }
  }
  else if (BROAD_L3_IS_ROUTE_ECMP(pRouteEntry))
  {
    /* For ECMP, flag all individual nhops and ECMP nhop */
    if (pRouteEntry->pEcmp != L7_NULL)
    {
      /* Just flag and do not queue for deletion yet */
      for (i=0; i < pRouteEntry->numNextHops; i++)
      {
        if (pRouteEntry->pEcmp->pNhop[i] != L7_NULL)
        {
          pRouteEntry->pEcmp->pNhop[i]->flags |= BROAD_L3_NH_ZERO_COUNT;
          pRouteEntry->pEcmp->pNhop[i]->ref_count--;
        }
      }
      pRouteEntry->pEcmp->flags |= BROAD_L3_ECMP_ZERO_COUNT;
      pRouteEntry->pEcmp->ref_count--;
    }
  }

  if (numNextHops == 0)
  {
    /* No next hops for the given route. Will be Pointed to CPU next hop */
    /* Typically, these are reject routes */
    if (pRouteEntry->numNextHops > 0)
    {
      hapiBroadL3ProcessZeroCountNhops = L7_TRUE;
    }
    pRouteEntry->numNextHops = numNextHops;
    pRouteEntry->pNhop = L7_NULL;
    return L7_SUCCESS;
  }


  /* Loop through the new set of next hops and search the next hop tree */
  for (i=0; i < numNextHops; i++)
  {
    memset(&nhKey, 0, sizeof(BROAD_L3_NH_KEY_t));

    /* Fill the next hop key from the given route information */
    nhKey.family = pRouteEntry->key.family;
    nhKey.addrUsp.usp = pRtNhop->equalCostRoute[i].usp;
    if (L7_AF_INET6 == nhKey.family)
    {
      memcpy(&nhKey.addrUsp.addr, &pRtNhop->equalCostRoute[i].ip6Addr,
             sizeof(bcm_ip6_t));
    }
    else
    {
      nhKey.addrUsp.addr.ipv4  = pRtNhop->equalCostRoute[i].ipAddr;
    }

    /* Search the next hop tree to see if the NH already exists */
    if (IS_USP_TYPE_TUNNEL(&nhKey.addrUsp.usp))
    {
      pNhopEntry = hapiBroadL3TunnelNhopGet(&nhKey, dapi_g);

      if (pNhopEntry == L7_NULL)
      {
        HAPI_BROAD_L3_LOG_ERROR(0);
      }

      /* If this route is a 6to4 route, see if it resolves any relay routes */
      if(L7_IP6_IS_ADDR_6TO4(&pRouteEntry->key.ipAddr.ipv6))
      {
        hapiBroadL3Tunnel6to4RelayUpdate(&nhKey.addrUsp.usp, pRouteEntry,
                                         pNhopEntry, dapi_g);
      }
    }
    else
    {
      pNhopEntry = avlSearchLVL7(&hapiBroadL3NhopTree, &nhKey, AVL_EXACT);
      if (pNhopEntry == L7_NULL)
      {
        /* Create the next hop entry */
        pNhopEntry = hapiBroadL3NhopEntryAdd(&nhKey, dapi_g);
      }
      else
      {
        HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3RouteNhopProcess: Reusing"
                            " next hop %d\n", pNhopEntry->egressId);

        /* Next hop is used by this route, make sure it's not being deleted */
        hapiBroadL3NhopEntryUpdate(pNhopEntry, BROAD_L3_NH_CMD_ADD);
      }
    }

    pNhops[i] = pNhopEntry;

    /* Increment ref count for all nhops, even if route already had this nhop */
    pNhopEntry->ref_count++;
    pNhopEntry->flags &= ~BROAD_L3_NH_ZERO_COUNT;
  }

  /* If the route has more than one nexthop, create/reuse ECMP entry */
  if (numNextHops > 1)
  {
    /* If re-using ECMP nhop, the count gets incremented */
    pEcmpEntry = hapiBroadL3EcmpEntryAdd (pNhops, numNextHops);
    pEcmpEntry->flags &= ~BROAD_L3_ECMP_ZERO_COUNT;
  }

  /* Check if any of the next hops / ECMP are marked for deletion */
  if (BROAD_L3_IS_ROUTE_ECMP(pRouteEntry))
  {
    if (pRouteEntry->pEcmp != L7_NULL)
    {
      for (i=0; i < pRouteEntry->numNextHops; i++)
      {
        pNhopEntry  = pRouteEntry->pEcmp->pNhop[i];
        if ((pNhopEntry != L7_NULL) && (pNhopEntry->flags & BROAD_L3_NH_ZERO_COUNT))
        {
          hapiBroadL3ProcessZeroCountNhops = L7_TRUE;
          break;
        }
      }
      if (pRouteEntry->pEcmp->flags & BROAD_L3_ECMP_ZERO_COUNT)
      {
        hapiBroadL3ProcessZeroCountNhops = L7_TRUE;
      }
    }
  }
  else
  {
    pNhopEntry = pRouteEntry->pNhop;
    if ((pNhopEntry != L7_NULL) && (pNhopEntry->flags & BROAD_L3_NH_ZERO_COUNT))
    {
      hapiBroadL3ProcessZeroCountNhops = L7_TRUE;
    }
  }

  /* Update the route entry to point to new nhops/ecmp */
  if (numNextHops > 1)
  {
    pRouteEntry->pEcmp = pEcmpEntry;
    pRouteEntry->pNhop = L7_NULL;
  }
  else
  {
    pRouteEntry->pNhop = pNhops[0];
    pRouteEntry->pEcmp = L7_NULL;
  }

  pRouteEntry->numNextHops = numNextHops;
  /* Note: Caller will enqueue the route entry to work list if required */

  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose Creates a next hop entry and queue it to Wlist.
*
* @param   pNhopKey  Next hop key information
*
* @returns Pointer to the next hop entry.
*
* @notes   Caller must take hapiBroadL3Sema before invoking this function.
*
* @end
*
*******************************************************************************/
static BROAD_L3_NH_ENTRY_t * hapiBroadL3NhopEntryAdd(BROAD_L3_NH_KEY_t *pNhopKey,
                                                     DAPI_t *dapi_g)
{
  BROAD_PORT_t            *hapiPortPtr;
  BROAD_L3_NH_ENTRY_t     *pNhopEntry;
  BROAD_L3_NH_ENTRY_t     nextHop;

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "*** hapiBroadL3NhopEntryAdd ***\n");

  memset(&nextHop, 0, sizeof(BROAD_L3_NH_ENTRY_t));
  memcpy(&nextHop.key, pNhopKey, sizeof(BROAD_L3_NH_KEY_t));

  pNhopEntry = hapiBroadL3AvlInsert(&hapiBroadL3NhopTree, &nextHop);

  if (pNhopEntry != L7_NULL)
  {
    hapiPortPtr = HAPI_PORT_GET(&pNhopKey->addrUsp.usp, dapi_g);

    pNhopEntry->flags = 0; /* Unresolved */
    pNhopEntry->pMac = L7_NULL;
    pNhopEntry->rv = 0;

    /* Default the next hop to outgoing L3 interface. This can change
     * if next hop resolves to a tunnel
     */
    pNhopEntry->l3_intf_id = hapiPortPtr->l3_interface_id;
    pNhopEntry->egressId = HAPI_BROAD_L3_INVALID_EGR_ID;

    /* Set the command and Queue the next hop in the work list for creating
     * new next hop entry in hardware
     */
    pNhopEntry->wl.cmd = BROAD_L3_NH_CMD_ADD;

    HAPI_BROAD_L3_NH_WLIST_ADD(pNhopEntry);
    HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;
  }
  else
  {
    HAPI_BROAD_L3_LOG_ERROR(0);
  }

  return pNhopEntry;
}


/******************************************************************************
*
* @purpose Update the Work list command of an existing next hop entry.
*
* @param   Pointer to the next hop entry.
* @param   Next hop command.
*
* @returns none.
*
* @notes   Caller must take hapiBroadL3Sema before invoking this function.
*
* @end
*
*******************************************************************************/
void hapiBroadL3NhopEntryUpdate (BROAD_L3_NH_ENTRY_t *pNhopEntry,
                                 BROAD_L3_NH_CMD_t   nextCmd)
{
  BROAD_L3_NH_CMD_t curCmd;
  L7_BOOL  enqueue = L7_FALSE;
  L7_int32 action;

  /* Action based on [current cmd][next cmd].
   *       next-cmd  NO_OP  ADD  MODIFY  DELETE
   *  cur-cmd
   *   NO_OP          x      x     x       x
   *   ADD            x      x     x       x
   *   MODIFY         x      x     x       x
   *   DELETE         x      x     x       x
   *
   */

  static L7_int32 nhState[4][4] =
  {
   {-1,                    -1, BROAD_L3_NH_CMD_MODIFY, BROAD_L3_NH_CMD_DELETE},
   {-1,                    -1,                     -1, BROAD_L3_NH_CMD_DELETE},
   {-1,                    -1,                     -1, BROAD_L3_NH_CMD_DELETE},
   {-1, BROAD_L3_NH_CMD_NO_OP,                     -1,                     -1},
  };


  if (pNhopEntry == L7_NULL)
  {
    return;
  }

  curCmd = pNhopEntry->wl.cmd;
  action = nhState[curCmd][nextCmd];

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3NhopEntryUpdate: %d "
                     "Count %d, Current cmd %d, Next cmd %d, action %d\n",
                      pNhopEntry->egressId, pNhopEntry->ref_count,
                      curCmd, nextCmd, action);
  switch (action)
  {
    case -1:
      /* Nothing to be done */
      return;

    case BROAD_L3_NH_CMD_NO_OP:
      /* Next hop must be removed from WL */
      HAPI_BROAD_L3_NH_WLIST_DEL(pNhopEntry);
      break;

    case BROAD_L3_NH_CMD_MODIFY:
      /* Next hop must be modified */
      pNhopEntry->wl.cmd = BROAD_L3_NH_CMD_MODIFY;

      /* If the entry is not already on the work list, add it */
      if (BROAD_L3_NH_CMD_NO_OP == curCmd)
      {
        enqueue = L7_TRUE;
      }
      break;

    case BROAD_L3_NH_CMD_DELETE:
      /* Next hop must be deleted if ref_count is 0 and if not marked for
       * delayed deletion
       */
      pNhopEntry->ref_count--;
      if ((pNhopEntry->ref_count == 0) &&
          ((pNhopEntry->flags & BROAD_L3_NH_ZERO_COUNT) == 0))
      {
        pNhopEntry->wl.cmd = BROAD_L3_NH_CMD_DELETE;
        /* If the entry is not already on the work list, add it */
        if (BROAD_L3_NH_CMD_NO_OP == curCmd)
        {
          enqueue = L7_TRUE;
        }
        else if (BROAD_L3_NH_CMD_ADD == curCmd) /* next hop never got processed */
        {
          HAPI_BROAD_L3_NH_WLIST_DEL(pNhopEntry);
          avlDeleteEntry(&hapiBroadL3NhopTree, pNhopEntry);
        }
      }
      else
      {
        /* Still being used, don't delete it */
        return;
      }

      break;

    default:
      return;
  }

  if (L7_TRUE == enqueue)
  {
    HAPI_BROAD_L3_NH_WLIST_ADD(pNhopEntry);
    HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;

    /* If the next hop is linked to ECMP, update ECMP object too */
    hapiBroadL3EcmpResolve(pNhopEntry);

    /* If any tunnels are linked to this nexthop, update them too */
    hapiBroadL3TunnelResolve(pNhopEntry);
  }
}

/******************************************************************************
*
* @purpose Creates an ECMP  entry and queue it to Wlist.
*
* @param   pNhops      Next hop key information
* @param   numNextHops Number of next hops
*
* @returns Pointer to the next hop entry.
*
* @notes   Caller must take hapiBroadL3Sema before invoking this function.
*
* @end
*
*******************************************************************************/
static BROAD_L3_ECMP_ENTRY_t * hapiBroadL3EcmpEntryAdd(BROAD_L3_NH_ENTRY_t **pNhops,
                                                       L7_uint8 numNextHops)
{
  L7_uint8 i;
  BROAD_L3_ECMP_ENTRY_t *pEcmpEntry = L7_NULL;
  BROAD_L3_ECMP_ENTRY_t ecmp;

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "*** hapiBroadL3EcmpEntryAdd ***\n");

  memset(&ecmp, 0, sizeof(BROAD_L3_ECMP_ENTRY_t));

  for (i=0; i < numNextHops; i++)
  {
    ecmp.key.family = pNhops[i]->key.family;
    memcpy(&ecmp.key.addrUsp[i], &pNhops[i]->key.addrUsp, sizeof(BROAD_L3_ADDR_USP_t));
  }

  /* Search the ECMP tree to see if the ECMP exists */
  pEcmpEntry = avlSearchLVL7(&hapiBroadL3EcmpTree, &ecmp, AVL_EXACT);
  if (pEcmpEntry == L7_NULL)
  {
    /* Create the ECMP entry */
    pEcmpEntry = hapiBroadL3AvlInsert(&hapiBroadL3EcmpTree, &ecmp);

    if (pEcmpEntry != L7_NULL)
    {
      pEcmpEntry->egressId = HAPI_BROAD_L3_INVALID_EGR_ID;
      pEcmpEntry->flags = 0; /* Unresolved */
      pEcmpEntry->wl.cmd = BROAD_L3_ECMP_CMD_ADD;

      HAPI_BROAD_L3_ECMP_WLIST_ADD(pEcmpEntry);
      HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;
      /* Note, wlist will be processed only after releasing hapiBroadL3Sema */
    }
    else
    {
      HAPI_BROAD_L3_LOG_ERROR(0);
    }
  }

  if (pEcmpEntry != L7_NULL)
  {
    for (i=0; i < numNextHops; i++)
    {
      pEcmpEntry->pNhop[i] = pNhops[i];
    }

    pEcmpEntry->ref_count++;
  }

  return pEcmpEntry;

}


/******************************************************************************
*
* @purpose Update the Work list command of an existing ECMP entry.
*
* @param   Pointer to the ECMP entry.
* @param   ECMP command.
*
* @returns none.
*
* @notes   Caller must take hapiBroadL3Sema before invoking this function.
*
* @end
*
*******************************************************************************/
void hapiBroadL3EcmpEntryUpdate (BROAD_L3_ECMP_ENTRY_t *pEcmpEntry,
                                 BROAD_L3_ECMP_CMD_t   nextCmd)
{
  BROAD_L3_ECMP_CMD_t curCmd;
  L7_BOOL  enqueue = L7_FALSE;
  L7_int32 action;

  static L7_int32 ecmpState[4][4] =
  {
   {-1,                    -1, BROAD_L3_ECMP_CMD_MODIFY, BROAD_L3_ECMP_CMD_DELETE},
   {-1,                    -1,                     -1, BROAD_L3_ECMP_CMD_DELETE},
   {-1,                    -1,                     -1, BROAD_L3_ECMP_CMD_DELETE},
   {-1, BROAD_L3_ECMP_CMD_NO_OP,                   -1,                     -1},
  };

  if (pEcmpEntry == L7_NULL)
  {
    return;
  }

  curCmd = pEcmpEntry->wl.cmd;
  action = ecmpState[curCmd][nextCmd];

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3EcmpEntryUpdate: %d "
                     "Count %d, Current cmd %d, Next cmd %d, action %d\n",
                      pEcmpEntry->egressId, pEcmpEntry->ref_count,
                      curCmd, nextCmd, action);

  switch (action)
  {
    case -1:
      /* Nothing to be done */
      return;

    case BROAD_L3_ECMP_CMD_NO_OP:
      /* Ecmp node must be removed from WL */
      HAPI_BROAD_L3_ECMP_WLIST_DEL(pEcmpEntry);
      break;

    case BROAD_L3_ECMP_CMD_MODIFY:
      /* ECMP object to be modified */
      pEcmpEntry->wl.cmd = BROAD_L3_ECMP_CMD_MODIFY;

      /* If the entry is not already on the work list, add it */
      if (BROAD_L3_ECMP_CMD_NO_OP == curCmd)
      {
        enqueue = L7_TRUE;
      }
      break;

    case BROAD_L3_ECMP_CMD_DELETE:

      /* Ecmp object must be deleted if ref_count is 0 and if not marked for
       * delayed deletion
       */
      pEcmpEntry->ref_count--;
      if ((pEcmpEntry->ref_count == 0) &&
          ((pEcmpEntry->flags & BROAD_L3_ECMP_ZERO_COUNT) == 0))
      {
        pEcmpEntry->wl.cmd = BROAD_L3_ECMP_CMD_DELETE;
        /* If the entry is not already on the work list, add it */
        if (BROAD_L3_ECMP_CMD_NO_OP == curCmd)
        {
          enqueue = L7_TRUE;
        }
      }
      else
      {
        /* Still being used, don't delete it */
        return;
      }

      if (BROAD_L3_ECMP_CMD_ADD == curCmd) /* ECMP never got processed */
      {
        HAPI_BROAD_L3_ECMP_WLIST_DEL(pEcmpEntry);
        avlDeleteEntry(&hapiBroadL3EcmpTree, pEcmpEntry);
      }
      break;

    default:
      return;
  }

  if (L7_TRUE == enqueue)
  {
    HAPI_BROAD_L3_ECMP_WLIST_ADD(pEcmpEntry);
    HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;
  }
}


/******************************************************************************
*
* @purpose Given a nexthop entry, Search for an ECMP entry and update it.
*
* @param   pNhop      Next hop key information
*
* @returns none
*
* @notes   Caller must take hapiBroadL3Sema before invoking this function.
*
* @end
*
*******************************************************************************/
static void hapiBroadL3EcmpResolve(BROAD_L3_NH_ENTRY_t *pNhop)
{
  L7_uint8 i;
  BROAD_L3_ECMP_ENTRY_t *pEcmpEntry;
  BROAD_L3_ECMP_KEY_t   ecmp;

  memset(&ecmp, 0, sizeof(BROAD_L3_ECMP_KEY_t));

  /* This function is typically called when a next hop gets resolved.
   * As number of ECMP groups are limited [512 max], searching the entire ECMP
   * tree is not really a performance issue
   */

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "*** hapiBroadL3EcmpResolve ***");
  pEcmpEntry = avlSearchLVL7(&hapiBroadL3EcmpTree, (void *)&ecmp,  AVL_EXACT);
  if (pEcmpEntry == L7_NULL)
  {
    pEcmpEntry = avlSearchLVL7(&hapiBroadL3EcmpTree, (void *)&ecmp, AVL_NEXT);
  }

  while(pEcmpEntry != L7_NULL)
  {
    for (i=0; i < L7_RT_MAX_EQUAL_COST_ROUTES; i++)
    {
      /* Check if the ECMP nhop has any route pending on it */
      if ((pEcmpEntry->ref_count > 0) && (pEcmpEntry->pNhop[i] == pNhop))
      {
        hapiBroadL3EcmpEntryUpdate(pEcmpEntry, BROAD_L3_ECMP_CMD_MODIFY);
        break;
      }
    }
    pEcmpEntry = avlSearchLVL7(&hapiBroadL3EcmpTree, pEcmpEntry, AVL_NEXT);
  }
}

/******************************************************************************
*
* @purpose Modifies a route in the AVL tree and adds it to work list
*
* @param  usp    unit/slot/port (not used for this command)
* @param  cmd    DAPI_CMD_ROUTING_ROUTE_ENTRY_MODIFY or
*                DAPI_CMD_IPV6_ROUTE_ENTRY_MODIFY
* @param  data   Data associated with the command
* @param *dapi_g DAPI driver object
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
static L7_RC_t hapiBroadL3RouteEntryModify(DAPI_USP_t *usp,
                                           DAPI_CMD_t cmd,
                                           void *data,
                                           DAPI_t *dapi_g)
{
  BROAD_L3_ROUTE_ENTRY_t   route;
  BROAD_L3_ROUTE_ENTRY_t  *pRouteEntry;
  DAPI_ROUTING_MGMT_CMD_t *dapiCmd = (DAPI_ROUTING_MGMT_CMD_t*)data;
  DAPI_ROUTING_ROUTE_ENTRY_t *pRtNhop   = &dapiCmd->cmdData.routeModify.route;

  hapiBroadL3DebugDapiCmdPrint(__FUNCTION__, usp, cmd, data);

  memset(&route, 0, sizeof(BROAD_L3_ROUTE_ENTRY_t));

  switch (cmd)
  {
    case DAPI_CMD_ROUTING_ROUTE_ENTRY_MODIFY:
      /* Fill the key route information */
      route.key.family = L7_AF_INET;
      route.key.ipAddr.ipv4 = dapiCmd->cmdData.routeModify.ipAddr;
      route.key.ipMask.ipv4 = dapiCmd->cmdData.routeModify.ipMask;
      break;

    case DAPI_CMD_IPV6_ROUTE_ENTRY_MODIFY:
      /*
       * Link local addresses are not kept in the route table, they
       * are picked up by the FE80::0/10 route that is installed when
       * IPv6 is enabled.  Just return success here if the caller
       * supplied a link-local address.
       */
      if (L7_IP6_IS_ADDR_LINK_LOCAL(&dapiCmd->cmdData.routeModify.ip6Addr))
      {
        return L7_SUCCESS;
      }

      /* Fill the key route information */
      route.key.family = L7_AF_INET6;
      memcpy(&route.key.ipAddr, &dapiCmd->cmdData.routeModify.ip6Addr,
             sizeof(bcm_ip6_t));
      memcpy(&route.key.ipMask, &dapiCmd->cmdData.routeModify.ip6Mask,
             sizeof(bcm_ip6_t));
      break;

    default:
      HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3RouteEntryModify: invalid"
                          " cmd");
      return L7_FAILURE;
  }

  /* Check if the route nexthops can be accomodated */
  if (hapiBroadL3RouteTableFull(BROAD_L3_ROUTE_CMD_MODIFY,
                                route.key.family,
                                pRtNhop, dapi_g) == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID,
            "hapiBroadL3RouteEntryModify: Nexthop/ECMP table full.");
    return L7_FAILURE;
  }

  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  /* Check if the route exists */
  pRouteEntry = avlSearchLVL7(&hapiBroadL3RouteTree, &route, AVL_EXACT);

  if (pRouteEntry == L7_NULL)
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3RouteEntryModify: not found\n");
    HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
    return L7_SUCCESS;
  }

  hapiBroadL3RouteNhopProcess(pRouteEntry, pRtNhop, dapi_g);

  /* Enqueue the route to work list */
  if (pRouteEntry->wl.cmd == BROAD_L3_ROUTE_CMD_NO_OP)
  {
    pRouteEntry->wl.cmd = BROAD_L3_ROUTE_CMD_MODIFY;
    HAPI_BROAD_L3_ROUTE_WLIST_ADD(pRouteEntry);
  }
  /* If an ADD is pending, nhops are updated, so the add will pick new nhops */
  /* If a MODIFY is pending, it will pick new nhops */
  /* If a DEL is pending, leave the route to be deleted. */

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

  /* Wake up Async task to process the work lists */
  HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;

  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose Deletes an existing route.
*
* @param  usp    unit/slot/port (not used for this command)
* @param  cmd    DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE or
*                DAPI_CMD_IPV6_ROUTE_ENTRY_DELETE
* @param  data   Data associated with the command.
* @param *dapi_g DAPI Driver object
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3RouteEntryDelete(DAPI_USP_t *usp,
                                    DAPI_CMD_t cmd,
                                    void *data,
                                    DAPI_t *dapi_g)
{
  L7_uint8                 i;
  BROAD_L3_ROUTE_ENTRY_t   route;
  BROAD_L3_ROUTE_ENTRY_t  *pRouteEntry;
  BROAD_L3_NH_ENTRY_t     *pNhopEntry;
  BROAD_L3_ECMP_ENTRY_t   *pEcmpEntry;
  DAPI_ROUTING_MGMT_CMD_t *dapiCmd = (DAPI_ROUTING_MGMT_CMD_t*)data;

  hapiBroadL3DebugDapiCmdPrint(__FUNCTION__, usp, cmd, data);

  hapiBroadL3DebugTaskIdSet();

  memset(&route, 0, sizeof(BROAD_L3_ROUTE_ENTRY_t));

  switch (cmd)
  {
    case DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE:
      /* Fill the key route information */
      route.key.family = L7_AF_INET;
      route.key.ipAddr.ipv4 = dapiCmd->cmdData.routeDelete.ipAddr;
      route.key.ipMask.ipv4 = dapiCmd->cmdData.routeDelete.ipMask;
      break;

    case DAPI_CMD_IPV6_ROUTE_ENTRY_DELETE:
      /*
       * Link local addresses are not kept in the route table, they
       * are picked up by the FE80::0/10 route that is installed when
       * IPv6 is enabled.  Just return success here if the caller
       * supplied a link-local address.
       */
      if (L7_IP6_IS_ADDR_LINK_LOCAL(&dapiCmd->cmdData.routeDelete.ip6Addr))
      {
        return L7_SUCCESS;
      }

      /* Fill the key route information */
      route.key.family = L7_AF_INET6;
      memcpy(&route.key.ipAddr, &dapiCmd->cmdData.routeDelete.ip6Addr,
             sizeof(bcm_ip6_t));
      memcpy(&route.key.ipMask, &dapiCmd->cmdData.routeDelete.ip6Mask,
             sizeof(bcm_ip6_t));
      break;

    default:
      HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3RouteEntryDelete: invalid"
                          " cmd");
      return L7_FAILURE;
  }

  dapiCmd->cmdData.routeDelete.count--;

  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  /* Check if the route exists */
  pRouteEntry = avlSearchLVL7(&hapiBroadL3RouteTree, &route, AVL_EXACT);

  if (pRouteEntry == L7_NULL)
  {
    /* IPmap may delete a non-existing route. Ignore. Also, discards routes
     * are skipped while adding, so these routes we will not be found
     */
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3RouteEntryDelete: not found\n");
    HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
    return L7_SUCCESS;
  }

  /* Check if there is a pending operation on the route */
  if (pRouteEntry->wl.cmd == BROAD_L3_ROUTE_CMD_NO_OP)
  {
    /* Enqueue the route to work list */
    pRouteEntry->wl.cmd = BROAD_L3_ROUTE_CMD_DELETE;
    HAPI_BROAD_L3_ROUTE_WLIST_ADD(pRouteEntry);

    /* Note: The next hop objects should be deleted only on the Async side
     * except for the special case below
     */
  }
  else
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3RouteEntryDelete: pending "
                        "L3 operation on route %d\n", pRouteEntry->wl.cmd);
    if (pRouteEntry->wl.cmd == BROAD_L3_ROUTE_CMD_ADD)
    {
      /* This route didn't get processed by async task, delete it from tree */
      pNhopEntry = pRouteEntry->pNhop;
      pEcmpEntry = pRouteEntry->pEcmp;
      HAPI_BROAD_L3_ROUTE_WLIST_DEL(pRouteEntry);

      if (avlDeleteEntry(&hapiBroadL3RouteTree, pRouteEntry) == L7_NULL)
      {
        HAPI_BROAD_L3_LOG_ERROR(0);
      }

      /* Special case: Delete the next hop is not being used */
      if (pNhopEntry != L7_NULLPTR)
      {
        hapiBroadL3NhopEntryUpdate(pNhopEntry, BROAD_L3_NH_CMD_DELETE);
      }

      if (pEcmpEntry != L7_NULLPTR)
      {
        for (i=0; i < L7_RT_MAX_EQUAL_COST_ROUTES; i++)
        {
          if (pEcmpEntry->pNhop[i] != L7_NULL)
          {
            hapiBroadL3NhopEntryUpdate(pEcmpEntry->pNhop[i], BROAD_L3_NH_CMD_DELETE);
          }
        }
        hapiBroadL3EcmpEntryUpdate(pEcmpEntry, BROAD_L3_ECMP_CMD_DELETE);
      }
    }
    else if (pRouteEntry->wl.cmd == BROAD_L3_ROUTE_CMD_MODIFY)
    {
      /* Mark the route to delete instead */
      pRouteEntry->wl.cmd = BROAD_L3_ROUTE_CMD_DELETE;
    }
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

  /* Wake up Async task to process the work lists */
  HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;

  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Deletes all the existing IPv4 routes.
*
* @param  usp    unit/slot/port (not used for this command)
* @param  cmd    DAPI_CMD_ROUTING_ROUTE_ENTRY_DELETE_ALL
* @param  data   Data associated with the command.
* @param *dapi_g DAPI Driver object
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t
hapiBroadL3RouteEntryDeleteAll (DAPI_USP_t *usp,
                                DAPI_CMD_t cmd,
                                void *data,
                                DAPI_t *dapi_g)
{
  L7_ushort16 count = 0;
  BROAD_L3_ROUTE_KEY_t rtKey;
  BROAD_L3_ROUTE_ENTRY_t *pRouteEntry = L7_NULLPTR;

  hapiBroadL3DebugDapiCmdPrint (__FUNCTION__, usp, cmd, data);

  hapiBroadL3DebugTaskIdSet();

  memset (&rtKey, 0, sizeof(rtKey));

  if (hapiBroadL3RouteTree.count > 0)
  {
    HAPI_BROAD_L3_DEBUG (broadL3Debug, "hapiBroadL3RouteEntryDeleteAll: Deleting "
                         "only IPv4 Route entries among %d routes\n",
                         hapiBroadL3RouteTree.count);

    HAPI_BROAD_L3_SEMA_TAKE (hapiBroadL3Sema, L7_WAIT_FOREVER);
    pRouteEntry = avlSearchLVL7 (&hapiBroadL3RouteTree, &rtKey, AVL_NEXT);
    while(pRouteEntry != L7_NULL)
    {
      if (pRouteEntry->key.family == L7_AF_INET)
      {
        if (pRouteEntry->wl.cmd == BROAD_L3_ROUTE_CMD_NO_OP)
        {
          pRouteEntry->wl.cmd = BROAD_L3_ROUTE_CMD_DELETE;
          HAPI_BROAD_L3_ROUTE_WLIST_ADD (pRouteEntry);
          count++;
        }
        else
        {
          pRouteEntry->wl.cmd = BROAD_L3_ROUTE_CMD_DELETE;
        }
      }
      pRouteEntry = avlSearchLVL7 (&hapiBroadL3RouteTree, pRouteEntry, AVL_NEXT);
    }
    HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

    if (count > 0)
    {
      HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3RouteEntryDeleteAll: Queued %d "
                          "routes for for deletion\n", count);
      /* Wake up Async task to process the work lists */
      HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;
    }
  }

  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose Adds a IPv4/IPv6Host in the AVL tree and queues it to work list.
*
* @param  usp     unit/slot/port of routing interface
* @param  cmd     DAPI_CMD_ROUTING_ARP_ENTRY_ADD or
*                 DAPI_CMD_IPV6_NEIGH_ENTRY_ADD
* @param  data    Data associate with host add command
* @param *dapi_g  DAPI driver object
*
* @returns L7_RC_t L7_SUCCESS or L7_FAILURE
*
* @notes  It is not out of question for the V6 stack to report changes to
*         IPv6 hosts as host-add requests (in fact, that seems to be the way
*         it is).
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3HostEntryAdd(DAPI_USP_t *usp,
                                DAPI_CMD_t cmd,
                                void *data,
                                DAPI_t *dapi_g)
{
  BROAD_L3_NH_KEY_t     nhKey;
  BROAD_L3_HOST_ENTRY_t host;
  BROAD_L3_HOST_ENTRY_t *pHostEntry;
  BROAD_L3_NH_ENTRY_t   *pNhopEntry;
  BROAD_L3_NH_ENTRY_t   *pTunNhop = L7_NULL;
  BROAD_L3_MAC_ENTRY_t  *pMacEntry;
  DAPI_ROUTING_ARP_CMD_t *dapiCmd = (DAPI_ROUTING_ARP_CMD_t*)data;

  hapiBroadL3DebugDapiCmdPrint(__FUNCTION__, usp, cmd, data);

  hapiBroadL3DebugTaskIdSet();

  if (dapiCmd->cmdData.arpAdd.getOrSet == DAPI_CMD_GET)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_HAPI_ERROR, "hapiBroadL3HostEntryAdd: GET command"
                        " not supported\n");
    return L7_FAILURE;
  }

  /* if this host entry is not resolved, don't add it */
  if ((dapiCmd->cmdData.arpAdd.flags & DAPI_ROUTING_ARP_DEST_KNOWN) == 0)
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3HostEntryAdd: unresolved"
                        "host cannot be added\n");
    return L7_SUCCESS;
  }

  if ((dapiCmd->cmdData.arpAdd.macAddr.addr[0] & 0x1) ||
      (dapiCmd->cmdData.arpAdd.flags & DAPI_ROUTING_ARP_NET_DIR_BCAST))
  {
    return L7_FAILURE;
  }

  memset(&host, 0, sizeof(BROAD_L3_HOST_ENTRY_t));
  memset(&nhKey, 0, sizeof(BROAD_L3_NH_KEY_t));

  if (isValidUsp(usp, dapi_g) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  host.key.addrUsp.usp = *usp;

  switch (cmd)
  {
    case DAPI_CMD_ROUTING_ARP_ENTRY_ADD:
      /* Fill the key information */
      host.key.family = L7_AF_INET;
      host.key.addrUsp.addr.ipv4 = dapiCmd->cmdData.arpAdd.ipAddr;
      break;

    case DAPI_CMD_IPV6_NEIGH_ENTRY_ADD:
      host.key.family = L7_AF_INET6;
      memcpy(&host.key.addrUsp.addr, &dapiCmd->cmdData.arpAdd.ip6Addr,
             sizeof(bcm_ip6_t));
      break;

    default:
      HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3HostEntryAdd: invalid cmd");
      return L7_FAILURE;
  }


  /* Fill the next hop key from the given host information */
  nhKey.family = host.key.family;
  if (L7_AF_INET6 == nhKey.family)
  {
    memcpy(&nhKey.addrUsp.addr, &host.key.addrUsp.addr, sizeof(bcm_ip6_t));
  }
  else
  {
    nhKey.addrUsp.addr.ipv4  = host.key.addrUsp.addr.ipv4;
  }

  nhKey.addrUsp.usp = host.key.addrUsp.usp;

  /* Before processing the host add, pre-check the L3 tables for full
   * condition. This is likely to occur when L3 Async task doesn't get
   * a chance to delete the queued host/nhop entries
   */
  if (hapiBroadL3HostTableFull(BROAD_L3_HOST_CMD_ADD,
                               &host.key,
                               &dapiCmd->cmdData.arpAdd.macAddr,
                               dapi_g) == L7_TRUE)
  {
    /* Wait for Async task to finish processing */
    hapiBroadL3AsyncWait(usp, cmd);

    /* If there is room, continue. Else bail out */
    if (hapiBroadL3HostTableFull(BROAD_L3_HOST_CMD_ADD,
                                 &host.key,
                                 &dapiCmd->cmdData.arpAdd.macAddr,
                                 dapi_g) == L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID,
              "hapiBroadL3HostEntryAdd:: Host/Nexthop/MAC table full.");
      return L7_FAILURE;
    }
  }

  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  /* Add the MAC entry to the AVL tree (if not exists) */
  pMacEntry = hapiBroadL3MacEntryAdd(usp, &(dapiCmd->cmdData.arpAdd.macAddr),
                                     dapi_g);

  /* Check if MAC entry resolves to a tunnel */
  if (pMacEntry->pTunnelEntry != L7_NULL)
  {
    if (pMacEntry->pTunnelEntry->key.tunnelMode == L7_TUNNEL_MODE_4OVER4)
    {
      /* Use the tunnel next hop for this host */
      pTunNhop = pMacEntry->pTunnelEntry->pTunnelNhop;
      if (pTunNhop == L7_NULL)
      {
        /* If tunnel exists, then a tunnel nexthop must exist */
        HAPI_BROAD_L3_LOG_ERROR(0);
      }
      pTunNhop->ref_count++;
    }
    else
    {
      HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
      return L7_FAILURE;
    }
  }

  /* Search the next hop tree to see if the NH already exists. If the host
   * goes over a tunnel, then the tunnel nexthop will be used instead.
   */
  pNhopEntry = avlSearchLVL7(&hapiBroadL3NhopTree, &nhKey, AVL_EXACT);
  if (pNhopEntry == L7_NULL)
  {
    /* Create the next hop entry */
    pNhopEntry = hapiBroadL3NhopEntryAdd(&nhKey, dapi_g);
  }
  else
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3HostEntryAdd: Reusing"
                        " next hop %d\n", pNhopEntry->egressId);
    hapiBroadL3NhopEntryUpdate(pNhopEntry, BROAD_L3_NH_CMD_ADD);
  }

  /* Link the Next hop to a MAC entry */
  if (pNhopEntry->pMac == L7_NULL)
  {
    hapiBroadL3MacNhopLink (pNhopEntry, pMacEntry);
  }
  else
  {
    /* Here, if the next hop for this host already had a MAC address.
     * See notes. A host add can be issued on a existing host entry
     * instead of a modify
     */
     if (memcmp(pNhopEntry->pMac->key.macAddr,
                dapiCmd->cmdData.arpAdd.macAddr.addr,
                sizeof(bcm_mac_t)) != 0)
     {
       /* Unlink the nhop from old MAC entry and link it to the new MAC */
       hapiBroadL3MacNhopUnlink(pNhopEntry);
       hapiBroadL3MacNhopLink (pNhopEntry, pMacEntry);
     }
     else
     {
       /* No change in MAC address. Update nhop based on MAC */
       if (pNhopEntry->pMac->resolved == L7_TRUE)
       {
         pNhopEntry->flags |= BROAD_L3_NH_RESOLVED;
       }
       else
       {
         pNhopEntry->flags &= ~BROAD_L3_NH_RESOLVED;
       }
     }
  }

  /* For some reason, the VLAN ID in the HOST add command is not passed
   * correctly. Hosts on port based routing intfs have VLAN = 0!!.
   * So, don't cross check VLAN in DAPI cmd against the routing VLAN
   */

  /* Search the HOST tree for duplicate entries */
  pHostEntry = avlSearchLVL7(&hapiBroadL3HostTree, &host, AVL_EXACT);

  if (pHostEntry != L7_NULL)
  {
    /* Duplicate host already exist. */
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3HostEntryAdd: Duplicate"
                        " host add \n");

    /* Application shouldn't really add duplicate entries, unless a previous
     * host delete didn't get processed.
     */
    if (pHostEntry->wl.cmd == BROAD_L3_HOST_CMD_ADD)
    {
      HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
      return L7_FAILURE;
    }
    else if (pHostEntry->wl.cmd == BROAD_L3_HOST_CMD_NO_OP)
    {
      /* Treat the add as a modify operation! See notes of this function */

      pHostEntry->wl.cmd = BROAD_L3_HOST_CMD_MODIFY;
      HAPI_BROAD_L3_HOST_WLIST_ADD(pHostEntry);
    }
    else if (pHostEntry->wl.cmd == BROAD_L3_HOST_CMD_DELETE)
    {
      /* Here if the HOST was marked for deletion, but Async task didn't get a
       * chance to process the host delete. Treat this as modify
       */
      pHostEntry->wl.cmd = BROAD_L3_HOST_CMD_MODIFY;
    }
  }
  else
  {
    /* Host entry not found. Create it */
    pHostEntry = hapiBroadL3AvlInsert(&hapiBroadL3HostTree, &host);

    if (pHostEntry != L7_NULL)
    {
      pHostEntry->pNhop = pNhopEntry;
      pNhopEntry->ref_count++;

      if (dapiCmd->cmdData.arpAdd.flags & DAPI_ROUTING_ARP_LOCAL)
      {
        /* This host entry is one of our local IP address */
        pHostEntry->flags |= BROAD_L3_HOST_LOCAL;
        pNhopEntry->flags |= BROAD_L3_NH_LOCAL;
      }

      /* Enqueue the host to the work list */
      pHostEntry->wl.cmd = BROAD_L3_HOST_CMD_ADD;
      HAPI_BROAD_L3_HOST_WLIST_ADD(pHostEntry);
    }
    else
    {
      HAPI_BROAD_L3_LOG_ERROR(0);
    }

    /* If MAC resolves to tunnel, link the host to MAC. */
    if (pMacEntry->pTunnelEntry != L7_NULL)
    {
      pHostEntry->flags |= BROAD_L3_HOST_TUNNEL;
      pHostEntry->pTunNhop = pTunNhop;
      hapiBroadL3TunnelHostMacLink(pHostEntry, pMacEntry);
    }
  }

  /* Modify the next hop to resolve any routes dependent on this host */
  hapiBroadL3NhopEntryUpdate(pNhopEntry, BROAD_L3_NH_CMD_MODIFY);

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

  /* Wake up Async task to process the work lists */
  HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;

  return L7_SUCCESS;
}


/*******************************************************************************
*
* @purpose  Create a MAC AVL entry and try to resolve the MAC
*
* @param  DAPI_USP_t           usp
* @param  L7_enetMacAddr_t    *pMacAddr
* @param  dapi_g - DAPI driver object
*
* @returns L7_RC_t result
*
* @notes MAC entry is created if not found. A MAC entry can be associated with
*        multiple hosts. Caller must take L3 sema.
*
* @end
*
*******************************************************************************/
static BROAD_L3_MAC_ENTRY_t * hapiBroadL3MacEntryAdd(DAPI_USP_t *usp,
                                                     L7_enetMacAddr_t *pMacAddr,
                                                     DAPI_t *dapi_g)
{
  L7_BOOL      isVlanRouting = L7_FALSE;
  BROAD_L3_MAC_ENTRY_t  mac;
  BROAD_L3_MAC_ENTRY_t  *pMacEntry = L7_NULL;
  DAPI_PORT_t           *dapiPortPtr;
  BROAD_PORT_t          *hapiPortPtr;

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "*** hapiBroadL3MacEntryAdd ***\n");

  /* Search for the MAC entry */
  memset(&mac, 0, sizeof(BROAD_L3_MAC_ENTRY_t));
  memcpy(&mac.key.macAddr, pMacAddr, sizeof(bcm_mac_t));

  /* Get VLAN Id for the MAC from the next hop USP */
  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
  {
    if (dapiPortPtr->modeparm.physical.routerIntfEnabled != L7_TRUE)
    {
      HAPI_BROAD_L3_LOG_ERROR (0);
    }

    isVlanRouting = L7_FALSE;
    mac.key.vlanId = hapiPortPtr->port_based_routing_vlanid;
  }
  else if (IS_PORT_TYPE_LOGICAL_VLAN(dapiPortPtr) == L7_TRUE)
  {
    mac.key.vlanId = dapiPortPtr->modeparm.router.vlanID;
    isVlanRouting = L7_TRUE;
  }
  else if (IS_PORT_TYPE_CPU(dapiPortPtr) == L7_TRUE)
  {
    mac.key.vlanId =  HPC_STACKING_VLAN_ID;
    isVlanRouting = L7_FALSE;
  }
  else
  {
    HAPI_BROAD_L3_LOG_ERROR(dapiPortPtr->type);
  }

  pMacEntry = avlSearchLVL7(&hapiBroadL3MacTree, &mac, AVL_EXACT);
  if (pMacEntry == L7_NULL)
  {
    /* Entry not found, create it */
    pMacEntry = hapiBroadL3AvlInsert(&hapiBroadL3MacTree, &mac);
    if (pMacEntry != L7_NULL)
    {
      pMacEntry->nHopList = L7_NULL;
      pMacEntry->hostList = L7_NULL;
      pMacEntry->pTunnelEntry = L7_NULL;
      pMacEntry->vlan_routing_intf = isVlanRouting;
      pMacEntry->target_is_trunk = L7_FALSE;

      if (pMacEntry->vlan_routing_intf == L7_FALSE)
      {
        /* Treat all MACs on port based routing interface as resolved */
        pMacEntry->resolved = L7_TRUE;
      }
      else
      {
        /* For VLAN intf, next hop is resolved if L2 MAC entry is resolved */
        pMacEntry->resolved = L7_FALSE; /* Init to unresolved */
      }

      /*
       * Initially set the MAC usp of the nHop to the one in the key.
       * This will be what we use for port-based routing.  For VLANs, it
       * will be replaced by what is in the MAC Entry.
       */
      pMacEntry->usp = *usp;

      /* For VLAN routing intf, Get the MAC address info from driver. */

      if (L7_TRUE == pMacEntry->vlan_routing_intf)
      {
        hapiBroadL3MacEntryResolve(pMacEntry, dapi_g);
      }
    }
    else
    {
      HAPI_BROAD_L3_LOG_ERROR(0);
    }
  }

  return pMacEntry;
}


/*******************************************************************************
*
* @purpose  Links Next hop entry to a MAC entry
*
* @param  pNhopEntry - Nhop entry
* @param  pMacEntry - Mac entry
*
* @returns L7_RC_t result
*
* @notes A MAC entry can be associated with multiple hosts.
*        Caller must take L3 sema.
*
* @end
*
*******************************************************************************/
static L7_RC_t hapiBroadL3MacNhopLink (BROAD_L3_NH_ENTRY_t *pNhopEntry,
                                       BROAD_L3_MAC_ENTRY_t  *pMacEntry)
{
  /* NOTE: Caller must take hapiBroadL3Sema */

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "*** hapiBroadL3MacNhopLink ***\n");

  pNhopEntry->pMacNhopNext = pMacEntry->nHopList;
  pMacEntry->nHopList = pNhopEntry;
  pNhopEntry->pMac = pMacEntry;

  /* Update the next hop state based on whether MAC is resolved or not */
  if (pMacEntry->resolved == L7_TRUE)
  {
    pNhopEntry->flags |= BROAD_L3_NH_RESOLVED;
  }
  else
  {
    pNhopEntry->flags &= ~BROAD_L3_NH_RESOLVED;
  }

  return L7_SUCCESS;
}

/*******************************************************************************
*
* @purpose  Links Host entry to a L3 tunnel MAC entry
*
* @param  pHostEntry - Host entry
* @param  pMacEntry -  MAC entry
*
* @returns L7_RC_t result
*
* @notes This link is only required for hosts going over wireless tunnels.
*        Caller must take L3 sema.
*
* @end
*
*******************************************************************************/
static L7_RC_t hapiBroadL3TunnelHostMacLink (BROAD_L3_HOST_ENTRY_t *pHostEntry,
                                             BROAD_L3_MAC_ENTRY_t  *pMacEntry)

{
  BROAD_L3_TUNNEL_ENTRY_t *pTunEntry;

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "*** hapiBroadL3TunnelHostMacLink ***\n");

  pTunEntry = pMacEntry->pTunnelEntry;
  /* NOTE: Caller must take hapiBroadL3Sema */
  if (pTunEntry->key.tunnelMode != L7_TUNNEL_MODE_4OVER4)
  {
    return L7_FAILURE;
  }

  pHostEntry->pMacHostNext = pMacEntry->hostList;
  pMacEntry->hostList = pHostEntry;

  return L7_SUCCESS;
}


/*******************************************************************************
*
* @purpose  Unlinks Host entry from a L3 tunnel MAC entry
*
* @param  pHostEntry - Host entry
* @param  pMacEntry -  MAC entry
*
* @returns L7_RC_t result
*
* @notes The link exists only for hosts going over wireless tunnels.
*        Caller must take L3 sema.
*
* @end
*
*******************************************************************************/
static L7_RC_t hapiBroadL3TunnelHostMacUnlink (BROAD_L3_HOST_ENTRY_t *pHostEntry,
                                               BROAD_L3_MAC_ENTRY_t  *pMacEntry)

{
  BROAD_L3_TUNNEL_ENTRY_t *pTunEntry;
  BROAD_L3_HOST_ENTRY_t *pTmpHost, *pPrevHost;

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "*** hapiBroadL3TunnelHostMacUnlink ***\n");

  pTunEntry = pMacEntry->pTunnelEntry;
  /* NOTE: Caller must take hapiBroadL3Sema */

  pTmpHost = pMacEntry->hostList;
  if (pTmpHost == pHostEntry)
  {
    pMacEntry->hostList = pHostEntry->pMacHostNext;
    pHostEntry->pMacHostNext = L7_NULL;
  }
  else
  {
    while (pTmpHost != L7_NULL)
    {
      pPrevHost = pTmpHost;
      pTmpHost = pPrevHost->pMacHostNext;
      if (pTmpHost == pHostEntry)
      {
        pPrevHost->pMacHostNext = pHostEntry->pMacHostNext;
        pHostEntry->pMacHostNext = L7_NULL;
        break;
      }
    }

    if (pTmpHost == L7_NULL) /* Host not found */
    {
      HAPI_BROAD_L3_LOG_ERROR(0);
    }
  }

  return L7_SUCCESS;
}



/*******************************************************************************
*
* @purpose Resolve a given MAC entry by querying the hardware
*
* @param  pMac    MAC Entry
* @param *dapi_g  DAPI driver object
*
* @returns L7_RC_t result
*
* @notes  This function involves a BCM call and can block on SCHAN operation.
*         If done on application thread, then we are likely to block until
*         BCM call is completed. As, the BCM call is restricted to single unit
*         on manager, it doesn't involve RPC/ATP transaction (minimal perf hit).
*         The idea is to not to maintain all MAC addresses learnt in the system,
*         but query the driver whenever a host is added.
*
* @end
*
*******************************************************************************/
static L7_RC_t hapiBroadL3MacEntryResolve (BROAD_L3_MAC_ENTRY_t *pMac,
                                           DAPI_t *dapi_g)
{
  DAPI_USP_t   usp;
  L7_int32 rv = BCM_E_NOT_FOUND;
  L7_uint32 local_bcm_unit;
  bcmx_uport_t uport;
  bcm_l2_addr_t bcm_l2;
  bcmx_l2_addr_t bcmx_l2;

  /* Note: The BCMX call returns as soon as the L2 entry is found on one of the
   * units (local units first)
   */

  for (local_bcm_unit = 0;
       local_bcm_unit < bde->num_devices(BDE_SWITCH_DEVICES);
       local_bcm_unit++)
  {
    if (!SOC_IS_XGS_FABRIC(local_bcm_unit))
    {
      bcm_l2_addr_init(&bcm_l2, pMac->key.macAddr, pMac->key.vlanId);
      rv = bcm_l2_addr_get(local_bcm_unit, pMac->key.macAddr,
                           pMac->key.vlanId, &bcm_l2);
      if (BCM_SUCCESS(rv))
      {
        rv = bcmx_l2_addr_from_bcm(&bcmx_l2, L7_NULLPTR, &bcm_l2);
        break;
      }
    }
  }

  /* If address found, update mac entry/Nhop. Else, MAC/Nhop is unresolved */
  if (rv == BCM_E_NONE)
  {
    if (bcmx_l2.flags & BCM_L2_TRUNK_MEMBER)
    {
      if (hapiBroadTgidToUspConvert(bcmx_l2.tgid, &usp, dapi_g) == L7_SUCCESS)
      {
        pMac->usp = usp;
        pMac->resolved = L7_TRUE;
        pMac->target_is_trunk = L7_TRUE;
        pMac->trunkId = bcmx_l2.tgid;
      }
    }

    else if (BCM_GPORT_IS_WLAN_PORT(bcmx_l2.lport))
    {
#ifdef L7_WIRELESS_PACKAGE
      if (hapiBroadWlanUspGet(dapi_g, bcmx_l2.lport, &usp) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      if (isValidUsp(&usp,dapi_g) == L7_TRUE)
      {
        pMac->resolved = L7_TRUE;
        pMac->usp = usp;
      }
#endif
    }
    else
    {
      /* not a trunk, calculate based off of the present info */
      uport = BCMX_UPORT_GET(bcmx_l2.lport);

      if (uport != BCMX_UPORT_INVALID_DEFAULT)
      {
        HAPI_BROAD_UPORT_TO_USP(uport,&usp);
        if (isValidUsp(&usp,dapi_g) == L7_TRUE)
        {
          pMac->resolved = L7_TRUE;
          pMac->usp = usp;
        }
      }
    }
  }
  else
  {
    pMac->resolved = L7_FALSE;
  }

  return L7_SUCCESS;
}


/*******************************************************************************
*
* @purpose Modifies a Host in the tables
*
* @param  usp     unit/slot/port of the routing interface
* @param  cmd     DAPI_CMD_ROUTING_ARP_ENTRY_MODIFY or
*                 DAPI_CMD_IPV6_NEIGH_ENTRY_MODIFY
* @param  data    Data associated with host modify command
* @param *dapi_g  DAPI driver object
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
static L7_RC_t hapiBroadL3HostEntryModify(DAPI_USP_t *usp,
                                          DAPI_CMD_t cmd,
                                          void *data,
                                          DAPI_t *dapi_g)
{
  BROAD_L3_HOST_KEY_t    hostKey;
  BROAD_L3_HOST_ENTRY_t *pHostEntry;
  BROAD_L3_NH_ENTRY_t   *pNhopEntry;
  BROAD_L3_NH_ENTRY_t   *pTunNhop;
  BROAD_L3_MAC_ENTRY_t  *pMacEntry;
  L7_BOOL                hostUpdate = L7_FALSE;

  DAPI_ROUTING_ARP_CMD_t *dapiCmd = (DAPI_ROUTING_ARP_CMD_t*)data;

  hapiBroadL3DebugDapiCmdPrint(__FUNCTION__, usp, cmd, data);

  memset(&hostKey, 0, sizeof(BROAD_L3_HOST_KEY_t));
  hostKey.addrUsp.usp = *usp;

  switch (cmd)
  {
    case DAPI_CMD_ROUTING_ARP_ENTRY_MODIFY:
      /* Fill the key information */
      hostKey.family = L7_AF_INET;
      hostKey.addrUsp.addr.ipv4 = dapiCmd->cmdData.arpModify.ipAddr;
      break;

    case DAPI_CMD_IPV6_NEIGH_ENTRY_MODIFY:
      hostKey.family = L7_AF_INET6;
      memcpy(&hostKey.addrUsp.addr, &dapiCmd->cmdData.arpModify.ip6Addr,
             sizeof(bcm_ip6_t));
      break;

    default:
      HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3HostEntryModify: invalid"
                          " cmd");
      return L7_FAILURE;
  }

  /* check if there is room for NHOP and MAC entry */
  if (hapiBroadL3HostTableFull(BROAD_L3_HOST_CMD_MODIFY,
                               &hostKey,
                               &dapiCmd->cmdData.arpModify.macAddr,
                               dapi_g) == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID,
            "hapiBroadL3HostEntryModify: Nexthop/MAC table full.");
    return L7_FAILURE;
  }

  /* Search the host in the host tree */
  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  pHostEntry = avlSearchLVL7(&hapiBroadL3HostTree, &hostKey, AVL_EXACT);

  if (pHostEntry != L7_NULL)
  {
    /* Host found, check for pending operation */
    pNhopEntry = pHostEntry->pNhop;

    /* Update the next hop based on given host information */
    if ((dapiCmd->cmdData.arpModify.flags & DAPI_ROUTING_ARP_DEST_KNOWN) == 0)
    {
      if (pNhopEntry->flags & BROAD_L3_NH_RESOLVED)
      {
        /* Update the next hop as host is now unknown */
        HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3HostEntryModify:"
                            "Marking next hop as unresolved\n");
        pNhopEntry->flags &= ~BROAD_L3_NH_RESOLVED;

      }
    }
    else
    {
      if (pNhopEntry->pMac != L7_NULL)
      {
        if (memcmp(pNhopEntry->pMac->key.macAddr,
                   dapiCmd->cmdData.arpModify.macAddr.addr,
                   sizeof(bcm_mac_t)) != 0)
        {
          /* See if this host is going over a tunnel */
          if (pHostEntry->flags & BROAD_L3_HOST_TUNNEL)
          {
            /* Unlink this host from the MAC */
            hapiBroadL3TunnelHostMacUnlink(pHostEntry, pNhopEntry->pMac);
            /* Host entry needs to be reprogrammed */
            hostUpdate = L7_TRUE;

            pTunNhop = pHostEntry->pTunNhop;
            hapiBroadL3NhopEntryUpdate(pTunNhop, BROAD_L3_NH_CMD_DELETE);
            pHostEntry->flags &= ~BROAD_L3_HOST_TUNNEL;
          }

          /* Unlink the nhop from old MAC entry  */
          hapiBroadL3MacNhopUnlink(pNhopEntry);

          /* Add the new MAC entry to the AVL tree (if not exists) */
          pMacEntry = hapiBroadL3MacEntryAdd(usp,
                                             &(dapiCmd->cmdData.arpModify.macAddr),
                                             dapi_g);
          /* Link the new MAC entry to the next hop */
          hapiBroadL3MacNhopLink (pNhopEntry, pMacEntry);

          /* If the new MAC address resolves to a tunnel, link the host to MAC */
          if (pMacEntry->pTunnelEntry != L7_NULL)
          {
            hapiBroadL3TunnelHostMacLink(pHostEntry, pMacEntry);
            pTunNhop = pMacEntry->pTunnelEntry->pTunnelNhop;
            if (pTunNhop != L7_NULL)
            {
              pHostEntry->flags |= BROAD_L3_HOST_TUNNEL;
              pHostEntry->pTunNhop = pTunNhop;
              pTunNhop->ref_count++;
            }
            hostUpdate = L7_TRUE;
          }
        }
        else
        {
          /* Host mac-address has not changed */

          if (pNhopEntry->pMac->resolved == L7_TRUE)
          {
            pNhopEntry->flags |= BROAD_L3_NH_RESOLVED;
          }
          else
          {
            pNhopEntry->flags &= ~BROAD_L3_NH_RESOLVED;
          }
        }
      }
    }

    /* Modify the next hop for MAC address changes or other host changes */
    hapiBroadL3NhopEntryUpdate(pNhopEntry, BROAD_L3_NH_CMD_MODIFY);

    /* Check the host entry needs to reprogrammed. This is the case when host
     * moves from one tunnel to another tunnel
     */
    if (hostUpdate == L7_TRUE)
    {
      if (pHostEntry->wl.cmd == BROAD_L3_HOST_CMD_NO_OP)
      {
        /* Mark for modify */
        pHostEntry->wl.cmd = BROAD_L3_HOST_CMD_MODIFY;
        HAPI_BROAD_L3_HOST_WLIST_ADD(pHostEntry);
      }
    }
  }
  else
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3HostEntryModify: not found\n");
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

  HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;

  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Deletes a Host from the tables
*
* @param  usp     unit/slot/port of routing interface
* @param  cmd     DAPI_CMD_ROUTING_ARP_ENTRY_DELETE or
*                 DAPI_CMD_IPV6_NEIGH_ENTRY_DELETE
* @param  data    Data associated with host delete command.
* @param *dapi_g  DAPI driver object
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3HostEntryDelete(DAPI_USP_t *usp,
                                   DAPI_CMD_t cmd,
                                   void *data,
                                   DAPI_t *dapi_g)
{
  BROAD_L3_HOST_KEY_t    hostKey;
  BROAD_L3_HOST_ENTRY_t *pHostEntry;
  BROAD_L3_NH_ENTRY_t   *pNhopEntry;
  BROAD_L3_MAC_ENTRY_t  *pMacEntry;
  DAPI_ROUTING_ARP_CMD_t *dapiCmd = (DAPI_ROUTING_ARP_CMD_t*)data;


  if (dapiCmd->cmdData.arpDelete.getOrSet == DAPI_CMD_GET)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_HAPI_ERROR, "hapiBroadL3HostEntryDelete: Get"
                        " command not supported\n");
    return L7_FAILURE;
  }

  hapiBroadL3DebugDapiCmdPrint(__FUNCTION__, usp, cmd, data);

  hapiBroadL3DebugTaskIdSet();

  memset(&hostKey, 0, sizeof(BROAD_L3_HOST_KEY_t));
  hostKey.addrUsp.usp = *usp;

  switch (cmd)
  {
    case DAPI_CMD_ROUTING_ARP_ENTRY_DELETE:
      /* Fill the key information */
      hostKey.family = L7_AF_INET;
      hostKey.addrUsp.addr.ipv4 = dapiCmd->cmdData.arpDelete.ipAddr;
      break;

    case DAPI_CMD_IPV6_NEIGH_ENTRY_DELETE:
      hostKey.family = L7_AF_INET6;
      memcpy(&hostKey.addrUsp.addr, &dapiCmd->cmdData.arpDelete.ip6Addr,
             sizeof(bcm_ip6_t));
      break;

    default:
      HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3HostEntryDelete: invalid"
                          " cmd");
      return L7_FAILURE;
  }

  /* Search the host in the host tree */
  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  pHostEntry = avlSearchLVL7(&hapiBroadL3HostTree, &hostKey, AVL_EXACT);

  if (pHostEntry != L7_NULL)
  {
    pNhopEntry = pHostEntry->pNhop;
    /* If host goes over tunnel, unlink this host from MAC */
    pMacEntry = pNhopEntry->pMac;
    if (pHostEntry->flags & BROAD_L3_HOST_TUNNEL)
    {
      hapiBroadL3TunnelHostMacUnlink(pHostEntry, pMacEntry);
    }

    /* Host found, check if any pending operation */
    if (pHostEntry->wl.cmd != BROAD_L3_HOST_CMD_ADD)
    {
      /* Mark for deletion, if there is no command or a modify is pending */
      if (pHostEntry->wl.cmd == BROAD_L3_HOST_CMD_NO_OP)
      {
        pHostEntry->wl.cmd = BROAD_L3_HOST_CMD_DELETE;
        HAPI_BROAD_L3_HOST_WLIST_ADD(pHostEntry);
      }
      else
      {
        /* Modify pending, just change the command. Do not queue */
        pHostEntry->wl.cmd = BROAD_L3_HOST_CMD_DELETE;
      }

      /* Note: The next hop objects should be deleted only on the Async side
       * except for the special case below
       */

      /* Mark the Next hop as unresolved */
      pNhopEntry->flags &= ~BROAD_L3_NH_RESOLVED;

      /* Also unlink the nexthop and MAC, so that a learn callback doesn't
       * update the next hop as resolved.
       */
      hapiBroadL3MacNhopUnlink(pNhopEntry);
    }
    else /* Host Add is still pending */
    {
      HAPI_BROAD_L3_HOST_WLIST_DEL(pHostEntry);

      if (avlDeleteEntry(&hapiBroadL3HostTree, pHostEntry) == L7_NULL)
      {
        HAPI_BROAD_L3_LOG_ERROR(0);
      }

      hapiBroadL3MacNhopUnlink(pNhopEntry);

      /* Special case: Delete the Next hop if not being used */
      hapiBroadL3NhopEntryUpdate(pNhopEntry, BROAD_L3_NH_CMD_DELETE);
    }

    HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

    /* Wake up Async task to process the work lists */
    HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;
  }
  else
  {
    /* Host is not found.*/
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3HostEntryDelete: not found\n");
    HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
  }

  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Deletes all the IPv4 Host entries from the tables
*
* @param  usp     unit/slot/port of routing interface
* @param  cmd     DAPI_CMD_ROUTING_ARP_ENTRY_DELETE_ALL
* @param  data    Data associated with host delete command.
* @param *dapi_g  DAPI driver object
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t
hapiBroadL3HostEntryDeleteAll (DAPI_USP_t *usp,
                               DAPI_CMD_t cmd,
                               void *data,
                               DAPI_t *dapi_g)
{
  BROAD_L3_HOST_KEY_t    hostKey;
  BROAD_L3_HOST_ENTRY_t *pHostEntry = L7_NULLPTR;
  L7_ushort16 count = 0;

  hapiBroadL3DebugDapiCmdPrint(__FUNCTION__, usp, cmd, data);

  hapiBroadL3DebugTaskIdSet();

  if (hapiBroadL3HostTree.count > 0)
  {
    HAPI_BROAD_L3_DEBUG (broadL3Debug, "hapiBroadL3HostEntryDeleteAll: Deleting "
                         "only IPv4 Host entries among %d Host entries\n",
                         hapiBroadL3HostTree.count);

    count = 0;
    memset (&hostKey, 0, sizeof(hostKey));

    /* Search the host in the host tree */
    HAPI_BROAD_L3_SEMA_TAKE (hapiBroadL3Sema, L7_WAIT_FOREVER);
    pHostEntry = avlSearchLVL7 (&hapiBroadL3HostTree, &hostKey, AVL_NEXT);
    while (pHostEntry != L7_NULLPTR)
    {
      if (pHostEntry->key.family == L7_AF_INET)
      {
        if (pHostEntry->wl.cmd == BROAD_L3_HOST_CMD_NO_OP)
        {
          pHostEntry->wl.cmd = BROAD_L3_HOST_CMD_DELETE;
          HAPI_BROAD_L3_HOST_WLIST_ADD (pHostEntry);
          count++;
        }
        else
        {
          pHostEntry->wl.cmd = BROAD_L3_HOST_CMD_DELETE;
        }
      }
      pHostEntry = avlSearchLVL7 (&hapiBroadL3HostTree, pHostEntry, AVL_NEXT);
    }
    HAPI_BROAD_L3_SEMA_GIVE (hapiBroadL3Sema);

    if (count > 0)
    {
      HAPI_BROAD_L3_DEBUG (broadL3Debug, "hapiBroadL3HostEntryDeleteAll: "
                           "Queued %d hosts for for deletion\n", count);
      /* Wake up Async task to process the work lists */
      HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;
    }
  }

  return L7_SUCCESS;
}


/*******************************************************************************
*
* @purpose Queries the host tables for use
*
* @param  usp       unit/slot/port of routing interface
* @param  cmd       DAPI_CMD_ROUTING_ARP_ENTRY_QUERY (for IPv4)
*                   DAPI_CMD_IPV6_NEIGH_ENTRY_QUERY (for IPv6)
* @param  *data     Command data
* @param *dapi_g    DAPI Driver object
*
* @returns L7_RC_t result
*
* @notes This routine will return several flags indicating whether the
*        entry is known, valid, and has been seen on source or destination
*        of frames flowing through the system.  The return code will be
*        L7_SUCCESS if there are no prarmeter or hardware errors, and
*        L7_FAILURE if the parameters are clearly wrong.
*
* @end
*
*******************************************************************************/
static L7_RC_t hapiBroadL3HostEntryQuery(DAPI_USP_t *usp,
                                         DAPI_CMD_t cmd,
                                         void *data,
                                         DAPI_t *dapi_g)
{
  L7_ulong32 entry;                 /* curr. entry (DAPI_ROUTING_ARP_CMD_t) */
  L7_ulong32 family;                /* address family (BROAD_HOST_KEY_t) */
  L7_RC_t result;                   /* working result */
  DAPI_ROUTING_ARP_CMD_t *dapiCmd;  /* pointer to working command data*/
#ifdef L7_STACKING_PACKAGE
  L7_BOOL inHw;                     /* did USL know about the entry? */
  L7_uint32 hwFlags;                /* hardware flags from USL copy */
#else /* def L7_STACKING_PACKAGE */
  int rv;                           /* result buffer for broadcom queries */
  BROAD_L3_HOST_ENTRY_t host;       /* host search buffer */
  BROAD_L3_HOST_ENTRY_t *pHostEntry;/* pointer to located host entry */
  bcmx_l3_host_t hostInfo;          /* host query buffer */
  bcm_if_t theIf;                   /* interface for this host */
  L7_uint32 currUpTime;             /* current time; used as last hit time */
  L7_BOOL notInHw;                  /* indicates error submitting to hw */
#endif /* def L7_STACKING_PACKAGE */

  /* hope for the best, get the hosts to query, and get the current time */
  result = L7_SUCCESS;
  dapiCmd = (DAPI_ROUTING_ARP_CMD_t*)data;

#ifndef L7_STACKING_PACKAGE
  currUpTime = osapiUpTimeRaw();
#endif /* ndef L7_STACKING_PACKAGE */

  HAPI_BROAD_L3_DEBUG(broadL3ArpDebug, "*** hapiBroadL3HostEntryQuery begin ***");

  if (dapiCmd->cmdData.arpQuery.getOrSet ==  DAPI_CMD_SET)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: In %s Set not supported for this command\n",
                   __FILE__, __LINE__, __FUNCTION__);

    return L7_FAILURE;
  }

  if (dapiCmd->cmdData.arpQuery.numOfEntries > L7_IP_ARP_MAX_QUERY)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_HAPI_ERROR,
                  "%s %d: In %s Cannot query the number of specified hosts\n",
                  __FILE__, __LINE__, __FUNCTION__);

    return L7_FAILURE;
  }

  /* derive address family while being sure the command is valid */
  /* note that this applies to the *entire* query; it's not one per addr */
  switch (cmd)
  {
    case DAPI_CMD_ROUTING_ARP_ENTRY_QUERY:
      family = L7_AF_INET;
      break;

    case DAPI_CMD_IPV6_NEIGH_ENTRY_QUERY:
      family = L7_AF_INET6;
      break;

    default:
      HAPI_BROAD_L3_DEBUG(broadL3ArpDebug, "*** hapiBroadL3HostEntryQuery"
                          "Invalid cmd");
      return L7_FAILURE;
  }

  /*
   * For each entry queried, check on the status of the host.
   * In case of single, just query the hardware and parse the result.
   * In case of stack, ask USL to do the work (since USL has it cached).
   */
  for (entry=0; entry<dapiCmd->cmdData.arpQuery.numOfEntries; entry++)
  {
    dapiCmd->cmdData.arpQuery.arpQueryFlags[entry] = DAPI_ROUTING_ARP_FLAGS_ZERO;

    /* Link-local Ipv6 addresses are not added to hardware, because we shouldn't
     * forward v6 traffic destined to link-local. If there is no h/w entry, then
     * there are no hit bits associated. Just say that entry is in s/w and h/w.
     * We don't need link-locals if there is no route associated with them.
     * Note: If a v6 route has link-local nexthop, then that link-local nexthop
     * becomes gateway and all gateways are always renewed and will NOT be
     * queried.
     */
    if ((L7_AF_INET6 == family) &&
        (L7_IP6_IS_ADDR_LINK_LOCAL(&(dapiCmd->cmdData.arpQuery.ip6Addr[entry]))))
    {
      dapiCmd->cmdData.arpQuery.arpQueryFlags[entry] |= (DAPI_ROUTING_ARP_ENTRY_IN_SW |
                                                         DAPI_ROUTING_ARP_ENTRY_IN_HW);
      continue;
    }

#ifdef L7_STACKING_PACKAGE
    /* Just ask the USL for its cached value */
    if (L7_AF_INET6 == family)
    {
      (void)usl_bcmx_l3host_hit_flags_get(family,
                                          (L7_uint8*)(&(dapiCmd->cmdData.arpQuery.ip6Addr[entry])),
                                          &(hwFlags),
                                          &(dapiCmd->cmdData.arpQuery.lastHitDst[entry]),
                                          &(dapiCmd->cmdData.arpQuery.lastHitSrc[entry]), &inHw);

    }
    else
    {
      (void) usl_bcmx_l3host_hit_flags_get(family,
                                           (L7_uint8*)(&(dapiCmd->cmdData.arpQuery.ipAddr[entry])),
                                           &(hwFlags),
                                           &(dapiCmd->cmdData.arpQuery.lastHitDst[entry]),
                                           &(dapiCmd->cmdData.arpQuery.lastHitSrc[entry]), &inHw);
    } /* end if (L7_AF_INET6 == family) */

    if (L7_TRUE == inHw)
    {
      /* host was found; set _IN_ flags */
      dapiCmd->cmdData.arpQuery.arpQueryFlags[entry] |= (DAPI_ROUTING_ARP_ENTRY_IN_SW |
                                                         DAPI_ROUTING_ARP_ENTRY_IN_HW);
      /* also set source hit and dest hit if appropriate */
      if (0 != (hwFlags & BCM_L3_S_HIT))
      {
        /* source hit; mark that -- means it is sending to others */
        dapiCmd->cmdData.arpQuery.arpQueryFlags[entry] |= DAPI_ROUTING_ARP_HIT_SOURCE;
      }
      if (0 != (hwFlags & BCM_L3_D_HIT))
      {
        /* destination hit; mark that -- means others are sending to it */
        dapiCmd->cmdData.arpQuery.arpQueryFlags[entry] |= DAPI_ROUTING_ARP_HIT_DEST;
      }
    }
    else
    {
      /* host was not found, so set _ERR_HW flag */
      dapiCmd->cmdData.arpQuery.arpQueryFlags[entry] |= DAPI_ROUTING_ARP_ENTRY_ERR_HW;
      /*
       * we don't know whether it was in the software table, as that was not
       * checked, so don't set _IN_SW or _IN_HW (which is derived from a flag
       * in the software table).  These could also be checked, but that means
       * another lookup and the associated drawbacks, so we don't do it.
       */
    } /* end if (L7_TRUE == inHw) */

#else /* def L7_STACKING_PACKAGE */
    /* Look in our table first */
    memset(&host, 0, sizeof (BROAD_L3_HOST_ENTRY_t));
    host.key.family = family;

    if (L7_AF_INET6 == family)
    {
      memcpy(&host.key.addrUsp.addr.ipv6,
             &(dapiCmd->cmdData.arpQuery.ip6Addr[entry]),
             sizeof (bcm_ip6_t));
    }
    else
    {
      host.key.addrUsp.addr.ipv4 = dapiCmd->cmdData.arpQuery.ipAddr[entry];
    }

    host.key.addrUsp.usp = *usp;

    /* lock the table, do the lookup */
    /* table lock is maintained for the shortest period possible */
    HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);
    pHostEntry = avlSearchLVL7(&hapiBroadL3HostTree, &host, AVL_EXACT);

    if (L7_NULL != pHostEntry)
    {
      /* the host exists; copy data we need, release lock, mark it found. */
      theIf = pHostEntry->pNhop->l3_intf_id;
      notInHw = pHostEntry->hwError;
      HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
      dapiCmd->cmdData.arpQuery.arpQueryFlags[entry] |= DAPI_ROUTING_ARP_ENTRY_IN_SW;

      if (L7_FALSE == notInHw)
      {
        /* the host should also exist in the hardware; mark it */
        dapiCmd->cmdData.arpQuery.arpQueryFlags[entry] |= DAPI_ROUTING_ARP_ENTRY_IN_HW;
        memset(&hostInfo, 0, sizeof(hostInfo));

        if (L7_AF_INET6 == family)
        {
          memcpy(&(hostInfo.l3a_ip_addr),
                 &(dapiCmd->cmdData.arpQuery.ip6Addr[entry]),
                 sizeof(dapiCmd->cmdData.arpQuery.ip6Addr[entry]));
          hostInfo.l3a_flags = BCM_L3_IP6 | BCM_L3_HIT_CLEAR;
        }
        else
        {
          memcpy(&(hostInfo.l3a_ip_addr),
                 &(dapiCmd->cmdData.arpQuery.ipAddr[entry]),
                 sizeof(dapiCmd->cmdData.arpQuery.ipAddr[entry]));
          hostInfo.l3a_flags = BCM_L3_HIT_CLEAR;
        }

        hostInfo.l3a_intf = theIf;

        /* query the hardware for this host */
        rv = bcmx_l3_host_find(&hostInfo);
        if ( L7_TRUE == L7_BCMX_OK(rv) )
        {
          /* found it; set the flags & time for source hit and dest hit */
          if (0 != (hostInfo.l3a_flags & BCM_L3_S_HIT))
          {
            /* source hit; mark that -- means it is sending to others */
            dapiCmd->cmdData.arpQuery.arpQueryFlags[entry] |= DAPI_ROUTING_ARP_HIT_SOURCE;
            dapiCmd->cmdData.arpQuery.lastHitSrc[entry] = currUpTime;
          }
          if (0 != (hostInfo.l3a_flags & BCM_L3_D_HIT))
          {
            /* destination hit; mark that -- means others are sending to it */
            dapiCmd->cmdData.arpQuery.arpQueryFlags[entry] |= DAPI_ROUTING_ARP_HIT_DEST;
            dapiCmd->cmdData.arpQuery.lastHitDst[entry] = currUpTime;
          }
        }
        else /* if ( L7_TRUE == L7_BCMX_OK(rv) ) */
        {
          dapiCmd->cmdData.arpQuery.arpQueryFlags[entry] |= DAPI_ROUTING_ARP_ENTRY_ERR_HW;
        } /* if ( L7_TRUE == L7_BCMX_OK(rv) ) */
      } /* if (L7_FALSE == notInHw) */
    }
    else /* if (L7_NULL != pHostEntry) */
    {
      /* this host does not exist; release semaphore & continue with next */
      HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
    } /* if (L7_NULL != pHostEntry) */

#endif /* def L7_STACKING_PACKAGE */
  } /* for (entry=0;entry<dapiCmd->cmdData.arpQuery.numOfEntries;entry++) */

  hapiBroadL3DebugArpQueryPrint(usp, cmd, data);

  HAPI_BROAD_L3_DEBUG(broadL3ArpDebug, "*** hapiBroadL3HostEntryQuery end ***");
  return result;
}


/*******************************************************************************
*
* @purpose Creates L3 tunnel
*
* @param  usp     unit/slot/port - used for IPv6 tunnels only
* @param  cmd     DAPI_CMD_L3_TUNNEL_ADD & DAPI_CMD_TUNNEL_CREATE
* @param  data    Data associated with tunnel create command
* @param *dapi_g  DAPI driver object
*
* @returns L7_RC_t result
*
* @notes This function handles both 6o4 and 4o4 tunnels. Although the DAPI cmds
*        for 6o4 and 4o4 tunnels are different, the SDK tunnel APIs are same.
*        Again, 6o4 tunnels have USP/interfaces and are managed by RLIM module,
*        whereas 4o4 tunnels do not have USP/interfaces and are typically used
*        by Wireless component.
*
* @end
*
*******************************************************************************/
static L7_RC_t hapiBroadL3TunnelCreate(DAPI_USP_t *usp,
                                       DAPI_CMD_t cmd,
                                       void *data,
                                       DAPI_t *dapi_g)
{
  L7_uint8  index;
  DAPI_TUNNEL_MGMT_CMD_t *dapiCmd = (DAPI_TUNNEL_MGMT_CMD_t*)data;
  BROAD_L3_TUNNEL_ENTRY_t tunnel;
  BROAD_L3_TUNNEL_ENTRY_t *pTunnelEntry = L7_NULL;
  L7_sockaddr_union_t nextHopAddr;
  DAPI_USP_t          nextHopUsp;
  DAPI_PORT_t        *nhDapiPortPtr;
  BROAD_PORT_t       *tnlHapiPortPtr = L7_NULL;

  hapiBroadL3DebugDapiCmdPrint (__FUNCTION__, usp, cmd, data);

  memset(&tunnel, 0, sizeof (BROAD_L3_TUNNEL_ENTRY_t));
  for (index=0; index < HAPI_BROAD_L3_MAX_6TO4_NHOP; index++)
  {
    tunnel.tun6to4Nhops[index].relayTunnelId = HAPI_BROAD_INVALID_L3_INTF_ID;
  }

  /* Note:  The current implementation doesn't support multiple tunnels of the
   * same type and same IPv4 address set(local & remote).
   *
   * Tunnel key includes local/remote addresses and tunnel mode. There is
   * no obvious advantage with multiple tunnels having identical local/remote
   * address/tunnel modes and hence not allowed. For example, if parallel 6o4
   * tunnels are configured between a local and remote endpoint, v6 packets will
   * follow the same path [no load balancing benefit].
   */
  switch (cmd)
  {
    case DAPI_CMD_TUNNEL_CREATE:  /* RLIM tunnels 6o4, 6to4 */
      tunnel.key.tunnelMode = dapiCmd->cmdData.tunnelCreate.tunnelMode;
      tunnel.key.localAddr = dapiCmd->cmdData.tunnelCreate.localAddr;
      tunnel.key.remoteAddr = dapiCmd->cmdData.tunnelCreate.remoteAddr;
      nextHopAddr = dapiCmd->cmdData.tunnelCreate.nextHopAddr;
      nextHopUsp = dapiCmd->cmdData.tunnelCreate.nextHopUsp;

      if ((tunnel.key.tunnelMode != L7_TUNNEL_MODE_6OVER4) &&
          (tunnel.key.tunnelMode != L7_TUNNEL_MODE_6TO4))
      {
        HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3TunnelCreate: Invalid"
                            "Tunnel type %d\n", tunnel.key.tunnelMode);
        return L7_FAILURE;
      }

      /* This is tunnel USP and is valid for 6o4 and 6to4 tunnels only */
      tnlHapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

      break;

    case DAPI_CMD_L3_TUNNEL_ADD: /* Wireless tunnels 4o4 */
      tunnel.key.tunnelMode = dapiCmd->cmdData.l3TunnelAddDelete.tunnelMode;
      tunnel.key.localAddr = dapiCmd->cmdData.l3TunnelAddDelete.localAddr;
      tunnel.key.remoteAddr = dapiCmd->cmdData.l3TunnelAddDelete.remoteAddr;
      nextHopAddr = dapiCmd->cmdData.l3TunnelAddDelete.nextHopAddr;
      nextHopUsp = dapiCmd->cmdData.l3TunnelAddDelete.nextHopUsp;

      if (tunnel.key.tunnelMode != L7_TUNNEL_MODE_4OVER4)
      {
        HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3TunnelCreate: Invalid"
                            "Tunnel type %d\n", tunnel.key.tunnelMode);
        return L7_FAILURE;
      }

      break;

    default:
      HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3TunnelCreate: Invalid cmd\n");
      return L7_FAILURE;
  }

  /* Validate paramters */
  if (tunnel.key.localAddr.u.sa.sa_family != tunnel.key.remoteAddr.u.sa.sa_family ||
      tunnel.key.remoteAddr.u.sa.sa_family != nextHopAddr.u.sa.sa_family)
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug,"Address families are not consistent\n");
    return L7_FAILURE;
  }

  if (((tunnel.key.tunnelMode == L7_TUNNEL_MODE_6OVER4) ||
       (tunnel.key.tunnelMode == L7_TUNNEL_MODE_6TO4) ||
       (tunnel.key.tunnelMode == L7_TUNNEL_MODE_4OVER4)) &&
      tunnel.key.localAddr.u.sa.sa_family != L7_AF_INET)
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug,"Mode and Addr family not consistent\n");
    return L7_FAILURE;
  }

  /* Verify that the transport nexthop is a physical or vlan interface. */
  nhDapiPortPtr = DAPI_PORT_GET(&nextHopUsp, dapi_g);
  if (IS_PORT_TYPE_PHYSICAL(nhDapiPortPtr) == L7_FALSE &&
      IS_PORT_TYPE_LOGICAL_VLAN(nhDapiPortPtr) == L7_FALSE)
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "Invalid tunnel nexthop %u:%u:%u\n",
                        nextHopUsp.unit, nextHopUsp.slot, nextHopUsp.port);

    return L7_FAILURE;
  }

  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  /* Search Tunnel entry in the Tunnel AVL tree */
  pTunnelEntry = avlSearchLVL7(&hapiBroadL3TunnelTree, &tunnel, AVL_EXACT);
  if (pTunnelEntry != (BROAD_L3_TUNNEL_ENTRY_t *)L7_NULL)
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3TunnelCreate: Duplicate tunnel");

    /* Check whether the tunnel was already created!! */
    if (pTunnelEntry->wl.cmd == BROAD_L3_TUNNEL_CMD_NO_OP)
    {
      HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
      return L7_FAILURE;
    }

    /* Else, there might be a pending delete on this tunnel */
    if (pTunnelEntry->pXportNhop != NULL)
    {
      hapiBroadL3NhopEntryUpdate(pTunnelEntry->pXportNhop, BROAD_L3_NH_CMD_DELETE);
    }

    /* Treat the create as a modify. */
    pTunnelEntry->wl.cmd = BROAD_L3_TUNNEL_CMD_MODIFY;
  }
  else
  {
    pTunnelEntry = hapiBroadL3AvlInsert(&hapiBroadL3TunnelTree, &tunnel);
    if (pTunnelEntry == (BROAD_L3_TUNNEL_ENTRY_t *)L7_NULL)
    {
      /* Table full !!! */
      HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
      return L7_FAILURE;
    }
    else
    {
      pTunnelEntry->bcmL3IntfId = HAPI_BROAD_INVALID_L3_INTF_ID;
      pTunnelEntry->hasTerminator = L7_FALSE;
      pTunnelEntry->hasInitiator = L7_FALSE;
      pTunnelEntry->pXportNhop = L7_NULL;
      pTunnelEntry->pTunnelNhop = L7_NULL;

      pTunnelEntry->nextHopAddr = nextHopAddr;
      pTunnelEntry->nextHopUsp = nextHopUsp;

      pTunnelEntry->wl.cmd = BROAD_L3_TUNNEL_CMD_CREATE;
      HAPI_BROAD_L3_TUNNEL_WLIST_ADD(pTunnelEntry);

      if (((pTunnelEntry->key.tunnelMode == L7_TUNNEL_MODE_6OVER4) ||
           (pTunnelEntry->key.tunnelMode == L7_TUNNEL_MODE_6TO4)) &&
          (tnlHapiPortPtr != L7_NULL))
      {
        /* For v6 tunnels, store the pointer to AVL node */
        tnlHapiPortPtr->hapiModeparm.pTunnel = pTunnelEntry;
      }
    }
  }

  /* Process tunnel next hop */
  if (pTunnelEntry != (BROAD_L3_TUNNEL_ENTRY_t *)L7_NULL)
  {
    hapiBroadL3TunnelNhopProcess(usp, pTunnelEntry, dapi_g);
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
  /* Wake up Async task to process the work lists */
  HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;

  return L7_SUCCESS;
}

/*******************************************************************************
*
* @purpose Process the nexthop associated with tunnel
*
* @param  pTunEntry   Pointer to tunnel entry
* @param  dapi_g      Pointer to DAPI struct
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
static L7_RC_t hapiBroadL3TunnelNhopProcess(DAPI_USP_t *usp,
                                            BROAD_L3_TUNNEL_ENTRY_t *pTunEntry,
                                            DAPI_t  *dapi_g)
{
  BROAD_L3_NH_KEY_t     nhKey;
  BROAD_L3_NH_ENTRY_t  *pNhopEntry;

  if (pTunEntry->key.tunnelMode == L7_TUNNEL_MODE_6TO4)
  {
    return L7_FAILURE; /* For 6to4, tunnel has multiple nhops which are known
                        * only when routes are added (not known when tunnel is
                        * created).
                        */
  }

  memset(&nhKey, 0, sizeof (BROAD_L3_NH_KEY_t));

  /* Search the next hop tree with the given information */
  nhKey.family = pTunEntry->nextHopAddr.u.sa.sa_family;
  if (L7_AF_INET6 == nhKey.family)
  {
    /* v6 nexthops, not supported as of now. maybe for 6o6 tunnels! */
    return L7_FAILURE;
  }
  else
  {
    nhKey.addrUsp.addr.ipv4  = pTunEntry->nextHopAddr.u.sa4.sin_addr.s_addr;
  }

  nhKey.addrUsp.usp = pTunEntry->nextHopUsp;

  /* Search the next hop tree to see if the v4 NH already exists */
  pNhopEntry = avlSearchLVL7(&hapiBroadL3NhopTree, &nhKey, AVL_EXACT);
  if (pNhopEntry == L7_NULL)
  {
    /* Create the next hop entry */
    pNhopEntry = hapiBroadL3NhopEntryAdd(&nhKey, dapi_g);
  }
  else
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3TunnelNhopProcess: Reusing"
                        " next hop %d\n", pNhopEntry->egressId);
    hapiBroadL3NhopEntryUpdate(pNhopEntry, BROAD_L3_NH_CMD_ADD);
  }

  pTunEntry->pXportNhop = pNhopEntry;
  pNhopEntry->ref_count++;

  /* Create a next hop for routes/hosts going over tunnel. */
  if ((pTunEntry->key.tunnelMode == L7_TUNNEL_MODE_6OVER4) &&
      (pTunEntry->pTunnelNhop == L7_NULL))
  {
    memset(&nhKey, 0, sizeof (BROAD_L3_NH_KEY_t));
    /* Use tunnel USP, not nexthop USP */
    nhKey.addrUsp.usp = *usp;
    /* Use Local address of the tunnel. It doesn't really matter what address
     * we use, as this egress obj takes tunnel's v4 nexthop (xport) attributes
     */
    nhKey.family = pTunEntry->key.localAddr.u.sa.sa_family;
    nhKey.addrUsp.addr.ipv4  = pTunEntry->key.localAddr.u.sa4.sin_addr.s_addr;

    pNhopEntry = avlSearchLVL7(&hapiBroadL3NhopTree, &nhKey, AVL_EXACT);
    if (pNhopEntry == L7_NULL)
    {
      pNhopEntry = hapiBroadL3NhopEntryAdd(&nhKey, dapi_g);
    }
    else
    {
      hapiBroadL3NhopEntryUpdate(pNhopEntry, BROAD_L3_NH_CMD_ADD);
    }

    pNhopEntry->ref_count++;
    pNhopEntry->flags |= BROAD_L3_NH_TUNNEL;
    pTunEntry->pTunnelNhop = pNhopEntry;
  }
  else if ((pTunEntry->key.tunnelMode == L7_TUNNEL_MODE_4OVER4) &&
          (pTunEntry->pTunnelNhop == L7_NULL))
  {
    /* Wireless tunnels do not have USP. Use CPU USP to create a next hop
     * for all the wireless clients on this tunnel. This USP is just a place
     * holder.
     */
    memset(&nhKey, 0, sizeof (BROAD_L3_NH_KEY_t));
    if (CPU_USP_GET(&nhKey.addrUsp.usp) != L7_SUCCESS)
    {
      HAPI_BROAD_L3_LOG_ERROR(0);
    }

    /* Use remote address of tunnel, so that each tunnel has a unique key */
    nhKey.family = pTunEntry->key.remoteAddr.u.sa.sa_family;
    nhKey.addrUsp.addr.ipv4 = pTunEntry->key.remoteAddr.u.sa4.sin_addr.s_addr;

    pNhopEntry = avlSearchLVL7(&hapiBroadL3NhopTree, &nhKey, AVL_EXACT);
    if (pNhopEntry == L7_NULL)
    {
      pNhopEntry = hapiBroadL3NhopEntryAdd(&nhKey, dapi_g);
    }
    else
    {
      hapiBroadL3NhopEntryUpdate(pNhopEntry, BROAD_L3_NH_CMD_ADD);
    }

    pNhopEntry->ref_count++;
    pNhopEntry->flags |= BROAD_L3_NH_TUNNEL;
    pTunEntry->pTunnelNhop = pNhopEntry;
  }

  return L7_SUCCESS;
}


/*******************************************************************************
*
* @purpose Get the next hop associated with L3 tunnel
*
* @param  pTunNhopKey - Next hop key information
*
* @returns BROAD_L3_NH_ENTRY_t - pointer to tunnel next hop
*
* @notes For 6over4 configured tunnels, only one next hop exists (one-to-one)
*        For 6to4 automatic tunnels, more than one next hop can be present
*        (one-to many tunnel)
*
* @end
*
*******************************************************************************/
static BROAD_L3_NH_ENTRY_t *
hapiBroadL3TunnelNhopGet(BROAD_L3_NH_KEY_t *pTunNhopKey, DAPI_t * dapi_g)
{
  L7_uint8  index;
  BROAD_L3_NH_KEY_t   nHopKey;
  BROAD_L3_NH_ENTRY_t * pNhopEntry = (BROAD_L3_NH_ENTRY_t *)L7_NULL;
  BROAD_L3_ROUTE_KEY_t routeKey;
  BROAD_L3_ROUTE_ENTRY_t *pRouteEntry;
  BROAD_L3_TUNNEL_ENTRY_t *pTunnelEntry;
  BROAD_PORT_t  *nhHapiPortPtr;
  BROAD_L3_6TO4_NHOP_t *p6to4 = (BROAD_L3_6TO4_NHOP_t *)L7_NULL;
  bcm_ip_t      v4Addr;

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "*** hapiBroadL3TunnelNhopGet ***\n");

  memset(&nHopKey, 0, sizeof(BROAD_L3_NH_KEY_t));
  memset(&routeKey, 0, sizeof(BROAD_L3_ROUTE_KEY_t));

  /* Valid only 6over4 and 6to4 tunnels */

  /* Route's next hop is a tunnel, get the tunnel pointer */
  if (IS_USP_TYPE_TUNNEL(&pTunNhopKey->addrUsp.usp))
  {
    nhHapiPortPtr = HAPI_PORT_GET(&pTunNhopKey->addrUsp.usp, dapi_g);
    pTunnelEntry = (BROAD_L3_TUNNEL_ENTRY_t *)nhHapiPortPtr->hapiModeparm.pTunnel;

    if (pTunnelEntry == (BROAD_L3_TUNNEL_ENTRY_t *)L7_NULL)
    {
      HAPI_BROAD_L3_LOG_ERROR(0);
    }

    /* For 6over4 tunnels, use the tunnel next hop (only one nhop for 6o4) */
    if (pTunnelEntry->key.tunnelMode == L7_TUNNEL_MODE_6OVER4)
    {
      pNhopEntry = pTunnelEntry->pTunnelNhop; /* Got to be valid */
      if (pNhopEntry == L7_NULL)
      {
        HAPI_BROAD_L3_LOG_ERROR(0);
      }
      return pNhopEntry;
    }
    else if (pTunnelEntry->key.tunnelMode == L7_TUNNEL_MODE_6TO4)
    {
      /* Find the tunnel next hop. If not found, create it */
      for (index=0; index < HAPI_BROAD_L3_MAX_6TO4_NHOP; index++)
      {
        pNhopEntry = pTunnelEntry->tun6to4Nhops[index].pV6Nhop;
        if (pNhopEntry != L7_NULL)
        {
          if (memcmp(&pTunNhopKey->addrUsp.addr.ipv6,
                     pNhopEntry->key.addrUsp.addr.ipv6, sizeof(bcm_ip6_t)) == 0)
          {
            HAPI_BROAD_L3_DEBUG(broadL3Debug, "6to4 nexthop already exists\n");
            return pNhopEntry;
          }
        }
        else if (p6to4 == L7_NULL) /* Note an unused entry */
        {
          p6to4 = &pTunnelEntry->tun6to4Nhops[index];
        }
      }

      /* Reached max 6to4 nhop limit */
      if (p6to4 == (BROAD_L3_6TO4_NHOP_t *)L7_NULL)
      {
        return L7_NULL;
      }

      /* Here if a tunnel next hop is not found. Double-check before adding.
       * Do not increment ref count for tunnel nexthop here.
       * (done by the caller).
       */
      pNhopEntry = avlSearchLVL7(&hapiBroadL3NhopTree, pTunNhopKey, AVL_EXACT);
      if (pNhopEntry == L7_NULL)
      {
        pNhopEntry = hapiBroadL3NhopEntryAdd(pTunNhopKey, dapi_g);
      }
      p6to4->pV6Nhop = pNhopEntry;
      pNhopEntry->flags |= BROAD_L3_NH_TUNNEL;

      /* Try to resolve the v6 next hop */
      if(L7_IP6_IS_ADDR_6TO4(&pTunNhopKey->addrUsp.addr.ipv6))
      {
        /* Next hop is a 2002:ipv4 address (relay or local) */
        v4Addr = pTunNhopKey->addrUsp.addr.ipv6[2] << 24;
        v4Addr |= pTunNhopKey->addrUsp.addr.ipv6[3] << 16;
        v4Addr |= pTunNhopKey->addrUsp.addr.ipv6[4] << 8;
        v4Addr |= pTunNhopKey->addrUsp.addr.ipv6[5];

        HAPI_BROAD_L3_DEBUG(broadL3Debug, "6to4 nexthop 0x%x\n", v4Addr);

        /* If the next hop address is that of tunnel's local address, then this
         * is a local route. The v4 nexthop must point to CPU and not over tunnel
         */
        if (v4Addr == pTunnelEntry->key.localAddr.u.sa4.sin_addr.s_addr)
        {
          p6to4->pV4Nhop = L7_NULL;
        }
        else
        {
          /* Nexthop is a 6to4 addr which is not our local addr. So, this is
           * a relay next hop. Find the route to this relay nexthop and link the
           * tunnel next hop to that route.
           */
          routeKey.family = L7_AF_INET6;
          /* 48-bit prefix only. 2002:ipv4:: route */
          memcpy(&routeKey.ipAddr, &pTunNhopKey->addrUsp.addr.ipv6, 6);
          memcpy(&routeKey.ipMask, &hapiBroadL3Ipv6Mask6to4, sizeof(bcm_ip6_t));
          pRouteEntry = avlSearchLVL7(&hapiBroadL3RouteTree, &routeKey, AVL_EXACT);

          if (pRouteEntry != L7_NULL)
          {
            HAPI_BROAD_L3_DEBUG(broadL3Debug, "Route to relay found\n");
            for (index=0; index < HAPI_BROAD_L3_MAX_6TO4_NHOP; index++)
            {
              if (pTunnelEntry->tun6to4Nhops[index].pV6Nhop == pRouteEntry->pNhop)
              {
                p6to4->pV4Nhop = pTunnelEntry->tun6to4Nhops[index].pV4Nhop; /* TBD.. ECMP */
                /* Increment the ref count for the v4 nexthop */
                if (p6to4->pV4Nhop != L7_NULL)
                {
                  p6to4->pV4Nhop->ref_count++;
                }
              }
            }
          }
          else
          {
            HAPI_BROAD_L3_DEBUG(broadL3Debug, "Route to relay NOT found\n");
            p6to4->pV4Nhop = L7_NULL;
          }
        }
      }
      else
      {
        /* This is nexthop for a tunnel endpoint.  Extract v4 address from lower
         * 32 bits of v6 address
         */
        v4Addr = pTunNhopKey->addrUsp.addr.ipv6[12] << 24;
        v4Addr |= pTunNhopKey->addrUsp.addr.ipv6[13] << 16;
        v4Addr |= pTunNhopKey->addrUsp.addr.ipv6[14] << 8;
        v4Addr |= pTunNhopKey->addrUsp.addr.ipv6[15];

        nHopKey.addrUsp.usp = pTunnelEntry->nextHopUsp;
        nHopKey.family = L7_AF_INET;
        nHopKey.addrUsp.addr.ipv4  = v4Addr;

        pNhopEntry = avlSearchLVL7(&hapiBroadL3NhopTree, &nHopKey, AVL_EXACT);
        if (pNhopEntry == L7_NULL)
        {
          pNhopEntry = hapiBroadL3NhopEntryAdd(&nHopKey, dapi_g);
        }

        p6to4->pV4Nhop = pNhopEntry; /* If not resolved, tunnel points to CPU */
        pNhopEntry->ref_count++;
      }

      return p6to4->pV6Nhop;
    }
  }
  else
  {
    /* Other tunnel types not handled */
    HAPI_BROAD_L3_LOG_ERROR(0);
  }

  return L7_NULL;
}


/*******************************************************************************
*
* @purpose Update existing 6to4 relay routes
*
* @param  usp          unit/slot/port - tunnel USP
* @param  pRouteEntry  Route to tunnel endpoint
* @param  pV6Nhop      v6 Nexthop of the tunnel endpoint
* @param *dapi_g       DAPI driver object
*
* @returns none
*
* @notes none
*
* @end
*
*******************************************************************************/
static void hapiBroadL3Tunnel6to4RelayUpdate (DAPI_USP_t *tnlUsp,
                                              BROAD_L3_ROUTE_ENTRY_t *pRouteEntry,
                                              BROAD_L3_NH_ENTRY_t * pV6TunNhop,
                                              DAPI_t * dapi_g)
{
  L7_uint8 index;
  BROAD_PORT_t  *nhHapiPortPtr;
  BROAD_L3_NH_ENTRY_t *pV6RelayNhop;
  BROAD_L3_NH_ENTRY_t *pV4RelayNhop = L7_NULL;
  BROAD_L3_TUNNEL_ENTRY_t *pTunnelEntry;

  nhHapiPortPtr = HAPI_PORT_GET(tnlUsp, dapi_g);
  if (nhHapiPortPtr->hapiModeparm.pTunnel == (void *)L7_NULL)
  {
    HAPI_BROAD_L3_LOG_ERROR(0);
  }

  pTunnelEntry = (BROAD_L3_TUNNEL_ENTRY_t *)nhHapiPortPtr->hapiModeparm.pTunnel;

  /* Applies to 6to4 tunnels only */
  if (pTunnelEntry->key.tunnelMode != L7_TUNNEL_MODE_6TO4)
    return;

  /* Got to be 6to4 route */
  if(!L7_IP6_IS_ADDR_6TO4(&pRouteEntry->key.ipAddr.ipv6))
    return;

  /* Find the v4 next hop */
  for (index=0; index < HAPI_BROAD_L3_MAX_6TO4_NHOP; index++)
  {
    if (pV6TunNhop == pTunnelEntry->tun6to4Nhops[index].pV6Nhop)
    {
      pV4RelayNhop = pTunnelEntry->tun6to4Nhops[index].pV4Nhop;
    }
  }

  /* Endpoint is not resolved */
  if (pV4RelayNhop == L7_NULL)
    return;

  for (index=0; index < HAPI_BROAD_L3_MAX_6TO4_NHOP; index++)
  {
    pV6RelayNhop = pTunnelEntry->tun6to4Nhops[index].pV6Nhop;

    if (pV6RelayNhop == L7_NULL)
      continue;

    /* Only compare /48 prefix */
    if (memcmp(pV6RelayNhop->key.addrUsp.addr.ipv6,
               pRouteEntry->key.ipAddr.ipv6, 6) == 0)
    {
      /* The route to this relay is now resolved, update v4 nexthop */
      if (pTunnelEntry->tun6to4Nhops[index].pV4Nhop == L7_NULL)
      {
        pTunnelEntry->tun6to4Nhops[index].pV4Nhop = pV4RelayNhop;
        pV4RelayNhop->ref_count++;
        /* Update the V6 relay next hop */
        hapiBroadL3NhopEntryUpdate(pV6RelayNhop, BROAD_L3_NH_CMD_MODIFY);
      }
      break;
    }
  }
}

/*******************************************************************************
*
* @purpose Deletes an existing L3 tunnel
*
* @param  usp     unit/slot/port - used for IPv6 tunnels only
* @param  cmd     DAPI_CMD_L3_TUNNEL_DELETE or DAPI_CMD_TUNNEL_DELETE
* @param  data    Data associated with tunnel delete command
* @param *dapi_g  DAPI driver object
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
static L7_RC_t hapiBroadL3TunnelDelete(DAPI_USP_t *usp,
                                       DAPI_CMD_t cmd,
                                       void *data,
                                       DAPI_t *dapi_g)
{
  DAPI_TUNNEL_MGMT_CMD_t *dapiCmd = (DAPI_TUNNEL_MGMT_CMD_t*)data;
  BROAD_L3_TUNNEL_KEY_t tunKey;
  BROAD_L3_TUNNEL_ENTRY_t *pTunnelEntry = L7_NULL;
  BROAD_PORT_t       *tnlHapiPortPtr = L7_NULL;

  hapiBroadL3DebugDapiCmdPrint (__FUNCTION__, usp, cmd, data);

  memset(&tunKey, 0, sizeof (BROAD_L3_TUNNEL_KEY_t));

  switch (cmd)
  {
    case DAPI_CMD_TUNNEL_DELETE:  /* RLIM tunnels 6o4, 6to4 */

      /* Get the tunnel ptr from the hapiPortPtr */
      tnlHapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
      pTunnelEntry = tnlHapiPortPtr->hapiModeparm.pTunnel;

      if(pTunnelEntry == L7_NULL)
      {
        return L7_FAILURE;
      }
      break;

    case DAPI_CMD_L3_TUNNEL_DELETE: /* Wireless 4o4 tunnels */

      /* Search the AVL tree based on given paramters */
      tunKey.tunnelMode = dapiCmd->cmdData.l3TunnelAddDelete.tunnelMode;
      tunKey.localAddr  = dapiCmd->cmdData.l3TunnelAddDelete.localAddr;
      tunKey.remoteAddr  = dapiCmd->cmdData.l3TunnelAddDelete.remoteAddr;

      if (tunKey.tunnelMode != L7_TUNNEL_MODE_4OVER4)
      {
        HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3TunnelDelete: Invalid"
                            "Tunnel type %d\n", tunKey.tunnelMode);
        return L7_FAILURE;
      }

      if (tunKey.localAddr.u.sa.sa_family != tunKey.remoteAddr.u.sa.sa_family)
      {
        return L7_FAILURE;
      }
      break;

    default:
      return L7_FAILURE;
  }

  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);
  if (pTunnelEntry == L7_NULL)
  {
    pTunnelEntry = avlSearchLVL7(&hapiBroadL3TunnelTree, &tunKey, AVL_EXACT);
    if (pTunnelEntry == L7_NULL)
    {
      HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3TunnelDelete: not found\n");
      HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
      return L7_FAILURE;
    }
  }

  /* Mark the tunnel for deletion. Check for pending operations */
  if (pTunnelEntry->wl.cmd == BROAD_L3_TUNNEL_CMD_NO_OP)
  {
    pTunnelEntry->wl.cmd = BROAD_L3_TUNNEL_CMD_DELETE;
    HAPI_BROAD_L3_TUNNEL_WLIST_ADD(pTunnelEntry);
  }
  else if (pTunnelEntry->wl.cmd == BROAD_L3_TUNNEL_CMD_CREATE)
  {
    /* Async task didn't get a chance to create */
    HAPI_BROAD_L3_TUNNEL_WLIST_DEL(pTunnelEntry);

    if (pTunnelEntry->pTunnelNhop != NULL)
    {
      hapiBroadL3NhopEntryUpdate(pTunnelEntry->pTunnelNhop, BROAD_L3_NH_CMD_DELETE);
    }
    if (pTunnelEntry->pXportNhop != NULL)
    {
      hapiBroadL3NhopEntryUpdate(pTunnelEntry->pXportNhop, BROAD_L3_NH_CMD_DELETE);
    }
    if (avlDeleteEntry(&hapiBroadL3TunnelTree, pTunnelEntry) == L7_NULL)
    {
      HAPI_BROAD_L3_LOG_ERROR(0);
    }
  }
  else if ((pTunnelEntry->wl.cmd == BROAD_L3_TUNNEL_CMD_MODIFY) ||
           (pTunnelEntry->wl.cmd == BROAD_L3_TUNNEL_CMD_NHOP_SET))
  {
    pTunnelEntry->wl.cmd = BROAD_L3_TUNNEL_CMD_DELETE;
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
  /* Wake up Async task to process the work lists */
  HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;

  return L7_SUCCESS;
}


/*******************************************************************************
*
* @purpose Adds an L3 tunnel MAC address
*
* @param  usp     unit/slot/port
* @param  cmd     DAPI_CMD_L3_TUNNEL_MAC_ADDRESS_ADD
* @param  data    Data associated with tunnel MAC add command
* @param *dapi_g  DAPI driver object
*
* @returns L7_RC_t result
*
* @notes  This DAPI cmd is valid for 4over4/Wireless tunnels only.
*
* @end
*
*******************************************************************************/
static L7_RC_t hapiBroadL3TunnelMacAddressAdd(DAPI_USP_t *usp,
                                              DAPI_CMD_t cmd,
                                              void *data,
                                              DAPI_t *dapi_g)
{
  DAPI_TUNNEL_MGMT_CMD_t  *dapiCmd = (DAPI_TUNNEL_MGMT_CMD_t*)data;
  BROAD_L3_TUNNEL_ENTRY_t *pTunnelEntry;
  BROAD_L3_TUNNEL_KEY_t    tunKey;
  BROAD_L3_MAC_ENTRY_t     mac;
  BROAD_L3_MAC_ENTRY_t    *pMacEntry;
  BROAD_L3_HOST_KEY_t      hostKey;
  BROAD_L3_HOST_ENTRY_t   *pHostEntry;
  DAPI_PORT_t             *dapiPortPtr;
  L7_enetMacAddr_t        *pCmdMacAddr;

  hapiBroadL3DebugDapiCmdPrint (__FUNCTION__, usp, cmd, data);

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

  if (IS_PORT_TYPE_LOGICAL_VLAN(dapiPortPtr) == L7_FALSE)
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3TunnelMacAddressAdd: "
                   "Invalid USP %u:%u:%u\n", usp->unit, usp->slot, usp->port);
    return L7_FAILURE;
  }

  memset(&tunKey, 0, sizeof (BROAD_L3_TUNNEL_KEY_t));
  memset(&mac, 0, sizeof (BROAD_L3_MAC_ENTRY_t));

  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  /* Search the AVL tree based on given paramters */
  tunKey.tunnelMode = dapiCmd->cmdData.l3TunnelMacAddressAddDelete.tunnelMode;
  tunKey.localAddr  = dapiCmd->cmdData.l3TunnelMacAddressAddDelete.localAddr;
  tunKey.remoteAddr  = dapiCmd->cmdData.l3TunnelMacAddressAddDelete.remoteAddr;
  pCmdMacAddr = &dapiCmd->cmdData.l3TunnelMacAddressAddDelete.macAddr;

  pTunnelEntry = avlSearchLVL7(&hapiBroadL3TunnelTree, &tunKey, AVL_EXACT);
  if (pTunnelEntry == L7_NULL)
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3TunnelMacAddressAdd: not found\n");
    HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
    return L7_FAILURE;
  }

  mac.key.vlanId = dapiPortPtr->modeparm.router.vlanID;
  memcpy(&mac.key.macAddr, pCmdMacAddr, L7_ENET_MAC_ADDR_LEN);

  pMacEntry = avlSearchLVL7(&hapiBroadL3MacTree, &mac, AVL_EXACT);
  if (pMacEntry == L7_NULL)
  {
    /* Entry not found, create it */
    pMacEntry = hapiBroadL3AvlInsert(&hapiBroadL3MacTree, &mac);
    if (pMacEntry != L7_NULL)
    {
      pMacEntry->pTunnelEntry = pTunnelEntry;
    }
    else
    {
      HAPI_BROAD_L3_LOG_ERROR(0);
    }
  }
  else
  {
    /* Mac entry already exists, reuse it */
    if (pMacEntry->pTunnelEntry == L7_NULL)
    {
      HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3TunnelMacAddressAdd: MAC entry"
          " already exists, but not linked to tunnel. Link it \n");

      pMacEntry->pTunnelEntry = pTunnelEntry;

      /* Find if any hosts are linked to the MAC. Mostly, the wireless client's MAC
       * entry gets added first and then the HOST entry is added. But, when HOST
       * entry is static, then it possible that HOST is added first and then its MAC
       * is linked to tunnel.
       */
      memset(&hostKey, 0, sizeof(hostKey));
      pHostEntry = avlSearchLVL7(&hapiBroadL3HostTree, (void *)&hostKey, AVL_EXACT);
      if (pHostEntry == L7_NULL)
      {
        pHostEntry = avlSearchLVL7(&hapiBroadL3HostTree, (void *)&hostKey, AVL_NEXT);
      }

      while(pHostEntry != L7_NULL)
      {
        if (pHostEntry->pNhop != L7_NULL)
        {
          if (pHostEntry->pNhop->pMac == pMacEntry)
          {
            pHostEntry->flags |= BROAD_L3_HOST_TUNNEL;
            hapiBroadL3TunnelHostMacLink(pHostEntry, pMacEntry);
          }
        }
        pHostEntry = avlSearchLVL7(&hapiBroadL3HostTree, pHostEntry, AVL_NEXT);
      }
    }
    else
    {
      HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3TunnelMacAddressAdd: MAC entry"
                          " is already linked to an existing tunnel\n");
      HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
      return L7_FAILURE; /* MAC entry must be disassociated first */
    }
  }

  /* Update all the hosts linked to this MAC address!! */
  pHostEntry = pMacEntry->hostList;
  while (pHostEntry != L7_NULL)
  {
    pHostEntry->pTunNhop = pTunnelEntry->pTunnelNhop;
    if (pHostEntry->pTunNhop != L7_NULL)
    {
      pHostEntry->pTunNhop->ref_count++;
    }

    if (pHostEntry->wl.cmd == BROAD_L3_HOST_CMD_NO_OP)
    {
      pHostEntry->wl.cmd = BROAD_L3_HOST_CMD_MODIFY;
      HAPI_BROAD_L3_HOST_WLIST_ADD(pHostEntry);
    }
    pHostEntry = pHostEntry->pMacHostNext;
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

  HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;

  /* Create L2 entry for the MAC address with L3 bit set */
  hapiBroadL3MacAddressAdd(usp, pCmdMacAddr, dapi_g);

  return L7_SUCCESS;
}


/*******************************************************************************
*
* @purpose Deletes an L3 tunnel MAC address
*
* @param  usp     unit/slot/port
* @param  cmd     DAPI_CMD_L3_TUNNEL_MAC_ADDRESS_DELETE
* @param  data    Data associated with tunnel MAC delete command
* @param *dapi_g  DAPI driver object
*
* @returns L7_RC_t result
*
* @notes  This DAPI cmd is valid for 4over4/Wireless tunnels only.
*
* @end
*
*******************************************************************************/
static L7_RC_t hapiBroadL3TunnelMacAddressDelete(DAPI_USP_t *usp,
                                                 DAPI_CMD_t cmd,
                                                 void *data,
                                                 DAPI_t *dapi_g)
{
  DAPI_TUNNEL_MGMT_CMD_t  *dapiCmd = (DAPI_TUNNEL_MGMT_CMD_t*)data;
  BROAD_L3_MAC_ENTRY_t     mac;
  BROAD_L3_MAC_ENTRY_t    *pMacEntry;
  BROAD_L3_HOST_ENTRY_t   *pHostEntry;
  BROAD_L3_HOST_ENTRY_t   *pNextHostEntry;
  BROAD_L3_TUNNEL_ENTRY_t *pTunnelEntry;
  DAPI_PORT_t             *dapiPortPtr;
  L7_enetMacAddr_t        *pCmdMacAddr;

  hapiBroadL3DebugDapiCmdPrint (__FUNCTION__, usp, cmd, data);

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  if (IS_PORT_TYPE_LOGICAL_VLAN(dapiPortPtr) != L7_TRUE)
  {
    HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3TunnelMacAddressDelete: "
                   "Invalid USP %u:%u:%u\n", usp->unit, usp->slot, usp->port);
    return L7_FAILURE;
  }

  pCmdMacAddr = &dapiCmd->cmdData.l3TunnelMacAddressAddDelete.macAddr;
  memset(&mac, 0, sizeof (BROAD_L3_MAC_ENTRY_t));

  mac.key.vlanId = dapiPortPtr->modeparm.router.vlanID;
  memcpy(&mac.key.macAddr, pCmdMacAddr, L7_ENET_MAC_ADDR_LEN);

  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  pMacEntry = avlSearchLVL7(&hapiBroadL3MacTree, &mac, AVL_EXACT);
  if (pMacEntry != (BROAD_L3_MAC_ENTRY_t *)L7_NULL)
  {
    pTunnelEntry = pMacEntry->pTunnelEntry;

    if (pTunnelEntry == L7_NULL)
    {
      HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3TunnelMacAddressDelete: "
                          " Tunnel not found\n");
      HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
      return L7_FAILURE;
    }

    pMacEntry->pTunnelEntry = L7_NULL;

    /* Update all the hosts linked to this MAC address!! */
    pHostEntry = pMacEntry->hostList;
    pNextHostEntry = pHostEntry;
    while (pNextHostEntry != L7_NULL)
    {
      pNextHostEntry = pHostEntry->pMacHostNext;

      if (pHostEntry->pTunNhop != L7_NULL)
      {
        hapiBroadL3NhopEntryUpdate(pHostEntry->pTunNhop, BROAD_L3_NH_CMD_DELETE);
        pHostEntry->pTunNhop = L7_NULL;
        pHostEntry->flags &= ~BROAD_L3_HOST_TUNNEL;
        hapiBroadL3TunnelHostMacUnlink(pHostEntry, pMacEntry);
      }

      if (pHostEntry->wl.cmd == BROAD_L3_HOST_CMD_NO_OP)
      {
        pHostEntry->wl.cmd = BROAD_L3_HOST_CMD_MODIFY;
        HAPI_BROAD_L3_HOST_WLIST_ADD(pHostEntry);
      }

      pHostEntry = pNextHostEntry;
    }

    if ((pMacEntry->nHopList == (BROAD_L3_NH_ENTRY_t *)L7_NULL) &&
        (pMacEntry->hostList == (BROAD_L3_HOST_ENTRY_t *)L7_NULL))
    {
      /* No more nhops/hosts linked to this MAC. Delete it */
      HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3TunnelMacAddressDelete: "
                          " Deleting MAC AVL entry\n");
      if (avlDeleteEntry(&hapiBroadL3MacTree, pMacEntry) == L7_NULL)
      {
        HAPI_BROAD_L3_LOG_ERROR(0);
      }
    }
  }
  else
  {
    HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
    return L7_FAILURE;
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

  HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;

  hapiBroadL3MacAddressDelete(usp, pCmdMacAddr, dapi_g);
  return L7_SUCCESS;
}


/*******************************************************************************
*
* @purpose Set the nexthop transport address of the tunnel
*
* @param  usp     unit/slot/port
* @param  cmd     DAPI_CMD_TUNNEL_NEXT_HOP_SET
* @param  data    Data associated with tunnel next hop set command
* @param *dapi_g  DAPI driver object
*
* @returns L7_RC_t result
*
* @notes This command is valid for 6o4, 6to4 tunnels only!
*
* @end
*
*******************************************************************************/
static L7_RC_t hapiBroadL3TunnelNextHopSet(DAPI_USP_t *usp,
                                           DAPI_CMD_t cmd,
                                           void *data,
                                           DAPI_t *dapi_g)
{
  DAPI_TUNNEL_MGMT_CMD_t *dapiCmd = (DAPI_TUNNEL_MGMT_CMD_t*)data;
  BROAD_L3_TUNNEL_ENTRY_t *pTunnelEntry;
  BROAD_L3_NH_ENTRY_t  *pNhopEntry;
  BROAD_L3_NH_KEY_t     nhKey;
  BROAD_PORT_t         *tnlHapiPortPtr;
  L7_sockaddr_union_t  nHopAddr;

  hapiBroadL3DebugDapiCmdPrint (__FUNCTION__, usp, cmd, data);

  nHopAddr = dapiCmd->cmdData.tunnelNextHopSet.nextHopAddr;

  if (dapiCmd->cmdData.tunnelNextHopSet.getOrSet == DAPI_CMD_GET)
  {
    return L7_FAILURE;
  }

  /* Nexthop must be v4 address, as tunnel carries v6 over v4. */
  if (nHopAddr.u.sa.sa_family != L7_AF_INET)
  {
    return L7_FAILURE;
  }

  if (isValidUsp(&dapiCmd->cmdData.tunnelNextHopSet.nextHopUsp, dapi_g) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  memset(&nhKey, 0, sizeof (BROAD_L3_NH_KEY_t));
  tnlHapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  pTunnelEntry = tnlHapiPortPtr->hapiModeparm.pTunnel;

  if (pTunnelEntry == L7_NULL)
  {
    return L7_FAILURE;
  }

  /* Check if the new nexthop exists */
  nhKey.addrUsp.usp = dapiCmd->cmdData.tunnelNextHopSet.nextHopUsp;
  nhKey.family = L7_AF_INET;
  nhKey.addrUsp.addr.ipv4  = nHopAddr.u.sa4.sin_addr.s_addr ;

  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  pNhopEntry = avlSearchLVL7(&hapiBroadL3NhopTree, &nhKey, AVL_EXACT);
  if (pNhopEntry == L7_NULL)
  {
    /*  Create the Xport next hop */
    pNhopEntry = hapiBroadL3NhopEntryAdd(&nhKey, dapi_g);
    pNhopEntry->ref_count++;
  }
  else
  {
    /* Xport next hop already exists. Check if tunnel next hop is the same */
    hapiBroadL3NhopEntryUpdate(pNhopEntry, BROAD_L3_NH_CMD_ADD);
    if (pNhopEntry != pTunnelEntry->pXportNhop)
    {
      pNhopEntry->ref_count++;
    }
    else
    {
      HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3TunnelNextHopSet: nexthop"
                          " is same as current nexthop\n");
      HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
      return L7_SUCCESS;
    }
  }

  /* Delete the current next hop. Note, Tunnels are not dependent on the
   * Xport egress Id, so that nexthop object can be deleted here (no need
   * delay the decrement/delete)
   */
  if ((pTunnelEntry->pXportNhop != L7_NULL) &&
      (pTunnelEntry->pXportNhop != pNhopEntry))
  {
    hapiBroadL3NhopEntryUpdate(pTunnelEntry->pXportNhop, BROAD_L3_NH_CMD_DELETE);
  }

  pTunnelEntry->pXportNhop = pNhopEntry;

  /* Note: routes going over this tunnel will be updated on async side */

  /* Enqueue the tunnel for next hop set cmd. Always, check the current cmd */
  if (pTunnelEntry->wl.cmd == BROAD_L3_TUNNEL_CMD_NO_OP)
  {
    pTunnelEntry->wl.cmd = BROAD_L3_TUNNEL_CMD_NHOP_SET;
    HAPI_BROAD_L3_TUNNEL_WLIST_ADD(pTunnelEntry);
  }
  else if (pTunnelEntry->wl.cmd == BROAD_L3_TUNNEL_CMD_MODIFY)
  {
    pTunnelEntry->wl.cmd = BROAD_L3_TUNNEL_CMD_NHOP_SET;
  }
  /* if the tunnel is being created, it will pick the new nexthop */

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
  /* Wake up Async task to process the work lists */
  HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;
  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Given a nexthop entry, Search for an Tunnel entry and update it.
*
* @param   pNhop      Next hop key information
*
* @returns none
*
* @notes   Caller must take hapiBroadL3Sema before invoking this function.
*
* @end
*
*******************************************************************************/
static void hapiBroadL3TunnelResolve(BROAD_L3_NH_ENTRY_t *pNhop)
{
  L7_uint8 index = 0;
  L7_BOOL  enqueue = L7_FALSE;
  BROAD_L3_TUNNEL_KEY_t zero;
  BROAD_L3_TUNNEL_ENTRY_t *pTunEntry;

  memset(&zero, 0, sizeof(BROAD_L3_TUNNEL_KEY_t));

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "*** hapiBroadL3TunnelResolve ***");

  pTunEntry = avlSearchLVL7(&hapiBroadL3TunnelTree, (void *)&zero,  AVL_EXACT);
  if (pTunEntry == L7_NULL)
  {
    pTunEntry = avlSearchLVL7(&hapiBroadL3TunnelTree, (void *)&zero, AVL_NEXT);
  }

  while(pTunEntry != L7_NULL)
  {
    enqueue = L7_FALSE;
    /* Note: There is one nexthop for configured 6o4/4o4 tunnels (point-to-point)
     * For 6to4 tunnels, there could be more than one nexthop (one-to-many)
     * Also, we just mark the tunnel for modify. How exactly the tunnel is
     * modified is determined on the Async side (like the initiator update,
     * tunnel next hop update etc).
     */
    if (((pTunEntry->key.tunnelMode == L7_TUNNEL_MODE_6OVER4) ||
         (pTunEntry->key.tunnelMode == L7_TUNNEL_MODE_4OVER4)) &&
        (pTunEntry->pXportNhop == pNhop))
    {
      enqueue = L7_TRUE;
    }
    else if (pTunEntry->key.tunnelMode == L7_TUNNEL_MODE_6TO4)
    {
      /* Check if any of the 6to4 nexthops match */
      for (index=0; index < HAPI_BROAD_L3_MAX_6TO4_NHOP; index++)
      {
        if (pTunEntry->tun6to4Nhops[index].pV6Nhop != L7_NULL)
        {
          if (pTunEntry->tun6to4Nhops[index].pV4Nhop == pNhop)
          {
            pTunEntry->tun6to4Nhops[index].update = L7_TRUE;
            enqueue = L7_TRUE;
          }
        }
      }
    }

    if (enqueue == L7_TRUE)
    {
      if (pTunEntry->wl.cmd == BROAD_L3_TUNNEL_CMD_NO_OP)
      {
        pTunEntry->wl.cmd = BROAD_L3_TUNNEL_CMD_MODIFY;
        HAPI_BROAD_L3_TUNNEL_WLIST_ADD(pTunEntry);
      }
    }
    pTunEntry = avlSearchLVL7(&hapiBroadL3TunnelTree, pTunEntry, AVL_NEXT);
  }
}

/*******************************************************************************
*
* @purpose Create VRID on the specified L3 interface
*
* @param  usp     unit/slot/port of the routing interface
* @param  cmd     DAPI_CMD_ROUTING_INTF_VRID_ADD
* @param  data    Data associated with VRID Add command
* @param *dapi_g  DAPI driver object
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
static L7_RC_t hapiBroadL3IntfVRIDAdd(DAPI_USP_t *usp,
                                      DAPI_CMD_t cmd,
                                      void *data,
                                      DAPI_t *dapi_g)
{
  L7_RC_t result = L7_SUCCESS;
  DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd = (DAPI_ROUTING_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  bcm_vlan_t vid = 0;
  usl_bcm_l3_intf_t intfInfo;
  L7_int32 rv;
  L7_ushort16 vrid;

  if (DAPI_CMD_GET == dapiCmd->cmdData.rtrIntfVRIDAdd.getOrSet)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: In %s Get not supported for this command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return L7_FAILURE;
  }

  vrid = dapiCmd->cmdData.rtrIntfVRIDAdd.vrID;

  if (vrid > L7_PLATFORM_ROUTING_VRRP_MAX_VRID)
  {
    return L7_FAILURE;
  }

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3IntfVRIDAdd: USP %d:%d:%d, VRID"
                      " %d\n", usp->unit, usp->slot, usp->port, vrid);

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
  {
    vid = hapiPortPtr->port_based_routing_vlanid;
  }
  else if (IS_PORT_TYPE_LOGICAL_VLAN(dapiPortPtr) == L7_TRUE)
  {
    vid = dapiPortPtr->modeparm.router.vlanID;
  }
  else
  {
    HAPI_BROAD_L3_LOG_ERROR(dapiPortPtr->type);
  }

  /* add the entry to the HW */
  usl_bcm_l3_intf_t_init(&intfInfo);

  memcpy(intfInfo.bcm_data.l3a_mac_addr, &(L7_ENET_VRRP_MAC_ADDR), sizeof(bcm_mac_t));
  intfInfo.bcm_data.l3a_mac_addr[5] = vrid;

  intfInfo.bcm_data.l3a_vid = vid;
  intfInfo.bcm_data.l3a_flags |= BCM_L3_ADD_TO_ARL;

  rv = usl_bcmx_l3_intf_create(&intfInfo);
  if ((L7_BCMX_OK(rv) != L7_TRUE))
  {
    HAPI_BROAD_L3_LOG_ERROR(rv);
  }
  else
  {
    hapiBroadL3HostPolicyInstall (usp, (L7_uchar8 *)intfInfo.bcm_data.l3a_mac_addr, dapi_g);
    hapiPortPtr->vrrp_interface_id[vrid] = intfInfo.bcm_data.l3a_intf_id;
  }

  return result;
}


/*******************************************************************************
*
* @purpose Delete VRID on the specified L3 interface
*
* @param  usp     unit/slot/port of the routing interface
* @param  cmd     DAPI_CMD_ROUTING_INTF_VRID_DELETE
* @param  data    Data associated with VRID delete command
* @param *dapi_g  DAPI driver object
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
static L7_RC_t hapiBroadL3IntfVRIDDelete(DAPI_USP_t *usp,
                                         DAPI_CMD_t cmd,
                                         void *data,
                                         DAPI_t *dapi_g)
{
  L7_RC_t result = L7_SUCCESS;
  DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd = (DAPI_ROUTING_INTF_MGMT_CMD_t*)data;
  BROAD_PORT_t *hapiPortPtr;
  usl_bcm_l3_intf_t intfInfo;
  L7_ushort16 vrid;
  L7_int32 rv;

  if(DAPI_CMD_GET == dapiCmd->cmdData.rtrIntfVRIDDelete.getOrSet)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: In %s Get not supported for this command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return L7_FAILURE;
  }

  vrid = dapiCmd->cmdData.rtrIntfVRIDDelete.vrID;

  if (vrid > L7_PLATFORM_ROUTING_VRRP_MAX_VRID)
  {
    return L7_FAILURE;
  }

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3IntfVRIDDelete: USP %d:%d:%d,"
                      "VRID %d\n", usp->unit, usp->slot, usp->port, vrid);

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  usl_bcm_l3_intf_t_init(&intfInfo);
  intfInfo.bcm_data.l3a_intf_id = hapiPortPtr->vrrp_interface_id[vrid];

  rv = usl_bcmx_l3_intf_delete(&intfInfo);
  if ((L7_BCMX_OK(rv) != L7_TRUE))
  {
    HAPI_BROAD_L3_LOG_ERROR(rv);
  }

  hapiPortPtr->vrrp_interface_id[vrid] = 0;

  /* Remove the host policy for VRRP MAC */
  memcpy(intfInfo.bcm_data.l3a_mac_addr, &(L7_ENET_VRRP_MAC_ADDR), sizeof(bcm_mac_t));
  intfInfo.bcm_data.l3a_mac_addr[5] = vrid;
  hapiBroadL3HostPolicyRemove (usp, (L7_uchar8 *)intfInfo.bcm_data.l3a_mac_addr, dapi_g);

  return result;
}


/*******************************************************************************
*
* @purpose Control ICMP redirect to CPU option.
*
* @param  usp     unit/slot/port - not valid for this command
* @param  cmd     DAPI_CMD_ROUTING_ROUTE_ICMP_REDIRECTS_CONFIG
* @param  data    Data associated with ICMP redirect command
* @param *dapi_g  DAPI driver object
*
* @returns L7_RC_t result
*
* @notes This is a global knob only and not per-interface.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3IcmpRedirConfig(DAPI_USP_t *usp,
                                   DAPI_CMD_t cmd,
                                   void *data,
                                   DAPI_t *dapi_g)
{
  L7_int32 i, rv;
  L7_int32 enable = 0;
  L7_RC_t  result = L7_SUCCESS;
  DAPI_USP_t portUsp, vlanUsp;
  DAPI_ROUTING_MGMT_CMD_t *dapiCmd = (DAPI_ROUTING_MGMT_CMD_t*)data;
  DAPI_SYSTEM_t *dapiSystemPtr = dapi_g->system;
  DAPI_PORT_t *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;

  HAPI_BROAD_L3_DEBUG(broadL3Debug, "hapiBroadL3IcmpRedirConfig: %d\n",
                      dapiCmd->cmdData.icmpRedirectsConfig.enable);

  if (dapiCmd->cmdData.icmpRedirectsConfig.getOrSet == DAPI_CMD_GET)
  {
    SYSAPI_PRINTF (SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: In %s Set not supported for this command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return L7_FAILURE;
  }

  if (dapiCmd->cmdData.icmpRedirectsConfig.enable == L7_TRUE)
  {
    enable = 1;
    dapiSystemPtr->icmpRedirEnable = L7_TRUE;
  }
  else
  {
    dapiSystemPtr->icmpRedirEnable = L7_FALSE;
  }

  /* Always set the switch control. Do not get the current value
   * and skip the set. That causes issues when new unit gets attached
   */
  rv = bcmx_switch_control_get(bcmSwitchIcmpRedirectToCpu, &i);
  if (rv == BCM_E_NONE)
  {
  rv = bcmx_switch_control_set(bcmSwitchIcmpRedirectToCpu, enable);

    HAPI_BROAD_L3_BCMX_DBG(rv, "bcmx_switch_control_set: ICMP redirect");
    if (rv != BCM_E_NONE)
    {
      result = L7_FAILURE;
    }
  }
  else
  {
    /* Some devices, like Triumph, support per VLAN control for ICMP redirect,
     * instead of global control knob in CPU_CONTROL register. Currently,
     * FastPath support global knob for ICMP redirect, and not per VLAN/intf.
     * In future, this may change, in which case consider the input USP
     * and apply the ICMP redirect config only on that routed VLAN.
     */

    /* Check the VLAN routing interfaces */
    vlanUsp.unit = L7_LOGICAL_UNIT;
    vlanUsp.slot = L7_VLAN_SLOT_NUM;

    for (vlanUsp.port = 0; vlanUsp.port < L7_MAX_NUM_ROUTER_INTF; vlanUsp.port++)
    {
      if (isValidUsp(&vlanUsp,dapi_g) == L7_FALSE)
      {
        continue;
      }

      dapiPortPtr = GET_DAPI_PORT(dapi_g, &vlanUsp);

      /* Check if VLAN is enabled for routing */
      if ((dapiPortPtr->modeparm.router.vlanID != 0) &&
          (dapiPortPtr->modeparm.router.routerIntfEnabled == L7_TRUE))
      {
        result = hapiBroadL3IcmpRedirVlanConfig(
                     dapiPortPtr->modeparm.router.vlanID, enable);
      }
    }

    /* Check port based routing VLANs */
    for (portUsp.unit = 0; portUsp.unit < dapi_g->system->totalNumOfUnits; portUsp.unit++)
    {
      for (portUsp.slot = 0; portUsp.slot < dapi_g->unit[portUsp.unit]->numOfSlots; portUsp.slot++)
      {
        if (dapi_g->unit[portUsp.unit]->slot[portUsp.slot]->cardPresent == L7_TRUE)
        {
          for (portUsp.port = 0; portUsp.port < dapi_g->unit[portUsp.unit]->slot[portUsp.slot]->numOfPortsInSlot; portUsp.port++)
          {
            if (isValidUsp(&portUsp, dapi_g) == L7_FALSE)
            {
              continue;
            }
            dapiPortPtr = DAPI_PORT_GET(&portUsp, dapi_g);
            hapiPortPtr = HAPI_PORT_GET(&portUsp, dapi_g);

            if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE) &&
                (dapiPortPtr->modeparm.physical.routerIntfEnabled == L7_TRUE))
            {
              result = hapiBroadL3IcmpRedirVlanConfig(hapiPortPtr->port_based_routing_vlanid, enable);
            }
          }
        }
      }
    }
  }

  return result;
}

/*******************************************************************************
*
* @purpose Enable ICMP redirect to CPU option for the given VLAN.
*
* @param  vlanId    ID of the routed VLAN
* @param  enable    1 to enable ICMP redir, else disable
*
* @returns L7_RC_t result
*
* @notes This is a per VLAN knob. Some devices, like Triumph, support per VLAN
*        control for ICMP redirect, instead of global control knob in
*        CPU_CONTROL register.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3IcmpRedirVlanConfig(L7_ushort16 vlanId, L7_int32 enable)
{
  L7_int32  rv;
  L7_uint32 flags = BCM_VLAN_ICMP_REDIRECT_TOCPU;
  USL_CMD_t cmd;

  /* Set the ICMP redirect to CPU */
  if (enable)
  {
    cmd = USL_CMD_ADD;
  }
  else
  {
    cmd = USL_CMD_REMOVE;
  }

  /* Update the vlan control flag */
  rv = usl_bcmx_vlan_control_flag_update(vlanId, flags, cmd);

  HAPI_BROAD_L3_BCMX_DBG(rv, "bcmx_vlan_control_vlan_set: returned %s",
                         bcm_errmsg(rv));

  if (rv != BCM_E_NONE)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/*******************************************************************************
*
* @purpose Get Interface L3 stats
*
* @param  usp     unit/slot/port of the routing interface
* @param  cmd     DAPI_CMD_IPV6_INTF_STATS_GET
* @param  data    Data associated with stats command
* @param *dapi_g  DAPI driver object
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
static L7_RC_t hapiBroadL3IntfStatsGet(DAPI_USP_t *usp,
                                       DAPI_CMD_t cmd,
                                       void *data,
                                       DAPI_t *dapi_g)
{
  L7_RC_t result = L7_FAILURE;
  DAPI_ROUTING_INTF_MGMT_CMD_t *dapiCmd = (DAPI_ROUTING_INTF_MGMT_CMD_t *)data;
  L7_uint32 counterId;
  DAPI_PORT_t *dapiPortPtr;

  dapiPortPtr = DAPI_PORT_GET(usp,dapi_g);

  if (dapiCmd->cmdData.rtrL3IntfStatsGet.getOrSet == DAPI_CMD_SET)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: In %s Set not supported for this command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  switch (cmd)
  {
    extern void hapiBroadStatPortUpdate(DAPI_USP_t *usp,
                                        DAPI_INTF_STATS_t hapiCounterId,
                                        DAPI_t *dapi_g);
    case DAPI_CMD_IPV6_INTF_STATS_GET:
      memset(&dapiCmd->cmdData.rtrL3IntfStatsGet.stats.ipv6,
             0,
             sizeof(dapiCmd->cmdData.rtrL3IntfStatsGet.stats.ipv6));
      counterId = DAPI_STATS_IPV6_IN_RECEIVES;
      hapiBroadStatPortUpdate(usp, counterId, dapi_g);
      dapiCmd->cmdData.rtrL3IntfStatsGet.stats.ipv6.ipv6IfStatsInReceives =
            dapiPortPtr->modeparm.physical.stats[counterId].low;

      counterId = DAPI_STATS_IPV6_IN_HDR_ERRS;
      hapiBroadStatPortUpdate(usp, counterId, dapi_g);
      dapiCmd->cmdData.rtrL3IntfStatsGet.stats.ipv6.ipv6IfStatsInHdrErrors =
            dapiPortPtr->modeparm.physical.stats[counterId].low;

      counterId = DAPI_STATS_IPV6_IN_ADDR_ERRS;
      hapiBroadStatPortUpdate(usp, counterId, dapi_g);
      dapiCmd->cmdData.rtrL3IntfStatsGet.stats.ipv6.ipv6IfStatsInAddrErrors =
            dapiPortPtr->modeparm.physical.stats[counterId].low;

      counterId = DAPI_STATS_IPV6_IN_DISCARDS;
      hapiBroadStatPortUpdate(usp, counterId, dapi_g);
      dapiCmd->cmdData.rtrL3IntfStatsGet.stats.ipv6.ipv6IfStatsInDiscards =
            dapiPortPtr->modeparm.physical.stats[counterId].low;

      counterId = DAPI_STATS_IPV6_OUT_FORW_DATAGRAMS;
      hapiBroadStatPortUpdate(usp, counterId, dapi_g);
      dapiCmd->cmdData.rtrL3IntfStatsGet.stats.ipv6.ipv6IfStatsOutForwDatagrams =
            dapiPortPtr->modeparm.physical.stats[counterId].low;

      counterId = DAPI_STATS_IPV6_OUT_DISCARDS;
      hapiBroadStatPortUpdate(usp, counterId, dapi_g);
      dapiCmd->cmdData.rtrL3IntfStatsGet.stats.ipv6.ipv6IfStatsOutDiscards =
            dapiPortPtr->modeparm.physical.stats[counterId].low;

      counterId = DAPI_STATS_IPV6_IN_MCAST_PKTS;
      hapiBroadStatPortUpdate(usp, counterId, dapi_g);
      dapiCmd->cmdData.rtrL3IntfStatsGet.stats.ipv6.ipv6IfStatsInMcastPkts =
            dapiPortPtr->modeparm.physical.stats[counterId].low;

      counterId = DAPI_STATS_IPV6_OUT_MCAST_PKTS;
      hapiBroadStatPortUpdate(usp, counterId, dapi_g);
      dapiCmd->cmdData.rtrL3IntfStatsGet.stats.ipv6.ipv6IfStatsOutMcastPkts =
            dapiPortPtr->modeparm.physical.stats[counterId].low;

      result = L7_SUCCESS;
      break;

    default:
      break;

  }

  return result;
}


/* No-op handler for unsupported DAPI commands */
static L7_RC_t hapiBroadL3NoOp(DAPI_USP_t *usp,
                               DAPI_CMD_t cmd,
                               void *data,
                               DAPI_t *dapi_g)
{
  return L7_SUCCESS;
}



/******************************************************************************
*
* @purpose Updates the L2 mac info and updates the nexthop entries if necessary
*
* @param *macAddr   MAC address learnt
* @param  vlanId    VLAN id of the learnt MAC address
* @param  usp       unit/slot/port on which MAC is learnt.
* @param *dapi_g    DAPI driver object
*
* @returns L7_RC_t result
*
* @notes This learn call back is invoked  by BCM L2X thread for non-stacking and
*        by USL MAC sync thread for stacking packages.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3UpdateMacLearn(bcm_mac_t macAddr,
                                  L7_ushort16 vlanId,
                                  DAPI_USP_t *usp,
                                  DAPI_t *dapi_g)
{
  L7_BOOL isLAG = L7_FALSE;
  L7_uint32 tgid = -1;
  DAPI_USP_t  vlanUsp;
  DAPI_PORT_t *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  BROAD_L3_MAC_KEY_t key;
  BROAD_L3_MAC_ENTRY_t *pMacEntry;
  BROAD_L3_NH_ENTRY_t *pNhopEntry;

  if (hapiBroadL3RouterCardInited != L7_TRUE)
  {
    return L7_SUCCESS;
  }

  if (isValidUsp(usp, dapi_g) != L7_TRUE)
  {
    return L7_SUCCESS;
  }

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE) &&
      (dapiPortPtr->modeparm.physical.routerIntfEnabled == L7_TRUE))
  {
    /*
     * Do not learn mac addresses on physical routing interfaces
     * They will be added to the mac table when the arp is added
     */
    return L7_SUCCESS;
  }
  else if (IS_PORT_TYPE_LOGICAL_VLAN(dapiPortPtr) == L7_TRUE)
  {
    /* MAC address can only be linked to physical/LAG/CPU USP */
    return L7_SUCCESS;
  }
  else if (hapiBroadL3VlanRoutingEnabled(vlanId, &vlanUsp, dapi_g) == L7_FALSE)
  {
    /* Do not learn mac addresses if routing is not enabled on VLAN */
    return L7_SUCCESS;
  }

  HAPI_BROAD_L3_DEBUG(broadL3Debug,
                      "hapiBroadL3UpdateMacLearn: Mac : "
                      "%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x : Vlan %d USP %d:%d:%d\n",
                      macAddr[0], macAddr[1], macAddr[2], macAddr[3],
                      macAddr[4], macAddr[5], vlanId,
                      usp->unit, usp->slot, usp->port);

  /* If this address was learned on a LAG then get the LAG ID. */
  if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
  {
    /* Double check that the trunk ID is valid. If trunk ID is not valid then
     * simply return.
     */
    if (hapiPortPtr->hapiModeparm.lag.numMembers == 0)
    {
      return L7_SUCCESS;
    }
    else
    {
      isLAG = L7_TRUE;
      tgid = hapiPortPtr->hapiModeparm.lag.tgid;
    }
  }

  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  key.vlanId = vlanId;
  memcpy(key.macAddr, macAddr, sizeof(bcm_mac_t));
  pMacEntry = avlSearchLVL7(&hapiBroadL3MacTree, &key, AVL_EXACT);

  if(pMacEntry != L7_NULL)
  {
    /* Check if L2 information has changed */
    if (pMacEntry->resolved == L7_TRUE)
    {
      if ((pMacEntry->usp.unit == usp->unit) &&
          (pMacEntry->usp.slot == usp->slot) &&
          (pMacEntry->usp.port == usp->port))
      {
        /* Duplicate learn callback. Nothing changed */
        HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
        return L7_SUCCESS;
      }
    }

    pMacEntry->resolved = L7_TRUE;
    pMacEntry->usp = *usp;
    pMacEntry->target_is_trunk = isLAG;
    pMacEntry->trunkId = tgid;

    /* Update the nexthops associated with this MAC */
    pNhopEntry = pMacEntry->nHopList;
    while (pNhopEntry != L7_NULL)
    {
      if ((pNhopEntry != L7_NULL) &&
          !(pNhopEntry->flags & BROAD_L3_NH_LOCAL))
      {
        pNhopEntry->flags |= BROAD_L3_NH_RESOLVED;

        /* Update the N.H Work list to mark the entry for modification */
        hapiBroadL3NhopEntryUpdate(pNhopEntry, BROAD_L3_NH_CMD_MODIFY);
      }
      pNhopEntry = pNhopEntry->pMacNhopNext;
    }
  }
  else
  {
    /* Mac entry not found. Implies there is no host associated with this MAC.
     * Ignore this MAC learn. Only MAC addresses linked to hosts are stored in
     * the AVL tree.
     */
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Updates the L2 mac info and updates the nexthop entries if necessary
*
* @param *macAddr   MAC address aged
* @param  vlanId    VLAN ID of the aged MAC address
* @param *dapi_g    system information
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3UpdateMacAge(bcm_mac_t macAddr,
                                L7_ushort16 vlanId,
                                DAPI_t *dapi_g)
{
  DAPI_USP_t  vlanUsp;
  BROAD_L3_MAC_KEY_t key;
  BROAD_L3_MAC_ENTRY_t *pMacEntry;
  BROAD_L3_NH_ENTRY_t *pNhopEntry;

  if (hapiBroadL3RouterCardInited != L7_TRUE)
  {
    return L7_SUCCESS;
  }

  if (hapiBroadL3VlanRoutingEnabled(vlanId, &vlanUsp, dapi_g) == L7_FALSE)
  {
    /* Ignore age callback if routing is not enabled on VLAN */
    return L7_SUCCESS;
  }


  HAPI_BROAD_L3_DEBUG(broadL3Debug,
                      "hapiBroadL3UpdateMacAge: Mac : "
                      "%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x : Vlan %d\n",
                      macAddr[0], macAddr[1], macAddr[2], macAddr[3],
                      macAddr[4], macAddr[5], vlanId);

  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  key.vlanId = vlanId;
  memcpy(key.macAddr, macAddr, sizeof(bcm_mac_t));
  pMacEntry = avlSearchLVL7(&hapiBroadL3MacTree, &key, AVL_EXACT);

  if(pMacEntry != L7_NULL)
  {
    pMacEntry->resolved = L7_FALSE;

    /* Update the hosts/nexthops associated with this MAC */
    pNhopEntry = pMacEntry->nHopList;
    while (pNhopEntry != L7_NULL)
    {
      if ((pNhopEntry != L7_NULL) &&
          !(pNhopEntry->flags & BROAD_L3_NH_LOCAL))
      {
        pNhopEntry->flags &= ~BROAD_L3_NH_RESOLVED;

        /* Update the N.H Work list to mark the entry for modification */
        hapiBroadL3NhopEntryUpdate(pNhopEntry, BROAD_L3_NH_CMD_MODIFY);
      }
      pNhopEntry = pNhopEntry->pMacNhopNext;
    }
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Handles sending the packet when destination is a routing
*          interface
*
* @param   usp       dest unit/slot/port
* @param   frameType frame type, unicast, mcast
* @param   pktData   frame handle
* @param   *dapi_g   system information
*
* @returns L7_RC_t result
*
* @notes   Called for VLAN routing interfaces only
*
* @end
*
*******************************************************************************/
L7_RC_t
hapiBroadL3RoutingSend(DAPI_USP_t *usp,
                       DAPI_FRAME_TYPE_t *frameType,
                       L7_uchar8 *pktData,
                       DAPI_t *dapi_g)
{
  DAPI_PORT_t *dapiPortPtr;
  BROAD_L3_MAC_KEY_t key;
  BROAD_L3_MAC_ENTRY_t *pMacEntry;

  /* No need to search the multicast mac address */
  if ((*frameType == DAPI_FRAME_TYPE_MCAST_DOMAIN) ||
      (*frameType == DAPI_FRAME_TYPE_NO_L2_EGRESS_MCAST_DOMAIN))
  {
    return L7_SUCCESS;
  }

  memset(&key, 0, sizeof(key));

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

  /* Copy Mac and vlanId for search */
  key.vlanId = dapiPortPtr->modeparm.router.vlanID;
  memcpy(key.macAddr, pktData, sizeof(bcm_mac_t));

  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);
  pMacEntry = avlSearchLVL7(&hapiBroadL3MacTree, &key, AVL_EXACT);

  if ((pMacEntry == L7_NULL) || (pMacEntry->resolved == L7_FALSE))
  {
    switch (*frameType)
    {
      case DAPI_FRAME_TYPE_NO_L2_EGRESS_DATA_TO_PORT:
        *frameType = DAPI_FRAME_TYPE_NO_L2_EGRESS_MCAST_DOMAIN;
        break;

      case DAPI_FRAME_TYPE_DATA_TO_PORT:
        *frameType = DAPI_FRAME_TYPE_MCAST_DOMAIN;
        break;

      default:
        break;
    }

    usp->unit = 0;
    usp->slot = 0;
    usp->port = 0;
  }
  else
  {
    *usp = pMacEntry->usp;
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Determine if the given MAC destination address is a proxy
*          address for the given VLAN
*
* @param  macAddr     MAC address
* @param  vlanId      VLAN ID
*
* @returns L7_TRUE    If the MAC address is a proxy address
* @returns L7_FALSE   Otherwise
*
* @notes none
*
* @end
*
******************************************************************************/
L7_BOOL hapiBroadL3MacDestIsProxy(L7_uchar8 *macAddr, L7_uint32 vlanId)
{
  BROAD_L3_MAC_KEY_t macKey;
  BROAD_L3_MAC_ENTRY_t *pMacEntry;
  L7_BOOL isProxy;

  memset(&macKey, 0, sizeof(macKey));
  memcpy(&macKey.macAddr, macAddr, L7_ENET_MAC_ADDR_LEN);
  macKey.vlanId = vlanId;

  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  pMacEntry = avlSearchLVL7(&hapiBroadL3MacTree, &macKey, AVL_EXACT);
  if ((pMacEntry != L7_NULL) && (pMacEntry->pTunnelEntry != L7_NULL))
  {
    /*
     * As we are, essentially, acting as a proxy for MAC addresses
     * that are associated with clients reached by L3 Tunnels,
     * these are valid local destination addresses.
     */
    isProxy = L7_TRUE;
  }
  else
  {
    isProxy = L7_FALSE;
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

  return isProxy;
}

