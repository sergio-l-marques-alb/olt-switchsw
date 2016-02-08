/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename trap_qos_acl.c
*
* @purpose Trap Manager QOS ACL functions
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

#include <stdio.h>
#include "l7_common.h"
#include "usmdb_sim_api.h"
#include "usmdb_snmp_api.h"
#include "usmdb_util_api.h"

#ifdef L7_QOS_PACKAGE
#include "usmdb_qos_acl_api.h"
#endif

#include "snmp_trap_api_qos.h"
#include "trap_qos_api.h"
#include "usmdb_snmp_trap_qos_acl_api.h"
#include "trapapi.h"
#include "trap.h"
#include "trapstr_qos_acl.h"

extern trapMgrCfgData_t trapMgrCfgData;
extern trapMgrTrapData_t trapMgrTrapData;

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
L7_RC_t trapMgrAclRuleLogEventTrap(SNMP_QOS_ACL_RULE_LOG_EVENT_TRAP_t *pTrapData)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;
  L7_char8 *pActionStr;
  L7_char8 hitCountStr[32];             /* 2**64 requires 20 digits to display */
  L7_uchar8 macAclName[L7_ACL_NAME_LEN_MAX+1];

  unit = usmDbThisUnitGet();

  if (pTrapData->action == L7_ACL_PERMIT)
    pActionStr = qosAclRuleActionPermit_str;
  else
    pActionStr = qosAclRuleActionDeny_str;

  memset(hitCountStr, 0, sizeof(hitCountStr));
  (void)usmDb64BitsToString(pTrapData->hitCount, hitCountStr);

  memset(macAclName, 0, sizeof(macAclName));

  switch (pTrapData->aclType)
  {
  case L7_ACL_TYPE_IP:
    sprintf(trapStringBuf, qosAclRuleLogEvent_str, pTrapData->aclId, pTrapData->ruleNum, pActionStr, hitCountStr);
    break;

  case L7_ACL_TYPE_MAC:
    (void)usmDbQosAclMacNameGet(unit, pTrapData->aclId, macAclName);
    sprintf(trapStringBuf, qosAclMacRuleLogEvent_str, macAclName, pTrapData->ruleNum, pActionStr, hitCountStr);
    break;

  default:
    return L7_SUCCESS;
    /*PASSTHRU*/

  } /* endswitch */

  /* make sure message string is terminated (in case content was too long) */
  trapStringBuf[sizeof(trapStringBuf)-1] = '\0';

  if (trapMgrCfgData.trapAcl == L7_ENABLE)
  {
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_ACL_RULE_LOG_EVENT;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpQosAclRuleLogEventTrapSend(unit, pTrapData);
  }

  return rc;
}

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
L7_uint32 trapMgrGetTrapAcl(void)
{
  return(trapMgrCfgData.trapAcl);
}

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
void trapMgrSetTrapAcl(L7_uint32 val)
{
  trapMgrCfgData.trapAcl = val;
  trapMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
}

