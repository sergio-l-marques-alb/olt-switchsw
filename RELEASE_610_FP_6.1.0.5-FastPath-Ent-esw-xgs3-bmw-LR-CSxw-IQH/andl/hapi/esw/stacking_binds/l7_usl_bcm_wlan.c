/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2009
*
**********************************************************************
*
* @filename   l7_usl_bcm_wlan.c
*
* @purpose    General routine for USL wlan
*
* @component  HAPI
*
* @comments   none
*
* @create     02/26/2009 
*
* @author     jasonsha 
* @end
*
**********************************************************************/

#include "l7_usl_bcm_wlan.h"
#include "broad_common.h"
#include "bcmx/wlan.h"
#include "bcmx/port.h"
#include "bcmx/bcmx_int.h"



/*********************************************************************
* @purpose  Adds the entry to the usl wlan table 
*
* @param   flags     @b{(input)} creation flags
* @param   group    @b{(input)} multicast group
* @param   vid     @b{(input)} vlan Id 
*
* @returns bcm_error_t
*
* @end
*
*********************************************************************/
bcm_error_t usl_bcmx_wlan_tunnel_vlan_add(uint32 flags, bcm_multicast_t group, bcm_vlan_t vid)
{
    int rv = BCM_E_NONE;
    bcm_vlan_control_vlan_t vlan_control;

    rv = bcmx_multicast_create(flags, &group);
    if (L7_BCMX_OK(rv) == L7_FALSE)
    {
        return rv;
    }

    if ((rv = bcmx_vlan_control_vlan_get(vid, &vlan_control)) == BCM_E_NONE)
    {
      vlan_control.broadcast_group = group;
      vlan_control.unknown_multicast_group = group;
      vlan_control.unknown_unicast_group = group;
      if ((rv = bcmx_vlan_control_vlan_set(vid, vlan_control)) != BCM_E_NONE)
      {
        return rv;
      }
    }

    return (rv);
}
 
/*********************************************************************
* @purpose  Deletes the entry from the uslwlan table 
*
* @param   vid     @b{(input)} vlan Id 
*
* @returns  bcm_error_t 
*
* @end
*
*********************************************************************/
bcm_error_t usl_bcmx_wlan_tunnel_vlan_delete(bcm_multicast_t group, bcm_vlan_t vid)
{
    int rv = BCM_E_NONE;
    bcm_vlan_control_vlan_t vlan_control;

    /* Call BCMX layer to delete table entry in all units. */

    if ((rv = bcmx_vlan_control_vlan_get(vid, &vlan_control)) == BCM_E_NONE)
    {
      vlan_control.broadcast_group = 0;
      vlan_control.unknown_multicast_group = 0;
      vlan_control.unknown_unicast_group = 0;
      if ((rv = bcmx_vlan_control_vlan_set(vid, vlan_control)) != BCM_E_NONE)
      {
        return rv;
      }
    }

    rv = bcmx_multicast_destroy(group);

    return (rv);
}

#ifdef INCLUDE_L3

static int l7_bcmx_tunnel_terminator_add(bcm_tunnel_terminator_t *terminator)
{
  int i, bcm_unit, rv = BCM_E_NONE, rv_final = BCM_E_NONE;
  bcm_port_config_t config;

  BCMX_UNIT_ITER(bcm_unit, i) 
  {
    rv = bcm_port_config_get(bcm_unit, &config);
    if (rv == BCM_E_NONE)
    {
      BCM_PBMP_ASSIGN(terminator->pbmp, config.port);
      rv = bcm_tunnel_terminator_add(bcm_unit, terminator);
    }
    if (rv < rv_final)
    {
      rv_final = rv;
    }
  }

  return rv;
}


static int l7_bcmx_tunnel_terminator_delete(bcm_tunnel_terminator_t *terminator)
{
  int i, bcm_unit, rv = BCM_E_NONE;

  BCMX_UNIT_ITER(bcm_unit, i) 
  {
    rv = bcm_tunnel_terminator_delete(bcm_unit, terminator);
  }

  return rv;
}

/*********************************************************************
* @purpose  Adds the entry to the usl wlan table 
*
* @param   flags     @b{(input)} creation flags
* @param   group    @b{(input)} multicast group
* @param   vid     @b{(input)} vlan Id 
*
* @returns bcm_error_t
*
* @end
*
*********************************************************************/
bcm_error_t usl_bcmx_wlan_port_add(bcm_tunnel_initiator_t *init, 
                                   bcm_tunnel_terminator_t *term, 
                                   bcm_wlan_port_t *wlan_port)
{
  int rv = BCM_E_NONE;
  int replace_flag = 0;

  if ((init != NULL && (init->flags & BCM_TUNNEL_REPLACE)) ||
      (term != NULL && (term->flags & BCM_TUNNEL_REPLACE)) ||
       (wlan_port != NULL && (wlan_port->flags & BCM_WLAN_PORT_REPLACE))
       )
  {
    replace_flag = 1;
  }

  /* call bcmx */

  if (init != NULL)
  {
    rv = bcmx_wlan_tunnel_initiator_create(init);
    if (rv != BCM_E_NONE)
    {
      return rv;
    }
  }

  if (term != NULL && init != NULL)
  {
    term->tunnel_id = init->tunnel_id; /*  use same tunnel id for term & init */
    rv = l7_bcmx_tunnel_terminator_add (term);
    if (rv != BCM_E_NONE)
    {
      (void) bcmx_wlan_tunnel_initiator_destroy(init->tunnel_id);
      return rv;
    }
  }

  if (wlan_port != NULL && term != NULL && init != NULL)
  {
    wlan_port->match_tunnel = term->tunnel_id;
    wlan_port->egress_tunnel = init->tunnel_id;
  }

  if (wlan_port != NULL)
  {
    rv = bcmx_wlan_port_add(wlan_port);

    if (rv == BCM_E_NONE)
    {
      (void) bcmx_port_vlan_member_set(wlan_port->wlan_port_id, BCM_PORT_VLAN_MEMBER_INGRESS);
      (void) bcmx_port_stat_enable_set(wlan_port->wlan_port_id, 1);
    }
  }


  return(rv);
}

/*********************************************************************
* @purpose  Deletes the entry from the uslwlan table 
*
* @param   port     @b{(input)} wlan port Id 
*
* @returns  bcm_error_t 
*
* @end
*
*********************************************************************/
bcm_error_t usl_bcmx_wlan_port_delete(bcm_gport_t wlan_port_id, bcm_gport_t tunnel_id, 
                                      bcm_tunnel_terminator_t *term)
{
  int rv = BCM_E_NONE;

  /* call bcmx */

  if (wlan_port_id != 0)
  {
    rv = bcmx_wlan_port_delete(wlan_port_id);
  }

  if (term != NULL)
  {
    rv = l7_bcmx_tunnel_terminator_delete(term);
  }

  if (tunnel_id != 0)
  {
    rv = bcmx_wlan_tunnel_initiator_destroy(tunnel_id);
  }

  return(rv);
}
#endif



