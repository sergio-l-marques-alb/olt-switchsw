/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_usl_port_cache.c
*
* @purpose    Implements a cache used for temporarily storing certain 
*             hardware configurations of the ports in a given 
*             Card (Unit/Slot). The stored configuration is applied 
*             to the hardware once the application layer is done 
*             issuing port config to the driver for the card (unit/slot).
*             
*             
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

#include "l7_common.h"
#include "l7_usl_port_cache.h"

/*********************************************************************
*
* @purpose Initialize the usl port cache
*
*
* @returns L7_RC_t
*********************************************************************/
L7_RC_t l7_usl_port_cache_init(void)
{
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Clear the usl port cache
*
*
* @returns L7_RC_t
*********************************************************************/
L7_RC_t l7_usl_port_cache_clear(void)
{
  return L7_SUCCESS;
}

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
L7_RC_t l7_usl_port_cache_entry_setup(L7_int8 fpUnit, L7_int8 fpSlot, int bcmUnit, int bcmPort)
{
  return L7_SUCCESS;
}

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
L7_RC_t l7_usl_port_cache_commit(L7_int8 fpUnit, L7_int8 fpSlot)
{
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Add/Remove a port from the vlan in the usl cache
*
* @param   unit         - FP unit number
* @param   slot         - FP slot number
* @param   bcmUnit      - bcmUnit of the port
* @param   bcmPort      - bcmPort of the port
* @param   vlanId       - Vlan-id
* @param   tagged       - port is tagged member of vlan
* @param   vlanCmd      - L7_TRUE: Add port to the vlan
*                         L7_FALSE: Remove port from the vlan
*
*********************************************************************/
void l7_usl_port_cache_vlan_entry_set(L7_uint32 unit, L7_uint32 slot, L7_int32 bcmUnit, L7_int32 bcmPort, L7_uint32 vlanId, L7_BOOL tagged, L7_BOOL vlanCmd)
{
  return;
}
