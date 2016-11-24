
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: privatetrap_qos_acl.c
*
* Purpose: QOS ACL trap functions
*
* Created by: gpaussa 06/28/2005
*
* Component: SNMP
*
*********************************************************************/

#include "sr_conf.h"

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif /* HAVE_STDIO_H */
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif /* HAVE_MEMORY_H */
#include "sr_snmp.h"
#include "diag.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "mibout.h"
#include "getvar.h"
#include "sr_ntfy.h"

#include "k_private_base.h" 
#include "l7_common.h"
#include "acl_exports.h"
#include "privatetrap_qos_acl.h"
#include "snmp_trap_api_qos.h"
        
/* Begin Function Declarations: privatetrap_qos_acl.c */

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
L7_RC_t snmp_qosAclRuleLogEventTrapSend(SNMP_QOS_ACL_RULE_LOG_EVENT_TRAP_t *pTrapData)
{
  L7_uint32 status;
  L7_uint32 snmp_aclType;
  L7_uint32 snmp_action;
  UInt64    snmp_hitCount;
  VarBind   *temp_vb = NULL;

  OID *snmpTrapOID = MakeOIDFromDot("aclNotifications.1");

  OID *oid_aclIfAclType = MakeOIDFromDot("aclIfAclType");
  OID *oid_aclIfAclId = MakeOIDFromDot("aclIfAclId");
  OID *oid_aclTrapRuleIndex = MakeOIDFromDot("aclTrapRuleIndex");
  OID *oid_aclTrapRuleAction = MakeOIDFromDot("aclTrapRuleAction");
  OID *oid_aclTrapRuleHitCount = MakeOIDFromDot("aclTrapRuleHitCount");

  UInt64 *ui64_aclTrapRuleHitCount;
  
  VarBind *var_aclIfAclType;
  VarBind *var_aclIfAclId;
  VarBind *var_aclTrapRuleIndex;
  VarBind *var_aclTrapRuleAction;
  VarBind *var_aclTrapRuleHitCount;

  /* convert incoming ACL type to corresponding SNMP data values */
  if (pTrapData->aclType == L7_ACL_TYPE_IP)
    snmp_aclType = D_aclIfAclType_ip;
  else
    snmp_aclType = D_aclIfAclType_mac;

  /* convert incoming rule action to corresponding SNMP data values */
  if (pTrapData->action == L7_ACL_PERMIT)
    snmp_action = D_aclTrapRuleAction_permit;
  else
    snmp_action = D_aclTrapRuleAction_deny;

  /* set up an SNMP 64-bit counter value */
  snmp_hitCount.big_end = (SR_UINT32)pTrapData->hitCount.high;
  snmp_hitCount.little_end = (SR_UINT32)pTrapData->hitCount.low;

  ui64_aclTrapRuleHitCount = CloneUInt64(&snmp_hitCount);
  
  var_aclIfAclType = MakeVarBindWithValue(oid_aclIfAclType, NULL, INTEGER_TYPE, &snmp_aclType);
  var_aclIfAclId = MakeVarBindWithValue(oid_aclIfAclId, NULL, INTEGER_32_TYPE, &pTrapData->aclId);
  var_aclTrapRuleIndex = MakeVarBindWithValue(oid_aclTrapRuleIndex, NULL, INTEGER_32_TYPE, &pTrapData->ruleNum);
  var_aclTrapRuleAction = MakeVarBindWithValue(oid_aclTrapRuleAction, NULL, INTEGER_TYPE, &snmp_action);
  var_aclTrapRuleHitCount = MakeVarBindWithValue(oid_aclTrapRuleHitCount, NULL, COUNTER_64_TYPE, ui64_aclTrapRuleHitCount);

  FreeOID(oid_aclIfAclType);
  FreeOID(oid_aclIfAclId);
  FreeOID(oid_aclTrapRuleIndex);
  FreeOID(oid_aclTrapRuleAction);
  FreeOID(oid_aclTrapRuleHitCount);

  if (snmpTrapOID == NULL || 
      var_aclIfAclType == NULL || 
      var_aclIfAclId == NULL ||
      var_aclTrapRuleIndex == NULL ||
      var_aclTrapRuleAction == NULL ||
      var_aclTrapRuleHitCount == NULL)
  {
    FreeOID(snmpTrapOID);

    if (var_aclTrapRuleHitCount == NULL)
      FreeUInt64(ui64_aclTrapRuleHitCount);

    FreeVarBind(var_aclIfAclType);
    FreeVarBind(var_aclIfAclId);
    FreeVarBind(var_aclTrapRuleIndex);
    FreeVarBind(var_aclTrapRuleAction);
    FreeVarBind(var_aclTrapRuleHitCount);

    return L7_FAILURE;
  }

  /* chain together the variable list elements */
  temp_vb = var_aclIfAclType;
  var_aclIfAclType->next_var = var_aclIfAclId;
  var_aclIfAclId->next_var = var_aclTrapRuleIndex;
  var_aclTrapRuleIndex->next_var = var_aclTrapRuleAction;
  var_aclTrapRuleAction->next_var = var_aclTrapRuleHitCount;
  var_aclTrapRuleHitCount->next_var = NULL;

  status = SendNotificationsSMIv2Params(snmpTrapOID, temp_vb, NULL);

  FreeOID(snmpTrapOID);
  FreeVarBindList(temp_vb);

  if (status == 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}
