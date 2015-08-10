/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_l2_lag.c
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
#include "sysapi.h"
#include "osapi.h"
#include "l7_common.h"
#include "log.h"

#include "broad_common.h"
#include "broad_l2_lag.h"
#include "broad_l2_mcast.h"
#include "broad_l2_vlan.h"
#include "broad_l2_std.h"
#include "broad_l2_ipsg.h"
#include "broad_l3.h"
#include "broad_llpf.h"

#include "dot1s_exports.h"
#include "dot3ad_exports.h"

#ifdef L7_QOS_PACKAGE
  #include "broad_qos.h"
#endif

#ifdef L7_MCAST_PACKAGE
  #include "broad_l3_mcast.h"
#endif

#ifdef L7_WIRELESS_PACKAGE
#include "broad_l2_tunnel.h"
#endif

#include "bcm/trunk.h"
#include "bcm/port.h"

#include "bcmx/lport.h"
#include "bcmx/trunk.h"
#include "bcmx/port.h"

#include "l7_usl_bcmx_l2.h"
#include "l7_usl_bcmx_port.h"

#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
#include "broad_l2_dot1ad.h"
#include "broad_dot1ad.h"
#endif
#endif

#ifdef L7_LLPF_PACKAGE
#include "broad_llpf.h"
#endif

#include "ptin_globaldefs.h"
#include "logger.h"   /* PTin added: trunks */

extern DAPI_t *dapi_g;

int hapiBroadDebugLag= 0;

void hapiBroadDebugLagEnable(int enable)
{
  hapiBroadDebugLag = enable;

  return;
}

/* LAG access semaphore.
*/
static void                *hapiBroadLagSema;

/* LAG Async queue and async done sema
*/
extern void *hapiBroadLagAsyncCmdQueue, *hapiBroadLagAsyncDoneSema;

L7_RC_t hapiBroadLagPortAsyncAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);
L7_RC_t hapiBroadLagPortAsyncDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);
extern L7_RC_t hapiBroadSystemMirroringSet(DAPI_USP_t *fromUsp, DAPI_USP_t *toUsp, DAPI_t *dapi_g,
                                    L7_BOOL add, L7_uint32 probeType);
L7_RC_t hapiBroadLagHashToPsc(L7_uint32 hashMode, L7_int32 *bcmPsc);

/* PTin added */
#if ( LAG_DIRECT_CONTROL_FEATURE )
extern int dapi_usp_is_internal_lag_member(DAPI_USP_t *dusp);
#define ACCESS_LAG_AT_SDK_LEVEL(command)    ((command)==cmd || !dapi_usp_is_internal_lag_member(usp))
#else
#define ACCESS_LAG_AT_SDK_LEVEL(command)    (1)
#endif


/*********************************************************************
*
* @purpose Wait for LAG async work to finish.
*
* @param   timeout {(input)} Time (secs) to wait for Lag Async task to finish 
*                            processing
*
* @returns L7_RC_t: L7_SUCCESS - Operation successful
*                   L7_FAILURE - Timeout waiting for driver to finish or other
*                                failures 
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLagAsyncDoneWait(L7_int32 timeout)
{

  hapi_broad_lag_async_msg_t   lag_cmd;
  L7_int32                     wait;
  L7_RC_t                      rc;

  lag_cmd.async_cmd = HAPI_BROAD_LAG_ASYNC_DONE;

  rc = osapiMessageSend (hapiBroadLagAsyncCmdQueue, 
                         (void*)&lag_cmd,
                         sizeof (lag_cmd), 
                         L7_WAIT_FOREVER, 
                         L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
            "Lag async done message send failed on Driver Lag queue 0x%x",
            hapiBroadLagAsyncCmdQueue);
    return rc;
  }

  /* Signal the L2 Async task that work is pending. */
  (void)hapiBroadL2AsyncTaskSignal();

  wait = timeout;

  if (wait != L7_WAIT_FOREVER)
  {
    /* Convert to msecs */
    wait = wait * 1000;        
  }

  rc = osapiSemaTake(hapiBroadLagAsyncDoneSema, wait);
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
            "Lag Async done message wait timeout on lag queue 0x%x, timeout %d secs",
            hapiBroadLagAsyncCmdQueue, timeout);
  }

  return rc;
}

/*********************************************************************
*
* @purpose Take LAG access semaphore.
*
* @param none
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadLagCritSecEnter(void)
{
  osapiSemaTake(hapiBroadLagSema,L7_WAIT_FOREVER);
}

/*********************************************************************
*
* @purpose Give LAG access semaphore.
*
* @param none
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadLagCritSecExit(void)
{
  osapiSemaGive(hapiBroadLagSema);
}

/*********************************************************************
*
* @purpose Process asynchronous LAG commands.
*
* @param   num_msgs  Number of Lag membership update messages to be processed. 
* @param   dapi_g    Driver object 
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadLagAsyncProcessMessages(L7_uint32 num_msgs, DAPI_t *dapi_g)
{
  hapi_broad_lag_async_msg_t lag_cmd;
  DAPI_LAG_MGMT_CMD_t    cmdLagPortAdd;
  DAPI_LAG_MGMT_CMD_t    cmdLagPortDelete;


  while (num_msgs--)
  {
    if (osapiMessageReceive(hapiBroadLagAsyncCmdQueue, (void *)&lag_cmd,
                            sizeof(lag_cmd),
                            L7_NO_WAIT) != L7_SUCCESS)
    {
      break;
    }

    switch (lag_cmd.async_cmd)
    {
      case HAPI_BROAD_LAG_ASYNC_PORT_ADD:
        cmdLagPortAdd.cmdData.lagPortAdd.getOrSet = lag_cmd.asyncData.lagPortAdd.getOrSet;
        cmdLagPortAdd.cmdData.lagPortAdd.numOfMembers = lag_cmd.asyncData.lagPortAdd.numOfMembers;
        cmdLagPortAdd.cmdData.lagPortAdd.maxFrameSize = lag_cmd.asyncData.lagPortAdd.maxFrameSize;
        cmdLagPortAdd.cmdData.lagPortAdd.memberSet  = &lag_cmd.asyncData.lagPortAdd.memberSet[0];
        cmdLagPortAdd.cmdData.lagPortAdd.hashMode = lag_cmd.asyncData.lagPortAdd.hashMode;

        (void)hapiBroadLagPortAsyncAdd(&lag_cmd.usp, DAPI_CMD_LAG_PORT_ADD, &cmdLagPortAdd, dapi_g);
        break;

      case HAPI_BROAD_LAG_ASYNC_PORT_DELETE:
        cmdLagPortDelete.cmdData.lagPortDelete.getOrSet = lag_cmd.asyncData.lagPortDelete.getOrSet;
        cmdLagPortDelete.cmdData.lagPortDelete.numOfMembers = lag_cmd.asyncData.lagPortDelete.numOfMembers;
        cmdLagPortDelete.cmdData.lagPortDelete.maxFrameSize = lag_cmd.asyncData.lagPortDelete.maxFrameSize;
        cmdLagPortDelete.cmdData.lagPortDelete.memberSet  = &lag_cmd.asyncData.lagPortDelete.memberSet[0];
        cmdLagPortDelete.cmdData.lagPortDelete.hashMode = lag_cmd.asyncData.lagPortAdd.hashMode;

        (void)hapiBroadLagPortAsyncDelete(&lag_cmd.usp, DAPI_CMD_LAG_PORT_DELETE, &cmdLagPortDelete, dapi_g);
        break;

      case HAPI_BROAD_LAG_ASYNC_DONE:
        osapiSemaGive(hapiBroadLagAsyncDoneSema);
        break;

      default:
        LOG_ERROR (lag_cmd.async_cmd);
    }
  }

}

/*********************************************************************
*
* @purpose Initialize the LAG component.
*
* @param none
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadLagInit (void)
{

  hapiBroadLagSema = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);
  if (hapiBroadLagSema == L7_NULL)
  {
    LOG_ERROR(0);
  }

}

/*********************************************************************
*
* @purpose Add a LAG to the specified VLAN ID.
*
* @param hapiLagPortPtr - Pointer to the specified LAG.
* @param vlan_id        - Specified VLAN ID.
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadLagAddLagToVlanHw (DAPI_USP_t *usp,
                                 L7_ushort16   vlan_id,
                                 DAPI_t        *dapi_g)
{
  DAPI_LAG_ENTRY_t   *lagMemberSet;
  L7_uint32           i;
  L7_BOOL             tagged;
#ifdef L7_MCAST_PACKAGE
  bcm_chip_family_t   board_family=0; 
#endif

  lagMemberSet = DAPI_PORT_GET(usp, dapi_g)->modeparm.lag.memberSet;

  /* Loop through all LAG members and add or remove them from the VLAN.
  */
  for (i = 0;i < L7_MAX_MEMBERS_PER_LAG;i++)
  {
    if (lagMemberSet[i].inUse == L7_TRUE)
    {
      if (BROAD_IS_VLAN_TAGGING(usp, vlan_id, dapi_g))
      {
        tagged = L7_TRUE;
      }
      else
      {
        tagged = L7_FALSE;
      }

      hapiBroadL2VlanAddPortToVlanHw (&lagMemberSet[i].usp,
                                      vlan_id,
                                      tagged,
                                      dapi_g);
    }
  }

#ifdef L7_MCAST_PACKAGE
  /* Notify the IP multicast routing component that a LAG has been added 
  ** to a VLAN.
  */
  /* We only need to make this call for non-XGS3 devices. Otherwise the work is
     done as part of hapiBroadL2VlanAddPortToVlanHw() above. */
  hapiBroadGetSystemBoardFamily(&board_family);
  if ((board_family == BCM_FAMILY_DRACO) ||
      (board_family == BCM_FAMILY_TUCANA))
  {
    hapiBroadL3McastPortVlanAddNotify (usp, vlan_id, dapi_g);
  }
#endif

}

/*********************************************************************
*
* @purpose Remove a LAG tfrom the specified VLAN ID.
*
* @param hapiLagPortPtr - Pointer to the specified LAG.
* @param vlan_id        - Specified VLAN ID.
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadLagRemoveLagFromVlanHw (DAPI_USP_t *usp,
                                      L7_ushort16   vlan_id,
                                      DAPI_t        *dapi_g)
{
  DAPI_LAG_ENTRY_t     *lagMemberSet;
  L7_uint32             i;
#ifdef L7_MCAST_PACKAGE
  bcm_chip_family_t     board_family=0; 
#endif

  lagMemberSet = DAPI_PORT_GET(usp,dapi_g)->modeparm.lag.memberSet;

#ifdef L7_MCAST_PACKAGE
  /* Notify the IP multicast routing component that a LAG is about to be removed
  ** from a VLAN.
  */
  /* We only need to make this call for non-XGS3 devices. Otherwise the work is
     done as part of hapiBroadL2VlanRemovePortFromVlanHw() below. */
  hapiBroadGetSystemBoardFamily(&board_family);
  if ((board_family == BCM_FAMILY_DRACO) ||
      (board_family == BCM_FAMILY_TUCANA))
  {
    hapiBroadL3McastPortVlanRemoveNotify (usp, vlan_id, dapi_g);
  }
#endif

  /* Loop through all LAG members and add or remove them from the VLAN.
  */
  for (i = 0;i < L7_MAX_MEMBERS_PER_LAG;i++)
  {
    if (lagMemberSet[i].inUse == L7_TRUE)
    {
      hapiBroadL2VlanRemovePortFromVlanHw (&lagMemberSet[i].usp,
                                           vlan_id,
                                           dapi_g);
    }
  }
}

/*********************************************************************
*
* @purpose Add/Remove a LAG from the specified Vlan list.
*
* @param usp                  - Pointer to the LAG USP
* @param vlanCmd              - L7_TRUE:   Add Lag to list of vlans
*                               L7_FALSE:  Remove Lag from list of vlans
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
void hapiBroadLagSetLagToVlanListHw (DAPI_USP_t       *usp,
                                     L7_BOOL           vlanCmd,
                                     L7_VLAN_MASK_t   *vlanMemberSet,
                                     L7_VLAN_MASK_t   *vlanTagSet,
                                     DAPI_t           *dapi_g)
{
  DAPI_LAG_ENTRY_t     *lagMemberSet;
  L7_uint32             i;

  lagMemberSet = DAPI_PORT_GET(usp,dapi_g)->modeparm.lag.memberSet;

  /* Loop through all LAG members and add or remove them from the VLAN.
  */
  for (i = 0;i < L7_MAX_MEMBERS_PER_LAG;i++)
  {
    if (lagMemberSet[i].inUse == L7_TRUE)
    {
      hapiBroadL2VlanSetPortToVlanListHw (&lagMemberSet[i].usp,
                                          vlanCmd,
                                          vlanMemberSet,
                                          vlanTagSet,
                                          dapi_g);
    }
  }
}

/*********************************************************************
*
* @purpose Add or remove specified LAG from hardware VLAN membership.
*      LAG ports are added or removed indivisually.
*
* @param hapiLagPortPtr - Pointer to the specified LAG.
* @param port_cmd    - 1 - Add port to the hardware VLAN membership
*            0 - Remove port from the hardware VLAN membership.
*
* @returns none
*
* @notes none
*
* @end
*
*********************************************************************/
void hapiBroadLagAddRemoveLagFromVlans (DAPI_USP_t *usp,
                                        L7_uint32   port_cmd,
                                        DAPI_t     *dapi_g)
{
  L7_ushort16   vlan_id, numVlans;
  DAPI_LAG_ENTRY_t     *lagMemberSet;
  L7_uint32      i;
  int rv;
  usl_bcm_port_vlan_t port_command;
  BROAD_PORT_t       *hapiPortPtr, *hapiLagMemberPortPtr;
  L7_BOOL        tag;


  lagMemberSet = DAPI_PORT_GET(usp,dapi_g)->modeparm.lag.memberSet;

  /* Add all LAG members to the hardware.
  */
  if (port_cmd == 1)
  {
    /* Loop through all LAG members and add them to the VLAN.
    */
    for (i = 0;i < L7_MAX_MEMBERS_PER_LAG;i++)
    {
      if (lagMemberSet[i].inUse == L7_TRUE)
      {
        memset (&port_command, 0, sizeof (usl_bcm_port_vlan_t));

        numVlans=0;
        for (vlan_id = 1; vlan_id <= L7_PLATFORM_MAX_VLAN_ID; vlan_id++)
        {
          if ((BROAD_IS_VLAN_MEMBER(usp, vlan_id, dapi_g)) &&
              (!BROAD_IS_HW_VLAN_MEMBER(&lagMemberSet[i].usp,vlan_id,dapi_g)))
          {
            /* Add the LAG member to VLAN, only when LAG is member of VLAN and the LAG 
             * member port is not member of that VLAN
             */ 
            numVlans++;
            BROAD_HW_VLAN_MEMBER_SET(&lagMemberSet[i].usp,vlan_id,dapi_g);

            L7_VLAN_SETMASKBIT (port_command.vlan_membership_mask, vlan_id);
            if (BROAD_IS_VLAN_TAGGING (usp, vlan_id, dapi_g))
            {
              tag = L7_TRUE;
            }
            else
            {
              tag = L7_FALSE;
            }

            if (tag)
            {
              BROAD_HW_VLAN_TAGGING_SET(&lagMemberSet[i].usp,vlan_id,dapi_g);

              L7_VLAN_SETMASKBIT (port_command.vlan_tag_mask, vlan_id);
            }

            hapiLagMemberPortPtr = HAPI_PORT_GET(&lagMemberSet[i].usp, dapi_g);
          }

        }

        if (numVlans) 
        {
          hapiPortPtr = HAPI_PORT_GET(&lagMemberSet[i].usp,dapi_g);

          rv = usl_bcmx_port_vlan_member_add (hapiPortPtr->bcmx_lport, &port_command);
          if (L7_BCMX_OK(rv) != L7_TRUE)
          {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't add port 0x%x to VLANs, rv = %d", hapiPortPtr->bcmx_lport, rv);
          }
        }
      }
    }
  }


  /* Send notifications to other components.
  */
  for (vlan_id = 1; vlan_id <= L7_PLATFORM_MAX_VLAN_ID; vlan_id++)
  {
    if (BROAD_IS_VLAN_MEMBER(usp, vlan_id, dapi_g))
    {
      if (port_cmd == 1)
      {
        hapiBroadL3PortVlanNotify(usp, vlan_id, 0, dapi_g);

#ifdef L7_MCAST_PACKAGE
        /* Notify the IP multicast routing component that a LAG has been added 
        ** to a VLAN.
        */
        hapiBroadL3McastPortVlanAddNotify (usp, vlan_id, dapi_g);
#endif
      } else
      {
        hapiBroadL3PortVlanNotify(usp, vlan_id, 1, dapi_g);

#ifdef L7_MCAST_PACKAGE
        /* Notify the IP multicast routing component that a LAG is about to be removed
        ** from a VLAN.
        */
        hapiBroadL3McastPortVlanRemoveNotify (usp, vlan_id, dapi_g);
#endif
      }
    }
  }

  /* Remove all LAG members from the hardware.
  */
  if (port_cmd == 0)
  {
    /* Loop through all LAG members and add them to the VLAN.
    */
    for (i = 0;i < L7_MAX_MEMBERS_PER_LAG;i++)
    {
      if (lagMemberSet[i].inUse == L7_TRUE)
      {
        memset (&port_command, 0, sizeof (usl_bcm_port_vlan_t));

        numVlans=0;
        for (vlan_id = 1; vlan_id <= L7_PLATFORM_MAX_VLAN_ID; vlan_id++)
        {
          if ((BROAD_IS_VLAN_MEMBER(usp, vlan_id, dapi_g)) &&
              (BROAD_IS_HW_VLAN_MEMBER(&lagMemberSet[i].usp, vlan_id, dapi_g)))
          {
            /* Remove the LAG member from VLAN, only when it is part of that VLAN */
            numVlans++;
            L7_VLAN_SETMASKBIT (port_command.vlan_membership_mask, vlan_id);
            BROAD_HW_VLAN_MEMBER_CLEAR(&lagMemberSet[i].usp, vlan_id, dapi_g);
            BROAD_HW_VLAN_TAGGING_CLEAR(&lagMemberSet[i].usp, vlan_id, dapi_g);

            hapiLagMemberPortPtr = HAPI_PORT_GET(&lagMemberSet[i].usp, dapi_g);
          }

        }

        if (numVlans)
        {
          hapiPortPtr = HAPI_PORT_GET(&lagMemberSet[i].usp,dapi_g);
          rv = usl_bcmx_port_vlan_member_delete (hapiPortPtr->bcmx_lport, &port_command);
          if (L7_BCMX_OK(rv) != L7_TRUE)
          {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't remove port 0x%x from VLANs, rv = %d", hapiPortPtr->bcmx_lport, rv);
          } 
        }
      }
    }
  }

}

/*********************************************************************
*
* @purpose Add or remove specified port from LAG VLANs
*
* @param usp
* @param port_cmd    - 1 - Add port to the hardware VLAN membership
*            0 - Remove port from the hardware VLAN membership.
* @param dapi_g
*
* @returns none
*
* @notes This function is called only for LAG members.
*
* @end
*
*********************************************************************/
static void hapiBroadAddRemovePortFromLagVlans (DAPI_USP_t *usp,
                                                L7_uint32   port_cmd,
                                                DAPI_t     *dapi_g)
{
  L7_ushort16   vlan_id;
  BROAD_PORT_t *hapiPortPtr;
  usl_bcm_port_vlan_t port_command;
  int rv;
  DAPI_USP_t   *lag_usp;
  bcm_chip_family_t board_family=0; 
  L7_BOOL       tag;

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  memset (&port_command, 0, sizeof (usl_bcm_port_vlan_t));

  /* This port must be a LAG member.
  */
  if (hapiPortPtr->hapiModeparm.physical.isMemberOfLag != L7_TRUE)
  {
    LOG_ERROR (usp->port);
  }

  lag_usp = &hapiPortPtr->hapiModeparm.physical.lagUsp;

  /* Figure out to which VLANs the port needs to be added or removed.
  */
  for (vlan_id = 1; vlan_id <= L7_PLATFORM_MAX_VLAN_ID; vlan_id++)
  {
    if (BROAD_IS_VLAN_MEMBER(lag_usp, vlan_id, dapi_g))
    {
      L7_VLAN_SETMASKBIT (port_command.vlan_membership_mask, vlan_id);
      if (BROAD_IS_VLAN_TAGGING (lag_usp, vlan_id, dapi_g))
      {
        L7_VLAN_SETMASKBIT (port_command.vlan_tag_mask, vlan_id);
      }
    }
  }

  /* Add port to the VLAN.
  */
  if (port_cmd == 1)
  {
    rv = usl_bcmx_port_vlan_member_add (hapiPortPtr->bcmx_lport, &port_command);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't add port 0x%x to VLANs, rv = %d", hapiPortPtr->bcmx_lport, rv);
    }
  }

  /* Update membership masks.
  */
  for (vlan_id = 1; vlan_id <= L7_PLATFORM_MAX_VLAN_ID; vlan_id++)
  {
    if (BROAD_IS_VLAN_MEMBER(lag_usp, vlan_id, dapi_g))
    {
      if (port_cmd == 1)
      {
        BROAD_HW_VLAN_MEMBER_SET(usp,vlan_id,dapi_g);
        if (BROAD_IS_VLAN_TAGGING(lag_usp, vlan_id, dapi_g))
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

      } else
      {
        BROAD_HW_VLAN_MEMBER_CLEAR(usp,vlan_id,dapi_g);
        BROAD_HW_VLAN_TAGGING_CLEAR(usp,vlan_id,dapi_g);
      }
    }
  }

  if (port_cmd == 0)
  {
    rv = usl_bcmx_port_vlan_member_delete (hapiPortPtr->bcmx_lport, &port_command);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't remove port 0x%x from VLANs, rv = %d", hapiPortPtr->bcmx_lport, rv);
    }
  }

  hapiBroadGetSystemBoardFamily(&board_family);
  if ((board_family != BCM_FAMILY_DRACO) &&
      (board_family != BCM_FAMILY_TUCANA))
  {
    for (vlan_id = 1; vlan_id <= L7_PLATFORM_MAX_VLAN_ID; vlan_id++)
    {
      if (BROAD_IS_VLAN_MEMBER(lag_usp, vlan_id, dapi_g))
      {
        /* For XGS3 devices, all ports added to a LAG need to be included
           in the bitmaps for the IPMC entries. */
        if (port_cmd == 1)
        {
#ifdef L7_MCAST_PACKAGE
          /* Notify the IP multicast routing component that a port has been added 
          ** to a VLAN.
          */
          hapiBroadL3McastPortVlanAddNotify (usp, vlan_id, dapi_g);
#endif

#ifdef L7_WIRELESS_PACKAGE
        /* Notify the L2 tunnel initiators about lag member changes 
        */
        hapiBroadL2TunnelLagMemberChangeNotify (usp, vlan_id, 1, dapi_g);
#endif

          hapiBroadL3PortVlanNotify(usp, vlan_id, 0, dapi_g);
        }
        else
        {
#ifdef L7_MCAST_PACKAGE
          /* Notify the IP multicast routing component that a LAG is about to be removed
          ** from a VLAN.
          */
          hapiBroadL3McastPortVlanRemoveNotify (usp, vlan_id, dapi_g);
#endif

#ifdef L7_WIRELESS_PACKAGE
        /* Notify the L2 tunnel initiators about lag member changes 
        */
        hapiBroadL2TunnelLagMemberChangeNotify (usp, vlan_id, 0, dapi_g);
#endif

          hapiBroadL3PortVlanNotify(usp, vlan_id, 1, dapi_g);
        }
      }
    }
  }
}


/*********************************************************************
*
* @purpose Hook in the lag functionality and create any data
*
* @param   DAPI_PORT_t *dapiPortPtr - generic driver port member
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL2LagPortInit(DAPI_PORT_t *dapiPortPtr)
{

  L7_RC_t result = L7_SUCCESS;

  dapiPortPtr->cmdTable[DAPI_CMD_LAG_CREATE       ] = (HAPICTLFUNCPTR_t)hapiBroadLagCreate;
  dapiPortPtr->cmdTable[DAPI_CMD_LAG_PORT_ADD     ] = (HAPICTLFUNCPTR_t)hapiBroadLagPortAdd;
  dapiPortPtr->cmdTable[DAPI_CMD_LAG_PORT_DELETE  ] = (HAPICTLFUNCPTR_t)hapiBroadLagPortDelete;
  /* PTin added */
  dapiPortPtr->cmdTable[DAPI_CMD_INTERNAL_LAG_PORT_ADD     ] = (HAPICTLFUNCPTR_t)hapiBroadLagPortAsyncAdd;
  dapiPortPtr->cmdTable[DAPI_CMD_INTERNAL_LAG_PORT_DELETE  ] = (HAPICTLFUNCPTR_t)hapiBroadLagPortAsyncDelete;
  /* end of PTin added */
  dapiPortPtr->cmdTable[DAPI_CMD_LAG_DELETE       ] = (HAPICTLFUNCPTR_t)hapiBroadLagDelete;
  dapiPortPtr->cmdTable[DAPI_CMD_AD_TRUNK_MODE_SET] = (HAPICTLFUNCPTR_t)hapiBroadADTrunkModeSet;
  dapiPortPtr->cmdTable[DAPI_CMD_LAG_HASHMODE_SET ] = (HAPICTLFUNCPTR_t)hapiBroadLagHashModeSet;
  dapiPortPtr->cmdTable[DAPI_CMD_LAGS_SYNCHRONIZE ] = (HAPICTLFUNCPTR_t)hapiBroadLagsSync;

  return result;

}



/*********************************************************************
*
* @purpose Initializes logical lag cards
*
* @param   L7_ushort16  unitNum - unit number for this logical card
* @param   L7_ushort16  slotNum - slot number for this logical card
* @param   DAPI_t      *dapi_g  - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLagCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, void *data, void *handle)
{

  L7_RC_t         result = L7_SUCCESS;
  DAPI_USP_t            usp;
  BROAD_PORT_t         *hapiPortPtr;
  DAPI_t               *dapi_g = (DAPI_t*)handle;

  hapiBroadLagCritSecEnter();

  usp.unit = dapiUsp->unit;
  usp.slot = dapiUsp->slot;

  for (usp.port=0;usp.port<dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot;usp.port++)
  {
    dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port]->hapiPort = (void *)osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(BROAD_PORT_t));
    hapiPortPtr = (BROAD_PORT_t*)dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port]->hapiPort;
    bzero((L7_char8*)hapiPortPtr,sizeof(BROAD_PORT_t));

    hapiPortPtr->hapiModeparm.lag.tgid = -1;
    hapiPortPtr->hapiModeparm.lag.lastValidTgid = -1;

    hapiPortPtr->port_is_lag = L7_TRUE;
	hapiPortPtr->l2FlushSyncSem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
	if (hapiPortPtr->l2FlushSyncSem == L7_NULL) 
	{
	  LOG_ERROR(usp.port);
	}
  }

  /* Initialize all the ports for the given unit/slot combination */
  if (hapiBroadSlotCtlInit(usp.unit, usp.slot, dapi_g) == L7_FAILURE)
  {
    hapiBroadLagCritSecExit();
    result = L7_FAILURE;
    return result;
  }

#ifdef L7_QOS_PACKAGE
  /* Initialize the QOS package for all ports on this card */
  if (hapiBroadQosCardInit(usp.unit,usp.slot,dapi_g) == L7_FAILURE)
  {
    hapiBroadLagCritSecExit();
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadQosCardInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }
#endif

#ifdef L7_MCAST_PACKAGE
  /* Initialize the QOS package for all ports on this card */
  if (hapiBroadL3McastCardInit(usp.unit,usp.slot,dapi_g) == L7_FAILURE)
  {
    hapiBroadLagCritSecExit();
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadL3McastCardInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }
#endif

  hapiBroadLagCritSecExit();
  return result;

}



/*********************************************************************
*
* @purpose Takes the dapiCmd info and formats a message for each PPE
*
* @param   DAPI_USP_t *usp    - the usp of this lag
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_LAG_CREATE
* @param   void       *data   - lag config data
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLagCreate(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                result = L7_SUCCESS;
  DAPI_PORT_t           *dapiPortPtr;
  BROAD_PORT_t          *hapiPortPtr;
  DAPI_LAG_ENTRY_t      *portLagSet;
  L7_ulong32             entry;
  /* PTin added: trunks (two lines) */
  int                    rc;
  bcm_trunk_t            tid;

  /* Check to ensure this slot is the correct slot for LAG */
  if (dapi_g->unit[usp->unit]->slot[usp->slot]->cardPresent == L7_FALSE)
  {
    /* return failure if the call did not succeed. */
    result = L7_FAILURE;
    return result;
  }

  hapiBroadLagCritSecEnter ();
  /* Get the pointer to the information for this dapiPort and begin initializing
     the necessary values. */
  dapiPortPtr = DAPI_PORT_GET(usp,dapi_g);
  dapiPortPtr->statsEnable   = L7_FALSE;
  dapiPortPtr->type          = L7_IANA_LAG_DESC;

  /* Default Spanning tree state for LAGs is "Not Participating".
  */
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr->stp_state = DAPI_PORT_STP_NOT_PARTICIPATING;

  hapiPortPtr->dhcpSnoopingEnabled         = L7_TRUE; /*L7_FALSE;*/   /* PTin modified: DHCP */
  hapiPortPtr->dynamicArpInspectionTrusted = L7_TRUE;
  hapiPortPtr->ipsgEnabled                 = L7_FALSE;

  /* Initialize all entries in the LAG table so that are set as disabled and no
     ports appear to be members. */
  portLagSet = dapiPortPtr->modeparm.lag.memberSet;

  for (entry=0; entry < L7_MAX_MEMBERS_PER_LAG; entry++)
  {
    portLagSet[entry].inUse    = L7_FALSE;
    portLagSet[entry].usp.unit = 0xFF;
    portLagSet[entry].usp.slot = 0xFF;
    portLagSet[entry].usp.port = 0xFFFF;
  }

  if (usp->port >= L7_MAX_NUM_LAG_INTF)
  {
    LOG_ERROR (usp->port);
  }

  hapiPortPtr->hapiModeparm.lag.numMembers = 0;
  hapiPortPtr->hapiModeparm.lag.hashMode = L7_DOT3AD_HASH_SADA_VLAN_ETYPE_INTF;

  /* PTin added: trunks (code ported from hapiBroadLagPortAsyncAdd()) */
  /* PTin modified: LAGs */
  #if 1
  tid = usp->port;
  LOG_DEBUG(LOG_CTX_PTIN_TRUNKS, "Suggested TID is %u", tid);
  #else
  tid = hapiPortPtr->hapiModeparm.lag.lastValidTgid; // Try to assign the previous tid * BCM_TRUNK_INVALID;
  #endif
  rc = usl_bcmx_trunk_create(usp->port, &tid);
  if (L7_BCMX_OK(rc) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't create trunk ID %d, rv = %d", usp->port, rc);
  }
  LOG_DEBUG(LOG_CTX_PTIN_TRUNKS, "LAG {%d.%d.%d} => New trunk created: tid=%u", usp->unit, usp->slot, usp->port, tid);
  hapiPortPtr->hapiModeparm.lag.tgid = tid;
  BCM_GPORT_TRUNK_SET(hapiPortPtr->bcmx_lport, tid);

  /* Need to apply learn mode now that we have a tgid */
  usl_bcmx_trunk_learn_mode_set(usp->port, tid, hapiPortPtr->locked);

  /* PTin end */

  hapiBroadLagCritSecExit ();
  return result;

}


/*********************************************************************
*
* @purpose Takes the dapiCmd info and formats a message for each PPE
*
* @param   DAPI_USP_t *usp    - the usp of this lag
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_LAG_PORT_ADD
* @param   void       *data   - DAPI_LAG_MGMT_CMD_t.cmdData.lagPortAdd
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLagPortAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  DAPI_LAG_MGMT_CMD_t          *cmdLagPortAdd = (DAPI_LAG_MGMT_CMD_t*)data;
  hapi_broad_lag_async_msg_t   lag_cmd;
  L7_RC_t                      rc;
  L7_ushort16                  entry;
  DAPI_USP_t                   tmpUsp;
  BROAD_PORT_t                 *hapiLagMemberPortPtr;

  lag_cmd.usp = *usp;
  lag_cmd.async_cmd = HAPI_BROAD_LAG_ASYNC_PORT_ADD;

  lag_cmd.asyncData.lagPortAdd.getOrSet = cmdLagPortAdd->cmdData.lagPortAdd.getOrSet;
  lag_cmd.asyncData.lagPortAdd.numOfMembers = cmdLagPortAdd->cmdData.lagPortAdd.numOfMembers;
  lag_cmd.asyncData.lagPortAdd.maxFrameSize = cmdLagPortAdd->cmdData.lagPortAdd.maxFrameSize;
  lag_cmd.asyncData.lagPortAdd.hashMode = cmdLagPortAdd->cmdData.lagPortAdd.hashMode;
  memcpy (lag_cmd.asyncData.lagPortAdd.memberSet, 
          cmdLagPortAdd->cmdData.lagPortAdd.memberSet,
          sizeof (lag_cmd.asyncData.lagPortAdd.memberSet));

  /* Run through this for all the members to add to the LAG. */
  for (entry=0; entry < cmdLagPortAdd->cmdData.lagPortAdd.numOfMembers; entry++)
  {
    tmpUsp.unit = cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry].unit;
    tmpUsp.slot = cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry].slot;
    tmpUsp.port = cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry].port;
	hapiLagMemberPortPtr = HAPI_PORT_GET(&tmpUsp,dapi_g);
	hapiLagMemberPortPtr->hapiModeparm.physical.isLagAddPending = L7_TRUE;
  }

  rc = osapiMessageSend (hapiBroadLagAsyncCmdQueue, 
                         (void*)&lag_cmd,
                         sizeof (lag_cmd), 
                         L7_WAIT_FOREVER, 
                         L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

  /* Signal the L2 Async task that work is pending. */
  (void)hapiBroadL2AsyncTaskSignal();

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Takes the dapiCmd info and formats a message for each PPE
*
* @param   DAPI_USP_t *usp    - the usp of this lag
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_LAG_PORT_ADD
* @param   void       *data   - DAPI_LAG_MGMT_CMD_t.cmdData.lagPortAdd
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLagPortAsyncAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{

  L7_RC_t                 result        = L7_SUCCESS;
  DAPI_LAG_MGMT_CMD_t          *cmdLagPortAdd = (DAPI_LAG_MGMT_CMD_t*)data;
  DAPI_PORT_t                  *lagDapiPortPtr;
  DAPI_PORT_t                  *lagMemberDapiPortPtr;
  DAPI_LAG_ENTRY_t             *lagMemberSet;
  L7_ushort16                   entry;
  L7_ushort16                   searchEntry;
  BROAD_PORT_t                 *hapiLagPortPtr;
  BROAD_PORT_t                 *hapiLagMemberPortPtr;
  L7_ulong32                    port_exists;
  L7_uint32                     memberCount = 0;
  int                           rc;
  bcm_port_discard_t            mode;
  bcm_trunk_add_info_t          bcmTrunkInfo;
  bcmx_lport_t                  lport;
  bcm_trunk_t                   tid;
  usl_bcm_port_priority_t      priority;
  L7_BOOL                       lag_has_members;
  DAPI_USP_t                    newMembers[L7_MAX_MEMBERS_PER_LAG];
  L7_int32                      newMemberCnt = 0;
  bcm_chip_family_t             board_family=0; 
  usl_bcm_port_frame_size_t    maxFrameSize;
  
  LOG_INFO(LOG_CTX_MISC, "This function was called");

  if (!ACCESS_LAG_AT_SDK_LEVEL(DAPI_CMD_INTERNAL_LAG_PORT_ADD))
  {
    LOG_WARNING(LOG_CTX_MISC, "Cannot proceed!");
    return L7_SUCCESS;
  }

  memset(newMembers,0,sizeof(newMembers));
  memset(&bcmTrunkInfo, 0, sizeof(bcmTrunkInfo));

  lagDapiPortPtr = dapi_g->unit[usp->unit]->slot[usp->slot]->port[usp->port];

  /* Check to make sure we are dealing with a valid LAG Port */
  if (IS_PORT_TYPE_LOGICAL_LAG(lagDapiPortPtr) == L7_FALSE)
  {
    result = L7_FAILURE;
    return result;
  }

  /* acquire the drivers vlan database during this operation */
  hapiBroadVlanCritSecEnter();

  /* Acquire the drivers L2MC now (before acquiring LAG semaphore)
     to avoid possible deadlock with other tasks that might be calling
     the L2MC APIs (which may then need to acquire the LAG semaphore). */
  hapiBroadL2McastCritSecEnter();

#ifdef L7_MCAST_PACKAGE
  /* Acquire the semaphore for L3 mcast. The L3 Mcast code is dependent on
     VLAN membership when setting the replication info for an IPMC group.
     Since this routine can modify VLAN membership as a port is added/removed
     from a LAG, we need to ensure that the work here is done before
     the L3 MC code attempts to modify any IPMC group membership. */
  hapiBroadL3McastCritEnter();
#endif

#ifdef L7_LLPF_PACKAGE
  /* Acquire the drivers LLFP now (before acquiring LAG semaphore)
     to avoid possible deadlock with other tasks that might be calling
     the LLFP APIs (which may then need to acquire the LAG semaphore). */
  hapiBroadLlpfSemTake();  
#endif
  /* Acquire the drivers IPSG now (before acquiring LAG semaphore)
     to avoid possible deadlock with other tasks that might be calling
     the IPSG APIs (which may then need to acquire the LAG semaphore). */
  hapiBroadIpsgSemTake();  
  hapiLagPortPtr = HAPI_PORT_GET(usp, dapi_g);

  hapiBroadLagCritSecEnter ();

  lagMemberSet = lagDapiPortPtr->modeparm.lag.memberSet;

  /* Determine whether this LAG already has members.
  */
  if (lagMemberSet[0].inUse == L7_TRUE)
  {
    lag_has_members = L7_TRUE;
  } else
  {
    lag_has_members = L7_FALSE;
  }

  /* Run through this for all the members to add to the LAG. */
  for (entry=0; entry < cmdLagPortAdd->cmdData.lagPortAdd.numOfMembers; entry++)
  {
    /* check if entry already exists */
    port_exists = 0;
    for (searchEntry=0;searchEntry<L7_MAX_MEMBERS_PER_LAG;searchEntry++)
    {
      if ((lagMemberSet[searchEntry].inUse    == L7_TRUE) &&
          (lagMemberSet[searchEntry].usp.unit == cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry].unit) &&
          (lagMemberSet[searchEntry].usp.slot == cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry].slot) &&
          (lagMemberSet[searchEntry].usp.port == cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry].port))
        port_exists = 1;
    }

    if (port_exists)
    {
      DAPI_USP_t tmpUsp;
      tmpUsp.unit = cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry].unit;
      tmpUsp.port = cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry].port;
      tmpUsp.slot = cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry].slot;
          hapiLagMemberPortPtr = HAPI_PORT_GET(&tmpUsp,dapi_g);
          hapiLagMemberPortPtr->hapiModeparm.physical.isLagAddPending = L7_FALSE;
      continue;  /* Process next port */
    }

    /* add the new member to the list of newMembers */
    memcpy(&newMembers[newMemberCnt],&cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry],sizeof(DAPI_USP_t));

    /* set the port to blocked to prevent loops */
    hapiBroadDot1sPortAllGroupsStateSet(&newMembers[newMemberCnt],
                                        BCM_STG_STP_BLOCK,
                                        dapi_g);
    newMemberCnt++;

    /* find first free entry */
    for (searchEntry=0;searchEntry<L7_MAX_MEMBERS_PER_LAG;searchEntry++)
    {
      if (lagMemberSet[searchEntry].inUse == L7_FALSE)
      {
        if (hapiBroadDebugLag)
        {
          LOG_DEBUG(LOG_CTX_PTIN_TRUNKS, "Adding %d.%d.%d to %d.%d.%d, psc %d, (Up Time: %d)",
                    cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry].unit,
                    cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry].slot,
                    cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry].port,
                    usp->unit,
                    usp->slot,
                    usp->port,
                    cmdLagPortAdd->cmdData.lagPortAdd.hashMode,
                    osapiUpTimeRaw());
        }

        lagMemberSet[searchEntry].usp.unit = cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry].unit;
        lagMemberSet[searchEntry].usp.slot = cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry].slot;
        lagMemberSet[searchEntry].usp.port = cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry].port;

        /* Remove the LAG member from all vlans 
        ** This will also remove the port from all L2 multicast groups.
        */
        hapiBroadAddRemovePortFromVlans (&lagMemberSet[searchEntry].usp, 0, dapi_g);

        /* Found a free entry */
        lagMemberSet[searchEntry].inUse    = L7_TRUE;

        hapiLagMemberPortPtr = HAPI_PORT_GET(&lagMemberSet[searchEntry].usp,dapi_g);

        hapiLagMemberPortPtr->hapiModeparm.physical.isMemberOfLag = L7_TRUE;
        hapiLagMemberPortPtr->hapiModeparm.physical.lagUsp.unit   = usp->unit;
        hapiLagMemberPortPtr->hapiModeparm.physical.lagUsp.slot   = usp->slot;
        hapiLagMemberPortPtr->hapiModeparm.physical.lagUsp.port   = usp->port;


        /* If LAG already has members then add the new member to LAGs VLANs.
        ** If this is the first member of the LAG then it will be added to the 
        ** VLANs when the whole LAG is added to the VLANs.
        */
        if (lag_has_members == L7_TRUE)
        {
          hapiBroadAddRemovePortFromLagVlans (&lagMemberSet[searchEntry].usp,
                                              1,
                                              dapi_g);
        }

        break; /* updated lagMemberSet with new entry, exit loop */
      }
    }


    if (searchEntry == L7_MAX_MEMBERS_PER_LAG)
    {
      hapiBroadLagCritSecExit();
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\n%s %d: In %s max members per LAG exceeded\n",
                     __FILE__, __LINE__, __FUNCTION__);

     hapiBroadIpsgSemGive();

#ifdef L7_LLPF_PACKAGE     
     hapiBroadLlpfSemGive();
#endif

#ifdef L7_MCAST_PACKAGE
      hapiBroadL3McastCritExit();
#endif

      hapiBroadL2McastCritSecExit ();

      /* acquire the drivers vlan database during this operation */
      hapiBroadVlanCritSecExit();

      return result;
    }
  }

  /* We need to add LAG to the multicast groups and the VLAN only if 
  ** the LAG didn't already have any members. 
  */
  if (lag_has_members == L7_FALSE)
  {
    /* Add the LAG back to the VLANs.
    */
    hapiBroadLagAddRemoveLagFromVlans (usp, 1, dapi_g);

  }

  /* Set Spanning tree state, port priority and port access parameters for the 
  ** new LAG members.
  */
  for (entry=0; entry < cmdLagPortAdd->cmdData.lagPortAdd.numOfMembers; entry++)
  {
    hapiLagMemberPortPtr = HAPI_PORT_GET(&cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry],dapi_g);

    /* Get the logical port */
    lport = hapiLagMemberPortPtr->bcmx_lport;


    /* update LAG ports maxFrameSize */
    maxFrameSize = cmdLagPortAdd->cmdData.lagPortAdd.maxFrameSize;
    rc = usl_bcmx_port_frame_max_set(lport, maxFrameSize);

    LOG_INFO(LOG_CTX_PTIN_HAPI, "usp{%d,%d,%d}: maxFrameSize %u applied over lport=0x%x (bcm_port=%u): rc=%d",
             usp->unit, usp->slot, usp->port, maxFrameSize, lport, hapiLagMemberPortPtr->bcm_port, rc);

    /* Port Priority 
    */
    priority = hapiLagPortPtr->priority;
    rc = usl_bcmx_port_untagged_priority_set(lport, priority);
    if (L7_BCMX_OK(rc) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't set priority for port 0x%x, rv = %d", lport, rc);
    }

    /* PVID - applied to LAG below, not individual physical ports as they are acquired
    */

    /* Acceptable Frame Type
    */
    switch (hapiLagPortPtr->acceptFrameType)
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

    rc = usl_bcmx_port_discard_set(lport, mode);
    if (L7_BCMX_OK(rc) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't set discard mode for port 0x%x, rv = %d", lport, rc);
    }

    /* Ingress Filtering.
    */
    result = hapiBroadVlanIngressFilterSet(lport,hapiLagPortPtr->ingressFilteringEnabled);

    if (result != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't set ingress filter mode for port 0x%x, rv = %d", lport, result);
    }

    /* Protocol based VLANs */
    hapiBroadProtocolVlanLagPortAddNotify(&cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry], usp, dapi_g);

    /* Port MAC Locking */
    if (hapiLagPortPtr->locked)
    {
      hapiBroadLearnSet(&cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry], BCM_PORT_LEARN_CPU, dapi_g);
    } else
    {
	  /* enable learning on this port */
	  if (hpcSoftwareLearningEnabled () == L7_TRUE)
	        hapiBroadLearnSet(&cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry], 
						(BCM_PORT_LEARN_CPU | BCM_PORT_LEARN_FWD), dapi_g);
	  else
            hapiBroadLearnSet(&cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry], 
							  (BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD), dapi_g);
    }
  }


  hapiBroadGetSystemBoardFamily(&board_family);

  bcmTrunkInfo.dlf_index = 0;
  bcmTrunkInfo.mc_index = 0;
  bcmTrunkInfo.ipmc_index = 0;

  for (searchEntry=0;searchEntry<L7_MAX_MEMBERS_PER_LAG;searchEntry++)
  {
    if (lagMemberSet[searchEntry].inUse == L7_TRUE)
    {
      hapiLagMemberPortPtr = HAPI_PORT_GET(&lagMemberSet[searchEntry].usp,dapi_g);
      bcmTrunkInfo.tm[memberCount] = hapiLagMemberPortPtr->bcm_modid;

      bcmTrunkInfo.tp[memberCount] = BCMX_LPORT_MODPORT(hapiLagMemberPortPtr->bcmx_lport);
      memberCount++;
    }
  }

  bcmTrunkInfo.num_ports = memberCount;

  /*For XGS3 we can distribute unknown unicast and multicast 
           traffic across the lag members*/

  if ((board_family != BCM_FAMILY_DRACO) &&
         (board_family != BCM_FAMILY_TUCANA))
  {    
    bcmTrunkInfo.dlf_index = -1;
    bcmTrunkInfo.mc_index = -1;
    bcmTrunkInfo.ipmc_index = -1;
  }
    
  /* Set PVID for LAG which will update all member ports. */
  result = hapiBroadPortUntaggedVlanSet(usp, hapiLagPortPtr->pvid, dapi_g);
  if (L7_SUCCESS != result)
  {
    return result;
  }
  if (bcmTrunkInfo.num_ports != 0)
  {
    /* if there are no members yet, create the trunk in HW */
/* PTin removed: trunks
 * NOTE
 *  Trunks are being created when LAG is created, meaning that when ports are
 *  added or removed (members), the trunk MUST exist, and if not, it is a FATAL
 *  error.
 */
#if 0
    if (hapiLagPortPtr->hapiModeparm.lag.numMembers == 0)
    {
      rc = usl_bcmx_trunk_create(usp->port, &tid);
      if (L7_BCMX_OK(rc) != L7_TRUE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't create trunk ID %d, rv = %d", usp->port, rc);
      }
      hapiLagPortPtr->hapiModeparm.lag.tgid = tid;
      BCM_GPORT_TRUNK_SET(hapiLagPortPtr->bcmx_lport, tid);

      /* Need to apply learn mode now that we have a tgid */
      usl_bcmx_trunk_learn_mode_set(usp->port, tid, hapiLagPortPtr->locked);
    } else
#endif
    {
      tid = hapiLagPortPtr->hapiModeparm.lag.tgid;
      if (tid == BCM_TRUNK_INVALID)
      {
        LOG_CRITICAL(LOG_CTX_PTIN_TRUNKS, "There trunk ID is invalid!!! CRITICAL!");
      }
    }

    /* Map the application hash value to BCM PSC value. There is a race condition 
     * between DAPI_CMD_LAG_PORT_ADD and DAPI_CMD_LAG_HASHMODE_SET. The LAG async 
     * mechanism could have a port_add pending, while the appliction changes the
     * hash mode. Always, use the value set by the DAPI_CMD_LAG_HASHMODE_SET, and 
     * not as passed in DAPI_CMD_LAG_PORT_ADD.
     */
    hapiBroadLagHashToPsc(hapiLagPortPtr->hapiModeparm.lag.hashMode,
                          &bcmTrunkInfo.psc);

    /* then add the member ports */
    /* PTin modified */
    rc = usl_bcmx_trunk_set(usp->port, tid, &bcmTrunkInfo);
    if (L7_BCMX_OK(rc) != L7_TRUE) {
      LOG_ERR(LOG_CTX_MISC, "Error adding port");
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't set info for trunk ID %d, rv = %d", tid, rc);
    }
    LOG_INFO(LOG_CTX_MISC, "Port added");
    /* PTin modified */
  }

  hapiLagPortPtr->hapiModeparm.lag.numMembers = memberCount;


  /* Flush the FDB of the new physical ports added to the LAG */
  for (entry=0;entry<cmdLagPortAdd->cmdData.lagPortAdd.numOfMembers;entry++)
  {
    DAPI_ADDR_MGMT_CMD_t flushCmd;
    hapiBroadAddrFlush(&cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry],
                       DAPI_CMD_ADDR_FLUSH,
                       (void*)&flushCmd,
                       dapi_g);
  }


  /* for each new member port, notify the flex components that the port now belongs to a LAG */
  for (entry = 0; entry < cmdLagPortAdd->cmdData.lagPortAdd.numOfMembers; entry++)
  {
#ifdef L7_QOS_PACKAGE
    /* Notify QOS component that the port has been added to a LAG.
    */
    hapiBroadQosPortLagAddNotify (&cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry],usp,dapi_g);
#endif
    hapiBroadDoubleVlanLagNotify(usp,&cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry],L7_TRUE,dapi_g); 
    hapiBroadDhcpSnoopingLagAddNotify (&cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry],usp,dapi_g);
    hapiBroadIpsgLagAddNotify (&cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry],usp,dapi_g);
    hapiBroadDynamicArpInspectionLagAddNotify (&cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry],usp,dapi_g);

#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
    hapiBroadDot1adLagNotify(usp,&cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry],L7_TRUE,dapi_g);
    hapiBroadDot1adLagAddNotify(&cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry],
                                   usp,
                                   dapi_g);

#endif
#endif

#ifdef L7_LLPF_PACKAGE
   hapiBroadLlpfLagAddNotify(&cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry],usp,dapi_g);
#endif
    /* Ensure that the physical port has the same mirroring characteristics as the LAG. */
    lagMemberDapiPortPtr = DAPI_PORT_GET(&cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry], dapi_g);
    if ((lagDapiPortPtr->isMirrored != lagMemberDapiPortPtr->isMirrored) ||
        ((lagDapiPortPtr->isMirrored       == L7_TRUE) && 
         (lagMemberDapiPortPtr->isMirrored == L7_TRUE) && 
         (lagDapiPortPtr->mirrorType != lagMemberDapiPortPtr->mirrorType)))
    {
      (void)hapiBroadSystemMirroringSet(&cmdLagPortAdd->cmdData.lagPortAdd.memberSet[entry],
                                        &dapi_g->system->probeUsp,
                                         dapi_g,
                                         lagDapiPortPtr->isMirrored,
                                         lagDapiPortPtr->mirrorType);
    }
  }

  for (entry = 0; entry < newMemberCnt; entry++)
  {
    hapiBroadMgmLagMemberAddNotify (&newMembers[entry], usp, dapi_g);
  }

  hapiBroadMacFilterLagModifiedNotify(usp, dapi_g);


  /* Set the stp state of the new ports to the states of the LAG
  */
  for (entry = 0; entry < newMemberCnt; entry++)
  {
    hapiLagMemberPortPtr = HAPI_PORT_GET(&newMembers[entry],dapi_g);
    if (L7_BRIDGE_SPEC_SUPPORTED == L7_BRIDGE_SPEC_802DOT1D)
    {
      hapiLagPortPtr = HAPI_PORT_GET(usp,dapi_g);

      lport = hapiLagMemberPortPtr->bcmx_lport;

      rc = bcmx_port_stp_set(lport, hapiLagPortPtr->hw_stp_state);
      if (L7_BCMX_OK(rc) != L7_TRUE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't set STP state for port %d, rv = %d", lport, rc);
      }
    } else
    {
      hapiBroadDot1sPortStateCopy(&newMembers[entry], usp, dapi_g);
    }

	/* Clear the Lag add pending flag for the newly added member */ 
	hapiLagMemberPortPtr->hapiModeparm.physical.isLagAddPending = L7_FALSE;
  }

  hapiBroadLagCritSecExit ();

  hapiBroadIpsgSemGive();

#ifdef L7_LLPF_PACKAGE     
  hapiBroadLlpfSemGive();
#endif

#ifdef L7_MCAST_PACKAGE
  hapiBroadL3McastCritExit();
#endif

  hapiBroadL2McastCritSecExit ();

  hapiBroadVlanCritSecExit();

  return result;
}


/*********************************************************************
*
* @purpose Takes the dapiCmd info and formats a message for each PPE
*
* @param   DAPI_USP_t *usp    - the usp of this lag
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_LAG_PORT_ADD
* @param   void       *data   - DAPI_LAG_MGMT_CMD_t.cmdData.lagPortAdd
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLagPortDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  DAPI_LAG_MGMT_CMD_t          *cmdLagPortDelete = (DAPI_LAG_MGMT_CMD_t*)data;
  hapi_broad_lag_async_msg_t   lag_cmd;
  L7_RC_t     rc;

  lag_cmd.usp = *usp;
  lag_cmd.async_cmd = HAPI_BROAD_LAG_ASYNC_PORT_DELETE;

  lag_cmd.asyncData.lagPortDelete.getOrSet = cmdLagPortDelete->cmdData.lagPortDelete.getOrSet;
  lag_cmd.asyncData.lagPortDelete.numOfMembers = cmdLagPortDelete->cmdData.lagPortDelete.numOfMembers;
  lag_cmd.asyncData.lagPortDelete.maxFrameSize = cmdLagPortDelete->cmdData.lagPortDelete.maxFrameSize;
  lag_cmd.asyncData.lagPortDelete.hashMode = cmdLagPortDelete->cmdData.lagPortDelete.hashMode;
  memcpy (lag_cmd.asyncData.lagPortDelete.memberSet, 
          cmdLagPortDelete->cmdData.lagPortDelete.memberSet,
          sizeof (lag_cmd.asyncData.lagPortDelete.memberSet));

  rc = osapiMessageSend (hapiBroadLagAsyncCmdQueue, 
                         (void*)&lag_cmd,
                         sizeof (lag_cmd), 
                         L7_WAIT_FOREVER, 
                         L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    LOG_ERROR (rc);
  }

  /* Signal the L2 Async task that work is pending. */
  (void)hapiBroadL2AsyncTaskSignal();

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Takes the dapiCmd info and formats a message for each PPE
*
* @param   DAPI_USP_t *usp    - the usp of this lag
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_LAG_PORT_DELETE
* @param   void       *data   - DAPI_LAG_MGMT_CMD_t.cmdData.lagPortDelete
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLagPortAsyncDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                 result           = L7_SUCCESS;
  DAPI_LAG_MGMT_CMD_t          *cmdLagPortDelete = (DAPI_LAG_MGMT_CMD_t*)data;
  DAPI_PORT_t                  *lagDapiPortPtr;
  DAPI_PORT_t                  *lagMemberDapiPortPtr;
  BROAD_PORT_t                 *hapiLagPortPtr;
  DAPI_LAG_ENTRY_t             *lagMemberSet;
  BROAD_PORT_t                 *hapiLagMemberPortPtr;
  L7_ushort16                   entry;
  L7_ushort16                   searchEntry = 0;
  L7_uint32                     memberCount = 0;
  int                           rc;
  bcm_port_discard_t            mode;
  bcm_trunk_add_info_t          bcmTrunkInfo;
  bcmx_lport_t                  lport;
  bcm_trunk_t                   tid;
  usl_bcm_port_priority_t      priority;
  L7_BOOL                       member_found;
  L7_BOOL                       removing_first_member;
  bcm_chip_family_t             board_family=0; 
  usl_bcm_port_frame_size_t    maxFrameSize;
  L7_BOOL cond0, cond1, cond2, cond3, cond;     /* PTin added: BUG correction */

  LOG_TRACE(LOG_CTX_MISC, "This function was called: usp {%d,%d,%d}", usp->unit, usp->slot, usp->port);

  if (!ACCESS_LAG_AT_SDK_LEVEL(DAPI_CMD_INTERNAL_LAG_PORT_DELETE) &&
      !ACCESS_LAG_AT_SDK_LEVEL(DAPI_CMD_LAG_DELETE))
  {
    LOG_WARNING(LOG_CTX_MISC, "Cannot proceed!");
    return L7_SUCCESS;
  }

  memset(&bcmTrunkInfo, 0, sizeof(bcmTrunkInfo));

  lagDapiPortPtr = dapi_g->unit[usp->unit]->slot[usp->slot]->port[usp->port];
  hapiLagPortPtr = dapi_g->unit[usp->unit]->slot[usp->slot]->port[usp->port]->hapiPort;

  if (IS_PORT_TYPE_LOGICAL_LAG(lagDapiPortPtr) == L7_FALSE)
    LOG_ERROR (lagDapiPortPtr->type);

  lagMemberSet = lagDapiPortPtr->modeparm.lag.memberSet;
  member_found = L7_FALSE;
  removing_first_member = L7_FALSE;

  /* Before we start messing with the LAG make sure that the application is trying to
  ** delete is a member of the LAG. If the port is not a member then ignore this command.
  */ 
  for (entry=0;entry<cmdLagPortDelete->cmdData.lagPortDelete.numOfMembers;entry++)
  {
    for (searchEntry=0;searchEntry<L7_MAX_MEMBERS_PER_LAG;searchEntry++)
    {
      /* PTin added: BUG! The 'if' with the 4 conditions together was not correctly asserted! */
      cond0 = (lagMemberSet[searchEntry].inUse    == L7_TRUE);
      cond1 = (lagMemberSet[searchEntry].usp.unit == cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].unit);
      cond2 = (lagMemberSet[searchEntry].usp.slot == cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].slot);
      cond3 = (lagMemberSet[searchEntry].usp.port == cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].port);
      cond  = cond0 && cond1 && cond2 && cond3;

      if (hapiBroadDebugLag)
      {
        LOG_DEBUG(LOG_CTX_PTIN_TRUNKS, "lagPortDelete.memberSet[%d]={%d,%d,%d}", entry,
                  cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].unit, cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].slot, cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].port);
        LOG_DEBUG(LOG_CTX_PTIN_TRUNKS, "lagMemberSet[%u].usp={%u,%u,%u}  cmdLagPortDelete->cmdData.lagPortDelete.memberSet[%u]={%u,%u,%u}",
                  searchEntry,lagMemberSet[searchEntry].usp.unit,lagMemberSet[searchEntry].usp.slot,lagMemberSet[searchEntry].usp.port,
                  entry,cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].unit,cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].slot,cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].port);
        LOG_DEBUG(LOG_CTX_PTIN_TRUNKS, "(lagMemberSet[%d].inUse    == L7_TRUE): %u\n",
                  searchEntry, cond0);
        LOG_DEBUG(LOG_CTX_PTIN_TRUNKS, "(lagMemberSet[%d].usp.unit == cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].unit): %u",
                  searchEntry, cond1);
        LOG_DEBUG(LOG_CTX_PTIN_TRUNKS, "(lagMemberSet[%d].usp.slot == cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].slot): %u",
                  searchEntry, cond2);
        LOG_DEBUG(LOG_CTX_PTIN_TRUNKS, "(lagMemberSet[%d].usp.port == cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].port): %u",
                  searchEntry, cond3);
        LOG_DEBUG(LOG_CTX_PTIN_TRUNKS, "Total: %u\n", __FUNCTION__, __LINE__, cond);
      }

      /* This code was never asserted as true, although all the conditions were right... weird! */
//    if ((lagMemberSet[searchEntry].inUse    == L7_TRUE) &&
//        (lagMemberSet[searchEntry].usp.unit == cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].unit) &&
//        (lagMemberSet[searchEntry].usp.slot == cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].slot) &&
//        (lagMemberSet[searchEntry].usp.port == cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].port))
      if (cond) /* This way the if is correctly asserted. Weird... */
      /* PTin end */
      {
        LOG_TRACE(LOG_CTX_PTIN_TRUNKS, "lagMemberSet[%d].usp={%d,%d,%d}",
                  searchEntry, lagMemberSet[searchEntry].usp.unit, lagMemberSet[searchEntry].usp.slot,lagMemberSet[searchEntry].usp.port);

        /* PTin added: debug */
        if (hapiBroadDebugLag)
        {
          LOG_DEBUG(LOG_CTX_PTIN_TRUNKS, "**** Found LAG: searchEntry=%u", searchEntry);
          LOG_DEBUG(LOG_CTX_PTIN_TRUNKS, "lagMemberSet[%d].usp={%d,%d,%d}",
                    searchEntry, lagMemberSet[searchEntry].usp.unit, lagMemberSet[searchEntry].usp.slot,lagMemberSet[searchEntry].usp.port);
        }
        /* PTin end */
        if (searchEntry == 0)
        {
          removing_first_member = L7_TRUE;
        }
        member_found = L7_TRUE;
        LOG_TRACE(LOG_CTX_PTIN_TRUNKS, "lagMemberSet[%d].usp={%d,%d,%d}",
                  searchEntry, lagMemberSet[searchEntry].usp.unit, lagMemberSet[searchEntry].usp.slot,lagMemberSet[searchEntry].usp.port);
        break;
      }
    }

    if (member_found == L7_TRUE)
    {
      LOG_TRACE(LOG_CTX_PTIN_TRUNKS, "lagMemberSet[%d].usp={%d,%d,%d}",
                searchEntry, lagMemberSet[searchEntry].usp.unit, lagMemberSet[searchEntry].usp.slot,lagMemberSet[searchEntry].usp.port);
      break;
    }
  }

  if (member_found == L7_FALSE)
  {
    LOG_TRACE(LOG_CTX_PTIN_TRUNKS, "Member not found");
    return L7_SUCCESS;
  }

  LOG_TRACE(LOG_CTX_PTIN_TRUNKS, "lagMemberSet[%d].usp={%d,%d,%d}",
            searchEntry, lagMemberSet[searchEntry].usp.unit, lagMemberSet[searchEntry].usp.slot,lagMemberSet[searchEntry].usp.port);

  /* acquire the drivers vlan database during this operation */
  hapiBroadVlanCritSecEnter();
  /* Acquire the drivers L2MC now (before acquiring LAG semaphore)
     to avoid possible deadlock with other tasks that might be calling
     the L2MC APIs (which may then need to acquire the LAG semaphore). */
  hapiBroadL2McastCritSecEnter();

#ifdef L7_MCAST_PACKAGE
  /* Acquire the semaphore for L3 mcast. The L3 Mcast code is dependent on
     VLAN membership when setting the replication info for an IPMC group.
     Since this routine can modify VLAN membership as a port is added/removed
     from a LAG, we need to ensure that the work here is done before
     the L3 MC code attempts to modify any IPMC group membership. */
  hapiBroadL3McastCritEnter();
#endif

#ifdef L7_LLPF_PACKAGE
  /* Acquire the drivers LLFP now (before acquiring LAG semaphore)
     to avoid possible deadlock with other tasks that might be calling
     the LLFP APIs (which may then need to acquire the LAG semaphore). */
  hapiBroadLlpfSemTake();  
#endif
  /* Acquire the drivers IPSG now (before acquiring LAG semaphore)
     to avoid possible deadlock with other tasks that might be calling
     the IPSG APIs (which may then need to acquire the LAG semaphore). */
  hapiBroadIpsgSemTake();

  hapiBroadLagCritSecEnter ();

  if (removing_first_member == L7_TRUE)
  {
    /* Remove LAG from all VLANs. If there are any members left after the
    ** delete operation then we will put it back into the VLANs.
    ** We don't need to delete the LAG from the VLAN if the first member 
    ** is untouched.
    */
    hapiBroadLagAddRemoveLagFromVlans (usp, 0, dapi_g);
  }
  else if (lagMemberSet[searchEntry].inUse == L7_TRUE)    /* PTin modified: trunks */
  {
    /* Remove port from LAGs VLANs.
    */
    hapiBroadAddRemovePortFromLagVlans (&lagMemberSet[searchEntry].usp,
                                        0,
                                        dapi_g);
  }

/* IMPORTANT!!!
 * PTin removed: this code is redundant! 'entry' and 'searchEntry' are already determined!!!
 */
#if 1
  lagMemberSet = lagDapiPortPtr->modeparm.lag.memberSet;

  for (entry=0;entry<cmdLagPortDelete->cmdData.lagPortDelete.numOfMembers;entry++)
  {
    for (searchEntry=0;searchEntry<L7_MAX_MEMBERS_PER_LAG;searchEntry++)
    {
      /* PTin added: debug (but influences the correct execution of the 'if'!!! (DO NOT REMOVE!) */
      if (hapiBroadDebugLag)
      {
        cond0 = (lagMemberSet[searchEntry].inUse    == L7_TRUE);
        cond1 = (lagMemberSet[searchEntry].usp.unit == cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].unit);
        cond2 = (lagMemberSet[searchEntry].usp.slot == cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].slot);
        cond3 = (lagMemberSet[searchEntry].usp.port == cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].port);
        cond  = cond0 && cond1 && cond2 && cond3;

        LOG_DEBUG(LOG_CTX_PTIN_TRUNKS, "lagMemberSet[%u].usp={%u,%u,%u}  cmdLagPortDelete->cmdData.lagPortDelete.memberSet[%u]={%u,%u,%u}",
                searchEntry,lagMemberSet[searchEntry].usp.unit,lagMemberSet[searchEntry].usp.slot,lagMemberSet[searchEntry].usp.port,
                entry,cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].unit,cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].slot,cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].port);

        LOG_DEBUG(LOG_CTX_PTIN_TRUNKS, "(lagMemberSet[searchEntry].inUse    == L7_TRUE): %u",
                cond0);
        LOG_DEBUG(LOG_CTX_PTIN_TRUNKS, "(lagMemberSet[searchEntry].usp.unit == cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].unit): %u",
                cond1);
        LOG_DEBUG(LOG_CTX_PTIN_TRUNKS, "(lagMemberSet[searchEntry].usp.slot == cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].slot): %u",
                cond2);
        LOG_DEBUG(LOG_CTX_PTIN_TRUNKS, "(lagMemberSet[searchEntry].usp.port == cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].port): %u",
                cond3);
        LOG_DEBUG(LOG_CTX_PTIN_TRUNKS, "Total: %u",cond);
      }
      /* PTin end */
      if ((lagMemberSet[searchEntry].inUse    == L7_TRUE) &&
          (lagMemberSet[searchEntry].usp.unit == cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].unit) &&
          (lagMemberSet[searchEntry].usp.slot == cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].slot) &&
          (lagMemberSet[searchEntry].usp.port == cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].port))
      {
#endif
        if (hapiBroadDebugLag)
        {
          LOG_DEBUG(LOG_CTX_PTIN_TRUNKS, "Removing %d.%d.%d from %d.%d.%d (Up Time: %d)",
                    cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].unit,
                    cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].slot,
                    cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry].port,
                    usp->unit,
                    usp->slot,
                    usp->port,
          osapiUpTimeRaw());
        }

        hapiLagMemberPortPtr = HAPI_PORT_GET(&lagMemberSet[searchEntry].usp, dapi_g);

        hapiLagMemberPortPtr->hapiModeparm.physical.isMemberOfLag = L7_FALSE;
        hapiLagMemberPortPtr->hapiModeparm.physical.lagUsp.unit   = 0xFF;
        hapiLagMemberPortPtr->hapiModeparm.physical.lagUsp.slot   = 0xFF;
        hapiLagMemberPortPtr->hapiModeparm.physical.lagUsp.port   = 0xFFFF;

        lagMemberSet[searchEntry].inUse = L7_FALSE;
        lagMemberSet[searchEntry].usp.unit = 0xFF;
        lagMemberSet[searchEntry].usp.slot = 0xFF;
        lagMemberSet[searchEntry].usp.port = 0xFFFF;
/* PTin removed: trunks (belongs to the previous removed block) */
#if 1
        break;
      }
    }
  }
#endif
  if ((removing_first_member == L7_TRUE) &&
       (hapiLagPortPtr->hapiModeparm.lag.isolatePending == L7_FALSE))
  {
    DAPI_LAG_ENTRY_t temp_entry;

    memcpy (&temp_entry, &lagMemberSet[0], sizeof (DAPI_LAG_ENTRY_t));

    /* If we removed the first member then we need to reshuffle remaning members.
    ** Its OK not to have any members in the list.
    */
    for (searchEntry = 1; searchEntry < L7_MAX_MEMBERS_PER_LAG; searchEntry++)
    {
      if (lagMemberSet[searchEntry].inUse == L7_TRUE)
      {
        memcpy (&lagMemberSet[0], 
                &lagMemberSet[searchEntry], 
                sizeof (DAPI_LAG_ENTRY_t));

        memcpy (&lagMemberSet[searchEntry],
                &temp_entry,
                sizeof (DAPI_LAG_ENTRY_t));

        break;
      }
    }
    /* Add the LAG back to it's VLANs
    */
    hapiBroadLagAddRemoveLagFromVlans (usp, 1, dapi_g);
  }

  bcmTrunkInfo.mc_index = 0;
  bcmTrunkInfo.dlf_index = 0;
  bcmTrunkInfo.ipmc_index = 0;

  for (searchEntry=0;searchEntry<L7_MAX_MEMBERS_PER_LAG;searchEntry++)
  {
    if (lagMemberSet[searchEntry].inUse == L7_TRUE)
    {
      hapiLagMemberPortPtr = HAPI_PORT_GET(&lagMemberSet[searchEntry].usp, dapi_g);
      bcmTrunkInfo.tm[memberCount] = hapiLagMemberPortPtr->bcm_modid;
      bcmTrunkInfo.tp[memberCount] = BCMX_LPORT_MODPORT(hapiLagMemberPortPtr->bcmx_lport);

      memberCount++;
    }
  }

  bcmTrunkInfo.num_ports = memberCount;

  hapiBroadGetSystemBoardFamily(&board_family);

  /*For XGS3 we can distribute unknown unicast and multicast 
           traffic across the lag members*/
 if ((board_family != BCM_FAMILY_DRACO) &&
         (board_family != BCM_FAMILY_TUCANA))
  {    
    bcmTrunkInfo.mc_index = -1;
    bcmTrunkInfo.dlf_index = -1;
    bcmTrunkInfo.ipmc_index = -1;
  }

  /* Map the application hash value to BCM PSC value */
  hapiBroadLagHashToPsc(cmdLagPortDelete->cmdData.lagPortDelete.hashMode,
                        &bcmTrunkInfo.psc);

  tid = hapiLagPortPtr->hapiModeparm.lag.tgid;

  /* if there were previously members in this lag */
  if (hapiLagPortPtr->hapiModeparm.lag.numMembers > 0)
  {
    /* if there are now no members in this lag, delete it */
    if (memberCount == 0)
    {
/* PTin removed: trunks (the trunk will be removed, but created immediatly after) */
//#ifdef L7_WIRELESS_PACKAGE
//       (void)hapiBroadL2TunnelLagDeleteNotify(usp, dapi_g);
//#endif
      rc = usl_bcmx_trunk_destroy(usp->port, tid);
      if (L7_BCMX_OK(rc) != L7_TRUE) {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't destroy trunk ID %d, rv = %d", usp->port, rc);
        LOG_ERR(LOG_CTX_MISC, "Error removing port");
      }
      LOG_INFO(LOG_CTX_MISC, "Port removed");

      /* PTin added: trunks (code ported from hapiBroadLagPortAsyncAdd()) */
      LOG_DEBUG(LOG_CTX_PTIN_TRUNKS, "Trunk ID# %d was temporarily removed", tid);
      rc = usl_bcmx_trunk_create(usp->port, &tid);
      if (L7_BCMX_OK(rc) != L7_TRUE) {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't create trunk ID %d, rv = %d", usp->port, rc);
      }
      LOG_DEBUG(LOG_CTX_PTIN_TRUNKS, "Trunk ID# %d is alive again :-)", tid);
      hapiLagPortPtr->hapiModeparm.lag.tgid = tid;
      BCM_GPORT_TRUNK_SET(hapiLagPortPtr->bcmx_lport, tid);

      /* Need to apply learn mode now that we have a tgid */
      usl_bcmx_trunk_learn_mode_set(usp->port, tid, hapiLagPortPtr->locked);


      /* PTin end */

/* PTin removed: trunks (the trunk wasn't removed anyway) */
//    /* Set it to something invalid */
//    hapiLagPortPtr->hapiModeparm.lag.isolatePending = L7_FALSE;
//    hapiLagPortPtr->hapiModeparm.lag.lastValidTgid = hapiLagPortPtr->hapiModeparm.lag.tgid;
//    hapiLagPortPtr->hapiModeparm.lag.tgid = -1;
//    hapiLagPortPtr->bcmx_lport = 0;
    }
    /* otherwise set the membership to the remaining members */
    else
    {
      rc = usl_bcmx_trunk_set(usp->port, tid, &bcmTrunkInfo);
      if (L7_BCMX_OK(rc) != L7_TRUE) {
        LOG_ERR(LOG_CTX_MISC, "Error removing port");
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't set trunk info for ID %d, rv = %d", tid, rc);
      }
      LOG_INFO(LOG_CTX_MISC, "Port removed");
    }
  }

  hapiLagPortPtr->hapiModeparm.lag.numMembers = memberCount;

  /* Set Spanning tree state, port priority and port access parameters as part
  ** of releasing the ports from the LAG.
  */
  for (entry=0; entry < cmdLagPortDelete->cmdData.lagPortDelete.numOfMembers; entry++)
  {
    hapiLagMemberPortPtr = HAPI_PORT_GET(&cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry],dapi_g);

    /* Get the logical port */
    lport = hapiLagMemberPortPtr->bcmx_lport;

    /* update port config maxFrameSize */
    #if 1
    maxFrameSize = hapiLagMemberPortPtr->max_frame_size;    /* PTin modified: maxFrameSize from physical port */
    #else
    maxFrameSize = cmdLagPortDelete->cmdData.lagPortDelete.maxFrameSize;
    #endif
    rc = usl_bcmx_port_frame_max_set(lport, maxFrameSize);

    LOG_INFO(LOG_CTX_PTIN_HAPI, "usp{%d,%d,%d}: maxFrameSize %u applied over lport=0x%x (bcm_port=%u): rc=%d",
             usp->unit, usp->slot, usp->port, maxFrameSize, lport, hapiLagMemberPortPtr->bcm_port, rc);

    if (L7_BCMX_OK(rc) != L7_TRUE)
    {
      LOG_ERR(LOG_CTX_PTIN_TRUNKS, "Failed to set max frame on %d",lport);

      hapiBroadLagCritSecExit ();

      hapiBroadIpsgSemGive();

#ifdef L7_LLPF_PACKAGE     
      hapiBroadLlpfSemGive();
#endif
#ifdef L7_MCAST_PACKAGE
      hapiBroadL3McastCritExit();
#endif

      hapiBroadL2McastCritSecExit ();

      hapiBroadVlanCritSecExit();
      return L7_FAILURE;
    }

    /* Add the LAG member to all it's vlans 
    */
    hapiBroadAddRemovePortFromVlans (&cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry], 1, dapi_g);

    /* Spanning Tree.
    */
    if (L7_BRIDGE_SPEC_SUPPORTED == L7_BRIDGE_SPEC_802DOT1D)
    {
      rc = bcmx_port_stp_set(hapiLagMemberPortPtr->bcmx_lport, hapiLagMemberPortPtr->hw_stp_state);
      if (L7_BCMX_OK(rc) != L7_TRUE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't set STP state for port 0x%x, rv = %d", hapiLagMemberPortPtr->bcmx_lport, rc);
      }
    } else
    {
      hapiBroadDot1sPortStateCopy(&cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry],
                                  &cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry],
                                  dapi_g);
    }

    /* Port Priority 
    */
    priority = hapiLagMemberPortPtr->priority;
    rc = usl_bcmx_port_untagged_priority_set(lport, priority);
    if (L7_BCMX_OK(rc) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't set priority for port 0x%x, rv = %d", lport, rc);
    }

    /* PVID
    */
    result = hapiBroadPortUntaggedVlanSet(&cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry],
                                          hapiLagMemberPortPtr->pvid, dapi_g);
    if (L7_SUCCESS != result)
    {
      LOG_ERR(LOG_CTX_PTIN_TRUNKS, "Failed to set pvid \n");
      hapiBroadLagCritSecExit ();

      hapiBroadIpsgSemGive();

#ifdef L7_LLPF_PACKAGE     
      hapiBroadLlpfSemGive();
#endif

#ifdef L7_MCAST_PACKAGE
      hapiBroadL3McastCritExit();
#endif

      hapiBroadL2McastCritSecExit ();

      hapiBroadVlanCritSecExit();
      return result;
    }

    /* Acceptable Frame Type
    */

    switch (hapiLagMemberPortPtr->acceptFrameType)
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

    rc = usl_bcmx_port_discard_set(lport, mode);
    if (L7_BCMX_OK(rc) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't set discard mode for port 0x%x, rv = %d", lport, rc);
    }

    /* Ingress Filtering.
    */
    result = hapiBroadVlanIngressFilterSet(lport, hapiLagMemberPortPtr->ingressFilteringEnabled);

    if (result != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't set ingress filtering mode for port 0x%x, rv = %d", lport, result);
    }

    /* Protocol based VLANs */
    hapiBroadProtocolVlanLagPortDeleteNotify(&cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry], usp, dapi_g);

    if (hapiLagMemberPortPtr->cpBlocked == L7_TRUE) 
    {
       /* Configure hw to drop unknown SA on this port */
       hapiBroadLearnSet(&cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry], 0, dapi_g);
    }
    /* Check the port-security mode of the port */
    else if ((hapiLagMemberPortPtr->locked == L7_TRUE) ||
             (hapiLagMemberPortPtr->cpEnabled == L7_TRUE))
    {
      hapiBroadLearnSet(&cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry], BCM_PORT_LEARN_CPU, dapi_g);
    } else
    {
		if (hpcSoftwareLearningEnabled () == L7_TRUE)
		    hapiBroadLearnSet(&cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry], 
						  (BCM_PORT_LEARN_CPU | BCM_PORT_LEARN_FWD), dapi_g);
		else
			hapiBroadLearnSet(&cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry], 
							  (BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD), dapi_g);
    }
  }

  /* for each member port, notify the flex components that the port is removed from the LAG */
  for (entry = 0; entry < cmdLagPortDelete->cmdData.lagPortDelete.numOfMembers; entry++)
  {
#ifdef L7_QOS_PACKAGE
    /* Notify QOS component that the port has been removed from the LAG.
    */
    hapiBroadQosPortLagDeleteNotify (&cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry],usp,dapi_g);
#endif
    hapiBroadDoubleVlanLagNotify(usp,&cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry],L7_FALSE,dapi_g); 
    hapiBroadDhcpSnoopingLagDeleteNotify(&cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry],usp,dapi_g);
    hapiBroadIpsgLagDeleteNotify(&cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry],usp,dapi_g);
    hapiBroadDynamicArpInspectionLagDeleteNotify(&cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry],usp,dapi_g);

#ifdef L7_LLPF_PACKAGE
    hapiBroadLlpfLagDeleteNotify(&cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry],usp,dapi_g);
#endif
#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
    hapiBroadDot1adLagNotify(usp,&cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry],L7_FALSE,dapi_g);
    hapiBroadDot1adLagDeleteNotify(&cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry],
                                   usp,
                                   dapi_g);
#endif
#endif

    hapiBroadMgmLagMemberRemoveNotify (&cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry],usp,dapi_g);

    /* Restore the mirroring characteristcs of the physical port. */
    lagMemberDapiPortPtr = DAPI_PORT_GET(&cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry], dapi_g);
    if ((lagDapiPortPtr->isMirrored != lagMemberDapiPortPtr->isMirrored) ||
        ((lagDapiPortPtr->isMirrored       == L7_TRUE) && 
         (lagMemberDapiPortPtr->isMirrored == L7_TRUE) && 
         (lagDapiPortPtr->mirrorType != lagMemberDapiPortPtr->mirrorType)))
    {
      (void)hapiBroadSystemMirroringSet(&cmdLagPortDelete->cmdData.lagPortDelete.memberSet[entry],
                                        &dapi_g->system->probeUsp,
                                         dapi_g,
                                         lagMemberDapiPortPtr->isMirrored,
                                         lagMemberDapiPortPtr->mirrorType);
    }
  }

  hapiBroadMacFilterLagModifiedNotify(usp, dapi_g);

  hapiBroadLagCritSecExit ();

  hapiBroadIpsgSemGive();

#ifdef L7_LLPF_PACKAGE     
      hapiBroadLlpfSemGive();
#endif

#ifdef L7_MCAST_PACKAGE
  hapiBroadL3McastCritExit();
#endif

  hapiBroadL2McastCritSecExit ();

  hapiBroadVlanCritSecExit();

  return result;

}



/*********************************************************************
*
* @purpose Takes the dapiCmd info and formats a message for each PPE
*
* @param   DAPI_USP_t *usp    - the usp of this lag
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_LAG_DELETE
* @param   void       *data   - DAPI_LAG_MGMT_CMD_t.cmdData.lagDelete
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLagDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{

  L7_RC_t          result = L7_SUCCESS;
//  DAPI_LAG_MGMT_CMD_t    cmdLagDelete;
  DAPI_PORT_t           *dapiPortPtr;
  DAPI_LAG_ENTRY_t      *portLagSet;
  L7_ushort16            entry;
  L7_ushort16            intfCount;
  DAPI_USP_t             memberSetBuf[L7_MAX_MEMBERS_PER_LAG];
  DAPI_LAG_MGMT_CMD_t    cmdLagPortDelete;

  /* PTin added: trunks */
  int           rc;
  BROAD_PORT_t *hapiLagPortPtr;

  LOG_INFO(LOG_CTX_MISC, "This function was called");

  dapiPortPtr    = dapi_g->unit[usp->unit]->slot[usp->slot]->port[usp->port];
  hapiLagPortPtr = dapi_g->unit[usp->unit]->slot[usp->slot]->port[usp->port]->hapiPort;
  /* PTin end */

/* PTin: old function */
#if 0
  dapiPortPtr = dapi_g->unit[usp->unit]->slot[usp->slot]->port[usp->port];

  /* Check to ensure this is a valid LAG port and return failure if not. */
  if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_FALSE)
  {
    result = L7_FAILURE;
    return result;
  }

  /* Find out how many ports are currently configured as LAG members. */
  portLagSet = dapiPortPtr->modeparm.lag.memberSet;
  intfCount  = 0;
  hapiBroadLagCritSecEnter ();
  for (entry=0; entry < L7_MAX_MEMBERS_PER_LAG; entry++)
  {
    if (portLagSet[entry].inUse == L7_TRUE)
    {
      memberSetBuf[intfCount] = portLagSet[entry].usp;
      intfCount++;
    }
  }
  hapiBroadLagCritSecExit ();

  /* Call function to delete the Lag Port */
  cmdLagDelete.cmdData.lagPortDelete.numOfMembers = intfCount;
  cmdLagDelete.cmdData.lagPortDelete.memberSet    = memberSetBuf;

  dapiCtl(usp,DAPI_CMD_LAG_PORT_DELETE,(void *)&cmdLagDelete);
#endif

  /* PTin added: trunks */

  /* Check to ensure this is a valid LAG port and return failure if not. */
  if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_FALSE)
  {
    result = L7_FAILURE;
    return result;
  }

  /* Going to remove LAG ports */
  #if 1
  memset(&cmdLagPortDelete, 0x00, sizeof(cmdLagPortDelete));
  memset(memberSetBuf, 0x00, sizeof(memberSetBuf));

  /* Find out how many ports are currently configured as LAG members. */
  portLagSet = dapiPortPtr->modeparm.lag.memberSet;
  intfCount = 0;
  for (entry=0; entry < L7_MAX_MEMBERS_PER_LAG; entry++)
  {
    if (portLagSet[entry].inUse == L7_TRUE)
    {
      memberSetBuf[intfCount].unit = portLagSet[entry].usp.unit;
      memberSetBuf[intfCount].slot = portLagSet[entry].usp.slot;
      memberSetBuf[intfCount].port = portLagSet[entry].usp.port;

      intfCount++;
      LOG_INFO(LOG_CTX_MISC, "Going to remove port {%d,%d,%d}...",memberSetBuf[intfCount].unit, memberSetBuf[intfCount].slot, memberSetBuf[intfCount].port);
    }
  }
  cmdLagPortDelete.cmdData.lagPortDelete.getOrSet     = DAPI_CMD_SET;
  cmdLagPortDelete.cmdData.lagPortDelete.memberSet    = memberSetBuf;
  cmdLagPortDelete.cmdData.lagPortDelete.numOfMembers = intfCount;

  if (hapiBroadLagPortAsyncDelete(usp, cmd, &cmdLagPortDelete, dapi_g) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_MISC, "Error removing LAG ports!");
    return L7_FAILURE;
  }
  #endif
  LOG_INFO(LOG_CTX_MISC, "Going to destroy LAG...");

  do
  {
    /* Find out how many ports are currently configured as LAG members. */
    portLagSet = dapiPortPtr->modeparm.lag.memberSet;
    intfCount  = 0;
    hapiBroadLagCritSecEnter ();
    for (entry=0; entry < L7_MAX_MEMBERS_PER_LAG; entry++)
    {
      if (portLagSet[entry].inUse == L7_TRUE)
      {
        memberSetBuf[intfCount] = portLagSet[entry].usp;
        intfCount++;
      }
    }
    hapiBroadLagCritSecExit ();
    
    if (intfCount != 0)
    {
      LOG_NOTICE(LOG_CTX_PTIN_TRUNKS, "LAG (tid=%u) still has %u members! Waiting 250ms...", usp->port, intfCount);
      osapiSleepMSec(250);

      /* NOTE: when a LAG is removed and it has active ports, it is issued a command
       * to remove them. However, the ports removal runs on a separate thread and thus
       * this function concurs with it. So it may happen that this function runs
       * before the ports removal, and thus it is necessary to wait until there are
       * no ports attached to the LAG */ 
      continue;
    }
  
    /* Delete LAG */
    hapiBroadLagCritSecEnter ();
    rc = usl_bcmx_trunk_destroy(usp->port, hapiLagPortPtr->hapiModeparm.lag.tgid);
    if (L7_BCMX_OK(rc) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't destroy trunk ID %d, rv = %d", usp->port, rc);
    }
    LOG_DEBUG(LOG_CTX_PTIN_TRUNKS, "Trunk ID# %d was successfully removed", hapiLagPortPtr->hapiModeparm.lag.tgid);

    /* Set it to something invalid */
    hapiLagPortPtr->hapiModeparm.lag.isolatePending = L7_FALSE;
    hapiLagPortPtr->hapiModeparm.lag.lastValidTgid = hapiLagPortPtr->hapiModeparm.lag.tgid;
    hapiLagPortPtr->hapiModeparm.lag.tgid = -1;
    hapiLagPortPtr->bcmx_lport = 0;
  
    hapiBroadLagCritSecExit ();

    break;

  } while (1);
  /* PTin end */

  return result;

}



/*********************************************************************
*
* @purpose Enables 802.3AD trunking mode.
*
* @param   DAPI_USP_t *usp    - the usp of this lag
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_AD_TRUNK_MODE_SET
* @param   void       *data   - DAPI_LAG_MGMT_CMD_t.cmdData.portADTrunk
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadADTrunkModeSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{

  L7_RC_t          result = L7_SUCCESS;
  DAPI_PORT_t           *dapiPortPtr;
  BROAD_PORT_t          *hapiPortPtr;
  DAPI_LAG_MGMT_CMD_t   *cmdADTrunk = (DAPI_LAG_MGMT_CMD_t*)data;

  if (cmdADTrunk->cmdData.portADTrunk.getOrSet == DAPI_CMD_GET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s GET not supported for this command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  dapiPortPtr = dapi_g->unit[usp->unit]->slot[usp->slot]->port[usp->port];
  hapiPortPtr = dapiPortPtr->hapiPort;

  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_FALSE)
  {
    LOG_ERROR(dapiPortPtr->type);
  }
  /* Set the mode in HAPI port struct so that it sends 802.3AD PDUs to the host CPU. */
  hapiPortPtr->hapiModeparm.physical.acceptLAPDU = cmdADTrunk->cmdData.portADTrunk.enable;

  return result;

}

/*********************************************************************
*
* @purpose Map the incoming hash mode value to BCM_TRUNK_PSC value.
*
* @param   L7_uint32  hashMode - hashMode value passed by application
* @param   L7_int32  *bcmPsc - port selection criteria for BCM API
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLagHashToPsc(L7_uint32 hashMode, L7_int32 *bcmPsc)
{
  switch (hashMode)
  {
    case L7_DOT3AD_HASH_SA_VLAN_ETYPE_INTF:
      *bcmPsc = BCM_TRUNK_PSC_SRCMAC;
      break;
    case L7_DOT3AD_HASH_DA_VLAN_ETYPE_INTF:
      *bcmPsc = BCM_TRUNK_PSC_DSTMAC;
      break;
    case L7_DOT3AD_HASH_SADA_VLAN_ETYPE_INTF:
      *bcmPsc = BCM_TRUNK_PSC_SRCDSTMAC;
      break;
    case L7_DOT3AD_HASH_SIP_SPORT:
      *bcmPsc = BCM_TRUNK_PSC_SRCIP;
      break;
    case L7_DOT3AD_HASH_DIP_DPORT:
      *bcmPsc = BCM_TRUNK_PSC_DSTIP;
      break;
    case L7_DOT3AD_HASH_SIP_DIP_SPORT_DPORT:
      *bcmPsc = BCM_TRUNK_PSC_SRCDSTIP;
      break;
    case L7_DOT3AD_HASH_ENHANCED: 
      /* Enhance hashing mode maps to RTAG7. 
       * See hapiBroadRtag7SwitchControlSet() for RTAG7 settings 
       */
      *bcmPsc = BCM_TRUNK_PSC_PORTFLOW;
      break;
    default:
      *bcmPsc = BCM_TRUNK_PSC_SRCDSTMAC;
      break;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Sets the Hash Mode for a LAG.
*
* @param   DAPI_USP_t *usp    - the usp of this lag
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_LAG_HASHMODE_SET
* @param   void       *data   - DAPI_LAG_MGMT_CMD_t.cmdData.hashMode
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLagHashModeSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{

  L7_RC_t          result = L7_SUCCESS;
  DAPI_PORT_t           *dapiPortPtr;
  BROAD_PORT_t          *hapiLagPortPtr;
  DAPI_LAG_MGMT_CMD_t   *cmdLagHashMode = (DAPI_LAG_MGMT_CMD_t*)data;
  bcm_trunk_t           tid;
  L7_RC_t               rc;
  L7_int32              psc = BCM_TRUNK_PSC_SRCDSTMAC;

  if (cmdLagHashMode->cmdData.lagHashMode.getOrSet == DAPI_CMD_GET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s GET not supported for this command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  dapiPortPtr = dapi_g->unit[usp->unit]->slot[usp->slot]->port[usp->port];
  hapiLagPortPtr = dapi_g->unit[usp->unit]->slot[usp->slot]->port[usp->port]->hapiPort;

  if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_FALSE)
  {
    result = L7_FAILURE;
    return result;
  }

  hapiBroadLagCritSecEnter();

  if (hapiLagPortPtr->hapiModeparm.lag.hashMode != cmdLagHashMode->cmdData.lagHashMode.hashMode)
  {
    /* Hashing mode is changed on active LAG with members */
    if (hapiLagPortPtr->hapiModeparm.lag.numMembers > 0)
    {
      tid = hapiLagPortPtr->hapiModeparm.lag.tgid;

      /* Map the application hash value to BCM PSC value */
      hapiBroadLagHashToPsc(cmdLagHashMode->cmdData.lagHashMode.hashMode, &psc);

      rc = usl_bcmx_trunk_psc_set (usp->port, tid, psc);
      if (L7_BCMX_OK(rc) != L7_TRUE)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't set hash mode mode for trunk ID %d, rv = %d", tid, rc);
      }
    }

    /* applied when first member port is added to the trunk */
    hapiLagPortPtr->hapiModeparm.lag.hashMode = cmdLagHashMode->cmdData.lagHashMode.hashMode;
  }

  hapiBroadLagCritSecExit();

  return result;

}


/*********************************************************************
*
* @purpose Wait for Hapi Lag Async processing to complete
*
* @param   DAPI_USP_t *usp    {(input)} Cpu Usp
* @param   DAPI_CMD_t  cmd    {(input)} 
* @param   void       *data   {(input)} DAPI_LAG_MGMT_CMD_t.cmdData.
* @param   DAPI_t     *dapi_g {(input)} The driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLagsSync(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{

  L7_RC_t                result = L7_SUCCESS;
  DAPI_LAG_MGMT_CMD_t   *cmdLagSync = (DAPI_LAG_MGMT_CMD_t*)data;
  
  if (cmdLagSync->cmdData.lagsSync.getOrSet == DAPI_CMD_GET)
  {
    result = L7_FAILURE;
    return result;
  }

  result = hapiBroadLagAsyncDoneWait(cmdLagSync->cmdData.lagsSync.timeout);

  return result;

}



/*********************************************************************
*
* @purpose Use the LAG's protocol VLAN config for the member port's
*          protocol VLAN config
*
* @param   DAPI_USP_t *memberUsp
* @param   DAPI_USP_t *lagUsp
* @param   DAPI_t     *dapi_g
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadProtocolVlanLagPortAddNotify(DAPI_USP_t *memberUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g)
{
  L7_uint32         i;
  BROAD_PORT_t     *hapiLagPortPtr;
  BROAD_PORT_t     *hapiLagMemberPortPtr;
  BROAD_SYSTEM_t   *hapiSystemPtr;

  hapiSystemPtr = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;
  hapiLagPortPtr = HAPI_PORT_GET(lagUsp, dapi_g);
  hapiLagMemberPortPtr = HAPI_PORT_GET(memberUsp, dapi_g);

  for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
  {
    if (hapiLagMemberPortPtr->pbvlan_table[i] != 0)
    {
      hapiBroadProtocolVlanRemove(memberUsp, 
                                  hapiSystemPtr->etherProtocol[i], dapi_g);
    }
    if (hapiLagPortPtr->pbvlan_table[i] != 0)
    {
      hapiBroadProtocolVlanAdd(memberUsp, hapiSystemPtr->etherProtocol[i],
                               hapiLagPortPtr->pbvlan_table[i], dapi_g);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Use the member port's protocol VLAN config for the member port's
*          protocol VLAN config
*
* @param   DAPI_USP_t *memberUsp
* @param   DAPI_USP_t *lagUsp
* @param   DAPI_t     *dapi_g
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadProtocolVlanLagPortDeleteNotify(DAPI_USP_t *memberUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g)
{
  L7_uint32         i;
  BROAD_PORT_t     *hapiLagPortPtr;
  BROAD_PORT_t     *hapiLagMemberPortPtr;
  BROAD_SYSTEM_t   *hapiSystemPtr;

  hapiSystemPtr = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;
  hapiLagPortPtr = HAPI_PORT_GET(lagUsp, dapi_g);
  hapiLagMemberPortPtr = HAPI_PORT_GET(memberUsp, dapi_g);

  for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
  {
    if (hapiLagPortPtr->pbvlan_table[i] != 0)
    {
      hapiBroadProtocolVlanRemove(memberUsp, 
                                  hapiSystemPtr->etherProtocol[i], dapi_g);
    }
    if (hapiLagMemberPortPtr->pbvlan_table[i] != 0)
    {
      hapiBroadProtocolVlanAdd(memberUsp, hapiSystemPtr->etherProtocol[i],
                               hapiLagMemberPortPtr->pbvlan_table[i], dapi_g);
    }
  }

  return L7_SUCCESS;
}


/**********************************
**  Swap two LAG members. 
**  This function is used for testing traffic distribution.
**********************************/
L7_uint32 hapiBroadDebugLagSwap (L7_uint32 lag_id)
{
  int rv;
  bcmx_trunk_add_info_t trunk_info;
  L7_uint32  list_size;
  bcmx_lport_t port1, port2;

  bcmx_lplist_init(&trunk_info.ports, L7_MAX_MEMBERS_PER_LAG, BCMX_LP_UNIQ);

  do
  {
    rv = bcmx_trunk_get (lag_id, &trunk_info);
    if (rv != BCM_E_NONE)
    {
      printf("Error: bcmx_trunk_egress_get(), tid = %d, rv = %d\n",
             lag_id,
             rv);
      break;
    }

    list_size = BCMX_LPLIST_COUNT (&trunk_info.ports);
    printf("List Size = %d\n", list_size);

    if (list_size != 2)
    {
      printf("Error: List size is no equal to 2.\n");
      break;
    }

    port1 = bcmx_lplist_index (&trunk_info.ports, 0);
    port2 = bcmx_lplist_index (&trunk_info.ports, 1);

    BCMX_LPLIST_REMOVE(&trunk_info.ports, port1);
    BCMX_LPLIST_REMOVE(&trunk_info.ports, port2);



    BCMX_LPLIST_ADD (&trunk_info.ports, port2);
    BCMX_LPLIST_ADD (&trunk_info.ports, port1);

    rv = bcmx_trunk_set (lag_id, &trunk_info);
    if (rv != BCM_E_NONE)
    {
      printf("Error: bcmx_trunk_egress_set (), tid = %d, rv = %d\n",
             lag_id,
             rv);
      break;
    }

    printf("Swapped ports %d and %d in lag %d\n",
           port1,
           port2,
           lag_id);


  } while (0);

  bcmx_lplist_free(&trunk_info.ports);

  return 0;
}
