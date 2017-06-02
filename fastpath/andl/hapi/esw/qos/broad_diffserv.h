/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name: broad_diffserv.h
*
* @purpose This file contains the prototypes for DiffServ
*
* Component: hapi
*
* Comments:
*
* Created by: grantc 7/24/02
*
*********************************************************************/
#ifndef INCLUDE_BROAD_QOS_DIFFSERV_H
#define INCLUDE_BROAD_QOS_DIFFSERV_H

#include "broad_qos_common.h"

/*********************************************************************
*
* @purpose Initialize DiffServ component of QOS package
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
L7_RC_t hapiBroadQosDiffServInit(DAPI_t *dapi_g);

/*********************************************************************
*
* @function hapiBroadQosDiffServPortInit
*
* @purpose  Per-port DiffServ init
*
* @param    DAPI_PORT_t* dapiPortPtr - generic port instance
*
* @returns  L7_RC_t
*
* @notes    Invoked once per physical port
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosDiffservPortInit(DAPI_PORT_t *dapiPortPtr);

/*********************************************************************
*
* @functions hapiBroadDiffServPolicyCreate
*
* @purpose Parses a TLV and applies a DiffServ policy to a port.
*
* @param DAPI_USP_t *usp
* @param L7_tlv_t   *pTLV
* @param HAPI_QOS_INTF_DIR_t  direction
* @param L7_uint32   groupId
* @param L7_BOOL     keepStats
* @param L7_uint32  *policyId
* @param DAPI_t     *dapi_g
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDiffServPolicyCreate(DAPI_USP_t          *usp, 
                                      L7_tlv_t            *pTLV, 
                                      HAPI_QOS_INTF_DIR_t  direction,
                                      L7_uint32            groupId, 
                                      L7_BOOL              keepStats,
                                      L7_uint32           *policyId, 
                                      DAPI_t              *dapi_g);

/*********************************************************************
*
* @functions hapiBroadQosDiffServInstAdd
*
* @purpose Adds an policy-class instance to an interface
*
* @param DAPI_USP_t *usp
* @param DAPI_CMD_t  cmd
* @param void       *data
* @param DAPI_t     *dapi_g
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_DIFFSERV_INST_ADD */
L7_RC_t hapiBroadQosDiffServInstAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @functions hapiBroadQosDiffServInstDelete
*
* @purpose Deletes an policy-class instance from an interface
*
* @param DAPI_USP_t *usp
* @param DAPI_CMD_t  cmd
* @param void       *data
* @param DAPI_t     *dapi_g
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_DIFFSERV_INST_DELETE */
L7_RC_t hapiBroadQosDiffServInstDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @functions hapiBroadQosDiffServIntfStatInGet
*
* @purpose Gets a DiffServ statistic from an inbound interface
*
* @param DAPI_USP_t *usp
* @param DAPI_CMD_t  cmd
* @param void       *data
* @param DAPI_t     *dapi_g
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_DIFFSERV_INTF_STAT_IN_GET */
L7_RC_t hapiBroadQosDiffServIntfStatInGet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @functions hapiBroadQosDiffServIntfStatOutGet
*
* @purpose Gets a DiffServ statistic from an outbound interface
*
* @param DAPI_USP_t *usp
* @param DAPI_CMD_t  cmd
* @param void       *data
* @param DAPI_t     *dapi_g
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_DIFFSERV_INTF_STAT_OUT_GET */
L7_RC_t hapiBroadQosDiffServIntfStatOutGet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

#endif
