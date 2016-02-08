/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\l7public\api\pbvlan_api.h
*
* @purpose Contains LVL7 Protocol-Based VLAN offerings
*
* @component Protocol-Based VLAN
*
* @comments tba
*
* @create 01/07/2002
*
* @author rjindal
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_PBVLAN_API_H
#define INCLUDE_PBVLAN_API_H

#include "l7_common.h"

#define L7_PBVLAN_MIN_GROUP_NAME  1
#define L7_PBVLAN_MAX_GROUP_NAME  16
#define L7_PBVLAN_MAX_NUM_VLANS   4094

/*********************************************************************
*
* @purpose  To check if a group name already exists.
*
* @param    L7_char8  *Name  name of the group
*
* @returns  L7_TRUE, if the group name is found
* @returns  L7_FALSE, if the group name is not found
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_BOOL pbVlanGroupExist(L7_char8 *Name);

/*********************************************************************
*
* @purpose  To create a new group.
*
* @param    L7_char8  *Name  name of the group
*
* @returns  L7_SUCCESS, if the group name is added
* @returns  L7_FAILURE, if the group name is not added
* @returns  L7_ALREADY_CONFIGURED, if the group name already exists
*
* @comments If a group is created, it is also assigned a group id.
*
* @end
*
*********************************************************************/
extern L7_RC_t pbVlanGroupCreate(L7_char8 *Name);

/*********************************************************************
*
* @purpose  To delete a group.
*
* @param    L7_uint32  groupId  group id of an existing group 
*
* @returns  L7_SUCCESS, if the group is deleted
* @returns  L7_FAILURE, if invalid groupid or unconfigured group 
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t pbVlanGroupDelete(L7_uint32 groupId);

/*********************************************************************
*
* @purpose  To add a protocol to a group.
*
* @param    L7_uint32  groupId  group id of an existing group
* @param    L7_uint32  prtl     protocol to be added to the group

* @returns  L7_SUCCESS, if protocol is added to the group
* @returns  L7_FAILURE, if invalid groupid, protocol, or 
*                       unconfigured group
* @returns  L7_ALREADY_CONFIGURED, if this protocol already exists
*
* @notes    If A be a group to which 'prtl' is to be added, and there
*           are other groups (X = B,C,...) sharing ports of group A
*           and using 'prtl', then 'prtl' cannot be added to group A.
* @notes    In the sample implementation there are only three 
*           configurable protocols: IP, IPX, ARP.
*
* @end
*
*********************************************************************/
extern L7_RC_t pbVlanGroupProtocolAdd(L7_uint32 groupId, L7_uint32 prtl);

/*********************************************************************
*
* @purpose  To delete a protocol from a group.
*
* @param    L7_uint32  groupId  group id of an existing group
* @param    L7_uint32  prtl     protocol to be deleted from the group
*
* @returns  L7_SUCCESS, if protocol is deleted from the group
* @returns  L7_FAILURE, if invalid groupid, protocol, or
*                       unconfigured group
* @returns  L7_ERROR, if the group does not have this protocol
*
* @comments The protocol is deleted from all the ports in the group.
*
* @end
*
*********************************************************************/
extern L7_RC_t pbVlanGroupProtocolDelete(L7_uint32 groupId, L7_uint32 prtl);

/*********************************************************************
*
* @purpose  To add a vlan id to a group.
*
* @param    L7_uint32  groupId  group id of an existing group
* @param    L7_uint32  vid      vlan id of the group
*
* @returns  L7_SUCCESS, if vlan id is added to the group
* @returns  L7_FAILURE, if invalid groupid, vlanid, or unconfigured group
* @returns  L7_ERROR, if the group already has a vlan id
*
* @notes    Vlanid can be any number in the range 1-4094, 1 being
*           the default.
* @notes    Vlanid is added to all the ports in the group.
*
* @end
*
*********************************************************************/
extern L7_RC_t pbVlanGroupVlanIDAdd(L7_uint32 groupId, L7_uint32 vid);

/*********************************************************************
*
* @purpose  To delete a vlan id from a group.
*
* @param    L7_uint32  groupId  group id of an existing group
* @param    L7_uint32  vid      vlan id of the group
*
* @returns  L7_SUCCESS, if vlan id is deleted from the group
* @returns  L7_FAILURE, if invalid groupid, vlanid, or unconfigured group
* @returns  L7_ERROR, if the group does not have this vlan id
*
* @comments Vlanid is deleted from all the ports in the group.
*
* @end
*
*********************************************************************/
extern L7_RC_t pbVlanGroupVlanIDDelete(L7_uint32 groupId, L7_uint32 vid);

/*********************************************************************
*
* @purpose  To add a port to a group.
*
* @param    L7_uint32  groupId   group id of an existing group
* @param    L7_uint32  intIfNum  internal intf number to be added
*
* @returns  L7_SUCCESS, if the port is added to the group
* @returns  L7_FAILURE, if invalid groupid, port, or unconfigured group
* @returns  L7_ALREADY_CONFIGURED, if the port is already configured
*
* @notes    If A be a group to which 'intIfNum' is to be added, and
*           there are other groups (X = B,C,...) sharing the same 
*           port and using same protocols as group A, then 'intIfNum'
*           cannot be added to group A.
*
* @end
*
*********************************************************************/
extern L7_RC_t pbVlanGroupPortAdd(L7_uint32 groupId, L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  To delete a port from a group.
*
* @param    L7_uint32  groupId   group id of an existing group
* @param    L7_uint32  intIfNum  internal intf number to be deleted
*
* @returns  L7_SUCCESS, if port is deleted from the group
* @returns  L7_FAILURE, if invalid groupid, port, or unconfigured group
* @returns  L7_ERROR, if no ports in the group or the specified port
*                     is not in use
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t pbVlanGroupPortDelete(L7_uint32 groupId, L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Given a group id, get the next created group's id.
*
* @param    L7_uint32  groupId  @b((input)) group id, start
* @param    L7_uint32  *next    @b((input)) group id, next
*
* @returns  L7_SUCCESS, if the next created group is found
* @returns  L7_FAILURE, if invalid groupid or next group is not found
*
* @comments If the 'groupId' does not exist, the id of the next
*           created group will be returned, e.g., groupId is 0
*           when trying to find the very first created group.
*
* @end
*
*********************************************************************/
extern L7_RC_t pbVlanGroupGetNext(L7_uint32 groupId, L7_uint32 *next);

/*********************************************************************
*
* @purpose  To check if the specified group exists.
*
* @param    L7_uint32  groupId  @b((input)) specified group's id
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
extern L7_RC_t pbVlanGroupGetExact(L7_uint32 groupId);

/*********************************************************************
*
* @purpose  To get the name of the group identified by groupId.
*
* @param    L7_uint32  groupId  @b((input)) group id
* @param    L7_char8   *buf     @b{(Output)) buffer to be filled in
*                               with the name of this group
*
* @returns  L7_SUCCESS, if the specified group exists
* @returns  L7_FAILURE, if invalid groupid or group does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t pbVlanGroupNameGet(L7_uint32 groupId, L7_char8 *buf);

/*********************************************************************
*
* @purpose  To get the protocol(s) of the group identified by groupId.
*
* @param    L7_uint32  groupId  @b((input)) group id
* @param    L7_uint32  *prtl    @b{(Output)) protocol array containing
*                               the protocol(s) of this group
*
* @returns  L7_SUCCESS, if the specified group exists
* @returns  L7_FAILURE, if invalid groupid or group does not exist
*
* @comments A group can have up to three configurable protocols:
*           IP, ARP, and IPX.
*
* @end
*
*********************************************************************/
extern L7_RC_t pbVlanGroupProtocolGet(L7_uint32 groupId, L7_uint32 *prtl);

/*********************************************************************
*
* @purpose  To get the VLAN ID of the group identified by groupId.
*
* @param    L7_uint32  groupId  @b((input)) group id
* @param    L7_uint32  *vid     @b{(Output)) the vlan id of this group
*
* @returns  L7_SUCCESS, if the specified group exists
* @returns  L7_FAILURE, if invalid groupid or group does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t pbVlanGroupVIDGet(L7_uint32 groupId, L7_uint32 *vid);

/*********************************************************************
*
* @purpose  To get the port(s) of the group identified by groupId.
*
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
extern L7_RC_t pbVlanGroupPortGetNext(L7_uint32 groupId, L7_uint32 intIfNum, L7_uint32 *nextIfNum);

/*********************************************************************
*
* @purpose  To check if the specified port exists in the group.
*
* @param    L7_uint32  groupId   @b((input)) specified group's id
* @param    L7_uint32  intIfNum  @b{(input)) group port
*
* @returns  L7_SUCCESS, if the specified port exists
* @returns  L7_FAILURE, if invalid groupid, intIfNum, or 
*                       the specified group or port does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t pbVlanGroupPortGetExact(L7_uint32 groupId, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine if pbVlan is interested in the given interface
*
* @param    intIfNum              @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
extern L7_BOOL pbVlanIsValidIntf(L7_uint32 intIfNum);

#endif  /* PBVLAN_API_H */
