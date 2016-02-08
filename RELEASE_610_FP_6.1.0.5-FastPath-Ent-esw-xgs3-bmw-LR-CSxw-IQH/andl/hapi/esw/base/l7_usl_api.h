/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_api.h
*
* @purpose    Generic Unit Synchronization API
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

#ifndef L7_USL_API_H
#define L7_USL_API_H

#include "l7_common.h"
#include "bcmx/lplist.h"
#include "appl/cpudb/cpudb.h"

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
extern L7_RC_t usl_init(void);

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
extern L7_RC_t usl_fini(void);

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
extern L7_BOOL usl_is_present(void);

/*********************************************************************
* @purpose  Set whether USL is going to resync or not
*
* @param    present   {(input)} Set the resync policy, L7_TRUE mean USL resyncs
*
* @end
*********************************************************************/
extern void usl_is_present_set(L7_BOOL present);


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
extern L7_RC_t usl_database_invalidate(void);

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
extern void usl_mac_table_sync_resume (void);

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
extern void usl_mac_table_sync_suspend (void);

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
extern void usl_mac_table_all_flush (void);

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
extern L7_RC_t usl_card_insert(L7_uint32 fp_unit,L7_uint32 fp_slot, L7_BOOL *newBcmUnitInserted);

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
extern L7_RC_t usl_lplist_attach(L7_uint32 fp_unit, L7_uint32 fp_slot, bcmx_lplist_t *lpList);

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
L7_BOOL usl_cpu_sync_check (cpudb_key_t *cpu_key);

/*********************************************************************
* @purpose  Determine whether specified Fastpath unit is synced.
*
* @param    fp_unit     {(input)}   The FASTPATH unit
*
* @returns  L7_TRUE or L7_FALSE 
*
* @notes    
*       
* @end
*********************************************************************/
L7_BOOL usl_unit_sync_check (L7_uint32 fp_unit);


/*********************************************************************
* @purpose  Determine whether specified Fastpath unit is being 
*           synchronized.
*
* @param    fp_unit     {(input)}   The FASTPATH unit
*
* @returns  L7_TRUE or L7_FALSE 
*
* @notes    
*       
* @end
*********************************************************************/
L7_BOOL usl_unit_sync_in_progress_check (L7_uint32 fp_unit);

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
extern L7_RC_t usl_card_remove(L7_uint32 fp_unit,L7_uint32 fp_slot);

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
extern void usl_bcmx_suspend(void);

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
extern void usl_bcmx_resume(void);


#endif
