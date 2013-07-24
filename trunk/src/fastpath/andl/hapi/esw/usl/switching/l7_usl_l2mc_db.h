/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename   l7_usl_l2mc_db.h
*
* @purpose    USL L2 Mc DB API implementation 
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

#ifndef L7_USL_L2MC_DB_H
#define L7_USL_L2MC_DB_H

#include "broad_common.h"
#include "l7_usl_bcm_l2.h"

typedef struct 
{
  usl_bcm_mcast_addr_t bcm_data;
  L7_BOOL   isValid;
  void *avl_data; /* needed by the AVL code */
} usl_l2mcast_db_elem_t;

typedef struct
{
  L7_BOOL used;
} usl_l2mc_hw_id_list_t;

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
L7_int32 usl_get_l2mc_db_elem(USL_DB_TYPE_t     dbType, 
                              L7_uint32         flags,
                              uslDbElemInfo_t   searchElem,
                              uslDbElemInfo_t  *elemInfo);

/*********************************************************************
* @purpose  Delete a given element from L2 Mc Db
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
L7_int32 usl_delete_l2mc_db_elem(USL_DB_TYPE_t   dbType, 
                                 uslDbElemInfo_t elemInfo);

/*********************************************************************
* @purpose  Add/Remove a L2 Multicast MAC entry to/from the USL Database
*
* @param    mac          @{(input)} the MAC address
* @param    vid          @{(input)} the VLAN ID the MAC address belongs to
* @param    updateCmd    @{(input)} Add or Remove the address
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l2mc_addr_update(USL_DB_TYPE_t dbType, 
                            usl_bcm_mcast_addr_t *mcastAddr, 
                            USL_CMD_t updateCmd);
 
/*********************************************************************
* @purpose  Add/Remove ports to a L2 Multicast MAC entry to/from the USL Database
*
* @param    mcastAddress @{(input)} Multicast address info
* @param    updateCmd    @{(input)} Add or Remove the ports
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l2mc_update_ports(USL_DB_TYPE_t dbType, usl_bcm_mcast_addr_t *mcastAddr, 
                             USL_CMD_t updateCmd);

/*********************************************************************
* @purpose  Add/Remove port to L2 Multicast MAC entries to/from the USL Database
*
* @param    mcastAddress @{(input)} Multicast address info
* @param    updateCmd    @{(input)} Add or Remove the ports
*
* @returns  BCM error code
*
* @end
*********************************************************************/
int usl_db_l2mc_port_update_groups(USL_DB_TYPE_t dbType, bcm_gport_t gport, L7_uint32 *l2mc_index, L7_uint32 l2mc_index_count, USL_CMD_t updateCmd);

/*********************************************************************
* @purpose  Create the USL L2 MC db
*
* @params   none
*
* @returns  none
*
* @notes    USL L2 MC database is allocated only on Stacking platform.
*
* @end
*********************************************************************/
L7_RC_t usl_l2mc_db_init(void);

/*********************************************************************
* @purpose  De-allocate the USL L2 MC db
*
* @params   none
*
* @returns  none
*
* @end
*********************************************************************/
L7_RC_t usl_l2mc_db_fini(void);

/*********************************************************************
* @purpose  Invalidate the content of the USL L2MC db
*
* @params   flags @{(input)} Type of database to be invalidated
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t usl_l2mc_db_invalidate(USL_DB_TYPE_t flags);

/*********************************************************************
* @purpose  Set the L2Mc Db Handle
*
* @params   dbType @{(input)} Operational or Shadow
*
* @returns  None
*
* @end
*********************************************************************/
void usl_l2mc_current_db_handle_set(USL_DB_TYPE_t dbType);

/*********************************************************************
* @purpose  Initialize L2MC hw id generator
*
*
* @returns  None
*
* @end
*********************************************************************/
L7_RC_t usl_l2mc_hw_id_generator_init(void);

/*********************************************************************
* @purpose  Reset L2Mc hw id generator
*
* @params   none
*
* @returns  None
*
* @end
*********************************************************************/
L7_RC_t usl_l2mc_hw_id_generator_reset();

/*********************************************************************
* @purpose  Synchronize the L2Mc hwId generator with contents of Oper Db
*
* @params   None
*
* @returns  L7_RC_t. Assumes BCMX is suspended.
*
* @end
*********************************************************************/
L7_RC_t usl_l2mc_hw_id_generator_sync();

/*********************************************************************
* @purpose  Allocate a hw l2mc index for a given group
*
* @param    groupInfo @{(input)}  L2MC group
* @param    index     @{(output)} Hw Index
*
* @returns  BCM_E_NONE: HwId Generated successfully
*           BCM_E_FULL: No valid HwId exists
*
* @end
*********************************************************************/
int usl_l2mc_hw_id_allocate(usl_bcm_mcast_addr_t *mcAddr, L7_int32 *index);

/*********************************************************************
* @purpose  Free a given L2 mc index
*
* @param    index   @{(input)} L2 Mc index
*
* @returns  BCM_E_NONE: HwId freed successfully
*           BCM_E_FAIL: HwId could not be freed
*
* @end
*********************************************************************/
int usl_l2mc_hw_id_free(L7_int32 index);

#endif


