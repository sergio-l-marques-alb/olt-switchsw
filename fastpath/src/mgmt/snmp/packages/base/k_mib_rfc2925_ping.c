/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\snmp\snmp_sr\snmpb\unix\k_mib_rfc2925_ping.c
*
* @purpose  
*
* @component SNMP
*
* @comments
*
* @create 17/11/2006
*
* @author sowjanya
* @end
*
**********************************************************************/
/*********************************************************************
*
********************************************************************/
#include <k_private_base.h>
#include "sr_ip.h"
#include <usmdb_ping_api.h>
#include <k_mib_rfc2925_ping_api.h>
#include "l7_common.h"
#include "usmdb_ping_api.h"
#include "ping_exports.h"

  pingObjects_t *
k_pingObjects_get(int serialNum, ContextInfo *contextInfo,
    int nominator)
{
  static pingObjects_t pingObjectsData;

  ZERO_VALID(pingObjectsData.valid);

  switch (nominator)
  {
    case -1:
      break;

    case I_pingMaxConcurrentRequests :
      if ( usmDbMaxPingSessionsGet(&pingObjectsData.pingMaxConcurrentRequests ) == L7_SUCCESS)
        SET_VALID(I_pingMaxConcurrentRequests,pingObjectsData.valid);
      break;

    default:
      /* unknown nominator */
      return(NULL);
      break;
  }

  if (nominator >= 0 && !VALID(nominator, pingObjectsData.valid))
    return(NULL);

  return(&pingObjectsData);
}

#ifdef SETS
  int
k_pingObjects_test(ObjectInfo *object, ObjectSyntax *value,
    doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

  int
k_pingObjects_ready(ObjectInfo *object, ObjectSyntax *value,
    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

  int
k_pingObjects_set(pingObjects_t *data,
    ContextInfo *contextInfo, int function)
{

  return COMMIT_FAILED_ERROR;
}

#ifdef SR_pingObjects_UNDO
/* add #define SR_pingObjects_UNDO in sitedefs.h to
 * include the undo routine for the pingObjects family.
 */
  int
pingObjects_undo(doList_t *doHead, doList_t *doCur,
    ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
#endif /* SR_pingObjects_UNDO */

#endif /* SETS */


  pingCtlEntry_t *
k_pingCtlEntry_get(int serialNum, ContextInfo *contextInfo,
    int nominator,
    int searchType,
    OctetString * pingCtlOwnerIndex,
    OctetString * pingCtlTestName)
{

  static pingCtlEntry_t pingCtlEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uchar8 snmp_buffer_name[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_name_len=0;
  L7_uint32 snmp_buffer_len = 0;
  L7_ushort16 handle;
  L7_uint32 ipDa;  

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    pingCtlEntryData.pingCtlOwnerIndex  = MakeOctetString(NULL, 0);
    pingCtlEntryData.pingCtlTestName = MakeOctetString(NULL, 0);
    pingCtlEntryData.pingCtlTargetAddress = MakeOctetString(NULL, 0);
    pingCtlEntryData.pingCtlDataFill = MakeOctetString(NULL, 0);
    pingCtlEntryData.pingCtlTrapGeneration = MakeOctetString(NULL, 0);
    pingCtlEntryData.pingCtlDescr = MakeOctetString(NULL, 0);
    pingCtlEntryData.pingCtlSourceAddress = MakeOctetString(NULL, 0);
    pingCtlEntryData.pingCtlType =  MakeOID(NULL, 0);
  }

  ZERO_VALID(pingCtlEntryData.valid);

  memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer, pingCtlOwnerIndex->octet_ptr, pingCtlOwnerIndex->length);
  snmp_buffer_len = pingCtlOwnerIndex->length;

  memset(snmp_buffer_name, 0, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer_name, pingCtlTestName->octet_ptr, pingCtlTestName->length);
  snmp_buffer_name_len = pingCtlTestName->length;

  if ((searchType == EXACT) ?
      (usmDbPingSessionHandleFromIndexTestNameGet(snmp_buffer,snmp_buffer_name,&handle)!= L7_SUCCESS):
      ((usmDbPingSessionHandleFromIndexTestNameGet(snmp_buffer,snmp_buffer_name,&handle)!= L7_SUCCESS) &&
      (usmDbPingGetNextHandle( snmp_buffer,snmp_buffer_name,&handle ) != L7_SUCCESS)))
  {
    ZERO_VALID(pingCtlEntryData.valid);
    return(NULL);
  }

  if (SafeMakeOctetString(&pingCtlEntryData.pingCtlOwnerIndex, snmp_buffer, strlen(snmp_buffer)) != L7_TRUE)
  {
    ZERO_VALID(pingCtlEntryData.valid);
    return(NULL);
  }
  SET_VALID(I_pingCtlOwnerIndex, pingCtlEntryData.valid);

  if (SafeMakeOctetString(&pingCtlEntryData.pingCtlTestName, snmp_buffer_name, strlen(snmp_buffer_name)) != L7_TRUE)
  {
    ZERO_VALID(pingCtlEntryData.valid);
    return(NULL);
  }
  SET_VALID(I_pingCtlTestName, pingCtlEntryData.valid);


  switch (nominator)
  {
    case -1:
    case I_pingCtlOwnerIndex :
    case I_pingCtlTestName :  
      break;

    case I_pingCtlTargetAddressType :  
      pingCtlEntryData.pingCtlTargetAddressType = D_pingCtlTargetAddressType_ipv4;
      SET_VALID(I_pingCtlTargetAddressType, pingCtlEntryData.valid);
      break;

    case I_pingCtlTargetAddress :         
      if(usmDbPingQueryTargetAddrs(handle,&ipDa) == L7_SUCCESS)
      {
        pingCtlEntryData.pingCtlTargetAddress = IPToOctetString(ipDa);
        SET_VALID(I_pingCtlTargetAddress, pingCtlEntryData.valid);
      }
      break;   

    case I_pingCtlDataSize :
      if(usmDbPingProbeSizeGet(handle,&pingCtlEntryData.pingCtlDataSize)== L7_SUCCESS)
        SET_VALID(I_pingCtlDataSize, pingCtlEntryData.valid);
      break;    

    case I_pingCtlTimeOut :          
      if(usmDbPingProbeIntervalGet(handle,&pingCtlEntryData.pingCtlTimeOut)== L7_SUCCESS)
        SET_VALID(I_pingCtlTimeOut, pingCtlEntryData.valid);
      break;   

    case I_pingCtlProbeCount :            
      if(usmDbPingProbeCountGet(handle,&pingCtlEntryData.pingCtlProbeCount)== L7_SUCCESS)
        SET_VALID(I_pingCtlProbeCount, pingCtlEntryData.valid);
      break;  

    case I_pingCtlAdminStatus :         
      if(snmpPingAdminStatusGet(handle ,&pingCtlEntryData.pingCtlAdminStatus)== L7_SUCCESS)
        SET_VALID(I_pingCtlAdminStatus, pingCtlEntryData.valid);
      break;

      
    case I_pingCtlFrequency :
      pingCtlEntryData.pingCtlFrequency = 0;
      SET_VALID(I_pingCtlFrequency, pingCtlEntryData.valid);
      break;

    case I_pingCtlStorageType :              
      pingCtlEntryData.pingCtlStorageType =  D_pingCtlStorageType_volatile;
      SET_VALID(I_pingCtlStorageType, pingCtlEntryData.valid);
      break;

    case I_pingCtlType :
      pingCtlEntryData.pingCtlType = MakeOIDFromDot("1.3.6.1.2.1.80.3.1") ;
      SET_VALID(I_pingCtlType, pingCtlEntryData.valid);
      break;

    case I_pingCtlDescr :          
      pingCtlEntryData.pingCtlDescr = MakeOctetString(NULL, 0);
      SET_VALID(I_pingCtlDescr, pingCtlEntryData.valid);
      break;
      
    case I_pingCtlIfIndex :         
      pingCtlEntryData.pingCtlIfIndex = 0;
      SET_VALID(I_pingCtlIfIndex, pingCtlEntryData.valid);
      break;
      
    case I_pingCtlDSField :       
      pingCtlEntryData.pingCtlDSField =0 ;
      SET_VALID(I_pingCtlDSField, pingCtlEntryData.valid);
      break;

    case I_pingCtlRowStatus :       
      if(snmpPingRowStatusGet(handle ,&pingCtlEntryData.pingCtlRowStatus)== L7_SUCCESS) 
        SET_VALID(I_pingCtlRowStatus, pingCtlEntryData.valid);
      break;

    default:
      /* unknown nominator */
      return(NULL);
      break;
  }

  if (nominator >= 0 && !VALID(nominator, pingCtlEntryData.valid))
    return(NULL);

  return(&pingCtlEntryData);
}


#ifdef SETS
  int
k_pingCtlEntry_test(ObjectInfo *object, ObjectSyntax *value,
    doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

  int
k_pingCtlEntry_ready(ObjectInfo *object, ObjectSyntax *value,
    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

  int
k_pingCtlEntry_set_defaults(doList_t *dp)
{
  pingCtlEntry_t *data = (pingCtlEntry_t *) (dp->data);

  data->pingCtlTargetAddressType = D_pingCtlTargetAddressType_ipv4;
  if ((data->pingCtlTargetAddress = MakeOctetStringFromText("")) == 0) {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->pingCtlDataSize = PING_DEFAULT_PROBE_SIZE;
  data->pingCtlTimeOut = PING_DEFAULT_PROBE_INTERVAL;
  data->pingCtlProbeCount = PING_DEFAULT_PROBE_COUNT;
  data->pingCtlAdminStatus = D_pingCtlAdminStatus_disabled;
  if ((data->pingCtlDataFill = MakeOctetStringFromText("")) == 0) {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->pingCtlFrequency = 0;
  data->pingCtlMaxRows = 50;
  data->pingCtlStorageType = D_pingCtlStorageType_volatile;
  if ((data->pingCtlTrapGeneration = MakeOctetStringFromText("")) == 0) {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->pingCtlTrapProbeFailureFilter = 1;
  data->pingCtlTrapTestFailureFilter = 1;
  /* pingIcmpEcho = 1.3.6.1.2.1.80.3.1 */
  if ((data->pingCtlType = MakeOIDFromDot("1.3.6.1.2.1.80.3.1")) == 0) {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->pingCtlDescr = MakeOctetStringFromText("")) == 0) {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->pingCtlSourceAddressType = D_pingCtlSourceAddressType_ipv4;
  if ((data->pingCtlSourceAddress = MakeOctetStringFromText("")) == 0) {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->pingCtlByPassRouteTable = D_pingCtlByPassRouteTable_false;
  data->pingCtlDSField = 0;
  ZERO_VALID(data->valid);
  return NO_ERROR;
}

  int
k_pingCtlEntry_set(pingCtlEntry_t *data,
    ContextInfo *contextInfo, int function)
{
  char snmp_buffer[SNMP_BUFFER_LEN];
  char snmp_buffer_name[SNMP_BUFFER_LEN];
  char snmp_buffer_ipDa[SNMP_BUFFER_LEN];
  L7_uint32 ipDa = 0,i = 0;
  L7_RC_t rc = L7_FAILURE;
  L7_ushort16 handle;
  
  bzero(snmp_buffer, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer, data->pingCtlOwnerIndex->octet_ptr, data->pingCtlOwnerIndex->length);
  bzero(snmp_buffer_name, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer_name, data->pingCtlTestName->octet_ptr, data->pingCtlTestName->length);


  if (VALID(I_pingCtlRowStatus, data->valid))
  {
    if (snmpPingRowStatusSet(snmp_buffer,snmp_buffer_name,data->pingCtlRowStatus,contextInfo)!= L7_SUCCESS) 
      return(COMMIT_FAILED_ERROR);
  }

  if (VALID(I_pingCtlDataSize, data->valid) )
  {
    if (snmpPingDataSizeSet(snmp_buffer,snmp_buffer_name,data->pingCtlDataSize) != L7_SUCCESS)
    {
      return(COMMIT_FAILED_ERROR);
    }
  }

  if (VALID(I_pingCtlTimeOut, data->valid))
  {
    if (snmpPingTimeOutSet(snmp_buffer,snmp_buffer_name,data->pingCtlTimeOut) != L7_SUCCESS)
    {
      return(COMMIT_FAILED_ERROR);
    }
  }

  if (VALID(I_pingCtlProbeCount, data->valid))
  {
    if (snmpPingProbeCountSet(snmp_buffer,snmp_buffer_name,data->pingCtlProbeCount) != L7_SUCCESS)
    {
      return(COMMIT_FAILED_ERROR);
    }
  }

  if (VALID(I_pingCtlTargetAddress, data->valid))
  {
    bzero(snmp_buffer_ipDa, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer_ipDa, data->pingCtlTargetAddress->octet_ptr, data->pingCtlTargetAddress->length);
    for(i=0; i < strlen(snmp_buffer_ipDa); i++)
    {
      if(snmp_buffer_ipDa[i] == '.')
      {
        break;
      }
    }
    
    if (i < strlen(snmp_buffer_ipDa))
    {
       if (snmpPingTargetAddressSet(snmp_buffer,snmp_buffer_name,snmp_buffer_ipDa)!= L7_SUCCESS)
       {
        return(COMMIT_FAILED_ERROR);
       } 
    }
    else 
    {
      ipDa = OctetStringToIP(data->pingCtlTargetAddress); 
      if (ipDa != 0)
      {
        rc= usmDbPingSessionHandleFromIndexTestNameGet( snmp_buffer,snmp_buffer_name, &handle);
        if (rc == L7_SUCCESS)
        {
         rc = usmDbPingDestAddrSet(handle,ipDa);
        }
      }
      
      if (rc != L7_SUCCESS) 
      {
        return(COMMIT_FAILED_ERROR);
      }
    }
  }

  if (VALID(I_pingCtlAdminStatus, data->valid))
  {
    if( snmpPingAdminStatusSet(snmp_buffer,snmp_buffer_name,data->pingCtlAdminStatus)!= L7_SUCCESS)
    {
      return(COMMIT_FAILED_ERROR);
    }
  }

  return (NO_ERROR);
}

#ifdef SR_pingCtlEntry_UNDO
/* add #define SR_pingCtlEntry_UNDO in sitedefs.h to
 * include the undo routine for the pingCtlEntry family.
 */
  int
pingCtlEntry_undo(doList_t *doHead, doList_t *doCur,
    ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
#endif /* SR_pingCtlEntry_UNDO */

#endif /* SETS */

  pingResultsEntry_t *
k_pingResultsEntry_get(int serialNum, ContextInfo *contextInfo,
    int nominator,
    int searchType,
    OctetString * pingCtlOwnerIndex,
    OctetString * pingCtlTestName)
{

  static pingResultsEntry_t pingResultsEntryData;
  L7_ushort16 handle;
  L7_BOOL operStatus;
  L7_uint32 ipDa;
  char snmp_buffer[SNMP_BUFFER_LEN];
  char snmp_buffer_name[SNMP_BUFFER_LEN];
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    pingResultsEntryData.pingResultsIpTargetAddress= MakeOctetString(NULL, 0);
    pingResultsEntryData.pingCtlOwnerIndex = MakeOctetString(NULL, 0);
    pingResultsEntryData.pingCtlTestName = MakeOctetString(NULL, 0);
    pingResultsEntryData.pingResultsLastGoodProbe = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(pingResultsEntryData.valid);

  memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer, pingCtlOwnerIndex->octet_ptr, pingCtlOwnerIndex->length);

  memset(snmp_buffer_name, 0, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer_name, pingCtlTestName->octet_ptr, pingCtlTestName->length);

  SET_VALID(I_pingResultsIpTargetAddress, pingResultsEntryData.valid);


  if ((searchType == EXACT) ? (usmDbPingSessionHandleFromIndexTestNameGet( snmp_buffer,
          snmp_buffer_name, &handle) != L7_SUCCESS):
      ((usmDbPingSessionHandleFromIndexTestNameGet( snmp_buffer,
                                                    snmp_buffer_name, &handle) != L7_SUCCESS)&&
       (usmDbPingGetNextHandle(snmp_buffer,snmp_buffer_name,
                               &handle) != L7_SUCCESS)))
  {
    ZERO_VALID(pingResultsEntryData.valid);
    return(NULL);
  }

  if (SafeMakeOctetString(&(pingResultsEntryData.pingCtlOwnerIndex), snmp_buffer, strlen(snmp_buffer)) != L7_TRUE)
  {
    ZERO_VALID(pingResultsEntryData.valid);
    return(NULL);
  }
  SET_VALID(I_pingResultsEntryIndex_pingCtlOwnerIndex, pingResultsEntryData.valid);

  if (SafeMakeOctetString(&(pingResultsEntryData.pingCtlTestName), snmp_buffer_name, strlen(snmp_buffer_name)) != L7_TRUE)
  {
    ZERO_VALID(pingResultsEntryData.valid);
    return(NULL);
  }
  SET_VALID(I_pingResultsEntryIndex_pingCtlTestName, pingResultsEntryData.valid);

  switch (nominator)
  {
    case -1:
    case I_pingResultsEntryIndex_pingCtlOwnerIndex:
    case I_pingResultsEntryIndex_pingCtlTestName:
      break;

    case I_pingResultsOperStatus :            
      if (usmDbPingQueryOperStatus( handle, &operStatus )== L7_SUCCESS)
      {
        if (operStatus == L7_TRUE)
          pingResultsEntryData.pingResultsOperStatus = D_pingResultsOperStatus_enabled;
        else 
          pingResultsEntryData.pingResultsOperStatus = D_pingResultsOperStatus_disabled;

        SET_VALID(I_pingResultsOperStatus, pingResultsEntryData.valid);
      }
      break;

    case I_pingResultsIpTargetAddressType :                  
      pingResultsEntryData.pingResultsIpTargetAddressType = D_pingCtlTargetAddressType_ipv4;
      SET_VALID(I_pingResultsIpTargetAddressType, pingResultsEntryData.valid);
      break;

    case I_pingResultsIpTargetAddress :
      if(usmDbPingQueryTargetAddrs(handle, &ipDa)== L7_SUCCESS)
      {  
        pingResultsEntryData.pingResultsIpTargetAddress = IPToOctetString(ipDa);  
        SET_VALID(I_pingResultsIpTargetAddress, pingResultsEntryData.valid);
      }
      break;

    case I_pingResultsMinRtt :           

      if (usmDbPingQueryMinRtt( handle,&pingResultsEntryData.pingResultsMinRtt )== L7_SUCCESS)
      {  
        SET_VALID(I_pingResultsMinRtt, pingResultsEntryData.valid);
      }
      break;

    case I_pingResultsMaxRtt :           
      if (usmDbPingQueryMaxRtt( handle, &pingResultsEntryData.pingResultsMaxRtt )== L7_SUCCESS)
      {
        SET_VALID(I_pingResultsMaxRtt, pingResultsEntryData.valid);
      }
      break;

    case I_pingResultsAverageRtt : 
      if (usmDbPingQueryAvgRtt( handle, &pingResultsEntryData.pingResultsAverageRtt )== L7_SUCCESS)
      {
        SET_VALID(I_pingResultsAverageRtt, pingResultsEntryData.valid);
      }
      break;

    case I_pingResultsProbeResponses :           
      if (usmDbPingQueryProbeResponse( handle, &pingResultsEntryData.pingResultsProbeResponses )== L7_SUCCESS)
      {
        SET_VALID(I_pingResultsProbeResponses, pingResultsEntryData.valid);
      }
      break;

    case I_pingResultsSentProbes :           
      if (usmDbPingQueryProbeSent(handle,&pingResultsEntryData.pingResultsSentProbes )== L7_SUCCESS)
      {
        SET_VALID(I_pingResultsSentProbes, pingResultsEntryData.valid);
      }
      break;

    default:
      /* unknown nominator */
      return(NULL);
      break;
  }

  if (nominator >= 0 && !VALID(nominator, pingResultsEntryData.valid))
    return(NULL);

  return(&pingResultsEntryData);
}


  pingProbeHistoryEntry_t *
k_pingProbeHistoryEntry_get(int serialNum, ContextInfo *contextInfo,
    int nominator,
    int searchType,
    OctetString * pingCtlOwnerIndex,
    OctetString * pingCtlTestName,
    SR_UINT32 pingProbeHistoryIndex)
{
#ifdef NOT_YET
  static pingProbeHistoryEntry_t pingProbeHistoryEntryData;

  /*
   * put your code to retrieve the information here
   */

  pingProbeHistoryEntryData.pingProbeHistoryIndex = ;
  pingProbeHistoryEntryData.pingProbeHistoryResponse = ;
  pingProbeHistoryEntryData.pingProbeHistoryStatus = ;
  pingProbeHistoryEntryData.pingProbeHistoryLastRC = ;
  pingProbeHistoryEntryData.pingProbeHistoryTime = ;
  pingProbeHistoryEntryData.pingCtlOwnerIndex = ;
  pingProbeHistoryEntryData.pingCtlTestName = ;
  SET_ALL_VALID(pingProbeHistoryEntryData.valid);
  return(&pingProbeHistoryEntryData);
#else /* NOT_YET */
  return(NULL);
#endif /* NOT_YET */
}



