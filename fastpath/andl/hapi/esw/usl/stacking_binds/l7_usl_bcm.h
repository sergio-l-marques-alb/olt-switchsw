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
#ifndef L7_USL_BCM_H
#define L7_USL_BCM_H

#include "l7_common.h"
#include "log.h"
#include "l7_cnfgr_api.h"
#include "l7_usl_common.h"
#include "l7_usl_bcm_debug.h"

typedef void (*syncNotifyCallbackFunc_t) (void *data);
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
L7_RC_t usl_database_invalidate(USL_DB_TYPE_t dbType, L7_BOOL *dbGroup);

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
void usl_hw_id_generator_reset();

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
L7_RC_t usl_init(void);

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
L7_BOOL usl_is_present(void);


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
                                        uslDbSyncFuncs_t *funcList);


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
                                   uslDbSyncFuncs_t *funcList);

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
void usl_db_sync_failure_notify_callback_register(USL_DB_ID_t dbId, syncNotifyCallbackFunc_t notifyCallback);

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
void usl_db_sync_failure_notify_callback(USL_DB_ID_t dbId, void *data);

/*********************************************************************
* @purpose  Allow no usl_bcmx_*** calls
*
* @param    dbGroup {(input)} Db Group to suspend the bcmx
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
void usl_bcmx_suspend(L7_BOOL *dbGroup);

/*********************************************************************
* @purpose  Resume usl_bcmx_*** calls
*
* @param    dbGroup {(input)} Db Group to resume the bcmx
*
* @returns  void
*       
* @notes    
* @end
*********************************************************************/
void usl_bcmx_resume(L7_BOOL *dbGroup);

#endif /* L7_USL_BCM_H */
