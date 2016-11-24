/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcm_l2.c
*
* @purpose    New layer to handle directing driver calls to specified units
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

#ifndef L7_BCMX_IPMCAST_H
#define L7_BCMX_IPMCAST_H

#include "l7_common.h"
#include "osapi.h"
#include "bcmx/ipmc.h"
#include "hpc_hw_api.h"

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
                              bcm_vlan_t     vid);

#endif
