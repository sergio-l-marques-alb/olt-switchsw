/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: k_mib_radius.c
*
* Purpose: System-specific code for Radius Private MIB
*
* Created by: Prashant Murthy 05/07/2003
*
* Component: SNMP
*
*********************************************************************/ 

#include <k_private_base.h>
#include <k_mib_radius_api.h>
#include "usmdb_common.h"
#include "radius_exports.h"
#include "usmdb_radius_api.h"
#include "usmdb_util_api.h"
#include "usmdb_dot1x_api.h"

agentRadiusConfigGroup_t *
k_agentRadiusConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                             int nominator)
{
  static agentRadiusConfigGroup_t agentRadiusConfigGroupData;  
  L7_uchar8 strIpAddr[20];
  L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus;
  L7_uint32 local_flag = L7_RADIUSGLOBAL;
  L7_uint32 temp_uint;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  
  ZERO_VALID(agentRadiusConfigGroupData.valid);

  usmDbInetNtoa(ALL_RADIUS_SERVERS, strIpAddr);

  switch(nominator)
  {
#ifdef I_agentRadiusMaxTransmit
    case I_agentRadiusMaxTransmit:
    if (usmDbRadiusServerRetransGet(strIpAddr, 
                                    &agentRadiusConfigGroupData.agentRadiusMaxTransmit,
                                    &paramStatus) == L7_SUCCESS)

      SET_VALID(I_agentRadiusMaxTransmit, agentRadiusConfigGroupData.valid);
    break;
#endif
#ifdef I_agentRadiusRetransmit
    case I_agentRadiusRetransmit:
    if (usmDbRadiusServerRetransGet(strIpAddr, 
                                    &agentRadiusConfigGroupData.agentRadiusRetransmit,
                                    &paramStatus) == L7_SUCCESS)

      SET_VALID(I_agentRadiusRetransmit, agentRadiusConfigGroupData.valid);
    break;
#endif

  case I_agentRadiusAuthenticationServers:
    if (usmDbRadiusConfiguredServersCountGet(USMDB_UNIT_CURRENT, 
                     RADIUS_SERVER_TYPE_AUTH, &agentRadiusConfigGroupData.agentRadiusAuthenticationServers) == L7_SUCCESS )
      SET_VALID(I_agentRadiusAuthenticationServers, agentRadiusConfigGroupData.valid);
    break;

  case I_agentRadiusAccountingServers:
    if (usmDbRadiusConfiguredServersCountGet(USMDB_UNIT_CURRENT, 
                     RADIUS_SERVER_TYPE_ACCT, &agentRadiusConfigGroupData.agentRadiusAccountingServers) == L7_SUCCESS )
      SET_VALID(I_agentRadiusAccountingServers, agentRadiusConfigGroupData.valid);
    break;

  case I_agentRadiusNamedAuthenticationServerGroups:
    if (usmDbRadiusConfiguredNamedServersGroupCountGet(USMDB_UNIT_CURRENT, 
                     RADIUS_SERVER_TYPE_AUTH, &agentRadiusConfigGroupData.agentRadiusNamedAuthenticationServerGroups) == L7_SUCCESS )
      SET_VALID(I_agentRadiusNamedAuthenticationServerGroups, agentRadiusConfigGroupData.valid);
    break;

  case I_agentRadiusNamedAccountingServerGroups:
    if (usmDbRadiusConfiguredNamedServersGroupCountGet(USMDB_UNIT_CURRENT, 
                     RADIUS_SERVER_TYPE_ACCT, &agentRadiusConfigGroupData.agentRadiusNamedAccountingServerGroups) == L7_SUCCESS )
      SET_VALID(I_agentRadiusNamedAccountingServerGroups, agentRadiusConfigGroupData.valid);
    break;

  case I_agentRadiusTimeout:
    if (usmDbRadiusServerTimeOutGet(strIpAddr, 
                                    &agentRadiusConfigGroupData.agentRadiusTimeout,
                                    &paramStatus) == L7_SUCCESS)

      SET_VALID(I_agentRadiusTimeout, agentRadiusConfigGroupData.valid);
    break;
  
  case I_agentRadiusAccountingMode:
    if (snmpAgentRadiusAccountingModeGet(USMDB_UNIT_CURRENT, 
                                    &agentRadiusConfigGroupData.agentRadiusAccountingMode) == L7_SUCCESS)
      SET_VALID(I_agentRadiusAccountingMode, agentRadiusConfigGroupData.valid);
    break;

  case I_agentRadiusStatsClear:
    agentRadiusConfigGroupData.agentRadiusStatsClear = D_agentRadiusStatsClear_disable;
    SET_VALID(I_agentRadiusStatsClear, agentRadiusConfigGroupData.valid);
    break;

  case I_agentRadiusAccountingIndexNextValid:
    if (snmpAgentRadiusAccountingIndexNextValidGet(USMDB_UNIT_CURRENT, 
                                               &agentRadiusConfigGroupData.agentRadiusAccountingIndexNextValid) == L7_SUCCESS)
      SET_VALID(I_agentRadiusAccountingIndexNextValid, agentRadiusConfigGroupData.valid);
    break;

  case I_agentRadiusServerIndexNextValid:
    if (snmpAgentRadiusServerIndexNextValidGet(USMDB_UNIT_CURRENT, 
                                               &agentRadiusConfigGroupData.agentRadiusServerIndexNextValid) == L7_SUCCESS)
      SET_VALID(I_agentRadiusServerIndexNextValid, agentRadiusConfigGroupData.valid);
    break;

  case I_agentRadiusDeadTime:
     if(usmDbRadiusServerDeadtimeGet("0.0.0.0",
                                     &agentRadiusConfigGroupData.agentRadiusDeadTime,
                                     &local_flag ) == L7_SUCCESS )
     {
        SET_VALID(nominator, agentRadiusConfigGroupData.valid);
     }
    break;

  case I_agentRadiusServerKey:
     memset(snmp_buffer, 0, SNMP_BUFFER_LEN);
     if(usmDbRadiusAccountingHostNameServerSharedSecretGet(USMDB_UNIT_CURRENT, "0.0.0.0", L7_IP_ADDRESS_TYPE_IPV4, snmp_buffer) == L7_SUCCESS)
     {
        if(SafeMakeOctetStringFromTextExact(&agentRadiusConfigGroupData.agentRadiusServerKey,
                                            "******") == L7_TRUE)
        {
           SET_VALID(nominator ,agentRadiusConfigGroupData.valid);
        }
        else
        {
           agentRadiusConfigGroupData.agentRadiusServerKey = MakeOctetString(NULL, 0);
           SET_VALID(nominator, agentRadiusConfigGroupData.valid);
        }
     }
     break;
  case I_agentRadiusSourceIPAddr:
     if(usmDbRadiusServerSourceIPGet("0.0.0.0",
                                     &agentRadiusConfigGroupData.agentRadiusSourceIPAddr,
                                     &local_flag) == L7_SUCCESS)
     {
       SET_VALID(nominator, agentRadiusConfigGroupData.valid);
     }
     break;

  case I_agentRadiusNasIpAddress:
     if(usmDbRadiusAttribute4Get(USMDB_UNIT_CURRENT,&temp_uint,&agentRadiusConfigGroupData.agentRadiusNasIpAddress) == L7_SUCCESS)
     {
        SET_VALID(nominator, agentRadiusConfigGroupData.valid);
     }
     break;

  case I_agentAuthorizationNetworkRadiusMode:
     if(usmDbDot1xVlanAssignmentModeGet(USMDB_UNIT_CURRENT,&agentRadiusConfigGroupData.agentAuthorizationNetworkRadiusMode) == L7_SUCCESS)
     {
       SET_VALID(nominator, agentRadiusConfigGroupData.valid);
     }
     break;

  default:
    /* unknown nominator */
    return(NULL);
  }

  if ((nominator >= 0) && !VALID(nominator, agentRadiusConfigGroupData.valid))
     return(NULL);
  return(&agentRadiusConfigGroupData);
}

#ifdef SETS
int
k_agentRadiusConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentRadiusConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                               doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentRadiusConfigGroup_set(agentRadiusConfigGroup_t *data,
                             ContextInfo *contextInfo, int function)
{
  L7_uchar8 strIpAddr[20];
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  usmDbInetNtoa(ALL_RADIUS_SERVERS, strIpAddr);

#ifdef I_agentRadiusMaxTransmit
  if (VALID(I_agentRadiusMaxTransmit, data->valid) && 
      usmDbRadiusServerRetransSet(strIpAddr, data->agentRadiusMaxTransmit, L7_RADIUSGLOBAL) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;
#endif
#ifdef I_agentRadiusRetransmit
  if (VALID(I_agentRadiusRetransmit, data->valid) && 
      usmDbRadiusServerRetransSet(strIpAddr, data->agentRadiusRetransmit, L7_RADIUSGLOBAL) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;
#endif

  if (VALID(I_agentRadiusTimeout, data->valid) && 
      usmDbRadiusServerTimeOutSet(strIpAddr, data->agentRadiusTimeout, L7_RADIUSGLOBAL) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;
  
  if (VALID(I_agentRadiusAccountingMode, data->valid) && 
      snmpAgentRadiusAccountingModeSet(USMDB_UNIT_CURRENT, data->agentRadiusAccountingMode) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentRadiusStatsClear, data->valid) &&
      snmpAgentRadiusStatsClearSet(USMDB_UNIT_CURRENT, data->agentRadiusStatsClear) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  /* deadtime */
  if (VALID(I_agentRadiusDeadTime, data->valid) &&
      usmDbRadiusServerDeadtimeSet("0.0.0.0", data->agentRadiusDeadTime, L7_RADIUSGLOBAL) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  /* global radius server key agentRadiusServerKey */
  if (VALID(I_agentRadiusServerKey, data->valid) )
  {
    osapiStrncpy(snmp_buffer, data->agentRadiusServerKey->octet_ptr,
                 data->agentRadiusServerKey->length);
    if(usmDbRadiusAccountingHostNameServerSharedSecretSet(USMDB_UNIT_CURRENT, "0.0.0.0", L7_IP_ADDRESS_TYPE_IPV4, snmp_buffer) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
  }

  /* Radius source IP Address */
  if(VALID(I_agentRadiusSourceIPAddr, data->valid) && 
     usmDbRadiusServerSourceIPSet("0.0.0.0", data->agentRadiusSourceIPAddr, L7_RADIUSGLOBAL) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  /* Radius server attribute 4  */
  if (VALID(I_agentRadiusNasIpAddress, data->valid) &&
      L7_SUCCESS != usmDbRadiusAttribute4Set(USMDB_UNIT_CURRENT,                                               
                                             (0 != data->agentRadiusNasIpAddress) ? L7_TRUE : L7_FALSE,
                                             data->agentRadiusNasIpAddress))
    return COMMIT_FAILED_ERROR;

  /* Radius source IP Address */
  if(VALID(I_agentAuthorizationNetworkRadiusMode, data->valid) &&
     usmDbDot1xVlanAssignmentModeSet(USMDB_UNIT_CURRENT, data->agentAuthorizationNetworkRadiusMode) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  return NO_ERROR;
}

#ifdef SR_agentRadiusConfigGroup_UNDO
/* add #define SR_agentRadiusConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentRadiusConfigGroup family.
 */
int
agentRadiusConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentRadiusConfigGroup_UNDO */

#endif /* SETS */

agentRadiusAccountingConfigEntry_t *
k_agentRadiusAccountingConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                                       int nominator,
                                       int searchType,
                                       SR_INT32 agentRadiusAccountingServerIndex)
{
  static agentRadiusAccountingConfigEntry_t agentRadiusAccountingConfigEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_BOOL secretConfigured;
  L7_char8 name[L7_RADIUS_SERVER_NAME_LENGTH+1];

  if (firstTime == L7_TRUE)
  {
    agentRadiusAccountingConfigEntryData.agentRadiusAccountingSecret = MakeOctetString(NULL, 0);
    agentRadiusAccountingConfigEntryData.agentRadiusAccountingServerAddress = MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }

   
  ZERO_VALID(agentRadiusAccountingConfigEntryData.valid);
  agentRadiusAccountingConfigEntryData.agentRadiusAccountingServerIndex = agentRadiusAccountingServerIndex;
  memset (snmp_buffer,0,SNMP_BUFFER_LEN);
  SET_VALID(I_agentRadiusAccountingServerIndex, agentRadiusAccountingConfigEntryData.valid);
  
  if ((searchType == EXACT) ?
      (snmpAgentRadiusAccountingConfigEntryGet(USMDB_UNIT_CURRENT, agentRadiusAccountingConfigEntryData.agentRadiusAccountingServerIndex, 
                                               snmp_buffer, &addrType) != L7_SUCCESS) :
      (snmpAgentRadiusAccountingConfigEntryGet(USMDB_UNIT_CURRENT, agentRadiusAccountingConfigEntryData.agentRadiusAccountingServerIndex, 
                                              snmp_buffer, &addrType) != L7_SUCCESS &&
       snmpAgentRadiusAccountingConfigEntryNextGet(USMDB_UNIT_CURRENT, &agentRadiusAccountingConfigEntryData.agentRadiusAccountingServerIndex, 
                                                   snmp_buffer, &addrType) != L7_SUCCESS))
  {
    ZERO_VALID(agentRadiusAccountingConfigEntryData.valid);
    return(NULL);
  }
  switch(nominator)
  {
  case I_agentRadiusAccountingServerIndex:
    break;

  case I_agentRadiusAccountingServerAddress:
     if(SafeMakeOctetStringFromTextExact(&agentRadiusAccountingConfigEntryData.agentRadiusAccountingServerAddress, snmp_buffer) == L7_TRUE)
     {
       SET_VALID(I_agentRadiusAccountingServerAddress, agentRadiusAccountingConfigEntryData.valid);
     }
    break;

  case I_agentRadiusAccountingServerAddressType:
      agentRadiusAccountingConfigEntryData.agentRadiusAccountingServerAddressType = addrType;
      SET_VALID(I_agentRadiusAccountingServerAddressType, agentRadiusAccountingConfigEntryData.valid);
    break;

  case I_agentRadiusAccountingPort:
    if (usmDbRadiusAccountingHostNameServerPortNumGet(USMDB_UNIT_CURRENT, 
                                              snmp_buffer, addrType,
                                              &agentRadiusAccountingConfigEntryData.agentRadiusAccountingPort) == L7_SUCCESS)
      SET_VALID(I_agentRadiusAccountingPort, agentRadiusAccountingConfigEntryData.valid);
    break;

  case I_agentRadiusAccountingSecret:
    if ((usmDbRadiusAccountingHostNameServerSharedSecretConfigured(USMDB_UNIT_CURRENT,
        snmp_buffer, addrType, &secretConfigured) != L7_SUCCESS) 
         || secretConfigured == L7_FALSE)
    {
       break;
    }
    else
    {
      SET_VALID(I_agentRadiusAccountingSecret, agentRadiusAccountingConfigEntryData.valid);
    }
    break;

  case I_agentRadiusAccountingStatus:
    agentRadiusAccountingConfigEntryData.agentRadiusAccountingStatus = D_agentRadiusAccountingStatus_active;
    SET_VALID(I_agentRadiusAccountingStatus, agentRadiusAccountingConfigEntryData.valid);
    break;

  case I_agentRadiusAccountingServerName:
    if( (usmDbRadiusServerAcctHostNameGet(USMDB_UNIT_CURRENT, snmp_buffer, 
                     addrType, 
                     name) 
                     == L7_SUCCESS ) &&
      (SafeMakeOctetString(&agentRadiusAccountingConfigEntryData.agentRadiusAccountingServerName,
                    name, strlen(name)) == L7_TRUE) )    
    {
      SET_VALID(I_agentRadiusAccountingServerName, agentRadiusAccountingConfigEntryData.valid);
    }
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }
if (nominator != 0 && !VALID(nominator, agentRadiusAccountingConfigEntryData.valid))
  return(NULL);

return(&agentRadiusAccountingConfigEntryData);
}

#ifdef SETS
int
k_agentRadiusAccountingConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentRadiusAccountingConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                         doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentRadiusAccountingConfigEntry_set_defaults(doList_t *dp)
{
    agentRadiusAccountingConfigEntry_t *data = (agentRadiusAccountingConfigEntry_t *) (dp->data);

    if ((data->agentRadiusAccountingSecret = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentRadiusAccountingServerAddress = MakeOctetStringFromText("")) == 0){
              return RESOURCE_UNAVAILABLE_ERROR;
    }
    return NO_ERROR;
}

int
k_agentRadiusAccountingConfigEntry_set(agentRadiusAccountingConfigEntry_t *data,
                                       ContextInfo *contextInfo, int function)
{
  L7_uchar8 serverAddress[SNMP_BUFFER_LEN];
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_IP_ADDRESS_TYPE_t serverAddrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_uint32 nextValidIndex;
  L7_uint32 wait_time = 10; /* controls how long to wait for Radius server to process create request */
  L7_uint32 ipaddrCheck; 
  L7_BOOL nameConfigured = L7_FALSE;
  
   memset(serverAddress, 0, SNMP_BUFFER_LEN);
   if( VALID(I_agentRadiusAccountingStatus, data->valid))
   {
     if(data->agentRadiusAccountingStatus == 
                     D_agentRadiusAccountingStatus_createAndGo)
     {

       if(!VALID(I_agentRadiusAccountingServerIndex, data->valid) )
       {
         return COMMIT_FAILED_ERROR;
       }
                           
       if (snmpAgentRadiusAccountingConfigEntryGet(USMDB_UNIT_CURRENT, 
          data->agentRadiusAccountingServerIndex, serverAddress, &addrType) 
                                  != L7_SUCCESS)
       {
         if (snmpAgentRadiusAccountingIndexNextValidGet(USMDB_UNIT_CURRENT, 
                 &nextValidIndex) == L7_SUCCESS &&
             data->agentRadiusAccountingServerIndex == nextValidIndex) 
         {
            
           memset(serverAddress, 0, SNMP_BUFFER_LEN);
           memcpy(serverAddress, 
                  data->agentRadiusAccountingServerAddress->octet_ptr,
                  data->agentRadiusAccountingServerAddress->length);
          /* We should validate ipaddress. wheather it is dns name or dotted format ip address */
          if(usmDbIPHostAddressValidate(serverAddress,&ipaddrCheck, &serverAddrType) != L7_SUCCESS)
          {
            return COMMIT_FAILED_ERROR;
          }

           bzero(snmp_buffer,sizeof(snmp_buffer));
           if(VALID(I_agentRadiusAccountingServerName, data->valid))
           {             
             memcpy(snmp_buffer,data->agentRadiusAccountingServerName->octet_ptr,
                     data->agentRadiusAccountingServerName->length);             
             if(usmDbStringAlphaNumericCheck(snmp_buffer) != L7_SUCCESS)
             {
               return COMMIT_FAILED_ERROR;
             }
           }
           else
           {             
             memcpy(snmp_buffer,L7_RADIUS_SERVER_DEFAULT_NAME_ACCT,
                     strlen(L7_RADIUS_SERVER_DEFAULT_NAME_ACCT));
           }
           if(usmDbRadiusAccountingHostNameServerAdd(
              USMDB_UNIT_CURRENT, serverAddress, 
              serverAddrType,snmp_buffer) == L7_SUCCESS)
           {
             while (wait_time && 
                    snmpAgentRadiusAccountingConfigEntryGet(
                   USMDB_UNIT_CURRENT, data->agentRadiusAccountingServerIndex, 
                                      serverAddress, &addrType) != L7_SUCCESS)
             {
               /* sleep for a microsecond to give the Radius component 
                  time to process the add request */
               osapiSleepUSec(1);
               wait_time--;
             }
             nameConfigured = L7_TRUE;
           }

           else
             return COMMIT_FAILED_ERROR;
         }
         else
             return COMMIT_FAILED_ERROR;
       }
       else
         return COMMIT_FAILED_ERROR;
     }
     else if(data->agentRadiusAccountingStatus == 
                         D_agentRadiusAccountingStatus_destroy)
     {
       if(!VALID(I_agentRadiusAccountingServerIndex, data->valid)) 
       {
         return COMMIT_FAILED_ERROR;
       }
       memset(serverAddress, 0, SNMP_BUFFER_LEN);
       if( usmDbRadiusAccountingServerIPHostNameByIndexGet(USMDB_UNIT_CURRENT,
               data->agentRadiusAccountingServerIndex, serverAddress, &addrType)
              == L7_SUCCESS)
       {
         if((usmDbRadiusAccountingHostNameServerRemove(USMDB_UNIT_CURRENT, 
              serverAddress,
             addrType)) != L7_SUCCESS)
         {
           return COMMIT_FAILED_ERROR;
         }
       }
       else
           return COMMIT_FAILED_ERROR;
     }
     else if ((data->agentRadiusAccountingStatus == D_agentRadiusAccountingStatus_notInService)||
              (data->agentRadiusAccountingStatus == D_agentRadiusAccountingStatus_notReady)||
              (data->agentRadiusAccountingStatus == D_agentRadiusAccountingStatus_createAndWait))
              
     {
       return COMMIT_FAILED_ERROR;
     }
   }
  
  if (VALID(I_agentRadiusAccountingPort, data->valid))
  {
    if (snmpAgentRadiusAccountingConfigEntryGet(USMDB_UNIT_CURRENT, 
          data->agentRadiusAccountingServerIndex, serverAddress, &addrType) 
                                  != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
    if(usmDbRadiusAccountingHostNameServerPortNumSet(USMDB_UNIT_CURRENT, 
         serverAddress,addrType,
         data->agentRadiusAccountingPort) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
  }

  if (VALID(I_agentRadiusAccountingSecret, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    strncpy(snmp_buffer, data->agentRadiusAccountingSecret->octet_ptr,
            data->agentRadiusAccountingSecret->length);
    if (snmpAgentRadiusAccountingConfigEntryGet(USMDB_UNIT_CURRENT, 
          data->agentRadiusAccountingServerIndex, serverAddress, &addrType) 
                                  != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
    if (usmDbRadiusAccountingHostNameServerSharedSecretSet(USMDB_UNIT_CURRENT, 
          serverAddress,addrType, snmp_buffer) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
  }

  if ( L7_TRUE != nameConfigured  && 
                     VALID(I_agentRadiusAccountingServerName, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_buffer, data->agentRadiusAccountingServerName->octet_ptr,
                     data->agentRadiusAccountingServerName->length);


    if (snmpAgentRadiusAccountingConfigEntryGet(USMDB_UNIT_CURRENT, 
        data->agentRadiusAccountingServerIndex, serverAddress, &addrType) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
    if((usmDbStringAlphaNumericCheck(snmp_buffer) != L7_SUCCESS) ||
       ( usmDbRadiusAccountingServerNameSet(USMDB_UNIT_CURRENT, serverAddress, 
                     addrType, snmp_buffer) != L7_SUCCESS ))
    {
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentRadiusAccountingConfigEntry_UNDO
/* add #define SR_agentRadiusAccountingConfigEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentRadiusAccountingConfigEntry family.
 */
int
agentRadiusAccountingConfigEntry_undo(doList_t *doHead, doList_t *doCur,
                                      ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentRadiusAccountingConfigEntry_UNDO */

#endif /* SETS */

agentRadiusServerConfigEntry_t *
k_agentRadiusServerConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator,
                                   int searchType,
                                   SR_INT32 agentRadiusServerIndex)
{
  static agentRadiusServerConfigEntry_t agentRadiusServerConfigEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_char8 name[L7_RADIUS_SERVER_NAME_LENGTH+1];
  L7_char8 secret[L7_RADIUS_MAX_SECRET+1];
  L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus = L7_RADIUSLOCAL;
  
  if (firstTime == L7_TRUE)
  {
    agentRadiusServerConfigEntryData.agentRadiusServerSecret = MakeOctetString(NULL, 0);
    agentRadiusServerConfigEntryData.agentRadiusServerInetAddress = MakeOctetString(NULL, 0);
    firstTime = L7_FALSE;
  }
  
  ZERO_VALID(agentRadiusServerConfigEntryData.valid);
  agentRadiusServerConfigEntryData.agentRadiusServerIndex = agentRadiusServerIndex;
  memset(snmp_buffer,0,SNMP_BUFFER_LEN);
  SET_VALID(I_agentRadiusServerIndex, agentRadiusServerConfigEntryData.valid);
  if ((searchType == EXACT) ?
     (snmpAgentRadiusServerConfigEntryGet(USMDB_UNIT_CURRENT, 
                agentRadiusServerConfigEntryData.agentRadiusServerIndex, 
                snmp_buffer, &addrType) != L7_SUCCESS) :
     (snmpAgentRadiusServerConfigEntryGet(USMDB_UNIT_CURRENT, agentRadiusServerConfigEntryData.agentRadiusServerIndex, 
                                          snmp_buffer, &addrType) != L7_SUCCESS &&
      snmpAgentRadiusServerConfigEntryNextGet(USMDB_UNIT_CURRENT, &agentRadiusServerConfigEntryData.agentRadiusServerIndex, 
                                              snmp_buffer, &addrType) != L7_SUCCESS))
  {
    ZERO_VALID(agentRadiusServerConfigEntryData.valid);
    return(NULL);
  }
  
  switch (nominator)
  {
  case I_agentRadiusServerIndex:
    break;

  case I_agentRadiusServerInetAddress:
      if(SafeMakeOctetStringFromTextExact(
            &agentRadiusServerConfigEntryData.agentRadiusServerInetAddress, snmp_buffer) 
                        == L7_TRUE)
       SET_VALID(I_agentRadiusServerInetAddress, agentRadiusServerConfigEntryData.valid);
    break;

  case I_agentRadiusServerAddressType:
      agentRadiusServerConfigEntryData.agentRadiusServerAddressType = addrType;
      SET_VALID(I_agentRadiusServerAddressType, agentRadiusServerConfigEntryData.valid);
    break;
  case I_agentRadiusServerPort:
  if (usmDbRadiusHostNameServerPortNumGet(USMDB_UNIT_CURRENT, 
                                  snmp_buffer, addrType,
                                  &agentRadiusServerConfigEntryData.agentRadiusServerPort) == L7_SUCCESS)
    SET_VALID(I_agentRadiusServerPort, agentRadiusServerConfigEntryData.valid);
    break;

  case I_agentRadiusServerSecret:
    bzero(secret,sizeof(secret));
	if ((usmDbRadiusHostNameServerSharedSecretGet(snmp_buffer, secret, &paramStatus) == L7_SUCCESS) && 
		(SafeMakeOctetString(&agentRadiusServerConfigEntryData.agentRadiusServerSecret, 
							 secret, strlen(secret)) == L7_TRUE))
    {
      SET_VALID(I_agentRadiusServerSecret, agentRadiusServerConfigEntryData.valid);
    }
    break;

  case I_agentRadiusServerPrimaryMode:
    if (snmpAgentRadiusServerPrimaryModeGet(USMDB_UNIT_CURRENT, 
                 snmp_buffer, addrType, 
                 &agentRadiusServerConfigEntryData.agentRadiusServerPrimaryMode) 
                 == L7_SUCCESS)
      SET_VALID(I_agentRadiusServerPrimaryMode, agentRadiusServerConfigEntryData.valid);
    break;

  case I_agentRadiusServerCurrentMode:
    if (snmpAgentRadiusServerCurrentModeGet(USMDB_UNIT_CURRENT, 
                snmp_buffer , addrType,
                &agentRadiusServerConfigEntryData.agentRadiusServerCurrentMode) 
                   == L7_SUCCESS)
      SET_VALID(I_agentRadiusServerCurrentMode, agentRadiusServerConfigEntryData.valid);
    break;

  case I_agentRadiusServerMsgAuth:
    if (snmpAgentRadiusServerMsgAuthGet(USMDB_UNIT_CURRENT, 
                     snmp_buffer, addrType, 
                     &agentRadiusServerConfigEntryData.agentRadiusServerMsgAuth) 
                     == L7_SUCCESS)
      SET_VALID(I_agentRadiusServerMsgAuth, agentRadiusServerConfigEntryData.valid);
    break;

#ifdef I_agentRadiusServerRowStatus
  case I_agentRadiusServerRowStatus:
    agentRadiusServerConfigEntryData.agentRadiusServerRowStatus = D_agentRadiusServerRowStatus_active;
    SET_VALID(I_agentRadiusServerRowStatus, agentRadiusServerConfigEntryData.valid);
    break;
#endif

#ifdef I_agentRadiusServerAddressRowStatus
  case I_agentRadiusServerAddressRowStatus:
    agentRadiusServerConfigEntryData.agentRadiusServerAddressRowStatus = D_agentRadiusServerAddressRowStatus_active;
    SET_VALID(I_agentRadiusServerAddressRowStatus, agentRadiusServerConfigEntryData.valid);
    break;
#endif

  case I_agentRadiusServerName:
    bzero(name,sizeof(name));
    if(  (usmDbRadiusServerHostNameGet(USMDB_UNIT_CURRENT, snmp_buffer, 
                     addrType, 
                     name) 
                     == L7_SUCCESS ) &&    
      (SafeMakeOctetString(&agentRadiusServerConfigEntryData.agentRadiusServerName,
                    name, strlen(name)) == L7_TRUE) )
    {
      {
        SET_VALID(I_agentRadiusServerName, agentRadiusServerConfigEntryData.valid);
      }
    }
    break;

  case I_agentRadiusServerTimeout:
    if (usmDbRadiusServerTimeOutGet(snmp_buffer, 
									&agentRadiusServerConfigEntryData.agentRadiusServerTimeout,
                                    &paramStatus) == L7_SUCCESS)
	{
      SET_VALID(I_agentRadiusServerTimeout, agentRadiusServerConfigEntryData.valid);
	}
    break;

  case I_agentRadiusServerRetransmit:
    if (usmDbRadiusServerRetransGet(snmp_buffer,
                                    &agentRadiusServerConfigEntryData.agentRadiusServerRetransmit,
                                    &paramStatus) == L7_SUCCESS)
	{
      SET_VALID(I_agentRadiusServerRetransmit, agentRadiusServerConfigEntryData.valid);
	}
	break;

  case I_agentRadiusServerDeadtime:
	if (usmDbRadiusServerDeadtimeGet(snmp_buffer,
									&agentRadiusServerConfigEntryData.agentRadiusServerDeadtime,
									&paramStatus) == L7_SUCCESS)
	{
      SET_VALID(I_agentRadiusServerDeadtime, agentRadiusServerConfigEntryData.valid);
	}
	break;

  case I_agentRadiusServerSourceIPAddr:
	if (usmDbRadiusServerSourceIPGet(snmp_buffer, 
									 &agentRadiusServerConfigEntryData.agentRadiusServerSourceIPAddr, 
									 &paramStatus) == L7_SUCCESS)
	{
      SET_VALID(I_agentRadiusServerSourceIPAddr, agentRadiusServerConfigEntryData.valid);
	}
	break;

  case I_agentRadiusServerPriority:
	if (usmDbRadiusServerPriorityGet(snmp_buffer,
									&agentRadiusServerConfigEntryData.agentRadiusServerPriority) == L7_SUCCESS)
	{
      SET_VALID(I_agentRadiusServerPriority, agentRadiusServerConfigEntryData.valid);
	}
	break;

  case I_agentRadiusServerUsageType:
	if (usmDbRadiusServerUsageTypeGet(snmp_buffer,
         (L7_RADIUS_SERVER_USAGE_TYPE_t *)&agentRadiusServerConfigEntryData.agentRadiusServerUsageType) == L7_SUCCESS)
	{
      SET_VALID(I_agentRadiusServerUsageType, agentRadiusServerConfigEntryData.valid);
	}
	break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != 0 && !VALID(nominator, agentRadiusServerConfigEntryData.valid))
    return(NULL);
  return(&agentRadiusServerConfigEntryData);
}

#ifdef SETS
int
k_agentRadiusServerConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                    doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentRadiusServerConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                     doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentRadiusServerConfigEntry_set_defaults(doList_t *dp)
{
    agentRadiusServerConfigEntry_t *data = (agentRadiusServerConfigEntry_t *) (dp->data);

    if ((data->agentRadiusServerSecret = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentRadiusServerInetAddress = MakeOctetStringFromText("")) == 0){
      return RESOURCE_UNAVAILABLE_ERROR;
    }
    return NO_ERROR;
}

int
k_agentRadiusServerConfigEntry_set(agentRadiusServerConfigEntry_t *data,
                                   ContextInfo *contextInfo, int function)
{
  L7_uchar8 serverAddress[SNMP_BUFFER_LEN];
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_IP_ADDRESS_TYPE_t serverAddrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_uint32 nextValidIndex;
  L7_uint32 wait_time = 10; /* controls how long to wait for Radius server to process create request */
  L7_uint32 ipaddrCheck; 
  L7_BOOL nameConfigured = L7_FALSE;
  L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus = L7_RADIUSLOCAL;

#ifdef I_agentRadiusServerRowStatus
  if(VALID(I_agentRadiusServerRowStatus, data->valid))
  {
    if(data->agentRadiusServerRowStatus == D_agentRadiusServerRowStatus_createAndWait ||
         data->agentRadiusServerRowStatus == D_agentRadiusServerRowStatus_createAndGo) 
    {
      if(!VALID(I_agentRadiusServerIndex, data->valid))
      {
        return COMMIT_FAILED_ERROR;
      }
      if (snmpAgentRadiusServerConfigEntryGet(USMDB_UNIT_CURRENT, 
            data->agentRadiusServerIndex, serverAddress, &addrType) != L7_SUCCESS)
      {
        if ((snmpAgentRadiusServerIndexNextValidGet(USMDB_UNIT_CURRENT, 
              &nextValidIndex) == L7_SUCCESS) &&
               data->agentRadiusServerIndex == nextValidIndex )
        {
          memset(serverAddress, 0, SNMP_BUFFER_LEN);
          memcpy(serverAddress, 
                 data->agentRadiusServerInetAddress->octet_ptr,
                    data->agentRadiusServerInetAddress->length);
          /* We should validate ipaddress. wheather it is dns name or dotted format ip address */
          if(usmDbIPHostAddressValidate(serverAddress,&ipaddrCheck, &serverAddrType) != L7_SUCCESS)
          {
            return COMMIT_FAILED_ERROR;
          }

          bzero(snmp_buffer,sizeof(snmp_buffer));
          if(VALID(I_agentRadiusServerName, data->valid))
          {
            memcpy(snmp_buffer,data->agentRadiusServerName->octet_ptr,
                     data->agentRadiusServerName->length);

            if(usmDbStringAlphaNumericCheck(snmp_buffer) != L7_SUCCESS) 
            {
              return COMMIT_FAILED_ERROR;
            }
          }
          else
          {
            memcpy(snmp_buffer,L7_RADIUS_SERVER_DEFAULT_NAME_AUTH,
                     strlen(L7_RADIUS_SERVER_DEFAULT_NAME_AUTH));
          }
          if(usmDbRadiusIPHostNameServerAdd(USMDB_UNIT_CURRENT, serverAddress,
                         serverAddrType,snmp_buffer) == L7_SUCCESS)
          {
            while (wait_time && snmpAgentRadiusServerConfigEntryGet
                   (USMDB_UNIT_CURRENT, data->agentRadiusServerIndex, 
                    serverAddress, &addrType) != L7_SUCCESS)
            {
              /* sleep for a microsecond to give the Radius component time 
                 to process the add request */
              osapiSleepUSec(1);
              wait_time--;
            }
            nameConfigured = L7_TRUE;
          }
          else
            return COMMIT_FAILED_ERROR;
        }
        else
        {
          return COMMIT_FAILED_ERROR;
        }
      }
      else
      {
        return COMMIT_FAILED_ERROR;
      }
    }
    else if (data->agentRadiusServerRowStatus == D_agentRadiusServerRowStatus_destroy )
    {
      if(!VALID(I_agentRadiusServerIndex, data->valid)) 
      {
        return COMMIT_FAILED_ERROR;
      }
      if (snmpAgentRadiusServerConfigEntryGet(USMDB_UNIT_CURRENT, 
            data->agentRadiusServerIndex, serverAddress, &addrType) == L7_SUCCESS)
      {
        if(usmDbRadiusAuthHostNameServerRemove(USMDB_UNIT_CURRENT, serverAddress, 
              addrType) != L7_SUCCESS)
        {
          return COMMIT_FAILED_ERROR;
        }
      }
      else
          return COMMIT_FAILED_ERROR;
    }
    else if ((data->agentRadiusServerRowStatus == D_agentRadiusAccountingStatus_notInService)||
        (data->agentRadiusServerRowStatus == D_agentRadiusAccountingStatus_notReady))
    {
      return COMMIT_FAILED_ERROR;
    }
  }
#endif /* I_agentRadiusServerRowStatus */

#ifdef I_agentRadiusServerAddressRowStatus
  if(VALID(I_agentRadiusServerAddressRowStatus, data->valid))
  {
    if(data->agentRadiusServerAddressRowStatus == D_agentRadiusServerAddressRowStatus_createAndWait ||
         data->agentRadiusServerAddressRowStatus == D_agentRadiusServerAddressRowStatus_createAndGo) 
    {
      if(!VALID(I_agentRadiusServerIndex, data->valid))
      {
        return COMMIT_FAILED_ERROR;
      }
      if (snmpAgentRadiusServerConfigEntryGet(USMDB_UNIT_CURRENT, 
            data->agentRadiusServerIndex, serverAddress, &addrType) != L7_SUCCESS)
      {
        if ((snmpAgentRadiusServerIndexNextValidGet(USMDB_UNIT_CURRENT, 
              &nextValidIndex) == L7_SUCCESS) &&
               data->agentRadiusServerIndex == nextValidIndex )
        {
          memset(serverAddress, 0, SNMP_BUFFER_LEN);
          memcpy(serverAddress, 
                 data->agentRadiusServerInetAddress->octet_ptr,
                    data->agentRadiusServerInetAddress->length);
          /* We should validate ipaddress. wheather it is dns name or dotted format ip address */
          if(usmDbIPHostAddressValidate(serverAddress,&ipaddrCheck, &serverAddrType) != L7_SUCCESS)
          {
            return COMMIT_FAILED_ERROR;
          }

          bzero(snmp_buffer,sizeof(snmp_buffer));
          if(VALID(I_agentRadiusServerName, data->valid))
          {
            memcpy(snmp_buffer,data->agentRadiusServerName->octet_ptr,
                     data->agentRadiusServerName->length);
          }
          else
          {
            memcpy(snmp_buffer,L7_RADIUS_SERVER_DEFAULT_NAME_AUTH,
                     strlen(L7_RADIUS_SERVER_DEFAULT_NAME_AUTH));
          }
          if(usmDbRadiusIPHostNameServerAdd(USMDB_UNIT_CURRENT, serverAddress,
                         serverAddrType,snmp_buffer) == L7_SUCCESS)
          {
            while (wait_time && snmpAgentRadiusServerConfigEntryGet
                   (USMDB_UNIT_CURRENT, data->agentRadiusServerIndex, 
                    serverAddress, &addrType) != L7_SUCCESS)
            {
              /* sleep for a microsecond to give the Radius component time 
                 to process the add request */
              osapiSleepUSec(1);
              wait_time--;
            }
            nameConfigured = L7_TRUE;
          }
          else
            return COMMIT_FAILED_ERROR;
        }
        else
        {
          return COMMIT_FAILED_ERROR;
        }
      }
      else
      {
        return COMMIT_FAILED_ERROR;
      }
    }
    else if (data->agentRadiusServerAddressRowStatus == D_agentRadiusServerAddressRowStatus_destroy )
    {
      if(!VALID(I_agentRadiusServerIndex, data->valid)) 
      {
        return COMMIT_FAILED_ERROR;
      }
      if (snmpAgentRadiusServerConfigEntryGet(USMDB_UNIT_CURRENT, 
            data->agentRadiusServerIndex, serverAddress, &addrType) == L7_SUCCESS)
      {
        if(usmDbRadiusAuthHostNameServerRemove(USMDB_UNIT_CURRENT, serverAddress, 
              addrType) != L7_SUCCESS)
        {
          return COMMIT_FAILED_ERROR;
        }
      }
      else
          return COMMIT_FAILED_ERROR;
    }
    else if ((data->agentRadiusServerAddressRowStatus == D_agentRadiusAccountingStatus_notInService)||
        (data->agentRadiusServerAddressRowStatus == D_agentRadiusAccountingStatus_notReady))
    {
      return COMMIT_FAILED_ERROR;
    }
  }
#endif /* I_agentRadiusServerAddressRowStatus */

 if (VALID(I_agentRadiusServerPort, data->valid))
  {
    if (snmpAgentRadiusServerConfigEntryGet(USMDB_UNIT_CURRENT, 
        data->agentRadiusServerIndex, serverAddress, &addrType) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
    if(usmDbRadiusHostNameServerPortNumSet(USMDB_UNIT_CURRENT, serverAddress, 
         addrType, data->agentRadiusServerPort) 
         != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
  }

  if (VALID(I_agentRadiusServerSecret, data->valid))
  {
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    strncpy(snmp_buffer, data->agentRadiusServerSecret->octet_ptr,
            data->agentRadiusServerSecret->length);
    if (snmpAgentRadiusServerConfigEntryGet(USMDB_UNIT_CURRENT, 
        data->agentRadiusServerIndex, serverAddress, &addrType) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
    if (usmDbRadiusHostNameServerSharedSecretSet(serverAddress, 
            snmp_buffer, L7_RADIUSLOCAL) != L7_SUCCESS)
      return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentRadiusServerPrimaryMode, data->valid))
  {
    if (snmpAgentRadiusServerConfigEntryGet(USMDB_UNIT_CURRENT, 
        data->agentRadiusServerIndex, serverAddress, &addrType) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
    if(snmpAgentRadiusServerPrimaryModeSet(USMDB_UNIT_CURRENT, serverAddress,addrType,
                                          data->agentRadiusServerPrimaryMode) != L7_SUCCESS)
      return COMMIT_FAILED_ERROR;
  } 

  if (VALID(I_agentRadiusServerMsgAuth, data->valid))
  {
    if (snmpAgentRadiusServerConfigEntryGet(USMDB_UNIT_CURRENT, 
        data->agentRadiusServerIndex, serverAddress, &addrType) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
    if( snmpAgentRadiusServerMsgAuthSet(USMDB_UNIT_CURRENT, serverAddress, addrType,
                                      data->agentRadiusServerMsgAuth) != L7_SUCCESS)
      return COMMIT_FAILED_ERROR;
  }

  if ( L7_TRUE != nameConfigured && 
                     VALID(I_agentRadiusServerName, data->valid))
  {
    bzero(snmp_buffer, sizeof(snmp_buffer));
    memcpy(snmp_buffer, data->agentRadiusServerName->octet_ptr,
                     data->agentRadiusServerName->length);


    if (snmpAgentRadiusServerConfigEntryGet(USMDB_UNIT_CURRENT, 
        data->agentRadiusServerIndex, serverAddress, &addrType) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
    if((usmDbStringAlphaNumericCheck(snmp_buffer) != L7_SUCCESS) ||
       ( usmDbRadiusServerNameSet(USMDB_UNIT_CURRENT, serverAddress, 
                     addrType, snmp_buffer) != L7_SUCCESS ))
    {
      return COMMIT_FAILED_ERROR;
    }
  }

  if (VALID(I_agentRadiusServerTimeout, data->valid))
  {
    if (snmpAgentRadiusServerConfigEntryGet(USMDB_UNIT_CURRENT, 
        data->agentRadiusServerIndex, serverAddress, &addrType) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }

    if (usmDbRadiusServerTimeOutSet(serverAddress,
									data->agentRadiusServerTimeout, 
									paramStatus) != L7_SUCCESS)
	{
      return COMMIT_FAILED_ERROR;
	}
  } 

  if (VALID(I_agentRadiusServerRetransmit, data->valid))
  {
    if (snmpAgentRadiusServerConfigEntryGet(USMDB_UNIT_CURRENT, 
        data->agentRadiusServerIndex, serverAddress, &addrType) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }

    if (usmDbRadiusServerRetransSet(serverAddress,
									data->agentRadiusServerRetransmit, 
									paramStatus) != L7_SUCCESS)
	{
      return COMMIT_FAILED_ERROR;
	}
  } 

  if (VALID(I_agentRadiusServerDeadtime, data->valid))
  {
    if (snmpAgentRadiusServerConfigEntryGet(USMDB_UNIT_CURRENT, 
        data->agentRadiusServerIndex, serverAddress, &addrType) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }

    if (usmDbRadiusServerDeadtimeSet(serverAddress, 
									 data->agentRadiusServerDeadtime,
									 paramStatus) != L7_SUCCESS)
	{
      return COMMIT_FAILED_ERROR;
	}
  } 

  if (VALID(I_agentRadiusServerSourceIPAddr, data->valid))
  {
    if (snmpAgentRadiusServerConfigEntryGet(USMDB_UNIT_CURRENT, 
        data->agentRadiusServerIndex, serverAddress, &addrType) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }

    if (usmDbRadiusServerSourceIPSet(serverAddress, 
									 data->agentRadiusServerSourceIPAddr,
									 paramStatus) != L7_SUCCESS)
	{
      return COMMIT_FAILED_ERROR;
	}
  } 

  if (VALID(I_agentRadiusServerPriority, data->valid))
  {
    if (snmpAgentRadiusServerConfigEntryGet(USMDB_UNIT_CURRENT, 
        data->agentRadiusServerIndex, serverAddress, &addrType) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }

    if (usmDbRadiusServerPrioritySet(serverAddress, 
									 data->agentRadiusServerPriority) != L7_SUCCESS)
	{
      return COMMIT_FAILED_ERROR;
	}
  } 

  if (VALID(I_agentRadiusServerUsageType, data->valid))
  {
    if (snmpAgentRadiusServerConfigEntryGet(USMDB_UNIT_CURRENT, 
        data->agentRadiusServerIndex, serverAddress, &addrType) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }

    if (usmDbRadiusServerUsageTypeSet(serverAddress, 
                                      (L7_RADIUS_SERVER_USAGE_TYPE_t)data->agentRadiusServerUsageType) != L7_SUCCESS)
	{
      return COMMIT_FAILED_ERROR;
	}
  } 

  return NO_ERROR;
}

#ifdef SR_agentRadiusServerConfigEntry_UNDO
/* add #define SR_agentRadiusServerConfigEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentRadiusServerConfigEntry family.
 */
int
agentRadiusServerConfigEntry_undo(doList_t *doHead, doList_t *doCur,
                                  ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentRadiusServerConfigEntry_UNDO */

#endif /* SETS */

