/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name: broad_acl.h
*
* @purpose This file contains the prototypes for Access Control Lists
*
* Component: hapi
*
* Comments:
*
* Created by: grantc 7/24/02
*
*********************************************************************/
#ifndef INCLUDE_BROAD_QOS_ACL_H
#define INCLUDE_BROAD_QOS_ACL_H

#include "broad_qos_common.h"
#include "acl_exports.h"

/*********************************************************************
*
* @purpose Initialize ACL component of QOS package
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
L7_RC_t hapiBroadQosAclInit(DAPI_t *dapi_g);

/*********************************************************************
*
* @function hapiBroadQosAclPortInit
*
* @purpose  Per-port ACL init
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
L7_RC_t hapiBroadQosAclPortInit(DAPI_PORT_t *dapiPortPtr);

/*********************************************************************
*
* @functions hapiBroadQosAclAdd
*
* @purpose Adds an Access Control List to an interface
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
/* DAPI_CMD_QOS_ACL_ADD */
L7_RC_t hapiBroadQosAclAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @functions hapiBroadQosAclDelete
*
* @purpose Deletes an Access Control List from an interface
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
/* DAPI_CMD_QOS_ACL_DELETE */
L7_RC_t hapiBroadQosAclDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @functions hapiBroadQosAclRuleCountGet
*
* @purpose Get the ACL rule counter for the specified correlator value
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
/* DAPI_CMD_QOS_ACL_RULE_COUNT_GET */
L7_RC_t hapiBroadQosAclRuleCountGet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);
/*********************************************************************
*
* @purpose  Set status for policy Rule ID
*
* @param    BROAD_POLICY_t    policy
* @param    DAPI_USP_t       *usp
* @param    DAPI_t           *dapi_g
*
* @returns  L7_RC_t
*
* @notes    none
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_ACL_RULE_STATUS_SET*/
L7_RC_t hapiBroadQosAclRuleStatusSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);
#endif
