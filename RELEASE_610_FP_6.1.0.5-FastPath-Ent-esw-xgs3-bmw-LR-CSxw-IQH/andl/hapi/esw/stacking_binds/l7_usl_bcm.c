
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcm.c
*
* @purpose    General routine for USL
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

#include <stdio.h>

#include "l7_common.h"
#include "osapi.h"
#include "l7_usl_bcm.h"
#include "l7_usl_bcm_l2.h"
#include "l7_usl_bcm_l3.h"
#include "l7_usl_bcm_ipmcast.h"
#include "l7_usl_api.h"

static L7_BOOL uslIsPresent = L7_FALSE;

/*********************************************************************
* @purpose  Initialize USL
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
L7_RC_t usl_init(void)
{
  L7_RC_t rc = L7_ERROR;

  do
  {
    uslIsPresent = L7_FALSE;

    if (usl_l2_init() != L7_SUCCESS)
    {
      USL_LOG_ERROR("USL: failed to init the L2 DBs\n");
      break;
    }
#ifdef L7_ROUTING_PACKAGE 
    else if (usl_l3_init() != L7_SUCCESS)
    {
      USL_LOG_ERROR("USL: failed to init the L3 DBs\n");
      break;
    }
#endif
#ifdef L7_MCAST_PACKAGE
    else if (usl_ipmc_init() != L7_SUCCESS)
    {
      USL_LOG_ERROR("USL: failed to init the IPMC DBs\n");
      break;
    }
#endif
    rc = L7_SUCCESS;

  } while ( 0 );

  /* if any of the inits failed, all will fail */
  if (rc != L7_SUCCESS) usl_fini();

  return rc;
}

/*********************************************************************
* @purpose  Deallocate all resources of USL
*
* @param    none
*
* @returns  L7_SUCCESS - if all resources were initialized
* @returns  L7_ERROR   - if any resourses were not initialized 
*
* @notes    This should only be called iff the configurator is going to move the 
*           components back to uninitialized (pre-phase 1)
*       
* @end
*********************************************************************/
L7_RC_t usl_fini(void)
{
  L7_RC_t rc = L7_SUCCESS;

  uslIsPresent = L7_FALSE;

  usl_l2_fini();
#ifdef L7_ROUTING_PACKAGE 
  usl_l3_fini();
#endif
#ifdef L7_MCAST_PACKAGE
  usl_ipmc_fini();
#endif

  return rc;
}

/*********************************************************************
* @purpose  Determines whether USL is going to resync or not
*
* @param    none
*
* @returns  L7_TRUE    - USL is responsible for resynching system
* @returns  L7_FALSE   - USL is not responsible for resynching system
*
* @notes    On error, all resources will be released
*       
* @end
*********************************************************************/
L7_BOOL usl_is_present(void)
{
  return uslIsPresent;
}

/*********************************************************************
* @purpose  Set whether USL is going to resync or not
*
* @param    present   {(input)} Set the resync policy, L7_TRUE mean USL resyncs
*
* @end
*********************************************************************/
void usl_is_present_set(L7_BOOL present)
{
  uslIsPresent = present;
}

/*********************************************************************
* @purpose  Responsible for starting a synchronization event if a new card 
*           results in a new Broadcom unit and USL is responsible for syncing units
*
* @param    fp_unit     {(input)}   The FASTPATH unit being inserted
* @param    fp_slot     {(input)}   The FASTPATH slot being inserted
* @param    newBcmUnitInserted     {(output)}  Returns whether this fp_unit/fp_slot
*                                              insert resulted in a new bcm_unit insert.
*
* @returns  L7_SUCCESS or L7_ERROR 
*
* @notes    The caller of this function may be blocked for a long time (seconds)
*
* @notes    The present implementation of the sync is using the BCMX calls and therefore
*           does not require the sync to target on new units.  If in the future only
*           new units are to be targeted, the code must maintain separate lists for the 
*           present and new units.
* 
* @notes    presently this code simply looks at the ports being added and not the 
*           fabric chips being added to determine if a new chip has been inserted 
*           in the system.
*       
* @end
*********************************************************************/
extern L7_RC_t usl_card_insert(L7_uint32 fp_unit,L7_uint32 fp_slot, L7_BOOL *newBcmUnitInserted)
{
  L7_RC_t rc = L7_SUCCESS;

  *newBcmUnitInserted = L7_TRUE;
  return rc;
}

/*********************************************************************
* @purpose  Modify the list of chips that USL knows about
*
* @param    fp_unit     {(input)}   The FASTPATH unit being inserted
* @param    fp_slot     {(input)}   The FASTPATH slot being inserted
*
* @returns  L7_SUCCESS or L7_ERROR 
*
* @notes    The caller of this function may be blocked for a long time (seconds)
*       
* @end
*********************************************************************/
L7_RC_t usl_card_remove(L7_uint32 fp_unit,L7_uint32 fp_slot)
{
  L7_RC_t rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
* @purpose  Invalidate all of the Databases so that USL will start from scratch 
*
* @param    none
*
* @returns  L7_SUCCESS or L7_ERROR 
*
* @notes    The caller of this function may be blocked for a long time (seconds)
*       
* @end
*********************************************************************/
L7_RC_t usl_database_invalidate(void)
{
  L7_RC_t rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
* @purpose  Resume MAC table syncronization.
*
* @param    void 
*
* @returns  void
*
* @notes    Re-register the callbacks to BCM
*
* @end
*********************************************************************/
void usl_mac_table_sync_resume (void)
{
  return;
}

/*********************************************************************
* @purpose  Suspend MAC table syncronization.
*
* @param    void 
*
* @returns  void
*
* @notes    This function should be called before starting bcm_clear().
* @end
*********************************************************************/
void usl_mac_table_sync_suspend (void)
{
  return;
}

/*********************************************************************
* @purpose  Initiate table flush.
*
* @param    void 
*
* @returns  void
*
* @notes    This command should be issued before calling bcm_clear(). 
* @end
*********************************************************************/
void usl_mac_table_all_flush (void)
{
  return;
}

/*********************************************************************
* @purpose  Allow no usl_bcmx_*** calls
*
* @param    void
*
* @returns  void
*
* @notes    This function needs to be called prior to calling usl_card_insert in
*           order to prevent usl_bcmx calls between the synchronization and the 
*           bcmx attach.  This prevents the case where a chip will be synchronized,
*           and potentially miss a later modification to the sync'd item before 
*           broadcom knows about the chip (via the bcmx_device_attach)
*       
* @end
*********************************************************************/
void usl_bcmx_suspend(void)
{
  return;
}

/*********************************************************************
* @purpose  Resume usl_bcmx_*** calls
*
* @param    void
*
* @returns  void
*       
* @notes    
* @end
*********************************************************************/
void usl_bcmx_resume(void)
{
  return;
}

/*********************************************************************
* @purpose  Associate the lport characteristics with the FP unit/slot
*
* @param    fp_unit     {(input)}   The FASTPATH unit being inserted
* @param    fp_slot     {(input)}   The FASTPATH slot being inserted
* @param    lpList      {(input)}   The list of lports
*
* @returns  L7_SUCCESS or L7_ERROR 
*
* @end
*********************************************************************/
L7_RC_t usl_lplist_attach(L7_uint32 fp_unit, L7_uint32 fp_slot, bcmx_lplist_t *lpList)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine whether specified CPU is synced.
*
* @param    fp_unit     {(input)}   The FASTPATH unit
*
* @returns  L7_TRUE or L7_FALSE 
*
* @notes    
*       
* @end
*********************************************************************/
L7_BOOL usl_cpu_sync_check (cpudb_key_t *cpu_key)
{
  return L7_TRUE;
}
