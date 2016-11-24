/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename trap_qos_api.h
*
* @purpose Trap Manager QOS functions
*
* @component trapmgr
*
* @comments none
*
* @created  06/27/2005
*
* @author   gpaussa
*
* @end
*
**********************************************************************/

#ifndef TRAP_QOS_API_H
#define TRAP_QOS_API_H

#include "l7_common.h"
#include "snmp_trap_api_qos.h"

#define TRAPMGR_SPECIFIC_ACL_RULE_LOG_EVENT                          1

/* Begin Function Prototypes */

/*********************************************************************
*
* @purpose  An ACL rule log event trap signifies at a particular ACL
*           rule action was taken during packet processing in the hardware.
*           A count is maintained during the trap reporting interval
*           for each ACL rule being logged.
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
L7_RC_t trapMgrAclRuleLogEventTrap(SNMP_QOS_ACL_RULE_LOG_EVENT_TRAP_t *pTrapData);

/*********************************************************************
* @purpose  Returns Trap Manager's ACL trap mode  
*
* @param    void  
*
* @returns  L7_uint32   ACL trap mode (L7_ENABLE or L7_DISABLE)
*           
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 trapMgrGetTrapAcl(void);

/*********************************************************************
* @purpose  Sets Trap Manager's ACL trap flag
*
* @param    val         ACL trap flag (L7_ENABLE or L7_DISABLE)  
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
void trapMgrSetTrapAcl(L7_uint32 val);

/* End Function Prototypes */

#endif /* TRAP_QOS_API_H */
