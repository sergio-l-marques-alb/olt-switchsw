/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename  usmdb_protected_port.h
*
* @purpose   Protected Port USMDB APIs
*
* @component Protected Port 
*
* @comments
*
* @create   06/06/2005 
*
* @author    ryadagiri
* @end
*
**********************************************************************/

#ifndef USMDB_PROTECTED_PORT_API_H
#define USMDB_PROTECTED_PORT_API_H

#include "l7_common.h"
#include "comm_mask.h"

/* Begin Function Prototypes */

/*********************************************************************
* @purpose  Adds an interface to a protected port group
*
* @param    unitIndex    @b{(input)}    unit Index
* @param    groupId      @b{(input)}    group identifier
* @param    intIfNum     @b{(input)}    internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t usmdbProtectedPortGroupIntfAdd(L7_uint32 UnitIndex, L7_uint32 groupId, 
                                              L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Deletes an interface to a protected port group
*
* @param    unitIndex    @b{(input)}    unit Index
* @param    groupId      @b{(input)}    group identifier
* @param    intIfNum     @b{(input)}    internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t usmdbProtectedPortGroupIntfDelete(L7_uint32 UnitIndex, L7_uint32 groupId, 
                                                 L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Given a group and an interface, get the next group and interface
*
* @param    unitIndex       @b{(input)}    unit Index 
* @param    groupId         @b{(output)}    group identifier
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
* 
*
* @end
*********************************************************************/
L7_RC_t usmdbProtectedPortGroupIntfGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
                                              L7_uint32 *groupId);

/*********************************************************************
* @purpose  Given a group and an interface, get the next group and interface
*
* @param    unitIndex       @b{(input)}    unit Index
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
L7_RC_t usmdbProtectedPortGroupIntfGetNext(L7_uint32 UnitIndex, L7_uint32 groupId, 
                                                  L7_uint32 intIfNum, L7_uint32 * nextGroupId, 
                                                  L7_uint32 * nextIntIfNum);
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
L7_RC_t usmdbProtectedPortGroupNameSet(L7_uint32 UnitIndex, L7_uint32 groupId, 
                                              L7_char8 *name);
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
L7_RC_t usmdbProtectedPortGroupNameGet(L7_uint32 UnitIndex, L7_uint32 groupId, 
                                              L7_char8 *name);

/*********************************************************************
*  @purpose Given a group, get the interface mask list with information
*           of the ports that are part of that group and hence protected
*
* @param    unitIndex          @b{(input)}  unit Index
* @param    groupId            @b{(input)}  group identifier
* @param    *intIfMask         @b{(output)} Internal mask for protected ports
*
*
* @returns  L7_SUCCESS,    
* @returns  L7_FAILURE 
*
*
@end*********************************************************************/
L7_RC_t usmdbProtectedPortGroupIntfMaskGet(L7_uint32 UnitIndex, L7_uint32 groupId,
                                           L7_INTF_MASK_t *intIfMask);

/*********************************************************************
* @purpose  Determine if the interface is valid in private edge vlans
*
* @param    unitIndex          @b{(input)}  unit Index
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmdbProtectedPortIntfIsValid(L7_uint32 UnitIndex,L7_uint32 intIfNum);

/* End Function Prototypes */
#endif
