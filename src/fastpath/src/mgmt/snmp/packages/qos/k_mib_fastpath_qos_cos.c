/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename k_mib_fastpath_qos_cos.c
*
* @purpose  Provide interface to COS Private MIB
*                  
* @component SNMP
*
* @comments 
*
* @create 05/04/2004
*
* @author cpverne
* @end
*
**********************************************************************/
#include "k_private_base.h"
#include "k_mib_fastpath_qos_cos_api.h"
#include "usmdb_common.h"
#include "cos_exports.h"
#include "usmdb_util_api.h"

#ifdef L7_QOS_PACKAGE
#include "usmdb_qos_cos_api.h"
#endif


agentCosMapIpPrecEntry_t *
k_agentCosMapIpPrecEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_INT32 agentCosMapIpPrecIntfIndex,
                             SR_UINT32 agentCosMapIpPrecValue)
{
  static agentCosMapIpPrecEntry_t agentCosMapIpPrecEntryData;
  L7_uint32 intIfNum = 0;

  if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                             L7_COS_FEATURE_SUPPORTED) != L7_TRUE) ||
      (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                             L7_COS_MAP_IPPREC_FEATURE_ID) != L7_TRUE))
    return(NULL);

  ZERO_VALID(agentCosMapIpPrecEntryData.valid);
  agentCosMapIpPrecEntryData.agentCosMapIpPrecIntfIndex = agentCosMapIpPrecIntfIndex;
  SET_VALID(I_agentCosMapIpPrecIntfIndex, agentCosMapIpPrecEntryData.valid);
  agentCosMapIpPrecEntryData.agentCosMapIpPrecValue = agentCosMapIpPrecValue;
  SET_VALID(I_agentCosMapIpPrecValue, agentCosMapIpPrecEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpAgentCosMapIpPrecEntryGet(USMDB_UNIT_CURRENT,
                                     agentCosMapIpPrecEntryData.agentCosMapIpPrecIntfIndex,
                                     agentCosMapIpPrecEntryData.agentCosMapIpPrecValue,
                                     &intIfNum) != L7_SUCCESS) :
      ((snmpAgentCosMapIpPrecEntryGet(USMDB_UNIT_CURRENT,
                                      agentCosMapIpPrecEntryData.agentCosMapIpPrecIntfIndex,
                                      agentCosMapIpPrecEntryData.agentCosMapIpPrecValue,
                                      &intIfNum) != L7_SUCCESS) &&
       (snmpAgentCosMapIpPrecEntryNextGet(USMDB_UNIT_CURRENT,
                                          &agentCosMapIpPrecEntryData.agentCosMapIpPrecIntfIndex,
                                          &agentCosMapIpPrecEntryData.agentCosMapIpPrecValue,
                                          &intIfNum) != L7_SUCCESS)))
    return(NULL);

  switch (nominator)
  {
  case -1:
  case I_agentCosMapIpPrecIntfIndex:
  case I_agentCosMapIpPrecValue:
    if (nominator != -1) break;


  case I_agentCosMapIpPrecTrafficClass:
    if (usmDbQosCosMapIpPrecTrafficClassGet(USMDB_UNIT_CURRENT, intIfNum, agentCosMapIpPrecEntryData.agentCosMapIpPrecValue,
                                            &agentCosMapIpPrecEntryData.agentCosMapIpPrecTrafficClass) == L7_SUCCESS)
      SET_VALID(I_agentCosMapIpPrecTrafficClass, agentCosMapIpPrecEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentCosMapIpPrecEntryData.valid))
    return(NULL);

  return(&agentCosMapIpPrecEntryData);
}

#ifdef SETS
int
k_agentCosMapIpPrecEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentCosMapIpPrecEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentCosMapIpPrecEntry_set_defaults(doList_t *dp)
{
  agentCosMapIpPrecEntry_t *data = (agentCosMapIpPrecEntry_t *) (dp->data);


  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentCosMapIpPrecEntry_set(agentCosMapIpPrecEntry_t *data,
                             ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_uint32 intIfNum = 0;

  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if ((snmpAgentCosMapIpPrecEntryGet(USMDB_UNIT_CURRENT, data->agentCosMapIpPrecIntfIndex, data->agentCosMapIpPrecValue, &intIfNum) != L7_SUCCESS) ||
      (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                             L7_COS_FEATURE_SUPPORTED) != L7_TRUE) ||
      (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                             L7_COS_MAP_IPPREC_FEATURE_ID) != L7_TRUE))
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentCosMapIpPrecTrafficClass, data->valid))
  {
    if (usmDbQosCosMapIpPrecTrafficClassSet(USMDB_UNIT_CURRENT, intIfNum, data->agentCosMapIpPrecValue, 
                                            data->agentCosMapIpPrecTrafficClass) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentCosMapIpPrecTrafficClass, tempValid);
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentCosMapIpPrecEntry_UNDO
/* add #define SR_agentCosMapIpPrecEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentCosMapIpPrecEntry family.
 */
int
agentCosMapIpPrecEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
  agentCosMapIpPrecEntry_t *data = (agentCosMapIpPrecEntry_t *) doCur->data;
  agentCosMapIpPrecEntry_t *undodata = (agentCosMapIpPrecEntry_t *) doCur->undodata;
  agentCosMapIpPrecEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return(UNDO_FAILED_ERROR);
  memcpy(undodata->valid, data->valid, sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if (k_agentCosMapIpPrecEntry_set(setdata, contextInfo, function) == NO_ERROR)
    return(NO_ERROR);

  return(UNDO_FAILED_ERROR);
}
#endif /* SR_agentCosMapIpPrecEntry_UNDO */

#endif /* SETS */

agentCosMapIpDscpEntry_t *
k_agentCosMapIpDscpEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_INT32 agentCosMapIpDscpIntfIndex,
                             SR_UINT32 agentCosMapIpDscpValue)
{
  static agentCosMapIpDscpEntry_t agentCosMapIpDscpEntryData;
  L7_uint32 intIfNum = 0;

  if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                             L7_COS_FEATURE_SUPPORTED) != L7_TRUE) ||
      (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                             L7_COS_MAP_IPDSCP_FEATURE_ID) != L7_TRUE))
    return(NULL);

  ZERO_VALID(agentCosMapIpDscpEntryData.valid);
  agentCosMapIpDscpEntryData.agentCosMapIpDscpIntfIndex = agentCosMapIpDscpIntfIndex;
  SET_VALID(I_agentCosMapIpDscpIntfIndex, agentCosMapIpDscpEntryData.valid);
  agentCosMapIpDscpEntryData.agentCosMapIpDscpValue = agentCosMapIpDscpValue;
  SET_VALID(I_agentCosMapIpDscpValue, agentCosMapIpDscpEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpAgentCosMapIpDscpEntryGet(USMDB_UNIT_CURRENT,
                                     agentCosMapIpDscpEntryData.agentCosMapIpDscpIntfIndex,
                                     agentCosMapIpDscpEntryData.agentCosMapIpDscpValue,
                                     &intIfNum) != L7_SUCCESS) :
      ((snmpAgentCosMapIpDscpEntryGet(USMDB_UNIT_CURRENT,
                                      agentCosMapIpDscpEntryData.agentCosMapIpDscpIntfIndex,
                                      agentCosMapIpDscpEntryData.agentCosMapIpDscpValue,
                                      &intIfNum) != L7_SUCCESS) &&
       (snmpAgentCosMapIpDscpEntryNextGet(USMDB_UNIT_CURRENT,
                                          &agentCosMapIpDscpEntryData.agentCosMapIpDscpIntfIndex,
                                          &agentCosMapIpDscpEntryData.agentCosMapIpDscpValue,
                                          &intIfNum) != L7_SUCCESS)))
    return(NULL);

  switch (nominator)
  {
  case -1:
  case I_agentCosMapIpDscpIntfIndex:
  case I_agentCosMapIpDscpValue:
    if (nominator != -1) break;


  case I_agentCosMapIpDscpTrafficClass:
    if (usmDbQosCosMapIpDscpTrafficClassGet(USMDB_UNIT_CURRENT, intIfNum, agentCosMapIpDscpEntryData.agentCosMapIpDscpValue,
                                            &agentCosMapIpDscpEntryData.agentCosMapIpDscpTrafficClass) == L7_SUCCESS)
      SET_VALID(I_agentCosMapIpDscpTrafficClass, agentCosMapIpDscpEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentCosMapIpDscpEntryData.valid))
    return(NULL);

  return(&agentCosMapIpDscpEntryData);
}

#ifdef SETS
int
k_agentCosMapIpDscpEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentCosMapIpDscpEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentCosMapIpDscpEntry_set_defaults(doList_t *dp)
{
  agentCosMapIpDscpEntry_t *data = (agentCosMapIpDscpEntry_t *) (dp->data);


  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentCosMapIpDscpEntry_set(agentCosMapIpDscpEntry_t *data,
                             ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_uint32 intIfNum = 0;

  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if ((snmpAgentCosMapIpDscpEntryGet(USMDB_UNIT_CURRENT, data->agentCosMapIpDscpIntfIndex, data->agentCosMapIpDscpValue, &intIfNum) != L7_SUCCESS) ||
      (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                             L7_COS_FEATURE_SUPPORTED) != L7_TRUE) ||
      (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                             L7_COS_MAP_IPDSCP_FEATURE_ID) != L7_TRUE))
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentCosMapIpDscpTrafficClass, data->valid))
  {
    if (usmDbQosCosMapIpDscpTrafficClassSet(USMDB_UNIT_CURRENT, intIfNum, data->agentCosMapIpDscpValue, 
                                            data->agentCosMapIpDscpTrafficClass) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentCosMapIpDscpTrafficClass, tempValid);
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentCosMapIpDscpEntry_UNDO
/* add #define SR_agentCosMapIpDscpEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentCosMapIpDscpEntry family.
 */
int
agentCosMapIpDscpEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
  agentCosMapIpDscpEntry_t *data = (agentCosMapIpDscpEntry_t *) doCur->data;
  agentCosMapIpDscpEntry_t *undodata = (agentCosMapIpDscpEntry_t *) doCur->undodata;
  agentCosMapIpDscpEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return(UNDO_FAILED_ERROR);
  memcpy(undodata->valid, data->valid, sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if (k_agentCosMapIpDscpEntry_set(setdata, contextInfo, function) == NO_ERROR)
    return(NO_ERROR);

  return(UNDO_FAILED_ERROR);
}
#endif /* SR_agentCosMapIpDscpEntry_UNDO */

#endif /* SETS */

agentCosMapIntfTrustEntry_t *
k_agentCosMapIntfTrustEntry_get(int serialNum, ContextInfo *contextInfo,
                                int nominator,
                                int searchType,
                                SR_INT32 agentCosMapIntfTrustIntfIndex)
{
  static agentCosMapIntfTrustEntry_t agentCosMapIntfTrustEntryData;
  L7_uint32 intIfNum = 0;

  if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                             L7_COS_FEATURE_SUPPORTED) != L7_TRUE) ||
      (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                             L7_COS_MAP_TRUST_MODE_FEATURE_ID) != L7_TRUE))
    return(NULL);

  ZERO_VALID(agentCosMapIntfTrustEntryData.valid);
  agentCosMapIntfTrustEntryData.agentCosMapIntfTrustIntfIndex = agentCosMapIntfTrustIntfIndex;
  SET_VALID(I_agentCosMapIntfTrustIntfIndex, agentCosMapIntfTrustEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpAgentCosMapIntfTrustEntryGet(USMDB_UNIT_CURRENT, 
                                        agentCosMapIntfTrustEntryData.agentCosMapIntfTrustIntfIndex,
                                        &intIfNum) != L7_SUCCESS) :
      ((snmpAgentCosMapIntfTrustEntryGet(USMDB_UNIT_CURRENT, 
                                        agentCosMapIntfTrustEntryData.agentCosMapIntfTrustIntfIndex,
                                        &intIfNum) != L7_SUCCESS) &&
       (snmpAgentCosMapIntfTrustEntryNextGet(USMDB_UNIT_CURRENT, 
                                             &agentCosMapIntfTrustEntryData.agentCosMapIntfTrustIntfIndex,
                                             &intIfNum) != L7_SUCCESS)))
    return(NULL);

  switch (nominator)
  {
  case -1:
  case I_agentCosMapIntfTrustIntfIndex:
    if (nominator != -1) break;

  case I_agentCosMapIntfTrustMode:
    if (snmpAgentCosMapIntfTrustModeGet(USMDB_UNIT_CURRENT, intIfNum,
                                        &agentCosMapIntfTrustEntryData.agentCosMapIntfTrustMode) == L7_SUCCESS)
      SET_VALID(I_agentCosMapIntfTrustMode, agentCosMapIntfTrustEntryData.valid);
    if (nominator != -1) break;

  case I_agentCosMapUntrustedTrafficClass:
    if (usmDbQosCosMapUntrustedPortDefaultTrafficClassGet(USMDB_UNIT_CURRENT, intIfNum,
                                        &agentCosMapIntfTrustEntryData.agentCosMapUntrustedTrafficClass) == L7_SUCCESS)
      SET_VALID(I_agentCosMapUntrustedTrafficClass, agentCosMapIntfTrustEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentCosMapIntfTrustEntryData.valid))
    return(NULL);

  return(&agentCosMapIntfTrustEntryData);
}

#ifdef SETS
int
k_agentCosMapIntfTrustEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *dp, ContextInfo *contextInfo)
{

  L7_uint32 compId = L7_FLEX_QOS_COS_COMPONENT_ID;
  L7_uint32 featureId;

  if (L7_TRUE != usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, compId, L7_COS_MAP_TRUST_MODE_FEATURE_ID))
  {
    return NOT_WRITABLE_ERROR;
  }

  switch (object->nominator)
  {

    case I_agentCosMapIntfTrustMode:
      switch ( value->sl_value )
      {
        case D_agentCosMapIntfTrustMode_untrusted:
          featureId = L7_COS_FEATURE_SUPPORTED;
          break;
        case D_agentCosMapIntfTrustMode_trustDot1p:
          compId = L7_DOT1P_COMPONENT_ID;
          featureId = L7_DOT1P_CONFIGURE_TRAFFIC_CLASS_FEATURE_ID;
          break;
        case D_agentCosMapIntfTrustMode_trustIpPrecedence:
          featureId = L7_COS_MAP_IPPREC_FEATURE_ID;
          break;
        case D_agentCosMapIntfTrustMode_trustIpDscp:
          featureId = L7_COS_MAP_IPDSCP_FEATURE_ID;
          break;
        default:
          return WRONG_VALUE_ERROR;
          break;
      }

      if (L7_TRUE != usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, compId, featureId))
      {
        return WRONG_VALUE_ERROR;
      }

      break;


    default:
      /* fall through */
      break;
  }


  return NO_ERROR;
}

int
k_agentCosMapIntfTrustEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                  doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentCosMapIntfTrustEntry_set_defaults(doList_t *dp)
{
  agentCosMapIntfTrustEntry_t *data = (agentCosMapIntfTrustEntry_t *) (dp->data);


  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentCosMapIntfTrustEntry_set(agentCosMapIntfTrustEntry_t *data,
                                ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_uint32 intIfNum = 0;

  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if ((snmpAgentCosMapIntfTrustEntryGet(USMDB_UNIT_CURRENT, data->agentCosMapIntfTrustIntfIndex, &intIfNum) != L7_SUCCESS) ||
      (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                             L7_COS_FEATURE_SUPPORTED) != L7_TRUE) ||
      (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                             L7_COS_MAP_TRUST_MODE_FEATURE_ID) != L7_TRUE))
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentCosMapIntfTrustMode, data->valid))
  {
    if (snmpAgentCosMapIntfTrustModeSet(USMDB_UNIT_CURRENT, intIfNum, 
                                        data->agentCosMapIntfTrustMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentCosMapIntfTrustMode, tempValid);
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentCosMapIntfTrustEntry_UNDO
/* add #define SR_agentCosMapIntfTrustEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentCosMapIntfTrustEntry family.
 */
int
agentCosMapIntfTrustEntry_undo(doList_t *doHead, doList_t *doCur,
                               ContextInfo *contextInfo)
{
  agentCosMapIntfTrustEntry_t *data = (agentCosMapIntfTrustEntry_t *) doCur->data;
  agentCosMapIntfTrustEntry_t *undodata = (agentCosMapIntfTrustEntry_t *) doCur->undodata;
  agentCosMapIntfTrustEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return(UNDO_FAILED_ERROR);
  memcpy(undodata->valid, data->valid, sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if (k_agentCosMapIntfTrustEntry_set(setdata, contextInfo, function) == NO_ERROR)
    return(NO_ERROR);

  return(UNDO_FAILED_ERROR);
}
#endif /* SR_agentCosMapIntfTrustEntry_UNDO */

#endif /* SETS */

agentCosQueueCfgGroup_t *
k_agentCosQueueCfgGroup_get(int serialNum, ContextInfo *contextInfo,
                            int nominator)
{
  static agentCosQueueCfgGroup_t agentCosQueueCfgGroupData;

  ZERO_VALID(agentCosQueueCfgGroupData.valid);

  switch (nominator)
  {
  case I_agentCosQueueNumQueuesPerPort:
    if (usmDbQosCosQueueNumQueuesPerPortGet(USMDB_UNIT_CURRENT, 
                                            &agentCosQueueCfgGroupData.agentCosQueueNumQueuesPerPort) == L7_SUCCESS)
      SET_VALID(I_agentCosQueueNumQueuesPerPort, agentCosQueueCfgGroupData.valid);
    break;

  case I_agentCosQueueNumDropPrecedenceLevels:
    if (usmDbQosCosQueueNumDropPrecLevelsGet(USMDB_UNIT_CURRENT, 
                                             &agentCosQueueCfgGroupData.agentCosQueueNumDropPrecedenceLevels) == L7_SUCCESS)
      /* Add 1 for non-TCP traffic */
      agentCosQueueCfgGroupData.agentCosQueueNumDropPrecedenceLevels++;
      SET_VALID(I_agentCosQueueNumDropPrecedenceLevels, agentCosQueueCfgGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentCosQueueCfgGroupData.valid))
    return(NULL);

  return(&agentCosQueueCfgGroupData);
}

agentCosQueueControlEntry_t *
k_agentCosQueueControlEntry_get(int serialNum, ContextInfo *contextInfo,
                                int nominator,
                                int searchType,
                                SR_INT32 agentCosQueueIntfIndex)
{
  static agentCosQueueControlEntry_t agentCosQueueControlEntryData;
  L7_uint32 intIfNum = 0;

  if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                             L7_COS_FEATURE_SUPPORTED) != L7_TRUE) ||
      ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                              L7_COS_QUEUE_CFG_PER_INTF_FEATURE_ID) != L7_TRUE) &&
       (agentCosQueueIntfIndex != 0)))
    return(NULL);

  ZERO_VALID(agentCosQueueControlEntryData.valid);
  agentCosQueueControlEntryData.agentCosQueueIntfIndex = agentCosQueueIntfIndex;
  SET_VALID(I_agentCosQueueIntfIndex, agentCosQueueControlEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpAgentCosQueueControlEntryGet(USMDB_UNIT_CURRENT,
                                        agentCosQueueControlEntryData.agentCosQueueIntfIndex,
                                        &intIfNum) != L7_SUCCESS) :
      ((snmpAgentCosQueueControlEntryGet(USMDB_UNIT_CURRENT,
                                         agentCosQueueControlEntryData.agentCosQueueIntfIndex,
                                         &intIfNum) != L7_SUCCESS) &&
       (snmpAgentCosQueueControlEntryNextGet(USMDB_UNIT_CURRENT,
                                             &agentCosQueueControlEntryData.agentCosQueueIntfIndex,
                                             &intIfNum) != L7_SUCCESS)))
    return(NULL);

  switch (nominator)
  {
  case -1:
  case I_agentCosQueueIntfIndex:
    if (nominator != -1) break;

  case I_agentCosQueueIntfShapingRate:
    if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                               L7_COS_QUEUE_INTF_SHAPING_FEATURE_ID) == L7_TRUE) &&
        (usmDbQosCosQueueIntfShapingRateGet(USMDB_UNIT_CURRENT, intIfNum,
                                            &agentCosQueueControlEntryData.agentCosQueueIntfShapingRate) == L7_SUCCESS))
      SET_VALID(I_agentCosQueueIntfShapingRate, agentCosQueueControlEntryData.valid);
    if (nominator != -1) break;

  case I_agentCosQueueIntfShapingRateUnits:
    {
      L7_RATE_UNIT_t  value;
      if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                                 L7_COS_QUEUE_INTF_SHAPING_FEATURE_ID) == L7_TRUE) &&
          (usmDbQosCosQueueIntfShapingRateUnitsGet(&value) == L7_SUCCESS))
      {
        if (value == L7_RATE_UNIT_PERCENT)
        {
          agentCosQueueControlEntryData.agentCosQueueIntfShapingRateUnits = 1;
          SET_VALID(I_agentCosQueueIntfShapingRateUnits, agentCosQueueControlEntryData.valid);
        }
        else if (value  == L7_RATE_UNIT_KBPS)
        {
          agentCosQueueControlEntryData.agentCosQueueIntfShapingRateUnits = 2;
          SET_VALID(I_agentCosQueueIntfShapingRateUnits, agentCosQueueControlEntryData.valid);
        }
      }
    }
    if (nominator != -1) break;

  case I_agentCosQueueMgmtTypeIntf:
    if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                               L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID) == L7_TRUE) &&
        (snmpAgentCosQueueMgmtTypeIntfGet(USMDB_UNIT_CURRENT, intIfNum,
                                          &agentCosQueueControlEntryData.agentCosQueueMgmtTypeIntf) == L7_SUCCESS))
      SET_VALID(I_agentCosQueueMgmtTypeIntf, agentCosQueueControlEntryData.valid);
    if (nominator != -1) break;

  case I_agentCosQueueWredDecayExponent:
    if (((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                                L7_COS_QUEUE_WRED_DECAY_EXP_SYSTEM_ONLY_FEATURE_ID) == L7_FALSE) ||
         (intIfNum == L7_ALL_INTERFACES)) &&
        (usmDbQosCosQueueWredDecayExponentGet(USMDB_UNIT_CURRENT, intIfNum,
                                              &agentCosQueueControlEntryData.agentCosQueueWredDecayExponent) == L7_SUCCESS))
      SET_VALID(I_agentCosQueueWredDecayExponent, agentCosQueueControlEntryData.valid);
    if (nominator != -1) break;

  case I_agentCosQueueDefaultsRestore:
    agentCosQueueControlEntryData.agentCosQueueDefaultsRestore = D_agentCosQueueDefaultsRestore_disable;
    SET_VALID(I_agentCosQueueDefaultsRestore, agentCosQueueControlEntryData.valid);
    break;

  default:
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentCosQueueControlEntryData.valid))
    return(NULL);

  return(&agentCosQueueControlEntryData);
}

#ifdef SETS
int
k_agentCosQueueControlEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentCosQueueControlEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                  doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentCosQueueControlEntry_set_defaults(doList_t *dp)
{
  agentCosQueueControlEntry_t *data = (agentCosQueueControlEntry_t *) (dp->data);

  data->agentCosQueueMgmtTypeIntf = D_agentCosQueueMgmtTypeIntf_taildrop;

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentCosQueueControlEntry_set(agentCosQueueControlEntry_t *data,
                                ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_uint32 intIfNum = 0;

  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if ((snmpAgentCosQueueControlEntryGet(USMDB_UNIT_CURRENT, data->agentCosQueueIntfIndex, &intIfNum) != L7_SUCCESS) ||
      (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                             L7_COS_FEATURE_SUPPORTED) != L7_TRUE) ||
      ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                             L7_COS_QUEUE_CFG_PER_INTF_FEATURE_ID) != L7_TRUE) &&
       (data->agentCosQueueIntfIndex != 0)))
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentCosQueueIntfShapingRate, data->valid))
  {
    if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                               L7_COS_QUEUE_INTF_SHAPING_FEATURE_ID) != L7_TRUE) ||
        (usmDbQosCosQueueIntfShapingRateSet(USMDB_UNIT_CURRENT, intIfNum,
                                            data->agentCosQueueIntfShapingRate) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentCosQueueIntfShapingRate, tempValid);
    }
  }

  if (VALID(I_agentCosQueueMgmtTypeIntf, data->valid))
  {
    if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                               L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID) != L7_TRUE) ||
        (snmpAgentCosQueueMgmtTypeIntfSet(USMDB_UNIT_CURRENT, intIfNum,
                                          data->agentCosQueueMgmtTypeIntf) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentCosQueueMgmtTypeIntf, tempValid);
    }
  }

  if (VALID(I_agentCosQueueWredDecayExponent, data->valid))
  {
    if (((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                                L7_COS_QUEUE_WRED_DECAY_EXP_SYSTEM_ONLY_FEATURE_ID) == L7_TRUE) &&
         (intIfNum != L7_ALL_INTERFACES)) ||
        (usmDbQosCosQueueWredDecayExponentSet(USMDB_UNIT_CURRENT, intIfNum,
                                              data->agentCosQueueWredDecayExponent) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentCosQueueWredDecayExponent, tempValid);
    }
  }

  if (VALID(I_agentCosQueueDefaultsRestore, data->valid))
  {
    if (snmpAgentCosQueueDefaultsRestoreSet(USMDB_UNIT_CURRENT, intIfNum,
                                            data->agentCosQueueDefaultsRestore) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentCosQueueDefaultsRestore, tempValid);
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentCosQueueControlEntry_UNDO
/* add #define SR_agentCosQueueControlEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentCosQueueControlEntry family.
 */
int
agentCosQueueControlEntry_undo(doList_t *doHead, doList_t *doCur,
                               ContextInfo *contextInfo)
{
  agentCosQueueControlEntry_t *data = (agentCosQueueControlEntry_t *) doCur->data;
  agentCosQueueControlEntry_t *undodata = (agentCosQueueControlEntry_t *) doCur->undodata;
  agentCosQueueControlEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return(UNDO_FAILED_ERROR);
  memcpy(undodata->valid, data->valid, sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if (k_agentCosQueueControlEntry_set(setdata, contextInfo, function) == NO_ERROR)
    return(NO_ERROR);

  return(UNDO_FAILED_ERROR);
}
#endif /* SR_agentCosQueueControlEntry_UNDO */

#endif /* SETS */

agentCosQueueEntry_t *
k_agentCosQueueEntry_get(int serialNum, ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         SR_INT32 agentCosQueueIntfIndex,
                         SR_UINT32 agentCosQueueIndex)
{
  static agentCosQueueEntry_t agentCosQueueEntryData;
  L7_uint32 intIfNum = 0;

  if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                             L7_COS_FEATURE_SUPPORTED) != L7_TRUE) ||
      ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                              L7_COS_QUEUE_CFG_PER_INTF_FEATURE_ID) != L7_TRUE) &&
       (agentCosQueueIntfIndex != 0)))
    return(NULL);

  ZERO_VALID(agentCosQueueEntryData.valid);
  agentCosQueueEntryData.agentCosQueueIntfIndex = agentCosQueueIntfIndex;
  SET_VALID(I_agentCosQueueEntryIndex_agentCosQueueIntfIndex, agentCosQueueEntryData.valid);
  agentCosQueueEntryData.agentCosQueueIndex = agentCosQueueIndex;
  SET_VALID(I_agentCosQueueIndex, agentCosQueueEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpAgentCosQueueEntryGet(USMDB_UNIT_CURRENT,
                                 agentCosQueueEntryData.agentCosQueueIntfIndex,
                                 agentCosQueueEntryData.agentCosQueueIndex,
                                 &intIfNum) != L7_SUCCESS) :
      ((snmpAgentCosQueueEntryGet(USMDB_UNIT_CURRENT,
                                  agentCosQueueEntryData.agentCosQueueIntfIndex,
                                  agentCosQueueEntryData.agentCosQueueIndex,
                                  &intIfNum) != L7_SUCCESS) &&
       (snmpAgentCosQueueEntryNextGet(USMDB_UNIT_CURRENT,
                                      &agentCosQueueEntryData.agentCosQueueIntfIndex,
                                      &agentCosQueueEntryData.agentCosQueueIndex,
                                      &intIfNum) != L7_SUCCESS)))
    return(NULL);

  switch (nominator)
  {
  case -1:
  case I_agentCosQueueEntryIndex_agentCosQueueIntfIndex:
  case I_agentCosQueueIndex:
    if (nominator != -1) break;

  case I_agentCosQueueSchedulerType:
    if (snmpAgentCosQueueSchedulerTypeGet(USMDB_UNIT_CURRENT, intIfNum, agentCosQueueEntryData.agentCosQueueIndex,
                                          &agentCosQueueEntryData.agentCosQueueSchedulerType) == L7_SUCCESS)
      SET_VALID(I_agentCosQueueSchedulerType, agentCosQueueEntryData.valid);
    if (nominator != -1) break;

  case I_agentCosQueueMinBandwidth:
    if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                              L7_COS_QUEUE_MIN_BW_FEATURE_ID) == L7_TRUE &&
        snmpAgentCosQueueMinBandwidthGet(USMDB_UNIT_CURRENT, intIfNum, agentCosQueueEntryData.agentCosQueueIndex,
                                         &agentCosQueueEntryData.agentCosQueueMinBandwidth) == L7_SUCCESS)
      SET_VALID(I_agentCosQueueMinBandwidth, agentCosQueueEntryData.valid);
    if (nominator != -1) break;

  case I_agentCosQueueMaxBandwidth:
    if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                              L7_COS_QUEUE_MAX_BW_FEATURE_ID) == L7_TRUE &&
        snmpAgentCosQueueMaxBandwidthGet(USMDB_UNIT_CURRENT, intIfNum, agentCosQueueEntryData.agentCosQueueIndex,
                                         &agentCosQueueEntryData.agentCosQueueMaxBandwidth) == L7_SUCCESS)
      SET_VALID(I_agentCosQueueMaxBandwidth, agentCosQueueEntryData.valid);
    if (nominator != -1) break;

  case I_agentCosQueueMgmtType:
    if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                              L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID) == L7_FALSE &&
        snmpAgentCosQueueMgmtTypeGet(USMDB_UNIT_CURRENT, intIfNum, agentCosQueueEntryData.agentCosQueueIndex,
                                     &agentCosQueueEntryData.agentCosQueueMgmtType) == L7_SUCCESS)
      SET_VALID(I_agentCosQueueMgmtType, agentCosQueueEntryData.valid);
    break;
  
  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentCosQueueEntryData.valid))
    return(NULL);

  return(&agentCosQueueEntryData);
}

#ifdef SETS
int
k_agentCosQueueEntry_test(ObjectInfo *object, ObjectSyntax *value,
                          doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentCosQueueEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentCosQueueEntry_set_defaults(doList_t *dp)
{
  agentCosQueueEntry_t *data = (agentCosQueueEntry_t *) (dp->data);

  data->agentCosQueueSchedulerType = D_agentCosQueueSchedulerType_weighted;
  data->agentCosQueueMgmtType = D_agentCosQueueMgmtType_taildrop;

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentCosQueueEntry_set(agentCosQueueEntry_t *data,
                         ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_uint32 intIfNum = 0;

  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if ((snmpAgentCosQueueEntryGet(USMDB_UNIT_CURRENT, data->agentCosQueueIntfIndex, data->agentCosQueueIndex, &intIfNum) != L7_SUCCESS) ||
      (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                             L7_COS_FEATURE_SUPPORTED) != L7_TRUE) ||
      ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                             L7_COS_QUEUE_CFG_PER_INTF_FEATURE_ID) != L7_TRUE) &&
       (data->agentCosQueueIntfIndex != 0)))
    return(COMMIT_FAILED_ERROR);

  if (VALID(I_agentCosQueueSchedulerType, data->valid))
  {
    if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                               L7_COS_QUEUE_SCHED_STRICT_ONLY_FEATURE_ID) != L7_FALSE) ||
        (snmpAgentCosQueueSchedulerTypeSet(USMDB_UNIT_CURRENT, intIfNum, data->agentCosQueueIndex,
                                          data->agentCosQueueSchedulerType) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentCosQueueSchedulerType, tempValid);
    }
  }

  if (VALID(I_agentCosQueueMinBandwidth, data->valid))
  {
    if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                               L7_COS_QUEUE_MIN_BW_FEATURE_ID) != L7_TRUE) ||
        (snmpAgentCosQueueMinBandwidthSet(USMDB_UNIT_CURRENT, intIfNum, data->agentCosQueueIndex,
                                          data->agentCosQueueMinBandwidth) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentCosQueueMinBandwidth, tempValid);
    }
  }

  if (VALID(I_agentCosQueueMaxBandwidth, data->valid))
  {
    if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                               L7_COS_QUEUE_MAX_BW_FEATURE_ID) != L7_TRUE) ||
        (snmpAgentCosQueueMaxBandwidthSet(USMDB_UNIT_CURRENT, intIfNum, data->agentCosQueueIndex,
                                          data->agentCosQueueMaxBandwidth) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentCosQueueMaxBandwidth, tempValid);
    }
  }

  if (VALID(I_agentCosQueueMgmtType, data->valid))
  {
    if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                               L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID) != L7_FALSE) ||
        (snmpAgentCosQueueMgmtTypeSet(USMDB_UNIT_CURRENT, intIfNum, data->agentCosQueueIndex,
                                      data->agentCosQueueMgmtType) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentCosQueueMgmtType, tempValid);
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentCosQueueEntry_UNDO
/* add #define SR_agentCosQueueEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentCosQueueEntry family.
 */
int
agentCosQueueEntry_undo(doList_t *doHead, doList_t *doCur,
                        ContextInfo *contextInfo)
{
  agentCosQueueEntry_t *data = (agentCosQueueEntry_t *) doCur->data;
  agentCosQueueEntry_t *undodata = (agentCosQueueEntry_t *) doCur->undodata;
  agentCosQueueEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return(UNDO_FAILED_ERROR);
  memcpy(undodata->valid, data->valid, sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if (k_agentCosQueueEntry_set(setdata, contextInfo, function) == NO_ERROR)
    return(NO_ERROR);

  return(UNDO_FAILED_ERROR);
}
#endif /* SR_agentCosQueueEntry_UNDO */

#endif /* SETS */

agentCosQueueMgmtEntry_t *
k_agentCosQueueMgmtEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_INT32 agentCosQueueIntfIndex,
                             SR_UINT32 agentCosQueueIndex,
                             SR_UINT32 agentCosQueueDropPrecIndex)
{
  static agentCosQueueMgmtEntry_t agentCosQueueMgmtEntryData;
  L7_uint32 intIfNum = 0;

  if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                             L7_COS_FEATURE_SUPPORTED) != L7_TRUE) ||
      ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                              L7_COS_QUEUE_DROP_CFG_PER_INTF_FEATURE_ID) != L7_TRUE) &&
       (agentCosQueueIntfIndex != 0)))
    return(NULL);

  ZERO_VALID(agentCosQueueMgmtEntryData.valid);
  agentCosQueueMgmtEntryData.agentCosQueueIntfIndex = agentCosQueueIntfIndex;
  SET_VALID(I_agentCosQueueMgmtEntryIndex_agentCosQueueIntfIndex, agentCosQueueMgmtEntryData.valid);
  agentCosQueueMgmtEntryData.agentCosQueueIndex = agentCosQueueIndex;
  SET_VALID(I_agentCosQueueMgmtEntryIndex_agentCosQueueIndex, agentCosQueueMgmtEntryData.valid);
  agentCosQueueMgmtEntryData.agentCosQueueDropPrecIndex = agentCosQueueDropPrecIndex;
  SET_VALID(I_agentCosQueueDropPrecIndex, agentCosQueueMgmtEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpAgentCosQueueMgmtEntryGet(USMDB_UNIT_CURRENT, 
                                     agentCosQueueMgmtEntryData.agentCosQueueIntfIndex,
                                     agentCosQueueMgmtEntryData.agentCosQueueIndex,
                                     agentCosQueueMgmtEntryData.agentCosQueueDropPrecIndex,
                                     &intIfNum) != L7_SUCCESS) :
      ((snmpAgentCosQueueMgmtEntryGet(USMDB_UNIT_CURRENT, 
                                      agentCosQueueMgmtEntryData.agentCosQueueIntfIndex,
                                      agentCosQueueMgmtEntryData.agentCosQueueIndex,
                                      agentCosQueueMgmtEntryData.agentCosQueueDropPrecIndex,
                                      &intIfNum) != L7_SUCCESS) &&
       (snmpAgentCosQueueMgmtEntryNextGet(USMDB_UNIT_CURRENT, 
                                          &agentCosQueueMgmtEntryData.agentCosQueueIntfIndex,
                                          &agentCosQueueMgmtEntryData.agentCosQueueIndex,
                                          &agentCosQueueMgmtEntryData.agentCosQueueDropPrecIndex,
                                          &intIfNum) != L7_SUCCESS)))
    return(NULL);

  switch (nominator)
  {
  case -1:
  case I_agentCosQueueMgmtEntryIndex_agentCosQueueIntfIndex:
  case I_agentCosQueueMgmtEntryIndex_agentCosQueueIndex:
  case I_agentCosQueueDropPrecIndex:
  case I_agentCosQueueMgmtTailDropThreshold:
  case I_agentCosQueueMgmtWredMinThreshold:
  case I_agentCosQueueMgmtWredMaxThreshold:
  case I_agentCosQueueMgmtWredDropProbScale:
    if (nominator != -1) break;

  case I_agentCosQueueMgmtPercentTailDropThreshold:
    if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                              L7_COS_QUEUE_TDROP_THRESH_FEATURE_ID) == L7_TRUE &&
        snmpAgentCosQueueMgmtTailDropThresholdGet(USMDB_UNIT_CURRENT, intIfNum, 
                                                  agentCosQueueMgmtEntryData.agentCosQueueIndex,
                                                  agentCosQueueMgmtEntryData.agentCosQueueDropPrecIndex,
                                                  &agentCosQueueMgmtEntryData.agentCosQueueMgmtPercentTailDropThreshold) == L7_SUCCESS)
      SET_VALID(I_agentCosQueueMgmtPercentTailDropThreshold, agentCosQueueMgmtEntryData.valid);
    if (nominator != -1) break;

  case I_agentCosQueueMgmtPercentWredMinThreshold:
    if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                              L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) == L7_TRUE &&
        snmpAgentCosQueueMgmtWredMinThresholdGet(USMDB_UNIT_CURRENT, intIfNum,
                                                 agentCosQueueMgmtEntryData.agentCosQueueIndex,
                                                 agentCosQueueMgmtEntryData.agentCosQueueDropPrecIndex,
                                                 &agentCosQueueMgmtEntryData.agentCosQueueMgmtPercentWredMinThreshold) == L7_SUCCESS)
      SET_VALID(I_agentCosQueueMgmtPercentWredMinThreshold, agentCosQueueMgmtEntryData.valid);
    if (nominator != -1) break;

  case I_agentCosQueueMgmtPercentWredMaxThreshold:
    if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                              L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) == L7_TRUE &&
        cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                              L7_COS_QUEUE_WRED_MAX_THRESH_FEATURE_ID) == L7_TRUE &&
        snmpAgentCosQueueMgmtWredMaxThresholdGet(USMDB_UNIT_CURRENT, intIfNum,
                                                 agentCosQueueMgmtEntryData.agentCosQueueIndex,
                                                 agentCosQueueMgmtEntryData.agentCosQueueDropPrecIndex,
                                                 &agentCosQueueMgmtEntryData.agentCosQueueMgmtPercentWredMaxThreshold) == L7_SUCCESS)
      SET_VALID(I_agentCosQueueMgmtPercentWredMaxThreshold, agentCosQueueMgmtEntryData.valid);
    if (nominator != -1) break;

  case I_agentCosQueueMgmtWredDropProbability:
    if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                              L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) == L7_TRUE &&
        snmpAgentCosQueueMgmtWredDropProbabilityGet(USMDB_UNIT_CURRENT, intIfNum,
                                                  agentCosQueueMgmtEntryData.agentCosQueueIndex,
                                                  agentCosQueueMgmtEntryData.agentCosQueueDropPrecIndex,
                                                  &agentCosQueueMgmtEntryData.agentCosQueueMgmtWredDropProbability) == L7_SUCCESS)
      SET_VALID(I_agentCosQueueMgmtWredDropProbability, agentCosQueueMgmtEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentCosQueueMgmtEntryData.valid))
    return(NULL);

  return(&agentCosQueueMgmtEntryData);
}

#ifdef SETS
int
k_agentCosQueueMgmtEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentCosQueueMgmtEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentCosQueueMgmtEntry_set_defaults(doList_t *dp)
{
  agentCosQueueMgmtEntry_t *data = (agentCosQueueMgmtEntry_t *) (dp->data);


  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentCosQueueMgmtEntry_set(agentCosQueueMgmtEntry_t *data,
                             ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_uint32 intIfNum = 0;

  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if ((snmpAgentCosQueueMgmtEntryGet(USMDB_UNIT_CURRENT, data->agentCosQueueIntfIndex, data->agentCosQueueIndex, data->agentCosQueueDropPrecIndex, &intIfNum) != L7_SUCCESS) ||
      (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                             L7_COS_FEATURE_SUPPORTED) != L7_TRUE) ||
      ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                              L7_COS_QUEUE_DROP_CFG_PER_INTF_FEATURE_ID) != L7_TRUE) &&
       (data->agentCosQueueIntfIndex != 0)))
    return(COMMIT_FAILED_ERROR);

  if (VALID(I_agentCosQueueMgmtPercentTailDropThreshold, data->valid))
  {
    if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                              L7_COS_QUEUE_TDROP_THRESH_FEATURE_ID) != L7_TRUE) ||
        (snmpAgentCosQueueMgmtTailDropThresholdSet(USMDB_UNIT_CURRENT, intIfNum, data->agentCosQueueIndex, data->agentCosQueueDropPrecIndex, 
                                                   data->agentCosQueueMgmtPercentTailDropThreshold) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentCosQueueMgmtPercentTailDropThreshold, tempValid);
    }
  }

  if (VALID(I_agentCosQueueMgmtPercentWredMinThreshold, data->valid))
  {
    if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                              L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) != L7_TRUE) ||
        (snmpAgentCosQueueMgmtWredMinThresholdSet(USMDB_UNIT_CURRENT, intIfNum, data->agentCosQueueIndex, data->agentCosQueueDropPrecIndex, 
                                                  data->agentCosQueueMgmtPercentWredMinThreshold) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentCosQueueMgmtPercentWredMinThreshold, tempValid);
    }
  }

  if (VALID(I_agentCosQueueMgmtPercentWredMaxThreshold, data->valid))
  {
    if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                              L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) != L7_TRUE) ||
        (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                               L7_COS_QUEUE_WRED_MAX_THRESH_FEATURE_ID) != L7_TRUE) ||
        (snmpAgentCosQueueMgmtWredMaxThresholdSet(USMDB_UNIT_CURRENT, intIfNum, data->agentCosQueueIndex, data->agentCosQueueDropPrecIndex, 
                                                  data->agentCosQueueMgmtPercentWredMaxThreshold) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentCosQueueMgmtPercentWredMaxThreshold, tempValid);
    }
  }

  if (VALID(I_agentCosQueueMgmtWredDropProbability, data->valid))
  {
    if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID,
                              L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) != L7_TRUE) ||
        (snmpAgentCosQueueMgmtWredDropProbabilitySet(USMDB_UNIT_CURRENT, intIfNum, data->agentCosQueueIndex, data->agentCosQueueDropPrecIndex, 
                                                   data->agentCosQueueMgmtWredDropProbability) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentCosQueueMgmtWredDropProbability, tempValid);
    }
  }

  if (VALID(I_agentCosQueueMgmtWredDropProbScale, data->valid) || VALID(I_agentCosQueueMgmtWredMaxThreshold, data->valid) || 
      VALID(I_agentCosQueueMgmtWredMinThreshold, data->valid) || VALID(I_agentCosQueueMgmtTailDropThreshold, data->valid)) 
  {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
  }

  return NO_ERROR;
}

#ifdef SR_agentCosQueueMgmtEntry_UNDO
/* add #define SR_agentCosQueueMgmtEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentCosQueueMgmtEntry family.
 */
int
agentCosQueueMgmtEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
  agentCosQueueMgmtEntry_t *data = (agentCosQueueMgmtEntry_t *) doCur->data;
  agentCosQueueMgmtEntry_t *undodata = (agentCosQueueMgmtEntry_t *) doCur->undodata;
  agentCosQueueMgmtEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return(UNDO_FAILED_ERROR);
  memcpy(undodata->valid, data->valid, sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if (k_agentCosQueueMgmtEntry_set(setdata, contextInfo, function) == NO_ERROR)
    return(NO_ERROR);

  return(UNDO_FAILED_ERROR);
}
#endif /* SR_agentCosQueueMgmtEntry_UNDO */

#endif /* SETS */

