/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   k_mib_fastpathlogging.c
*
* @purpose    Private MIB for configuring and displaying system event logs.
*
* @component  SNMP
*
* @comments
*
* @create     2/17/2004
*
* @author     cpverne
* @end
*
**********************************************************************/
#include "k_private_base.h"
#include "k_mib_fastpathlogging_api.h"
#include "usmdb_log_api.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"
#include "log_exports.h"

agentLogInMemoryConfigGroup_t *
k_agentLogInMemoryConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                  int nominator)
{
  static agentLogInMemoryConfigGroup_t agentLogInMemoryConfigGroupData;

  switch (nominator)
  {
  case -1:
  case I_agentLogInMemoryAdminStatus:
    if (snmpAgentLogInMemoryAdminStatusGet(&agentLogInMemoryConfigGroupData.agentLogInMemoryAdminStatus) == L7_SUCCESS)
      SET_VALID(I_agentLogInMemoryAdminStatus, agentLogInMemoryConfigGroupData.valid);
    if (nominator != -1)
      break;
  
  case I_agentLogInMemoryBehavior:
    if (snmpAgentLogInMemoryBehaviorGet(&agentLogInMemoryConfigGroupData.agentLogInMemoryBehavior) == L7_SUCCESS)
      SET_VALID(I_agentLogInMemoryBehavior, agentLogInMemoryConfigGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != -1 && !VALID(nominator, agentLogInMemoryConfigGroupData.valid))
    return(NULL);

  return(&agentLogInMemoryConfigGroupData);
}

#ifdef SETS
int
k_agentLogInMemoryConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                   doList_t *dp, ContextInfo *contextInfo)
{

  return(NO_ERROR);
}

int
k_agentLogInMemoryConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                                    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return(NO_ERROR);
}

int
k_agentLogInMemoryConfigGroup_set(agentLogInMemoryConfigGroup_t *data,
                                  ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];

  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_agentLogInMemoryAdminStatus, data->valid))
  {
    if (snmpAgentLogInMemoryAdminStatusSet(data->agentLogInMemoryAdminStatus) != L7_SUCCESS)
    {
      /* overwrite the valid bits with the currently set objects */
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    /* record this as being set */
    SET_VALID(I_agentLogInMemoryAdminStatus, tempValid);
  }

  
  if (VALID(I_agentLogInMemoryBehavior, data->valid))
  {
    if (snmpAgentLogInMemoryBehaviorSet(data->agentLogInMemoryBehavior) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    SET_VALID(I_agentLogInMemoryBehavior, tempValid);
  }

  return(NO_ERROR);
}

  #ifdef SR_agentLogInMemoryConfigGroup_UNDO
/* add #define SR_agentLogInMemoryConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentLogInMemoryConfigGroup family.
 */
int
agentLogInMemoryConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                 ContextInfo *contextInfo)
{
  agentLogInMemoryConfigGroup_t *data = (agentLogInMemoryConfigGroup_t *) doCur->data;
  agentLogInMemoryConfigGroup_t *undodata = (agentLogInMemoryConfigGroup_t *) doCur->undodata;
  agentLogInMemoryConfigGroup_t *setdata = NULL;
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
  if (k_agentLogInMemoryConfigGroup_set(setdata, contextInfo, function) == NO_ERROR)
    return(NO_ERROR);

  return(UNDO_FAILED_ERROR);
}
  #endif /* SR_agentLogInMemoryConfigGroup_UNDO */

#endif /* SETS */

agentLogConsoleConfigGroup_t *
k_agentLogConsoleConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                 int nominator)
{
  static agentLogConsoleConfigGroup_t agentLogConsoleConfigGroupData;

  switch (nominator)
  {
  case -1:

  case I_agentLogConsoleAdminStatus:
    if (snmpAgentLogConsoleAdminStatusGet(&agentLogConsoleConfigGroupData.agentLogConsoleAdminStatus) == L7_SUCCESS)
      SET_VALID(I_agentLogConsoleAdminStatus, agentLogConsoleConfigGroupData.valid);
    if (nominator != -1)
      break;

  case I_agentLogConsoleSeverityFilter:
    if (snmpAgentLogConsoleSeverityFilterGet(&agentLogConsoleConfigGroupData.agentLogConsoleSeverityFilter) == L7_SUCCESS)
      SET_VALID(I_agentLogConsoleSeverityFilter, agentLogConsoleConfigGroupData.valid);
      break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != -1 && !VALID(nominator, agentLogConsoleConfigGroupData.valid))
    return(NULL);

  return(&agentLogConsoleConfigGroupData);
}

#ifdef SETS
int
k_agentLogConsoleConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                  doList_t *dp, ContextInfo *contextInfo)
{

  return(NO_ERROR);
}

int
k_agentLogConsoleConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                                   doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return(NO_ERROR);
}

int
k_agentLogConsoleConfigGroup_set(agentLogConsoleConfigGroup_t *data,
                                 ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];

  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_agentLogConsoleAdminStatus, data->valid))
  {
    if (snmpAgentLogConsoleAdminStatusSet(data->agentLogConsoleAdminStatus) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    
    SET_VALID(I_agentLogConsoleAdminStatus, tempValid);
  }

  if (VALID(I_agentLogConsoleSeverityFilter, data->valid))
  {
    if (snmpAgentLogConsoleSeverityFilterSet(data->agentLogConsoleSeverityFilter) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    
    SET_VALID(I_agentLogConsoleSeverityFilter, tempValid);
  }

  return(NO_ERROR);
}

  #ifdef SR_agentLogConsoleConfigGroup_UNDO
/* add #define SR_agentLogConsoleConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentLogConsoleConfigGroup family.
 */
int
agentLogConsoleConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                ContextInfo *contextInfo)
{
  agentLogConsoleConfigGroup_t *data = (agentLogConsoleConfigGroup_t *) doCur->data;
  agentLogConsoleConfigGroup_t *undodata = (agentLogConsoleConfigGroup_t *) doCur->undodata;
  agentLogConsoleConfigGroup_t *setdata = NULL;
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
  if (k_agentLogConsoleConfigGroup_set(setdata, contextInfo, function) == NO_ERROR)
    return(NO_ERROR);

  return(UNDO_FAILED_ERROR);
}
  #endif /* SR_agentLogConsoleConfigGroup_UNDO */

#endif /* SETS */



agentLogPersistentConfigGroup_t *
k_agentLogPersistentConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                    int nominator)
{
  static agentLogPersistentConfigGroup_t agentLogPersistentConfigGroupData;

  switch (nominator)
  {
  case -1:

  case I_agentLogPersistentAdminStatus:
    if (snmpAgentLogPersistentAdminStatusGet(&agentLogPersistentConfigGroupData.agentLogPersistentAdminStatus) == L7_SUCCESS)
      SET_VALID(I_agentLogPersistentAdminStatus, agentLogPersistentConfigGroupData.valid);
    if (nominator != -1)
      break;

  case I_agentLogPersistentSeverityFilter:
    if (snmpAgentLogPersistentSeverityFilterGet(&agentLogPersistentConfigGroupData.agentLogPersistentSeverityFilter) == L7_SUCCESS)
      SET_VALID(I_agentLogPersistentSeverityFilter, agentLogPersistentConfigGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != -1 && !VALID(nominator, agentLogPersistentConfigGroupData.valid))
    return(NULL);

  return(&agentLogPersistentConfigGroupData);
}

#ifdef SETS
int
k_agentLogPersistentConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *dp, ContextInfo *contextInfo)
{
  return(NO_ERROR);
}

int
k_agentLogPersistentConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                                      doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return(NO_ERROR);
}

int
k_agentLogPersistentConfigGroup_set(agentLogPersistentConfigGroup_t *data,
                                    ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];

  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_agentLogPersistentAdminStatus, data->valid))
  {
    if (snmpAgentLogPersistentAdminStatusSet(data->agentLogPersistentAdminStatus) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    
    SET_VALID(I_agentLogPersistentAdminStatus, tempValid);
  }

  if (VALID(I_agentLogPersistentSeverityFilter, data->valid))
  {
    if (snmpAgentLogPersistentSeverityFilterSet(data->agentLogPersistentSeverityFilter) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    
    SET_VALID(I_agentLogPersistentSeverityFilter, tempValid);
  }

  return(NO_ERROR);
}

  #ifdef SR_agentLogPersistentConfigGroup_UNDO
/* add #define SR_agentLogPersistentConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentLogPersistentConfigGroup family.
 */
int
agentLogPersistentConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                   ContextInfo *contextInfo)
{
  agentLogPersistentConfigGroup_t *data = (agentLogPersistentConfigGroup_t *) doCur->data;
  agentLogPersistentConfigGroup_t *undodata = (agentLogPersistentConfigGroup_t *) doCur->undodata;
  agentLogPersistentConfigGroup_t *setdata = NULL;
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
  if (k_agentLogPersistentConfigGroup_set(setdata, contextInfo, function) == NO_ERROR)
    return(NO_ERROR);

  return(UNDO_FAILED_ERROR);
}
  #endif /* SR_agentLogPersistentConfigGroup_UNDO */

#endif /* SETS */

agentLogSysLogConfigGroup_t *
k_agentLogSysLogConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                int nominator)
{
  static agentLogSysLogConfigGroup_t agentLogSysLogConfigGroupData;
  L7_ushort16 temp_short;

  switch (nominator)
  {
  case -1:

  case I_agentLogSyslogAdminStatus:
    if (snmpAgentLogSyslogAdminStatusGet(&agentLogSysLogConfigGroupData.agentLogSyslogAdminStatus) == L7_SUCCESS)
      SET_VALID(I_agentLogSyslogAdminStatus, agentLogSysLogConfigGroupData.valid);
    if (nominator != -1)
      break;

  case I_agentLogSyslogLocalPort:
    if (usmDbLogLocalPortGet(USMDB_UNIT_CURRENT, &temp_short) == L7_SUCCESS)
    {
      /* cast the short into a unsigned int */
      agentLogSysLogConfigGroupData.agentLogSyslogLocalPort = temp_short;
      SET_VALID(I_agentLogSyslogLocalPort, agentLogSysLogConfigGroupData.valid);
    }
    if (nominator != -1)
      break;

  case I_agentLogSyslogMaxHosts:
    if (usmDbLogMaxSyslogHostsGet(USMDB_UNIT_CURRENT, 
                                  &agentLogSysLogConfigGroupData.agentLogSyslogMaxHosts) == L7_SUCCESS)
      SET_VALID(I_agentLogSyslogMaxHosts, agentLogSysLogConfigGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != -1 && !VALID(nominator, agentLogSysLogConfigGroupData.valid))
    return(NULL);

  return(&agentLogSysLogConfigGroupData);
}

#ifdef SETS
int
k_agentLogSysLogConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *dp, ContextInfo *contextInfo)
{

  return(NO_ERROR);
}

int
k_agentLogSysLogConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                                  doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return(NO_ERROR);
}

int
k_agentLogSysLogConfigGroup_set(agentLogSysLogConfigGroup_t *data,
                                ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];
  L7_RC_t rc;

  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_agentLogSyslogAdminStatus, data->valid))
  {
    if (snmpAgentLogSyslogAdminStatusSet(data->agentLogSyslogAdminStatus) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    
    SET_VALID(I_agentLogSyslogAdminStatus, tempValid);
  }

  if (VALID(I_agentLogSyslogLocalPort, data->valid))
  {
    rc = usmDbLogLocalPortSet(USMDB_UNIT_CURRENT, 
                              (L7_ushort16)data->agentLogSyslogLocalPort);
    if ((rc != L7_SUCCESS) && (rc != L7_ALREADY_CONFIGURED))
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    
    SET_VALID(I_agentLogSyslogLocalPort, tempValid);
  }

  return(NO_ERROR);
}

  #ifdef SR_agentLogSysLogConfigGroup_UNDO
/* add #define SR_agentLogSysLogConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentLogSysLogConfigGroup family.
 */
int
agentLogSysLogConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                               ContextInfo *contextInfo)
{
  agentLogSysLogConfigGroup_t *data = (agentLogSysLogConfigGroup_t *) doCur->data;
  agentLogSysLogConfigGroup_t *undodata = (agentLogSysLogConfigGroup_t *) doCur->undodata;
  agentLogSysLogConfigGroup_t *setdata = NULL;
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
  if (k_agentLogSysLogConfigGroup_set(setdata, contextInfo, function) == NO_ERROR)
    return(NO_ERROR);

  return(UNDO_FAILED_ERROR);
}
  #endif /* SR_agentLogSysLogConfigGroup_UNDO */

#endif /* SETS */

agentLogSyslogHostEntry_t *
k_agentLogSyslogHostEntry_get(int serialNum, ContextInfo *contextInfo,
                              int nominator,
                              int searchType,
                              SR_UINT32 agentLogHostTableIndex)
{
  static agentLogSyslogHostEntry_t agentLogSyslogHostEntryData;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  static L7_BOOL firstTime = L7_TRUE;
  L7_ushort16 temp_short;

  if (firstTime == L7_TRUE)
  {
    agentLogSyslogHostEntryData.agentLogHostTableIpAddress = MakeOctetString(NULL,0);
    firstTime = L7_FALSE;
  }

  agentLogSyslogHostEntryData.agentLogHostTableIndex = agentLogHostTableIndex;
  SET_VALID(I_agentLogHostTableIndex, agentLogSyslogHostEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpAgentLogSyslogHostEntryGet(agentLogSyslogHostEntryData.agentLogHostTableIndex) != L7_SUCCESS) :
      ((snmpAgentLogSyslogHostEntryGet(agentLogSyslogHostEntryData.agentLogHostTableIndex) != L7_SUCCESS) &&
       (snmpAgentLogSyslogHostEntryNextGet(&agentLogSyslogHostEntryData.agentLogHostTableIndex) != L7_SUCCESS)))
    return(NULL);

  switch (nominator)
  {
    case I_agentLogHostTableIndex:
      break;

    case -1:

    case I_agentLogHostTableIpAddress:
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
      if (usmDbLogHostTableIpAddressGet(USMDB_UNIT_CURRENT, 
                                        agentLogSyslogHostEntryData.agentLogHostTableIndex,
                                        snmp_buffer) == L7_SUCCESS &&
          SafeMakeOctetStringFromTextExact(&agentLogSyslogHostEntryData.agentLogHostTableIpAddress,
                                           snmp_buffer) == L7_TRUE)
        SET_VALID(I_agentLogHostTableIpAddress, agentLogSyslogHostEntryData.valid);
      if (nominator != -1)
        break;

    case I_agentLogHostTableIpAddressType:
      if (snmpAgentLogHostIpAddressTypeGet(agentLogSyslogHostEntryData.agentLogHostTableIndex,
                                                 &agentLogSyslogHostEntryData.agentLogHostTableIpAddressType) == L7_SUCCESS)
        SET_VALID(I_agentLogHostTableIpAddressType, agentLogSyslogHostEntryData.valid);
      if (nominator != -1)
        break;

    case I_agentLogHostTablePort:
      if (usmDbLogHostTablePortGet(USMDB_UNIT_CURRENT, 
                                   agentLogSyslogHostEntryData.agentLogHostTableIndex,
                                   &temp_short) == L7_SUCCESS)
      {
        /* cast short to unsigned integer */
        agentLogSyslogHostEntryData.agentLogHostTablePort = (L7_uint32)temp_short;
        SET_VALID(I_agentLogHostTablePort, agentLogSyslogHostEntryData.valid);
      }
      if (nominator != -1)
        break;

    case I_agentLogHostTableSeverityFilter:
      if (snmpAgentLogHostTableSeverityFilterGet(agentLogSyslogHostEntryData.agentLogHostTableIndex,
                                                 &agentLogSyslogHostEntryData.agentLogHostTableSeverityFilter) == L7_SUCCESS)
        SET_VALID(I_agentLogHostTableSeverityFilter, agentLogSyslogHostEntryData.valid);
      if (nominator != -1)
        break;

    case I_agentLogHostTableRowStatus:
      if (snmpAgentLogHostTableRowStatusGet(agentLogSyslogHostEntryData.agentLogHostTableIndex,
                                            &agentLogSyslogHostEntryData.agentLogHostTableRowStatus) == L7_SUCCESS)
        SET_VALID(I_agentLogHostTableRowStatus, agentLogSyslogHostEntryData.valid);
      break;

    default:
      /* unknown nominator */
      return(NULL);
      break;
  }

  if (nominator != -1 && !VALID(nominator, agentLogSyslogHostEntryData.valid))
    return(NULL);

  return(&agentLogSyslogHostEntryData);
}

#ifdef SETS
int
k_agentLogSyslogHostEntry_test(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *dp, ContextInfo *contextInfo)
{

  return(NO_ERROR);
}

int
k_agentLogSyslogHostEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return(NO_ERROR);
}

int
k_agentLogSyslogHostEntry_set_defaults(doList_t *dp)
{
  agentLogSyslogHostEntry_t *data = (agentLogSyslogHostEntry_t *) (dp->data);

  if ((data->agentLogHostTableIpAddress = MakeOctetStringFromText("")) == 0)
  {
    return(RESOURCE_UNAVAILABLE_ERROR);
  }

  ZERO_VALID(data->valid);
  return(NO_ERROR);
}

int
k_agentLogSyslogHostEntry_set(agentLogSyslogHostEntry_t *data,
                              ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];
  L7_BOOL setRowStatus = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_RC_t rc;

  bzero(tempValid, sizeof(tempValid));

  /* validate index values */
  if (snmpAgentLogSyslogHostEntryGet(data->agentLogHostTableIndex) != L7_SUCCESS)
  {
    /* perform row creation */
    if (VALID(I_agentLogHostTableRowStatus, data->valid) &&
        snmpAgentLogHostTableRowStatusSet(data->agentLogHostTableIndex,
                                          data->agentLogHostTableRowStatus) == L7_SUCCESS)
    {
      SET_VALID(I_agentLogHostTableRowStatus, tempValid);
      setRowStatus = L7_FALSE;
    }
    else
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
  }

  if (VALID(I_agentLogHostTableIpAddress, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    strncpy(snmp_buffer, data->agentLogHostTableIpAddress->octet_ptr, data->agentLogHostTableIpAddress->length);

    rc = usmDbLogHostTableHostSet(USMDB_UNIT_CURRENT, 
                                  data->agentLogHostTableIndex, snmp_buffer);
    if ((rc != L7_SUCCESS) && (rc != L7_ALREADY_CONFIGURED))
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    
    SET_VALID(I_agentLogHostTableIpAddress, tempValid);
  }

  if (VALID(I_agentLogHostTableIpAddressType, data->valid))
  {

    rc = snmpAgentLogHostIpAddressTypeSet(data->agentLogHostTableIndex,
                               data->agentLogHostTableIpAddressType);

    if ((rc != L7_SUCCESS) && (rc != L7_ALREADY_CONFIGURED))
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    
    SET_VALID(I_agentLogHostTableIpAddressType, tempValid);
  }

  if (VALID(I_agentLogHostTablePort, data->valid))
  {
    rc = usmDbLogHostTablePortSet(USMDB_UNIT_CURRENT, 
                                  data->agentLogHostTableIndex, (L7_uint32)data->agentLogHostTablePort);
    if (rc != L7_SUCCESS && rc != L7_ALREADY_CONFIGURED)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    
    SET_VALID(I_agentLogHostTablePort, tempValid);
  }

  if (VALID(I_agentLogHostTableSeverityFilter, data->valid))
  {
    if (snmpAgentLogHostTableSeverityFilterSet(data->agentLogHostTableIndex,
                                               data->agentLogHostTableSeverityFilter) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    
    SET_VALID(I_agentLogHostTableSeverityFilter, tempValid);
  }

  if (VALID(I_agentLogHostTableRowStatus, data->valid) && setRowStatus == L7_TRUE)
  {
    if (snmpAgentLogHostTableRowStatusSet(data->agentLogHostTableIndex,
                                          data->agentLogHostTableRowStatus) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    
    SET_VALID(I_agentLogHostTableRowStatus, tempValid);
  }

  return(NO_ERROR);
}

  #ifdef SR_agentLogSyslogHostEntry_UNDO
/* add #define SR_agentLogSyslogHostEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentLogSyslogHostEntry family.
 */
int
agentLogSyslogHostEntry_undo(doList_t *doHead, doList_t *doCur,
                             ContextInfo *contextInfo)
{
  agentLogSyslogHostEntry_t *data = (agentLogSyslogHostEntry_t *) doCur->data;
  agentLogSyslogHostEntry_t *undodata = (agentLogSyslogHostEntry_t *) doCur->undodata;
  agentLogSyslogHostEntry_t *setdata = undodata;
  L7_int32 function = SR_UNKNOWN;

  if ( data == NULL )
    return(UNDO_FAILED_ERROR);

  /* if there's no previous data, then assume a creation request */
  if ( undodata == NULL )
  {
    /* undoing a create, check that the entry exists */
    if (snmpAgentLogSyslogHostEntryGet(data->agentLogHostTableIndex) != L7_SUCCESS)
    {
      return(NO_ERROR);
    }
    ZERO_VALID(data->valid);
    data->agentLogHostTableRowStatus = D_agentLogHostTableRowStatus_destroy;
    SET_VALID(I_agentLogHostTableIndex, data->valid);
    SET_VALID(I_agentLogHostTableRowStatus, data->valid);
    function = SR_DELETE;
    /* point setdata to new data structure */
    setdata = data;
  }
  else
  {
    /* undoing a modify, replace the original data */
    function = SR_ADD_MODIFY;
  
    /* if the rowstatus was changed during this operation */
    if (VALID(I_agentLogHostTableRowStatus, data->valid) &&
        data->agentLogHostTableRowStatus != undodata->agentLogHostTableRowStatus)
    {
      /* handle unusual state transitions */
  
      /* undo a row deletion */
      if (data->agentLogHostTableRowStatus == D_agentLogHostTableRowStatus_destroy)
      {
        /* depending on the old state, pick the correct create request */
        if (undodata->agentLogHostTableRowStatus == D_agentLogHostTableRowStatus_notInService)
        {
          undodata->agentLogHostTableRowStatus = D_agentLogHostTableRowStatus_createAndWait;
        }
        else
        {
          undodata->agentLogHostTableRowStatus = D_agentLogHostTableRowStatus_createAndGo;
        }
        SET_VALID(I_agentLogHostTableRowStatus, undodata->valid);
      }
      /* other state transitions are reversable */
    }
    else
    {
      /* only set objects which were set during the set operation */
      memcpy(undodata->valid, data->valid, sizeof(data->valid));
    }
  }

  /* use the set method for the undo */
  if (k_agentLogSyslogHostEntry_set(undodata, contextInfo, function) == NO_ERROR)
    return(NO_ERROR);

  return(UNDO_FAILED_ERROR);
}
  #endif /* SR_agentLogSyslogHostEntry_UNDO */

#endif /* SETS */

/* CLI Command Logging - needs to be conditional on CLI present */

agentLogCliCommandsConfigGroup_t *
k_agentLogCliCommandsConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                     int nominator)
{
   static agentLogCliCommandsConfigGroup_t agentLogCliCommandsConfigGroupData;

   ZERO_VALID(agentLogCliCommandsConfigGroupData.valid);

   switch (nominator)
   {
       case -1:

       case I_agentLogCliCommandsAdminStatus:
               if (snmpAgentLogCliCommandsAdminStatusGet(
                 &agentLogCliCommandsConfigGroupData.agentLogCliCommandsAdminStatus) == L7_SUCCESS)
                   SET_VALID(I_agentLogCliCommandsAdminStatus, agentLogCliCommandsConfigGroupData.valid);
       break;

       default:
         /* unknown nominator */
          return(NULL);
          break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentLogCliCommandsConfigGroupData.valid))
          return(NULL);

  return(&agentLogCliCommandsConfigGroupData);
}

#ifdef SETS
int
k_agentLogCliCommandsConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_agentLogCliCommandsConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                       doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentLogCliCommandsConfigGroup_set(agentLogCliCommandsConfigGroup_t *data,
                                     ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if ( VALID(I_agentLogCliCommandsAdminStatus, data->valid))
  {
    if( snmpAgentLogCliCommandsAdminStatusSet(data->agentLogCliCommandsAdminStatus)
        != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentLogCliCommandsAdminStatus, tempValid);
    }
  }

  return(NO_ERROR);
}

#ifdef SR_agentLogCliCommandsConfigGroup_UNDO
/* add #define SR_agentLogCliCommandsConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentLogCliCommandsConfigGroup family.
 */
int
agentLogCliCommandsConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                    ContextInfo *contextInfo)
{
  agentLogCliCommandsConfigGroup_t *data = (agentLogCliCommandsConfigGroup_t *) doCur->data;
  agentLogCliCommandsConfigGroup_t *undodata = (agentLogCliCommandsConfigGroup_t *) doCur->undodata;
  agentLogCliCommandsConfigGroup_t *setdata = NULL;
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
  if (k_agentLogCliCommandsConfigGroup_set(setdata, contextInfo, function) == NO_ERROR)
    return(NO_ERROR);

   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentLogCliCommandsConfigGroup_UNDO */

#endif /* SETS */




agentLogStatisticsGroup_t *
k_agentLogStatisticsGroup_get(int serialNum, ContextInfo *contextInfo,
                              int nominator)
{
  static agentLogStatisticsGroup_t agentLogStatisticsGroupData;
  L7_uint32 temp_uint=0;

  switch (nominator)
  {
  case I_agentLogMessagesReceived:
    if (usmDbLogMessagesReceivedGet(USMDB_UNIT_CURRENT, 
                                    &agentLogStatisticsGroupData.agentLogMessagesReceived) == L7_SUCCESS)
      SET_VALID(nominator, agentLogStatisticsGroupData.valid);
    break;

  case I_agentLogMessagesDropped:
    if (usmDbLogMessagesDroppedGet(USMDB_UNIT_CURRENT, 
                                   &agentLogStatisticsGroupData.agentLogMessagesDropped) == L7_SUCCESS)
      SET_VALID(nominator, agentLogStatisticsGroupData.valid);
    break;

  case I_agentLogSyslogMessagesRelayed:
    if (usmDbLogSyslogMessagesRelayedGet(USMDB_UNIT_CURRENT, 
                                         &agentLogStatisticsGroupData.agentLogSyslogMessagesRelayed) == L7_SUCCESS)
      SET_VALID(nominator, agentLogStatisticsGroupData.valid);
    break;

  case I_agentLogMessageReceivedTime:
    if (usmDbLogMessageReceivedTimeGet(USMDB_UNIT_CURRENT, 
                                       &temp_uint) == L7_SUCCESS &&
        SafeMakeDateAndTime(&agentLogStatisticsGroupData.agentLogMessageReceivedTime, temp_uint) == L7_TRUE)
      SET_VALID(nominator, agentLogStatisticsGroupData.valid);
    break;

  case I_agentLogSyslogMessageDeliveredTime:
    if (usmDbLogSyslogMessageDeliveredTimeGet(USMDB_UNIT_CURRENT, 
                                              &temp_uint) == L7_SUCCESS &&
        SafeMakeDateAndTime(&agentLogStatisticsGroupData.agentLogSyslogMessageDeliveredTime, temp_uint) == L7_TRUE)
      SET_VALID(nominator, agentLogStatisticsGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != -1 && !VALID(nominator, agentLogStatisticsGroupData.valid))
    return(NULL);

  return(&agentLogStatisticsGroupData);
}

agentLogInMemoryGroup_t *
k_agentLogInMemoryGroup_get(int serialNum, ContextInfo *contextInfo,
                            int nominator)
{
  static agentLogInMemoryGroup_t agentLogInMemoryGroupData;

  switch (nominator)
  {
  case I_agentLogInMemoryLogCount:
    if (usmDbLogInMemoryLogCountGet(USMDB_UNIT_CURRENT, 
                                    &agentLogInMemoryGroupData.agentLogInMemoryLogCount) == L7_SUCCESS)
      SET_VALID(nominator, agentLogInMemoryGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != -1 && !VALID(nominator, agentLogInMemoryGroupData.valid))
    return(NULL);

  return(&agentLogInMemoryGroupData);
}

agentLogInMemoryEntry_t *
k_agentLogInMemoryEntry_get(int serialNum, ContextInfo *contextInfo,
                            int nominator,
                            int searchType,
                            SR_UINT32 agentLogInMemoryMsgIndex)
{
  static agentLogInMemoryEntry_t agentLogInMemoryEntryData;
  L7_char8 snmp_buffer[L7_LOG_MESSAGE_LENGTH];
  bzero(snmp_buffer, L7_LOG_MESSAGE_LENGTH);

  ZERO_VALID(agentLogInMemoryEntryData.valid);
  agentLogInMemoryEntryData.agentLogInMemoryMsgIndex = agentLogInMemoryMsgIndex;
  SET_VALID(I_agentLogInMemoryMsgIndex, agentLogInMemoryEntryData.valid);

  if ((searchType == EXACT) ?
      (usmDbLogMsgInMemoryGet(USMDB_UNIT_CURRENT, 
                              agentLogInMemoryEntryData.agentLogInMemoryMsgIndex, 
                              snmp_buffer) != L7_SUCCESS) :
      ((usmDbLogMsgInMemoryGet(USMDB_UNIT_CURRENT, 
                               agentLogInMemoryEntryData.agentLogInMemoryMsgIndex, 
                               snmp_buffer) != L7_SUCCESS) &&
       (usmDbLogMsgInMemoryGetNext(USMDB_UNIT_CURRENT, 
                                   agentLogInMemoryEntryData.agentLogInMemoryMsgIndex, 
                                   snmp_buffer, 
                                   &agentLogInMemoryEntryData.agentLogInMemoryMsgIndex) != L7_SUCCESS)))
    return(NULL);

  switch (nominator)
  {
  case I_agentLogInMemoryMsgIndex:
    break;

  case I_agentLogInMemoryMsgText:
    if (SafeMakeOctetStringFromTextExact(&agentLogInMemoryEntryData.agentLogInMemoryMsgText,
                                         snmp_buffer) == L7_TRUE)
    {
      SET_VALID(nominator, agentLogInMemoryEntryData.valid);
    }
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != -1 && !VALID(nominator, agentLogInMemoryEntryData.valid))
    return(NULL);

  return(&agentLogInMemoryEntryData);
}



agentLogPersistentGroup_t *
k_agentLogPersistentGroup_get(int serialNum, ContextInfo *contextInfo,
                              int nominator)
{
  static agentLogPersistentGroup_t agentLogPersistentGroupData;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_LOG_COMPONENT_ID, L7_LOG_PERSISTENT_FEATURE_ID) != L7_TRUE)
  {
     return NULL;
  }

  switch (nominator)
  {

  case I_agentLogPersistentLogCount:
    if (usmDbLogPersistentLogCountGet(USMDB_UNIT_CURRENT, 
                                      &agentLogPersistentGroupData.agentLogPersistentLogCount) == L7_SUCCESS)
      SET_VALID(nominator, agentLogPersistentGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != -1 && !VALID(nominator, agentLogPersistentGroupData.valid))
    return(NULL);

  return(&agentLogPersistentGroupData);
}

agentLogPersistentEntry_t *
k_agentLogPersistentEntry_get(int serialNum, ContextInfo *contextInfo,
                              int nominator,
                              int searchType,
                              SR_UINT32 agentLogMsgPersistentMsgIndex)
{
  static agentLogPersistentEntry_t agentLogPersistentEntryData;
  L7_char8 snmp_buffer[L7_LOG_MESSAGE_LENGTH];

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_LOG_COMPONENT_ID, L7_LOG_PERSISTENT_FEATURE_ID) != L7_TRUE)
  {
     return NULL;
  }


  bzero(snmp_buffer, L7_LOG_MESSAGE_LENGTH);

  agentLogPersistentEntryData.agentLogMsgPersistentMsgIndex = agentLogMsgPersistentMsgIndex;
  SET_VALID(I_agentLogMsgPersistentMsgIndex, agentLogPersistentEntryData.valid);

  if ((searchType == EXACT) ?
      (usmDbLogMsgPersistentGet(L7_ALL_UNITS, 
                                agentLogPersistentEntryData.agentLogMsgPersistentMsgIndex, 
                                snmp_buffer) != L7_SUCCESS) :
      ((usmDbLogMsgPersistentGet(L7_ALL_UNITS, 
                                 agentLogPersistentEntryData.agentLogMsgPersistentMsgIndex, 
                                 snmp_buffer) != L7_SUCCESS) &&
       (usmDbLogMsgPersistentGetNext(L7_ALL_UNITS, 
                                     agentLogPersistentEntryData.agentLogMsgPersistentMsgIndex, 
                                     snmp_buffer, 
                                     &agentLogPersistentEntryData.agentLogMsgPersistentMsgIndex) != L7_SUCCESS)))
    return(NULL);

  switch (nominator)
  {
  case I_agentLogMsgPersistentMsgIndex:
    break;

  case I_agentLogMsgPersistentMsgText:
    if (SafeMakeOctetStringFromTextExact(&agentLogPersistentEntryData.agentLogMsgPersistentMsgText,
                                         snmp_buffer) == L7_TRUE)
    {
      SET_VALID(nominator, agentLogPersistentEntryData.valid);
    }
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != -1 && !VALID(nominator, agentLogPersistentEntryData.valid))
    return(NULL);

  return(&agentLogPersistentEntryData);
}
