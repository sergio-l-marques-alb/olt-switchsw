/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcm_l3.h
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
#ifndef L7_USL_BCM_L3_H
#define L7_USL_BCM_L3_H

#include "l7_common.h"

#ifdef L7_ROUTING_PACKAGE

#include "dapi.h"
#include "bcmx/l3.h"
#include "bcmx/tunnel.h"

extern L7_BOOL uslDebugL3Enable;

typedef struct
{
  union 
  {
    bcm_ip_t  l3a_ip_net;   /* Subnet address (IPv4) */
    bcm_ip6_t l3a_ip6_net;   /* Subnet address (IPv4) */
  } l3a_subnet;

  union 
  {
    bcm_ip_t  l3a_ip_mask;  /* Subnet mask (IPv4) */
    bcm_ip6_t l3a_ip6_mask;  /* Subnet mask (IPv4) */
  } l3a_mask;

}usl_bcm_route_key_t;

typedef struct
{
  usl_bcm_route_key_t  routeKey; 
  uint32               l3a_flags;                   /* See BCM_L3_xxx flag definitions. */
  bcm_if_t             l3a_intf;                  /* L3 interface associated with route. */
  bcm_cos_t            l3a_pri;                  /* Priority (COS). */
} usl_bcm_l3_route_t;


typedef struct
{
  union 
  {
    bcm_ip_t l3a_ip_addr;   /* Subnet address (IPv4) */
    bcm_ip6_t l3a_ip6_addr;   /* Subnet address (IPv4) */
  } addr;
}usl_bcm_host_key_t;

typedef struct
{
  usl_bcm_host_key_t hostKey;
  uint32             l3a_flags;           /* See BCM_L3_xxx flag definitions. */
  bcm_cos_t          l3a_pri;          /* New priority in packet. */
  bcm_if_t           l3a_intf;          /* L3 intf associated with this address. */
} usl_bcm_l3_host_t;

typedef struct
{
  uint32 flags; /* Indicates V6 or V4 (BCM_L3_IP6) */
  union 
  {
    bcm_ip_t l3a_ip_addr;   /* Subnet address (IPv4) */
    bcm_ip6_t l3a_ip6_addr;   /* Subnet address (IPv6) */
  } addr;

  DAPI_USP_t  usp;  /* Passed by HAPI. Needed to uniquely identify NHOPs */

} usl_bcm_egr_nhop_key_t;

typedef struct
{
  usl_bcm_egr_nhop_key_t key;
  bcm_l3_egress_t        bcm_data;
} usl_bcm_l3_egress_t;

typedef struct
{
  bcm_ip_t sip;
  bcm_ip_t dip;
  bcm_tunnel_type_t type;
} usl_bcm_l3_intf_tunnel_info_t;

typedef struct
{
  bcm_l3_intf_t                   bcm_data;
  usl_bcm_l3_intf_tunnel_info_t   tunnelInfo; /* Valid only for tunnel L3 interfaces */
} usl_bcm_l3_intf_t;

#define USL_BCM_L3_INTF_INVALID ((bcm_if_t) -1)

/*********************************************************************
* @purpose  Convert usl_bcm_l3_host_t structure to the BCM host structure.
*
*
* @end
*********************************************************************/
void l7_l3host_to_bcm(usl_bcm_l3_host_t *l7BcmHost, bcm_l3_host_t *bcmHost);

/*********************************************************************
* @purpose  Convert usl_bcm_l3_route_t structure to the BCM route structure.
*
*
* @end
*********************************************************************/
void l7_l3route_to_bcm(usl_bcm_l3_route_t *l7BcmRoute, bcm_l3_route_t *bcmRoute);


/*********************************************************************
* @purpose  Initialize the usl_bcm_l3_intf_t structure
*
* @param
*
* @returns  BCM error code
*
* @end
*********************************************************************/
void usl_bcm_l3_intf_t_init(usl_bcm_l3_intf_t *intf);

/*********************************************************************
* @purpose  Custom RPC function to handle interface create
*
* @param
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_bcm_l3_intf_create (usl_bcm_l3_intf_t *intf);

/*********************************************************************
* @purpose  USL BCM API to handle interface delete
*
* @param
*
* @returns  BCM error code 
*
* @end
*********************************************************************/
int usl_bcm_l3_intf_delete (usl_bcm_l3_intf_t *intf);

/*********************************************************************
* @purpose  USL BCM API to handle Host adds.
*
* @param    
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_bcm_l3_host_add (usl_bcm_l3_host_t  *pHostInfo);

/*********************************************************************
* @purpose  USL BCM API to handle Host deletes.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
int usl_bcm_l3_host_delete (usl_bcm_l3_host_t  *pHostInfo);

/*********************************************************************
* @purpose  USL BCM API to handle Route adds.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
int usl_bcm_l3_route_add (usl_bcm_l3_route_t  *pRouteInfo);

/*********************************************************************
* @purpose  Custom RPC function to handle Route deletes.
*
* @param    
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
int usl_bcm_l3_route_delete (usl_bcm_l3_route_t  *pRouteInfo);


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
void usl_bcm_l3_egress_t_init(usl_bcm_l3_egress_t *egr);

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
                              bcm_if_t *egrId);

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
int usl_bcm_l3_egress_destroy (usl_bcm_l3_egress_t *pBcmInfo, bcm_if_t egrId);

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
                                        bcm_if_t *mpintf);

/*********************************************************************
* @purpose  USL BCM API to delete multipath egress objects
*
* @param    mpIntf         {(input)}  Mpath egress Id to be deleted
* @returns  bcm error code
*
* @notes    
* @end
*********************************************************************/
int usl_bcm_l3_egress_multipath_destroy (bcm_if_t mpIntf);

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
                                     bcm_tunnel_initiator_t *initiator);

/*********************************************************************
* @purpose  USL BCM API to handle tunnel initiator clears.
*
* @param    intf {(input)}
*
* @returns  bcm error code
*
* @end
*********************************************************************/
int usl_bcm_l3_tunnel_initiator_clear (bcm_l3_intf_t *intf);

/*********************************************************************
* @purpose  USL BCM API to handle tunnel terminator adds
*
* @param    terminator {(input)}
*
* @returns  bcm error code
*
* @end
*********************************************************************/
int usl_bcm_l3_tunnel_terminator_add(bcm_tunnel_terminator_t *terminator);

/*********************************************************************
* @purpose  USL BCM API to handle tunnel terminator deletes
*
* @param    terminator {(input)}
*
* @returns  bcm error code
*
* @end
*********************************************************************/
int usl_bcm_l3_tunnel_terminator_delete(bcm_tunnel_terminator_t *terminator);

#endif /* L7_ROUTING_PACKAGE */
#endif /* L7_USL_BCM_L3_H */
