/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dvmrp_igmp_group_table.h
*
* @purpose   This file contains prototypes for operations on the Group table 
*
* @component DVMRP Group TABLE API's (AVL Trees)
*
* @comments  
*            Functions that return values from the list always return a copy
*            of the data in the list.
*
* @create    30/05/2006
*
* @author    shashidhar Dasoju
*
* @end
*
**********************************************************************/

#ifndef _INCLUDE_DVMRP_GROUP_TABLE_H_
#define _INCLUDE_DVMRP_GROUP_TABLE_H_
#include "dvmrp_common.h"


#define DVMRP_MAX_LOCAL_MEMB_ENTRIES (L7_MGMD_GROUPS_MAX_ENTRIES + 1)

/* Function Prototypes. */
/*********************************************************************
* @purpose  Initializes the igmp group table
*
* @param    dvmrpcb   -  @b{(input)}  Pointer to the DVMRP control block.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   None
*       
* @end
*********************************************************************/
L7_RC_t dvmrpGroupTableInit(dvmrp_t *dvmrpcb);


/*********************************************************************
* @purpose  De-Initializes the Group table
*
* @param    dvmrpcb   -  @b{(input)}  Pointer to the DVMRP control block.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  None
*       
* @end
*********************************************************************/
L7_RC_t dvmrpGroupTableDestroy(dvmrp_t *dvmrpcb);

/*********************************************************************
* @purpose  Add a Group to the Group table
*
* @param    dvmrpcb   -  @b{(input)}  Pointer to the DVMRP control block.
* @param    cacheData -   @b{(input)} Pointer to the Route information to be added
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  None
*
* @end
*********************************************************************/
L7_RC_t dvmrpGroupTableGroupAdd(dvmrp_t *dvmrpcb,dvmrp_Group_entry_t *cacheData);

/*********************************************************************
* @purpose  Delete a Group from the Group table
*
* @param    dvmrpcb   -  @b{(input)}  Pointer to the DVMRP control block.
* @param    pSource    -  @b{(input)} Pointer to the Source of the multicast data
* @param    pGroup     -  @b{(input)} Pointer to the Multicast group address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes None
*
* @end
*********************************************************************/
L7_RC_t dvmrpGroupTableGroupDelete(dvmrp_t *dvmrpcb,L7_inet_addr_t *pSource, 
                                   L7_inet_addr_t *pGroup);

/*********************************************************************
* @purpose  Check whether the cache exists in the Group table
*
* 
* @param    dvmrpcb       -  @b{(input)}  Pointer to the DVMRP control block.
* @param    srcRtData     -    @b{(input)} Pointer to the   Route info to look for
* @param    routeData     -     @b{(output)} Pointer to the route info node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  None
*
* @end
*********************************************************************/
L7_RC_t dvmrpGroupTableGroupGet(dvmrp_t *dvmrpcb,dvmrp_Group_entry_t *srcRtData,
                                dvmrp_Group_entry_t **cacheData);

/*********************************************************************
* @purpose  Get the next cache after the specified cache in the cache table
*
* 
* @param    dvmrpcb       -  @b{(input)}  Pointer to the DVMRP control block.
* @param    srcRtData    -     @b{(input)} Pointer to the Cache info to look for
* @param    cacheData  -     @b{(output)} Pointer to the next Cache info node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  None
*
* @end
*********************************************************************/
L7_RC_t dvmrpGroupTableGroupNextGet(dvmrp_t *dvmrpcb,dvmrp_Group_entry_t *srcRtData,
                                    dvmrp_Group_entry_t **cacheData);


/*********************************************************************
* @purpose  Search the Group table
*
* 
* @param    dvmrpcb   -  @b{(input)}  Pointer to the DVMRP control block.
* @param    pTmpNode - @b{(input)}  Pointer to the group entry node which just holds the key values       
* @param    cacheData  -     @b{(output)} Pointer to the route info node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  None
*
* @end
*********************************************************************/
L7_RC_t dvmrpGroupTableGroupSearch(dvmrp_t *dvmrpcb,dvmrp_Group_entry_t *pTmpNode,
                                   dvmrp_Group_entry_t **cacheData);

/*********************************************************************
 * @purpose  Updates the Group Membership Table
 *
 * @param    dvmrpcb   -  @b{(input)}  Pointer to the DVMRP control block. 
 * @param    grp_info   -  @b{(input)}   Pointer to the group information.
 * @param    event      -  @b{(input)}   Event.
 *
 * @returns  None
 *
 * @notes None
 *
 * @end
 *********************************************************************/
void dvmrpGroupSourcesUpdate(dvmrp_t *dvmrpcb,mgmdMrpEventInfo_t *grp_info, L7_uint32 event);

#endif /* _INCLUDE_DVMRP_GROUP_TABLE_H_ */

