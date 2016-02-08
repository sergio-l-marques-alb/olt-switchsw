/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
**********************************************************************
* @filename  k_mib_qos_acl.c
*
* @purpose   Quality of Service - ACL MIB implementation
*
* @create    07/24/2002
*
* @author    Colin Verne
*
* @end
*
**********************************************************************/
#include <k_private_base.h>
#include "k_mib_qos_acl_api.h"
#include "usmdb_common.h"
#include "acl_exports.h"
#include "diffserv_exports.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_util_api.h"

#ifdef L7_QOS_PACKAGE
#include "usmdb_qos_acl_api.h"
#endif


L7_uint32 aclIfEntry_count =0 , aclRuleEntry_count =0;

aclEntry_t *
k_aclEntry_get(int serialNum, ContextInfo *contextInfo,
               int nominator,
               int searchType,
               SR_INT32 aclIndex)
{
  static aclEntry_t aclEntryData;
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    aclEntryData.aclName = MakeOctetString(NULL, 0);
  }


  ZERO_VALID(aclEntryData.valid);
  aclEntryData.aclIndex = aclIndex;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  SET_VALID(I_aclIndex, aclEntryData.valid);

  if ( (searchType == EXACT) ?
       (snmpQosAclEntryGet(USMDB_UNIT_CURRENT, &aclEntryData, nominator) != L7_SUCCESS) :
       ((snmpQosAclEntryGet(USMDB_UNIT_CURRENT, &aclEntryData, nominator) != L7_SUCCESS) &&
        (snmpQosAclEntryGetNext(USMDB_UNIT_CURRENT, &aclEntryData, nominator) != L7_SUCCESS)) )
  {
    ZERO_VALID(aclEntryData.valid);
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, aclEntryData.valid))
    return(NULL);

  return(&aclEntryData);
}

#ifdef SETS
int
k_aclEntry_test(ObjectInfo *object, ObjectSyntax *value,
                doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_aclEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                 doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_aclEntry_set_defaults(doList_t *dp)
{
  aclEntry_t *data = (aclEntry_t *) (dp->data);

  if ((data->aclName = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_aclEntry_set(aclEntry_t *data,
               ContextInfo *contextInfo, int function)
{
  L7_BOOL isCreatedNew = L7_FALSE;
  aclRuleEntry_t temp_aclRuleEntryData;
  L7_uint32      temp_aclIndex;
  char snmp_buffer[SNMP_BUFFER_LEN];

   /* This variable is added to pass to snmpDiffServClassGet() function so that 
    * our set values in 'data' are not lost.
    */
   aclEntry_t *tempData;

  /*
   * Defining temporary variable for storing the valid bits for the case when th
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  tempData = Clone_aclEntry(data);
  tempData->aclIndex = data->aclIndex;                /* QSCAN: DELETE -- Temp fixup */

  if (snmpQosAclEntryGet(USMDB_UNIT_CURRENT, tempData, -1) != L7_SUCCESS)
  {
    if (!VALID(I_aclStatus, data->valid) ||
        (data->aclStatus != D_aclStatus_createAndGo &&
         data->aclStatus != D_aclStatus_createAndWait) ||
        usmDbQosAclCreate(USMDB_UNIT_CURRENT, data->aclIndex) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return NO_CREATION_ERROR;
    }
    else
    {
      isCreatedNew = L7_TRUE;
      SET_VALID(I_aclStatus, tempValid);
    }
  }

  /* Code for checking if dependent instances of aclIfEntry or aclRuleEntry exist */
  if(data->aclStatus == D_aclStatus_destroy)
  {
    /* check to see if this IP ACL has associated entries in other mibs */
    if (snmpIsQosAclIdAttachedToInterface(USMDB_UNIT_CURRENT, data->aclIndex) == L7_TRUE)
    {
      /* deletion is not allowed if related instance of aclIfEntry exists */
      return COMMIT_FAILED_ERROR;
    }

    /* check to see if this MAC ACL has associated entries in other mibs */
    if (snmpIsQosAclIdAttachedToVlan(USMDB_UNIT_CURRENT, data->aclIndex) == L7_TRUE)
    {
      /* deletion is not allowed if related instance of aclIfEntry exists */
      return COMMIT_FAILED_ERROR;
    }

    memset(&temp_aclRuleEntryData, 0, sizeof(temp_aclRuleEntryData));
    temp_aclRuleEntryData.aclIndex = data->aclIndex;
    temp_aclRuleEntryData.aclRuleIndex = 0;
    if ((snmpQosAclRuleEntryGetNext(USMDB_UNIT_CURRENT, &temp_aclRuleEntryData, -1) == L7_SUCCESS) &&
        (temp_aclRuleEntryData.aclIndex == data->aclIndex))
    {
      /* deletion is not allowed if related instance of aclRuleEntry exists */
      return COMMIT_FAILED_ERROR;
    }
  }

  if (VALID(I_aclStatus, data->valid) && (isCreatedNew != L7_TRUE) &&
      snmpQosAclStatusSet(USMDB_UNIT_CURRENT, data->aclIndex,
                       data->aclStatus) != L7_SUCCESS)
  {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_aclName, data->valid))
  {
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->aclName->octet_ptr, data->aclName->length);

    if ((usmDbQosAclNameStringCheck(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS) ||
        ((usmDbQosAclNameToIndex(USMDB_UNIT_CURRENT, snmp_buffer, &temp_aclIndex) == L7_SUCCESS) && (temp_aclIndex != data->aclIndex)) ||
        (usmDbQosAclNameAdd(USMDB_UNIT_CURRENT, data->aclIndex, snmp_buffer) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }

  }

  return NO_ERROR;
}

  #ifdef SR_aclEntry_UNDO
/* add #define SR_aclEntry_UNDO in sitedefs.h to
 * include the undo routine for the aclEntry family.
 */
int
aclEntry_undo(doList_t *doHead, doList_t *doCur,
              ContextInfo *contextInfo)
{
  aclEntry_t *data = (aclEntry_t *) doCur->data;
  aclEntry_t *undodata = (aclEntry_t *) doCur->undodata;
  aclEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  if( aclIfEntry_count > 0 || aclRuleEntry_count > 0 )
  {
    aclIfEntry_count =0;
    aclRuleEntry_count =0;
    return NO_ERROR;
  }

  /*Copy valid bits from data to undodata */
  if ( undodata != NULL && (data->aclStatus != D_aclStatus_destroy))
      memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
    /* ignore if deleting a non-existant entry */
    if (data->aclStatus == D_aclStatus_destroy)
      return NO_ERROR;

      /* undoing an add, so delete */
      data->aclStatus = D_aclStatus_destroy;
      setdata = data;
      function = SR_DELETE;
  }
  else
  {
      /* undoing a delete or modify, replace the original data */
      if((undodata->aclStatus == D_aclStatus_notReady)
         || (undodata->aclStatus == D_aclStatus_notInService))
      {
          undodata->aclStatus = D_aclStatus_createAndWait;
      }
      else
      {
         if(undodata->aclStatus == D_aclStatus_active)
           undodata->aclStatus = D_aclStatus_createAndGo;
      }
      setdata = undodata;
      function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_aclEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_aclEntry_UNDO */

#endif /* SETS */

aclIfEntry_t *
k_aclIfEntry_get(int serialNum, ContextInfo *contextInfo,
               int nominator,
               int searchType,
               SR_INT32 aclIfIndex,
               SR_INT32 aclIfDirection,
               SR_UINT32 aclIfSequence,
               SR_INT32 aclIfAclType,
               SR_INT32 aclIfAclId)
{
  static aclIfEntry_t aclIfEntryData;
  L7_uint32 intIfNum;

  ZERO_VALID(aclIfEntryData.valid);
  
  aclIfEntryData.aclIfIndex = aclIfIndex;
  SET_VALID(I_aclIfIndex, aclIfEntryData.valid);

  aclIfEntryData.aclIfDirection = aclIfDirection;
  SET_VALID(I_aclIfDirection, aclIfEntryData.valid);

  aclIfEntryData.aclIfSequence = aclIfSequence;
  SET_VALID(I_aclIfSequence, aclIfEntryData.valid);
  
  aclIfEntryData.aclIfAclType = aclIfAclType;
  SET_VALID(I_aclIfAclType, aclIfEntryData.valid);

  aclIfEntryData.aclIfAclId = aclIfAclId;
  SET_VALID(I_aclIfAclId, aclIfEntryData.valid);

  if ( (searchType == EXACT) ?
       ( (usmDbIntIfNumFromExtIfNum(aclIfEntryData.aclIfIndex, &intIfNum) != L7_SUCCESS) ||
         (snmpQosAclIntfCheckValid(USMDB_UNIT_CURRENT,
                                   intIfNum,
                                   aclIfEntryData.aclIfDirection,
                                   aclIfEntryData.aclIfSequence,
                                   aclIfEntryData.aclIfAclType,
                                   aclIfEntryData.aclIfAclId) != L7_SUCCESS) ) :
       ( ( (usmDbVisibleExtIfNumberCheck( USMDB_UNIT_CURRENT, aclIfEntryData.aclIfIndex) != L7_SUCCESS) &&
           (usmDbGetNextVisibleExtIfNumber(aclIfEntryData.aclIfIndex, &aclIfEntryData.aclIfIndex) != L7_SUCCESS) ) ||
         (usmDbIntIfNumFromExtIfNum(aclIfEntryData.aclIfIndex, &intIfNum) != L7_SUCCESS) ||
         ( (snmpQosAclIntfCheckValid(USMDB_UNIT_CURRENT,
                                   intIfNum,
                                   aclIfEntryData.aclIfDirection,
                                   aclIfEntryData.aclIfSequence,
                                   aclIfEntryData.aclIfAclType,
                                   aclIfEntryData.aclIfAclId) != L7_SUCCESS) &&
           (snmpQosAclIntfNext(USMDB_UNIT_CURRENT,
                               &intIfNum,
                               &aclIfEntryData.aclIfDirection,
                               &aclIfEntryData.aclIfSequence,
                               &aclIfEntryData.aclIfAclType,
                               &aclIfEntryData.aclIfAclId) != L7_SUCCESS)) ||
         (usmDbExtIfNumFromIntIfNum(intIfNum, &aclIfEntryData.aclIfIndex) != L7_SUCCESS) ) )
  {
    ZERO_VALID(aclIfEntryData.valid);
    return(NULL);
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statemen
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
    case -1:
    case I_aclIfIndex:
    case I_aclIfDirection:
    case I_aclIfSequence:
    case I_aclIfAclType:
    case I_aclIfAclId:
      if (nominator != -1 ) break;
      /* else pass through */

  case I_aclIfStatus:
      aclIfEntryData.aclIfStatus = D_aclIfStatus_active;
      SET_VALID(I_aclIfStatus, aclIfEntryData.valid);
      break;

    default:
      return(NULL);
      break;
  }

  if (nominator >= 0 && !VALID(nominator, aclIfEntryData.valid))
    return(NULL);

  return(&aclIfEntryData);
}

#ifdef SETS
int
k_aclIfEntry_test(ObjectInfo *object, ObjectSyntax *value,
                doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_aclIfEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                 doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_aclIfEntry_set_defaults(doList_t *dp)
{
  aclIfEntry_t *data = (aclIfEntry_t *) (dp->data);

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_aclIfEntry_set(aclIfEntry_t *data,
               ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;
  L7_BOOL isCreatedNew = L7_FALSE;

  /*
   * Defining temporary variable for storing the valid bits for the case when th
   * set request is only paritally successful
  */
  
  
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbIntIfNumFromExtIfNum(data->aclIfIndex, &intIfNum) != L7_SUCCESS)
  {
    return COMMIT_FAILED_ERROR;
  }
    

  if (snmpQosAclIntfCheckValid(USMDB_UNIT_CURRENT,
                                   intIfNum,
                                   data->aclIfDirection,
                                   data->aclIfSequence,
                                   data->aclIfAclType,
                                   data->aclIfAclId) != L7_SUCCESS)
  {
    if (!VALID(I_aclIfStatus, data->valid) ||
        (data->aclIfStatus != D_aclIfStatus_createAndGo &&
        data->aclIfStatus != D_aclIfStatus_createAndWait) ||
        snmpQosAclIntfAdd(USMDB_UNIT_CURRENT, 
                          intIfNum,
                          data->aclIfDirection,
                          data->aclIfSequence,
                          data->aclIfAclType,
                          data->aclIfAclId) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      isCreatedNew = L7_TRUE;
      SET_VALID(I_aclIfStatus, tempValid);
    }
  }


  if (VALID(I_aclIfStatus, data->valid) && (isCreatedNew != L7_TRUE) &&
      snmpQosAclIntfStatusSet(USMDB_UNIT_CURRENT, 
                              intIfNum,
                              data->aclIfDirection,
                              data->aclIfSequence,
                              data->aclIfAclType,
                              data->aclIfAclId,
                              data->aclIfStatus) != L7_SUCCESS)
  {
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return COMMIT_FAILED_ERROR;
  }
  
  return NO_ERROR;
}

  #ifdef SR_aclIfEntry_UNDO
/* add #define SR_aclIfEntry_UNDO in sitedefs.h to
 * include the undo routine for the aclIfEntry family.
 */
int
aclIfEntry_undo(doList_t *doHead, doList_t *doCur,
              ContextInfo *contextInfo)
{

  aclIfEntry_t *data = (aclIfEntry_t *) doCur->data;
  aclIfEntry_t *undodata = (aclIfEntry_t *) doCur->undodata;
  aclIfEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*Copy valid bits from data to undodata */
  if( undodata != NULL && (data->aclIfStatus != D_aclIfStatus_destroy ))
      memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
    /* ignore if deleting a non-existant entry */
    if (data->aclIfStatus == D_aclIfStatus_destroy)
      return NO_ERROR;

      /* undoing an add, so delete */
      data->aclIfStatus = D_aclIfStatus_destroy;
      setdata = data;
      function = SR_DELETE;
  }
  else
  {
      /* undoing a delete or modify, replace the original data */
      if((undodata->aclIfStatus == D_aclIfStatus_notReady)
         || (undodata->aclIfStatus == D_aclIfStatus_notInService))
      {
          undodata->aclIfStatus = D_aclIfStatus_createAndWait;
      }
      else
      {
         if(undodata->aclIfStatus == D_aclIfStatus_active)
           undodata->aclIfStatus = D_aclIfStatus_createAndGo;
      }
      setdata = undodata;
      function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_aclIfEntry_set(setdata, contextInfo, function) == NO_ERROR))
      return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_aclIfEntry_UNDO */

#endif /* SETS */

aclRuleEntry_t *
k_aclRuleEntry_get(int serialNum, ContextInfo *contextInfo,
                   int nominator,
                   int searchType,
                   SR_INT32 aclIndex,
                   SR_INT32 aclRuleIndex)
{
  static aclRuleEntry_t aclRuleEntryData;
  
  ZERO_VALID(aclRuleEntryData.valid);
  aclRuleEntryData.aclIndex = aclIndex;
  aclRuleEntryData.aclRuleIndex = aclRuleIndex;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  SET_VALID(I_aclRuleEntryIndex_aclIndex, aclRuleEntryData.valid);
  SET_VALID(I_aclRuleIndex, aclRuleEntryData.valid);

  if (searchType == EXACT ?
      (snmpQosAclRuleEntryGet(USMDB_UNIT_CURRENT,
                              &aclRuleEntryData,
                              nominator) != L7_SUCCESS) :
      (snmpQosAclRuleEntryGet(USMDB_UNIT_CURRENT,
                              &aclRuleEntryData,
                              nominator) != L7_SUCCESS &&
       snmpQosAclRuleEntryGetNext(USMDB_UNIT_CURRENT,
                                  &aclRuleEntryData,
                                  nominator) != L7_SUCCESS))
  {
	  ZERO_VALID(aclRuleEntryData.valid);
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, aclRuleEntryData.valid))
    return(NULL);

  return(&aclRuleEntryData);
}

#ifdef SETS
int
k_aclRuleEntry_test(ObjectInfo *object, ObjectSyntax *value,
                    doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_aclRuleEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                     doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_aclRuleEntry_set_defaults(doList_t *dp)
{
  aclRuleEntry_t *data = (aclRuleEntry_t *) (dp->data);
  
  data->aclRuleAction = D_aclRuleAction_deny;

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_aclRuleEntry_set(aclRuleEntry_t *data,
                   ContextInfo *contextInfo, int function)
{
  L7_uint32 temp_uint1;
  L7_uint32 temp_uint2;
  L7_BOOL isCreatedNew = L7_FALSE;

  /* This variable is added to pass to snmpAclRuleEntryGet() function so that 
   * our set values in 'data' are not lost.
   */
  aclRuleEntry_t *tempData;

  /*
   * Defining temporary variable for storing the valid bits for the case when th
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  tempData = Clone_aclRuleEntry(data);
  tempData->aclIndex = data->aclIndex;          /* QSCAN: DELETE -- Temp fixup */
  tempData->aclRuleIndex = data->aclRuleIndex;  /* QSCAN: DELETE -- Temp fixup */

  if (snmpQosAclRuleEntryGet(USMDB_UNIT_CURRENT, tempData, -1) != L7_SUCCESS)
  {
    if (!VALID(I_aclRuleStatus, data->valid) ||
        (data->aclRuleStatus != D_aclRuleStatus_createAndGo) ||
        snmpQosAclRuleEntryAdd(USMDB_UNIT_CURRENT, data->aclIndex, data->aclRuleIndex) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return NO_CREATION_ERROR;
    }
    else
    {
      isCreatedNew = L7_TRUE;
      SET_VALID(I_aclRuleStatus, tempValid);
    }
  }

  if (VALID(I_aclRuleAction, data->valid))
  {
    if (snmpQosAclRuleActionAdd(USMDB_UNIT_CURRENT,
                              data->aclIndex,
                              data->aclRuleIndex,
                              data->aclRuleAction) != L7_SUCCESS)
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclRuleAction,tempValid);
    }
  }

  if (VALID(I_aclRuleProtocol, data->valid))
  {
    if  ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_RULE_MATCH_PROTOCOL_FEATURE_ID) != L7_TRUE) ||
        (usmDbQosAclRuleProtocolAdd(USMDB_UNIT_CURRENT,
                                  data->aclIndex,
                                  data->aclRuleIndex,
                                  data->aclRuleProtocol) != L7_SUCCESS))
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclRuleProtocol, tempValid);
    }
  }

  if (VALID(I_aclRuleSrcIpAddress, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID) != L7_TRUE) ||
       ((snmpQosAclRuleSrcIpMaskGet(USMDB_UNIT_CURRENT,
                                   data->aclIndex,
                                   data->aclRuleIndex,
                                   &temp_uint1,
                                   &temp_uint2) != L7_SUCCESS) ||
       (snmpQosAclRuleSrcIpMaskAdd(USMDB_UNIT_CURRENT,
                                   data->aclIndex,
                                   data->aclRuleIndex,
                                   data->aclRuleSrcIpAddress,
                                   temp_uint2) != L7_SUCCESS)))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclRuleSrcIpAddress,tempValid);
    }
  }

  if (VALID(I_aclRuleSrcIpMask, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID) != L7_TRUE) ||
        (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) != L7_TRUE) ||
       ((snmpQosAclRuleSrcIpMaskGet(USMDB_UNIT_CURRENT,
                                    data->aclIndex,
                                    data->aclRuleIndex,
                                    &temp_uint1,
                                    &temp_uint2) != L7_SUCCESS) ||
        (snmpQosAclRuleSrcIpMaskAdd(USMDB_UNIT_CURRENT,
                                    data->aclIndex,
                                    data->aclRuleIndex,
                                    temp_uint1,
                                    data->aclRuleSrcIpMask) != L7_SUCCESS)))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else

    {
      SET_VALID(I_aclRuleSrcIpMask,tempValid);
    }
  }

  if (VALID(I_aclRuleSrcL4Port, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_RULE_MATCH_SRCL4PORT_FEATURE_ID) != L7_TRUE) ||
       (usmDbQosAclRuleSrcL4PortAdd(USMDB_UNIT_CURRENT,
                                   data->aclIndex,
                                   data->aclRuleIndex,
                                   data->aclRuleSrcL4Port) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclRuleSrcL4Port, tempValid);
    }
  }

  if (VALID(I_aclRuleSrcL4PortRangeStart, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_RULE_MATCH_SRCL4PORT_RANGE_FEATURE_ID) != L7_TRUE) ||
       ((usmDbQosAclRuleSrcL4PortRangeGet(USMDB_UNIT_CURRENT,
                                         data->aclIndex,
                                         data->aclRuleIndex,
                                         &temp_uint1,
                                         &temp_uint2) != L7_SUCCESS) ||
       (usmDbQosAclRuleSrcL4PortRangeAdd(USMDB_UNIT_CURRENT,
                                         data->aclIndex,
                                         data->aclRuleIndex,
                                         data->aclRuleSrcL4PortRangeStart,
                                         temp_uint2) != L7_SUCCESS)))
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclRuleSrcL4PortRangeStart,tempValid);
    }
  }

  if (VALID(I_aclRuleSrcL4PortRangeEnd, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_RULE_MATCH_SRCL4PORT_RANGE_FEATURE_ID) != L7_TRUE) ||
       ((usmDbQosAclRuleSrcL4PortRangeGet(USMDB_UNIT_CURRENT,
                                         data->aclIndex,
                                         data->aclRuleIndex,
                                         &temp_uint1,
                                         &temp_uint2) != L7_SUCCESS) ||
       (usmDbQosAclRuleSrcL4PortRangeAdd(USMDB_UNIT_CURRENT,
                                         data->aclIndex,
                                         data->aclRuleIndex,
                                         temp_uint1,
                                         data->aclRuleSrcL4PortRangeEnd) != L7_SUCCESS)))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclRuleSrcL4PortRangeEnd,tempValid);
    }
  }

    if (VALID(I_aclRuleDestIpAddress, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID) != L7_TRUE) ||
        (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) != L7_TRUE) ||
       ((snmpQosAclRuleDstIpMaskGet(USMDB_UNIT_CURRENT,
                                    data->aclIndex,
                                    data->aclRuleIndex,
                                    &temp_uint1,
                                    &temp_uint2) != L7_SUCCESS) ||
       ( snmpQosAclRuleDstIpMaskAdd(USMDB_UNIT_CURRENT,
                                    data->aclIndex,
                                    data->aclRuleIndex,
                                    data->aclRuleDestIpAddress,
                                    temp_uint2) != L7_SUCCESS)))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclRuleDestIpAddress, tempValid);
    }
  }

  if (VALID(I_aclRuleDestIpMask, data->valid))
  {
    if  ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID) != L7_TRUE) ||
       ((snmpQosAclRuleDstIpMaskGet(USMDB_UNIT_CURRENT,
                                    data->aclIndex,
                                    data->aclRuleIndex,
                                    &temp_uint1,
                                    &temp_uint2) != L7_SUCCESS) ||
       (snmpQosAclRuleDstIpMaskAdd(USMDB_UNIT_CURRENT,
                                    data->aclIndex,
                                    data->aclRuleIndex,
                                    temp_uint1,
                                    data->aclRuleDestIpMask) != L7_SUCCESS)))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclRuleDestIpMask, tempValid);
    }
  }

  if (VALID(I_aclRuleDestL4Port, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_RULE_MATCH_DSTL4PORT_FEATURE_ID) != L7_TRUE) ||
       (usmDbQosAclRuleDstL4PortAdd(USMDB_UNIT_CURRENT,
                                   data->aclIndex,
                                   data->aclRuleIndex,
                                   data->aclRuleDestL4Port) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclRuleDestL4Port, tempValid);
    }
  }

  if (VALID(I_aclRuleDestL4PortRangeStart, data->valid))
  {
     if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_RULE_MATCH_DSTL4PORT_RANGE_FEATURE_ID) != L7_TRUE) ||
        (usmDbQosAclRuleDstL4PortRangeGet(USMDB_UNIT_CURRENT,
                                         data->aclIndex,
                                         data->aclRuleIndex,
                                         &temp_uint1,
                                         &temp_uint2) != L7_SUCCESS) ||
        ( usmDbQosAclRuleDstL4PortRangeAdd(USMDB_UNIT_CURRENT,
                                         data->aclIndex,
                                         data->aclRuleIndex,
                                         data->aclRuleDestL4PortRangeStart,
                                         temp_uint2) != L7_SUCCESS))
     {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
     }
     else
     {
       SET_VALID(I_aclRuleDestL4PortRangeStart, tempValid);
     }
  }

  if (VALID(I_aclRuleDestL4PortRangeEnd, data->valid))
  {
    if  ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_RULE_MATCH_DSTL4PORT_RANGE_FEATURE_ID) != L7_TRUE) ||
       (usmDbQosAclRuleDstL4PortRangeGet(USMDB_UNIT_CURRENT,
                                         data->aclIndex,
                                         data->aclRuleIndex,
                                         &temp_uint1,
                                         &temp_uint2) != L7_SUCCESS) ||
        (usmDbQosAclRuleDstL4PortRangeAdd(USMDB_UNIT_CURRENT,
                                         data->aclIndex,
                                         data->aclRuleIndex,
                                         temp_uint1,
                                         data->aclRuleDestL4PortRangeEnd) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclRuleDestL4PortRangeEnd, tempValid);
    }
  }

  if (VALID(I_aclRuleIPDSCP, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_RULE_MATCH_IPDSCP_FEATURE_ID) != L7_TRUE) ||
       (usmDbQosAclRuleIPDscpAdd(USMDB_UNIT_CURRENT,
                                data->aclIndex,
                                data->aclRuleIndex,
                                data->aclRuleIPDSCP) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclRuleIPDSCP, tempValid);
    }
  }

  if (VALID(I_aclRuleIpPrecedence, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_RULE_MATCH_IPPRECEDENCE_FEATURE_ID) != L7_TRUE) ||
       (usmDbQosAclRuleIPPrecedenceAdd(USMDB_UNIT_CURRENT,
                                      data->aclIndex,
                                      data->aclRuleIndex,
                                      data->aclRuleIpPrecedence) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclRuleIpPrecedence, tempValid);
    }
  }

  if (VALID(I_aclRuleIpTosBits, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_RULE_MATCH_IPTOS_FEATURE_ID) != L7_TRUE) ||
       ((usmDbQosAclRuleIPTosGet(USMDB_UNIT_CURRENT,
                                data->aclIndex,
                                data->aclRuleIndex,
                                &temp_uint1,
                                &temp_uint2) != L7_SUCCESS) ||
       (usmDbQosAclRuleIPTosAdd(USMDB_UNIT_CURRENT,
                                data->aclIndex,
                                data->aclRuleIndex,
                                data->aclRuleIpTosBits,
                                temp_uint2) != L7_SUCCESS)))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclRuleIpTosBits, tempValid);
    }
  }

  if (VALID(I_aclRuleIpTosMask, data->valid))
  {
    if  ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_RULE_MATCH_IPTOS_FEATURE_ID) != L7_TRUE) ||
        ((usmDbQosAclRuleIPTosGet(USMDB_UNIT_CURRENT,
                                data->aclIndex,
                                data->aclRuleIndex,
                                &temp_uint1,
                                &temp_uint2) != L7_SUCCESS) ||
        (usmDbQosAclRuleIPTosAdd(USMDB_UNIT_CURRENT,
                                data->aclIndex,
                                data->aclRuleIndex,
                                temp_uint1,
                                ~data->aclRuleIpTosMask & 0x000000FF) != L7_SUCCESS)))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclRuleIpTosMask, tempValid);
    }
  }

  if (VALID(I_aclRuleStatus, data->valid) &&
      (isCreatedNew != L7_TRUE) &&
      snmpQosAclRuleStatusSet(USMDB_UNIT_CURRENT,
                              data->aclIndex,
                              data->aclRuleIndex,
                              data->aclRuleStatus) != L7_SUCCESS)
  {
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_aclRuleAssignQueueId, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_ASSIGN_QUEUE_FEATURE_ID) != L7_TRUE) ||
       (usmDbQosAclRuleAssignQueueIdAdd(USMDB_UNIT_CURRENT,
                                      data->aclIndex,
                                      data->aclRuleIndex,
                                      data->aclRuleAssignQueueId) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclRuleAssignQueueId, tempValid);
    }
  }

  if (VALID(I_aclRuleRedirectIntf, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_REDIRECT_FEATURE_ID) != L7_TRUE) ||
       (snmpQosAclRuleRedirectIntfAdd(USMDB_UNIT_CURRENT,
                                      data->aclIndex,
                                      data->aclRuleIndex,
                                      data->aclRuleRedirectIntf) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclRuleRedirectIntf, tempValid);
    }
  }

  if (VALID(I_aclRuleMatchEvery, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                              L7_ACL_RULE_MATCH_EVERY_FEATURE_ID) != L7_TRUE) ||
       (snmpQosAclRuleEveryAdd(USMDB_UNIT_CURRENT,
                                    data->aclIndex,
                                    data->aclRuleIndex,
                                    data->aclRuleMatchEvery) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclRuleMatchEvery, tempValid);
    }
  }

  if (VALID(I_aclRuleMirrorIntf, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_MIRROR_FEATURE_ID) != L7_TRUE) ||
       (snmpQosAclRuleMirrorIntfAdd(USMDB_UNIT_CURRENT,
                                    data->aclIndex,
                                    data->aclRuleIndex,
                                    data->aclRuleMirrorIntf) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclRuleMirrorIntf, tempValid);
    }
  }

  if (VALID(I_aclRuleLogging, data->valid))
  {
    if ((usmDbQosAclRuleLoggingAllowed(USMDB_UNIT_CURRENT, data->aclIndex, 
                                       data->aclRuleIndex) != L7_SUCCESS) ||
        (snmpQosAclRuleLoggingAdd(USMDB_UNIT_CURRENT,
                                  data->aclIndex,
                                  data->aclRuleIndex,
                                  data->aclRuleLogging) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclRuleLogging, tempValid);
    }
  }

  return NO_ERROR;
}

  #ifdef SR_aclRuleEntry_UNDO
/* add #define SR_aclRuleEntry_UNDO in sitedefs.h to
 * include the undo routine for the aclRuleEntry family.
 */
int
aclRuleEntry_undo(doList_t *doHead, doList_t *doCur,
                  ContextInfo *contextInfo)
{
  aclRuleEntry_t *data = (aclRuleEntry_t *) doCur->data;
  aclRuleEntry_t *undodata = (aclRuleEntry_t *) doCur->undodata;
  aclRuleEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*Copy valid bits from data to undodata */
  if( undodata != NULL  && (data->aclRuleStatus != D_aclRuleStatus_destroy))
      memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
    /* ignore if deleting a non-existant entry */
    if (data->aclRuleStatus == D_aclRuleStatus_destroy)
      return NO_ERROR;

      /* undoing an add, so delete */
      data->aclRuleStatus = D_aclRuleStatus_destroy;
      setdata = data;
      function = SR_DELETE;
  }
  else
  {
      /* undoing a delete or modify, replace the original data */
      if((undodata->aclRuleStatus == D_aclRuleStatus_notReady)
         || (undodata->aclRuleStatus == D_aclRuleStatus_notInService))
      {
          undodata->aclRuleStatus = D_aclRuleStatus_createAndWait;
      }
      else
      {
         if(undodata->aclRuleStatus == D_aclRuleStatus_active)
           undodata->aclRuleStatus = D_aclRuleStatus_createAndGo;
      }

      setdata = undodata;
      function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_aclRuleEntry_set(setdata, contextInfo, function) == NO_ERROR))
      return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_aclRuleEntry_UNDO */

#endif /* SETS */

fastPathQOSACL_t *
k_fastPathQOSACL_get(int serialNum, ContextInfo *contextInfo,
                     int nominator)
{
  static fastPathQOSACL_t fastPathQOSACLData;

  ZERO_VALID(fastPathQOSACLData.valid);
   
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_TYPE_MAC_FEATURE_ID) != L7_TRUE)
  {
    return(NULL);
  }

  switch (nominator)
  {
    case -1:
      break;

    case I_aclMacIndexNextFree:
      if (usmDbQosAclMacIndexNext(USMDB_UNIT_CURRENT,
                                  &fastPathQOSACLData.aclMacIndexNextFree)== L7_SUCCESS)
        SET_VALID(I_aclMacIndexNextFree, fastPathQOSACLData.valid);
      break;
    case I_aclIpv6IndexNextFree:
      if (usmDbQosAclNamedIndexNextFree(USMDB_UNIT_CURRENT, L7_ACL_TYPE_IPV6,
                                  &fastPathQOSACLData.aclIpv6IndexNextFree)== L7_SUCCESS)
        SET_VALID(I_aclIpv6IndexNextFree, fastPathQOSACLData.valid);
      break;  
  case I_aclNamedIpv4IndexNextFree:
    if (usmDbQosAclNamedIndexNextFree(USMDB_UNIT_CURRENT, L7_ACL_TYPE_IP,
                                &fastPathQOSACLData.aclNamedIpv4IndexNextFree)== L7_SUCCESS)
      SET_VALID(I_aclNamedIpv4IndexNextFree, fastPathQOSACLData.valid);
    break;  
    default:
     /* unknown nominator */
       return(NULL);
       break;
   }

  if (nominator >= 0 && !VALID(nominator, fastPathQOSACLData.valid))
    return(NULL);

  return(&fastPathQOSACLData);
}

aclMacEntry_t *
k_aclMacEntry_get(int serialNum, ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_INT32 aclMacIndex)
{
  static aclMacEntry_t aclMacEntryData;
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    aclMacEntryData.aclMacName = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(aclMacEntryData.valid);
  aclMacEntryData.aclMacIndex = aclMacIndex;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_TYPE_MAC_FEATURE_ID) != L7_TRUE)
  {
    return(NULL);
  }

  SET_VALID(I_aclMacIndex, aclMacEntryData.valid);

  if ( (searchType == EXACT) ?
       (snmpQosAclMacEntryGet(USMDB_UNIT_CURRENT, &aclMacEntryData, nominator) != L7_SUCCESS) :
       ((snmpQosAclMacEntryGet(USMDB_UNIT_CURRENT, &aclMacEntryData, nominator) != L7_SUCCESS) &&
        (snmpQosAclMacEntryGetNext(USMDB_UNIT_CURRENT, &aclMacEntryData, nominator) != L7_SUCCESS)) )
  {
    ZERO_VALID(aclMacEntryData.valid);
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, aclMacEntryData.valid))
    return(NULL);

  return(&aclMacEntryData);
}

#ifdef SETS
int
k_aclMacEntry_test(ObjectInfo *object, ObjectSyntax *value,
                   doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_aclMacEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_aclMacEntry_set_defaults(doList_t *dp)
{
  aclMacEntry_t *data = (aclMacEntry_t *) (dp->data);

  if ((data->aclMacName = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_aclMacEntry_set(aclMacEntry_t *data,
                  ContextInfo *contextInfo, int function)
{

  L7_BOOL isCreatedNew = L7_FALSE;
  L7_uint32 temp_aclMacIndex = 0;
  L7_uint32 temp_aclRuleIndex = 0;
  L7_uint32 temp_aclIndex = 0;
  char snmp_buffer[SNMP_BUFFER_LEN];

  /* This variable is added to pass to snmpQosAclMacRuleEntryGet() function 
   * so that our set values in 'data' are not lost.
   */
  aclMacEntry_t *tempData;

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  tempData = Clone_aclMacEntry(data);
  tempData->aclMacIndex = data->aclMacIndex;          /* QSCAN: DELETE -- Temp fixup */

  if (snmpQosAclMacEntryGet(USMDB_UNIT_CURRENT, tempData, -1) != L7_SUCCESS)
  {
    if (!VALID(I_aclMacStatus, data->valid) ||
        (data->aclMacStatus != D_aclStatus_createAndGo &&
         data->aclMacStatus != D_aclStatus_createAndWait) ||
        usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_TYPE_MAC_FEATURE_ID) != L7_TRUE ||
        usmDbQosAclMacCreate(USMDB_UNIT_CURRENT, data->aclMacIndex) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return NO_CREATION_ERROR;
    }
    else
    {
      isCreatedNew = L7_TRUE;
      SET_VALID(I_aclMacStatus, tempValid);
    }
  }

  /* Code for checking if dependent instances of aclMacRuleEntry exist */
  if(data->aclMacStatus == D_aclMacStatus_destroy)
  {
    
    /* check to see if this MAC ACL has associated entries in other mibs */
    if (snmpIsQosAclIdAttachedToInterface(USMDB_UNIT_CURRENT, data->aclMacIndex) == L7_TRUE)
    {
      /* deletion is not allowed if related instance of aclIfEntry exists */
      return COMMIT_FAILED_ERROR;
    }

    /* check to see if this MAC ACL has associated entries in other mibs */
    if (snmpIsQosAclIdAttachedToVlan(USMDB_UNIT_CURRENT, data->aclMacIndex) == L7_TRUE)
    {
      /* deletion is not allowed if related instance of aclIfEntry exists */
      return COMMIT_FAILED_ERROR;
    }

    temp_aclMacIndex = data->aclMacIndex;
    temp_aclRuleIndex = 0;
  if (usmDbQosAclMacRuleGetNext(USMDB_UNIT_CURRENT, temp_aclMacIndex, 0,&temp_aclRuleIndex) == L7_SUCCESS)
    {
      /* deletion is not allowed if related instance of aclMacRuleEntry exists */
      return COMMIT_FAILED_ERROR;
    }
  }

  if (VALID(I_aclMacName, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->aclMacName->octet_ptr, data->aclMacName->length);
    if ((usmDbQosAclMacNameStringCheck(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS) ||
        ((usmDbQosAclMacNameToIndex(USMDB_UNIT_CURRENT, snmp_buffer, &temp_aclIndex) == L7_SUCCESS) &&
         (temp_aclIndex != data->aclMacIndex)) ||
        (usmDbQosAclMacNameAdd(USMDB_UNIT_CURRENT, data->aclMacIndex,
                               snmp_buffer) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }

  }

  if (VALID(I_aclMacStatus, data->valid) && (isCreatedNew != L7_TRUE) &&
      snmpQosMacAclStatusSet(USMDB_UNIT_CURRENT, data->aclMacIndex,
                       data->aclMacStatus) != L7_SUCCESS)
  {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
  }

  return NO_ERROR;
}

  #ifdef SR_aclMacEntry_UNDO
/* add #define SR_aclMacEntry_UNDO in sitedefs.h to
 * include the undo routine for the aclMacEntry family.
 */
int
aclMacEntry_undo(doList_t *doHead, doList_t *doCur,
                 ContextInfo *contextInfo)
{
  aclMacEntry_t *data = (aclMacEntry_t *) doCur->data;
  aclMacEntry_t *undodata = (aclMacEntry_t *) doCur->undodata;
  aclMacEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
  if( aclIfEntry_count > 0 || aclRuleEntry_count > 0 )
  {
    aclIfEntry_count =0;
    aclRuleEntry_count =0;
    return NO_ERROR;
  }
  */

  /*Copy valid bits from data to undodata */
  if ( undodata != NULL && (data->aclMacStatus != D_aclMacStatus_destroy))
      memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
    /* ignore if deleting a non-existant entry */
    if (data->aclMacStatus == D_aclMacStatus_destroy)
      return NO_ERROR;

      /* undoin g an add, so delete */
      data->aclMacStatus = D_aclMacStatus_destroy;
      setdata = data;
      function = SR_DELETE;
  }
  else
  {
      /* undoing a delete or modify, replace the original data */
      if((undodata->aclMacStatus == D_aclMacStatus_notReady)
         || (undodata->aclMacStatus == D_aclMacStatus_notInService))
      {
          undodata->aclMacStatus = D_aclMacStatus_createAndWait;
      }
      else
      {
         if(undodata->aclMacStatus == D_aclMacStatus_active)
           undodata->aclMacStatus = D_aclMacStatus_createAndGo;
      }
      setdata = undodata;
      function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_aclMacEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_aclMacEntry_UNDO */

#endif /* SETS */

aclMacRuleEntry_t *
k_aclMacRuleEntry_get(int serialNum, ContextInfo *contextInfo,
                      int nominator,
                      int searchType,
                      SR_INT32 aclMacIndex,
                      SR_INT32 aclMacRuleIndex)
{
  static aclMacRuleEntry_t aclMacRuleEntryData;

  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    aclMacRuleEntryData.aclMacRuleDestMacAddr = MakeOctetString(NULL, 0);
    aclMacRuleEntryData.aclMacRuleDestMacMask = MakeOctetString(NULL, 0);
    aclMacRuleEntryData.aclMacRuleSrcMacAddr = MakeOctetString(NULL, 0);
    aclMacRuleEntryData.aclMacRuleSrcMacMask = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(aclMacRuleEntryData.valid);
  aclMacRuleEntryData.aclMacIndex = aclMacIndex;
  aclMacRuleEntryData.aclMacRuleIndex = aclMacRuleIndex;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_TYPE_MAC_FEATURE_ID) != L7_TRUE)
  {
    return(NULL);
  }

  SET_VALID(I_aclMacRuleEntryIndex_aclMacIndex, aclMacRuleEntryData.valid);
  SET_VALID(I_aclMacRuleIndex, aclMacRuleEntryData.valid);

  if ( (searchType == EXACT) ?
       (snmpQosAclMacRuleEntryGet(USMDB_UNIT_CURRENT,
                                  &aclMacRuleEntryData,
                                  nominator) != L7_SUCCESS) :
       ((snmpQosAclMacRuleEntryGet(USMDB_UNIT_CURRENT,
                                   &aclMacRuleEntryData,
                                   nominator) != L7_SUCCESS) &&
        (snmpQosAclMacRuleEntryGetNext(USMDB_UNIT_CURRENT,
                                       &aclMacRuleEntryData,
                                       nominator) != L7_SUCCESS)) )
    
  {
    ZERO_VALID(aclMacRuleEntryData.valid);
    return(NULL);
  }
      
  if (nominator >= 0 && !VALID(nominator, aclMacRuleEntryData.valid))
    return(NULL);

  return(&aclMacRuleEntryData);
}

#ifdef SETS
int
k_aclMacRuleEntry_test(ObjectInfo *object, ObjectSyntax *value,
                       doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_aclMacRuleEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                        doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_aclMacRuleEntry_set_defaults(doList_t *dp)
{
  aclMacRuleEntry_t *data = (aclMacRuleEntry_t *) (dp->data);

  data->aclMacRuleAction = D_aclMacRuleAction_deny;
  if ((data->aclMacRuleDestMacAddr = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->aclMacRuleDestMacMask = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->aclMacRuleSrcMacAddr = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->aclMacRuleSrcMacMask = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_aclMacRuleEntry_set(aclMacRuleEntry_t *data,
                      ContextInfo *contextInfo, int function)
{
  L7_BOOL isCreatedNew = L7_FALSE;

  char snmp_buffer[SNMP_BUFFER_LEN];
  char snmp_buffer1[SNMP_BUFFER_LEN];
  char snmp_buffer2[SNMP_BUFFER_LEN];

   /* This variable is added to pass to snmpQosAclMacRuleEntryGet() function 
    * so that our set values in 'data' are not lost.
    */
   aclMacRuleEntry_t *tempData;

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  tempData = Clone_aclMacRuleEntry(data);
  tempData->aclMacIndex = data->aclMacIndex;          /* QSCAN: DELETE -- Temp fixup */
  tempData->aclMacRuleIndex = data->aclMacRuleIndex;  /* QSCAN: DELETE -- Temp fixup */

  if (snmpQosAclMacRuleEntryGet(USMDB_UNIT_CURRENT, tempData, -1) != L7_SUCCESS)
  {
    if (!VALID(I_aclMacRuleStatus, data->valid) ||
        (data->aclMacRuleStatus != D_aclMacRuleStatus_createAndGo) ||
        snmpQosAclMacRuleEntryAdd(USMDB_UNIT_CURRENT, data->aclMacIndex, data->aclMacRuleIndex) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return NO_CREATION_ERROR;
    }
    else
    {
      isCreatedNew = L7_TRUE;
      SET_VALID(I_aclMacRuleStatus, tempValid);
    }
  }
  
  if (VALID(I_aclMacRuleAction, data->valid))
  {
    if (snmpQosAclMacRuleActionAdd(USMDB_UNIT_CURRENT,
                                   data->aclMacIndex,
                                   data->aclMacRuleIndex,
                                   data->aclMacRuleAction) != L7_SUCCESS)
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclMacRuleAction,tempValid);
    }
  }

  if (VALID(I_aclMacRuleCos, data->valid))
  {
    if  ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_COS_FEATURE_ID) != L7_TRUE) ||
        (usmDbQosAclMacRuleCosAdd(USMDB_UNIT_CURRENT,
                                       data->aclMacIndex,
                                       data->aclMacRuleIndex,
                                       data->aclMacRuleCos) != L7_SUCCESS))
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclMacRuleCos, tempValid);
    }
  }

  if (VALID(I_aclMacRuleCos2, data->valid))
  {
    if  ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_COS2_FEATURE_ID) != L7_TRUE) ||
        (usmDbQosAclMacRuleCos2Add(USMDB_UNIT_CURRENT,
                                       data->aclMacIndex,
                                       data->aclMacRuleIndex,
                                       data->aclMacRuleCos2) != L7_SUCCESS))
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclMacRuleCos2, tempValid);
    }
  }

  if (VALID(I_aclMacRuleDestMacAddr, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    bzero(snmp_buffer1, SNMP_BUFFER_LEN);
    bzero(snmp_buffer2, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->aclMacRuleDestMacAddr->octet_ptr,
    data->aclMacRuleDestMacAddr->length);

    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_RULE_MATCH_DSTMAC_FEATURE_ID) != L7_TRUE) ||
       ((snmpQosAclMacRuleDstMacAddrMaskGet(USMDB_UNIT_CURRENT,
                                        data->aclMacIndex,
                                        data->aclMacRuleIndex,
                                        snmp_buffer1,
                                        snmp_buffer2) != L7_SUCCESS) ||
        (snmpQosAclMacRuleDstMacAdd(USMDB_UNIT_CURRENT,
                                        data->aclMacIndex,
                                        data->aclMacRuleIndex,
                                        snmp_buffer,
                                        snmp_buffer2) != L7_SUCCESS)))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclMacRuleDestMacAddr,tempValid);
    }
  }

  if (VALID(I_aclMacRuleDestMacMask, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    bzero(snmp_buffer1, SNMP_BUFFER_LEN);
    bzero(snmp_buffer2, SNMP_BUFFER_LEN);

    memcpy(snmp_buffer, data->aclMacRuleDestMacMask->octet_ptr,
    data->aclMacRuleDestMacMask->length);
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_RULE_MATCH_DSTMAC_MASK_FEATURE_ID) != L7_TRUE) ||
       ((snmpQosAclMacRuleDstMacAddrMaskGet(USMDB_UNIT_CURRENT,
                                        data->aclMacIndex,
                                        data->aclMacRuleIndex,
                                        snmp_buffer1,
                                        snmp_buffer2) != L7_SUCCESS) ||
        (snmpQosAclMacRuleDstMacAdd(USMDB_UNIT_CURRENT,
                                        data->aclMacIndex,
                                        data->aclMacRuleIndex,
                                        snmp_buffer1,
                                        snmp_buffer) != L7_SUCCESS)))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclMacRuleDestMacMask,tempValid);
    }
  }

  if (VALID(I_aclMacRuleEtypeKey, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_RULE_MATCH_ETYPE_FEATURE_ID) != L7_TRUE) ||
        (snmpQosAclMacRuleEtypeKeyAdd(USMDB_UNIT_CURRENT,
                                      data->aclMacIndex,
                                      data->aclMacRuleIndex,
                                      data->aclMacRuleEtypeKey) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclMacRuleEtypeKey,tempValid);
    }
  }

  if (VALID(I_aclMacRuleEtypeValue, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_RULE_MATCH_ETYPE_FEATURE_ID) != L7_TRUE) ||
        (snmpQosAclMacRuleEtypeValueAdd(USMDB_UNIT_CURRENT,
                                        data->aclMacIndex,
                                        data->aclMacRuleIndex,
                                        data->aclMacRuleEtypeValue) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclMacRuleEtypeValue,tempValid);
    }
  }

  if (VALID(I_aclMacRuleSrcMacAddr, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    bzero(snmp_buffer1, SNMP_BUFFER_LEN);
    bzero(snmp_buffer2, SNMP_BUFFER_LEN);

    memcpy(snmp_buffer, data->aclMacRuleSrcMacAddr->octet_ptr,
    data->aclMacRuleSrcMacAddr->length);

    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_RULE_MATCH_SRCMAC_FEATURE_ID) != L7_TRUE) ||
       ((snmpQosAclMacRuleSrcMacAddrMaskGet(USMDB_UNIT_CURRENT,
                                        data->aclMacIndex,
                                        data->aclMacRuleIndex,
                                        snmp_buffer1,
                                        snmp_buffer2) != L7_SUCCESS) ||
        (snmpQosAclMacRuleSrcMacAdd(USMDB_UNIT_CURRENT,
                                        data->aclMacIndex,
                                        data->aclMacRuleIndex,
                                        snmp_buffer,
                                        snmp_buffer2) != L7_SUCCESS)))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclMacRuleSrcMacAddr,tempValid);
    }
  }

  if (VALID(I_aclMacRuleSrcMacMask, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    bzero(snmp_buffer1, SNMP_BUFFER_LEN);
    bzero(snmp_buffer2, SNMP_BUFFER_LEN);

    memcpy(snmp_buffer, data->aclMacRuleSrcMacMask->octet_ptr,
    data->aclMacRuleSrcMacMask->length);

    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_RULE_MATCH_SRCMAC_MASK_FEATURE_ID) != L7_TRUE) ||
       ((snmpQosAclMacRuleSrcMacAddrMaskGet(USMDB_UNIT_CURRENT,
                                        data->aclMacIndex,
                                        data->aclMacRuleIndex,
                                        snmp_buffer1,
                                        snmp_buffer2) != L7_SUCCESS) ||
        (snmpQosAclMacRuleSrcMacAdd(USMDB_UNIT_CURRENT,
                                        data->aclMacIndex,
                                        data->aclMacRuleIndex,
                                        snmp_buffer1,
                                        snmp_buffer) != L7_SUCCESS)))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclMacRuleSrcMacMask,tempValid);
    }
  }

  if (VALID(I_aclMacRuleVlanId, data->valid))
  {
    if  ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID, 
                                   L7_ACL_RULE_MATCH_VLANID_FEATURE_ID) != L7_TRUE) ||
         (usmDbQosAclMacRuleVlanIdAdd(USMDB_UNIT_CURRENT,
                                      data->aclMacIndex,
                                      data->aclMacRuleIndex,
                                      data->aclMacRuleVlanId) != L7_SUCCESS))
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
    }
    else
   {
     SET_VALID(I_aclMacRuleVlanId, tempValid);
    }
  }

  if (VALID(I_aclMacRuleVlanIdRangeStart, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_RULE_MATCH_VLANID_RANGE_FEATURE_ID) != L7_TRUE) ||
        (snmpQosAclMacRuleVlanIdRangeStartAdd(USMDB_UNIT_CURRENT,
                                              data->aclMacIndex,
                                              data->aclMacRuleIndex,
                                              data->aclMacRuleVlanIdRangeStart) != L7_SUCCESS))
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclMacRuleVlanIdRangeStart,tempValid);
    }
  }

  if (VALID(I_aclMacRuleVlanIdRangeEnd, data->valid))
  {
    
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_RULE_MATCH_VLANID_RANGE_FEATURE_ID) != L7_TRUE) ||
        (snmpQosAclMacRuleVlanIdRangeEndAdd(USMDB_UNIT_CURRENT,
                                            data->aclMacIndex,
                                            data->aclMacRuleIndex,
                                            data->aclMacRuleVlanIdRangeEnd) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclMacRuleVlanIdRangeEnd,tempValid);
    }
  }

  if (VALID(I_aclMacRuleVlanId2, data->valid))
  {
    if  ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_VLANID2_FEATURE_ID) != L7_TRUE) ||
         (usmDbQosAclMacRuleVlanId2Add(USMDB_UNIT_CURRENT,
                                       data->aclMacIndex,
                                       data->aclMacRuleIndex,
                                       data->aclMacRuleVlanId2) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclMacRuleVlanId2, tempValid);
    }
  }

  if (VALID(I_aclMacRuleVlanId2RangeStart, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_VLANID2_RANGE_FEATURE_ID) != L7_TRUE) ||
        (snmpQosAclMacRuleVlanId2RangeStartAdd(USMDB_UNIT_CURRENT,
                                               data->aclMacIndex,
                                               data->aclMacRuleIndex,
                                               data->aclMacRuleVlanId2RangeStart) != L7_SUCCESS))
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclMacRuleVlanId2RangeStart,tempValid);
    }
  }

  if (VALID(I_aclMacRuleVlanId2RangeEnd, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_RULE_MATCH_VLANID2_RANGE_FEATURE_ID) != L7_TRUE) ||
        (snmpQosAclMacRuleVlanId2RangeEndAdd(USMDB_UNIT_CURRENT,
                                             data->aclMacIndex,
                                             data->aclMacRuleIndex,
                                             data->aclMacRuleVlanId2RangeEnd) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclMacRuleVlanId2RangeEnd,tempValid);
    }
  }


  if (VALID(I_aclMacRuleStatus, data->valid) &&
      (isCreatedNew != L7_TRUE) &&
      snmpQosAclMacRuleStatusSet(USMDB_UNIT_CURRENT,
                                 data->aclMacIndex,
                                 data->aclMacRuleIndex,
                                 data->aclMacRuleStatus) != L7_SUCCESS)
  {
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return COMMIT_FAILED_ERROR;
  }


  if (VALID(I_aclMacRuleAssignQueueId, data->valid))
  {
    if  ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_ASSIGN_QUEUE_FEATURE_ID) != L7_TRUE) ||
         (usmDbQosAclMacRuleAssignQueueIdAdd(USMDB_UNIT_CURRENT,
             data->aclMacIndex,
             data->aclMacRuleIndex,
             data->aclMacRuleAssignQueueId) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclMacRuleAssignQueueId, tempValid);
    }
  }


  if (VALID(I_aclMacRuleRedirectIntf, data->valid))
  {
    if  ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_REDIRECT_FEATURE_ID) != L7_TRUE) ||
         (snmpQosAclMacRuleRedirectIntfAdd(USMDB_UNIT_CURRENT,
                                     data->aclMacIndex,
                                     data->aclMacRuleIndex,
                                     data->aclMacRuleRedirectIntf) != L7_SUCCESS))
   {
     memcpy(data->valid, tempValid, sizeof(data->valid));
     return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclMacRuleRedirectIntf, tempValid);
    }
  }


  if (VALID(I_aclMacRuleMatchEvery, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                              L7_ACL_RULE_MATCH_EVERY_FEATURE_ID) != L7_TRUE) ||
       (snmpQosAclMacRuleEveryAdd(USMDB_UNIT_CURRENT,
                                    data->aclMacIndex,
                                    data->aclMacRuleIndex,
                                    data->aclMacRuleMatchEvery) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclMacRuleMatchEvery, tempValid);
    }
  }


  if (VALID(I_aclMacRuleMirrorIntf, data->valid))
  {
    if  ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_MIRROR_FEATURE_ID) != L7_TRUE) ||
         (snmpQosAclMacRuleMirrorIntfAdd(USMDB_UNIT_CURRENT,
                                         data->aclMacIndex,
                                         data->aclMacRuleIndex,
                                         data->aclMacRuleMirrorIntf) != L7_SUCCESS))
   {
     memcpy(data->valid, tempValid, sizeof(data->valid));
     return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclMacRuleMirrorIntf, tempValid);
    }
  }


  if (VALID(I_aclMacRuleLogging, data->valid))
  {
    if ((usmDbQosAclMacRuleLoggingAllowed(USMDB_UNIT_CURRENT, 
                                          data->aclMacIndex, 
                                          data->aclMacRuleIndex) != L7_SUCCESS) ||
        (snmpQosAclMacRuleLoggingAdd(USMDB_UNIT_CURRENT,
                                     data->aclMacIndex,
                                     data->aclMacRuleIndex,
                                     data->aclMacRuleLogging) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclMacRuleLogging, tempValid);
    }
  }

  return NO_ERROR;
}

  #ifdef SR_aclMacRuleEntry_UNDO
/* add #define SR_aclMacRuleEntry_UNDO in sitedefs.h to
 * include the undo routine for the aclMacRuleEntry family.
 */
int
aclMacRuleEntry_undo(doList_t *doHead, doList_t *doCur,
                     ContextInfo *contextInfo)
{
  aclMacRuleEntry_t *data = (aclMacRuleEntry_t *) doCur->data;
  aclMacRuleEntry_t *undodata = (aclMacRuleEntry_t *) doCur->undodata;
  aclMacRuleEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*Copy valid bits from data to undodata */
  if( undodata != NULL  && (data->aclMacRuleStatus != D_aclMacRuleStatus_destroy))
      memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
    /* ignore if deleting a non-existant entry */
    if (data->aclMacRuleStatus == D_aclMacRuleStatus_destroy)
      return NO_ERROR;

      /* undoing an add, so delete */
      data->aclMacRuleStatus = D_aclMacRuleStatus_destroy;
      setdata = data;
      function = SR_DELETE;
  }
  else
  {
      /* undoing a delete or modify, replace the original data */
      if((undodata->aclMacRuleStatus == D_aclMacRuleStatus_notReady)
         || (undodata->aclMacRuleStatus == D_aclMacRuleStatus_notInService))
      {
          undodata->aclMacRuleStatus = D_aclMacRuleStatus_createAndWait;
      }
      else
      {
         if(undodata->aclMacRuleStatus == D_aclMacRuleStatus_active)
            undodata->aclMacRuleStatus = D_aclMacRuleStatus_createAndGo;
      }

      setdata = undodata;
      function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) &&(k_aclMacRuleEntry_set(setdata, contextInfo, function) == NO_ERROR))
      return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_aclMacRuleEntry_UNDO */

#endif /* SETS */

aclLoggingGroup_t *
k_aclLoggingGroup_get(int serialNum, ContextInfo *contextInfo,
                      int nominator)
{
  static aclLoggingGroup_t aclLoggingGroupData;
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    aclLoggingGroupData.aclTrapRuleHitCount = MakeCounter64(0);
    firstTime = L7_FALSE;
  }

  ZERO_VALID(aclLoggingGroupData.valid);
   
  if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_TYPE_MAC_FEATURE_ID) != L7_TRUE) ||
      ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_LOG_DENY_FEATURE_ID) != L7_TRUE) &&
       (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_LOG_PERMIT_FEATURE_ID) != L7_TRUE)))
  {
    return(NULL);
  }

  switch (nominator)
  {
    case -1:
    case I_aclTrapRuleIndex:
    case I_aclTrapRuleAction:
    case I_aclTrapRuleHitCount:
      if (nominator != -1) break;
      /* else pass through */

    case I_aclTrapFlag:
      if (snmpQosAclTrapFlagGet(USMDB_UNIT_CURRENT, &aclLoggingGroupData.aclTrapFlag) == L7_SUCCESS)
        SET_VALID(I_aclTrapFlag, aclLoggingGroupData.valid);
      break;

    default:
     /* unknown nominator */
       return(NULL);
       break;
   }

  if (nominator >= 0 && !VALID(nominator, aclLoggingGroupData.valid))
    return(NULL);

  return(&aclLoggingGroupData);
}

#ifdef SETS
int
k_aclLoggingGroup_test(ObjectInfo *object, ObjectSyntax *value,
                       doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_aclLoggingGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                        doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_aclLoggingGroup_set_defaults(doList_t *dp)
{
  aclLoggingGroup_t *data = (aclLoggingGroup_t *) (dp->data);
  
  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_aclLoggingGroup_set(aclLoggingGroup_t *data,
                      ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_LOG_DENY_FEATURE_ID) != L7_TRUE) &&
      (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_LOG_PERMIT_FEATURE_ID) != L7_TRUE))
  {
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return COMMIT_FAILED_ERROR;
  }

  if ( VALID(I_aclTrapFlag, data->valid))
  {
    if(snmpQosAclTrapFlagSet(USMDB_UNIT_CURRENT, data->aclTrapFlag) != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
  }

  return NO_ERROR;
}

#ifdef SR_aclLoggingGroup_UNDO
/* add #define SR_aclLoggingGroup_UNDO in qos_sitedefs.h to
 * include the undo routine for the aclLoggingGroup family.
 */
int
aclLoggingGroup_undo(doList_t *doHead, doList_t *doCur,
                     ContextInfo *contextInfo)
{
  aclLoggingGroup_t *data = (aclLoggingGroup_t *) doCur->data;
  aclLoggingGroup_t *undodata = (aclLoggingGroup_t *) doCur->undodata;
  aclLoggingGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
   */
  if ( data->valid == NULL  || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if (k_aclLoggingGroup_set(setdata, contextInfo, function) == NO_ERROR) 
      return NO_ERROR;

   return UNDO_FAILED_ERROR;
}
#endif /* SR_aclLoggingGroup_UNDO */

#endif /* SETS */

/* IPv6 ACL changes start */
aclIpv6Entry_t *
k_aclIpv6Entry_get(int serialNum, ContextInfo *contextInfo,
                   int nominator,
                   int searchType,
                   SR_INT32 aclIpv6Index)
{
  static aclIpv6Entry_t aclIpv6EntryData;
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    aclIpv6EntryData.aclIpv6Name = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(aclIpv6EntryData.valid);
  aclIpv6EntryData.aclIpv6Index = aclIpv6Index;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_TYPE_IPV6_FEATURE_ID) != L7_TRUE)
  {
    return(NULL);
  }

  SET_VALID(I_aclIpv6Index, aclIpv6EntryData.valid);
  
  if ( (searchType == EXACT) ?
       (snmpQosAclIpv6EntryGet(USMDB_UNIT_CURRENT, &aclIpv6EntryData, nominator) != L7_SUCCESS) :
       ((snmpQosAclIpv6EntryGet(USMDB_UNIT_CURRENT, &aclIpv6EntryData, nominator) != L7_SUCCESS) &&
        (snmpQosAclIpv6EntryGetNext(USMDB_UNIT_CURRENT, &aclIpv6EntryData, nominator) != L7_SUCCESS)) )
  {
     ZERO_VALID(aclIpv6EntryData.valid);
     return(NULL);
  }
  
  if (nominator >= 0 && !VALID(nominator, aclIpv6EntryData.valid))
    return(NULL);

  return(&aclIpv6EntryData);
}

#ifdef SETS
int
k_aclIpv6Entry_test(ObjectInfo *object, ObjectSyntax *value,
                   doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_aclIpv6Entry_ready(ObjectInfo *object, ObjectSyntax *value,
                    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_aclIpv6Entry_set_defaults(doList_t *dp)
{
  aclIpv6Entry_t *data = (aclIpv6Entry_t *) (dp->data);

  if ((data->aclIpv6Name = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_aclIpv6Entry_set(aclIpv6Entry_t *data,
                  ContextInfo *contextInfo, int function)
{

  L7_BOOL isCreatedNew = L7_FALSE;
  L7_uint32 temp_aclIpv6Index = 0;
  L7_uint32 temp_aclIpv6RuleIndex = 0;
  char snmp_buffer[SNMP_BUFFER_LEN];

  /* This variable is added to pass to snmpQosAclIpv6RuleEntryGet() function 
   * so that our set values in 'data' are not lost.
   */
  aclIpv6Entry_t *tempData;

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  tempData = Clone_aclIpv6Entry(data); 
  tempData->aclIpv6Index = data->aclIpv6Index;          /* QSCAN: DELETE -- Temp fixup */

  if (snmpQosAclIpv6EntryGet(USMDB_UNIT_CURRENT, tempData, -1) != L7_SUCCESS)
  {
    if (!VALID(I_aclIpv6Status, data->valid) ||
        (data->aclIpv6Status != D_aclStatus_createAndGo &&
         data->aclIpv6Status != D_aclStatus_createAndWait) ||
        usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_TYPE_IPV6_FEATURE_ID) != L7_TRUE || 
        usmDbQosAclNamedIndexRangeCheck(USMDB_UNIT_CURRENT, L7_ACL_TYPE_IPV6,
                                        data->aclIpv6Index) != L7_SUCCESS ||
        usmDbQosAclCreate(USMDB_UNIT_CURRENT, data->aclIpv6Index) != L7_SUCCESS)
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return NO_CREATION_ERROR;
    }
    else
    {
      isCreatedNew = L7_TRUE;
      SET_VALID(I_aclIpv6Status, tempValid);
    }
  }

  /* Code for checking if dependent instances of aclIpv6RuleEntry exist */
  if(data->aclIpv6Status == D_aclIpv6Status_destroy)
  {
    /* check to see if this Ipv6 ACL has associated entries in other mibs */
    if (snmpIsQosAclIdAttachedToInterface(USMDB_UNIT_CURRENT, data->aclIpv6Index) == L7_TRUE)
    {
      /* deletion is not allowed if related instance of aclIfEntry exists */
      return COMMIT_FAILED_ERROR;
    }

    /* check to see if this Ipv6 ACL has associated entries in other mibs */
    if (snmpIsQosAclIdAttachedToVlan(USMDB_UNIT_CURRENT, data->aclIpv6Index) == L7_TRUE)
    {
      /* deletion is not allowed if related instance of aclIfEntry exists */
      return COMMIT_FAILED_ERROR;
    }

    temp_aclIpv6Index = data->aclIpv6Index;
    temp_aclIpv6RuleIndex = 0;
    if (usmDbQosAclRuleGetNext(USMDB_UNIT_CURRENT, temp_aclIpv6Index, 0,&temp_aclIpv6RuleIndex) == L7_SUCCESS)
    {
      /* deletion is not allowed if related instance of aclIpv6RuleEntry exists */
      return COMMIT_FAILED_ERROR;
    }
  }

  if (VALID(I_aclIpv6Name, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->aclIpv6Name->octet_ptr, data->aclIpv6Name->length);
    if ((usmDbQosAclNameStringCheck(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS) ||
        ((usmDbQosAclNameToIndex(USMDB_UNIT_CURRENT, snmp_buffer, &temp_aclIpv6Index) == L7_SUCCESS) &&
         (temp_aclIpv6Index != data->aclIpv6Index)) ||
        (usmDbQosAclNameAdd(USMDB_UNIT_CURRENT, data->aclIpv6Index,
                            snmp_buffer) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }

  }

  if (VALID(I_aclIpv6Status, data->valid) && (isCreatedNew != L7_TRUE) &&
      snmpQosAclIpv6StatusSet(USMDB_UNIT_CURRENT, data->aclIpv6Index,
                              data->aclIpv6Status) != L7_SUCCESS)
  {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
  }

  return NO_ERROR;
}

  #ifdef SR_aclIpv6Entry_UNDO
/* add #define SR_aclIpv6Entry_UNDO in sitedefs.h to
 * include the undo routine for the aclIpv6Entry family.
 */
int
aclIpv6Entry_undo(doList_t *doHead, doList_t *doCur,
                  ContextInfo *contextInfo)
{
  aclIpv6Entry_t *data = (aclIpv6Entry_t *) doCur->data;
  aclIpv6Entry_t *undodata = (aclIpv6Entry_t *) doCur->undodata;
  aclIpv6Entry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*Copy valid bits from data to undodata */
  if ( undodata != NULL && (data->aclIpv6Status != D_aclIpv6Status_destroy))
      memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
    /* ignore if deleting a non-existant entry */
    if (data->aclIpv6Status == D_aclIpv6Status_destroy)
      return NO_ERROR;

      /* undoin g an add, so delete */
      data->aclIpv6Status = D_aclIpv6Status_destroy;
      setdata = data;
      function = SR_DELETE;
  }
  else
  {
      /* undoing a delete or modify, replace the original data */
      if((undodata->aclIpv6Status == D_aclIpv6Status_notReady)
         || (undodata->aclIpv6Status == D_aclIpv6Status_notInService))
      {
          undodata->aclIpv6Status = D_aclIpv6Status_createAndWait;
      }
      else
      {
         if(undodata->aclIpv6Status == D_aclIpv6Status_active)
           undodata->aclIpv6Status = D_aclIpv6Status_createAndGo;
      }
      setdata = undodata;
      function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_aclIpv6Entry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
 #endif /* SR_aclIpv6Entry_UNDO */

#endif /* SETS */

aclIpv6RuleEntry_t *
k_aclIpv6RuleEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_INT32 aclIpv6Index,
                       SR_INT32 aclIpv6RuleIndex)
{
  static aclIpv6RuleEntry_t aclIpv6RuleEntryData;
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    aclIpv6RuleEntryData.aclRuleSrcIpv6Prefix = MakeOctetString(NULL, 0);
    aclIpv6RuleEntryData.aclRuleDstIpv6Prefix = MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }
 
  ZERO_VALID(aclIpv6RuleEntryData.valid);
  aclIpv6RuleEntryData.aclIpv6Index = aclIpv6Index;
  aclIpv6RuleEntryData.aclIpv6RuleIndex = aclIpv6RuleIndex;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_TYPE_IPV6_FEATURE_ID) != L7_TRUE)
  {
     return(NULL);
  }

  SET_VALID(I_aclIpv6RuleEntryIndex_aclIpv6Index, aclIpv6RuleEntryData.valid);
  SET_VALID(I_aclIpv6RuleIndex, aclIpv6RuleEntryData.valid);

  if ( (searchType == EXACT) ?
       (snmpQosAclIpv6RuleEntryGet(USMDB_UNIT_CURRENT,
                                   &aclIpv6RuleEntryData,
                                   nominator) != L7_SUCCESS) :
       ((snmpQosAclIpv6RuleEntryGet(USMDB_UNIT_CURRENT,
                                    &aclIpv6RuleEntryData,
                                    nominator) != L7_SUCCESS) &&
        (snmpQosAclIpv6RuleEntryGetNext(USMDB_UNIT_CURRENT,
                                        &aclIpv6RuleEntryData,
                                        nominator) != L7_SUCCESS)) )
  {
    ZERO_VALID(aclIpv6RuleEntryData.valid);
    return(NULL);
  }
      
  if (nominator >= 0 && !VALID(nominator, aclIpv6RuleEntryData.valid))
    return(NULL);

  return(&aclIpv6RuleEntryData);
}

#ifdef SETS
int
k_aclIpv6RuleEntry_test(ObjectInfo *object, ObjectSyntax *value,
                        doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_aclIpv6RuleEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                         doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_aclIpv6RuleEntry_set_defaults(doList_t *dp)
{
  aclIpv6RuleEntry_t *data = (aclIpv6RuleEntry_t *) (dp->data);

  data->aclIpv6RuleAction = D_aclIpv6RuleAction_deny;
  if ((data->aclRuleSrcIpv6Prefix = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }

  data->aclRuleDstIpv6PrefixLength = 0;
  if ((data->aclRuleDstIpv6Prefix = MakeOctetStringFromText("")) == 0) {
      return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->aclRuleDstIpv6PrefixLength = 0;

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_aclIpv6RuleEntry_set(aclIpv6RuleEntry_t *data,
                       ContextInfo *contextInfo, int function)
{
  L7_BOOL            isCreatedNew = L7_FALSE;
  L7_uint32          temp_uint1, temp_uint2;
  L7_in6_prefix_t    prefix;  
  L7_uchar8          snmp_buffer[SNMP_BUFFER_LEN];

  /* This variable is added to pass to snmpQosAclIpv6RuleEntryGet() function 
   * so that our set values in 'data' are not lost.
   */
  aclIpv6RuleEntry_t *tempData;
  L7_char8 tempValid[sizeof(data->valid)];
  
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  memset(snmp_buffer, 0x00, sizeof(snmp_buffer));
  memset(tempValid, 0x00, sizeof(tempValid));
  memset(prefix.in6Addr.in6.addr8, 0x00, sizeof(prefix.in6Addr.in6.addr8));
  tempData = Clone_aclIpv6RuleEntry(data);
  tempData->aclIpv6Index = data->aclIpv6Index;          
  tempData->aclIpv6RuleIndex = data->aclIpv6RuleIndex;  
  
  if (snmpQosAclIpv6RuleEntryGet(USMDB_UNIT_CURRENT, tempData, -1) != L7_SUCCESS)
  {
    if (!VALID(I_aclIpv6RuleStatus, data->valid) ||
        (data->aclIpv6RuleStatus != D_aclIpv6RuleStatus_createAndGo) ||
        snmpQosAclIpv6RuleEntryAdd(USMDB_UNIT_CURRENT, data->aclIpv6Index, data->aclIpv6RuleIndex) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return NO_CREATION_ERROR;
    }
    else
    {
      isCreatedNew = L7_TRUE;
      SET_VALID(I_aclIpv6RuleStatus, tempValid);
    }
  }

  if (VALID(I_aclIpv6RuleAction, data->valid))
  {
    if (snmpQosAclIpv6RuleActionAdd(USMDB_UNIT_CURRENT,
                                    data->aclIpv6Index,
                                    data->aclIpv6RuleIndex,
                                    data->aclIpv6RuleAction) != L7_SUCCESS)
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclIpv6RuleAction, tempValid);
    }
  }

  if (VALID(I_aclIpv6RuleMatchEvery, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_RULE_MATCH_EVERY_FEATURE_ID) != L7_TRUE) ||
        (snmpQosAclIpv6RuleEveryAdd(USMDB_UNIT_CURRENT,
                                    data->aclIpv6Index,
                                    data->aclIpv6RuleIndex,
                                    data->aclIpv6RuleMatchEvery) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclIpv6RuleMatchEvery, tempValid);
    }
  }

  if (VALID(I_aclIpv6RuleLogging, data->valid))
  {
    if ((usmDbQosAclRuleLoggingAllowed(USMDB_UNIT_CURRENT, data->aclIpv6Index, 
                                       data->aclIpv6RuleIndex) != L7_SUCCESS) ||
        (snmpQosAclIpv6RuleLoggingAdd(USMDB_UNIT_CURRENT,
                                  data->aclIpv6Index,
                                  data->aclIpv6RuleIndex,
                                  data->aclIpv6RuleLogging) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclIpv6RuleLogging, tempValid);
    }
  }

  if (VALID(I_aclIpv6RuleStatus, data->valid) &&
      (isCreatedNew != L7_TRUE) &&
      snmpQosAclIpv6RuleStatusSet(USMDB_UNIT_CURRENT,
                                  data->aclIpv6Index,
                                  data->aclIpv6RuleIndex,
                                  data->aclIpv6RuleStatus) != L7_SUCCESS)
  {
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_aclIpv6RuleAssignQueueId, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_ASSIGN_QUEUE_FEATURE_ID) != L7_TRUE) ||
       (usmDbQosAclRuleAssignQueueIdAdd(USMDB_UNIT_CURRENT,
                                      data->aclIpv6Index,
                                      data->aclIpv6RuleIndex,
                                      data->aclIpv6RuleAssignQueueId) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclIpv6RuleAssignQueueId, tempValid);
    }
  }

  if (VALID(I_aclIpv6RuleRedirectIntf, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_REDIRECT_FEATURE_ID) != L7_TRUE) ||
       (snmpQosAclIpv6RuleRedirectIntfAdd(USMDB_UNIT_CURRENT,
                                      data->aclIpv6Index,
                                      data->aclIpv6RuleIndex,
                                      data->aclIpv6RuleRedirectIntf) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclIpv6RuleRedirectIntf, tempValid);
    }
  }

  if (VALID(I_aclIpv6RuleMirrorIntf, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_MIRROR_FEATURE_ID) != L7_TRUE) ||
       (snmpQosAclIpv6RuleMirrorIntfAdd(USMDB_UNIT_CURRENT,
                                        data->aclIpv6Index,
                                        data->aclIpv6RuleIndex,
                                        data->aclIpv6RuleMirrorIntf) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclIpv6RuleMirrorIntf, tempValid);
    }
  }
  
  if (VALID(I_aclIpv6RuleProtocol, data->valid))
  {
    if  ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_IPV6_PROTOCOL_FEATURE_ID) != L7_TRUE) ||
         (usmDbQosAclRuleProtocolAdd(USMDB_UNIT_CURRENT,
                                     data->aclIpv6Index,
                                     data->aclIpv6RuleIndex,
                                     data->aclIpv6RuleProtocol) != L7_SUCCESS))
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclIpv6RuleProtocol, tempValid);
    }
  }

  if (VALID(I_aclIpv6RuleSrcL4Port, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_RULE_MATCH_IPV6_SRCL4PORT_FEATURE_ID) != L7_TRUE) ||
       (usmDbQosAclRuleSrcL4PortAdd(USMDB_UNIT_CURRENT,
                                   data->aclIpv6Index,
                                   data->aclIpv6RuleIndex,
                                   data->aclIpv6RuleSrcL4Port) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclIpv6RuleSrcL4Port, tempValid);
    }
  }

  if (VALID(I_aclIpv6RuleSrcL4PortRangeStart, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_RULE_MATCH_IPV6_SRCL4PORT_RANGE_FEATURE_ID) != L7_TRUE) ||
       ((usmDbQosAclRuleSrcL4PortRangeGet(USMDB_UNIT_CURRENT,
                                         data->aclIpv6Index,
                                         data->aclIpv6RuleIndex,
                                         &temp_uint1,
                                         &temp_uint2) != L7_SUCCESS) ||
       (usmDbQosAclRuleSrcL4PortRangeAdd(USMDB_UNIT_CURRENT,
                                         data->aclIpv6Index,
                                         data->aclIpv6RuleIndex,
                                         data->aclIpv6RuleSrcL4PortRangeStart,
                                         temp_uint2) != L7_SUCCESS)))
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclIpv6RuleSrcL4PortRangeStart,tempValid);
    }
  }

  if (VALID(I_aclIpv6RuleSrcL4PortRangeEnd, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_RULE_MATCH_IPV6_SRCL4PORT_RANGE_FEATURE_ID) != L7_TRUE) ||
       ((usmDbQosAclRuleSrcL4PortRangeGet(USMDB_UNIT_CURRENT,
                                         data->aclIpv6Index,
                                         data->aclIpv6RuleIndex,
                                         &temp_uint1,
                                         &temp_uint2) != L7_SUCCESS) ||
       (usmDbQosAclRuleSrcL4PortRangeAdd(USMDB_UNIT_CURRENT,
                                         data->aclIpv6Index,
                                         data->aclIpv6RuleIndex,
                                         temp_uint1,
                                         data->aclIpv6RuleSrcL4PortRangeEnd) != L7_SUCCESS)))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclIpv6RuleSrcL4PortRangeEnd,tempValid);
    }
  }

  if (VALID(I_aclIpv6RuleDestL4Port, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_RULE_MATCH_IPV6_DSTL4PORT_FEATURE_ID) != L7_TRUE) ||
       (usmDbQosAclRuleDstL4PortAdd(USMDB_UNIT_CURRENT,
                                   data->aclIpv6Index,
                                   data->aclIpv6RuleIndex,
                                   data->aclIpv6RuleDestL4Port) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclIpv6RuleDestL4Port, tempValid);
    }
  }

  if (VALID(I_aclIpv6RuleDestL4PortRangeStart, data->valid))
  {
     if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_IPV6_DSTL4PORT_RANGE_FEATURE_ID) != L7_TRUE) ||
        (usmDbQosAclRuleDstL4PortRangeGet(USMDB_UNIT_CURRENT,
                                         data->aclIpv6Index,
                                         data->aclIpv6RuleIndex,
                                         &temp_uint1,
                                         &temp_uint2) != L7_SUCCESS) ||
        ( usmDbQosAclRuleDstL4PortRangeAdd(USMDB_UNIT_CURRENT,
                                         data->aclIpv6Index,
                                         data->aclIpv6RuleIndex,
                                         data->aclIpv6RuleDestL4PortRangeStart,
                                         temp_uint2) != L7_SUCCESS))
     {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
     }
     else
     {
       SET_VALID(I_aclIpv6RuleDestL4PortRangeStart, tempValid);
     }
  }

  if (VALID(I_aclIpv6RuleDestL4PortRangeEnd, data->valid))
  {
    if  ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_IPV6_DSTL4PORT_RANGE_FEATURE_ID) != L7_TRUE) ||
       (usmDbQosAclRuleDstL4PortRangeGet(USMDB_UNIT_CURRENT,
                                         data->aclIpv6Index,
                                         data->aclIpv6RuleIndex,
                                         &temp_uint1,
                                         &temp_uint2) != L7_SUCCESS) ||
        (usmDbQosAclRuleDstL4PortRangeAdd(USMDB_UNIT_CURRENT,
                                         data->aclIpv6Index,
                                         data->aclIpv6RuleIndex,
                                         temp_uint1,
                                         data->aclIpv6RuleDestL4PortRangeEnd) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclIpv6RuleDestL4PortRangeEnd, tempValid);
    }
  }

  if (VALID(I_aclIpv6RuleIPDSCP, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_RULE_MATCH_IPV6_IPDSCP_FEATURE_ID) != L7_TRUE) ||
       (usmDbQosAclRuleIPDscpAdd(USMDB_UNIT_CURRENT,
                                data->aclIpv6Index,
                                data->aclIpv6RuleIndex,
                                data->aclIpv6RuleIPDSCP) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclIpv6RuleIPDSCP, tempValid);
    }
  }

  if (VALID(I_aclIpv6RuleFlowLabel, data->valid))
  {
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_RULE_MATCH_IPV6_FLOWLBL_FEATURE_ID) != L7_TRUE) ||
       (usmDbQosAclRuleIpv6FlowLabelAdd(USMDB_UNIT_CURRENT,
                                        data->aclIpv6Index,
                                        data->aclIpv6RuleIndex,
                                        data->aclIpv6RuleFlowLabel) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_aclIpv6RuleFlowLabel, tempValid);
    }
  }

  if(VALID(I_aclRuleSrcIpv6Prefix, data->valid) && VALID(I_aclRuleSrcIpv6PrefixLength, data->valid))
  {   
    prefix.in6PrefixLen = data->aclRuleSrcIpv6PrefixLength; 
    memcpy(snmp_buffer, data->aclRuleSrcIpv6Prefix->octet_ptr, data->aclRuleSrcIpv6Prefix->length);
    
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_RULE_MATCH_IPV6_SRCIP_FEATURE_ID) != L7_TRUE) ||
        (osapiInetPton(L7_AF_INET6, snmp_buffer, (L7_uchar8 *) prefix.in6Addr.in6.addr8) != L7_SUCCESS) ||
        (usmDbQosAclRuleSrcIpv6AddrAdd(USMDB_UNIT_CURRENT, data->aclIpv6Index, data->aclIpv6RuleIndex,
                                       &prefix) != L7_SUCCESS))
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
    }
    else
    {
       SET_VALID(I_aclRuleSrcIpv6Prefix, tempValid);
       SET_VALID(I_aclRuleSrcIpv6PrefixLength, tempValid);
    }
  }

  if(VALID(I_aclRuleDstIpv6Prefix, data->valid) && VALID(I_aclRuleDstIpv6PrefixLength, data->valid))
  {   
    prefix.in6PrefixLen = data->aclRuleDstIpv6PrefixLength; 
    memcpy(snmp_buffer, data->aclRuleDstIpv6Prefix->octet_ptr, data->aclRuleDstIpv6Prefix->length);
    
    if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                  L7_ACL_RULE_MATCH_IPV6_DSTIP_FEATURE_ID) != L7_TRUE) ||
        (osapiInetPton(L7_AF_INET6, snmp_buffer, (L7_uchar8 *) prefix.in6Addr.in6.addr8) != L7_SUCCESS) ||
        (usmDbQosAclRuleDstIpv6AddrAdd(USMDB_UNIT_CURRENT, data->aclIpv6Index, data->aclIpv6RuleIndex,
                                       &prefix) != L7_SUCCESS))
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
    }
    else
    {
       SET_VALID(I_aclRuleDstIpv6Prefix, tempValid);
       SET_VALID(I_aclRuleDstIpv6PrefixLength, tempValid);
    }
  }

  return NO_ERROR;
}
   #ifdef SR_aclIpv6RuleEntry_UNDO
/* add #define SR_aclIpv6RuleEntry_UNDO in sitedefs.h to
 * include the undo routine for the aclIpv6RuleEntry family.
 */
int
aclIpv6RuleEntry_undo(doList_t *doHead, doList_t *doCur,
                     ContextInfo *contextInfo)
{
  aclIpv6RuleEntry_t *data = (aclIpv6RuleEntry_t *) doCur->data;
  aclIpv6RuleEntry_t *undodata = (aclIpv6RuleEntry_t *) doCur->undodata;
  aclIpv6RuleEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*Copy valid bits from data to undodata */
  if( undodata != NULL  && (data->aclIpv6RuleStatus != D_aclIpv6RuleStatus_destroy))
      memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
    /* ignore if deleting a non-existant entry */
    if (data->aclIpv6RuleStatus == D_aclIpv6RuleStatus_destroy)
      return NO_ERROR;

      /* undoing an add, so delete */
      data->aclIpv6RuleStatus = D_aclIpv6RuleStatus_destroy;
      setdata = data;
      function = SR_DELETE;
  }
  else
  {
      /* undoing a delete or modify, replace the original data */
      if((undodata->aclIpv6RuleStatus == D_aclIpv6RuleStatus_notReady)
         || (undodata->aclIpv6RuleStatus == D_aclIpv6RuleStatus_notInService))
      {
          undodata->aclIpv6RuleStatus = D_aclIpv6RuleStatus_createAndWait;
      }
      else
      {
         if(undodata->aclIpv6RuleStatus == D_aclIpv6RuleStatus_active)
            undodata->aclIpv6RuleStatus = D_aclIpv6RuleStatus_createAndGo;
      }

      setdata = undodata;
      function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) &&(k_aclIpv6RuleEntry_set(setdata, contextInfo, function) == NO_ERROR))
      return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_aclIpv6RuleEntry_UNDO */

#endif /* SETS */ 


/* Changes for acl based vlans */

aclVlanEntry_t *
k_aclVlanEntry_get(int serialNum, ContextInfo *contextInfo,
                   int nominator,
                   int searchType,
                   SR_INT32 aclVlanIndex,
                   SR_INT32 aclVlanDirection,
                   SR_UINT32 aclVlanSequence,
                   SR_INT32 aclVlanAclType,
                   SR_INT32 aclVlanAclId)
{
  static aclVlanEntry_t aclVlanEntryData;

  L7_uint32 vlanId;
  ZERO_VALID(aclVlanEntryData.valid);

  vlanId = aclVlanEntryData.aclVlanIndex = aclVlanIndex;
  SET_VALID(I_aclVlanIndex, aclVlanEntryData.valid);

  aclVlanEntryData.aclVlanDirection = aclVlanDirection;
  SET_VALID(I_aclVlanDirection, aclVlanEntryData.valid);

  aclVlanEntryData.aclVlanSequence = aclVlanSequence;
  SET_VALID(I_aclVlanSequence, aclVlanEntryData.valid);
  
  aclVlanEntryData.aclVlanAclType = aclVlanAclType;
  SET_VALID(I_aclVlanAclType, aclVlanEntryData.valid);

  aclVlanEntryData.aclVlanAclId = aclVlanAclId;
  SET_VALID(I_aclVlanAclId, aclVlanEntryData.valid);

  if ((searchType == EXACT) ?
      ((usmDbVlanIDCheck(aclVlanEntryData.aclVlanIndex) != L7_SUCCESS) ||
       (snmpQosAclVlanNext(USMDB_UNIT_CURRENT,
                             &vlanId,
                             &aclVlanEntryData.aclVlanDirection,
                             &aclVlanEntryData.aclVlanSequence,
                             &aclVlanEntryData.aclVlanAclType,
                             &aclVlanEntryData.aclVlanAclId) != L7_SUCCESS) ||
       (snmpQosAclVlanCheckValid(USMDB_UNIT_CURRENT, 
                                 aclVlanEntryData.aclVlanIndex, 
                                 aclVlanEntryData.aclVlanDirection, 
                                 aclVlanEntryData.aclVlanSequence, 
                                 aclVlanEntryData.aclVlanAclType, 
                                 aclVlanEntryData.aclVlanAclId) != L7_SUCCESS))  :
        ((usmDbVlanIDCheck(aclVlanEntryData.aclVlanIndex) != L7_SUCCESS) &&
         (usmDbNextVlanGetNext(aclVlanEntryData.aclVlanIndex, 
                              &aclVlanEntryData.aclVlanIndex) != L7_SUCCESS) ) ||
        ((snmpQosAclVlanNext(USMDB_UNIT_CURRENT,
                             &vlanId,
                             &aclVlanEntryData.aclVlanDirection,
                             &aclVlanEntryData.aclVlanSequence,
                             &aclVlanEntryData.aclVlanAclType,
                             &aclVlanEntryData.aclVlanAclId) != L7_SUCCESS) &&
        (snmpQosAclVlanCheckValid(USMDB_UNIT_CURRENT,
                                  vlanId, 
                                  aclVlanEntryData.aclVlanDirection,
                                  aclVlanEntryData.aclVlanSequence,
                                  aclVlanEntryData.aclVlanAclType,
                                  aclVlanEntryData.aclVlanAclId) != L7_SUCCESS)) ||
      (usmDbVlanIDGetNewVlanId(vlanId, &aclVlanEntryData.aclVlanIndex) != L7_SUCCESS))
  {
     ZERO_VALID(aclVlanEntryData.valid);
     return(NULL);
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statemen
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
    case -1:
    case I_aclVlanIndex:
    case I_aclVlanDirection:
    case I_aclVlanSequence:
    case I_aclVlanAclType:
    case I_aclVlanAclId:
      if (nominator != -1 ) break;
      
  case I_aclVlanStatus:
      aclVlanEntryData.aclVlanStatus = D_aclVlanStatus_active;
      SET_VALID(I_aclVlanStatus, aclVlanEntryData.valid);
      break;

    default:
      return(NULL);
      break;
  }

  if (nominator >= 0 && !VALID(nominator, aclVlanEntryData.valid))
  {
     return(NULL);
  }

  return(&aclVlanEntryData);
}

#ifdef SETS
int
k_aclVlanEntry_test(ObjectInfo *object, ObjectSyntax *value,
                doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_aclVlanEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                 doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_aclVlanEntry_set_defaults(doList_t *dp)
{
  aclVlanEntry_t *data = (aclVlanEntry_t *) (dp->data);
  
  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_aclVlanEntry_set(aclVlanEntry_t *data,
               ContextInfo *contextInfo, int function)
{
  L7_uint32 vlanId;
  L7_BOOL isCreatedNew = L7_FALSE;

  /*
   * Defining temporary variable for storing the valid bits for the case when th
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  vlanId = data->aclVlanIndex;
  if(usmDbVlanIDCheck(data->aclVlanIndex) != L7_SUCCESS)
  {
    return COMMIT_FAILED_ERROR;
  }
    
  if (snmpQosAclVlanCheckValid(USMDB_UNIT_CURRENT,
                               vlanId,
                               data->aclVlanDirection,
                               data->aclVlanSequence,
                               data->aclVlanAclType,
                               data->aclVlanAclId) != L7_SUCCESS)
  {
    if (!VALID(I_aclVlanStatus, data->valid) ||
        (data->aclVlanStatus != D_aclVlanStatus_createAndGo &&
        data->aclVlanStatus != D_aclVlanStatus_createAndWait) ||
        snmpQosAclVlanAdd(USMDB_UNIT_CURRENT, 
                          vlanId,
                          data->aclVlanDirection,
                          data->aclVlanSequence,
                          data->aclVlanAclType,
                          data->aclVlanAclId) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      isCreatedNew = L7_TRUE;
      SET_VALID(I_aclVlanStatus, tempValid);
    }
  }


  if (VALID(I_aclVlanStatus, data->valid) && (isCreatedNew != L7_TRUE) &&
      snmpQosAclVlanStatusSet(USMDB_UNIT_CURRENT, 
                              vlanId,
                              data->aclVlanDirection,
                              data->aclVlanSequence,
                              data->aclVlanAclType,
                              data->aclVlanAclId,
                              data->aclVlanStatus) != L7_SUCCESS)
  {
    memcpy(data->valid, tempValid, sizeof(data->valid));
    return COMMIT_FAILED_ERROR;
  }
  
  return NO_ERROR;
}

  #ifdef SR_aclVlanEntry_UNDO
/* add #define SR_aclVlanEntry_UNDO in sitedefs.h to
 * include the undo routine for the aclVlanEntry family.
 */
int
aclVlanEntry_undo(doList_t *doHead, doList_t *doCur,
              ContextInfo *contextInfo)
{

  aclVlanEntry_t *data = (aclVlanEntry_t *) doCur->data;
  aclVlanEntry_t *undodata = (aclVlanEntry_t *) doCur->undodata;
  aclVlanEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*Copy valid bits from data to undodata */
  if( undodata != NULL && (data->aclVlanStatus != D_aclVlanStatus_destroy ))
      memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
    /* ignore if deleting a non-existant entry */
    if (data->aclVlanStatus == D_aclVlanStatus_destroy)
      return NO_ERROR;

      /* undoing an add, so delete */
      data->aclVlanStatus = D_aclVlanStatus_destroy;
      setdata = data;
      function = SR_DELETE;
  }
  else
  {
      /* undoing a delete or modify, replace the original data */
      if((undodata->aclVlanStatus == D_aclVlanStatus_notReady)
         || (undodata->aclVlanStatus == D_aclVlanStatus_notInService))
      {
          undodata->aclVlanStatus = D_aclVlanStatus_createAndWait;
      }
      else
      {
         if(undodata->aclVlanStatus == D_aclVlanStatus_active)
           undodata->aclVlanStatus = D_aclVlanStatus_createAndGo;
      }
      setdata = undodata;
      function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_aclVlanEntry_set(setdata, contextInfo, function) == NO_ERROR))
      return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_aclVlanEntry_UNDO */

#endif /* SETS */
 

