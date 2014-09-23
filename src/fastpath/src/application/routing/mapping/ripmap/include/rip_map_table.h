/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  rip_map_table.h
*
* @purpose   This file contains prototypes for operations on the route table maintained in
*            mapping layer that contains best routes other than RIP routes.
*
* @component RIP Mapping Layer
*
* @comments  Access to elements in the list is protected by a semaphore.
*            Functions that return values from the list always return a copy
*            of the data in the list.
*
* @create    7/8/2003
*
* @author    skanchi
*
* @end
*
**********************************************************************/

#ifndef _INCLUDE_RIP_MAP_TABLE_H_
#define _INCLUDE_RIP_MAP_TABLE_H_

typedef struct ripMapRouteData_s
{
  L7_IP_ADDR_t    network;
  L7_IP_MASK_t    subnetMask;
  L7_IP_ADDR_t    gateway;
  L7_REDIST_RT_INDICES_t       protocol;
  L7_RTO_PROTOCOL_INDICES_t    protoSpecific;
  L7_BOOL                      routeAdded;
  /* This field is needed by the AVL Library */
  void         *avlNext;
}ripMapRouteData_t;


/* Function Prototypes. */
/*********************************************************************
* @purpose  Initializes the ripMap route table
*
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapRouteTableInit();


/*********************************************************************
* @purpose  Acquires the semaphore of the ripMap route table
*
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapRouteTableResourceAcquire();


/*********************************************************************
* @purpose  Releases the semaphore of the ripMap rouet table
*
* @param
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapRouteTableResourceRelease();


/*********************************************************************
* @purpose  Add a route to the ripMap route table
*
* @param    *routeData         @b{(input)} Route information to be added
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapRouteTableRouteAdd(ripMapRouteData_t *routeData);


/*********************************************************************
* @purpose  Delete a route from the ripMap route table
*
* @param    network         @b{(input)} Newtork ip address of the route
* @param    subnetMask      @b{(input)} Subnet mask of the route
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapRouteTableRouteDelete(L7_IP_ADDR_t network, L7_IP_MASK_t subnetMask);


/*********************************************************************
* @purpose  Modify a route in the ripMap route table
*
* @param    *routeData         @b{(input)} Route information to be modified
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapRouteTableRouteModify(ripMapRouteData_t *routeData);


/*********************************************************************
* @purpose  Check whether the route exists in the ripMap route table
*
* @param    *srcRtData      @b{(input)} Route info to look for
* @param    **routeData     @b{(output)} Pointer to the route info node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapRouteTableRouteGet(ripMapRouteData_t *srcRtData, ripMapRouteData_t **routeData);


/*********************************************************************
* @purpose  Get the next route after the specified route in the ripMap route table
*
* @param    *srcRtData      @b{(input)} Route info to look for
* @param    **routeData     @b{(output)} Pointer to the next route info node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapRouteTableRouteGetNext(ripMapRouteData_t *srcRtData, ripMapRouteData_t **routeData);


/*********************************************************************
* @purpose  Search the ripMap route table
*
* @param    network         @b{(input)} Network ip address of the route
* @param    subnetMask      @b{(input)} Subnet mask of the route
* @param    **routeData     @b{(output)} Pointer to the route info node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapRouteTableRouteSearch(L7_IP_ADDR_t network, L7_IP_MASK_t subnetMask, ripMapRouteData_t **routeData);

/*********************************************************************
*
*  DEBUG Routines
*
*********************************************************************/
L7_RC_t ripMapRouteTableShow();

#endif /* _INCLUDE_RIP_MAP_TABLE_H_ */

