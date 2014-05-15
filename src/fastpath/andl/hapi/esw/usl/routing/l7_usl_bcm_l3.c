/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcm_l3.c
*
* @purpose    USL BCM APIs for L3
*
* @component  HAPI
*
* @comments   none
*
* @create     11/19/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/

#include "l7_common.h"

#ifdef L7_ROUTING_PACKAGE

#include "ibde.h"
#include "sal/core/libc.h"
#include "soc/drv.h"

#include "broad_common.h"
#include "l7_usl_sm.h"
#include "l7_usl_bcm_l3.h"
#include "l7_usl_l3_db.h"

/* PTin added: SDK 6.3.0 */
#include "ptin_globaldefs.h"
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
#include "logger.h"
#endif

L7_BOOL uslDebugL3Enable = L7_FALSE;
void uslDebugL3EnableSet(L7_BOOL flag)
{
  uslDebugL3Enable = flag;
}

/*********************************************************************
* @purpose  Convert usl_bcm_l3_host_t structure to the BCM host structure.
*
*
* @end
*********************************************************************/
void l7_l3host_to_bcm(usl_bcm_l3_host_t *l7BcmHost, bcm_l3_host_t *bcmHost)
{
  bcm_l3_host_t_init(bcmHost);
 
  bcmHost->l3a_flags = l7BcmHost->l3a_flags;
  bcmHost->l3a_intf = l7BcmHost->l3a_intf;
  bcmHost->l3a_pri = l7BcmHost->l3a_pri;
  
  if (l7BcmHost->l3a_flags & BCM_L3_IP6)
  {
    memcpy(&(bcmHost->l3a_ip6_addr), &(l7BcmHost->hostKey.addr.l3a_ip6_addr), sizeof(bcm_ip6_t));
  }
  else
  {
    memcpy(&(bcmHost->l3a_ip_addr), &(l7BcmHost->hostKey.addr.l3a_ip_addr), sizeof(bcm_ip_t));
  }

  return;
}

/*********************************************************************
* @purpose  Convert usl_bcm_l3_route_t structure to the BCM route structure.
*
*
* @end
*********************************************************************/
void l7_l3route_to_bcm(usl_bcm_l3_route_t *l7BcmRoute, bcm_l3_route_t *bcmRoute)
{
  bcm_l3_route_t_init(bcmRoute);
 
  printf("%s (%d)\n", __FUNCTION__, __LINE__);

  bcmRoute->l3a_flags = l7BcmRoute->l3a_flags;
  bcmRoute->l3a_intf = l7BcmRoute->l3a_intf;
  bcmRoute->l3a_pri  = l7BcmRoute->l3a_pri;

  if (l7BcmRoute->l3a_flags & BCM_L3_IP6)
  {
    memcpy(&(bcmRoute->l3a_ip6_net), 
           &(l7BcmRoute->routeKey.l3a_subnet.l3a_ip6_net), 
           sizeof(bcm_ip6_t));
    memcpy(&(bcmRoute->l3a_ip6_mask), 
           &(l7BcmRoute->routeKey.l3a_mask.l3a_ip6_mask), 
           sizeof(bcm_ip6_t));
  }
  else
  {
    memcpy(&(bcmRoute->l3a_subnet), 
           &(l7BcmRoute->routeKey.l3a_subnet.l3a_ip_net), 
           sizeof(bcm_ip_t));
    memcpy(&(bcmRoute->l3a_ip_mask), 
           &(l7BcmRoute->routeKey.l3a_mask.l3a_ip_mask), 
           sizeof(bcm_ip_t));
  }

  return;
}

/*********************************************************************
* @purpose  Convert BCMX terminator structure to the BCM terminator structure
*
* @param    l3xtun     @{(input)} BCMX terminator info
* @param    l3tun      @{(input)} BCM terminator info
*
* @returns  void
*
* @end
*********************************************************************/
void
l7_bcmx_terminator_to_bcm(bcmx_tunnel_terminator_t *l3xtun,
                          bcm_tunnel_terminator_t *l3tun)
{
  memset(l3tun, 0, sizeof(*l3tun));
  l3tun->flags = l3xtun->flags;
  l3tun->sip = l3xtun->sip;
  l3tun->dip = l3xtun->dip;
  l3tun->sip_mask = l3xtun->sip_mask;
  l3tun->dip_mask = l3xtun->dip_mask;
  l3tun->udp_dst_port = l3xtun->udp_dst_port;
  l3tun->udp_src_port = l3xtun->udp_src_port;
  l3tun->type = l3xtun->type;
  l3tun->vlan = l3xtun->vlan;
}

/*********************************************************************
* @purpose  Initialize the usl_bcm_l3_intf_t structure
*
* @param
*
* @returns  BCM error code
*
* @end
*********************************************************************/
void usl_bcm_l3_intf_t_init(usl_bcm_l3_intf_t *intf)
{
  if (intf != L7_NULLPTR)
  {
    bcm_l3_intf_t_init(&(intf->bcm_data));
    memset(&(intf->tunnelInfo), 0, sizeof(intf->tunnelInfo));
  }

  return;
}

/*********************************************************************
* @purpose  Custom RPC function to handle interface create
*
* @param
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_bcm_l3_intf_create (usl_bcm_l3_intf_t *intf)
{
  int rv = BCM_E_NONE;
  L7_uint32 i;

  printf("%s (%d)\n", __FUNCTION__, __LINE__);

  /* Check if hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L3_INTF_DB_ID) == L7_TRUE)
  {
    printf("%s (%d)\n", __FUNCTION__, __LINE__);
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      printf("%s (%d)\n", __FUNCTION__, __LINE__);
      if (!SOC_IS_XGS_FABRIC(i))
      {
        printf("%s (%d)\n", __FUNCTION__, __LINE__);
        rv = bcm_l3_intf_create (i, &(intf->bcm_data));
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          printf("%s (%d)\n", __FUNCTION__, __LINE__);
          break;
        }
      }
    }
  }

  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L3_INTF_DB_ID) == L7_TRUE))
  {
    printf("%s (%d): l3a_intf_id=%d\n", __FUNCTION__, __LINE__, intf->bcm_data.l3a_intf_id);

    rv = usl_db_l3_intf_create(USL_CURRENT_DB, intf); 
  }

  printf("%s (%d): rv=%d\n", __FUNCTION__, __LINE__, rv);
  return rv;
}


/*********************************************************************
* @purpose  USL BCM API to handle interface delete
*
* @param
*
* @returns  BCM error code 
*
* @end
*********************************************************************/
int usl_bcm_l3_intf_delete (usl_bcm_l3_intf_t *intf)
{
  int rv = BCM_E_NONE, db_rv = BCM_E_NONE;
  L7_uint32 i;

  printf("%s (%d)\n", __FUNCTION__, __LINE__);

  /* Check if hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L3_INTF_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        rv = bcm_l3_intf_delete (i, &(intf->bcm_data));
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;
        }
      }
    }
  }

  /* Update the USL Db  */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L3_INTF_DB_ID) == L7_TRUE))
  {
    db_rv = usl_db_l3_intf_delete(USL_CURRENT_DB, intf);
  }

  /* Return the worst error code */
  rv = min(rv, db_rv);

  return rv;
}
/*********************************************************************
* @purpose  USL BCM API to handle Host adds.
*
* @param    
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_bcm_l3_host_add (usl_bcm_l3_host_t  *pHostInfo)
{
  bcm_l3_host_t bcmHostInfo;
  L7_int32      rv = BCM_E_NONE, i;

  /* Convert from usl_bcm_l3_host_t to bcm_l3_host_t */
  l7_l3host_to_bcm(pHostInfo, &bcmHostInfo);
    
  /* Check if hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L3_HOST_DB_ID) == L7_TRUE)
  {
    printf("usl_bcm_l3_host_add (%d)\n", __LINE__);

    /* Add each of the host entries to all the local units */
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        rv = bcm_l3_host_add (i, &bcmHostInfo);
        printf("usl_bcm_l3_host_add (%d): rv=%d\n", __LINE__, rv);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;
        }
      }
    }
  }

  printf("usl_bcm_l3_host_add (%d)\n", __LINE__);
 
  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L3_HOST_DB_ID) == L7_TRUE))
  {
    printf("usl_bcm_l3_host_add (%d)\n", __LINE__);
    rv = usl_db_l3_host_add(USL_CURRENT_DB, pHostInfo);
    printf("usl_bcm_l3_host_add (%d): rv=%d\n", __LINE__, rv);
  }
    
  printf("usl_bcm_l3_host_add (%d): rv=%d\n", __LINE__, rv);
  return rv;
}

/*********************************************************************
* @purpose  USL BCM API to handle Host deletes.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
int usl_bcm_l3_host_delete (usl_bcm_l3_host_t  *pHostInfo)
{
  bcm_l3_host_t bcmHostInfo;
  L7_int32      i, rv = BCM_E_NONE, db_rv = BCM_E_NONE; 

  printf("%s (%d)\n", __FUNCTION__, __LINE__);

  /* Convert from usl_bcm_l3_host_t to bcm_l3_host_t */
  l7_l3host_to_bcm(pHostInfo, &bcmHostInfo);

  /* Check if hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L3_HOST_DB_ID) == L7_TRUE)
  {
    /* Delete each of the host entries from all the local units */
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        rv = bcm_l3_host_delete (i, &bcmHostInfo);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;
        }
      }
    }
  }

  /* Update the USL Db even if hw delete failed */
  if (USL_BCM_CONFIGURE_DB(USL_L3_HOST_DB_ID) == L7_TRUE)
  {
    db_rv = usl_db_l3_host_delete(USL_CURRENT_DB, pHostInfo);
  }

  /* Return the worst error code */
  rv = min(rv, db_rv);

  return rv;
}

/*********************************************************************
* @purpose  USL BCM API to handle Route adds.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
int usl_bcm_l3_route_add (usl_bcm_l3_route_t  *pRouteInfo)
{
  L7_uint32      i;
  L7_int32       rv = BCM_E_NONE;
  bcm_l3_route_t bcmRouteInfo;

  /* Convert l7_bcm_route_t structure to bcm_l3_route_t */
  l7_l3route_to_bcm(pRouteInfo, &bcmRouteInfo);

  if (uslDebugL3Enable)
  {
    printf("USL_BCM: Adding route  %x/%x\n",
           pRouteInfo->routeKey.l3a_subnet.l3a_ip_net,
           pRouteInfo->routeKey.l3a_mask.l3a_ip_mask);
  }

  /* Check if hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L3_LPM_DB_ID) == L7_TRUE)
  {
    if (uslDebugL3Enable)
    {
      printf("USL_BCM: updating BCM..\n");
    }

    /* Add each of the route entries to all the local units */
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        rv = bcm_l3_route_add (i, &bcmRouteInfo);

        /* We need to return rv per FP unit/stack unit. If there are multiple
         * BCM units per FP unit, note the worst 'rv'.
         */
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          if (uslDebugL3Enable)
          {
            printf("Route add failed, rv %d\n", rv);
          }
          break;
        }
      }
    }
  }

  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L3_LPM_DB_ID) == L7_TRUE))
  {

    if (uslDebugL3Enable)
    {
      printf("USL_BCM: updating db..\n");
    }
    rv = usl_db_l3_route_add(USL_CURRENT_DB, pRouteInfo);
  }
    
  return rv;
}


/*********************************************************************
* @purpose  Custom RPC function to handle Route deletes.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
int usl_bcm_l3_route_delete (usl_bcm_l3_route_t  *pRouteInfo)
{
  bcm_l3_route_t bcmRouteInfo;
  L7_uint32      i;
  int            rv = BCM_E_NONE, db_rv = BCM_E_NONE;

  /* Convert l7_bcm_route_t structure to bcm_l3_route_t */
  l7_l3route_to_bcm(pRouteInfo, &bcmRouteInfo);

  printf("%s (%d)\n", __FUNCTION__, __LINE__);

    if (uslDebugL3Enable)
    {
      printf("USL_BCM: Deleting route  %x/%x\n",
             pRouteInfo->routeKey.l3a_subnet.l3a_ip_net,
             pRouteInfo->routeKey.l3a_mask.l3a_ip_mask);
    }

  /* Check if hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L3_LPM_DB_ID) == L7_TRUE)
  {
    /* Delete each of the route entries from all the local units */
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        rv = bcm_l3_route_delete (i, &bcmRouteInfo);

        /* We need to return rv per FP unit/stack unit. If there are multiple
         * BCM units per FP unit, note the worst 'rv'.
         */

        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          if (uslDebugL3Enable)
          {
            printf("Route add failed, rv %d\n", rv);
          }
          break;
        }
      }
    }
  }

  /* Update the USL Db even if hw delete failed */
  if (USL_BCM_CONFIGURE_DB(USL_L3_LPM_DB_ID) == L7_TRUE)
  {
    db_rv = usl_db_l3_route_delete(USL_CURRENT_DB, pRouteInfo);
  }

  /* Return the worst error code */
  rv = min(rv, db_rv);

  return rv;
}


/*********************************************************************
* @purpose  Initialize USL BCM  L3 Egress structure
*
* @params   egr  {(input)} pointer to structure to be initialized
*
* @returns  none
*
* @notes    
*
* @end
*********************************************************************/
void usl_bcm_l3_egress_t_init(usl_bcm_l3_egress_t *egr)
{
  if (egr != L7_NULLPTR)
  {
    memset(egr, 0, sizeof(*egr));  
  }

  return;
}

/*********************************************************************
* @purpose  USL BCM API to create egress objects
*
*
* @returns  bcm error code
*
* @notes    To synchronize with L7 RPC implementation of host/route, we
*           must use L7 RPC and not BCMX for egress objects. There are 
*           scenarios, like clear config, where L7 RPCs for host/route are
*           skipped, but BCMX calls will not be.
*           Also, L7 RPC has performance benefit when multiple BCM units
*           exist on a single FastPath unit.
*
* @end
*********************************************************************/
int usl_bcm_l3_egress_create (L7_uint32 flags, 
                              usl_bcm_l3_egress_t *egr,
                              bcm_if_t *egrId)
{
  int       rv = BCM_E_NONE;
  L7_uint32 i;

  printf("usl_bcm_l3_egress_create (%d)\n", __LINE__);

  /* Check if hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L3_EGR_NHOP_DB_ID) == L7_TRUE)
  {
    /* Create egress object on all the local units */
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        /* PTIN Added */
        egr->bcm_data.mpls_label = -1;
        egr->bcm_data.dynamic_scaling_factor=-1;
        egr->bcm_data.dynamic_load_weight=-1;

        rv = bcm_l3_egress_create (i, flags, &(egr->bcm_data), egrId);

        printf("usl_bcm_l3_egress_create (%d): rv=%d\n", __LINE__, rv);

        /* PTin added: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
        if (rv == BCM_E_UNAVAIL)
        {

          printf("usl_bcm_l3_egress_create (%d): rv=%d\n", __LINE__, rv);

          LOG_WARNING(LOG_CTX_PTIN_HAPI,"bcm_l3_egress_create is not supported in this version... ignoring!");
          rv = BCM_E_INIT;
        }
        #endif

        if (L7_BCMX_OK(rv) != L7_TRUE)
        {

          printf("usl_bcm_l3_egress_create (%d): rv=%d\n", __LINE__, rv);

          break;
        }
      }
    }
  }

  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L3_EGR_NHOP_DB_ID)  == L7_TRUE))
  {

    printf("usl_bcm_l3_egress_create (%d): rv=%d\n", __LINE__, rv);

    rv = usl_db_l3_egress_create(USL_CURRENT_DB, *egrId, egr);
  }

  printf("usl_bcm_l3_egress_create (%d): rv=%d\n", __LINE__, rv);
  return rv;
}

/*********************************************************************
* @purpose  USL BCM API to delete egress objects
*
* @param    pBcmInfo @{(input)}  BCMX data for each nexthops
* @param    pEgrIntf @{(input)} Egress object id(s)
*
* @returns  none
*
* @notes    Egress destroy must use L7 RPC implementation to synchronize
*           with host/route RPC operations.
*
* @end
*********************************************************************/
int usl_bcm_l3_egress_destroy (usl_bcm_l3_egress_t *pBcmInfo, bcm_if_t egrId)
{
  int       rv = BCM_E_NONE, db_rv = BCM_E_NONE;
  L7_uint32 i;

  printf("%s (%d)\n", __FUNCTION__, __LINE__);

  /* Check if hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L3_EGR_NHOP_DB_ID) == L7_TRUE)
  {
    /* Delete the egress object from all the local units */
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        rv = bcm_l3_egress_destroy (i, egrId);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;
        }
      }
    }
  }

  /* It is important to keep the hw and db in sync for mpath egress objects.
  ** So delete from db only if hw delete succeeded.
  */
  if (L7_BCMX_OK(rv) == L7_TRUE)
  {
    if (USL_BCM_CONFIGURE_DB(USL_L3_EGR_NHOP_DB_ID)  == L7_TRUE)
    {
      db_rv = usl_db_l3_egress_destroy(USL_CURRENT_DB, pBcmInfo, egrId);
    }
  }

  /* Return the worst error code */
  rv = min(rv, db_rv);

  return rv;
}


/*********************************************************************
* @purpose  USL BCM API to create multipath egress objects
*
* @param    flags       {(input)} 
* @param    count       {(input)}
* @param    intf_array  {(input)}
* @param    mpintf      {(input/output)} Mpath egress Id
*
* @returns  bcm error code
*
* @notes    
* @end
*********************************************************************/
int usl_bcm_l3_egress_multipath_create (L7_uint32 flags,
                                        L7_int32  count,
                                        bcm_if_t *intf_array,
                                        bcm_if_t *mpintf)
{
  int       rv = BCM_E_NONE;
  L7_uint32 i;

  printf("%s (%d)\n", __FUNCTION__, __LINE__);

  /* Check if hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L3_MPATH_EGR_NHOP_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        rv = bcm_l3_egress_multipath_create(i, flags, count, 
                                            intf_array, mpintf);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;
        }
      }
    }
  }

  if (L7_BCMX_OK(rv) == L7_TRUE)
  {
    /* Update the USL Db */
    if (USL_BCM_CONFIGURE_DB(USL_L3_MPATH_EGR_NHOP_DB_ID) == L7_TRUE)
    { 
      rv = usl_db_l3_egress_multipath_create(USL_CURRENT_DB, *mpintf, 
                                             count, 
                                             intf_array);
    }

  }

  return rv;
}


/*********************************************************************
* @purpose  USL BCM API to delete multipath egress objects
*
* @param    mpIntf         {(input)}  Mpath egress Id to be deleted
*
* @returns  bcm error code
*
* @notes    
* @end
*********************************************************************/
int usl_bcm_l3_egress_multipath_destroy (bcm_if_t mpIntf)
{
  int       rv = BCM_E_NONE, db_rv = BCM_E_NONE;
  L7_uint32 i;

  printf("%s (%d)\n", __FUNCTION__, __LINE__);

  /* Check if hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L3_MPATH_EGR_NHOP_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        rv = bcm_l3_egress_multipath_destroy (i, mpIntf);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;
        }
      }
    }
  }

  /* It is important to keep the hw and db in sync for mpath egress objects.
  ** So delete from db only if hw delete succeeded.
  */
  if (L7_BCMX_OK(rv) == L7_TRUE)
  {
    if (USL_BCM_CONFIGURE_DB(USL_L3_MPATH_EGR_NHOP_DB_ID) == L7_TRUE)
    {
      db_rv = usl_db_l3_egress_multipath_destroy(USL_CURRENT_DB, mpIntf);
    }    
  }

  /* Return the worst error code */
  rv = min(rv, db_rv);
  
  return rv;
}


/*********************************************************************
* @purpose  USL BCM API to handle tunnel initiator sets.
*
* @param    intf {(input)}
* @param    initiator {(input)}
*
* @returns  bcm error code
*
* @end
*********************************************************************/
int usl_bcm_l3_tunnel_initiator_set (bcm_l3_intf_t *intf,
                                     bcm_tunnel_initiator_t *initiator)
{
  L7_uint32 i;
  int       rv = BCM_E_NONE;

  printf("%s (%d)\n", __FUNCTION__, __LINE__);

  /* Check if hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L3_TUNNEL_INITIATOR_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        rv = bcm_tunnel_initiator_set (i, intf, initiator);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          break;
        }
      }
    }
  }

  /* Update the USL Db */
  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L3_TUNNEL_INITIATOR_DB_ID) == L7_TRUE))
  {
    rv = usl_db_l3_tunnel_initiator_set(USL_CURRENT_DB, intf, initiator);
  }

  return rv;
}

/*********************************************************************
* @purpose  USL BCM API to handle tunnel initiator clears.
*
* @param    intf {(input)}
*
* @returns  bcm error code
*
* @end
*********************************************************************/
int usl_bcm_l3_tunnel_initiator_clear (bcm_l3_intf_t *intf)
{
  L7_uint32 i;
  int       rv = BCM_E_NONE, db_rv = BCM_E_NONE;

  printf("%s (%d)\n", __FUNCTION__, __LINE__);

  /* Check if hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L3_TUNNEL_INITIATOR_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        rv = bcm_tunnel_initiator_clear (i, intf);
        if (L7_BCMX_OK(rv) != L7_TRUE)
          break;
      }
    }
  }

  /* Update the USL Db even if hw delete failed */
  if (USL_BCM_CONFIGURE_DB(USL_L3_TUNNEL_INITIATOR_DB_ID) == L7_TRUE)
  {
    db_rv = usl_db_l3_tunnel_initiator_clear(USL_CURRENT_DB, intf);
  }

  /* Return the worst error code */
  rv = min(rv, db_rv);

  return rv;
}

/*********************************************************************
* @purpose  USL BCM API to handle tunnel terminator adds
*
* @param    terminator {(input)}
*
* @returns  bcm error code
*
* @end
*********************************************************************/
int usl_bcm_l3_tunnel_terminator_add(bcm_tunnel_terminator_t *terminator)
{
  bcm_port_config_t config;
  L7_uint32         i;
  int               rv = BCM_E_NONE;

  printf("%s (%d)\n", __FUNCTION__, __LINE__);

  /* Check if hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L3_TUNNEL_TERMINATOR_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        rv = bcm_port_config_get(i, &config);
        if (rv == BCM_E_NONE)
        {
          terminator->pbmp = config.all;
          rv = bcm_tunnel_terminator_add (i, terminator);
          if (L7_BCMX_OK(rv) != L7_TRUE)
            break;
        }
      }
    }
  }

  /* Update the USL Db even if hw delete failed */

  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_L3_TUNNEL_TERMINATOR_DB_ID) == L7_TRUE))
  {
    rv = usl_db_l3_tunnel_terminator_add(USL_CURRENT_DB, terminator);
  }

  return rv;
}


/*********************************************************************
* @purpose  USL BCM API to handle tunnel terminator deletes
*
* @param    terminator {(input)}
*
* @returns  bcm error code
*
* @end
*********************************************************************/
int usl_bcm_l3_tunnel_terminator_delete(bcm_tunnel_terminator_t *terminator)
{
  bcm_port_config_t config;
  L7_uint32 i;
  int rv = BCM_E_NONE;

  printf("%s (%d)\n", __FUNCTION__, __LINE__);

  /* Check if hw should be configured */
  if (USL_BCM_CONFIGURE_HW(USL_L3_TUNNEL_TERMINATOR_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        rv = bcm_port_config_get(i, &config);
        if (rv == BCM_E_NONE)
        {
          terminator->pbmp = config.all;
          rv = bcm_tunnel_terminator_delete (i, terminator);
          if (L7_BCMX_OK(rv) != L7_TRUE)
            break;
        }
      }
    }
  }

  /* Update the USL Db even if hw delete failed */
  if (USL_BCM_CONFIGURE_DB(USL_L3_TUNNEL_TERMINATOR_DB_ID) == L7_TRUE)
  {
    rv = usl_db_l3_tunnel_terminator_delete(USL_CURRENT_DB, terminator);
  }

  return rv;
}

#endif /* L7_ROUTING_PACKAGE */
