/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\application\mgmt\snmp\packages\base\k_mib_rfc2925_traceroute.c
*
* @purpose
*
* @component
*
* @comments
*
* @create 17/11/2006
*
* @author soma
* @end
*
**********************************************************************/
/*********************************************************************
*
********************************************************************/

#include <k_private_base.h>
#include "sr_ip.h"
#include <usmdb_traceroute_api.h>
#include <k_mib_rfc2925_traceroute_api.h>
#include "l7_common.h"
#include "usmdb_traceroute_api.h"
#include "traceroute_exports.h"

  traceRouteObjects_t *
k_traceRouteObjects_get(int serialNum, ContextInfo *contextInfo,
    int nominator)
{
  static traceRouteObjects_t traceRouteObjectsData;

  ZERO_VALID(traceRouteObjectsData.valid);

  switch (nominator)
  {
    case -1:
      break;

    case I_traceRouteMaxConcurrentRequests :
      if ( usmDbTraceRouteMaxSessionsGet(&traceRouteObjectsData.traceRouteMaxConcurrentRequests ) == L7_SUCCESS)
        SET_VALID(I_traceRouteMaxConcurrentRequests,traceRouteObjectsData.valid);
      break;

    default:
      /* unknown nominator */
      return(NULL);
      break;
  }

  if (nominator >= 0 && !VALID(nominator, traceRouteObjectsData.valid))
    return(NULL);

  return(&traceRouteObjectsData);
}

#ifdef SETS
  int
k_traceRouteObjects_test(ObjectInfo *object, ObjectSyntax *value,
    doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

  int
k_traceRouteObjects_ready(ObjectInfo *object, ObjectSyntax *value,
    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

  int
k_traceRouteObjects_set(traceRouteObjects_t *data,
    ContextInfo *contextInfo, int function)
{

  return COMMIT_FAILED_ERROR;
}

#ifdef SR_traceRouteObjects_UNDO
/* add #define SR_traceRouteObjects_UNDO in sitedefs.h to
 * include the undo routine for the traceRouteObjects family.
 */
  int
traceRouteObjects_undo(doList_t *doHead, doList_t *doCur,
    ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
#endif /* SR_traceRouteObjects_UNDO */

#endif /* SETS */

  traceRouteCtlEntry_t *
k_traceRouteCtlEntry_get(int serialNum, ContextInfo *contextInfo,
    int nominator,
    int searchType,
    OctetString * traceRouteCtlOwnerIndex,
    OctetString * traceRouteCtlTestName)
{
  static traceRouteCtlEntry_t traceRouteCtlEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uchar8 snmp_buffer_name[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_name_len=0;
  L7_uint32 snmp_buffer_len = 0;
  L7_ushort16 handle;
  L7_BOOL operStatus;
  L7_ushort16 vrfId = 1;
  L7_uint32 ipDa;
  L7_ushort16 probeSize, probePerHop, probeInterval;
  L7_BOOL dontFrag = L7_FALSE;
  L7_ushort16 port, maxTtl, initTtl, maxFail;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    traceRouteCtlEntryData.traceRouteCtlOwnerIndex  = MakeOctetString(NULL, 0);
    traceRouteCtlEntryData.traceRouteCtlTestName = MakeOctetString(NULL, 0);
    traceRouteCtlEntryData.traceRouteCtlTargetAddress = MakeOctetString(NULL, 0);
    traceRouteCtlEntryData.traceRouteCtlSourceAddress = MakeOctetString(NULL, 0);
    traceRouteCtlEntryData.traceRouteCtlMiscOptions = MakeOctetString(NULL, 0);
    traceRouteCtlEntryData.traceRouteCtlDescr = MakeOctetString(NULL, 0);
    traceRouteCtlEntryData.traceRouteCtlTrapGeneration = MakeOctetString(NULL, 0);
    traceRouteCtlEntryData.traceRouteCtlType = MakeOID(NULL, 0);

  }
  ZERO_VALID(traceRouteCtlEntryData.valid);

  memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer, traceRouteCtlOwnerIndex->octet_ptr, traceRouteCtlOwnerIndex->length);
  snmp_buffer_len = traceRouteCtlOwnerIndex->length;

  memset(snmp_buffer_name, 0, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer_name, traceRouteCtlTestName->octet_ptr, traceRouteCtlTestName->length);
  snmp_buffer_name_len = traceRouteCtlTestName->length; 

  if ((searchType == EXACT) ?
      (usmDbTraceRouteResultGet(snmp_buffer,snmp_buffer_name,&handle, &operStatus, &vrfId,
                                &ipDa, &probeSize, &probePerHop, &probeInterval, &dontFrag, &port,
                                &maxTtl, &initTtl, &maxFail) != L7_SUCCESS):
      ((usmDbTraceRouteResultGet(snmp_buffer,snmp_buffer_name,&handle, &operStatus, &vrfId,
                                 &ipDa, &probeSize, &probePerHop, &probeInterval, &dontFrag, &port,
                                 &maxTtl, &initTtl, &maxFail) != L7_SUCCESS) &&
       (usmDbTraceRouteGetNext( snmp_buffer,snmp_buffer_name,&handle,
                                &operStatus, &vrfId, &ipDa, &probeSize, &probePerHop,
                                &probeInterval, &dontFrag, &port, &maxTtl, &initTtl,&maxFail) != L7_SUCCESS)))
  {
    ZERO_VALID(traceRouteCtlEntryData.valid);
    return(NULL);
  }

  if (SafeMakeOctetString(&traceRouteCtlEntryData.traceRouteCtlOwnerIndex, snmp_buffer, strlen(snmp_buffer)) != L7_TRUE)
  {
    ZERO_VALID(traceRouteCtlEntryData.valid);
    return(NULL);
  }
  SET_VALID(I_traceRouteCtlOwnerIndex, traceRouteCtlEntryData.valid);

  if (SafeMakeOctetString(&traceRouteCtlEntryData.traceRouteCtlTestName, snmp_buffer_name, strlen(snmp_buffer_name)) != L7_TRUE)
  {
    ZERO_VALID(traceRouteCtlEntryData.valid);
    return(NULL);
  }
  SET_VALID(I_traceRouteCtlTestName, traceRouteCtlEntryData.valid);

  switch (nominator)
  {
    case -1:
    case I_traceRouteCtlOwnerIndex:
    case I_traceRouteCtlTestName:
      break;      

    case I_traceRouteCtlTargetAddressType:  
      traceRouteCtlEntryData.traceRouteCtlTargetAddressType = D_traceRouteCtlTargetAddressType_ipv4;
      SET_VALID(I_traceRouteCtlTargetAddressType, traceRouteCtlEntryData.valid);
      break;
    case I_traceRouteCtlTargetAddress:
      traceRouteCtlEntryData.traceRouteCtlTargetAddress = IPToOctetString(ipDa);
      SET_VALID(I_traceRouteCtlTargetAddress, traceRouteCtlEntryData.valid);
      break;
    case I_traceRouteCtlDataSize:                  
      traceRouteCtlEntryData.traceRouteCtlDataSize = probeSize;
      SET_VALID(I_traceRouteCtlDataSize, traceRouteCtlEntryData.valid);
      break;
    case I_traceRouteCtlTimeOut:                            
      traceRouteCtlEntryData.traceRouteCtlTimeOut =probeInterval;
      SET_VALID(I_traceRouteCtlTimeOut, traceRouteCtlEntryData.valid);
      break;
    case I_traceRouteCtlProbesPerHop:                                      
      traceRouteCtlEntryData.traceRouteCtlProbesPerHop = probePerHop;
      SET_VALID(I_traceRouteCtlProbesPerHop, traceRouteCtlEntryData.valid);
      break;
    case I_traceRouteCtlPort:       
      traceRouteCtlEntryData.traceRouteCtlPort = port ;
      SET_VALID(I_traceRouteCtlPort, traceRouteCtlEntryData.valid);
      break;
    case I_traceRouteCtlMaxTtl:                                                                
      traceRouteCtlEntryData.traceRouteCtlMaxTtl = maxTtl ;
      SET_VALID(I_traceRouteCtlMaxTtl, traceRouteCtlEntryData.valid);
      break;
    case I_traceRouteCtlDSField:
      traceRouteCtlEntryData.traceRouteCtlDSField = 0;
      SET_VALID(I_traceRouteCtlDSField, traceRouteCtlEntryData.valid);
      break;
    case I_traceRouteCtlIfIndex:
      traceRouteCtlEntryData.traceRouteCtlIfIndex = 0 ;
      SET_VALID(I_traceRouteCtlIfIndex, traceRouteCtlEntryData.valid);
      break;
      
    case I_traceRouteCtlMiscOptions:
      traceRouteCtlEntryData.traceRouteCtlMiscOptions = MakeOctetString(NULL, 0);
      SET_VALID(I_traceRouteCtlMiscOptions, traceRouteCtlEntryData.valid);
      break;
    case I_traceRouteCtlMaxFailures:                                   
      traceRouteCtlEntryData.traceRouteCtlMaxFailures = maxFail;
      SET_VALID(I_traceRouteCtlMaxFailures, traceRouteCtlEntryData.valid); 
      break;
    case I_traceRouteCtlDontFragment:      
      traceRouteCtlEntryData.traceRouteCtlDontFragment = D_traceRouteCtlDontFragment_false ;
      SET_VALID(I_traceRouteCtlDontFragment, traceRouteCtlEntryData.valid);
      break;
    case I_traceRouteCtlInitialTtl: 
      traceRouteCtlEntryData.traceRouteCtlInitialTtl = initTtl ;
      SET_VALID(I_traceRouteCtlInitialTtl, traceRouteCtlEntryData.valid);
      break;
    case I_traceRouteCtlStorageType:                               
      traceRouteCtlEntryData.traceRouteCtlStorageType = D_traceRouteCtlStorageType_volatile;
      SET_VALID(I_traceRouteCtlStorageType, traceRouteCtlEntryData.valid);
      break;
    case I_traceRouteCtlAdminStatus:
      if (operStatus == L7_TRUE)
        traceRouteCtlEntryData.traceRouteCtlAdminStatus = D_traceRouteCtlAdminStatus_enabled;
      else 
        traceRouteCtlEntryData.traceRouteCtlAdminStatus = D_traceRouteCtlAdminStatus_disabled; 
      SET_VALID(I_traceRouteCtlAdminStatus, traceRouteCtlEntryData.valid);
      break;
    case I_traceRouteCtlType:
      traceRouteCtlEntryData.traceRouteCtlType = MakeOIDFromDot("1.3.6.1.2.1.81.3.1");
      SET_VALID(I_traceRouteCtlType, traceRouteCtlEntryData.valid);
      break;
    case I_traceRouteCtlRowStatus:
      if (ipDa != 0)
        traceRouteCtlEntryData.traceRouteCtlRowStatus = D_traceRouteCtlRowStatus_active;
      else 
        traceRouteCtlEntryData.traceRouteCtlRowStatus = D_traceRouteCtlRowStatus_createAndGo;
      SET_VALID(I_traceRouteCtlRowStatus, traceRouteCtlEntryData.valid);
      break;
    default:
      /* unknown nominator */
      return(NULL);
      break;
  }

  if (nominator >= 0 && !VALID(nominator,traceRouteCtlEntryData.valid))
    return(NULL);

  return(&traceRouteCtlEntryData);
}


#ifdef SETS
  int
k_traceRouteCtlEntry_test(ObjectInfo *object, ObjectSyntax *value,
    doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

  int
k_traceRouteCtlEntry_ready(ObjectInfo *object, ObjectSyntax *value,
    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

  int
k_traceRouteCtlEntry_set_defaults(doList_t *dp)
{
  traceRouteCtlEntry_t *data = (traceRouteCtlEntry_t *) (dp->data);


  data->traceRouteCtlTargetAddressType = D_traceRouteCtlTargetAddressType_ipv4;
  if ((data->traceRouteCtlTargetAddress = MakeOctetStringFromText("")) == 0) {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->traceRouteCtlByPassRouteTable = D_traceRouteCtlByPassRouteTable_false;
  data->traceRouteCtlDataSize = TRACEROUTE_DEFAULT_PROBE_SIZE;
  data->traceRouteCtlTimeOut = TRACEROUTE_DEFAULT_PROBE_INTERVAL;
  data->traceRouteCtlProbesPerHop = TRACEROUTE_DEFAULT_PROBE_PER_HOP;
  data->traceRouteCtlPort = TRACEROUTE_DEFAULT_PORT;
  data->traceRouteCtlMaxTtl = TRACEROUTE_DEFAULT_MAX_TTL;
  data->traceRouteCtlDSField = 0;
  data->traceRouteCtlSourceAddressType = D_traceRouteCtlSourceAddressType_unknown;
  if ((data->traceRouteCtlSourceAddress = MakeOctetStringFromText("")) == 0) {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->traceRouteCtlMiscOptions = MakeOctetStringFromText("")) == 0) {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->traceRouteCtlMaxFailures = TRACEROUTE_DEFAULT_MAX_FAIL;
  data->traceRouteCtlDontFragment = D_traceRouteCtlDontFragment_false;
  data->traceRouteCtlInitialTtl = TRACEROUTE_DEFAULT_INIT_TTL;
  data->traceRouteCtlFrequency = 0;
  data->traceRouteCtlStorageType = D_traceRouteCtlStorageType_volatile;
  data->traceRouteCtlAdminStatus = D_traceRouteCtlAdminStatus_disabled;
  if ((data->traceRouteCtlDescr = MakeOctetStringFromText("")) == 0) {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->traceRouteCtlMaxRows = 50;
  if ((data->traceRouteCtlTrapGeneration = MakeOctetStringFromText("")) == 0) {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->traceRouteCtlCreateHopsEntries = D_traceRouteCtlCreateHopsEntries_false;
  /* traceRouteUsingUdpProbes = 1.3.6.1.2.1.81.3.1 */
  if ((data->traceRouteCtlType = MakeOIDFromDot("1.3.6.1.2.1.81.3.1")) == 0) {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

  int
k_traceRouteCtlEntry_set(traceRouteCtlEntry_t *data,
    ContextInfo *contextInfo, int function)
{
  char snmp_buffer[SNMP_BUFFER_LEN];
  char snmp_buffer_name[SNMP_BUFFER_LEN];
  char snmp_buffer_ipDa[SNMP_BUFFER_LEN];
  L7_uint32 ipDa = 0,i = 0;
  L7_ushort16 handle;
  L7_RC_t rc = L7_FAILURE;

  bzero(snmp_buffer, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer, data->traceRouteCtlOwnerIndex->octet_ptr, data->traceRouteCtlOwnerIndex->length);
  bzero(snmp_buffer_name, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer_name, data->traceRouteCtlTestName->octet_ptr, data->traceRouteCtlTestName->length);

  if (VALID(I_traceRouteCtlRowStatus, data->valid))
  {
    if (snmpTraceRouteRowStatusSet(snmp_buffer,snmp_buffer_name,data->traceRouteCtlRowStatus,contextInfo)!= L7_SUCCESS)     
      return(COMMIT_FAILED_ERROR);
  }
  if (VALID(I_traceRouteCtlTargetAddress, data->valid))
  {
    bzero(snmp_buffer_ipDa, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer_ipDa, data->traceRouteCtlTargetAddress->octet_ptr, data->traceRouteCtlTargetAddress->length);

    for(i = 0; i < strlen(snmp_buffer_ipDa); i++)
    {
      if(snmp_buffer_ipDa[i] == '.')
      {
        break;
      }
    }
    if (i < strlen(snmp_buffer_ipDa))
    {
      if (snmpTraceRouteTargetAddressSet(snmp_buffer,snmp_buffer_name,snmp_buffer_ipDa) != L7_SUCCESS)  
      {
        return(COMMIT_FAILED_ERROR);
      }
    }
    else
    {
      ipDa = OctetStringToIP(data->traceRouteCtlTargetAddress);  
      if (ipDa != 0)
      {
        rc = usmDbTraceRouteHandleGet( snmp_buffer,snmp_buffer_name, &handle); 
        if ( rc == L7_SUCCESS )
        {
          rc = usmDbTraceRouteDestAddrSet(handle,ipDa);
        }
      } 
      if ((rc != L7_SUCCESS) || (ipDa == 0))
      {
        return(COMMIT_FAILED_ERROR);    
      }
    }
   }
  if (VALID( I_traceRouteCtlDataSize, data->valid))
  {
    if (snmpTraceRouteDataSizeSet(snmp_buffer,snmp_buffer_name,data->traceRouteCtlDataSize) != L7_SUCCESS) 
      return(COMMIT_FAILED_ERROR);
  }
  if (VALID( I_traceRouteCtlTimeOut, data->valid))
  {
    if (snmpTraceRouteTimeOutSet(snmp_buffer,snmp_buffer_name,data->traceRouteCtlTimeOut) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }
  if (VALID( I_traceRouteCtlProbesPerHop, data->valid))
  {
    if (snmpTraceRouteProbePerHopSet(snmp_buffer,snmp_buffer_name,data->traceRouteCtlProbesPerHop) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }
  if (VALID( I_traceRouteCtlPort, data->valid))
  {
    if (snmpTraceRoutePortSet(snmp_buffer,snmp_buffer_name,data->traceRouteCtlPort) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }
  if (VALID( I_traceRouteCtlMaxTtl, data->valid))
  {
    if (snmpTraceRouteMaxTtlSet(snmp_buffer,snmp_buffer_name,data->traceRouteCtlMaxTtl) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }
  if (VALID( I_traceRouteCtlMaxFailures, data->valid))
  {
    if (snmpTraceRouteMaxFailSet(snmp_buffer,snmp_buffer_name,data->traceRouteCtlMaxFailures) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);
  }
  if (VALID( I_traceRouteCtlDontFragment, data->valid))
  {
    if(snmpTraceRouteDontFragmentSet(snmp_buffer,snmp_buffer_name,data->traceRouteCtlDontFragment) != L7_SUCCESS)
    {
      return(COMMIT_FAILED_ERROR);
    }
  }
  if (VALID( I_traceRouteCtlInitialTtl, data->valid))
  {
    if(snmpTraceRouteInitTtlSet(snmp_buffer,snmp_buffer_name,data->traceRouteCtlInitialTtl) != L7_SUCCESS)
    {
      return(COMMIT_FAILED_ERROR);
    }
  }
  if (VALID( I_traceRouteCtlAdminStatus, data->valid))
  {
    if(snmpTraceRouteAdminStatusSet(snmp_buffer,snmp_buffer_name,data->traceRouteCtlAdminStatus) != L7_SUCCESS)
    {
      return(COMMIT_FAILED_ERROR);
    }
  }
  return (NO_ERROR);
}

#ifdef SR_traceRouteCtlEntry_UNDO
/* add #define SR_traceRouteCtlEntry_UNDO in sitedefs.h to
 * include the undo routine for the traceRouteCtlEntry family.
 */
  int
traceRouteCtlEntry_undo(doList_t *doHead, doList_t *doCur,
    ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
#endif /* SR_traceRouteCtlEntry_UNDO */

#endif /* SETS */

  traceRouteResultsEntry_t *
k_traceRouteResultsEntry_get(int serialNum, ContextInfo *contextInfo,
    int nominator,
    int searchType,
    OctetString * traceRouteCtlOwnerIndex,
    OctetString * traceRouteCtlTestName)
{
  static traceRouteResultsEntry_t traceRouteResultsEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uchar8 snmp_buffer_name[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_name_len=0;
  L7_uint32 snmp_buffer_len = 0;
  L7_BOOL operStatus;
  L7_RC_t rc;
  L7_ushort16 vrfId = 1;
  L7_uint32 ipDa;
  L7_ushort16 probeSize,probePerHop, probeInterval;
  L7_BOOL dontFrag = L7_FALSE;
  L7_ushort16 handle, port, maxTtl, initTtl, maxFail, currTtl;
  L7_ushort16 currHopCount, currProbeCount, testAttempt, testSuccess;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    traceRouteResultsEntryData.traceRouteCtlOwnerIndex  = MakeOctetString(NULL, 0);
    traceRouteResultsEntryData.traceRouteCtlTestName = MakeOctetString(NULL, 0);
    traceRouteResultsEntryData.traceRouteResultsIpTgtAddr = MakeOctetString(NULL, 0);
    traceRouteResultsEntryData.traceRouteResultsLastGoodPath = MakeOctetString(NULL, 0);
  }
  ZERO_VALID(traceRouteResultsEntryData.valid);

  memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer, traceRouteCtlOwnerIndex->octet_ptr, traceRouteCtlOwnerIndex->length);
  snmp_buffer_len = traceRouteCtlOwnerIndex->length;

  memset(snmp_buffer_name, 0, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer_name, traceRouteCtlTestName->octet_ptr, traceRouteCtlTestName->length);
  snmp_buffer_name_len = traceRouteCtlTestName->length;

  if ((searchType == EXACT) ?
      (usmDbTraceRouteResultGet(snmp_buffer,snmp_buffer_name,&handle, &operStatus, &vrfId,
                                &ipDa, &probeSize, &probePerHop, &probeInterval, &dontFrag, &port,
                                &maxTtl, &initTtl, &maxFail) != L7_SUCCESS):
      ((usmDbTraceRouteResultGet(snmp_buffer,snmp_buffer_name,&handle, &operStatus, &vrfId,
                                 &ipDa, &probeSize, &probePerHop, &probeInterval, &dontFrag, &port,
                                 &maxTtl, &initTtl, &maxFail) != L7_SUCCESS) &&
       (usmDbTraceRouteGetNext( snmp_buffer,snmp_buffer_name,&handle,
                                &operStatus, &vrfId, &ipDa, &probeSize, &probePerHop,
                                &probeInterval, &dontFrag, &port, &maxTtl, &initTtl,&maxFail) != L7_SUCCESS)))
  {
    ZERO_VALID(traceRouteResultsEntryData.valid);
    return(NULL);
  }

  if (SafeMakeOctetString(&traceRouteResultsEntryData.traceRouteCtlOwnerIndex, snmp_buffer, strlen(snmp_buffer)) != L7_TRUE)
  {
    ZERO_VALID(traceRouteResultsEntryData.valid);
    return(NULL);
  }
  SET_VALID(I_traceRouteResultsEntryIndex_traceRouteCtlOwnerIndex, traceRouteResultsEntryData.valid);

  if (SafeMakeOctetString(&traceRouteResultsEntryData.traceRouteCtlTestName, snmp_buffer_name, strlen(snmp_buffer_name)) != L7_TRUE)
  {
    ZERO_VALID(traceRouteResultsEntryData.valid);
    return(NULL);
  }
  SET_VALID(I_traceRouteResultsEntryIndex_traceRouteCtlTestName, traceRouteResultsEntryData.valid);

  rc = usmDbTraceRouteQuery( handle, &operStatus, &currTtl, &currHopCount, &currProbeCount, &testAttempt,
                  &testSuccess );
  
  switch (nominator)
  {
    case -1:
    case I_traceRouteResultsEntryIndex_traceRouteCtlOwnerIndex:
    case I_traceRouteResultsEntryIndex_traceRouteCtlTestName:
      break;  

    case I_traceRouteResultsOperStatus:
      if( rc == L7_SUCCESS)
      {
        if (operStatus == L7_TRUE )
          traceRouteResultsEntryData.traceRouteResultsOperStatus = D_traceRouteResultsOperStatus_enabled;
        else
          traceRouteResultsEntryData.traceRouteResultsOperStatus = D_traceRouteResultsOperStatus_disabled;
      }
      SET_VALID(I_traceRouteResultsOperStatus, traceRouteResultsEntryData.valid);
      break;

    case I_traceRouteResultsCurHopCount:
      if( rc == L7_SUCCESS)
        traceRouteResultsEntryData.traceRouteResultsCurHopCount = currHopCount;
      SET_VALID(I_traceRouteResultsCurHopCount, traceRouteResultsEntryData.valid);
      break;
    case I_traceRouteResultsCurProbeCount:
      if( rc == L7_SUCCESS)
        traceRouteResultsEntryData.traceRouteResultsCurProbeCount = currProbeCount;
      SET_VALID(I_traceRouteResultsCurProbeCount, traceRouteResultsEntryData.valid);
      break;

    case I_traceRouteResultsIpTgtAddrType:  
      traceRouteResultsEntryData.traceRouteResultsIpTgtAddrType = D_traceRouteResultsIpTgtAddrType_ipv4;
      SET_VALID(I_traceRouteResultsIpTgtAddrType, traceRouteResultsEntryData.valid);
      break;

    case I_traceRouteResultsIpTgtAddr: 
      traceRouteResultsEntryData.traceRouteResultsIpTgtAddr = IPToOctetString(ipDa) ;
      SET_VALID(I_traceRouteResultsIpTgtAddr, traceRouteResultsEntryData.valid);
      break;
    case I_traceRouteResultsTestAttempts:       
      if( rc == L7_SUCCESS)
        traceRouteResultsEntryData.traceRouteResultsTestAttempts = testAttempt ;
      SET_VALID(I_traceRouteResultsTestAttempts, traceRouteResultsEntryData.valid);
      break;
    case I_traceRouteResultsTestSuccesses:
      if( rc == L7_SUCCESS)
        traceRouteResultsEntryData.traceRouteResultsTestSuccesses = testSuccess ;
      SET_VALID(I_traceRouteResultsTestSuccesses, traceRouteResultsEntryData.valid);
      break;


    default:
      /* unknown nominator */
      return(NULL);
      break;
  }

  if (nominator >= 0 && !VALID(nominator, traceRouteResultsEntryData.valid))
    return(NULL);

  return(&traceRouteResultsEntryData);


}

  traceRouteProbeHistoryEntry_t *
k_traceRouteProbeHistoryEntry_get(int serialNum, ContextInfo *contextInfo,
    int nominator,
    int searchType,
    OctetString * traceRouteCtlOwnerIndex,
    OctetString * traceRouteCtlTestName,
    SR_UINT32 traceRouteProbeHistoryIndex,
    SR_UINT32 traceRouteProbeHistoryHopIndex,
    SR_UINT32 traceRouteProbeHistoryProbeIndex)
{
#ifdef NOT_YET
  static traceRouteProbeHistoryEntry_t traceRouteProbeHistoryEntryData;

  /*
   * put your code to retrieve the information here
   */

  traceRouteProbeHistoryEntryData.traceRouteProbeHistoryIndex = ;
  traceRouteProbeHistoryEntryData.traceRouteProbeHistoryHopIndex = ;
  traceRouteProbeHistoryEntryData.traceRouteProbeHistoryProbeIndex = ;
  traceRouteProbeHistoryEntryData.traceRouteProbeHistoryHAddrType = ;
  traceRouteProbeHistoryEntryData.traceRouteProbeHistoryHAddr = ;
  traceRouteProbeHistoryEntryData.traceRouteProbeHistoryResponse = ;
  traceRouteProbeHistoryEntryData.traceRouteProbeHistoryStatus = ;
  traceRouteProbeHistoryEntryData.traceRouteProbeHistoryLastRC = ;
  traceRouteProbeHistoryEntryData.traceRouteProbeHistoryTime = ;
  traceRouteProbeHistoryEntryData.traceRouteCtlOwnerIndex = ;
  traceRouteProbeHistoryEntryData.traceRouteCtlTestName = ;
  SET_ALL_VALID(traceRouteProbeHistoryEntryData.valid);
  return(&traceRouteProbeHistoryEntryData);
#else /* NOT_YET */
  return(NULL);
#endif /* NOT_YET */
}
  traceRouteHopsEntry_t *
k_traceRouteHopsEntry_get(int serialNum, ContextInfo *contextInfo,
    int nominator,
    int searchType,
    OctetString * traceRouteCtlOwnerIndex,
    OctetString * traceRouteCtlTestName,
    SR_UINT32 traceRouteHopsHopIndex)
{
  static traceRouteHopsEntry_t traceRouteHopsEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uchar8 snmp_buffer_name[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_name_len=0;
  L7_uint32 snmp_buffer_len = 0;
  L7_ushort16 handle, ttl;
  L7_uint32 hopDa, minRtt, maxRtt, avgRtt;
  L7_ushort16 probeSent, probeRecvd, hopIndex;
  
  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    traceRouteHopsEntryData.traceRouteCtlOwnerIndex  = MakeOctetString(NULL, 0);
    traceRouteHopsEntryData.traceRouteCtlTestName = MakeOctetString(NULL, 0);
    traceRouteHopsEntryData.traceRouteHopsIpTgtAddress = MakeOctetString(NULL, 0);
    traceRouteHopsEntryData.traceRouteHopsLastGoodProbe = MakeOctetString(NULL, 0);
  }
  ZERO_VALID(traceRouteHopsEntryData.valid);

  traceRouteHopsEntryData.traceRouteHopsHopIndex = traceRouteHopsHopIndex;
  SET_VALID(I_traceRouteHopsHopIndex,traceRouteHopsEntryData.valid);
  
  hopIndex = (SR_UINT16)traceRouteHopsHopIndex;

  memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer, traceRouteCtlOwnerIndex->octet_ptr, traceRouteCtlOwnerIndex->length);
  snmp_buffer_len = traceRouteCtlOwnerIndex->length;

  memset(snmp_buffer_name, 0, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer_name, traceRouteCtlTestName->octet_ptr, traceRouteCtlTestName->length);
  snmp_buffer_name_len = traceRouteCtlTestName->length;

  if (searchType == EXACT)
  {
    if(usmDbTraceRouteHandleGet( snmp_buffer, snmp_buffer_name, &handle)== L7_SUCCESS) 
    {
      if( usmDbTraceRouteHopGet( handle, traceRouteHopsEntryData.traceRouteHopsHopIndex, &ttl,
            &hopDa, &minRtt,&maxRtt, &avgRtt, &probeSent,
            &probeRecvd ) != L7_SUCCESS)
      {
        ZERO_VALID(traceRouteHopsEntryData.valid);
        return(NULL);
      }
    }
    else
    {
      ZERO_VALID(traceRouteHopsEntryData.valid);
      return(NULL);
    }
  }
  if(searchType == NEXT)    
  {
    if (usmDbTraceRouteHandleGet( snmp_buffer, snmp_buffer_name, &handle)== L7_SUCCESS)
    {
      if ((usmDbTraceRouteHopGet( handle,traceRouteHopsEntryData.traceRouteHopsHopIndex, &ttl,
              &hopDa, &minRtt,&maxRtt, &avgRtt, &probeSent,
              &probeRecvd ) != L7_SUCCESS) &&
          (usmDbTraceRouteHopGetNext( handle,&hopIndex, &ttl,
                                      &hopDa, &minRtt,&maxRtt, &avgRtt, &probeSent,
                                      &probeRecvd) != L7_SUCCESS))
      {
        if (usmDbTraceRouteSessionHandleGetNext( snmp_buffer, snmp_buffer_name, &handle)== L7_SUCCESS)
        {
          hopIndex =0;
          if( (usmDbTraceRouteHopGet( handle,hopIndex, &ttl,
                  &hopDa, &minRtt,&maxRtt, &avgRtt, &probeSent,
                  &probeRecvd ) != L7_SUCCESS))
          {
            ZERO_VALID(traceRouteHopsEntryData.valid);
            return(NULL);
          }
        }  
        else 
        {
          ZERO_VALID(traceRouteHopsEntryData.valid);
          return(NULL);
        }

      } 
    }
    else if (usmDbTraceRouteSessionHandleGetNext( snmp_buffer, snmp_buffer_name, &handle)== L7_SUCCESS)
    {
      if ((usmDbTraceRouteHopGet( handle,traceRouteHopsEntryData.traceRouteHopsHopIndex, &ttl,
              &hopDa, &minRtt,&maxRtt, &avgRtt, &probeSent,
              &probeRecvd ) != L7_SUCCESS) &&
          (usmDbTraceRouteHopGetNext( handle,&hopIndex, &ttl,
                                      &hopDa, &minRtt,&maxRtt, &avgRtt, &probeSent,
                                      &probeRecvd) != L7_SUCCESS))
      {
        ZERO_VALID(traceRouteHopsEntryData.valid);
        return(NULL);

      }
    }
    else
    {
      ZERO_VALID(traceRouteHopsEntryData.valid);
      return(NULL);
    }
  }

  traceRouteHopsEntryData.traceRouteHopsHopIndex = (SR_UINT32)hopIndex;
  SET_VALID(I_traceRouteHopsHopIndex,traceRouteHopsEntryData.valid);

  if (SafeMakeOctetString(&traceRouteHopsEntryData.traceRouteCtlOwnerIndex, snmp_buffer, strlen(snmp_buffer)) != L7_TRUE)
  {
    ZERO_VALID(traceRouteHopsEntryData.valid);
    return(NULL);
  }
  SET_VALID(I_traceRouteHopsEntryIndex_traceRouteCtlOwnerIndex, traceRouteHopsEntryData.valid);

  if (SafeMakeOctetString(&traceRouteHopsEntryData.traceRouteCtlTestName, snmp_buffer_name, strlen(snmp_buffer_name)) != L7_TRUE)
  {
    ZERO_VALID(traceRouteHopsEntryData.valid);
    return(NULL);
  }

  SET_VALID(I_traceRouteHopsEntryIndex_traceRouteCtlTestName, traceRouteHopsEntryData.valid);

  switch(nominator) 
  {
    case -1:
    case I_traceRouteHopsEntryIndex_traceRouteCtlOwnerIndex:
    case I_traceRouteHopsEntryIndex_traceRouteCtlTestName:   
    case I_traceRouteHopsHopIndex:  
      break;
    case I_traceRouteHopsIpTgtAddressType:
      traceRouteHopsEntryData.traceRouteHopsIpTgtAddressType =  D_traceRouteHopsIpTgtAddressType_ipv4;
      SET_VALID(I_traceRouteHopsIpTgtAddressType, traceRouteHopsEntryData.valid); 
      break;
    case I_traceRouteHopsIpTgtAddress:
      traceRouteHopsEntryData.traceRouteHopsIpTgtAddress = IPToOctetString(hopDa) ;
      SET_VALID(I_traceRouteHopsIpTgtAddress, traceRouteHopsEntryData.valid);
      break;               
    case I_traceRouteHopsMinRtt:             
      traceRouteHopsEntryData.traceRouteHopsMinRtt = minRtt/TRACEROUTE_RTT_MULTIPLIER;
      SET_VALID(I_traceRouteHopsMinRtt, traceRouteHopsEntryData.valid);
      break;
    case I_traceRouteHopsMaxRtt:
      traceRouteHopsEntryData.traceRouteHopsMaxRtt = maxRtt/TRACEROUTE_RTT_MULTIPLIER;
      SET_VALID(I_traceRouteHopsMaxRtt, traceRouteHopsEntryData.valid);
      break;
    case I_traceRouteHopsAverageRtt:                  
      traceRouteHopsEntryData.traceRouteHopsAverageRtt = avgRtt/TRACEROUTE_RTT_MULTIPLIER;
      SET_VALID(I_traceRouteHopsAverageRtt, traceRouteHopsEntryData.valid);
      break;
    case I_traceRouteHopsSentProbes:                   
      traceRouteHopsEntryData.traceRouteHopsSentProbes = probeSent;
      SET_VALID(I_traceRouteHopsSentProbes, traceRouteHopsEntryData.valid);
      break;
    case I_traceRouteHopsProbeResponses:                      
      traceRouteHopsEntryData.traceRouteHopsProbeResponses = probeRecvd;
      SET_VALID(I_traceRouteHopsProbeResponses, traceRouteHopsEntryData.valid);
      break;

    default:
      /* unknown nominator */
      return(NULL);
      break;
  }

  if (nominator >= 0 && !VALID(nominator, traceRouteHopsEntryData.valid))
    return(NULL);

  return(&traceRouteHopsEntryData); 
}

  
