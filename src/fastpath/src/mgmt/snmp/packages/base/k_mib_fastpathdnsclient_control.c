/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_fastpathdnsclient_control.c
*
* @purpose    System-Specific code to support the DNS Client Control protocol
*
* @component  SNMP
*
* @comments
*
* @create     3/29/2005
*
* @author     I.Kiran
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "k_private_base.h"
#include "k_mib_fastpathdnsclient_control_api.h"
#include "usmdb_dns_client_api.h"
#include "sr_ip.h"

agentResCtlglobal_t *
k_agentResCtlglobal_get(int serialNum, ContextInfo *contextInfo,
                   int nominator)
{
  static agentResCtlglobal_t agentResCtlglobalData;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  static L7_BOOL firstTime = L7_TRUE;
  L7_ushort16 timeout;

  if (firstTime == L7_TRUE)
  {
    agentResCtlglobalData.agentResCtlDefDomainName  = MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }

  switch (nominator)
  {
  case -1:
  case I_agentResCtlAdminMode:
    if (snmpDnsResCtlglobalAdminModeGet(&agentResCtlglobalData.agentResCtlAdminMode) 
                                    == L7_SUCCESS)
      SET_VALID(I_agentResCtlAdminMode, agentResCtlglobalData.valid);
    if (nominator != -1)
      break;                          
  
  case I_agentResCtlDefDomainName:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((usmDbDNSClientDefaultDomainGet(snmp_buffer) == L7_SUCCESS) &&
        SafeMakeOctetStringFromTextExact(&agentResCtlglobalData.agentResCtlDefDomainName,
                                         snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentResCtlDefDomainName, agentResCtlglobalData.valid);
    if(nominator != -1)
      break;

  case I_agentResCtlCacheFlushStatus:
   /* By Default cache Flush status is disable always */
    agentResCtlglobalData.agentResCtlCacheFlushStatus = 
                      D_agentResCtlCacheFlushStatus_dnsCacheFlushDisable;
    SET_VALID(I_agentResCtlCacheFlushStatus, agentResCtlglobalData.valid);
    if(nominator != -1)
      break;

  case I_agentResCtlRequestTimeout:
   /* By Default cache Flush status is disable always */

    if (usmDbDNSClientQueryTimeoutGet(&timeout) == L7_SUCCESS)
	    agentResCtlglobalData.agentResCtlRequestTimeout = (L7_uint32) timeout;
    SET_VALID(I_agentResCtlRequestTimeout, agentResCtlglobalData.valid);
    if(nominator != -1)
      break;

  case I_agentResCtlRequestRetransmits:
   /* By Default cache Flush status is disable always */
    if ( usmDbDNSClientRetransmitsGet(&agentResCtlglobalData.agentResCtlRequestRetransmits) == L7_SUCCESS)
    SET_VALID(I_agentResCtlRequestRetransmits, agentResCtlglobalData.valid);

    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != -1 && !VALID(nominator, agentResCtlglobalData.valid))
    return(NULL);

   return(&agentResCtlglobalData);
}

#ifdef SETS
int
k_agentResCtlglobal_test(ObjectInfo *object, ObjectSyntax *value,
                    doList_t *dp, ContextInfo *contextInfo)
{
    return NO_ERROR;
}

int
k_agentResCtlglobal_ready(ObjectInfo *object, ObjectSyntax *value, 
                     doList_t *doHead, doList_t *dp)
{
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentResCtlglobal_set(agentResCtlglobal_t *data,
                   ContextInfo *contextInfo, int function)
{
  L7_char8 tempValid[sizeof(data->valid)];
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  bzero(tempValid, sizeof(tempValid));
  bzero(snmp_buffer, SNMP_BUFFER_LEN);


  if (VALID(I_agentResCtlAdminMode, data->valid))
  {
    if (snmpDnsResCtlglobalAdminModeSet(data->agentResCtlAdminMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    
    SET_VALID(I_agentResCtlAdminMode, tempValid);
  }

  if (VALID(I_agentResCtlDefDomainName, data->valid))
  {
    memcpy(snmp_buffer, data->agentResCtlDefDomainName->octet_ptr, 
		              data->agentResCtlDefDomainName->length);
    if (usmDbDNSClientDefaultDomainSet(snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    
    SET_VALID(I_agentResCtlDefDomainName, tempValid);
  }

  if (VALID(I_agentResCtlCacheFlushStatus, data->valid))
  {
    /* Only For Enable flush the status. For rest everything
       the set is ignored */
    if(data->agentResCtlCacheFlushStatus ==
                      D_agentResCtlCacheFlushStatus_dnsCacheFlushEnable)
    {
      /* Flush  Entire DNS Cache */
      if (usmDbDNSClientCacheFlush() != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(tempValid));
        return(COMMIT_FAILED_ERROR);
      }
    }
    
    SET_VALID(I_agentResCtlCacheFlushStatus, tempValid);
  }

  if (VALID(I_agentResCtlRequestTimeout, data->valid))
  {
    /* Flush  Entire DNS Cache */
    if (usmDbDNSClientQueryTimeoutSet(data->agentResCtlRequestTimeout) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    SET_VALID(I_agentResCtlRequestTimeout, tempValid);
  }

  if (VALID(I_agentResCtlRequestRetransmits, data->valid))
  {
    /* Flush  Entire DNS Cache */
    if (usmDbDNSClientRetransmitsSet(data->agentResCtlRequestRetransmits) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    SET_VALID(I_agentResCtlRequestRetransmits, tempValid);
  }
  return(NO_ERROR);
  
}

#ifdef SR_agentResCtlglobal_UNDO
/* add #define SR_agentResCtlglobal_UNDO in sitedefs.h to
 * include the undo routine for the agentResCtlglobal family.
 */
int
agentResCtlglobal_undo(doList_t *doHead, doList_t *doCur,
                  ContextInfo *contextInfo)
{
  agentResCtlglobal_t *data = (agentResCtlglobal_t *) doCur->data;
  agentResCtlglobal_t *undodata = (agentResCtlglobal_t *) doCur->undodata;
  agentResCtlglobal_t *setdata = NULL;
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
  if (k_agentResCtlglobal_set(setdata, contextInfo, function) == NO_ERROR)
    return(NO_ERROR);

  return(UNDO_FAILED_ERROR);
}
#endif /* SR_agentResCtlglobal_UNDO */

#endif /* SETS */

agentResCtlDomainListEntry_t *
k_agentResCtlDomainListEntry_get(int serialNum, ContextInfo *contextInfo,
                            int nominator,
                            int searchType,
                            OctetString * agentResCtlDomainListName)
{
  static agentResCtlDomainListEntry_t agentResCtlDomainListEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_len;
  
  if (firstTime == L7_TRUE )
  {
    agentResCtlDomainListEntryData.agentResCtlDomainListName = MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }
  
  ZERO_VALID(agentResCtlDomainListEntryData.valid);

  memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer, agentResCtlDomainListName->octet_ptr, agentResCtlDomainListName->length);
  snmp_buffer_len = agentResCtlDomainListName->length;


  if ((searchType == EXACT) ? (usmDbDNSClientDomainNameListGet(snmp_buffer)!= L7_SUCCESS) :
                              ((usmDbDNSClientDomainNameListGet(snmp_buffer)!= L7_SUCCESS) &&
                               (usmDbDNSClientDomainNameListGetNext(snmp_buffer)!= L7_SUCCESS)))
  {
     ZERO_VALID(agentResCtlDomainListEntryData.valid);
     return(NULL);
  }
  
  if (SafeMakeOctetString(&agentResCtlDomainListEntryData.agentResCtlDomainListName, snmp_buffer, strlen(snmp_buffer)) != L7_TRUE)
  {
    ZERO_VALID(agentResCtlDomainListEntryData.valid);
    return(NULL);
  }
  SET_VALID(I_agentResCtlDomainListName, agentResCtlDomainListEntryData.valid);
  
  switch (nominator)
  {
    case -1:
    case I_agentResCtlDomainListName:
      break;

    case I_agentResCtlDomainListNameStatus:  
      
      agentResCtlDomainListEntryData.agentResCtlDomainListNameStatus = D_agentResCtlDomainListNameStatus_active;
      SET_VALID(I_agentResCtlDomainListNameStatus, agentResCtlDomainListEntryData.valid);
      break;

    default:
      /* unknown nominator */
      return(NULL);
      break;
  }

  if (nominator != -1 && !VALID(nominator, agentResCtlDomainListEntryData.valid))
    return(NULL);

  return(&agentResCtlDomainListEntryData);
}
#ifdef SETS
int
k_agentResCtlDomainListEntry_test(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentResCtlDomainListEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                              doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentResCtlDomainListEntry_set_defaults(doList_t *dp)
{
    agentResCtlDomainListEntry_t *data = (agentResCtlDomainListEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentResCtlDomainListEntry_set(agentResCtlDomainListEntry_t *data,
                            ContextInfo *contextInfo, int function)
{
   L7_char8 tempValid[sizeof(data->valid)];
   L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

   bzero(tempValid, sizeof(tempValid));
   bzero(snmp_buffer, SNMP_BUFFER_LEN);

  memcpy(snmp_buffer, data->agentResCtlDomainListName->octet_ptr, 
                      data->agentResCtlDomainListName->length);

  
   if (VALID(I_agentResCtlDomainListNameStatus, data->valid))
   {
     if (data->agentResCtlDomainListNameStatus == D_agentResCtlDomainListNameStatus_createAndGo)
     if ( usmDbDNSClientDomainNameListSet(snmp_buffer) != L7_SUCCESS)
     {
       memcpy(data->valid, tempValid, sizeof(tempValid));
       return(COMMIT_FAILED_ERROR);
     }
     if (data->agentResCtlDomainListNameStatus == D_agentResCtlDomainListNameStatus_destroy)
     {
       if (usmDbDNSClientDomainNameListRemove(snmp_buffer)  != L7_SUCCESS)
       {
         memcpy(data->valid, tempValid, sizeof(tempValid));
         return(COMMIT_FAILED_ERROR);
       }
     }
     SET_VALID(I_agentResCtlDomainListNameStatus, tempValid);
   }

    
   return NO_ERROR;
}

#ifdef SR_agentResCtlDomainListEntry_UNDO
/* add #define SR_agentResCtlDomainListEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentResCtlDomainListEntry family.
 */
int
agentResCtlDomainListEntry_undo(doList_t *doHead, doList_t *doCur,
                           ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentResCtlDomainListEntry_UNDO */

#endif /* SETS */

agentResCtlConfigIPEntry_t *
k_agentResCtlConfigIPEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator,
                          int searchType,
                          SR_UINT32 agentResCtlDnsNameServerIP)
{
  static agentResCtlConfigIPEntry_t agentResCtlConfigIPEntryData;
  L7_inet_addr_t serverAddr;

  inetAddressReset(&serverAddr);
  agentResCtlConfigIPEntryData.agentResCtlDnsNameServerIP = agentResCtlDnsNameServerIP;
  SET_VALID(I_agentResCtlDnsNameServerIP, agentResCtlConfigIPEntryData.valid);
  /* supports IPv4 only */
  inetAddressSet(L7_AF_INET, &agentResCtlConfigIPEntryData.agentResCtlDnsNameServerIP, &serverAddr);
  if ((searchType == EXACT) ?
      (usmDbDNSClientNameServerEntryGet (&serverAddr) != L7_SUCCESS) :
      ((usmDbDNSClientNameServerEntryGet (&serverAddr) != L7_SUCCESS) &&
       (usmDbDNSClientNameServerEntryNextGet (&serverAddr) != L7_SUCCESS)))
   {
    ZERO_VALID(agentResCtlConfigIPEntryData.valid);
    return(NULL);
   }
  /* updating for server address */
  inetAddressGet(L7_AF_INET, &serverAddr, &agentResCtlConfigIPEntryData.agentResCtlDnsNameServerIP);
  SET_VALID(I_agentResCtlDnsNameServerIP, agentResCtlConfigIPEntryData.valid);
   
  switch (nominator)
  {
  case -1:
  case I_agentResCtlDnsNameServerIP:
    if (nominator != -1)
      break;

  case I_agentResCtlDnsNameServerStatus:
    if(usmDbDNSClientNameServerEntryGet (&serverAddr) == L7_SUCCESS) 
    {
      agentResCtlConfigIPEntryData.agentResCtlDnsNameServerStatus = 
                         D_agentResCtlDnsNameServerStatus_active;
      SET_VALID(I_agentResCtlDnsNameServerStatus, agentResCtlConfigIPEntryData.valid);
    }
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }
  if (nominator != -1 && !VALID(nominator, agentResCtlConfigIPEntryData.valid))
    return(NULL);
  return(&agentResCtlConfigIPEntryData);

}

#ifdef SETS
int
k_agentResCtlConfigIPEntry_test(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentResCtlConfigIPEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                            doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentResCtlConfigIPEntry_set_defaults(doList_t *dp)
{
    agentResCtlConfigIPEntry_t *data = (agentResCtlConfigIPEntry_t *) (dp->data);
    data->agentResCtlDnsNameServerStatus = D_agentResCtlDnsNameServerStatus_notInService;

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentResCtlConfigIPEntry_set(agentResCtlConfigIPEntry_t *data,
                          ContextInfo *contextInfo, int function)
{
 /*
  * Defining temporary variable for storing the valid bits for the case when the
  * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  L7_inet_addr_t serverAddr;
                                                                                
  bzero(tempValid, sizeof(tempValid));
  inetAddressReset(&serverAddr);
    
  if (VALID(I_agentResCtlDnsNameServerStatus, data->valid))
  {
    /* only IPv4 mib support */
    inetAddressSet(L7_AF_INET, &data->agentResCtlDnsNameServerIP, &serverAddr);
    /*
     * If row status is set to delete and row is not existent return error
     */
    if ((data->agentResCtlDnsNameServerStatus == D_agentResCtlDnsNameServerStatus_destroy) 
        && (usmDbDNSClientNameServerEntryRemove(&serverAddr) != L7_SUCCESS))
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    /*
     * If row status is set to create, create the row and set the status to active
     */
    else if (data->agentResCtlDnsNameServerStatus == 
                         D_agentResCtlDnsNameServerStatus_createAndGo) 
    {
      if(usmDbDNSClientNameServerEntryAdd(&serverAddr) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(tempValid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        data->agentResCtlDnsNameServerStatus = 
                                     D_agentResCtlDnsNameServerStatus_active;
      }
    }
    else if((data->agentResCtlDnsNameServerStatus != D_agentResCtlDnsNameServerStatus_destroy) &&
            (data->agentResCtlDnsNameServerStatus != D_agentResCtlDnsNameServerStatus_createAndGo))
    {
      return(COMMIT_FAILED_ERROR);
    }
                                                                                
    SET_VALID(I_agentResCtlDnsNameServerStatus, tempValid);
  }
  return NO_ERROR;

}

#ifdef SR_agentResCtlConfigIPEntry_UNDO
/* add #define SR_agentResCtlConfigIPEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentResCtlConfigIPEntry family.
 */
int
agentResCtlConfigIPEntry_undo(doList_t *doHead, doList_t *doCur,
                         ContextInfo *contextInfo)
{
  agentResCtlConfigIPEntry_t *data = (agentResCtlConfigIPEntry_t *) doCur->data;
  agentResCtlConfigIPEntry_t  *undodata = (agentResCtlConfigIPEntry_t *) doCur->undodata;
  agentResCtlConfigIPEntry_t  *setdata = undodata;
  L7_int32 function = SR_ADD_MODIFY;

  if ( data == NULL || undodata == NULL)
    return(UNDO_FAILED_ERROR);
   
  memcpy(undodata->valid, data->valid, sizeof(data->valid));

  /*Copy valid bits from data to undodata */
  if( undodata != NULL && data != NULL )
    memcpy(undodata->valid,data->valid,sizeof(data->valid));
                                                                        
  /* we are either trying to undo an add,or  a delete */
  if ( undodata == NULL )   {
    /* undoing an add, so delete */
    data->agentResCtlDnsNameServerStatus = D_agentResCtlDnsNameServerStatus_destroy;
    setdata = data;
    function = SR_DELETE;
  }
  else
  {
    /* undoing a delete */
    if(undodata->agentResCtlDnsNameServerStatus == D_agentResCtlStaticNameServerStatus_active)
    {
      undodata->agentResCtlDnsNameServerStatus = D_agentResCtlDnsNameServerStatus_createAndGo;
    }

    if (data->agentResCtlDnsNameServerStatus == D_agentResCtlDnsNameServerStatus_destroy)
    {
      /* Setting all valid bits to restore previous values */
      memset(undodata->valid, 0xff, sizeof(undodata->valid));
    }
                                                                        
    setdata = undodata;
    function = SR_ADD_MODIFY;
 }
    
  /* use the set method for the undo */
  if ((setdata != NULL) && 
     (k_agentResCtlConfigIPEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;

}
#endif /* SR_agentResCtlConfigIPEntry_UNDO */

#endif /* SETS */


agentResCtlStaticServEntry_t *
k_agentResCtlStaticServEntry_get(int serialNum, ContextInfo *contextInfo,
                            int nominator,
                            int searchType,
                            OctetString * agentResCtlStaticHostName,
                            SR_UINT32 agentResCtlStaticIPAddress)
{
  static agentResCtlStaticServEntry_t agentResCtlStaticServEntryData;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_inet_addr_t  inetAddr;

  inetAddressReset(&inetAddr);
  bzero(snmp_buffer, SNMP_BUFFER_LEN);

  memcpy(snmp_buffer, agentResCtlStaticHostName->octet_ptr, 
		      agentResCtlStaticHostName->length);

  agentResCtlStaticServEntryData.agentResCtlStaticIPAddress = agentResCtlStaticIPAddress;
  inetAddressSet(L7_AF_INET, &agentResCtlStaticServEntryData.agentResCtlStaticIPAddress, &inetAddr);
  SET_VALID(I_agentResCtlStaticIPAddress, agentResCtlStaticServEntryData.valid);


  if ((searchType == EXACT) ?
      (usmDbDNSClientStaticHostEntryGet(snmp_buffer, &inetAddr) != L7_SUCCESS) :
      ((usmDbDNSClientStaticHostEntryGet
          (snmp_buffer, &inetAddr) != L7_SUCCESS) &&
    (snmpDNSClientStaticHostEntryNextGet(snmp_buffer, &inetAddr) != L7_SUCCESS)))
  {
    ZERO_VALID(agentResCtlStaticServEntryData.valid);
    return(NULL);
  }
  else
  {
    if(SafeMakeOctetStringFromTextExact( 
         &agentResCtlStaticServEntryData.agentResCtlStaticHostName, snmp_buffer) == L7_TRUE)
    {
      SET_VALID(I_agentResCtlStaticHostName, agentResCtlStaticServEntryData.valid);
    }
    else
    {
      ZERO_VALID(agentResCtlStaticServEntryData.valid);
      return(NULL);
    }
  }
  inetAddressGet(L7_AF_INET, &inetAddr, &agentResCtlStaticServEntryData.agentResCtlStaticIPAddress);
  
  switch (nominator)
  {
  case -1:
  case I_agentResCtlStaticHostName:
  case I_agentResCtlStaticIPAddress:
    if (nominator != -1)
      break;

  case I_agentResCtlStaticNameServerStatus:
    agentResCtlStaticServEntryData.agentResCtlStaticNameServerStatus = 
                         D_agentResCtlStaticNameServerStatus_active;
    SET_VALID(I_agentResCtlStaticNameServerStatus, agentResCtlStaticServEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != -1 && !VALID(nominator, agentResCtlStaticServEntryData.valid))
    return(NULL);

   return(&agentResCtlStaticServEntryData);
}

#ifdef SETS
int
k_agentResCtlStaticServEntry_test(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentResCtlStaticServEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                              doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentResCtlStaticServEntry_set_defaults(doList_t *dp)
{
    agentResCtlStaticServEntry_t *data = (agentResCtlStaticServEntry_t *) (dp->data);


    data->agentResCtlStaticNameServerStatus = 
                    D_agentResCtlStaticNameServerStatus_notInService;

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentResCtlStaticServEntry_set(agentResCtlStaticServEntry_t *data,
                            ContextInfo *contextInfo, int function)
{
 /*
  * Defining temporary variable for storing the valid bits for the case when the
  * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_inet_addr_t inetAddr;
                                                                                
  bzero(tempValid, sizeof(tempValid));
  bzero(snmp_buffer, SNMP_BUFFER_LEN);
  inetAddressReset(&inetAddr);

  if (VALID(I_agentResCtlStaticNameServerStatus, data->valid))
  {
    memcpy(snmp_buffer, data->agentResCtlStaticHostName->octet_ptr,
                      data->agentResCtlStaticHostName->length); 
    /*
     * If row status is set to delete and row is not existent return error
     */
    if (data->agentResCtlStaticNameServerStatus == 
                 D_agentResCtlStaticNameServerStatus_destroy) 
    {
      if(usmDbDNSClientStaticHostEntryRemove(snmp_buffer, L7_AF_INET) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(tempValid));
        return(COMMIT_FAILED_ERROR);
      }
    }
    /*
     * If row status is set to create, create the row and set the status to active
     */
    else if (data->agentResCtlStaticNameServerStatus == 
                         D_agentResCtlStaticNameServerStatus_createAndGo) 
    {
      inetAddressSet(L7_AF_INET, &data->agentResCtlStaticIPAddress, &inetAddr);
      if(usmDbDNSClientStaticHostEntryAdd(snmp_buffer, &inetAddr)
                                          != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(tempValid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        data->agentResCtlStaticNameServerStatus = 
                                     D_agentResCtlStaticNameServerStatus_active;
      }
    }
    else
    {
      return(COMMIT_FAILED_ERROR);
    }
    SET_VALID(I_agentResCtlStaticNameServerStatus, tempValid);
  }

  return NO_ERROR;

}

#ifdef SR_agentResCtlStaticServEntry_UNDO
/* add #define SR_agentResCtlStaticServEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentResCtlStaticServEntry family.
 */
int
agentResCtlStaticServEntry_undo(doList_t *doHead, doList_t *doCur,
                           ContextInfo *contextInfo)
{
  agentResCtlStaticServEntry_t *data = (agentResCtlStaticServEntry_t *) doCur->data;
  agentResCtlStaticServEntry_t  *undodata = (agentResCtlStaticServEntry_t *) doCur->undodata;
  agentResCtlStaticServEntry_t  *setdata = undodata;
  L7_int32 function = SR_ADD_MODIFY;

  if ( data == NULL || undodata == NULL)
    return(UNDO_FAILED_ERROR);

  memcpy(undodata->valid, data->valid, sizeof(data->valid));

  /*Copy valid bits from data to undodata */
  if( undodata != NULL && data != NULL )
    memcpy(undodata->valid,data->valid,sizeof(data->valid));
                                                                        
  /* we are either trying to undo an add,or  a delete */
  if ( undodata == NULL )   {
    /* undoing an add, so delete */
    data->agentResCtlDnsNameServerStatus = D_agentResCtlStaticNameServerStatus_destroy;
    setdata = data;
    function = SR_DELETE;
  }
  else
  {
    /* undoing a delete */
    if(undodata->agentResCtlStaticNameServerStatus == D_agentResCtlStaticNameServerStatus_active)
    {
      undodata->agentResCtlStaticNameServerStatus = D_agentResCtlStaticNameServerStatus_createAndGo;
    }

    if (data->agentResCtlStaticNameServerStatus == D_agentResCtlDnsNameServerStatus_destroy)
    {
      /* Setting all valid bits to restore previous values */
      memset(undodata->valid, 0xff, sizeof(undodata->valid));
    }
                                                                        
    setdata = undodata;
    function = SR_ADD_MODIFY;
 }
                                                                        
  /* use the set method for the undo */
  if ((setdata != NULL) && 
     (k_agentResCtlStaticServEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;
                                                                        
  return UNDO_FAILED_ERROR;

}
#endif /* SR_agentResCtlStaticServEntry_UNDO */

#endif /* SETS */

