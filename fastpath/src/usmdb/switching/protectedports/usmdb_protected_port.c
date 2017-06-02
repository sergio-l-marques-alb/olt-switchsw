/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  protected_port_api.c
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "l7_common.h"
#include "usmdb_protected_port_api.h"
#include "osapi.h"

#include "protected_port_api.h"


/*
**********************************************************************
*                    API FUNCTIONS  -  AREA CONFIG
**********************************************************************
*/


/*********************************************************************
* @purpose  Add an interface to a Protected port group
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    groupId      @b{(input)}    group identifier
* @param    intIfNum     @b{(input)}    internal interface number
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @end
*********************************************************************/
L7_RC_t usmdbProtectedPortGroupIntfAdd(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 intIfNum)
{
  return protectedPortGroupIntfAdd(groupId, intIfNum);
}

/*********************************************************************
* @purpose  Delete an interface from a Protected Port group
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    groupId      @b{(input)}    group identifier
* @param    intIfNum     @b{(input)}    internal interface number
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @end
*********************************************************************/
L7_RC_t usmdbProtectedPortGroupIntfDelete(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 intIfNum)
{
  return protectedPortGroupIntfDelete(groupId, intIfNum);
}


/*********************************************************************
* @purpose  Given a interface number,gets a groupid,for which an 
*           interface is associated with
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    groupId         @b{(input)}    group identifier
* @param    intIfNum        @b{(input)}    internal interface number
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
* 
*
* @end
*********************************************************************/
L7_RC_t usmdbProtectedPortGroupIntfGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *groupId)
{
  return protectedPortGroupIntfGet(intIfNum, groupId);
}

/*********************************************************************
* @purpose  Given a group and an interface, get the next group and interface
*          
* @param    UnitIndex   L7_uint32 the unit for this operation
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
                                           L7_uint32 * nextIntIfNum)
{
  return protectedPortGroupIntfGetNext(groupId, intIfNum, nextGroupId, nextIntIfNum);
}

/*********************************************************************
* @purpose  Sets a group identifer name
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmdbProtectedPortGroupNameSet(L7_uint32 UnitIndex, L7_uint32 groupId, L7_char8 *name)
{
  return protectedPortGroupNameSet(groupId, name);
}

/*********************************************************************
* @purpose  Given a group ID, gets the name of that particular group
*
* @param    UnitIndex   L7_uint32 the unit for this operation
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
L7_RC_t usmdbProtectedPortGroupNameGet(L7_uint32 UnitIndex, L7_uint32 groupId, L7_char8 *name)
{
  return protectedPortGroupNameGet(groupId, name);
}

/*********************************************************************
* @purpose  Given a group, get all the protected ports which are members of
*           that group  
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    groupId         @b{(input)}    group identifier
* @param    *intIfMask      @b{(output)}   Mask specifying the member ports
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
*
* @end
*********************************************************************/
L7_RC_t usmdbProtectedPortGroupIntfMaskGet(L7_uint32 UnitIndex, L7_uint32 groupId,
                                           L7_INTF_MASK_t *intIfMask)
{
  return protectedPortGroupIntfMaskGet(groupId, intIfMask);
}

/*********************************************************************
* @purpose  Determine if the interface is valid in private edge vlans
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmdbProtectedPortIntfIsValid(L7_uint32 UnitIndex,L7_uint32 intIfNum)
{
   return protectedPortIntfIsValid(intIfNum);
}

