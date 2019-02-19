
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_usl_port_cache.h
*
* @purpose    Implements a cache used for temporarily storing certain 
*             hardware configurations of the ports in a given 
*             Card (Unit/Slot). The stored configuration is applied 
*             to the hardware once the application layer is done 
*             issuing port config to the driver for the card (unit/slot).

*
* @component  HAPI
*
* @comments   none
*
* @create     3/31/2008
*
* @author     nshrivastav
* @end
*
**********************************************************************/

#ifndef L7_USL_PORT_CACHE_H
#define L7_USL_PORT_CACHE_H

#include "l7_common.h"
#include "osapi.h"

/*********************************************************************
*
* @purpose Initialize the usl port cache
*
*
* @returns L7_RC_t
*********************************************************************/
L7_RC_t l7_usl_port_cache_init(void);

/*********************************************************************
*
* @purpose Clear the usl port cache
*
*
* @returns L7_RC_t
*********************************************************************/
L7_RC_t l7_usl_port_cache_clear(void);

/*********************************************************************
*
* @purpose Setup an entry in the usl port cache for a bcmUnit/bcmPort
*
* @param   fpUnit    -  FP unit number
* @param   fpSlot    -  FP slot number
* @param   bcmUnit   -  The global bcm unit number
* @param   bcmPort   -  Bcm port number 
*
* @returns L7_RC_t
*********************************************************************/
L7_RC_t l7_usl_port_cache_entry_setup(L7_int8 fpUnit, L7_int8 fpSlot, int bcmUnit, int bcmPort);

/*********************************************************************
*
* @purpose Commit the contents of the usl port cache to the hardware
*
*
* @param   fpUnit         -  fp_unit number of the card to be committed   
* @param   fpSlot         -  fp_slot number of the card to be committed   

* @returns none
*
* @notes   Caller must take the card config lock before calling this routine.
*
*********************************************************************/
L7_RC_t l7_usl_port_cache_commit(L7_int8 fpUnit, L7_int8 fpSlot);


#endif
