/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename usmdb_snmp_trap_qos_acl.c
*
* @purpose   Provide interface to SNMP Trap API's for unitmgr components
*
* @component ACL
*
* @comments  none
*
* @create    06/28/2005
*
* @author    gpaussa
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "usmdb_snmp_trap_qos_acl_api.h"
#include "snmp_trap_api_qos.h"


/*********************************************************************
*
* @purpose  Send a trap when ACL rule logging event occurs
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
L7_RC_t usmDbSnmpQosAclRuleLogEventTrapSend(L7_uint32 UnitIndex,
                                            SNMP_QOS_ACL_RULE_LOG_EVENT_TRAP_t *pTrapData)
{
    return SnmpQosAclRuleLogEventTrapSend(pTrapData);
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
L7_RC_t usmDbSnmpQosAclRuleTimeRangeEventTrapSend(L7_uint32 UnitIndex,
                                            SNMP_QOS_ACL_RULE_TIMERANGE_EVENT_TRAP_t *pTrapData)
{
    return SnmpQosAclRuleTimeRangeEventTrapSend(pTrapData);
}
