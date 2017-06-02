/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_usl_trunk_db.h
*
* @purpose    USL L2 Trunk DB API implementation 
*
* @component  USL
*
* @comments   none
*
* @create     11/21/2008
*
* @author     nshrivastav
* @end
*
**********************************************************************/
#ifndef L7_USL_TRUNK_DB_H
#define L7_USL_TRUNK_DB_H

#include "l7_common.h"
#include "l7_usl_common.h"
#include "l7_usl_bcm_l2.h"

/* 
 * Type used to store the data for Trunk in USL Db 
 */
typedef struct
{
  bcm_trunk_t   tid;
  L7_uint32     appId; /* Application identifier for this trunk */
  L7_BOOL       isValid;
  L7_BOOL       learningLocked;
  bcm_trunk_add_info_t add_info;

} usl_trunk_db_elem_t;

typedef struct
{
  L7_BOOL used;
} usl_trunk_hw_id_list_t;


/*********************************************************************
* @purpose  Get info about an element from the Db
*
* @params   dbType        {(input)}  dbType to be searched
*           flags         {(input)}  EXACT or GET_NEXT
*           searchInfo    {(input)}  Information about element to be searched
*           elemInfo      {(output)} Storage for element & index
*
* @returns  BCM error code
*
* @notes    Caller must allocate memory to copy the vlan db elem in
*           elemInfo.dbElem
*
* @end
*********************************************************************/
L7_int32 usl_get_trunk_db_elem(USL_DB_TYPE_t     dbType, 
                               L7_uint32         flags,
                               uslDbElemInfo_t   searchElem,
                               uslDbElemInfo_t  *elemInfo);

/*********************************************************************
* @purpose  Delete a given element from Trunk Db
*
* @params   dbType   {(input)} 
*           elemInfo {(input)}
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_trunk_db_elem(USL_DB_TYPE_t   dbType, 
                                  uslDbElemInfo_t elemInfo);

/*********************************************************************
* @purpose  Set the laern mode for a trunk
*
* @params   dbType    @{(input)} Db type to modify
* @params   appId     @{(input)} Application-identifier for the trunk
* @params   tid       @{(input)} Hw Trunk identifier
* @params   learnMode @{(input)} Learn mode
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_trunk_learn_mode_set(USL_DB_TYPE_t dbType, L7_uint32 appId,
                                bcm_trunk_t tid, L7_BOOL learningLocked);

/*********************************************************************
* @purpose  Set the hash mode for a trunk
*
* @params   dbType    @{(input)} Db type to modify
* @params   appId     @{(input)} Application-identifier for the trunk
* @params   tid       @{(input)} Hw Trunk identifier
* @params   psc       @{(input)} Trunk hash index
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_trunk_hash_set(USL_DB_TYPE_t dbType, L7_uint32 appId, 
                          bcm_trunk_t tid, int psc);

/*********************************************************************
* @purpose  Create a trunk in the USL db
*
*
* @params   dbType    @{(input)} Db type to modify
* @params   appId     @{(input)} Application-identifier for the trunk
* @params   tid       @{(input)} Hw Trunk identifier
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_create_trunk(USL_DB_TYPE_t dbType, L7_uint32 appId, bcm_trunk_t tid);

/*********************************************************************
* @purpose  Delete a trunk from the USL db
*
* @params   dbType    @{(input)} Db type to modify
* @params   appId     @{(input)} Application-identifier for the trunk
* @params   tid       @{(input)} Hw Trunk identifier
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_delete_trunk(USL_DB_TYPE_t dbType, L7_uint32 appId, 
                        bcm_trunk_t tid);

/*********************************************************************
* @purpose  Update a trunk information in the USL db
*
* @params   dbType    @{(input)} Db type to modify
* @params   appId     @{(input)} Application-identifier for the trunk
* @params   tid       @{(input)} Hw Trunk identifier
* @params   add_info  @{(input)} Trunk information
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_update_trunk_info(USL_DB_TYPE_t dbType, L7_uint32 appId,
                             bcm_trunk_t tid, bcm_trunk_add_info_t * add_info);

/*********************************************************************
* @purpose  Create the USL Trunk db
*
* @params   none
*
* @returns  none
*
* @notes    USL Trunk database is allocated on Stacking Platform
*
* @end
*********************************************************************/
L7_RC_t usl_trunk_db_init(void);

/*********************************************************************
* @purpose  Deallocate the USL Trunk db
*
* @params   none
*
* @returns  none
*
* @notes    USL Trunk database is allocated only on Stacking platform.
*
* @end
*********************************************************************/
L7_RC_t usl_trunk_db_fini(void);

/*********************************************************************
* @purpose  Delete all the trunks from the hardware
*
* @params   
*
* @returns  L7_RC_t
*
* @notes    Works on the content of Operational Db. Database contents
*           are not cleared. Called from hpcHardwareDriverReset as
*           bcm_clear would not cleanup trunk.
*
* @end
*********************************************************************/
void usl_trunk_clear(void);
  
/*********************************************************************
* @purpose  Invalidate the content of the USL trunk db
*
* @params   flags @{(input)} Type of database to be invalidated
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t usl_trunk_db_invalidate(USL_DB_TYPE_t flags);

/*********************************************************************
* @purpose  Set the Trunk Db Handle
*
* @params   dbType @{(input)} Operational or Shadow
*
* @returns  None
*
* @end
*********************************************************************/
void usl_trunk_current_db_handle_set(USL_DB_TYPE_t dbType);

/*********************************************************************
* @purpose  Initialize trunk hw id generator
*
*
* @returns  None
*
* @end
*********************************************************************/
L7_RC_t usl_trunk_hw_id_generator_init(void);

/*********************************************************************
* @purpose  Reset trunk hw id generator
*
* @params   none
*
* @returns  None
*
* @end
*********************************************************************/
L7_RC_t usl_trunk_hw_id_generator_reset();

/*********************************************************************
* @purpose  Synchronize the trunk hwId generator with contents of Oper Db
*
* @params   None
*
* @returns  L7_RC_t. Assumes BCMX is suspended.
*
* @end
*********************************************************************/
L7_RC_t usl_trunk_hw_id_generator_sync();


/*********************************************************************
* @purpose  Allocate a hw trunk-id for given appId
*
* @param    appId      @{(input)}  Application identifier for Trunk
* @param    tid        @{(output)} Hw trunk-id
*
* @returns  BCM_E_NONE: HwId Generated successfully
*           BCM_E_FULL: No valid HwId exists
*           BCM_E_UNAVAIL: Id generation feature is not available
*
* @end
*********************************************************************/
int usl_trunk_hw_id_allocate(L7_uint32 appId, bcm_trunk_t *tid);

/*********************************************************************
* @purpose  Free a given trunkId 
*
* @param    tid   @{(input)} Hw trunk-id
*
* @returns  BCM_E_NONE: HwId freed successfully
*           BCM_E_FAIL: HwId could not be freed
*
* @end
*********************************************************************/
int usl_trunk_hw_id_free(bcm_trunk_t tid);

#ifdef L7_STACKING_PACKAGE
/*********************************************************************
* @purpose  Search trunk db to determine if tgid is valid.
*
* @params   dbType     {(input)} Db type 
*           tgid       {(input)} BCM tgid
*
* @returns  BCM_E_NONE:      tgid valid
*           BCM_E_NOT_FOUND: tgid not valid
*
* @notes    
*
* @end
*********************************************************************/
L7_int32 usl_trunk_db_tgid_validate(USL_DB_TYPE_t dbType, bcm_trunk_t tid);

/*********************************************************************
* @purpose  Used to clean up the HW trunk tables of any references to 
*           modids of the old manager unit. This function is called
*           by each stack unit after a failover occurs.
*
* @param    *missingModIds     @{(input)} array of missing modIds
*            missingModIdCount @{(input)} count of missing modIds
*
* @returns  BCM_E_NONE
*           BCM_E_FAIL
*
* @end
*********************************************************************/
int usl_trunk_db_dataplane_cleanup(L7_int32 *missingModIds, L7_uint32 missingModIdCount);
#endif

#endif
