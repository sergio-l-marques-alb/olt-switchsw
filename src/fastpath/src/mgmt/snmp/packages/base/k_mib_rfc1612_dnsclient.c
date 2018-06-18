/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_rfc1612_dnsclient.c
*
* @purpose    System-Specific code to support the DNS protocol
*
* @component  SNMP
*
* @comments
*
* @create     3/29/2005
*
* @author     I. Kiran
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "k_private_base.h"
#include "k_mib_rfc1612_dnsclient_api.h"
#include "usmdb_dns_client_api.h"

#define L7_DEF_CONFIG_RESET_TIME 0
#define L7_STD_OPCODE            0

dnsResConfig_t *
k_dnsResConfig_get(int serialNum, ContextInfo *contextInfo,
                   int nominator)
{
  static dnsResConfig_t dnsResConfigData;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  static L7_BOOL firstTime = L7_TRUE;
                                                                                
  if (firstTime == L7_TRUE)
  {
    dnsResConfigData.dnsResConfigImplementIdent= MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }

 switch (nominator)
  {
  case -1:
  case I_dnsResConfigImplementIdent:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if((snmpdnsResConfigImplementIdentGet(snmp_buffer) == L7_SUCCESS)  &&  
        SafeMakeOctetStringFromTextExact
            (&dnsResConfigData.dnsResConfigImplementIdent, snmp_buffer) 
                                                            == L7_TRUE)
      SET_VALID(I_dnsResConfigImplementIdent, dnsResConfigData.valid);
    if (nominator != -1)
      break;

  case I_dnsResConfigService:
    if (usmDbDNSClientServiceGet(&dnsResConfigData.dnsResConfigService)
                                    == L7_SUCCESS)
      SET_VALID(I_dnsResConfigService, dnsResConfigData.valid);
    if (nominator != -1)
      break;

  case I_dnsResConfigMaxCnames:
    if (usmDbDNSClientMaxCnamesGet(&dnsResConfigData.dnsResConfigMaxCnames)
                                    == L7_SUCCESS)
      SET_VALID(I_dnsResConfigMaxCnames, dnsResConfigData.valid);
    if (nominator != -1)
      break;

  case I_dnsResConfigUpTime:
    if (usmDbDNSClientUpTimeGet(&dnsResConfigData.dnsResConfigUpTime)
                                    == L7_SUCCESS)
      SET_VALID(I_dnsResConfigUpTime, dnsResConfigData.valid);
    if (nominator != -1)
      break;

  case I_dnsResConfigResetTime:
      dnsResConfigData.dnsResConfigResetTime = L7_DEF_CONFIG_RESET_TIME;
      SET_VALID(I_dnsResConfigResetTime, dnsResConfigData.valid);
    if (nominator != -1)
      break;

  case I_dnsResConfigReset:
    if (snmpdnsResConfigResetGet(&dnsResConfigData.dnsResConfigReset)
                                                      == L7_SUCCESS)
      SET_VALID(I_dnsResConfigReset, dnsResConfigData.valid);
      break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }
 
  if (nominator != -1 && !VALID(nominator, dnsResConfigData.valid))
    return(NULL);
                                                                                
  return(&dnsResConfigData);

}

dnsResConfigSbeltEntry_t *
k_dnsResConfigSbeltEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_UINT32 dnsResConfigSbeltAddr,
                             OctetString * dnsResConfigSbeltSubTree,
                             SR_INT32 dnsResConfigSbeltClass)
{
#if 1 
  /* we don't really support this table */
  return NULL;
#else
  static dnsResConfigSbeltEntry_t dnsResConfigSbeltEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_inet_addr_t serverAddr;

  inetAddressReset(&serverAddr);
  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    dnsResConfigSbeltEntryData.dnsResConfigSbeltName = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(dnsResConfigSbeltEntryData.valid);
  dnsResConfigSbeltEntryData.dnsResConfigSbeltAddr = dnsResConfigSbeltAddr;
  SET_VALID(I_dnsResConfigSbeltAddr, dnsResConfigSbeltEntryData.valid);

  dnsResConfigSbeltEntryData.dnsResConfigSbeltClass = dnsResConfigSbeltClass;
  SET_VALID(I_dnsResConfigSbeltClass, dnsResConfigSbeltEntryData.valid);

  bzero(snmp_buffer, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer, dnsResConfigSbeltSubTree->octet_ptr, 
                      dnsResConfigSbeltSubTree->length);

  /* supports IPv4 only */
  inetAddressSet(L7_AF_INET, dnsResConfigSbeltEntryData.dnsResConfigSbeltAddr, &serverAddr);
  if ( (searchType == EXACT) ?
       (usmDbDNSClientNameServerEntryGet (&serverAddr) != L7_SUCCESS) :
      ((usmDbDNSClientNameServerEntryGet (&serverAddr) != L7_SUCCESS) &&
       (usmDbDNSClientNameServerEntryNextGet (&serverAddr) != L7_SUCCESS) ) )
  {
    ZERO_VALID(dnsResConfigSbeltEntryData.valid);
    return(NULL);
  }
  else
  {
    if (SafeMakeOctetStringFromTextExact
         (&dnsResConfigSbeltEntryData.dnsResConfigSbeltSubTree , snmp_buffer) 
                          == L7_TRUE)
    {
      SET_VALID(I_dnsResConfigSbeltSubTree, dnsResConfigSbeltEntryData.valid);
    }
    else
    {
      ZERO_VALID(dnsResConfigSbeltEntryData.valid);
      return(NULL);
    }
  }
  inetAddressGet(L7_AF_INET, &serverAddr, &dnsResConfigSbeltEntryData.dnsResConfigSbeltAddr);
  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_dnsResConfigSbeltAddr:
  case I_dnsResConfigSbeltSubTree:
  case I_dnsResConfigSbeltClass:
    if (nominator != -1) break;
    /* else pass through */

  case I_dnsResConfigSbeltName:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (SafeMakeOctetStringFromTextExact(
          &dnsResConfigSbeltEntryData.dnsResConfigSbeltName, snmp_buffer)
                                                             == L7_TRUE)
      SET_VALID(I_dnsResConfigSbeltName, dnsResConfigSbeltEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dnsResConfigSbeltRecursion:
    dnsResConfigSbeltEntryData.dnsResConfigSbeltRecursion =  2; /* 2 for recursion */
    SET_VALID(I_dnsResConfigSbeltRecursion, dnsResConfigSbeltEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dnsResConfigSbeltPref:
    if (usmDbDNSClientNameServerEntryPreferenceGet(&serverAddr,
          &dnsResConfigSbeltEntryData.dnsResConfigSbeltPref) == L7_SUCCESS) 
      SET_VALID(I_dnsResConfigSbeltPref, dnsResConfigSbeltEntryData.valid);
    if (nominator != -1) break;

  case I_dnsResConfigSbeltStatus:
    /* if entry shows up, it's active */
    dnsResConfigSbeltEntryData.dnsResConfigSbeltStatus = 
                                     D_dnsResConfigSbeltStatus_active;
    SET_VALID(I_dnsResConfigSbeltStatus, dnsResConfigSbeltEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dnsResConfigSbeltEntryData.valid))
    return(NULL);

   return(&dnsResConfigSbeltEntryData);
#endif
}

#ifdef SETS
int
k_dnsResConfigSbeltEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_dnsResConfigSbeltEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                               doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_dnsResConfigSbeltEntry_set_defaults(doList_t *dp)
{
    dnsResConfigSbeltEntry_t *data = (dnsResConfigSbeltEntry_t *) (dp->data);

    if ((data->dnsResConfigSbeltName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_dnsResConfigSbeltEntry_set(dnsResConfigSbeltEntry_t *data,
                             ContextInfo *contextInfo, int function)
{
 /*
  * Defining temporary variable for storing the valid bits for the case when the
  * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  L7_inet_addr_t serverAddr;

  inetAddressReset(&serverAddr);
  bzero(tempValid, sizeof(tempValid));
                                                                                
  if (VALID(I_dnsResConfigSbeltStatus, data->valid))
  {
    /*
     * If row status is set to delete and row is not existent return error
     */
    /* only IPv4 mib support */
    inetAddressSet(L7_AF_INET, &data->dnsResConfigSbeltAddr, &serverAddr);
    if (data->dnsResConfigSbeltStatus == D_dnsResConfigSbeltStatus_destroy)
    {
      if (usmDbDNSClientNameServerEntryRemove(&serverAddr) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(tempValid));
        return(COMMIT_FAILED_ERROR);
      }
    }
    /*
     * If row status is set to create, create the row and set the status to active
     */
    else if (data->dnsResConfigSbeltStatus == 
                         D_dnsResConfigSbeltStatus_createAndGo) 
    {
      if(usmDbDNSClientNameServerEntryAdd(&serverAddr) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(tempValid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        data->dnsResConfigSbeltStatus = 
                                     D_dnsResConfigSbeltStatus_active;
      }
    }
    else
    {
      return(COMMIT_FAILED_ERROR);
    }
                                                                                
    SET_VALID(I_dnsResConfigSbeltStatus, tempValid);
  }

   return NO_ERROR;
}

#ifdef SR_dnsResConfigSbeltEntry_UNDO
/* add #define SR_dnsResConfigSbeltEntry_UNDO in sitedefs.h to
 * include the undo routine for the dnsResConfigSbeltEntry family.
 */
int
dnsResConfigSbeltEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
  dnsResConfigSbeltEntry_t *data = (dnsResConfigSbeltEntry_t *) doCur->data;
  dnsResConfigSbeltEntry_t *undodata = 
                    (dnsResConfigSbeltEntry_t *) doCur->undodata;  
  dnsResConfigSbeltEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;
                                                                        
  /*Copy valid bits from data to undodata */
  if( undodata != NULL && data != NULL )
    memcpy(undodata->valid,data->valid,sizeof(data->valid));
                                                                        
  /* we are either trying to undo an add,or  a delete */
  if ( undodata == NULL )   {
    /* undoing an add, so delete */
    data->dnsResConfigSbeltStatus = D_dnsResConfigSbeltStatus_destroy;
    setdata = data;
    function = SR_DELETE;
  }
  else
  {
    /* undoing a delete */
    if(undodata->dnsResConfigSbeltStatus == D_dnsResConfigSbeltStatus_active)
    {
      undodata->dnsResConfigSbeltStatus = D_dnsResConfigSbeltStatus_createAndGo;
    }

    if (data->dnsResConfigSbeltStatus == D_dnsResConfigSbeltStatus_destroy)
    {
      /* Setting all valid bits to restore previous values */
      memset(undodata->valid, 0xff, sizeof(undodata->valid));
    }
                                                                        
    setdata = undodata;
    function = SR_ADD_MODIFY;
 }
                                                                        
  /* use the set method for the undo */
  if ((setdata != NULL) && 
     (k_dnsResConfigSbeltEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;
                                                                        
  return UNDO_FAILED_ERROR;
}
#endif /* SR_dnsResConfigSbeltEntry_UNDO */

#endif /* SETS */

dnsResCounter_t *
k_dnsResCounter_get(int serialNum, ContextInfo *contextInfo,
                    int nominator)
{
  static dnsResCounter_t dnsResCounterData;

  switch (nominator)
  {
  case -1:
  case I_dnsResCounterNonAuthDataResps:
    if (usmDbDNSClientCounterNonAuthDataResponsesGet(
         &dnsResCounterData.dnsResCounterNonAuthDataResps) == L7_SUCCESS)
      SET_VALID(I_dnsResCounterNonAuthDataResps, dnsResCounterData.valid);
    if (nominator != -1)
      break;                          
  
  case I_dnsResCounterNonAuthNoDataResps:
    if (usmDbDNSClientCounterNonAuthNoDataResponsesGet(
         &dnsResCounterData.dnsResCounterNonAuthNoDataResps ) == L7_SUCCESS)
      SET_VALID(I_dnsResCounterNonAuthNoDataResps, dnsResCounterData.valid);
    if (nominator != -1)
      break;                          
  
  case I_dnsResCounterMartians:
    if (usmDbDNSClientCounterMartiansGet(
         &dnsResCounterData.dnsResCounterMartians ) == L7_SUCCESS)
      SET_VALID(I_dnsResCounterMartians, dnsResCounterData.valid);
    if (nominator != -1)
      break;                          
  
  case I_dnsResCounterRecdResponses:
    if (usmDbDNSClientCounterReceivedResponsesGet(
         &dnsResCounterData.dnsResCounterRecdResponses ) == L7_SUCCESS)
      SET_VALID(I_dnsResCounterRecdResponses, dnsResCounterData.valid);
    if (nominator != -1)
      break;                          
  
  case I_dnsResCounterUnparseResps:
    if (usmDbDNSClientCounterUnparsedResponsesGet(
         &dnsResCounterData.dnsResCounterUnparseResps ) == L7_SUCCESS)
      SET_VALID(I_dnsResCounterUnparseResps, dnsResCounterData.valid);
    if (nominator != -1)
      break;                          
  
  case I_dnsResCounterFallbacks:
    if (usmDbDNSClientCounterFallbacksGet(
         &dnsResCounterData.dnsResCounterFallbacks ) == L7_SUCCESS)
      SET_VALID(I_dnsResCounterFallbacks, dnsResCounterData.valid);
      break;                          
  
  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != -1 && !VALID(nominator, dnsResCounterData.valid))
    return(NULL);

   return(&dnsResCounterData);
}

dnsResCounterByOpcodeEntry_t *
k_dnsResCounterByOpcodeEntry_get(int serialNum, ContextInfo *contextInfo,
                                 int nominator,
                                 int searchType,
                                 SR_INT32 dnsResCounterByOpcodeCode)
{
  static dnsResCounterByOpcodeEntry_t dnsResCounterByOpcodeEntryData;

  if(dnsResCounterByOpcodeCode != L7_STD_OPCODE && searchType != EXACT)
  {
    ZERO_VALID(dnsResCounterByOpcodeEntryData.valid);
    return(NULL);
  }
    

  dnsResCounterByOpcodeEntryData.dnsResCounterByOpcodeCode = 
                                          dnsResCounterByOpcodeCode;
  SET_VALID(I_dnsResCounterByOpcodeCode, dnsResCounterByOpcodeEntryData.valid);

  switch (nominator)
  {
  case -1:
  case I_dnsResCounterByOpcodeCode:
    if (nominator != -1)
    break;

  case I_dnsResCounterByOpcodeQueries:
    if (usmDbDNSClientCounterByOpcodeQueriesGet(
         dnsResCounterByOpcodeEntryData.dnsResCounterByOpcodeCode,
        &dnsResCounterByOpcodeEntryData.dnsResCounterByOpcodeQueries) 
                        == L7_SUCCESS)
      SET_VALID(I_dnsResCounterByOpcodeQueries, 
                 dnsResCounterByOpcodeEntryData.valid);
    break;

  case I_dnsResCounterByOpcodeResponses:
    if (usmDbDNSClientCounterByOpcodeResponsesGet(
         dnsResCounterByOpcodeEntryData.dnsResCounterByOpcodeCode,
         &dnsResCounterByOpcodeEntryData.dnsResCounterByOpcodeResponses) 
                         == L7_SUCCESS)
      SET_VALID(I_dnsResCounterByOpcodeResponses, 
                   dnsResCounterByOpcodeEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != -1 && !VALID(nominator, dnsResCounterByOpcodeEntryData.valid))
    return(NULL);

  return(&dnsResCounterByOpcodeEntryData);
}

dnsResCounterByRcodeEntry_t *
k_dnsResCounterByRcodeEntry_get(int serialNum, ContextInfo *contextInfo,
                                int nominator,
                                int searchType,
                                SR_INT32 dnsResCounterByRcodeCode)
{
  static dnsResCounterByRcodeEntry_t dnsResCounterByRcodeEntryData;
  L7_uint32 rcode = dnsResCounterByRcodeCode;
  /*
   * Get the first rcode compare with  the given rcode if it doesnot match loop
   * thru the get next till a  matching entry is found otherwise return an 
   * error.
   */
  if(searchType == EXACT)
  {
    if(usmDbDNSClientCounterByRcodeFirstGet(&rcode) != L7_SUCCESS)
    {
      ZERO_VALID(dnsResCounterByRcodeEntryData.valid);
      return(NULL);
    }

    while(rcode != dnsResCounterByRcodeCode) 
    {
      if (usmDbDNSClientCounterByRcodeNextGet(&rcode) != L7_SUCCESS)
      {
        ZERO_VALID(dnsResCounterByRcodeEntryData.valid);
        return(NULL);
      }
    }
  }
  else
  {
    if (usmDbDNSClientCounterByRcodeGet(rcode)!= L7_SUCCESS)
    {      
      if (usmDbDNSClientCounterByRcodeNextGet(&rcode) != L7_SUCCESS)
      {
        ZERO_VALID(dnsResCounterByRcodeEntryData.valid);
        return(NULL);
      }
    }
  }

  dnsResCounterByRcodeEntryData.dnsResCounterByRcodeCode = rcode;
  SET_VALID(I_dnsResCounterByRcodeCode, dnsResCounterByRcodeEntryData.valid);


  switch (nominator)
  {
  case -1:
  case I_dnsResCounterByRcodeCode:
    if (nominator != -1)
    break;

  case I_dnsResCounterByRcodeResponses:
    if (usmDbDNSClientCounterByRcodeResponsesGet(
        dnsResCounterByRcodeEntryData.dnsResCounterByRcodeCode,
        &dnsResCounterByRcodeEntryData.dnsResCounterByRcodeResponses) 
                        == L7_SUCCESS)
      SET_VALID(I_dnsResCounterByRcodeResponses, 
                 dnsResCounterByRcodeEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != -1 && !VALID(nominator, dnsResCounterByRcodeEntryData.valid))
    return(NULL);

  return(&dnsResCounterByRcodeEntryData);

}

dnsResLameDelegation_t *
k_dnsResLameDelegation_get(int serialNum, ContextInfo *contextInfo,
                           int nominator)
{
   return(NULL);
}

dnsResLameDelegationEntry_t *
k_dnsResLameDelegationEntry_get(int serialNum, ContextInfo *contextInfo,
                                int nominator,
                                int searchType,
                                SR_UINT32 dnsResLameDelegationSource,
                                OctetString * dnsResLameDelegationName,
                                SR_INT32 dnsResLameDelegationClass)
{
   return(NULL);
}

#ifdef SETS
int
k_dnsResLameDelegationEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_dnsResLameDelegationEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                  doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_dnsResLameDelegationEntry_set_defaults(doList_t *dp)
{
    dnsResLameDelegationEntry_t *data = (dnsResLameDelegationEntry_t *) (dp->data);

    data->dnsResLameDelegationCounts = (SR_UINT32) 0;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_dnsResLameDelegationEntry_set(dnsResLameDelegationEntry_t *data,
                                ContextInfo *contextInfo, int function)
{
   return COMMIT_FAILED_ERROR;
}

#ifdef SR_dnsResLameDelegationEntry_UNDO
/* add #define SR_dnsResLameDelegationEntry_UNDO in sitedefs.h to
 * include the undo routine for the dnsResLameDelegationEntry family.
 */
int
dnsResLameDelegationEntry_undo(doList_t *doHead, doList_t *doCur,
                               ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_dnsResLameDelegationEntry_UNDO */

#endif /* SETS */


dnsResCache_t *
k_dnsResCache_get(int serialNum, ContextInfo *contextInfo,
                  int nominator)
{
 static dnsResCache_t dnsResCacheData;
                                                                                
 switch (nominator)
  {
  case -1:
  case I_dnsResCacheStatus:
      dnsResCacheData.dnsResCacheStatus = D_dnsResCacheStatus_enabled;
      SET_VALID(I_dnsResCacheStatus, dnsResCacheData.valid);
    if (nominator != -1)
      break;

  case I_dnsResCacheMaxTTL:
    if (usmDbDNSClientCacheMaxTTLGet(&dnsResCacheData.dnsResCacheMaxTTL) 
                                                   == L7_SUCCESS)
      SET_VALID(I_dnsResCacheMaxTTL, dnsResCacheData.valid);
    if (nominator != -1)
      break;

  case I_dnsResCacheGoodCaches:
    if (usmDbDNSClientCacheGoodCachesGet(&dnsResCacheData.dnsResCacheGoodCaches)
                                    == L7_SUCCESS)
      SET_VALID(I_dnsResCacheGoodCaches, dnsResCacheData.valid);
    if (nominator != -1)
      break;

  case I_dnsResCacheBadCaches:
    if (usmDbDNSClientCacheBadCachesGet(&dnsResCacheData.dnsResCacheBadCaches)
                                    == L7_SUCCESS)
      SET_VALID(I_dnsResCacheBadCaches, dnsResCacheData.valid);
      break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }
 
  if (nominator != -1 && !VALID(nominator, dnsResCacheData.valid))
    return(NULL);
                                                                                
  return(&dnsResCacheData);

}

dnsResCacheRREntry_t *
k_dnsResCacheRREntry_get(int serialNum, ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         OctetString * dnsResCacheRRName,
                         SR_INT32 dnsResCacheRRClass,
                         SR_INT32 dnsResCacheRRType,
                         SR_INT32 dnsResCacheRRIndex)
{
  static dnsResCacheRREntry_t dnsResCacheRREntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_char8 name_buffer[SNMP_BUFFER_LEN];
  L7_char8 qname[SNMP_BUFFER_LEN];
  L7_BOOL rrvalue;
  L7_inet_addr_t sourceAddr;

  inetAddressReset(&sourceAddr);
  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    dnsResCacheRREntryData.dnsResCacheRRData = MakeOctetString(NULL, 0);
    dnsResCacheRREntryData.dnsResCacheRRPrettyName = MakeOctetString(NULL, 0);
    dnsResCacheRREntryData.dnsResCacheRRName = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(dnsResCacheRREntryData.valid);
  dnsResCacheRREntryData.dnsResCacheRRClass = dnsResCacheRRClass;
  SET_VALID(I_dnsResCacheRRClass, dnsResCacheRREntryData.valid);

  dnsResCacheRREntryData.dnsResCacheRRType = dnsResCacheRRType;
  SET_VALID(I_dnsResCacheRRType, dnsResCacheRREntryData.valid);

  dnsResCacheRREntryData.dnsResCacheRRIndex = dnsResCacheRRIndex;
  SET_VALID(I_dnsResCacheRRIndex, dnsResCacheRREntryData.valid);

  bzero(name_buffer, SNMP_BUFFER_LEN);
  memcpy(name_buffer, dnsResCacheRRName->octet_ptr, 
                      dnsResCacheRRName->length);

  if ( (searchType == EXACT) ?
       (usmDbDNSClientCacheRRGet
        (name_buffer, dnsResCacheRREntryData.dnsResCacheRRType,
         dnsResCacheRREntryData.dnsResCacheRRIndex ,
         dnsResCacheRREntryData.dnsResCacheRRClass ) != L7_SUCCESS) :
      ((usmDbDNSClientCacheRRGet
        (name_buffer, dnsResCacheRREntryData.dnsResCacheRRType,
         dnsResCacheRREntryData.dnsResCacheRRIndex ,
         dnsResCacheRREntryData.dnsResCacheRRClass ) != L7_SUCCESS) &&
       (snmpDNSClientCacheRRNextGet
        (name_buffer, (dnsRRType_t *)&dnsResCacheRREntryData.dnsResCacheRRType, 
         &dnsResCacheRREntryData.dnsResCacheRRIndex , 
         (dnsRRClass_t *)&dnsResCacheRREntryData.dnsResCacheRRClass, qname) != L7_SUCCESS) ) )
  {
    ZERO_VALID(dnsResCacheRREntryData.valid);
    return(NULL);
  }
  else
  {
    if (SafeMakeOctetStringFromTextExact
         (&dnsResCacheRREntryData.dnsResCacheRRName , name_buffer) 
                          == L7_TRUE)
    {
      SET_VALID(I_dnsResCacheRRName, dnsResCacheRREntryData.valid);
    }
    else
    {
      ZERO_VALID(dnsResCacheRREntryData.valid);
      return(NULL);
    }
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_dnsResCacheRRName:
  case I_dnsResCacheRRType:
  case I_dnsResCacheRRIndex:
  case I_dnsResCacheRRClass:
    if (nominator != -1) break;
    /* else pass through */

  case I_dnsResCacheRRTTL:
    if (usmDbDNSClientCacheRRTTLGet(
          name_buffer, 
          dnsResCacheRREntryData.dnsResCacheRRType,
          dnsResCacheRREntryData.dnsResCacheRRIndex,
          &dnsResCacheRREntryData.dnsResCacheRRTTL ) == L7_SUCCESS) 
      SET_VALID(I_dnsResCacheRRTTL, dnsResCacheRREntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_dnsResCacheRRElapsedTTL:
    if (usmDbDNSClientCacheRRTTLElapsedGet(
          name_buffer, 
          dnsResCacheRREntryData.dnsResCacheRRType,
          dnsResCacheRREntryData.dnsResCacheRRIndex,
          &dnsResCacheRREntryData.dnsResCacheRRElapsedTTL ) == L7_SUCCESS) 
      SET_VALID(I_dnsResCacheRRElapsedTTL, dnsResCacheRREntryData.valid);
    if (nominator != -1) break;

  case I_dnsResCacheRRSource:
    if (usmDbDNSClientCacheRRSourceGet(
          name_buffer, 
          dnsResCacheRREntryData.dnsResCacheRRType,
          dnsResCacheRREntryData.dnsResCacheRRIndex,
          &sourceAddr) == L7_SUCCESS)
      inetAddressGet(L7_AF_INET, &sourceAddr, &dnsResCacheRREntryData.dnsResCacheRRSource);
      SET_VALID(I_dnsResCacheRRSource, dnsResCacheRREntryData.valid);
    if (nominator != -1) break;

  case I_dnsResCacheRRData:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((usmDbDNSClientCacheRRDataGet(
          name_buffer,
          dnsResCacheRREntryData.dnsResCacheRRType,
          dnsResCacheRREntryData.dnsResCacheRRIndex,
          snmp_buffer, &rrvalue) == L7_SUCCESS) &&
         (SafeMakeOctetStringFromTextExact(
          &dnsResCacheRREntryData.dnsResCacheRRData, snmp_buffer)
                                                             == L7_TRUE))
      SET_VALID(I_dnsResCacheRRData, dnsResCacheRREntryData.valid);
    if (nominator != -1) break;

  case I_dnsResCacheRRPrettyName:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((usmDbDNSClientCacheRRPrettyNameGet(
          name_buffer, 
          dnsResCacheRREntryData.dnsResCacheRRType,
          dnsResCacheRREntryData.dnsResCacheRRIndex,
          snmp_buffer) == L7_SUCCESS) &&
         (SafeMakeOctetStringFromTextExact(
          &dnsResCacheRREntryData.dnsResCacheRRPrettyName, snmp_buffer)
                                                             == L7_TRUE))
      SET_VALID(I_dnsResCacheRRPrettyName, dnsResCacheRREntryData.valid);
    if (nominator != -1) break;

  case I_dnsResCacheRRStatus:
    /* if entry shows up, it's active */
    dnsResCacheRREntryData.dnsResCacheRRStatus = 
                                     D_dnsResConfigSbeltStatus_active;
    SET_VALID(I_dnsResCacheRRStatus, dnsResCacheRREntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, dnsResCacheRREntryData.valid))
    return(NULL);

   return(&dnsResCacheRREntryData);
}

#ifdef SETS
int
k_dnsResCacheRREntry_test(ObjectInfo *object, ObjectSyntax *value,
                          doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_dnsResCacheRREntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                           doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_dnsResCacheRREntry_set_defaults(doList_t *dp)
{
    dnsResCacheRREntry_t *data = (dnsResCacheRREntry_t *) (dp->data);

    if ((data->dnsResCacheRRData = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->dnsResCacheRRPrettyName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_dnsResCacheRREntry_set(dnsResCacheRREntry_t *data,
                         ContextInfo *contextInfo, int function)
{
 /*
  * Defining temporary variable for storing the valid bits for the case when the
  * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  L7_uint32 snmp_buffer_len;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_char8 hostname[DNS_DISPLAY_DOMAIN_NAME_SIZE_MAX];
  L7_uchar8 prettyName[DNS_DOMAIN_NAME_SIZE_MAX];


  bzero(tempValid, sizeof(tempValid));
                                                                                
  if (VALID(I_dnsResCacheRRStatus, data->valid))
  {
    /*
     * If row status is set to delete and row is not existent return error
     */
    if (data->dnsResCacheRRStatus == D_dnsResCacheRRStatus_destroy) 
    {
       memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
       memset(hostname , 0, DNS_DISPLAY_DOMAIN_NAME_SIZE_MAX);
       memset (prettyName, 0, DNS_DOMAIN_NAME_SIZE_MAX);

       memcpy(snmp_buffer, data->dnsResCacheRRName->octet_ptr, data->dnsResCacheRRName->length);
       snmp_buffer_len = data->dnsResCacheRRName->length;

       if (usmDbDNSClientCacheRRPrettyNameGet(snmp_buffer,
                                           data->dnsResCacheRRType,
                                           data->dnsResCacheRRIndex,
                                           prettyName) != L7_SUCCESS)
       {
             
          return(COMMIT_FAILED_ERROR);
       }
        
       if ( usmDbDNSClientDisplayNameGet( prettyName, hostname)!= L7_SUCCESS)
       {
          return(COMMIT_FAILED_ERROR);
       }
       
       if (usmDbDNSClientCacheHostFlush(hostname)
                     != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(tempValid));
        return(COMMIT_FAILED_ERROR);
      }
    } 
    else if (data->dnsResCacheRRStatus != D_dnsResCacheRRStatus_destroy)
    {
	 memcpy(data->valid, tempValid, sizeof(tempValid));
	 return(COMMIT_FAILED_ERROR);
    }
    /*
     * NO other operation is permitted on the cache. Return error except for 
     * destroy
     */
    SET_VALID(I_dnsResConfigSbeltStatus, tempValid);
  }

   return NO_ERROR;

}

#ifdef SR_dnsResCacheRREntry_UNDO
/* add #define SR_dnsResCacheRREntry_UNDO in sitedefs.h to
 * include the undo routine for the dnsResCacheRREntry family.
 */
int
dnsResCacheRREntry_undo(doList_t *doHead, doList_t *doCur,
                        ContextInfo *contextInfo)
{
  /*  
   * Always return error because the only operation on the table is
   * destroy. Once destroyed the cache cannot be added again. 
   */
   return UNDO_FAILED_ERROR;
}
#endif /* SR_dnsResCacheRREntry_UNDO */

#endif /* SETS */

dnsResNCache_t *
k_dnsResNCache_get(int serialNum, ContextInfo *contextInfo,
                   int nominator)
{
   return(NULL);
}

#ifdef SETS
int
k_dnsResNCache_test(ObjectInfo *object, ObjectSyntax *value,
                    doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_dnsResNCache_ready(ObjectInfo *object, ObjectSyntax *value, 
                     doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_dnsResNCache_set(dnsResNCache_t *data,
                   ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_dnsResNCache_UNDO
/* add #define SR_dnsResNCache_UNDO in sitedefs.h to
 * include the undo routine for the dnsResNCache family.
 */
int
dnsResNCache_undo(doList_t *doHead, doList_t *doCur,
                  ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_dnsResNCache_UNDO */

#endif /* SETS */

dnsResNCacheErrEntry_t *
k_dnsResNCacheErrEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           OctetString * dnsResNCacheErrQName,
                           SR_INT32 dnsResNCacheErrQClass,
                           SR_INT32 dnsResNCacheErrQType,
                           SR_INT32 dnsResNCacheErrIndex)
{
   return(NULL);
}

#ifdef SETS
int
k_dnsResNCacheErrEntry_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_dnsResNCacheErrEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                             doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_dnsResNCacheErrEntry_set_defaults(doList_t *dp)
{
    dnsResNCacheErrEntry_t *data = (dnsResNCacheErrEntry_t *) (dp->data);

    if ((data->dnsResNCacheErrPrettyName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_dnsResNCacheErrEntry_set(dnsResNCacheErrEntry_t *data,
                           ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_dnsResNCacheErrEntry_UNDO
/* add #define SR_dnsResNCacheErrEntry_UNDO in sitedefs.h to
 * include the undo routine for the dnsResNCacheErrEntry family.
 */
int
dnsResNCacheErrEntry_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_dnsResNCacheErrEntry_UNDO */

#endif /* SETS */

dnsResOptCounter_t *
k_dnsResOptCounter_get(int serialNum, ContextInfo *contextInfo,
                       int nominator)
{
   return(NULL);
}

