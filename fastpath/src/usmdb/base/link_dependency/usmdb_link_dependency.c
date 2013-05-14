/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2006-2007
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename usmdb_link_dependency.c
*
* @purpose USMDB APIs for link dependency
*
* @component link dependency
*
* @comments none
*
* @create 12/06/2006
*
* @author dflint
* @end
*
**********************************************************************/

#include "usmdb_link_dependency_api.h"
#include "link_dependency_api.h"


/*********************************************************************
* @purpose  Indicates if the interface is valid for link dependencies
*
* @param    intIfNum     @b{(input)}  the Internal Interface Number to check
*
* @returns  L7_TRUE      If interface is valid
* @returns  L7_FALSE     Otherwise
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL usmDbLinkDependencyIsValidIntf( L7_uint32 intIfNum )
{
  return( linkDependencyIsValidIntf( intIfNum ) );
}

/*********************************************************************
* @purpose  Indicates if groupId is a valid group ID
*
* @param    groupId      @b{(input)}  the group ID to check
*
* @returns  L7_TRUE      If group Id is valid
* @returns  L7_FALSE     Otherwise
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL usmDbLinkDependencyIsValidGroup( L7_uint32 groupId )
{
  return( linkDependencyIsValidGroup( groupId ) );
}

/*********************************************************************
* @purpose  Get the members of a group and the links upon which they depend.
*
* @param    groupId      @b{(input)}     the ID of the group
* @param    members      @b{(in/output)} bitmask of member interfaces
* @param    dependencies @b{(in/output)} bitmask of interfaces depended on
*
* @returns  L7_SUCCESS   If successful
* @returns  L7_ERROR     If the group is unconfigured
* @returns  L7_NOT_EXIST If the groupId is out of range
* @returns  L7_FAILURE   Any other failure
*
* @comments Copies the group members to the masks passed in.
*
* @end
*********************************************************************/
L7_RC_t usmDbLinkDependencyGroupGet( L7_uint32       groupId,
                                     L7_INTF_MASK_t *members,
                                     L7_INTF_MASK_t *dependencies )
{
  return( linkDependencyGroupGet( groupId, members, dependencies ) );
}

/*********************************************************************
* @purpose  Set the members of a group and the links upon which they depend.
*
* @param    groupId      @b{(input)}  the ID of the group
* @param    members      @b{(input)}  bitmask of member interfaces
* @param    dependencies @b{(input)}  bitmask of interfaces depended on
*
* @returns  L7_SUCCESS             If successful
* @returns  L7_NOT_SUPPORTED       If an unsupported interface is specified
* @returns  L7_NOT_EXIST           If the groupId is out of range
* @returns  L7_ALREADY_CONFIGURED  If attempting to add an interface
*                                  to both member and dependency lists
* @returns  L7_FAILURE             Any other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbLinkDependencyGroupSet( L7_uint32      *_groupId,
                                     L7_INTF_MASK_t *members,
                                     L7_INTF_MASK_t *dependencies )
{
  return( linkDependencyGroupSet( _groupId, members, dependencies ) );
}

/*********************************************************************
* @purpose  Clear all interfaces from the group
*
* @param    groupId      @b{(input)}  the ID of the group
*
* @returns  L7_SUCCESS   If successful
* @returns  L7_NOT_EXIST If the groupId is out of range
* @returns  L7_FAILURE   Any other failure
*
* @comments Puts the group in "unconfigured" state.
*
* @end
*********************************************************************/
L7_RC_t usmDbLinkDependencyGroupClear( L7_uint32 groupId )
{
  return( linkDependencyGroupClear( groupId ) );
}

/*********************************************************************
* @purpose  Add an interface to a group's member list.
*
* @param    groupId      @b{(input)}  the ID of the group
* @param    intIfNum     @b{(input)}  internal interface number to be added
*
* @returns  L7_SUCCESS             If successful
* @returns  L7_NOT_SUPPORTED       If an unsupported interface is specified
* @returns  L7_NOT_EXIST           If the groupId is out of range
* @returns  L7_ALREADY_CONFIGURED  If attempting to add an interface
*                                  to both member and dependency lists
* @returns  L7_FAILURE             Any other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbLinkDependencyGroupMemberIntfAdd( L7_uint32 groupId,
                                               L7_uint32 intIfNum )
{
  return( linkDependencyGroupMemberIntfAdd( groupId, intIfNum ) );
}

/*********************************************************************
* @purpose  Remove an interface from a group's member list.
*
* @param    groupId      @b{(input)}  the ID of the group
* @param    intIfNum     @b{(input)}  internal interface number to be removed
*
* @returns  L7_SUCCESS             If successful
* @returns  L7_NOT_SUPPORTED       If an unsupported interface is specified
* @returns  L7_NOT_EXIST           If the groupId is out of range
* @returns  L7_FAILURE             Any other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbLinkDependencyGroupMemberIntfDelete( L7_uint32 groupId,
                                                  L7_uint32 intIfNum )
{
  return( linkDependencyGroupMemberIntfDelete( groupId, intIfNum ) );
}

/*********************************************************************
* @purpose  Add an interface to a group's dependency list.
*
* @param    groupId      @b{(input)}  the ID of the group
* @param    intIfNum     @b{(input)}  internal interface number to be added
*
* @returns  L7_SUCCESS             If successful
* @returns  L7_NOT_SUPPORTED       If an unsupported interface is specified
* @returns  L7_NOT_EXIST           If the groupId is out of range
* @returns  L7_ALREADY_CONFIGURED  If attempting to add an interface
*                                  to both member and dependency lists
* @returns  L7_FAILURE             Any other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbLinkDependencyGroupDependencyAdd( L7_uint32 groupId,
                                               L7_uint32 intIfNum )
{
  return( linkDependencyGroupDependencyAdd( groupId, intIfNum ) );
}

/*********************************************************************
* @purpose  Set group members action
*
* @param    groupId      @b{(input)}  the ID of the group
* @param    link_up      @b{(input)}  whetehr the group members are to be brought up
*
* @returns  L7_SUCCESS             If successful
* @returns  L7_NOT_SUPPORTED       If an unsupported interface is specified
* @returns  L7_NOT_EXIST           If the groupId is out of range
* @returns  L7_ALREADY_CONFIGURED  If attempting to add an interface
*                                  to both member and dependency lists
* @returns  L7_FAILURE             Any other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbLinkDependencyGroupDependencyActionSet( L7_uint32 groupId,
                                                     link_dependency_action_constants_t link_action )
{
  return( linkDependencyGroupDependencyActionSet( groupId, link_action ) );
}

/*********************************************************************
* @purpose  Get group members action
*
* @param    groupId      @b{(input)}  the ID of the group
* @param   *link_up      @b{(input)}  whetehr the group members are to be brought up
*
* @returns  L7_SUCCESS             If successful
* @returns  L7_NOT_SUPPORTED       If an unsupported interface is specified
* @returns  L7_NOT_EXIST           If the groupId is out of range
* @returns  L7_ALREADY_CONFIGURED  If attempting to add an interface
*                                  to both member and dependency lists
* @returns  L7_FAILURE             Any other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbLinkDependencyGroupDependencyActionGet( L7_uint32 groupId,
                                                     link_dependency_action_constants_t *link_action )
{
  return( linkDependencyGroupDependencyActionGet( groupId, link_action ) );
}

/*********************************************************************
* @purpose  Remove an interface from a group's dependency list.
*
* @param    groupId      @b{(input)}  the ID of the group
* @param    intIfNum     @b{(input)}  internal interface number to be removed
*
* @returns  L7_SUCCESS             If successful
* @returns  L7_NOT_SUPPORTED       If an unsupported interface is specified
* @returns  L7_NOT_EXIST           If the groupId is out of range
* @returns  L7_FAILURE             Any other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbLinkDependencyGroupDependencyDelete( L7_uint32 groupId,
                                                  L7_uint32 intIfNum )
{
  return( linkDependencyGroupDependencyDelete( groupId, intIfNum ) );
}
