/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_l2_vlan.c
*
* @purpose   This file contains the entry points to the Layer 2 interface
*
* @component hapi
*
* @comments
*
* @create    2/22/2001
*
* @author    nsummers
*
* @end
*
**********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "osapi.h"
#include "sysapi.h"
#include "sysbrds.h"
#include "dapi.h"
#include "log.h"
#include "dapi_struct.h"
#include "l7_usl_bcmx_l2.h"
#include "comm_mask.h"
#include "dot1s_exports.h"

#include "broad_common.h"
#include "broad_system.h"
#include "broad_l2_vlan.h"
#include "broad_l2_mcast.h"
#include "broad_l2_lag.h"
#include "broad_policy.h"
#include "broad_cos_util.h"
#include "broad_l3.h"
#include "l7_usl_bcmx_port.h"

#include "bcm/vlan.h"
#include "bcm/cosq.h"
#include "soc/drv.h"
#include "bcm/types.h"

#include "bcmx/lport.h"
#include "bcmx/lplist.h"
#include "bcmx/vlan.h"
#include "bcmx/port.h"
#include "bcmx/cosq.h"
#include "bcmx/l2.h"
#include "bcmx/l3.h"
/* PTin removed: SDK 6.3.0 */
#include "ptin_globaldefs.h"
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
/* No include */
#else
#include "bcmx/filter.h"
#endif
#include "bcmx/bcmx_int.h"

#ifdef L7_MCAST_PACKAGE
#include "broad_l3_mcast.h"
#endif

#ifdef L7_WIRELESS_PACKAGE
#include "broad_l2_tunnel.h"
#endif

static L7_VLAN_MASK_t vlanExists;

/* synchronization sema to obtain the driver VLAN table */
extern void  *broadVlanSema;

/* synchronization sema for COSQ policies */
extern void  *broadCOSQsema;

/*********************************************************************
*
* @purpose Add the specified port to the hardware VLAN configuration.
*
* @param hapiPortPtr - Port to add to the VLAN.
* @param vlanId - Requested VLAN id.
* @param is_tagged - Flag set to one if the VLAN is tagged on the port.
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadL2VlanReInit (void)
{
}
                            
/*********************************************************************
*
* @purpose Add the specified port to the hardware VLAN configuration.
*
* @param hapiPortPtr - Port to add to the VLAN.
* @param vlanId - Requested VLAN id.
* @param is_tagged - L7_TRUE: Vlan is tagged on this port
*                    L7_FALSE: Vlan is not tagged on this port
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadL2VlanAddPortToVlanHw (DAPI_USP_t *usp,
                                     L7_ushort16 vlanId,
                                     L7_BOOL   is_tagged,
                                     DAPI_t      *dapi_g)
{
  BROAD_PORT_t     *hapiPortPtr;
  L7_VLAN_MASK_t    vlanMemberSet, vlanTaggedSet;

  hapiPortPtr      = HAPI_PORT_GET(usp,dapi_g);

  /* This function only handles physical ports. */
  if (hapiPortPtr->port_is_lag == L7_TRUE)
  {
    LOG_ERROR (vlanId);
  }

  memset(&vlanMemberSet, 0, sizeof(vlanMemberSet));
  memset(&vlanTaggedSet, 0, sizeof(vlanTaggedSet));

  L7_VLAN_SETMASKBIT(vlanMemberSet, vlanId);
  if (is_tagged != L7_FALSE)
    L7_VLAN_SETMASKBIT(vlanTaggedSet, vlanId);

  BROAD_HW_VLAN_MEMBER_SET(usp,vlanId,dapi_g);
  if (is_tagged != L7_FALSE)
    BROAD_HW_VLAN_TAGGING_SET(usp,vlanId,dapi_g);

  hapiBroadL2VlanSetPortToVlanListHw(usp, L7_TRUE, &vlanMemberSet, &vlanTaggedSet, dapi_g);
  
  hapiBroadL3PortVlanNotify(usp, vlanId, 0, dapi_g);

#ifdef L7_MCAST_PACKAGE
  /* Notify the IP multicast routing component that a port has been added 
  ** to a VLAN.
  */
  hapiBroadL3McastPortVlanAddNotify (usp, vlanId, dapi_g);
#endif


#ifdef L7_WIRELESS_PACKAGE
  /* Notify the L2 tunnel initiators about tagging changes on a physical port 
  */
  hapiBroadL2TunnelVlanAddNotify (usp, vlanId, is_tagged, dapi_g);
#endif
}

/*********************************************************************
*
* @purpose Remove the specified port from the hardware VLAN configuration.
*
* @param hapiPortPtr - Port to remove from the VLAN.
* @param vlanId - Requested VLAN id.
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadL2VlanRemovePortFromVlanHw (DAPI_USP_t *usp,
                                          L7_ushort16 vlanId,
                                          DAPI_t *dapi_g)
{
  BROAD_PORT_t *hapiPortPtr;
  L7_VLAN_MASK_t    vlanMemberSet, vlanTaggedSet;

  memset(&vlanMemberSet, 0, sizeof(vlanMemberSet));
  memset(&vlanTaggedSet, 0, sizeof(vlanTaggedSet));

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  /* This function only handles physical ports.
   */
  if (hapiPortPtr->port_is_lag == L7_TRUE)
  {
    LOG_ERROR (vlanId);
  }

  hapiBroadL3PortVlanNotify(usp, vlanId, 1, dapi_g);

#ifdef L7_MCAST_PACKAGE
  /* Notify the IP multicast routing component that a port is about to be removed
  ** from a VLAN.
  */
  hapiBroadL3McastPortVlanRemoveNotify (usp, vlanId, dapi_g);
#endif

  BROAD_HW_VLAN_MEMBER_CLEAR(usp,vlanId,dapi_g);
  BROAD_HW_VLAN_TAGGING_CLEAR(usp,vlanId,dapi_g);

  L7_VLAN_SETMASKBIT(vlanMemberSet, vlanId);
  
  hapiBroadL2VlanSetPortToVlanListHw(usp, L7_FALSE, &vlanMemberSet, &vlanTaggedSet, dapi_g);

}

/*********************************************************************
*
* @purpose Add or remove specified port from hardware VLAN membership.
*
* @param usp
* @param port_cmd    - 1 - Add port to the hardware VLAN membership
*            0 - Remove port from the hardware VLAN membership.
* @param dapi_g
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadAddRemovePortFromVlans (DAPI_USP_t *usp,
                                      L7_uint32   port_cmd,
                                      DAPI_t     *dapi_g)
{
  L7_ushort16   vlan_id;
  L7_BOOL       tag;
  BROAD_PORT_t *hapiPortPtr;
  usl_bcm_port_vlan_t port_command;
  int rv;

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  memset (&port_command, 0, sizeof (usl_bcm_port_vlan_t));

  /* Figure out to which VLANs the port needs to be added or removed.
  */
  for (vlan_id = 1; vlan_id <= L7_PLATFORM_MAX_VLAN_ID; vlan_id++)
  {
    if (BROAD_IS_VLAN_MEMBER(usp, vlan_id, dapi_g))
    {
      L7_VLAN_SETMASKBIT (port_command.vlan_membership_mask, vlan_id);
      if (BROAD_IS_VLAN_TAGGING (usp, vlan_id, dapi_g))
      {
        L7_VLAN_SETMASKBIT (port_command.vlan_tag_mask, vlan_id);
      }
    }
  }

  /* Port are added to the VLAN before notifications are sent.
  */
  if (port_cmd == 1)
  {
    rv = usl_bcmx_port_vlan_member_add (hapiPortPtr->bcmx_lport, &port_command);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      LOG_ERROR (rv);
    }
  }


  /* Send add/delete notifications.
  */
  for (vlan_id = 1; vlan_id <= L7_PLATFORM_MAX_VLAN_ID; vlan_id++)
  {
    if (BROAD_IS_VLAN_MEMBER(usp, vlan_id, dapi_g))
    {
      if (port_cmd == 1)
      {
        BROAD_HW_VLAN_MEMBER_SET(usp,vlan_id,dapi_g);
        if (BROAD_IS_VLAN_TAGGING(usp, vlan_id, dapi_g))
        {
          tag = L7_TRUE;
        }
        else
        {
          tag = L7_FALSE;
        }

        if (tag)
        {
          BROAD_HW_VLAN_TAGGING_SET(usp,vlan_id,dapi_g);
        }


#ifdef L7_MCAST_PACKAGE
        /* Notify the IP multicast routing component that a port has been added 
        ** to a VLAN.
        */
        hapiBroadL3McastPortVlanAddNotify (usp, vlan_id, dapi_g);
#endif

        hapiBroadMgmPortVlanAddNotify (usp, vlan_id, dapi_g);

        hapiBroadL3PortVlanNotify(usp, vlan_id, 0, dapi_g);
#ifdef L7_WIRELESS_PACKAGE
        /* Notify the L2 tunnel initiators about tagging changes on a physical port 
        */
        hapiBroadL2TunnelVlanAddNotify (usp, vlan_id, tag, dapi_g);
#endif
      }
      else
      {
        hapiBroadMgmPortVlanRemoveNotify (usp, vlan_id, dapi_g);

        hapiBroadL3PortVlanNotify(usp, vlan_id, 1, dapi_g);

#ifdef L7_MCAST_PACKAGE
        /* Notify the IP multicast routing component that a port is about to be removed
        ** from a VLAN.
        */
        hapiBroadL3McastPortVlanRemoveNotify (usp, vlan_id, dapi_g);
#endif
        BROAD_HW_VLAN_MEMBER_CLEAR(usp,vlan_id,dapi_g);
        BROAD_HW_VLAN_TAGGING_CLEAR(usp,vlan_id,dapi_g);
      }
    }
  }

  /* Ports are deleted from the VLAN after notifications are done.
  */
  if (port_cmd == 0)
  {
    rv = usl_bcmx_port_vlan_member_delete (hapiPortPtr->bcmx_lport, &port_command);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      LOG_ERROR (rv);
    }
  }

}


/*********************************************************************
*
* @purpose Hook in the Vlan functionality and create any data
*
* @param   DAPI_PORT_t *dapiPortPtr - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2VlanPortInit(DAPI_PORT_t *dapiPortPtr)
{

  L7_RC_t result = L7_SUCCESS;

  dapiPortPtr->cmdTable[DAPI_CMD_QVLAN_VLAN_CREATE                 ] = (HAPICTLFUNCPTR_t)hapiBroadQvlanVlanCreate;
  dapiPortPtr->cmdTable[DAPI_CMD_QVLAN_VLAN_LIST_CREATE            ] = (HAPICTLFUNCPTR_t)hapiBroadQvlanVlanListCreate;
  dapiPortPtr->cmdTable[DAPI_CMD_QVLAN_VLAN_LIST_PURGE             ] = (HAPICTLFUNCPTR_t)hapiBroadQvlanVlanListPurge;
  dapiPortPtr->cmdTable[DAPI_CMD_QVLAN_VLAN_PORT_ADD               ] = (HAPICTLFUNCPTR_t)hapiBroadQvlanVlanPortAdd;
  dapiPortPtr->cmdTable[DAPI_CMD_QVLAN_VLAN_PORT_DELETE            ] = (HAPICTLFUNCPTR_t)hapiBroadQvlanVlanPortDelete;
  dapiPortPtr->cmdTable[DAPI_CMD_QVLAN_VLAN_PURGE                  ] = (HAPICTLFUNCPTR_t)hapiBroadQvlanVlanPurge;
  dapiPortPtr->cmdTable[DAPI_CMD_QVLAN_PORT_VLAN_LIST_SET          ] = (HAPICTLFUNCPTR_t)hapiBroadQvlanPortVlanListSet;
  dapiPortPtr->cmdTable[DAPI_CMD_QVLAN_PORT_CONFIG                 ] = (HAPICTLFUNCPTR_t)hapiBroadQvlanPortConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_QVLAN_PBVLAN_CONFIG               ] = (HAPICTLFUNCPTR_t)hapiBroadQvlanPbvlanConfig;
  dapiPortPtr->cmdTable[DAPI_CMD_QVLAN_PORT_PRIORITY               ] = (HAPICTLFUNCPTR_t)hapiBroadQvlanPortPriority;
  dapiPortPtr->cmdTable[DAPI_CMD_QVLAN_PORT_PRIORITY_TO_TC_MAP     ] = (HAPICTLFUNCPTR_t)hapiBroadQvlanPortPriorityToTcMap;

  dapiPortPtr->cmdTable[DAPI_CMD_GARP_GVRP                         ] = (HAPICTLFUNCPTR_t)hapiBroadGarpGvrp;
  dapiPortPtr->cmdTable[DAPI_CMD_QVLAN_MCAST_FLOOD_SET             ] = (HAPICTLFUNCPTR_t)hapiBroadQvlanMcastFloodModeSet;

  return result;

}   


/*********************************************************************
*
* @purpose Sets the acceptable frame type for the specified port.
*
* @param   DAPI_USP_t   *usp
* @param   L7_BOOL       acceptAll
* @param   DAPI_t       *dapi_g
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPortAcceptableFrameTypeSet(DAPI_USP_t *usp, L7_uint32 acceptFrameType, DAPI_t *dapi_g)
{
  DAPI_PORT_t                     *dapiPortPtr;
  BROAD_PORT_t                    *hapiPortPtr;
  L7_int32                         rv;
  bcm_port_discard_t               mode;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  switch (acceptFrameType)
  {
    case L7_DOT1Q_ADMIT_ALL:
        mode = BCM_PORT_DISCARD_NONE;
        break;
    case L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED:
        mode = BCM_PORT_DISCARD_UNTAG;
        break;
    case L7_DOT1Q_ADMIN_ONLY_VLAN_UNTAGGED:
        mode = BCM_PORT_DISCARD_TAG;
        break;
    default:
        mode = BCM_PORT_DISCARD_ALL;
        break;
      
  }

  if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
  {
    int i;

    hapiBroadLagCritSecEnter ();
    for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
    {
      if (dapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
      {
        BROAD_PORT_t   *hapiLagMemberPortPtr;

        hapiLagMemberPortPtr = HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[i].usp, dapi_g);

        rv = usl_bcmx_port_discard_set(hapiLagMemberPortPtr->bcmx_lport, mode);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          LOG_ERROR(rv);
        }
      }
    }
    hapiBroadLagCritSecExit();
  }
  else
  {
    /* Defer configuration for acquired ports. */
    if ((dapiPortPtr->modeparm.physical.routerIntfEnabled == L7_FALSE) &&
        (hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_FALSE) &&
        (hapiBroadSystemMirroringUSPIsProbe(usp,dapi_g) == L7_FALSE)) 
    {
      rv = usl_bcmx_port_discard_set(hapiPortPtr->bcmx_lport, mode);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        LOG_ERROR(rv);
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Sets the untagged VLAN (PVID) for the specified port.
*
* @param   DAPI_USP_t   *usp
* @param   L7_ushort16   pvid
* @param   DAPI_t       *dapi_g
*
* @returns L7_RC_t result
*
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPortUntaggedVlanSet(DAPI_USP_t *usp, L7_ushort16 pvid, DAPI_t *dapi_g)
{
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  L7_int32      rv;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);


  if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
  {
    int i;

    for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
    {
      if (dapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
      {
        BROAD_PORT_t   *hapiLagMemberPortPtr;

        hapiLagMemberPortPtr = HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[i].usp, dapi_g);

        rv = usl_bcmx_port_untagged_vlan_set(hapiLagMemberPortPtr->bcmx_lport,pvid);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          LOG_ERROR(pvid);
        }

      }
    }
  }
  else
  {
    /* Defer PVID configuration for acquired ports. */
    if ((dapiPortPtr->modeparm.physical.routerIntfEnabled == L7_FALSE) &&
        (hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_FALSE) &&
        (hapiBroadSystemMirroringUSPIsProbe(usp,dapi_g) == L7_FALSE)) 
    {

      rv = usl_bcmx_port_untagged_vlan_set(hapiPortPtr->bcmx_lport, pvid);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        LOG_ERROR(pvid);
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Takes the dapiCmd info modifies the NP's table accordingly.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_QVLAN_VLAN_CREATE
* @param   void       *data   - DAPI_QVLAN_MGMT_CMD_t.cmdData.vlanCreate
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQvlanVlanCreate(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                  result = L7_SUCCESS;
  L7_ushort16              vlanID;
  L7_int32                 rv = 0;
  DAPI_USP_t               cpuUsp;
  DAPI_QVLAN_MGMT_CMD_t   *cmdVlanCreate = (DAPI_QVLAN_MGMT_CMD_t*)data;
  BROAD_SYSTEM_t          *hapiSystemPtr;

  if (cmdVlanCreate->cmdData.vlanCreate.getOrSet == DAPI_CMD_GET)
  {
    result = L7_FAILURE;
    return result;
  }

  hapiSystemPtr = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;
  vlanID =  cmdVlanCreate->cmdData.vlanCreate.vlanID;

  rv = usl_bcmx_vlan_create(vlanID);
  if (vlanID == 1)
  {
    if (!((rv == BCM_E_EXISTS) || (L7_BCMX_OK(rv) == L7_TRUE)))
    {
      LOG_ERROR (vlanID);
    }
  }
  else if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    LOG_ERROR (vlanID);
  }
  
  /* PTin added: Only add cpu ports for system vlans */
  if (vlanID==1 || vlanID>=4094)
  {
    /* Management cpu port is added when the vlan is created. Update the Hw 
    *  Vlan member mask
    */
    if (CPU_USP_GET(&cpuUsp) == L7_FAILURE)
    {
      LOG_ERROR (vlanID);
    }
      
    /* Add all the local CPUs to the vlan */
    for (cpuUsp.port=0; cpuUsp.port < dapi_g->unit[cpuUsp.unit]->slot[cpuUsp.slot]->numOfPortsInSlot; cpuUsp.port++)
    {
      BROAD_HW_VLAN_MEMBER_SET(&cpuUsp,vlanID,dapi_g);
    }
  }

  L7_VLAN_SETMASKBIT(vlanExists, vlanID);

  /* The new VLAN is automatically assigned to the 
  ** default spanning tree instance.
  */
  hapiSystemPtr->stg_instNumber[vlanID] = DOT1S_CIST_ID;
 
  return result;
}

/*********************************************************************
*
* @purpose Create a list of vlans in the hardware.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_QVLAN_VLAN_CREATE
* @param   void       *data   - DAPI_QVLAN_MGMT_CMD_t.cmdData.vlanCreate
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQvlanVlanListCreate(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                  result        = L7_SUCCESS;
  DAPI_QVLAN_MGMT_CMD_t   *cmdVlanCreate = (DAPI_QVLAN_MGMT_CMD_t*)data;
  L7_int32                 rv = 0;
  DAPI_USP_t               cpuUsp;
  L7_ushort16              vlanID = 0;
  BROAD_SYSTEM_t          *hapiSystemPtr;
  L7_uchar8                tmpVlanMask, tmpVlanFailureMask;
  L7_ushort16              vidx = 0, pos = 0, numVlans = 0;
  L7_uint32                hapiL2VlanFailureCount = 0;
  L7_BOOL                  bulkCmd;
  L7_VLAN_MASK_t           hapiL2VlanMaskFailure;

  hapiSystemPtr = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;


  if (cmdVlanCreate->cmdData.vlanCreateList.getOrSet == DAPI_CMD_GET)
  {
    result = L7_FAILURE;
    return result;
  }

  numVlans = cmdVlanCreate->cmdData.vlanCreateList.numVlans;

  memset(cmdVlanCreate->cmdData.vlanCreateList.vlanMaskFailure, 0, sizeof(L7_VLAN_MASK_t));
  memset(cmdVlanCreate->cmdData.vlanCreateList.vlanFailureCount, 0, sizeof(L7_uint32));

  bulkCmd = L7_TRUE; /* Create the vlans in the hardware */
  rv = usl_bcmx_vlan_bulk_configure(bulkCmd,
                                    &(cmdVlanCreate->cmdData.vlanCreateList.vlanMask),
                                    cmdVlanCreate->cmdData.vlanCreateList.numVlans, 
                                    cmdVlanCreate->cmdData.vlanCreateList.vlanMaskFailure, 
                                    cmdVlanCreate->cmdData.vlanCreateList.vlanFailureCount 
                                   );
  if (rv != BCM_E_NONE) 
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_HAPI_ERROR,
                  "Vlan bulk cmd %d failed in %s:%d rv %d\n", 
                  bulkCmd, __FUNCTION__, __LINE__, rv
                 );
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID,
            "Vlan bulk cmd %d failed rv %d\n", 
             bulkCmd, rv
           );
    result = L7_FAILURE;
              
  }
  
  /* If there are any failures, make sure the failed vlans are deleted from all the units. */
  if (*(cmdVlanCreate->cmdData.vlanCreateList.vlanFailureCount) > 0) 
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF(SYSAPI_LOGGING_HAPI_ERROR,
                  "Driver failed to create %d vlans in the hardware\n", 
                  *(cmdVlanCreate->cmdData.vlanCreateList.vlanFailureCount)
                 );
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID,
            "Driver failed to create %d vlans in the hardware\n", 
            *(cmdVlanCreate->cmdData.vlanCreateList.vlanFailureCount)
           );
     
    bulkCmd = L7_FALSE; /* Delete the vlans from the hardware */
    memset(&hapiL2VlanMaskFailure, 0, sizeof(L7_VLAN_MASK_t));
    rv = usl_bcmx_vlan_bulk_configure(bulkCmd,
                                      cmdVlanCreate->cmdData.vlanCreateList.vlanMaskFailure, 
                                      *(cmdVlanCreate->cmdData.vlanCreateList.vlanFailureCount),
                                      &hapiL2VlanMaskFailure,
                                      &hapiL2VlanFailureCount
                                     );
    if (rv != BCM_E_NONE) 
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_HAPI_ERROR,
                    "Vlan bulk cmd %d failed in %s:%d rv %d\n", 
                    bulkCmd, __FUNCTION__, __LINE__, rv
                   );
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID,
              "Vlan bulk cmd %d failed rv %d\n", 
              bulkCmd, rv
             );
      result = L7_FAILURE;
    }
    /* Ignoring the failures on deletion */
  }

  /* Create an lplist of the cpu USPs. */
  if (CPU_USP_GET(&cpuUsp) == L7_FAILURE)
  {
    LOG_ERROR (vlanID);
  }
    
    
  /* Walk through the application vlan mask and update the HAPI data-structures for 
     vlans successfully created in the hardware. */
  for (vidx = 0; vidx < L7_VLAN_INDICES; vidx++) 
  {
	pos = 0;
	tmpVlanMask = cmdVlanCreate->cmdData.vlanCreateList.vlanMask.value[vidx];
	tmpVlanFailureMask = cmdVlanCreate->cmdData.vlanCreateList.vlanMaskFailure->value[vidx];
	while (tmpVlanMask) 
	{
	  pos++;
        
	  if ((tmpVlanMask & 0x1) &&
          (!(tmpVlanFailureMask & 0x1)) /* If vlan was created in the hardware */
          )
	  {
	    vlanID = (vidx*8) + pos;
            L7_VLAN_SETMASKBIT(vlanExists, vlanID);

            /* The new VLAN is automatically assigned to the 
            ** default spanning tree instance.
            */
            hapiSystemPtr->stg_instNumber[vlanID] = DOT1S_CIST_ID;

            /* Add all the local CPUs to the vlan */
            for (cpuUsp.port=0; cpuUsp.port < dapi_g->unit[cpuUsp.unit]->slot[cpuUsp.slot]->numOfPortsInSlot; cpuUsp.port++)
            {
              BROAD_HW_VLAN_MEMBER_SET(&cpuUsp,vlanID,dapi_g);
            }

            numVlans--;
	  }

          tmpVlanMask >>= 1;
          tmpVlanFailureMask >>= 1;
        }

	if (numVlans == 0) 
	{
	  break;
	}
  }

  return result;
}



/*********************************************************************
*
* @purpose Takes the dapiCmd info modifies the NP's table accordingly.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_QVLAN_VLAN_PORT_ADD
* @param   void       *data   - DAPI_QVLAN_MGMT_CMD_t.cmdData.vlanPortAdd
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQvlanVlanPortAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                             result         = L7_SUCCESS;
  DAPI_QVLAN_MGMT_CMD_t              *cmdVlanPortAdd = (DAPI_QVLAN_MGMT_CMD_t*)data;
  DAPI_PORT_t                        *dapiPortPtr;
  BROAD_PORT_t                       *hapiPortPtr;
  DAPI_USP_t                          memberUsp;
  L7_ushort16                         entry;
  L7_ushort16                         vlanId;
  L7_BOOL                             tagged = L7_FALSE;

  /* 
   * protect the VLAN database in case any other components are accessing the data
   * CAUTION: early returns must exit the crit sec 
   */
  hapiBroadVlanCritSecEnter();

#ifdef L7_MCAST_PACKAGE
  /* Acquire the semaphore for L3 mcast. The L3 Mcast code is dependent on
     VLAN membership when setting the replication info for an IPMC group.
     Since this routine can modify VLAN membership of a port,
     we need to ensure that the work here is done before
     the L3 MC code attempts to modify any IPMC group membership. */
  hapiBroadL3McastCritEnter();
#endif

  vlanId = cmdVlanPortAdd->cmdData.vlanPortAdd.vlanID;

  /* First go through the tagged list and set tagging on specified ports.
  ** We look at tagging first to optimize performance during boot time. 
  ** We need to avoid setting ports to untagged than flipping them to tagged
  ** because this requires extra RPC calls as well as a call to bcmx_l3_untagged_update().
  */
  for (entry=0; entry < cmdVlanPortAdd->cmdData.vlanPortAdd.numOfTagSetEntries; entry++)
  {
    memberUsp.unit = cmdVlanPortAdd->cmdData.vlanPortAdd.operationalTagSet[entry].unit;
    memberUsp.slot = cmdVlanPortAdd->cmdData.vlanPortAdd.operationalTagSet[entry].slot;
    memberUsp.port = cmdVlanPortAdd->cmdData.vlanPortAdd.operationalTagSet[entry].port;

    dapiPortPtr = dapi_g->unit[memberUsp.unit]->slot[memberUsp.slot]->port[memberUsp.port];

    /* We can only add/delete/modify physical ports and LAGs.
    */
    if ((IS_PORT_TYPE_PHYSICAL   (dapiPortPtr) == L7_FALSE) &&
        (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_FALSE) &&
        (IS_PORT_TYPE_CAPWAP_TUNNEL(dapiPortPtr) == L7_FALSE))
      continue;

    hapiPortPtr = dapi_g->unit[memberUsp.unit]->slot[memberUsp.slot]->port[memberUsp.port]->hapiPort;

    BROAD_VLAN_TAGGING_SET (&memberUsp, vlanId, dapi_g);

    /* If port-based routing is enabled on this port or the port is a LAG member
    ** then don't modify the hardware VLAN membership.
    */
    if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr)                == L7_TRUE) &&
        ((hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_TRUE) ||
         (dapiPortPtr->modeparm.physical.routerIntfEnabled == L7_TRUE)))
    {
      continue;
    }

    if (BROAD_IS_VLAN_MEMBER(&memberUsp, vlanId, dapi_g))
    {
      if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
      {
        hapiBroadLagAddLagToVlanHw (&memberUsp, vlanId, dapi_g);
      }
      else
      {
        hapiBroadL2VlanAddPortToVlanHw (&memberUsp,
                                        vlanId, 
                                        L7_TRUE,
                                        dapi_g);
      }

      /* make sure all L2 mcast entries use correct tagging */
      hapiBroadMgmTaggingEnable(&memberUsp, vlanId, dapi_g);
    }
  }


  for (entry=0; entry < cmdVlanPortAdd->cmdData.vlanPortAdd.numOfMemberSetEntries; entry++)
  {
    memberUsp.unit = cmdVlanPortAdd->cmdData.vlanPortAdd.operationalMemberSet[entry].unit;
    memberUsp.slot = cmdVlanPortAdd->cmdData.vlanPortAdd.operationalMemberSet[entry].slot;
    memberUsp.port = cmdVlanPortAdd->cmdData.vlanPortAdd.operationalMemberSet[entry].port;

    dapiPortPtr = dapi_g->unit[memberUsp.unit]->slot[memberUsp.slot]->port[memberUsp.port];

    /* We can only add/delete/modify physical ports and LAGs.
    */
    if ((IS_PORT_TYPE_PHYSICAL   (dapiPortPtr) == L7_FALSE) &&
        (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_FALSE) &&
        (IS_PORT_TYPE_CAPWAP_TUNNEL(dapiPortPtr) == L7_FALSE))
      continue;

    hapiPortPtr = dapi_g->unit[memberUsp.unit]->slot[memberUsp.slot]->port[memberUsp.port]->hapiPort;

    BROAD_VLAN_MEMBER_SET (&memberUsp, vlanId, dapi_g);

    /* If port-based routing is enabled on this port or the port is a LAG member
    ** then don't modify the hardware VLAN membership.
    */
    if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr)                == L7_TRUE) &&
        ((hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_TRUE) ||
         (dapiPortPtr->modeparm.physical.routerIntfEnabled == L7_TRUE)))
    {
      continue;
    }

    if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
    {
      hapiBroadLagAddLagToVlanHw (&memberUsp, vlanId, dapi_g);
    }
    else
    {
      if (BROAD_IS_VLAN_TAGGING(&memberUsp, vlanId, dapi_g))
      {
        tagged = L7_TRUE;
      }
      else
      {
        tagged = L7_FALSE;
      }

      hapiBroadL2VlanAddPortToVlanHw (&memberUsp,
                                      vlanId,
                                      tagged,
                                      dapi_g);
    }

    /* Notify multicast group manager that a port or a LAG has been added
    ** to the hardware.
    */
    hapiBroadMgmPortVlanAddNotify (&memberUsp, vlanId,dapi_g);

    /* Notify dot1x that a port or a LAG has been added
    ** to the hardware.
    */
    hapiBroadDot1xPortVlanAddNotify (&memberUsp, vlanId,dapi_g);

    hapiBroadL3PortVlanNotify(usp, vlanId, 0, dapi_g);
#ifdef L7_WIRELESS_PACKAGE
    /* Notify the L2 tunnel initiators about tagging changes on a physical port 
    */
    hapiBroadL2TunnelVlanAddNotify (usp, vlanId, tagged, dapi_g);
#endif
  }

#ifdef L7_MCAST_PACKAGE
  hapiBroadL3McastCritExit();
#endif

  hapiBroadVlanCritSecExit();

  return result;   
}



/*********************************************************************
*
* @purpose Takes the dapiCmd info modifies the NP's table accordingly
*          so that a port is deleted from the Vlan.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_QVLAN_VLAN_PORT_DELETE
* @param   void       *data   - DAPI_QVLAN_MGMT_CMD_t.cmdData.vlanPortDelete
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQvlanVlanPortDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t          result         = L7_SUCCESS;
  DAPI_QVLAN_MGMT_CMD_t *cmdVlanPortDelete = (DAPI_QVLAN_MGMT_CMD_t*)data;
  DAPI_PORT_t           *dapiPortPtr;
  BROAD_PORT_t           *hapiPortPtr;
  L7_ushort16            entry;
  DAPI_USP_t             memberUsp;
  L7_ushort16            vlanId;

  /* 
   * protect the VLAN database in case any other components are accessing the data
   * CAUTION: early returns must exit the crit sec 
   */
  hapiBroadVlanCritSecEnter();

#ifdef L7_MCAST_PACKAGE
  /* Acquire the semaphore for L3 mcast. The L3 Mcast code is dependent on
     VLAN membership when setting the replication info for an IPMC group.
     Since this routine can modify VLAN membership of a port,
     we need to ensure that the work here is done before
     the L3 MC code attempts to modify any IPMC group membership. */
  hapiBroadL3McastCritEnter();
#endif

  vlanId = cmdVlanPortDelete->cmdData.vlanPortDelete.vlanID;

  /* The port-delete command is processed before tagging-delete command because
  ** it is very inneficient to set port to untagged and than delete it.
  */
  for (entry=0; entry < cmdVlanPortDelete->cmdData.vlanPortDelete.numOfMemberSetEntries; entry++)
  {
    memberUsp.unit = cmdVlanPortDelete->cmdData.vlanPortDelete.operationalMemberSet[entry].unit;
    memberUsp.slot = cmdVlanPortDelete->cmdData.vlanPortDelete.operationalMemberSet[entry].slot;
    memberUsp.port = cmdVlanPortDelete->cmdData.vlanPortDelete.operationalMemberSet[entry].port;

    /* We can only add/delete/modify physical ports and LAGs.
    */
    if ((memberUsp.slot > L7_MAX_PHYSICAL_SLOTS_PER_UNIT ) &&
        (memberUsp.slot != L7_LAG_SLOT_NUM) &&
        (memberUsp.slot != L7_CAPWAP_TUNNEL_SLOT_NUM))
    {
      continue;
    }

    dapiPortPtr = dapi_g->unit[memberUsp.unit]->slot[memberUsp.slot]->port[memberUsp.port];
    hapiPortPtr = dapi_g->unit[memberUsp.unit]->slot[memberUsp.slot]->port[memberUsp.port]->hapiPort;

    /* Notify multicast group manager that a port or a LAG is about to be
    ** removed from a VLAN.
    */
    hapiBroadMgmPortVlanRemoveNotify (&memberUsp, vlanId, dapi_g);

    /* Notify dot1x that a port or a LAG is about to be
    ** removed from a VLAN.
    */
    hapiBroadDot1xPortVlanRemoveNotify (&memberUsp, vlanId, dapi_g);

    hapiBroadL3PortVlanNotify(usp, vlanId, 1, dapi_g);

    BROAD_VLAN_MEMBER_CLEAR (&memberUsp, vlanId, dapi_g);
    BROAD_VLAN_TAGGING_CLEAR(&memberUsp, vlanId, dapi_g);

    /* If port-based routing is enabled on this port or the port is a LAG member
    ** then don't modify the hardware VLAN membership.
    */
    if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr)                == L7_TRUE) &&
        ((hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_TRUE) ||
         (dapiPortPtr->modeparm.physical.routerIntfEnabled == L7_TRUE)))
    {
      continue;
    }

    if (memberUsp.slot == L7_LAG_SLOT_NUM)
    {
      hapiBroadLagRemoveLagFromVlanHw (&memberUsp, vlanId, dapi_g);
    }
    else
    {
      hapiBroadL2VlanRemovePortFromVlanHw (&memberUsp,
                                           vlanId,
                                           dapi_g);
    }
  }

  /* Go through the tagged list and disable tagging on specified ports.
  */
  for (entry=0; entry < cmdVlanPortDelete->cmdData.vlanPortDelete.numOfTagSetEntries; entry++)
  {
    memberUsp.unit = cmdVlanPortDelete->cmdData.vlanPortDelete.operationalTagSet[entry].unit;
    memberUsp.slot = cmdVlanPortDelete->cmdData.vlanPortDelete.operationalTagSet[entry].slot;
    memberUsp.port = cmdVlanPortDelete->cmdData.vlanPortDelete.operationalTagSet[entry].port;

    /* We can only add/delete/modify physical ports and LAGs.
    */
    if ((memberUsp.slot > L7_MAX_PHYSICAL_SLOTS_PER_UNIT ) &&
        (memberUsp.slot != L7_LAG_SLOT_NUM) &&
        (memberUsp.slot != L7_CAPWAP_TUNNEL_SLOT_NUM))
    {
      continue;
    }

    dapiPortPtr = dapi_g->unit[memberUsp.unit]->slot[memberUsp.slot]->port[memberUsp.port];
    hapiPortPtr = dapi_g->unit[memberUsp.unit]->slot[memberUsp.slot]->port[memberUsp.port]->hapiPort;

    BROAD_VLAN_TAGGING_CLEAR(&memberUsp, vlanId, dapi_g);

    /* If port-based routing is enabled on this port or the port is a LAG member
    ** then don't modify the hardware VLAN membership.
    */
    if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr)                == L7_TRUE) &&
        ((hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_TRUE) ||
         (dapiPortPtr->modeparm.physical.routerIntfEnabled == L7_TRUE)))
    {
      continue;
    }

    if (BROAD_IS_VLAN_MEMBER(&memberUsp, vlanId, dapi_g))
    {
      if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
      {
        hapiBroadLagAddLagToVlanHw (&memberUsp, vlanId, dapi_g);
      }
      else
      {
        hapiBroadL2VlanAddPortToVlanHw (&memberUsp,
                                        vlanId,
                                        L7_FALSE,
                                        dapi_g);
      }

      /* make sure all L2 mcast entries use correct tagging */
      hapiBroadMgmTaggingDisable(&memberUsp, vlanId, dapi_g);
    }

  }

#ifdef L7_MCAST_PACKAGE
  hapiBroadL3McastCritExit();
#endif

  hapiBroadVlanCritSecExit();

  return result;   
}

/*********************************************************************
*
* @purpose Delete all the members (except cpu port) of this vlan if any
*        
*
* @param   vid - Vlan Id 
* @param   dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   Vlan lock must be held by the caller
*
* @end
*
*********************************************************************/
void hapiBroadQvlanVlanMemberPurge(DAPI_USP_t *usp, L7_ushort16 vid, DAPI_t *dapi_g)
{
  DAPI_USP_t             dapiUsp;
  DAPI_USP_t             memberSet[L7_MAX_INTERFACE_COUNT];
  L7_uint32              memberCount = 0;
  DAPI_QVLAN_MGMT_CMD_t  cmdVlanDeletePort;

 /* loop through all physical and LAG ports to determine members of this vlan */
  for (dapiUsp.unit=0;dapiUsp.unit<dapi_g->system->totalNumOfUnits;dapiUsp.unit++)
  {
    for (dapiUsp.slot=0;dapiUsp.slot<dapi_g->unit[dapiUsp.unit]->numOfSlots;dapiUsp.slot++)
    {
      if (dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->cardPresent == L7_TRUE)
      {
        /* loop through physical ports */
        for (dapiUsp.port=0;dapiUsp.port<dapi_g->unit[dapiUsp.unit]->slot[dapiUsp.slot]->numOfPortsInSlot;dapiUsp.port++)
        {
          if (isValidUsp (&dapiUsp, dapi_g) != L7_TRUE)
          {
           continue;
          }

          if (BROAD_IS_VLAN_MEMBER(&dapiUsp, vid, dapi_g))
          {
            memberSet[memberCount] = dapiUsp;
            memberCount++;
          }
        }
      }
    }
  }

  /* if there are members in this VLAN, delete them from the VLAN */
  if (memberCount > 0)
  {
    cmdVlanDeletePort.cmdData.vlanPortDelete.getOrSet = DAPI_CMD_SET;
    cmdVlanDeletePort.cmdData.vlanPortDelete.numOfMemberSetEntries = memberCount;
    cmdVlanDeletePort.cmdData.vlanPortDelete.numOfTagSetEntries = 0;
    cmdVlanDeletePort.cmdData.vlanPortDelete.operationalMemberSet = memberSet;
    cmdVlanDeletePort.cmdData.vlanPortDelete.operationalTagSet = L7_NULL;
    cmdVlanDeletePort.cmdData.vlanPortDelete.vlanID = vid;

    hapiBroadQvlanVlanPortDelete(usp,DAPI_CMD_QVLAN_VLAN_PORT_DELETE,&cmdVlanDeletePort,dapi_g);
  }


  return; 
 
}


/*********************************************************************
*
* @purpose Takes the dapiCmd info modifies the NP's table accordingly
*          so that all entries in a Vlan table are deleted.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_QVLAN_VLAN_PURGE
* @param   void       *data   - DAPI_QVLAN_MGMT_CMD_t.cmdData.vlanPurge
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQvlanVlanPurge(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                   result       = L7_SUCCESS;
  DAPI_QVLAN_MGMT_CMD_t    *cmdVlanPurge = (DAPI_QVLAN_MGMT_CMD_t*)data;
  L7_uint32                 vid;
  BROAD_L2ADDR_FLUSH_t      l2addr_vlan;
  BROAD_SYSTEM_t           *hapiSystemPtr;
  DAPI_USP_t                cpuUsp;
  int                       rv;

  hapiSystemPtr = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  vid = cmdVlanPurge->cmdData.vlanPurge.vlanID;

  if (cmdVlanPurge->cmdData.vlanPurge.getOrSet == DAPI_CMD_GET)
  {
    result = L7_FAILURE;
    return result;
  }

  /* 
   * protect the VLAN database in case any other components are accessing the data
   * CAUTION: early returns must exit the crit sec 
   */
  hapiBroadVlanCritSecEnter();
  
  hapiBroadQvlanVlanMemberPurge(usp, vid, dapi_g);

  /* No need to delete cpu port from hw as all the members are deleted when 
   *  vlan is destroyed. 
   */

  /* Find the first CPU interface */
  if (CPU_USP_GET(&cpuUsp) == L7_FAILURE)
  {
    LOG_ERROR (vid);
  }

  for (cpuUsp.port=0; 
       cpuUsp.port < dapi_g->unit[cpuUsp.unit]->slot[cpuUsp.slot]->numOfPortsInSlot; 
       cpuUsp.port++)
  {
    BROAD_HW_VLAN_MEMBER_CLEAR(&cpuUsp,vid,dapi_g);
    BROAD_HW_VLAN_TAGGING_CLEAR(&cpuUsp,vid,dapi_g);
  }



  /* Fill in the structure */
  l2addr_vlan.bcmx_lport = 0; 
  l2addr_vlan.vlanID = vid;
  l2addr_vlan.flushtype = BROAD_FLUSH_BY_VLAN;
  l2addr_vlan.port_is_lag = L7_FALSE;
  l2addr_vlan.tgid = 0;

  /* Send a message to L2 address flushing task with the vlan info */
  hapiBroadL2FlushRequest(l2addr_vlan);

  L7_VLAN_CLRMASKBIT(vlanExists, vid);

  /* Invalidate the spanning tree instance for the deleted VLAN.
  */
  hapiSystemPtr->stg_instNumber[vid] = -1;

  rv = usl_bcmx_vlan_destroy(vid);
  if (L7_BCMX_OK(rv) != TRUE)
  {
    LOG_ERROR(rv);
  }

  hapiBroadVlanCritSecExit();

  return result;
}


/*********************************************************************
*
* @purpose Purge a list of vlans
*         
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_QVLAN_VLAN_PURGE
* @param   void       *data   - DAPI_QVLAN_MGMT_CMD_t.cmdData.vlanPurge
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQvlanVlanListPurge(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                  result        = L7_SUCCESS;
  DAPI_QVLAN_MGMT_CMD_t   *cmdVlanDestroy = (DAPI_QVLAN_MGMT_CMD_t*)data;
  L7_uchar8                tmpVlanMask;
  L7_ushort16              vlanId = 0, vidx = 0, pos = 0, numVlans = 0;
  L7_uint32                rv = BCM_E_NONE, tmpVlanFailureCount = 0;
  L7_BOOL                  bulkCmd;
  BROAD_L2ADDR_FLUSH_t     l2addr_vlan;
  BROAD_SYSTEM_t          *hapiSystemPtr;
  DAPI_USP_t               cpuUsp;
  L7_VLAN_MASK_t           hapiL2VlanMaskFailure;

  hapiSystemPtr = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  if (cmdVlanDestroy->cmdData.vlanPurgeList.getOrSet == DAPI_CMD_GET)
  {
    result = L7_FAILURE;
    return result;
  }

  numVlans = cmdVlanDestroy->cmdData.vlanPurgeList.numVlans;

  /* Find the first CPU interface */
  if (CPU_USP_GET(&cpuUsp) == L7_FAILURE)
  {
    LOG_ERROR (numVlans);
  }

  hapiBroadVlanCritSecEnter();

  /* Walk through the application vlan mask and clean-up the vlan port membership */
  for (vidx = 0; vidx < L7_VLAN_INDICES; vidx++) 
  {
    pos = 0;
	tmpVlanMask = cmdVlanDestroy->cmdData.vlanPurgeList.vlanMask.value[vidx];
	while (tmpVlanMask) 
	{
	  pos++;
        
	  if (tmpVlanMask & 0x1)
	  {
	    vlanId = (vidx*8) + pos;
        
        /* Purge the vlan membership */     
        hapiBroadQvlanVlanMemberPurge(usp, vlanId, dapi_g);

        for (cpuUsp.port=0; cpuUsp.port < dapi_g->unit[cpuUsp.unit]->slot[cpuUsp.slot]->numOfPortsInSlot; cpuUsp.port++)
        {
          BROAD_HW_VLAN_MEMBER_CLEAR(&cpuUsp,vlanId,dapi_g);
          BROAD_HW_VLAN_TAGGING_CLEAR(&cpuUsp,vlanId,dapi_g);
        }

        /* Fill in the structure */
        l2addr_vlan.bcmx_lport = 0; 
        l2addr_vlan.vlanID = vlanId;
        l2addr_vlan.flushtype = BROAD_FLUSH_BY_VLAN;
        l2addr_vlan.port_is_lag = L7_FALSE;
        l2addr_vlan.tgid = 0;

        /* Send a message to L2 address flushing task with the vlan info */
        hapiBroadL2FlushRequest(l2addr_vlan);
        L7_VLAN_CLRMASKBIT(vlanExists, vlanId);

        /* Invalidate the spanning tree instance for the deleted VLAN.
        */
        hapiSystemPtr->stg_instNumber[vlanId] = -1;

		numVlans--;
      }

      tmpVlanMask >>= 1;
    }

	if (numVlans == 0) 
	{
	  break;
	}
  }


  bulkCmd = L7_FALSE; /* Delete the vlans from the hardware */
  memset(&hapiL2VlanMaskFailure, 0, sizeof(L7_VLAN_MASK_t));
  rv = usl_bcmx_vlan_bulk_configure(bulkCmd,
                                    &(cmdVlanDestroy->cmdData.vlanPurgeList.vlanMask), 
                                    cmdVlanDestroy->cmdData.vlanPurgeList.numVlans,
                                    &hapiL2VlanMaskFailure,
                                    &tmpVlanFailureCount
                                   );
  if (rv != BCM_E_NONE) 
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_HAPI_ERROR,
                  "Vlan bulk cmd %d failed in %s:%d rv %d\n", 
                  bulkCmd, __FUNCTION__, __LINE__, rv
                 );
     L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID,
             "Vlan bulk cmd %d failed rv %d\n", 
             bulkCmd, rv
            );
    result = L7_FAILURE;
  }
  
  hapiBroadVlanCritSecExit();
 
  return result;
}

/*********************************************************************
*
* @purpose Add/Remove a port from the specified Vlan list.
*
* @param usp                  - Pointer to the port USP
* @param vlanCmd              - L7_TRUE:   Add port to list of vlans
*                               L7_FALSE:  Remove port from list of vlans
* @param vlanMemberSet        - List of vlans
* @param vlanTagSet           - List of tagged vlans 
* @param dapi_g               - Driver object 
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadL2VlanSetPortToVlanListHw (DAPI_USP_t      *usp,
                                         L7_BOOL          vlanCmd,
                                         L7_VLAN_MASK_t  *vlanMemberSet,
                                         L7_VLAN_MASK_t  *vlanTagSet,
                                         DAPI_t          *dapi_g)
{
  L7_int32                          rv = BCM_E_NONE;
  BROAD_PORT_t                     *hapiPortPtr;
  usl_bcm_port_vlan_t          port_command;

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  memset (&port_command, 0, sizeof (usl_bcm_port_vlan_t));

  memcpy(&port_command.vlan_membership_mask, vlanMemberSet, sizeof(port_command.vlan_membership_mask));
  memcpy(&port_command.vlan_tag_mask, vlanTagSet, sizeof(port_command.vlan_tag_mask));

  /* Add a port to list of vlans */
  if (vlanCmd == L7_TRUE) 
  {
    rv = usl_bcmx_port_vlan_member_add (hapiPortPtr->bcmx_lport, &port_command);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      LOG_ERROR (rv);
    } 
  }
  else /* Remove the port from list of vlans */
  {
    rv = usl_bcmx_port_vlan_member_delete (hapiPortPtr->bcmx_lport, &port_command);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      LOG_ERROR (rv);
    }
  }
}

/*********************************************************************
*
* @purpose To configure vlan membership for a port.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_QVLAN_PORT_VLAN_LIST_SET
* @param   void       *data   - DAPI_QVLAN_MGMT_CMD_t.cmdData.portVlanListSet
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   When DAPI_QVLAN_MGMT_CMD_t.cmdData.portVlanListSet.vlanCmd is L7_TRUE,
*          port is added to the vlans. When the vlanCmd is L7_FALSE, port is 
*          deleted from the vlans. 
*          This dapi command helps in vlan scaling by reducing the number of
*          RPC calls required for updating a port's vlan membership.
*           
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQvlanPortVlanListSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_uchar8                         tmpMemMask, tmpTagMask;
  L7_RC_t                           result = L7_SUCCESS;
  L7_ushort16                       i, k = 0, pos = 0;
  L7_ushort16                       vlanId;
  L7_BOOL                           addedHw = L7_FALSE, remFromHw = L7_TRUE;
  DAPI_QVLAN_MGMT_CMD_t            *cmdPortVlanListSet = (DAPI_QVLAN_MGMT_CMD_t*)data;
  DAPI_PORT_t                      *dapiPortPtr;
  BROAD_PORT_t                     *hapiPortPtr, *hapiLagMemberPortPtr;
  DAPI_LAG_ENTRY_t                 *lagMemberSet;
  L7_BOOL                           makeDot1xOutcall = L7_FALSE;


  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  
  /* We can only add/delete physical ports and LAGs.
  */
  if ((IS_PORT_TYPE_PHYSICAL   (dapiPortPtr) == L7_FALSE) &&
      (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_FALSE) &&
      (IS_PORT_TYPE_CAPWAP_TUNNEL(dapiPortPtr) == L7_FALSE))
    return L7_FAILURE; 



  hapiBroadVlanCritSecEnter();

  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
  {
    hapiBroadDot1xSemTake();
    if ((hapiPortPtr->dot1x.dot1xStatus == L7_DOT1X_PORT_STATUS_UNAUTHORIZED) &&
        (hapiPortPtr->dot1x.authorizedClientCount > 0))
    {
      makeDot1xOutcall = L7_TRUE;
    }
    hapiBroadDot1xSemGive();
  }
  
#ifdef L7_MCAST_PACKAGE
  /* Acquire the semaphore for L3 mcast. The L3 Mcast code is dependent on
     VLAN membership when setting the replication info for an IPMC group.
     Since this routine can modify VLAN membership of a port,
     we need to ensure that the work here is done before
     the L3 MC code attempts to modify any IPMC group membership. */
  hapiBroadL3McastCritEnter();
#endif

  /* Add the port to the list of vlans */
  if (cmdPortVlanListSet->cmdData.portVlanListSet.vlanAddCmd == L7_TRUE) 
  {

    /* If port-based routing is enabled on this port or the port is a LAG member
    ** then don't modify the hardware VLAN membership.
    */
    if (!((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE) &&
        ((hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_TRUE) ||
         (dapiPortPtr->modeparm.physical.routerIntfEnabled == L7_TRUE))))
    {
      addedHw = L7_TRUE;
      if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
      {
        hapiBroadLagSetLagToVlanListHw (usp, L7_TRUE, 
                                        &cmdPortVlanListSet->cmdData.portVlanListSet.operationalMemberSet,
                                        &cmdPortVlanListSet->cmdData.portVlanListSet.operationalTagSet,
                                        dapi_g);
      }
      else
      {
        
         hapiBroadL2VlanSetPortToVlanListHw (usp, L7_TRUE,
                                                &cmdPortVlanListSet->cmdData.portVlanListSet.operationalMemberSet,
                                                &cmdPortVlanListSet->cmdData.portVlanListSet.operationalTagSet,
                                                dapi_g);
      }
    }

    /* Do the outcalls and update the mask*/
    for (k = 0; k < L7_VLAN_INDICES; k++) 
    {
      pos = 0;
      tmpMemMask = cmdPortVlanListSet->cmdData.portVlanListSet.operationalMemberSet.value[k];
      tmpTagMask = cmdPortVlanListSet->cmdData.portVlanListSet.operationalTagSet.value[k];
      while (tmpMemMask) 
      {
         L7_BOOL tagged = L7_FALSE;

         pos++;
         if (tmpMemMask & 0x1) 
         {
           vlanId = (k*8) + pos;

           if (tmpTagMask & 0x1) 
           {
             tagged = L7_TRUE;
           }

           BROAD_VLAN_MEMBER_SET (usp, vlanId, dapi_g);
           if (tagged == L7_TRUE) 
           {
             BROAD_VLAN_TAGGING_SET (usp, vlanId, dapi_g);
           }
           else
           {
             BROAD_VLAN_TAGGING_CLEAR (usp, vlanId, dapi_g);
           }

           if (addedHw == L7_TRUE) 
           {
             BROAD_HW_VLAN_MEMBER_SET (usp, vlanId, dapi_g);

             /* If port is a LAG, set VLAN membership mask for individual members too */
             if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
             {
               lagMemberSet = DAPI_PORT_GET(usp,dapi_g)->modeparm.lag.memberSet;

               for (i = 0;i < L7_MAX_MEMBERS_PER_LAG;i++)
               {
                 if (lagMemberSet[i].inUse == L7_TRUE)
                 {
                   BROAD_HW_VLAN_MEMBER_SET (&lagMemberSet[i].usp, vlanId, dapi_g);
                   if (tagged == L7_TRUE) 
                   {
                     BROAD_HW_VLAN_TAGGING_SET (&lagMemberSet[i].usp, vlanId, dapi_g);
                   }
                   else
                   {
                     BROAD_HW_VLAN_TAGGING_CLEAR (&lagMemberSet[i].usp, vlanId, dapi_g);
                   }

                   hapiLagMemberPortPtr = HAPI_PORT_GET(&lagMemberSet[i].usp, dapi_g);
                 }
               }
             }

             if (tagged == L7_TRUE) 
             {
               BROAD_HW_VLAN_TAGGING_SET (usp, vlanId, dapi_g);
             }
             else
             {
               BROAD_HW_VLAN_TAGGING_CLEAR (usp, vlanId, dapi_g);
             }

#ifdef L7_MCAST_PACKAGE
             /* Notify the IP multicast routing component that a port has been added 
             ** to a VLAN.
             */
             hapiBroadL3McastPortVlanAddNotify (usp, vlanId, dapi_g);
#endif
             /* Notify multicast group manager that a port or a LAG has been added
             ** to the hardware.
             */
             hapiBroadMgmPortVlanAddNotify (usp, vlanId, dapi_g);

             /* Notify dot1x that a port or a LAG has been added
             ** to the hardware.
             */
             if (makeDot1xOutcall)
             {
               hapiBroadDot1xPortVlanAddNotify (usp, vlanId,dapi_g);
             }

             hapiBroadL3PortVlanNotify(usp, vlanId, 0, dapi_g);
#ifdef L7_WIRELESS_PACKAGE
             /* Notify the L2 tunnel initiators about tagging changes on a physical port 
             */
             hapiBroadL2TunnelVlanAddNotify (usp, vlanId, tagged, dapi_g);
#endif
           }
        }
        tmpMemMask >>= 1;
        tmpTagMask >>= 1;
      }
    }
  }
  else /* Delete the port from list of vlans */
  {

    if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE) &&
        ((hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_TRUE) ||
         (dapiPortPtr->modeparm.physical.routerIntfEnabled == L7_TRUE)))
    {
      remFromHw = L7_FALSE;
    }
    /* Do the outcalls and update the mask*/
    for (k = 0; k < L7_VLAN_INDICES; k++) 
    {
      pos = 0;
      tmpMemMask = cmdPortVlanListSet->cmdData.portVlanListSet.operationalMemberSet.value[k];
      while (tmpMemMask) 
      {
        pos++;
        if (tmpMemMask & 0x1) 
        {
          vlanId = (k*8) + pos;

          /* Notify multicast group manager that a port or a LAG has been removed
          ** from the hardware.
          */
          hapiBroadMgmPortVlanRemoveNotify (usp, vlanId, dapi_g);

          /* Notify dot1x that a port or a LAG is about to be
          ** removed from a VLAN.
          */
          if (makeDot1xOutcall)
          {
            hapiBroadDot1xPortVlanRemoveNotify (usp, vlanId, dapi_g);
          }

          hapiBroadL3PortVlanNotify(usp, vlanId, 1, dapi_g);

          BROAD_VLAN_MEMBER_CLEAR  (usp, vlanId, dapi_g);
          BROAD_VLAN_TAGGING_CLEAR (usp, vlanId, dapi_g);

          /* If port-based routing is enabled on this port or the port is a LAG member
          ** then don't modify the hardware VLAN membership.
          */
          if (remFromHw == L7_TRUE) 
          {
#ifdef L7_MCAST_PACKAGE
            /* Notify the IP multicast routing component that a port has been removed 
            ** from a VLAN.
            */
            hapiBroadL3McastPortVlanRemoveNotify (usp, vlanId, dapi_g);
#endif
            BROAD_HW_VLAN_MEMBER_CLEAR  (usp, vlanId, dapi_g);
            BROAD_HW_VLAN_TAGGING_CLEAR (usp, vlanId, dapi_g);
 
            /* If port is a LAG, clear VLAN membership mask for individual members too */
            if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
            {
              lagMemberSet = DAPI_PORT_GET(usp,dapi_g)->modeparm.lag.memberSet;

              for (i = 0;i < L7_MAX_MEMBERS_PER_LAG;i++)
              {
                if (lagMemberSet[i].inUse == L7_TRUE)
                {
                   BROAD_HW_VLAN_MEMBER_CLEAR (&lagMemberSet[i].usp, vlanId, dapi_g);
                   BROAD_HW_VLAN_TAGGING_CLEAR (&lagMemberSet[i].usp, vlanId, dapi_g);
                   hapiLagMemberPortPtr = HAPI_PORT_GET(&lagMemberSet[i].usp, dapi_g);
                }
              }
            }
           
          }
        }
        tmpMemMask >>= 1;
      }
    }
    /* Delete the port from list of vlans*/
    if (remFromHw == L7_TRUE) 
    {

      if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
      {
          hapiBroadLagSetLagToVlanListHw (usp, L7_FALSE,
                                          &cmdPortVlanListSet->cmdData.portVlanListSet.operationalMemberSet,
                                          &cmdPortVlanListSet->cmdData.portVlanListSet.operationalTagSet,
                                            dapi_g);
      }
      else
      {
          hapiBroadL2VlanSetPortToVlanListHw (usp, L7_FALSE, 
                                              &cmdPortVlanListSet->cmdData.portVlanListSet.operationalMemberSet,
                                              &cmdPortVlanListSet->cmdData.portVlanListSet.operationalTagSet,
                                              dapi_g);
      }
    }
  }

#ifdef L7_MCAST_PACKAGE
  hapiBroadL3McastCritExit();
#endif

  hapiBroadVlanCritSecExit();

  return result;

}

/*********************************************************************
*
* @purpose Takes the dapiCmd info and modifies the NP's Vlan table
*          accordingly.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_QVLAN_PORT_CONFIG
* @param   void       *data   - DAPI_QVLAN_MGMT_CMD_t.cmdData.portConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQvlanPortConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t          result         = L7_SUCCESS;
  DAPI_QVLAN_MGMT_CMD_t *cmdVlanPortCfg = (DAPI_QVLAN_MGMT_CMD_t*)data;
  DAPI_CMD_GET_SET_t     getOrSet;
  L7_ushort16            vlanID;
  L7_BOOL                ingressFilteringEnabled;
  L7_uint32              acceptFrameType;
  BROAD_PORT_t          *hapiPortPtr;
  DAPI_PORT_t           *dapiPortPtr;
  BROAD_PORT_t          *hapiLagMemberPortPtr;
  L7_uint32              i;
  BROAD_SYSTEM_t       *hapiSystemPtr;

  hapiSystemPtr = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  getOrSet = cmdVlanPortCfg->cmdData.portConfig.getOrSet;

  if (getOrSet == DAPI_CMD_GET)
  {
    result = L7_FAILURE;
    return result;
  }

  /*
   * protect the VLAN database in case any other components are accessing the data
   * CAUTION: early returns must exit the crit sec
   */
  hapiBroadVlanCritSecEnter();

  vlanID                  = cmdVlanPortCfg->cmdData.portConfig.vlanID;
  ingressFilteringEnabled = cmdVlanPortCfg->cmdData.portConfig.ingressFilteringEnabled;
  acceptFrameType         = cmdVlanPortCfg->cmdData.portConfig.acceptFrameType;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  /* check for change in PVID */
  if ((cmdVlanPortCfg->cmdData.portConfig.vlanIDModify) == L7_TRUE)
  {
    result = hapiBroadPortUntaggedVlanSet(usp, vlanID, dapi_g);
    if (L7_SUCCESS != result)
    {
      LOG_ERROR(result);
    }

    hapiPortPtr->pvid = vlanID;
  }

  /* check for change in ingress filtering */
  if (cmdVlanPortCfg->cmdData.portConfig.ingressFilteringEnabledModify == L7_TRUE)
  {
    if (usp->slot == L7_LAG_SLOT_NUM)
    {
      dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

      hapiBroadLagCritSecEnter ();
      for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
      {
        if (dapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
        {
          hapiLagMemberPortPtr = HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[i].usp, dapi_g);

          result = hapiBroadVlanIngressFilterSet(hapiLagMemberPortPtr->bcmx_lport, ingressFilteringEnabled);
          if (result != L7_SUCCESS)
          {
            LOG_ERROR(result);
          }
        }
      }
      hapiBroadLagCritSecExit ();
    }
    else
    {
      if ((dapiPortPtr->modeparm.physical.routerIntfEnabled == L7_FALSE) &&
          (hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_FALSE) &&
          (hapiBroadSystemMirroringUSPIsProbe(usp,dapi_g) == L7_FALSE))
      {
        result = hapiBroadVlanIngressFilterSet(hapiPortPtr->bcmx_lport, ingressFilteringEnabled);
        if (result != L7_SUCCESS)
        {
          LOG_ERROR(result);
        }
      }
    }

    hapiPortPtr->ingressFilteringEnabled =  cmdVlanPortCfg->cmdData.portConfig.ingressFilteringEnabled;
  }

  /* check for change in acceptable frame */
  if (cmdVlanPortCfg->cmdData.portConfig.acceptFrameTypeModify == L7_TRUE)
  {
    result = hapiBroadPortAcceptableFrameTypeSet(usp, acceptFrameType, dapi_g);
    if (L7_SUCCESS != result)
    {
      LOG_ERROR(result);
    }

    hapiPortPtr->acceptFrameType =  cmdVlanPortCfg->cmdData.portConfig.acceptFrameType;
  }

  if (cmdVlanPortCfg->cmdData.portConfig.protocolModify == L7_TRUE)
  {
    for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
    {
      if (cmdVlanPortCfg->cmdData.portConfig.protocol[i] != hapiPortPtr->pbvlan_table[i])
      {
        if ((IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE) ||
            ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE) && (dapiPortPtr->modeparm.physical.routerIntfEnabled == L7_FALSE) &&
             (hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_FALSE)))
        {
          /* if a VLAN was previously configured, remove it */
          if (hapiPortPtr->pbvlan_table[i] != 0)
          {
            hapiBroadProtocolVlanRemove(usp, hapiSystemPtr->etherProtocol[i], dapi_g);
          }

          /* if a VLAN is now configured, add it */
          if (cmdVlanPortCfg->cmdData.portConfig.protocol[i] != 0)
          {
            hapiBroadProtocolVlanAdd(usp, hapiSystemPtr->etherProtocol[i], cmdVlanPortCfg->cmdData.portConfig.protocol[i], dapi_g);
          }
        }
      }

      hapiPortPtr->pbvlan_table[i] =  cmdVlanPortCfg->cmdData.portConfig.protocol[i];
    }
  }

  hapiBroadVlanCritSecExit();

  return result;
}


/*********************************************************************
*
* @purpose Takes the dapiCmd info and modifies the NP's Vlan table
*          accordingly.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_QVLAN_PORT_CONFIG
* @param   void       *data   - DAPI_QVLAN_MGMT_CMD_t.cmdData.portConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQvlanPbvlanConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{

  L7_RC_t          result         = L7_SUCCESS;
  DAPI_SYSTEM_CMD_t* cmdPbvlanConfig= (DAPI_SYSTEM_CMD_t*)data;
  BROAD_SYSTEM_t       *hapiSystemPtr;
  DAPI_CMD_GET_SET_t     getOrSet;

  getOrSet = cmdPbvlanConfig->cmdData.pbVlanConfig.getOrSet;
  hapiSystemPtr = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  if (getOrSet == DAPI_CMD_GET)
  {
    result = L7_FAILURE;
    return result;
  }

  hapiBroadVlanCritSecEnter();
  hapiSystemPtr->etherProtocol[cmdPbvlanConfig->cmdData.pbVlanConfig.protoIndex] = cmdPbvlanConfig->cmdData.pbVlanConfig.protoNum;

  hapiBroadVlanCritSecExit();

  return result;
}

/*********************************************************************
*
* @purpose Takes the dapiCmd info and modifies the ports default priority
*          for untagged frames 
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_QVLAN_PORT_PRIORITY
* @param   void       *data   - DAPI_QVLAN_MGMT_CMD_t.cmdData.portPriority
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQvlanPortPriority(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t               result            = L7_SUCCESS;
  DAPI_QVLAN_MGMT_CMD_t      *cmdVlanPortConfig = (DAPI_QVLAN_MGMT_CMD_t*)data;
  BROAD_PORT_t               *hapiPortPtr;
  DAPI_PORT_t                *dapiPortPtr;
  L7_uint32                   i;
  int                         rv;
  usl_bcm_port_priority_t    priority;

  priority = cmdVlanPortConfig->cmdData.portPriority.priority;

  dapiPortPtr = DAPI_PORT_GET(usp,dapi_g);

  if (IS_PORT_TYPE_CPU(dapiPortPtr) == L7_TRUE)
  {
    /*
     *don't do any of this for the cpu ifc
     *just pretend like it worked
     */
    return L7_SUCCESS;
  }

  if (cmdVlanPortConfig->cmdData.portPriority.getOrSet == DAPI_CMD_GET)
  {
    result = L7_FAILURE;
    return result;
  }


  if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_TRUE)
  {
    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
    hapiPortPtr->priority = priority;
    hapiBroadLagCritSecEnter ();
    for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
    {
      if (dapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
      {
        hapiPortPtr = HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[i].usp, dapi_g);

        rv = usl_bcmx_port_untagged_priority_set(hapiPortPtr->bcmx_lport, priority);
        if (L7_BCMX_OK(rv) != TRUE)
        {
          LOG_ERROR(0);
        }
      }
    }
    hapiBroadLagCritSecExit ();
  }
  else if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
  {
    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
    hapiPortPtr->priority = priority;

    rv = usl_bcmx_port_untagged_priority_set(hapiPortPtr->bcmx_lport, priority);
    if (L7_BCMX_OK(rv) != TRUE)
    {
      LOG_ERROR(priority);
    }
  }


  return result;
}

/*********************************************************************
*
* @purpose Set priority to traffic class map.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_QVLAN_PORT_PRIORITY_TO_TC_MAP
* @param   void       *data   - DAPI_QVLAN_MGMT_CMD_t.cmdData.portPriorityToTcMap
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   On Broadcom platform the TC to Priority map is set up 
*          during system boot and should not be be changed.
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QVLAN_PORT_PRIORITY_TO_TC_MAP */
L7_RC_t hapiBroadQvlanPortPriorityToTcMap(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                result         = L7_SUCCESS;
  DAPI_QVLAN_MGMT_CMD_t *cmdVlanPortCfg = (DAPI_QVLAN_MGMT_CMD_t*)data;
  L7_uchar8              priority;
  L7_uchar8              cosq;

  priority = cmdVlanPortCfg->cmdData.portPriorityToTcMap.priority;
  cosq     = cmdVlanPortCfg->cmdData.portPriorityToTcMap.traffic_class;

  result = hapiBroadCosSetDot1pParams(usp, priority, cosq, dapi_g);

  return result;
}

/*********************************************************************
*
* @purpose  Set all dot1p to cos queue mappings for a port to hardware
*
* @param   BROAD_PORT_t *hapiPortPtr  - port to update dot1p mapping
* @param   L7_uchar8    *dot1pMap     - actual dot1p mapping to use
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @notes    Do not use the dot1p mapping in the hapiPortPtr; instead,
*           use the input dot1pMap specified as this routine is
*           shared between QoS and non-QoS builds.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadCosCommitDot1pParams(BROAD_PORT_t *hapiPortPtr, L7_uchar8 *dot1pMap)
{
  int     i;
  L7_BOOL complete = L7_TRUE;
  L7_RC_t result   = L7_SUCCESS;

  /* For performance reasons make sure all the dot1p mappings are configured
   * prior to sending down any policies.
   */
  for (i = 0; i < L7_DOT1P_MAX_PRIORITY+1; i++)
  {
    if (dot1pMap[i] > L7_DOT1P_MAX_PRIORITY)
    {
        complete = L7_FALSE;
        break;
    }
  }

  if (complete)
  {
    BROAD_POLICY_t      cosqId;
    BROAD_POLICY_RULE_t ruleId;
    L7_BOOL             policyFound = L7_FALSE;
    L7_uchar8           exactMask[] = {FIELD_MASK_NONE, FIELD_MASK_NONE};

    result = osapiSemaTake(broadCOSQsema,L7_WAIT_FOREVER);
    if (result != L7_SUCCESS)
    {
      LOG_ERROR(0);
    }
   
    /* Try to find existing policy with same mapping. */
    if (hapiBroadCosPolicyUtilLookup(dot1pMap, L7_DOT1P_MAX_PRIORITY+1, &cosqId) == L7_TRUE)
    {
      if (hapiPortPtr->dot1pPolicy == cosqId)
      {
          result = osapiSemaGive(broadCOSQsema);
          if (result != L7_SUCCESS)
          {
            LOG_ERROR(0);
          }
          return L7_SUCCESS;   /* policy has not changed */
      }
	  policyFound = L7_TRUE;
    }

    if (L7_FALSE == policyFound)   /* create new policy */
    {
      /* create policy with new mappings */
      hapiBroadPolicyCreate(BROAD_POLICY_TYPE_COSQ);

      for (i = 0; i < L7_DOT1P_MAX_PRIORITY+1; i++)
      {
        L7_uchar8 dot1p = i;
        hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_LOWEST);
        hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OCOS, &dot1p, exactMask);
        hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, dot1pMap[i], 0, 0);
      }

      result = hapiBroadPolicyCommit(&cosqId);

      if (L7_SUCCESS == result)
        (void)hapiBroadCosPolicyUtilAdd(dot1pMap, L7_DOT1P_MAX_PRIORITY+1, cosqId);

    }

    if (L7_SUCCESS == result)
      result = hapiBroadCosPolicyUtilApply(cosqId, hapiPortPtr->bcmx_lport);

    /* delete the old policy */
    if (BROAD_POLICY_INVALID != hapiPortPtr->dot1pPolicy)
    {
      hapiBroadCosPolicyUtilRemove(hapiPortPtr->dot1pPolicy, hapiPortPtr->bcmx_lport);
      hapiPortPtr->dot1pPolicy = BROAD_POLICY_INVALID;
    }

    if (L7_SUCCESS == result)
    {
        hapiPortPtr->dot1pPolicy = cosqId;
    }
    else
    {
      hapiBroadCosPolicyUtilDelete(cosqId);
    }

    result = osapiSemaGive(broadCOSQsema); 
    if (result != L7_SUCCESS)
    {
       LOG_ERROR(0);
    }

  }

  return result;
}

/*********************************************************************
*
* @purpose  Set a single dot1p to cos queue mapping for an interface,
*           either physical port or LAG.
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadCosSetDot1pParams(DAPI_USP_t *usp, L7_uchar8 dot1p, L7_uchar8 cosq, DAPI_t *dapi_g)
{
  L7_RC_t       result = L7_SUCCESS;
  BROAD_PORT_t *hapiPortPtr;
  DAPI_PORT_t  *dapiPortPtr;

  if ((dot1p > L7_DOT1P_MAX_PRIORITY) || (cosq > L7_DOT1P_MAX_PRIORITY))
    return L7_FAILURE;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (hapiPortPtr->dot1pMap[dot1p] == cosq)
    return L7_SUCCESS;
      
  hapiPortPtr->dot1pMap[dot1p] = cosq;

  if(hapiBroadRoboCheck() == L7_TRUE)
  {
     bcmx_cosq_port_mapping_set(hapiPortPtr->bcmx_lport, (bcm_cos_t)dot1p,  (bcm_cos_queue_t)cosq);
     return L7_SUCCESS;
  }

  if (L7_TRUE == hapiPortPtr->port_is_lag)
  {
    int               i;
    DAPI_LAG_ENTRY_t *lagMemberSet;
    BROAD_PORT_t     *lagMemberPtr;
    L7_RC_t           tmpRc;

    lagMemberSet = dapiPortPtr->modeparm.lag.memberSet;

    hapiBroadLagCritSecEnter ();
    /* apply dot1p mapping to each LAG member */
    for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
    {
      if (L7_TRUE == lagMemberSet[i].inUse)
      {
        lagMemberPtr = HAPI_PORT_GET(&lagMemberSet[i].usp, dapi_g);

        tmpRc = hapiBroadCosCommitDot1pParams(lagMemberPtr, hapiPortPtr->dot1pMap);
        if (L7_SUCCESS != tmpRc)
            result = tmpRc;
      }
    }
    hapiBroadLagCritSecExit ();
  }
  else
  {
    /* apply dot1p mapping to port if not acquired */
    if (L7_FALSE == hapiPortPtr->hapiModeparm.physical.isMemberOfLag)
    {
      result = hapiBroadCosCommitDot1pParams(hapiPortPtr, hapiPortPtr->dot1pMap);
    }
  }

  return result;
}

/*********************************************************************
*
* @purpose Set mcast flood mode for a vlan
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_QVLAN_MCAST_FLOOD_SET
* @param   void       *data   - DAPI_QVLAN_MGMT_CMD_t.cmdData.mcastFloodModeSet
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQvlanMcastFloodModeSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                result         = L7_SUCCESS;
  DAPI_QVLAN_MGMT_CMD_t *cmdVlanCfg = (DAPI_QVLAN_MGMT_CMD_t*)data;
  L7_uint32              vlanId;
  L7_uint32              mode;
  bcm_vlan_mcast_flood_t bcmxMode;

  vlanId = cmdVlanCfg->cmdData.mcastFloodModeSet.vlanId;
  mode = cmdVlanCfg->cmdData.mcastFloodModeSet.mode;

  switch (mode)
  {
  case L7_FILTER_FORWARD_ALL:
    bcmxMode = BCM_VLAN_MCAST_FLOOD_ALL;
    break;
  case L7_FILTER_FORWARD_UNREGISTERED:
    bcmxMode = BCM_VLAN_MCAST_FLOOD_UNKNOWN;
    break;
  case L7_FILTER_FILTER_UNREGISTERED:
    bcmxMode = BCM_VLAN_MCAST_FLOOD_NONE;
    break;
  default:
    return L7_FAILURE;
  }

  if (usl_bcmx_vlan_mcast_flood_set(vlanId, bcmxMode) != BCM_E_NONE)
  {
    result = L7_FAILURE;
  }

  return result;
}
/*********************************************************************
*
* @purpose Takes the dapiCmd info and enables GVRP on a port.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_GARP_GVRP
* @param   void       *data   - DAPI_GARP_MGMT_CMD_t.cmdData.portGvrp
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadGarpGvrp(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{

  L7_RC_t          result      = L7_SUCCESS;
  DAPI_GARP_MGMT_CMD_t  *cmdGvrpGmrp = (DAPI_GARP_MGMT_CMD_t*)data;
  BROAD_PORT_t          *hapiPortPtr;

  if (cmdGvrpGmrp->cmdData.portGvrp.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    return result;
  }

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (cmdGvrpGmrp->cmdData.portGvrp.enable == L7_TRUE)
  {
    hapiPortPtr->gvrpEnabled = L7_TRUE;
  }
  else
  {
    hapiPortPtr->gvrpEnabled = L7_FALSE;
  } 

  return result;

}


/*********************************************************************
*
* @purpose Associate a VLAN w/ a protocol on a port
*
* @param   DAPI_USP_t *usp
* @param   L7_ushort16 etherType
* @param   L7_ushort16 vlanId
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadProtocolVlanAdd(DAPI_USP_t *usp, L7_ushort16 etherType, L7_ushort16 vlanId, DAPI_t *dapi_g)
{
  BROAD_PORT_t                 *hapiPortPtr;
  BROAD_PORT_t                 *hapiLagMemberPortPtr;
  DAPI_PORT_t                  *dapiPortPtr;
  L7_uint32                     lagMemberIdx;
  int                           rv;
  L7_RC_t                       rc = L7_SUCCESS;
  usl_bcm_port_pbvlan_config_t  pbvlanConfig;

  memset(&pbvlanConfig, 0, sizeof(pbvlanConfig));

  pbvlanConfig.frameType = BCM_PORT_FRAMETYPE_ETHER2;
  pbvlanConfig.etherType = etherType;
  pbvlanConfig.vid       = vlanId;


  /*
   * protect the VLAN database in case any other components are accessing the data
   * CAUTION: early returns must exit the crit sec
   */
  hapiBroadVlanCritSecEnter();

  if (usp->slot == L7_LAG_SLOT_NUM)
  {
    dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

    hapiBroadLagCritSecEnter ();
    for (lagMemberIdx = 0; lagMemberIdx < L7_MAX_MEMBERS_PER_LAG; lagMemberIdx++)
    {
      if (dapiPortPtr->modeparm.lag.memberSet[lagMemberIdx].inUse == L7_TRUE)
      {
        hapiLagMemberPortPtr = HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[lagMemberIdx].usp, dapi_g);

        rv = usl_bcmx_port_protocol_vlan_add(hapiLagMemberPortPtr->bcmx_lport, 
                                             pbvlanConfig);
        if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_EXISTS))
        {
          rc = L7_FAILURE;
        }
      }
    }

    hapiBroadLagCritSecExit ();
  }
  else
  {
    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

    rv = usl_bcmx_port_protocol_vlan_add(hapiPortPtr->bcmx_lport, 
                                         pbvlanConfig);
    if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_EXISTS))
    {
      rc = L7_FAILURE;
    }
  }

  hapiBroadVlanCritSecExit();

  return rc;
}

/*********************************************************************
*
* @purpose Disassociate a VLAN w/ a protocol on a port
*
* @param   DAPI_USP_t *usp
* @param   L7_ushort16 etherType
* @param   L7_ushort16 vlanId
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadProtocolVlanRemove(DAPI_USP_t *usp, L7_ushort16 etherType, DAPI_t *dapi_g)
{
  BROAD_PORT_t                   *hapiPortPtr;
  BROAD_PORT_t                   *hapiLagMemberPortPtr;
  DAPI_PORT_t                    *dapiPortPtr;
  L7_uint32                       lagMemberIdx;
  int                             rv;
  L7_RC_t                         rc = L7_SUCCESS;
  usl_bcm_port_pbvlan_config_t    pbvlanConfig;

  memset(&pbvlanConfig, 0, sizeof(pbvlanConfig));
  pbvlanConfig.frameType = BCM_PORT_FRAMETYPE_ETHER2;
  pbvlanConfig.etherType = etherType;
  pbvlanConfig.vid       = 0; /* vlan-id is ignored for delete */

  /*
   * protect the VLAN database in case any other components are accessing the data
   * CAUTION: early returns must exit the crit sec
   */
  hapiBroadVlanCritSecEnter();

  if (usp->slot == L7_LAG_SLOT_NUM)
  {
    dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

    hapiBroadLagCritSecEnter ();
    for (lagMemberIdx = 0; lagMemberIdx < L7_MAX_MEMBERS_PER_LAG; lagMemberIdx++)
    {
      if (dapiPortPtr->modeparm.lag.memberSet[lagMemberIdx].inUse == L7_TRUE)
      {
        hapiLagMemberPortPtr = HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[lagMemberIdx].usp, dapi_g);

        rv = usl_bcmx_port_protocol_vlan_delete(hapiLagMemberPortPtr->bcmx_lport, 
                                                pbvlanConfig);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          rc = L7_FAILURE;
        }
      }
    }

    hapiBroadLagCritSecExit ();
  }
  else
  {
    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

    rv = usl_bcmx_port_protocol_vlan_delete(hapiPortPtr->bcmx_lport, 
                                            pbvlanConfig);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      rc = L7_FAILURE;
    }
  }

  hapiBroadVlanCritSecExit();

  return rc;
}

/*********************************************************************
*
* @purpose Acquire the Drivers VLAN Database
*
* @param   void
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadVlanCritSecEnter(void)
{
  if (broadVlanSema == L7_NULLPTR)
  {
      LOG_ERROR(broadVlanSema);
      return L7_FAILURE;
    }
  return osapiSemaTake(broadVlanSema,L7_WAIT_FOREVER);
}

/*********************************************************************
*
* @purpose Release the Drivers VLAN Database
*
* @param   void
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadVlanCritSecExit(void)
{
  if (broadVlanSema == L7_NULLPTR)
  {
    LOG_ERROR(broadVlanSema);
    return L7_FAILURE;
  }

  return osapiSemaGive(broadVlanSema);
}

/*********************************************************************
*
* @purpose Determine if specified VLAN exists.
*
* @param   vid - Vlan ID.
*
* @returns L7_TRUE - Exists or L7_FALSE - Doesn't exist.
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadQvlanVlanExists(L7_uint32 vid)
{
  if ((vid > 0) && (vid <= L7_MAX_VLAN_ID))
  {
    return (L7_VLAN_ISMASKBITSET(vlanExists, vid) ? L7_TRUE : L7_FALSE);
  }

  return L7_FALSE;
}


/*********************************************************************
*
* @purpose Set the Ingress Filtering mode
*
* @param   lport  - broadcom lport
* @param   val    - True, enabled
*                 - False, disabled
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes   
* @end
*
*********************************************************************/
L7_RC_t hapiBroadVlanIngressFilterSet(bcmx_lport_t lport, L7_BOOL val)
{
  usl_bcm_port_filter_mode_t mode;
  int rc = BCM_E_NONE;

  /* don't allow changes for wlan ports here */
  if (BCM_GPORT_IS_WLAN_PORT(lport))
  {
    return L7_SUCCESS;
  }

  mode.flags = BCM_PORT_VLAN_MEMBER_INGRESS;
  mode.setFlags = val;
  rc = usl_bcmx_port_vlan_member_set(lport, mode);               

  return ((L7_BCMX_OK(rc))?L7_SUCCESS:L7_FAILURE);
}

/*********************************************************************
*
* @purpose Set the Egress Filtering mode
*
* @param   lport  - broadcom lport
* @param   val    - True, enabled
*                 - False, disabled
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadVlanEgressFilterSet(bcmx_lport_t lport, L7_BOOL val, DAPI_t *dapi_g)
{
  int rv = BCM_E_NONE;
  BROAD_PORT_t   *hapiLagMemberPortPtr;
  DAPI_USP_t lagUsp;
  DAPI_PORT_t  *dapiPortPtr;
  int i;
  usl_bcm_port_filter_mode_t filterMode;

  filterMode.flags = BCM_PORT_VLAN_MEMBER_EGRESS;
  filterMode.setFlags = val;

  if (BCM_GPORT_IS_TRUNK(lport))
  {
    if (hapiBroadTgidToUspConvert(BCM_GPORT_TRUNK_GET(lport), 
                                  &lagUsp, 
                                  dapi_g) == L7_SUCCESS)
    {
      dapiPortPtr = DAPI_PORT_GET(&lagUsp, dapi_g);

      hapiBroadLagCritSecEnter ();

      for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
      {
        if (dapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
        {

          hapiLagMemberPortPtr = 
            HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[i].usp, dapi_g);

          rv = usl_bcmx_port_vlan_member_set(hapiLagMemberPortPtr->bcmx_lport, 
                                             filterMode);               

        }
      }

      hapiBroadLagCritSecExit();
    }
  }
  else
  {
    rv = usl_bcmx_port_vlan_member_set(lport, filterMode);               
  }

  return ((L7_BCMX_OK(rv))?L7_SUCCESS:L7_FAILURE);
}

/*********************************************************************
*
* @purpose Set the tpid on a port
*
* @param   lport  - broadcom lport
* @param   val    - True, enabled
*                 - False, disabled
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPortTpidSet(bcmx_lport_t lport, L7_ushort16 val, DAPI_t *dapi_g)
{
  int rc = BCM_E_NONE;
  DAPI_USP_t lagUsp;
  DAPI_PORT_t  *dapiPortPtr;
  int i;

  if (BCM_GPORT_IS_TRUNK(lport))
  {
    if (hapiBroadTgidToUspConvert(lport, &lagUsp, dapi_g) == L7_SUCCESS)
    {
      dapiPortPtr = DAPI_PORT_GET(&lagUsp, dapi_g);

      hapiBroadLagCritSecEnter ();
      for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
      {
        if (dapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
        {
          BROAD_PORT_t   *hapiLagMemberPortPtr;

          hapiLagMemberPortPtr = HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[i].usp, dapi_g);
          rc = bcmx_port_tpid_set(hapiLagMemberPortPtr->bcmx_lport, val);               
        }
      }
      hapiBroadLagCritSecExit();
    }
  }
  else
  { 
    rc = bcmx_port_tpid_set(lport, val);               
  }

  return ((L7_BCMX_OK(rc))?L7_SUCCESS:L7_FAILURE);
}

extern DAPI_t *dapi_g;
void hapiBroadDebugVlanMembershipMaskPrint(L7_uint32 unit, 
                                           L7_uint32 slot,
                                           L7_uint32 port)
{
  L7_uint32      vlanId, j;
  DAPI_USP_t     usp;

  usp.unit = unit;
  usp.slot = slot;
  usp.port = (port - 1);
  
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Vlan software membership: \n");
  for (vlanId = 1, j = 0; vlanId <= L7_PLATFORM_MAX_VLAN_ID; vlanId++)
  {
    if (BROAD_IS_VLAN_MEMBER(&usp, vlanId, dapi_g))
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "   %d", vlanId );

      if (BROAD_IS_VLAN_TAGGING(&usp, vlanId, dapi_g))
      {
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "(t)", vlanId );
      }

      j++;
      if (j%10 == 0 )   
      {
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\n");
      }
    }
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\nVlan hardware membership: \n");
  for (vlanId = 1, j = 0; vlanId <= L7_PLATFORM_MAX_VLAN_ID; vlanId++)
  {
    if (BROAD_IS_HW_VLAN_MEMBER(&usp, vlanId, dapi_g))
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "   %d", vlanId );

      if (BROAD_IS_HW_VLAN_TAGGING(&usp, vlanId, dapi_g))
      {
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "(t)", vlanId );
      }

      j++;
      if (j%10 == 0 )   
      {
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "\n");
      }
    }
  }

  return;

}
