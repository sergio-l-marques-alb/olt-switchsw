/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2006-2007
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
***********************************************************************
*
* @filename link_dependency.c
*
* @purpose The main event processing
*
* @component link dependency
*
* @comments
*
* @create 12/06/2006
*
* @author dflint
* @end
*
**********************************************************************/

#include <string.h>
#include "comm_mask.h"
#include "dot3ad_api.h"
#include "link_dependency_data.h"
#include "link_dependency.h"
#include "link_dependency_api.h"
#include "nvstoreapi.h"

void *LinkDependencySemaphore = L7_NULLPTR;

link_dependency_global_data_t LinkDependencyData_g;

/* Link dependency configuration data structure used during save */
link_dependency_cfg_data_t LinkDependencyData_cfg;

/*********************************************************************
* @purpose  Save the binary configuration to .cfg file
*
* @param    No parameters
*
* @returns  L7_SUCCESS
*
* @comments Since only text-based configuration is used by this project,
*           simply reset the config changed flag.
*
* @end
*********************************************************************/
L7_RC_t linkDependencyCfgSave( void )
{
  if (LinkDependencyData_g.configChanged == L7_FALSE)
  {
    return L7_SUCCESS;
  }

  if ( L7_SUCCESS != osapiSemaTake( LinkDependencySemaphore, L7_LINK_DEPENDENCY_SEMAPHORE_WAIT ) )
  {
    return( L7_FAILURE );
  }

  LinkDependencyData_g.configChanged = L7_FALSE;
  LinkDependencyData_cfg.hdr.dataChanged = L7_FALSE;

  /* copy the required information into the LinkDependencyData_cfg structure */
  memcpy(LinkDependencyData_cfg.groups, LinkDependencyData_g.groups, sizeof (LinkDependencyData_g.groups));

  osapiSemaGive( LinkDependencySemaphore );

  LinkDependencyData_cfg.checkSum = nvStoreCrc32((L7_uchar8*)&LinkDependencyData_cfg,
                                    (L7_uint32)(sizeof(link_dependency_cfg_data_t) -
                                    sizeof(LinkDependencyData_cfg.checkSum)));

  if ( (sysapiCfgFileWrite(L7_LINK_DEPENDENCY_COMPONENT_ID, LINK_DEPENDENCY_FILENAME,
                           (L7_char8*)&LinkDependencyData_cfg, (L7_int32)sizeof(link_dependency_cfg_data_t))) != L7_SUCCESS)
  {
    LOG_MSG("Error on call to sysapiCfgFileWrite routine on config file %s\n", LINK_DEPENDENCY_FILENAME);
  }

  return( L7_SUCCESS );
}

/*********************************************************************
* @purpose  Indicate whether the configuration has changed since the last save.
*
* @param    No parameters
*
* @returns  L7_TRUE                If the configuration is not saved.
* @returns  L7_FALSE               If the configuration is saved.
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL linkDependencyHasDataChanged( void )
{
  return( LinkDependencyData_g.configChanged );
}

void linkDependencyResetDataChanged( void )
{
  LinkDependencyData_g.configChanged = L7_FALSE;

  return;
}

/*********************************************************************
* @purpose  Updates the state of an interface.
*
* @param    intIfNum     @b{(input)}  the internal interface number
*
* @returns  None
*
* @comments Only updates the state in execute phase
*
* @end
*********************************************************************/
L7_RC_t linkDependencyIntfStateUpdate( L7_uint32 intIfNum )
{
  link_dependency_membership_t *membership    = &LinkDependencyData_g.membership[intIfNum - 1];
  link_dependency_group_t      *group;
  L7_BOOL                       allGroupsDown = L7_TRUE;
  L7_RC_t                       rc            = L7_SUCCESS;
  NIM_HANDLE_t                  handle;
  NIM_EVENT_NOTIFY_INFO_t       eventInfo;
  L7_uint32                     groupId;
  L7_uint32                     adminState;
  L7_uint32                     result;

  if ( L7_CNFGR_STATE_E != linkDependencyCnfgrPhaseGet() )
  {
    return( L7_FAILURE );
  }

  L7_NONZEROMASK( membership->member, result, L7_LINK_DEPENDENCY_MAX_GROUPS );
  if ( 0 == result )            /* Link not a member of any group */
  {
    allGroupsDown = L7_FALSE;
  }
  else
  {
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
          allGroupsDown = L7_FALSE;
          break;
        }
      }
    }
  }

  eventInfo.component = L7_LINK_DEPENDENCY_COMPONENT_ID;
  eventInfo.pCbFunc   = L7_NULLPTR;
  eventInfo.intIfNum  = intIfNum;

  if ( L7_TRUE == allGroupsDown )
  {
    eventInfo.event = L7_DOWN;
    rc = nimEventIntfNotify(eventInfo, &handle);
    adminState = L7_DISABLE;
    eventInfo.event = L7_PORT_DISABLE;
  }
  else
  {
    rc = nimGetIntfMgmtAdminState( intIfNum, &adminState );
    if ( ( L7_SUCCESS != rc ) ||
         ( L7_ENABLE  != adminState ) )
    {
      return( L7_FAILURE );
    }
    eventInfo.event = L7_PORT_ENABLE;
  }

  if ( L7_TRUE == dot3adIsLag( intIfNum ) )
  {
    /*
     * If interface is a LAG then let dot3ad take care of admin state.
     * dot3ad will eventually call nimSetIntfAdminState to set the admin
     * state for the LAG interface.
     */
    rc = dot3adAdminModeSet( intIfNum, adminState );
  }
  else /* the interface is not a LAG interface */
  {
    rc = nimSetIntfAdminState( intIfNum, adminState );
  }

  return( rc );
}

/*********************************************************************
* @purpose  Updates the state of an group.
*
* @param    groupId      @b{(input)}  the group ID
*
* @returns  None
*
* @comments Only updates the state in execute phase
*
* @end
*********************************************************************/
L7_RC_t linkDependencyGroupStateUpdate( L7_uint32 groupId )
{
  link_dependency_group_t *group = &LinkDependencyData_g.groups[groupId - 1];
  L7_uint32                intIfNum;
  L7_uint32                i;
  L7_INTF_MASK_t          *dependencies;
  L7_INTF_MASK_t          *status;
  L7_BOOL                  linksDown = L7_TRUE;
  L7_uint32                result;

  if ( L7_CNFGR_STATE_E != linkDependencyCnfgrPhaseGet() )
  {
    return( L7_FAILURE );
  }

  if ( L7_FALSE == group->inUse )
  {
    return( L7_SUCCESS );
  }

  dependencies = &group->dependencies;
  status       = &LinkDependencyData_g.intfStatus;

  L7_INTF_NONZEROMASK( group->dependencies, result );
  if ( 0 == result )            /* Link not a member of any group */
  {
    linksDown = L7_FALSE;
  }
  else
  {
    for ( i = 0; i < L7_INTF_INDICES; i++ )
    {
      if ( 0 != ( dependencies->value[i] & status->value[i] ) )
      {
        linksDown = L7_FALSE;
        break;
      }
    }
  }
  if ( linksDown != group->allLinksDown )
  {
    group->allLinksDown = linksDown;

    for ( intIfNum = 1; intIfNum <= L7_MAX_INTERFACE_COUNT; intIfNum++ )
    {
      if ( 0 != L7_INTF_ISMASKBITSET( group->members, intIfNum ) )
      {
        linkDependencyIntfStateUpdate( intIfNum );
      }
    }
  }

  return( L7_SUCCESS );
}

/*********************************************************************
* @purpose  Handle L7_UP/L7_DOWN events from NIM.
*
* @param    intIfNum     @b{(input)}  the internal interface number
* @param    event        @b{(input)}  the interface event
* @param    correlator   @b{(input)}  the correlator used by NIM for tallying
*
* @returns  L7_SUCCESS             Always, but ignored by NIM
*
* @comments This is where the main work of this component is performed.
*
* @notes    This function runs in NIM's thread. This thread MUST NOT be blocked!
*
* @end
*********************************************************************/
L7_uint32 linkDependencyIntfChangeCallBack( L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator )
{
  NIM_EVENT_COMPLETE_INFO_t     status;
  L7_uint32                     linkState;
  L7_uint32                     groupId;
  L7_BOOL                       intfChanged = L7_FALSE;
  L7_RC_t                       rc          = L7_SUCCESS;
  link_dependency_membership_t *membership;

  status.intIfNum   = intIfNum;
  status.component  = L7_LINK_DEPENDENCY_COMPONENT_ID;
  status.event      = event;
  status.correlator = correlator;

  if ( L7_TRUE == linkDependencyIsValidIntf( intIfNum ) )
  {
    /*
     * Can't wait forever on this semaphore since the wait blocks NIM.
     */
    if ( L7_SUCCESS != osapiSemaTake( LinkDependencySemaphore, L7_LINK_DEPENDENCY_SEMAPHORE_WAIT ) )
    {
      return( L7_FAILURE );
    }

    if ( 0 == L7_INTF_ISMASKBITSET( LinkDependencyData_g.intfStatus, intIfNum ) )
    {
      linkState = L7_DOWN;
    }
    else
    {
      linkState = L7_UP;
    }

    switch (event)
    {
      case L7_UP:
        if ( linkState != event )
        {
          L7_INTF_SETMASKBIT( LinkDependencyData_g.intfStatus, intIfNum );
          intfChanged = L7_TRUE;
        }

        break;

      case L7_DOWN:
        if ( linkState != event )
        {
          L7_INTF_CLRMASKBIT( LinkDependencyData_g.intfStatus, intIfNum );
          intfChanged = L7_TRUE;
        }

        break;

      default:
        /* fall through */
        break;
    } /* switch */

    if ( L7_TRUE == intfChanged )
    {
      membership = &LinkDependencyData_g.membership[intIfNum - 1];

      for ( groupId  = L7_LINK_DEPENDENCY_FIRST_GROUP;
            groupId <= L7_LINK_DEPENDENCY_LAST_GROUP;
            groupId++ )
      {
        if ( 0 != L7_ISMASKBITSET( membership->dependency, groupId ) )
        {
          linkDependencyGroupStateUpdate( groupId );
        }
      }
    }

    osapiSemaGive( LinkDependencySemaphore );
  }

  /* tell NIM that we are done processing the event */
  status.response.rc  = rc;
  nimEventStatusCallback(status);

  return( rc );
}

void linkDependencyMaskDump( L7_mask_values_t *mask, L7_uint32 size )
{
  L7_uint32 i;
  L7_uint32 result;
  L7_BOOL   firstOne = L7_TRUE;

  L7_NONZEROMASK( mask, result, size );
  if ( 0 == result )
  {
    printf( "\tNone\r\n" );
  }
  else
  {
    printf( "\t" );
    for ( i = 1; i <= size; i++ )
    {
      if ( 0 != L7_ISMASKBITSET( mask, i ) )
      {
        if ( L7_FALSE == firstOne )
        {
          printf( "," );
        }
        else
        {
          firstOne = L7_FALSE;
        }
        printf( "%d", i );
      }
    }
  }
}

void linkDependencyGroupDump( L7_uint32 groupId )
{
  link_dependency_group_t *group = &LinkDependencyData_g.groups[groupId - 1];

  printf( "Group ID = %d\r\n", groupId );
  printf( "\tinUse = %d\r\n", group->inUse );
  printf( "\tallLinksDown = %d\r\n", group->allLinksDown );

  printf( "Group members:\r\n" );
  linkDependencyMaskDump( group->members.value,      L7_MAX_INTERFACE_COUNT );
  printf( "\r\nGroup depends on:\r\n" );
  linkDependencyMaskDump( group->dependencies.value, L7_MAX_INTERFACE_COUNT );
  printf( "\r\n" );
}

void linkDependencyMembershipDump( L7_uint32 intIfNum )
{
  link_dependency_membership_t *membership = &LinkDependencyData_g.membership[intIfNum - 1];

  printf( "Interface number = %d\r\n", intIfNum );
  printf( "Member of groups:\r\n" );
  linkDependencyMaskDump( membership->member,     L7_LINK_DEPENDENCY_MAX_GROUPS );
  printf( "\r\nGroups which depend on:\r\n" );
  linkDependencyMaskDump( membership->dependency, L7_LINK_DEPENDENCY_MAX_GROUPS );
  printf( "\r\n" );
}
