
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
#include "broad_common.h"
#include "l7_usl_bcm.h"
#include "l7_usl_api.h"
#include "l7_usl_trace.h"

static L7_BOOL uslIsPresent = L7_FALSE;
L7_BOOL uslDatabaseActive = L7_FALSE;
static L7_BOOL uslInited = L7_FALSE;

/* Externs */
/* Port */
extern L7_RC_t usl_port_db_init();
extern L7_RC_t usl_port_db_fini();
extern L7_RC_t usl_port_db_invalidate(USL_DB_TYPE_t dbType);
extern L7_RC_t usl_port_db_handle_set(USL_DB_TYPE_t dbType);
extern L7_RC_t usl_port_bcmx_init();
extern L7_RC_t usl_port_bcmx_suspend();
extern L7_RC_t usl_port_bcmx_resume();

/* L2 */
extern L7_RC_t usl_l2_db_init();
extern L7_RC_t usl_l2_db_fini();
extern L7_RC_t usl_l2_db_invalidate(USL_DB_TYPE_t dbType);
extern L7_RC_t usl_l2_db_handle_set(USL_DB_TYPE_t dbType);
extern L7_RC_t usl_l2_bcmx_init();
extern L7_RC_t usl_l2_bcmx_suspend();
extern L7_RC_t usl_l2_bcmx_resume();
extern L7_RC_t usl_l2_hw_id_generator_init();
extern L7_RC_t usl_l2_hw_id_generator_reset();

/* L3 */
extern L7_RC_t usl_l3_db_init();
extern L7_RC_t usl_l3_db_fini();
extern L7_RC_t usl_l3_db_invalidate(USL_DB_TYPE_t dbType);
extern L7_RC_t usl_l3_db_handle_set(USL_DB_TYPE_t dbType);
extern L7_RC_t usl_l3_bcmx_init();
extern L7_RC_t usl_l3_bcmx_suspend();
extern L7_RC_t usl_l3_bcmx_resume();
extern L7_RC_t usl_l3_hw_id_generator_init();
extern L7_RC_t usl_l3_hw_id_generator_reset();

/* IPMC */
extern L7_RC_t usl_ipmc_db_init();
extern L7_RC_t usl_ipmc_db_fini();
extern L7_RC_t usl_ipmc_db_invalidate(USL_DB_TYPE_t dbType);
extern L7_RC_t usl_ipmc_db_handle_set(USL_DB_TYPE_t dbType);
extern L7_RC_t usl_ipmc_bcmx_init();
extern L7_RC_t usl_ipmc_bcmx_suspend();
extern L7_RC_t usl_ipmc_bcmx_resume();
extern L7_RC_t usl_ipmc_hw_id_generator_init();
extern L7_RC_t usl_ipmc_hw_id_generator_reset();


/* Policy */
extern L7_RC_t usl_policy_db_init();
extern L7_RC_t usl_policy_db_fini();
extern L7_RC_t usl_policy_db_invalidate(USL_DB_TYPE_t dbType);
extern L7_RC_t usl_policy_db_handle_set(USL_DB_TYPE_t dbType);
extern L7_RC_t usl_policy_bcmx_init();
extern L7_RC_t usl_policy_bcmx_suspend();
extern L7_RC_t usl_policy_bcmx_resume();

/* Metro */
extern L7_RC_t usl_metro_db_init();
extern L7_RC_t usl_metro_db_fini();
extern L7_RC_t usl_metro_db_invalidate(USL_DB_TYPE_t dbType);
extern L7_RC_t usl_metro_current_db_handle_set(USL_DB_TYPE_t dbType);
extern L7_RC_t usl_metro_bcmx_init();
extern L7_RC_t usl_metro_bcmx_suspend();
extern L7_RC_t usl_metro_bcmx_resume();

/* WLAN */
extern L7_RC_t usl_wlan_port_db_init();
extern L7_RC_t usl_wlan_port_db_fini();
extern L7_RC_t usl_wlan_port_db_invalidate(USL_DB_TYPE_t dbType);
extern L7_RC_t usl_wlan_port_db_handle_set(USL_DB_TYPE_t dbType);
extern L7_RC_t usl_wlan_bcmx_init();
extern L7_RC_t usl_wlan_bcmx_suspend();
extern L7_RC_t usl_wlan_bcmx_resume();
extern L7_RC_t usl_wlan_port_hw_id_generator_init();
extern L7_RC_t usl_wlan_vlan_db_init();
extern L7_RC_t usl_wlan_vlan_hw_id_generator_init(void);

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

  return rc;
}


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
  L7_RC_t rc = L7_SUCCESS;

  do
  {
    /* Go ahead and set the USL log to print to the screen */
    usl_print_log(L7_TRUE);

    usl_trace_init(USL_TRACE_ENTRY_MAX, USL_TRACE_ENTRY_SIZE_MAX);

    if (usl_port_db_init() != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      USL_L7_LOG_ERROR("USL: failed to init the Port DBs\n");
      break;
    }

    if (usl_port_bcmx_init() != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      USL_L7_LOG_ERROR("USL: failed to init the Port Bcmx\n");
      break;
    }

    
    if (usl_l2_db_init() != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      USL_L7_LOG_ERROR("USL: failed to init the L2 DBs\n");
      break;
    }

    if (usl_l2_bcmx_init() != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      USL_L7_LOG_ERROR("USL: failed to init the L2 Bcmx\n");
      break;
    }
    else if (usl_l2_hw_id_generator_init() != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      USL_L7_LOG_ERROR("USL: failed to init the L2 Hw Index Generation module\n");
      break;
    }
#ifdef L7_ROUTING_PACKAGE
    else if (usl_l3_db_init() != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      USL_L7_LOG_ERROR("USL: failed to init the L3 DBs\n");
      break;
    }
    else if (usl_l3_hw_id_generator_init() != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      USL_L7_LOG_ERROR("USL: failed to init the L3 Hw Id generation\n");
      break;
    }
    else if (usl_l3_bcmx_init() != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      USL_L7_LOG_ERROR("USL: failed to init the L3 Bcmx\n");
      break;
    }
#ifdef L7_MCAST_PACKAGE
    else if (usl_ipmc_db_init() != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      USL_L7_LOG_ERROR("USL: failed to init the IPMC DBs\n");
      break;
    }
    else if (usl_ipmc_bcmx_init() != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      USL_L7_LOG_ERROR("USL: failed to init the IPMC Bcmx\n");
      break;
    }
    else if (usl_ipmc_hw_id_generator_init() != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      USL_L7_LOG_ERROR("USL: failed to init the IPMC Hw Id Generation\n");
      break;
    }

#endif /* ends L7_MCAST_PACKAGE   */
#endif /* ends L7_ROUTING_PACKAGE */
    else if (usl_policy_db_init() != L7_SUCCESS)
    {
        rc = L7_FAILURE;
        USL_L7_LOG_ERROR("USL: failed to init the Policy DB\n");
        break;
    }
    else if (usl_policy_bcmx_init() != L7_SUCCESS)
    {
        rc = L7_FAILURE;
        USL_L7_LOG_ERROR("USL: failed to init Policy Bcmx\n");
        break;
    }
#ifdef L7_METRO_FLEX_PACKAGE
    else if (usl_metro_bcmx_init() != L7_SUCCESS)
    {
        rc = L7_FAILURE;
        USL_L7_LOG_ERROR("USL: failed to init Metro Bcmx\n");
        break;
    }
#endif

#ifdef L7_WIRELESS_PACKAGE
    else if (usl_wlan_port_db_init() != L7_SUCCESS)
    {
        rc = L7_FAILURE;
        USL_L7_LOG_ERROR("USL: failed to init the WLAN Port DB\n");
        break;
    }
    else if (usl_wlan_vlan_db_init() != L7_SUCCESS)
    {
        rc = L7_FAILURE;
        USL_L7_LOG_ERROR("USL: failed to init the WLAN VLAN DB\n");
        break;
    }
    else if (usl_wlan_bcmx_init() != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      USL_L7_LOG_ERROR("USL: failed to init the WLAN Bcmx\n");
      break;
    }
    else if (usl_wlan_port_hw_id_generator_init() != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      USL_L7_LOG_ERROR("USL: failed to init the WLAN port Hw Id Generation\n");
      break;
    }
    else if (usl_wlan_vlan_hw_id_generator_init() != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      USL_L7_LOG_ERROR("USL: failed to init the WLAN vlan Hw Id Generation\n");
      break;
    }
#endif
    rc = L7_SUCCESS;

  } while ( 0 );

  /* if any of the inits failed, all will fail */
  if (rc != L7_SUCCESS)
    usl_fini();
  else
  {
    uslInited = L7_TRUE;
    uslDatabaseActive = L7_TRUE;
    uslIsPresent = L7_TRUE;
  }

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
* @param    type {(input)} Bitmap of DbType to invalidate (Operational or Shadow)
* @param    dbGroupList {(input)} List of Db Groups to be invalidated
*
* @returns  L7_SUCCESS or L7_ERROR 
*
* @notes    The caller of this function may be blocked for a long time (seconds)
*       
* @end
*********************************************************************/
L7_RC_t usl_database_invalidate(USL_DB_TYPE_t dbType, L7_BOOL *dbGroup)
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
                                 bcmx_lplist_t *lpList)
{
  return L7_SUCCESS;
}

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
L7_BOOL usl_cpu_key_present (cpudb_key_t *cpu_key)
{
  return L7_TRUE;
}

L7_BOOL usl_unit_sync_check (L7_uint32 fp_unit)
{
  return L7_TRUE;
}

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
                 L7_uint32 lastMgrUnitId, L7_enetMacAddr_t lastMgrKey)
{
  return L7_SUCCESS;
}


L7_RC_t usl_hw_apply_notify(L7_CNFGR_HW_APPLY_t hwApplyPhase)
{
  return L7_SUCCESS;
}

L7_BOOL usl_state_get(void)
{
  return USL_OPERATIONAL_STATE;
}


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
void usl_sm_reset(void)
{
  return;
}

/*********************************************************************
*
* @purpose Reset USL Hw Id Generator
*
* @params  none
*
* @returns none
*
* @notes   Called on driver cold start
* @end
*
*********************************************************************/
void usl_hw_id_generator_reset()
{
  return;
}


/*********************************************************************
* @purpose  Check whether the driver is in steady-state
*
* @param    none
*
* @returns  L7_TRUE: Driver is not in steady state
* @returns  L7_FALSE: Driver is in steady state
*
* @notes    Should be called on mgmt unit only
*       
* @end
*********************************************************************/
L7_BOOL hpcDriverInRestart(void)
{
  return L7_FALSE;
}


/*********************************************************************
* @purpose  Register sync routines for a given Db
*
* @param    dbId     {(input)}  Db Id to register
* @param    funcList {(input)}  List of db functions
*
* @returns  L7_RC_t
*
* @notes    The registered routines are used by generic sync routines
*
*
* @end
*********************************************************************/
L7_RC_t usl_db_sync_func_table_register(USL_DB_ID_t dbId, 
                                        uslDbSyncFuncs_t *funcList)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the sync function table for a Db
*
* @param    dbId     {(input)}  Db Id to get
* @param    funcList {(input)}  List of db functions
*
* @returns  L7_RC_t
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usl_db_sync_func_table_get(USL_DB_ID_t dbId, 
                                   uslDbSyncFuncs_t *funcList)
{
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Registers a callback function to be called on the mgmt
*           unit for an item that cannot be synced/reconciled.
*
* @param    dbId             {(input)}  Db Id 
* @param    notifyCallback   {(input)}  notifyCallback
*
* @returns  
*
* @notes    
*
* @end
*********************************************************************/
void usl_db_sync_failure_notify_callback_register(USL_DB_ID_t dbId, syncNotifyCallbackFunc_t notifyCallback)
{
  return;
}

/*********************************************************************
* @purpose  On the mgmt unit, calls back to the registered function
*           indicating a USL sync failure.
*
* @param    dbId     {(input)}  Db Id 
* @param    data     {(input)}  data
*
* @returns  
*
* @notes    
*
* @end
*********************************************************************/
void usl_db_sync_failure_notify_callback(USL_DB_ID_t dbId, void *data)
{
  return;
}


void usl_bcmx_suspend(L7_BOOL *dbGroup)
{
  return;
}

void usl_bcmx_resume(L7_BOOL *dbGroup)
{
  return;
}


