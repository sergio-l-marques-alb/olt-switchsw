/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_port.c
*
* @purpose   This file contains the routines for port configuration done
*            through the hapi interface
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

#include <stdlib.h>

/*#include "osapi.h"*/
#include "sysapi.h"
#include "l7_common.h"

#include "dapi.h"

#include "broad_common.h"
#include "broad_stats.h"
#include "broad_l2_mcast.h"
#include "broad_l2_std.h"
#include "broad_l2_vlan.h"
#include "broad_l2_lag.h"
#include "broad_l3.h"
#include "phy_hapi.h"

#ifdef L7_QOS_PACKAGE
#include "broad_qos.h"
#endif
#ifdef L7_WIRELESS_PACKAGE
#include "broad_l2_tunnel.h"
#endif

#ifdef L7_MCAST_PACKAGE
#include "broad_l3_mcast.h"
#endif

#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
#include "broad_l2_dot1ad.h"
#include "broad_dot1ad.h"
#endif

#ifdef L7_DOT3AH_PACKAGE
#include "broad_l2_dot3ah.h"
#endif
#endif

#include "logger.h"

void *hapiLinkStatusQueue = L7_NULL;

/*********************************************************************
*
* @purpose Initializes the hooks for the control commands on a port
*
* @param   DAPI_PORT_t *dapiPortPtr - used to hook in commands for a port
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPortCtlInit(DAPI_PORT_t *dapiPortPtr)
{

  L7_RC_t      result       = L7_SUCCESS;

  /* hook in the standard commands for a port */
  if (hapiBroadStdPortInit(dapiPortPtr) == L7_FAILURE )
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: In %s call to 'hapiBroadStdPortInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
  }
  /* hook in the Statistics commands */
  else if (hapiBroadStatisticsPortInit(dapiPortPtr) == L7_FAILURE )
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: In %s call to 'hapiBroadStatisticsPortInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
  }
  /* hook in the general Layer 2 Commands */
  else if (hapiBroadL2StdPortInit(dapiPortPtr) == L7_FAILURE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: In %s call to 'hapiBroadL2StdPortInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
  }
  /* hook in the 802.1Q VLAN Commands */
  else if (hapiBroadL2VlanPortInit(dapiPortPtr) == L7_FAILURE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: In %s call to 'hapiBroadL2VlanPortInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
  }
  /* hook in the Link Aggregation Commands */
  else if (hapiBroadL2LagPortInit(dapiPortPtr) == L7_FAILURE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: In %s call to 'hapiBroadL2LagPortInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
  }
  else if (hapiBroadL2McastPortInit(dapiPortPtr) == L7_FAILURE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: In %s call to 'hapiBroadL2McastPortInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
  }
  /* hook in the Layer 3 commands. For a Layer 2 package, the function should */
  /* not touch the function table  */
  else if (hapiBroadL3PortInit(dapiPortPtr) == L7_FAILURE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: In %s call to 'hapiBroadL3PortInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
  }
#ifdef L7_MCAST_PACKAGE
  else if (hapiBroadL3McastPortInit(dapiPortPtr) == L7_FAILURE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadL3McastPortInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
  }
#endif
#ifdef L7_QOS_PACKAGE
  else if (hapiBroadQosPortInit(dapiPortPtr) == L7_FAILURE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadQosPortInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
  }
#endif

#ifdef L7_WIRELESS_PACKAGE
  else if (hapiBroadL2TunnelPortInit(dapiPortPtr) == L7_FAILURE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadQosPortInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
  }
#endif

#ifdef L7_LLPF_PACKAGE
  else if (hapiBroadLlpfPortInit(dapiPortPtr) == L7_FAILURE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadLlpfPortInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
  }
#endif

#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
  else if (hapiBroadL2Dot1adPortInit(dapiPortPtr) == L7_FAILURE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadL2Dot1adPortInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
  }
  else if (hapiBroadDot1adPortInit(dapiPortPtr) == L7_FAILURE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadPoePortInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
  }
#endif
#ifdef L7_DOT3AH_PACKAGE
  else if(hapiBroadL2Dot3ahPortRemLbConfig(dapiPortPtr) == L7_FAILURE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadL2Dot3ahPortRemLb'\n",
                   __FILE__, __LINE__, __FUNCTION__);

  }
  else if(hapiBroadL2Dot3ahPortConfigAdd(dapiPortPtr) == L7_FAILURE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadL2Dot3ahPortConfigAdd'\n",
                   __FILE__, __LINE__, __FUNCTION__);

  }
  else if(hapiBroadL2Dot3ahPortConfigRemove(dapiPortPtr) == L7_FAILURE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadL2Dot3ahPortConfigRemove'\n",
                   __FILE__, __LINE__, __FUNCTION__);
  }
  else if(hapiBroadL2Dot3ahPortRemLbInit(dapiPortPtr) == L7_FAILURE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadL2Dot3ahPortRemLbInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
  }
  else if(hapiBroadL2Dot3ahPortRemLbInitUni(dapiPortPtr) == L7_FAILURE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadL2Dot3ahPortRemLbInitUni'\n",
                   __FILE__, __LINE__, __FUNCTION__);
  }
  else if(hapiBroadL2Dot3ahPortRemLbExit(dapiPortPtr) == L7_FAILURE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadL2Dot3ahPortRemLbExit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
  }
#endif
#endif
  return result;

}

extern DAPI_t *dapi_g;
#ifndef L7_PRODUCT_SMARTPATH
/*********************************************************************
*
* @purpose Handle port link changes
*
* @param   int               devId
* @param   soc_port_t        port
* @param   bcm_port_info_t  *portInfo
*
* @returns NONE
*
* @notes   Port events must be queued to a task in order to
*          avoid deadlocks.
*
* @end
*
*********************************************************************/
void hapiBroadPortLinkStatusChange(int unit, bcm_port_t port, bcm_port_info_t *portInfo)
{
  portLinkStatus_t link_msg;

  /* Fill struct */
  link_msg.bcm_unit = unit;
  link_msg.bcm_port = port;
  link_msg.linkstatus = portInfo->linkstatus;

  /* Fill gport and usp */
  if (bcmy_lut_unit_port_to_gport_get(unit, port, &link_msg.gport) != BCMY_E_NONE ||
     bcmy_lut_unit_port_to_usp_get(unit, port, &link_msg.usp) != BCMY_E_NONE)
  {
    PT_LOG_WARN(LOG_CTX_INTF,"unit %u, bcm_port %u: No GPORT/USP found!", unit, port);
    return;
  }

  /* Only proceed if gport and usp are valid */
  if (link_msg.gport == 0x0 || link_msg.gport == BCM_GPORT_INVALID)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"bcm_unit %u, bcm_port %u: Invalid gport 0x%x", unit, port, link_msg.gport);
    return;
  }
  if ((link_msg.usp.unit < 0 || link_msg.usp.slot < 0 || link_msg.usp.port < 0) ||
     isValidUsp(&link_msg.usp, dapi_g) == L7_FALSE)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"bcm_unit %u, bcm_port %u: Invalid usp {%d,%d,%d}",
               unit, port, link_msg.usp.unit, link_msg.usp.slot, link_msg.usp.port);
    return;
  }

  PT_LOG_NOTICE(LOG_CTX_INTF  ,"unit %d, port %d, gport 0x%x, usp={%d,%d,%d}: link=%d",
                unit, port, link_msg.gport, 
                link_msg.usp.unit, link_msg.usp.slot, link_msg.usp.port,
                portInfo->linkstatus);
  PT_LOG_NOTICE(LOG_CTX_EVENTS,"unit %d, port %d, gport 0x%x, usp={%d,%d,%d}: link=%d",
                unit, port, link_msg.gport, 
                link_msg.usp.unit, link_msg.usp.slot, link_msg.usp.port,
                portInfo->linkstatus);

  /* Send Message */
  if (osapiMessageSend(hapiLinkStatusQueue,
                      (void *)&link_msg,
                      sizeof(link_msg),
                      L7_WAIT_FOREVER,
                      L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    L7_LOG_ERROR(link_msg.gport);
  }

}
#else /* L7_PRODUCT_SMARTPATH */

/*********************************************************************
 *
 * @purpose Handle port link changes
 *
 * @param   int               devId
 * @param   soc_port_t        port
 * @param   bcm_port_info_t  *portInfo
 *
 * @returns NONE
 *
 * @notes   Port events must be queued to a task in order to
 *          avoid deadlocks.
 *
 * @end
 *
 *********************************************************************/
void hapiBroadPortLinkStatusChange(int unit, bcm_port_t port, bcm_port_info_t *portInfo)
{
  int linkstatus;
  DAPI_USP_t        usp;
  DAPI_PORT_t       *dapiPortPtr;
  bcm_gport_t       gport;

  linkstatus = portInfo->linkstatus;

  if (bcmy_lut_unit_port_to_gport_get(unit, port, &gport) != BCMY_E_NONE ||
      bcmy_lut_unit_port_to_usp_get(unit, port, &usp) != BCMY_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_INTF,"unit %u, bcm_port %u: No GPORT/USP found!", unit, port);
    return;
  }

  /* Make sure that card is not unplugged while we are using the
   ** pointers.
   */
  dapiCardRemovalReadLockTake ();

  if (isValidUsp(&usp,dapi_g) == L7_FALSE)
  {
    /* this is either an invalid port or the port hasn't been created yet */
    dapiCardRemovalReadLockGive ();
    return;
  }

  dapiPortPtr = DAPI_PORT_GET(&usp, dapi_g);

  if ((dapiPortPtr->modeparm.physical.isLinkUp == L7_FALSE) && (linkstatus == TRUE))
  {
    /* link is up */
    PT_LOG_NOTICE(LOG_CTX_INTF  ,"gport 0x%08x / unit %d port %d link is up", gport, unit, port);
    PT_LOG_NOTICE(LOG_CTX_EVENTS,"gport 0x%08x / unit %d port %d link is up", gport, unit, port);
    dapiPortPtr->modeparm.physical.isLinkUp = L7_TRUE;
    hapiBroadPortLinkUp(&usp, dapi_g);
  }
  else if ((dapiPortPtr->modeparm.physical.isLinkUp == L7_TRUE) && (linkstatus == FALSE))
  {
    /* link is down */
    PT_LOG_NOTICE(LOG_CTX_INTF  ,"gport 0x%08x / unit %d port %d link is down", gport, unit, port);
    PT_LOG_NOTICE(LOG_CTX_EVENTS,"gport 0x%08x / unit %d port %d link is down", gport, unit, port);
    dapiPortPtr->modeparm.physical.isLinkUp = L7_FALSE;
    hapiBroadPortLinkDown(&usp, dapi_g);
  }
  dapiCardRemovalReadLockGive ();

}
#endif  /* L7_PRODUCT_SMARTPATH */

/*********************************************************************
*
* @purpose Task that receives port link status changes.
*
* @param   int               devId
* @param   soc_port_t        port
* @param   bcm_port_info_t  *portInfo
*
* @returns NONE
*
* @notes   Identifies the link change on a port and updates the
*          port structures accordingly
*
* @end
*
*********************************************************************/
void hapiBroadPortLinkStatusTask(void)
{
  DAPI_USP_t        usp;
  DAPI_PORT_t      *dapiPortPtr;
  portLinkStatus_t  link_msg;

  do
  {
    if (osapiMessageReceive(hapiLinkStatusQueue,
                            (void *)&link_msg,sizeof(link_msg),L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      L7_LOG_ERROR(0);
    }

    /* Extract usp */
    usp = link_msg.usp;

    /* Make sure that card is not unplugged while we are using the
    ** pointers.
    */
    dapiCardRemovalReadLockTake ();

    if (isValidUsp(&usp,dapi_g) == L7_FALSE)
    {
      /* this is either an invalid port or the port hasn't been created yet */
      dapiCardRemovalReadLockGive ();
      continue;
    }

    dapiPortPtr = DAPI_PORT_GET(&usp, dapi_g);

    /* PTin modified: linkstatus is being returned as 1 or 3 */
    if ((dapiPortPtr->modeparm.physical.isLinkUp == L7_FALSE) && (link_msg.linkstatus == TRUE))
    {
      /* link is up */
      dapiPortPtr->modeparm.physical.isLinkUp = L7_TRUE;
      hapiBroadPortLinkUp(&usp, dapi_g);
    }
    /* PTin modified: linkstatus is being returned as 1 or 3 */
    else if ((dapiPortPtr->modeparm.physical.isLinkUp == L7_TRUE) && (link_msg.linkstatus != TRUE /*== FALSE*/))
    {
        /* link is down */
        dapiPortPtr->modeparm.physical.isLinkUp = L7_FALSE;
        hapiBroadPortLinkDown(&usp, dapi_g);
    }
    dapiCardRemovalReadLockGive ();
  } while (1);
}

/*********************************************************************
*
* @purpose Makes the changes to the driver for link becoming active
*
* @param   DAPI_USP_t *usp    - the port that has gained link
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t return
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPortLinkUp(DAPI_USP_t *usp, DAPI_t *dapi_g)
{

  L7_RC_t                      result = L7_SUCCESS;
  DAPI_USP_t                   eventUsp;
  DAPI_INTF_MGMT_CMD_t         eventInfo;
  DAPI_PORT_t                 *dapiPortPtr;
  BROAD_PORT_t                *hapiPortPtr;
  DAPI_PORT_SPEED_t            speed;
  DAPI_PORT_DUPLEX_t           duplex;
  L7_BOOL                      NA;
  L7_BOOL                      isSfpLink;
  L7_BOOL                      txPauseStatus, rxPauseStatus;
#if L7_FEAT_DUAL_PHY_COMBO
  L7_uint32                    anStatus;
  bcm_port_abil_t              abilityMask;
#endif


  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  /* notify the application that the port has gained link */
  eventUsp.unit = usp->unit;
  eventUsp.slot = usp->slot;
  eventUsp.port = usp->port;

  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
  {
    /* Update the MAC, prior to getting the results from the phy */
	  hapiBroadMacPauseResolveOnLinkUp(usp, dapi_g);

    /* Get the speed and duplex state of the port */
    hapiBroadPhyModeGet(usp, &speed, &duplex, &NA, &isSfpLink, &txPauseStatus, &rxPauseStatus, dapi_g);
    hapiPortPtr->speed  = speed;
    hapiPortPtr->duplex  = duplex;
    hapiPortPtr->link_up = L7_TRUE;
    hapiPortPtr->sfpLink = isSfpLink;
    hapiPortPtr->rx_pause = rxPauseStatus;
    hapiPortPtr->tx_pause = txPauseStatus;
#if L7_FEAT_DUAL_PHY_COMBO
    bcm_port_autoneg_get(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_port, &anStatus);
    if(anStatus == L7_ENABLE)
    {
      bcm_port_advert_get(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_port, &abilityMask);
      hapiPortPtr->autonegotiationAbilityMask = abilityMask;
    } else {
      hapiPortPtr->autonegotiationAbilityMask = L7_DISABLE;
    }
#endif


#ifdef L7_QOS_PACKAGE
    /* Reapply any QOS policies in effect on the port after hapiPortPtr is updated. */
    hapiBroadQosPortLinkUpNotify(&eventUsp, dapi_g);
#endif

    dapiCallback(&eventUsp,
                 DAPI_FAMILY_INTF_MGMT,
                 DAPI_CMD_INTF_UNSOLICITED_EVENT,
                 DAPI_EVENT_INTF_LINK_UP,
                 &eventInfo);


  }

  return result;
}



/*********************************************************************
*
* @purpose Makes the changes to the driver for Link becoming inactive
*
* @param   DAPI_USP_t *usp    - the port that has lost link
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t return
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPortLinkDown(DAPI_USP_t *usp, DAPI_t *dapi_g)
{

  L7_RC_t                    result = L7_SUCCESS;
  DAPI_USP_t                 eventUsp;
  DAPI_INTF_MGMT_CMD_t       eventInfo;
  DAPI_SYSTEM_CMD_t          cmdbsrRateSet;
  BROAD_PORT_t              *hapiPortPtr;
#if L7_FEAT_DUAL_PHY_COMBO
  L7_uint32                  anStatus;
  bcm_port_abil_t        abilityMask;
#endif

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  hapiPortPtr->link_up = L7_FALSE;
  hapiPortPtr->sfpLink = L7_FALSE;
  hapiPortPtr->rx_pause = L7_FALSE;
  hapiPortPtr->tx_pause = L7_FALSE;
#if L7_FEAT_DUAL_PHY_COMBO
  bcm_port_autoneg_get(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_port, &anStatus);
  if(anStatus == L7_ENABLE)
  {
    bcm_port_advert_get(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_port, &abilityMask);
    hapiPortPtr->autonegotiationAbilityMask = abilityMask;
  }
  else {
    hapiPortPtr->autonegotiationAbilityMask = L7_DISABLE;
  }

#endif

  /* notify the application that the port has lost link */
  eventUsp.unit = usp->unit;
  eventUsp.slot = usp->slot;
  eventUsp.port = usp->port;

  dapiCallback(&eventUsp,
               DAPI_FAMILY_INTF_MGMT,
               DAPI_CMD_INTF_UNSOLICITED_EVENT,
               DAPI_EVENT_INTF_LINK_DOWN,
               &eventInfo);

  /* If BSR is enabled, call BSR Rate Set routine now
     that link state has changed */
  if (dapi_g->system->broadcastControlEnable == L7_TRUE)
  {
    cmdbsrRateSet.cmdData.broadcastControl.enable = L7_TRUE;
    cmdbsrRateSet.cmdData.broadcastControl.getOrSet = DAPI_CMD_SET;
    dapiCtl(usp,DAPI_CMD_SYSTEM_BROADCAST_CONTROL_MODE_SET,&cmdbsrRateSet);
  }

  return result;

}



/*********************************************************************
*
* @purpose Initializes the hooks for the control commands
*
* @param   L7_ushort16  unitNum - the specified unit
* @param   L7_ushort16  slotNum - the specified slot
* @param   DAPI_t      *dapi_g  - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSlotCtlInit(L7_ushort16 unitNum, L7_ushort16 slotNum, DAPI_t *dapi_g)
{

  L7_RC_t      result       = L7_SUCCESS;
  DAPI_USP_t         usp;
  DAPI_PORT_t       *dapiPortPtr;

  usp.unit = unitNum;
  usp.slot = slotNum;

  /* Loop through the Physical Ports in this slot */
  for (usp.port=0; usp.port< dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot; usp.port++)
  {
    dapiPortPtr = DAPI_PORT_GET(&usp, dapi_g);

    /* hook in the individual ports function tables */
    if (hapiBroadPortCtlInit(dapiPortPtr) == L7_FAILURE)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "%s %d: In %s call to 'hapiBroadPortCtlInit'\n",
                     __FILE__, __LINE__, __FUNCTION__);
    }
  } /* ends physical ports */

  return result;

}

/*********************************************************************
*
* @purpose Determines the bandwidth of an interface
*
* @param   BROAD_PORT_t       *hapiPortPtr (in)
* @param   l7_cosq_set_t      queueSet (in)
* @param   L7_uint32          *portSpeed (output)
*
* @returns none
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadIntfSpeedGet(BROAD_PORT_t *hapiPortPtr, l7_cosq_set_t queueSet, L7_uint32 *portSpeed)
{
#if (PLAT_BCM_CHIP == L7_BCM_TRIDENT3_X3)
    /* Considering virtual ports (GPON + XGSPON) with different speeds */
    if (L7_MAX_CFG_QUEUESETS_PER_PORT > 1 && queueSet < L7_MAX_CFG_QUEUESETS_PER_PORT)
    {
        if ( (hapiPortPtr->usp.slot == 0)                                   /* Physical port */
             && ((PTIN_SYSTEM_PON_PORTS_MASK >> hapiPortPtr->usp.port) & 1) /* PON port */
             && hapiPortPtr->speed == DAPI_PORT_SPEED_GE_10GBPS )           /* 10G port */
        {
            switch (queueSet)
            {
                case L7_QOS_QSET_WIRED:
                    *portSpeed = 2500000;
                    break;
                case L7_QOS_QSET_WIRELESS:
                default:
                    *portSpeed = 10000000;
                    break;
            }
            return; /* We have already a valid output to return */
        }
    }
#endif

    /* use the cached value */
    switch (hapiPortPtr->speed)
    {
    case DAPI_PORT_SPEED_FE_10MBPS:
        *portSpeed = 10000;
        break;
    case DAPI_PORT_SPEED_FE_100MBPS:
        *portSpeed = 100000;
        break;
    case DAPI_PORT_SPEED_GE_1GBPS:
        *portSpeed = 1000000;
        break;
    /* PTin added: Speed 2.5G */
    case DAPI_PORT_SPEED_GE_2G5BPS:
        *portSpeed = 2500000;
        break;
    /* PTin end */
    case DAPI_PORT_SPEED_GE_10GBPS:
        *portSpeed = 10000000;
        break;
    /* PTin added: Speed 40G */
    case DAPI_PORT_SPEED_GE_40GBPS:
        *portSpeed = 40000000;
        break;
    /* PTin added: Speed 100G */
    case DAPI_PORT_SPEED_GE_100GBPS:
        *portSpeed = 100000000;
        break;
    /* PTin end */
    default:
        *portSpeed = 10000;
        break;
    }
}

