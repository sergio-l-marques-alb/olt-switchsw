/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename   l7_usl_bcm_wlan.h
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

#ifndef L7_USL_BCM_WLAN_H
#define L7_USL_BCM_WLAN_H

#include "bcmx/multicast.h"
#include "bcmx/vlan.h"
#include "bcm/tunnel.h"
#include "bcm/wlan.h"

bcm_error_t usl_bcmx_wlan_tunnel_vlan_add(uint32 flags, bcm_multicast_t group, bcm_vlan_t vid);
bcm_error_t usl_bcmx_wlan_tunnel_vlan_delete(bcm_multicast_t group, bcm_vlan_t vid);
#ifdef INCLUDE_L3
bcm_error_t usl_bcmx_wlan_port_add(bcm_tunnel_initiator_t *init, 
                                   bcm_tunnel_terminator_t *term, 
                                   bcm_wlan_port_t *wlan_port);
bcm_error_t usl_bcmx_wlan_port_delete(bcm_port_t port, bcm_gport_t tunnel_id, bcm_tunnel_terminator_t *term);
#endif

#endif /* L7_USL_BCM_WLAN */
