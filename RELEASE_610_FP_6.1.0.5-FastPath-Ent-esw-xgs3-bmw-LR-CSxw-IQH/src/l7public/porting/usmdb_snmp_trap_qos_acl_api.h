/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename usmdb_snmp_trap_qos_acl_api.h
*
* @purpose Provide interface to SNMP Trap API's for unitmgr components
*
* @component
*
* @comments
*
* @create   06/27/2005
*
* @author   gpaussa
* @end
*
**********************************************************************/

#ifndef USMDB_SNMP_TRAP_QOS_ACL_API_H
#define USMDB_SNMP_TRAP_QOS_ACL_API_H

#include "l7_common.h"
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
                                            SNMP_QOS_ACL_RULE_LOG_EVENT_TRAP_t *pTrapData);

#endif /* USMDB_SNMP_TRAP_QOS_API_H */
