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

    int           unit;
    DAPI_PORT_t  *dapiPortPtr;
    BROAD_PORT_t *hapiPortPtr;

    bcm_module_t      module = 0;
    bcm_port_t        port = 0;
    bcm_trunk_t       trunk = -1;
    bcm_mac_t         mac;
    bcm_if_t          interface = -1;
    int               l2tocpu = 0;
    int               rv = L7_SUCCESS;


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



    /* _l3_cmd_egress_object_create */

    bcm_l3_egress_t   egress_object;             
    int               copytocpu = 0;
    int               drop = 0;
    int               mpls_label = BCM_MPLS_LABEL_INVALID;
    int               object_id = -1;
    uint32            flags = 0;

    bcm_l3_egress_t_init(&egress_object);
    sal_memset(mac, 0, sizeof(bcm_mac_t));

    /* DMAC */
    sal_memcpy(mac, data->dstMacAddr, sizeof(L7_uint8)*L7_MAC_ADDR_LEN);

    /* Intf id */
    interface = data->l3_intf;

    egress_object.intf   = interface;
    egress_object.mpls_label = mpls_label;
    sal_memcpy(egress_object.mac_addr, mac, sizeof(mac));

    if (BCM_GPORT_IS_SET(port)) {
        egress_object.port = port;
    } else {
        egress_object.module = module;
    if (trunk >= 0) {
        egress_object.flags |= BCM_L3_TGID;
        egress_object.trunk = trunk;
    } else {
        egress_object.port = port;
    }
    }

    if (l2tocpu) {
        egress_object.flags |= BCM_L3_L2TOCPU;
    }

    if (copytocpu) {
        egress_object.flags |= (BCM_L3_COPY_TO_CPU);
    }

    if (drop) {
        egress_object.flags |= (BCM_L3_DST_DISCARD);
    }

    if (object_id >= 0) {
        flags = (BCM_L3_WITH_ID | BCM_L3_REPLACE);
    }


    rv = bcm_l3_egress_create(unit, flags, &egress_object, &object_id);
    if (BCM_FAILURE(rv)) {
        printk("Error creating egress object entry: %s\n", bcm_errmsg(rv));
        return L7_FAILURE;
    }


    /* _l3_cmd_host_add */

    bcm_l3_host_t   host_info;
    int             host_as_route = 0;
    bcm_ip_t        ip_addr = 0;
    int             untag = 0;
    int             hits = 0;
    int             replace = 0;
    int             rpe = 0;
    int             v6  = 0;
    int             pri = 0;
    int             vrf = 0;
    int             lookup_class = 0;
    bcm_ip6_t       ip6_addr;


    /* Init struct */
    bcm_l3_host_t_init(&host_info);

    host_info.l3a_vrf     = vrf;
    host_info.l3a_pri     = pri;
    host_info.l3a_lookup_class = lookup_class;

    /* Intf id */
    interface = object_id;  /* Object ID from _l3_cmd_egress_object_create */

    host_as_route = data->host_as_route;
    ip_addr       = data->dstIpAddr;    /* IP address */
    untag         = data->untag;
    hits          = data->hits;
    replace       = data->replace;
    l2tocpu       = data->l2ToCpu;
    rpe           = data->rpe;
    v6            = 0;
    pri           = data->pri;
    vrf           = data->vrf;
    lookup_class  = data->lookup_class;

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
