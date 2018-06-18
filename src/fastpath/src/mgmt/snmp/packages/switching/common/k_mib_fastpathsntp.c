/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_fastpathsntp.c
*
* @purpose    System-Specific code to support the SNTP protocol
*
* @component  SNMP
*
* @comments
*
* @create     2/23/2004
*
* @author     cpverne
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"
#include "k_private_base.h"
#include "k_mib_fastpathsntp_api.h"
#include "usmdb_sntp_api.h"

agentSntpClient_t *
k_agentSntpClient_get(int serialNum, ContextInfo *contextInfo,
                      int nominator)
{
  static agentSntpClient_t agentSntpClientData;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 temp_unsigned;
  L7_ushort16 temp_short;
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    agentSntpClientData.agentSntpClientSupportedMode = MakeOctetString(NULL, 0);
    agentSntpClientData.agentSntpClientLastUpdateTime = MakeOctetString(NULL, 0);
    agentSntpClientData.agentSntpClientLastAttemptTime = MakeOctetString(NULL, 0);
    agentSntpClientData.agentSntpClientServerAddress = MakeOctetString(NULL, 0);
    agentSntpClientData.agentSntpClientServerRefClkId = MakeOctetString(NULL, 0);

    firstTime = L7_FALSE;
  }

  switch (nominator)
  {
  case -1:
  case I_agentSntpClientVersion:
    if (snmpAgentSntpClientVersionGet(&agentSntpClientData.agentSntpClientVersion) == L7_SUCCESS)
      SET_VALID(I_agentSntpClientVersion, agentSntpClientData.valid);
    if (nominator != -1)
      break;
  
  case I_agentSntpClientSupportedMode:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((snmpAgentSntpClientSupportedModeGet(&(snmp_buffer[0])) == L7_SUCCESS) &&
        SafeMakeOctetString(&agentSntpClientData.agentSntpClientSupportedMode, snmp_buffer, 1))
      SET_VALID(I_agentSntpClientSupportedMode, agentSntpClientData.valid);
    if (nominator != -1)
      break;

  case I_agentSntpClientMode:
    if (snmpAgentSntpClientModeGet(&agentSntpClientData.agentSntpClientMode) == L7_SUCCESS)
      SET_VALID(I_agentSntpClientMode, agentSntpClientData.valid);
    if (nominator != -1)
      break;

  case I_agentSntpClientPort:
    if (usmDbSntpClientPortGet(USMDB_UNIT_CURRENT, &temp_short) == L7_SUCCESS)
    {
      /* cast short value into unsigned integer */
      agentSntpClientData.agentSntpClientPort = (L7_uint32)temp_short;
      SET_VALID(I_agentSntpClientPort, agentSntpClientData.valid);
    }
    if (nominator != -1)
      break;

  case I_agentSntpClientLastUpdateTime:
    if ((usmDbSntpLastUpdateTimeGet(USMDB_UNIT_CURRENT, &temp_unsigned) == L7_SUCCESS) &&
        SafeMakeDateAndTime(&agentSntpClientData.agentSntpClientLastUpdateTime, temp_unsigned) == L7_TRUE)
      SET_VALID(I_agentSntpClientLastUpdateTime, agentSntpClientData.valid);
    if (nominator != -1)
      break;

  case I_agentSntpClientLastAttemptTime:

    if ((usmDbSntpLastAttemptTimeGet(USMDB_UNIT_CURRENT, &temp_unsigned) == L7_SUCCESS) &&
        SafeMakeDateAndTime(&agentSntpClientData.agentSntpClientLastAttemptTime, temp_unsigned) == L7_TRUE)
      SET_VALID(I_agentSntpClientLastAttemptTime, agentSntpClientData.valid);

    if (nominator != -1)
      break;

  case I_agentSntpClientLastAttemptStatus:
    if (snmpAgentSntpClientLastAttemptStatusGet(&agentSntpClientData.agentSntpClientLastAttemptStatus) == L7_SUCCESS)
      SET_VALID(I_agentSntpClientLastAttemptStatus, agentSntpClientData.valid);
    if (nominator != -1)
      break;

  case I_agentSntpClientServerAddressType:
    if (snmpAgentSntpClientServerAddressTypeGet(&agentSntpClientData.agentSntpClientServerAddressType) == L7_SUCCESS)
      SET_VALID(I_agentSntpClientServerAddressType, agentSntpClientData.valid);
    if (nominator != -1)
      break;

  case I_agentSntpClientServerAddress:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((usmDbSntpServerIpAddressGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS) &&
        SafeMakeOctetStringFromTextExact(&agentSntpClientData.agentSntpClientServerAddress,
                                         snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentSntpClientServerAddress, agentSntpClientData.valid);
    if (nominator != -1)
      break;

  case I_agentSntpClientServerMode:
    if (usmDbSntpServerModeGet(USMDB_UNIT_CURRENT, &agentSntpClientData.agentSntpClientServerMode) == L7_SUCCESS)
      SET_VALID(I_agentSntpClientServerMode, agentSntpClientData.valid);
    if (nominator != -1)
      break;

  case I_agentSntpClientServerStratum:
    if (usmDbSntpServerStratumGet(USMDB_UNIT_CURRENT, &agentSntpClientData.agentSntpClientServerStratum) == L7_SUCCESS)
      SET_VALID(I_agentSntpClientServerStratum, agentSntpClientData.valid);
    if (nominator != -1)
      break;

  case I_agentSntpClientServerRefClkId:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((usmDbSntpServerRefIdGet(USMDB_UNIT_CURRENT, snmp_buffer) == L7_SUCCESS) &&
        (SafeMakeOctetStringFromTextExact(&agentSntpClientData.agentSntpClientServerRefClkId,
                                          snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentSntpClientServerRefClkId, agentSntpClientData.valid);
    if (nominator != -1)
      break;

#ifdef I_agentSntpClientPollInterval
    case I_agentSntpClientPollInterval:
      if ( L7_SUCCESS == usmDbSntpClientUnicastPollIntervalGet(USMDB_UNIT_CURRENT, &agentSntpClientData.agentSntpClientPollInterval) )
      {
        SET_VALID( I_agentSntpClientPollInterval, agentSntpClientData.valid );
      }
      if (nominator != -1)
        break;
#endif /* I_agentSntpClientPollInterval */

    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != -1 && !VALID(nominator, agentSntpClientData.valid))
    return(NULL);

  return(&agentSntpClientData);
}

#ifdef SETS
int
k_agentSntpClient_test(ObjectInfo *object, ObjectSyntax *value,
                       doList_t *dp, ContextInfo *contextInfo)
{

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID) == L7_FALSE)
  {
    return NO_SUCH_NAME_ERROR;
  }

  switch (object->nominator)
  {
      #ifdef I_agentSntpClientMode
      case I_agentSntpClientMode:
      if ( L7_SUCCESS != snmpAgentSntpClientModeTest(value->ul_value) )
      {
        return WRONG_VALUE_ERROR;
      }
      break;
      #endif

      #ifdef I_agentSntpClientPort
      case I_agentSntpClientPort:
      break;
      #endif

      default:
      return GEN_ERROR;
  }

    return NO_ERROR;
}

int
k_agentSntpClient_ready(ObjectInfo *object, ObjectSyntax *value, 
                        doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSntpClient_set(agentSntpClient_t *data,
                      ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];

  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_agentSntpClientMode, data->valid))
  {
    if (snmpAgentSntpClientModeSet(data->agentSntpClientMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    
    SET_VALID(I_agentSntpClientMode, tempValid);
  }

  if (VALID(I_agentSntpClientPort, data->valid))
  {
    if (usmDbSntpClientPortSet(USMDB_UNIT_CURRENT, (L7_ushort16) data->agentSntpClientPort) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    
    SET_VALID(I_agentSntpClientPort, tempValid);
  }

  return(NO_ERROR);
}

#ifdef SR_agentSntpClient_UNDO
/* add #define SR_agentSntpClient_UNDO in sitedefs.h to
 * include the undo routine for the agentSntpClient family.
 */
int
agentSntpClient_undo(doList_t *doHead, doList_t *doCur,
                     ContextInfo *contextInfo)
{
  agentSntpClient_t *data = (agentSntpClient_t *) doCur->data;
  agentSntpClient_t *undodata = (agentSntpClient_t *) doCur->undodata;
  agentSntpClient_t *setdata = NULL;
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
  if (k_agentSntpClient_set(setdata, contextInfo, function) == NO_ERROR)
    return(NO_ERROR);

  return(UNDO_FAILED_ERROR);
}
#endif /* SR_agentSntpClient_UNDO */

#endif /* SETS */

agentSntpClientUnicast_t *
k_agentSntpClientUnicast_get(int serialNum, ContextInfo *contextInfo,
                             int nominator)
{
  static agentSntpClientUnicast_t agentSntpClientUnicastData;
  
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_UNICAST_FEATURE_ID) != L7_TRUE)
    return(NULL);

  switch (nominator)
  {
  case -1:
  case I_agentSntpClientUnicastPollInterval:
    if (usmDbSntpClientUnicastPollIntervalGet(USMDB_UNIT_CURRENT, &agentSntpClientUnicastData.agentSntpClientUnicastPollInterval) == L7_SUCCESS)
      SET_VALID(I_agentSntpClientUnicastPollInterval, agentSntpClientUnicastData.valid);
    if (nominator != -1)
      break;

  case I_agentSntpClientUnicastPollTimeout:
    if (usmDbSntpClientUnicastPollTimeoutGet(USMDB_UNIT_CURRENT, &agentSntpClientUnicastData.agentSntpClientUnicastPollTimeout) == L7_SUCCESS)
      SET_VALID(I_agentSntpClientUnicastPollTimeout, agentSntpClientUnicastData.valid);
    if (nominator != -1)
      break;

  case I_agentSntpClientUnicastPollRetry:
    if (usmDbSntpClientUnicastPollRetryGet(USMDB_UNIT_CURRENT, &agentSntpClientUnicastData.agentSntpClientUnicastPollRetry) == L7_SUCCESS)
      SET_VALID(I_agentSntpClientUnicastPollRetry, agentSntpClientUnicastData.valid);
    if (nominator != -1)
      break;

  case I_agentSntpClientUcastServerMaxEntries:
    if (usmDbSntpServerMaxEntriesGet(USMDB_UNIT_CURRENT, &agentSntpClientUnicastData.agentSntpClientUcastServerMaxEntries) == L7_SUCCESS)
      SET_VALID(I_agentSntpClientUcastServerMaxEntries, agentSntpClientUnicastData.valid);
    if (nominator != -1)
      break;

  case I_agentSntpClientUcastServerCurrEntries:
    if (usmDbSntpServerCurrentEntriesGet(USMDB_UNIT_CURRENT, &agentSntpClientUnicastData.agentSntpClientUcastServerCurrEntries) == L7_SUCCESS)
      SET_VALID(I_agentSntpClientUcastServerCurrEntries, agentSntpClientUnicastData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != -1 && !VALID(nominator, agentSntpClientUnicastData.valid))
    return(NULL);

  return(&agentSntpClientUnicastData);
}

#ifdef SETS
int
k_agentSntpClientUnicast_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSntpClientUnicast_ready(ObjectInfo *object, ObjectSyntax *value, 
                               doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSntpClientUnicast_set(agentSntpClientUnicast_t *data,
                             ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_UNICAST_FEATURE_ID) != L7_TRUE)
    return COMMIT_FAILED_ERROR;

  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_agentSntpClientUnicastPollInterval, data->valid))
  {
    if (usmDbSntpClientUnicastPollIntervalSet(USMDB_UNIT_CURRENT, data->agentSntpClientUnicastPollInterval) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    SET_VALID(I_agentSntpClientUnicastPollInterval, tempValid);
  }

  if (VALID(I_agentSntpClientUnicastPollTimeout, data->valid))
  {
    if (usmDbSntpClientUnicastPollTimeoutSet(USMDB_UNIT_CURRENT, data->agentSntpClientUnicastPollTimeout) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    
    SET_VALID(I_agentSntpClientUnicastPollTimeout, tempValid);
  }

  if (VALID(I_agentSntpClientUnicastPollRetry, data->valid))
  {
    if (usmDbSntpClientUnicastPollRetrySet(USMDB_UNIT_CURRENT, data->agentSntpClientUnicastPollRetry) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    
    SET_VALID(I_agentSntpClientUnicastPollRetry, tempValid);
  }

  return NO_ERROR;
}

#ifdef SR_agentSntpClientUnicast_UNDO
/* add #define SR_agentSntpClientUnicast_UNDO in sitedefs.h to
 * include the undo routine for the agentSntpClientUnicast family.
 */
int
agentSntpClientUnicast_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
  agentSntpClientUnicast_t *data = (agentSntpClientUnicast_t *) doCur->data;
  agentSntpClientUnicast_t *undodata = (agentSntpClientUnicast_t *) doCur->undodata;
  agentSntpClientUnicast_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_UNICAST_FEATURE_ID) != L7_TRUE)
    return UNDO_FAILED_ERROR;

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
  if (k_agentSntpClientUnicast_set(setdata, contextInfo, function) == NO_ERROR)
    return(NO_ERROR);

  return(UNDO_FAILED_ERROR);
}
#endif /* SR_agentSntpClientUnicast_UNDO */

#endif /* SETS */

agentSntpClientUcastServerEntry_t *
k_agentSntpClientUcastServerEntry_get(int serialNum, ContextInfo *contextInfo,
                                      int nominator,
                                      int searchType,
                                      SR_UINT32 agentSntpClientUcastServerIndex)
{
  static agentSntpClientUcastServerEntry_t agentSntpClientUcastServerEntryData;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 temp_unsigned;
  L7_ushort16 temp_short;
  static L7_BOOL firstTime = L7_TRUE;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_UNICAST_FEATURE_ID) != L7_TRUE)
    return(NULL);

  if (firstTime == L7_TRUE)
  {
    agentSntpClientUcastServerEntryData.agentSntpClientUcastServerAddress = MakeOctetString(NULL, 0);
    agentSntpClientUcastServerEntryData.agentSntpClientUcastServerLastUpdateTime = MakeOctetString(NULL, 0);
    agentSntpClientUcastServerEntryData.agentSntpClientUcastServerLastAttemptTime = MakeOctetString(NULL, 0);

    firstTime = L7_FALSE;
  }
  
  agentSntpClientUcastServerEntryData.agentSntpClientUcastServerIndex = agentSntpClientUcastServerIndex;
  SET_VALID(I_agentSntpClientUcastServerIndex, agentSntpClientUcastServerEntryData.valid);
  
  if ((searchType == EXACT) ?
      (snmpAgentSntpClientUcastServerEntryGet(agentSntpClientUcastServerEntryData.agentSntpClientUcastServerIndex) != L7_SUCCESS) :
      ((snmpAgentSntpClientUcastServerEntryGet(agentSntpClientUcastServerEntryData.agentSntpClientUcastServerIndex) != L7_SUCCESS) &&
       (snmpAgentSntpClientUcastServerEntryNextGet(&agentSntpClientUcastServerEntryData.agentSntpClientUcastServerIndex) != L7_SUCCESS)))
    return(NULL);
  
  
  switch (nominator)
  {
  case -1:
  case I_agentSntpClientUcastServerIndex:
    if (nominator != -1)
      break;

  case I_agentSntpClientUcastServerAddressType:
    if (snmpAgentSntpClientUcastServerAddressTypeGet(agentSntpClientUcastServerEntryData.agentSntpClientUcastServerIndex,
                                                     &agentSntpClientUcastServerEntryData.agentSntpClientUcastServerAddressType) == L7_SUCCESS)
      SET_VALID(I_agentSntpClientUcastServerAddressType, agentSntpClientUcastServerEntryData.valid);
    if (nominator != -1)
      break;

  case I_agentSntpClientUcastServerAddress:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((usmDbSntpServerTableIpAddressGet(USMDB_UNIT_CURRENT, agentSntpClientUcastServerEntryData.agentSntpClientUcastServerIndex, snmp_buffer) == L7_SUCCESS) &&
        SafeMakeOctetStringFromTextExact(&agentSntpClientUcastServerEntryData.agentSntpClientUcastServerAddress, snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentSntpClientUcastServerAddress, agentSntpClientUcastServerEntryData.valid);
    if (nominator != -1)
      break;

  case I_agentSntpClientUcastServerPort:
    if (usmDbSntpServerTablePortGet(USMDB_UNIT_CURRENT,
                                    agentSntpClientUcastServerEntryData.agentSntpClientUcastServerIndex,
                                    &temp_short)== L7_SUCCESS)
    {
      agentSntpClientUcastServerEntryData.agentSntpClientUcastServerPort = (L7_uint32) temp_short;
      SET_VALID(I_agentSntpClientUcastServerPort, agentSntpClientUcastServerEntryData.valid);
    }
    if (nominator != -1)
      break;

  case I_agentSntpClientUcastServerVersion:
    if (snmpAgentSntpClientUcastServerVersionGet(agentSntpClientUcastServerEntryData.agentSntpClientUcastServerIndex,
                                                 &agentSntpClientUcastServerEntryData.agentSntpClientUcastServerVersion) == L7_SUCCESS)
      SET_VALID(I_agentSntpClientUcastServerVersion, agentSntpClientUcastServerEntryData.valid);
    if (nominator != -1)
      break;

  case I_agentSntpClientUcastServerPrecedence:
    if (usmDbSntpServerTablePriorityGet(USMDB_UNIT_CURRENT,
                                        agentSntpClientUcastServerEntryData.agentSntpClientUcastServerIndex,
                                        &temp_short) == L7_SUCCESS)
    {
      agentSntpClientUcastServerEntryData.agentSntpClientUcastServerPrecedence = (L7_uint32)temp_short;
      SET_VALID(I_agentSntpClientUcastServerPrecedence, agentSntpClientUcastServerEntryData.valid);
    }
    if (nominator != -1)
      break;

  case I_agentSntpClientUcastServerLastUpdateTime:
    if ((usmDbSntpServerStatsLastUpdateTimeGet(USMDB_UNIT_CURRENT,
                                               agentSntpClientUcastServerEntryData.agentSntpClientUcastServerIndex,
                                               &temp_unsigned) == L7_SUCCESS) &&
        (SafeMakeDateAndTime(&agentSntpClientUcastServerEntryData.agentSntpClientUcastServerLastUpdateTime, temp_unsigned) == L7_TRUE))
      SET_VALID(I_agentSntpClientUcastServerLastUpdateTime, agentSntpClientUcastServerEntryData.valid);
    if (nominator != -1)
      break;

  case I_agentSntpClientUcastServerLastAttemptTime:
    if ((usmDbSntpServerStatsLastAttemptTimeGet(USMDB_UNIT_CURRENT,
                                                agentSntpClientUcastServerEntryData.agentSntpClientUcastServerIndex,
                                                &temp_unsigned) == L7_SUCCESS) &&
        (SafeMakeDateAndTime(&agentSntpClientUcastServerEntryData.agentSntpClientUcastServerLastAttemptTime, temp_unsigned) == L7_TRUE))
      SET_VALID(I_agentSntpClientUcastServerLastAttemptTime, agentSntpClientUcastServerEntryData.valid);
    if (nominator != -1)
      break;

  case I_agentSntpClientUcastServerLastAttemptStatus:
    if (snmpAgentSntpClientUcastServerLastAttemptStatusGet(agentSntpClientUcastServerEntryData.agentSntpClientUcastServerIndex,
                                                           &agentSntpClientUcastServerEntryData.agentSntpClientUcastServerLastAttemptStatus) == L7_SUCCESS)
      SET_VALID(I_agentSntpClientUcastServerLastAttemptStatus, agentSntpClientUcastServerEntryData.valid);
    if (nominator != -1)
      break;

  case I_agentSntpClientUcastServerNumRequests:
    if (usmDbSntpServerStatsUnicastServerNumRequestsGet(USMDB_UNIT_CURRENT,
                                                        agentSntpClientUcastServerEntryData.agentSntpClientUcastServerIndex,
                                                        &agentSntpClientUcastServerEntryData.agentSntpClientUcastServerNumRequests) == L7_SUCCESS)
      SET_VALID(I_agentSntpClientUcastServerNumRequests, agentSntpClientUcastServerEntryData.valid);
    if (nominator != -1)
      break;

  case I_agentSntpClientUcastServerNumFailedRequests:
    if (usmDbSntpServerStatsUnicastServerNumFailedRequestsGet(USMDB_UNIT_CURRENT,
                                                              agentSntpClientUcastServerEntryData.agentSntpClientUcastServerIndex,
                                                              &agentSntpClientUcastServerEntryData.agentSntpClientUcastServerNumFailedRequests) == L7_SUCCESS)
      SET_VALID(I_agentSntpClientUcastServerNumFailedRequests, agentSntpClientUcastServerEntryData.valid);
    if (nominator != -1)
      break;

  case I_agentSntpClientUcastServerRowStatus:
    if (snmpAgentSntpClientUcastServerRowStatusGet(agentSntpClientUcastServerEntryData.agentSntpClientUcastServerIndex,
                                                   &agentSntpClientUcastServerEntryData.agentSntpClientUcastServerRowStatus) == L7_SUCCESS)
      SET_VALID(I_agentSntpClientUcastServerRowStatus, agentSntpClientUcastServerEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != -1 && !VALID(nominator, agentSntpClientUcastServerEntryData.valid))
    return(NULL);

  return(&agentSntpClientUcastServerEntryData);
}

#ifdef SETS
int
k_agentSntpClientUcastServerEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                       doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSntpClientUcastServerEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                        doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSntpClientUcastServerEntry_set_defaults(doList_t *dp)
{
    agentSntpClientUcastServerEntry_t *data = (agentSntpClientUcastServerEntry_t *) (dp->data);

    if ((data->agentSntpClientUcastServerAddress = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentSntpClientUcastServerLastUpdateTime = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentSntpClientUcastServerLastAttemptTime = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->agentSntpClientUcastServerLastAttemptStatus = D_agentSntpClientUcastServerLastAttemptStatus_other;
    data->agentSntpClientUcastServerNumRequests = (SR_UINT32) 0;
    data->agentSntpClientUcastServerNumFailedRequests = (SR_UINT32) 0;

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentSntpClientUcastServerEntry_set(agentSntpClientUcastServerEntry_t *data,
                                      ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_UNICAST_FEATURE_ID) != L7_TRUE)
    return COMMIT_FAILED_ERROR;

  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_agentSntpClientUcastServerRowStatus, data->valid))
  {
    if (snmpAgentSntpClientUcastServerRowStatusSet(data->agentSntpClientUcastServerIndex,
                                                   data->agentSntpClientUcastServerRowStatus) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    SET_VALID(I_agentSntpClientUcastServerRowStatus, tempValid);
  }

  if (VALID(I_agentSntpClientUcastServerAddressType, data->valid))
  {
    if (snmpAgentSntpClientUcastServerAddressTypeSet(data->agentSntpClientUcastServerIndex,
                                                     data->agentSntpClientUcastServerAddressType) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }

    SET_VALID(I_agentSntpClientUcastServerAddressType, tempValid);
  }

  if (VALID(I_agentSntpClientUcastServerAddress, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    strncpy(snmp_buffer, data->agentSntpClientUcastServerAddress->octet_ptr, 
            data->agentSntpClientUcastServerAddress->length);
    if (usmDbSntpServerTableIpAddressSet(USMDB_UNIT_CURRENT,
                                         data->agentSntpClientUcastServerIndex,
                                         snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    SET_VALID(I_agentSntpClientUcastServerAddress, tempValid);
  }

  if (VALID(I_agentSntpClientUcastServerPort, data->valid))
  {
    if (usmDbSntpServerTablePortSet(USMDB_UNIT_CURRENT,
                                    data->agentSntpClientUcastServerIndex,
                                    (L7_uint32)data->agentSntpClientUcastServerPort) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    SET_VALID(I_agentSntpClientUcastServerPort, tempValid);
  }

  if (VALID(I_agentSntpClientUcastServerVersion, data->valid))
  {
    if (snmpAgentSntpClientUcastServerVersionSet(data->agentSntpClientUcastServerIndex,
                                                 data->agentSntpClientUcastServerVersion) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    SET_VALID(I_agentSntpClientUcastServerVersion, tempValid);
  }

  if (VALID(I_agentSntpClientUcastServerPrecedence, data->valid))
  {
    if (usmDbSntpServerTablePrioritySet(USMDB_UNIT_CURRENT,
                                        data->agentSntpClientUcastServerIndex,
                                        (L7_uint32)data->agentSntpClientUcastServerPrecedence) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    SET_VALID(I_agentSntpClientUcastServerPrecedence, tempValid);
  }

  return(NO_ERROR);
}
#ifdef SR_agentSntpClientUcastServerEntry_UNDO
/* add #define SR_agentSntpClientUcastServerEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSntpClientUcastServerEntry family.
 */
int
agentSntpClientUcastServerEntry_undo(doList_t *doHead, doList_t *doCur,
                                     ContextInfo *contextInfo)
{
  agentSntpClientUcastServerEntry_t *data = (agentSntpClientUcastServerEntry_t *) doCur->data;
  agentSntpClientUcastServerEntry_t *undodata = (agentSntpClientUcastServerEntry_t *) doCur->undodata;
  agentSntpClientUcastServerEntry_t *setdata = undodata;
  L7_int32 function = SR_UNKNOWN;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_UNICAST_FEATURE_ID) != L7_TRUE)
    return UNDO_FAILED_ERROR;

  if ( data == NULL )
    return(UNDO_FAILED_ERROR);

  /* if there's no previous data, then assume a creation request */
  if ( undodata == NULL )
  {
    /* undoing a create, check that the entry exists */
    if (snmpAgentSntpClientUcastServerEntryGet(data->agentSntpClientUcastServerIndex) != L7_SUCCESS)
    {
      return(NO_ERROR);
    }
    ZERO_VALID(data->valid);
    data->agentSntpClientUcastServerRowStatus = D_agentSntpClientUcastServerRowStatus_destroy;
    SET_VALID(I_agentSntpClientUcastServerIndex, data->valid);
    SET_VALID(I_agentSntpClientUcastServerRowStatus, data->valid);
    function = SR_DELETE;
    /* point setdata to new data structure */
    setdata = data;
  }
  else
  {
    /* undoing a modify, replace the original data */
    function = SR_ADD_MODIFY;
  
    /* if the rowstatus was changed during this operation */
    if (VALID(I_agentSntpClientUcastServerRowStatus, data->valid) &&
        data->agentSntpClientUcastServerRowStatus != undodata->agentSntpClientUcastServerRowStatus)
    {
      /* handle unusual state transitions */
  
      /* undo a row deletion */
      if (data->agentSntpClientUcastServerRowStatus == D_agentSntpClientUcastServerRowStatus_destroy)
      {
        /* depending on the old state, pick the correct create request */
        if (undodata->agentSntpClientUcastServerRowStatus == D_agentSntpClientUcastServerRowStatus_notInService)
        {
          undodata->agentSntpClientUcastServerRowStatus = D_agentSntpClientUcastServerRowStatus_createAndWait;
        }
        else
        {
          undodata->agentSntpClientUcastServerRowStatus = D_agentSntpClientUcastServerRowStatus_createAndGo;
        }
        SET_VALID(I_agentSntpClientUcastServerRowStatus, undodata->valid);
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
  if (k_agentSntpClientUcastServerEntry_set(undodata, contextInfo, function) == NO_ERROR)
    return(NO_ERROR);

  return(UNDO_FAILED_ERROR);
}
#endif /* SR_agentSntpClientUcastServerEntry_UNDO */

#endif /* SETS */

agentSntpClientBroadcast_t *
k_agentSntpClientBroadcast_get(int serialNum, ContextInfo *contextInfo,
                               int nominator)
{
  static agentSntpClientBroadcast_t agentSntpClientBroadcastData;
  
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_BROADCAST_FEATURE_ID) != L7_TRUE)
    return(NULL);

  switch (nominator)
  {
  case -1:
  case I_agentSntpClientBroadcastCount:
    if (usmDbSntpBroadcastCountGet(USMDB_UNIT_CURRENT,
                                   &agentSntpClientBroadcastData.agentSntpClientBroadcastCount) == L7_SUCCESS)
      SET_VALID(I_agentSntpClientBroadcastCount, agentSntpClientBroadcastData.valid);
    if (nominator != -1)
      break;
  
  case I_agentSntpClientBroadcastInterval:
    if (usmDbSntpClientBroadcastPollIntervalGet(USMDB_UNIT_CURRENT,
                                                &agentSntpClientBroadcastData.agentSntpClientBroadcastInterval) == L7_SUCCESS)
      SET_VALID(I_agentSntpClientBroadcastInterval, agentSntpClientBroadcastData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != -1 && !VALID(nominator, agentSntpClientBroadcastData.valid))
    return(NULL);

  return(&agentSntpClientBroadcastData);
}

#ifdef SETS
int
k_agentSntpClientBroadcast_test(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSntpClientBroadcast_ready(ObjectInfo *object, ObjectSyntax *value, 
                                 doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSntpClientBroadcast_set(agentSntpClientBroadcast_t *data,
                               ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_BROADCAST_FEATURE_ID) != L7_TRUE)
    return(COMMIT_FAILED_ERROR);

  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_agentSntpClientBroadcastInterval, data->valid))
  {
    if (usmDbSntpClientBroadcastPollIntervalSet(USMDB_UNIT_CURRENT,
                                                data->agentSntpClientBroadcastInterval) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    SET_VALID(I_agentSntpClientBroadcastInterval, tempValid);
  }

  return(NO_ERROR);
}

#ifdef SR_agentSntpClientBroadcast_UNDO
/* add #define SR_agentSntpClientBroadcast_UNDO in sitedefs.h to
 * include the undo routine for the agentSntpClientBroadcast family.
 */
int
agentSntpClientBroadcast_undo(doList_t *doHead, doList_t *doCur,
                              ContextInfo *contextInfo)
{
  agentSntpClientBroadcast_t *data = (agentSntpClientBroadcast_t *) doCur->data;
  agentSntpClientBroadcast_t *undodata = (agentSntpClientBroadcast_t *) doCur->undodata;
  agentSntpClientBroadcast_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_BROADCAST_FEATURE_ID) != L7_TRUE)
    return(UNDO_FAILED_ERROR);

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
  if (k_agentSntpClientBroadcast_set(setdata, contextInfo, function) == NO_ERROR)
    return(NO_ERROR);

  return(UNDO_FAILED_ERROR);
}
#endif /* SR_agentSntpClientBroadcast_UNDO */

#endif /* SETS */

