
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: privatetrap_qos_acl.h
*
* Purpose: QOS ACL trap functions
*
* Created by: gpaussa 06/28/2005
*
* Component: SNMP
*
*********************************************************************/

#ifndef PRIVATETRAP_QOS_ACL_H
#define PRIVATETRAP_QOS_ACL_H

#include "l7_common.h"
#include "snmp_trap_api_qos.h"

/* Begin Function Prototypes */

/*********************************************************************
*
* @purpose  Send a trap when ACL rule logging event occurs
*
* @param    pTrapData         ptr to ACL trap information
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t snmp_qosAclRuleLogEventTrapSend(SNMP_QOS_ACL_RULE_LOG_EVENT_TRAP_t *pTrapData);

#if defined (L7_TIMERANGES_PACKAGE) && (L7_FEAT_ACL_RULE_TIME_RANGE)
/*********************************************************************
*
* @purpose  Send a trap when time range notification occurs for time based ACL
*
* @param    pTrapData         ptr to ACL trap information
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t snmp_qosAclRuleTimeRangeEventTrapSend(SNMP_QOS_ACL_RULE_TIMERANGE_EVENT_TRAP_t *pTrapData);
#endif
#endif /* PRIVATETRAP_QOS_ACL_H */
