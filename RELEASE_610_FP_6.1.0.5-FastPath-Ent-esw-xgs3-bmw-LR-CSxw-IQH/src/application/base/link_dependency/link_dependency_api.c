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
* @filename link_dependency_api.c
*
* @purpose application level APIs
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

#include "nimapi.h"
#include "cnfgr.h"
#include "link_dependency_data.h"
#include "link_dependency.h"
#include "link_dependency_api.h"

extern void *LinkDependencySemaphore;

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
L7_BOOL linkDependencyIsValidIntf( L7_uint32 intIfNum )
{
  L7_INTF_TYPES_t sysIntfType;

  if (cnfgrIsComponentPresent(L7_LINK_DEPENDENCY_COMPONENT_ID) == L7_FALSE)
  {
    return L7_FALSE;
  }
  if ( ( intIfNum != 0 ) &&
       ( L7_SUCCESS == nimGetIntfType(intIfNum, &sysIntfType) ) &&
       ( ( L7_PHYSICAL_INTF == sysIntfType ) ||
         ( L7_LAG_INTF      == sysIntfType ) ) )
  {
    return( L7_TRUE );
  }

  return( L7_FALSE );
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
L7_BOOL linkDependencyIsValidGroup( L7_uint32 groupId )
{
  if (cnfgrIsComponentPresent(L7_LINK_DEPENDENCY_COMPONENT_ID) == L7_FALSE)
  {
    return L7_FALSE;
  }
  if ( ( L7_LINK_DEPENDENCY_FIRST_GROUP > groupId ) ||
       ( L7_LINK_DEPENDENCY_LAST_GROUP  < groupId ) )
  {
    return( L7_FALSE );
  }

  return( L7_TRUE );
}

/*********************************************************************
* @purpose  Indicates if the interface is can be enabled
*
* @param    intIfNum     @b{(input)}  the Internal Interface Number to check
*
* @returns  L7_TRUE      If interface is valid
* @returns  L7_FALSE     Otherwise
*
* @comments An interface can NOT be enabled if all interfaces upon which
*           it depends are down.
*
* @end
*********************************************************************/
L7_BOOL linkDependencyIntfCanBeEnabled( L7_uint32 intIfNum )
{
  link_dependency_membership_t *membership = &LinkDependencyData_g.membership[intIfNum - 1];
  link_dependency_group_t      *group;
  L7_uint32                     groupId;
  L7_uint32                     result;

  if (cnfgrIsComponentPresent(L7_LINK_DEPENDENCY_COMPONENT_ID) == L7_FALSE)
  {
    return L7_TRUE;
  }
  L7_NONZEROMASK( membership->member, result, L7_LINK_DEPENDENCY_MAX_GROUPS );
  if ( 0 == result )            /* Link not a member of any group */
  {
    return( L7_TRUE );
  }

  for ( groupId  = L7_LINK_DEPENDENCY_FIRST_GROUP;
        groupId <= L7_LINK_DEPENDENCY_LAST_GROUP;
        groupId++ )
  {
    if ( 0 != L7_ISMASKBITSET( membership->member, groupId ) )
    {
      group = &LinkDependencyData_g.groups[groupId - 1];

      if ( ( L7_TRUE  == group->inUse )  &&
           ( L7_FALSE == group->allLinksDown ) )
      {
        return( L7_TRUE );
      }
    }
  }

  return( L7_FALSE );
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
L7_RC_t linkDependencyGroupGet( L7_uint32       groupId,
                                L7_INTF_MASK_t *members,
                                L7_INTF_MASK_t *dependencies )
{
  link_dependency_group_t *group;

  if (cnfgrIsComponentPresent(L7_LINK_DEPENDENCY_COMPONENT_ID) == L7_FALSE)
  {
    return L7_FAILURE;
  }
  if ( ( L7_NULLPTR == members ) ||
       ( L7_NULLPTR == dependencies ) )
  {
    return( L7_FAILURE );
  }

  if ( L7_FALSE == linkDependencyIsValidGroup( groupId ) )
  {
    return( L7_NOT_EXIST );
  }

  group = &LinkDependencyData_g.groups[groupId - 1];

  if ( L7_TRUE != group->inUse )
  {
    return( L7_ERROR );
  }

  if ( L7_SUCCESS != osapiSemaTake( LinkDependencySemaphore, L7_WAIT_FOREVER ) )
  {
    return( L7_FAILURE );
  }

  L7_INTF_MASKEQ( *members,      group->members );
  L7_INTF_MASKEQ( *dependencies, group->dependencies );

  osapiSemaGive( LinkDependencySemaphore );

  return( L7_SUCCESS );
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
L7_RC_t linkDependencyGroupSet( L7_uint32       groupId,
                                L7_INTF_MASK_t *members,
                                L7_INTF_MASK_t *dependencies )
{
  link_dependency_group_t *group;
  L7_INTF_MASK_t           added;
  L7_INTF_MASK_t           deleted;
  L7_uint32                emptyMembers;
  L7_uint32                emptyDependencies;
  L7_uint32                intIfNum;
  L7_uint32                duplicates;
  L7_uint32                result;

  if (cnfgrIsComponentPresent(L7_LINK_DEPENDENCY_COMPONENT_ID) == L7_FALSE)
  {
    return L7_FAILURE;
  }
  if ( ( L7_NULLPTR == members ) ||
       ( L7_NULLPTR == dependencies ) )
  {
    return( L7_FAILURE );
  }

  if ( L7_FALSE == linkDependencyIsValidGroup( groupId ) )
  {
    return( L7_NOT_EXIST );
  }

  group = &LinkDependencyData_g.groups[groupId - 1];

  L7_INTF_NONZEROMASK( (*members),      emptyMembers );
  L7_INTF_NONZEROMASK( (*dependencies), emptyDependencies );

  if ( ( 0 == emptyMembers ) &&
       ( 0 == emptyDependencies ) )
  {
    return( linkDependencyGroupClear( groupId ) );
  }

  L7_INTF_MASKEQ(      added, *members );
  L7_INTF_MASKANDEQ(   added, *dependencies );
  L7_INTF_NONZEROMASK( added, duplicates );
  if ( 0 != duplicates )
  {
    return( L7_ALREADY_CONFIGURED );
  }

  group->inUse = L7_TRUE;

  if ( L7_SUCCESS != osapiSemaTake( LinkDependencySemaphore, L7_WAIT_FOREVER ) )
  {
    return( L7_FAILURE );
  }

  /*
   * Update the dependency list.
   */

  /*
   * First get the mask of interfaces which are in the new mask but not the
   * old mask  -- Added interfaces.
   */
  L7_INTF_MASKEQ      ( added, *dependencies );
  L7_INTF_MASKANDEQINV( added, group->dependencies );

  /*
   * Now get the mask of interfaces which are in the old mask but not the new
   * mask -- Deleted interfaces.
   */
  L7_INTF_MASKEQ      ( deleted, group->dependencies );
  L7_INTF_MASKANDEQINV( deleted, *dependencies );

  for ( intIfNum = 1; intIfNum <= L7_MAX_INTERFACE_COUNT; intIfNum++ )
  {
    if ( 0 != L7_INTF_ISMASKBITSET( added, intIfNum ) )
    {
      L7_INTF_SETMASKBIT( group->dependencies, intIfNum );
      L7_SETMASKBIT( LinkDependencyData_g.membership[intIfNum - 1].dependency, groupId );
      if ( L7_FALSE == group->inUse )
      {
        group->inUse = L7_TRUE;
      }
    }
    else if ( 0 != L7_INTF_ISMASKBITSET( deleted, intIfNum ) )
    {
      L7_INTF_CLRMASKBIT( group->dependencies, intIfNum );
      L7_CLRMASKBIT( LinkDependencyData_g.membership[intIfNum - 1].dependency, groupId );
    }
  }

  /*
   * Set allLinksDown to FALSE so that the group will only be updated if allLinksDown is
   * found to be TRUE by linkDependencyGroupStateUpdate.
   */
  group->allLinksDown = L7_FALSE;

  /*
   * Update the membership list.  Removed interfaces must be updated here.
   */

  /*
   * First get the mask of interfaces which are in the new mask but not the
   * old mask  -- Added interfaces.
   */
  L7_INTF_MASKEQ      ( added, *members );
  L7_INTF_MASKANDEQINV( added, group->members );

  /*
   * Now get the mask of interfaces which are in the old mask but not the new
   * mask -- Deleted interfaces.
   */
  L7_INTF_MASKEQ      ( deleted, group->members );
  L7_INTF_MASKANDEQINV( deleted, *members );

  for ( intIfNum = 1; intIfNum <= L7_MAX_INTERFACE_COUNT; intIfNum++ )
  {
    if ( 0 != L7_INTF_ISMASKBITSET( added, intIfNum ) )
    {
      L7_INTF_SETMASKBIT( group->members, intIfNum );
      L7_SETMASKBIT( LinkDependencyData_g.membership[intIfNum - 1].member, groupId );
      if ( L7_FALSE == group->inUse )
      {
        group->inUse = L7_TRUE;
      }
    }
    else if ( 0 != L7_INTF_ISMASKBITSET( deleted, intIfNum ) )
    {
      L7_INTF_CLRMASKBIT( group->members, intIfNum );
      L7_CLRMASKBIT( LinkDependencyData_g.membership[intIfNum - 1].member, groupId );
      linkDependencyIntfStateUpdate( intIfNum );
    }
  }

  L7_NONZEROMASK( group->members, result, L7_MAX_INTERFACE_COUNT );
  if ( 0 == result )
  {
    L7_NONZEROMASK( group->dependencies, result, L7_MAX_INTERFACE_COUNT );
    if ( 0 == result )
    {
      group->inUse = L7_FALSE;
    }
  }

  if ( L7_TRUE == group->inUse )
  {
    /*
     * Now update the status of all interfaces in the group.
     */
    linkDependencyGroupStateUpdate( groupId );
  }

  osapiSemaGive( LinkDependencySemaphore );

  return( L7_SUCCESS );
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
L7_RC_t linkDependencyGroupClear( L7_uint32 groupId )
{
  link_dependency_group_t *group;
  L7_uint32                intIfNum;

  if ( L7_FALSE == linkDependencyIsValidGroup( groupId ) )
  {
    return( L7_NOT_EXIST );
  }

  group = &LinkDependencyData_g.groups[groupId - 1];

  if ( L7_TRUE == group->inUse )
  {
    if ( L7_SUCCESS != osapiSemaTake( LinkDependencySemaphore, L7_WAIT_FOREVER ) )
    {
      return( L7_FAILURE );
    }

    /*
     * Clear the dependency list.  No interface state updating necessary here.
     */
    for ( intIfNum = 1; intIfNum <= L7_MAX_INTERFACE_COUNT; intIfNum++ )
    {
      if ( 0 != L7_INTF_ISMASKBITSET( group->dependencies, intIfNum ) )
      {
        L7_INTF_CLRMASKBIT( group->dependencies, intIfNum );
        L7_CLRMASKBIT( LinkDependencyData_g.membership[intIfNum - 1].dependency, groupId );
      }
    }

    /*
     * Clear the membership list.  Interface state must be updated here.
     */
    for ( intIfNum = 1; intIfNum <= L7_MAX_INTERFACE_COUNT; intIfNum++ )
    {
      if ( 0 != L7_INTF_ISMASKBITSET( group->members, intIfNum ) )
      {
        L7_INTF_CLRMASKBIT( group->members, intIfNum );
        L7_CLRMASKBIT( LinkDependencyData_g.membership[intIfNum - 1].member, groupId );
        linkDependencyIntfStateUpdate( intIfNum );
      }
    }

    group->inUse        = L7_FALSE;
    group->allLinksDown = L7_FALSE;

    osapiSemaGive( LinkDependencySemaphore );
  }

  return( L7_SUCCESS );
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
L7_RC_t linkDependencyGroupMemberIntfAdd( L7_uint32 groupId,
                                          L7_uint32 intIfNum )
{
  link_dependency_group_t *group;

  if ( L7_FALSE == linkDependencyIsValidIntf( intIfNum ) )
  {
    return( L7_NOT_SUPPORTED );
  }

  if ( L7_FALSE == linkDependencyIsValidGroup( groupId ) )
  {
    return( L7_NOT_EXIST );
  }

  group = &LinkDependencyData_g.groups[groupId - 1];

  if ( 0 != L7_INTF_ISMASKBITSET( group->dependencies, intIfNum ) )
  {
    return( L7_ALREADY_CONFIGURED );
  }

  /*
   * If the interface is not already in the group, then make it so.
   */
  if ( 0 == L7_INTF_ISMASKBITSET( group->members, intIfNum ) )
  {
    if ( L7_SUCCESS != osapiSemaTake( LinkDependencySemaphore, L7_WAIT_FOREVER ) )
    {
      return( L7_FAILURE );
    }

    /* Make the interface a member of the group */
    L7_INTF_SETMASKBIT( group->members, intIfNum );

    /* Add the group to the interface's membership list */
    L7_SETMASKBIT( LinkDependencyData_g.membership[intIfNum - 1].member, groupId );

    if ( L7_TRUE != group->inUse )
    {
      group->inUse = L7_TRUE;
      group->allLinksDown = L7_FALSE;
    }
    else
    {
      if ( L7_TRUE == group->allLinksDown )
      {
        linkDependencyIntfStateUpdate( intIfNum );
      }
    }

    LinkDependencyData_g.configChanged = L7_TRUE;

    osapiSemaGive( LinkDependencySemaphore );
  }

  return( L7_SUCCESS );
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
L7_RC_t linkDependencyGroupMemberIntfDelete( L7_uint32 groupId,
                                             L7_uint32 intIfNum )
{
  link_dependency_group_t *group;
  L7_uint32                result;

  if ( L7_FALSE == linkDependencyIsValidIntf( intIfNum ) )
  {
    return( L7_NOT_SUPPORTED );
  }

  if ( L7_FALSE == linkDependencyIsValidGroup( groupId ) )
  {
    return( L7_NOT_EXIST );
  }

  group = &LinkDependencyData_g.groups[groupId - 1];

  /*
   * If the interface is in the group, then remove it.
   */
  if ( 0 != L7_INTF_ISMASKBITSET( group->members, intIfNum ) )
  {
    if ( L7_SUCCESS != osapiSemaTake( LinkDependencySemaphore, L7_WAIT_FOREVER ) )
    {
      return( L7_FAILURE );
    }

    /* Remove the interface from the group members */
    L7_INTF_CLRMASKBIT( group->members, intIfNum );
    L7_NONZEROMASK( group->members, result, L7_MAX_INTERFACE_COUNT );
    if ( 0 == result )
    {
      L7_NONZEROMASK( group->dependencies, result, L7_MAX_INTERFACE_COUNT );
      if ( 0 == result )
      {
        group->inUse = L7_FALSE;
      }
    }

    /* Remove the group from the interface's membership list */
    L7_CLRMASKBIT( LinkDependencyData_g.membership[intIfNum - 1].member, groupId );

    linkDependencyIntfStateUpdate( intIfNum );

    LinkDependencyData_g.configChanged = L7_TRUE;

    osapiSemaGive( LinkDependencySemaphore );
  }

  return( L7_SUCCESS );
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
L7_RC_t linkDependencyGroupDependencyAdd( L7_uint32 groupId,
                                          L7_uint32 intIfNum )
{
  link_dependency_group_t *group;

  if ( L7_FALSE == linkDependencyIsValidIntf( intIfNum ) )
  {
    return( L7_NOT_SUPPORTED );
  }

  if ( L7_FALSE == linkDependencyIsValidGroup( groupId ) )
  {
    return( L7_NOT_EXIST );
  }

  group = &LinkDependencyData_g.groups[groupId - 1];

  if ( 0 != L7_INTF_ISMASKBITSET( group->members, intIfNum ) )
  {
    return( L7_ALREADY_CONFIGURED );
  }

  /*
   * If the interface is not already in the group, then make it so.
   */
  if ( 0 == L7_INTF_ISMASKBITSET( group->dependencies, intIfNum ) )
  {
    if ( L7_SUCCESS != osapiSemaTake( LinkDependencySemaphore, L7_WAIT_FOREVER ) )
    {
      return( L7_FAILURE );
    }

    /* Make the interface a dependency of the group */
    L7_INTF_SETMASKBIT( group->dependencies, intIfNum );

    if ( L7_TRUE == group->inUse )
    {
      /* See if the group state has changed */
      linkDependencyGroupStateUpdate( groupId );
    }
    else                        /* No members yet */
    {
      group->inUse = L7_TRUE;
      if ( 0 == L7_INTF_ISMASKBITSET( LinkDependencyData_g.intfStatus, intIfNum ) )
      {
        group->allLinksDown = L7_TRUE;
      }
      else
      {
        group->allLinksDown = L7_FALSE;
      }
    }

    /* Add the group to the interface's dependency list */
    L7_SETMASKBIT( LinkDependencyData_g.membership[intIfNum - 1].dependency, groupId );

    LinkDependencyData_g.configChanged = L7_TRUE;

    osapiSemaGive( LinkDependencySemaphore );
  }

  return( L7_SUCCESS );
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
L7_RC_t linkDependencyGroupDependencyDelete( L7_uint32 groupId,
                                             L7_uint32 intIfNum )
{
  link_dependency_group_t *group;

  if ( L7_FALSE == linkDependencyIsValidIntf( intIfNum ) )
  {
    return( L7_NOT_SUPPORTED );
  }

  if ( L7_FALSE == linkDependencyIsValidGroup( groupId ) )
  {
    return( L7_NOT_EXIST );
  }

  group = &LinkDependencyData_g.groups[groupId - 1];

  /*
   * If the interface is in the group, then remove it.
   */
  if ( 0 != L7_INTF_ISMASKBITSET( group->dependencies, intIfNum ) )
  {
    if ( L7_SUCCESS != osapiSemaTake( LinkDependencySemaphore, L7_WAIT_FOREVER ) )
    {
      return( L7_FAILURE );
    }

    /* Remove the interface from the group dependencies */
    L7_INTF_CLRMASKBIT( group->dependencies, intIfNum );

    /* Remove the group from the interface's dependency list */
    L7_CLRMASKBIT( LinkDependencyData_g.membership[intIfNum - 1].dependency, groupId );

    /* See if the group state has changed */
    linkDependencyGroupStateUpdate( groupId );

    LinkDependencyData_g.configChanged = L7_TRUE;

    osapiSemaGive( LinkDependencySemaphore );
  }

  return( L7_SUCCESS );
}

/*********************************************************************
* @purpose  Apply Link Dependency Configuration Data
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    This should not be called from Phase3 only. This function
*           is called to apply the synchronised running configuration
*           on the standby during failover.
*
* @end
*********************************************************************/
L7_RC_t linkDependencyApplyConfigData(void)
{
  L7_uint32 groupId;
  L7_uint32 intIfNum;

  link_dependency_group_t * group_cfg, *group_g;

  if (cnfgrIsComponentPresent(L7_LINK_DEPENDENCY_COMPONENT_ID) == L7_FALSE)
  {
    return L7_SUCCESS;
  }
  /* update the configuration of the operational datastructure */
  for ( groupId = L7_LINK_DEPENDENCY_FIRST_GROUP; groupId <= L7_LINK_DEPENDENCY_LAST_GROUP; groupId++ )
  {
    group_cfg = &LinkDependencyData_cfg.groups[groupId - 1];

    if ( L7_TRUE != group_cfg->inUse )
    {
      continue;
    }

    group_g = &LinkDependencyData_g.groups[groupId - 1];

    for ( intIfNum = 1; intIfNum <= L7_MAX_INTERFACE_COUNT; intIfNum++ )
    {

      /*
       * Update the dependency list.
       */
      if (L7_INTF_ISMASKBITSET(group_cfg->dependencies, intIfNum) != L7_NULL)
      {
        if ( 0 == L7_INTF_ISMASKBITSET( group_g->dependencies, intIfNum ) )
        {
          /* Make the interface a dependency of the group */
          L7_INTF_SETMASKBIT( group_g->dependencies, intIfNum );
          group_g->inUse = L7_TRUE;

          /* Add the group to the interface's dependency list */
          L7_SETMASKBIT( LinkDependencyData_g.membership[intIfNum - 1].dependency, groupId );
          LinkDependencyData_g.configChanged = L7_TRUE;
        }
      }

      /*
       * Update the membership list.
       */
      if ( L7_INTF_ISMASKBITSET( group_cfg->members, intIfNum )  != L7_NULL)
      {
        if ( 0 == L7_INTF_ISMASKBITSET( group_g->members, intIfNum ) )
        {
          /* Make the interface a dependency of the group */
          L7_INTF_SETMASKBIT( group_g->members, intIfNum );
          group_g->inUse = L7_TRUE;

          /* Add the group to the interface's dependency list */
          L7_SETMASKBIT( LinkDependencyData_g.membership[intIfNum - 1].member, groupId );
          LinkDependencyData_g.configChanged = L7_TRUE;
        }
      }
    }/* for intf */
  } /* for group */

  return L7_SUCCESS;
}
