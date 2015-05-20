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
#include "bcmx/port.h"

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
void hapiBroadPortLinkStatusChange(bcmx_lport_t lport, bcm_port_info_t *portInfo)
{
 portLinkStatus_t link_msg;

 PT_LOG_NOTICE(LOG_CTX_HAPI,"LPort 0x%08x link=%d", lport, portInfo->linkstatus);

 link_msg.lport = lport;
 link_msg.linkstatus = portInfo->linkstatus;

 if (osapiMessageSend(hapiLinkStatusQueue,
                      (void *)&link_msg,
                      sizeof(link_msg),
                      L7_WAIT_FOREVER,
                      L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
 {
   LOG_ERROR (lport);
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
void hapiBroadPortLinkStatusChange(bcmx_lport_t lport, bcm_port_info_t *portInfo)
{
  int linkstatus;
  DAPI_USP_t        usp;
  DAPI_PORT_t       *dapiPortPtr;
  bcmx_uport_t      uport;

  linkstatus = portInfo->linkstatus;

  uport = BCMX_UPORT_GET(lport);

  HAPI_BROAD_UPORT_TO_USP(uport,&usp);

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
    PT_LOG_NOTICE(LOG_CTX_HAPI,"LPort 0x%08x link is up", lport);
    dapiPortPtr->modeparm.physical.isLinkUp = L7_TRUE;
    hapiBroadPortLinkUp(&usp, dapi_g);
  }
  else if ((dapiPortPtr->modeparm.physical.isLinkUp == L7_TRUE) && (linkstatus == FALSE))
  {
    /* link is down */
    PT_LOG_NOTICE(LOG_CTX_HAPI,"LPort 0x%08x link is down", lport);
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
  bcmx_uport_t         uport;
  portLinkStatus_t  link_msg;

  do
  {
    if (osapiMessageReceive(hapiLinkStatusQueue,
                            (void *)&link_msg,sizeof(link_msg),L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      LOG_ERROR(0);
    }

    uport = BCMX_UPORT_GET(link_msg.lport);

    HAPI_BROAD_UPORT_TO_USP(uport,&usp);

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
    bcmx_port_autoneg_get(hapiPortPtr->bcmx_lport,&anStatus);
    if(anStatus == L7_ENABLE)
    {
      bcmx_port_advert_get(hapiPortPtr->bcmx_lport,
                                       &abilityMask);
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
  bcmx_port_autoneg_get(hapiPortPtr->bcmx_lport,&anStatus);
  if(anStatus == L7_ENABLE)
  {
    bcmx_port_advert_get(hapiPortPtr->bcmx_lport,
                                    &abilityMask);
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


