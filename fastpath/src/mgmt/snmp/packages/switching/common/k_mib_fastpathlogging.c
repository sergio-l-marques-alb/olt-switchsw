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
#include "osapi_support.h"
#include <inst_lib.h>

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
      setdata = undodata;
    }
  }

  /* use the set method for the undo */
  if (k_agentLogSyslogHostEntry_set(setdata, contextInfo, function) == NO_ERROR)
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

#if 0
/* WEB Logging - log web actions*/

agentLogWebConfigGroup_t *
k_agentLogWebConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                     int nominator)
{
   static agentLogWebConfigGroup_t agentLogWebConfigGroupData;

   ZERO_VALID(agentLogWebConfigGroupData.valid);

   switch (nominator)
   {
       case -1:

   case I_agentLogWebAdminStatus:
               if (snmpAgentLogWebAdminStatusGet(
                 &agentLogWebConfigGroupData.agentLogWebAdminStatus) == L7_SUCCESS)
                   SET_VALID(I_agentLogWebAdminStatus, agentLogWebConfigGroupData.valid);
       break;

       default:
         /* unknown nominator */
          return(NULL);
          break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentLogWebConfigGroupData.valid))
          return(NULL);

  return(&agentLogWebConfigGroupData);
}

#ifdef SETS
int
k_agentLogWebConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_agentLogWebConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                       doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentLogWebConfigGroup_set(agentLogWebConfigGroup_t *data,
                                     ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if ( VALID(I_agentLogWebAdminStatus, data->valid))
  {
    if( snmpAgentLogWebAdminStatusSet(data->agentLogWebAdminStatus)
        != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentLogWebAdminStatus, tempValid);
    }
  }

  return(NO_ERROR);
}

#ifdef SR_agentLogWebConfigGroup_UNDO
/* add #define SR_agentLogWebConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentLogWebConfigGroup family.
 */
int
agentLogWebConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                    ContextInfo *contextInfo)
{
  agentLogWebConfigGroup_t *data = (agentLogWebConfigGroup_t *) doCur->data;
  agentLogWebConfigGroup_t *undodata = (agentLogWebConfigGroup_t *) doCur->undodata;
  agentLogWebConfigGroup_t *setdata = NULL;
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
  if (k_agentLogWebConfigGroup_set(setdata, contextInfo, function) == NO_ERROR)
    return(NO_ERROR);

   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentLogWebConfigGroup_UNDO */

#endif /* SETS */

/* SNMP Logging - log SNMP requests*/

agentLogSnmpConfigGroup_t *
k_agentLogSnmpConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                     int nominator)
{
   static agentLogSnmpConfigGroup_t agentLogSnmpConfigGroupData;

   ZERO_VALID(agentLogSnmpConfigGroupData.valid);

   switch (nominator)
   {
       case -1:

   case I_agentLogSnmpAdminStatus:
               if (snmpAgentLogSnmpAdminStatusGet(
                 &agentLogSnmpConfigGroupData.agentLogSnmpAdminStatus) == L7_SUCCESS)
                   SET_VALID(I_agentLogSnmpAdminStatus, agentLogSnmpConfigGroupData.valid);
       break;

       default:
         /* unknown nominator */
          return(NULL);
          break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentLogSnmpConfigGroupData.valid))
          return(NULL);

  return(&agentLogSnmpConfigGroupData);
}

#ifdef SETS
int
k_agentLogSnmpConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_agentLogSnmpConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                       doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentLogSnmpConfigGroup_set(agentLogSnmpConfigGroup_t *data,
                                     ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if ( VALID(I_agentLogSnmpAdminStatus, data->valid))
  {
    if( snmpAgentLogSnmpAdminStatusSet(data->agentLogSnmpAdminStatus)
        != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentLogSnmpAdminStatus, tempValid);
    }
  }

  return(NO_ERROR);
}

#ifdef SR_agentLogSnmpConfigGroup_UNDO
/* add #define SR_agentLogSnmpConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentLogSnmpConfigGroup family.
 */
int
agentLogSnmpConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                    ContextInfo *contextInfo)
{
  agentLogSnmpConfigGroup_t *data = (agentLogSnmpConfigGroup_t *) doCur->data;
  agentLogSnmpConfigGroup_t *undodata = (agentLogSnmpConfigGroup_t *) doCur->undodata;
  agentLogSnmpConfigGroup_t *setdata = NULL;
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
  if (k_agentLogSnmpConfigGroup_set(setdata, contextInfo, function) == NO_ERROR)
    return(NO_ERROR);

   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentLogSnmpConfigGroup_UNDO */

#endif /* SETS */


/* Switch Auditing - audit user's actions on each management interface*/

agentLogAuditConfigGroup_t *
k_agentLogAuditConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                     int nominator)
{
   static agentLogAuditConfigGroup_t agentLogAuditConfigGroupData;

   ZERO_VALID(agentLogAuditConfigGroupData.valid);

   switch (nominator)
   {
       case -1:

   case I_agentLogAuditAdminStatus:
               if (snmpAgentLogAuditAdminStatusGet(
                 &agentLogAuditConfigGroupData.agentLogAuditAdminStatus) == L7_SUCCESS)
                   SET_VALID(I_agentLogAuditAdminStatus, agentLogAuditConfigGroupData.valid);
       break;

       default:
         /* unknown nominator */
          return(NULL);
          break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentLogAuditConfigGroupData.valid))
          return(NULL);

  return(&agentLogAuditConfigGroupData);
}

#ifdef SETS
int
k_agentLogAuditConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_agentLogAuditConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                       doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentLogAuditConfigGroup_set(agentLogAuditConfigGroup_t *data,
                                     ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if ( VALID(I_agentLogAuditAdminStatus, data->valid))
  {
    if( snmpAgentLogAuditAdminStatusSet(data->agentLogAuditAdminStatus)
        != L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentLogAuditAdminStatus, tempValid);
    }
  }

  return(NO_ERROR);
}

#ifdef SR_agentLogAuditConfigGroup_UNDO
/* add #define SR_agentLogAuditConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentLogAuditConfigGroup family.
 */
int
agentLogAuditConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                    ContextInfo *contextInfo)
{
  agentLogAuditConfigGroup_t *data = (agentLogAuditConfigGroup_t *) doCur->data;
  agentLogAuditConfigGroup_t *undodata = (agentLogAuditConfigGroup_t *) doCur->undodata;
  agentLogAuditConfigGroup_t *setdata = NULL;
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
  if (k_agentLogAuditConfigGroup_set(setdata, contextInfo, function) == NO_ERROR)
    return(NO_ERROR);

   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentLogAuditConfigGroup_UNDO */

#endif /* SETS */
#endif

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

agentLogEmailAlertConfigGroup_t *
k_agentLogEmailAlertConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                    int nominator)
{
  static agentLogEmailAlertConfigGroup_t agentLogemailAlertConfigGroupData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uint32 featureStatus;	
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentLogemailAlertConfigGroupData.agentLogEmailfromAddr = MakeOctetString(NULL, 0);
  }

  switch (nominator)
  {
    case I_agentLogEmailAdminStatus:
      if (usmDbLogEmailAlertStatusGet(USMDB_UNIT_CURRENT, 
                                      &featureStatus) == L7_SUCCESS)
      {
      	 if(featureStatus == L7_DISABLE)
      	 {
			agentLogemailAlertConfigGroupData.agentLogEmailAdminStatus = D_agentLogEmailAdminStatus_disable; 
      	 }
		 else
		 {
			agentLogemailAlertConfigGroupData.agentLogEmailAdminStatus = D_agentLogEmailAdminStatus_enable; 
		 }
        SET_VALID(nominator, agentLogemailAlertConfigGroupData.valid);
      }
      break;

    case I_agentLogEmailfromAddr:
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
      if ((usmDbLogEmailAlertFromAddrGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS) &&
        (SafeMakeOctetStringFromTextExact(&agentLogemailAlertConfigGroupData.agentLogEmailfromAddr, snmp_buffer) == L7_TRUE))
        SET_VALID(nominator, agentLogemailAlertConfigGroupData.valid);
      break;

    case I_agentLogEmaillogDuration:
      if (usmDbLogEmailAlertLogTimeGet(USMDB_UNIT_CURRENT, 
                                      &agentLogemailAlertConfigGroupData.agentLogEmaillogDuration) == L7_SUCCESS)
        SET_VALID(nominator, agentLogemailAlertConfigGroupData.valid);
      break;
  
    case I_agentLogEmailUrgentSeverity:
      if (usmDbLogEmailAlertUrgentSeverityGet(USMDB_UNIT_CURRENT, 
                                      &agentLogemailAlertConfigGroupData.agentLogEmailUrgentSeverity) == L7_SUCCESS)
        SET_VALID(nominator, agentLogemailAlertConfigGroupData.valid);
      break;

    case I_agentLogEmailNonUrgentSeverity:
      if (usmDbLogEmailAlertNonUrgentSeverityGet(USMDB_UNIT_CURRENT, 
                                      &agentLogemailAlertConfigGroupData.agentLogEmailNonUrgentSeverity) == L7_SUCCESS)
        SET_VALID(nominator, agentLogemailAlertConfigGroupData.valid);
      break;

    case I_agentLogEmailTrapsSeverity:
      if (usmDbLogEmailAlertTrapSeverityGet(USMDB_UNIT_CURRENT, 
                                      &agentLogemailAlertConfigGroupData.agentLogEmailTrapsSeverity) == L7_SUCCESS)
        SET_VALID(nominator, agentLogemailAlertConfigGroupData.valid);
      break;

    default:
    /* unknown nominator */
      return(NULL);
    break;

  }

    if (nominator != -1 && !VALID(nominator, agentLogemailAlertConfigGroupData.valid))
      return(NULL);

   return(&agentLogemailAlertConfigGroupData);
}
#ifdef SETS
int
k_agentLogEmailAlertConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *dp, ContextInfo *contextInfo)
{
    return NO_ERROR;
}

int
k_agentLogEmailAlertConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *doHead, doList_t *dp)
{
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}


int
k_agentLogEmailAlertConfigGroup_set(agentLogEmailAlertConfigGroup_t *data,
                                    ContextInfo *contextInfo, int function)
{
  L7_char8 tempValid[sizeof(data->valid)];
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  bzero(tempValid, sizeof(tempValid));

  memset(snmp_buffer, L7_NULL, sizeof(snmp_buffer));

  if (VALID(I_agentLogEmailAdminStatus, data->valid))
  {
    if (usmDbLogEmailAlertStatusSet(USMDB_UNIT_CURRENT,(data->agentLogEmailAdminStatus == D_agentLogEmailAdminStatus_enable ? L7_ENABLE : L7_DISABLE)) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }

    SET_VALID(I_agentLogEmailAdminStatus, tempValid);
  }

  if (data->agentLogEmailfromAddr != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentLogEmailfromAddr->octet_ptr, data->agentLogEmailfromAddr->length);
    if ( VALID(I_agentLogEmailfromAddr, data->valid))
    {
      if(usmDbLogEmailAlertFromAddrSet(USMDB_UNIT_CURRENT, snmp_buffer, L7_NULL) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentLogEmailfromAddr, tempValid);
      }   
    }
  }

  if (VALID(I_agentLogEmaillogDuration, data->valid))
  {
    if (usmDbLogEmailAlerLogTimeSet(USMDB_UNIT_CURRENT,data->agentLogEmaillogDuration) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    
    SET_VALID(I_agentLogEmaillogDuration, tempValid);
  }
  
  if (VALID(I_agentLogEmailUrgentSeverity, data->valid))
  {
    if (usmDbLogEmailAlertUrgentSeveritySet(USMDB_UNIT_CURRENT,data->agentLogEmailUrgentSeverity) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    
    SET_VALID(I_agentLogEmailUrgentSeverity, tempValid);
  }

  if (VALID(I_agentLogEmailNonUrgentSeverity, data->valid))
  {
    if (usmDbLogEmailAlertNonUrgentSeveritySet(USMDB_UNIT_CURRENT,data->agentLogEmailNonUrgentSeverity) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    
    SET_VALID(I_agentLogEmailNonUrgentSeverity, tempValid);
  }

  if (VALID(I_agentLogEmailTrapsSeverity, data->valid))
  {
    if (usmDbLogEmailAlertTrapSeveritySet(USMDB_UNIT_CURRENT,data->agentLogEmailTrapsSeverity) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    
    SET_VALID(I_agentLogEmailTrapsSeverity, tempValid);
  }
	

  return(NO_ERROR);
}

#ifdef SR_agentLogemailAlertConfigGroup_UNDO
/* add #define SR_agentLogemailAlertConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentLogemailAlertConfigGroup family.
 */
int
agentLogEmailAlertConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                   ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentLogemailAlertConfigGroup_UNDO */

#endif /* SETS */

agentLogEmailAlertStatsGroup_t *
k_agentLogEmailAlertStatsGroup_get (int serialNum, ContextInfo *contextInfo,
                                    int nominator)
{
  static agentLogEmailAlertStatsGroup_t agentLogemailAlertStatsGroupData;
 
  emailAlertingStatsData_t emailAlertStatistics; 
  L7_RC_t  rc = L7_FAILURE;

  rc=usmDbLogEmailAlertStatsGet(USMDB_UNIT_CURRENT,&emailAlertStatistics);
  switch (nominator)
  {
    case I_agentLogEmailStatsemailsSentCount:
      if (rc == L7_SUCCESS)
      {
        agentLogemailAlertStatsGroupData.agentLogEmailStatsemailsSentCount= emailAlertStatistics.noEmailsSent;                                         
        SET_VALID(nominator, agentLogemailAlertStatsGroupData.valid);
      }
      break;
    case I_agentLogEmailStatsemailsFailureCount:
      if (rc == L7_SUCCESS)
      {
        agentLogemailAlertStatsGroupData.agentLogEmailStatsemailsFailureCount= emailAlertStatistics.noEmailFailures;                                         
        SET_VALID(nominator, agentLogemailAlertStatsGroupData.valid);
      }
      break;
    case I_agentLogEmailStatsTimeSinceLastEmailSent:
      if (rc == L7_SUCCESS)
      {
        agentLogemailAlertStatsGroupData.agentLogEmailStatsTimeSinceLastEmailSent = emailAlertStatistics.timeSinceLastMail;                                         
        SET_VALID(nominator, agentLogemailAlertStatsGroupData.valid);
      }
      break;
    case I_agentLogEmailStatsClear:
      if (rc == L7_SUCCESS)
      {
        agentLogemailAlertStatsGroupData.agentLogEmailStatsClear = D_agentLogEmailStatsClear_disable;                                         
        SET_VALID(nominator, agentLogemailAlertStatsGroupData.valid);
      }
      break;     
    default:
    /* unknown nominator */
    return(NULL);
    break;
  }
  
  if (nominator != -1 && !VALID(nominator, agentLogemailAlertStatsGroupData.valid))
    return(NULL);

  return (&agentLogemailAlertStatsGroupData);
}

int k_agentLogEmailAlertStatsGroup_test
      (ObjectInfo *object,
                   ObjectSyntax *value,
                   doList_t *dp,
                   ContextInfo *contextInfo)
{


    return NO_ERROR;



}
int k_agentLogEmailAlertStatsGroup_set (agentLogEmailAlertStatsGroup_t  *data,
                   ContextInfo *contextInfo,
                   int function)
{
 L7_char8 tempValid[sizeof(data->valid)];

  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_agentLogEmailStatsClear, data->valid))
  {
    if (data->agentLogEmailStatsClear == D_agentLogEmailStatsClear_enable)
    {
      if (usmDbLogEmailAlertStatsClear(USMDB_UNIT_CURRENT) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(tempValid));
        return(COMMIT_FAILED_ERROR);
      }
      
      SET_VALID(I_agentLogEmailStatsClear, tempValid);
    }
  }
 

 return(NO_ERROR);

}
int k_agentLogEmailAlertStatsGroup_ready
      (ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;

}
int agentLogEmailAlertStatsGroup_undo
      (doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo)
{


   return UNDO_FAILED_ERROR;



}

agentLogEmailToAddrEntry_t *k_agentLogEmailToAddrEntry_get
    (int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_INT32 agentLogEmailToAddrMessageType,
                  OctetString * agentLogEmailToAddr)
{
  static agentLogEmailToAddrEntry_t agentLogEmailToAddrEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentLogEmailToAddrEntryData.agentLogEmailToAddr = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentLogEmailToAddrEntryData.valid);
  bzero(snmp_buffer, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer, agentLogEmailToAddr->octet_ptr, agentLogEmailToAddr->length);
	
  agentLogEmailToAddrEntryData.agentLogEmailToAddrMessageType = agentLogEmailToAddrMessageType;
  SET_VALID(I_agentLogEmailToAddrMessageType, agentLogEmailToAddrEntryData.valid);


  if ((searchType == EXACT) ?
      (snmpLogEmailAlertToAddrGet(USMDB_UNIT_CURRENT,
                              agentLogEmailToAddrEntryData.agentLogEmailToAddrMessageType,
                              snmp_buffer) != L7_SUCCESS) :
      (snmpLogEmailAlertToAddrGet(USMDB_UNIT_CURRENT,
                               agentLogEmailToAddrEntryData.agentLogEmailToAddrMessageType,
                               snmp_buffer) != L7_SUCCESS) &&
      (snmpLogEmailAlertNextToAddrGet(USMDB_UNIT_CURRENT,
                                   &agentLogEmailToAddrEntryData.agentLogEmailToAddrMessageType,
                                   snmp_buffer) != L7_SUCCESS))
 {
  return(NULL);
 }

  if (SafeMakeOctetStringFromTextExact(&agentLogEmailToAddrEntryData.agentLogEmailToAddr, snmp_buffer) != L7_TRUE)
  {
    ZERO_VALID(agentLogEmailToAddrEntryData.valid);
    return(NULL);
  }
 SET_VALID(I_agentLogEmailToAddr, agentLogEmailToAddrEntryData.valid);

  switch (nominator)
  {
  case I_agentLogEmailToAddrMessageType:
    break;

  case I_agentLogEmailToAddr:
    break;

  case I_agentLogEmailToAddrEntryStatus:
  agentLogEmailToAddrEntryData.agentLogEmailToAddrEntryStatus = D_agentLogEmailToAddrEntryStatus_active;
  SET_VALID(nominator, agentLogEmailToAddrEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != -1 && !VALID(nominator, agentLogEmailToAddrEntryData.valid))
    return(NULL);

  return(&agentLogEmailToAddrEntryData);

}



int k_agentLogEmailToAddrEntry_test
    (ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo)
{
    return NO_ERROR;
}


int k_agentLogEmailToAddrEntry_ready
    (ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp)
{
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}


int agentLogEmailToAddrEntry_undo
    (doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}



int k_agentLogEmailToAddrEntry_set
    (agentLogEmailToAddrEntry_t *data,
                  ContextInfo *contextInfo,
                  int function)
{
  
  L7_BOOL isSetSuccess = L7_FALSE;
  L7_char8 tempValid[sizeof(data->valid)];
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  bzero(tempValid, sizeof(tempValid));

  memset(snmp_buffer, L7_NULL, sizeof(snmp_buffer));

  if (data->agentLogEmailToAddr != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentLogEmailToAddr->octet_ptr, data->agentLogEmailToAddr->length);
  }

  if ((VALID (I_agentLogEmailToAddrEntryStatus, data->valid)) && (data->agentLogEmailToAddrEntryStatus == D_agentLogEmailToAddrEntryStatus_destroy))
  {
    if (usmDbLogEmailAlertToAddrRemove (USMDB_UNIT_CURRENT,
                                        data->agentLogEmailToAddrMessageType,
                                        snmp_buffer) != L7_SUCCESS)
    {
      CLR_VALID (I_agentLogEmailToAddrEntryStatus, data->valid);
      return COMMIT_FAILED_ERROR;
    }
 
    return NO_ERROR;
  }

  if((VALID (I_agentLogEmailToAddrEntryStatus, data->valid)) &&
     ((data->agentLogEmailToAddrEntryStatus == D_agentLogEmailToAddrEntryStatus_createAndGo) || 
      (data->agentLogEmailToAddrEntryStatus == D_agentLogEmailToAddrEntryStatus_createAndWait)))
  {
    if (usmDbLogEmailAlertToAddrSet (USMDB_UNIT_CURRENT,
                                     data->agentLogEmailToAddrMessageType,
                                     snmp_buffer, L7_NULL)
                                   == L7_SUCCESS)
    {
      isSetSuccess = L7_TRUE;
      SET_VALID(I_agentLogEmailToAddrEntryStatus, tempValid);
    }

    if(isSetSuccess != L7_TRUE)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
  }
  return(NO_ERROR);
}

agentLogEmailSubjectEntry_t *k_agentLogEmailSubjectEntry_get
    (int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_INT32 agentLogEmailSubjectMessageType)
{
  static agentLogEmailSubjectEntry_t agentLogEmailSubjectEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentLogEmailSubjectEntryData.agentLogEmailSubject = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentLogEmailSubjectEntryData.valid);
  agentLogEmailSubjectEntryData.agentLogEmailSubjectMessageType= agentLogEmailSubjectMessageType;
  SET_VALID(I_agentLogEmailSubjectMessageType, agentLogEmailSubjectEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpAgentLogEmailSubjectMessageTypeGet(USMDB_UNIT_CURRENT, 
                                             agentLogEmailSubjectEntryData.agentLogEmailSubjectMessageType)
                                             != L7_SUCCESS) :
      (snmpAgentLogEmailSubjectMessageTypeGet(USMDB_UNIT_CURRENT, 
                                             agentLogEmailSubjectEntryData.agentLogEmailSubjectMessageType)
                                             != L7_SUCCESS) &&
      (snmpAgentLogEmailSubjectMessageTypeNextGet(USMDB_UNIT_CURRENT, 
                                             &agentLogEmailSubjectEntryData.agentLogEmailSubjectMessageType)
                                             != L7_SUCCESS))
  return(NULL);

  switch (nominator)
  {
  case I_agentLogEmailSubjectMessageType:
    break;

  case I_agentLogEmailSubject:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((usmDbLogEmailAlertSubjectGet(USMDB_UNIT_CURRENT, 
                                     agentLogEmailSubjectEntryData.agentLogEmailSubjectMessageType,
                                     snmp_buffer) == L7_SUCCESS) &&
        (SafeMakeOctetStringFromTextExact(&agentLogEmailSubjectEntryData.agentLogEmailSubject, snmp_buffer) == L7_TRUE))
        SET_VALID(nominator, agentLogEmailSubjectEntryData.valid);
      break;

  case I_agentLogEmailSubjectEntryStatus:
    agentLogEmailSubjectEntryData.agentLogEmailSubjectEntryStatus = D_agentLogEmailSubjectEntryStatus_active;
    SET_VALID(nominator, agentLogEmailSubjectEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != -1 && !VALID(nominator, agentLogEmailSubjectEntryData.valid))
  {
    return(NULL);
  }

  return(&agentLogEmailSubjectEntryData);

}

int k_agentLogEmailSubjectEntry_test
    (ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo)
{

    return NO_ERROR;


}
int k_agentLogEmailSubjectEntry_set
    (agentLogEmailSubjectEntry_t *data,
                  ContextInfo *contextInfo,
                  int function)
{
  L7_BOOL isSetSuccess = L7_FALSE;
  L7_char8 tempValid[sizeof(data->valid)];
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  bzero(tempValid, sizeof(tempValid));

  memset(snmp_buffer, L7_NULL, sizeof(snmp_buffer));
    
  if ((VALID (I_agentLogEmailSubjectEntryStatus, data->valid)) && (data->agentLogEmailSubjectEntryStatus == D_agentLogEmailSubjectEntryStatus_destroy))
  {
    if (data->agentLogEmailSubjectMessageType == D_agentLogEmailSubjectMessageType_critical)
    {
      if (usmDbLogEmailAlertSubjectSet (USMDB_UNIT_CURRENT,
                                        data->agentLogEmailSubjectMessageType,
                                        "Urgent Log Messages") != L7_SUCCESS)
      {
        CLR_VALID (I_agentLogEmailSubjectEntryStatus, data->valid);
        return COMMIT_FAILED_ERROR;
      }
    }
    else
    {
      if (usmDbLogEmailAlertSubjectSet (USMDB_UNIT_CURRENT,
                                        data->agentLogEmailSubjectMessageType,
                                        "Non Urgent Log Messages") != L7_SUCCESS)
      {
        CLR_VALID (I_agentLogEmailSubjectEntryStatus, data->valid);
        return COMMIT_FAILED_ERROR;
      }
    }
    return NO_ERROR;
  }

  if (data->agentLogEmailSubject != NULL)
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentLogEmailSubject->octet_ptr, data->agentLogEmailSubject->length);
  }

  if(VALID (I_agentLogEmailSubject, data->valid))  
      
  {
    if (usmDbLogEmailAlertSubjectSet (USMDB_UNIT_CURRENT,
                                      data->agentLogEmailSubjectMessageType,
                                      snmp_buffer)
                                    == L7_SUCCESS)
    {
      isSetSuccess = L7_TRUE;
      SET_VALID(I_agentLogEmailSubject, tempValid);
    }

    if(isSetSuccess != L7_TRUE)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
  }
  return(NO_ERROR);

}
int k_agentLogEmailSubjectEntry_ready
    (ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp)
{
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;



}
int agentLogEmailSubjectEntry_undo
    (doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;

}


int k_agentLogEmailToAddrEntry_set_defaults(doList_t *dp)
{
  agentLogEmailToAddrEntry_t *data = (agentLogEmailToAddrEntry_t *) (dp->data);
  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int k_agentLogEmailSubjectEntry_set_defaults(doList_t *dp)
{
  agentLogEmailSubjectEntry_t *data = (agentLogEmailSubjectEntry_t *) (dp->data);

  if ((data->agentLogEmailSubject = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  ZERO_VALID(data->valid);
  return NO_ERROR;
}

agentLogEmailMailServerEntry_t *k_agentLogEmailMailServerEntry_get(int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_INT32 agentLogEmailSmtpAddrType,
                  OctetString * agentLogEmailSmtpAddr)
{
  
  static agentLogEmailMailServerEntry_t agentLogEmailMailServerEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_mailaddress_buffer[SNMP_BUFFER_LEN];
  L7_char8 snmp_username_buffer[SNMP_BUFFER_LEN];		
  L7_char8 snmp_passwd_buffer[SNMP_BUFFER_LEN];			
  L7_uint32 ipAddress = 0;	
  L7_in6_addr_t serverV6Addr;
	
  L7_uint32 securityProtocol;	

  if((agentLogEmailSmtpAddrType!= L7_NULL) &&
      (agentLogEmailSmtpAddrType != L7_IP_ADDRESS_TYPE_IPV4) &&
       (agentLogEmailSmtpAddrType != L7_IP_ADDRESS_TYPE_IPV6) &&
       (agentLogEmailSmtpAddrType != L7_IP_ADDRESS_TYPE_DNS))
  {
    return(NULL);
  }


  if (firstTime== L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentLogEmailMailServerEntryData.agentLogEmailSmtpAddr = MakeOctetString (NULL, 0);
    agentLogEmailMailServerEntryData.agentLogEmailloginID = MakeOctetString (NULL, 0);
    agentLogEmailMailServerEntryData.agentLogEmailPassword = MakeOctetString (NULL, 0); 
  }
 
  /* Clear all the bits*/
  ZERO_VALID (agentLogEmailMailServerEntryData.valid);
  bzero(snmp_mailaddress_buffer, L7_EMAIL_ALERT_SERVER_ADDRESS_SIZE);
  memcpy(snmp_mailaddress_buffer, agentLogEmailSmtpAddr->octet_ptr, agentLogEmailSmtpAddr->length);

  if (agentLogEmailSmtpAddrType == D_agentLogEmailSmtpAddrType_ipv4 &&
      agentLogEmailSmtpAddr->length == sizeof(ipAddress))
  {
    /* Convert octet string to uint32 ip address, then to dotted decimal */
    memcpy(&ipAddress, agentLogEmailSmtpAddr->octet_ptr, sizeof(ipAddress));
    osapiStrncpy(snmp_mailaddress_buffer, osapiInet_ntoa(ipAddress), sizeof(snmp_mailaddress_buffer));
  }
  else if (agentLogEmailSmtpAddrType == D_agentLogEmailSmtpAddrType_dns &&
           agentLogEmailSmtpAddr->length)
  {
    osapiStrncpy(snmp_mailaddress_buffer, agentLogEmailSmtpAddr->octet_ptr, agentLogEmailSmtpAddr->length);		
  }
  else if (agentLogEmailSmtpAddrType == D_agentLogEmailSmtpAddrType_ipv6 )
  {
    L7_uchar8 str_addr[64];
	 L7_in6_addr_t serverV6Addr1;		
	 L7_int32 i;
	 L7_uchar8     temp_addr8[2] ={0};	

	 bzero(str_addr,64);
    OctetStringToIP6 (agentLogEmailSmtpAddr, (L7_char8*)&serverV6Addr1);

    for(i=0;i<16;i++)
	{
		sprintf( temp_addr8, "%d",(serverV6Addr1.in6.addr8[i] ));

		if(temp_addr8[1] ==0)
		{
			serverV6Addr.in6.addr8[i] = (temp_addr8[0]&0x0f);
		}
		else
		{
			serverV6Addr.in6.addr8[i] = (unsigned char)(((temp_addr8[0]&0x0f)<<4) + (temp_addr8[1]&0x0f));
		}
		
	}		
	
    osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&serverV6Addr,str_addr,IPV6_DISP_ADDR_LEN);		
    osapiStrncpy(snmp_mailaddress_buffer, str_addr, sizeof(snmp_mailaddress_buffer));		
		
  }
	
  agentLogEmailMailServerEntryData.agentLogEmailSmtpAddrType = agentLogEmailSmtpAddrType;

  if(agentLogEmailSmtpAddrType == L7_NULL)
  {
    agentLogEmailMailServerEntryData.agentLogEmailSmtpAddrType = L7_IP_ADDRESS_TYPE_IPV4;  
  }

    
  if ((searchType == EXACT) ?
      (snmpLogEmailAlertMailServerAddressGet(USMDB_UNIT_CURRENT,
                                             agentLogEmailMailServerEntryData.agentLogEmailSmtpAddrType,
                                             snmp_mailaddress_buffer) != L7_SUCCESS) :
      ((snmpLogEmailAlertMailServerAddressGet(USMDB_UNIT_CURRENT,
                                             agentLogEmailMailServerEntryData.agentLogEmailSmtpAddrType,
                                             snmp_mailaddress_buffer) != L7_SUCCESS) &&
       (snmpLogEmailAlertNextMailServerAddressGet(USMDB_UNIT_CURRENT,
                                                  &agentLogEmailMailServerEntryData.agentLogEmailSmtpAddrType,
                                                  snmp_mailaddress_buffer) != L7_SUCCESS)))
  {
      firstTime = L7_TRUE;
      return L7_NULLPTR;
  }
  
  /*  converting the IP Address to Octet String */
  if(agentLogEmailMailServerEntryData.agentLogEmailSmtpAddrType != D_agentTacacsServerIpAddrType_dns )
  {
		if(agentLogEmailMailServerEntryData.agentLogEmailSmtpAddrType == D_agentTacacsServerIpAddrType_ipv4)
		{
		    L7_uint32 serverV4Addr;
			serverV4Addr = osapiInet_addr(snmp_mailaddress_buffer);
		    if (SafeMakeOctetString (&agentLogEmailMailServerEntryData.agentLogEmailSmtpAddr,
		                           (L7_uchar8 *)&serverV4Addr, sizeof(L7_uint32)) == L7_TRUE )
		    {
		      SET_VALID(I_agentLogEmailSmtpAddr, agentLogEmailMailServerEntryData.valid);
		    }
		}
	   else
	   {
	      L7_in6_addr_t serverV6Addr;
		  L7_int32 i;
		  L7_in6_addr_t serverV6Addr1;					
				
		  osapiInetPton(L7_AF_INET6, (L7_uchar8 *)snmp_mailaddress_buffer, (L7_uchar8 *)&serverV6Addr1);

		  for(i=0;i<16;i++)
		  {
			serverV6Addr.in6.addr8[i] = ((serverV6Addr1.in6.addr8[i]/16)*10) + serverV6Addr1.in6.addr8[i]%16;
		  }		

		  if (SafeMakeOctetString (&agentLogEmailMailServerEntryData.agentLogEmailSmtpAddr,
	                           (L7_uchar8 *)&serverV6Addr, sizeof(L7_in6_addr_t)) == L7_TRUE )
	     {
		      SET_VALID(I_agentLogEmailSmtpAddr, agentLogEmailMailServerEntryData.valid);
	     }
	   }
  }
  else
  {
      if(SafeMakeOctetStringFromTextExact(&agentLogEmailMailServerEntryData.agentLogEmailSmtpAddr, snmp_mailaddress_buffer) == L7_TRUE)
      {
	      SET_VALID(I_agentLogEmailSmtpAddr, agentLogEmailMailServerEntryData.valid);
      }
  }

  SET_VALID (I_agentLogEmailSmtpAddrType, agentLogEmailMailServerEntryData.valid);    
	

  switch (nominator)
  {
  case I_agentLogEmailSmtpAddrType:
    break;

  case I_agentLogEmailSmtpAddr:
    break;

  case I_agentLogEmailSmtpPort:
    if (usmDbLogEmailAlertMailServerPortGet(USMDB_UNIT_CURRENT, snmp_mailaddress_buffer,
                                    &agentLogEmailMailServerEntryData.agentLogEmailSmtpPort) == L7_SUCCESS)
      SET_VALID(nominator, agentLogEmailMailServerEntryData.valid);
    break;

  case I_agentLogEmailSecurity:
    if (usmDbLogEmailAlertMailServerSecurityGet(USMDB_UNIT_CURRENT, snmp_mailaddress_buffer,
                                  &securityProtocol) == L7_SUCCESS)
    {
		if(securityProtocol == L7_LOG_EMAIL_ALERT_TLSV1)
		{
		  agentLogEmailMailServerEntryData.agentLogEmailSecurity = D_agentLogEmailSecurity_tlsv1;
		}
		else
		{
		  agentLogEmailMailServerEntryData.agentLogEmailSecurity = D_agentLogEmailSecurity_none ;
		}
    	SET_VALID(nominator, agentLogEmailMailServerEntryData.valid);
    }
    break;

  case I_agentLogEmailloginID:
    bzero(snmp_username_buffer, L7_EMAIL_ALERT_USERNAME_STRING_SIZE);
    if ((usmDbLogEmailAlertMailServerUsernameGet(USMDB_UNIT_CURRENT, snmp_mailaddress_buffer, snmp_username_buffer) == L7_SUCCESS) &&
       (SafeMakeOctetStringFromTextExact(&agentLogEmailMailServerEntryData.agentLogEmailloginID, snmp_username_buffer) == L7_TRUE))
      SET_VALID(nominator, agentLogEmailMailServerEntryData.valid);
    break;
  
  case I_agentLogEmailPassword:
    bzero(snmp_passwd_buffer, L7_EMAIL_ALERT_PASSWD_STRING_SIZE);
    if ((usmDbLogEmailAlertMailServerPasswdGet(USMDB_UNIT_CURRENT, snmp_mailaddress_buffer, snmp_passwd_buffer) == L7_SUCCESS) &&
       (SafeMakeOctetStringFromTextExact(&agentLogEmailMailServerEntryData.agentLogEmailPassword, snmp_passwd_buffer) == L7_TRUE))
      SET_VALID(nominator, agentLogEmailMailServerEntryData.valid);
    break;

  case I_agentLogEmailEntryStatus:
    agentLogEmailMailServerEntryData.agentLogEmailEntryStatus = D_agentLogEmailEntryStatus_active;  
      SET_VALID(nominator, agentLogEmailMailServerEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }
  if (nominator != -1 && !VALID(nominator, agentLogEmailMailServerEntryData.valid))
    return(NULL);

  return(&agentLogEmailMailServerEntryData);
}

int k_agentLogEmailMailServerEntry_set(agentLogEmailMailServerEntry_t *data,
                  ContextInfo *contextInfo,
                  int function)
{
  L7_BOOL isSetSuccess = L7_FALSE;
  L7_char8 tempValid[sizeof(data->valid)];
  L7_uchar8 snmp_mailaddress_buffer[SNMP_BUFFER_LEN];
  L7_uchar8 snmp_username_buffer[SNMP_BUFFER_LEN];	
  L7_uchar8 snmp_password_buffer[SNMP_BUFFER_LEN];		
  L7_uint32 securityProtocol;		
  L7_uint32 ipAddress = 0;	
  L7_in6_addr_t serverV6Addr;
	
  bzero(tempValid, sizeof(tempValid));


  memset(snmp_mailaddress_buffer, L7_NULL, sizeof(snmp_mailaddress_buffer));

  if (data->agentLogEmailSmtpAddrType == D_agentLogEmailSmtpAddrType_ipv4 &&
      data->agentLogEmailSmtpAddr->length == sizeof(ipAddress))
  {
    /* Convert octet string to uint32 ip address, then to dotted decimal */
    memcpy(&ipAddress, data->agentLogEmailSmtpAddr->octet_ptr, sizeof(ipAddress));
    osapiStrncpy(snmp_mailaddress_buffer, osapiInet_ntoa(ipAddress), sizeof(snmp_mailaddress_buffer));
  }
  else if (data->agentLogEmailSmtpAddrType == D_agentLogEmailSmtpAddrType_dns &&
           data->agentLogEmailSmtpAddr->length)
  {
    osapiStrncpy(snmp_mailaddress_buffer, data->agentLogEmailSmtpAddr->octet_ptr, data->agentLogEmailSmtpAddr->length);		
  }
  else if (data->agentLogEmailSmtpAddrType == D_agentLogEmailSmtpAddrType_ipv6)
  {
    L7_uchar8 str_addr[64];
	L7_in6_addr_t serverV6Addr1;		
	L7_int32 i;
   L7_uchar8     temp_addr8[2] ={0};	
    bzero(str_addr,64);
    OctetStringToIP6 (data->agentLogEmailSmtpAddr, (L7_char8*)&serverV6Addr1);
	for(i=0;i<16;i++)
	{
		sprintf( temp_addr8, "%d",(serverV6Addr1.in6.addr8[i] ));

		if(temp_addr8[1] ==0)
		{
			serverV6Addr.in6.addr8[i] = (temp_addr8[0]&0x0f);
		}
		else
		{
			serverV6Addr.in6.addr8[i] = (unsigned char)(((temp_addr8[0]&0x0f)<<4) + (temp_addr8[1]&0x0f));


		}
		
	}		
	
    osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&serverV6Addr,str_addr,IPV6_DISP_ADDR_LEN);		
    osapiStrncpy(snmp_mailaddress_buffer, str_addr, sizeof(snmp_mailaddress_buffer));		
  }
	
  if ((VALID (I_agentLogEmailEntryStatus, data->valid)) && (data->agentLogEmailEntryStatus == D_agentLogEmailEntryStatus_destroy))
  {
    if (usmDbLogEmailAlertMailServerAddressRemove(USMDB_UNIT_CURRENT, data->agentLogEmailSmtpAddrType,
                                        snmp_mailaddress_buffer) != L7_SUCCESS)
    {
      CLR_VALID (I_agentLogEmailEntryStatus, data->valid);
      return COMMIT_FAILED_ERROR;
    }
    return NO_ERROR;
  }

  if((VALID (I_agentLogEmailEntryStatus, data->valid)) &&
     ((data->agentLogEmailEntryStatus == D_agentLogEmailEntryStatus_createAndGo) || 
      (data->agentLogEmailEntryStatus == D_agentLogEmailEntryStatus_createAndWait)))
  {
    if (usmDbLogEmailAlertMailServerAddressSet(USMDB_UNIT_CURRENT,data->agentLogEmailSmtpAddrType,
                                              snmp_mailaddress_buffer) == L7_SUCCESS)
    {
      isSetSuccess = L7_TRUE;
      SET_VALID(I_agentLogEmailEntryStatus, tempValid);
    }

    if(isSetSuccess != L7_TRUE)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }

  }

  if (VALID(I_agentLogEmailSmtpPort, data->valid))
  {
    if (usmDbLogEmailAlertMailServerPortSet(USMDB_UNIT_CURRENT,
                                            snmp_mailaddress_buffer,
                                            data->agentLogEmailSmtpPort) == L7_SUCCESS)
    {
      isSetSuccess = L7_TRUE;
      SET_VALID(I_agentLogEmailSmtpPort, tempValid);
    }

    if(isSetSuccess != L7_TRUE)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
  }

  if (VALID(I_agentLogEmailSecurity, data->valid))
  {
	if(data->agentLogEmailSecurity == D_agentLogEmailSecurity_tlsv1)
	{
		securityProtocol = L7_LOG_EMAIL_ALERT_TLSV1;
	}
	else
	{
		securityProtocol = L7_LOG_EMAIL_ALERT_NONE;
	}
    if (usmDbLogEmailAlertMailServerSecuritySet(USMDB_UNIT_CURRENT,
                                                snmp_mailaddress_buffer,
                                                securityProtocol)
                                              == L7_SUCCESS)
    {
      isSetSuccess = L7_TRUE;
      SET_VALID(I_agentLogEmailSecurity, tempValid);
    }

    if(isSetSuccess != L7_TRUE)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
  }

  if (VALID(I_agentLogEmailloginID, data->valid))
  {
	  if (data->agentLogEmailloginID != NULL)
	  {
	    bzero(snmp_username_buffer, L7_EMAIL_ALERT_USERNAME_STRING_SIZE);
	    memcpy(snmp_username_buffer, data->agentLogEmailloginID->octet_ptr, data->agentLogEmailloginID->length);
		snmp_username_buffer[data->agentLogEmailloginID->length]  = '\0';	
	    if (usmDbLogEmailAlertMailServerUsernameSet(USMDB_UNIT_CURRENT,
	                                                  snmp_mailaddress_buffer,
	                                                  snmp_username_buffer) == L7_SUCCESS)
	    {
	      isSetSuccess = L7_TRUE;
	      SET_VALID(I_agentLogEmailloginID, tempValid);
	    }

	    if(isSetSuccess != L7_TRUE)
	    {
	      memcpy(data->valid, tempValid, sizeof(data->valid));
	      return(COMMIT_FAILED_ERROR);
	    }
	  }
  }

  if (VALID(I_agentLogEmailPassword, data->valid))
  {
	  if (data->agentLogEmailPassword != NULL)
	  {
	    bzero(snmp_password_buffer, L7_EMAIL_ALERT_PASSWD_STRING_SIZE);
	    memcpy(snmp_password_buffer, data->agentLogEmailPassword->octet_ptr, data->agentLogEmailPassword->length);
		 snmp_password_buffer[data->agentLogEmailPassword->length] ='\0';	
	      if (usmDbLogEmailAlertMailServerPasswdSet(USMDB_UNIT_CURRENT,
	                                                snmp_mailaddress_buffer,
	                                                snmp_password_buffer) == L7_SUCCESS)
	    {
	      isSetSuccess = L7_TRUE;
	      SET_VALID(I_agentLogEmailPassword, tempValid);
	    }

	    if(isSetSuccess != L7_TRUE)
	    {
	      memcpy(data->valid, tempValid, sizeof(data->valid));
	      return(COMMIT_FAILED_ERROR);
	    }
	  }
  }
  
 return NO_ERROR;
}

int k_agentLogEmailMailServerEntry_ready(ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;

}

int k_agentLogEmailMailServerEntry_set_defaults(doList_t *dp)
{  
  agentLogEmailMailServerEntry_t *data = (agentLogEmailMailServerEntry_t *) (dp->data);

  if ((data->agentLogEmailloginID = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->agentLogEmailPassword = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  ZERO_VALID(data->valid);

  return NO_ERROR;
}


int agentLogEmailMailServerEntry_undo(doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}


int k_agentLogEmailMailServerEntry_test    (ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo)
{

    return NO_ERROR;

}

