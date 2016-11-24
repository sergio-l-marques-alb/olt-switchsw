/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
**********************************************************************
* @filename  k_mib_qos_acl_api.h
*
* @purpose   Quality of Service - ACL MIB
*
* @create    07/24/2002
*
* @author    Colin Verne
*
* @end
*
**********************************************************************/
#include "osapi_support.h"
#include "acl_exports.h"
#include "diffserv_exports.h"
#include "dot1q_exports.h"
#include "usmdb_common.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_util_api.h"

#ifdef L7_QOS_PACKAGE
#include "usmdb_qos_acl_api.h"
#endif


L7_RC_t
snmpQosAclStatusSet(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_aclStatus_active:
    break;

  case D_aclStatus_destroy:
    rc = usmDbQosAclDelete(UnitIndex, aclIndex);
    break;

  default:
    rc = L7_FAILURE;
  }

  return rc;
}

/**************************************************************************************************************/
L7_RC_t
snmpQosAclEntryGet(L7_uint32 UnitIndex, aclEntry_t *aclEntryData, L7_int32 nominator)
{
  L7_RC_t   rc = L7_FAILURE;
  L7_char8  snmp_buffer[SNMP_BUFFER_LEN];

  if ((usmDbQosAclNumCheckValid(UnitIndex, aclEntryData->aclIndex) == L7_SUCCESS) ||
      (usmDbQosAclNamedIndexCheckValid(UnitIndex, L7_ACL_TYPE_IP, aclEntryData->aclIndex) == L7_SUCCESS))
  {
    CLR_VALID(nominator, aclEntryData->valid);

    /*
     * if ( nominator != -1 ) condition is added to all the case statemen
     * for storing all the values to support the undo functionality.
     */
    switch (nominator)
    {
      case -1:
      case I_aclIndex:
        rc = L7_SUCCESS;
        if( nominator != -1) break;
        /* else pass through */

    case I_aclName:
      memset(snmp_buffer, 0, (size_t)SNMP_BUFFER_LEN);
      if ((usmDbQosAclNameGet(UnitIndex,
                              aclEntryData->aclIndex,
                              snmp_buffer) == L7_SUCCESS) &&
          (SafeMakeOctetString(&aclEntryData->aclName,
                               snmp_buffer, 
                               strlen((char *)snmp_buffer)) == L7_TRUE))
      {
        rc = L7_SUCCESS;
        SET_VALID(I_aclName, aclEntryData->valid);
      }
      if (nominator != -1 ) break;
      /* else pass through */

      case I_aclStatus:
        /* all entries are active */
        rc = L7_SUCCESS;
        aclEntryData->aclStatus = D_aclStatus_active;
        SET_VALID(I_aclStatus, aclEntryData->valid);
        break;
        /* last valid nominator */

      default:
        rc = L7_FAILURE;
        break;
    }

    if (nominator >= 0 && rc == L7_SUCCESS)
      SET_VALID(nominator, aclEntryData->valid);
    else if (nominator == -1)
      rc = L7_SUCCESS;
  }

  return rc;
}

L7_RC_t
snmpQosAclEntryGetNext(L7_uint32 UnitIndex, aclEntry_t *aclEntryData, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;

  while ((usmDbQosAclNumGetNext(UnitIndex, aclEntryData->aclIndex, &aclEntryData->aclIndex) == L7_SUCCESS) ||
         (usmDbQosAclNamedIndexGetNext(UnitIndex, L7_ACL_TYPE_IP, aclEntryData->aclIndex, &aclEntryData->aclIndex) == L7_SUCCESS))
  {
    if (snmpQosAclEntryGet(UnitIndex, aclEntryData, nominator) == L7_SUCCESS)
      {
        rc = L7_SUCCESS;
        break;
      }
  }

  return rc;
}


/**************************************************************************************************************/

L7_RC_t
snmpQosAclIntfCheckValid(L7_uint32 UnitIndex, L7_uint32 aclIfIndex, L7_uint32 aclIfDirection,
                         L7_uint32 aclIfSequence, L7_uint32 aclIfAclType, L7_uint32 aclIfAclId)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 count = 0;

  
  L7_ACL_INTF_DIR_LIST_t listInfo;
  
  L7_uint32 temp_direction = 3; /* Initializing with a wrong direction value */


  switch (aclIfDirection)
  {
    case D_aclIfDirection_inbound:
      temp_direction = L7_INBOUND_ACL;
      break;

    case D_aclIfDirection_outbound:
      temp_direction = L7_OUTBOUND_ACL;
      break;

    default:
      rc = L7_FAILURE;
      break;
  }

  rc = usmDbQosAclIntfDirAclListGet( UnitIndex,aclIfIndex,temp_direction,&listInfo);

  if(rc == L7_SUCCESS)
    for(count = 0;count < listInfo.count;count++)
    {
      rc = L7_FAILURE;
      if(listInfo.listEntry[count].seqNum == aclIfSequence)
      {
        rc = L7_SUCCESS;
        break;
      }
    }
   
  if((rc == L7_SUCCESS) && (listInfo.listEntry[count].aclType == aclIfAclType)
                        && (listInfo.listEntry[count].aclId == aclIfAclId))
  {
    rc = L7_SUCCESS;
  }
  else
    rc = L7_FAILURE;

  return rc;
}

L7_RC_t
snmpQosAclIntfNext(L7_uint32 UnitIndex, L7_uint32 *aclIfIndex, L7_uint32 *aclIfDirection, 
                   L7_uint32 *aclIfSequence, L7_uint32 *aclIfAclType, L7_uint32 *aclIfAclId)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 count = 0;

  L7_ACL_INTF_DIR_LIST_t listInfo;

  L7_uint32 nextAclIfIndex,nextAclIfDirection,nextAclIfSequence;
  
  L7_uint32 temp_direction, temp_sequence; 

  temp_sequence = *aclIfSequence;

  switch (*aclIfDirection)
  {
    case 0:
      temp_direction = 0;
      temp_sequence = 0;
      break;

    case D_aclIfDirection_inbound:
      temp_direction = L7_INBOUND_ACL;
      break;

    case D_aclIfDirection_outbound:
      temp_direction = L7_OUTBOUND_ACL;
      break;

    default:
      temp_direction = (*aclIfDirection)-1;
      temp_sequence = 0;
      break;
  }

  while(1)
  {
    rc = usmDbQosAclIntfDirSequenceGetNext(UnitIndex,*aclIfIndex,temp_direction,
                                           temp_sequence, 
                                           &nextAclIfSequence); 
    if(rc == L7_SUCCESS)
    {
      *aclIfSequence = nextAclIfSequence;
      break;
    }
   
    if (rc != L7_SUCCESS)
    {
      /* Trying to get next (interface + direction) in use */
      rc = usmDbQosAclIntfDirGetNext(UnitIndex,
                                     *aclIfIndex,
                                     temp_direction,
                                     &nextAclIfIndex,
                                     &nextAclIfDirection);
    
      if(rc != L7_SUCCESS)
      {
        break;
      }
           
      else if( rc == L7_SUCCESS && (usmDbQosAclIsInterfaceInUse(UnitIndex,
                                                       nextAclIfIndex,
                                                       nextAclIfDirection)) == L7_TRUE)
      {
          *aclIfIndex = nextAclIfIndex;
          temp_direction = nextAclIfDirection;
          *aclIfSequence = 0;
          rc = L7_SUCCESS;
      }

      else if( rc == L7_SUCCESS && (usmDbQosAclIsInterfaceInUse(UnitIndex,
                                                       nextAclIfIndex,
                                                       nextAclIfDirection)) != L7_TRUE)
      {
          *aclIfIndex = nextAclIfIndex;
          temp_direction = nextAclIfDirection;
          *aclIfSequence = 0;
          rc = L7_FAILURE;
          continue;
      }
      

      if (rc == L7_SUCCESS)
      { 
        /* Get the first sequence for this interface and direction with input value of 
           *aclIfSequence = 0 */
        rc = usmDbQosAclIntfDirSequenceGetNext(UnitIndex,nextAclIfIndex,nextAclIfDirection,
                                               *aclIfSequence,
                                               &nextAclIfSequence);

        if(rc == L7_SUCCESS)
        {
          *aclIfIndex = nextAclIfIndex;
          temp_direction = nextAclIfDirection; 
          *aclIfSequence = nextAclIfSequence;
          break;
        }
          
      }
    } /* end of if (rc != L7_SUCCESS) */

  } /* end of while(1) */


  /* Once got the values of next intfIndex, Direction and sequence, now need to get
     next aclType and aclId. For a pair of (intfIndex+ Direction), there would be only one
     combination of (aclType + aclId) for a particular value of aclIfSequence. */
  if(rc == L7_SUCCESS)
    rc = usmDbQosAclIntfDirAclListGet( UnitIndex,*aclIfIndex,temp_direction,&listInfo);

  if(rc == L7_SUCCESS)
    for(count = 0;count < listInfo.count;count++)
    {
      rc = L7_FAILURE;
      if(listInfo.listEntry[count].seqNum == *aclIfSequence)
      {
        rc = L7_SUCCESS;
        break;
      }
    }
   
  if(rc == L7_SUCCESS)
  {
    switch (temp_direction)
    {
      case L7_INBOUND_ACL: 
        *aclIfDirection = D_aclIfDirection_inbound;
        break;

      case L7_OUTBOUND_ACL: 
        *aclIfDirection = D_aclIfDirection_outbound;
        break;

      default:
        rc = L7_FAILURE;
        break;
    }
    *aclIfAclType = listInfo.listEntry[count].aclType; 
    *aclIfAclId =   listInfo.listEntry[count].aclId;  
  }

  return rc;
}

L7_BOOL
snmpIsQosAclIdAttachedToInterface(L7_uint32 UnitIndex, L7_uint32 aclIndex)
{
  L7_RC_t rc = L7_FALSE;
  L7_ACL_ASSIGNED_INTF_LIST_t intfList;
   
  /* Passing 3rd argument as 0 for inward and 1 for outward directions */
  if((usmDbQosAclAssignedIntfDirListGet(UnitIndex,aclIndex, 
                                       0, &intfList) == L7_SUCCESS)
    ||(usmDbQosAclAssignedIntfDirListGet(UnitIndex,aclIndex, 
                                       1, &intfList) == L7_SUCCESS))

  {
    rc = L7_TRUE;
  }
  else 
    rc = L7_FALSE;
  
  return rc;
}


L7_RC_t
snmpQosAclIntfAdd(L7_uint32 UnitIndex, L7_uint32 aclIfIndex, L7_uint32 aclIfDirection,
                  L7_uint32 aclIfSequence, L7_uint32 aclIfAclType, L7_uint32 aclIfAclId)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_direction = 3; /* Initializing with a wrong direction value */


  switch (aclIfDirection)
  {
  case D_aclIfDirection_inbound:
    temp_direction = L7_INBOUND_ACL;
    break;

  case D_aclIfDirection_outbound:
    temp_direction = L7_OUTBOUND_ACL;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }


  if (rc == L7_SUCCESS && aclIfAclType == D_aclIfAclType_ip)
    rc = usmDbQosAclInterfaceDirectionAdd(UnitIndex, aclIfIndex, 
                                          temp_direction, aclIfAclId, aclIfSequence);  

  if (rc == L7_SUCCESS && aclIfAclType == D_aclIfAclType_ipv6)
    rc = usmDbQosAclInterfaceDirectionAdd(UnitIndex, aclIfIndex, 
                                          temp_direction, aclIfAclId, aclIfSequence);  

  if (rc == L7_SUCCESS && aclIfAclType == D_aclIfAclType_mac)
    rc = usmDbQosAclMacInterfaceDirectionAdd(UnitIndex, aclIfIndex, 
                                          temp_direction, aclIfAclId, aclIfSequence);  

  return rc;
}

L7_RC_t
snmpQosAclIntfStatusSet(L7_uint32 UnitIndex, L7_uint32 aclIfIndex, L7_uint32 aclIfDirection,
                        L7_uint32 aclIfSequence, L7_uint32 aclIfAclType, L7_uint32 aclIfAclId,
                        L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  L7_uint32 temp_direction = 3; /* Initializing with a wrong direction value */


  switch (aclIfDirection)
  {
  case D_aclIfDirection_inbound:
    temp_direction = L7_INBOUND_ACL;
    break;

  case D_aclIfDirection_outbound:
    temp_direction = L7_OUTBOUND_ACL;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }
  
  switch (val)
  {
  case D_aclStatus_active:
    break;

  case D_aclStatus_destroy:
  
  if (rc == L7_SUCCESS && aclIfAclType == D_aclIfAclType_ip)
  {
    rc = usmDbQosAclInterfaceDirectionRemove(UnitIndex, aclIfIndex, 
                                             temp_direction, aclIfAclId);
  }

  if (rc == L7_SUCCESS && aclIfAclType == D_aclIfAclType_ipv6)
  {
    rc = usmDbQosAclInterfaceDirectionRemove(UnitIndex, aclIfIndex, 
                                             temp_direction, aclIfAclId);
  }

  if (rc == L7_SUCCESS && aclIfAclType == D_aclIfAclType_mac)
  {

    rc = usmDbQosAclMacInterfaceDirectionRemove(UnitIndex, aclIfIndex, 
                                          temp_direction, aclIfAclId);    

  }
        break;

  default:
    rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t
snmpQosAclRuleEntryAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 aclRuleIndex)
{
  L7_RC_t rc = L7_SUCCESS;

  rc = usmDbQosAclRuleActionAdd(UnitIndex, aclIndex, aclRuleIndex, L7_ACL_DENY);

  return rc;
}

L7_RC_t
snmpQosAclRuleActionGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 aclRuleIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_action;

  rc = usmDbQosAclRuleActionGet(UnitIndex, aclIndex, aclRuleIndex, &temp_action);

  if (rc == L7_SUCCESS)
  {
    switch (temp_action)
    {
    case L7_ACL_PERMIT:
      *val = D_aclRuleAction_permit;
      break;

    case L7_ACL_DENY:
      *val = D_aclRuleAction_deny;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpQosAclRuleActionAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 aclRuleIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_action = 0;

  switch (val)
  {
  case D_aclRuleAction_permit:
    temp_action = L7_ACL_PERMIT;
    break;

  case D_aclRuleAction_deny:
    temp_action = L7_ACL_DENY;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbQosAclRuleActionAdd(UnitIndex, aclIndex, aclRuleIndex, temp_action);
  }

  return rc;
}

L7_RC_t
snmpQosAclRuleStatusSet(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 aclRuleIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_aclStatus_active:
    break;

  case D_aclStatus_destroy:
    rc = usmDbQosAclRuleRemove(UnitIndex, aclIndex, aclRuleIndex);
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

L7_RC_t
snmpQosAclRuleSrcIpMaskGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 aclRuleIndex, L7_uint32 *val1, L7_uint32 *val2)
{
  L7_RC_t rc;

  rc = usmDbQosAclRuleSrcIpMaskGet(UnitIndex, aclIndex, aclRuleIndex, val1, val2);

  if (usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) != L7_TRUE)
  {
    *val2 = L7_IP_SUBMASK_EXACT_MATCH_MASK;
  }

  if(rc == L7_SUCCESS)
     /* *val2 = (*val2)^(~(L7_uint32 )0); */
    *val2 = ~(*val2);
  return rc;
}

L7_RC_t
snmpQosAclRuleSrcIpMaskAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 aclRuleIndex, L7_uint32 val1, L7_uint32 val2)
{
  L7_RC_t rc = L7_SUCCESS;

  /* val2  = val2^(~(L7_uint32 )0);*/
  val2  = ~(val2);

  rc = usmDbQosAclRuleSrcIpMaskAdd(UnitIndex, aclIndex, aclRuleIndex, val1, val2);

  return rc;
}

L7_RC_t
snmpQosAclRuleDstIpMaskGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 aclRuleIndex, L7_uint32 *val1, L7_uint32 *val2)
{
  L7_RC_t rc;

  rc = usmDbQosAclRuleDstIpMaskGet(UnitIndex, aclIndex, aclRuleIndex, val1, val2);

  if (usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) != L7_TRUE)
  {
    *val2 = L7_IP_SUBMASK_EXACT_MATCH_MASK;
  }

  if(rc == L7_SUCCESS)
    /* *val2 = (*val2)^(~(L7_uint32 )0); */
    *val2 = ~(*val2);

  return rc;
}

L7_RC_t
snmpQosAclRuleDstIpMaskAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 aclRuleIndex, L7_uint32 val1, L7_uint32 val2)
{
  L7_RC_t rc = L7_SUCCESS;

  /* val2  = val2^(~(L7_uint32 )0); */
  val2  = ~(val2);

  rc = usmDbQosAclRuleDstIpMaskAdd(UnitIndex, aclIndex, aclRuleIndex, val1, val2);

  return rc;
}


L7_RC_t
snmpQosAclRuleRedirectIntfGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                              L7_uint32 aclRuleIndex, L7_uint32 *ifIndex)
{
  L7_RC_t   rc;
  L7_uint32 intIfNum;
             
  rc = usmDbQosAclRuleRedirectIntfGet(UnitIndex, aclIndex, aclRuleIndex,
                                      &intIfNum);
   
  /* If no redirect Interface is found, show the defaut value i.e., 0 */
  if (rc != L7_SUCCESS)
  {
    *ifIndex = 0;
    rc = L7_SUCCESS; 
  }
  else
  {
    /* translate intIfNum to external ifIndex */
    rc = usmDbExtIfNumFromIntIfNum(intIfNum, ifIndex);
  }
  
  return rc;
}

L7_RC_t
snmpQosAclRuleRedirectIntfAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                              L7_uint32 aclRuleIndex, L7_uint32 ifIndex)
{
  L7_RC_t   rc;
  L7_uint32 intIfNum;
             
  /* translate external ifIndex to intIfNum */
  rc = usmDbIntIfNumFromExtIfNum(ifIndex, &intIfNum);

  if (rc == L7_SUCCESS)
  {
    rc = usmDbQosAclRuleRedirectIntfAdd(UnitIndex, aclIndex, aclRuleIndex, 
                                        intIfNum);
  }
  
  return rc;
}

L7_RC_t          
snmpQosAclRuleEveryGet ( L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                         L7_uint32 aclRuleIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbQosAclRuleEveryGet (UnitIndex, aclIndex, aclRuleIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_aclRuleMatchEvery_true;
      break;

    case L7_FALSE:
      *val = D_aclRuleMatchEvery_false;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}


L7_RC_t          
snmpQosAclRuleEveryAdd ( L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                         L7_uint32 aclRuleIndex, L7_int32 val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  switch (val)
  {
    case D_aclRuleMatchEvery_true:
      temp_val = L7_TRUE;
      break;

    case D_aclRuleMatchEvery_false:
      temp_val = L7_FALSE;
      break;

    default:
      temp_val = 0;
      rc = L7_FAILURE;
      break;
  }

  rc = usmDbQosAclRuleEveryAdd (UnitIndex, aclIndex, aclRuleIndex, temp_val);


  return rc;
}

L7_RC_t
snmpQosAclRuleMirrorIntfGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                            L7_uint32 aclRuleIndex, L7_uint32 *ifIndex)
{
  L7_RC_t   rc;
  L7_uint32 intIfNum;
             
  rc = usmDbQosAclRuleMirrorIntfGet(UnitIndex, aclIndex, aclRuleIndex,
                                    &intIfNum);
   
  /* If no mirror Interface is found, show the defaut value i.e., 0 */
  if (rc != L7_SUCCESS)
  {
    *ifIndex = 0;
    rc = L7_SUCCESS; 
  }
  else
  {
    /* translate intIfNum to external ifIndex */
    rc = usmDbExtIfNumFromIntIfNum(intIfNum, ifIndex);
  }
  
  return rc;
}

L7_RC_t
snmpQosAclRuleMirrorIntfAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                            L7_uint32 aclRuleIndex, L7_uint32 ifIndex)
{
  L7_RC_t   rc;
  L7_uint32 intIfNum;
             
  /* translate external ifIndex to intIfNum */
  rc = usmDbIntIfNumFromExtIfNum(ifIndex, &intIfNum);

  if (rc == L7_SUCCESS)
  {
    rc = usmDbQosAclRuleMirrorIntfAdd(UnitIndex, aclIndex, aclRuleIndex, 
                                      intIfNum);
  }
  
  return rc;
}

L7_RC_t          
snmpQosAclRuleLoggingGet ( L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                           L7_uint32 aclRuleIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbQosAclRuleLoggingGet (UnitIndex, aclIndex, aclRuleIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_aclRuleLogging_true;
      break;

    case L7_FALSE:
      *val = D_aclRuleLogging_false;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}


L7_RC_t          
snmpQosAclRuleLoggingAdd ( L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                           L7_uint32 aclRuleIndex, L7_int32 val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  switch (val)
  {
    case D_aclRuleLogging_true:
      temp_val = L7_TRUE;
      break;

    case D_aclRuleLogging_false:
      temp_val = L7_FALSE;
      break;

    default:
      temp_val = 0;
      rc = L7_FAILURE;
      break;
  }

  rc = usmDbQosAclRuleLoggingAdd (UnitIndex, aclIndex, aclRuleIndex, temp_val);


  return rc;
}

/**************************************************************************************************************/

L7_RC_t
snmpQosAclRuleEntryGet(L7_uint32 UnitIndex, aclRuleEntry_t *aclRuleEntryData, L7_int32 nominator)
{
  L7_RC_t   rc = L7_FAILURE;
  L7_BOOL   isStandardAcl = L7_FALSE;
  L7_uint32 temp_uint;

  if ((aclRuleEntryData->aclIndex >= L7_ACL_MIN_STD1_ID) && 
      (aclRuleEntryData->aclIndex <= L7_ACL_MAX_STD1_ID))
    isStandardAcl = L7_TRUE;

  if (usmDbQosAclRuleCheckValid(UnitIndex, aclRuleEntryData->aclIndex, aclRuleEntryData->aclRuleIndex) == L7_SUCCESS)
  {
    CLR_VALID(nominator, aclRuleEntryData->valid);

    /*
     * if ( nominator != -1 ) condition is added to all the case statements
     * for storing all the values to support the undo functionality.
     */
    switch (nominator)
    {
      case -1:
      case I_aclRuleEntryIndex_aclIndex:
      case I_aclRuleIndex:
        rc = L7_SUCCESS;
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclRuleAction:
        if (usmDbQosAclIsFieldConfigured(UnitIndex, aclRuleEntryData->aclIndex,
                                         aclRuleEntryData->aclRuleIndex,
                                         ACL_ACTION) == L7_TRUE)
        {
          rc = snmpQosAclRuleActionGet(UnitIndex,
                                       aclRuleEntryData->aclIndex,
                                       aclRuleEntryData->aclRuleIndex,
                                       &aclRuleEntryData->aclRuleAction);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclRuleAction, aclRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclRuleProtocol:
        if ((isStandardAcl == L7_FALSE) &&
            (usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_PROTOCOL_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclIsFieldConfigured(UnitIndex, aclRuleEntryData->aclIndex,
                                          aclRuleEntryData->aclRuleIndex,
                                          ACL_PROTOCOL) == L7_TRUE))
        {
          rc = usmDbQosAclRuleProtocolGet(UnitIndex,
                                          aclRuleEntryData->aclIndex,
                                          aclRuleEntryData->aclRuleIndex,
                                          &aclRuleEntryData->aclRuleProtocol);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclRuleProtocol, aclRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclRuleSrcIpAddress:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclIsFieldConfigured(UnitIndex, aclRuleEntryData->aclIndex,
                                          aclRuleEntryData->aclRuleIndex,
                                          ACL_SRCIP) == L7_TRUE))
        {
          rc = snmpQosAclRuleSrcIpMaskGet(UnitIndex,
                                          aclRuleEntryData->aclIndex,
                                          aclRuleEntryData->aclRuleIndex,
                                          &aclRuleEntryData->aclRuleSrcIpAddress,
                                          &temp_uint);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclRuleSrcIpAddress, aclRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclRuleSrcIpMask:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID) == L7_TRUE) &&
            (usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclIsFieldConfigured(UnitIndex, aclRuleEntryData->aclIndex,
                                          aclRuleEntryData->aclRuleIndex,
                                          ACL_SRCIP_MASK) == L7_TRUE))
        {
          rc = snmpQosAclRuleSrcIpMaskGet(UnitIndex,
                                          aclRuleEntryData->aclIndex,
                                          aclRuleEntryData->aclRuleIndex,
                                          &temp_uint,
                                          &aclRuleEntryData->aclRuleSrcIpMask);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclRuleSrcIpMask, aclRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclRuleSrcL4Port:
        if ((isStandardAcl == L7_FALSE) &&
            (usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_SRCL4PORT_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclIsFieldConfigured(UnitIndex, aclRuleEntryData->aclIndex,
                                          aclRuleEntryData->aclRuleIndex,
                                          ACL_SRCPORT) == L7_TRUE))
        {
          rc = usmDbQosAclRuleSrcL4PortGet(UnitIndex,
                                           aclRuleEntryData->aclIndex,
                                           aclRuleEntryData->aclRuleIndex,
                                           &aclRuleEntryData->aclRuleSrcL4Port);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclRuleSrcL4Port, aclRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclRuleSrcL4PortRangeStart:
        if ((isStandardAcl == L7_FALSE) &&
            (usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_SRCL4PORT_RANGE_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclIsFieldConfigured(UnitIndex, aclRuleEntryData->aclIndex,
                                          aclRuleEntryData->aclRuleIndex,
                                          ACL_SRCSTARTPORT) == L7_TRUE))
        {
          rc = usmDbQosAclRuleSrcL4PortRangeGet(UnitIndex,
                                                aclRuleEntryData->aclIndex,
                                                aclRuleEntryData->aclRuleIndex,
                                                &aclRuleEntryData->aclRuleSrcL4PortRangeStart,
                                                &temp_uint);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclRuleSrcL4PortRangeStart, aclRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclRuleSrcL4PortRangeEnd:
        if ((isStandardAcl == L7_FALSE) &&
            (usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_SRCL4PORT_RANGE_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclIsFieldConfigured(UnitIndex, aclRuleEntryData->aclIndex,
                                          aclRuleEntryData->aclRuleIndex,
                                          ACL_SRCENDPORT) == L7_TRUE))
        {
          rc = usmDbQosAclRuleSrcL4PortRangeGet(UnitIndex,
                                                aclRuleEntryData->aclIndex,
                                                aclRuleEntryData->aclRuleIndex,
                                                &temp_uint,
                                                &aclRuleEntryData->aclRuleSrcL4PortRangeEnd);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclRuleSrcL4PortRangeEnd, aclRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclRuleDestIpAddress:
        if ((isStandardAcl == L7_FALSE) &&
            (usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclIsFieldConfigured(UnitIndex, aclRuleEntryData->aclIndex,
                                          aclRuleEntryData->aclRuleIndex,
                                          ACL_DSTIP) == L7_TRUE))
        {
          rc = usmDbQosAclRuleDstIpMaskGet(UnitIndex,
                                           aclRuleEntryData->aclIndex,
                                           aclRuleEntryData->aclRuleIndex,
                                           &aclRuleEntryData->aclRuleDestIpAddress,
                                           &temp_uint);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclRuleDestIpAddress, aclRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclRuleDestIpMask:
        if ((isStandardAcl == L7_FALSE) &&
            (usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID) == L7_TRUE) &&
            (usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclIsFieldConfigured(UnitIndex, aclRuleEntryData->aclIndex,
                                          aclRuleEntryData->aclRuleIndex,
                                          ACL_DSTIP_MASK) == L7_TRUE))
        {
          rc = snmpQosAclRuleDstIpMaskGet(UnitIndex,
                                          aclRuleEntryData->aclIndex,
                                          aclRuleEntryData->aclRuleIndex,
                                          &temp_uint,
                                          &aclRuleEntryData->aclRuleDestIpMask);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclRuleDestIpMask, aclRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclRuleDestL4Port:
        if ((isStandardAcl == L7_FALSE) &&
            (usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_DSTL4PORT_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclIsFieldConfigured(UnitIndex, aclRuleEntryData->aclIndex,
                                          aclRuleEntryData->aclRuleIndex,
                                          ACL_DSTPORT) == L7_TRUE))
        {
          rc = usmDbQosAclRuleDstL4PortGet(UnitIndex,
                                           aclRuleEntryData->aclIndex,
                                           aclRuleEntryData->aclRuleIndex,
                                           &aclRuleEntryData->aclRuleDestL4Port);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclRuleDestL4Port, aclRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclRuleDestL4PortRangeStart:
        if ((isStandardAcl == L7_FALSE) &&
            (usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_DSTL4PORT_RANGE_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclIsFieldConfigured(UnitIndex, aclRuleEntryData->aclIndex,
                                          aclRuleEntryData->aclRuleIndex,
                                          ACL_DSTSTARTPORT) == L7_TRUE))
        {
          rc = usmDbQosAclRuleDstL4PortRangeGet(UnitIndex,
                                                aclRuleEntryData->aclIndex,
                                                aclRuleEntryData->aclRuleIndex,
                                                &aclRuleEntryData->aclRuleDestL4PortRangeStart,
                                                &temp_uint);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclRuleDestL4PortRangeStart, aclRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

    case I_aclRuleDestL4PortRangeEnd:
        if ((isStandardAcl == L7_FALSE) &&
            (usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_DSTL4PORT_RANGE_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclIsFieldConfigured(UnitIndex, aclRuleEntryData->aclIndex,
                                          aclRuleEntryData->aclRuleIndex,
                                          ACL_DSTENDPORT) == L7_TRUE))
        {
          rc = usmDbQosAclRuleDstL4PortRangeGet(UnitIndex,
                                                aclRuleEntryData->aclIndex,
                                                aclRuleEntryData->aclRuleIndex,
                                                &temp_uint,
                                                &aclRuleEntryData->aclRuleDestL4PortRangeEnd);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclRuleDestL4PortRangeEnd, aclRuleEntryData->valid);
        }
        if (nominator != -1 ) break;

    case I_aclRuleIPDSCP:
        if ((isStandardAcl == L7_FALSE) &&
            (usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_IPDSCP_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclIsFieldConfigured(UnitIndex, aclRuleEntryData->aclIndex,
                                          aclRuleEntryData->aclRuleIndex,
                                          ACL_IPDSCP) == L7_TRUE))
        {
          rc = usmDbQosAclRuleIPDscpGet(UnitIndex,
                                        aclRuleEntryData->aclIndex,
                                        aclRuleEntryData->aclRuleIndex,
                                        &aclRuleEntryData->aclRuleIPDSCP);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclRuleIPDSCP, aclRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

    case I_aclRuleIpPrecedence:
        if ((isStandardAcl == L7_FALSE) &&
            (usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_IPPRECEDENCE_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclIsFieldConfigured(UnitIndex, aclRuleEntryData->aclIndex,
                                          aclRuleEntryData->aclRuleIndex,
                                          ACL_IPPREC) == L7_TRUE))
        {
          rc = usmDbQosAclRuleIPPrecedenceGet(UnitIndex,
                                              aclRuleEntryData->aclIndex,
                                              aclRuleEntryData->aclRuleIndex,
                                              &aclRuleEntryData->aclRuleIpPrecedence);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclRuleIpPrecedence, aclRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

    case I_aclRuleIpTosBits:
       if ((isStandardAcl == L7_FALSE) &&
           (usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_RULE_MATCH_IPTOS_FEATURE_ID) == L7_TRUE) &&
           (usmDbQosAclIsFieldConfigured(UnitIndex, aclRuleEntryData->aclIndex,
                                         aclRuleEntryData->aclRuleIndex,
                                         ACL_IPTOS) == L7_TRUE))
       {
         rc = usmDbQosAclRuleIPTosGet(UnitIndex,
                                      aclRuleEntryData->aclIndex,
                                      aclRuleEntryData->aclRuleIndex,
                                      &aclRuleEntryData->aclRuleIpTosBits,
                                      &temp_uint);
         if (rc == L7_SUCCESS)
           SET_VALID(I_aclRuleIpTosBits, aclRuleEntryData->valid);
       }
       if (nominator != -1 ) break;
       /* else pass through */

    case I_aclRuleIpTosMask:
       if ((isStandardAcl == L7_FALSE) &&
           (usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_RULE_MATCH_IPTOS_FEATURE_ID) == L7_TRUE) &&
           (usmDbQosAclIsFieldConfigured(UnitIndex, aclRuleEntryData->aclIndex,
                                         aclRuleEntryData->aclRuleIndex,
                                         ACL_IPTOS) == L7_TRUE))
       {
         rc = usmDbQosAclRuleIPTosGet(UnitIndex,
                                      aclRuleEntryData->aclIndex,
                                      aclRuleEntryData->aclRuleIndex,
                                      &temp_uint,
                                      &aclRuleEntryData->aclRuleIpTosMask);
         if (rc == L7_SUCCESS)
         {
           aclRuleEntryData->aclRuleIpTosMask=~(aclRuleEntryData->aclRuleIpTosMask) & 0x000000FF; 
           SET_VALID(I_aclRuleIpTosMask, aclRuleEntryData->valid);
         }
       }
       if (nominator != -1 ) break;
       /* else pass through */

    case I_aclRuleStatus:
       /* all entries are active */
       rc = L7_SUCCESS;
       aclRuleEntryData->aclRuleStatus = D_aclRuleStatus_active;
       SET_VALID(I_aclRuleStatus, aclRuleEntryData->valid);
       if (nominator != -1 ) break;
       /* else pass through */

    case I_aclRuleAssignQueueId:
       if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_ASSIGN_QUEUE_FEATURE_ID) == L7_TRUE) &&
           (usmDbQosAclIsFieldConfigured(UnitIndex, aclRuleEntryData->aclIndex,
                                         aclRuleEntryData->aclRuleIndex,
                                         ACL_ASSIGN_QUEUEID) == L7_TRUE))
       {
         rc = usmDbQosAclRuleAssignQueueIdGet(UnitIndex,
                                              aclRuleEntryData->aclIndex,
                                              aclRuleEntryData->aclRuleIndex,
                                              &aclRuleEntryData->aclRuleAssignQueueId);
         if (rc == L7_SUCCESS)
           SET_VALID(I_aclRuleAssignQueueId, aclRuleEntryData->valid);
       }
       if (nominator != -1 ) break;
       /* else pass through */

    case I_aclRuleRedirectIntf:
       if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE) &&
           (usmDbQosAclIsFieldConfigured(UnitIndex, aclRuleEntryData->aclIndex,
                                         aclRuleEntryData->aclRuleIndex,
                                         ACL_REDIRECT_INTF) == L7_TRUE))
       {
         rc = snmpQosAclRuleRedirectIntfGet(UnitIndex,
                                            aclRuleEntryData->aclIndex,
                                            aclRuleEntryData->aclRuleIndex,
                                            &aclRuleEntryData->aclRuleRedirectIntf);
         if (rc == L7_SUCCESS)
           SET_VALID(I_aclRuleRedirectIntf, aclRuleEntryData->valid);
       }
       if (nominator != -1 ) break;
       /* else pass through */

    case I_aclRuleMatchEvery:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_EVERY_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclIsFieldConfigured(UnitIndex, aclRuleEntryData->aclIndex,
                                          aclRuleEntryData->aclRuleIndex,
                                          ACL_EVERY) == L7_TRUE))
        {
          rc = snmpQosAclRuleEveryGet(UnitIndex,
                                      aclRuleEntryData->aclIndex,
                                      aclRuleEntryData->aclRuleIndex,
                                      &aclRuleEntryData->aclRuleMatchEvery);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclRuleMatchEvery, aclRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

    case I_aclRuleMirrorIntf:
       if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE) &&
           (usmDbQosAclIsFieldConfigured(UnitIndex, aclRuleEntryData->aclIndex,
                                         aclRuleEntryData->aclRuleIndex,
                                         ACL_MIRROR_INTF) == L7_TRUE))
       {
         rc = snmpQosAclRuleMirrorIntfGet(UnitIndex,
                                          aclRuleEntryData->aclIndex,
                                          aclRuleEntryData->aclRuleIndex,
                                          &aclRuleEntryData->aclRuleMirrorIntf);
         if (rc == L7_SUCCESS)
           SET_VALID(I_aclRuleMirrorIntf, aclRuleEntryData->valid);
       }
       if (nominator != -1 ) break;
       /* else pass through */

    case I_aclRuleLogging:
        if ((usmDbQosAclRuleLoggingAllowed(USMDB_UNIT_CURRENT, 
                                           aclRuleEntryData->aclIndex, 
                                           aclRuleEntryData->aclRuleIndex) == L7_SUCCESS) &&
            (usmDbQosAclIsFieldConfigured(UnitIndex, 
                                          aclRuleEntryData->aclIndex,
                                          aclRuleEntryData->aclRuleIndex,
                                          ACL_LOGGING) == L7_TRUE))
        {
          rc = snmpQosAclRuleLoggingGet(UnitIndex,
                                        aclRuleEntryData->aclIndex,
                                        aclRuleEntryData->aclRuleIndex,
                                        &aclRuleEntryData->aclRuleLogging);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclRuleLogging, aclRuleEntryData->valid);
        }
       break;
       /* last valid nominator */

    default:
      rc = L7_FAILURE;
      break;
    }

    if (nominator >= 0 && rc == L7_SUCCESS)
      SET_VALID(nominator, aclRuleEntryData->valid);
    else if (nominator == -1)
      rc = L7_SUCCESS;
  }

  return rc;
}

L7_RC_t
snmpQosAclRuleEntryGetNext(L7_uint32 UnitIndex, aclRuleEntry_t *aclRuleEntryData, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;

  while (1)
  {
    if (usmDbQosAclRuleGetNext(UnitIndex, 
                               aclRuleEntryData->aclIndex, 
                               aclRuleEntryData->aclRuleIndex, 
                               &aclRuleEntryData->aclRuleIndex) == L7_SUCCESS)
    {
      if (snmpQosAclRuleEntryGet(UnitIndex, aclRuleEntryData, nominator) == L7_SUCCESS)
      {
        rc = L7_SUCCESS;
        break;
      }
    }
    else
    {
      /* no more rules in current access list, so move on to next list */
      if ((usmDbQosAclNumGetNext(UnitIndex, aclRuleEntryData->aclIndex, &aclRuleEntryData->aclIndex) == L7_SUCCESS) ||
          (usmDbQosAclNamedIndexGetNext(UnitIndex, L7_ACL_TYPE_IP, aclRuleEntryData->aclIndex, &aclRuleEntryData->aclIndex) == L7_SUCCESS))
        aclRuleEntryData->aclRuleIndex = 0;
      else
        break;
    }
  }

  return rc;
}


/**************************************************************************************************************/

L7_RC_t
snmpQosMacAclStatusSet(L7_uint32 UnitIndex, L7_uint32 aclMacIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_aclMacStatus_active:
    break;

  case D_aclMacStatus_destroy:
    rc = usmDbQosAclMacDelete(UnitIndex, aclMacIndex);
    break;

  default:
    rc = L7_FAILURE;
  }

  return rc;
}


/**************************************************************************************************************/

L7_RC_t
snmpQosAclMacEntryGet(L7_uint32 UnitIndex, aclMacEntry_t *aclMacEntryData, L7_int32 nominator)
{
  L7_RC_t   rc = L7_FAILURE;
  L7_char8  snmp_buffer[SNMP_BUFFER_LEN];

  if (usmDbQosAclMacIndexCheckValid(UnitIndex, aclMacEntryData->aclMacIndex) == L7_SUCCESS)
  {
    CLR_VALID(nominator, aclMacEntryData->valid);

    /*
     * if ( nominator != -1 ) condition is added to all the case statemen
     * for storing all the values to support the undo functionality.
     */
    switch (nominator)
    {
      case -1:
      case I_aclMacIndex:
        rc = L7_SUCCESS;
        if( nominator != -1) break;
        /* else pass through */

      case I_aclMacName:
        memset(snmp_buffer, 0, (size_t)SNMP_BUFFER_LEN);
        if ((usmDbQosAclMacNameGet(UnitIndex,
                                   aclMacEntryData->aclMacIndex,
                                   snmp_buffer) == L7_SUCCESS) &&
            (SafeMakeOctetString(&aclMacEntryData->aclMacName,
                                 snmp_buffer, 
                                 strlen((char *)snmp_buffer)) == L7_TRUE))
        {
          rc = L7_SUCCESS;
          SET_VALID(I_aclMacName, aclMacEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclMacStatus:
        /* all entries are active */
        rc = L7_SUCCESS;
        aclMacEntryData->aclMacStatus = D_aclStatus_active;
        SET_VALID(I_aclMacStatus, aclMacEntryData->valid);
        break;
        /* last valid nominator */

      default:
        rc = L7_FAILURE;
        break;
    }

    if (nominator >= 0 && rc == L7_SUCCESS)
      SET_VALID(nominator, aclMacEntryData->valid);
    else if (nominator == -1)
      rc = L7_SUCCESS;
  }

  return rc;
}

L7_RC_t
snmpQosAclMacEntryGetNext(L7_uint32 UnitIndex, aclMacEntry_t *aclMacEntryData, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;

  while (usmDbQosAclMacIndexGetNext(UnitIndex, 
                                    aclMacEntryData->aclMacIndex, 
                                    &aclMacEntryData->aclMacIndex) == L7_SUCCESS)
  {
    if (snmpQosAclMacEntryGet(UnitIndex, aclMacEntryData, nominator) == L7_SUCCESS)
      {
        rc = L7_SUCCESS;
        break;
      }
  }

  return rc;
}


/**************************************************************************************************************/

L7_RC_t
snmpQosAclMacRuleEntryAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 aclMacRuleIndex)
{
  L7_RC_t rc = L7_SUCCESS;
  
  /* Creating a rule with Action as DENY */ 
  rc = usmDbQosAclMacRuleActionAdd(UnitIndex, aclIndex, aclMacRuleIndex, L7_ACL_DENY);

  return rc;
}

L7_RC_t
snmpQosAclMacRuleActionGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 aclMacRuleIndex, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_action;

  rc = usmDbQosAclMacRuleActionGet(UnitIndex, aclIndex, aclMacRuleIndex, &temp_action);


  if (rc == L7_SUCCESS)
  {
    switch (temp_action)
    {
    case L7_ACL_PERMIT:
      *val = D_aclMacRuleAction_permit;
      break;

    case L7_ACL_DENY:
      *val = D_aclMacRuleAction_deny;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpQosAclMacRuleEtypeKeyGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 aclMacRuleIndex, L7_uint32 *val1)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 eTypeKey, eTypeValue;

  rc = usmDbQosAclMacRuleEtypeKeyGet(UnitIndex, aclIndex, aclMacRuleIndex, &eTypeKey, &eTypeValue);
  
  if (rc == L7_SUCCESS)
  {
    switch (eTypeKey)
    {
    case L7_QOS_ETYPE_KEYID_NONE:
      *val1 = 0;
      break;

    case L7_QOS_ETYPE_KEYID_CUSTOM:
      *val1 = D_aclMacRuleEtypeKey_custom;
      break;

    case L7_QOS_ETYPE_KEYID_APPLETALK:
      *val1 = D_aclMacRuleEtypeKey_appletalk;
      break;

    case L7_QOS_ETYPE_KEYID_ARP:
      *val1 = D_aclMacRuleEtypeKey_arp;
      break;

    case L7_QOS_ETYPE_KEYID_IBMSNA:
      *val1 = D_aclMacRuleEtypeKey_ibmsna;
      break;

    case L7_QOS_ETYPE_KEYID_IPV4:
      *val1 = D_aclMacRuleEtypeKey_ipv4;
      break;

    case L7_QOS_ETYPE_KEYID_IPV6:
      *val1 = D_aclMacRuleEtypeKey_ipv6;
      break;

    case L7_QOS_ETYPE_KEYID_IPX:
      *val1 = D_aclMacRuleEtypeKey_ipx;
      break;

    case L7_QOS_ETYPE_KEYID_MPLSMCAST:
      *val1 = D_aclMacRuleEtypeKey_mplsmcast;
      break;

    case L7_QOS_ETYPE_KEYID_MPLSUCAST:
      *val1 = D_aclMacRuleEtypeKey_mplsucast;
      break;

    case L7_QOS_ETYPE_KEYID_NETBIOS:
      *val1 = D_aclMacRuleEtypeKey_netbios;
      break;

    case L7_QOS_ETYPE_KEYID_NOVELL:
      *val1 = D_aclMacRuleEtypeKey_novell;
      break;

    case L7_QOS_ETYPE_KEYID_PPPOE:
      *val1 = D_aclMacRuleEtypeKey_pppoe;
      break;

    case L7_QOS_ETYPE_KEYID_RARP:
      *val1 = D_aclMacRuleEtypeKey_rarp;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}


L7_RC_t
snmpQosAclMacRuleEtypeValueGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 aclMacRuleIndex, L7_uint32 *val1)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 eTypeKey, eTypeValue;

  rc = usmDbQosAclMacRuleEtypeKeyGet(UnitIndex, aclIndex, aclMacRuleIndex, &eTypeKey, &eTypeValue);
  
  if (rc == L7_SUCCESS) 
  {
    if (eTypeKey == L7_QOS_ETYPE_KEYID_CUSTOM)
    {
      *val1 = eTypeValue;
    }
    else
    {
      /* etype value only defined for 'custom' etype key */
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpQosAclMacRuleEtypeKeyAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 aclMacRuleIndex, L7_uint32 val1)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 eTypeKey = L7_QOS_ETYPE_KEYID_NONE;

  switch (val1)
  {
    case D_aclMacRuleEtypeKey_custom:
      eTypeKey = L7_QOS_ETYPE_KEYID_CUSTOM;
      break;

    case D_aclMacRuleEtypeKey_appletalk:
      eTypeKey = L7_QOS_ETYPE_KEYID_APPLETALK;
      break;

    case D_aclMacRuleEtypeKey_arp:
      eTypeKey = L7_QOS_ETYPE_KEYID_ARP;
      break;

    case D_aclMacRuleEtypeKey_ibmsna:
      eTypeKey = L7_QOS_ETYPE_KEYID_IBMSNA;
      break;

    case D_aclMacRuleEtypeKey_ipv4:
      eTypeKey = L7_QOS_ETYPE_KEYID_IPV4;
      break;

    case D_aclMacRuleEtypeKey_ipv6:
      eTypeKey = L7_QOS_ETYPE_KEYID_IPV6;
      break;

    case D_aclMacRuleEtypeKey_ipx:
      eTypeKey = L7_QOS_ETYPE_KEYID_IPX;
      break;

    case D_aclMacRuleEtypeKey_mplsmcast:
      eTypeKey = L7_QOS_ETYPE_KEYID_MPLSMCAST;
      break;

    case D_aclMacRuleEtypeKey_mplsucast:
      eTypeKey = L7_QOS_ETYPE_KEYID_MPLSUCAST;
      break;

    case D_aclMacRuleEtypeKey_netbios:
      eTypeKey = L7_QOS_ETYPE_KEYID_NETBIOS;
      break;

    case D_aclMacRuleEtypeKey_novell:
      eTypeKey = L7_QOS_ETYPE_KEYID_NOVELL;
      break;

    case D_aclMacRuleEtypeKey_pppoe:
      eTypeKey = L7_QOS_ETYPE_KEYID_PPPOE;
      break;

    case D_aclMacRuleEtypeKey_rarp:
      eTypeKey = L7_QOS_ETYPE_KEYID_RARP;
      break;

    default:
      return L7_FAILURE;
      break;
  }
  
  rc = usmDbQosAclMacRuleEtypeKeyAdd(UnitIndex, aclIndex, aclMacRuleIndex, eTypeKey, L7_QOS_ETYPE_VALUE_UNUSED);
  return rc;
}

L7_RC_t
snmpQosAclMacRuleEtypeValueAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 aclMacRuleIndex, L7_uint32 val1)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 eTypeKey, eTypeValue;


  if ((usmDbQosAclMacIsFieldConfigured(UnitIndex, aclIndex, aclMacRuleIndex,
                                       ACL_MAC_ETYPE_KEYID) == L7_TRUE) &&
      (usmDbQosAclMacRuleEtypeKeyGet(UnitIndex, aclIndex, aclMacRuleIndex, 
                                     &eTypeKey, &eTypeValue) == L7_SUCCESS) &&
      (eTypeKey == L7_QOS_ETYPE_KEYID_CUSTOM))
  {
    rc = usmDbQosAclMacRuleEtypeKeyAdd(UnitIndex, aclIndex, aclMacRuleIndex, 
                                       L7_QOS_ETYPE_KEYID_CUSTOM, val1);
  }

  return rc;
}

L7_RC_t
snmpQosAclMacRuleDstMacAddrMaskGet( L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                    L7_uint32 aclMacRuleIndex,
                                    L7_uchar8 *dstmac, L7_uchar8 *dstmask)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i = 0;
  L7_uchar8 strDstMac [L7_MAC_ADDR_LEN];
  L7_uchar8 strDstMacMask [L7_MAC_ADDR_LEN];

  bzero( strDstMac, L7_MAC_ADDR_LEN);
  bzero( strDstMacMask, L7_MAC_ADDR_LEN);

  rc = usmDbQosAclMacRuleDstMacAddrMaskGet( UnitIndex, 
                                            aclIndex, 
                                            aclMacRuleIndex,
                                            strDstMac,
                                            strDstMacMask);

  if (usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_RULE_MATCH_DSTMAC_MASK_FEATURE_ID) != L7_TRUE)
  {
      memset(strDstMacMask,0xFF,L7_MAC_ADDR_LEN);
  }

  for (i=0;i<L7_MAC_ADDR_LEN;i++)
  {
      strDstMacMask[i] = strDstMacMask[i] ^ (0xFF);
  }

  memcpy(dstmac,strDstMac,L7_MAC_ADDR_LEN);
  memcpy(dstmask,strDstMacMask,L7_MAC_ADDR_LEN);

  return rc;

}



L7_RC_t
snmpQosAclMacRuleDstMacAdd( L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                    L7_uint32 aclMacRuleIndex,
                                    L7_uchar8 *dstmac, L7_uchar8 *dstmask)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i = 0;

  for (i=0;i<L7_MAC_ADDR_LEN;i++)
  {
    dstmask[i] = dstmask[i] ^ (0xFF);
  }
  
 
 rc = usmDbQosAclMacRuleDstMacAdd( UnitIndex, 
                                    aclIndex, 
                                    aclMacRuleIndex,
                                    dstmac,
                                    dstmask);

  return rc;

}

L7_RC_t
snmpQosAclMacRuleSrcMacAddrMaskGet( L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                    L7_uint32 aclMacRuleIndex,
                                    L7_uchar8 *srcmac, L7_uchar8 *srcmask)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i = 0;
  L7_uchar8 strSrcMac [L7_MAC_ADDR_LEN];
  L7_uchar8 strSrcMacMask [L7_MAC_ADDR_LEN];

  bzero( strSrcMac, L7_MAC_ADDR_LEN);
  bzero( strSrcMacMask, L7_MAC_ADDR_LEN);

  rc = usmDbQosAclMacRuleSrcMacAddrMaskGet( UnitIndex, 
                                            aclIndex, 
                                            aclMacRuleIndex,
                                            strSrcMac,
                                            strSrcMacMask);

  if (usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_RULE_MATCH_SRCMAC_MASK_FEATURE_ID) != L7_TRUE)
  {
      memset(strSrcMacMask,0xFF,L7_MAC_ADDR_LEN);
  }

  for (i=0;i<L7_MAC_ADDR_LEN;i++)
  {
      strSrcMacMask[i] = strSrcMacMask[i] ^ (0xFF);
  }

  memcpy(srcmac,strSrcMac,L7_MAC_ADDR_LEN);
  memcpy(srcmask,strSrcMacMask,L7_MAC_ADDR_LEN);

  return rc;

}



L7_RC_t
snmpQosAclMacRuleSrcMacAdd( L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                    L7_uint32 aclMacRuleIndex,
                                    L7_uchar8 *srcmac, L7_uchar8 *srcmask)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i = 0;

  for (i=0;i<L7_MAC_ADDR_LEN;i++)
  {
    srcmask[i] = srcmask[i] ^ (0xFF);
  }
  
  rc = usmDbQosAclMacRuleSrcMacAdd( UnitIndex, 
                                    aclIndex, 
                                    aclMacRuleIndex,
                                    srcmac,
                                    srcmask);
  return rc;

}

L7_RC_t
snmpQosAclMacRuleVlanIdRangeStartAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 aclMacRuleIndex, 
                                     L7_uint32 startVlan)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 endVlan, tempVlan;

  if (usmDbQosAclMacRuleVlanIdRangeGet(UnitIndex, aclIndex, aclMacRuleIndex,
                                       &tempVlan, &endVlan) == L7_SUCCESS)
  {
    /* if new start value is greater than existing end value, set end
     * to new start
     */
    if (startVlan > endVlan)
      endVlan = startVlan;
  }
  else
  {
    endVlan = startVlan;
  }

  rc = usmDbQosAclMacRuleVlanIdRangeAdd(UnitIndex, aclIndex, aclMacRuleIndex, startVlan, endVlan);
  return rc;
}

L7_RC_t
snmpQosAclMacRuleVlanIdRangeEndAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 aclMacRuleIndex, 
                                   L7_uint32 endVlan)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 startVlan, tempVlan;

  if (usmDbQosAclMacRuleVlanIdRangeGet(UnitIndex, aclIndex, aclMacRuleIndex,
                                       &startVlan, &tempVlan) == L7_SUCCESS)
  {
    /* if new end value is less than existing start value, set start
     * to new end
     */
    if (endVlan < startVlan)
      startVlan = endVlan;
  }
  else
  {
    startVlan = endVlan;
  }

  rc = usmDbQosAclMacRuleVlanIdRangeAdd(UnitIndex, aclIndex, aclMacRuleIndex, startVlan, endVlan);
  return rc;
}

L7_RC_t
snmpQosAclMacRuleVlanId2RangeStartAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 aclMacRuleIndex, 
                                      L7_uint32 startVlan)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 endVlan, tempVlan;

  if (usmDbQosAclMacRuleVlanId2RangeGet(UnitIndex, aclIndex, aclMacRuleIndex,
                                        &tempVlan, &endVlan) == L7_SUCCESS)
  {
    /* if new start value is greater than existing end value, set end
     * to new start
     */
    if (startVlan > endVlan)
      endVlan = startVlan;
  }
  else
  {
    endVlan = startVlan;
  }

  rc = usmDbQosAclMacRuleVlanId2RangeAdd(UnitIndex, aclIndex, aclMacRuleIndex, startVlan, endVlan);
  return rc;
}

L7_RC_t
snmpQosAclMacRuleVlanId2RangeEndAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 aclMacRuleIndex, 
                                    L7_uint32 endVlan)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 startVlan, tempVlan;

  if (usmDbQosAclMacRuleVlanId2RangeGet(UnitIndex, aclIndex, aclMacRuleIndex,
                                        &startVlan, &tempVlan) == L7_SUCCESS)
  {
    /* if new end value is less than existing start value, set start
     * to new end
     */
    if (endVlan < startVlan)
      startVlan = endVlan;
  }
  else
  {
    startVlan = endVlan;
  }

  rc = usmDbQosAclMacRuleVlanId2RangeAdd(UnitIndex, aclIndex, aclMacRuleIndex, startVlan, endVlan);
  return rc;
}

L7_RC_t
snmpQosAclMacRuleActionAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 aclMacRuleIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_action = 0;

  switch (val)
  {
  case D_aclMacRuleAction_permit:
    temp_action = L7_ACL_PERMIT;
    break;

  case D_aclMacRuleAction_deny:
    temp_action = L7_ACL_DENY;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbQosAclMacRuleActionAdd(UnitIndex, aclIndex, aclMacRuleIndex, temp_action);
  }

  return rc;
}

L7_RC_t
snmpQosAclMacRuleStatusSet(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 aclMacRuleIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_aclMacStatus_active:
    break;

  case D_aclMacStatus_destroy:
    rc = usmDbQosAclMacRuleRemove(UnitIndex, aclIndex, aclMacRuleIndex); 
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

L7_RC_t
snmpQosAclMacRuleRedirectIntfGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                 L7_uint32 aclMacRuleIndex, L7_uint32 *ifIndex)
{
  L7_RC_t   rc;
  L7_uint32 intIfNum;
             
  rc = usmDbQosAclMacRuleRedirectIntfGet(UnitIndex, aclIndex, aclMacRuleIndex,
                                         &intIfNum);
   
  /* If no redirect Interface is found, show the defaut value i.e., 0 */
  if (rc != L7_SUCCESS)
  {
    *ifIndex = 0;
    rc = L7_SUCCESS; 
  }
  else
  {
    /* translate intIfNum to external ifIndex */
    rc = usmDbExtIfNumFromIntIfNum(intIfNum, ifIndex);
  }
  
  return rc;
}

L7_RC_t
snmpQosAclMacRuleRedirectIntfAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                 L7_uint32 aclMacRuleIndex, L7_uint32 ifIndex)
{
  L7_RC_t   rc;
  L7_uint32 intIfNum;
             
  /* translate external ifIndex to intIfNum */
  rc = usmDbIntIfNumFromExtIfNum(ifIndex, &intIfNum);

  if (rc == L7_SUCCESS)
  {
    rc = usmDbQosAclMacRuleRedirectIntfAdd(UnitIndex, aclIndex, aclMacRuleIndex,
                                           intIfNum);
  }
  
  return rc;
}

L7_RC_t          
snmpQosAclMacRuleEveryGet ( L7_uint32 UnitIndex, L7_uint32 aclMacIndex, 
                         L7_uint32 aclMacRuleIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbQosAclMacRuleEveryGet (UnitIndex, aclMacIndex, aclMacRuleIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_aclRuleMatchEvery_true;
      break;

    case L7_FALSE:
      *val = D_aclRuleMatchEvery_false;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}


L7_RC_t          
snmpQosAclMacRuleEveryAdd ( L7_uint32 UnitIndex, L7_uint32 aclMacIndex, 
                         L7_uint32 aclMacRuleIndex, L7_int32 val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  switch (val)
  {
    case D_aclRuleMatchEvery_true:
      temp_val = L7_TRUE;
      break;

    case D_aclRuleMatchEvery_false:
      temp_val = L7_FALSE;
      break;

    default:
      temp_val = 0;
      rc = L7_FAILURE;
      break;
  }
  
  
  rc = usmDbQosAclMacRuleEveryAdd (UnitIndex, aclMacIndex, aclMacRuleIndex, temp_val);


  return rc;
}


L7_RC_t
snmpQosAclMacRuleMirrorIntfGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                               L7_uint32 aclMacRuleIndex, L7_uint32 *ifIndex)
{
  L7_RC_t   rc;
  L7_uint32 intIfNum;
             
  rc = usmDbQosAclMacRuleMirrorIntfGet(UnitIndex, aclIndex, aclMacRuleIndex,
                                       &intIfNum);
   
  /* If no mirror Interface is found, show the defaut value i.e., 0 */
  if (rc != L7_SUCCESS)
  {
    *ifIndex = 0;
    rc = L7_SUCCESS; 
  }
  else
  {
    /* translate intIfNum to external ifIndex */
    rc = usmDbExtIfNumFromIntIfNum(intIfNum, ifIndex);
  }
  
  return rc;
}

L7_RC_t
snmpQosAclMacRuleMirrorIntfAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                               L7_uint32 aclMacRuleIndex, L7_uint32 ifIndex)
{
  L7_RC_t   rc;
  L7_uint32 intIfNum;
             
  /* translate external ifIndex to intIfNum */
  rc = usmDbIntIfNumFromExtIfNum(ifIndex, &intIfNum);

  if (rc == L7_SUCCESS)
  {
    rc = usmDbQosAclMacRuleMirrorIntfAdd(UnitIndex, aclIndex, aclMacRuleIndex,
                                         intIfNum);
  }
  
  return rc;
}



L7_RC_t          
snmpQosAclMacRuleLoggingGet ( L7_uint32 UnitIndex, L7_uint32 aclMacIndex, 
                              L7_uint32 aclMacRuleIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbQosAclMacRuleLoggingGet (UnitIndex, aclMacIndex, aclMacRuleIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_aclRuleLogging_true;
      break;

    case L7_FALSE:
      *val = D_aclRuleLogging_false;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}


L7_RC_t          
snmpQosAclMacRuleLoggingAdd ( L7_uint32 UnitIndex, L7_uint32 aclMacIndex, 
                              L7_uint32 aclMacRuleIndex, L7_int32 val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  switch (val)
  {
    case D_aclRuleLogging_true:
      temp_val = L7_TRUE;
      break;

    case D_aclRuleLogging_false:
      temp_val = L7_FALSE;
      break;

    default:
      temp_val = 0;
      rc = L7_FAILURE;
      break;
  }
  
  
  rc = usmDbQosAclMacRuleLoggingAdd (UnitIndex, aclMacIndex, aclMacRuleIndex, temp_val);


  return rc;
}

/**************************************************************************************************************/

L7_RC_t
snmpQosAclMacRuleEntryGet(L7_uint32 UnitIndex, aclMacRuleEntry_t *aclMacRuleEntryData, L7_int32 nominator)
{
  L7_RC_t   rc = L7_FAILURE;
  L7_uint32 temp_uint;
  L7_uchar8 snmp_buffer1[SNMP_BUFFER_LEN];
  L7_uchar8 snmp_buffer2[SNMP_BUFFER_LEN];

  if (usmDbQosAclMacRuleCheckValid(UnitIndex, aclMacRuleEntryData->aclMacIndex, aclMacRuleEntryData->aclMacRuleIndex) == L7_SUCCESS)
  {
    CLR_VALID(nominator, aclMacRuleEntryData->valid);

    /*
     * if ( nominator != -1 ) condition is added to all the case statements
     * for storing all the values to support the undo functionality.
     */
    switch (nominator)
    {
      case -1:
      case I_aclMacRuleEntryIndex_aclMacIndex:
      case I_aclMacRuleIndex:
        rc = L7_SUCCESS;
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclMacRuleAction:
        if ((usmDbQosAclMacIsFieldConfigured(UnitIndex, 
                                            aclMacRuleEntryData->aclMacIndex,
                                            aclMacRuleEntryData->aclMacRuleIndex,
                                            ACL_MAC_ACTION) == L7_TRUE))
        {
          rc = snmpQosAclMacRuleActionGet(UnitIndex,
                                          aclMacRuleEntryData->aclMacIndex,
                                          aclMacRuleEntryData->aclMacRuleIndex,
                                          &aclMacRuleEntryData->aclMacRuleAction);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclMacRuleAction, aclMacRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclMacRuleCos:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_COS_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclMacIsFieldConfigured(UnitIndex, 
                                             aclMacRuleEntryData->aclMacIndex,
                                             aclMacRuleEntryData->aclMacRuleIndex,
                                             ACL_MAC_COS) == L7_TRUE))
        {
          rc = usmDbQosAclMacRuleCosGet(UnitIndex,
                                        aclMacRuleEntryData->aclMacIndex,
                                        aclMacRuleEntryData->aclMacRuleIndex,
                                        &aclMacRuleEntryData->aclMacRuleCos);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclMacRuleCos,aclMacRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclMacRuleCos2:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_COS2_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclMacIsFieldConfigured(UnitIndex, 
                                             aclMacRuleEntryData->aclMacIndex,
                                             aclMacRuleEntryData->aclMacRuleIndex,
                                             ACL_MAC_COS2) == L7_TRUE))
        {
          rc = usmDbQosAclMacRuleCos2Get(UnitIndex,
                                         aclMacRuleEntryData->aclMacIndex,
                                         aclMacRuleEntryData->aclMacRuleIndex,
                                         &aclMacRuleEntryData->aclMacRuleCos2);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclMacRuleCos2,aclMacRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclMacRuleDestMacAddr:
        memset(snmp_buffer1, 0, (size_t)SNMP_BUFFER_LEN);
        memset(snmp_buffer2, 0, (size_t)SNMP_BUFFER_LEN);
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_DSTMAC_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclMacIsFieldConfigured(UnitIndex, 
                                             aclMacRuleEntryData->aclMacIndex,
                                             aclMacRuleEntryData->aclMacRuleIndex,
                                             ACL_MAC_DSTMAC) == L7_TRUE))
        {
          if ((snmpQosAclMacRuleDstMacAddrMaskGet(UnitIndex,
                                                  aclMacRuleEntryData->aclMacIndex,
                                                  aclMacRuleEntryData->aclMacRuleIndex,
                                                  snmp_buffer1,
                                                  snmp_buffer2) == L7_SUCCESS) &&
              (SafeMakeOctetString(&aclMacRuleEntryData->aclMacRuleDestMacAddr, 
                                   snmp_buffer1, L7_MAC_ADDR_LEN) == L7_TRUE))
          {
            rc = L7_SUCCESS;
            SET_VALID(I_aclMacRuleDestMacAddr, aclMacRuleEntryData->valid);
          }
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclMacRuleDestMacMask:
        memset(snmp_buffer1, 0, (size_t)SNMP_BUFFER_LEN);
        memset(snmp_buffer2, 0, (size_t)SNMP_BUFFER_LEN);
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_DSTMAC_MASK_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclMacIsFieldConfigured(UnitIndex, 
                                             aclMacRuleEntryData->aclMacIndex,
                                             aclMacRuleEntryData->aclMacRuleIndex,
                                             ACL_MAC_DSTMAC_MASK) == L7_TRUE))
        {
          if ((snmpQosAclMacRuleDstMacAddrMaskGet(UnitIndex,
                                                  aclMacRuleEntryData->aclMacIndex,
                                                  aclMacRuleEntryData->aclMacRuleIndex,
                                                  snmp_buffer1,
                                                  snmp_buffer2) == L7_SUCCESS) &&
              (SafeMakeOctetString(&aclMacRuleEntryData->aclMacRuleDestMacMask, 
                                   snmp_buffer2, L7_MAC_ADDR_LEN) == L7_TRUE))
          {
            rc = L7_SUCCESS;
            SET_VALID(I_aclMacRuleDestMacMask, aclMacRuleEntryData->valid);
          }
        }
       if (nominator != -1 ) break;
       /* else pass through */

      case I_aclMacRuleEtypeKey:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_ETYPE_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclMacIsFieldConfigured(UnitIndex, 
                                             aclMacRuleEntryData->aclMacIndex,
                                             aclMacRuleEntryData->aclMacRuleIndex,
                                             ACL_MAC_ETYPE_KEYID) == L7_TRUE))
        {
          rc = snmpQosAclMacRuleEtypeKeyGet(UnitIndex,
                                            aclMacRuleEntryData->aclMacIndex,
                                            aclMacRuleEntryData->aclMacRuleIndex,
                                            &aclMacRuleEntryData->aclMacRuleEtypeKey);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclMacRuleEtypeKey,aclMacRuleEntryData->valid);
        }
         if (nominator != -1 ) break;
        /* else pass through */

      case I_aclMacRuleEtypeValue:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_ETYPE_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclMacIsFieldConfigured(UnitIndex, 
                                             aclMacRuleEntryData->aclMacIndex,
                                             aclMacRuleEntryData->aclMacRuleIndex,
                                             ACL_MAC_ETYPE_VALUE) == L7_TRUE))
        {
          rc = snmpQosAclMacRuleEtypeValueGet(UnitIndex,
                                              aclMacRuleEntryData->aclMacIndex,
                                              aclMacRuleEntryData->aclMacRuleIndex,
                                              &aclMacRuleEntryData->aclMacRuleEtypeValue);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclMacRuleEtypeValue,aclMacRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclMacRuleSrcMacAddr:
        memset(snmp_buffer1, 0, (size_t)SNMP_BUFFER_LEN);
        memset(snmp_buffer2, 0, (size_t)SNMP_BUFFER_LEN);
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_SRCMAC_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclMacIsFieldConfigured(UnitIndex, 
                                             aclMacRuleEntryData->aclMacIndex,
                                             aclMacRuleEntryData->aclMacRuleIndex,
                                             ACL_MAC_SRCMAC) == L7_TRUE))
        {
          if ((snmpQosAclMacRuleSrcMacAddrMaskGet(UnitIndex,
                                                  aclMacRuleEntryData->aclMacIndex,
                                                  aclMacRuleEntryData->aclMacRuleIndex,
                                                  snmp_buffer1,
                                                  snmp_buffer2) == L7_SUCCESS) &&
              (SafeMakeOctetString(&aclMacRuleEntryData->aclMacRuleSrcMacAddr,
                                   snmp_buffer1, L7_MAC_ADDR_LEN) == L7_TRUE))
          {
            rc = L7_SUCCESS;
            SET_VALID(I_aclMacRuleSrcMacAddr, aclMacRuleEntryData->valid);
          }
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclMacRuleSrcMacMask:
        memset(snmp_buffer1, 0, (size_t)SNMP_BUFFER_LEN);
        memset(snmp_buffer2, 0, (size_t)SNMP_BUFFER_LEN);
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID, 
                                      L7_ACL_RULE_MATCH_SRCMAC_MASK_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclMacIsFieldConfigured(UnitIndex, 
                                             aclMacRuleEntryData->aclMacIndex,
                                             aclMacRuleEntryData->aclMacRuleIndex,
                                             ACL_MAC_SRCMAC_MASK) == L7_TRUE))
        {
          if ((snmpQosAclMacRuleSrcMacAddrMaskGet(UnitIndex,
                                                  aclMacRuleEntryData->aclMacIndex,
                                                  aclMacRuleEntryData->aclMacRuleIndex,
                                                  snmp_buffer1,
                                                  snmp_buffer2) == L7_SUCCESS) &&
              (SafeMakeOctetString(&aclMacRuleEntryData->aclMacRuleSrcMacMask, 
                                   snmp_buffer2, L7_MAC_ADDR_LEN) == L7_TRUE))
          {
            rc = L7_SUCCESS;
            SET_VALID(I_aclMacRuleSrcMacMask, aclMacRuleEntryData->valid);
          }
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclMacRuleVlanId:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_VLANID_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclMacIsFieldConfigured(UnitIndex, 
                                             aclMacRuleEntryData->aclMacIndex,
                                             aclMacRuleEntryData->aclMacRuleIndex,
                                             ACL_MAC_VLANID) == L7_TRUE))
        {
          rc = usmDbQosAclMacRuleVlanIdGet(UnitIndex,
                                           aclMacRuleEntryData->aclMacIndex,
                                           aclMacRuleEntryData->aclMacRuleIndex,
                                           &aclMacRuleEntryData->aclMacRuleVlanId);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclMacRuleVlanId, aclMacRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclMacRuleVlanIdRangeStart:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_VLANID_RANGE_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclMacIsFieldConfigured(UnitIndex, 
                                             aclMacRuleEntryData->aclMacIndex,
                                             aclMacRuleEntryData->aclMacRuleIndex,
                                             ACL_MAC_VLANID_START) == L7_TRUE))
        {
          rc = usmDbQosAclMacRuleVlanIdRangeGet(UnitIndex,
                                                aclMacRuleEntryData->aclMacIndex,
                                                aclMacRuleEntryData->aclMacRuleIndex,
                                                &aclMacRuleEntryData->aclMacRuleVlanIdRangeStart,
                                                &temp_uint);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclMacRuleVlanIdRangeStart, aclMacRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclMacRuleVlanIdRangeEnd:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_VLANID_RANGE_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclMacIsFieldConfigured(UnitIndex, 
                                             aclMacRuleEntryData->aclMacIndex,
                                             aclMacRuleEntryData->aclMacRuleIndex,
                                             ACL_MAC_VLANID_END) == L7_TRUE))
        {
          rc = usmDbQosAclMacRuleVlanIdRangeGet(UnitIndex,
                                                aclMacRuleEntryData->aclMacIndex,
                                                aclMacRuleEntryData->aclMacRuleIndex,
                                                &temp_uint,
                                                &aclMacRuleEntryData->aclMacRuleVlanIdRangeEnd);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclMacRuleVlanIdRangeEnd, aclMacRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclMacRuleVlanId2:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID, 
                                      L7_ACL_RULE_MATCH_VLANID2_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclMacIsFieldConfigured(UnitIndex, 
                                             aclMacRuleEntryData->aclMacIndex,
                                             aclMacRuleEntryData->aclMacRuleIndex,
                                             ACL_MAC_VLANID2) == L7_TRUE))
        {
          rc = usmDbQosAclMacRuleVlanId2Get(UnitIndex,
                                            aclMacRuleEntryData->aclMacIndex,
                                            aclMacRuleEntryData->aclMacRuleIndex,
                                            &aclMacRuleEntryData->aclMacRuleVlanId2);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclMacRuleVlanId2, aclMacRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclMacRuleVlanId2RangeStart:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_VLANID2_RANGE_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclMacIsFieldConfigured(UnitIndex, 
                                             aclMacRuleEntryData->aclMacIndex,
                                             aclMacRuleEntryData->aclMacRuleIndex,
                                             ACL_MAC_VLANID2_START) == L7_TRUE))
        {
          rc = usmDbQosAclMacRuleVlanId2RangeGet(UnitIndex,
                                                 aclMacRuleEntryData->aclMacIndex,
                                                 aclMacRuleEntryData->aclMacRuleIndex,
                                                 &aclMacRuleEntryData->aclMacRuleVlanId2RangeStart,
                                                 &temp_uint);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclMacRuleVlanId2RangeStart, aclMacRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclMacRuleVlanId2RangeEnd:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID, 
                                      L7_ACL_RULE_MATCH_VLANID2_RANGE_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclMacIsFieldConfigured(UnitIndex, 
                                             aclMacRuleEntryData->aclMacIndex,
                                             aclMacRuleEntryData->aclMacRuleIndex,
                                             ACL_MAC_VLANID2_END) == L7_TRUE))
        {
          rc = usmDbQosAclMacRuleVlanId2RangeGet(UnitIndex,
                                                 aclMacRuleEntryData->aclMacIndex,
                                                 aclMacRuleEntryData->aclMacRuleIndex,
                                                 &temp_uint,
                                                 &aclMacRuleEntryData->aclMacRuleVlanId2RangeEnd);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclMacRuleVlanId2RangeEnd, aclMacRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclMacRuleStatus:
        /* all entries are active */
        rc = L7_SUCCESS;
        aclMacRuleEntryData->aclMacRuleStatus = D_aclMacRuleStatus_active;
        SET_VALID(I_aclMacRuleStatus, aclMacRuleEntryData->valid);
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclMacRuleAssignQueueId:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID, 
                                      L7_ACL_ASSIGN_QUEUE_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclMacIsFieldConfigured(UnitIndex, 
                                             aclMacRuleEntryData->aclMacIndex,
                                             aclMacRuleEntryData->aclMacRuleIndex,
                                             ACL_MAC_ASSIGN_QUEUEID) == L7_TRUE))
        {
          rc = usmDbQosAclMacRuleAssignQueueIdGet(UnitIndex,
                                                  aclMacRuleEntryData->aclMacIndex,
                                                  aclMacRuleEntryData->aclMacRuleIndex,
                                                  &aclMacRuleEntryData->aclMacRuleAssignQueueId);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclMacRuleAssignQueueId, aclMacRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclMacRuleRedirectIntf:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID, 
                                      L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclMacIsFieldConfigured(UnitIndex, 
                                             aclMacRuleEntryData->aclMacIndex,
                                             aclMacRuleEntryData->aclMacRuleIndex,
                                             ACL_MAC_REDIRECT_INTF) == L7_TRUE))
        {
          rc = snmpQosAclMacRuleRedirectIntfGet(UnitIndex,
                                                aclMacRuleEntryData->aclMacIndex,
                                                aclMacRuleEntryData->aclMacRuleIndex,
                                                &aclMacRuleEntryData->aclMacRuleRedirectIntf);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclMacRuleRedirectIntf, aclMacRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclMacRuleMatchEvery:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_EVERY_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclMacIsFieldConfigured(UnitIndex, 
                                             aclMacRuleEntryData->aclMacIndex,
                                             aclMacRuleEntryData->aclMacRuleIndex,
                                             ACL_MAC_EVERY) == L7_TRUE))
        {
          rc = snmpQosAclMacRuleEveryGet(UnitIndex,
                                         aclMacRuleEntryData->aclMacIndex,
                                         aclMacRuleEntryData->aclMacRuleIndex,
                                         &aclMacRuleEntryData->aclMacRuleMatchEvery);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclMacRuleMatchEvery, aclMacRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclMacRuleMirrorIntf:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID, 
                                      L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclMacIsFieldConfigured(UnitIndex, 
                                             aclMacRuleEntryData->aclMacIndex,
                                             aclMacRuleEntryData->aclMacRuleIndex,
                                             ACL_MAC_MIRROR_INTF) == L7_TRUE))
        {
          rc = snmpQosAclMacRuleMirrorIntfGet(UnitIndex,
                                              aclMacRuleEntryData->aclMacIndex,
                                              aclMacRuleEntryData->aclMacRuleIndex,
                                              &aclMacRuleEntryData->aclMacRuleMirrorIntf);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclMacRuleMirrorIntf, aclMacRuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclMacRuleLogging:
        if ((usmDbQosAclMacRuleLoggingAllowed(USMDB_UNIT_CURRENT, 
                                              aclMacRuleEntryData->aclMacIndex, 
                                              aclMacRuleEntryData->aclMacRuleIndex) == L7_SUCCESS) &&
            (usmDbQosAclMacIsFieldConfigured(UnitIndex, 
                                             aclMacRuleEntryData->aclMacIndex,
                                             aclMacRuleEntryData->aclMacRuleIndex,
                                             ACL_MAC_LOGGING) == L7_TRUE))
        {
          rc = snmpQosAclMacRuleLoggingGet(UnitIndex,
                                           aclMacRuleEntryData->aclMacIndex,
                                           aclMacRuleEntryData->aclMacRuleIndex,
                                           &aclMacRuleEntryData->aclMacRuleLogging);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclMacRuleLogging, aclMacRuleEntryData->valid);
        }
        break;
        /* last valid nominator */

      default:
        rc = L7_FAILURE;
        break;
    }

    if (nominator >= 0 && rc == L7_SUCCESS)
      SET_VALID(nominator, aclMacRuleEntryData->valid);
    else if (nominator == -1)
      rc = L7_SUCCESS;
  }

  return rc;
}

L7_RC_t
snmpQosAclMacRuleEntryGetNext(L7_uint32 UnitIndex, aclMacRuleEntry_t *aclMacRuleEntryData, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;

  while (1)
  {
    if (usmDbQosAclMacRuleGetNext(UnitIndex, 
                                  aclMacRuleEntryData->aclMacIndex, 
                                  aclMacRuleEntryData->aclMacRuleIndex, 
                                  &aclMacRuleEntryData->aclMacRuleIndex) == L7_SUCCESS)
    {
      if (snmpQosAclMacRuleEntryGet(UnitIndex, aclMacRuleEntryData, nominator) == L7_SUCCESS)
      {
        rc = L7_SUCCESS;
        break;
      }
    }
    else
    {
      /* no more rules in current access list, so move on to next list */
      if (usmDbQosAclMacIndexGetNext(UnitIndex, aclMacRuleEntryData->aclMacIndex, 
                                     &aclMacRuleEntryData->aclMacIndex) == L7_SUCCESS)
        aclMacRuleEntryData->aclMacRuleIndex = 0;
      else
        break;
    }
  }

  return rc;
}


L7_RC_t
snmpQosAclTrapFlagGet(L7_uint32 UnitIndex, L7_int32 *val)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmDbQosAclTrapFlagGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_aclTrapFlag_enable;
      break;

    case L7_DISABLE:
      *val = D_aclTrapFlag_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpQosAclTrapFlagSet(L7_uint32 UnitIndex, L7_int32 val)
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_aclTrapFlag_enable:
    temp_val = L7_ENABLE;
    break;

  case D_aclTrapFlag_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbQosAclTrapFlagSet(UnitIndex, temp_val);
  }

  return rc;
}

L7_RC_t snmpQosAclRuleSrcIpv6AddrGet(L7_uint32 UnitIndex,
                                     L7_uint32 aclIpv6Index,
                                     L7_uint32 aclIpv6RuleIndex,
                                     L7_char8  *snmp_buffer, 
                                     L7_uint32 *prefix_len, 
                                     L7_uint32 *prefixLen)
{
   L7_in6_prefix_t prefix;

   if(usmDbQosAclRuleSrcIpv6AddrGet(UnitIndex, aclIpv6Index, aclIpv6RuleIndex, &prefix) == L7_SUCCESS)
   {
      memcpy(snmp_buffer, prefix.in6Addr.in6.addr8, sizeof(prefix.in6Addr.in6.addr8)); 
      *prefix_len = sizeof(prefix.in6Addr.in6.addr8);
      *prefixLen = prefix.in6PrefixLen;
      return L7_SUCCESS;
   }

   return L7_FAILURE;
}   

L7_RC_t snmpQosAclRuleDstIpv6AddrGet(L7_uint32 UnitIndex,
                                     L7_uint32 aclIpv6Index,
                                     L7_uint32 aclIpv6RuleIndex,
                                     L7_char8  *snmp_buffer, 
                                     L7_uint32 *prefix_len, 
                                     L7_uint32 *prefixLen)
{
   L7_in6_prefix_t prefix;

   if(usmDbQosAclRuleDstIpv6AddrGet(UnitIndex, aclIpv6Index, aclIpv6RuleIndex, &prefix) == L7_SUCCESS)
   {
      memcpy(snmp_buffer, prefix.in6Addr.in6.addr8, sizeof(prefix.in6Addr.in6.addr8)); 
      *prefix_len = sizeof(prefix.in6Addr.in6.addr8);
      *prefixLen = prefix.in6PrefixLen;
      return L7_SUCCESS;
   }

   return L7_FAILURE;
}   

L7_RC_t          
snmpQosAclIpv6RuleLoggingAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                             L7_uint32 aclRuleIndex, L7_int32 val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  switch (val)
  {
    case D_aclIpv6RuleLogging_true:
      temp_val = L7_TRUE;
      break;

    case D_aclIpv6RuleLogging_false:
      temp_val = L7_FALSE;
      break;

    default:
      temp_val = 0;
      rc = L7_FAILURE;
      break;
  }

  rc = usmDbQosAclRuleLoggingAdd (UnitIndex, aclIndex, aclRuleIndex, temp_val);
  return rc;

}


L7_RC_t
snmpQosAclIpv6RuleRedirectIntfGet(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                  L7_uint32 aclRuleIndex, L7_uint32 *ifIndex)
{
  L7_RC_t   rc;
  L7_uint32 intIfNum;
             
  rc = usmDbQosAclRuleRedirectIntfGet(UnitIndex, aclIndex, aclRuleIndex,
                                      &intIfNum);
   
  /* If no redirect Interface is found, show the defaut value i.e., 0 */
  if (rc != L7_SUCCESS)
  {
    *ifIndex = 0;
    rc = L7_SUCCESS; 
  }
  else
  {
    /* translate intIfNum to external ifIndex */
    rc = usmDbExtIfNumFromIntIfNum(intIfNum, ifIndex);
  }
  
  return rc;
}

L7_RC_t
snmpQosAclIpv6RuleRedirectIntfAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                                  L7_uint32 aclRuleIndex, L7_uint32 ifIndex)
{
  L7_RC_t   rc;
  L7_uint32 intIfNum;
             
  /* translate external ifIndex to intIfNum */
  rc = usmDbIntIfNumFromExtIfNum(ifIndex, &intIfNum);

  if (rc == L7_SUCCESS)
  {
    rc = usmDbQosAclRuleRedirectIntfAdd(UnitIndex, aclIndex, aclRuleIndex, 
                                        intIfNum);
  }
  
  return rc;
}

L7_RC_t
snmpQosAclIpv6RuleMirrorIntfAdd(L7_uint32 UnitIndex, L7_uint32 aclIndex, 
                            L7_uint32 aclRuleIndex, L7_uint32 ifIndex)
{
  L7_RC_t   rc;
  L7_uint32 intIfNum;
             
  /* translate external ifIndex to intIfNum */
  rc = usmDbIntIfNumFromExtIfNum(ifIndex, &intIfNum);

  if (rc == L7_SUCCESS)
  {
    rc = usmDbQosAclRuleMirrorIntfAdd(UnitIndex, aclIndex, aclRuleIndex, 
                                          intIfNum);
  }
  
  return rc;
}

L7_RC_t
snmpQosAclIpv6RuleStatusSet(L7_uint32 UnitIndex, L7_uint32 aclIndex, L7_uint32 aclRuleIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_aclIpv6Status_active:
    break;

  case D_aclIpv6Status_destroy:
    rc = usmDbQosAclRuleRemove(UnitIndex, aclIndex, aclRuleIndex);
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

L7_RC_t
snmpQosAclIpv6RuleActionGet(L7_uint32 UnitIndex, L7_uint32 aclIpv6Index, L7_uint32 aclIpv6RuleIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_action;

  rc = usmDbQosAclRuleActionGet(UnitIndex, aclIpv6Index, aclIpv6RuleIndex, &temp_action);

  if (rc == L7_SUCCESS)
  {
    switch (temp_action)
    {
    case L7_ACL_PERMIT:
      *val = D_aclIpv6RuleAction_permit;
      break;

    case L7_ACL_DENY:
      *val = D_aclIpv6RuleAction_deny;
      break;

    default:
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpQosAclIpv6RuleActionAdd(L7_uint32 UnitIndex, L7_uint32 aclIpv6Index, L7_uint32 aclIpv6RuleIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_action = 0;

  switch (val)
  {
  case D_aclIpv6RuleAction_permit:
    temp_action = L7_ACL_PERMIT;
    break;

  case D_aclIpv6RuleAction_deny:
    temp_action = L7_ACL_DENY;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbQosAclRuleActionAdd(UnitIndex, aclIpv6Index, aclIpv6RuleIndex, temp_action);
  }

  return rc;
}

L7_RC_t          
snmpQosAclIpv6RuleEveryGet(L7_uint32 UnitIndex, L7_uint32 aclIpv6Index, 
                           L7_uint32 aclIpv6RuleIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbQosAclRuleEveryGet (UnitIndex, aclIpv6Index, aclIpv6RuleIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_aclIpv6RuleMatchEvery_true;
      break;

    case L7_FALSE:
      *val = D_aclIpv6RuleMatchEvery_false;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t          
snmpQosAclIpv6RuleEveryAdd(L7_uint32 UnitIndex, L7_uint32 aclIpv6Index, 
                           L7_uint32 aclIpv6RuleIndex, L7_int32 val)
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  switch (val)
  {
    case D_aclIpv6RuleMatchEvery_true:
      temp_val = L7_TRUE;
      break;

    case D_aclIpv6RuleMatchEvery_false:
      temp_val = L7_FALSE;
      break;

    default:
      temp_val = 0;
      rc = L7_FAILURE;
      break;
  }

  rc = usmDbQosAclRuleEveryAdd(UnitIndex, aclIpv6Index, aclIpv6RuleIndex, temp_val);


  return rc;
}



L7_RC_t
snmpQosAclIpv6RuleEntryGet(L7_uint32 UnitIndex, aclIpv6RuleEntry_t *aclIpv6RuleEntryData, L7_int32 nominator)
{
  L7_RC_t            rc = L7_FAILURE;
  L7_uint32          temp_uint;
  L7_uint32          snmp_buffer_len, prefixLen;  
  static L7_uint32   count=0;
  L7_char8           snmp_buffer[SNMP_BUFFER_LEN]; 

  memset(snmp_buffer, 0x00, (size_t)SNMP_BUFFER_LEN);
  count++;
  memset(snmp_buffer, 0x00, sizeof(snmp_buffer));
  if(usmDbQosAclRuleCheckValid(UnitIndex, aclIpv6RuleEntryData->aclIpv6Index, aclIpv6RuleEntryData->aclIpv6RuleIndex) == L7_SUCCESS)
  {
    CLR_VALID(nominator, aclIpv6RuleEntryData->valid);
    /*
     * if ( nominator != -1 ) condition is added to all the case statements
     * for storing all the values to support the undo functionality.
     */
    switch (nominator)
    {
      case -1:
      case I_aclIpv6RuleEntryIndex_aclIpv6Index:
      case I_aclIpv6RuleIndex:
        rc = L7_SUCCESS;
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclIpv6RuleAction:
        if (usmDbQosAclIsFieldConfigured(UnitIndex, aclIpv6RuleEntryData->aclIpv6Index,
                                         aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                         ACL_ACTION) == L7_TRUE)
        {
          rc = snmpQosAclIpv6RuleActionGet(UnitIndex,
                                           aclIpv6RuleEntryData->aclIpv6Index,
                                           aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                           &aclIpv6RuleEntryData->aclIpv6RuleAction);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclIpv6RuleAction, aclIpv6RuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

    case I_aclIpv6RuleStatus:
       /* all entries are active */
       rc = L7_SUCCESS;
       aclIpv6RuleEntryData->aclIpv6RuleStatus = D_aclIpv6RuleStatus_active;
       SET_VALID(I_aclIpv6RuleStatus, aclIpv6RuleEntryData->valid);
       if (nominator != -1 ) break;
       /* else pass through */

    case I_aclIpv6RuleAssignQueueId:
       if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_ASSIGN_QUEUE_FEATURE_ID) == L7_TRUE) &&
           (usmDbQosAclIsFieldConfigured(UnitIndex, aclIpv6RuleEntryData->aclIpv6Index,
                                         aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                         ACL_ASSIGN_QUEUEID) == L7_TRUE))
       {
         rc = usmDbQosAclRuleAssignQueueIdGet(UnitIndex,
                                              aclIpv6RuleEntryData->aclIpv6Index,
                                              aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                              &aclIpv6RuleEntryData->aclIpv6RuleAssignQueueId);
         if (rc == L7_SUCCESS)
           SET_VALID(I_aclIpv6RuleAssignQueueId, aclIpv6RuleEntryData->valid);
       }
       if (nominator != -1 ) break;
       /* else pass through */


    case I_aclIpv6RuleMatchEvery:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_EVERY_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclIsFieldConfigured(UnitIndex, aclIpv6RuleEntryData->aclIpv6Index,
                                          aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                          ACL_EVERY) == L7_TRUE))
        {
          rc = snmpQosAclIpv6RuleEveryGet(UnitIndex,
                                          aclIpv6RuleEntryData->aclIpv6Index,
                                          aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                          &aclIpv6RuleEntryData->aclIpv6RuleMatchEvery);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclIpv6RuleMatchEvery, aclIpv6RuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

   
    case I_aclIpv6RuleLogging:
        if ((usmDbQosAclRuleLoggingAllowed(USMDB_UNIT_CURRENT, 
                                           aclIpv6RuleEntryData->aclIpv6Index, 
                                           aclIpv6RuleEntryData->aclIpv6RuleIndex) == L7_SUCCESS) &&
            (usmDbQosAclIsFieldConfigured(UnitIndex, 
                                          aclIpv6RuleEntryData->aclIpv6Index,
                                          aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                          ACL_LOGGING) == L7_TRUE))
        {
          rc = snmpQosAclRuleLoggingGet(UnitIndex,
                                        aclIpv6RuleEntryData->aclIpv6Index,
                                        aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                        &aclIpv6RuleEntryData->aclIpv6RuleLogging);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclIpv6RuleLogging, aclIpv6RuleEntryData->valid);
        }
       break;
       /* last valid nominator */
    case I_aclIpv6RuleRedirectIntf:
       if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE) &&
           (usmDbQosAclIsFieldConfigured(UnitIndex, aclIpv6RuleEntryData->aclIpv6Index,
                                         aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                         ACL_REDIRECT_INTF) == L7_TRUE))
       {
         rc = snmpQosAclRuleRedirectIntfGet(UnitIndex,
                                            aclIpv6RuleEntryData->aclIpv6Index,
                                            aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                            &aclIpv6RuleEntryData->aclIpv6RuleRedirectIntf);
         if (rc == L7_SUCCESS)
           SET_VALID(I_aclIpv6RuleRedirectIntf, aclIpv6RuleEntryData->valid);
       }
       if (nominator != -1 ) break;
       /* else pass through */

    case I_aclIpv6RuleMirrorIntf:
       if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                     L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE) &&
           (usmDbQosAclIsFieldConfigured(UnitIndex, aclIpv6RuleEntryData->aclIpv6Index,
                                         aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                         ACL_MIRROR_INTF) == L7_TRUE))
       {
         rc = snmpQosAclRuleMirrorIntfGet(UnitIndex,
                                          aclIpv6RuleEntryData->aclIpv6Index,
                                          aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                          &aclIpv6RuleEntryData->aclIpv6RuleMirrorIntf);
         if (rc == L7_SUCCESS)
           SET_VALID(I_aclIpv6RuleMirrorIntf, aclIpv6RuleEntryData->valid);
       }
       if (nominator != -1 ) break;
       /* else pass through */

    case I_aclIpv6RuleProtocol:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_IPV6_PROTOCOL_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclIsFieldConfigured(UnitIndex, aclIpv6RuleEntryData->aclIpv6Index,
                                          aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                          ACL_PROTOCOL) == L7_TRUE))
        {
          if ((rc = usmDbQosAclRuleProtocolGet(UnitIndex, aclIpv6RuleEntryData->aclIpv6Index, 
                                         aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                         &aclIpv6RuleEntryData->aclIpv6RuleProtocol)) == L7_SUCCESS){
             SET_VALID(I_aclIpv6RuleProtocol, aclIpv6RuleEntryData->valid);
          }
        }
        if (nominator != -1 ) break;
        /* else pass through */   

    case I_aclIpv6RuleDestL4Port:
      if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                    L7_ACL_RULE_MATCH_IPV6_DSTL4PORT_FEATURE_ID) == L7_TRUE) &&
          (usmDbQosAclIsFieldConfigured(UnitIndex, aclIpv6RuleEntryData->aclIpv6Index,
                                        aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                        ACL_DSTPORT) == L7_TRUE))
      {
        rc = usmDbQosAclRuleDstL4PortGet(UnitIndex,
                                         aclIpv6RuleEntryData->aclIpv6Index,
                                         aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                         &aclIpv6RuleEntryData->aclIpv6RuleDestL4Port);
        if (rc == L7_SUCCESS)
          SET_VALID(I_aclIpv6RuleDestL4Port, aclIpv6RuleEntryData->valid);
      }
      if (nominator != -1 ) break;
      /* else pass through */

    case I_aclIpv6RuleDestL4PortRangeStart:
      if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                    L7_ACL_RULE_MATCH_DSTL4PORT_RANGE_FEATURE_ID) == L7_TRUE) &&
          (usmDbQosAclIsFieldConfigured(UnitIndex, aclIpv6RuleEntryData->aclIpv6Index,
                                        aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                        ACL_DSTSTARTPORT) == L7_TRUE))
      {
        rc = usmDbQosAclRuleDstL4PortRangeGet(UnitIndex,
                                              aclIpv6RuleEntryData->aclIpv6Index,
                                              aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                              &aclIpv6RuleEntryData->aclIpv6RuleDestL4PortRangeStart,
                                              &temp_uint);
        if (rc == L7_SUCCESS)
          SET_VALID(I_aclIpv6RuleDestL4PortRangeStart, aclIpv6RuleEntryData->valid);
      }
      if (nominator != -1 ) break;
      /* else pass through */

  case I_aclIpv6RuleDestL4PortRangeEnd:
      if((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                    L7_ACL_RULE_MATCH_DSTL4PORT_RANGE_FEATURE_ID) == L7_TRUE) &&
          (usmDbQosAclIsFieldConfigured(UnitIndex, aclIpv6RuleEntryData->aclIpv6Index,
                                        aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                        ACL_DSTENDPORT) == L7_TRUE))
      {
        rc = usmDbQosAclRuleDstL4PortRangeGet(UnitIndex,
                                              aclIpv6RuleEntryData->aclIpv6Index,
                                              aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                              &temp_uint,
                                              &aclIpv6RuleEntryData->aclIpv6RuleDestL4PortRangeEnd);
        if (rc == L7_SUCCESS)
          SET_VALID(I_aclIpv6RuleDestL4PortRangeEnd, aclIpv6RuleEntryData->valid);
      }
      if (nominator != -1 ) break;

  case I_aclIpv6RuleSrcL4Port:
      if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                    L7_ACL_RULE_MATCH_IPV6_SRCL4PORT_FEATURE_ID) == L7_TRUE) &&
          (usmDbQosAclIsFieldConfigured(UnitIndex, aclIpv6RuleEntryData->aclIpv6Index,
                                        aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                        ACL_SRCPORT) == L7_TRUE))
      {
        rc = usmDbQosAclRuleSrcL4PortGet(UnitIndex,
                                         aclIpv6RuleEntryData->aclIpv6Index,
                                         aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                         &aclIpv6RuleEntryData->aclIpv6RuleSrcL4Port);
        if (rc == L7_SUCCESS)
          SET_VALID(I_aclIpv6RuleSrcL4Port, aclIpv6RuleEntryData->valid);
      }
      if (nominator != -1 ) break;
      /* else pass through */

    case I_aclIpv6RuleSrcL4PortRangeStart:
      if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                    L7_ACL_RULE_MATCH_SRCL4PORT_RANGE_FEATURE_ID) == L7_TRUE) &&
          (usmDbQosAclIsFieldConfigured(UnitIndex, aclIpv6RuleEntryData->aclIpv6Index,
                                        aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                        ACL_SRCSTARTPORT) == L7_TRUE))
      {
        rc = usmDbQosAclRuleSrcL4PortRangeGet(UnitIndex,
                                              aclIpv6RuleEntryData->aclIpv6Index,
                                              aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                              &aclIpv6RuleEntryData->aclIpv6RuleSrcL4PortRangeStart,
                                              &temp_uint);
        if (rc == L7_SUCCESS)
          SET_VALID(I_aclIpv6RuleSrcL4PortRangeStart, aclIpv6RuleEntryData->valid);
      }
      if (nominator != -1 ) break;
      /* else pass through */

    case I_aclIpv6RuleSrcL4PortRangeEnd:
      if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                    L7_ACL_RULE_MATCH_SRCL4PORT_RANGE_FEATURE_ID) == L7_TRUE) &&
          (usmDbQosAclIsFieldConfigured(UnitIndex, aclIpv6RuleEntryData->aclIpv6Index,
                                        aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                        ACL_SRCENDPORT) == L7_TRUE))
      {
        rc = usmDbQosAclRuleSrcL4PortRangeGet(UnitIndex,
                                              aclIpv6RuleEntryData->aclIpv6Index,
                                              aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                              &temp_uint,
                                              &aclIpv6RuleEntryData->aclIpv6RuleSrcL4PortRangeEnd);
        if (rc == L7_SUCCESS)
          SET_VALID(I_aclIpv6RuleSrcL4PortRangeEnd, aclIpv6RuleEntryData->valid);
      }
      if (nominator != -1 ) break;
      /* else pass through */

    case I_aclIpv6RuleFlowLabel:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_IPV6_FLOWLBL_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclIsFieldConfigured(UnitIndex, aclIpv6RuleEntryData->aclIpv6Index,
                                          aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                          ACL_FLOWLBLV6) == L7_TRUE))
        {
          rc = usmDbQosAclRuleIpv6FlowLabelGet(UnitIndex,
                                               aclIpv6RuleEntryData->aclIpv6Index,
                                               aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                               &aclIpv6RuleEntryData->aclIpv6RuleFlowLabel);
          if (rc == L7_SUCCESS)
            SET_VALID(I_aclIpv6RuleFlowLabel, aclIpv6RuleEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

    case I_aclRuleSrcIpv6Prefix:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_IPV6_SRCIP_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclIsFieldConfigured(UnitIndex, aclIpv6RuleEntryData->aclIpv6Index,
                                          aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                          ACL_SRCIPV6) == L7_TRUE))
        {
          if((rc = snmpQosAclRuleSrcIpv6AddrGet(UnitIndex,
                                            aclIpv6RuleEntryData->aclIpv6Index,
                                            aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                            snmp_buffer, &snmp_buffer_len, &prefixLen) == L7_SUCCESS)
             && (SafeMakeOctetString(&aclIpv6RuleEntryData->aclRuleSrcIpv6Prefix, 
                                     snmp_buffer, snmp_buffer_len) == L7_TRUE))
          {
             rc = L7_SUCCESS;
             SET_VALID(I_aclRuleSrcIpv6Prefix, aclIpv6RuleEntryData->valid);
          }
        }
        if (nominator != -1 ) break;

    case I_aclRuleSrcIpv6PrefixLength:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_IPV6_SRCIP_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclIsFieldConfigured(UnitIndex, aclIpv6RuleEntryData->aclIpv6Index,
                                          aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                          ACL_SRCIPV6) == L7_TRUE))
        {
          if((rc = snmpQosAclRuleSrcIpv6AddrGet(UnitIndex,
                                                aclIpv6RuleEntryData->aclIpv6Index,
                                                aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                                snmp_buffer, &snmp_buffer_len, 
                                                &aclIpv6RuleEntryData->aclRuleSrcIpv6PrefixLength)) == L7_SUCCESS)
          {
             SET_VALID(I_aclRuleSrcIpv6PrefixLength, aclIpv6RuleEntryData->valid);
          }
        }
        if (nominator != -1 ) break;

    case I_aclRuleDstIpv6Prefix:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                      L7_ACL_RULE_MATCH_IPV6_DSTIP_FEATURE_ID) == L7_TRUE) &&
            (usmDbQosAclIsFieldConfigured(UnitIndex, aclIpv6RuleEntryData->aclIpv6Index,
                                          aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                          ACL_DSTIPV6) == L7_TRUE))
        {
          if((rc = snmpQosAclRuleDstIpv6AddrGet(UnitIndex,
                                                aclIpv6RuleEntryData->aclIpv6Index,
                                                aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                                snmp_buffer, &snmp_buffer_len, &prefixLen) == L7_SUCCESS)
             && (SafeMakeOctetString(&aclIpv6RuleEntryData->aclRuleDstIpv6Prefix, 
                                     snmp_buffer, snmp_buffer_len) == L7_TRUE))
          {
             rc = L7_SUCCESS;
             SET_VALID(I_aclRuleDstIpv6Prefix, aclIpv6RuleEntryData->valid);
          }
        }
        if (nominator != -1 ) break;

    case I_aclRuleDstIpv6PrefixLength:
        if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID, 
                                      L7_ACL_RULE_MATCH_IPV6_DSTIP_FEATURE_ID) == L7_TRUE) && 
            (usmDbQosAclIsFieldConfigured(UnitIndex, aclIpv6RuleEntryData->aclIpv6Index, 
                                          aclIpv6RuleEntryData->aclIpv6RuleIndex, 
                                          ACL_DSTIPV6) == L7_TRUE))
        {
          if((rc = snmpQosAclRuleDstIpv6AddrGet(UnitIndex,
                                                aclIpv6RuleEntryData->aclIpv6Index,
                                                aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                                snmp_buffer, &snmp_buffer_len, 
                                                &aclIpv6RuleEntryData->aclRuleDstIpv6PrefixLength)) == L7_SUCCESS)
          {
             SET_VALID(I_aclRuleDstIpv6PrefixLength, aclIpv6RuleEntryData->valid);
          }
        }
        if (nominator != -1 ) break;

  case I_aclIpv6RuleIPDSCP:
      if ((usmDbFeaturePresentCheck(UnitIndex, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                    L7_ACL_RULE_MATCH_IPDSCP_FEATURE_ID) == L7_TRUE) &&
          (usmDbQosAclIsFieldConfigured(UnitIndex, aclIpv6RuleEntryData->aclIpv6Index,
                                        aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                        ACL_IPDSCP) == L7_TRUE))
      {
        rc = usmDbQosAclRuleIPDscpGet(UnitIndex,
                                      aclIpv6RuleEntryData->aclIpv6Index,
                                      aclIpv6RuleEntryData->aclIpv6RuleIndex,
                                      &aclIpv6RuleEntryData->aclIpv6RuleIPDSCP);
        if (rc == L7_SUCCESS)
          SET_VALID(I_aclIpv6RuleIPDSCP, aclIpv6RuleEntryData->valid);
      }
      if (nominator != -1 ) break;
      /* else pass through */

    default:
      rc = L7_FAILURE;
      break;
    }

    if (nominator >= 0 && rc == L7_SUCCESS)
      SET_VALID(nominator, aclIpv6RuleEntryData->valid);
    else if (nominator == -1)
      rc = L7_SUCCESS;
  }
  return rc;
}

L7_RC_t
snmpQosAclIpv6RuleEntryGetNext(L7_uint32 UnitIndex, aclIpv6RuleEntry_t *aclIpv6RuleEntryData, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;

  while (1)
  {
    if (usmDbQosAclRuleGetNext(UnitIndex, 
                               aclIpv6RuleEntryData->aclIpv6Index, 
                               aclIpv6RuleEntryData->aclIpv6RuleIndex, 
                               &aclIpv6RuleEntryData->aclIpv6RuleIndex) == L7_SUCCESS)
    {
      if (snmpQosAclIpv6RuleEntryGet(UnitIndex, aclIpv6RuleEntryData, nominator) == L7_SUCCESS)
      {
        rc = L7_SUCCESS;
        break;
      }
    }
    else
    {
      /* no more rules in current access list, so move on to next list */
      if (usmDbQosAclNamedIndexGetNext(UnitIndex, L7_ACL_TYPE_IPV6, aclIpv6RuleEntryData->aclIpv6Index, 
                                       &aclIpv6RuleEntryData->aclIpv6Index) == L7_SUCCESS)
      {
         aclIpv6RuleEntryData->aclIpv6RuleIndex = 0;
      }
      else
      {
         break;
      }
    }
  }
  return rc;
}


/**************************************************************************************************************/

L7_RC_t
snmpQosAclIpv6StatusSet(L7_uint32 UnitIndex, L7_uint32 aclIpv6Index, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_aclIpv6Status_active:
    break;

  case D_aclIpv6Status_destroy:
    rc = usmDbQosAclDelete(UnitIndex, aclIpv6Index);
    break;

  default:
    rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t
snmpQosAclIpv6EntryGet(L7_uint32 UnitIndex, aclIpv6Entry_t *aclIpv6EntryData, L7_int32 nominator)
{
  L7_RC_t   rc = L7_FAILURE;
  L7_char8  snmp_buffer[SNMP_BUFFER_LEN];

  if (usmDbQosAclNamedIndexCheckValid(UnitIndex, L7_ACL_TYPE_IPV6, aclIpv6EntryData->aclIpv6Index) == L7_SUCCESS)
  {
    CLR_VALID(nominator, aclIpv6EntryData->valid);

    /*
     * if ( nominator != -1 ) condition is added to all the case statemen
     * for storing all the values to support the undo functionality.
     */
    switch (nominator)
    {
      case -1:
      case I_aclIpv6Index:
        rc = L7_SUCCESS;
        if( nominator != -1) break;
        /* else pass through */

      case I_aclIpv6Name:
        memset(snmp_buffer, 0, (size_t)SNMP_BUFFER_LEN);
        if ((usmDbQosAclNameGet(UnitIndex,
                                aclIpv6EntryData->aclIpv6Index,
                                snmp_buffer) == L7_SUCCESS) &&
            (SafeMakeOctetString(&aclIpv6EntryData->aclIpv6Name,
                                 snmp_buffer, 
                                 strlen((char *)snmp_buffer)) == L7_TRUE))
        {
          rc = L7_SUCCESS;
          SET_VALID(I_aclIpv6Name, aclIpv6EntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_aclIpv6Status:
        /* all entries are active */
        rc = L7_SUCCESS;
        aclIpv6EntryData->aclIpv6Status = D_aclIpv6Status_active;
        SET_VALID(I_aclIpv6Status, aclIpv6EntryData->valid);
        break;
        /* last valid nominator */

      default:
        rc = L7_FAILURE;
        break;
    }

    if (nominator >= 0 && rc == L7_SUCCESS)
      SET_VALID(nominator, aclIpv6EntryData->valid);
    else if (nominator == -1)
      rc = L7_SUCCESS;
  }

  return rc;
}

L7_RC_t
snmpQosAclIpv6EntryGetNext(L7_uint32 UnitIndex, aclIpv6Entry_t *aclIpv6EntryData, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;

  while (usmDbQosAclNamedIndexGetNext(UnitIndex, L7_ACL_TYPE_IPV6,
                                      aclIpv6EntryData->aclIpv6Index, 
                                      &aclIpv6EntryData->aclIpv6Index) == L7_SUCCESS)
  {
    if (snmpQosAclIpv6EntryGet(UnitIndex, aclIpv6EntryData, nominator) == L7_SUCCESS)
      {
        rc = L7_SUCCESS;
        break;
      }
  }

  return rc;
}

L7_RC_t
snmpQosAclIpv6RuleEntryAdd(L7_uint32 UnitIndex, L7_uint32 aclIpv6Index, L7_uint32 aclIpv6RuleIndex)
{
  L7_RC_t rc = L7_SUCCESS;

  rc = usmDbQosAclRuleActionAdd(UnitIndex, aclIpv6Index, aclIpv6RuleIndex, L7_ACL_DENY);

  return rc;
}

/* Changes related to vlan based ACLs */
L7_RC_t
snmpQosAclVlanCheckValid(L7_uint32 UnitIndex, L7_uint32 aclVlanIndex, L7_uint32 aclVlanDirection,
                         L7_uint32 aclVlanSequence, L7_uint32 aclVlanAclType, L7_uint32 aclVlanAclId)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 count = 0;
  L7_ACL_VLAN_DIR_LIST_t listInfo;
  
  L7_uint32 temp_direction = 3; /* Initializing with a wrong direction value */


  switch (aclVlanDirection)
  {
    case D_aclVlanDirection_inbound:
      temp_direction = L7_INBOUND_ACL;
      break;

    case D_aclVlanDirection_outbound:
      temp_direction = L7_OUTBOUND_ACL;
      break;

    default:
      rc = L7_FAILURE;
      break;
  }

  rc = usmDbQosAclVlanDirAclListGet(UnitIndex,aclVlanIndex,temp_direction,&listInfo);

  if(rc == L7_SUCCESS)
    for(count = 0;count < listInfo.count;count++)
    {
      rc = L7_FAILURE;
      if(listInfo.listEntry[count].seqNum == aclVlanSequence)
      {
        rc = L7_SUCCESS;
        break;
      }
    }
   
  if((rc == L7_SUCCESS) && (listInfo.listEntry[count].aclType == aclVlanAclType)
                        && (listInfo.listEntry[count].aclId == aclVlanAclId))
  {
    rc = L7_SUCCESS;
  }
  else
    rc = L7_FAILURE;

  return rc;
}

L7_RC_t
snmpQosAclVlanNext(L7_uint32 UnitIndex, L7_uint32 *aclVlanIndex, L7_uint32 *aclVlanDirection, 
                   L7_uint32 *aclVlanSequence, L7_uint32 *aclVlanAclType, L7_uint32 *aclVlanAclId)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 count = 0;
  L7_ACL_VLAN_DIR_LIST_t listInfo;
  L7_uint32 nextAclVlanIndex, nextAclVlanDirection, nextAclVlanSequence;
  L7_uint32 temp_direction; 

  switch (*aclVlanDirection)
  {
    case 0:
      temp_direction = 0;
      break;

    case D_aclVlanDirection_inbound:
      temp_direction = L7_INBOUND_ACL;
      break;

    case D_aclVlanDirection_outbound:
      temp_direction = L7_OUTBOUND_ACL;
      break;

    default:
      temp_direction = (*aclVlanDirection)-1;
      break;
  }

  while(1)
  {
    rc = usmDbQosAclVlanDirSequenceGetNext(UnitIndex, *aclVlanIndex, temp_direction,
                                           *aclVlanSequence, &nextAclVlanSequence); 
    if(rc == L7_SUCCESS)
    {
      *aclVlanSequence = nextAclVlanSequence;
      break;
    }
   
    if (rc != L7_SUCCESS)
    {
      /* Trying to get next (interface + direction) in use */
      rc = usmDbQosAclVlanDirGetNext(UnitIndex,
                                     *aclVlanIndex,
                                     temp_direction,
                                     &nextAclVlanIndex,
                                     &nextAclVlanDirection);
    
      if(rc != L7_SUCCESS)
      {
        break;
      }
           
      else if(rc == L7_SUCCESS && (usmDbQosAclIsVlanInUse(UnitIndex, 
                                                          nextAclVlanIndex,
                                                          nextAclVlanDirection)) == L7_TRUE)
      {
          *aclVlanIndex = nextAclVlanIndex;
          temp_direction = nextAclVlanDirection;
          *aclVlanSequence = 0;
          rc = L7_SUCCESS;
      }
      else if( rc == L7_SUCCESS && (usmDbQosAclIsVlanInUse(UnitIndex, 
                                                           nextAclVlanIndex,
                                                           nextAclVlanDirection)) != L7_TRUE)
      {
         *aclVlanIndex = nextAclVlanIndex;
         temp_direction = nextAclVlanDirection;
         *aclVlanSequence = 0;
         rc = L7_FAILURE;
         continue;
      }
      
      if (rc == L7_SUCCESS)
      { 
        /* Get the first sequence for this interface and direction with input value of 
           *aclVlanSequence = 0 */
        rc = usmDbQosAclVlanDirSequenceGetNext(UnitIndex,nextAclVlanIndex,nextAclVlanDirection,
                                               *aclVlanSequence,
                                               &nextAclVlanSequence);

        if(rc == L7_SUCCESS)
        {
          *aclVlanIndex = nextAclVlanIndex;
          temp_direction = nextAclVlanDirection; 
          *aclVlanSequence = nextAclVlanSequence;
          break;
        }
          
      }
    } /* end of if (rc != L7_SUCCESS) */

  } /* end of while(1) */


  /* Once got the values of next intfIndex, Direction and sequence, now need to get
     next aclType and aclId. For a pair of (intfIndex+ Direction), there would be only one
     combination of (aclType + aclId) for a particular value of aclVlanSequence. */
  if(rc == L7_SUCCESS)
    rc = usmDbQosAclVlanDirAclListGet( UnitIndex,*aclVlanIndex,temp_direction,&listInfo);

  if(rc == L7_SUCCESS)
    for(count = 0;count < listInfo.count;count++)
    {
      rc = L7_FAILURE;
      if(listInfo.listEntry[count].seqNum == *aclVlanSequence)
      {
        rc = L7_SUCCESS;
        break;
      }
    }
   
  if(rc == L7_SUCCESS)
  {
    switch (temp_direction)
    {
      case L7_INBOUND_ACL: 
        *aclVlanDirection = D_aclVlanDirection_inbound;
        break;

      case L7_OUTBOUND_ACL: 
        *aclVlanDirection = D_aclVlanDirection_outbound;
        break;

      default:
        rc = L7_FAILURE;
        break;
    }
    *aclVlanAclType = listInfo.listEntry[count].aclType; 
    *aclVlanAclId =   listInfo.listEntry[count].aclId;  
  }

  return rc;
}


L7_BOOL
snmpIsQosAclIdAttachedToVlan(L7_uint32 UnitIndex, L7_uint32 aclIndex)
{
  L7_RC_t rc = L7_FALSE;
  L7_ACL_ASSIGNED_VLAN_LIST_t infoList;
   
  /* Passing 3rd argument as 0 for inbound and 1 for outbound directions */
  if((usmDbQosAclAssignedVlanDirListGet(UnitIndex,aclIndex, 
                                        L7_INBOUND_ACL , &infoList) == L7_SUCCESS)
    ||(usmDbQosAclAssignedVlanDirListGet(UnitIndex,aclIndex, 
                                        L7_OUTBOUND_ACL, &infoList) == L7_SUCCESS))

  {
    rc = L7_TRUE;
  }
  else 
    rc = L7_FALSE;
  
  return rc;
}


L7_RC_t
snmpQosAclVlanAdd(L7_uint32 UnitIndex, L7_uint32 aclVlanIndex, L7_uint32 aclVlanDirection,
                  L7_uint32 aclVlanSequence, L7_uint32 aclVlanAclType, L7_uint32 aclVlanAclId)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_direction = 3; /* Initializing with a wrong direction value */


  switch (aclVlanDirection)
  {
  case D_aclVlanDirection_inbound:
    temp_direction = L7_INBOUND_ACL;
    break;

  case D_aclVlanDirection_outbound:
    temp_direction = L7_OUTBOUND_ACL;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }


  if (rc == L7_SUCCESS && aclVlanAclType == D_aclVlanAclType_ip)
    rc = usmDbQosAclVlanDirectionAdd(UnitIndex, aclVlanIndex, 
                                     temp_direction, aclVlanAclId, aclVlanSequence);  

  if (rc == L7_SUCCESS && aclVlanAclType == D_aclVlanAclType_ipv6)
    rc = usmDbQosAclVlanDirectionAdd(UnitIndex, aclVlanIndex, 
                                     temp_direction, aclVlanAclId, aclVlanSequence);  

  if (rc == L7_SUCCESS && aclVlanAclType == D_aclVlanAclType_mac)
    rc = usmDbQosAclMacVlanDirectionAdd(UnitIndex, aclVlanIndex, 
                                        temp_direction, aclVlanAclId, aclVlanSequence);  

  return rc;
}

L7_RC_t
snmpQosAclVlanStatusSet(L7_uint32 UnitIndex, L7_uint32 aclVlanIndex, L7_uint32 aclVlanDirection,
                        L7_uint32 aclVlanSequence, L7_uint32 aclVlanAclType, L7_uint32 aclVlanAclId,
                        L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  L7_uint32 temp_direction = 3;           /* Initializing with a wrong direction value */


  switch (aclVlanDirection)
  {
  case D_aclVlanDirection_inbound:
    temp_direction = L7_INBOUND_ACL;
    break;

  case D_aclVlanDirection_outbound:
    temp_direction = L7_OUTBOUND_ACL;
    break;

  default:
    rc = L7_FAILURE;
    break;
  }
  
  switch (val)
  {
  case D_aclStatus_active:
    break;

  case D_aclStatus_destroy:
  
  if (rc == L7_SUCCESS && aclVlanAclType == D_aclVlanAclType_ip)
  {
    rc = usmDbQosAclVlanDirectionRemove(UnitIndex, aclVlanIndex, 
                                        temp_direction, aclVlanAclId);
  }

  if (rc == L7_SUCCESS && aclVlanAclType == D_aclVlanAclType_ipv6)
  {
    rc = usmDbQosAclVlanDirectionRemove(UnitIndex, aclVlanIndex, 
                                        temp_direction, aclVlanAclId);
  }

  if (rc == L7_SUCCESS && aclVlanAclType == D_aclVlanAclType_mac)
  {

    rc = usmDbQosAclMacVlanDirectionRemove(UnitIndex, aclVlanIndex, 
                                           temp_direction, aclVlanAclId);    

  }
        break;

  default:
    rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t usmDbVlanIDGetNewVlanId(L7_uint32 vlanId, L7_uint32 *aclVlanIndex)
{
   if(vlanId >=L7_DOT1Q_DEFAULT || vlanId <= L7_MAX_VLAN_ID)
   {
      *aclVlanIndex = vlanId;
      return L7_SUCCESS;
   }

   return L7_FAILURE;
}


L7_RC_t 
usmDbVlanIDCheck(L7_uint32 aclVlanIndex)
{
   if(aclVlanIndex >= L7_DOT1Q_DEFAULT || aclVlanIndex <= L7_MAX_VLAN_ID)
   {
     return L7_SUCCESS;
   }
   return L7_FAILURE;
}

L7_RC_t usmDbNextVlanGetNext(L7_uint32 aclVlanIndex, L7_uint32 *aclVlanIndexNext)
{
   if(aclVlanIndex >= L7_DOT1Q_DEFAULT-1  || aclVlanIndex < L7_MAX_VLAN_ID)
   {
     *aclVlanIndexNext = aclVlanIndex+1;
     return L7_SUCCESS;
   }
   return L7_FAILURE;
}
