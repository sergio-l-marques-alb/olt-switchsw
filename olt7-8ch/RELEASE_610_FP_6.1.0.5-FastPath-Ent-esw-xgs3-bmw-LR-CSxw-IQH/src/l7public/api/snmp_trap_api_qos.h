/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name: snmp_trap_api_qos.h
*
* Purpose: API interface for SNMP Agent Traps
*
* Created by: gpaussa 06/28/2005
*
* Component: SNMP
*
*********************************************************************/

#ifndef SNMP_TRAP_API_QOS_H
#define SNMP_TRAP_API_QOS_H

#include "l7_common.h"
#include "acl_exports.h"
#include "acl_exports.h"

/* ACL Trap data */
typedef struct
{
  L7_ACL_TYPE_t     aclType;            /* type of ACL                        */
  L7_uint32         aclId;              /* ACL identifier for this type       */
  L7_uint32         ruleNum;            /* ACL rule number                    */
  L7_ACL_ACTION_t   action;             /* ACL rule action                    */
  L7_ulong64        hitCount;           /* number of times this rule was hit  */

} SNMP_QOS_ACL_RULE_LOG_EVENT_TRAP_t;


/* Begin Function Prototypes */

/*********************************************************************
*
* @purpose  Send a trap when ACL rule logging event occurs
*
* @param    pTrapData   ptr to ACL trap information
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t SnmpQosAclRuleLogEventTrapSend(SNMP_QOS_ACL_RULE_LOG_EVENT_TRAP_t *pTrapData);

/* End Function Prototypes */

#endif /* SNMP_TRAP_API_QOS_H */
