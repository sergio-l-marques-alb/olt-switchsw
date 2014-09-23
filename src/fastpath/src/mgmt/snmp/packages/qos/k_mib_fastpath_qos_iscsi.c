/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename k_mib_fastpath_qos_iscsi.c
*
* @purpose  Provide interface to iSCSI MIB
*                  
* @component SNMP
*
* @comments 
*
* @create 10/20/2008
*
* @author jeffr
* @end
*
**********************************************************************/
#include "k_private_base.h"
#include "k_mib_fastpath_qos_iscsi_api.h"
#include "usmdb_common.h"
#include "iscsi_exports.h"
#include "usmdb_util_api.h"

#ifdef L7_ISCSI_PACKAGE
  #include "usmdb_qos_iscsi.h"
#endif

agentIscsiFlowAccelerationGlobalConfigGroup_t *
k_agentIscsiFlowAccelerationGlobalConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                     int nominator)
{
  static agentIscsiFlowAccelerationGlobalConfigGroup_t agentIscsiFlowAccelerationGlobalConfigGroupData;

  if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_ISCSI_COMPONENT_ID,
                             L7_ISCSI_FEATURE_SUPPORTED) != L7_TRUE))
    return(NULL);

  ZERO_VALID(agentIscsiFlowAccelerationGlobalConfigGroupData.valid);

  switch (nominator)
  {
  case I_agentIscsiFlowAccelerationEnable:
    if (snmpAgentIscsiEnableGet(&agentIscsiFlowAccelerationGlobalConfigGroupData.agentIscsiFlowAccelerationEnable) == L7_SUCCESS)
      SET_VALID(I_agentIscsiFlowAccelerationEnable, agentIscsiFlowAccelerationGlobalConfigGroupData.valid);
    break;

  case I_agentIscsiFlowAccelerationAgingTimeOut:
    if (snmpAgentIscsiAgingTimeOutGet(&agentIscsiFlowAccelerationGlobalConfigGroupData.agentIscsiFlowAccelerationAgingTimeOut) == L7_SUCCESS)
      SET_VALID(I_agentIscsiFlowAccelerationAgingTimeOut, agentIscsiFlowAccelerationGlobalConfigGroupData.valid);
    break;

  case I_agentIscsiFlowAccelerationQosType:
    if (snmpAgentIscsiQosTypeGet(&agentIscsiFlowAccelerationGlobalConfigGroupData.agentIscsiFlowAccelerationQosType) == L7_SUCCESS)
      SET_VALID(I_agentIscsiFlowAccelerationQosType, agentIscsiFlowAccelerationGlobalConfigGroupData.valid);
    break;

  case I_agentIscsiFlowAccelerationQosVptValue:
    if (snmpAgentIscsiQosVptValueGet(&agentIscsiFlowAccelerationGlobalConfigGroupData.agentIscsiFlowAccelerationQosVptValue) == L7_SUCCESS)
      SET_VALID(I_agentIscsiFlowAccelerationQosVptValue, agentIscsiFlowAccelerationGlobalConfigGroupData.valid);
    break;

  case I_agentIscsiFlowAccelerationQosDscpValue:
    if (snmpAgentIscsiQosDscpValueGet(&agentIscsiFlowAccelerationGlobalConfigGroupData.agentIscsiFlowAccelerationQosDscpValue) == L7_SUCCESS)
      SET_VALID(I_agentIscsiFlowAccelerationQosDscpValue, agentIscsiFlowAccelerationGlobalConfigGroupData.valid);
    break;

  case I_agentIscsiFlowAccelerationQosRemark:
    if (snmpAgentIscsiQosRemarkGet(&agentIscsiFlowAccelerationGlobalConfigGroupData.agentIscsiFlowAccelerationQosRemark) == L7_SUCCESS)
      SET_VALID(I_agentIscsiFlowAccelerationQosRemark, agentIscsiFlowAccelerationGlobalConfigGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentIscsiFlowAccelerationGlobalConfigGroupData.valid))
    return(NULL);

  return(&agentIscsiFlowAccelerationGlobalConfigGroupData);
}

#ifdef SETS
int
k_agentIscsiFlowAccelerationGlobalConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentIscsiFlowAccelerationGlobalConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                                       doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentIscsiFlowAccelerationGlobalConfigGroup_set(agentIscsiFlowAccelerationGlobalConfigGroup_t *data,
                                     ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];

  memset(tempValid, 0, sizeof(tempValid));

  if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_ISCSI_COMPONENT_ID,
                             L7_ISCSI_FEATURE_SUPPORTED) != L7_TRUE))
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentIscsiFlowAccelerationEnable, data->valid))
  {
    if (snmpAgentIscsiEnableSet(data->agentIscsiFlowAccelerationEnable) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentIscsiFlowAccelerationEnable, tempValid);
    }
  }

  if (VALID(I_agentIscsiFlowAccelerationAgingTimeOut, data->valid))
  {
    if (snmpAgentIscsiAgingTimeOutSet(data->agentIscsiFlowAccelerationAgingTimeOut) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentIscsiFlowAccelerationAgingTimeOut, tempValid);
    }
  }

  if (VALID(I_agentIscsiFlowAccelerationQosType, data->valid))
  {
    if (snmpAgentIscsiQosTypeSet(data->agentIscsiFlowAccelerationQosType) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentIscsiFlowAccelerationQosType, tempValid);
    }
  }

  if (VALID(I_agentIscsiFlowAccelerationQosVptValue, data->valid))
  {
    if (snmpAgentIscsiQosVptValueSet(data->agentIscsiFlowAccelerationQosVptValue) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentIscsiFlowAccelerationQosVptValue, tempValid);
    }
  }

  if (VALID(I_agentIscsiFlowAccelerationQosDscpValue, data->valid))
  {
    if (snmpAgentIscsiQosDscpValueSet(data->agentIscsiFlowAccelerationQosDscpValue) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentIscsiFlowAccelerationQosDscpValue, tempValid);
    }
  }

  if (VALID(I_agentIscsiFlowAccelerationQosRemark, data->valid))
  {
    if (snmpAgentIscsiQosRemarkSet(data->agentIscsiFlowAccelerationQosRemark) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentIscsiFlowAccelerationQosRemark, tempValid);
    }
  }

  return NO_ERROR;
}

  #ifdef SR_agentIscsiFlowAccelerationGlobalConfigGroup_UNDO
/* add #define SR_agentIscsiFlowAccelerationGlobalConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentIscsiFlowAccelerationGlobalConfigGroup family.
 */
int
agentIscsiFlowAccelerationGlobalConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                    ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentIscsiFlowAccelerationGlobalConfigGroup_UNDO */

#endif /* SETS */

agentIscsiFlowAccelerationTargetConfigEntry_t *
k_agentIscsiFlowAccelerationTargetConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                                     int nominator,
                                     int searchType,
                                     SR_UINT32 agentIscsiFlowAccelerationTargetConfigTcpPort,
                                     SR_UINT32 agentIscsiFlowAccelerationTargetConfigAddr)
{
  static agentIscsiFlowAccelerationTargetConfigEntry_t agentIscsiFlowAccelerationTargetConfigEntryData;
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentIscsiFlowAccelerationTargetConfigEntryData.agentIscsiFlowAccelerationTargetConfigName = MakeOctetString(NULL, 0);
  }

  if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_ISCSI_COMPONENT_ID,
                             L7_ISCSI_FEATURE_SUPPORTED) != L7_TRUE))
    return(NULL);

  ZERO_VALID(agentIscsiFlowAccelerationTargetConfigEntryData.valid);

  agentIscsiFlowAccelerationTargetConfigEntryData.agentIscsiFlowAccelerationTargetConfigTcpPort = agentIscsiFlowAccelerationTargetConfigTcpPort;
  agentIscsiFlowAccelerationTargetConfigEntryData.agentIscsiFlowAccelerationTargetConfigAddr = agentIscsiFlowAccelerationTargetConfigAddr;

  SET_VALID(I_agentIscsiFlowAccelerationTargetConfigTcpPort, agentIscsiFlowAccelerationTargetConfigEntryData.valid);
  SET_VALID(I_agentIscsiFlowAccelerationTargetConfigAddr, agentIscsiFlowAccelerationTargetConfigEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpAgentIscsiTargetConfigEntryGet(&agentIscsiFlowAccelerationTargetConfigEntryData, nominator) != L7_SUCCESS) :
      ((snmpAgentIscsiTargetConfigEntryGet(&agentIscsiFlowAccelerationTargetConfigEntryData, nominator) != L7_SUCCESS) &&
       (snmpAgentIscsiTargetConfigEntryGetNext(&agentIscsiFlowAccelerationTargetConfigEntryData, nominator) != L7_SUCCESS)))
  {
    ZERO_VALID(agentIscsiFlowAccelerationTargetConfigEntryData.valid);
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, agentIscsiFlowAccelerationTargetConfigEntryData.valid))
    return(NULL);

  return(&agentIscsiFlowAccelerationTargetConfigEntryData);
}

#ifdef SETS
int
k_agentIscsiFlowAccelerationTargetConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentIscsiFlowAccelerationTargetConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                       doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentIscsiFlowAccelerationTargetConfigEntry_set_defaults(doList_t *dp)
{
  agentIscsiFlowAccelerationTargetConfigEntry_t *data = (agentIscsiFlowAccelerationTargetConfigEntry_t *) (dp->data);

  if ((data->agentIscsiFlowAccelerationTargetConfigName = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentIscsiFlowAccelerationTargetConfigEntry_set(agentIscsiFlowAccelerationTargetConfigEntry_t *data,
                                     ContextInfo *contextInfo, int function)
{
  L7_RC_t rc = L7_FAILURE;

  if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_ISCSI_COMPONENT_ID,
                             L7_ISCSI_FEATURE_SUPPORTED) != L7_TRUE))
    return COMMIT_FAILED_ERROR;

  if (((VALID(I_agentIscsiFlowAccelerationTargetConfigStatus, data->valid)) &&       
       ((data->agentIscsiFlowAccelerationTargetConfigStatus == D_agentIscsiFlowAccelerationTargetConfigStatus_destroy) ||
        (data->agentIscsiFlowAccelerationTargetConfigStatus == D_agentIscsiFlowAccelerationTargetConfigStatus_createAndGo))) ||
      (VALID(I_agentIscsiFlowAccelerationTargetConfigName, data->valid)))
  {
    rc = snmpAgentIscsiTargetConfigEntrySet(data);
  }

  if(rc == L7_SUCCESS)
  {
    return NO_ERROR;
  }
  else
  {
    return COMMIT_FAILED_ERROR;
  }
}

  #ifdef SR_agentIscsiFlowAccelerationTargetConfigEntry_UNDO
/* add #define SR_agentIscsiFlowAccelerationTargetConfigEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentIscsiFlowAccelerationTargetConfigEntry family.
 */
int
agentIscsiFlowAccelerationTargetConfigEntry_undo(doList_t *doHead, doList_t *doCur,
                                    ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentIscsiFlowAccelerationTargetConfigEntry_UNDO */

#endif /* SETS */

agentIscsiFlowAccelerationSessionEntry_t *
k_agentIscsiFlowAccelerationSessionEntry_get(int serialNum, ContextInfo *contextInfo,
                                int nominator,
                                int searchType,
                                SR_UINT32 agentIscsiFlowAccelerationSessionIndex)
{
  static agentIscsiFlowAccelerationSessionEntry_t agentIscsiFlowAccelerationSessionEntryData;
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentIscsiFlowAccelerationSessionEntryData.agentIscsiFlowAccelerationTargetName = MakeOctetString(NULL, 0);
    agentIscsiFlowAccelerationSessionEntryData.agentIscsiFlowAccelerationInitiatorName = MakeOctetString(NULL, 0);
    agentIscsiFlowAccelerationSessionEntryData.agentIscsiFlowAccelerationSessionISID = MakeOctetString(NULL, 0);
  }

  if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_ISCSI_COMPONENT_ID,
                             L7_ISCSI_FEATURE_SUPPORTED) != L7_TRUE))
    return(NULL);

  ZERO_VALID(agentIscsiFlowAccelerationSessionEntryData.valid);

  agentIscsiFlowAccelerationSessionEntryData.agentIscsiFlowAccelerationSessionIndex = agentIscsiFlowAccelerationSessionIndex;
  SET_VALID(I_agentIscsiFlowAccelerationSessionIndex, agentIscsiFlowAccelerationSessionEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpAgentIscsiFlowAccelerationSessionEntryGet(&agentIscsiFlowAccelerationSessionEntryData, nominator) != L7_SUCCESS) :
      ((snmpAgentIscsiFlowAccelerationSessionEntryGet(&agentIscsiFlowAccelerationSessionEntryData, nominator) != L7_SUCCESS) &&
       (snmpAgentIscsiFlowAccelerationSessionEntryGetNext(&agentIscsiFlowAccelerationSessionEntryData, nominator) != L7_SUCCESS)))
  {
    ZERO_VALID(agentIscsiFlowAccelerationSessionEntryData.valid);
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, agentIscsiFlowAccelerationSessionEntryData.valid))
    return(NULL);

  return(&agentIscsiFlowAccelerationSessionEntryData);
}

agentIscsiFlowAccelerationConnectionEntry_t *
k_agentIscsiFlowAccelerationConnectionEntry_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator,
                                   int searchType,
                                   SR_UINT32 agentIscsiFlowAccelerationConnectionIndex)
{
  static agentIscsiFlowAccelerationConnectionEntry_t agentIscsiFlowAccelerationConnectionEntryData;

  if ((cnfgrIsFeaturePresent(L7_FLEX_QOS_ISCSI_COMPONENT_ID,
                             L7_ISCSI_FEATURE_SUPPORTED) != L7_TRUE))
    return(NULL);

  ZERO_VALID(agentIscsiFlowAccelerationConnectionEntryData.valid);

  agentIscsiFlowAccelerationConnectionEntryData.agentIscsiFlowAccelerationConnectionIndex = agentIscsiFlowAccelerationConnectionIndex;
  SET_VALID(I_agentIscsiFlowAccelerationConnectionIndex, agentIscsiFlowAccelerationConnectionEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpAgentIscsiFlowAccelerationConnectionEntryGet(&agentIscsiFlowAccelerationConnectionEntryData, nominator) != L7_SUCCESS) :
      ((snmpAgentIscsiFlowAccelerationConnectionEntryGet(&agentIscsiFlowAccelerationConnectionEntryData, nominator) != L7_SUCCESS) &&
       (snmpAgentIscsiFlowAccelerationConnectionEntryGetNext(&agentIscsiFlowAccelerationConnectionEntryData, nominator) != L7_SUCCESS)))
  {
    ZERO_VALID(agentIscsiFlowAccelerationConnectionEntryData.valid);
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, agentIscsiFlowAccelerationConnectionEntryData.valid))
    return(NULL);

  return(&agentIscsiFlowAccelerationConnectionEntryData);
}

