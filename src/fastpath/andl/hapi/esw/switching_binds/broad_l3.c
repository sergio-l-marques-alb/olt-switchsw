/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: broad_l3.c
*
* Purpose: This file contains the entry point to the Layer 3 hapi interface
*
* Component: hapi
*
* Comments:
*
* Created by: Andrey Tsigler 5/07/2002
*
*********************************************************************/
#include <string.h>

#include "l7_common.h"
#include "sysapi.h"
#include "osapi.h"
#include "avl_api.h"
#include "log.h"
#include "l7_packet.h"

#include "dapi.h"

#include "broad_common.h"
#include "broad_stats.h"
#include "broad_l3.h"

#include "bcm/port.h"
#include "bcm/vlan.h"
#include "bcm/l3.h"
#include "bcmx/l2.h"
#include "bcmx/l3.h"
#include "bcmx/vlan.h"
#include "soc/drv.h"
 
/*********************************************************************
*
* @purpose Initializes L3 code
*
* @param *dapi_g          system information
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL3Init(DAPI_t *dapi_g)
{
  return L7_SUCCESS;
}

/*******************************************************************************
*
* @purpose Initialize routing for all ports on this card
*
* @param   L7_ushort16  unitNum - unit number for this card
* @param   L7_ushort16  slotNum - slot number for this card
* @param   DAPI_t *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   Called from hapiBroadPhysicalCardInsert() for physical cards and
*          from hapiBroadL3RouterCardInsert() for logical cards.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3CardInit(L7_ushort16 unitNum, L7_ushort16 slotNum,
                            DAPI_t *dapi_g)
{
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose Removes logical card
*
* @param  dapiUsp         USP for the logical router card
* @param  cmd             DAPI cmd for removing card (not used)
* @param  data            Data associated with the DAPI cmd (not used)
* @param *handle          DAPI driver object handle
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
******************************************************************************/
L7_RC_t hapiBroadL3RouterCardRemove(DAPI_USP_t *usp,
                                   DAPI_CMD_t cmd,
                                   void *data,
                                   DAPI_t *dapi_g)
{
  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Initializes logical router cards
*
* @param  dapiUsp         USP for the logical router card
* @param  cmd             DAPI cmd for inserting card (not used)
* @param  data            Data associated with the DAPI cmd (not used)
* @param *handle          DAPI driver object handle
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3RouterCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, 
                                    void *data, void *handle)
{
  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Initializes logical tunnel card
*
* @param  dapiUsp         USP for the logical tunnel card
* @param  cmd             DAPI cmd for inserting card (not used)
* @param  data            Data associated with the DAPI cmd (not used)
* @param *handle          DAPI driver object handle
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
******************************************************************************/
L7_RC_t hapiBroadL3TunnelCardInsert(DAPI_USP_t *dapiUsp,
                                    DAPI_CMD_t cmd,
                                    void *data,
                                    void *handle)
{
  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Removes logical tunnel card
*
* @param  dapiUsp         USP for the logical tunnel card
* @param  cmd             DAPI cmd for removing card (not used)
* @param  data            Data associated with the DAPI cmd (not used)
* @param *handle          DAPI driver object handle
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
******************************************************************************/
L7_RC_t hapiBroadL3TunnelCardRemove(DAPI_USP_t *dapiUsp,
                                    DAPI_CMD_t cmd,
                                    void *data,
                                    void *handle)
{
  return L7_SUCCESS;
}


/*********************************************************************
*
* @function hapiBroadL3PortInit
*
* @purpose Hook in L3 functionality and initialize any data
*
* @param DAPI_PORT_t* dapiPortPtr - generic port instance
*
* @returns L7_RC_t
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL3PortInit(DAPI_PORT_t *dapiPortPtr)
{
  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Updates the L2 mac info and updates the nexthop entries if necessary
*
* @param *macAddr   MAC address learnt
* @param  vlanId    VLAN id of the learnt MAC address
* @param  usp       unit/slot/port on which MAC is learnt.
* @param *dapi_g    DAPI driver object
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3UpdateMacLearn(bcm_mac_t macAddr,
                                  L7_ushort16 vlanId,
                                  DAPI_USP_t *usp,
                                  DAPI_t *dapi_g)
{
  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Updates the L2 mac info and updates the nexthop entries if necessary
*
* @param *macAddr   MAC address aged
* @param  vlanId    VLAN ID of the aged MAC address
* @param *dapi_g    system information
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3UpdateMacAge(bcm_mac_t macAddr,
                                L7_ushort16 vlanId,
                                DAPI_t *dapi_g)
{
  return L7_SUCCESS;
}


/******************************************************************************
*
* @purpose Handles sending the packet when destination is a routing
*          interface
*
* @param  usp       dest unit/slot/port
* @param  frameType frame type, unicast, mcast
* @param  pktData   frame handle
* @param *dapi_g    system information
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*******************************************************************************/
L7_RC_t
hapiBroadL3RoutingSend(DAPI_USP_t *usp,
                       DAPI_FRAME_TYPE_t *frameType,
                       L7_uchar8 *pktData,
                       DAPI_t *dapi_g)
{
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Enables/Disables routing on the device
*
* @param  usp         unit slot port
* @param  cmd         add router interface cmd
* @param *dapi_g      system information
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadL3RouteFwdConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  return L7_SUCCESS;
}

/******************************************************************************
*
* @purpose Notification for VLAN port add/delete
*
* @param   DAPI_USP_t  *usp    - unit/slot/port
* @param   L7_ushort16 vlanId  - VLAN Id
* @param   L7_BOOL addDel      - 0 implies port is added to vlan. 1 for delete.
* @param   DAPI_t      *dapi_g
*
* @returns L7_RC_t result
*
* @notes   The callback is used for VLAN routing interfaces. As ports can be
*          added/deleted from routed VLAN, the host policy needs to be updated.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadL3PortVlanNotify(DAPI_USP_t  *usp,
                                  L7_ushort16 vlanId,
                                  L7_BOOL     addDel,
                                  DAPI_t      *dapi_g)
{
  return L7_SUCCESS;
}
