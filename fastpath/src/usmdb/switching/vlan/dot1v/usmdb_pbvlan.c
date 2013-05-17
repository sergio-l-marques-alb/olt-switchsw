/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\usmdb\usmdb_pbvlan.c
*
* @purpose Provide interface to pbVlan APIs for unitmgr components
*
* @component unitmgr
*
* @comments none
*
* @create 1/28/2002
*
* @author rjindal
*
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#include <string.h>
#include "l7_common.h"
#include "usmdb_pbvlan_api.h"

#include "usmdb_util_api.h"
#include "pbvlan_api.h"
#include "defaultconfig.h"

/*********************************************************************
*
* @purpose  To create a new group.
*
* @param    L7_uint32  UnitIndex  @b((input)) unit for this operation
* @param    L7_uint32  groupId    @b((input)) groupId of the group
*
* @returns  L7_SUCCESS, if the group id is assigned
* @returns  L7_FAILURE, if unable to assign
* @returns  L7_ERROR, if the group id already exists
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanGroupCreate(L7_uint32 UnitIndex, L7_uint32 groupId)
{
  return pbVlanGroupCreate(groupId);
}

/*********************************************************************
*
* @purpose  To create a new group by name.
*
* @param    L7_uint32  UnitIndex  @b((input)) unit for this operation
* @param    L7_char8  *Name  name of the group
*
* @returns  L7_SUCCESS, if the group name is added
* @returns  L7_FAILURE, if the group name is not added
* @returns  L7_ALREADY_CONFIGURED, if the group name already exists
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanGroupNameCreate(L7_uint32 UnitIndex, L7_uchar8 *Name)
{
  return pbVlanGroupNameCreate(Name);
}


/*********************************************************************
*
* @purpose  To delete a group.
*
* @param    L7_uint32  UnitIndex  @b((input)) unit for this operation
* @param    L7_uint32  groupId    @b((input)) id of an existing group 
*
* @returns  L7_SUCCESS, if group id deleted
* @returns  L7_FAILURE, if unable to delete
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanGroupDelete(L7_uint32 UnitIndex, L7_uint32 groupId)
{
  return pbVlanGroupDelete(groupId);
}


/*********************************************************************
*
* @purpose  To add a protocol to a group.
*
* @param    L7_uint32  UnitIndex  @b((input)) unit for this operation
* @param    L7_uint32  groupId    @b((input)) id of an existing group
* @param    L7_uint32  prtl       @b((input)) protocol to be added
*
* @returns  L7_SUCCESS, if protocol is added to group id
* @returns  L7_FAILURE, if unable to add
* @returns  L7_ERROR, if this protocol already exists
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanGroupProtocolAdd(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 protocol, L7_uint32 ptype)
{
  L7_RC_t rc;

  rc = pbVlanGroupProtocolAdd(groupId, protocol, ptype);

  return rc;
}


/*********************************************************************
*
* @purpose  To delete a protocol from a group.
*
* @param    L7_uint32  UnitIndex  @b((input)) unit for this operation
* @param    L7_uint32  groupId    @b((input)) id of an existing group
* @param    L7_uint32  prtl       @b((input)) protocol to be deleted
*
* @returns  L7_SUCCESS, if protocol is deleted from the group id
* @returns  L7_FAILURE, if group id or port is invalid
* @returns  L7_ERROR, if the group does not have this protocol or
*                     the existing one does not match 'prtl'
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanGroupProtocolDelete(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 prtl)
{
  L7_RC_t rc;

  rc = pbVlanGroupProtocolDelete(groupId, prtl);

  return rc;
}


/*********************************************************************
*
* @purpose  To add a vlan id to a group.
*
* @param    L7_uint32  UnitIndex  @b((input)) unit for this operation
* @param    L7_uint32  groupId    @b((input)) id of an existing group
* @param    L7_uint32  vid        @b((input)) vlan id to be added
*
* @returns  L7_SUCCESS, if vlan id is added to the group
* @returns  L7_FAILURE, if invalid group id or vlan id
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanGroupVlanIDAdd(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 vid)
{
  return pbVlanGroupVlanIDAdd(groupId, vid);
}


/*********************************************************************
*
* @purpose  To delete the vlan id from a group.
*
* @param    L7_uint32  UnitIndex  @b((input)) unit for this operation
* @param    L7_uint32  groupId    @b((input)) id of an existing group
* @param    L7_uint32  vid        @b((input)) vlan id to be deleted
*
* @returns  L7_SUCCESS, if vlan id is deleted from the group
* @returns  L7_FAILURE, if invalid group id or vlan id
* @returns  L7_ERROR, if the group does not have this vlan id or
*                     the existing one does not match 'vid'
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanGroupVlanIDDelete(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 vid)
{
  return pbVlanGroupVlanIDDelete(groupId, vid);
}


/*********************************************************************
*
* @purpose  To add a port to a group.
*
* @param    L7_uint32  UnitIndex  @b((input)) unit for this operation
* @param    L7_uint32  groupId    @b((input)) id of an existing group
* @param    L7_uint32  intIfNum   @b((input)) internal intf number
*
* @returns  L7_SUCCESS, if port is added to the group
* @returns  L7_FAILURE, if invalid groupId or port
* @returns  L7_ERROR, if the specified port is already in the list
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanGroupPortAdd(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 intIfNum)
{
  return pbVlanGroupPortAdd(groupId, intIfNum);
}


/*********************************************************************
*
* @purpose  To delete a port from a group.
*
* @param    L7_uint32  UnitIndex  @b((input)) unit for this operation
* @param    L7_uint32  groupId    @b((input)) id of an existing group
* @param    L7_uint32  intIfNum   @b((input)) internal intf number
*
* @returns  L7_SUCCESS, if port is deleted from the group
* @returns  L7_FAILURE, if unable to delete, invalid groupId, or port
* @returns  L7_ERROR, if the specified port is not in use
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanGroupPortDelete(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 intIfNum)
{
  return pbVlanGroupPortDelete(groupId, intIfNum);
}


/*********************************************************************
*
* @purpose  To get the id of the first created group.
*
* @param    L7_uint32  UnitIndex  @b((input)) unit for this operation
* @param    L7_uint32  *next      @b((input)) next created group's id
*
* @returns  L7_SUCCESS, if the next created group is found
* @returns  L7_FAILURE, if invalid groupid or next group is not found
*
* @comments The 'groupId' is zero (0) when trying to find the very
*           first created group.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanGroupGetFirst(L7_uint32 UnitIndex, L7_uint32 *next)
{
  if (pbVlanGroupGetNext(L7_NULL, next) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Given a group id, get the id of the next created group.
*
* @param    L7_uint32  UnitIndex  @b((input)) unit for this operation
* @param    L7_uint32  groupId    @b((input)) group id, start
* @param    L7_uint32  *next      @b((input)) group id, next
*
* @returns  L7_SUCCESS, if the next created group is found
* @returns  L7_FAILURE, if invalid groupid or next group is not found
*
* @comments The id of the next created group will be returned.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanGroupGetNext(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 *next)
{
  if (pbVlanGroupGetNext(groupId, next) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To check if the specified group exists.
*
* @param    L7_uint32  UnitIndex  @b((input)) unit for this operation
* @param    L7_uint32  groupId    @b((input)) group id
*
* @returns  L7_SUCCESS, if the specified group exists
* @returns  L7_FAILURE, if invalid groupid or
*                       specified group does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanGroupGetExact(L7_uint32 UnitIndex, L7_uint32 groupId)
{
  return pbVlanGroupGetExact(groupId);
}

/*********************************************************************
*
* @purpose  To set a group name.
*
* @param    L7_uint32 groupId ID of the group
* @param    L7_char8  *Name  name of the group
*
* @returns  L7_SUCCESS, if the group name is added
* @returns  L7_FAILURE, if the group name adding added
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanGroupNameSet(L7_uint32 UnitIndex, L7_uint32 groupId, L7_char8 *Name)
{
  if (strlen(Name) > L7_PBVLAN_MAX_GROUP_NAME)
  {
    return L7_FAILURE;
  }

  return pbVlanGroupNameSet(groupId,Name);
}

/*********************************************************************
*
* @purpose  To get the name of the group identified by groupId.
*
* @param    L7_uint32  UnitIndex  @b((input)) unit for this operation
* @param    L7_uint32  groupId    @b((input)) group id
* @param    L7_char8   *buf       @b{(Output)) buffer to be filled in
*                                 with the name of this group
*
* @returns  L7_SUCCESS, if the specified group exists
* @returns  L7_FAILURE, if invalid groupid or group does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanGroupNameGet(L7_uint32 UnitIndex, L7_uint32 groupId, L7_char8 *buf)
{
  return pbVlanGroupNameGet(groupId, buf);
}

/*********************************************************************
*
* @purpose  To get the groupID by giving group name already exists.
*
* @param    L7_char8  *Name  name of the group
* @param    L7_uint32 *groupId  @b((output)) group id
*
* @returns  L7_SUCCESS, if the specified group exists
* @returns  L7_FAILURE, if invalid groupid or group does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanGroupIDGet(L7_uint32 UnitIndex, L7_char8 *Name, L7_uint32 *groupId)
{
  return pbVlanGroupIDGet(Name, groupId);
}

/*********************************************************************
*
* @purpose  To get the protocol(s) of the group identified by groupId.
*
* @param    L7_uint32  UnitIndex  @b((input)) unit for this operation
* @param    L7_uint32  groupId    @b((input)) group id
* @param    L7_uint32  *protocol  @b{(Output)) buffer to be filled in
*                                 with the protocol(s) of this group
*
* @returns  L7_SUCCESS, if the specified group exists
* @returns  L7_FAILURE, if invalid groupid or group does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanGroupProtocolGet(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 *protocol, L7_uint32 *type)
{
  L7_uint32 i;
  L7_uint32 prot[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32 ptype[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_RC_t rc;

  for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
  {
    prot[i] = L7_NULL;
  }

  rc = pbVlanGroupProtocolGet(groupId, prot, ptype);
  
  if (rc == L7_SUCCESS)
  {
    for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
    {
      *(protocol + i) = prot[i];
      *(type + i) = ptype[i];
    }
    rc = L7_SUCCESS;
  }

  return rc;
}


/*********************************************************************
*
* @purpose  To get the VLAN ID of the group identified by groupId.
*
* @param    L7_uint32  UnitIndex  @b((input)) unit for this operation
* @param    L7_uint32  groupId    @b((input)) group id
* @param    L7_uint32  *vid       @b{(Output)) vlan id of this group
*
* @returns  L7_SUCCESS, if the specified group exists
* @returns  L7_FAILURE, if invalid groupid or group does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanGroupVIDGet(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 *vid)
{
  return pbVlanGroupVIDGet(groupId, vid);
}
        

/*********************************************************************
*
* @purpose  To get the port(s) of the group identified by groupId.
*
* @param    L7_uint32  UnitIndex   @b((input)) unit for this operation
* @param    L7_uint32  groupId     @b((input)) group id
* @param    L7_uint32  intIfNum    @b{(input)) group port, start
* @param    L7_uint32  *nextIfNum  @b{(Output)) group port, next
*
* @returns  L7_SUCCESS, if the specified group exists
* @returns  L7_FAILURE, if invalid groupid, group does not exist, or
*                       no more ports in this group      
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanGroupPortGetNext(L7_uint32 UnitIndex, L7_uint32 groupId, 
                                    L7_uint32 intIfNum, L7_uint32 *nextIfNum)
{
  return pbVlanGroupPortGetNext(groupId, intIfNum, nextIfNum);
}


/*********************************************************************
*
* @purpose  To check if the specified port exists in the group.
*
* @param    L7_uint32  UnitIndex   @b((input)) unit for this operation
* @param    L7_uint32  groupId     @b((input)) group id
* @param    L7_uint32  intIfNum    @b{(input)) group port
*
* @returns  L7_SUCCESS, if the specified port exists
* @returns  L7_FAILURE, if invalid groupid, group does not exist, or
*                       specified port does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanGroupPortGetExact(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 intIfNum)
{
  return pbVlanGroupPortGetExact(groupId, intIfNum);
}

/*********************************************************************
*
* @purpose  To check if the specified protocol exists in the group.
*
* @param    L7_uint32  UnitIndex   @b((input)) unit for this operation
* @param    L7_uint32  groupId     @b((input)) group id
* @param    L7_uint32  proto       @b{(input)) group protocol
*
* @returns  L7_SUCCESS, if the specified protocol exists
* @returns  L7_FAILURE, if invalid groupid, group does not exist, or
*                       specified protocol does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanGroupProtocolGetExact(L7_uint32 UnitIndex, L7_uint32 groupId, L7_uint32 proto)
{
  return pbVlanGroupProtocolGetExact(groupId, proto);
}

/*********************************************************************
*
* @purpose  To get the next protocol of the group identified by groupId.
*
* @param    L7_uint32  groupId     @b((input)) group id
* @param    L7_uint32  proto       @b{(input)) group proto, start
* @param    L7_uint32  *nextProto  @b{(Output))group proto, next
*
* @returns  L7_SUCCESS, if the specified group exists
* @returns  L7_FAILURE, if invalid groupid, group does not exist, or
*                       no more protocol in this group
*
* @comments The value of 'proto' will be zero (0) when trying
*           to find the very first protocol of the group.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanGroupProtocolGetNext(L7_uint32 UnitIndex, L7_uint32 groupId,
                                    L7_uint32 proto, L7_uint32 *nextProto)
{
  return pbVlanGroupProtocolGetNext(groupId, proto, nextProto);
}

/*********************************************************************
*
* @purpose  To check if the given interface is valid for pbvlan
*
* @param    L7_uint32  UnitIndex   @b((input)) unit for this operation
* @param    L7_uint32  intIfNum    @b{(input)) group port
*
* @returns  L7_TRUE, if the interface is valid
* @returns  L7_FALSE, otherwise
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_BOOL usmDbPbVlanIsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  return pbVlanIsValidIntf(intIfNum);
}

