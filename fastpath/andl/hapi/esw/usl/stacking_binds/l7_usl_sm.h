/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_bcm.h
*
* @purpose    Private datastructures and prototypes for USL
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
#ifndef L7_USL_SM_H
#define L7_USL_SM_H

#include "l7_common.h"
#include "log.h"
#include "l7_usl_common.h"
#include "l7_cnfgr_api.h"
#include "appl/cpudb/cpudb.h"


/*********************************************************************
* @purpose  Reset all the USL State-machine variables
*
* @param    none
*
* @returns  void
*       
* @notes    Resets all the control tables except the current & prev state.    
*
* @end
*********************************************************************/
void usl_sm_reset(void);

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
L7_RC_t usl_db_bcm_unit_populate(L7_uint32 fp_unit, L7_uint32 fp_slot, 
                                 bcmx_lplist_t *lpList);

/*********************************************************************
* @purpose  Determine whether specified CPU is present in USL Db
*
* @param    fp_unit     {(input)}   The FASTPATH unit
*
* @returns  L7_TRUE or L7_FALSE 
*
* @notes    
*       
* @end
*********************************************************************/
unsigned int usl_cpu_key_present (cpudb_key_t *cpu_key);


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
* @purpose  Return the current USL state
*
* @params   none
*
* @returns  
*       

* @end
*********************************************************************/
USL_STATE_t usl_state_get();

/*********************************************************************
* @purpose  Start USL Layer
*
* @param    startupReason        {(input)} Reason code for starting driver
*           mgrFailover         {(input)} Indicates that the unit is
*                                        becoming mgr after a failover
*          lastMgrUnitId       {(input)} Unit number of the last mgr
*          lastMgrKey          {(input)} Key of the last mgr

*
* @returns  L7_SUCCESS or L7_FAILURE
*       
* @notes    Will be called when a unit becomes the Manager of the stack.
*           Performs either cold or warm start. If warm start could not
*           be initiated then cold start is performed.
*
* @end
*********************************************************************/
L7_RC_t uslStart(L7_LAST_STARTUP_REASON_t startupReason, L7_BOOL mgrFailover,
                 L7_uint32 lastMgrUnitId, L7_enetMacAddr_t lastMgrKey);

/*********************************************************************
* @purpose  Notify the driver that hw updates for a phase is complete
*           
*
* @param    hwApplyPhase {(input)} Completed phase
*
* @returns  L7_SUCCESS or L7_FAILURE
*       
* @notes    The message received from configurator is enqued for the USL
*           task to avoid blocking the configurator thread too long.
* 
* @end
*********************************************************************/
L7_RC_t usl_hw_apply_notify(L7_CNFGR_HW_APPLY_t hwApplyPhase);

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
extern L7_RC_t usl_card_insert(L7_uint32 fp_unit,L7_uint32 fp_slot, 
                               L7_BOOL *newBcmUnitInserted);

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


#define USL_BCMX_CONFIGURE_HW(dbId) (L7_TRUE)
#define USL_BCM_CONFIGURE_HW(dbId) (L7_TRUE)
#define USL_BCM_CONFIGURE_DB(dbId) (L7_FALSE)

#endif /* L7_USL_BCM_H */
