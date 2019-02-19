/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcm_l3.c
*
* @purpose    Handle synchronization responsibilities for Routing and ARP tables
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

#include "osapi.h"
#include "sysapi.h"
#include "l7_usl_bcm.h"
#include "bcmx/l3.h"
#include "bcmx/bcmx_int.h"
#include "bcmx/tunnel.h"
#include "bcm/port.h"
#include "ibde.h"
#include "soc/drv.h"
#include "bcm_int/esw/multicast.h"

/*********************************************************************
* @purpose  Initialize the IPMC tables, semaphores, ...
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized 
*
* @notes    On error, all resources will be released
*       
* @end
*********************************************************************/
L7_RC_t usl_l3_init()
{
  return L7_SUCCESS;
}
 
/*********************************************************************
* @purpose  Deallocate the IPMC tables, semaphores, ...
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized 
*       
* @end
*********************************************************************/
L7_RC_t usl_l3_fini()
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create an L3 Interface

*
* @param    intfInfo      @{(input)} Interface attributes
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_l3_intf_create(bcmx_l3_intf_t *info)
{
  return bcmx_l3_intf_create(info);
}

/*********************************************************************
* @purpose  Delete an L3 Interface
*
* @param    intfInfo      @{(input)} Interface attributes
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_l3_intf_delete(bcmx_l3_intf_t *info)
{
  return bcmx_l3_intf_delete(info);
}

/*********************************************************************
* @purpose  Get the max route entries that can be handled  in a single
*           transaction
*
* @param    none
*
* @returns  Number of route entries in a bulk transaction
*
* @end
*********************************************************************/
L7_uint32 usl_l3_route_max_entries_get(void)
{
  /* No bulk transactions on standalone packages */
  return 1;
}

/*********************************************************************
* @purpose  Add route entry (or entries) to the table
*
* @param    info     @{(input)}  The address, masks, and associated data
* @param    count    @{(input)}  Number of route entries passed
* @param    *rv      @{(output)} Return code each of the routes (SDK rv)
*
* @returns  L7_RC_t
*
* @notes    Caller must allocate enough memory for output param
*
* @end
*********************************************************************/
L7_RC_t usl_bcmx_l3_route_add(bcmx_l3_route_t *info, L7_uint32 count, L7_int32 *rv)
{
  L7_uint32 i;

  if ((count == 0) || (info == L7_NULLPTR) || (rv == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  for (i=0; i < count; i++)
  {
    rv[i] = bcmx_l3_route_add(info);
    info++;
  }
 
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete an LPM entry or entries from the table
*
* @param    info     @{(input)}  The address, masks, and associated data
* @param    count    @{(input)}  Number of route entries passed
* @param    *rv      @{(output)} Return code each of the routes (SDK rv)
*
* @returns  L7_RC_t
*
* @notes    Caller must allocate enough memory for output param
*
* @end
*********************************************************************/
L7_RC_t usl_bcmx_l3_route_delete(bcmx_l3_route_t *info, L7_uint32 count, L7_int32 *rv)
{
  L7_uint32 i;

  if ((count == 0) || (info == L7_NULLPTR) || (rv == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  for (i=0; i < count; i++)
  {
    rv[i] = bcmx_l3_route_delete(info);
    info++;
  }

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Get the max host entries that can be handled by underlying RPC layer
*           in a single transaction
*
* @param    none
*
* @returns  Number of host entries in a bulk transaction
*
* @end
*********************************************************************/
L7_uint32 usl_l3_host_max_entries_get(void)
{
  /* No bulk transactions on standalone packages */
  return 1;
}

/*********************************************************************
* @purpose  Add a Host entry(or entries)
*
* @param    info     @{(input)}  The address and associated data
* @param    count    @{(input)}  Number of host entries passed
* @param    *rv      @{(output)} Return code each of the hosts (SDK rv)
*
* @returns  L7_RC_t
*
* @notes    Caller must allocate enough memory for output param
*
* @end
*********************************************************************/
L7_RC_t usl_bcmx_l3_host_add(bcmx_l3_host_t *info, L7_uint32 count, L7_int32 *rv)

{
  L7_uint32 i;

  if ((count == 0) || (info == L7_NULLPTR) || (rv == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  for (i=0; i < count; i++)
  {
    info->l3a_flags &= ~(BCM_L3_S_HIT | BCM_L3_D_HIT); /* make sure hit clear */
    rv[i] = bcmx_l3_host_add(info);
    info++;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Delete a host entry or entries from the table
*
* @param    info     @{(input)}  IP address and associated data
* @param    count    @{(input)}  Number of host entries passed
* @param    *rv      @{(output)} Return code each of the host (SDK rv)
*
* @returns  L7_RC_t
*
* @notes    Caller must allocate enough memory for output param
*
* @end
*********************************************************************/
L7_RC_t usl_bcmx_l3_host_delete(bcmx_l3_host_t *info, L7_uint32 count, L7_int32 *rv)
{
  L7_uint32 i;

  if ((count == 0) || (info == L7_NULLPTR) || (rv == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  for (i=0; i < count; i++)
  {
    rv[i] = bcmx_l3_host_delete(info);
    info++;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Get the max egress nhop entries that can be handled
*           in a single transaction
*
* @param    none
*
* @returns  Number of egress nhop entries in a bulk transaction
*
* @end
*********************************************************************/
L7_uint32 usl_l3_egress_nhop_max_entries_get(void)
{
  /* No bulk operations on standalone package. return 1 */
  return 1; 
}

/*********************************************************************
* @purpose  Convert BCMX egress structure to the BCM egress structure.
*
*   This code is a copy of _bcmx_l3egress_to_bcm() which is not an API
*
* @end
*********************************************************************/
static void
l7_bcmx_l3egress_to_bcm(bcmx_l3_egress_t *info, bcm_l3_egress_t *l3)
{
  bcm_l3_egress_t_init(l3);

  l3->flags = info->flags;
  l3->intf  = info->intf;
  memcpy(l3->mac_addr, info->mac_addr, sizeof(bcm_mac_t));
  l3->vlan = info->vlan;
  l3->mpls_label = info->mpls_label;

  if (info->flags & BCM_L3_TGID) {
    l3->module= 0;
    l3->trunk = info->trunk;
  }
  else
  {
    if (BCM_GPORT_IS_WLAN_PORT(info->lport))
    {
      l3->module = 0;
      l3->port = info->lport;
    }
    else if (BCM_FAILURE(bcmx_lport_to_modid_port(info->lport,
                                             &l3->module,
                                             &l3->port)))
    {
      l3->module = -1;
      l3->port = -1;
    }
  }
}

/*********************************************************************
* @purpose  Create an egress next hop object(s)
*
* @param    pFlags    @{(input)}  Flags for each nexthops
* @param    pBcmxInfo @{(input)}  BCMX data for each nexthops
* @param    count     @{(input)}  Number of nexthops passed
* @param    pEgrIntf  @{(output)} Egress Ids of the nexthops
* @param    rv        @{(output)} Return code each of the routes (SDK rv)
*
* @returns  L7_RC_t
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usl_bcmx_l3_egress_create(L7_uint32 *pFlags,
                                  bcmx_l3_egress_t *pBcmxInfo,
                                  L7_uint32 count,
                                  bcm_if_t *pEgrIntf,
                                  L7_int32 *rv)
{
  L7_uint32 i;
  int j, bcm_unit;
  bcm_l3_egress_t bcmInfo;

  if ((count == 0) || (pBcmxInfo == L7_NULLPTR) || (pEgrIntf == L7_NULLPTR) ||
      (pFlags == L7_NULLPTR) || (rv == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  for (i=0; i < count; i++)
  {
    l7_bcmx_l3egress_to_bcm(pBcmxInfo, &bcmInfo);

    BCMX_UNIT_ITER(bcm_unit, j) {
      rv[i] = bcm_l3_egress_create(bcm_unit, pFlags[i], &bcmInfo, pEgrIntf);
    }
    pBcmxInfo++;
    pEgrIntf++;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Destroy an egress next hop object(s)
*
* @param    pEgrIntf    @{(input)} Egress object interface id(s)
* @param    count       @{(input)} Number of egress Ids to destroy
* @param    rv          @{(output)} Return code for each entry
*
* @returns  L7_RC_t
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usl_bcmx_l3_egress_destroy(bcm_if_t *pEgrIntf, L7_uint32 count,
                                   L7_int32 *rv)
{
  L7_uint32 i;

  if ((count == 0) || (pEgrIntf == L7_NULLPTR) || (rv == L7_NULLPTR))
  {
    return L7_FAILURE;
  }
  
  for (i=0; i < count; i++)
  {
    rv[i] = bcmx_l3_egress_destroy(pEgrIntf[i]);
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Create a multipath/ECMP egress next hop object
*
* @param    flags        @{(input)} Flags = create/replace
* @param    intf_count   @{(input)} Number of egress nhops
* @param    intf_array   @{(input)} Array of egress nhop Ids
* @param    mpintf       @{(input/ouput)} Multipath egress obj id 
*
* @returns  Defined by the Broadcom driver
*
* @notes
*
* @end
*********************************************************************/
int usl_bcmx_l3_egress_multipath_create(L7_uint32 flags, L7_int32 intf_count,
                                        bcm_if_t * intf_array, bcm_if_t *mpintf)
{
  return bcmx_l3_egress_multipath_create(flags, intf_count, intf_array, mpintf);
}


/*********************************************************************
* @purpose  Destroy a multipath/ECMP egress next hop object
*
* @param    mpintf   @{(input)} Multipath egress object id
*
* @returns  Defined by the Broadcom driver
*
* @notes
*
* @end
*********************************************************************/
int usl_bcmx_l3_egress_multipath_destroy(bcm_if_t mpintf)
{
  return bcmx_l3_egress_multipath_destroy(mpintf);
}


/*********************************************************************
* @purpose  Add a Tunnel Initiator entry to the table
*
* @param    intf       @{(input)} The tunnel interface info
* @param    initiator  @{(input)} The tunnel initiator info
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_tunnel_initiator_set(bcmx_l3_intf_t * intf,
                                     bcmx_tunnel_initiator_t * initiator)
{
  return bcmx_tunnel_initiator_set(intf, initiator);
}

/*********************************************************************
* @purpose  Delete a Tunnel Initiator entry from the table
*
* @param    intf       @{(input)} The tunnel interface info
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_tunnel_initiator_clear(bcmx_l3_intf_t * intf)
{
  return bcmx_tunnel_initiator_clear(intf);
}

/*********************************************************************
* @purpose  Convert BCMX terminator structure to the BCM terminator structure.
*
*   This code is a copy of _bcmx_l3tunnel_to_bcm()
*
* @end
*********************************************************************/
static void
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
* @purpose  Add a Tunnel Terminator entry to the table
*
* @param    terminator @{(input)} The tunnel terminator info
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_tunnel_terminator_add(bcmx_tunnel_terminator_t *terminator)
{
  bcm_tunnel_terminator_t bcm_terminator;
  bcm_port_config_t config;
  L7_uint32 i;
  int rv;
  int rv_final = BCM_E_NONE;

  l7_bcmx_terminator_to_bcm(terminator, &bcm_terminator);

#ifdef BCM_ROBO_SUPPORT
  for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
#else
  for (i = 0; i < bde->num_devices(BDE_ALL_DEVICES); i++)
#endif
  {
    if (!SOC_IS_XGS_FABRIC(i))
    {
      bcm_port_config_get(i, &config);
      bcm_terminator.pbmp = config.all;
      rv = bcm_tunnel_terminator_add (i, &bcm_terminator);
      if (rv < rv_final)
      {
        rv_final = rv;
      }
    }
  }

  return rv_final;  /* Return the worst error code. */
}

/*********************************************************************
* @purpose  Delete a Tunnel Terminator entry from the table
*
* @param    terminator @{(input)} The tunnel terminator info
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_tunnel_terminator_delete(
                                  bcmx_tunnel_terminator_t *terminator)
{
  bcm_tunnel_terminator_t bcm_terminator;
  bcm_port_config_t config;
  L7_uint32 i;
  int rv;
  int rv_final = BCM_E_NONE;

  l7_bcmx_terminator_to_bcm(terminator, &bcm_terminator);

#ifdef BCM_ROBO_SUPPORT
  for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
#else
  for (i = 0; i < bde->num_devices(BDE_ALL_DEVICES); i++)
#endif
  {
    if (!SOC_IS_XGS_FABRIC(i))
    {
      bcm_port_config_get(i, &config);
      bcm_terminator.pbmp = config.all;
      rv = bcm_tunnel_terminator_delete (i, &bcm_terminator);
      if (rv < rv_final)
      {
        rv_final = rv;
      }
    }
  }

  return rv_final;  /* Return the worst error code. */
}


/*********************************************************************
* @purpose  Add a WLAN virtual port to an IP multicast group
*
* @param    unit       @{(input)} bcm unit
* @param    index      @{(input)} ipmc hardware table index
* @param    vlan_id    @{(input)} vlan routing interface id
* @param    port       @{(input)} wlan gport
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_l3_ipmc_wlan_port_add(L7_int32 unit, L7_int32 index, 
                                  L7_uint32 vlan_id, bcm_port_t port)
{
  bcm_l3_egress_t egr;
  bcm_if_t egrintf;
  bcm_gport_t phys_port;
  bcm_wlan_port_t wlan_info;
  bcm_l3_intf_t l3intf;
  bcm_multicast_t group = 0;
  L7_int32 rv = BCM_E_NONE;

  bcm_wlan_port_t_init(&wlan_info);
  bcm_l3_egress_t_init(&egr);
  bcm_l3_intf_t_init(&l3intf);

  egr.port = port;
  egr.vlan = vlan_id;
  l3intf.l3a_vid = vlan_id;

  /* find the l3 interface object linked to the vlan routing interface */
  rv = bcm_l3_intf_find_vlan(unit, &l3intf);
  if (rv != BCM_E_NONE)
  {
    return BCM_E_FAIL;
  }

  egr.intf = l3intf.l3a_intf_id;

  rv = bcm_l3_egress_find(unit, &egr, &egrintf);
  if (rv != BCM_E_NONE)
  {
    egr.port = port;
    rv = bcm_l3_egress_create(unit, 0, &egr, &egrintf);
  }

  if (rv == BCM_E_NONE && bcm_wlan_port_get(unit, port, &wlan_info) == BCM_E_NONE)
  {
    phys_port = wlan_info.port;

    /* get a group handle */
    _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_L3, index);
    rv = bcm_multicast_egress_add(unit, group, phys_port, egrintf);

    if (rv != BCM_E_NONE)
    {
      sysapiPrintf(" bcm_multicast_egress_add with port %d; rv = %d _BCM_MULTICAST_ID_GET %d isl3 %d; group %d \n", 
             port, rv, _BCM_MULTICAST_ID_GET(group), _BCM_MULTICAST_IS_L3(group), group);
    }
  }
  else
  {
    sysapiPrintf(" bcm_l3_egress_create / bcm_l3_egress_find failed  with port %d; rv = %d; group %d \n", 
           port, rv, group);

  }

  return rv;
}

/*********************************************************************
* @purpose  Delete a WLAN virtual port from an IP multicast group
*
* @param    unit       @{(input)} bcm unit
* @param    index      @{(input)} ipmc hardware table index
* @param    vlan_id    @{(input)} vlan routing interface id
* @param    port       @{(input)} wlan gport
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_l3_ipmc_wlan_port_delete(L7_int32 unit, L7_int32 index, 
                                     L7_uint32 vlan_id, bcm_port_t port)
{
  bcm_l3_egress_t egr;
  bcm_if_t egrintf;
  bcm_gport_t phys_port;
  bcm_wlan_port_t wlan_info;
  bcm_l3_intf_t l3intf;
  bcm_multicast_t group;
  int rv = BCM_E_NONE;

  bcm_wlan_port_t_init(&wlan_info);
  bcm_l3_egress_t_init(&egr);
  bcm_l3_intf_t_init(&l3intf);

  egr.port = port;
  egr.vlan = vlan_id;
  l3intf.l3a_vid = vlan_id;

  /* find the l3 interface object linked to the vlan routing interface */
  rv = bcm_l3_intf_find_vlan(unit, &l3intf);
  if (rv != BCM_E_NONE)
  {
    return BCM_E_FAIL;
  }

  egr.intf = l3intf.l3a_intf_id;

  rv = bcm_l3_egress_find(unit, &egr, &egrintf);
  if (rv != BCM_E_NONE)
  {
    sysapiPrintf(" bcm_l3_egress_find with port %d; rv = %d \n", port, rv);
  }

  if (rv == BCM_E_NONE && bcm_wlan_port_get(unit, port, &wlan_info) == BCM_E_NONE)
  {
    phys_port = wlan_info.port;

    /* get a group handle */
    _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_L3, index);
    rv = bcm_multicast_egress_delete(unit, group, phys_port, egrintf);
    if (rv != BCM_E_NONE)
    {
      sysapiPrintf(" bcm_multicast_egress_delete with port %d; rv = %d \n", port, rv);
    }

    rv = bcm_l3_egress_destroy(unit, egrintf);
    if (rv != BCM_E_NONE)
    {
      sysapiPrintf(" bcm_l3_egress_destroy with port %d; rv = %d \n", port, rv);
    }

  }

  return rv;
}


/*********************************************************************
* @purpose  Add a WLAN virtual port to an IP multicast group (L2 bitmap)
*
* @param    unit       @{(input)} bcm unit
* @param    index      @{(input)} ipmc hardware table index
* @param    port       @{(input)} wlan gport
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_l2_ipmc_wlan_port_add(L7_int32 unit, L7_int32 index, 
                                  bcm_port_t port)
{
  bcm_wlan_port_t wlan_info;
  bcm_multicast_t group = 0;
  L7_int32 rv = BCM_E_NONE;
  bcm_if_t encap_id;
  bcm_gport_t wlanPort = 0;

  bcm_wlan_port_t_init(&wlan_info);

  if (!BCM_GPORT_IS_WLAN_PORT(port))
  {
    return BCM_E_FAIL;
  }

  rv = bcm_multicast_wlan_encap_get(unit, index, port, port, &encap_id);
  if (rv == BCM_E_NONE)
  {
    rv = bcm_wlan_port_get(unit, port, &wlan_info);
    wlanPort = port;
    port = wlan_info.port;

  }

  if (rv != BCM_E_NONE)
  {
    return rv;
  }

  /* get a group handle */
  _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_L3, index);

  rv = bcm_multicast_egress_add(unit, group, port, encap_id);

  return rv;
}

/*********************************************************************
* @purpose  Delete a WLAN virtual port from an IP multicast group (L2 bitmap)
*
* @param    unit       @{(input)} bcm unit
* @param    index      @{(input)} ipmc hardware table index
* @param    port       @{(input)} wlan gport
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_l2_ipmc_wlan_port_delete(L7_int32 unit, L7_int32 index, 
                                     bcm_port_t port)
{
  bcm_wlan_port_t wlan_info;
  bcm_multicast_t group;
  int rv = BCM_E_NONE;
  bcm_if_t encap_id;
  bcm_gport_t wlanPort = 0;

  bcm_wlan_port_t_init(&wlan_info);


  if (!BCM_GPORT_IS_WLAN_PORT(port))
  {
    return BCM_E_FAIL;
  }

  rv = bcm_multicast_wlan_encap_get(unit, index, port, port, &encap_id);
  if (rv == BCM_E_NONE)
  {
    rv = bcm_wlan_port_get(unit, port, &wlan_info);
    wlanPort = port;
    port = wlan_info.port;
  }

  if (rv != BCM_E_NONE)
  {
    return rv;
  }

  /* get a group handle */
  _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_L3, index);

  rv = bcm_multicast_egress_delete(unit, group, port, encap_id);

  return rv;
}


#endif /* L7_ROUTING_PACKAGE */
