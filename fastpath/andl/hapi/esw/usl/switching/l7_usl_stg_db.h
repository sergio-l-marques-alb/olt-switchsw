/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_usl_stg_db.h
*
* @purpose    USL L2 STG DB API implementation 
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
#ifndef L7_USL_STG_DB_H
#define L7_USL_STG_DB_H

#include "l7_common.h"
#include "l7_usl_common.h"
#include "l7_usl_bcm_l2.h"


/* 
 * Type used to store the STG data in USL Db
 */
typedef struct
{
  L7_uint32 stgId;
  L7_uint32 appId; /* Application identifier for this stg */
  L7_VLAN_MASK_t mask;
  L7_BOOL   isValid;
} usl_stg_db_elem_t;

typedef struct
{
  L7_BOOL used;
} usl_stg_hw_id_list_t;

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
* @notes    Caller must allocate memory to copy the stg db elem in
*           elemInfo.dbElem
*
* @end
*********************************************************************/
L7_int32 usl_get_stg_db_elem(USL_DB_TYPE_t     dbType, 
                             L7_uint32         flags,
                             uslDbElemInfo_t   searchElem,
                             uslDbElemInfo_t  *elemInfo);

/*********************************************************************
* @purpose  Delete a given element from Stg Db
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
L7_int32 usl_delete_stg_db_elem(USL_DB_TYPE_t   dbType, 
                                uslDbElemInfo_t elemInfo);

/*********************************************************************
* @purpose  Create a Stg
*
* @params   dbType @{(input)} Type of db to be modified
* @params   appId @{(input)} Application specific identifier for the stg
* @params   stgId @{(input)} Hardware stg-id
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_create_stg(USL_DB_TYPE_t dbType, L7_uint32 appId, bcm_stg_t stg);

/*********************************************************************
* @purpose  Delete a Stg
*
* @params   dbType @{(input)} Type of db to be modified
* @params   appId @{(input)} Application specific identifier for the stg
* @params   stgId  @{(input)} Hardware stg-id
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_destroy_stg(USL_DB_TYPE_t dbType, L7_uint32 appInstId, 
                       bcm_stg_t stg);

/*********************************************************************
* @purpose  Add/Remove a vlan to/from stg
*
* @params   dbType @{(input)} Type of db to be modified
* @params   stg @{(input)} Hardware stg-id
* @params   vid @{(input)} Vlan-id to be added/removed
* @params   cmd @{(input)} Db cmd (Add or Remove)
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_stg_vlan_update(USL_DB_TYPE_t dbType, L7_uint32 appId,
                           bcm_stg_t stg, bcm_vlan_t vid, USL_CMD_t cmd);

/*********************************************************************
* @purpose  Create the USL STG db
*
* @params   none
*
* @returns  none
*
* @notes    USL Stg database is allocated only on Stacking platform.
*
* @end
*********************************************************************/
L7_RC_t usl_stg_db_init(void);

/*********************************************************************
* @purpose  De-allocate the USL STG db
*
* @params   none
*
* @returns  none
*
* @end
*********************************************************************/
L7_RC_t usl_stg_db_fini(void);

/*********************************************************************
* @purpose  Invalidate the content of the USL Stg db
*
* @params   flags @{(input)} Type of database to be invalidated
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t usl_stg_db_invalidate(USL_DB_TYPE_t flags);

/*********************************************************************
* @purpose  Set the Stg Db Handle
*
* @params   dbType @{(input)} Operational or Shadow
*
* @returns  None
*
* @end
*********************************************************************/
void usl_stg_current_db_handle_set(USL_DB_TYPE_t dbType);

/*********************************************************************
* @purpose  Initialize stg hw id generator
*
*
* @returns  None
*
* @end
*********************************************************************/
L7_RC_t usl_stg_hw_id_generator_init(void);

/*********************************************************************
* @purpose  Reset stg hw id generator
*
* @params   None
*
* @returns  None
*
* @end
*********************************************************************/
L7_RC_t usl_stg_hw_id_generator_reset();

/*********************************************************************
* @purpose  Synchronize the Stg hwId generator with contents of Oper Db
*
* @params   None
*
* @returns  L7_RC_t. Assumes BCMX is suspended.
*
* @end
*********************************************************************/
L7_RC_t usl_stg_hw_id_generator_sync();

/*********************************************************************
* @purpose  Allocate a stgId for given appId
*
* @param    appId      @{(input)} Application identifier for STG
* @param    stg        @{(output)} Hw stg-id
*
* @returns  BCM_E_NONE: HwId Generated successfully
*           BCM_E_FULL: No valid HwId exists
*
* @end
*********************************************************************/
int usl_stg_hw_id_allocate(L7_uint32 appId, bcm_stg_t *stg);

/*********************************************************************
* @purpose  Free a given stgId 
*
* @param    stg        @{(input)} Hw stg-id
*
* @returns  BCM_E_NONE: HwId freed successfully
*           BCM_E_FAIL: HwId could not be freed
*
* @end
*********************************************************************/
int usl_stg_hw_id_free(bcm_stg_t stg);

#endif
