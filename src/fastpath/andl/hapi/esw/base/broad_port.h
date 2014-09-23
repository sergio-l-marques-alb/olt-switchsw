/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_port.h
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

#ifndef INCLUDE_BROAD_PORT_H
#define INCLUDE_BROAD_PORT_H

#include "dapi.h"

typedef struct portLinkStatus_s 
{
  bcmx_lport_t lport;
  int linkstatus;
} portLinkStatus_t;


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
L7_RC_t hapiBroadPortCtlInit(DAPI_PORT_t *dapiPortPtr);

/*********************************************************************
*
* @purpose Help
*
* @param   void   **ppv    
* @param   DAPI_t  *dapi_g - the driver object
* @param   void   
*
* @returns L7_RC_t return
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadPortLinkStatusChange(bcmx_lport_t lport, bcm_port_info_t *portInfo);

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
L7_RC_t hapiBroadPortLinkUp(DAPI_USP_t *usp, DAPI_t *dapi_g);

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
L7_RC_t hapiBroadPortLinkDown(DAPI_USP_t *usp, DAPI_t *dapi_g);

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
L7_RC_t hapiBroadSlotCtlInit(L7_ushort16 unitNum, L7_ushort16 slotNum, DAPI_t *dapi_g);

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
void hapiBroadPortLinkStatusTask(void);

#endif
