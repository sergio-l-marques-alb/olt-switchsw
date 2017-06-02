/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  protected_port_api.h
*
* @purpose   Protected Port API file
*
* @component protectedPort
*
* @comments
*
* @create    6/6/2005
*
* @author    ryadagiri
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_PROTECTED_PORT_API_H
#define INCLUDE_PROTECTED_PORT_API_H

/* Begin Function Prototypes */
/*********************************************************************
* @purpose  Add an interface to a private edge VLAN group
*
* @param    groupId      @b{(input)}    group identifier
* @param    intIfNum     @b{(input)}    internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t protectedPortGroupIntfAdd(L7_uint32 groupId, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Delete an interface from a private edge VLAN group
*
* @param    groupId      @b{(input)}    group identifier
* @param    intIfNum     @b{(input)}    internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t protectedPortGroupIntfDelete(L7_uint32 groupId, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Given a interface number,gets a groupid,for which an
*           interface is associated with
*
* @param    intIfNum        @b{(input)}    internal interface number
* @param    groupId         @b{(output)}    group identifier
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @end
*********************************************************************/
L7_RC_t protectedPortGroupIntfGet(L7_uint32 intIfNum, L7_uint32 *groupId);

/*********************************************************************
* @purpose  Given a group and an interface, get the next group and interface
*
* @param    groupId         @b{(input)}    group identifier
* @param    intIfNum        @b{(input)}    internal interface number
* @param    *nextGroupId    @b{(output)}   pointer to next group identifier
* @param    *nextIntIfNum   @b{(output)}   pointer to next internal interface number
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @end
*********************************************************************/
L7_RC_t protectedPortGroupIntfGetNext(L7_uint32 groupId, L7_uint32 intIfNum,
                                      L7_uint32 *nextGroupId, L7_uint32 *nextIntIfNum);

/*********************************************************************
* @purpose  Sets a group identifer name
*
* @param    unitIndex       @b{(input)}    unit Index
* @param    groupId         @b{(input)}    group identifier
* @param    buf             @b{(input)}    name of the group
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @end
*********************************************************************/
L7_RC_t protectedPortGroupNameSet(L7_uint32 groupId, L7_char8 *name);

/*********************************************************************
* @purpose  Gets a group identifer name
*
* @param    unitIndex       @b{(input)}    unit Index
* @param    groupId         @b{(input)}    group identifier
* @param    buf             @b{(output)}    name of the group
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @end
*********************************************************************/
L7_RC_t protectedPortGroupNameGet(L7_uint32 groupId, L7_char8 *name);

/*********************************************************************
* @purpose  Given a group, get all the protected ports which are members of
*           that group
*
* @param    groupId         @b{(input)}    group identifier
* @param    *intIfMask      @b{(output)}   Mask specifying the member ports
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @end
*********************************************************************/
extern L7_RC_t protectedPortGroupIntfMaskGet(L7_uint32 groupId, 
                                             L7_INTF_MASK_t *intIfMask);

/*********************************************************************
* @purpose  Determine if the interface is valid in private edge vlans
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
extern L7_RC_t protectedPortIntfIsValid(L7_uint32 intIfNum);

/* End Function Prototypes */

#endif /* PROTECTED_PORT_API_H */

