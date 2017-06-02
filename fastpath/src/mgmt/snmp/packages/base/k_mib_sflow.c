/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename k_mib_sflow.c
*
* @purpose  system specific code for sflow mib
*
* @component sflow
*
* @create  10/22/2007
*
* @author  Rajesh G
* @end
*
**********************************************************************/

#include <stdlib.h>

#include "l7_common.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"

#include "datatypes.h"
#include "k_private_base.h"
#include "oid_lib.h"
#include "frmt_lib.h"
#include "base_sitedefs.h"
#include "k_mib_sflow_api.h"
#include "sr_ip.h"
#include "snmp_api.h"
#include "inst_lib.h"

sFlowAgent_t *k_sFlowAgent_get(L7_int32 serialNum, ContextInfo *contextInfo, L7_int32 nominator)
{
  
  static sFlowAgent_t sFlowAgentStatus;
  L7_char8 buffer[SNMP_BUFFER_LEN];
  L7_inet_addr_t ipAddr;
  ZERO_VALID(sFlowAgentStatus.valid);
  switch(nominator)
  {
  case I_sFlowVersion:
       if(snmpSflowAgentVersionGet(USMDB_UNIT_CURRENT, buffer) == L7_SUCCESS &&
          SafeMakeOctetStringFromText(&sFlowAgentStatus.sFlowVersion, buffer) == L7_TRUE)
       {
         SET_VALID(I_sFlowVersion, sFlowAgentStatus.valid);
       }
       if(nominator != -1) break;

  case I_sFlowAgentAddressType:
       if( snmpSflowAgentAddressTypeGet(USMDB_UNIT_CURRENT, &sFlowAgentStatus.sFlowAgentAddressType) == L7_SUCCESS)
       {
          SET_VALID(I_sFlowAgentAddressType, sFlowAgentStatus.valid);
       }
       if(nominator != -1) break;

  case I_sFlowAgentAddress:
       memset(buffer, 0 , SNMP_BUFFER_LEN);
       if(snmpSflowAgentAddressGet(USMDB_UNIT_CURRENT, buffer) == L7_SUCCESS)
       {
         if (usmDbParseInetAddrFromStr(buffer, &ipAddr) == L7_SUCCESS)
         {
           if(ipAddr.family == L7_AF_INET) 
           {
	          if(SafeMakeOctetString( &sFlowAgentStatus.sFlowAgentAddress, (L7_uchar8 *)&ipAddr.addr.ipv4.s_addr,
									sizeof(L7_in_addr_t)) == L7_TRUE)
               SET_VALID(I_sFlowAgentAddress, sFlowAgentStatus.valid);
           }
           else if (ipAddr.family == L7_AF_INET6)
           {
             if(SafeMakeOctetString( &sFlowAgentStatus.sFlowAgentAddress, (L7_uchar8 *)&ipAddr.addr.ipv6,
                                  sizeof(L7_in6_addr_t)) == L7_TRUE)
             {
               SET_VALID(I_sFlowAgentAddress, sFlowAgentStatus.valid);
             }
           }
         }
       }
       if(nominator != -1) break;
  default:
    return NULL;
    break;
  }

  if (nominator != -1 && !VALID(nominator, sFlowAgentStatus.valid))
    return NULL;
  return &sFlowAgentStatus;
}

sFlowRcvrEntry_t *k_sFlowRcvrEntry_get(L7_int32 serialNum, ContextInfo *contextInfo, L7_int32 nominator,
                  L7_int32 searchType, SR_INT32 index)
{

  static sFlowRcvrEntry_t sFlowEntryStatus;
  L7_uchar8 ownerString[FD_SFLOW_OWNER_LENGTH];
  L7_inet_addr_t ipAddr;
  
  static L7_BOOL firstTime = L7_TRUE;
  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    sFlowEntryStatus.sFlowRcvrOwner = MakeOctetString(NULL, 0);
  }
  ZERO_VALID(sFlowEntryStatus.valid);
  sFlowEntryStatus.sFlowRcvrIndex = index;
  SET_VALID(I_sFlowRcvrIndex, sFlowEntryStatus.valid);
  
  if (searchType == EXACT ? ( snmpSflowRcvrEntryGet(USMDB_UNIT_CURRENT, sFlowEntryStatus.sFlowRcvrIndex) != 
       L7_SUCCESS ) : ( snmpSflowRcvrEntryGet(USMDB_UNIT_CURRENT, sFlowEntryStatus.sFlowRcvrIndex) != L7_SUCCESS
       && snmpSflowRcvrEntryNextGet(USMDB_UNIT_CURRENT, &sFlowEntryStatus.sFlowRcvrIndex) != L7_SUCCESS ) ) 
  {
    ZERO_VALID(sFlowEntryStatus.valid);
    return(NULL);
  }
  switch(nominator)
  {
    case -1:
    case I_sFlowRcvrIndex:
         if (nominator != -1) break;
     
    case I_sFlowRcvrOwner:
      memset(ownerString, 0, sizeof(FD_SFLOW_OWNER_LENGTH) );
      if(snmpsFlowRcvrOwnerGet(USMDB_UNIT_CURRENT, sFlowEntryStatus.sFlowRcvrIndex, ownerString) == L7_SUCCESS &&
         SafeMakeOctetStringFromText(&sFlowEntryStatus.sFlowRcvrOwner, ownerString) == L7_TRUE)
      {
        SET_VALID(I_sFlowRcvrOwner, sFlowEntryStatus.valid);
      }
      if (nominator != -1) break;

    case I_sFlowRcvrTimeout:

      if(snmpsFlowRcvrTimeoutGet(USMDB_UNIT_CURRENT, sFlowEntryStatus.sFlowRcvrIndex, &sFlowEntryStatus.sFlowRcvrTimeout) == L7_SUCCESS)
           SET_VALID(I_sFlowRcvrTimeout, sFlowEntryStatus.valid);
      if (nominator != -1) break;


    case I_sFlowRcvrMaximumDatagramSize:
     
       if(snmpsFlowRcvrMaximumDatagramSizeGet(USMDB_UNIT_CURRENT, sFlowEntryStatus.sFlowRcvrIndex, 
                                     &sFlowEntryStatus.sFlowRcvrMaximumDatagramSize) == L7_SUCCESS)
           SET_VALID(I_sFlowRcvrMaximumDatagramSize, sFlowEntryStatus.valid);
      if (nominator != -1) break;
 

    case I_sFlowRcvrAddressType:
     
      if(snmpsFlowRcvrAddressTypeGet(USMDB_UNIT_CURRENT, sFlowEntryStatus.sFlowRcvrIndex, 
                                               &sFlowEntryStatus.sFlowRcvrAddressType) == L7_SUCCESS)
           SET_VALID(I_sFlowRcvrAddressType, sFlowEntryStatus.valid); 
      if (nominator != -1) break;
    
    case I_sFlowRcvrAddress:
      if ( snmpsFlowRcvrAddressGet(USMDB_UNIT_CURRENT, sFlowEntryStatus.sFlowRcvrIndex, &ipAddr) == L7_SUCCESS)
      {
        if(ipAddr.family == L7_AF_INET) 
        {
  	   if(SafeMakeOctetString( &sFlowEntryStatus.sFlowRcvrAddress, (L7_uchar8 *)&ipAddr.addr.ipv4.s_addr,
									sizeof(L7_in_addr_t)) == L7_TRUE)
          {
            SET_VALID(I_sFlowRcvrAddress, sFlowEntryStatus.valid);
          }
        }
        else if (ipAddr.family == L7_AF_INET6)
        {
          if(SafeMakeOctetString( &sFlowEntryStatus.sFlowRcvrAddress, (L7_uchar8 *)&ipAddr.addr.ipv6,
                                  sizeof(L7_in6_addr_t)) == L7_TRUE)
            SET_VALID(I_sFlowRcvrAddress, sFlowEntryStatus.valid);
        }
      }
      if (nominator != -1) break;

    case I_sFlowRcvrPort:
      if( snmpsFlowRcvrPortGet(USMDB_UNIT_CURRENT, sFlowEntryStatus.sFlowRcvrIndex, 
                               &sFlowEntryStatus.sFlowRcvrPort) == L7_SUCCESS)
      {
         SET_VALID(I_sFlowRcvrPort, sFlowEntryStatus.valid);
      }   
    
      if (nominator != -1) break;
  
    case I_sFlowRcvrDatagramVersion:
      if( snmpsFlowRcvrDatagramVersionGet(USMDB_UNIT_CURRENT, sFlowEntryStatus.sFlowRcvrIndex, 
                         &sFlowEntryStatus.sFlowRcvrDatagramVersion) == L7_SUCCESS)
      {
         SET_VALID(I_sFlowRcvrDatagramVersion, sFlowEntryStatus.valid);
      }

     break;

    default:
       return(NULL);
       break;
  }
  if ( nominator >= 0 && !VALID(nominator, sFlowEntryStatus.valid) )
  {
    return(NULL);
  }
  return (&sFlowEntryStatus);
}

L7_int32 k_sFlowRcvrEntry_set(sFlowRcvrEntry_t *data, ContextInfo *contextInfo,L7_int32 function)
{
 
  L7_char8 buffer[SNMP_BUFFER_LEN];
  L7_uchar8 ownerBuffer[FD_SFLOW_OWNER_LENGTH];
  L7_uint32 type = 1, longIpAddr;
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];

  bzero(tempValid, sizeof(tempValid));
  bzero(buffer,SNMP_BUFFER_LEN);
  bzero(ownerBuffer,FD_SFLOW_OWNER_LENGTH);


  /* we can set owner string only to unclaim it other than this we have to set
   * owner string with timeout value
   */

  if( VALID(I_sFlowRcvrOwner, data->valid) &&
      !VALID(I_sFlowRcvrTimeout, data->valid))
  {
    if (SPrintAscii(data->sFlowRcvrOwner, ownerBuffer) < 0)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(WRONG_VALUE_ERROR);
    }
    else
    {
      if(ownerBuffer[0]!=L7_NULL)
      {
         return(COMMIT_FAILED_ERROR); /* first unclaim owner string */
      }
      if(snmpsFlowRcvrOwnerSet(USMDB_UNIT_CURRENT, data->sFlowRcvrIndex, ownerBuffer,
              FD_SFLOW_RCVR_TIMEOUT)!= L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_sFlowRcvrOwner, tempValid);
      }
    }
  }

  if ( VALID(I_sFlowRcvrOwner, data->valid) &&  VALID(I_sFlowRcvrTimeout, data->valid))
  {
    if (SPrintAscii(data->sFlowRcvrOwner, ownerBuffer) < 0)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(WRONG_VALUE_ERROR);
    }
    else
    {
      if(snmpsFlowRcvrOwnerSet(USMDB_UNIT_CURRENT, data->sFlowRcvrIndex, ownerBuffer, 
              data->sFlowRcvrTimeout)!= L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_sFlowRcvrOwner, tempValid);
        SET_VALID(I_sFlowRcvrTimeout, tempValid); 
      }
    }
    
  }
  
  if ( !VALID(I_sFlowRcvrOwner, data->valid) && 
       VALID(I_sFlowRcvrTimeout, data->valid))
  {
    if(snmpsFlowRcvrTimeoutSet(USMDB_UNIT_CURRENT,
    data->sFlowRcvrIndex,data->sFlowRcvrTimeout) != L7_SUCCESS)
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID(I_sFlowRcvrTimeout, tempValid);
    }
  }
  
  if ( VALID(I_sFlowRcvrMaximumDatagramSize, data->valid))
  {
     if(snmpsFlowRcvrMaximumDatagramSizeSet(USMDB_UNIT_CURRENT,
    data->sFlowRcvrIndex, data->sFlowRcvrMaximumDatagramSize) != L7_SUCCESS)
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID( I_sFlowRcvrMaximumDatagramSize, tempValid);
    }

  }
  
  if ( VALID(I_sFlowRcvrAddressType, data->valid)  && 
       !VALID(I_sFlowRcvrAddress, data->valid))
  {
    if(snmpsFlowRcvrAddressTypeSet(USMDB_UNIT_CURRENT,
    data->sFlowRcvrIndex, data->sFlowRcvrAddressType) != L7_SUCCESS)
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID( I_sFlowRcvrAddressType, tempValid);
    }

  }

  if ( VALID(I_sFlowRcvrAddress, data->valid) &&
       !VALID(I_sFlowRcvrAddressType, data->valid))
  {
    bzero(buffer, SNMP_BUFFER_LEN);
    if( snmpsFlowRcvrAddressTypeGet(USMDB_UNIT_CURRENT, data->sFlowRcvrIndex,
        &type ) != L7_SUCCESS)
    { 
      return(WRONG_VALUE_ERROR); 
    }
    else if( type == L7_AF_INET)
    {
      longIpAddr = OctetStringToIP(data->sFlowRcvrAddress);
      if(longIpAddr < 0)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(WRONG_VALUE_ERROR);
      }
      usmDbInetNtoa(longIpAddr, buffer);
    }
    else
    {
      OctetStringToIP6(data->sFlowRcvrAddress, buffer);
    }
    if(snmpsFlowRcvrAddressSet(USMDB_UNIT_CURRENT,
    data->sFlowRcvrIndex,buffer)!= L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_sFlowRcvrAddress, tempValid);
    }
  }
  if (VALID(I_sFlowRcvrAddress, data->valid) &&
      VALID(I_sFlowRcvrAddressType, data->valid))
  {
    bzero(buffer, SNMP_BUFFER_LEN);
    if( data->sFlowRcvrAddressType == L7_AF_INET)
    {
      longIpAddr = OctetStringToIP(data->sFlowRcvrAddress);
      if(longIpAddr < 0)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(WRONG_VALUE_ERROR);
      }
      usmDbInetNtoa(longIpAddr, buffer);
    }
    else if(data->sFlowRcvrAddressType == L7_AF_INET6)
    {
      OctetStringToIP6(data->sFlowRcvrAddress, buffer);
    }
    if(snmpsFlowRcvrAddressSet(USMDB_UNIT_CURRENT,
    data->sFlowRcvrIndex,buffer)!= L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_sFlowRcvrAddress, tempValid);
      SET_VALID(I_sFlowRcvrAddressType, tempValid);
    }
  }  
  if ( VALID(I_sFlowRcvrPort, data->valid))
  {
    if(snmpsFlowRcvrPortSet(USMDB_UNIT_CURRENT,
    data->sFlowRcvrIndex, data->sFlowRcvrPort) != L7_SUCCESS)
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID( I_sFlowRcvrPort, tempValid);
    }

  
  }

  if ( VALID(I_sFlowRcvrDatagramVersion, data->valid))
  {
    if(snmpsFlowRcvrDatagramVersionSet(USMDB_UNIT_CURRENT,
    data->sFlowRcvrIndex,data->sFlowRcvrDatagramVersion) != L7_SUCCESS)
    {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
    }
    else
    {
       SET_VALID( I_sFlowRcvrDatagramVersion, tempValid);
    }

  }
  return  NO_ERROR;
}



L7_int32 k_sFlowRcvrEntry_ready(ObjectInfo *object, ObjectSyntax *value, doList_t *doHead, doList_t *dp)
{

  sFlowRcvrEntry_t *data = (sFlowRcvrEntry_t *) (dp->data);
  /* check the length, since the Textual Convention OwnerString is defined in the
  IF-MIB as 0..255, but it should be 0..127 in this MIB (sflow Owner String uses from Rmon MIB)*/
  if (VALID(I_sFlowRcvrOwner, data->valid) &&
      data->sFlowRcvrOwner->length > 127)
    return(WRONG_LENGTH_ERROR);

   dp->state = SR_ADD_MODIFY;
   return NO_ERROR;
}

L7_int32 k_sFlowRcvrEntry_test(ObjectInfo *object, ObjectSyntax *value, doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}


L7_int32 k_sFlowRcvrEntry_set_defaults(doList_t *dp)
{
  sFlowRcvrEntry_t *data = (sFlowRcvrEntry_t *) (dp->data);
  L7_uint32 longip=0;
  if( (data->sFlowRcvrOwner = MakeOctetStringFromText("")) == 0)
  {
    return(RESOURCE_UNAVAILABLE_ERROR);
  }
  data->sFlowRcvrTimeout = (SR_UINT32) 0;
  data->sFlowRcvrMaximumDatagramSize = (SR_UINT32)1400; 
  data->sFlowRcvrAddressType = (SR_UINT32)1;
  if( (data->sFlowRcvrAddress = IPToOctetString(longip)) == 0)
  {
     return(RESOURCE_UNAVAILABLE_ERROR);
  }
  data->sFlowRcvrPort = (SR_UINT32)6343;
  data->sFlowRcvrDatagramVersion = (SR_UINT32)5;
  return(NO_ERROR);
}

/*  sflow sample mib */

sFlowFsEntry_t *k_sFlowFsEntry_get(L7_int32 serialNum, ContextInfo *contextInfo, L7_int32 nominator, 
                                   L7_int32 searchType, OID * sFlowFsDataSource, SR_INT32 sFlowFsInstance)
{

  static sFlowFsEntry_t sFlowFsEntry;
  L7_char8 strOID[SNMP_BUFFER_LEN];
  static L7_BOOL firstTime = L7_TRUE;
  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    
    sFlowFsEntry.sFlowFsDataSource = MakeOIDFromDot("0.0");
  }
  ZERO_VALID(sFlowFsEntry.valid);
  SET_VALID(I_sFlowFsDataSource, sFlowFsEntry.valid);
  sFlowFsEntry.sFlowFsInstance = sFlowFsInstance;
  SET_VALID(I_sFlowFsInstance, sFlowFsEntry.valid);
  if(MakeDotFromOID(sFlowFsDataSource, strOID) == -1)
  {
    strOID[0]='\0';
  }
  if  (searchType == EXACT ?
        (snmpsFlowFsEntryGet(USMDB_UNIT_CURRENT, strOID, sFlowFsEntry.sFlowFsInstance) != L7_SUCCESS) :
        ( snmpsFlowFsEntryGet(USMDB_UNIT_CURRENT, strOID, sFlowFsEntry.sFlowFsInstance) != L7_SUCCESS &&
          snmpsFlowFsEntryGetNext(USMDB_UNIT_CURRENT, strOID, &sFlowFsEntry.sFlowFsInstance) != L7_SUCCESS ) )
  {
    ZERO_VALID(sFlowFsEntry.valid);
    return(NULL);
  }
  FreeOID(sFlowFsEntry.sFlowFsDataSource);
  if( ( sFlowFsEntry.sFlowFsDataSource = MakeOIDFromDot(strOID))==NULL)
  {
    sFlowFsEntry.sFlowFsDataSource = MakeOIDFromDot("0.0");
  }
  switch( nominator)
  {
    case -1:
    case I_sFlowFsDataSource:
      if (nominator != -1) break;
    case I_sFlowFsInstance:
      if (nominator != -1) break;
    case I_sFlowFsReceiver:
      if( snmpsFlowFsReceiverGet(USMDB_UNIT_CURRENT, strOID, sFlowFsEntry.sFlowFsInstance,
               &sFlowFsEntry.sFlowFsReceiver) ==L7_SUCCESS)
      {
        SET_VALID(I_sFlowFsReceiver, sFlowFsEntry.valid);
      } 
      if (nominator != -1) break;
    case I_sFlowFsPacketSamplingRate:
      if( snmpsFlowFsPacketSamplingRateGet(USMDB_UNIT_CURRENT, strOID, sFlowFsEntry.sFlowFsInstance,
                  &sFlowFsEntry.sFlowFsPacketSamplingRate) ==L7_SUCCESS)
      {
        SET_VALID(I_sFlowFsPacketSamplingRate, sFlowFsEntry.valid);
      }
      if (nominator != -1) break;
    case I_sFlowFsMaximumHeaderSize:
      if( snmpsFlowFsMaximumHeaderSizeGet(USMDB_UNIT_CURRENT, strOID, sFlowFsEntry.sFlowFsInstance,
               &sFlowFsEntry.sFlowFsMaximumHeaderSize) ==L7_SUCCESS)
      {
        SET_VALID(I_sFlowFsMaximumHeaderSize, sFlowFsEntry.valid);
      }
      break;
    default:
       return(NULL);
     break;
  }
  if ( nominator >= 0 && !VALID(nominator, sFlowFsEntry.valid) )
  {
    return(NULL);
  }

  return &sFlowFsEntry;
    
}

L7_int32 k_sFlowFsEntry_test( ObjectInfo *object, ObjectSyntax *value, doList_t *dp, ContextInfo *contextInfo)
{
  return(NO_ERROR);
}

L7_int32 k_sFlowFsEntry_ready(ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp)
{
  
  dp->state = SR_ADD_MODIFY;
  return(NO_ERROR);

}

L7_int32 k_sFlowFsEntry_set_defaults( doList_t *dp)
{
  return (NO_ERROR);
}
L7_int32 k_sFlowFsEntry_set(sFlowFsEntry_t *data,
                  ContextInfo *contextInfo, L7_int32 function)
{
  L7_char8 tempValid[sizeof(data->valid)];

  bzero(tempValid, sizeof(tempValid));
  if( VALID(I_sFlowFsReceiver, data->valid) )
  {
    if (snmpsFlowFsReceiverSet(USMDB_UNIT_CURRENT, data->sFlowFsInstance,
      data->sFlowFsDataSource->oid_ptr[data->sFlowFsDataSource->length-1],
      data->sFlowFsReceiver)!= L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_sFlowFsReceiver, tempValid);
    }
    
  }
  if( VALID(I_sFlowFsPacketSamplingRate, data->valid) )
  {
    if (snmpsFlowFsPacketSamplingRateSet(USMDB_UNIT_CURRENT, data->sFlowFsInstance,
      data->sFlowFsDataSource->oid_ptr[data->sFlowFsDataSource->length-1], 
      data->sFlowFsPacketSamplingRate)!= L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_sFlowFsPacketSamplingRate, tempValid);
    }

  }
  if( VALID(I_sFlowFsMaximumHeaderSize, data->valid) )
  {
    if (snmpsFlowFsMaximumHeaderSizeSet(USMDB_UNIT_CURRENT, data->sFlowFsInstance,
      data->sFlowFsDataSource->oid_ptr[data->sFlowFsDataSource->length-1], 
      data->sFlowFsMaximumHeaderSize)!= L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_sFlowFsMaximumHeaderSize, tempValid);
    }

  }
  return(NO_ERROR);
}

/* sflow cp mib */

sFlowCpEntry_t *k_sFlowCpEntry_get(L7_int32 serialNum,
                  ContextInfo *contextInfo,
                  L7_int32 nominator,
                  L7_int32 searchType,
                  OID * sFlowCpDataSource,
                  SR_INT32 sFlowCpInstance)
{
  static sFlowCpEntry_t sFlowCpEntry;
  L7_char8 strOID[SNMP_BUFFER_LEN];
  static L7_BOOL firstTime = L7_TRUE;
  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    sFlowCpEntry.sFlowCpDataSource = MakeOIDFromDot("0.0");
  }
  ZERO_VALID(sFlowCpEntry.valid);
  SET_VALID(I_sFlowCpDataSource, sFlowCpEntry.valid);
  sFlowCpEntry.sFlowCpInstance = sFlowCpInstance;
  SET_VALID(I_sFlowCpInstance, sFlowCpEntry.valid);
  if(MakeDotFromOID(sFlowCpDataSource, strOID) == -1)
  {
    strOID[0]='\0';
  }
  if ( (searchType == EXACT ?
        (snmpsFlowCpEntryGet(USMDB_UNIT_CURRENT, strOID, sFlowCpEntry.sFlowCpInstance) != L7_SUCCESS) :
        ( snmpsFlowCpEntryGet(USMDB_UNIT_CURRENT, strOID, sFlowCpEntry.sFlowCpInstance) != L7_SUCCESS &&
          snmpsFlowCpEntryGetNext(USMDB_UNIT_CURRENT, strOID, &sFlowCpEntry.sFlowCpInstance) != L7_SUCCESS ) ))
  {

    ZERO_VALID(sFlowCpEntry.valid);
    return(NULL);
  }
		
  FreeOID(sFlowCpEntry.sFlowCpDataSource);
  if( ( sFlowCpEntry.sFlowCpDataSource = MakeOIDFromDot(strOID))==NULL)
  {
    sFlowCpEntry.sFlowCpDataSource = MakeOIDFromDot("0.0");
  }
  switch( nominator)
  {

    case -1:
    case I_sFlowCpDataSource:
      if (nominator != -1) break;
    case I_sFlowCpInstance:
      if (nominator != -1) break;
    case I_sFlowCpReceiver:
      if( snmpsFlowCpReceiverGet(USMDB_UNIT_CURRENT, strOID, sFlowCpEntry.sFlowCpInstance,
               &sFlowCpEntry.sFlowCpReceiver) ==L7_SUCCESS)
      {
        SET_VALID(I_sFlowCpReceiver, sFlowCpEntry.valid);
      }
      if (nominator != -1) break;
    case I_sFlowCpInterval:
      if( snmpsFlowCpIntervalGet(USMDB_UNIT_CURRENT, strOID, sFlowCpEntry.sFlowCpInstance,
                  &sFlowCpEntry.sFlowCpInterval) ==L7_SUCCESS)
      {
        SET_VALID(I_sFlowCpInterval, sFlowCpEntry.valid);
      }
      break;  
    
    default:
      return(NULL);
      break;
  }
  if ( nominator >= 0 && !VALID(nominator, sFlowCpEntry.valid) )
  {
    return(NULL);
  }
  return &sFlowCpEntry;
   
}

L7_int32 k_sFlowCpEntry_set(sFlowCpEntry_t *data,
                  ContextInfo *contextInfo,
                  L7_int32 function)
{
  L7_char8 buffer[SNMP_BUFFER_LEN];
  L7_char8 subBuffer[SNMP_BUFFER_LEN];
  L7_char8 tempValid[sizeof(data->valid)];

  bzero(tempValid, sizeof(tempValid));
  bzero(buffer,SNMP_BUFFER_LEN);
  bzero(subBuffer,SNMP_BUFFER_LEN);

  if ( VALID(I_sFlowCpDataSource, data->valid))
  {
    if (MakeDotFromOID(data->sFlowCpDataSource,buffer)!=-1)
    {
      strncpy(subBuffer,buffer,strlen("ifIndex."));

      if ((strcmp(subBuffer,"ifIndex.")!=0) || (usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT,
            data->sFlowCpDataSource->oid_ptr[data->sFlowCpDataSource->length-1]) != L7_SUCCESS))
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
    }
  }

  if( VALID(I_sFlowCpReceiver, data->valid) )
  {
    if (snmpsFlowCpReceiverSet(USMDB_UNIT_CURRENT, data->sFlowCpInstance,
      data->sFlowCpDataSource->oid_ptr[data->sFlowCpDataSource->length-1],
      data->sFlowCpReceiver)!= L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_sFlowCpReceiver, tempValid);
    }

  }
  if( VALID(I_sFlowCpInterval, data->valid) )
  {
    if (snmpsFlowCpIntervalSet(USMDB_UNIT_CURRENT, data->sFlowCpInstance,
      data->sFlowCpDataSource->oid_ptr[data->sFlowCpDataSource->length-1],
      data->sFlowCpInterval)!= L7_SUCCESS )
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_sFlowCpInterval, tempValid);
    }  
  }

  return(NO_ERROR);
}



L7_int32 k_sFlowCpEntry_test(ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo)
{
  return(NO_ERROR);
}

L7_int32 k_sFlowCpEntry_ready(ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return(NO_ERROR);

}

L7_int32 k_sFlowCpEntry_set_defaults(doList_t *dp)
{
  return(NO_ERROR);
}
