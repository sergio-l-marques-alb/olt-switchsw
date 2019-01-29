/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
********************************************************************************
*
* @filename  broad_l3_async.c
*
* @purpose   This file contains all asynchronous processing functions for Layer3
*            except for tunnel functions which are defined in broad_l3_tunnel.c 
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

#include "broad_l3_int.h"
#include "bcmx/tunnel.h"
#include "bcm/error.h"
#include "l7_usl_bcmx_l2.h"
#include "l7_usl_bcmx_l3.h"

#ifdef L7_WIRELESS_PACKAGE
#include "broad_l2_tunnel.h"
#endif

#define BROAD_L3_ARP_COLLISION_DELAY 333

/* Hardware route stats */
BROAD_L3_HW_STATS_t broadL3HwRouteStats;
BROAD_L3_HW_STATS_t broadL3HwHostStats;
BROAD_L3_HW_STATS_t broadL3HwNhopStats;
BROAD_L3_HW_STATS_t broadL3HwEcmpStats;

static L7_BOOL retryFailedRoutes = L7_FALSE;
static L7_BOOL retryFailedHosts = L7_FALSE;
static L7_BOOL retryFailedNhops = L7_FALSE;
static L7_BOOL retryFailedEcmpNhops = L7_FALSE;
static L7_BOOL retryTimerExpired = L7_FALSE;

/* Function prototypes */
static void hapiBroadL3AsyncNhopWlistProcess (DAPI_t *dapi_g);
static void hapiBroadL3AsyncEcmpWlistProcess (DAPI_t *dapi_g);
static void hapiBroadL3AsyncHostWlistProcess (DAPI_t *dapi_g);
static void hapiBroadL3AsyncRouteWlistProcess (DAPI_t *dapi_g);
static void hapiBroadL3AsyncTunnelWlistProcess (DAPI_t *dapi_g);
static void hapiBroadL3AsyncWaitqProcess(DAPI_t *dapi_g);
static void hapiBroadL3AsyncRetryFailures(DAPI_t *dapi_g);
static void hapiBroadL3AsyncZeroCountNhopDelete(DAPI_t *dapi_g);

static void hapiBroadL3AsyncTunnelCreate (BROAD_L3_TUNNEL_ENTRY_t *pTunEntry,
                                          L7_BOOL replace, DAPI_t *dapi_g);
static void hapiBroadL3AsyncTunnelModify (BROAD_L3_TUNNEL_ENTRY_t *pTunEntry);
static void hapiBroadL3AsyncTunnelDelete (BROAD_L3_TUNNEL_ENTRY_t *pTunEntry);
static void hapiBroadL3AsyncTunnelNhopResolve(BROAD_L3_NH_ENTRY_t *pNhop,
                                              BROAD_L3_MAC_ENTRY_t **pMac,
                                              DAPI_t *dapi_g);
static void hapiBroadL3AsyncTunnelNhopDelete(BROAD_L3_NH_ENTRY_t *pTunNhop,
                                             DAPI_t  * dapi_g);


/*******************************************************************************
*
* @purpose Task that handles all async L3 operations. Also updates host and
*          route entries to match the correct L2 state.
*
* @param   num_args - Number of arguments passed
* @param   dapi_g   - DAPI Driver object
*
* @returns none
*
* @notes none
*
* @end
*
*******************************************************************************/
void hapiBroadL3AsyncTask(L7_uint32 num_args, DAPI_t *dapi_g)
{
  /* This task is typically woken up when L3 entries are queued to work lists. 
   * Note that, work can be queued because of DAPI L3 command or L2 events
   */ 

  hapiBroadL3DebugTaskIdSet();

  memset(&broadL3HwRouteStats, 0, sizeof(broadL3HwRouteStats));
  memset(&broadL3HwHostStats, 0, sizeof(broadL3HwHostStats));
  memset(&broadL3HwNhopStats, 0, sizeof(broadL3HwNhopStats));
  memset(&broadL3HwEcmpStats, 0, sizeof(broadL3HwEcmpStats));

  while (1)
  {
    HAPI_BROAD_L3_ASYNC_WAIT;

    /* There is work pending on the work lists. Process the next hop work list
     * first, followed by host work list and then route work list. The order 
     * does matter. The egress objects must be created before host/route entries
     * are added.
     */
    
    hapiBroadL3AsyncNhopWlistProcess(dapi_g);
    hapiBroadL3AsyncEcmpWlistProcess(dapi_g);
    hapiBroadL3AsyncHostWlistProcess(dapi_g);
    hapiBroadL3AsyncRouteWlistProcess(dapi_g);
    hapiBroadL3AsyncTunnelWlistProcess(dapi_g);
    
    /* Submit nhops with zero reference count for deletion */
    if (hapiBroadL3ProcessZeroCountNhops == L7_TRUE)
    {
      hapiBroadL3AsyncZeroCountNhopDelete(dapi_g);
    }

    /* Retry L3 insertion failures */
    hapiBroadL3AsyncRetryFailures(dapi_g);
    
    /* Process the wait queue to see if anyone is waiting for async to finish up */
    hapiBroadL3AsyncWaitqProcess(dapi_g);
  }
}

/*******************************************************************************
*
* @purpose Processes the next hop entries queued in the next hop work list
*
* @param   dapi_g   - DAPI Driver object
*
* @returns none
*
* @notes none
*
* @end
*
*******************************************************************************/
static void hapiBroadL3AsyncNhopWlistProcess (DAPI_t *dapi_g)
{
  L7_uint32              flags = 0;
  bcm_if_t               egressIntfId;
  usl_bcm_l3_egress_t    bcmEgrObj;
  BROAD_L3_NH_ENTRY_t    *pNhop = L7_NULL;
  BROAD_L3_MAC_ENTRY_t   *pMac = L7_NULL;
  BROAD_PORT_t           *hapiPortPtr;
  BROAD_PORT_t           *cpuHapiPortPtr;
  BROAD_L3_NH_CMD_t      prevCmd = BROAD_L3_NH_CMD_NO_OP;

  /* Note: This semaphore must be released before making BCMX/RPC/USL call 
   * to avoid blocking the application thread
   */
  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  if (hapiBroadL3NhopListHead == L7_NULL)
  {
    HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
    return;
  }

  HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3AsyncNhopWlistProcess has work");

  do
  {
    pNhop = hapiBroadL3NhopListHead;
    flags = 0;

    usl_bcm_l3_egress_t_init(&bcmEgrObj);

    if (pNhop->key.family == L7_AF_INET6)
    {
      memcpy(&(bcmEgrObj.key.addr.l3a_ip6_addr), 
             &(pNhop->key.addrUsp.addr.ipv6),
             sizeof(bcm_ip6_t));
      bcmEgrObj.key.flags |= BCM_L3_IP6;            
    }
    else
    {
      memcpy(&(bcmEgrObj.key.addr.l3a_ip_addr), 
             &(pNhop->key.addrUsp.addr.ipv4),
             sizeof(bcm_ip_t));
    }

    memcpy(&(bcmEgrObj.key.usp), 
           &(pNhop->key.addrUsp.usp),
           sizeof(bcmEgrObj.key.usp));

    switch (pNhop->wl.cmd)
    {
      case BROAD_L3_NH_CMD_ADD:
      case BROAD_L3_NH_CMD_MODIFY:


        /* This MAC can change if nexthop goes over a tunnel */
        pMac = pNhop->pMac; /* pMac must exists if nhop is resovled */

        if (pNhop->flags & BROAD_L3_NH_TUNNEL)
        {
          /* This nhop has to go over a tunnel, resolve the tunnel nexthop based
           * on the state of its transport v4 nexthop. Note, a tunnel nexthop is
           * reachable only when its v4 transport nexthop is resolved.
           */
          hapiBroadL3AsyncTunnelNhopResolve(pNhop, &pMac, dapi_g);
        }
        else
        {
          /* In other cases, pick the L3 interface ID based on the USP */
                       
          hapiPortPtr = HAPI_PORT_GET(&(pNhop->key.addrUsp.usp), dapi_g);
          pNhop->l3_intf_id = hapiPortPtr->l3_interface_id;
        }

        /* Local next hops should go to CPU */
        if ((pNhop->flags & BROAD_L3_NH_RESOLVED) && 
            !(pNhop->flags & BROAD_L3_NH_LOCAL))
        {
          /* Next hop is resolved. Use the actual L3 interface */
          hapiPortPtr = HAPI_PORT_GET(&(pMac->usp), dapi_g);

          bcmEgrObj.bcm_data.vlan = pMac->key.vlanId; 

          if (pMac->target_is_trunk == L7_TRUE)
          {
            bcmEgrObj.bcm_data.flags |= BCM_L3_TGID;
            bcmEgrObj.bcm_data.trunk = pMac->trunkId; 
          }
          else
          {
            if (BCM_GPORT_IS_WLAN_PORT(hapiPortPtr->bcmx_lport))
            {
              bcmEgrObj.bcm_data.module = hapiPortPtr->bcmx_lport;
              bcmEgrObj.bcm_data.port   = hapiPortPtr->bcmx_lport;
            }
            else
            {
              bcmEgrObj.bcm_data.module = BCM_GPORT_MODPORT_MODID_GET(hapiPortPtr->bcmx_lport);
              bcmEgrObj.bcm_data.port   = BCM_GPORT_MODPORT_PORT_GET(hapiPortPtr->bcmx_lport);
            }
            if ((bcmEgrObj.bcm_data.module == HAPI_BROAD_INVALID_MODID) ||
                (bcmEgrObj.bcm_data.port   == HAPI_BROAD_INVALID_MODPORT))  
            {
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
                      "Failed to get modid/port for lport %x\n",
                      hapiPortPtr->bcmx_lport);
            }
          }
          bcmEgrObj.bcm_data.intf = pNhop->l3_intf_id;
          memcpy(bcmEgrObj.bcm_data.mac_addr, pMac->key.macAddr, sizeof(bcm_mac_t));
        }
        else
        {
          /* Next hop is not resolved or is local. Point it to CPU L3 interface */
          cpuHapiPortPtr = hapiBroadL3CpuHapiPortGet(dapi_g);
          bcmEgrObj.bcm_data.vlan = HPC_STACKING_VLAN_ID;

          bcmEgrObj.bcm_data.module = BCM_GPORT_MODPORT_MODID_GET(cpuHapiPortPtr->bcmx_lport);
          bcmEgrObj.bcm_data.port   = BCM_GPORT_MODPORT_PORT_GET(cpuHapiPortPtr->bcmx_lport);
          if ((bcmEgrObj.bcm_data.module == HAPI_BROAD_INVALID_MODID) ||
              (bcmEgrObj.bcm_data.port   == HAPI_BROAD_INVALID_MODPORT))  
          {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
                    "Failed to get modid/port for lport %x\n",
                    cpuHapiPortPtr->bcmx_lport);
          }
          bcmEgrObj.bcm_data.intf = hapiBroadL3CpuIntfId;
          memcpy(bcmEgrObj.bcm_data.mac_addr, hapiBroadL3CpuMac, sizeof(bcm_mac_t));
        }
  
        if (pNhop->wl.cmd == BROAD_L3_NH_CMD_MODIFY)
        {
          HAPI_BROAD_L3_ASYNC_DBG("Updating Nexthop %d\n", pNhop->egressId);
          flags = (BCM_L3_WITH_ID | BCM_L3_REPLACE);
        }

        /* Remove the next hop from WL */
        HAPI_BROAD_L3_NH_WLIST_DEL(pNhop);
        HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

        hapiBroadL3HwNhopAdd(flags, pNhop, &bcmEgrObj, BROAD_L3_ENTRY_CACHE);
        prevCmd = BROAD_L3_NH_CMD_ADD;
        break;

      case BROAD_L3_NH_CMD_DELETE:

        if (prevCmd == BROAD_L3_NH_CMD_ADD)
        {
          /* There might be some adds pending, give up and commit */
          prevCmd = BROAD_L3_NH_CMD_NO_OP;
          HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
          hapiBroadL3NhopCacheCommit(); 
        }
        else
        {
          egressIntfId = pNhop->egressId;

          /* Check if the MAC needs to be deleted. */
          hapiBroadL3MacNhopUnlink(pNhop); 

          HAPI_BROAD_L3_NH_WLIST_DEL(pNhop);
          if (avlDeleteEntry(&hapiBroadL3NhopTree, pNhop) == L7_NULL)
          {
            HAPI_BROAD_L3_L7_LOG_ERROR(0);
          }
      
          HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
 
          if (egressIntfId != HAPI_BROAD_L3_INVALID_EGR_ID)
          {
            hapiBroadL3HwNhopDelete(&bcmEgrObj, egressIntfId, BROAD_L3_ENTRY_CACHE);
          }
         
          prevCmd = BROAD_L3_NH_CMD_DELETE;
        }
        break;

      default:
        HAPI_BROAD_L3_L7_LOG_ERROR(0);
    }

    HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);  
  } while (hapiBroadL3NhopListHead != L7_NULL);

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

  /* We may have some egress objects cached. Commit them now */
  hapiBroadL3NhopCacheCommit();
}


/*******************************************************************************
*
* @purpose Processes the ECMP entries queued in the ECMP work list
*
* @param   dapi_g   - DAPI Driver object
*
* @returns none
*
* @notes   Instead of using bcmx_l3_egress_multipath_add() and 
*          bcmx_l3_egress_multipath_delete() APIs, the replace flag is used to 
*          update multipath egress objects (single RPC call).
*
* @end
*
*******************************************************************************/
static void hapiBroadL3AsyncEcmpWlistProcess (DAPI_t *dapi_g)
{
  L7_int32               i, rv, intf_count = 0;
  L7_uint32              flags = 0;
  bcm_if_t               intf[L7_RT_MAX_EQUAL_COST_ROUTES] = {0};
  bcm_if_t               egrId;
  BROAD_L3_ECMP_ENTRY_t *pEcmp;

  /* Note: This semaphore must be released before making BCMX/RPC/USL call
   * to avoid blocking the application thread
   */
  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  if (hapiBroadL3EcmpListHead == L7_NULL)
  {
    HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
    return;
  }

  HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3AsyncEcmpWlistProcess has work");

  do
  {
    pEcmp = hapiBroadL3EcmpListHead;
    flags = 0;
    intf_count = 0;    
  
    switch (pEcmp->wl.cmd)
    {
      case BROAD_L3_ECMP_CMD_ADD:
      case BROAD_L3_ECMP_CMD_MODIFY:
        /* Send all resolved nhops (even for modify) */
        for (i=0; i < L7_RT_MAX_EQUAL_COST_ROUTES; i++)
        {
          if ((pEcmp->pNhop[i] != L7_NULL) && 
              (pEcmp->pNhop[i]->flags & BROAD_L3_NH_RESOLVED) &&
              (pEcmp->pNhop[i]->egressId != HAPI_BROAD_INVALID_L3_INTF_ID))
          {
            intf[intf_count] = pEcmp->pNhop[i]->egressId;
            pEcmp->flags &= ~BROAD_L3_ECMP_TO_CPU;  /* Clear CPU flag */
            pEcmp->flags |= BROAD_L3_ECMP_RESOLVED; /* Set resolved flag */
            intf_count++;
          }
        }

        /* If none of the nhops are resolved, point to CPU */
        if ((intf_count == 0) && (!(pEcmp->flags & BROAD_L3_ECMP_TO_CPU)))
        {
          pEcmp->flags &= ~BROAD_L3_ECMP_RESOLVED; /* Clear resolved flag */
          pEcmp->flags |= BROAD_L3_ECMP_TO_CPU;    /* Set CPU flag */
          intf[intf_count] = hapiBroadL3CpuEgrNhopId;
          intf_count++;
        }

        if (pEcmp->wl.cmd == BROAD_L3_ECMP_CMD_MODIFY)
        {
          /* Use the replace flag for ECMP update, instead of adding/deleting
           * individual nexthops. This way, we don't have to maintain whether
           * individual nexthops are already part of ECMP group or not
           */
          HAPI_BROAD_L3_ASYNC_DBG("Updating Mpath object, %d", pEcmp->egressId);
          if (pEcmp->egressId != HAPI_BROAD_L3_INVALID_EGR_ID)
          {
            flags |= (BCM_L3_WITH_ID | BCM_L3_REPLACE);
          }
        }

        /* Remove the ECMP entry from WL */
        HAPI_BROAD_L3_ECMP_WLIST_DEL(pEcmp);
        HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

        if (intf_count > 0)
        {
          rv = usl_bcmx_l3_egress_multipath_create(flags, intf_count, intf, 
                                                   &pEcmp->egressId);

          HAPI_BROAD_L3_BCMX_DBG(rv, "bcmx_l3_egress_multipath_create"
                      " returned (%s) intf count %d, egress id %d \n", 
                       bcm_errmsg(rv), intf_count, pEcmp->egressId);

          HAPI_BROAD_L3_INCR_HW_STATS(&broadL3HwEcmpStats, 
                                      (flags & BCM_L3_REPLACE) ? 1:0, rv);
          pEcmp->rv = rv;
   
          if ((rv != BCM_E_NONE) && ((flags & BCM_L3_REPLACE) == L7_FALSE))
          {
            /* If a create failed, destroy it so that all units are in sync */
            if (pEcmp->egressId != HAPI_BROAD_L3_INVALID_EGR_ID)
            {
              usl_bcmx_l3_egress_multipath_destroy(pEcmp->egressId);
              pEcmp->egressId = HAPI_BROAD_L3_INVALID_EGR_ID;
            }
          }
        }
        else
        {
          /* It is possible that ECMP modify has no work to do. */
          HAPI_BROAD_L3_ASYNC_DBG("Multipath egress create/modify"
                    " skipped %d, flags 0x%x\n", pEcmp->egressId, pEcmp->flags);
        }
        break;

      case BROAD_L3_ECMP_CMD_DELETE:
         egrId = pEcmp->egressId;

         HAPI_BROAD_L3_ECMP_WLIST_DEL(pEcmp);
         if (avlDeleteEntry(&hapiBroadL3EcmpTree, pEcmp) == L7_NULL)
         {
           HAPI_BROAD_L3_L7_LOG_ERROR(0);
         }

         HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

         if (egrId != HAPI_BROAD_L3_INVALID_EGR_ID)
         {
           rv = usl_bcmx_l3_egress_multipath_destroy(egrId);
 
           HAPI_BROAD_L3_BCMX_DBG(rv, "bcmx_l3_egress_multipath_destroy:"
                                  " Id %d (%s)", egrId, bcm_errmsg(rv));

           HAPI_BROAD_L3_INCR_HW_STATS(&broadL3HwEcmpStats, 2, rv);
         }

         break;

      default:
        HAPI_BROAD_L3_L7_LOG_ERROR(0);
    }

    HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);
  } while (hapiBroadL3EcmpListHead != L7_NULL);

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
}

/*******************************************************************************
*
* @purpose Processes the host entries queued in the host work list
*
* @param   dapi_g   - DAPI Driver object
*
* @returns none
*
* @notes none
*
* @end
*
*******************************************************************************/
static void hapiBroadL3AsyncHostWlistProcess (DAPI_t *dapi_g)
{
  L7_BOOL                v6LinkLocal;
  usl_bcm_l3_host_t       bcmHostInfo;
  BROAD_L3_HOST_ENTRY_t *pHost = L7_NULL;
  BROAD_L3_NH_ENTRY_t   *pNhop;
  BROAD_L3_NH_ENTRY_t   *pTunNhop = L7_NULL;

  /* Note: This semaphore must be released before making BCMX/RPC/USL call
   * to avoid blocking the application thread
   */
  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  if (hapiBroadL3HostListHead == L7_NULL)
  {
    HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
    return;
  }

  HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3AsyncHostWlistProcess has work");

  do
  {
    pHost = hapiBroadL3HostListHead;
    pNhop = pHost->pNhop;
    v6LinkLocal = L7_FALSE;
    pTunNhop = L7_NULL;

    if (pNhop == (BROAD_L3_NH_ENTRY_t *)L7_NULL)
    {
      HAPI_BROAD_L3_L7_LOG_ERROR(pHost->wl.cmd); /* Shouldn't happen */
    }

    /* Check if this host goes over tunnel */
    if ((pHost->flags & BROAD_L3_HOST_TUNNEL) &&
        (pHost->pTunNhop != L7_NULL))
    {
      /* Use tunnel next hop instead */
      pTunNhop = pHost->pTunNhop;
      if (pTunNhop->egressId == HAPI_BROAD_L3_INVALID_EGR_ID)
      {
        break;
      }
    }

    /* Check the state of next hop. Nexthop must exists for Host add/del */
    if (pNhop->egressId == HAPI_BROAD_L3_INVALID_EGR_ID)
    { 
      if (pNhop->wl.cmd == BROAD_L3_NH_CMD_ADD)
      {
        /* Most of the time, egress object should be created by this time 
         * If not, break and give nhop wlist a chance
         */
        break; 
      }
      else if (pNhop->rv != BCM_E_NONE)
      {
        /* Here if next hop has error. Cannot process host entry */
        if (pHost->wl.cmd != BROAD_L3_HOST_CMD_DELETE)
        {
          if (pHost->rv == 0)
          { 
            pHost->rv = pNhop->rv;
            broadL3HwHostStats.current_add_failures++;
          }
          HAPI_BROAD_L3_HOST_WLIST_DEL(pHost);
          continue;
        }
        /* For HOST deletes, the entry needs to be removed from AVL tree. */
      }
    }

    /* Init the BCMX host structure and set the host details */
    
    memset(&bcmHostInfo, 0, sizeof(bcmHostInfo));
 
 
    if (pHost->key.family == L7_AF_INET)
    {
       memcpy(&(bcmHostInfo.hostKey.addr.l3a_ip_addr), &(pHost->key.addrUsp.addr.ipv4), sizeof(bcm_ip_t));
    }
    else
    {
       if (L7_IP6_IS_ADDR_LINK_LOCAL(&pHost->key.addrUsp.addr.ipv6) == L7_TRUE)
       {
         v6LinkLocal = L7_TRUE;
       }

       bcmHostInfo.l3a_flags |= BCM_L3_IP6;
       memcpy(&bcmHostInfo.hostKey.addr.l3a_ip6_addr, &(pHost->key.addrUsp.addr.ipv6),
                sizeof (bcm_ip6_t));
    }
    
    switch (pHost->wl.cmd)
    {
      case BROAD_L3_HOST_CMD_ADD:
      case BROAD_L3_HOST_CMD_MODIFY:

        if (pHost->wl.cmd == BROAD_L3_HOST_CMD_MODIFY)
        {
          bcmHostInfo.l3a_flags |= BCM_L3_REPLACE;
        }

        if (pTunNhop != L7_NULL)
        {
          bcmHostInfo.l3a_intf = pTunNhop->egressId;
        }
        else
        {
          bcmHostInfo.l3a_intf = pNhop->egressId;
        }

        if (pHost->flags & BROAD_L3_HOST_LOCAL)
        {
          bcmHostInfo.l3a_flags |= BCM_L3_RPE;
          bcmHostInfo.l3a_pri = HAPI_BROAD_INGRESS_LOCAL_IP_PRIORITY;
        }

        /* Remove the host from WL */
        HAPI_BROAD_L3_HOST_WLIST_DEL(pHost);
        HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
 
        if (v6LinkLocal == L7_TRUE)
        {
          /* Host entries for Link-Local IPv6 addreses are not needed in the
           * hardware because the IPv6 routes already have the interface and
           * MAC resolution and we will never forward to a link-local. All
           * incoming link-locals go to the CPU and all outgoing link-locals
           * originate on the stack and are resolved there.
           */

          /* Note: We need the link-locals in HOST AVL tree, its just that we 
           * don't add to h/w, as we can have IPv6 routes with link-local as
           * next hop. In other words, the egress next hop object is created 
           * for link-locals, but host entry is not.
           */
          HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3AsyncHostWlistProcess:"
                                  " Ignoring IPv6 link local host adds\n");
          break;
        }

        hapiBroadL3HwHostAdd(pHost, &bcmHostInfo, BROAD_L3_ENTRY_CACHE);
        break;

      case BROAD_L3_HOST_CMD_DELETE:

        if (pTunNhop != L7_NULL)
        {
          bcmHostInfo.l3a_intf = pTunNhop->egressId;
          /* Check if next hop needs to be deleted. If host goes over a tunnel
           * delete the tunnel next hop too
           */
          hapiBroadL3NhopEntryUpdate(pTunNhop, BROAD_L3_NH_CMD_DELETE);
        }
        else
        {
          bcmHostInfo.l3a_intf = pNhop->egressId;
        }

        if (pHost->flags & BROAD_L3_HOST_LOCAL)
        {
          /* Reset the local flag on next hop */
          pNhop->flags &= ~BROAD_L3_NH_LOCAL;
        }

        /* Delete the v4 next hop */
        hapiBroadL3NhopEntryUpdate(pNhop, BROAD_L3_NH_CMD_DELETE);
        
        if (pNhop->ref_count > 0)
        {
          /* Implies that next hop is not marked for deletion as there 
           * are routes dependent on this next hop. Update the next hop,
           * so that routes point to CPU.
           */
          hapiBroadL3NhopEntryUpdate(pNhop, BROAD_L3_NH_CMD_MODIFY);
        }

        /* Remove the host from WL and AVL*/
        HAPI_BROAD_L3_HOST_WLIST_DEL(pHost);

        if (pHost->rv != 0)
        {
          broadL3HwHostStats.current_add_failures--;
        }

        if (avlDeleteEntry(&hapiBroadL3HostTree, pHost) == L7_NULL)
        {
          HAPI_BROAD_L3_L7_LOG_ERROR(0);
        }

        HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

        if (v6LinkLocal == L7_TRUE)
        {
          /* As Link locals were not added to h/w, ignore the delete */
          HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3AsyncHostWlistProcess:"
                                  " Ignoring IPv6 link local host deletes\n");
          break;
        }

        hapiBroadL3HwHostDelete(&bcmHostInfo, BROAD_L3_ENTRY_CACHE);
        if (broadL3HwHostStats.current_add_failures > 0)
        {
          /* A host got deleted, so there might be some room for failed hosts.
           * Set the retry flag.
           */
          retryFailedHosts = L7_TRUE;
        }
        break;

      case BROAD_L3_HOST_CMD_ASYNC_FAIL_HANDLE:
        /* This host was not actually added to the HW by USL. Set this host up for retry. */
        HAPI_BROAD_L3_HOST_WLIST_DEL(pHost);
        broadL3HwHostStats.current_add_failures++;
        pHost->rv = BCM_E_FULL;

        HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

        /* Make sure to delete from all units in case the entry made it into 
           some units but not all of them. */
        hapiBroadL3HwHostDelete(&bcmHostInfo, BROAD_L3_ENTRY_CACHE);

        break;
 
      default: 
        HAPI_BROAD_L3_L7_LOG_ERROR(0);
    }

    HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER); 

  } while (hapiBroadL3HostListHead != L7_NULL);

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

  /* We may have some host entries cached. Commit them now */
  hapiBroadL3HostCacheCommit();

}

/*******************************************************************************
*
* @purpose  Unlinks Nhop entry from a MAC entry 
*
* @param  pNhopEntry - Next hop entry
*
* @returns L7_RC_t result
*
* @notes  MAC entry is deleted if no one is referencing the entry.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3MacNhopUnlink (BROAD_L3_NH_ENTRY_t *pNhopEntry)
{
  BROAD_L3_NH_ENTRY_t *pTmp, *pPrev;
  BROAD_L3_MAC_ENTRY_t  *pMacEntry;

  pMacEntry = pNhopEntry->pMac;
  
  if (pMacEntry == L7_NULL)
  {
    return L7_SUCCESS;
  }

  HAPI_BROAD_L3_ASYNC_DBG("**** hapiBroadL3MacNhopUnlink ****\n");

  /* Check if there are multiple next hops dependent on this MAC */
  pTmp = pMacEntry->nHopList;
  if (pTmp == pNhopEntry)
  {
    /* Unlink the given next hop entry */
    pMacEntry->nHopList = pNhopEntry->pMacNhopNext;
  }
  else
  {
    /* Search the list of next hops and unlink the given next hop entry */
    while (pTmp != L7_NULL)
    {
      pPrev = pTmp;
      pTmp = pTmp->pMacNhopNext;
      if (pTmp == pNhopEntry)
      {
        pPrev->pMacNhopNext = pNhopEntry->pMacNhopNext;
        break;
      }
    }
     
    if (pTmp == L7_NULL) /* Nhop was not found in the MAC list */
    {
      HAPI_BROAD_L3_L7_LOG_ERROR(0);
    }
  } 
  
  pNhopEntry->pMac = L7_NULL;
  pNhopEntry->pMacNhopNext = L7_NULL;

  /* Delete MAC only when there are no nexthops and tunnel hosts linked to it */
  if ((pMacEntry->nHopList == L7_NULL) && (pMacEntry->pTunnelEntry == L7_NULL) &&
      (pMacEntry->hostList == L7_NULL))
  {
    HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3MacNhopUnlink: Deleting MAC AVL entry\n");
    pTmp = avlDeleteEntry(&hapiBroadL3MacTree, pMacEntry);
    if (pTmp == L7_NULL)
    {
      HAPI_BROAD_L3_L7_LOG_ERROR(0);
    }
  }
 
  return L7_SUCCESS;
}

/*******************************************************************************
*
* @purpose Processes the route entries queued in the route work list
*
* @param   dapi_g   - DAPI Driver object
*
* @returns none
*
* @notes none
*
* @end
*
*******************************************************************************/
static void hapiBroadL3AsyncRouteWlistProcess (DAPI_t *dapi_g)
{
  L7_uint8               i;
  L7_BOOL                is128special;
  L7_int32               rv;
  usl_bcm_l3_route_t      bcmRouteInfo;
  usl_bcm_l3_host_t       bcmHostInfo;      
  BROAD_L3_ROUTE_ENTRY_t *pRoute = L7_NULL;
  BROAD_L3_NH_ENTRY_t    *pNhop;
  BROAD_L3_ECMP_ENTRY_t  *pEcmp;

  /* Note: This semaphore must be released before making BCMX/RPC/USL call
   * to avoid blocking the application thread
   */
  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  if (hapiBroadL3RouteListHead == L7_NULL)
  {
    HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
    return;
  }

  HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3AsyncRouteWlistProcess has work");
  do 
  {
    pRoute = hapiBroadL3RouteListHead;
    pNhop  = pRoute->pNhop;
    pEcmp  = pRoute->pEcmp;

    is128special  = L7_FALSE;

    memset(&bcmHostInfo, 0, sizeof(bcmHostInfo));
    memset(&bcmRouteInfo, 0, sizeof(bcmRouteInfo));

    if (pRoute->key.family == L7_AF_INET6)
    {
      bcmRouteInfo.l3a_flags |= BCM_L3_IP6;
      memcpy(&(bcmRouteInfo.routeKey.l3a_subnet.l3a_ip6_net),  &pRoute->key.ipAddr, sizeof(bcm_ip6_t));
      memcpy(&(bcmRouteInfo.routeKey.l3a_mask.l3a_ip6_mask), &pRoute->key.ipMask, sizeof(bcm_ip6_t));
    
      /* Check if route is 128-bit as it might require some special handling */
      if(memcmp(&pRoute->key.ipMask, hapiBroadL3Ipv6Mask128,
                sizeof(bcm_ip6_t)) == 0)
      {
        if (hapiBroadL3Ipv6Prefix128Supported() == L7_FALSE)
        {
          is128special = L7_TRUE;  /* Route will be added as a host entry */
          bcmHostInfo.l3a_flags |= BCM_L3_IP6;
          memcpy(&(bcmHostInfo.hostKey.addr.l3a_ip6_addr), &(bcmRouteInfo.routeKey.l3a_subnet.l3a_ip6_net),
                 sizeof (bcm_ip6_t));
        }
      }
    }
    else
    {
      memcpy(&(bcmRouteInfo.routeKey.l3a_subnet.l3a_ip_net), &(pRoute->key.ipAddr.ipv4), sizeof(bcm_ip_t));
      memcpy(&(bcmRouteInfo.routeKey.l3a_mask.l3a_ip_mask), &(pRoute->key.ipMask.ipv4), sizeof(bcm_ip_t));

      /* We don't need to set 'defip_nexthop_ip'. XGS/XGS2 only field and those
       * devices are not supported
       */
    }

    /* Pre-check the state of next hops */
    if (BROAD_L3_IS_ROUTE_ECMP(pRoute))
    {
      /* ECMP route */
      if (pEcmp == (BROAD_L3_ECMP_ENTRY_t *)L7_NULL)
      {
        HAPI_BROAD_L3_L7_LOG_ERROR(pRoute->wl.cmd);
      }

      if (pEcmp->egressId == HAPI_BROAD_L3_INVALID_EGR_ID)
      {
        if (pEcmp->wl.cmd == BROAD_L3_ECMP_CMD_ADD)
        {
          /* Most of the time, egress object should be created by this time
           * If not, break and give ECMP wlist a chance
           */
          break;
        }
        else if (pEcmp->rv != BCM_E_NONE)
        {
          /* Here if next hop has error. Cannot process route entry */
          if (pRoute->wl.cmd != BROAD_L3_ROUTE_CMD_DELETE)
          {
            if (pRoute->rv == 0)
            {
              pRoute->rv = pEcmp->rv;
              broadL3HwRouteStats.current_add_failures++;
            }
            HAPI_BROAD_L3_ROUTE_WLIST_DEL(pRoute);
            continue;
          }
          /* For delete, we still have to clear the route AVL entry */
        }
      }
    }
    else if (BROAD_L3_IS_ROUTE_NON_ECMP(pRoute))
    {
      /* Non-ECMP route must be next hop info */
      if (pNhop == (BROAD_L3_NH_ENTRY_t *)L7_NULL)
      {
         HAPI_BROAD_L3_L7_LOG_ERROR(0);
      }
  
      if (pNhop->egressId == HAPI_BROAD_L3_INVALID_EGR_ID)
      {
        if (pNhop->wl.cmd == BROAD_L3_NH_CMD_ADD)
        {
          /* Most of the time, egress object should be created by this time
           * If not, break and give nhop wlist a chance
           */
          break;
        }
        else if (pNhop->rv != BCM_E_NONE)
        {
          /* Here if next hop has error. Cannot process route entry */
          if (pRoute->wl.cmd != BROAD_L3_ROUTE_CMD_DELETE)
          {
            if (pRoute->rv == 0)
            {
              pRoute->rv = pNhop->rv;
              broadL3HwRouteStats.current_add_failures++;
            }
            HAPI_BROAD_L3_ROUTE_WLIST_DEL(pRoute);
            continue;
          }
          /* For delete, we have to clear the route AVL entry */
        }
      }
    }

    /* Process the command */
    switch (pRoute->wl.cmd)
    {
      case BROAD_L3_ROUTE_CMD_ADD:
        if (BROAD_L3_IS_ROUTE_ECMP(pRoute))
        {
          bcmRouteInfo.l3a_intf = pEcmp->egressId;  
          bcmRouteInfo.l3a_flags |= BCM_L3_MULTIPATH;
        }
        else 
        {
          /* Point the route to the egress object */
          if ((pNhop == L7_NULLPTR) || (pRoute->numNextHops == 0)) 
          {
            /* Reject route */
            bcmRouteInfo.l3a_intf = hapiBroadL3CpuEgrNhopId;
          }
          else
          {
            bcmRouteInfo.l3a_intf = pNhop->egressId;
          }
        }

        /* Remove the route from WL */
        HAPI_BROAD_L3_ROUTE_WLIST_DEL(pRoute);
        HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

        /* Check for 128-bit prefix routes - Special case */
        if (is128special == L7_TRUE)
        {
          bcmHostInfo.l3a_intf = bcmRouteInfo.l3a_intf;

          usl_bcmx_l3_host_add (&bcmHostInfo, 1, &rv);
     
          HAPI_BROAD_L3_BCMX_DBG(rv, "128-bit prefix special case: "
                    " bcmx_l3_host_add returned (%s)\n", bcm_errmsg(rv));
        }
        else
        {
          hapiBroadL3HwRouteAdd(pRoute, &bcmRouteInfo, BROAD_L3_ENTRY_CACHE);
        }

        break;

      case BROAD_L3_ROUTE_CMD_MODIFY:

        if (BROAD_L3_IS_ROUTE_ECMP(pRoute))
        {
          bcmRouteInfo.l3a_flags |= BCM_L3_MULTIPATH;
          bcmRouteInfo.l3a_intf = pEcmp->egressId;
        }
        else if (BROAD_L3_IS_ROUTE_NON_ECMP(pRoute))
        {
          bcmRouteInfo.l3a_intf = pNhop->egressId; /* Got to be valid */
        }
        else
        {
          /* Reject route */
          bcmRouteInfo.l3a_intf = hapiBroadL3CpuEgrNhopId;
        }

        /* Add the route with replace flag set. Effectively the existing entry
         * updated.
         */
        bcmRouteInfo.l3a_flags |= BCM_L3_REPLACE;
        /* Remove the route from WL */
        HAPI_BROAD_L3_ROUTE_WLIST_DEL(pRoute);
        HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

        hapiBroadL3HwRouteAdd(pRoute, &bcmRouteInfo, BROAD_L3_ENTRY_CACHE);

        break;

      case BROAD_L3_ROUTE_CMD_DELETE:
        if (BROAD_L3_IS_ROUTE_ECMP(pRoute))
        {
          bcmRouteInfo.l3a_flags |= BCM_L3_MULTIPATH;
          bcmRouteInfo.l3a_intf = pEcmp->egressId;
          for (i=0; i < L7_RT_MAX_EQUAL_COST_ROUTES; i++)
          {
            if (pEcmp->pNhop[i] != L7_NULL)
            {
              hapiBroadL3NhopEntryUpdate(pEcmp->pNhop[i], BROAD_L3_NH_CMD_DELETE);
            }
          }

          /* See if ECMP needs to be deleted */
          hapiBroadL3EcmpEntryUpdate(pEcmp, BROAD_L3_ECMP_CMD_DELETE);
        } 
        else /* Non-ECMP route or reject route */
        {
          if ((pNhop == L7_NULLPTR) || (pRoute->numNextHops == 0))
          {
            bcmRouteInfo.l3a_intf = hapiBroadL3CpuEgrNhopId;
          }
          else
          {
            bcmRouteInfo.l3a_intf = pNhop->egressId; /* Got to be valid */

            /* See if next hop needs to be deleted */
            if (IS_USP_TYPE_TUNNEL(&pNhop->key.addrUsp.usp))
            {
              /*  nexthop is on a tunnel */
              hapiBroadL3AsyncTunnelNhopDelete(pNhop, dapi_g);
            }
            else
            {
              hapiBroadL3NhopEntryUpdate(pNhop, BROAD_L3_NH_CMD_DELETE);
            }
          }
        }

        /* Remove the route from WL and AVL tree */
        HAPI_BROAD_L3_ROUTE_WLIST_DEL(pRoute);

        if (pRoute->rv != 0)
        {
          broadL3HwRouteStats.current_add_failures--;
        }

        if (avlDeleteEntry(&hapiBroadL3RouteTree, pRoute) == L7_NULL)
        {
          HAPI_BROAD_L3_L7_LOG_ERROR(0);
        }

        HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

        /* Check for 128-bit prefix, as we added a host instead */
        if (is128special == L7_TRUE)
        {
          bcmHostInfo.l3a_intf = bcmRouteInfo.l3a_intf;
          usl_bcmx_l3_host_delete (&bcmHostInfo, 1, &rv);
        }
        else
        {
          hapiBroadL3HwRouteDelete(&bcmRouteInfo, BROAD_L3_ENTRY_CACHE);
          if (broadL3HwRouteStats.current_add_failures > 0)
          {
            /* A route got deleted, so there might be some room for failed 
             * routes. Set the retry flag.
             */
            retryFailedRoutes = L7_TRUE;
          }
        }

        break;

      default:
        HAPI_BROAD_L3_L7_LOG_ERROR(0); 
    }
  
    HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);  
  } while (hapiBroadL3RouteListHead != L7_NULL);

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

  /* We may have some route entries cached. Commit them now */
  hapiBroadL3RouteCacheCommit();
}


/*******************************************************************************
*
* @purpose Processes the tunnel entries queued in the tunnel work list
*
* @param   dapi_g   - DAPI Driver object
*
* @returns none
*
* @notes none
*
* @end
*
*******************************************************************************/
static void hapiBroadL3AsyncTunnelWlistProcess (DAPI_t *dapi_g)
{
  BROAD_L3_TUNNEL_ENTRY_t *pTunEntry;
  L7_BOOL                  replace = L7_FALSE;

  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  if (hapiBroadL3TunnelListHead == L7_NULL)
  {
    HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
    return;
  }

  do
  {
    pTunEntry = hapiBroadL3TunnelListHead;
    replace = L7_FALSE;

    HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3AsyncTunnelWlistProcess has work");
    switch (pTunEntry->wl.cmd)
    {
      case BROAD_L3_TUNNEL_CMD_CREATE:
      case BROAD_L3_TUNNEL_CMD_NHOP_SET: 

        /* Treat next hop set just as create, but with replace flags */
        if (pTunEntry->wl.cmd == BROAD_L3_TUNNEL_CMD_NHOP_SET)
        {
          /* For tunnel nexthop set, the nhop can move from one routing intf
           * to another. Implies outgoing VLAN changes, so tunnel's L3 intf 
           * is affected. Nexthop MAC address can change, so tunnel's initiator
           * must be reprogrammed. Tunnel terminator is affected if tunnel's L3
           * interface is modified.
           * So, init, term and intf must be replaced. 
           */
           replace = L7_TRUE;
        }

        hapiBroadL3AsyncTunnelCreate (pTunEntry, replace, dapi_g); 
        HAPI_BROAD_L3_TUNNEL_WLIST_DEL(pTunEntry);
        break;

      case BROAD_L3_TUNNEL_CMD_MODIFY:
        /* Tunnel's Xport next hop got resolved or unresolved. Update initiator.
         * Note, Tunnel modify command is invoked only when nexthop resolution
         * changes. In other words, none of the tunnel parameters are changed
         */ 

        hapiBroadL3AsyncTunnelModify (pTunEntry);
        HAPI_BROAD_L3_TUNNEL_WLIST_DEL(pTunEntry);
        break;

      case BROAD_L3_TUNNEL_CMD_DELETE:

        hapiBroadL3AsyncTunnelDelete (pTunEntry);

        /* Remove the tunnel from WL and AVL tree */
        HAPI_BROAD_L3_TUNNEL_WLIST_DEL(pTunEntry);
        if (avlDeleteEntry(&hapiBroadL3TunnelTree, pTunEntry) == L7_NULL)
        {
          HAPI_BROAD_L3_L7_LOG_ERROR(0);
        }
        break;

      default:
        HAPI_BROAD_L3_L7_LOG_ERROR(pTunEntry->wl.cmd);
    }

  } while (hapiBroadL3TunnelListHead != L7_NULL);

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
}


/*******************************************************************************
*
* @purpose Create/replace tunnel interface, terminator and initiator in hardware
*
* @param   pTunnelEntry - pointer to tunnel AVL node
* @param   replace      - L7_FALSE creates new entry, L7_TRUE replaces entry.
* @param   dapi_g       - DAPI driver object
*
* @returns none
*
* @notes none
*
* @end
*
*******************************************************************************/
static void hapiBroadL3AsyncTunnelCreate (BROAD_L3_TUNNEL_ENTRY_t *pTunEntry,
                                          L7_BOOL replace,
                                          DAPI_t *dapi_g)
{
  L7_int32                 rv;
  L7_uint32                vlanId  = 0;
  DAPI_PORT_t             *nhDapiPortPtr;
  BROAD_PORT_t            *nhHapiPortPtr;
  usl_bcm_l3_intf_t        intfInfo;
  bcm_tunnel_terminator_t  termInfo;
  bcm_tunnel_initiator_t   initInfo;
  bcm_mac_t                mac_addr;
  BROAD_L3_NH_ENTRY_t     *pNhop;

  HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3AsyncTunnelCreate: Create/Replace %d", 
                          replace);  

  /* Common tunnel init */
  usl_bcm_l3_intf_t_init(&intfInfo);
  bcm_tunnel_terminator_t_init(&termInfo);
  bcm_tunnel_initiator_t_init(&initInfo);

  /* For terminator and initiator, SIP,DIP values are otherway around */
  if (pTunEntry->key.tunnelMode == L7_TUNNEL_MODE_6TO4)
  {
    termInfo.sip = 0;
    termInfo.sip_mask = 0;
  }
  else
  {
    termInfo.sip = osapiNtohl(pTunEntry->key.remoteAddr.u.sa4.sin_addr.s_addr);
    termInfo.sip_mask = 0xffffffff;
  }

  termInfo.dip = osapiNtohl(pTunEntry->key.localAddr.u.sa4.sin_addr.s_addr);
  termInfo.dip_mask = 0xffffffff;
  hapiBroadL3BcmTunnelType(pTunEntry->key.tunnelMode, &termInfo.type);

  initInfo.sip = osapiNtohl(pTunEntry->key.localAddr.u.sa4.sin_addr.s_addr);
  initInfo.dip = osapiNtohl(pTunEntry->key.remoteAddr.u.sa4.sin_addr.s_addr);
  initInfo.ttl = 255;
  initInfo.type = termInfo.type;

  /* Populate the tunnel fields for L3 intf. */
  intfInfo.tunnelInfo.sip = initInfo.sip;
  intfInfo.tunnelInfo.dip = initInfo.dip;
  intfInfo.tunnelInfo.type = initInfo.type;

  if (replace == L7_TRUE)
  {
    intfInfo.bcm_data.l3a_intf_id = pTunEntry->bcmL3IntfId; /* Got to be valid */

    rv = usl_bcmx_l3_intf_get(&intfInfo); /* Intf must exists */
    if (rv != BCM_E_NONE)
    {
      HAPI_BROAD_L3_L7_LOG_ERROR(rv);
    }

    intfInfo.bcm_data.l3a_flags |= (BCM_L3_REPLACE | BCM_L3_WITH_ID);
    termInfo.flags |= BCM_TUNNEL_REPLACE;
    initInfo.flags |= BCM_TUNNEL_REPLACE;
  }

  /* Create/replace L3 Tunnel Interface */
  nhDapiPortPtr = DAPI_PORT_GET(&pTunEntry->nextHopUsp, dapi_g);
  nhHapiPortPtr = HAPI_PORT_GET(&pTunEntry->nextHopUsp, dapi_g);

  /* Pick MAC/VLAN from outgoing routing interface which is identified by
   * Xport's USP
   */
  if (IS_PORT_TYPE_PHYSICAL(nhDapiPortPtr) == L7_TRUE)
  {
    memcpy((void *)&mac_addr, nhDapiPortPtr->modeparm.physical.macAddr,
           sizeof(mac_addr));
    vlanId = nhHapiPortPtr->port_based_routing_vlanid;
  }
  else if (IS_PORT_TYPE_LOGICAL_VLAN(nhDapiPortPtr) == L7_TRUE)
  {
    memcpy((void *)&mac_addr, nhDapiPortPtr->modeparm.router.macAddr,
          sizeof(mac_addr));
    vlanId = nhDapiPortPtr->modeparm.router.vlanID;
  }
  else
  {
    HAPI_BROAD_L3_L7_LOG_ERROR(nhDapiPortPtr->type);
  }

  intfInfo.bcm_data.l3a_vid = vlanId;
  memcpy(&intfInfo.bcm_data.l3a_mac_addr, mac_addr, sizeof(bcm_mac_t));

  /* Create or replace tunnel's L3 interface */
  rv = usl_bcmx_l3_intf_create(&intfInfo);

  HAPI_BROAD_L3_BCMX_DBG(rv, "Tunnel usl_bcmx_l3_intf_create returned (%s),"
                       "L3 intf id %d\n", bcm_errmsg(rv), intfInfo.bcm_data.l3a_intf_id);

  if (rv == BCM_E_NONE)
  {
    pTunEntry->bcmL3IntfId = intfInfo.bcm_data.l3a_intf_id;
  }
  else
  {
    pTunEntry->rv = rv;
  }

  /* Create/replace L3 Tunnel terminator */
  if (pTunEntry->bcmL3IntfId != HAPI_BROAD_INVALID_L3_INTF_ID)
  {
    termInfo.vlan = pTunEntry->bcmL3IntfId;

    rv = usl_bcmx_tunnel_terminator_add(&termInfo);

    HAPI_BROAD_L3_BCMX_DBG(rv, "usl_bcmx_l3_tunnel_terminator_add returned (%s)"
                           " flags 0x%x\n", bcm_errmsg(rv), termInfo.flags);

    /* HACK ALERT !! */
    if ((pTunEntry->key.tunnelMode == L7_TUNNEL_MODE_6TO4) &&
        (rv == BCM_E_UNAVAIL))
    {
      usl_bcmx_tunnel_terminator_delete(&termInfo);

      /* Handle the devices where secured 6to4 tunnels are not supported */
      termInfo.type = bcmTunnelType6In4Uncheck;
      initInfo.type = bcmTunnelType6In4Uncheck;
      rv = usl_bcmx_tunnel_terminator_add(&termInfo);
      HAPI_BROAD_L3_BCMX_DBG(rv, "usl_bcmx_l3_tunnel_terminator_add forcing "
                             "mode to unsecure type (%s)\n", bcm_errmsg(rv));
    }

    if (rv == BCM_E_NONE)
    {
      pTunEntry->hasTerminator = L7_TRUE;
    }
    else
    {
      pTunEntry->rv = rv;
    }

    /* For 6over4, Initiator can be set only when Xport nexthop is resolved. 
     * NOTE: For unicast routed packets, the MAC address is picked from tunnel
     * nexthop [EGR_L3_NEXT_HOP table]. The MAC address that we set here is used
     * for multicast packets going over a tunnel, as for multicast routing next
     * hop tables are not used.
     */
    if ((pTunEntry->key.tunnelMode == L7_TUNNEL_MODE_6OVER4) ||
        (pTunEntry->key.tunnelMode == L7_TUNNEL_MODE_4OVER4))
    {
      pNhop = pTunEntry->pXportNhop;
      if ((pNhop != L7_NULL) && (pNhop->flags & BROAD_L3_NH_RESOLVED))
      {
        memcpy(&initInfo.dmac, pNhop->pMac->key.macAddr, sizeof(bcm_mac_t));

        rv = usl_bcmx_tunnel_initiator_set(&(intfInfo.bcm_data), &initInfo);
        HAPI_BROAD_L3_BCMX_DBG(rv, "usl_bcmx_l3_tunnel_initiator_set returned (%s)"
                               " Flags 0x%x\n", bcm_errmsg(rv), initInfo.flags);

        if (rv == BCM_E_NONE)
        {
          pTunEntry->hasInitiator = L7_TRUE;
        }
        else
        {
          pTunEntry->rv = rv;
        }
      }
    }
    else if (pTunEntry->key.tunnelMode == L7_TUNNEL_MODE_6TO4)
    {
      memset(&initInfo.dmac, 0, sizeof(bcm_mac_t));
      rv = usl_bcmx_tunnel_initiator_set(&(intfInfo.bcm_data), &initInfo);
      HAPI_BROAD_L3_BCMX_DBG(rv, "usl_bcmx_l3_tunnel_initiator_set returned (%s)\n",
                             bcm_errmsg(rv));
 
      if (rv == BCM_E_NONE)
      {
        pTunEntry->hasInitiator = L7_TRUE;
      }
      else
      {
        pTunEntry->rv = rv;
      }
    }
  }

  /* If tunnel resolves routes/hosts, update the tunnel nexthop */
  if (pTunEntry->pTunnelNhop != L7_NULL)
  {
    hapiBroadL3NhopEntryUpdate(pTunEntry->pTunnelNhop,
                                BROAD_L3_NH_CMD_MODIFY);
  }
}

/*******************************************************************************
*
* @purpose Modify the tunnel initiator in hardware
*
* @param   pTunnelEntry - pointer to tunnel AVL node
*
* @returns none
*
* @notes   Only initiator is updated
*
* @end
*
*******************************************************************************/
static void hapiBroadL3AsyncTunnelModify (BROAD_L3_TUNNEL_ENTRY_t *pTunEntry)
{
  L7_uint8                index = 0;
  L7_int32                rv;
  BROAD_L3_NH_ENTRY_t    *pNhop;
  usl_bcm_l3_intf_t       intfInfo;
  bcm_tunnel_initiator_t  initInfo;

  usl_bcm_l3_intf_t_init(&intfInfo);
  bcm_tunnel_initiator_t_init(&initInfo);

  if (pTunEntry->key.tunnelMode == L7_TUNNEL_MODE_6TO4)
  {
    /* If this tunnel next hop is relay, update initiator - TBD */

    /* Update the tunnel next hops that are flagged */
    for (index=0; index < HAPI_BROAD_L3_MAX_6TO4_NHOP; index++)
    {
      pNhop = pTunEntry->tun6to4Nhops[index].pV6Nhop;
      if (pNhop != L7_NULL)
      {
        if (pTunEntry->tun6to4Nhops[index].update == L7_TRUE)
        {
          pTunEntry->tun6to4Nhops[index].update = L7_FALSE;
          hapiBroadL3NhopEntryUpdate(pNhop, BROAD_L3_NH_CMD_MODIFY);
        }
      }
    }
  }
  else if ((pTunEntry->key.tunnelMode == L7_TUNNEL_MODE_6OVER4) ||
           (pTunEntry->key.tunnelMode == L7_TUNNEL_MODE_4OVER4))
  {
    /* Update the tunnel based on transport next hop */
    pNhop = pTunEntry->pXportNhop;
    intfInfo.bcm_data.l3a_intf_id = pTunEntry->bcmL3IntfId;

    initInfo.sip = osapiNtohl(pTunEntry->key.localAddr.u.sa4.sin_addr.s_addr);
    initInfo.dip = osapiNtohl(pTunEntry->key.remoteAddr.u.sa4.sin_addr.s_addr);
    initInfo.ttl = 255;
    hapiBroadL3BcmTunnelType(pTunEntry->key.tunnelMode, &initInfo.type);

    if ((pNhop != L7_NULL) && (pNhop->flags & BROAD_L3_NH_RESOLVED))
    {
      /* Xport Nhop is resolved, set the intitiator. Use replace flag to
       * override existing entry.
       */
      memcpy(&initInfo.dmac, pNhop->pMac->key.macAddr, sizeof(bcm_mac_t));
      initInfo.flags |= BCM_TUNNEL_REPLACE;

      rv = usl_bcmx_tunnel_initiator_set(&(intfInfo.bcm_data), &initInfo);

      HAPI_BROAD_L3_BCMX_DBG(rv, "usl_bcmx_l3_tunnel_initiator_set returned (%s)\n",
                           bcm_errmsg(rv));

      if (rv == BCM_E_NONE)
      {
        pTunEntry->hasInitiator = L7_TRUE;
      }
    }
    else if (pTunEntry->hasInitiator == L7_TRUE)
    {
      /* Xport nexthop is unresolved now. If initiator was set, clear it */
      pTunEntry->hasInitiator = L7_FALSE;
      rv = usl_bcmx_tunnel_initiator_clear(&(intfInfo.bcm_data));

      HAPI_BROAD_L3_BCMX_DBG(rv, "usl_bcmx_tunnel_initiator_clear returned (%s)\n",
                             bcm_errmsg(rv));
    }

    /* If tunnel resolves routes/hosts, update the tunnel nexthop */
    pNhop = pTunEntry->pTunnelNhop;
    if (pNhop != L7_NULL)
    {
      /* This will take care of modifying the routes/hosts going over tunnel */
      hapiBroadL3NhopEntryUpdate(pNhop, BROAD_L3_NH_CMD_MODIFY);
    }
  }
}


/*******************************************************************************
*
* @purpose Delete tunnel interface, terminator and initiator from hardware
*
* @param   pTunnelEntry - pointer to tunnel AVL node
*
* @returns none
*
* @notes none
*
* @end
*
*******************************************************************************/
static void hapiBroadL3AsyncTunnelDelete (BROAD_L3_TUNNEL_ENTRY_t *pTunEntry)
{
  L7_int32              rv;
  BROAD_L3_NH_ENTRY_t  *pNhop;
  usl_bcm_l3_intf_t     intfInfo;
  bcm_tunnel_terminator_t termInfo;
  bcm_tunnel_initiator_t  initInfo;

  /* Common tunnel init */
  usl_bcm_l3_intf_t_init(&intfInfo);
  bcm_tunnel_terminator_t_init(&termInfo);
  bcm_tunnel_initiator_t_init(&initInfo);

  /* For terminator and initiator, SIP,DIP values are otherway around */
  if (pTunEntry->key.tunnelMode == L7_TUNNEL_MODE_6TO4)
  {
    termInfo.sip = 0;
    termInfo.sip_mask = 0;
  }
  else
  {
    termInfo.sip = osapiNtohl(pTunEntry->key.remoteAddr.u.sa4.sin_addr.s_addr);
    termInfo.sip_mask = 0xffffffff;
  }

  termInfo.dip = osapiNtohl(pTunEntry->key.localAddr.u.sa4.sin_addr.s_addr);
  termInfo.dip_mask = 0xffffffff;
  hapiBroadL3BcmTunnelType(pTunEntry->key.tunnelMode, &termInfo.type);

  initInfo.sip = osapiNtohl(pTunEntry->key.localAddr.u.sa4.sin_addr.s_addr);
  initInfo.dip = osapiNtohl(pTunEntry->key.remoteAddr.u.sa4.sin_addr.s_addr);
  initInfo.ttl = 255;
  initInfo.type = termInfo.type;

  /* Populate the tunnel interface fields */
  intfInfo.tunnelInfo.sip = initInfo.sip;
  intfInfo.tunnelInfo.dip = initInfo.dip;
  intfInfo.tunnelInfo.type = initInfo.type;

  /* See if next hop needs to be deleted */
  pNhop = pTunEntry->pXportNhop;
  if (pNhop != L7_NULL)
  {
    pTunEntry->pXportNhop = L7_NULL; /* Do this before deleting nhop */
    hapiBroadL3NhopEntryUpdate(pNhop, BROAD_L3_NH_CMD_DELETE);
  }

  pNhop = pTunEntry->pTunnelNhop;
  if (pNhop != L7_NULL)
  {
    /* Tunnel is being deleted, so all routes/hosts going over tunnel will
     * be deleted too. So, we don't need to update the nexthop to
     * point to CPU. Just mark it for deletion.
     */
    pNhop->flags &= ~BROAD_L3_NH_TUNNEL;
    hapiBroadL3NhopEntryUpdate(pNhop, BROAD_L3_NH_CMD_DELETE);
  }

  intfInfo.bcm_data.l3a_intf_id = pTunEntry->bcmL3IntfId;

  if (pTunEntry->hasInitiator == L7_TRUE)
  {
    pTunEntry->hasInitiator = L7_FALSE;
    rv = usl_bcmx_tunnel_initiator_clear(&(intfInfo.bcm_data));
    HAPI_BROAD_L3_BCMX_DBG(rv, "usl_bcmx_tunnel_initiator_clear returned (%s)\n",
                           bcm_errmsg(rv));
  }

  if (pTunEntry->hasTerminator == L7_TRUE)
  {
    pTunEntry->hasTerminator = L7_FALSE;
    termInfo.vlan = pTunEntry->bcmL3IntfId;
    rv = usl_bcmx_tunnel_terminator_delete(&termInfo);
    HAPI_BROAD_L3_BCMX_DBG(rv, "usl_bcmx_tunnel_terminator_delete returned (%s)\n",
                           bcm_errmsg(rv));
  }

  rv = usl_bcmx_l3_intf_delete(&intfInfo);
  pTunEntry->bcmL3IntfId = HAPI_BROAD_INVALID_L3_INTF_ID;

  HAPI_BROAD_L3_BCMX_DBG(rv, "Tunnel: usl_bcmx_l3_intf_delete returned (%s)\n", 
                         bcm_errmsg(rv));

}

/*******************************************************************************
*
* @purpose Resolve tunnel nexthop based on its v4 transport next hop
*
* @param   pTunNhop - pointer to tunnel next hop
* @param   pMac - pointer to the MAC address of transport nexthop
* @returns none
*
* @notes none
*
* @end
*
*******************************************************************************/
static void hapiBroadL3AsyncTunnelNhopResolve(BROAD_L3_NH_ENTRY_t *pTunNhop, 
                                              BROAD_L3_MAC_ENTRY_t **pMac,
                                              DAPI_t *dapi_g)
{
  L7_uint8  index;
  L7_int32                 rv;
  L7_uint32                vlanId  = 0;
  BROAD_PORT_t            *hapiPortPtr;
  DAPI_PORT_t             *nhDapiPortPtr;
  BROAD_PORT_t            *nhHapiPortPtr;
  BROAD_L3_NH_ENTRY_t     *pV4Nhop = L7_NULL;
  BROAD_L3_TUNNEL_KEY_t    zero;
  BROAD_L3_TUNNEL_ENTRY_t *pTunnelEntry = L7_NULL;
  BROAD_L3_6TO4_NHOP_t    *p6to4 = L7_NULL;
  bcm_mac_t                mac_addr;
  usl_bcm_l3_intf_t       intfInfo;
  bcm_tunnel_initiator_t  initInfo;
  bcm_ip_t                 v4Addr; 

  HAPI_BROAD_L3_ASYNC_DBG("***hapiBroadL3AsyncTunnelNhopResolve***");
  usl_bcm_l3_intf_t_init(&intfInfo);
  bcm_tunnel_initiator_t_init(&initInfo);

  /* Set the default to not resolved */
  pTunNhop->flags &= ~BROAD_L3_NH_RESOLVED; 

  /* Check for  6o4 and 6to4 tunnels */
  if (IS_USP_TYPE_TUNNEL(&pTunNhop->key.addrUsp.usp))
  {
    hapiPortPtr = HAPI_PORT_GET(&(pTunNhop->key.addrUsp.usp), dapi_g);
    pTunnelEntry = hapiPortPtr->hapiModeparm.pTunnel;

    if (pTunnelEntry->key.tunnelMode == L7_TUNNEL_MODE_6OVER4)
    {
      /* 6over4 tunnels are point-to-point, so there is only one transport v4
       * next hop. Use transport next hop attributes, except for L3 intf id,
       * which should the L3 intf associated with the tunnel 
       */
      if (pTunnelEntry->bcmL3IntfId != HAPI_BROAD_INVALID_L3_INTF_ID)
      {
        if (pTunnelEntry->pXportNhop->flags & BROAD_L3_NH_RESOLVED)
        {
          pTunNhop->flags |= BROAD_L3_NH_RESOLVED;
        }

        pTunNhop->l3_intf_id = pTunnelEntry->bcmL3IntfId;

        /* NOTE: This address becomes the MAC DA for tunneled unicast pkts */
        *pMac = pTunnelEntry->pXportNhop->pMac;
      }
    }
    else if (pTunnelEntry->key.tunnelMode == L7_TUNNEL_MODE_6TO4)
    {
      /* 6to4 tunnels are point-to-multipoint. There could be multiple transport v4
       * nexthops (or tunnel endpoints)
       */
      for (index=0; index < HAPI_BROAD_L3_MAX_6TO4_NHOP; index++)
      {
        if (pTunNhop == pTunnelEntry->tun6to4Nhops[index].pV6Nhop)
        {
          /* Get the v4 Next hop */
          pV4Nhop = pTunnelEntry->tun6to4Nhops[index].pV4Nhop;  
          p6to4 = &pTunnelEntry->tun6to4Nhops[index];
          break;
        }
      }
 
      if ((pV4Nhop != L7_NULL) && (pV4Nhop->flags & BROAD_L3_NH_RESOLVED))
      {
        pTunNhop->flags |= BROAD_L3_NH_RESOLVED;

        if(L7_IP6_IS_ADDR_6TO4(&pTunNhop->key.addrUsp.addr.ipv6))
        {
          /* Next hop is pointing to a 6to4 relay router. This is the case when
           * v6 traffic has to reach a v6 host beyond a 6to4 endpoint. Create a 
           * 6o4 tunnel to that relay nexthop. 
           * Note: We cannot use 6to4 automatic tunnel to carry v6 traffic 
           * destined to non-6to4 address. 
           */
          if (p6to4->relayTunnelId != HAPI_BROAD_INVALID_L3_INTF_ID)
          {
            /* A 6o4 tunnel already exists to this relay */
            pTunNhop->l3_intf_id = p6to4->relayTunnelId;
            *pMac = pV4Nhop->pMac;
            return;
          }
  
          initInfo.sip = osapiNtohl(pTunnelEntry->key.localAddr.u.sa4.sin_addr.s_addr);
 
          /* Next hop is a 2002:ipv4 address (relay or local) */
          v4Addr = pTunNhop->key.addrUsp.addr.ipv6[2] << 24;
          v4Addr |= pTunNhop->key.addrUsp.addr.ipv6[3] << 16;
          v4Addr |= pTunNhop->key.addrUsp.addr.ipv6[4] << 8;
          v4Addr |= pTunNhop->key.addrUsp.addr.ipv6[5];

          initInfo.dip = v4Addr;
          initInfo.ttl = 255;
          hapiBroadL3BcmTunnelType(L7_TUNNEL_MODE_6OVER4, &initInfo.type);
      
          /* Populate the tunnel fileds for L3 intf */
          intfInfo.tunnelInfo.sip = initInfo.sip;
          intfInfo.tunnelInfo.dip = initInfo.dip;
          intfInfo.tunnelInfo.type = initInfo.type;
  
          nhDapiPortPtr = DAPI_PORT_GET(&pTunnelEntry->nextHopUsp, dapi_g);
          nhHapiPortPtr = HAPI_PORT_GET(&pTunnelEntry->nextHopUsp, dapi_g);
  
          /* Pick MAC/VLAN from outgoing routing interface which is identified
           * by Xport's USP
           */
          if (IS_PORT_TYPE_PHYSICAL(nhDapiPortPtr) == L7_TRUE)
          {
            memcpy((void *)&mac_addr, nhDapiPortPtr->modeparm.physical.macAddr,
                    sizeof(mac_addr));
            vlanId = nhHapiPortPtr->port_based_routing_vlanid;
          }
          else if (IS_PORT_TYPE_LOGICAL_VLAN(nhDapiPortPtr) == L7_TRUE)
          {
            memcpy((void *)&mac_addr, nhDapiPortPtr->modeparm.router.macAddr,
                   sizeof(mac_addr));
            vlanId = nhDapiPortPtr->modeparm.router.vlanID;
          }
          else
          {
            HAPI_BROAD_L3_L7_LOG_ERROR(nhDapiPortPtr->type);
          } 
 
          intfInfo.bcm_data.l3a_vid = vlanId;
          memcpy(&intfInfo.bcm_data.l3a_mac_addr, mac_addr, sizeof(bcm_mac_t));

          rv = usl_bcmx_l3_intf_create(&intfInfo);
 
          HAPI_BROAD_L3_BCMX_DBG(rv, "Tunnel usl_bcmx_l3_intf_create returned (%s),"
                         "L3 intf id %d\n", bcm_errmsg(rv), intfInfo.bcm_data.l3a_intf_id);

          if (rv == BCM_E_NONE)
          {
            p6to4->relayTunnelId = intfInfo.bcm_data.l3a_intf_id;
  
            rv = usl_bcmx_tunnel_initiator_set(&(intfInfo.bcm_data), &initInfo);
  
            HAPI_BROAD_L3_BCMX_DBG(rv, "usl_bcmx_l3_tunnel_initiator_set (%s)\n",
                                 bcm_errmsg(rv));

            pTunNhop->l3_intf_id = intfInfo.bcm_data.l3a_intf_id;
            *pMac = pV4Nhop->pMac;
          }
        }
        else
        {
          /* Next hop is pointing to a 6to4 endpoint. Use automatic tunnel id */
          pTunNhop->l3_intf_id = pTunnelEntry->bcmL3IntfId;
          *pMac = pV4Nhop->pMac;
        }
      } 
      /* Else v4 nexthop is not resolved. Can't do much except pointing to CPU */
    }
  }
  else
  {
    /* The next hop USP is not a tunnel USP. This has to be a 4o4 tunnel. 
     * Search the tunnel AVL tree. Search is preferred instead of adding
     * a link between nhop and tunnels
     */
    memset(&zero, 0, sizeof(zero));
    pTunnelEntry = avlSearchLVL7(&hapiBroadL3TunnelTree, (void *)&zero, AVL_EXACT);
    if (pTunnelEntry == L7_NULL)
    {
      pTunnelEntry = avlSearchLVL7(&hapiBroadL3TunnelTree, (void *)&zero, AVL_NEXT);
    }

    while(pTunnelEntry != L7_NULL)
    {
      if (pTunnelEntry->pTunnelNhop == pTunNhop)
      {
        /* Found the 4o4 tunnel entry. */
        if (pTunnelEntry->bcmL3IntfId != HAPI_BROAD_INVALID_L3_INTF_ID)
        {
          if (pTunnelEntry->pXportNhop->flags & BROAD_L3_NH_RESOLVED)
          {
            pTunNhop->flags |= BROAD_L3_NH_RESOLVED;
          }

          pTunNhop->l3_intf_id = pTunnelEntry->bcmL3IntfId;
 
          /* NOTE: This address becomes the MAC DA for tunneled unicast pkts */
          *pMac = pTunnelEntry->pXportNhop->pMac;
        }
        break;
      }

      pTunnelEntry = avlSearchLVL7(&hapiBroadL3TunnelTree, pTunnelEntry, AVL_NEXT);
    }
    /* If a matching tunnel is not found, the tunnel nexthop will be unresolved
     * and points to CPU
     */
  }
}

/*******************************************************************************
*
* @purpose Handle the deletion of a tunnel next hop
*
* @param   dapi_g   - DAPI Driver object
*
* @returns none
*
* @notes   none
*
* @end
*
*******************************************************************************/
static void hapiBroadL3AsyncTunnelNhopDelete(BROAD_L3_NH_ENTRY_t *pTunNhop, 
                                             DAPI_t  * dapi_g)
{
  L7_int32  rv;
  L7_uint8  index;
  BROAD_PORT_t            *hapiPortPtr;
  BROAD_L3_NH_ENTRY_t     *pNhop;
  BROAD_L3_TUNNEL_ENTRY_t *pTunnelEntry = L7_NULL;
  usl_bcm_l3_intf_t        intfInfo;
  bcm_if_t                *pRelayTunnelId;

  usl_bcm_l3_intf_t_init(&intfInfo);

  hapiPortPtr = HAPI_PORT_GET(&(pTunNhop->key.addrUsp.usp), dapi_g);
  pTunnelEntry = hapiPortPtr->hapiModeparm.pTunnel;

  if (pTunnelEntry->key.tunnelMode == L7_TUNNEL_MODE_6TO4)
  {
    for (index=0; index < HAPI_BROAD_L3_MAX_6TO4_NHOP; index++)
    {
      if (pTunnelEntry->tun6to4Nhops[index].pV6Nhop == pTunNhop)
      {
        /* Check if the v4 next hops needs to be deleted. Note, that there 
         * could be multiple routes using the same tunnel nexthop
         */
        pNhop = pTunnelEntry->tun6to4Nhops[index].pV4Nhop;
        if ((pNhop != L7_NULL) && (pTunNhop->ref_count == 1))
        {
          /* The tunnel nexthop has no more routes, delete its v4 nexthop too */
          hapiBroadL3NhopEntryUpdate(pNhop, BROAD_L3_NH_CMD_DELETE);
          pTunnelEntry->tun6to4Nhops[index].pV6Nhop = L7_NULL;
          pTunnelEntry->tun6to4Nhops[index].pV4Nhop = L7_NULL;

          /* Delete the tunnel to relay also. */
          pRelayTunnelId = &pTunnelEntry->tun6to4Nhops[index].relayTunnelId;
          if (*pRelayTunnelId != HAPI_BROAD_INVALID_L3_INTF_ID)
          {
            intfInfo.bcm_data.l3a_intf_id = *pRelayTunnelId;

            rv = usl_bcmx_tunnel_initiator_clear(&(intfInfo.bcm_data));
            HAPI_BROAD_L3_BCMX_DBG(rv, "usl_bcmx_tunnel_initiator_clear (%s)\n",
                                   bcm_errmsg(rv));

            rv = usl_bcmx_l3_intf_delete(&intfInfo);
            *pRelayTunnelId = HAPI_BROAD_INVALID_L3_INTF_ID;
          }
        }
        break;
      }
    }
  }  

  hapiBroadL3NhopEntryUpdate(pTunNhop, BROAD_L3_NH_CMD_DELETE);
}

/*******************************************************************************
*
* @purpose Processes the wait queue
*
* @param   dapi_g   - DAPI Driver object
*
* @returns none
*
* @notes none
*
* @end
*
*******************************************************************************/
static void hapiBroadL3AsyncWaitqProcess (DAPI_t *dapi_g)
{
  L7_RC_t rc;
  L7_uint32 num_msg;
  L7_BOOL   isWorkPending = L7_FALSE;
  void      *waitSema;

  rc = osapiMsgQueueGetNumMsgs(hapiBroadL3WlistWaitQueue, &num_msg);

  if (rc != L7_SUCCESS)
  {
    HAPI_BROAD_L3_L7_LOG_ERROR(rc);
  }

  if (num_msg == 0)
  {
    return; /* Nobody is waiting */
  }

  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  if ((hapiBroadL3RouteListHead != L7_NULL) ||
      (hapiBroadL3HostListHead != L7_NULL) ||
      (hapiBroadL3NhopListHead != L7_NULL) ||
      (hapiBroadL3EcmpListHead != L7_NULL)||
      (hapiBroadL3TunnelListHead != L7_NULL) ||
      (hapiBroadL3ProcessZeroCountNhops == L7_TRUE))
  {
    isWorkPending = L7_TRUE;
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

  /* TBD. If more than one thread is waiting, loop through and give sema */
  if (isWorkPending == L7_FALSE)
  {
    /* No work pending on any of the work lists */
    rc = osapiMessageReceive(hapiBroadL3WlistWaitQueue, 
                             (void *)&waitSema,
                             sizeof(waitSema),
                             L7_NO_WAIT);
    if (rc != L7_SUCCESS)
    {
      /* This shouldn't be the case. There should be atleast one msg on queue */
      HAPI_BROAD_L3_L7_LOG_ERROR(rc);           
    }

    HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3AsyncWaitqProcess: waking up"
                        "sema %p\n", waitSema);
    HAPI_BROAD_L3_SEMA_GIVE(waitSema);
  }
}


/*******************************************************************************
*
* @purpose Walk through the next hop tree checking for entries marked for delayed 
*          deletion. Submit them for deletion.
*
* @param   dapi_g   - DAPI Driver object
*
* @returns none
*
* @notes   Call this only when nhops are marked for delayed deletion.
*
* @end
*
*******************************************************************************/
static void hapiBroadL3AsyncZeroCountNhopDelete (DAPI_t *dapi_g)
{
  BROAD_L3_NH_ENTRY_t   *pNhopEntry;
  BROAD_L3_ECMP_ENTRY_t *pEcmpEntry;
  BROAD_L3_ECMP_KEY_t    ecmpKey;
  BROAD_L3_NH_KEY_t      nhopKey;
  L7_BOOL                isWorkPending = L7_FALSE;

  HAPI_BROAD_L3_ASYNC_DBG("\n hapiBroadL3AsyncZeroCountNhopDelete\n");
  memset(&ecmpKey, 0, sizeof(BROAD_L3_ECMP_KEY_t));
  memset(&nhopKey, 0, sizeof(BROAD_L3_NH_KEY_t));

  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);
  
  if (hapiBroadL3RouteListHead != L7_NULL)
  {
    /* Don't do anything, till currently pending routes are processed. 
     * This will result in delayed deletion of nhops.
     */
    HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);  
    return;
  }

  pEcmpEntry = avlSearchLVL7(&hapiBroadL3EcmpTree, &ecmpKey,  AVL_EXACT);
  if (pEcmpEntry == L7_NULL)
  {
    pEcmpEntry = avlSearchLVL7(&hapiBroadL3EcmpTree, &ecmpKey, AVL_NEXT);
  }

  /* First check for ECMP next hops and then individual next hops */
  while (pEcmpEntry != L7_NULL)
  {
    if (pEcmpEntry->flags & BROAD_L3_ECMP_ZERO_COUNT)
    {
      pEcmpEntry->flags &= ~BROAD_L3_ECMP_ZERO_COUNT;

      HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3AsyncZeroCountNhopDelete:"
                   " Marking Mpath %d for deletion\n", pEcmpEntry->egressId);
  
      pEcmpEntry->ref_count++; /* Below function decrements and deletes */
      hapiBroadL3EcmpEntryUpdate(pEcmpEntry, BROAD_L3_ECMP_CMD_DELETE);       
      isWorkPending = L7_TRUE;
    }
    pEcmpEntry = avlSearchLVL7(&hapiBroadL3EcmpTree, pEcmpEntry, AVL_NEXT);
  }

  if (isWorkPending == L7_FALSE) /* Delete nhops after ECMP deletes are done */
  { 
    /* All ECMP objects with ref-count=0 should be deleted before nhop delete.
     * This ensures that nexthop is not used by an ECMP object.
     */
    pNhopEntry = avlSearchLVL7(&hapiBroadL3NhopTree, &nhopKey,  AVL_EXACT);
    if (pNhopEntry == L7_NULL)
    {
      pNhopEntry = avlSearchLVL7(&hapiBroadL3NhopTree, &nhopKey, AVL_NEXT);
    }

    while (pNhopEntry != L7_NULL)
    {
      if (pNhopEntry->flags & BROAD_L3_NH_ZERO_COUNT)
      {
        pNhopEntry->flags &= ~BROAD_L3_NH_ZERO_COUNT;

        HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3AsyncZeroCountNhopDelete:"
                     " Marking EgrId %d for deletion\n", pNhopEntry->egressId);

         pNhopEntry->ref_count++;
         hapiBroadL3NhopEntryUpdate(pNhopEntry, BROAD_L3_NH_CMD_DELETE);
         isWorkPending = L7_TRUE;
      }
      pNhopEntry = avlSearchLVL7(&hapiBroadL3NhopTree, pNhopEntry, AVL_NEXT);
    }
  }
  /* { else, ECMP objects have pending delete, so delete nhops in the next pass } */ 

  if (isWorkPending == L7_TRUE)
  {
    HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3AsyncZeroCountNhopDelete:"
                       " Waking up Async task \n");

    HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK; /* Wake ourselves up */
  }
  else
  {
    /* We have looped through ECMP and individual nexthops and there is nothing
     * to do, reset the flag
     */
    hapiBroadL3ProcessZeroCountNhops = L7_FALSE; 
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);
}


/*******************************************************************************
*
* @purpose Walk through the L3 trees, locate the failed entries, retry them.
*
* @param   dapi_g   - DAPI Driver object
*
* @returns none
*
* @notes   Failed entries are retried either periodically or if any other entry
*          gets deleted. Periodic retries handle cases when no L3 entry gets 
*          deleted and we have failed entries (due to transient condition).
*
* @end
*
*******************************************************************************/
static void hapiBroadL3AsyncRetryFailures (DAPI_t *dapi_g)
{
  L7_BOOL wakeUpAsyncTask = L7_FALSE;
  BROAD_L3_ROUTE_KEY_t    rtKey;
  BROAD_L3_ROUTE_ENTRY_t *pRoute;
  BROAD_L3_HOST_KEY_t     hostKey;
  BROAD_L3_HOST_ENTRY_t  *pHost;
  BROAD_L3_NH_KEY_t       nhopKey;
  BROAD_L3_NH_ENTRY_t    *pNhop;
  BROAD_L3_ECMP_KEY_t     ecmpKey;
  BROAD_L3_ECMP_ENTRY_t  *pEcmp;
  static L7_uint32        lastCallbackTime = 0;
  L7_uint32               currentTime;
  DAPI_USP_t              nullUsp;

  memset(&rtKey, 0, sizeof(rtKey));
  memset(&hostKey, 0, sizeof(hostKey));
  memset(&nhopKey, 0, sizeof(nhopKey));
  memset(&ecmpKey, 0, sizeof(ecmpKey));
  memset(&nullUsp, 0, sizeof(nullUsp));

  /* Nexthop failures are retried only once after warm start */
  if (retryFailedNhops == L7_TRUE)
  {

    HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3AsyncRetryFailures: Retrying failed "
                            "Nhops %d, timerExpired %d, retryFailedNhops %d\n", 
                             broadL3HwNhopStats.current_add_failures,
                             retryTimerExpired, retryFailedNhops);

    HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);
    pNhop = avlSearchLVL7(&hapiBroadL3NhopTree, (void *)&nhopKey, AVL_EXACT);
    if (pNhop == L7_NULL)
    {
      pNhop = avlSearchLVL7(&hapiBroadL3NhopTree, (void *)&nhopKey, AVL_NEXT);
    }

    while(pNhop != L7_NULL)
    {
      if ((pNhop->rv != 0) && (pNhop->wl.cmd == BROAD_L3_NH_CMD_NO_OP))
      {
        pNhop->wl.cmd = BROAD_L3_NH_CMD_ADD;
        HAPI_BROAD_L3_NH_WLIST_ADD(pNhop);
        wakeUpAsyncTask = L7_TRUE;
      } 

      pNhop = avlSearchLVL7(&hapiBroadL3NhopTree, pNhop, AVL_NEXT);
    }

    HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

    retryFailedNhops = L7_FALSE;
  }

  /* Ecmp failures are retried only once after warm start */
  if (retryFailedEcmpNhops == L7_TRUE)
  {

    HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3AsyncRetryFailures: Retrying failed "
                            "Ecmp Nhops %d, timerExpired %d, retryFailedNhops %d\n", 
                             broadL3HwEcmpStats.current_add_failures,
                             retryTimerExpired, retryFailedEcmpNhops);

    HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);
    pEcmp = avlSearchLVL7(&hapiBroadL3EcmpTree, (void *)&ecmpKey, AVL_EXACT);
    if (pEcmp == L7_NULL)
    {
      pEcmp = avlSearchLVL7(&hapiBroadL3EcmpTree, (void *)&ecmpKey, AVL_NEXT);
    }

    while(pEcmp != L7_NULL)
    {
      if ((pEcmp->rv != 0) && (pEcmp->wl.cmd == BROAD_L3_ECMP_CMD_NO_OP))
      {
        pEcmp->wl.cmd = BROAD_L3_ECMP_CMD_ADD;
        HAPI_BROAD_L3_ECMP_WLIST_ADD(pEcmp);
        wakeUpAsyncTask = L7_TRUE;
      } 
      pEcmp = avlSearchLVL7(&hapiBroadL3EcmpTree, pEcmp, AVL_NEXT);
    }

    HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

    retryFailedEcmpNhops = L7_FALSE;
  }

  /* Handle Route failures */
  if (broadL3HwRouteStats.current_add_failures > 0)
  {
    /* retryFailedRoutes is TRUE if any route gets deleted */
    /* retryTimerExpired is TRUE is retry timer expires */
    if ((retryFailedRoutes == L7_TRUE) || (retryTimerExpired == L7_TRUE))
    {

      HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3AsyncRetryFailures: Retrying failed "
                      "routes %d, timerExpired %d, routeDeleted %d\n", 
                       broadL3HwRouteStats.current_add_failures,
                       retryTimerExpired, retryFailedRoutes);

      HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);
      pRoute = avlSearchLVL7(&hapiBroadL3RouteTree, (void *)&rtKey, AVL_EXACT);
      if (pRoute == L7_NULL)
      {
        pRoute = avlSearchLVL7(&hapiBroadL3RouteTree, (void *)&rtKey, AVL_NEXT);
      }

      while(pRoute != L7_NULL)
      {
        if ((pRoute->rv != 0) && (pRoute->wl.cmd == BROAD_L3_ROUTE_CMD_NO_OP))
        {
          if ((retryFailedRoutes != L7_TRUE) && (pRoute->rv == BCM_E_FULL))
          {
            /* Do not retry FULL condition failures, unless an entry was deleted */ 
          }
          else
          {
            pRoute->wl.cmd = BROAD_L3_ROUTE_CMD_ADD;
            HAPI_BROAD_L3_ROUTE_WLIST_ADD(pRoute);
            wakeUpAsyncTask = L7_TRUE;
          }
        } 
        pRoute = avlSearchLVL7(&hapiBroadL3RouteTree, pRoute, AVL_NEXT);
      }

      HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

      retryFailedRoutes = L7_FALSE;
    }
  }

  /* Handle Host failures */
  if (broadL3HwHostStats.current_add_failures > 0)
  {
    /* See if it is time to report the HOST failure(s) to application */
    currentTime = osapiUpTimeRaw();
    if ((currentTime < lastCallbackTime) || 
        (currentTime > (lastCallbackTime+BROAD_L3_ARP_COLLISION_DELAY)))
    {
      /* We need to report it; do so and update the last report time. */
      dapiCallback(&(nullUsp),
                   DAPI_FAMILY_ROUTING_ARP_MGMT,
                   DAPI_CMD_ROUTING_ARP_UNSOLICITED_EVENT,
                   DAPI_EVENT_ROUTING_ARP_ENTRY_COLLISION,
                   L7_NULL);
      lastCallbackTime = currentTime;
    }

    if ((retryFailedHosts == L7_TRUE) || (retryTimerExpired == L7_TRUE))
    {
      HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3AsyncRetryFailures: retrying failed"
                              " hosts %d, timerExpired %d, hostDeleted %d\n",
                              broadL3HwHostStats.current_add_failures,
                              retryTimerExpired, retryFailedHosts);

      HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);
      pHost = avlSearchLVL7(&hapiBroadL3HostTree, (void *)&hostKey, AVL_EXACT);
      if (pHost == L7_NULL)
      {
        pHost = avlSearchLVL7(&hapiBroadL3HostTree, (void *)&hostKey, AVL_NEXT);
      }

      while (pHost != L7_NULL)
      {
        if ((pHost->rv != 0) && (pHost->wl.cmd == BROAD_L3_HOST_CMD_NO_OP))
        {
          if ((retryFailedHosts != L7_TRUE) && (pHost->rv == BCM_E_FULL))
          {
            /* Do not retry FULL condition failure, unless an entry was deleted */
          }
          else
          {
            pHost->wl.cmd = BROAD_L3_HOST_CMD_ADD;
            HAPI_BROAD_L3_HOST_WLIST_ADD(pHost);
            wakeUpAsyncTask = L7_TRUE;
          }
        }
        pHost = avlSearchLVL7(&hapiBroadL3HostTree, pHost, AVL_NEXT);
      }

      HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

      retryFailedHosts = L7_FALSE;
    }
  }

  retryTimerExpired = L7_FALSE;

  if (wakeUpAsyncTask == L7_TRUE)
  {
    /* Wakeup only if we enqueued work. */
    HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3AsyncRetryFailures: Waking up async task\n");
    HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;
  }
}


/*******************************************************************************
*
* @purpose Periodic timer to retry failures that have occured due to transient
*          error conditions that might have cleared overtime.
*
* @param   arg1
* @param   arg2
*
* @returns none
*
* @notes   Failed entries are retried either periodically or if any other entry
*          gets deleted
*
* @end
*
*******************************************************************************/
void hapiBroadL3RetryTimer(L7_uint32 arg1, L7_uint32 arg2)
{
  /* Set retry flag and wake up async task, only when we have failed entries */
  if ((broadL3HwRouteStats.current_add_failures > 0) ||
      (broadL3HwHostStats.current_add_failures > 0))
  {
    retryTimerExpired = L7_TRUE;
    HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;
  }
  else
  {
    retryTimerExpired = L7_FALSE;
  }

  osapiTimerAdd((void*)hapiBroadL3RetryTimer, 0, 0,
                HAPI_BROAD_L3_RETRY_INTERVAL, &pBroadL3RetryTimer );
}


/******************************************************************************
*
* @purpose  Manage the special IPv6 route that sends link local traffic
*           (i.e. traffic with an FE80::/10 prefix) to the CPU port. Idea is
*           to use one entry for all link locals.
*
* @param install          If L7_TRUE, install the route, L7_FALSE remove it.
* @param *dapi_g          system information
*
* @returns void
*
* @notes   Only Link-local IPv6 traffic destined to router MAC Address reaches
*          CPU. Currently all v6 interfaces have same Link-local IPv6
*          address by default. 
*          The Link local route will take one v6 route entry thereby reducing
*          max configurable v6 routes by 1.
*          Ideally, this should be done via route work list, but we don't want
*          to allow link local routes from application.
*
* @end
*
*******************************************************************************/
#ifdef L7_IPV6_PACKAGE
void
hapiBroadL3v6LinkLocalRouteAddDel(L7_BOOL install, DAPI_t *dapi_g)
{
  static L7_ushort16 addr[8] = {0xFE80, 0, 0, 0, 0, 0, 0, 0 };
  static L7_ushort16 mask[8] = {0xFFC0, 0, 0, 0, 0, 0, 0, 0 };
  static L7_BOOL haveLinkLocals = L7_FALSE;
  usl_bcm_l3_route_t      bcmRouteInfo;
  L7_int32 rv;
  L7_RC_t  result;

  HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3v6LinkLocalRouteAddDel:"
                          "Install %d\n", install);
 
  memset(&bcmRouteInfo, 0, sizeof(bcmRouteInfo));
  bcmRouteInfo.l3a_flags |= BCM_L3_IP6;

  memcpy(&(bcmRouteInfo.routeKey.l3a_subnet.l3a_ip6_net), addr, sizeof(bcm_ip6_t));
  memcpy(&(bcmRouteInfo.routeKey.l3a_mask.l3a_ip6_mask), mask, sizeof(bcm_ip6_t));

  if (install == L7_TRUE)
  {
    if ((hapiBroadL3CpuIntfId != HAPI_BROAD_INVALID_L3_INTF_ID) &&
        (hapiBroadL3CpuEgrNhopId != HAPI_BROAD_INVALID_L3_INTF_ID) && 
        (haveLinkLocals == L7_FALSE))
    {
      /*  Install the link local route pointing it to CPU next hop */
      memcpy(&(bcmRouteInfo.routeKey.l3a_subnet.l3a_ip6_net), addr, sizeof(bcm_ip6_t));
      memcpy(&(bcmRouteInfo.routeKey.l3a_mask.l3a_ip6_mask), mask, sizeof(bcm_ip6_t));
      haveLinkLocals = L7_TRUE;
      bcmRouteInfo.l3a_intf = hapiBroadL3CpuEgrNhopId;

      /* Not using caching mechanism here, as this is one-time under-the-cover 
       * route which is not added to route tree (so no internal struct).
       */
      result = usl_bcmx_l3_route_add(&bcmRouteInfo, 1, &rv);
      if ((result == L7_FAILURE) || (rv != BCM_E_NONE))
      {
        HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3v6LinkLocalRouteAddDel:"
                            " Failed to install link local route rv %d\n", rv);
        HAPI_BROAD_L3_L7_LOG_ERROR(rv);
      }
    }
  }
  else
  {
    if (haveLinkLocals == L7_TRUE)
    {
      /* Remove the link local route and the egress object. */
      bcmRouteInfo.l3a_intf = hapiBroadL3CpuEgrNhopId;

      result = usl_bcmx_l3_route_delete(&bcmRouteInfo, 1, &rv);
      if ((result == L7_FAILURE) || (rv != BCM_E_NONE))
      {
        HAPI_BROAD_L3_ASYNC_DBG("hapiBroadL3v6LinkLocalRouteAddDel:"
                            " Failed to delete link local route rv %d\n", rv);
        /* ignore return code for delete */
      }

      haveLinkLocals = L7_FALSE;
    }
  }
}
#else
void
hapiBroadL3v6LinkLocalRouteAddDel(L7_BOOL install, DAPI_t *dapi_g)
{
  return;
}
#endif

/*******************************************************************************
* @purpose  Callback function from USL to indicate that a host could not
*           be inserted into the HW. HAPI acts on this by utilizing the
*           retry mechanism.
*
* @param    data  {(input)} Pointer to host info
*
* @returns  
*
* @comments
*
* @end
*******************************************************************************/
void hapiBroadL3AsyncHostFailureCallback(void *data)
{
  usl_bcm_l3_host_t     *bcm_data = data;
  BROAD_L3_HOST_ENTRY_t  host;
  BROAD_L3_HOST_ENTRY_t *pHostEntry;
  L7_BOOL                matchFound;

  /* Set this host up for retry. */
  memset(&host, 0, sizeof(host));
  if (bcm_data->l3a_flags & BCM_L3_IP6)
  {
    host.key.family = L7_AF_INET6;
    memcpy(&host.key.addrUsp.addr.ipv6, &bcm_data->hostKey.addr.l3a_ip6_addr, sizeof(bcm_ip6_t));
  }
  else
  {
    host.key.family = L7_AF_INET;
    host.key.addrUsp.addr.ipv4 = bcm_data->hostKey.addr.l3a_ip_addr;
  }

  HAPI_BROAD_L3_SEMA_TAKE(hapiBroadL3Sema, L7_WAIT_FOREVER);

  matchFound = L7_FALSE;
  pHostEntry = avlSearchLVL7(&hapiBroadL3HostTree, &host, AVL_NEXT);

  if (pHostEntry != L7_NULL)
  {
    if (pHostEntry->key.family == host.key.family)
    {
      if (pHostEntry->key.family == L7_AF_INET)
      {
        if (pHostEntry->key.addrUsp.addr.ipv4 == host.key.addrUsp.addr.ipv4)
        {
          matchFound = L7_TRUE;
        }
      }
      else
      {
        if (memcmp(&host.key.addrUsp.addr.ipv6, &pHostEntry->key.addrUsp.addr.ipv6, sizeof(bcm_ip6_t)) == 0)
        {
          matchFound = L7_TRUE;
        }
      }

      if (matchFound == L7_TRUE)
      {
        pHostEntry->wl.cmd = BROAD_L3_HOST_CMD_ASYNC_FAIL_HANDLE;

        HAPI_BROAD_L3_HOST_WLIST_ADD(pHostEntry);

        HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;
      }
    }
  }

  HAPI_BROAD_L3_SEMA_GIVE(hapiBroadL3Sema);

  if (matchFound == L7_FALSE)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID, "Received async failure notification for unknown host from USL.");
  }
}


/*********************************************************************
*
* @purpose During warm start it is possible that some newly learned 
*          unicast and ecmp Next-hop entries could not be inserted in 
*          the USL Db and hardware. This routine triggers the Async
*          task to retry insertion of these failed entries.
*          
*
*
* @returns L7_RC_t result
*
* @notes   Triggers hardware retries of Nhop entries that could not be
*          inserted in the hardware due to temporary table full 
*          condition.
*
* @end
*
*********************************************************************/
void hapiBroadL3WarmStartRetryFailures(void)
{
  if (broadL3HwNhopStats.total_add_failures > 0)
  {
    retryFailedNhops = L7_TRUE;
    HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;
  }

  if (broadL3HwEcmpStats.total_add_failures > 0)
  {
    retryFailedEcmpNhops = L7_TRUE;
    HAPI_BROAD_L3_WAKE_UP_ASYNC_TASK;
  }

  return;
}
