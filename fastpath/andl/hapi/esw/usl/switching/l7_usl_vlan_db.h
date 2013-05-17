/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_usl_vlan_db.h
*
* @purpose    USL L2 DB API implementation 
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
#ifndef L7_USL_VLAN_DB_H
#define L7_USL_VLAN_DB_H

#include "l7_common.h"
#include "l7_usl_common.h"
#include "l7_usl_bcm_l2.h"


/* 
 * Type used to store the data for Vlan in USL Db 
 */
typedef struct
{
  bcm_vlan_t             vid;
  L7_BOOL                valid;
  bcm_vlan_mcast_flood_t mcastFloodMode;
  L7_uint32              controlFlags;
  bcm_vlan_forward_t     forwarding_mode; 
} usl_vlan_db_elem_t;

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
L7_int32 usl_get_vlan_db_elem(USL_DB_TYPE_t     dbType, 
                              L7_uint32         flags,
                              uslDbElemInfo_t   searchElem,
                              uslDbElemInfo_t  *elemInfo);

/*********************************************************************
* @purpose  Delete a given element from Vlan Db
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
L7_int32 usl_delete_vlan_db_elem(USL_DB_TYPE_t   dbType, 
                                 uslDbElemInfo_t elemInfo);

/*********************************************************************
* @purpose  Update the vlan database
*
* @param    dbType        @{(input)} Db type to be modified
* @param    vid           @{(input)} the VLAN ID to be created or deleted
* @param    updateCmd     @{(input)} update cmd
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_vlan_update(USL_DB_TYPE_t dbType, bcm_vlan_t vid, USL_CMD_t cmd);

/*********************************************************************
* @purpose  Set the mcast flood mode for a vlan
*
* @param    dbType       @{(input)} Db type to be modified
* @param    vid          @{(input)} 
* @param    floodMode    @{(input)}
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int usl_db_vlan_mcast_flood_set(USL_DB_TYPE_t dbType,
                                bcm_vlan_t vid, 
                                bcm_vlan_mcast_flood_t mcastFloodMode);

/*********************************************************************
* @purpose  Update the control flags for a vlan
*

* @param    dbType       @{(input)} Db type to be modified
* @param    vid          @{(input)} 
* @param    flags        @{(input)} 
* @param    cmd          @{(input)} USL_CMD_ADD: Add the specified flags
*                                                to existing flags
*                                   USL_CMD_REMOVE: Remove the flags
*                                                   from the existing flags
*                                   USL_CMD_SET: Override the existing flags
*                                                with the specified flags.
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int usl_db_vlan_control_flag_update(USL_DB_TYPE_t dbType,
                                    bcm_vlan_t vid, L7_uint32 flags, 
                                    USL_CMD_t  cmd);

/*********************************************************************
* @purpose  Update the forwarding mode for a vlan
*
* @param    dbType                 @{(input)} Db type to be modified
* @param    vid                    @{(input)} 
* @param    forwarding_mode        @{(input)} 
*
* @returns  Defined by the Broadcom driver
*       
* @end
*********************************************************************/
int usl_db_vlan_control_fwd_mode_set(USL_DB_TYPE_t dbType,
                                     bcm_vlan_t vid, bcm_vlan_forward_t forwarding_mode);

/*********************************************************************
* @purpose  Create the USL Vlan db
*
* @params   none
*
* @returns  none
*
* @notes    USL Vlan database is allocated only on Stacking platform.
*
* @end
*********************************************************************/
L7_RC_t usl_vlan_db_init(void);

/*********************************************************************
* @purpose  De-allocate the USL Vlan db
*
* @params   none
*
* @returns  none
*
* @end
*********************************************************************/
L7_RC_t usl_vlan_db_fini(void);

/*********************************************************************
* @purpose  Invalidate the content of the USL Vlan db
*
* @params   flags @{(input)} Type of database to be invalidated
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t usl_vlan_db_invalidate(USL_DB_TYPE_t flags);

/*********************************************************************
* @purpose  Set the Vlan Current Db Handle to Operational or Shadow
*
* @params   dbType @{(input)} Operational or Shadow
*
* @returns  None
*
* @end
*********************************************************************/
void usl_vlan_current_db_handle_set(USL_DB_TYPE_t dbType);

#endif
