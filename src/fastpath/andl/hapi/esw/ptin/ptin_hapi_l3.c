#include "ptin_globaldefs.h"
#include "logger.h"
#include "ptin_hapi.h"
#include <bcmx/switch.h>
#include <bcmx/port.h>
#include <bcmx/l3.h>


/**
 * Add L3 host
 * 
 * @param dapiPort 
 * @param intf_id 
 * @param dstMacAddr 
 * @param dstIpAddr 
 * 
 * @return RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_l3_host_add(ptin_dapi_port_t *dapiPort, st_ptin_l3 *data)
{
    bcm_l3_host_t   host_info;
    bcm_mac_t       mac;
    int             rv = L7_SUCCESS;
    int             host_as_route = 0;
    int             interface = 0;
    bcm_ip_t        ip_addr = 0;
    int		        module = 0;
    int             trunk = -1;
    int             untag = 0;
    int             port = 0;
    int             hits = 0;
    int             replace = 0;
#ifdef BCM_XGS_SWITCH_SUPPORT
    int             l2tocpu = 0;
#endif /* BCM_XGS_SWITCH_SUPPORT */ 
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    int             rpe = 0;
    int             v6  = 0;
    int             pri = 0;
    int             vrf = 0;
    int             lookup_class = 0;
    bcm_ip6_t       ip6_addr;
#endif
    int           unit;
    DAPI_PORT_t  *dapiPortPtr;
    BROAD_PORT_t *hapiPortPtr;
    /* Input parameters */

    /* Input port/trunk */
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "dapiPort={%d,%d,%d}",
              dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);

    /* Validate dapiPort */
    if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Invalid interface");
      return L7_FAILURE;
    }

    /* Get port pointers */
    DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);

    unit    = hapiPortPtr->bcm_unit;
    module  = hapiPortPtr->bcm_modid;

    /* Accept only physical and lag interfaces */
    if ( IS_PORT_TYPE_PHYSICAL(dapiPortPtr) )
    {
      port = hapiPortPtr->bcm_port;
    }
    else if ( IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
    {
      trunk = hapiPortPtr->hapiModeparm.lag.tgid;
    }
    else
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Interface has a not valid type: error!");
      return L7_FAILURE;
    }

    /* Init struct */
    bcm_l3_host_t_init(&host_info);

    host_info.l3a_vrf     = vrf;
    host_info.l3a_pri     = pri;
    host_info.l3a_lookup_class = lookup_class;

    host_as_route = data->host_as_route;
    ip_addr       = data->dstIpAddr;
    untag         = data->untag;
    hits          = data->hits;
    replace       = data->replace;
    l2tocpu       = data->l2ToCpu;
    rpe           = data->rpe;
    v6            = 0;
    pri           = data->pri;
    vrf           = data->vrf;
    lookup_class  = data->lookup_class;

    /* Intf id */
    interface = data->l3_intf;

    /* IP address */
    ip_addr = data->dstIpAddr;

    /* DMAC */
    sal_memcpy(mac, data->dstMacAddr, sizeof(L7_uint8)*L7_MAC_ADDR_LEN);

    if (rpe)
    {
      host_info.l3a_flags |= BCM_L3_RPE;
    }

    if (v6)
    {
        memcpy(host_info.l3a_ip6_addr, ip6_addr, BCM_IP6_ADDRLEN);
        host_info.l3a_flags |= BCM_L3_IP6;
    }
    else 
    {
        host_info.l3a_ip_addr = ip_addr;
    } 

    if (host_as_route) {
        host_info.l3a_flags |= BCM_L3_HOST_AS_ROUTE;
    } 
    host_info.l3a_intf = interface;
    sal_memcpy(host_info.l3a_nexthop_mac, mac, sizeof(mac));

    if (BCM_GPORT_IS_SET(port)) {
        host_info.l3a_port_tgid = port;
    }
    else
    {
      host_info.l3a_modid = module;
      if (trunk >= 0)
      {
          host_info.l3a_flags |= BCM_L3_TGID;
          host_info.l3a_port_tgid = trunk;
      }
      else
      {
          host_info.l3a_port_tgid = port;
      }
    }

    if (hits) {
        host_info.l3a_flags |= BCM_L3_HIT;
    }
    if (untag) {
        host_info.l3a_flags |= BCM_L3_UNTAG;
    }

    if (replace) {
        host_info.l3a_flags |= BCM_L3_REPLACE;
    }

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (l2tocpu)
    {
        host_info.l3a_flags |= BCM_L3_L2TOCPU;
    }
#endif

    /* Add host */
    rv = bcm_l3_host_add(unit, &host_info);

    if (BCM_FAILURE(rv))
    {
        LOG_ERR(LOG_CTX_PTIN_HAPI,"Error creating entry in L3 host table: rv=\"%s\"", bcm_errmsg(rv));
        return L7_FAILURE;
    }

    return L7_SUCCESS;
} 

/**
 * Remove L3 host
 * 
 * @param dapiPort 
 * @param intf_id 
 * @param dstMacAddr 
 * @param dstIpAddr 
 * 
 * @return RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_l3_host_remove(ptin_dapi_port_t *dapiPort, st_ptin_l3 *data)
{
  LOG_ERR(LOG_CTX_PTIN_HAPI,"Not implemented yet");
  return L7_NOT_IMPLEMENTED_YET;
}

/**
 * Add L3 route
 * 
 * @param dapiPort 
 * @param intf_id 
 * @param dstMacAddr 
 * @param dstIpAddr 
 * 
 * @return RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_l3_route_add(ptin_dapi_port_t *dapiPort, st_ptin_l3 *data)
{
  LOG_ERR(LOG_CTX_PTIN_HAPI,"Not implemented yet");
  return L7_NOT_IMPLEMENTED_YET;
}

/**
 * Remove L3 route
 * 
 * @param dapiPort 
 * @param intf_id 
 * @param dstMacAddr 
 * @param dstIpAddr 
 * 
 * @return RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_l3_route_remove(ptin_dapi_port_t *dapiPort, st_ptin_l3 *data)
{
  LOG_ERR(LOG_CTX_PTIN_HAPI,"Not implemented yet");
  return L7_NOT_IMPLEMENTED_YET;
}
