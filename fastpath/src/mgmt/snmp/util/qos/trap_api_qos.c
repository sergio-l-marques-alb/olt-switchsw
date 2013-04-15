/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name: trap_api_qos.c
*
* Purpose: API interface for SNMP Agent Traps
*
* Created by: gpaussa 06/28/2005
*
* Component: SNMP
*
*********************************************************************/

#include "l7_common.h"
#include "osapi.h"
#include "snmp_trap_api_qos.h"
#ifdef L7_QOS_PACKAGE
#include "privatetrap_qos_acl.h"
#endif /* L7_QOS_PACKAGE */
#include "snmp_api.h"
#include "sysapi.h"


typedef enum
{
  L7_SNMP_TRAP_QOS_AclRuleLogEvent = 0,
  L7_SNMP_TRAP_QOS_AclRuleTimeRangeEvent,
} L7_SNMP_TRAP_TYPE_QOS_t;

/*********************************************************************
*
* @purpose  Handles calling the trap code to send traps
*
* @param    trap_id   enumeration of the trap to send
* @param    data      data structure holding trap information
*
* @returns  void
*
* @notes    The datastructure is determined by the trap_id indicated.
*
* @end
*********************************************************************/
void SnmpTrapSendCallback_qos(L7_uint32 trap_id, void *data)
{
  switch (trap_id)
  {
  case L7_SNMP_TRAP_QOS_AclRuleLogEvent:
    (void)snmp_qosAclRuleLogEventTrapSend((SNMP_QOS_ACL_RULE_LOG_EVENT_TRAP_t*)data);
    break;

  case L7_SNMP_TRAP_QOS_AclRuleTimeRangeEvent:
  #if defined (L7_TIMERANGES_PACKAGE) && (L7_FEAT_ACL_RULE_TIME_RANGE)
    (void)snmp_qosAclRuleTimeRangeEventTrapSend((SNMP_QOS_ACL_RULE_TIMERANGE_EVENT_TRAP_t*)data);
  #endif
    break;
  default:
    /* unknown trap */
    break;
  }
}

/* Begin Function Declarations: trap_api_qos.c */

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
L7_RC_t SnmpQosAclRuleLogEventTrapSend(SNMP_QOS_ACL_RULE_LOG_EVENT_TRAP_t *pTrapData)
{
  SNMP_QOS_ACL_RULE_LOG_EVENT_TRAP_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(SNMP_QOS_ACL_RULE_LOG_EVENT_TRAP_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->aclId = pTrapData->aclId;
  trap->aclType = pTrapData->aclType;
  trap->action = pTrapData->action;
  trap->hitCount.low = pTrapData->hitCount.low;
  trap->hitCount.high = pTrapData->hitCount.high;
  trap->ruleNum = pTrapData->ruleNum;

  return snmpTrapSend(L7_SNMP_TRAP_QOS_AclRuleLogEvent, (void*)trap, &SnmpTrapSendCallback_qos);
}
/*********************************************************************
*
* @purpose  Send a trap when time range notification occurs for time based
*           ACL rule
*
* @param    UnitIndex   unit for this operation
* @param    pTrapData   ptr to ACL trap information
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t SnmpQosAclRuleTimeRangeEventTrapSend(SNMP_QOS_ACL_RULE_TIMERANGE_EVENT_TRAP_t *pTrapData)
{
  SNMP_QOS_ACL_RULE_TIMERANGE_EVENT_TRAP_t *trap = L7_NULLPTR;
  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(SNMP_QOS_ACL_RULE_TIMERANGE_EVENT_TRAP_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->aclId = pTrapData->aclId;
  trap->ruleNum = pTrapData->ruleNum;
  trap->aclType = pTrapData->aclType;
  trap->action = pTrapData->action;
  osapiStrncpySafe(trap->timeRangeName, pTrapData->timeRangeName, strlen(pTrapData->timeRangeName)+1);
  trap->notification = pTrapData->notification;
  trap->status = pTrapData->status;

  return snmpTrapSend(L7_SNMP_TRAP_QOS_AclRuleTimeRangeEvent, (void*)trap, &SnmpTrapSendCallback_qos);  
}
void SnmpTestTraps_qos()
{
  SNMP_QOS_ACL_RULE_LOG_EVENT_TRAP_t trap;
  L7_RC_t rc;

  trap.aclId = 1;
  trap.aclType = 2;
  trap.action = 3;
  trap.hitCount.low = 4;
  trap.hitCount.high = 5;
  trap.ruleNum = 6;

  rc = SnmpQosAclRuleLogEventTrapSend(&trap);
  sysapiPrintf("SnmpQosAclRuleLogEventTrapSend -> %d\n", rc);

  return;
}

void SnmpTestTimeRangeTraps_qos()
{
  SNMP_QOS_ACL_RULE_TIMERANGE_EVENT_TRAP_t trap;
  L7_RC_t rc;

  trap.aclId = 1;
  trap.aclType = 2;
  trap.action = 3;
  trap.ruleNum = 4;
strcpy(trap.timeRangeName, "abcdeabcdeabcdeabcdeabcdeabcdea");
  trap.notification = L7_TIMERANGE_EVENT_ACTIVATE; 
  trap.status = L7_SUCCESS;
  rc = SnmpQosAclRuleTimeRangeEventTrapSend(&trap);
  sysapiPrintf("SnmpQosAclRuleTimeRangeEventTrapSend -> %d\n", rc);

  return;
 
}

/* End Function Declarations */

