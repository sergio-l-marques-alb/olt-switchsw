
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_bcmx_ipmcast.c
*
* @purpose    New layer to handle directing driver calls to specified units
*
* @component  HAPI
*
* @comments   none
*
* @create     6/20/2007
*
* @author     colinw
* @end
*
**********************************************************************/

#include "l7_common.h"

#ifdef L7_MCAST_PACKAGE

#include "osapi.h"
#include "soc/drv.h"
#include "bcmx/ipmc.h"
#include "bcm_int/control.h"
#include "bcmx/bcmx_int.h"
#include "l7_bcmx_ipmcast.h"
#include "log.h"

#include "broad_common.h"
#include "broad_utils.h"
#include "l7_usl_bcm_ipmcast.h"
#include "sysbrds.h"

#define RV_REPLACE(_trv, _rv)                   \
        BCMX_RV_REPLACE_OK(_trv, _rv, BCM_E_UNAVAIL)


/*********************************************************************
* @purpose  Set L2 ports for an ip multicast group
*
* @param    ipmc_index  @{(input)} Index of IPMC entry to be modified
* @param    lplist      @{(input)} List of ports to be included in L2 bitmap
* @param    ut_lplist   @{(input)} List of ports to be included in L2 untagged bitmap
* @param    vid         @{(input)} VLAN ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_bcmx_ipmc_set_l2_ports(L7_uint32      ipmc_index,
                              bcmx_lplist_t *lplist, 
                              bcmx_lplist_t *ut_lplist,
                              bcm_vlan_t     vid)

{
  int          rv = BCM_E_NONE, tmp_rv;
  int          i, bcm_unit;
  bcm_pbmp_t   pbmp, ubmp;
  bcmx_lport_t lport;

  BCMX_UNIT_ITER(bcm_unit, i)
  {
    BCMX_LPLIST_TO_PBMP(*lplist, bcm_unit, pbmp);
    BCMX_LPLIST_TO_PBMP(*ut_lplist, bcm_unit, ubmp);

    /* Arbitrarily choose the first port on this unit. The port is not really relevant
       other than to ensure the RPC is dispatched to the correct BCMX unit. */
    lport = bcmx_unit_port_to_lport(bcm_unit, 0);
    tmp_rv = customx_ipmc_l2_port_set(lport, ipmc_index, pbmp, ubmp, vid);
    RV_REPLACE(tmp_rv, rv);
  }

  return rv;
}
#endif /* L7_MCAST_PACKAGE */

