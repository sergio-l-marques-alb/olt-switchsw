/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name:      broad_qos.h
*
* @purpose   This file contains the prototypes for QOS
*
* Component: hapi
*
* Comments:
*
* Created by: grantc 7/24/02
*
*********************************************************************/

#ifndef INCLUDE_BROAD_QOS_H
#define INCLUDE_BROAD_QOS_H

#include "broad_qos_common.h"
#include "broad_diffserv.h"
#include "broad_acl.h"
#include "broad_cos.h"

/*********************************************************************
*
* @purpose Initialize the QOS package
*
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosInit(DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Initialize the QOS package per card
*
* @param   L7_ushort16       unitNum
* @param   L7_ushort16       slotNum
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosCardInit(L7_ushort16 unitNum,L7_ushort16 slotNum, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Initialize the QOS package for all ports on a card
*
* @param   DAPI_PORT_t *dapiPortPtr
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosPortInit(DAPI_PORT_t *dapiPortPtr);

/*********************************************************************
*
* @purpose Update the stats from the QoS policies in effect.
*
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosStatsUpdate(DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Notifies the QOS component that a physical port has been
*          added to a LAG port
*
* @param   DAPI_USP_t       *portUsp
* @param   DAPI_USP_t       *lagUsp
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosPortLagAddNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Notifies the QOS component that a physical port has been
*          removed from a LAG port
*
* @param   DAPI_USP_t       *portUsp
* @param   DAPI_USP_t       *lagUsp
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosPortLagDeleteNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Notifies the QOS component of physical port speed changes
*
* @param   DAPI_USP_t       *portUsp
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosPortLinkUpNotify(DAPI_USP_t *portUsp, DAPI_t *dapi_g);

#endif
