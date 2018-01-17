/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: k_mib_tacacsclient.c
*
* Purpose: System-specific code for TACACS Private MIB
*
* Created by: nramesh 08/19/2005
*
* Component: SNMP
*
*********************************************************************/

#include <k_private_base.h>
#include <sr_ip.h>
#include "k_mib_tacacsclient_api.h"
#include "usmdb_tacacs_api.h"
#include "osapi_support.h"

agentTacacsGlobalConfigGroup_t *
k_agentTacacsGlobalConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator)
{
  static agentTacacsGlobalConfigGroup_t agentTacacsGlobalConfigGroupData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 tempTimeout[SNMP_BUFFER_LEN];
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  if (firstTime == L7_TRUE)
  {
    agentTacacsGlobalConfigGroupData.agentTacacsGlobalKey = MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }

  switch(nominator)
  {
    case -1:
    case I_agentTacacsGlobalTimeout:
      memset(tempTimeout, 0, SNMP_BUFFER_LEN);
      if(usmDbTacacsGblTimeOutGet(tempTimeout) == L7_SUCCESS)
      {
        agentTacacsGlobalConfigGroupData.agentTacacsGlobalTimeout = tempTimeout[0];
	SET_VALID(I_agentTacacsGlobalTimeout, agentTacacsGlobalConfigGroupData.valid);
      }
      if (nominator != -1)
	break;
    case I_agentTacacsGlobalKey:
      memset( snmp_buffer, 0, SNMP_BUFFER_LEN);
      if (usmDbTacacsGblKeyGet(snmp_buffer) == L7_SUCCESS &&
	  (SafeMakeOctetStringFromTextExact(&agentTacacsGlobalConfigGroupData.agentTacacsGlobalKey, snmp_buffer) == L7_TRUE))
	SET_VALID(I_agentTacacsGlobalKey, agentTacacsGlobalConfigGroupData.valid);
      break;
    default:
      return(NULL);
      break;
  }
  if (nominator != -1 && 
        !VALID(nominator, agentTacacsGlobalConfigGroupData.valid))
    return(NULL);
  return(&agentTacacsGlobalConfigGroupData);
}

#ifdef SETS
int
k_agentTacacsGlobalConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                    doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentTacacsGlobalConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                                     doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentTacacsGlobalConfigGroup_set(agentTacacsGlobalConfigGroup_t *data,
                                   ContextInfo *contextInfo, int function)
{
  L7_char8 tempValid[sizeof(data->valid)];
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  memset(tempValid, 0 , sizeof(tempValid));

  if (VALID(I_agentTacacsGlobalTimeout, data->valid))
  {
    if (usmDbTacacsGblTimeOutSet((L7_uchar8)data->agentTacacsGlobalTimeout) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }

    SET_VALID(I_agentTacacsGlobalTimeout, tempValid);
  }

  if (VALID(I_agentTacacsGlobalKey, data->valid))
  {
    memset(snmp_buffer, 0 , SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentTacacsGlobalKey->octet_ptr, data->agentTacacsGlobalKey->length);
    if (usmDbTacacsGblKeySet(snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(tempValid));
      return(COMMIT_FAILED_ERROR);
    }
    SET_VALID(I_agentTacacsGlobalKey, tempValid);
  }
  
  return NO_ERROR;
}

#ifdef SR_agentTacacsGlobalConfigGroup_UNDO
/* add #define SR_agentTacacsGlobalConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentTacacsGlobalConfigGroup family.
 */
int
agentTacacsGlobalConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                                  ContextInfo *contextInfo)
{
   agentTacacsGlobalConfigGroup_t *data = (agentTacacsGlobalConfigGroup_t *) doCur->data;
   agentTacacsGlobalConfigGroup_t *undodata = (agentTacacsGlobalConfigGroup_t *) doCur->undodata;
   agentTacacsGlobalConfigGroup_t *setdata = NULL;
   L7_int32 function = SR_UNKNOWN;

   if ( data == NULL || undodata == NULL )
	   return UNDO_FAILED_ERROR;
   memcpy(undodata->valid,data->valid,sizeof(data->valid));

   setdata = undodata;
   function = SR_ADD_MODIFY;

   if ((setdata != NULL) &&
       (k_agentTacacsGlobalConfigGroup_set(setdata, contextInfo,
					   function) == NO_ERROR))
	   return NO_ERROR;
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentTacacsGlobalConfigGroup_UNDO */

#endif /* SETS */

agentTacacsServerEntry_t *
k_agentTacacsServerEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             OctetString * agentTacacsServerIpAddress)
{
  static agentTacacsServerEntry_t  agentTacacsServerEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 tempTimeout[SNMP_BUFFER_LEN];
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uchar8 ipAddress[SNMP_BUFFER_LEN];
  L7_uint32 type;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentTacacsServerEntryData.agentTacacsKey = MakeOctetString(NULL, 0); 
    agentTacacsServerEntryData.agentTacacsServerIpAddress = MakeOctetString(NULL, 0); 
  }
  ZERO_VALID(agentTacacsServerEntryData.valid);

  FreeOctetString(agentTacacsServerEntryData.agentTacacsServerIpAddress);
  agentTacacsServerEntryData.agentTacacsServerIpAddress = CloneOctetString(agentTacacsServerIpAddress);
  SET_VALID(I_agentTacacsServerIpAddress, agentTacacsServerEntryData.valid);

  if((searchType == EXACT)?(snmpTacacsServerGet(agentTacacsServerEntryData.agentTacacsServerIpAddress,&type, ipAddress) != L7_SUCCESS):
                           (snmpTacacsServerGet(agentTacacsServerEntryData.agentTacacsServerIpAddress,&type, ipAddress) != L7_SUCCESS)&&
                           (snmpTacacsServerNextGet(agentTacacsServerEntryData.agentTacacsServerIpAddress,
                                                    &agentTacacsServerEntryData.agentTacacsServerIpAddress, &type, ipAddress) != L7_SUCCESS))
  {
    ZERO_VALID(agentTacacsServerEntryData.valid);
    return (NULL);
  }

  switch (nominator)
  {
    case -1:
    case I_agentTacacsServerIpAddress:
      if (nominator != -1)
        break;
    case I_agentTacacsServerIpAddrType:
      agentTacacsServerEntryData.agentTacacsServerIpAddrType = type;
      SET_VALID(I_agentTacacsServerIpAddrType, agentTacacsServerEntryData.valid);   
      if (nominator != -1)
        break;
    case I_agentTacacsPortNumber:
      if (snmpTacacsPortNumberGet(ipAddress,type, 
                    &agentTacacsServerEntryData.agentTacacsPortNumber)
                                                             == L7_SUCCESS)
        SET_VALID(I_agentTacacsPortNumber, agentTacacsServerEntryData.valid);
      if (nominator != -1)
        break;
    case I_agentTacacsTimeOut:
      memset(tempTimeout, 0, SNMP_BUFFER_LEN);
      if (snmpTacacsTimeOutGet(ipAddress,type, tempTimeout) == L7_SUCCESS)
      {
        agentTacacsServerEntryData.agentTacacsTimeOut = tempTimeout[0];
        SET_VALID(I_agentTacacsTimeOut, agentTacacsServerEntryData.valid);
      }
      if (nominator != -1)
        break;
    case I_agentTacacsKey:
      memset( snmp_buffer, 0, SNMP_BUFFER_LEN);
      if (snmpTacacsKeyGet(ipAddress,type, snmp_buffer) == L7_SUCCESS && 
          (SafeMakeOctetStringFromTextExact(&agentTacacsServerEntryData.agentTacacsKey, snmp_buffer) == L7_TRUE))
        SET_VALID(I_agentTacacsKey, agentTacacsServerEntryData.valid);
      if (nominator != -1)
        break;
    case I_agentTacacsPriority:
      if (snmpTacacsPriorityGet(ipAddress,type, 
                    &agentTacacsServerEntryData.agentTacacsPriority)
                                                             == L7_SUCCESS)
        SET_VALID(I_agentTacacsPriority, agentTacacsServerEntryData.valid);
      if (nominator != -1)
        break;
    case I_agentTacacsServerStatus:
      agentTacacsServerEntryData.agentTacacsServerStatus = D_agentTacacsServerStatus_active;
      SET_VALID(I_agentTacacsServerStatus, agentTacacsServerEntryData.valid);
      break;
    default:
    /* unknown nominator */
    return(NULL);
    break;
  }
  if (nominator >= 0 && !VALID(nominator, agentTacacsServerEntryData.valid))
    return(NULL);
  return (&agentTacacsServerEntryData);
}

#ifdef SETS
int
k_agentTacacsServerEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentTacacsServerEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                               doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentTacacsServerEntry_set_defaults(doList_t *dp)
{
    agentTacacsServerEntry_t *data = (agentTacacsServerEntry_t *) (dp->data);

    data->agentTacacsPortNumber = SNMP_TACACS_DEFAULT_PORT_NUM;
    if ((data->agentTacacsKey = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->agentTacacsPriority = SNMP_TACACS_DEFAULT_PRIORITY;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentTacacsServerEntry_set(agentTacacsServerEntry_t *data,
                             ContextInfo *contextInfo, int function)
{
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_char8 tempValid[sizeof(data->valid)];
  L7_uint32 ipAddress = 0;
  L7_uint32 addressType;
  L7_uchar8 ipAddress_str[SNMP_BUFFER_LEN];
  memset(tempValid, 0, sizeof(tempValid));

  memset(ipAddress_str,0,sizeof(ipAddress));
  if (VALID(I_agentTacacsServerIpAddrType, data->valid) &&
      data->agentTacacsServerIpAddrType == D_agentTacacsServerIpAddrType_ipv4 &&
      data->agentTacacsServerIpAddress->length == sizeof(ipAddress))
  {
    /* Convert octet string to uint32 ip address, then to dotted decimal */
    memcpy(&ipAddress, data->agentTacacsServerIpAddress->octet_ptr, sizeof(ipAddress));
    osapiStrncpy(ipAddress_str, osapiInet_ntoa(ipAddress), sizeof(ipAddress_str));
    addressType = data->agentTacacsServerIpAddrType;
  }
  else if (VALID(I_agentTacacsServerIpAddrType, data->valid) &&
           data->agentTacacsServerIpAddrType == D_agentTacacsServerIpAddrType_dns &&
           data->agentTacacsServerIpAddress->length)
  {
    osapiStrncpy(ipAddress_str, data->agentTacacsServerIpAddress->octet_ptr, data->agentTacacsServerIpAddress->length);
    addressType = data->agentTacacsServerIpAddrType;
  }
  else
  {
    if (snmpTacacsServerGet(data->agentTacacsServerIpAddress, &addressType, ipAddress_str) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
  }

  if(VALID(I_agentTacacsServerStatus, data->valid))
  {
    if(snmpTacacsRowStatusSet(ipAddress_str, addressType, data->agentTacacsServerStatus) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentTacacsServerStatus, tempValid);
    }
  }
  if(VALID(I_agentTacacsPortNumber, data->valid))
  {
    if(snmpTacacsPortNumberSet(ipAddress_str,addressType, data->agentTacacsPortNumber) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentTacacsPortNumber, tempValid);
    }
  }
  if(VALID(I_agentTacacsTimeOut, data->valid))
  {
    if(snmpTacacsTimeOutSet(ipAddress_str, addressType, (L7_uchar8)data->agentTacacsTimeOut) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentTacacsTimeOut, tempValid);
    }
  }
  if(VALID(I_agentTacacsKey, data->valid))
  {
    memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentTacacsKey->octet_ptr, data->agentTacacsKey->length);
    if(snmpTacacsKeySet(ipAddress_str,addressType, snmp_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentTacacsKey, tempValid);
    }
  }
  if(VALID(I_agentTacacsPriority, data->valid))
  {
    if(snmpTacacsPrioritySet(ipAddress_str,addressType, data->agentTacacsPriority) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentTacacsPriority, tempValid);
    }
  }
   return NO_ERROR;
}

#ifdef SR_agentTacacsServerEntry_UNDO
/* add #define SR_agentTacacsServerEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentTacacsServerEntry family.
 */
int
agentTacacsServerEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
   agentTacacsServerEntry_t *data = (agentTacacsServerEntry_t *) doCur->data;
   agentTacacsServerEntry_t *undodata = (agentTacacsServerEntry_t *) doCur->undodata;
   agentTacacsServerEntry_t *setdata = NULL;
   L7_int32 function = SR_UNKNOWN;

   if ( data == NULL || undodata == NULL )
	   return UNDO_FAILED_ERROR;
   memcpy(undodata->valid,data->valid,sizeof(data->valid));

   setdata = undodata;
   function = SR_ADD_MODIFY;

   if ((setdata != NULL) &&
       (k_agentTacacsServerEntry_set(setdata, contextInfo,
					   function) == NO_ERROR))
	   return NO_ERROR;
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentTacacsServerEntry_UNDO */

#endif /* SETS */

