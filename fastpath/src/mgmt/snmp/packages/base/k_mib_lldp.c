/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\mgmt\snmp\packages\base\k_mib_lldp.c
*
* @purpose  Provide interface to LLDP MIB
*
* @component SNMP
*
* @comments
*
* @create 04/15/2005
*
* @author ikiran
* @end
*
**********************************************************************/
#include <k_private_base.h>
#include "k_mib_lldp_api.h"
#include "usmdb_lldp_api.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"

lldpConfiguration_t *
k_lldpConfiguration_get(int serialNum, ContextInfo *contextInfo,
                        int nominator)
{
  static lldpConfiguration_t lldpConfigurationData;

  switch (nominator)
  {
  case -1:
    /* fallthrough all cases except default */
  case I_lldpMessageTxInterval:
    if (usmDbLldpTxIntervalGet(&lldpConfigurationData.lldpMessageTxInterval) 
        == L7_SUCCESS)
      SET_VALID(I_lldpMessageTxInterval, lldpConfigurationData.valid);
    if (nominator != -1) break;

  case I_lldpMessageTxHoldMultiplier:
    if (usmDbLldpTxHoldGet(&lldpConfigurationData.lldpMessageTxHoldMultiplier) 
        == L7_SUCCESS)
      SET_VALID(I_lldpMessageTxHoldMultiplier, lldpConfigurationData.valid);
    if (nominator != -1) break;

  case I_lldpReinitDelay:
    if (usmDbLldpTxReinitDelayGet(&lldpConfigurationData.lldpReinitDelay) 
        == L7_SUCCESS)
      SET_VALID(I_lldpReinitDelay, lldpConfigurationData.valid);
    if (nominator != -1) break;

  case I_lldpTxDelay:
    if (usmDbLldpTxDelayGet(&lldpConfigurationData.lldpTxDelay) 
        == L7_SUCCESS)
      SET_VALID(I_lldpTxDelay, lldpConfigurationData.valid);
    if (nominator != -1) break;

  case I_lldpNotificationInterval:
    if (usmDbLldpNotificationIntervalGet(
                                        &lldpConfigurationData.lldpNotificationInterval) == L7_SUCCESS)
      SET_VALID(I_lldpNotificationInterval, lldpConfigurationData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;

  }

  if (nominator >= 0 && !VALID(nominator, lldpConfigurationData.valid))
    return(NULL);

  return(&lldpConfigurationData);
}

#ifdef SETS
int
k_lldpConfiguration_test(ObjectInfo *object, ObjectSyntax *value,
                         doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_lldpConfiguration_ready(ObjectInfo *object, ObjectSyntax *value, 
                          doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_lldpConfiguration_set(lldpConfiguration_t *data,
                        ContextInfo *contextInfo, int function)
{
  L7_char8 tempValid[sizeof(data->valid)];

  memset(tempValid, 0, sizeof(tempValid));

  if (VALID(I_lldpMessageTxInterval, data->valid))
  {
    if (usmDbLldpTxIntervalSet(data->lldpMessageTxInterval) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    SET_VALID(I_lldpMessageTxInterval, tempValid);
  }

  if (VALID(I_lldpMessageTxHoldMultiplier, data->valid))
  {
    if (usmDbLldpTxHoldSet(data->lldpMessageTxHoldMultiplier) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    SET_VALID(I_lldpMessageTxHoldMultiplier, tempValid);
  }

  if (VALID(I_lldpReinitDelay, data->valid))
  {
    if (usmDbLldpTxReinitDelaySet(data->lldpReinitDelay) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    SET_VALID(I_lldpReinitDelay, tempValid);
  }

  if (VALID(I_lldpTxDelay, data->valid))
  {
    if (usmDbLldpTxDelaySet(data->lldpTxDelay) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    SET_VALID(I_lldpTxDelay, tempValid);
  }

  if (VALID(I_lldpNotificationInterval, data->valid))
  {
    if (usmDbLldpNotificationIntervalSet(data->lldpNotificationInterval) 
        != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    SET_VALID(I_lldpNotificationInterval, tempValid);
  }

  return(NO_ERROR);
}

  #ifdef SR_lldpConfiguration_UNDO
/* add #define SR_lldpConfiguration_UNDO in sitedefs.h to
 * include the undo routine for the lldpConfiguration family.
 */
int
lldpConfiguration_undo(doList_t *doHead, doList_t *doCur,
                       ContextInfo *contextInfo)
{
  lldpConfiguration_t *data = (lldpConfiguration_t *) doCur->data;
  lldpConfiguration_t *undodata = (lldpConfiguration_t *) doCur->undodata;
  lldpConfiguration_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
   */
  if (data == NULL || undodata == NULL)
    return(UNDO_FAILED_ERROR);

  memcpy(undodata->valid, data->valid, sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if (k_lldpConfiguration_set(setdata, contextInfo, function) == NO_ERROR)
    return(NO_ERROR);

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_lldpConfiguration_UNDO */

#endif /* SETS */

lldpPortConfigEntry_t *
k_lldpPortConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator,
                          int searchType,
                          SR_INT32 lldpPortConfigPortNum)
{
  static lldpPortConfigEntry_t lldpPortConfigEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32      intIfNum;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    lldpPortConfigEntryData.lldpPortConfigTLVsTxEnable = MakeOctetString(NULL, 0);
    lldpPortConfigEntryData.lldpXMedPortCapSupported = MakeOctetString(NULL, 0);
    lldpPortConfigEntryData.lldpXMedPortConfigTLVsTxEnable = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(lldpPortConfigEntryData.valid);
  lldpPortConfigEntryData.lldpPortConfigPortNum = lldpPortConfigPortNum;
  SET_VALID(I_lldpPortConfigPortNum, lldpPortConfigEntryData.valid);

  if ((searchType == EXACT ? 
       ( usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT, lldpPortConfigEntryData.lldpPortConfigPortNum) != L7_SUCCESS ) : 
       ( usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT, lldpPortConfigEntryData.lldpPortConfigPortNum) != L7_SUCCESS && 
         usmDbGetNextVisibleExtIfNumber(lldpPortConfigEntryData.lldpPortConfigPortNum, &lldpPortConfigEntryData.lldpPortConfigPortNum) != L7_SUCCESS ) ) ||
      usmDbIntIfNumFromExtIfNum(lldpPortConfigEntryData.lldpPortConfigPortNum, &intIfNum) != L7_SUCCESS )
  {
    ZERO_VALID(lldpPortConfigEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
    /* fallthrough all cases except default */
  case I_lldpPortConfigPortNum:
    if (nominator != -1) break;
  case I_lldpPortConfigAdminStatus:
    if (snmpLldpIntfAdminModeGet(intIfNum, &lldpPortConfigEntryData.lldpPortConfigAdminStatus) == L7_SUCCESS)
      SET_VALID(I_lldpPortConfigAdminStatus, lldpPortConfigEntryData.valid);
    if (nominator != -1) break;

  case I_lldpPortConfigNotificationEnable:
    if (snmpLldpIntfNoticationEnableGet(intIfNum, &lldpPortConfigEntryData.lldpPortConfigNotificationEnable) == L7_SUCCESS)
      SET_VALID(I_lldpPortConfigNotificationEnable, 
                lldpPortConfigEntryData.valid);
    if (nominator != -1) break;

  case I_lldpPortConfigTLVsTxEnable:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    if ((snmpLldpIntfTLVsTxEnableGet(intIfNum, &snmp_buffer[0]) == L7_SUCCESS) && 
        (SafeMakeOctetString(&lldpPortConfigEntryData.lldpPortConfigTLVsTxEnable, snmp_buffer, 1) == L7_TRUE))
      SET_VALID(I_lldpPortConfigTLVsTxEnable, lldpPortConfigEntryData.valid);
    if (nominator != -1) break;

  case I_lldpXMedPortCapSupported:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    if (snmpLldpXMedPortCapSupportedGet(intIfNum, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetString(&lldpPortConfigEntryData.lldpXMedPortCapSupported, snmp_buffer, 1) == L7_TRUE))
      SET_VALID(I_lldpXMedPortCapSupported, lldpPortConfigEntryData.valid);
    if (nominator != -1) break;

  case I_lldpXMedPortConfigTLVsTxEnable:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    if (snmpLldpXMedPortConfigTLVsTxEnableGet(intIfNum, snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOctetString(&lldpPortConfigEntryData.lldpXMedPortConfigTLVsTxEnable, snmp_buffer, 1) == L7_TRUE))
      SET_VALID(I_lldpXMedPortConfigTLVsTxEnable, lldpPortConfigEntryData.valid);
    if (nominator != -1) break;

  case I_lldpXMedPortConfigNotifEnable:
    if (snmpLldpXMedPortConfigNotifEnableGet(intIfNum, &lldpPortConfigEntryData.lldpXMedPortConfigNotifEnable) == L7_SUCCESS)
      SET_VALID(I_lldpXMedPortConfigNotifEnable, lldpPortConfigEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, lldpPortConfigEntryData.valid))
    return(NULL);

  return(&lldpPortConfigEntryData);
}

#ifdef SETS
int
k_lldpPortConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_lldpPortConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                            doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_lldpPortConfigEntry_set_defaults(doList_t *dp)
{
  lldpPortConfigEntry_t *data = (lldpPortConfigEntry_t *) (dp->data);

  data->lldpPortConfigAdminStatus = D_lldpPortConfigAdminStatus_txAndRx;
  data->lldpPortConfigNotificationEnable = D_lldpPortConfigNotificationEnable_false;
  if ((data->lldpPortConfigTLVsTxEnable = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->lldpXMedPortCapSupported = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->lldpXMedPortConfigTLVsTxEnable = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_lldpPortConfigEntry_set(lldpPortConfigEntry_t *data,
                          ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case 
   * when the set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32      intIfNum;

  memset(tempValid, 0, sizeof(tempValid));
  memset(snmp_buffer, 0, SNMP_BUFFER_LEN);

  if (usmDbIntIfNumFromExtIfNum(data->lldpPortConfigPortNum, &intIfNum) != L7_SUCCESS)
  {
    ZERO_VALID(data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (usmDbPhysicalIntIfNumberCheck(USMDB_UNIT_CURRENT, intIfNum) != L7_SUCCESS)
  {
    ZERO_VALID(data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_lldpPortConfigAdminStatus, data->valid))
  {
    if (snmpLldpIntfAdminModeSet(intIfNum,
                                 data->lldpPortConfigAdminStatus) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    SET_VALID(I_lldpPortConfigAdminStatus, tempValid);
  }

  if (VALID(I_lldpPortConfigNotificationEnable, data->valid))
  {
    if (snmpLldpIntfNoticationEnableSet(intIfNum,
                                        data->lldpPortConfigNotificationEnable) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    SET_VALID(I_lldpPortConfigNotificationEnable, tempValid);
  }

  if (VALID(I_lldpPortConfigTLVsTxEnable, data->valid))
  {
    memcpy(snmp_buffer, data->lldpPortConfigTLVsTxEnable->octet_ptr,
           data->lldpPortConfigTLVsTxEnable->length);
    if (snmpLldpIntfTLVsTxEnableSet(intIfNum, snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    SET_VALID(I_lldpPortConfigTLVsTxEnable, tempValid);
  }

  if (VALID(I_lldpXMedPortConfigTLVsTxEnable, data->valid))
  {
    memcpy(snmp_buffer, data->lldpXMedPortConfigTLVsTxEnable->octet_ptr,
           data->lldpXMedPortConfigTLVsTxEnable->length);
    if (snmpLldpXMedPortConfigTLVsTxEnableSet(intIfNum, snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    SET_VALID(I_lldpXMedPortConfigTLVsTxEnable, tempValid);
  }

  if (VALID(I_lldpXMedPortConfigNotifEnable, data->valid))
  {
    if (snmpLldpXMedPortConfigNotifEnableSet(intIfNum,
                                             data->lldpXMedPortConfigNotifEnable) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    SET_VALID(I_lldpXMedPortConfigNotifEnable, tempValid);
  }

  return(NO_ERROR);
}

  #ifdef SR_lldpPortConfigEntry_UNDO
/* add #define SR_lldpPortConfigEntry_UNDO in sitedefs.h to
 * include the undo routine for the lldpPortConfigEntry family.
 */
int
lldpPortConfigEntry_undo(doList_t *doHead, doList_t *doCur,
                         ContextInfo *contextInfo)
{
  lldpPortConfigEntry_t *data = (lldpPortConfigEntry_t *) doCur->data;
  lldpPortConfigEntry_t *undodata = (lldpPortConfigEntry_t *) doCur->undodata;
  lldpPortConfigEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if (data == NULL || undodata == NULL)
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && 
      (k_lldpPortConfigEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_lldpPortConfigEntry_UNDO */

#endif /* SETS */

lldpStatistics_t *
k_lldpStatistics_get(int serialNum, ContextInfo *contextInfo,
                     int nominator)
{
  static lldpStatistics_t lldpStatisticsData;

  switch (nominator)
  {
  case -1:
    /* fallthrough all cases except default */
  case I_lldpStatsRemTablesLastChangeTime:
    if (usmDbLldpStatsRemoteLastUpdateGet(&lldpStatisticsData.lldpStatsRemTablesLastChangeTime) == L7_SUCCESS)
    {
      /* This API returns the value in seconds. Need to multiply with 100
       * for timeticks */
      lldpStatisticsData.lldpStatsRemTablesLastChangeTime*=100;
      SET_VALID(I_lldpStatsRemTablesLastChangeTime, lldpStatisticsData.valid);
    }
    if (nominator != -1) break;

  case I_lldpStatsRemTablesInserts:
    if (usmDbLldpStatsRemoteInsertsGet(&lldpStatisticsData.lldpStatsRemTablesInserts) 
        == L7_SUCCESS)
      SET_VALID(I_lldpStatsRemTablesInserts, lldpStatisticsData.valid);
    if (nominator != -1) break;

  case I_lldpStatsRemTablesDeletes:
    if (usmDbLldpStatsRemoteDeletesGet(&lldpStatisticsData.lldpStatsRemTablesDeletes) == L7_SUCCESS)
      SET_VALID(I_lldpStatsRemTablesDeletes, lldpStatisticsData.valid);
    if (nominator != -1) break;

  case I_lldpStatsRemTablesDrops:
    if (usmDbLldpStatsRemoteDropsGet(&lldpStatisticsData.lldpStatsRemTablesDrops) == L7_SUCCESS)
      SET_VALID(I_lldpStatsRemTablesDrops, lldpStatisticsData.valid);
    if (nominator != -1) break;

  case I_lldpStatsRemTablesAgeouts:
    if (usmDbLldpStatsRemoteAgeoutsGet(&lldpStatisticsData.lldpStatsRemTablesAgeouts) == L7_SUCCESS)
      SET_VALID(I_lldpStatsRemTablesAgeouts, lldpStatisticsData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;

  }

  if (nominator >= 0 && !VALID(nominator, lldpStatisticsData.valid))
    return(NULL);

  return(&lldpStatisticsData);
}

lldpStatsTxPortEntry_t *
k_lldpStatsTxPortEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_INT32 lldpStatsTxPortNum)
{
  static lldpStatsTxPortEntry_t lldpStatsTxPortEntryData;
  L7_uint32      intIfNum;

  ZERO_VALID(lldpStatsTxPortEntryData.valid);

  if ((searchType == EXACT ? 
  (usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT, lldpStatsTxPortNum) != L7_SUCCESS) :
  (usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT,lldpStatsTxPortNum) != L7_SUCCESS) &&
  (usmDbNextPhysicalExtIfNumberGet(USMDB_UNIT_CURRENT,lldpStatsTxPortNum,&lldpStatsTxPortNum) != L7_SUCCESS)))
  {
    return(NULL);
  }

  if (usmDbIntIfNumFromExtIfNum(lldpStatsTxPortNum, &intIfNum) != L7_SUCCESS)
  {
    return(NULL);
  }

  lldpStatsTxPortEntryData.lldpStatsTxPortNum = lldpStatsTxPortNum;

  SET_VALID(I_lldpStatsTxPortNum, lldpStatsTxPortEntryData.valid);

  switch (nominator)
  {
  case -1:
    /* fallthrough all cases except default */
  case I_lldpStatsTxPortNum:
    if (nominator != -1) break;
  case I_lldpStatsTxPortFramesTotal:
    if (usmDbLldpStatsTxPortFramesTotalGet(intIfNum, &lldpStatsTxPortEntryData.lldpStatsTxPortFramesTotal) == L7_SUCCESS)
      SET_VALID(I_lldpStatsTxPortFramesTotal, lldpStatsTxPortEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, lldpStatsTxPortEntryData.valid))
    return(NULL);

  return(&lldpStatsTxPortEntryData);
}

lldpStatsRxPortEntry_t *
k_lldpStatsRxPortEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_INT32 lldpStatsRxPortNum)
{
  static lldpStatsRxPortEntry_t lldpStatsRxPortEntryData;
  L7_uint32      intIfNum;

  ZERO_VALID(lldpStatsRxPortEntryData.valid);
  if ((searchType == EXACT ? 
  (usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT, lldpStatsRxPortNum) != L7_SUCCESS) :
  (usmDbPhysicalExtIfNumberCheck(USMDB_UNIT_CURRENT,lldpStatsRxPortNum) != L7_SUCCESS) &&
  (usmDbNextPhysicalExtIfNumberGet(USMDB_UNIT_CURRENT,lldpStatsRxPortNum,&lldpStatsRxPortNum) != L7_SUCCESS)))
  {
    return(NULL);
  }

  if (usmDbIntIfNumFromExtIfNum(lldpStatsRxPortNum, &intIfNum) != L7_SUCCESS)
  {
    return(NULL);
  }

  lldpStatsRxPortEntryData.lldpStatsRxPortNum = lldpStatsRxPortNum;

  SET_VALID(I_lldpStatsRxPortNum, lldpStatsRxPortEntryData.valid);

  switch (nominator)
  {
  case -1:
    /* fallthrough all cases except default */
  case I_lldpStatsRxPortNum:
    if (nominator != -1) break;
  case I_lldpStatsRxPortFramesDiscardedTotal:
    if (usmDbLldpStatsRxPortFramesDiscardedGet(intIfNum, &lldpStatsRxPortEntryData.lldpStatsRxPortFramesDiscardedTotal) 
        == L7_SUCCESS)
      SET_VALID(I_lldpStatsRxPortFramesDiscardedTotal, 
                lldpStatsRxPortEntryData.valid);
    if (nominator != -1) break;

  case I_lldpStatsRxPortFramesErrors:
    if (usmDbLldpStatsRxPortFramesErrorsGet(intIfNum, &lldpStatsRxPortEntryData.lldpStatsRxPortFramesErrors) == L7_SUCCESS)
      SET_VALID(I_lldpStatsRxPortFramesErrors, lldpStatsRxPortEntryData.valid);
    if (nominator != -1) break;

  case I_lldpStatsRxPortFramesTotal:
    if (usmDbLldpStatsRxPortFramesTotalGet(intIfNum, &lldpStatsRxPortEntryData.lldpStatsRxPortFramesTotal) == L7_SUCCESS)
      SET_VALID(I_lldpStatsRxPortFramesTotal, lldpStatsRxPortEntryData.valid);
    if (nominator != -1) break;

  case I_lldpStatsRxPortTLVsDiscardedTotal:
    if (usmDbLldpStatsRxPortTLVsDiscardedGet(intIfNum, &lldpStatsRxPortEntryData.lldpStatsRxPortTLVsDiscardedTotal) 
        == L7_SUCCESS)
      SET_VALID(I_lldpStatsRxPortTLVsDiscardedTotal, 
                lldpStatsRxPortEntryData.valid);
    if (nominator != -1) break;

  case I_lldpStatsRxPortTLVsUnrecognizedTotal:
    if (usmDbLldpStatsRxPortTLVsUnrecognizedGet(intIfNum, &lldpStatsRxPortEntryData.lldpStatsRxPortTLVsUnrecognizedTotal) 
        == L7_SUCCESS)
      SET_VALID(I_lldpStatsRxPortTLVsUnrecognizedTotal, 
                lldpStatsRxPortEntryData.valid);
    if (nominator != -1) break;

  case I_lldpStatsRxPortAgeoutsTotal:
    if (usmDbLldpStatsRxPortAgeoutsGet(intIfNum, &lldpStatsRxPortEntryData.lldpStatsRxPortAgeoutsTotal) == L7_SUCCESS)
      SET_VALID(I_lldpStatsRxPortAgeoutsTotal, lldpStatsRxPortEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, lldpStatsRxPortEntryData.valid))
    return(NULL);

  return(&lldpStatsRxPortEntryData);
}

lldpLocalSystemData_t *
k_lldpLocalSystemData_get(int serialNum, ContextInfo *contextInfo,
                          int nominator)
{
  static lldpLocalSystemData_t lldpLocalSystemDataData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_ushort16 snmp_buffer_len;
  L7_ushort16 system_cap;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    lldpLocalSystemDataData.lldpLocChassisId = MakeOctetString(NULL, 0);
    lldpLocalSystemDataData.lldpLocSysName = MakeOctetString(NULL, 0);
    lldpLocalSystemDataData.lldpLocSysDesc = MakeOctetString(NULL, 0);
  }

  switch (nominator)
  {
  case -1:
    /* fallthrough all cases except default */
  case I_lldpLocChassisIdSubtype:
    if (usmDbLldpLocChassisIdSubtypeGet((lldpChassisIdSubtype_t*) &lldpLocalSystemDataData.lldpLocChassisIdSubtype) == L7_SUCCESS)
      SET_VALID(I_lldpLocChassisIdSubtype, lldpLocalSystemDataData.valid);
    if (nominator != -1) break;

  case I_lldpLocChassisId:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    if ((usmDbLldpLocChassisIdGet(snmp_buffer, &snmp_buffer_len) == L7_SUCCESS) &&
        (SafeMakeOctetString(&lldpLocalSystemDataData.lldpLocChassisId, snmp_buffer, snmp_buffer_len) == L7_TRUE))
      SET_VALID(I_lldpLocChassisId, lldpLocalSystemDataData.valid);
    if (nominator != -1) break;

  case I_lldpLocSysName:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    if ((usmDbLldpLocSysNameGet(snmp_buffer) == L7_SUCCESS) &&
        (SafeMakeOctetStringFromText(&lldpLocalSystemDataData.lldpLocSysName, snmp_buffer) == L7_TRUE))
      SET_VALID(I_lldpLocSysName, lldpLocalSystemDataData.valid);
    if (nominator != -1) break;

  case I_lldpLocSysDesc:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    if ((usmDbLldpLocSysDescGet(snmp_buffer) == L7_SUCCESS) &&
        (SafeMakeOctetStringFromText(&lldpLocalSystemDataData.lldpLocSysDesc, snmp_buffer) == L7_TRUE))
      SET_VALID(I_lldpLocSysDesc, lldpLocalSystemDataData.valid);
    if (nominator != -1) break;

  case I_lldpLocSysCapSupported:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    if (usmDbLldpLocSysCapSupportedGet(&system_cap) == L7_SUCCESS)
    {
      system_cap = snmpSysCapReverse(system_cap);
      memcpy(snmp_buffer,(char *)&system_cap,sizeof(system_cap));
      if (SafeMakeOctetString(&lldpLocalSystemDataData.lldpLocSysCapSupported, snmp_buffer,sizeof(system_cap)) == L7_TRUE)
        SET_VALID(I_lldpLocSysCapSupported, lldpLocalSystemDataData.valid);
    }
    if (nominator != -1) break;

  case I_lldpLocSysCapEnabled:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    if (usmDbLldpLocSysCapEnabledGet(&system_cap) == L7_SUCCESS)
    {
      system_cap = snmpSysCapReverse(system_cap);
      memcpy(snmp_buffer,(char *)&system_cap,sizeof(system_cap));
      if (SafeMakeOctetString(&lldpLocalSystemDataData.lldpLocSysCapEnabled, snmp_buffer,sizeof(system_cap)) == L7_TRUE)
        SET_VALID(I_lldpLocSysCapEnabled, lldpLocalSystemDataData.valid);
    }
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, lldpLocalSystemDataData.valid))
    return(NULL);

  return(&lldpLocalSystemDataData);
}

lldpLocPortEntry_t *
k_lldpLocPortEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_INT32 lldpLocPortNum)
{
  static lldpLocPortEntry_t lldpLocPortEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_short16 snmp_buffer_len;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    lldpLocPortEntryData.lldpLocPortId = MakeOctetString(NULL, 0);
    lldpLocPortEntryData.lldpLocPortDesc = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(lldpLocPortEntryData.valid);
  lldpLocPortEntryData.lldpLocPortNum = lldpLocPortNum;
  SET_VALID(I_lldpLocPortNum, lldpLocPortEntryData.valid);

  if ((searchType == EXACT ? 
       (usmDbPhysicalIntIfNumberCheck(USMDB_UNIT_CURRENT, 
                                      lldpLocPortEntryData.lldpLocPortNum) != L7_SUCCESS) :
       (usmDbPhysicalIntIfNumberCheck(USMDB_UNIT_CURRENT, 
                                      lldpLocPortEntryData.lldpLocPortNum) != L7_SUCCESS &&
        usmDbGetNextPhysicalIntIfNumber(
                                       lldpLocPortEntryData.lldpLocPortNum, 
                                       &lldpLocPortEntryData.lldpLocPortNum) != L7_SUCCESS)))
  {
    ZERO_VALID(lldpLocPortEntryData.valid);
    return(NULL);
  }


  switch (nominator)
  {
  case -1:
    /* fallthrough all cases except default */
  case I_lldpLocPortNum:
    if (nominator != -1) break;

  case I_lldpLocPortIdSubtype:
    if (usmDbLldpLocPortIdSubtypeGet(lldpLocPortEntryData.lldpLocPortNum,
                                     (lldpPortIdSubtype_t *)  &lldpLocPortEntryData.lldpLocPortIdSubtype)
        == L7_SUCCESS)
      SET_VALID(I_lldpLocPortIdSubtype, lldpLocPortEntryData.valid);
    if (nominator != -1) break;

  case I_lldpLocPortId:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    if ((usmDbLldpLocPortIdGet(lldpLocPortEntryData.lldpLocPortNum,
                               snmp_buffer, &snmp_buffer_len) == L7_SUCCESS) &&
        (SafeMakeOctetString(
                            &lldpLocPortEntryData.lldpLocPortId, snmp_buffer, snmp_buffer_len) == L7_TRUE))
      SET_VALID(I_lldpLocPortId, lldpLocPortEntryData.valid);
    if (nominator != -1) break;

  case I_lldpLocPortDesc:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    if ((usmDbLldpLocPortDescGet(lldpLocPortEntryData.lldpLocPortNum,
                                 snmp_buffer) == L7_SUCCESS) &&
        (SafeMakeOctetStringFromText(
                                    &lldpLocPortEntryData.lldpLocPortDesc, snmp_buffer) == L7_TRUE))
      SET_VALID(I_lldpLocPortDesc, lldpLocPortEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, lldpLocPortEntryData.valid))
    return(NULL);

  return(&lldpLocPortEntryData);
}

lldpLocManAddrEntry_t *
k_lldpLocManAddrEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator,
                          int searchType,
                          SR_INT32 lldpLocManAddrSubtype,
                          OctetString * lldpLocManAddr)
{
  static lldpLocManAddrEntry_t lldpLocManAddrEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_len = 0;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    lldpLocManAddrEntryData.lldpLocManAddrOID = MakeOIDFromDot("0.0");
    lldpLocManAddrEntryData.lldpConfigManAddrPortsTxEnable = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(lldpLocManAddrEntryData.valid);

  lldpLocManAddrEntryData.lldpLocManAddrSubtype = lldpLocManAddrSubtype;
  SET_VALID(I_lldpLocManAddrSubtype, lldpLocManAddrEntryData.valid);

  memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer, lldpLocManAddr->octet_ptr, lldpLocManAddr->length);
  snmp_buffer_len = lldpLocManAddr->length;

  if ((searchType == EXACT) ?
      (snmpLldpLocManAddrGet(lldpLocManAddrEntryData.lldpLocManAddrSubtype, 
                             snmp_buffer, 
                             &snmp_buffer_len) != L7_SUCCESS):
      (snmpLldpLocManAddrGetNext(&lldpLocManAddrEntryData.lldpLocManAddrSubtype,
                                 snmp_buffer,
                                 &snmp_buffer_len) != L7_SUCCESS))
  {
    ZERO_VALID(lldpLocManAddrEntryData.valid);
    return(NULL);
  }

  if (SafeMakeOctetString(&lldpLocManAddrEntryData.lldpLocManAddr, snmp_buffer, snmp_buffer_len) != L7_TRUE)
  {
    ZERO_VALID(lldpLocManAddrEntryData.valid);
    return(NULL);
  }
  SET_VALID(I_lldpLocManAddr, lldpLocManAddrEntryData.valid);

  switch (nominator)
  {
  case -1:
    /* fallthrough all cases except default */
  case I_lldpLocManAddrSubtype:
  case I_lldpLocManAddr:
    if (nominator != -1) break;

  case I_lldpLocManAddrLen:
    SET_VALID(I_lldpLocManAddrLen, lldpLocManAddrEntryData.valid);
    if (nominator != -1) break;

  case I_lldpLocManAddrIfSubtype:
    if (usmDbLldpLocManAddrIfSubtypeGet((lldpManAddrIfSubtype_t *)
                                        &lldpLocManAddrEntryData.lldpLocManAddrIfSubtype)
        == L7_SUCCESS)
      SET_VALID(I_lldpLocManAddrIfSubtype, lldpLocManAddrEntryData.valid);
    if (nominator != -1) break;

  case I_lldpLocManAddrIfId:
    if (usmDbLldpLocManAddrIfIdGet(
                                  &lldpLocManAddrEntryData.lldpLocManAddrIfId) == L7_SUCCESS)
      SET_VALID(I_lldpLocManAddrIfId, lldpLocManAddrEntryData.valid);
    if (nominator != -1) break;

  case I_lldpLocManAddrOID:
    if (usmDbLldpLocManAddrOIDGet(snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOIDFromDot(&lldpLocManAddrEntryData.lldpLocManAddrOID, snmp_buffer) == L7_TRUE))
      SET_VALID(I_lldpLocManAddrOID, lldpLocManAddrEntryData.valid);
    if (nominator != -1) break;

  case I_lldpConfigManAddrPortsTxEnable:
    memset(snmp_buffer, 0 ,SNMP_BUFFER_LEN);
    if ((snmpLldpConfigManAddrPortsTxEnableGet(snmp_buffer, 
                                               &snmp_buffer_len) == L7_SUCCESS) &&
        (SafeMakeOctetString(&lldpLocManAddrEntryData.lldpConfigManAddrPortsTxEnable,
                             snmp_buffer, snmp_buffer_len) == L7_TRUE))
    {
      SET_VALID(I_lldpConfigManAddrPortsTxEnable, lldpLocManAddrEntryData.valid);
    }
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, lldpLocManAddrEntryData.valid))
    return(NULL);

  return(&lldpLocManAddrEntryData);
}

#ifdef SETS
int
k_lldpLocManAddrEntry_test(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_lldpLocManAddrEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                            doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_lldpLocManAddrEntry_set_defaults(doList_t *dp)
{
  lldpLocManAddrEntry_t *data = (lldpLocManAddrEntry_t *) (dp->data);

  if ((data->lldpLocManAddrOID = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_lldpLocManAddrEntry_set(lldpLocManAddrEntry_t *data,
                          ContextInfo *contextInfo, int function)
{
  if (VALID(I_lldpConfigManAddrPortsTxEnable, data->valid))
  {
    if ( snmpLldpConfigManAddrPortsTxEnableSet(data->lldpConfigManAddrPortsTxEnable->octet_ptr, 
                                               data->lldpConfigManAddrPortsTxEnable->length) != L7_SUCCESS)
      return COMMIT_FAILED_ERROR;
  }
  return NO_ERROR;
}

  #ifdef SR_lldpLocManAddrEntry_UNDO
/* add #define SR_lldpLocManAddrEntry_UNDO in sitedefs.h to
 * include the undo routine for the lldpLocManAddrEntry family.
 */
int
lldpLocManAddrEntry_undo(doList_t *doHead, doList_t *doCur,
                         ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_lldpLocManAddrEntry_UNDO */

#endif /* SETS */

lldpRemEntry_t *
k_lldpRemEntry_get(int serialNum, ContextInfo *contextInfo,
                   int nominator,
                   int searchType,
                   SR_UINT32 lldpRemTimeMark,
                   SR_INT32 lldpRemLocalPortNum,
                   SR_INT32 lldpRemIndex)
{
  static lldpRemEntry_t lldpRemEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_ushort16 snmp_buffer_len;
  L7_ushort16 system_cap;
  L7_uint32 intIfNum;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    lldpRemEntryData.lldpRemChassisId = MakeOctetString(NULL, 0);
    lldpRemEntryData.lldpRemPortId = MakeOctetString(NULL, 0);
    lldpRemEntryData.lldpRemPortDesc = MakeOctetString(NULL, 0);
    lldpRemEntryData.lldpRemSysName = MakeOctetString(NULL, 0);
    lldpRemEntryData.lldpRemSysDesc = MakeOctetString(NULL, 0);
    lldpRemEntryData.lldpRemSysCapSupported = MakeOctetString(NULL, 0);
    lldpRemEntryData.lldpRemSysCapEnabled = MakeOctetString(NULL, 0);
  }

  memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
  ZERO_VALID(lldpRemEntryData.valid);

  lldpRemEntryData.lldpRemTimeMark = lldpRemTimeMark;
  SET_VALID(I_lldpRemTimeMark, lldpRemEntryData.valid);

  lldpRemEntryData.lldpRemLocalPortNum = lldpRemLocalPortNum;

  lldpRemEntryData.lldpRemIndex = lldpRemIndex;
  SET_VALID(I_lldpRemIndex, lldpRemEntryData.valid);

  if (((searchType == EXACT) ?
       ((usmDbIntIfNumFromExtIfNum(lldpRemEntryData.lldpRemLocalPortNum, &intIfNum) != L7_SUCCESS) ||
         (snmpLldpRemEntryGet(lldpRemEntryData.lldpRemTimeMark, intIfNum, lldpRemEntryData.lldpRemIndex) != L7_SUCCESS)) :

       (((usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT, lldpRemEntryData.lldpRemLocalPortNum) != L7_SUCCESS) &&
         (snmpLldpNextExtIfNumber(USMDB_UNIT_CURRENT, &lldpRemEntryData.lldpRemLocalPortNum) != L7_SUCCESS)) ||
        (usmDbIntIfNumFromExtIfNum(lldpRemEntryData.lldpRemLocalPortNum, &intIfNum) != L7_SUCCESS) ||
        ((snmpLldpRemEntryGet(lldpRemEntryData.lldpRemTimeMark, intIfNum, lldpRemEntryData.lldpRemIndex) != L7_SUCCESS) &&
         (snmpLldpRemEntryGetNext(&lldpRemEntryData.lldpRemTimeMark, &intIfNum,
                                   &lldpRemEntryData.lldpRemIndex) != L7_SUCCESS)))) ||
      (usmDbExtIfNumFromIntIfNum(intIfNum, &lldpRemEntryData.lldpRemLocalPortNum) != L7_SUCCESS))
  {
    ZERO_VALID(lldpRemEntryData.valid);
    return(NULL);
  }

  SET_VALID(I_lldpRemLocalPortNum, lldpRemEntryData.valid);

  switch (nominator)
  {
  case -1:
    /* fallthrough all cases except default */
  case I_lldpRemTimeMark:
  case I_lldpRemLocalPortNum:
  case I_lldpRemIndex:
    if (nominator != -1) break;

  case I_lldpRemChassisIdSubtype:
    if (usmDbLldpRemChassisIdSubtypeGet(intIfNum,
                                        lldpRemEntryData.lldpRemIndex, lldpRemEntryData.lldpRemTimeMark,
                                        (lldpChassisIdSubtype_t *)&lldpRemEntryData.lldpRemChassisIdSubtype) == L7_SUCCESS)
      SET_VALID(I_lldpRemChassisIdSubtype, lldpRemEntryData.valid);
    if (nominator != -1) break;

  case I_lldpRemChassisId:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    if ((usmDbLldpRemChassisIdGet(intIfNum, lldpRemEntryData.lldpRemIndex, 
                                  lldpRemEntryData.lldpRemTimeMark, snmp_buffer, 
                                  &snmp_buffer_len) == L7_SUCCESS) &&
        (SafeMakeOctetString(&lldpRemEntryData.lldpRemChassisId, 
                             snmp_buffer, snmp_buffer_len) == L7_TRUE))
      SET_VALID(I_lldpRemChassisId, lldpRemEntryData.valid);
    if (nominator != -1) break;

  case I_lldpRemPortIdSubtype:
    if (usmDbLldpRemPortIdSubtypeGet(intIfNum,
                                     lldpRemEntryData.lldpRemIndex, lldpRemEntryData.lldpRemTimeMark,
                                     (lldpPortIdSubtype_t *)  &lldpRemEntryData.lldpRemPortIdSubtype)
        == L7_SUCCESS)
      SET_VALID(I_lldpRemPortIdSubtype, lldpRemEntryData.valid);
    if (nominator != -1) break;

  case I_lldpRemPortId:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    if ((usmDbLldpRemPortIdGet(intIfNum,
                               lldpRemEntryData.lldpRemIndex, lldpRemEntryData.lldpRemTimeMark,
                               snmp_buffer, &snmp_buffer_len) == L7_SUCCESS) &&
        (SafeMakeOctetString(&lldpRemEntryData.lldpRemPortId, 
                             snmp_buffer, snmp_buffer_len) == L7_TRUE))
      SET_VALID(I_lldpRemPortId, lldpRemEntryData.valid);
    if (nominator != -1) break;

  case I_lldpRemPortDesc:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    if ((usmDbLldpRemPortDescGet(intIfNum,
                                 lldpRemEntryData.lldpRemIndex, lldpRemEntryData.lldpRemTimeMark,
                                 snmp_buffer) == L7_SUCCESS) &&
        (SafeMakeOctetStringFromText(&lldpRemEntryData.lldpRemPortDesc, 
                                     snmp_buffer) == L7_TRUE))
      SET_VALID(I_lldpRemPortDesc, lldpRemEntryData.valid);
    if (nominator != -1) break;

  case I_lldpRemSysName:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    if ((usmDbLldpRemSysNameGet(intIfNum,
                                lldpRemEntryData.lldpRemIndex, lldpRemEntryData.lldpRemTimeMark,
                                snmp_buffer) == L7_SUCCESS) &&
        (SafeMakeOctetStringFromText(&lldpRemEntryData.lldpRemSysName, 
                                     snmp_buffer) == L7_TRUE))
      SET_VALID(I_lldpRemSysName, lldpRemEntryData.valid);
    if (nominator != -1) break;

  case I_lldpRemSysDesc:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    if ((usmDbLldpRemSysDescGet(intIfNum,
                                lldpRemEntryData.lldpRemIndex, lldpRemEntryData.lldpRemTimeMark,
                                snmp_buffer) == L7_SUCCESS) &&
        (SafeMakeOctetStringFromText(
                                    &lldpRemEntryData.lldpRemSysDesc, 
                                    snmp_buffer) == L7_TRUE))
      SET_VALID(I_lldpRemSysDesc, lldpRemEntryData.valid);
    if (nominator != -1) break;

  case I_lldpRemSysCapSupported:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    if (usmDbLldpRemSysCapSupportedGet(intIfNum,
                                       lldpRemEntryData.lldpRemIndex, lldpRemEntryData.lldpRemTimeMark,
                                       &system_cap) == L7_SUCCESS)
    {
      system_cap = snmpSysCapReverse(system_cap);
      memcpy(snmp_buffer,(char *)&system_cap,sizeof(system_cap));
      if (SafeMakeOctetString(
                             &lldpRemEntryData.lldpRemSysCapSupported, 
                             snmp_buffer,sizeof(system_cap)) == L7_TRUE)
        SET_VALID(I_lldpRemSysCapSupported, lldpRemEntryData.valid);
    }
    if (nominator != -1) break;

  case I_lldpRemSysCapEnabled:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    if (usmDbLldpRemSysCapEnabledGet(intIfNum,
                                     lldpRemEntryData.lldpRemIndex, lldpRemEntryData.lldpRemTimeMark,
                                     &system_cap) == L7_SUCCESS)
    {
      system_cap = snmpSysCapReverse(system_cap);
      memcpy(snmp_buffer,(char *)&system_cap,sizeof(system_cap));
      if (SafeMakeOctetString(
                             &lldpRemEntryData.lldpRemSysCapEnabled, snmp_buffer,sizeof(system_cap)) == L7_TRUE)
        SET_VALID(I_lldpRemSysCapEnabled, lldpRemEntryData.valid);
    }
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, lldpRemEntryData.valid))
    return(NULL);

  return(&lldpRemEntryData);
}

lldpRemManAddrEntry_t *
k_lldpRemManAddrEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator,
                          int searchType,
                          SR_UINT32 lldpRemTimeMark,
                          SR_INT32 lldpRemLocalPortNum,
                          SR_INT32 lldpRemIndex,
                          SR_INT32 lldpRemManAddrSubtype,
                          OctetString * lldpRemManAddr)
{
  static lldpRemManAddrEntry_t lldpRemManAddrEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_len;
  L7_uint32 intIfNum;
  lldpIANAAddrFamilyNumber_t family;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    lldpRemManAddrEntryData.lldpRemManAddr = MakeOctetString(NULL, 0);
    lldpRemManAddrEntryData.lldpRemManAddrOID = MakeOIDFromDot("0.0");
  }

  ZERO_VALID(lldpRemManAddrEntryData.valid);

  lldpRemManAddrEntryData.lldpRemTimeMark = lldpRemTimeMark;
  SET_VALID(I_lldpRemManAddrEntryIndex_lldpRemTimeMark, 
            lldpRemManAddrEntryData.valid);

  lldpRemManAddrEntryData.lldpRemLocalPortNum = lldpRemLocalPortNum;
  SET_VALID(I_lldpRemManAddrEntryIndex_lldpRemLocalPortNum, 
            lldpRemManAddrEntryData.valid);

  lldpRemManAddrEntryData.lldpRemIndex = lldpRemIndex;
  SET_VALID(I_lldpRemManAddrEntryIndex_lldpRemIndex, 
            lldpRemManAddrEntryData.valid);

  lldpRemManAddrEntryData.lldpRemManAddrSubtype = 
  SET_VALID(I_lldpRemManAddrSubtype, lldpRemManAddrEntryData.valid);

  memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer, lldpRemManAddr->octet_ptr, lldpRemManAddr->length);
  snmp_buffer_len = lldpRemManAddr->length;

  if (snmpLldpAddrFamilyGet(lldpRemManAddrSubtype, &family) != L7_SUCCESS)
  {
    ZERO_VALID(lldpRemManAddrEntryData.valid);
    return(NULL);
  }

  if (((searchType == EXACT) ?
       ((usmDbIntIfNumFromExtIfNum(lldpRemManAddrEntryData.lldpRemLocalPortNum, &intIfNum) != L7_SUCCESS) ||
        (snmpLldpRemManAddrEntryGet(lldpRemManAddrEntryData.lldpRemTimeMark,
                                  intIfNum, lldpRemManAddrEntryData.lldpRemIndex, family,
                                  snmp_buffer, snmp_buffer_len) != L7_SUCCESS)) :

       (((usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT, lldpRemManAddrEntryData.lldpRemLocalPortNum) != L7_SUCCESS) &&
         (snmpLldpNextExtIfNumber(USMDB_UNIT_CURRENT, &lldpRemManAddrEntryData.lldpRemLocalPortNum) != L7_SUCCESS)) ||
        (usmDbIntIfNumFromExtIfNum(lldpRemManAddrEntryData.lldpRemLocalPortNum, &intIfNum) != L7_SUCCESS) ||
        ((snmpLldpRemManAddrEntryGet(lldpRemManAddrEntryData.lldpRemTimeMark, 
                                   intIfNum, lldpRemManAddrEntryData.lldpRemIndex, family,
                                   snmp_buffer, snmp_buffer_len) != L7_SUCCESS) && 
         (snmpLldpRemManAddrEntryGetNext(&lldpRemManAddrEntryData.lldpRemTimeMark, 
                                      &intIfNum, &lldpRemManAddrEntryData.lldpRemIndex, &family,
                                      snmp_buffer, &snmp_buffer_len) != L7_SUCCESS)))) ||
      (usmDbExtIfNumFromIntIfNum(intIfNum, &lldpRemManAddrEntryData.lldpRemLocalPortNum) != L7_SUCCESS))
  {
    ZERO_VALID(lldpRemManAddrEntryData.valid);
    return(NULL);
  }

  SET_VALID(I_lldpRemManAddrEntryIndex_lldpRemLocalPortNum, 
            lldpRemManAddrEntryData.valid);

  if (SafeMakeOctetString(&lldpRemManAddrEntryData.lldpRemManAddr, snmp_buffer, 
                          snmp_buffer_len) != L7_TRUE)
  {
    ZERO_VALID(lldpRemManAddrEntryData.valid);
    return(NULL);
  }
  SET_VALID(I_lldpRemManAddr, lldpRemManAddrEntryData.valid);

  if (snmpLldpAddrSubTypeGet(family, &lldpRemManAddrEntryData.lldpRemManAddrSubtype) != L7_SUCCESS)
  {
    ZERO_VALID(lldpRemManAddrEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
    /* fallthrough all cases except default */
  case I_lldpRemManAddrEntryIndex_lldpRemLocalPortNum:
  case I_lldpRemManAddrEntryIndex_lldpRemTimeMark:
  case I_lldpRemManAddrEntryIndex_lldpRemIndex:
  case I_lldpRemManAddrSubtype:
  case I_lldpRemManAddr:
    if (nominator != -1) break;

  case I_lldpRemManAddrIfSubtype:
    if (usmDbLldpRemManAddrIfSubtypeGet(
                                       lldpRemManAddrEntryData.lldpRemLocalPortNum,
                                       lldpRemManAddrEntryData.lldpRemIndex, 
                                       lldpRemManAddrEntryData.lldpRemTimeMark, 
                                       family, 
                                       lldpRemManAddrEntryData.lldpRemManAddr->octet_ptr,
                                       lldpRemManAddrEntryData.lldpRemManAddr->length, 
                                       (lldpManAddrIfSubtype_t *)
                                       &lldpRemManAddrEntryData.lldpRemManAddrIfSubtype) 
        == L7_SUCCESS)
      SET_VALID(I_lldpRemManAddrIfSubtype, lldpRemManAddrEntryData.valid);
    if (nominator != -1) break;

  case I_lldpRemManAddrIfId:
    if (usmDbLldpRemManAddrIfIdGet(
                                  lldpRemManAddrEntryData.lldpRemLocalPortNum,
                                  lldpRemManAddrEntryData.lldpRemIndex, 
                                  lldpRemManAddrEntryData.lldpRemTimeMark, 
                                  family,
                                  lldpRemManAddrEntryData.lldpRemManAddr->octet_ptr,
                                  lldpRemManAddrEntryData.lldpRemManAddr->length, 
                                  &lldpRemManAddrEntryData.lldpRemManAddrIfId) == L7_SUCCESS)
      SET_VALID(I_lldpRemManAddrIfId, lldpRemManAddrEntryData.valid);
    if (nominator != -1) break;

  case I_lldpRemManAddrOID:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    if (usmDbLldpRemManAddrOIDGet(
                                 lldpRemManAddrEntryData.lldpRemLocalPortNum,
                                 lldpRemManAddrEntryData.lldpRemIndex, 
                                 lldpRemManAddrEntryData.lldpRemTimeMark, 
                                 family,
                                 lldpRemManAddrEntryData.lldpRemManAddr->octet_ptr,
                                 lldpRemManAddrEntryData.lldpRemManAddr->length, 
                                 snmp_buffer) == L7_SUCCESS)
    {
      if (SafeMakeOIDFromDot(&lldpRemManAddrEntryData.lldpRemManAddrOID,
                             snmp_buffer) == L7_TRUE)
      {
        SET_VALID(I_lldpRemManAddrOID, lldpRemManAddrEntryData.valid);
      }
      else
      {
        SafeMakeOIDFromDot(&lldpRemManAddrEntryData.lldpRemManAddrOID, "0.0");
      }
    }
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, lldpRemManAddrEntryData.valid))
    return(NULL);

  return(&lldpRemManAddrEntryData);
}

lldpRemUnknownTLVEntry_t *
k_lldpRemUnknownTLVEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_UINT32 lldpRemTimeMark,
                             SR_INT32 lldpRemLocalPortNum,
                             SR_INT32 lldpRemIndex,
                             SR_INT32 lldpRemUnknownTLVType)
{
  static lldpRemUnknownTLVEntry_t lldpRemUnknownTLVEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_ushort16 snmp_buffer_len;
  L7_uint32 intIfNum;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    lldpRemUnknownTLVEntryData.lldpRemUnknownTLVInfo = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(lldpRemUnknownTLVEntryData.valid);

  lldpRemUnknownTLVEntryData.lldpRemTimeMark = lldpRemTimeMark;
  SET_VALID(I_lldpRemUnknownTLVEntryIndex_lldpRemTimeMark, 
            lldpRemUnknownTLVEntryData.valid);

  lldpRemUnknownTLVEntryData.lldpRemLocalPortNum = lldpRemLocalPortNum;

  lldpRemUnknownTLVEntryData.lldpRemIndex = lldpRemIndex;
  SET_VALID(I_lldpRemUnknownTLVEntryIndex_lldpRemIndex, 
            lldpRemUnknownTLVEntryData.valid);

  lldpRemUnknownTLVEntryData.lldpRemUnknownTLVType = lldpRemUnknownTLVType;
  SET_VALID(I_lldpRemUnknownTLVType, lldpRemUnknownTLVEntryData.valid);

  if (usmDbIntIfNumFromExtIfNum(lldpRemUnknownTLVEntryData.lldpRemLocalPortNum,
                                &intIfNum) != L7_SUCCESS)
  {
    ZERO_VALID(lldpRemUnknownTLVEntryData.valid);
    return(NULL);
  }

  if (searchType == EXACT ? 
      (snmpLldpRemUnknownTLVEntryGet(lldpRemUnknownTLVEntryData.lldpRemTimeMark,
                                     intIfNum, lldpRemUnknownTLVEntryData.lldpRemIndex, 
                                     lldpRemUnknownTLVEntryData.lldpRemUnknownTLVType) != L7_SUCCESS) :
      ((snmpLldpRemUnknownTLVEntryGet(lldpRemUnknownTLVEntryData.lldpRemTimeMark,
                                      intIfNum, lldpRemUnknownTLVEntryData.lldpRemIndex, 
                                      lldpRemUnknownTLVEntryData.lldpRemUnknownTLVType) != L7_SUCCESS) &&
       (snmpLldpRemUnknownTLVEntryGetNext(
                                         &lldpRemUnknownTLVEntryData.lldpRemTimeMark, 
                                         &intIfNum, &lldpRemUnknownTLVEntryData.lldpRemIndex, 
                                         &lldpRemUnknownTLVEntryData.lldpRemUnknownTLVType) != L7_SUCCESS)))
  {
    ZERO_VALID(lldpRemUnknownTLVEntryData.valid);
    return(NULL);
  }

  if (usmDbExtIfNumFromIntIfNum(intIfNum, 
                                &lldpRemUnknownTLVEntryData.lldpRemLocalPortNum) != L7_SUCCESS)
  {
    ZERO_VALID(lldpRemUnknownTLVEntryData.valid);
    return(NULL);
  }
  SET_VALID(I_lldpRemUnknownTLVEntryIndex_lldpRemLocalPortNum, 
            lldpRemUnknownTLVEntryData.valid);

  switch (nominator)
  {
  case -1:
    /* fallthrough all cases except default */
  case I_lldpRemUnknownTLVEntryIndex_lldpRemTimeMark:
  case I_lldpRemUnknownTLVEntryIndex_lldpRemLocalPortNum:
  case I_lldpRemUnknownTLVEntryIndex_lldpRemIndex:
  case I_lldpRemUnknownTLVType:
    if (nominator != -1) break;

  case I_lldpRemUnknownTLVInfo:
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    if ((usmDbLldpRemUnknownTLVInfoGet(intIfNum,
                                       lldpRemUnknownTLVEntryData.lldpRemIndex, 
                                       lldpRemUnknownTLVEntryData.lldpRemTimeMark, 
                                       lldpRemUnknownTLVEntryData.lldpRemUnknownTLVType, 
                                       snmp_buffer, &snmp_buffer_len) == L7_SUCCESS) &&
        (SafeMakeOctetString(&lldpRemUnknownTLVEntryData.lldpRemUnknownTLVInfo, 
                             snmp_buffer, snmp_buffer_len) == L7_TRUE))
      SET_VALID(I_lldpRemUnknownTLVInfo, lldpRemUnknownTLVEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, lldpRemUnknownTLVEntryData.valid))
    return(NULL);

  return(&lldpRemUnknownTLVEntryData);
}

lldpRemOrgDefInfoEntry_t *
k_lldpRemOrgDefInfoEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_UINT32 lldpRemTimeMark,
                             SR_INT32 lldpRemLocalPortNum,
                             SR_INT32 lldpRemIndex,
                             OctetString * lldpRemOrgDefInfoOUI,
                             SR_INT32 lldpRemOrgDefInfoSubtype,
                             SR_INT32 lldpRemOrgDefInfoIndex)
{
  static lldpRemOrgDefInfoEntry_t lldpRemOrgDefInfoEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_len;
  L7_uchar8 snmp_buffer1[SNMP_BUFFER_LEN];
  L7_uint32 subtype = 0;
  
  L7_uint32 intIfNum;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    lldpRemOrgDefInfoEntryData.lldpRemOrgDefInfo = MakeOctetString(NULL, 0);
    lldpRemOrgDefInfoEntryData.lldpRemOrgDefInfoOUI = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(lldpRemOrgDefInfoEntryData.valid);

  lldpRemOrgDefInfoEntryData.lldpRemTimeMark = lldpRemTimeMark;
  SET_VALID(I_lldpRemOrgDefInfoEntryIndex_lldpRemTimeMark, 
            lldpRemOrgDefInfoEntryData.valid);

  lldpRemOrgDefInfoEntryData.lldpRemLocalPortNum = lldpRemLocalPortNum;

  lldpRemOrgDefInfoEntryData.lldpRemIndex = lldpRemIndex;
  SET_VALID(I_lldpRemOrgDefInfoEntryIndex_lldpRemIndex, 
            lldpRemOrgDefInfoEntryData.valid);

  lldpRemOrgDefInfoEntryData.lldpRemOrgDefInfoSubtype = lldpRemOrgDefInfoSubtype;
  SET_VALID(I_lldpRemOrgDefInfoSubtype, lldpRemOrgDefInfoEntryData.valid);
  subtype = lldpRemOrgDefInfoSubtype;

  lldpRemOrgDefInfoEntryData.lldpRemOrgDefInfoIndex = lldpRemOrgDefInfoIndex;
  SET_VALID(I_lldpRemOrgDefInfoIndex, lldpRemOrgDefInfoEntryData.valid);

  FreeOctetString(lldpRemOrgDefInfoEntryData.lldpRemOrgDefInfoOUI);
  lldpRemOrgDefInfoEntryData.lldpRemOrgDefInfoOUI = CloneOctetString(lldpRemOrgDefInfoOUI);
  SET_VALID(I_lldpRemOrgDefInfoOUI, lldpRemOrgDefInfoEntryData.valid);

  if (usmDbIntIfNumFromExtIfNum(lldpRemOrgDefInfoEntryData.lldpRemLocalPortNum,
                                &intIfNum) != L7_SUCCESS)
  {
    ZERO_VALID(lldpRemOrgDefInfoEntryData.valid);
    return(NULL);
  }

  memset(snmp_buffer, 0, SNMP_BUFFER_LEN);

  if (lldpRemOrgDefInfoOUI->length == 0 || lldpRemOrgDefInfoOUI->octet_ptr == L7_NULLPTR)
  {
    memset(snmp_buffer1, 0, SNMP_BUFFER_LEN);
  }
  else if (lldpRemOrgDefInfoOUI->length > 0 && lldpRemOrgDefInfoOUI->length < SNMP_BUFFER_LEN && lldpRemOrgDefInfoOUI->octet_ptr != L7_NULLPTR)
  {
    memcpy(snmp_buffer1, lldpRemOrgDefInfoOUI->octet_ptr, lldpRemOrgDefInfoOUI->length);
  }
  else
  {
    return (NULL);
  }

  if (searchType == EXACT ? 
      (snmpLldpRemOrgDefInfoEntryGet(lldpRemOrgDefInfoEntryData.lldpRemTimeMark, 
                                     intIfNum, lldpRemOrgDefInfoEntryData.lldpRemIndex, 
                                     lldpRemOrgDefInfoEntryData.lldpRemOrgDefInfoIndex,snmp_buffer1,&subtype,
                                     snmp_buffer,&snmp_buffer_len) != L7_SUCCESS) :
      ((snmpLldpRemOrgDefInfoEntryGet(lldpRemOrgDefInfoEntryData.lldpRemTimeMark,
                                      intIfNum, lldpRemOrgDefInfoEntryData.lldpRemIndex, 
                                      lldpRemOrgDefInfoEntryData.lldpRemOrgDefInfoIndex,snmp_buffer1,&subtype,
                                      snmp_buffer,&snmp_buffer_len) != L7_SUCCESS) &&
       (snmpLldpRemOrgDefInfoEntryGetNext(&lldpRemOrgDefInfoEntryData.lldpRemTimeMark, 
                                          &intIfNum, &lldpRemOrgDefInfoEntryData.lldpRemIndex, 
                                          &lldpRemOrgDefInfoEntryData.lldpRemOrgDefInfoIndex,snmp_buffer1,&subtype,
                                          snmp_buffer,&snmp_buffer_len) != L7_SUCCESS)))
  {
    ZERO_VALID(lldpRemOrgDefInfoEntryData.valid);
    return(NULL);
  }
 
  if (usmDbExtIfNumFromIntIfNum(intIfNum, 
                                &lldpRemOrgDefInfoEntryData.lldpRemLocalPortNum) != L7_SUCCESS)
  {
    ZERO_VALID(lldpRemOrgDefInfoEntryData.valid);
    return(NULL);
  }

  SET_VALID(I_lldpRemOrgDefInfoEntryIndex_lldpRemLocalPortNum, 
            lldpRemOrgDefInfoEntryData.valid);

  if (SafeMakeOctetString(&lldpRemOrgDefInfoEntryData.lldpRemOrgDefInfoOUI, snmp_buffer1, 3) == L7_TRUE)
    SET_VALID(I_lldpRemOrgDefInfoOUI, lldpRemOrgDefInfoEntryData.valid);

  lldpRemOrgDefInfoEntryData.lldpRemOrgDefInfoSubtype = subtype;
  SET_VALID(I_lldpRemOrgDefInfoSubtype, lldpRemOrgDefInfoEntryData.valid);

  switch (nominator)
  {
  case -1:
    /* fallthrough all cases except default */
  case I_lldpRemOrgDefInfoEntryIndex_lldpRemTimeMark:
  case I_lldpRemOrgDefInfoEntryIndex_lldpRemLocalPortNum:
  case I_lldpRemOrgDefInfoEntryIndex_lldpRemIndex:
  case I_lldpRemOrgDefInfoIndex:
  case I_lldpRemOrgDefInfoOUI:
  case I_lldpRemOrgDefInfoSubtype:
    if (nominator != -1) break;

  case I_lldpRemOrgDefInfo:
    if (SafeMakeOctetString(&lldpRemOrgDefInfoEntryData.lldpRemOrgDefInfo, snmp_buffer, snmp_buffer_len) == L7_TRUE)
      SET_VALID(I_lldpRemOrgDefInfo, lldpRemOrgDefInfoEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, lldpRemOrgDefInfoEntryData.valid))
    return(NULL);

  return(&lldpRemOrgDefInfoEntryData);
}

lldpXdot3LocPortEntry_t *
k_lldpXdot3LocPortEntry_get(int serialNum, ContextInfo *contextInfo,
                            int nominator,
                            int searchType,
                            SR_INT32 lldpLocPortNum)
{
  static lldpXdot3LocPortEntry_t lldpXdot3LocPortEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 intIfNum;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    lldpXdot3LocPortEntryData.lldpXdot3LocPortAutoNegAdvertisedCap = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(lldpXdot3LocPortEntryData.valid);
  lldpXdot3LocPortEntryData.lldpLocPortNum = lldpLocPortNum;
  SET_VALID(I_lldpXdot3LocPortEntryIndex_lldpLocPortNum, lldpXdot3LocPortEntryData.valid);

  if ((searchType == EXACT ? 
     ( usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT, lldpXdot3LocPortEntryData.lldpLocPortNum) != L7_SUCCESS ) : 
     ( usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT, lldpXdot3LocPortEntryData.lldpLocPortNum) != L7_SUCCESS && 
     usmDbGetNextVisibleExtIfNumber(lldpXdot3LocPortEntryData.lldpLocPortNum, &lldpXdot3LocPortEntryData.lldpLocPortNum) != L7_SUCCESS ) ) ||
    usmDbIntIfNumFromExtIfNum(lldpXdot3LocPortEntryData.lldpLocPortNum, &intIfNum) != L7_SUCCESS )
  {
  ZERO_VALID(lldpXdot3LocPortEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
    /* fallthrough all cases except default */
  case I_lldpXdot3LocPortEntryIndex_lldpLocPortNum:
    if (nominator != -1) break;

  case I_lldpXdot3LocPortAutoNegSupported:
    if (snmpLldpXdot3LocPortAutoNegSupportedGet(intIfNum, &lldpXdot3LocPortEntryData.lldpXdot3LocPortAutoNegSupported) == L7_SUCCESS)
      SET_VALID(I_lldpXdot3LocPortAutoNegSupported, lldpXdot3LocPortEntryData.valid);
    if (nominator != -1) break;

  case I_lldpXdot3LocPortAutoNegEnabled:
    if (snmpLldpXdot3LocPortAutoNegEnabledGet(intIfNum, &lldpXdot3LocPortEntryData.lldpXdot3LocPortAutoNegEnabled) == L7_SUCCESS)
      SET_VALID(I_lldpXdot3LocPortAutoNegEnabled, lldpXdot3LocPortEntryData.valid);
    if (nominator != -1) break;

  case I_lldpXdot3LocPortAutoNegAdvertisedCap:
    memset(snmp_buffer, 0, sizeof(snmp_buffer));
    if (usmDbLldpXdot3LocPortAutoNegAdvertizedCapGet(intIfNum, snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetString(&lldpXdot3LocPortEntryData.lldpXdot3LocPortAutoNegAdvertisedCap, snmp_buffer, 2) == L7_TRUE)
      SET_VALID(I_lldpXdot3LocPortAutoNegAdvertisedCap, lldpXdot3LocPortEntryData.valid);
    if (nominator != -1) break;

  case I_lldpXdot3LocPortOperMauType:
    if (usmDbLldpXdot3LocPortAutoNegOperMauTypeGet(intIfNum, &lldpXdot3LocPortEntryData.lldpXdot3LocPortOperMauType) == L7_SUCCESS)
      SET_VALID(I_lldpXdot3LocPortOperMauType, lldpXdot3LocPortEntryData.valid);
    if (nominator != -1) break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, lldpXdot3LocPortEntryData.valid))
    return(NULL);

  return(&lldpXdot3LocPortEntryData);
}

lldpXdot3LocPowerEntry_t *
k_lldpXdot3LocPowerEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_INT32 lldpLocPortNum)
{
#ifdef NOT_YET
   static lldpXdot3LocPowerEntry_t lldpXdot3LocPowerEntryData;

   /*
    * put your code to retrieve the information here
    */

   lldpXdot3LocPowerEntryData.lldpXdot3LocPowerPortClass = ;
   lldpXdot3LocPowerEntryData.lldpXdot3LocPowerMDISupported = ;
   lldpXdot3LocPowerEntryData.lldpXdot3LocPowerMDIEnabled = ;
   lldpXdot3LocPowerEntryData.lldpXdot3LocPowerPairControlable = ;
   lldpXdot3LocPowerEntryData.lldpXdot3LocPowerPairs = ;
   lldpXdot3LocPowerEntryData.lldpXdot3LocPowerClass = ;
   lldpXdot3LocPowerEntryData.lldpLocPortNum = ;
   SET_ALL_VALID(lldpXdot3LocPowerEntryData.valid);
   return(&lldpXdot3LocPowerEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

lldpXdot3LocLinkAggEntry_t *
k_lldpXdot3LocLinkAggEntry_get(int serialNum, ContextInfo *contextInfo,
                               int nominator,
                               int searchType,
                               SR_INT32 lldpLocPortNum)
{
#ifdef NOT_YET
   static lldpXdot3LocLinkAggEntry_t lldpXdot3LocLinkAggEntryData;

   /*
    * put your code to retrieve the information here
    */

   lldpXdot3LocLinkAggEntryData.lldpXdot3LocLinkAggStatus = ;
   lldpXdot3LocLinkAggEntryData.lldpXdot3LocLinkAggPortId = ;
   lldpXdot3LocLinkAggEntryData.lldpLocPortNum = ;
   SET_ALL_VALID(lldpXdot3LocLinkAggEntryData.valid);
   return(&lldpXdot3LocLinkAggEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

lldpXdot3LocMaxFrameSizeEntry_t *
k_lldpXdot3LocMaxFrameSizeEntry_get(int serialNum, ContextInfo *contextInfo,
                                    int nominator,
                                    int searchType,
                                    SR_INT32 lldpLocPortNum)
{
#ifdef NOT_YET
   static lldpXdot3LocMaxFrameSizeEntry_t lldpXdot3LocMaxFrameSizeEntryData;

   /*
    * put your code to retrieve the information here
    */

   lldpXdot3LocMaxFrameSizeEntryData.lldpXdot3LocMaxFrameSize = ;
   lldpXdot3LocMaxFrameSizeEntryData.lldpLocPortNum = ;
   SET_ALL_VALID(lldpXdot3LocMaxFrameSizeEntryData.valid);
   return(&lldpXdot3LocMaxFrameSizeEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

lldpXdot3RemPortEntry_t *
k_lldpXdot3RemPortEntry_get(int serialNum, ContextInfo *contextInfo,
                            int nominator,
                            int searchType,
                            SR_UINT32 lldpRemTimeMark,
                            SR_INT32 lldpRemLocalPortNum,
                            SR_INT32 lldpRemIndex)
{
  static lldpXdot3RemPortEntry_t lldpXdot3RemPortEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 intIfNum;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    lldpXdot3RemPortEntryData.lldpXdot3RemPortAutoNegAdvertisedCap = MakeOctetString(NULL, 0);
  }

  lldpXdot3RemPortEntryData.lldpRemTimeMark = lldpRemTimeMark;
  SET_VALID(I_lldpXdot3RemPortEntryIndex_lldpRemTimeMark, lldpXdot3RemPortEntryData.valid);

  lldpXdot3RemPortEntryData.lldpRemLocalPortNum = lldpRemLocalPortNum;

  lldpXdot3RemPortEntryData.lldpRemIndex = lldpRemIndex;
  SET_VALID(I_lldpXdot3RemPortEntryIndex_lldpRemIndex, lldpXdot3RemPortEntryData.valid);

  if (usmDbIntIfNumFromExtIfNum(lldpXdot3RemPortEntryData.lldpRemLocalPortNum, 
                                &intIfNum) != L7_SUCCESS)
    return(NULL);

  if ((searchType == EXACT) ?
      (snmpLldpRemEntryGet(lldpXdot3RemPortEntryData.lldpRemTimeMark, intIfNum,
                           lldpXdot3RemPortEntryData.lldpRemIndex) != L7_SUCCESS) :
      ((snmpLldpRemEntryGet(lldpXdot3RemPortEntryData.lldpRemTimeMark, intIfNum,
                            lldpXdot3RemPortEntryData.lldpRemIndex) != L7_SUCCESS) &&
       (snmpLldpRemEntryGetNext(&lldpXdot3RemPortEntryData.lldpRemTimeMark, &intIfNum,
                                &lldpXdot3RemPortEntryData.lldpRemIndex) != L7_SUCCESS)))
    return(NULL);

  if (usmDbExtIfNumFromIntIfNum(intIfNum, 
                                &lldpXdot3RemPortEntryData.lldpRemLocalPortNum) != L7_SUCCESS)
    return(NULL);

  SET_VALID(I_lldpXdot3RemPortEntryIndex_lldpRemLocalPortNum, lldpXdot3RemPortEntryData.valid);

  switch (nominator)
  {
  case -1:
    /* fallthrough all cases except default */
  case I_lldpXdot3RemPortEntryIndex_lldpRemTimeMark:
  case I_lldpXdot3RemPortEntryIndex_lldpRemLocalPortNum:
  case I_lldpXdot3RemPortEntryIndex_lldpRemIndex:
    if (nominator != -1) break;

  case I_lldpXdot3RemPortAutoNegSupported:
    if (snmpLldpXdot3RemPortAutoNegSupportedGet(lldpXdot3RemPortEntryData.lldpRemTimeMark, intIfNum, 
                                                lldpXdot3RemPortEntryData.lldpRemIndex,
                                                &lldpXdot3RemPortEntryData.lldpXdot3RemPortAutoNegSupported) == L7_SUCCESS)
      SET_VALID(I_lldpXdot3RemPortAutoNegSupported, lldpXdot3RemPortEntryData.valid);
    break;

  case I_lldpXdot3RemPortAutoNegEnabled:
    if (snmpLldpXdot3RemPortAutoNegEnabledGet(lldpXdot3RemPortEntryData.lldpRemTimeMark, intIfNum, 
                                              lldpXdot3RemPortEntryData.lldpRemIndex,
                                              &lldpXdot3RemPortEntryData.lldpXdot3RemPortAutoNegEnabled) == L7_SUCCESS)
      SET_VALID(I_lldpXdot3RemPortAutoNegEnabled, lldpXdot3RemPortEntryData.valid);
    break;

  case I_lldpXdot3RemPortAutoNegAdvertisedCap:
    memset(snmp_buffer, 0, sizeof(snmp_buffer));
    if (snmpLldpXdot3RemPortAutoNegAdvertisedCapGet(lldpXdot3RemPortEntryData.lldpRemTimeMark, intIfNum, 
                                                    lldpXdot3RemPortEntryData.lldpRemIndex,
                                                    snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetString(&lldpXdot3RemPortEntryData.lldpXdot3RemPortAutoNegAdvertisedCap, snmp_buffer, 1) == L7_TRUE)
      SET_VALID(I_lldpXdot3RemPortAutoNegAdvertisedCap, lldpXdot3RemPortEntryData.valid);
    break;

  case I_lldpXdot3RemPortOperMauType:
    if (snmpLldpXdot3RemPortOperMauTypeGet(lldpXdot3RemPortEntryData.lldpRemTimeMark, intIfNum, 
                                           lldpXdot3RemPortEntryData.lldpRemIndex,
                                           &lldpXdot3RemPortEntryData.lldpXdot3RemPortOperMauType) == L7_SUCCESS)
      SET_VALID(I_lldpXdot3RemPortOperMauType, lldpXdot3RemPortEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, lldpXdot3RemPortEntryData.valid))
    return(NULL);

  return(&lldpXdot3RemPortEntryData);
}

lldpXdot3RemPowerEntry_t *
k_lldpXdot3RemPowerEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_UINT32 lldpRemTimeMark,
                             SR_INT32 lldpRemLocalPortNum,
                             SR_INT32 lldpRemIndex)
{
#ifdef NOT_YET
   static lldpXdot3RemPowerEntry_t lldpXdot3RemPowerEntryData;

   /*
    * put your code to retrieve the information here
    */

   lldpXdot3RemPowerEntryData.lldpXdot3RemPowerPortClass = ;
   lldpXdot3RemPowerEntryData.lldpXdot3RemPowerMDISupported = ;
   lldpXdot3RemPowerEntryData.lldpXdot3RemPowerMDIEnabled = ;
   lldpXdot3RemPowerEntryData.lldpXdot3RemPowerPairControlable = ;
   lldpXdot3RemPowerEntryData.lldpXdot3RemPowerPairs = ;
   lldpXdot3RemPowerEntryData.lldpXdot3RemPowerClass = ;
   lldpXdot3RemPowerEntryData.lldpRemTimeMark = ;
   lldpXdot3RemPowerEntryData.lldpRemLocalPortNum = ;
   lldpXdot3RemPowerEntryData.lldpRemIndex = ;
   SET_ALL_VALID(lldpXdot3RemPowerEntryData.valid);
   return(&lldpXdot3RemPowerEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

lldpXdot3RemLinkAggEntry_t *
k_lldpXdot3RemLinkAggEntry_get(int serialNum, ContextInfo *contextInfo,
                               int nominator,
                               int searchType,
                               SR_UINT32 lldpRemTimeMark,
                               SR_INT32 lldpRemLocalPortNum,
                               SR_INT32 lldpRemIndex)
{
#ifdef NOT_YET
   static lldpXdot3RemLinkAggEntry_t lldpXdot3RemLinkAggEntryData;

   /*
    * put your code to retrieve the information here
    */

   lldpXdot3RemLinkAggEntryData.lldpXdot3RemLinkAggStatus = ;
   lldpXdot3RemLinkAggEntryData.lldpXdot3RemLinkAggPortId = ;
   lldpXdot3RemLinkAggEntryData.lldpRemTimeMark = ;
   lldpXdot3RemLinkAggEntryData.lldpRemLocalPortNum = ;
   lldpXdot3RemLinkAggEntryData.lldpRemIndex = ;
   SET_ALL_VALID(lldpXdot3RemLinkAggEntryData.valid);
   return(&lldpXdot3RemLinkAggEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

lldpXdot3RemMaxFrameSizeEntry_t *
k_lldpXdot3RemMaxFrameSizeEntry_get(int serialNum, ContextInfo *contextInfo,
                                    int nominator,
                                    int searchType,
                                    SR_UINT32 lldpRemTimeMark,
                                    SR_INT32 lldpRemLocalPortNum,
                                    SR_INT32 lldpRemIndex)
{
#ifdef NOT_YET
   static lldpXdot3RemMaxFrameSizeEntry_t lldpXdot3RemMaxFrameSizeEntryData;

   /*
    * put your code to retrieve the information here
    */

   lldpXdot3RemMaxFrameSizeEntryData.lldpXdot3RemMaxFrameSize = ;
   lldpXdot3RemMaxFrameSizeEntryData.lldpRemTimeMark = ;
   lldpXdot3RemMaxFrameSizeEntryData.lldpRemLocalPortNum = ;
   lldpXdot3RemMaxFrameSizeEntryData.lldpRemIndex = ;
   SET_ALL_VALID(lldpXdot3RemMaxFrameSizeEntryData.valid);
   return(&lldpXdot3RemMaxFrameSizeEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

lldpXMedConfig_t *
k_lldpXMedConfig_get(int serialNum, ContextInfo *contextInfo,
                     int nominator)
{
  static lldpXMedConfig_t lldpXMedConfigData;

  ZERO_VALID(lldpXMedConfigData.valid);

  switch (nominator)
  {
  case I_lldpXMedLocDeviceClass:
    if (snmpLldpXMedLocDeviceClassGet(&lldpXMedConfigData.lldpXMedLocDeviceClass) == L7_SUCCESS)
    {
      SET_VALID(I_lldpXMedLocDeviceClass, lldpXMedConfigData.valid);
    }
    if (nominator != -1) break;
  case I_lldpXMedFastStartRepeatCount:
    if (usmDbLldpXMedFastStartRepeatCountGet(&lldpXMedConfigData.lldpXMedFastStartRepeatCount) == L7_SUCCESS)
    {
      SET_VALID(I_lldpXMedFastStartRepeatCount, lldpXMedConfigData.valid);
    }
    break;
  default:
    /* unknown nominator */
    return(NULL);
  }

  if (nominator != -1 && !VALID(nominator, lldpXMedConfigData.valid))
    return(NULL);

  return(&lldpXMedConfigData);
}

#ifdef SETS
int
k_lldpXMedConfig_test(ObjectInfo *object, ObjectSyntax *value,
                      doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_lldpXMedConfig_ready(ObjectInfo *object, ObjectSyntax *value, 
                       doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_lldpXMedConfig_set(lldpXMedConfig_t *data,
                     ContextInfo *contextInfo, int function)
{
  if (VALID(I_lldpXMedFastStartRepeatCount, data->valid) &&
      usmDbLldpXMedFastStartRepeatCountSet(data->lldpXMedFastStartRepeatCount) != L7_SUCCESS)
   return COMMIT_FAILED_ERROR;

  return NO_ERROR;
}

#ifdef SR_lldpXMedConfig_UNDO
/* add #define SR_lldpXMedConfig_UNDO in sitedefs.h to
 * include the undo routine for the lldpXMedConfig family.
 */
int
lldpXMedConfig_undo(doList_t *doHead, doList_t *doCur,
                    ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_lldpXMedConfig_UNDO */

#endif /* SETS */

lldpXMedLocalData_t *
k_lldpXMedLocalData_get(int serialNum, ContextInfo *contextInfo,
                        int nominator)
{
  static lldpXMedLocalData_t lldpXMedLocalDataData;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];

  ZERO_VALID(lldpXMedLocalDataData.valid);

  switch (nominator)
  {
  case -1:
    /* fallthrough all cases except default */
  case I_lldpXMedLocHardwareRev:
    if (usmDbLldpXMedLocHardwareRevGet(snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&lldpXMedLocalDataData.lldpXMedLocHardwareRev, snmp_buffer) == L7_TRUE)
    {
      SET_VALID(I_lldpXMedLocHardwareRev, lldpXMedLocalDataData.valid);
    }
    if (nominator != -1) break;
  case I_lldpXMedLocFirmwareRev:
    if (usmDbLldpXMedLocFirmwareRevGet(snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&lldpXMedLocalDataData.lldpXMedLocFirmwareRev, snmp_buffer) == L7_TRUE)
    {
      SET_VALID(I_lldpXMedLocFirmwareRev, lldpXMedLocalDataData.valid);
    }
    if (nominator != -1) break;
  case I_lldpXMedLocSoftwareRev:
    if (usmDbLldpXMedLocSoftwareRevGet(snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&lldpXMedLocalDataData.lldpXMedLocSoftwareRev, snmp_buffer) == L7_TRUE)
    {
      SET_VALID(I_lldpXMedLocSoftwareRev, lldpXMedLocalDataData.valid);
    }
    if (nominator != -1) break;
  case I_lldpXMedLocSerialNum:
    if (usmDbLldpXMedLocSerialNumGet(snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&lldpXMedLocalDataData.lldpXMedLocSerialNum, snmp_buffer) == L7_TRUE)
    {
      SET_VALID(I_lldpXMedLocSerialNum, lldpXMedLocalDataData.valid);
    }
    if (nominator != -1) break;
  case I_lldpXMedLocMfgName:
    if (usmDbLldpXMedLocMfgNameGet(snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&lldpXMedLocalDataData.lldpXMedLocMfgName, snmp_buffer) == L7_TRUE)
    {
      SET_VALID(I_lldpXMedLocMfgName, lldpXMedLocalDataData.valid);
    }
    if (nominator != -1) break;
  case I_lldpXMedLocModelName:
    if (usmDbLldpXMedLocModelNameGet(snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&lldpXMedLocalDataData.lldpXMedLocModelName, snmp_buffer) == L7_TRUE)
    {
      SET_VALID(I_lldpXMedLocModelName, lldpXMedLocalDataData.valid);
    }
    if (nominator != -1) break;
  case I_lldpXMedLocAssetID:
    if (usmDbLldpXMedLocAssetIDGet(snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&lldpXMedLocalDataData.lldpXMedLocAssetID, snmp_buffer) == L7_TRUE)
    {
      SET_VALID(I_lldpXMedLocAssetID, lldpXMedLocalDataData.valid);
    }
    if (nominator != -1) break;
  case I_lldpXMedLocXPoEDeviceType:
    if (snmpLldpXMedLocXPoeDeviceTypeGet(&lldpXMedLocalDataData.lldpXMedLocXPoEDeviceType) == L7_SUCCESS)
    {
      SET_VALID(I_lldpXMedLocXPoEDeviceType, lldpXMedLocalDataData.valid);
    }
    if (nominator != -1) break;
  case I_lldpXMedLocXPoEPSEPowerSource:
    if (snmpLldpXMedLocXPoEPSEPowerSourceGet(&lldpXMedLocalDataData.lldpXMedLocXPoEPSEPowerSource) == L7_SUCCESS)
    {
      SET_VALID(I_lldpXMedLocXPoEPSEPowerSource, lldpXMedLocalDataData.valid);
    }
    if (nominator != -1) break;
  case I_lldpXMedLocXPoEPDPowerReq:
    if (snmpLldpXMedLocXPoePDPowerReqGet(&lldpXMedLocalDataData.lldpXMedLocXPoEPDPowerReq) == L7_SUCCESS)
    {
      SET_VALID(I_lldpXMedLocXPoEPDPowerReq, lldpXMedLocalDataData.valid);
    }
    if (nominator != -1) break;
  case I_lldpXMedLocXPoEPDPowerSource:
    if (snmpLldpXMedLocXPoEPDPowerSourceGet(&lldpXMedLocalDataData.lldpXMedLocXPoEPDPowerSource) == L7_SUCCESS)
    {
      SET_VALID(I_lldpXMedLocXPoEPDPowerSource, lldpXMedLocalDataData.valid);
    }
    if (nominator != -1) break;
  case I_lldpXMedLocXPoEPDPowerPriority:
    if (snmpLldpXMedLocXPoEPDPowerPriorityGet(&lldpXMedLocalDataData.lldpXMedLocXPoEPDPowerPriority) == L7_SUCCESS)
    {
      SET_VALID(I_lldpXMedLocXPoEPDPowerPriority, lldpXMedLocalDataData.valid);
    }
    break;
  default:
    /* unknown nominator */
    return(NULL);
  }

  if (nominator != -1 && !VALID(nominator, lldpXMedLocalDataData.valid))
    return(NULL);
  return(&lldpXMedLocalDataData);
}

lldpXMedLocMediaPolicyEntry_t *
k_lldpXMedLocMediaPolicyEntry_get(int serialNum, ContextInfo *contextInfo,
                                  int nominator,
                                  int searchType,
                                  SR_INT32 lldpLocPortNum,
                                  OctetString * lldpXMedLocMediaPolicyAppType)
{
  static lldpXMedLocMediaPolicyEntry_t lldpXMedLocMediaPolicyEntryData;
  static L7_BOOL firstTime = L7_TRUE;

  ZERO_VALID(lldpXMedLocMediaPolicyEntryData.valid);

  if (firstTime == L7_TRUE)
  {
    lldpXMedLocMediaPolicyEntryData.lldpXMedLocMediaPolicyAppType = MakeOctetString(NULL,0);
    firstTime = L7_FALSE;
  }
  lldpXMedLocMediaPolicyEntryData.lldpLocPortNum = lldpLocPortNum;
  SET_VALID(I_lldpXMedLocMediaPolicyEntryIndex_lldpLocPortNum, lldpXMedLocMediaPolicyEntryData.valid);

  FreeOctetString(lldpXMedLocMediaPolicyEntryData.lldpXMedLocMediaPolicyAppType);
  lldpXMedLocMediaPolicyEntryData.lldpXMedLocMediaPolicyAppType = CloneOctetString(lldpXMedLocMediaPolicyAppType);
  SET_VALID(I_lldpXMedLocMediaPolicyAppType, lldpXMedLocMediaPolicyEntryData.valid);

  if (searchType == EXACT ?
    snmpLldpXMedLocMediaPolicyEntryGet(&lldpXMedLocMediaPolicyEntryData, nominator) != L7_SUCCESS :
    snmpLldpXMedLocMediaPolicyEntryNextGet(&lldpXMedLocMediaPolicyEntryData, nominator) != L7_SUCCESS)
  {
    return(NULL);
  }

  return(&lldpXMedLocMediaPolicyEntryData);
}

lldpXMedLocLocationEntry_t *
k_lldpXMedLocLocationEntry_get(int serialNum, ContextInfo *contextInfo,
                               int nominator,
                               int searchType,
                               SR_INT32 lldpLocPortNum,
                               SR_INT32 lldpXMedLocLocationSubtype)
{
  static lldpXMedLocLocationEntry_t lldpXMedLocLocationEntryData;
  L7_uint32 intIfNum;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  static L7_BOOL firstTime = L7_TRUE;

  ZERO_VALID(lldpXMedLocLocationEntryData.valid);

  if (firstTime == L7_TRUE)
  {
    lldpXMedLocLocationEntryData.lldpXMedLocLocationInfo = MakeOctetString(NULL,0);
    firstTime = L7_FALSE;
  }

  ZERO_VALID(lldpXMedLocLocationEntryData.valid);

  lldpXMedLocLocationEntryData.lldpLocPortNum = lldpLocPortNum;
  SET_VALID(I_lldpXMedLocLocationEntryIndex_lldpLocPortNum, lldpXMedLocLocationEntryData.valid);
  lldpXMedLocLocationEntryData.lldpXMedLocLocationSubtype = lldpXMedLocLocationSubtype;
  SET_VALID(I_lldpXMedLocLocationSubtype, lldpXMedLocLocationEntryData.valid);

  if (usmDbIntIfNumFromExtIfNum(lldpXMedLocLocationEntryData.lldpLocPortNum,
                                &intIfNum) != L7_SUCCESS)
    return(NULL);

  if (searchType == EXACT ? 
      (snmpLldpXMedLocLocationEntryGet(intIfNum, lldpXMedLocLocationEntryData.lldpXMedLocLocationSubtype, snmp_buffer) != L7_SUCCESS) :
      ((snmpLldpXMedLocLocationEntryGet(intIfNum, lldpXMedLocLocationEntryData.lldpXMedLocLocationSubtype, snmp_buffer) != L7_SUCCESS) &&
       (snmpLldpXMedLocLocationEntryNextGet(&intIfNum, &lldpXMedLocLocationEntryData.lldpXMedLocLocationSubtype, snmp_buffer) != L7_SUCCESS)))
    return(NULL);

  if (usmDbExtIfNumFromIntIfNum(intIfNum, 
                                &lldpXMedLocLocationEntryData.lldpLocPortNum) != L7_SUCCESS)
    return(NULL);

  switch (nominator)
  {
  case -1:
  case I_lldpXMedLocLocationEntryIndex_lldpLocPortNum:
  case I_lldpXMedLocLocationSubtype:
    if (nominator != -1) break;

  case I_lldpXMedLocLocationInfo:
    if (SafeMakeOctetStringFromText(&lldpXMedLocLocationEntryData.lldpXMedLocLocationInfo,
                                    snmp_buffer) == L7_TRUE)
      SET_VALID(I_lldpXMedLocLocationInfo, lldpXMedLocLocationEntryData.valid);
    break;
  default:
    /* unknown nominator */
    return(NULL);
  }

  if (nominator != -1 && !VALID(nominator, lldpXMedLocLocationEntryData.valid))
    return(NULL);
  return(&lldpXMedLocLocationEntryData);
}

#ifdef SETS
int
k_lldpXMedLocLocationEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_lldpXMedLocLocationEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                 doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_lldpXMedLocLocationEntry_set_defaults(doList_t *dp)
{
    lldpXMedLocLocationEntry_t *data = (lldpXMedLocLocationEntry_t *) (dp->data);

    if ((data->lldpXMedLocLocationInfo = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_lldpXMedLocLocationEntry_set(lldpXMedLocLocationEntry_t *data,
                               ContextInfo *contextInfo, int function)
{
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 temp_subType;
  L7_uint32 intIfNum;

  if (VALID(I_lldpXMedLocLocationInfo, data->valid))
  {
    memset(snmp_buffer, 0, sizeof(snmp_buffer));
    memcpy(snmp_buffer, data->lldpXMedLocLocationInfo->octet_ptr, data->lldpXMedLocLocationInfo->length);
    if (snmpLldpXMedLocLocationSubTypeGet(data->lldpXMedLocLocationSubtype, &temp_subType) != L7_SUCCESS ||
        usmDbIntIfNumFromExtIfNum(data->lldpLocPortNum, &intIfNum) != L7_SUCCESS ||
        usmDbLldpXMedLocLocationInfoSet(intIfNum, temp_subType, snmp_buffer) != L7_SUCCESS)
      return COMMIT_FAILED_ERROR;
  }

  return NO_ERROR;
}

#ifdef SR_lldpXMedLocLocationEntry_UNDO
/* add #define SR_lldpXMedLocLocationEntry_UNDO in sitedefs.h to
 * include the undo routine for the lldpXMedLocLocationEntry family.
 */
int
lldpXMedLocLocationEntry_undo(doList_t *doHead, doList_t *doCur,
                              ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_lldpXMedLocLocationEntry_UNDO */

#endif /* SETS */

lldpXMedLocXPoEPSEPortEntry_t *
k_lldpXMedLocXPoEPSEPortEntry_get(int serialNum, ContextInfo *contextInfo,
                                  int nominator,
                                  int searchType,
                                  SR_INT32 lldpLocPortNum)
{
  static lldpXMedLocXPoEPSEPortEntry_t lldpXMedLocXPoEPSEPortEntryData;
  L7_uint32 intIfNum;

  ZERO_VALID(lldpXMedLocXPoEPSEPortEntryData.valid);
  lldpXMedLocXPoEPSEPortEntryData.lldpLocPortNum = lldpLocPortNum;
  SET_VALID(I_lldpXMedLocXPoEPSEPortEntryIndex_lldpLocPortNum, lldpXMedLocXPoEPSEPortEntryData.valid);

  if ((searchType == EXACT ? 
       (usmDbPhysicalIntIfNumberCheck(USMDB_UNIT_CURRENT, 
                                      lldpXMedLocXPoEPSEPortEntryData.lldpLocPortNum) != L7_SUCCESS) :
       (usmDbPhysicalIntIfNumberCheck(USMDB_UNIT_CURRENT, 
                                      lldpXMedLocXPoEPSEPortEntryData.lldpLocPortNum) != L7_SUCCESS &&
        usmDbGetNextPhysicalIntIfNumber(lldpXMedLocXPoEPSEPortEntryData.lldpLocPortNum, 
                                        &lldpXMedLocXPoEPSEPortEntryData.lldpLocPortNum) != L7_SUCCESS)) ||
      (usmDbIntIfNumFromExtIfNum(lldpXMedLocXPoEPSEPortEntryData.lldpLocPortNum, &intIfNum) != L7_SUCCESS)||
      (usmDbLldpPoeIsValidIntf(intIfNum)!=L7_SUCCESS))
  {
    ZERO_VALID(lldpXMedLocXPoEPSEPortEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
    /* fallthrough all cases except default */
  case I_lldpXMedLocXPoEPSEPortEntryIndex_lldpLocPortNum:
    if (nominator != -1) break;

  case I_lldpXMedLocXPoEPSEPortPowerAv:
    if (usmDbLldpXMedLocXPoePSEPortPowerAvGet(intIfNum, &lldpXMedLocXPoEPSEPortEntryData.lldpXMedLocXPoEPSEPortPowerAv) == L7_SUCCESS)
      SET_VALID(I_lldpXMedLocXPoEPSEPortPowerAv, lldpXMedLocXPoEPSEPortEntryData.valid);
    if (nominator != -1) break;

  case I_lldpXMedLocXPoEPSEPortPDPriority:
    if (snmpLldpXMedLocXPoePSEPortPDPriorityGet(intIfNum, &lldpXMedLocXPoEPSEPortEntryData.lldpXMedLocXPoEPSEPortPDPriority) == L7_SUCCESS)
      SET_VALID(I_lldpXMedLocXPoEPSEPortPDPriority, lldpXMedLocXPoEPSEPortEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, lldpXMedLocXPoEPSEPortEntryData.valid))
    return(NULL);

  return(&lldpXMedLocXPoEPSEPortEntryData);
}

lldpXMedRemCapabilitiesEntry_t *
k_lldpXMedRemCapabilitiesEntry_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator,
                                   int searchType,
                                   SR_UINT32 lldpRemTimeMark,
                                   SR_INT32 lldpRemLocalPortNum,
                                   SR_INT32 lldpRemIndex)
{
  static lldpXMedRemCapabilitiesEntry_t lldpXMedRemCapabilitiesEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[1];
  L7_uint32 intIfNum;

  if (firstTime == L7_TRUE)
  {
    lldpXMedRemCapabilitiesEntryData.lldpXMedRemCapSupported = MakeOctetStringFromText("");
    lldpXMedRemCapabilitiesEntryData.lldpXMedRemCapCurrent = MakeOctetStringFromText("");
    firstTime = L7_FALSE;
  }

  memset(snmp_buffer, 0, 1);
  ZERO_VALID(lldpXMedRemCapabilitiesEntryData.valid);

  lldpXMedRemCapabilitiesEntryData.lldpRemTimeMark = lldpRemTimeMark;
  SET_VALID(I_lldpXMedRemCapabilitiesEntryIndex_lldpRemTimeMark, lldpXMedRemCapabilitiesEntryData.valid);

  lldpXMedRemCapabilitiesEntryData.lldpRemLocalPortNum = lldpRemLocalPortNum;
  SET_VALID(I_lldpXMedRemCapabilitiesEntryIndex_lldpRemLocalPortNum, lldpXMedRemCapabilitiesEntryData.valid);

  lldpXMedRemCapabilitiesEntryData.lldpRemIndex = lldpRemIndex;
  SET_VALID(I_lldpXMedRemCapabilitiesEntryIndex_lldpRemIndex, lldpXMedRemCapabilitiesEntryData.valid);

  if (usmDbIntIfNumFromExtIfNum(lldpXMedRemCapabilitiesEntryData.lldpRemLocalPortNum, 
                                &intIfNum) != L7_SUCCESS)
    return(NULL);

  if ((searchType == EXACT) ?
      (snmpLldpRemEntryGet(lldpXMedRemCapabilitiesEntryData.lldpRemTimeMark, intIfNum,
                           lldpXMedRemCapabilitiesEntryData.lldpRemIndex) != L7_SUCCESS) :
      ((snmpLldpRemEntryGet(lldpXMedRemCapabilitiesEntryData.lldpRemTimeMark, intIfNum,
                            lldpXMedRemCapabilitiesEntryData.lldpRemIndex) != L7_SUCCESS) &&
       (snmpLldpRemEntryGetNext(&lldpXMedRemCapabilitiesEntryData.lldpRemTimeMark, &intIfNum,
                                &lldpXMedRemCapabilitiesEntryData.lldpRemIndex) != L7_SUCCESS)))
    return(NULL);

  if (usmDbExtIfNumFromIntIfNum(intIfNum, 
                                &lldpXMedRemCapabilitiesEntryData.lldpRemLocalPortNum) != L7_SUCCESS)
    return(NULL);

  switch (nominator)
  {
  case -1:
    /* fallthrough all cases except default */
  case I_lldpXMedRemCapabilitiesEntryIndex_lldpRemTimeMark:
  case I_lldpXMedRemCapabilitiesEntryIndex_lldpRemLocalPortNum:
  case I_lldpXMedRemCapabilitiesEntryIndex_lldpRemIndex:
    if (nominator != -1) break;
  case I_lldpXMedRemCapSupported:
    if (snmpLldpXMedRemCapSupportedGet(intIfNum,
                                       lldpXMedRemCapabilitiesEntryData.lldpRemIndex,
                                       lldpXMedRemCapabilitiesEntryData.lldpRemTimeMark,
                                       snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetString(&lldpXMedRemCapabilitiesEntryData.lldpXMedRemCapSupported,
                            snmp_buffer, 1) == L7_TRUE)
      SET_VALID(I_lldpXMedRemCapSupported, lldpXMedRemCapabilitiesEntryData.valid);
    if (nominator != -1) break;
  case I_lldpXMedRemCapCurrent:
    if (snmpLldpXMedRemCapCurrentGet(intIfNum,
                                     lldpXMedRemCapabilitiesEntryData.lldpRemIndex,
                                     lldpXMedRemCapabilitiesEntryData.lldpRemTimeMark,
                                     snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetString(&lldpXMedRemCapabilitiesEntryData.lldpXMedRemCapCurrent,
                            snmp_buffer, 1) == L7_TRUE)
      SET_VALID(I_lldpXMedRemCapCurrent, lldpXMedRemCapabilitiesEntryData.valid);
    if (nominator != -1) break;
  case I_lldpXMedRemDeviceClass:
    if (snmpLldpXMedRemDeviceClassGet(intIfNum,
                                      lldpXMedRemCapabilitiesEntryData.lldpRemIndex,
                                      lldpXMedRemCapabilitiesEntryData.lldpRemTimeMark,
                                      &lldpXMedRemCapabilitiesEntryData.lldpXMedRemDeviceClass) == L7_SUCCESS)
      SET_VALID(I_lldpXMedRemDeviceClass, lldpXMedRemCapabilitiesEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
  }

  if (nominator != -1 && !VALID(nominator, lldpXMedRemCapabilitiesEntryData.valid))
    return(NULL);
  return(&lldpXMedRemCapabilitiesEntryData);
}

lldpXMedRemMediaPolicyEntry_t *
k_lldpXMedRemMediaPolicyEntry_get(int serialNum, ContextInfo *contextInfo,
                                  int nominator,
                                  int searchType,
                                  SR_UINT32 lldpRemTimeMark,
                                  SR_INT32 lldpRemLocalPortNum,
                                  SR_INT32 lldpRemIndex,
                                  OctetString * lldpXMedRemMediaPolicyAppType)
{
  static lldpXMedRemMediaPolicyEntry_t lldpXMedRemMediaPolicyEntryData;
  L7_uint32 intIfNum;
  lldpXMedPolicyAppTypeValue_t appType = videoSignaling_policyapptype;
  L7_uchar8 temp_buf[2];
  L7_uint32 temp_buf_len;
  static L7_BOOL firstTime = L7_TRUE;

  ZERO_VALID(lldpXMedRemMediaPolicyEntryData.valid);

  if (firstTime == L7_TRUE)
  {
    lldpXMedRemMediaPolicyEntryData.lldpXMedRemMediaPolicyAppType = MakeOctetString(NULL,0);
    firstTime = L7_FALSE;
  }

  lldpXMedRemMediaPolicyEntryData.lldpRemTimeMark = lldpRemTimeMark;
  lldpXMedRemMediaPolicyEntryData.lldpRemIndex = lldpRemIndex;

  if (usmDbIntIfNumFromExtIfNum(lldpRemLocalPortNum,
                                &intIfNum) != L7_SUCCESS)
    return(NULL);

  memcpy(temp_buf, lldpXMedRemMediaPolicyAppType->octet_ptr, lldpXMedRemMediaPolicyAppType->length);
  temp_buf_len = lldpXMedRemMediaPolicyAppType->length;

  snmpLldpXMedLocMediaPolicyAppTypeFromBitmask(temp_buf, temp_buf_len, &appType);

  if (searchType == EXACT ? 
      (snmpLldpXMedRemMediaPolicyEntryGet(intIfNum, lldpXMedRemMediaPolicyEntryData.lldpRemIndex, 
                                          lldpXMedRemMediaPolicyEntryData.lldpRemTimeMark,
                                          appType) != L7_SUCCESS) :
      (snmpLldpXMedRemMediaPolicyEntryNextGet(&intIfNum, &lldpXMedRemMediaPolicyEntryData.lldpRemIndex, 
                                              &lldpXMedRemMediaPolicyEntryData.lldpRemTimeMark,
                                              temp_buf, &temp_buf_len) != L7_SUCCESS))
    return(NULL);

  snmpLldpXMedLocMediaPolicyAppTypeFromBitmask(temp_buf, temp_buf_len, &appType);

  if ((SafeMakeOctetString(&lldpXMedRemMediaPolicyEntryData.lldpXMedRemMediaPolicyAppType,
                           temp_buf, temp_buf_len) != L7_TRUE) ||
      usmDbExtIfNumFromIntIfNum(intIfNum, 
                                &lldpXMedRemMediaPolicyEntryData.lldpRemLocalPortNum) != L7_SUCCESS)
    return(NULL);


  SET_VALID(I_lldpXMedRemMediaPolicyEntryIndex_lldpRemTimeMark, lldpXMedRemMediaPolicyEntryData.valid);
  SET_VALID(I_lldpXMedRemMediaPolicyEntryIndex_lldpRemIndex, lldpXMedRemMediaPolicyEntryData.valid);
  SET_VALID(I_lldpXMedRemMediaPolicyEntryIndex_lldpRemLocalPortNum, lldpXMedRemMediaPolicyEntryData.valid);
  SET_VALID(I_lldpXMedRemMediaPolicyAppType, lldpXMedRemMediaPolicyEntryData.valid);

  switch (nominator)
  {
  case -1:
    /* fallthrough all cases except default */
  case I_lldpXMedRemMediaPolicyEntryIndex_lldpRemTimeMark:
  case I_lldpXMedRemMediaPolicyEntryIndex_lldpRemIndex:
  case I_lldpXMedRemMediaPolicyEntryIndex_lldpRemLocalPortNum:
  case I_lldpXMedRemMediaPolicyAppType:
    if (nominator != -1) break;
  case I_lldpXMedRemMediaPolicyVlanID:
    if (usmDbLldpXMedRemMediaPolicyVlanIdGet(intIfNum, 
                                             lldpXMedRemMediaPolicyEntryData.lldpRemIndex, 
                                             lldpXMedRemMediaPolicyEntryData.lldpRemTimeMark,
                                             appType, 
                                             &lldpXMedRemMediaPolicyEntryData.lldpXMedRemMediaPolicyVlanID) == L7_SUCCESS)
      SET_VALID(I_lldpXMedRemMediaPolicyVlanID, lldpXMedRemMediaPolicyEntryData.valid);
    if (nominator != -1) break;
  case I_lldpXMedRemMediaPolicyPriority:
    if (usmDbLldpXMedRemMediaPolicyPriorityGet(intIfNum, 
                                               lldpXMedRemMediaPolicyEntryData.lldpRemIndex, 
                                               lldpXMedRemMediaPolicyEntryData.lldpRemTimeMark,
                                               appType, 
                                               &lldpXMedRemMediaPolicyEntryData.lldpXMedRemMediaPolicyPriority) == L7_SUCCESS)
      SET_VALID(I_lldpXMedRemMediaPolicyPriority, lldpXMedRemMediaPolicyEntryData.valid);
    if (nominator != -1) break;
  case I_lldpXMedRemMediaPolicyDscp:
    if (usmDbLldpXMedRemMediaPolicyDscpGet(intIfNum, 
                                           lldpXMedRemMediaPolicyEntryData.lldpRemIndex, 
                                           lldpXMedRemMediaPolicyEntryData.lldpRemTimeMark,
                                           appType, 
                                           &lldpXMedRemMediaPolicyEntryData.lldpXMedRemMediaPolicyDscp) == L7_SUCCESS)
      SET_VALID(I_lldpXMedRemMediaPolicyDscp, lldpXMedRemMediaPolicyEntryData.valid);
    if (nominator != -1) break;
  case I_lldpXMedRemMediaPolicyUnknown:
    if (snmpLldpXMedRemMediaPolicyUnknownGet(intIfNum, 
                                             lldpXMedRemMediaPolicyEntryData.lldpRemIndex, 
                                             lldpXMedRemMediaPolicyEntryData.lldpRemTimeMark,
                                             appType, 
                                             &lldpXMedRemMediaPolicyEntryData.lldpXMedRemMediaPolicyUnknown) == L7_SUCCESS)
      SET_VALID(I_lldpXMedRemMediaPolicyUnknown, lldpXMedRemMediaPolicyEntryData.valid);
    if (nominator != -1) break;
  case I_lldpXMedRemMediaPolicyTagged:
    if (snmpLldpXMedRemMediaPolicyTaggedGet(intIfNum, 
                                            lldpXMedRemMediaPolicyEntryData.lldpRemIndex, 
                                            lldpXMedRemMediaPolicyEntryData.lldpRemTimeMark,
                                            appType, 
                                            &lldpXMedRemMediaPolicyEntryData.lldpXMedRemMediaPolicyTagged) == L7_SUCCESS)
      SET_VALID(I_lldpXMedRemMediaPolicyTagged, lldpXMedRemMediaPolicyEntryData.valid);
    break;
  default:
    /* unknown nominator */
    return(NULL);
  }

  if (nominator != -1 && !VALID(nominator, lldpXMedRemMediaPolicyEntryData.valid))
    return(NULL);
  return(&lldpXMedRemMediaPolicyEntryData);
}

lldpXMedRemInventoryEntry_t *
k_lldpXMedRemInventoryEntry_get(int serialNum, ContextInfo *contextInfo,
                                int nominator,
                                int searchType,
                                SR_UINT32 lldpRemTimeMark,
                                SR_INT32 lldpRemLocalPortNum,
                                SR_INT32 lldpRemIndex)
{
  static lldpXMedRemInventoryEntry_t lldpXMedRemInventoryEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 intIfNum;

  if (firstTime == L7_TRUE)
  {
    lldpXMedRemInventoryEntryData.lldpXMedRemHardwareRev = MakeOctetStringFromText("");
    lldpXMedRemInventoryEntryData.lldpXMedRemFirmwareRev = MakeOctetStringFromText("");
    lldpXMedRemInventoryEntryData.lldpXMedRemSoftwareRev = MakeOctetStringFromText("");
    lldpXMedRemInventoryEntryData.lldpXMedRemSerialNum = MakeOctetStringFromText("");
    lldpXMedRemInventoryEntryData.lldpXMedRemMfgName = MakeOctetStringFromText("");
    lldpXMedRemInventoryEntryData.lldpXMedRemModelName = MakeOctetStringFromText("");
    lldpXMedRemInventoryEntryData.lldpXMedRemAssetID = MakeOctetStringFromText("");
    firstTime = L7_FALSE;
  }

  memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
  ZERO_VALID(lldpXMedRemInventoryEntryData.valid);

  lldpXMedRemInventoryEntryData.lldpRemTimeMark = lldpRemTimeMark;
  SET_VALID(I_lldpXMedRemInventoryEntryIndex_lldpRemTimeMark, lldpXMedRemInventoryEntryData.valid);

  lldpXMedRemInventoryEntryData.lldpRemLocalPortNum = lldpRemLocalPortNum;
  SET_VALID(I_lldpXMedRemInventoryEntryIndex_lldpRemLocalPortNum, lldpXMedRemInventoryEntryData.valid);

  lldpXMedRemInventoryEntryData.lldpRemIndex = lldpRemIndex;
  SET_VALID(I_lldpXMedRemInventoryEntryIndex_lldpRemIndex, lldpXMedRemInventoryEntryData.valid);

  if (usmDbIntIfNumFromExtIfNum(lldpXMedRemInventoryEntryData.lldpRemLocalPortNum, 
                                &intIfNum) != L7_SUCCESS)
    return(NULL);

  if ((searchType == EXACT) ?
      (snmpLldpRemEntryGet(lldpXMedRemInventoryEntryData.lldpRemTimeMark, intIfNum,
                           lldpXMedRemInventoryEntryData.lldpRemIndex) != L7_SUCCESS) :
      ((snmpLldpRemEntryGet(lldpXMedRemInventoryEntryData.lldpRemTimeMark, intIfNum,
                            lldpXMedRemInventoryEntryData.lldpRemIndex) != L7_SUCCESS) &&
       (snmpLldpRemEntryGetNext(&lldpXMedRemInventoryEntryData.lldpRemTimeMark, &intIfNum,
                                &lldpXMedRemInventoryEntryData.lldpRemIndex) != L7_SUCCESS)))
    return(NULL);

  if (usmDbExtIfNumFromIntIfNum(intIfNum, 
                                &lldpXMedRemInventoryEntryData.lldpRemLocalPortNum) != L7_SUCCESS)
    return(NULL);

  switch (nominator)
  {
  case I_lldpXMedRemHardwareRev:
    if (usmDbLldpXMedRemHardwareRevGet(intIfNum, lldpXMedRemInventoryEntryData.lldpRemIndex,
                                       lldpXMedRemInventoryEntryData.lldpRemTimeMark,
                                       snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&lldpXMedRemInventoryEntryData.lldpXMedRemHardwareRev, snmp_buffer) == L7_TRUE)
      SET_VALID(I_lldpXMedRemHardwareRev, lldpXMedRemInventoryEntryData.valid);
    if (nominator != -1) break;

  case I_lldpXMedRemFirmwareRev:
    if (usmDbLldpXMedRemFirmwareRevGet(intIfNum, lldpXMedRemInventoryEntryData.lldpRemIndex,
                                       lldpXMedRemInventoryEntryData.lldpRemTimeMark,
                                       snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&lldpXMedRemInventoryEntryData.lldpXMedRemFirmwareRev, snmp_buffer) == L7_TRUE)
      SET_VALID(I_lldpXMedRemFirmwareRev, lldpXMedRemInventoryEntryData.valid);
    if (nominator != -1) break;

  case I_lldpXMedRemSoftwareRev:
    if (usmDbLldpXMedRemSoftwareRevGet(intIfNum, lldpXMedRemInventoryEntryData.lldpRemIndex,
                                       lldpXMedRemInventoryEntryData.lldpRemTimeMark,
                                       snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&lldpXMedRemInventoryEntryData.lldpXMedRemSoftwareRev, snmp_buffer) == L7_TRUE)
      SET_VALID(I_lldpXMedRemSoftwareRev, lldpXMedRemInventoryEntryData.valid);
    if (nominator != -1) break;

  case I_lldpXMedRemSerialNum:
    if (usmDbLldpXMedRemSerialNumGet(intIfNum, lldpXMedRemInventoryEntryData.lldpRemIndex,
                                     lldpXMedRemInventoryEntryData.lldpRemTimeMark,
                                     snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&lldpXMedRemInventoryEntryData.lldpXMedRemSerialNum, snmp_buffer) == L7_TRUE)
      SET_VALID(I_lldpXMedRemSerialNum, lldpXMedRemInventoryEntryData.valid);
    if (nominator != -1) break;

  case I_lldpXMedRemMfgName:
    if (usmDbLldpXMedRemMfgNameGet(intIfNum, lldpXMedRemInventoryEntryData.lldpRemIndex,
                                   lldpXMedRemInventoryEntryData.lldpRemTimeMark,
                                   snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&lldpXMedRemInventoryEntryData.lldpXMedRemMfgName, snmp_buffer) == L7_TRUE)
      SET_VALID(I_lldpXMedRemMfgName, lldpXMedRemInventoryEntryData.valid);
    if (nominator != -1) break;

  case I_lldpXMedRemModelName:
    if (usmDbLldpXMedRemModelNameGet(intIfNum, lldpXMedRemInventoryEntryData.lldpRemIndex,
                                     lldpXMedRemInventoryEntryData.lldpRemTimeMark,
                                     snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&lldpXMedRemInventoryEntryData.lldpXMedRemModelName, snmp_buffer) == L7_TRUE)
      SET_VALID(I_lldpXMedRemModelName, lldpXMedRemInventoryEntryData.valid);
    if (nominator != -1) break;

  case I_lldpXMedRemAssetID:
    if (usmDbLldpXMedRemAssetIdGet(intIfNum, lldpXMedRemInventoryEntryData.lldpRemIndex,
                                   lldpXMedRemInventoryEntryData.lldpRemTimeMark,
                                   snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&lldpXMedRemInventoryEntryData.lldpXMedRemAssetID, snmp_buffer) == L7_TRUE)
      SET_VALID(I_lldpXMedRemAssetID, lldpXMedRemInventoryEntryData.valid);
    break;
  default:
    /* unknown nominator*/
    return(NULL);
  }

  if (nominator != -1 && !VALID(nominator, lldpXMedRemInventoryEntryData.valid))
    return(NULL);
  return(&lldpXMedRemInventoryEntryData);
}

lldpXMedRemLocationEntry_t *
k_lldpXMedRemLocationEntry_get(int serialNum, ContextInfo *contextInfo,
                               int nominator,
                               int searchType,
                               SR_UINT32 lldpRemTimeMark,
                               SR_INT32 lldpRemLocalPortNum,
                               SR_INT32 lldpRemIndex,
                               SR_INT32 lldpXMedRemLocationSubtype)
{
  static lldpXMedRemLocationEntry_t lldpXMedRemLocationEntryData;
  L7_uint32 intIfNum;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  static L7_BOOL firstTime = L7_TRUE;

  ZERO_VALID(lldpXMedRemLocationEntryData.valid);

  if (firstTime == L7_TRUE)
  {
    lldpXMedRemLocationEntryData.lldpXMedRemLocationInfo = MakeOctetString(NULL,0);
    firstTime = L7_FALSE;
  }

  ZERO_VALID(lldpXMedRemLocationEntryData.valid);

  lldpXMedRemLocationEntryData.lldpRemTimeMark = lldpRemTimeMark;
  SET_VALID(I_lldpXMedRemLocationEntryIndex_lldpRemTimeMark, lldpXMedRemLocationEntryData.valid);
  lldpXMedRemLocationEntryData.lldpRemLocalPortNum = lldpRemLocalPortNum;
  SET_VALID(I_lldpXMedRemLocationEntryIndex_lldpRemLocalPortNum, lldpXMedRemLocationEntryData.valid);
  lldpXMedRemLocationEntryData.lldpRemIndex = lldpRemIndex;
  SET_VALID(I_lldpXMedRemLocationEntryIndex_lldpRemIndex, lldpXMedRemLocationEntryData.valid);
  lldpXMedRemLocationEntryData.lldpXMedRemLocationSubtype = lldpXMedRemLocationSubtype;
  SET_VALID(I_lldpXMedRemLocationSubtype, lldpXMedRemLocationEntryData.valid);

  if (usmDbIntIfNumFromExtIfNum(lldpXMedRemLocationEntryData.lldpRemLocalPortNum,
                                &intIfNum) != L7_SUCCESS)
    return(NULL);

  if (searchType == EXACT ? 
      (snmpLldpXMedRemLocationEntryGet(lldpXMedRemLocationEntryData.lldpRemTimeMark, intIfNum,
                                       lldpXMedRemLocationEntryData.lldpRemIndex,
                                       lldpXMedRemLocationEntryData.lldpXMedRemLocationSubtype, snmp_buffer) != L7_SUCCESS) :
      ((snmpLldpXMedRemLocationEntryGet(lldpXMedRemLocationEntryData.lldpRemTimeMark, intIfNum,
                                        lldpXMedRemLocationEntryData.lldpRemIndex,
                                        lldpXMedRemLocationEntryData.lldpXMedRemLocationSubtype, snmp_buffer) != L7_SUCCESS) &&
       (snmpLldpXMedRemLocationEntryNextGet(&lldpXMedRemLocationEntryData.lldpRemTimeMark, &intIfNum,
                                            &lldpXMedRemLocationEntryData.lldpRemIndex,
                                            &lldpXMedRemLocationEntryData.lldpXMedRemLocationSubtype, snmp_buffer) != L7_SUCCESS)))
    return(NULL);

  if (usmDbExtIfNumFromIntIfNum(intIfNum, 
                                &lldpXMedRemLocationEntryData.lldpRemLocalPortNum) != L7_SUCCESS)
    return(NULL);

  switch (nominator)
  {
  case -1:
  case I_lldpXMedRemLocationEntryIndex_lldpRemTimeMark:
  case I_lldpXMedRemLocationEntryIndex_lldpRemLocalPortNum:
  case I_lldpXMedRemLocationEntryIndex_lldpRemIndex:
  case I_lldpXMedRemLocationSubtype:
    if (nominator != -1) break;

  case I_lldpXMedRemLocationInfo:
    if (SafeMakeOctetStringFromText(&lldpXMedRemLocationEntryData.lldpXMedRemLocationInfo,
                                    snmp_buffer) == L7_TRUE)
      SET_VALID(I_lldpXMedRemLocationInfo, lldpXMedRemLocationEntryData.valid);
    break;
  default:
    /* unknown nominator */
    return(NULL);
  }

  if (nominator != -1 && !VALID(nominator, lldpXMedRemLocationEntryData.valid))
    return(NULL);
  return(&lldpXMedRemLocationEntryData);
}

lldpXMedRemXPoEEntry_t *
k_lldpXMedRemXPoEEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_UINT32 lldpRemTimeMark,
                           SR_INT32 lldpRemLocalPortNum,
                           SR_INT32 lldpRemIndex)
{
  static lldpXMedRemXPoEEntry_t lldpXMedRemXPoEEntryData;
  L7_uint32 intIfNum;

  ZERO_VALID(lldpXMedRemXPoEEntryData.valid);

  lldpXMedRemXPoEEntryData.lldpRemTimeMark = lldpRemTimeMark;
  SET_VALID(I_lldpXMedRemXPoEEntryIndex_lldpRemTimeMark, lldpXMedRemXPoEEntryData.valid);

  lldpXMedRemXPoEEntryData.lldpRemLocalPortNum = lldpRemLocalPortNum;
  SET_VALID(I_lldpXMedRemXPoEEntryIndex_lldpRemLocalPortNum, lldpXMedRemXPoEEntryData.valid);

  lldpXMedRemXPoEEntryData.lldpRemIndex = lldpRemIndex;
  SET_VALID(I_lldpXMedRemXPoEEntryIndex_lldpRemIndex, lldpXMedRemXPoEEntryData.valid);

  if (usmDbIntIfNumFromExtIfNum(lldpXMedRemXPoEEntryData.lldpRemLocalPortNum, 
                                &intIfNum) != L7_SUCCESS)
    return(NULL);

  if ((searchType == EXACT) ?
      (snmpLldpRemEntryGet(lldpXMedRemXPoEEntryData.lldpRemTimeMark, intIfNum,
                           lldpXMedRemXPoEEntryData.lldpRemIndex) != L7_SUCCESS) :
      ((snmpLldpRemEntryGet(lldpXMedRemXPoEEntryData.lldpRemTimeMark, intIfNum,
                            lldpXMedRemXPoEEntryData.lldpRemIndex) != L7_SUCCESS) &&
       (snmpLldpRemEntryGetNext(&lldpXMedRemXPoEEntryData.lldpRemTimeMark, &intIfNum,
                                &lldpXMedRemXPoEEntryData.lldpRemIndex) != L7_SUCCESS)))
    return(NULL);

  if (usmDbExtIfNumFromIntIfNum(intIfNum, 
                                &lldpXMedRemXPoEEntryData.lldpRemLocalPortNum) != L7_SUCCESS)
    return(NULL);

  switch (nominator)
  {
  case I_lldpXMedRemXPoEDeviceType:
    if (snmpLldpXMedRemXPoEDeviceTypeGet(intIfNum, lldpXMedRemXPoEEntryData.lldpRemIndex,
                                         lldpXMedRemXPoEEntryData.lldpRemTimeMark,
                                         &lldpXMedRemXPoEEntryData.lldpXMedRemXPoEDeviceType) == L7_SUCCESS)
      SET_VALID(I_lldpXMedRemXPoEDeviceType, lldpXMedRemXPoEEntryData.valid);
    break;
  default:
    /* unknown nominator*/
    return(NULL);
  }

  if (nominator != -1 && !VALID(nominator, lldpXMedRemXPoEEntryData.valid))
    return(NULL);
  return(&lldpXMedRemXPoEEntryData);
}

lldpXMedRemXPoEPSEEntry_t *
k_lldpXMedRemXPoEPSEEntry_get(int serialNum, ContextInfo *contextInfo,
                              int nominator,
                              int searchType,
                              SR_UINT32 lldpRemTimeMark,
                              SR_INT32 lldpRemLocalPortNum,
                              SR_INT32 lldpRemIndex)
{
  static lldpXMedRemXPoEPSEEntry_t lldpXMedRemXPoEPSEEntryData;
  L7_uint32 intIfNum;

  ZERO_VALID(lldpXMedRemXPoEPSEEntryData.valid);

  lldpXMedRemXPoEPSEEntryData.lldpRemTimeMark = lldpRemTimeMark;
  SET_VALID(I_lldpXMedRemXPoEPSEEntryIndex_lldpRemTimeMark, lldpXMedRemXPoEPSEEntryData.valid);

  lldpXMedRemXPoEPSEEntryData.lldpRemLocalPortNum = lldpRemLocalPortNum;
  SET_VALID(I_lldpXMedRemXPoEPSEEntryIndex_lldpRemLocalPortNum, lldpXMedRemXPoEPSEEntryData.valid);

  lldpXMedRemXPoEPSEEntryData.lldpRemIndex = lldpRemIndex;
  SET_VALID(I_lldpXMedRemXPoEPSEEntryIndex_lldpRemIndex, lldpXMedRemXPoEPSEEntryData.valid);

  if (usmDbIntIfNumFromExtIfNum(lldpXMedRemXPoEPSEEntryData.lldpRemLocalPortNum, 
                                &intIfNum) != L7_SUCCESS)
    return(NULL);

  if ((searchType == EXACT) ?
      (snmpLldpRemEntryGet(lldpXMedRemXPoEPSEEntryData.lldpRemTimeMark, intIfNum,
                           lldpXMedRemXPoEPSEEntryData.lldpRemIndex) != L7_SUCCESS) :
      ((snmpLldpRemEntryGet(lldpXMedRemXPoEPSEEntryData.lldpRemTimeMark, intIfNum,
                            lldpXMedRemXPoEPSEEntryData.lldpRemIndex) != L7_SUCCESS) &&
       (snmpLldpRemEntryGetNext(&lldpXMedRemXPoEPSEEntryData.lldpRemTimeMark, &intIfNum,
                                &lldpXMedRemXPoEPSEEntryData.lldpRemIndex) != L7_SUCCESS)))
    return(NULL);

  if (usmDbExtIfNumFromIntIfNum(intIfNum, 
                                &lldpXMedRemXPoEPSEEntryData.lldpRemLocalPortNum) != L7_SUCCESS)
    return(NULL);

  switch (nominator)
  {
  case I_lldpXMedRemXPoEPSEPowerAv:
    if (usmDbLldpXMedRemXPoePSEPowerAvGet(intIfNum, lldpXMedRemXPoEPSEEntryData.lldpRemIndex,
                                          lldpXMedRemXPoEPSEEntryData.lldpRemTimeMark,
                                          &lldpXMedRemXPoEPSEEntryData.lldpXMedRemXPoEPSEPowerAv) == L7_SUCCESS)
      SET_VALID(I_lldpXMedRemXPoEPSEPowerAv, lldpXMedRemXPoEPSEEntryData.valid);
    if (nominator != -1) break;
  case I_lldpXMedRemXPoEPSEPowerSource:
    if (snmpLldpXMedRemXPoEPSEPowerSourceGet(intIfNum, lldpXMedRemXPoEPSEEntryData.lldpRemIndex,
                                             lldpXMedRemXPoEPSEEntryData.lldpRemTimeMark,
                                             &lldpXMedRemXPoEPSEEntryData.lldpXMedRemXPoEPSEPowerSource) == L7_SUCCESS)
      SET_VALID(I_lldpXMedRemXPoEPSEPowerSource, lldpXMedRemXPoEPSEEntryData.valid);
    if (nominator != -1) break;
  case I_lldpXMedRemXPoEPSEPowerPriority:
    if (snmpLldpXMedRemXPoEPSEPowerPriorityGet(intIfNum, lldpXMedRemXPoEPSEEntryData.lldpRemIndex,
                                               lldpXMedRemXPoEPSEEntryData.lldpRemTimeMark,
                                               &lldpXMedRemXPoEPSEEntryData.lldpXMedRemXPoEPSEPowerPriority) == L7_SUCCESS)
      SET_VALID(I_lldpXMedRemXPoEPSEPowerPriority, lldpXMedRemXPoEPSEEntryData.valid);
    break;
  default:
    /* unknown nominator*/
    return(NULL);
  }

  if (nominator != -1 && !VALID(nominator, lldpXMedRemXPoEPSEEntryData.valid))
    return(NULL);
  return(&lldpXMedRemXPoEPSEEntryData);
}

lldpXMedRemXPoEPDEntry_t *
k_lldpXMedRemXPoEPDEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_UINT32 lldpRemTimeMark,
                             SR_INT32 lldpRemLocalPortNum,
                             SR_INT32 lldpRemIndex)
{
  static lldpXMedRemXPoEPDEntry_t lldpXMedRemXPoEPDEntryData;
  L7_uint32 intIfNum;

  ZERO_VALID(lldpXMedRemXPoEPDEntryData.valid);

  lldpXMedRemXPoEPDEntryData.lldpRemTimeMark = lldpRemTimeMark;
  SET_VALID(I_lldpXMedRemXPoEPDEntryIndex_lldpRemTimeMark, lldpXMedRemXPoEPDEntryData.valid);

  lldpXMedRemXPoEPDEntryData.lldpRemLocalPortNum = lldpRemLocalPortNum;
  SET_VALID(I_lldpXMedRemXPoEPDEntryIndex_lldpRemLocalPortNum, lldpXMedRemXPoEPDEntryData.valid);

  lldpXMedRemXPoEPDEntryData.lldpRemIndex = lldpRemIndex;
  SET_VALID(I_lldpXMedRemXPoEPDEntryIndex_lldpRemIndex, lldpXMedRemXPoEPDEntryData.valid);

  if (usmDbIntIfNumFromExtIfNum(lldpXMedRemXPoEPDEntryData.lldpRemLocalPortNum, 
                                &intIfNum) != L7_SUCCESS)
    return(NULL);

  if ((searchType == EXACT) ?
      (snmpLldpRemEntryGet(lldpXMedRemXPoEPDEntryData.lldpRemTimeMark, intIfNum,
                           lldpXMedRemXPoEPDEntryData.lldpRemIndex) != L7_SUCCESS) :
      ((snmpLldpRemEntryGet(lldpXMedRemXPoEPDEntryData.lldpRemTimeMark, intIfNum,
                            lldpXMedRemXPoEPDEntryData.lldpRemIndex) != L7_SUCCESS) &&
       (snmpLldpRemEntryGetNext(&lldpXMedRemXPoEPDEntryData.lldpRemTimeMark, &intIfNum,
                                &lldpXMedRemXPoEPDEntryData.lldpRemIndex) != L7_SUCCESS)))
    return(NULL);

  if (usmDbExtIfNumFromIntIfNum(intIfNum, 
                                &lldpXMedRemXPoEPDEntryData.lldpRemLocalPortNum) != L7_SUCCESS)
    return(NULL);

  switch (nominator)
  {
  case I_lldpXMedRemXPoEPDPowerReq:
    if (usmDbLldpXMedRemXPoePDPowerReqGet(intIfNum, lldpXMedRemXPoEPDEntryData.lldpRemIndex,
                                         lldpXMedRemXPoEPDEntryData.lldpRemTimeMark,
                                         &lldpXMedRemXPoEPDEntryData.lldpXMedRemXPoEPDPowerReq) == L7_SUCCESS)
      SET_VALID(I_lldpXMedRemXPoEPDPowerReq, lldpXMedRemXPoEPDEntryData.valid);
    if (nominator != -1) break;
  case I_lldpXMedRemXPoEPDPowerSource:
    if (snmpLldpXMedRemXPoEPDPowerSourceGet(intIfNum, lldpXMedRemXPoEPDEntryData.lldpRemIndex,
                                            lldpXMedRemXPoEPDEntryData.lldpRemTimeMark,
                                            &lldpXMedRemXPoEPDEntryData.lldpXMedRemXPoEPDPowerSource) == L7_SUCCESS)
      SET_VALID(I_lldpXMedRemXPoEPDPowerSource, lldpXMedRemXPoEPDEntryData.valid);
    if (nominator != -1) break;
  case I_lldpXMedRemXPoEPDPowerPriority:
    if (snmpLldpXMedRemXPoEPDPowerPriorityGet(intIfNum, lldpXMedRemXPoEPDEntryData.lldpRemIndex,
                                              lldpXMedRemXPoEPDEntryData.lldpRemTimeMark,
                                              &lldpXMedRemXPoEPDEntryData.lldpXMedRemXPoEPDPowerPriority) == L7_SUCCESS)
      SET_VALID(I_lldpXMedRemXPoEPDPowerPriority, lldpXMedRemXPoEPDEntryData.valid);
    break;
  default:
    /* unknown nominator*/
    return(NULL);
  }

  if (nominator != -1 && !VALID(nominator, lldpXMedRemXPoEPDEntryData.valid))
    return(NULL);
  return(&lldpXMedRemXPoEPDEntryData);
}

