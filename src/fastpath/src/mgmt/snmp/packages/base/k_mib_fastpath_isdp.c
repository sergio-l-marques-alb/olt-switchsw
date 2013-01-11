/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
*
* @filename   k_mib_fastpathisdp.c
*
* @purpose    Code to support the ISDP protocol
*
* @component  SNMP
*
* @comments
*
* @create     12/10/2007
*
* @author     Rostyslav Ivasiv
* @end
*
**********************************************************************/
#include "snmpapi.h"
#include "k_private_base.h"
#include "usmdb_isdp_api.h"
#include "registry.h"
#include "commdefs.h"
#include "compdefs.h"
#include "k_mib_fastpath_isdp_api.h"
#include "usmdb_util_api.h"
#include "local.h"
#include "usmdb_common.h"
#include "usmdb_2233_stats_api.h"   /* for usmDbIfNameGet(), oddly enough */

agentIsdpGlobal_t * k_agentIsdpGlobal_get(int serialNum,
                                        ContextInfo *contextInfo, int nominator)
{
  static agentIsdpGlobal_t agentIsdpGlobalGroupData;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  static L7_BOOL firstTime = L7_TRUE;

  ZERO_VALID(agentIsdpGlobalGroupData.valid);
  if (firstTime == L7_TRUE)
  {
    agentIsdpGlobalGroupData.agentIsdpGlobalDeviceIdFormatCpb = MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }

  switch(nominator)
  {
  case -1:
  case I_agentIsdpGlobalRun :
    CLR_VALID(I_agentIsdpGlobalRun, agentIsdpGlobalGroupData.valid);
    if (L7_SUCCESS == usmdbIsdpModeGet(
                                &agentIsdpGlobalGroupData.agentIsdpGlobalRun))
      SET_VALID(I_agentIsdpGlobalRun, agentIsdpGlobalGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentIsdpGlobalMessageInterval :
    CLR_VALID(I_agentIsdpGlobalMessageInterval, agentIsdpGlobalGroupData.valid);
    if (L7_SUCCESS == usmdbIsdpTimerGet(
                      &agentIsdpGlobalGroupData.agentIsdpGlobalMessageInterval))
      SET_VALID(I_agentIsdpGlobalMessageInterval,
                                                agentIsdpGlobalGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentIsdpGlobalHoldTime :
    CLR_VALID(I_agentIsdpGlobalHoldTime, agentIsdpGlobalGroupData.valid);
    if (L7_SUCCESS == usmdbIsdpHoldTimeGet(
                           &agentIsdpGlobalGroupData.agentIsdpGlobalHoldTime))
      SET_VALID(I_agentIsdpGlobalHoldTime, agentIsdpGlobalGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentIsdpGlobalDeviceIdFormatCpb :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((L7_SUCCESS == snmpIsdpDeviceIdFormatCapabilityGet(snmp_buffer)) &&
       (SafeMakeOctetString(&agentIsdpGlobalGroupData.agentIsdpGlobalDeviceIdFormatCpb, snmp_buffer, 1) == L7_TRUE))
      SET_VALID(I_agentIsdpGlobalDeviceIdFormatCpb, agentIsdpGlobalGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentIsdpGlobalDeviceIdFormat :
    CLR_VALID(I_agentIsdpGlobalDeviceIdFormat, agentIsdpGlobalGroupData.valid);
    if (L7_SUCCESS == usmdbIsdpDeviceIdFormatGet(
                       &agentIsdpGlobalGroupData.agentIsdpGlobalDeviceIdFormat))
      SET_VALID(I_agentIsdpGlobalDeviceIdFormat, agentIsdpGlobalGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentIsdpGlobalDeviceId :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if((usmDbIsdpDeviceIdGet(snmp_buffer) == L7_SUCCESS) &&
       (SafeMakeOctetStringFromText(
         &agentIsdpGlobalGroupData.agentIsdpGlobalDeviceId, snmp_buffer)
                                                                    == L7_TRUE))
      SET_VALID(I_agentIsdpGlobalDeviceId, agentIsdpGlobalGroupData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentIsdpGlobalAdvertiseV2 :
    CLR_VALID(I_agentIsdpGlobalAdvertiseV2, agentIsdpGlobalGroupData.valid);
    if (L7_SUCCESS == usmdbIsdpV2ModeGet(
                          &agentIsdpGlobalGroupData.agentIsdpGlobalAdvertiseV2))
      SET_VALID(I_agentIsdpGlobalAdvertiseV2, agentIsdpGlobalGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentIsdpGlobalGroupData.valid))
    return(NULL);

  return(&agentIsdpGlobalGroupData);
}

int k_agentIsdpGlobal_test(ObjectInfo *object, ObjectSyntax *value,
                                        doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int k_agentIsdpGlobal_ready(ObjectInfo *object, ObjectSyntax *value,
                                                doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY; /* State indicates that the set method
                              should be called to accomplish the set oeration.*/
  return NO_ERROR;
}

int k_agentIsdpGlobal_set(agentIsdpGlobal_t *data,
                                         ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when
   * the set request is only partially successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  L7_RC_t rc;
  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_agentIsdpGlobalRun, data->valid))
  {
    if(data->agentIsdpGlobalRun == 0)
    {
      rc = usmdbIsdpModeSet(L7_DISABLE);
    }
    else
    {
      rc = usmdbIsdpModeSet(L7_ENABLE);
    }
    if( rc != L7_SUCCESS)
    {
    memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
    SET_VALID(I_agentIsdpGlobalRun, tempValid);
    }
  }
  if (VALID(I_agentIsdpGlobalMessageInterval, data->valid))
  {
    if(usmdbIsdpTimerSet(data->agentIsdpGlobalMessageInterval) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentIsdpGlobalMessageInterval, tempValid);
    }
  }
  if (VALID(I_agentIsdpGlobalHoldTime, data->valid))
  {
    if(usmdbIsdpHoldTimeSet(data->agentIsdpGlobalHoldTime) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentIsdpGlobalHoldTime, tempValid);
    }
  }
  if (VALID(I_agentIsdpGlobalAdvertiseV2, data->valid))
  {
    if(data->agentIsdpGlobalAdvertiseV2 == 0)
    {
      rc = usmdbIsdpV2ModeSet(L7_DISABLE);
    }
    else
    {
      rc = usmdbIsdpV2ModeSet(L7_ENABLE);
    }
    if(rc != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentIsdpGlobalAdvertiseV2, tempValid);
    }
  }

  return NO_ERROR;
}

agentIsdpClear_t * k_agentIsdpClear_get(int serialNum,
                                        ContextInfo *contextInfo, int nominator)
{
  static agentIsdpClear_t agentIsdpClearData;

  ZERO_VALID(agentIsdpClearData.valid);

  switch (nominator)
  {
  case -1:
  case I_agentIsdpClearStats :
    CLR_VALID(I_agentIsdpClearStats, agentIsdpClearData.valid);
    agentIsdpClearData.agentIsdpClearStats = 0;
      SET_VALID(I_agentIsdpClearStats, agentIsdpClearData.valid);
    if (nominator != -1) break;


  case I_agentIsdpClearEntries :
    CLR_VALID(I_agentIsdpClearEntries, agentIsdpClearData.valid);
    agentIsdpClearData.agentIsdpClearEntries = 0;
      SET_VALID(I_agentIsdpClearEntries, agentIsdpClearData.valid);
    break;

  default:

    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentIsdpClearData.valid) )
    return(NULL);

  return(&agentIsdpClearData);
}

int k_agentIsdpClear_set(agentIsdpClear_t *data,
                                         ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when
   * the set request is only partially successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_agentIsdpClearStats, data->valid))
  {
    if(usmdbIsdpTrafficCountersClear() != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
    SET_VALID(I_agentIsdpClearStats, tempValid);
    }
  }
  if (VALID(I_agentIsdpClearEntries, data->valid))
  {
    if(usmdbIsdpNeighborTableClear() != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentIsdpClearEntries, tempValid);
    }
  }

  return NO_ERROR;
}
int k_agentIsdpClear_test(ObjectInfo *object, ObjectSyntax *value,
                                        doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int k_agentIsdpClear_ready(ObjectInfo *object, ObjectSyntax *value,
                                                doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY; /* State indicates that the set method
                              should be called to accomplish the set oeration.*/
  return NO_ERROR;
}

agentIsdpStatistics_t * k_agentIsdpStatistics_get(int serialNum,
                                        ContextInfo *contextInfo, int nominator)
{
  static agentIsdpStatistics_t agentIsdpStatisticsData;
  L7_uint32 mode;

  ZERO_VALID(agentIsdpStatisticsData.valid);

  if ((usmdbIsdpModeGet(&mode) != L7_SUCCESS) || (mode != L7_ENABLE))
  {
    return (NULL);
  }
    switch (nominator)
    {
    case -1:
    case I_agentIsdpStatisticsPduReceived :
    CLR_VALID(I_agentIsdpStatisticsPduReceived, agentIsdpStatisticsData.valid);
    agentIsdpStatisticsData.agentIsdpStatisticsPduReceived =
                   usmdbIsdpTrafficPduReceivedGet();
    SET_VALID(I_agentIsdpStatisticsPduReceived, agentIsdpStatisticsData.valid);
    if (nominator != -1) break;
    /* else pass through */

    case I_agentIsdpStatisticsPduTransmit :
    CLR_VALID(I_agentIsdpStatisticsPduTransmit, agentIsdpStatisticsData.valid);
    agentIsdpStatisticsData.agentIsdpStatisticsPduTransmit =
                   usmdbIsdpTrafficPduTransmitGet();
    SET_VALID(I_agentIsdpStatisticsPduTransmit, agentIsdpStatisticsData.valid);
    if (nominator != -1) break;
    /* else pass through */

    case I_agentIsdpStatisticsV1PduReceived :
    CLR_VALID(I_agentIsdpStatisticsV1PduReceived, agentIsdpStatisticsData.valid);
    agentIsdpStatisticsData.agentIsdpStatisticsV1PduReceived =
                   usmdbIsdpTrafficV1PduReceivedGet();
    SET_VALID(I_agentIsdpStatisticsV1PduReceived, agentIsdpStatisticsData.valid);
    if (nominator != -1) break;
    /* else pass through */

    case I_agentIsdpStatisticsV1PduTransmit :
    CLR_VALID(I_agentIsdpStatisticsV1PduTransmit, agentIsdpStatisticsData.valid);
    agentIsdpStatisticsData.agentIsdpStatisticsV1PduTransmit =
               usmdbIsdpTrafficV1PduTransmitGet();
    SET_VALID(I_agentIsdpStatisticsV1PduTransmit, agentIsdpStatisticsData.valid);
    if (nominator != -1) break;
    /* else pass through */

    case I_agentIsdpStatisticsV2PduReceived :
    CLR_VALID(I_agentIsdpStatisticsV2PduReceived, agentIsdpStatisticsData.valid);
    agentIsdpStatisticsData.agentIsdpStatisticsV2PduReceived =
               usmdbIsdpTrafficV2PduReceivedGet();
    SET_VALID(I_agentIsdpStatisticsV2PduReceived, agentIsdpStatisticsData.valid);
    if (nominator != -1) break;
    /* else pass through */

    case I_agentIsdpStatisticsV2PduTransmit :
    CLR_VALID(I_agentIsdpStatisticsV2PduTransmit, agentIsdpStatisticsData.valid);
    agentIsdpStatisticsData.agentIsdpStatisticsV2PduTransmit =
               usmdbIsdpTrafficV2PduTransmitGet();
    SET_VALID(I_agentIsdpStatisticsV2PduTransmit, agentIsdpStatisticsData.valid);
    if (nominator != -1) break;
    /* else pass through */

    case I_agentIsdpStatisticsBadHeaderPduReceived :
    CLR_VALID(I_agentIsdpStatisticsBadHeaderPduReceived, agentIsdpStatisticsData.valid);
    agentIsdpStatisticsData.agentIsdpStatisticsBadHeaderPduReceived =
              usmdbIsdpTrafficBadHeaderPduReceivedGet();
    SET_VALID(I_agentIsdpStatisticsBadHeaderPduReceived, agentIsdpStatisticsData.valid);
    if (nominator != -1) break;
    /* else pass through */

    case I_agentIsdpStatisticsChkSumErrorPduReceived :
    CLR_VALID(I_agentIsdpStatisticsChkSumErrorPduReceived, agentIsdpStatisticsData.valid);
    agentIsdpStatisticsData.agentIsdpStatisticsChkSumErrorPduReceived =
                    usmdbIsdpTrafficChkSumErrorPduReceivedGet();
    SET_VALID(I_agentIsdpStatisticsChkSumErrorPduReceived, agentIsdpStatisticsData.valid);
    if (nominator != -1) break;
    /* else pass through */

    case I_agentIsdpStatisticsFailurePduTransmit :
    CLR_VALID(I_agentIsdpStatisticsFailurePduTransmit, agentIsdpStatisticsData.valid);
    agentIsdpStatisticsData.agentIsdpStatisticsFailurePduTransmit =
                      usmdbIsdpTrafficFailurePduTransmitGet();
    SET_VALID(I_agentIsdpStatisticsFailurePduTransmit, agentIsdpStatisticsData.valid);
    if (nominator != -1) break;
    /* else pass through */

    case I_agentIsdpStatisticsInvalidFormatPduReceived :
    CLR_VALID(I_agentIsdpStatisticsInvalidFormatPduReceived, agentIsdpStatisticsData.valid);
    agentIsdpStatisticsData.agentIsdpStatisticsInvalidFormatPduReceived =
             usmdbIsdpTrafficInvalidFormatPduReceivedGet();
    SET_VALID(I_agentIsdpStatisticsInvalidFormatPduReceived, agentIsdpStatisticsData.valid);
    if (nominator != -1) break;
    /* else pass through */

    case I_agentIsdpStatisticsTableFull :
    CLR_VALID(I_agentIsdpStatisticsTableFull, agentIsdpStatisticsData.valid);
    agentIsdpStatisticsData.agentIsdpStatisticsTableFull =
                  usmdbIsdpTrafficTableFullGet();
    SET_VALID(I_agentIsdpStatisticsTableFull, agentIsdpStatisticsData.valid);
    if (nominator != -1) break;
    /* else pass through */

    case I_agentIsdpStatisticsIpAddressTableFull :
    CLR_VALID(I_agentIsdpStatisticsTableFull, agentIsdpStatisticsData.valid);
    agentIsdpStatisticsData.agentIsdpStatisticsTableFull =
                  usmdbIsdpTrafficIpAddressTableFullGet();
    SET_VALID(I_agentIsdpStatisticsIpAddressTableFull, agentIsdpStatisticsData.valid);
    break;

    default:
    /* unknown nominator */
    return(NULL);
    break;
    }

    if (nominator >= 0 && !VALID(nominator, agentIsdpStatisticsData.valid))
    return(NULL);

    return(&agentIsdpStatisticsData);
}

agentIsdpInterfaceEntry_t *k_agentIsdpInterfaceEntry_get(int serialNum,
                        ContextInfo *contextInfo, int nominator, int searchType,
                                             SR_INT32 agentIsdpInterfaceIfIndex)
{
  static agentIsdpInterfaceEntry_t agentIsdpInterfaceEntryData;
  L7_uint32 intIfNum;
  L7_uint32 mode;

  if ((usmdbIsdpModeGet(&mode) != L7_SUCCESS) || (mode != L7_ENABLE))
  {
    return (NULL);
  }
  ZERO_VALID(agentIsdpInterfaceEntryData.valid);
  agentIsdpInterfaceEntryData.agentIsdpInterfaceIfIndex = agentIsdpInterfaceIfIndex;
  SET_VALID(I_agentIsdpInterfaceIfIndex, agentIsdpInterfaceEntryData.valid);

  if ( (searchType == EXACT) ?
       (snmpAgentIsdpTableEntryGet(agentIsdpInterfaceEntryData.agentIsdpInterfaceIfIndex, &intIfNum ) != L7_SUCCESS) :
       ((snmpAgentIsdpTableEntryGet(agentIsdpInterfaceEntryData.agentIsdpInterfaceIfIndex, &intIfNum ) != L7_SUCCESS) &&
         (snmpAgentIsdpTableEntryNextGet(&agentIsdpInterfaceEntryData.agentIsdpInterfaceIfIndex, &intIfNum ) != L7_SUCCESS) ) )
  {
    ZERO_VALID(agentIsdpInterfaceEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:

  case I_agentIsdpInterfaceIfIndex:
    break;
    /* else pass through */

  case I_agentIsdpInterfaceEnable:
    CLR_VALID(I_agentIsdpInterfaceEnable, agentIsdpInterfaceEntryData.valid);
    if (usmdbIsdpIntfModeGet(intIfNum,
           &agentIsdpInterfaceEntryData.agentIsdpInterfaceEnable) == L7_SUCCESS)
      SET_VALID(I_agentIsdpInterfaceEnable, agentIsdpInterfaceEntryData.valid);
    break;

  default:
    /* unknown nominator value */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentIsdpInterfaceEntryData.valid))
    return(NULL);

  return(&agentIsdpInterfaceEntryData);
}

int k_agentIsdpInterfaceEntry_ready(ObjectInfo *object, ObjectSyntax *value,
        doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY; /* State indicates that the set method
  should be called to accomplish the set operation.*/
  return NO_ERROR;
}

int k_agentIsdpInterfaceEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                        doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int k_agentIsdpInterfaceEntry_set(agentIsdpInterfaceEntry_t *data,
                  ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;

  if (usmDbIntIfNumFromExtIfNum(data->agentIsdpInterfaceIfIndex, &intIfNum) == L7_SUCCESS)
  {
    if (VALID(I_agentIsdpInterfaceEnable, data->valid))
    {
      if(usmdbIsdpIntfModeSet(intIfNum, data->agentIsdpInterfaceEnable) != L7_SUCCESS)
      {
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_agentIsdpInterfaceEnable, data->valid);
      }
    }
  }
  return NO_ERROR;
}

agentIsdpCacheEntry_t *k_agentIsdpCacheEntry_get(int serialNum,
                                ContextInfo *contextInfo, int nominator,
                                int searchType, SR_INT32 agentIsdpCacheIfIndex,
                                SR_INT32 agentIsdpCacheIndex)
{
  static agentIsdpCacheEntry_t agentIsdpCacheEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uint32 mode;
  L7_char8 deviceId[L7_ISDP_DEVICE_ID_LEN];
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];  

  memset(deviceId, 0x00, sizeof(deviceId));

  ZERO_VALID(agentIsdpCacheEntryData.valid);

  if ( (usmdbIsdpModeGet(&mode) != L7_SUCCESS) || (mode != L7_ENABLE) )
  {
    return (NULL);
  }

 if (firstTime == L7_TRUE) 
  {
      firstTime = L7_FALSE;

      agentIsdpCacheEntryData.agentIsdpCacheAddress = MakeOctetString(NULL, 0);
      agentIsdpCacheEntryData.agentIsdpCacheLocalIntf = MakeOctetString(NULL, 0);
      agentIsdpCacheEntryData.agentIsdpCacheVersion = MakeOctetString(NULL, 0);
      agentIsdpCacheEntryData.agentIsdpCacheDeviceId = MakeOctetString(NULL, 0);
      agentIsdpCacheEntryData.agentIsdpCacheDevicePort = MakeOctetString(NULL, 0);
      agentIsdpCacheEntryData.agentIsdpCachePlatform = MakeOctetString(NULL, 0);
      agentIsdpCacheEntryData.agentIsdpCacheCapabilities = MakeOctetString(NULL, 0);
  }

  agentIsdpCacheEntryData.agentIsdpCacheIfIndex = agentIsdpCacheIfIndex;
  SET_VALID(I_agentIsdpCacheIfIndex, agentIsdpCacheEntryData.valid);
  agentIsdpCacheEntryData.agentIsdpCacheIndex = agentIsdpCacheIndex;
  SET_VALID(I_agentIsdpCacheIndex, agentIsdpCacheEntryData.valid);

  if ( (searchType == EXACT) ?
     (usmdbIsdpNeighborIndexedGet(agentIsdpCacheEntryData.agentIsdpCacheIfIndex,
                                  agentIsdpCacheEntryData.agentIsdpCacheIndex, 
                                  deviceId) != L7_SUCCESS) :
     ((usmdbIsdpNeighborIndexedGet(agentIsdpCacheEntryData.agentIsdpCacheIfIndex,
                                  agentIsdpCacheEntryData.agentIsdpCacheIndex, 
                                  deviceId) != L7_SUCCESS) &&
     (usmdbIsdpNeighborIndexedGetNext(agentIsdpCacheEntryData.agentIsdpCacheIfIndex,
                                       agentIsdpCacheEntryData.agentIsdpCacheIndex,
                                       &agentIsdpCacheEntryData.agentIsdpCacheIfIndex, 
                                       &agentIsdpCacheEntryData.agentIsdpCacheIndex, 
                                       deviceId) != L7_SUCCESS))) 
    {
      ZERO_VALID(agentIsdpCacheEntryData.valid);
      return(NULL);
    }

  switch (nominator) {
  case -1:
  case I_agentIsdpCacheIfIndex :
    SET_VALID(I_agentIsdpCacheIfIndex, agentIsdpCacheEntryData.valid);
    if (nominator != -1) break;

  case I_agentIsdpCacheIndex :
    SET_VALID(I_agentIsdpCacheIndex, agentIsdpCacheEntryData.valid);
    if (nominator != -1) break;

  case I_agentIsdpCacheAddress :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((snmpAgentIsdpAddresseGet(agentIsdpCacheEntryData.agentIsdpCacheIfIndex, 
                                  deviceId, snmp_buffer) == L7_SUCCESS) &&
        (SafeMakeOctetStringFromText(&agentIsdpCacheEntryData.agentIsdpCacheAddress,
                                     snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentIsdpCacheAddress, agentIsdpCacheEntryData.valid);
    if (nominator != -1) break;

  case I_agentIsdpCacheLocalIntf :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((usmDbIfNameGet(USMDB_UNIT_CURRENT, agentIsdpCacheEntryData.agentIsdpCacheIfIndex, snmp_buffer) == L7_SUCCESS) &&
        (SafeMakeOctetStringFromText(&agentIsdpCacheEntryData.agentIsdpCacheLocalIntf, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentIsdpCacheLocalIntf, agentIsdpCacheEntryData.valid);
    if (nominator != -1) break;

  case I_agentIsdpCacheVersion :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((usmdbIsdpIntfNeighborVersionGet(agentIsdpCacheEntryData.agentIsdpCacheIfIndex, deviceId, snmp_buffer) == L7_SUCCESS) &&
        (SafeMakeOctetStringFromText(&agentIsdpCacheEntryData.agentIsdpCacheVersion, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentIsdpCacheVersion, agentIsdpCacheEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentIsdpCacheDeviceId :
    if (SafeMakeOctetStringFromText(&agentIsdpCacheEntryData.agentIsdpCacheDeviceId, deviceId) == L7_TRUE)
      SET_VALID(I_agentIsdpCacheDeviceId, agentIsdpCacheEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentIsdpCacheDevicePort :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((usmdbIsdpIntfNeighborPortIdGet(agentIsdpCacheEntryData.agentIsdpCacheIfIndex, deviceId, snmp_buffer) == L7_SUCCESS) &&
        (SafeMakeOctetStringFromText(&agentIsdpCacheEntryData.agentIsdpCacheDevicePort, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentIsdpCacheDevicePort, agentIsdpCacheEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentIsdpCachePlatform :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((usmdbIsdpIntfNeighborPlatformGet(agentIsdpCacheEntryData.agentIsdpCacheIfIndex, deviceId, snmp_buffer) == L7_SUCCESS)  &&
        (SafeMakeOctetStringFromText(&agentIsdpCacheEntryData.agentIsdpCachePlatform, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentIsdpCachePlatform, agentIsdpCacheEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentIsdpCacheCapabilities :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((snmpAgentIsdpCpbGet(agentIsdpCacheEntryData.agentIsdpCacheIfIndex, 
                             deviceId, snmp_buffer, SNMP_BUFFER_LEN) == L7_SUCCESS) && 
        (SafeMakeOctetStringFromText(&agentIsdpCacheEntryData.agentIsdpCacheCapabilities,
                                     snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentIsdpCacheCapabilities, agentIsdpCacheEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentIsdpCacheLastChange :
    if (usmdbIsdpIntfNeighborLastChangeTimeGet(agentIsdpCacheEntryData.agentIsdpCacheIfIndex,
          deviceId, &agentIsdpCacheEntryData.agentIsdpCacheLastChange) == L7_SUCCESS)
    {
      /* This API returns the value in seconds. Need to multiply with TICKS_PER_SECOND
      for timeticks */
      agentIsdpCacheEntryData.agentIsdpCacheLastChange *= TICKS_PER_SECOND;
      SET_VALID(I_agentIsdpCacheLastChange, agentIsdpCacheEntryData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_agentIsdpCacheProtocolVersion :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((snmpAgentIsdpProtocolVersion(agentIsdpCacheEntryData.agentIsdpCacheIfIndex,
                                      deviceId, snmp_buffer, SNMP_BUFFER_LEN) == L7_SUCCESS) &&
        (SafeMakeOctetStringFromText(&agentIsdpCacheEntryData.agentIsdpCacheProtocolVersion, snmp_buffer) == L7_TRUE))
      SET_VALID(I_agentIsdpCacheProtocolVersion, agentIsdpCacheEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentIsdpCacheHoldtime :
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmdbIsdpIntfNeighborHoldTimeGet(agentIsdpCacheEntryData.agentIsdpCacheIfIndex, deviceId,
                                         &snmp_buffer[0]) == L7_SUCCESS) 
    {
      agentIsdpCacheEntryData.agentIsdpCacheHoldtime =snmp_buffer[0];
      SET_VALID(I_agentIsdpCacheHoldtime, agentIsdpCacheEntryData.valid);
    }
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentIsdpCacheEntryData.valid))
    return(NULL);

  return(&agentIsdpCacheEntryData);
}
