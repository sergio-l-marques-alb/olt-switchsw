/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: k_mib_fastpathrouting.c
*
* Purpose: Private mibs for the switch
*
* Created by: Colin Verne, Soma 04/25/2001
*
* Component: 
*
*********************************************************************/
/*********************************************************************
 *
 ********************************************************************/

#include "k_private_base.h"

#include "k_mib_fastpathrouting_api.h"
#include "usmdb_1213_api.h"
#include "usmdb_common.h"
#include "usmdb_ip_api.h"
#include "usmdb_iputil_api.h"
#include "usmdb_mib_relay_api.h"
#include "usmdb_rtrdisc_api.h"
#include "usmdb_util_api.h"
#include "usmdb_dot1q_api.h"

agentSwitchArpGroup_t *
k_agentSwitchArpGroup_get(int serialNum, ContextInfo *contextInfo,
                          int nominator)
{
   static agentSwitchArpGroup_t agentSwitchArpGroupData;

   ZERO_VALID(agentSwitchArpGroupData.valid);

   switch (nominator)
   {
   case -1:
     break;

   case I_agentSwitchArpAgeoutTime :
     if (snmpAgentSwitchArpAgeoutTimeGet(USMDB_UNIT_CURRENT,
                             &agentSwitchArpGroupData.agentSwitchArpAgeoutTime) == L7_SUCCESS)
       SET_VALID(I_agentSwitchArpAgeoutTime, agentSwitchArpGroupData.valid);
     break;

   case I_agentSwitchArpResponseTime :
     if (snmpAgentSwitchArpResponseTimeGet(USMDB_UNIT_CURRENT,
                        &agentSwitchArpGroupData.agentSwitchArpResponseTime) == L7_SUCCESS)
       SET_VALID(I_agentSwitchArpResponseTime, agentSwitchArpGroupData.valid);
     break;

   case I_agentSwitchArpMaxRetries :
     if (snmpAgentSwitchArpMaxRetriesGet(USMDB_UNIT_CURRENT,
                         &agentSwitchArpGroupData.agentSwitchArpMaxRetries) == L7_SUCCESS)
       SET_VALID(I_agentSwitchArpMaxRetries, agentSwitchArpGroupData.valid);
     break;

   case I_agentSwitchArpCacheSize :
     if (snmpAgentSwitchArpCacheSizeGet(USMDB_UNIT_CURRENT,
                       &agentSwitchArpGroupData.agentSwitchArpCacheSize) == L7_SUCCESS)
       SET_VALID(I_agentSwitchArpCacheSize, agentSwitchArpGroupData.valid);
     break;

   case I_agentSwitchArpDynamicRenew :
     if (snmpAgentSwitchArpDynamicRenewGet(USMDB_UNIT_CURRENT,
                       &agentSwitchArpGroupData.agentSwitchArpDynamicRenew) == L7_SUCCESS)
       SET_VALID(I_agentSwitchArpDynamicRenew, agentSwitchArpGroupData.valid);
     break;

   case I_agentSwitchArpTotalEntryCountCurrent :
     if (snmpAgentSwitchArpTotalEntryCountCurrentGet(USMDB_UNIT_CURRENT,
                       &agentSwitchArpGroupData.agentSwitchArpTotalEntryCountCurrent) == L7_SUCCESS)
       SET_VALID(I_agentSwitchArpTotalEntryCountCurrent, agentSwitchArpGroupData.valid);
     break;

   case I_agentSwitchArpTotalEntryCountPeak :
     if (snmpAgentSwitchArpTotalEntryCountPeakGet(USMDB_UNIT_CURRENT,
                       &agentSwitchArpGroupData.agentSwitchArpTotalEntryCountPeak) == L7_SUCCESS)
       SET_VALID(I_agentSwitchArpTotalEntryCountPeak, agentSwitchArpGroupData.valid);
     break;

   case I_agentSwitchArpStaticEntryCountCurrent :
     if (snmpAgentSwitchArpStaticEntryCountCurrentGet(USMDB_UNIT_CURRENT,
                       &agentSwitchArpGroupData.agentSwitchArpStaticEntryCountCurrent) == L7_SUCCESS)
       SET_VALID(I_agentSwitchArpStaticEntryCountCurrent, agentSwitchArpGroupData.valid);
     break;

   case I_agentSwitchArpStaticEntryCountMax :
     if (snmpAgentSwitchArpStaticEntryCountMaxGet(USMDB_UNIT_CURRENT,
                       &agentSwitchArpGroupData.agentSwitchArpStaticEntryCountMax) == L7_SUCCESS)
       SET_VALID(I_agentSwitchArpStaticEntryCountMax, agentSwitchArpGroupData.valid);
     break;

   default:
     /* unknown nominator */
     return(NULL);
     break;
   }

   if (nominator >= 0 && !VALID(nominator, agentSwitchArpGroupData.valid))
     return(NULL);

   return(&agentSwitchArpGroupData);
}

#ifdef SETS
int
k_agentSwitchArpGroup_test(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchArpGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchArpGroup_set(agentSwitchArpGroup_t *data,
                          ContextInfo *contextInfo, int function)
{
  if (VALID(I_agentSwitchArpAgeoutTime, data->valid) &&
      snmpAgentSwitchArpAgeoutTimeSet(USMDB_UNIT_CURRENT, data->agentSwitchArpAgeoutTime) != L7_SUCCESS)
  {
    CLR_VALID(I_agentSwitchArpAgeoutTime, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentSwitchArpResponseTime, data->valid) &&
      snmpAgentSwitchArpResponseTimeSet(USMDB_UNIT_CURRENT, data->agentSwitchArpResponseTime) != L7_SUCCESS)
  {
    CLR_VALID(I_agentSwitchArpResponseTime, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentSwitchArpMaxRetries, data->valid) &&
      snmpAgentSwitchArpMaxRetriesSet(USMDB_UNIT_CURRENT, data->agentSwitchArpMaxRetries) != L7_SUCCESS)
  {
    CLR_VALID(I_agentSwitchArpMaxRetries, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentSwitchArpCacheSize, data->valid) &&
      snmpAgentSwitchArpCacheSizeSet(USMDB_UNIT_CURRENT, data->agentSwitchArpCacheSize) != L7_SUCCESS)
  {
    CLR_VALID(I_agentSwitchArpCacheSize, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentSwitchArpDynamicRenew, data->valid) &&
      snmpAgentSwitchArpDynamicRenewSet(USMDB_UNIT_CURRENT, data->agentSwitchArpDynamicRenew) != L7_SUCCESS)
  {
    CLR_VALID(I_agentSwitchArpDynamicRenew, data->valid);
    return COMMIT_FAILED_ERROR;
  }
  return NO_ERROR;
}

#ifdef SR_agentSwitchArpGroup_UNDO
/* add #define SR_agentSwitchArpGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchArpGroup family.
 */
int
agentSwitchArpGroup_undo(doList_t *doHead, doList_t *doCur,
                         ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchArpGroup_UNDO */

#endif /* SETS */

agentSwitchArpEntry_t *
k_agentSwitchArpEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator,
                          int searchType,
                          SR_UINT32 agentSwitchArpIpAddress)
{
   return(NULL);
}

#ifdef SETS
int
k_agentSwitchArpEntry_test(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchArpEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchArpEntry_set_defaults(doList_t *dp)
{
  return COMMIT_FAILED_ERROR;
}

int
k_agentSwitchArpEntry_set(agentSwitchArpEntry_t *data,
                          ContextInfo *contextInfo, int function)
{
  return COMMIT_FAILED_ERROR;
}

#endif /* SETS */

agentSwitchLocalProxyArpEntry_t *
k_agentSwitchLocalProxyArpEntry_get(int serialNum, ContextInfo *contextInfo,
                                    int nominator,
                                    int searchType,
                                    SR_INT32 ifIndex)
{
   static agentSwitchLocalProxyArpEntry_t agentSwitchLocalProxyArpEntryData;
   L7_uint32 intIfNum;
   
   ZERO_VALID(agentSwitchLocalProxyArpEntryData.valid);
   agentSwitchLocalProxyArpEntryData.ifIndex = ifIndex;
   SET_VALID(I_agentSwitchLocalProxyArpEntryIndex_ifIndex, agentSwitchLocalProxyArpEntryData.valid);

     /* Validating the row-entries for GET and GETNEXT operations */
   if((searchType == EXACT)?  
      (snmpAgentSwitchLocalProxyArpEntryValidate(USMDB_UNIT_CURRENT, agentSwitchLocalProxyArpEntryData.ifIndex,
						 &intIfNum) != L7_SUCCESS) :
      ((snmpAgentSwitchLocalProxyArpEntryValidate(USMDB_UNIT_CURRENT, agentSwitchLocalProxyArpEntryData.ifIndex,
						 &intIfNum) != L7_SUCCESS) &&
       (snmpAgentSwitchLocalProxyArpEntryGetNext(USMDB_UNIT_CURRENT, &agentSwitchLocalProxyArpEntryData.ifIndex,
						 &intIfNum) != L7_SUCCESS)))
   {
      ZERO_VALID(agentSwitchLocalProxyArpEntryData.valid);
      return(NULL);      
   }	   
      

   switch (nominator)
   { 
     case -1:
     case I_agentSwitchLocalProxyArpEntryIndex_ifIndex:
       if (nominator != -1)
         break;
       /* else pass through */
 
     case I_agentSwitchLocalProxyArpMode:
       if(snmpAgentSwitchLocalProxyArpModeGet(USMDB_UNIT_CURRENT, intIfNum,
 		               &agentSwitchLocalProxyArpEntryData.agentSwitchLocalProxyArpMode) == L7_SUCCESS)
       SET_VALID(I_agentSwitchLocalProxyArpMode, agentSwitchLocalProxyArpEntryData.valid);
        break;
 
     default:
        break;
   }

   if (nominator >= 0 && !VALID(nominator, agentSwitchLocalProxyArpEntryData.valid))
   {
      return(NULL);
   }       

   
   return(&agentSwitchLocalProxyArpEntryData);
}

#ifdef SETS
int
k_agentSwitchLocalProxyArpEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchLocalProxyArpEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchLocalProxyArpEntry_set_defaults(doList_t *dp)
{
    agentSwitchLocalProxyArpEntry_t *data = (agentSwitchLocalProxyArpEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentSwitchLocalProxyArpEntry_set(agentSwitchLocalProxyArpEntry_t *data,
                                    ContextInfo *contextInfo, int function)
{

  L7_uint32 intIfNum;	

  if(usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_IP_MAP_COMPONENT_ID, L7_IPMAP_FEATURE_PROXY_ARP) != L7_TRUE)
  {
     /* If Proxy Arp feaure is not supported */
     return COMMIT_FAILED_ERROR;
  }

  if ( usmDbIntIfNumFromExtIfNum(data->ifIndex, &intIfNum) != L7_SUCCESS )
  {
     ZERO_VALID(data->valid);
     return(WRONG_VALUE_ERROR);
  }

 if (VALID(I_agentSwitchLocalProxyArpMode, data->valid))
 {
    if (snmpAgentSwitchLocalProxyArpModeSet(USMDB_UNIT_CURRENT,intIfNum, data->agentSwitchLocalProxyArpMode) 
                       		    != L7_SUCCESS)
        return COMMIT_FAILED_ERROR;
 }

   return NO_ERROR;
}

#ifdef SR_agentSwitchLocalProxyArpEntry_UNDO
/* add #define SR_agentSwitchLocalProxyArpEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchLocalProxyArpEntry family.
 */
int
agentSwitchLocalProxyArpEntry_undo(doList_t *doHead, doList_t *doCur,
                                   ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchLocalProxyArpEntry_UNDO */

#endif /* SETS */


agentSwitchIntfArpEntry_t *
k_agentSwitchIntfArpEntry_get(int serialNum, ContextInfo *contextInfo,
                              int nominator,
                              int searchType,
                              SR_UINT32 agentSwitchIntfArpIpAddress,
                              SR_INT32 agentSwitchIntfArpIfIndex)
{
  static agentSwitchIntfArpEntry_t agentSwitchIntfArpEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 seconds;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentSwitchIntfArpEntryData.agentSwitchIntfArpMacAddress = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentSwitchIntfArpEntryData.valid);
  agentSwitchIntfArpEntryData.agentSwitchIntfArpIpAddress = agentSwitchIntfArpIpAddress;
  agentSwitchIntfArpEntryData.agentSwitchIntfArpIfIndex = agentSwitchIntfArpIfIndex;
  SET_VALID(I_agentSwitchIntfArpIpAddress, agentSwitchIntfArpEntryData.valid);
  SET_VALID(I_agentSwitchIntfArpIfIndex, agentSwitchIntfArpEntryData.valid);

  if ( (searchType == EXACT) ?
       (snmpagentSwitchIntfArpEntryGetCheck (USMDB_UNIT_CURRENT, agentSwitchIntfArpEntryData.agentSwitchIntfArpIpAddress, 
                                             agentSwitchIntfArpEntryData.agentSwitchIntfArpIfIndex) != L7_SUCCESS) :
       ( (snmpagentSwitchIntfArpEntryGetCheck (USMDB_UNIT_CURRENT, agentSwitchIntfArpEntryData.agentSwitchIntfArpIpAddress, 
                                               agentSwitchIntfArpEntryData.agentSwitchIntfArpIfIndex) != L7_SUCCESS) &&
         (snmpagentSwitchIntfArpEntryGetNextCheck (USMDB_UNIT_CURRENT, &agentSwitchIntfArpEntryData.agentSwitchIntfArpIpAddress, 
                                                   &agentSwitchIntfArpEntryData.agentSwitchIntfArpIfIndex) != L7_SUCCESS) ) )
  {
    ZERO_VALID(agentSwitchIntfArpEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_agentSwitchIntfArpIpAddress :
  case I_agentSwitchIntfArpIfIndex :
    break;

  case I_agentSwitchIntfArpAge:
    if (snmpagentSwitchIntfArpAgeGet(USMDB_UNIT_CURRENT, agentSwitchIntfArpEntryData.agentSwitchIntfArpIpAddress, 
                                     agentSwitchIntfArpEntryData.agentSwitchIntfArpIfIndex, &seconds) == L7_SUCCESS &&
        SafeMakeTimeTicksFromSeconds(seconds, &agentSwitchIntfArpEntryData.agentSwitchIntfArpAge) == L7_TRUE)
      SET_VALID(I_agentSwitchIntfArpAge, agentSwitchIntfArpEntryData.valid);
    break;

  case I_agentSwitchIntfArpMacAddress :
    if (snmpagentSwitchIntfArpMacAddressGet(USMDB_UNIT_CURRENT, agentSwitchIntfArpEntryData.agentSwitchIntfArpIpAddress, 
                                            agentSwitchIntfArpEntryData.agentSwitchIntfArpIfIndex, snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetString(&agentSwitchIntfArpEntryData.agentSwitchIntfArpMacAddress, snmp_buffer, L7_MAC_ADDR_LEN) == L7_TRUE)
      SET_VALID(I_agentSwitchIntfArpMacAddress, agentSwitchIntfArpEntryData.valid);
    break;

  case I_agentSwitchIntfArpType :
    if (snmpagentSwitchIntfArpTypeGet(USMDB_UNIT_CURRENT, agentSwitchIntfArpEntryData.agentSwitchIntfArpIpAddress,
                                      agentSwitchIntfArpEntryData.agentSwitchIntfArpIfIndex,
                                      &agentSwitchIntfArpEntryData.agentSwitchIntfArpType) == L7_SUCCESS)
      SET_VALID(I_agentSwitchIntfArpType, agentSwitchIntfArpEntryData.valid);
    break;

  case I_agentSwitchIntfArpStatus : /* always active */
    agentSwitchIntfArpEntryData.agentSwitchIntfArpStatus = D_agentSwitchIntfArpStatus_active;
    SET_VALID(I_agentSwitchIntfArpStatus, agentSwitchIntfArpEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentSwitchIntfArpEntryData.valid))
    return(NULL);

  return(&agentSwitchIntfArpEntryData);
}

#ifdef SETS
int
k_agentSwitchIntfArpEntry_test(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchIntfArpEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchIntfArpEntry_set_defaults(doList_t *dp)
{
    agentSwitchIntfArpEntry_t *data = (agentSwitchIntfArpEntry_t *) (dp->data);

    if ((data->agentSwitchIntfArpMacAddress = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentSwitchIntfArpEntry_set(agentSwitchIntfArpEntry_t *data,
                              ContextInfo *contextInfo, int function)
{
  L7_char8 snmp_buffer[L7_ENET_MAC_ADDR_LEN];
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 intIfNum = L7_INVALID_INTF;


#if 0
  
/* To be Consistent with the CLI Here we are passing L7_INVALID_INTF as an argument
   for creation/modification of the static ARP entries. In Future whenever we support
   UNNUMBERED interfaces then we need to take care of passing correct internal interface
   numbers.*/
  
    if (usmDbIntIfNumFromExtIfNum(data->agentSwitchIntfArpIfIndex, &intIfNum) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;

    }
#endif

  /* If entry doesn't exist */
  if (snmpagentSwitchIntfArpEntryGetCheck(USMDB_UNIT_CURRENT, 
                                          data->agentSwitchIntfArpIpAddress, 
                                          L7_INVALID_INTF) != L7_SUCCESS)
  {
    /* must set status to createAndGo and mac address */
    if (VALID(I_agentSwitchIntfArpStatus, data->valid) &&
        data->agentSwitchIntfArpStatus == D_agentSwitchIntfArpStatus_createAndGo &&
        VALID(I_agentSwitchIntfArpMacAddress, data->valid))
    {
      memset(snmp_buffer, 0, sizeof(snmp_buffer));

      if (data->agentSwitchIntfArpMacAddress->length > sizeof(snmp_buffer))
        return COMMIT_FAILED_ERROR;

      memcpy(snmp_buffer, data->agentSwitchIntfArpMacAddress->octet_ptr,  data->agentSwitchIntfArpMacAddress->length);
      rc = usmDbIpStaticArpAdd(USMDB_UNIT_CURRENT, data->agentSwitchIntfArpIpAddress, intIfNum, snmp_buffer);

      if ((rc != L7_SUCCESS) && (rc != L7_NOT_EXIST))
        return COMMIT_FAILED_ERROR;

      return NO_ERROR;
    }
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentSwitchIntfArpMacAddress, data->valid))
  {
    memset(snmp_buffer, 0, sizeof(snmp_buffer));

    if (data->agentSwitchIntfArpMacAddress->length > sizeof(snmp_buffer))
      return COMMIT_FAILED_ERROR;

    memcpy(snmp_buffer, data->agentSwitchIntfArpMacAddress->octet_ptr,  data->agentSwitchIntfArpMacAddress->length);

    if (usmDbIpStaticArpAdd(USMDB_UNIT_CURRENT, data->agentSwitchIntfArpIpAddress, intIfNum, snmp_buffer) != L7_SUCCESS)
      return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentSwitchIntfArpStatus, data->valid) &&
      snmpagentSwitchIntfArpStatusSet(USMDB_UNIT_CURRENT, data->agentSwitchIntfArpIpAddress, intIfNum, data->agentSwitchIntfArpStatus) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  return NO_ERROR;
}

#ifdef SR_agentSwitchIntfArpEntry_UNDO
/* add #define SR_agentSwitchIntfArpEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchIntfArpEntry family.
 */
int
agentSwitchIntfArpEntry_undo(doList_t *doHead, doList_t *doCur,
                             ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchIntfArpEntry_UNDO */

#endif /* SETS */

agentSwitchIpGroup_t *
k_agentSwitchIpGroup_get(int serialNum, ContextInfo *contextInfo,
                         int nominator)
{
  static agentSwitchIpGroup_t agentSwitchIpGroupData;

  ZERO_VALID(agentSwitchIpGroupData.valid);

  switch (nominator)
  {
  case -1:
    break;

  case I_agentSwitchIpRoutingMode:
    if (snmpAgentSwitchIpRoutingModeGet(USMDB_UNIT_CURRENT, &agentSwitchIpGroupData.agentSwitchIpRoutingMode) == L7_SUCCESS)
      SET_VALID(I_agentSwitchIpRoutingMode, agentSwitchIpGroupData.valid);
    break;

  case I_agentSwitchIpDefaultGateway:
    if (snmpAgentDefaultGlobalGatewayAddressGet(USMDB_UNIT_CURRENT, &agentSwitchIpGroupData.
                                                agentSwitchIpDefaultGateway)
                                                == L7_SUCCESS)
      SET_VALID(I_agentSwitchIpDefaultGateway, agentSwitchIpGroupData.valid);
    break;

  case I_agentSwitchClearIpDefaultGateway:
    agentSwitchIpGroupData.agentSwitchClearIpDefaultGateway = D_agentSwitchClearIpDefaultGateway_disable;
    SET_VALID(I_agentSwitchClearIpDefaultGateway, agentSwitchIpGroupData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentSwitchIpGroupData.valid))
    return(NULL);

  return(&agentSwitchIpGroupData);
}

#ifdef SETS
int
k_agentSwitchIpGroup_test(ObjectInfo *object, ObjectSyntax *value,
                          doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchIpGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchIpGroup_set(agentSwitchIpGroup_t *data,
                         ContextInfo *contextInfo, int function)
{
  if (VALID(I_agentSwitchIpRoutingMode, data->valid) &&
      snmpAgentSwitchIpRoutingModeSet(USMDB_UNIT_CURRENT, data->agentSwitchIpRoutingMode) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentSwitchIpDefaultGateway, data->valid) &&
      snmpAgentDefaultGlobalGatewayAddressSet(USMDB_UNIT_CURRENT, data->agentSwitchIpDefaultGateway)
                                              != L7_SUCCESS)
  {
    CLR_VALID(I_agentSwitchIpDefaultGateway, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentSwitchClearIpDefaultGateway, data->valid) &&
      snmpAgentGlobalDefaultGatewayAddressReset(data->agentSwitchClearIpDefaultGateway) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  return NO_ERROR;
}

#ifdef SR_agentSwitchIpGroup_UNDO
/* add #define SR_agentSwitchIpGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchIpGroup family.
 */
int
agentSwitchIpGroup_undo(doList_t *doHead, doList_t *doCur,
                        ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchIpGroup_UNDO */

#endif /* SETS */

agentSwitchIpInterfaceEntry_t *
k_agentSwitchIpInterfaceEntry_get(int serialNum, ContextInfo *contextInfo,
                                  int nominator,
                                  int searchType,
                                  SR_INT32 agentSwitchIpInterfaceIfIndex)
{
  static agentSwitchIpInterfaceEntry_t agentSwitchIpInterfaceEntryData;
  L7_uint32 intIfIndex;

  ZERO_VALID(agentSwitchIpInterfaceEntryData.valid);
  agentSwitchIpInterfaceEntryData.agentSwitchIpInterfaceIfIndex = agentSwitchIpInterfaceIfIndex;
  SET_VALID(I_agentSwitchIpInterfaceIfIndex, agentSwitchIpInterfaceEntryData.valid);

  if ( ( searchType == EXACT ? ( snmpAgentSwitchIpInterfaceGet(USMDB_UNIT_CURRENT, agentSwitchIpInterfaceEntryData.agentSwitchIpInterfaceIfIndex) != L7_SUCCESS ) :
         ( snmpAgentSwitchIpInterfaceGet(USMDB_UNIT_CURRENT, agentSwitchIpInterfaceEntryData.agentSwitchIpInterfaceIfIndex) != L7_SUCCESS &&
           snmpAgentSwitchIpInterfaceNextGet(USMDB_UNIT_CURRENT, &agentSwitchIpInterfaceEntryData.agentSwitchIpInterfaceIfIndex) != L7_SUCCESS ) ) ||
       usmDbIntIfNumFromExtIfNum(agentSwitchIpInterfaceEntryData.agentSwitchIpInterfaceIfIndex, &intIfIndex) != L7_SUCCESS )
  {
    ZERO_VALID(agentSwitchIpInterfaceEntryData.valid);
    return(NULL);
  }

/* lvl7_@p0971 start */
  switch (nominator)
  {
  case -1:
  case I_agentSwitchIpInterfaceIfIndex:
    break;

  case I_agentSwitchIPAddressConfigMethod:
    if (snmpAgentRtrIntfIpAddressMethodGet(USMDB_UNIT_CURRENT, intIfIndex,
                                           &agentSwitchIpInterfaceEntryData.
                                           agentSwitchIPAddressConfigMethod)
                                           == L7_SUCCESS)
      SET_VALID(I_agentSwitchIPAddressConfigMethod, agentSwitchIpInterfaceEntryData.valid);
    break;

  case I_agentSwitchIpInterfaceIpAddress:
    if (snmpAgentSwitchIpInterfaceIpAddressGet(USMDB_UNIT_CURRENT, intIfIndex,
                                               &agentSwitchIpInterfaceEntryData.agentSwitchIpInterfaceIpAddress) == L7_SUCCESS)
      SET_VALID(I_agentSwitchIpInterfaceIpAddress, agentSwitchIpInterfaceEntryData.valid);
    break;

  case I_agentSwitchIpInterfaceNetMask:
    if (snmpAgentSwitchIpInterfaceNetMaskGet(USMDB_UNIT_CURRENT, intIfIndex,
                                             &agentSwitchIpInterfaceEntryData.agentSwitchIpInterfaceNetMask) == L7_SUCCESS)
      SET_VALID(I_agentSwitchIpInterfaceNetMask, agentSwitchIpInterfaceEntryData.valid);
    break;

/* lvl7_@p1516 start */
  case I_agentSwitchIpInterfaceClearIp:
    agentSwitchIpInterfaceEntryData.agentSwitchIpInterfaceClearIp = D_agentSwitchIpInterfaceClearIp_disable;
    SET_VALID(I_agentSwitchIpInterfaceClearIp, agentSwitchIpInterfaceEntryData.valid);
    break;
/* lvl7_@p1516 end */

  case I_agentSwitchIpInterfaceRoutingMode:
    if (snmpAgentSwitchIpInterfaceRoutingModeGet(USMDB_UNIT_CURRENT, intIfIndex,
                                                 &agentSwitchIpInterfaceEntryData.agentSwitchIpInterfaceRoutingMode) == L7_SUCCESS)
      SET_VALID(I_agentSwitchIpInterfaceRoutingMode, agentSwitchIpInterfaceEntryData.valid);
    break;

  case I_agentSwitchIpInterfaceMtuValue:
    if (usmDbIntfIpMtuGet(USMDB_UNIT_CURRENT, intIfIndex,
                          &agentSwitchIpInterfaceEntryData.agentSwitchIpInterfaceMtuValue) == L7_SUCCESS)
      SET_VALID(I_agentSwitchIpInterfaceMtuValue, agentSwitchIpInterfaceEntryData.valid);
    break;
    
  case I_agentSwitchIpInterfaceBandwidth:
    if (usmDbIfBandwidthGet(intIfIndex,
          &agentSwitchIpInterfaceEntryData.agentSwitchIpInterfaceBandwidth) == L7_SUCCESS)
      SET_VALID(I_agentSwitchIpInterfaceBandwidth, agentSwitchIpInterfaceEntryData.valid);
    break;

  case I_agentSwitchIpInterfaceProxyARPMode:
    if (snmpAgentSwitchIpInterfaceProxyARPModeGet(USMDB_UNIT_CURRENT, intIfIndex,
                                                 &agentSwitchIpInterfaceEntryData.agentSwitchIpInterfaceProxyARPMode) == L7_SUCCESS)
      SET_VALID(I_agentSwitchIpInterfaceProxyARPMode, agentSwitchIpInterfaceEntryData.valid);
    break;

  case I_agentSwitchIpInterfaceUnnumberedIfIndex:
     
    if (snmpAgentSwitchIpInterfaceUnnumberedIfIndexGet(USMDB_UNIT_CURRENT, intIfIndex,
                                                       &agentSwitchIpInterfaceEntryData.agentSwitchIpInterfaceUnnumberedIfIndex) == L7_SUCCESS)
      SET_VALID(I_agentSwitchIpInterfaceUnnumberedIfIndex, agentSwitchIpInterfaceEntryData.valid);
    break;

 case I_agentSwitchIpInterfaceIcmpUnreachables:

    if (snmpAgentSwitchIpInterfaceIcmpUnreachablesGet (USMDB_UNIT_CURRENT, intIfIndex,
                                                 &agentSwitchIpInterfaceEntryData.agentSwitchIpInterfaceIcmpUnreachables) == L7_SUCCESS)
         SET_VALID(I_agentSwitchIpInterfaceIcmpUnreachables, agentSwitchIpInterfaceEntryData.valid);
    break;
   
  case I_agentSwitchIpInterfaceIcmpRedirects:
    if (snmpAgentSwitchIpInterfaceIcmpRedirectsGet (USMDB_UNIT_CURRENT, intIfIndex,
                                                 &agentSwitchIpInterfaceEntryData.agentSwitchIpInterfaceIcmpRedirects) == L7_SUCCESS)
       SET_VALID(I_agentSwitchIpInterfaceIcmpRedirects, agentSwitchIpInterfaceEntryData.valid);
    break;

  case I_agentSwitchDhcpOperation:
    agentSwitchIpInterfaceEntryData.agentSwitchDhcpOperation = D_agentSwitchDhcpOperation_none;
    SET_VALID(I_agentSwitchDhcpOperation, agentSwitchIpInterfaceEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }
/* lvl7_@p0971 end */

  if (nominator >= 0 && !VALID(nominator, agentSwitchIpInterfaceEntryData.valid))
    return(NULL);

  return(&agentSwitchIpInterfaceEntryData);
}

#ifdef SETS
int
k_agentSwitchIpInterfaceEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                   doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchIpInterfaceEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                    doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchIpInterfaceEntry_set_defaults(doList_t *dp)
{
    agentSwitchIpInterfaceEntry_t *data = (agentSwitchIpInterfaceEntry_t *) (dp->data);
/* lvl7_@p0971 start */
    data->agentSwitchIpInterfaceNetMask = L7_IP_SUBMASK_EXACT_MATCH_MASK;
    SET_VALID(I_agentSwitchIpInterfaceNetMask, data->valid);
/* lvl7_@p0971 end */
/* lvl7_@p1516 start */
    data->agentSwitchIpInterfaceClearIp = D_agentSwitchIpInterfaceClearIp_disable;
    SET_VALID(I_agentSwitchIpInterfaceClearIp, data->valid);
/* lvl7_@p1516 end */
    data->agentSwitchIpInterfaceRoutingMode = D_agentSwitchIpInterfaceRoutingMode_disable;
    SET_VALID(I_agentSwitchIpInterfaceRoutingMode, data->valid);
    data->agentSwitchDhcpOperation = D_agentSwitchDhcpOperation_none;
    SET_VALID(I_agentSwitchDhcpOperation, data->valid);

    return NO_ERROR;
}

int
k_agentSwitchIpInterfaceEntry_set(agentSwitchIpInterfaceEntry_t *data,
                                  ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfIndex;

  if (snmpAgentSwitchIpInterfaceGet(USMDB_UNIT_CURRENT, data->agentSwitchIpInterfaceIfIndex) != L7_SUCCESS ||
      usmDbIntIfNumFromExtIfNum(data->agentSwitchIpInterfaceIfIndex, &intIfIndex) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentSwitchIPAddressConfigMethod, data->valid) &&
      snmpAgentRtrIntfIpAddressMethodSet(USMDB_UNIT_CURRENT, intIfIndex,
                                         data->agentSwitchIPAddressConfigMethod)
                                         != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  /* if both values are provided, pass them to the usmdb function */
  if (VALID(I_agentSwitchIpInterfaceIpAddress, data->valid) &&
      VALID(I_agentSwitchIpInterfaceNetMask, data->valid) &&
      (usmDbNetmaskValidate32(data->agentSwitchIpInterfaceNetMask) != L7_SUCCESS ||
       usmDbIpRtrIntfIPAddressSet(USMDB_UNIT_CURRENT, intIfIndex,
                                  data->agentSwitchIpInterfaceIpAddress,
                                  data->agentSwitchIpInterfaceNetMask,
                                  L7_INTF_IP_ADDR_METHOD_CONFIG) != L7_SUCCESS))
    return COMMIT_FAILED_ERROR;

  if ((VALID(I_agentSwitchIpInterfaceIpAddress, data->valid) &&
       !VALID(I_agentSwitchIpInterfaceNetMask, data->valid)) ||
      (!VALID(I_agentSwitchIpInterfaceIpAddress, data->valid) &&
       VALID(I_agentSwitchIpInterfaceNetMask, data->valid)))
    return COMMIT_FAILED_ERROR;


/* lvl7_@p1516 start */
  if (VALID(I_agentSwitchIpInterfaceClearIp, data->valid) &&
      snmpAgentSwitchIpInterfaceClearIpSet(USMDB_UNIT_CURRENT, intIfIndex,
                                            data->agentSwitchIpInterfaceClearIp) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;
/* lvl7_@p1516 end */

  if (VALID(I_agentSwitchIpInterfaceRoutingMode, data->valid) &&
      snmpAgentSwitchIpInterfaceRoutingModeSet(USMDB_UNIT_CURRENT, intIfIndex,
                                                data->agentSwitchIpInterfaceRoutingMode) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentSwitchIpInterfaceMtuValue, data->valid) &&
      usmDbIntfIpMtuSet(USMDB_UNIT_CURRENT, intIfIndex,
                        data->agentSwitchIpInterfaceMtuValue) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentSwitchIpInterfaceBandwidth, data->valid) &&
      usmDbIfBandwidthSet( intIfIndex,
        data->agentSwitchIpInterfaceBandwidth) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;


  if(usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_IP_MAP_COMPONENT_ID, L7_IPMAP_FEATURE_PROXY_ARP) == L7_TRUE)
  {
    if (VALID(I_agentSwitchIpInterfaceProxyARPMode, data->valid) &&
        snmpAgentSwitchIpInterfaceProxyARPModeSet(USMDB_UNIT_CURRENT, intIfIndex,
                                                  data->agentSwitchIpInterfaceProxyARPMode) != L7_SUCCESS)
      return COMMIT_FAILED_ERROR;
  }
  else
  {
      /* If Proxy ARP feature not supported */
      return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentSwitchIpInterfaceUnnumberedIfIndex, data->valid) &&
      snmpAgentSwitchIpInterfaceUnnumberedIfIndexSet(USMDB_UNIT_CURRENT, intIfIndex,
                                                     data->agentSwitchIpInterfaceUnnumberedIfIndex) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;
 
  if (VALID(I_agentSwitchIpInterfaceIcmpUnreachables, data->valid) && 
      snmpAgentSwitchIpInterfaceIcmpUnreachablesSet(USMDB_UNIT_CURRENT, intIfIndex,
                                                     data->agentSwitchIpInterfaceIcmpUnreachables) != L7_SUCCESS)
     return COMMIT_FAILED_ERROR;

  if (VALID(I_agentSwitchIpInterfaceIcmpRedirects, data->valid) &&
      snmpAgentSwitchIpInterfaceIcmpRedirectsSet(USMDB_UNIT_CURRENT, intIfIndex,
                                                     data->agentSwitchIpInterfaceIcmpRedirects) != L7_SUCCESS)
     return COMMIT_FAILED_ERROR;

  if (VALID(I_agentSwitchDhcpOperation, data->valid) &&
      snmpAgentDhcpClientOperation(USMDB_UNIT_CURRENT, intIfIndex,
                                   data->agentSwitchDhcpOperation) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  return NO_ERROR;
}

#ifdef SR_agentSwitchIpInterfaceEntry_UNDO
/* add #define SR_agentSwitchIpInterfaceEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchIpInterfaceEntry family.
 */
int
agentSwitchIpInterfaceEntry_undo(doList_t *doHead, doList_t *doCur,
                                 ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchIpInterfaceEntry_UNDO */

#endif /* SETS */

agentSwitchIpRouterDiscoveryEntry_t *
k_agentSwitchIpRouterDiscoveryEntry_get(int serialNum, ContextInfo *contextInfo,
                                        int nominator,
                                        int searchType,
                                        SR_INT32 agentSwitchIpRouterDiscoveryIfIndex)
{
  static agentSwitchIpRouterDiscoveryEntry_t agentSwitchIpRouterDiscoveryEntryData;
  L7_uint32 intIfIndex;

  ZERO_VALID(agentSwitchIpRouterDiscoveryEntryData.valid);
  agentSwitchIpRouterDiscoveryEntryData.agentSwitchIpRouterDiscoveryIfIndex = agentSwitchIpRouterDiscoveryIfIndex;
  SET_VALID(I_agentSwitchIpRouterDiscoveryIfIndex, agentSwitchIpRouterDiscoveryEntryData.valid);

  if ( ( searchType == EXACT ? ( snmpAgentSwitchIpInterfaceGet(USMDB_UNIT_CURRENT, agentSwitchIpRouterDiscoveryEntryData.agentSwitchIpRouterDiscoveryIfIndex) != L7_SUCCESS ) :
         ( snmpAgentSwitchIpInterfaceGet(USMDB_UNIT_CURRENT, agentSwitchIpRouterDiscoveryEntryData.agentSwitchIpRouterDiscoveryIfIndex) != L7_SUCCESS &&
           snmpAgentSwitchIpInterfaceNextGet(agentSwitchIpRouterDiscoveryEntryData.agentSwitchIpRouterDiscoveryIfIndex, &agentSwitchIpRouterDiscoveryEntryData.agentSwitchIpRouterDiscoveryIfIndex) != L7_SUCCESS ) ) ||
       usmDbIntIfNumFromExtIfNum(agentSwitchIpRouterDiscoveryEntryData.agentSwitchIpRouterDiscoveryIfIndex, &intIfIndex) != L7_SUCCESS )
  {
    ZERO_VALID(agentSwitchIpRouterDiscoveryEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_agentSwitchIpRouterDiscoveryIfIndex:
    break;

  case I_agentSwitchIpRouterDiscoveryAdvertiseMode:
    if (snmpAgentSwitchIpRouterDiscoveryAdvertiseModeGet(USMDB_UNIT_CURRENT, intIfIndex,
                                                         &agentSwitchIpRouterDiscoveryEntryData.agentSwitchIpRouterDiscoveryAdvertiseMode) == L7_SUCCESS)
      SET_VALID(I_agentSwitchIpRouterDiscoveryAdvertiseMode, agentSwitchIpRouterDiscoveryEntryData.valid);
    break;

  case I_agentSwitchIpRouterDiscoveryMaxAdvertisementInterval:
    if (usmDbRtrDiscMaxAdvIntervalGet(USMDB_UNIT_CURRENT, intIfIndex,
                                      &agentSwitchIpRouterDiscoveryEntryData.agentSwitchIpRouterDiscoveryMaxAdvertisementInterval) == L7_SUCCESS)
      SET_VALID(I_agentSwitchIpRouterDiscoveryMaxAdvertisementInterval, agentSwitchIpRouterDiscoveryEntryData.valid);
    break;

  case I_agentSwitchIpRouterDiscoveryMinAdvertisementInterval:
    if (usmDbRtrDiscMinAdvIntervalGet(USMDB_UNIT_CURRENT, intIfIndex,
                                      &agentSwitchIpRouterDiscoveryEntryData.agentSwitchIpRouterDiscoveryMinAdvertisementInterval) == L7_SUCCESS)
      SET_VALID(I_agentSwitchIpRouterDiscoveryMinAdvertisementInterval, agentSwitchIpRouterDiscoveryEntryData.valid);
    break;

  case I_agentSwitchIpRouterDiscoveryAdvertisementLifetime:
    if (usmDbRtrDiscAdvLifetimeGet(USMDB_UNIT_CURRENT, intIfIndex,
                                   &agentSwitchIpRouterDiscoveryEntryData.agentSwitchIpRouterDiscoveryAdvertisementLifetime) == L7_SUCCESS)
      SET_VALID(I_agentSwitchIpRouterDiscoveryAdvertisementLifetime, agentSwitchIpRouterDiscoveryEntryData.valid);
    break;

  case I_agentSwitchIpRouterDiscoveryPreferenceLevel:
    if (usmDbRtrDiscPreferenceLevelGet(USMDB_UNIT_CURRENT, intIfIndex,
                                       &agentSwitchIpRouterDiscoveryEntryData.agentSwitchIpRouterDiscoveryPreferenceLevel) == L7_SUCCESS)
      SET_VALID(I_agentSwitchIpRouterDiscoveryPreferenceLevel, agentSwitchIpRouterDiscoveryEntryData.valid);
    break;

  case I_agentSwitchIpRouterDiscoveryAdvertisementAddress:
    if (usmDbRtrDiscAdvAddrGet(USMDB_UNIT_CURRENT, intIfIndex,
                               &agentSwitchIpRouterDiscoveryEntryData.agentSwitchIpRouterDiscoveryAdvertisementAddress) == L7_SUCCESS)
      SET_VALID(I_agentSwitchIpRouterDiscoveryAdvertisementAddress, agentSwitchIpRouterDiscoveryEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentSwitchIpRouterDiscoveryEntryData.valid))
    return(NULL);

  return(&agentSwitchIpRouterDiscoveryEntryData);
}

#ifdef SETS
int
k_agentSwitchIpRouterDiscoveryEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                         doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchIpRouterDiscoveryEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                          doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchIpRouterDiscoveryEntry_set_defaults(doList_t *dp)
{
    agentSwitchIpRouterDiscoveryEntry_t *data = (agentSwitchIpRouterDiscoveryEntry_t *) (dp->data);

    data->agentSwitchIpRouterDiscoveryAdvertiseMode = D_agentSwitchIpRouterDiscoveryAdvertiseMode_enable;
    SET_VALID(I_agentSwitchIpRouterDiscoveryAdvertiseMode, data->valid);
    data->agentSwitchIpRouterDiscoveryMaxAdvertisementInterval = 600;
    SET_VALID(I_agentSwitchIpRouterDiscoveryMaxAdvertisementInterval, data->valid);
    data->agentSwitchIpRouterDiscoveryMinAdvertisementInterval = 450;
    SET_VALID(I_agentSwitchIpRouterDiscoveryMinAdvertisementInterval, data->valid);
    data->agentSwitchIpRouterDiscoveryAdvertisementLifetime = 1800;
    SET_VALID(I_agentSwitchIpRouterDiscoveryAdvertisementLifetime, data->valid);
    data->agentSwitchIpRouterDiscoveryPreferenceLevel = 0;
    SET_VALID(I_agentSwitchIpRouterDiscoveryPreferenceLevel, data->valid);

    return NO_ERROR;
}

int
k_agentSwitchIpRouterDiscoveryEntry_set(agentSwitchIpRouterDiscoveryEntry_t *data,
                                        ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfIndex;

  if (snmpAgentSwitchIpInterfaceGet(USMDB_UNIT_CURRENT, data->agentSwitchIpRouterDiscoveryIfIndex) != L7_SUCCESS ||
      usmDbIntIfNumFromExtIfNum(data->agentSwitchIpRouterDiscoveryIfIndex, &intIfIndex) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentSwitchIpRouterDiscoveryAdvertiseMode, data->valid) &&
      snmpAgentSwitchIpRouterDiscoveryAdvertiseModeSet(USMDB_UNIT_CURRENT, intIfIndex,
                                                       data->agentSwitchIpRouterDiscoveryAdvertiseMode) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentSwitchIpRouterDiscoveryMaxAdvertisementInterval, data->valid) &&
      snmpAgentSwitchIpRouterDiscoveryMaxAdvertisementIntervalSet(USMDB_UNIT_CURRENT, intIfIndex,
                                                                  data->agentSwitchIpRouterDiscoveryMaxAdvertisementInterval) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentSwitchIpRouterDiscoveryMinAdvertisementInterval, data->valid) &&
      snmpAgentSwitchIpRouterDiscoveryMinAdvertisementIntervalSet(USMDB_UNIT_CURRENT, intIfIndex,
                                                                  data->agentSwitchIpRouterDiscoveryMinAdvertisementInterval) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentSwitchIpRouterDiscoveryAdvertisementLifetime, data->valid) &&
      snmpAgentSwitchIpRouterDiscoveryAdvertisementLifetimeSet(USMDB_UNIT_CURRENT, intIfIndex,
                                                               data->agentSwitchIpRouterDiscoveryAdvertisementLifetime) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentSwitchIpRouterDiscoveryPreferenceLevel, data->valid) &&
      (usmDbValidateRtrIntf(USMDB_UNIT_CURRENT, intIfIndex) != L7_SUCCESS ||
      usmDbRtrDiscPreferenceLevelSet(USMDB_UNIT_CURRENT, intIfIndex,
                                     data->agentSwitchIpRouterDiscoveryPreferenceLevel) != L7_SUCCESS))
    return COMMIT_FAILED_ERROR;

  if (VALID(I_agentSwitchIpRouterDiscoveryAdvertisementAddress, data->valid) &&
      (usmDbValidateRtrIntf(USMDB_UNIT_CURRENT, intIfIndex) != L7_SUCCESS ||
      usmDbRtrDiscAdvAddrSet(USMDB_UNIT_CURRENT, intIfIndex,
                             data->agentSwitchIpRouterDiscoveryAdvertisementAddress) != L7_SUCCESS))
    return COMMIT_FAILED_ERROR;

  return NO_ERROR;
}

#ifdef SR_agentSwitchIpRouterDiscoveryEntry_UNDO
/* add #define SR_agentSwitchIpRouterDiscoveryEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchIpRouterDiscoveryEntry family.
 */
int
agentSwitchIpRouterDiscoveryEntry_undo(doList_t *doHead, doList_t *doCur,
                                       ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchIpRouterDiscoveryEntry_UNDO */

#endif /* SETS */

agentSwitchIpVlanEntry_t *
k_agentSwitchIpVlanEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_INT32 agentSwitchIpVlanId)
{
  static agentSwitchIpVlanEntry_t agentSwitchIpVlanEntryData;
  L7_uint32 temp_intIfNum;

  ZERO_VALID(agentSwitchIpVlanEntryData.valid);
  agentSwitchIpVlanEntryData.agentSwitchIpVlanId = agentSwitchIpVlanId;
  SET_VALID(I_agentSwitchIpVlanId, agentSwitchIpVlanEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbIpVlanRtrVlanIdGet(USMDB_UNIT_CURRENT, agentSwitchIpVlanEntryData.agentSwitchIpVlanId) != L7_SUCCESS) :
       ( (usmDbIpVlanRtrVlanIdGet(USMDB_UNIT_CURRENT, agentSwitchIpVlanEntryData.agentSwitchIpVlanId) != L7_SUCCESS) &&
         (usmDbIpVlanRtrVlanIdGetNext(USMDB_UNIT_CURRENT, &agentSwitchIpVlanEntryData.agentSwitchIpVlanId) != L7_SUCCESS) ) )
  {
    ZERO_VALID(agentSwitchIpVlanEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_agentSwitchIpVlanId:
    break;

  case I_agentSwitchIpVlanIfIndex:
    if (usmDbIpVlanRtrVlanIdToIntIfNum(USMDB_UNIT_CURRENT, agentSwitchIpVlanEntryData.agentSwitchIpVlanId, &temp_intIfNum) == L7_SUCCESS &&
        usmDbExtIfNumFromIntIfNum(temp_intIfNum, &agentSwitchIpVlanEntryData.agentSwitchIpVlanIfIndex) == L7_SUCCESS)
      SET_VALID(I_agentSwitchIpVlanIfIndex, agentSwitchIpVlanEntryData.valid);
    break;

  case I_agentSwitchIpVlanRoutingStatus:
    /* all entries in this table are active */
    agentSwitchIpVlanEntryData.agentSwitchIpVlanRoutingStatus = D_agentSwitchIpVlanRoutingStatus_active;
    SET_VALID(I_agentSwitchIpVlanRoutingStatus, agentSwitchIpVlanEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentSwitchIpVlanEntryData.valid))
    return(NULL);

  return(&agentSwitchIpVlanEntryData);
}

#ifdef SETS
int
k_agentSwitchIpVlanEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchIpVlanEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchIpVlanEntry_set_defaults(doList_t *dp)
{
    return NO_ERROR;
}

int
k_agentSwitchIpVlanEntry_set(agentSwitchIpVlanEntry_t *data,
                             ContextInfo *contextInfo, int function)
{

  if (usmDbIpVlanRtrVlanIdGet(USMDB_UNIT_CURRENT, data->agentSwitchIpVlanId) != L7_SUCCESS)
  {
    if (VALID(I_agentSwitchIpVlanRoutingStatus, data->valid))
    {
      if (data->agentSwitchIpVlanRoutingStatus != D_agentSwitchIpVlanRoutingStatus_createAndGo ||
          usmDbIpVlanRoutingIntfCreate(USMDB_UNIT_CURRENT, data->agentSwitchIpVlanId, 0) != L7_SUCCESS)
        return COMMIT_FAILED_ERROR;

      CLR_VALID(I_agentSwitchIpVlanRoutingStatus, data->valid);
    }
  }

  if (VALID(I_agentSwitchIpVlanRoutingStatus, data->valid))
  {
    if (data->agentSwitchIpVlanRoutingStatus != D_agentSwitchIpVlanRoutingStatus_active &&
        (data->agentSwitchIpVlanRoutingStatus != D_agentSwitchIpVlanRoutingStatus_destroy ||
         usmDbIpVlanRoutingIntfDelete(USMDB_UNIT_CURRENT, data->agentSwitchIpVlanId) != L7_SUCCESS))
      return COMMIT_FAILED_ERROR;
  }

  return NO_ERROR;
}

#ifdef SR_agentSwitchIpVlanEntry_UNDO
/* add #define SR_agentSwitchIpVlanEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchIpVlanEntry family.
 */
int
agentSwitchIpVlanEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchIpVlanEntry_UNDO */

#endif /* SETS */

agentSwitchSecondaryAddressEntry_t *
k_agentSwitchSecondaryAddressEntry_get(int serialNum, ContextInfo *contextInfo,
                                       int nominator,
                                       int searchType,
                                       SR_INT32 agentSwitchIpInterfaceIfIndex,
                                       SR_UINT32 agentSwitchSecondaryIpAddress)
{
  static agentSwitchSecondaryAddressEntry_t agentSwitchSecondaryAddressEntryData;

  agentSwitchSecondaryAddressEntryData.agentSwitchIpInterfaceIfIndex = agentSwitchIpInterfaceIfIndex;
  SET_VALID(I_agentSwitchSecondaryAddressEntryIndex_agentSwitchIpInterfaceIfIndex, agentSwitchSecondaryAddressEntryData.valid);

  agentSwitchSecondaryAddressEntryData.agentSwitchSecondaryIpAddress = agentSwitchSecondaryIpAddress;
  SET_VALID(I_agentSwitchSecondaryIpAddress, agentSwitchSecondaryAddressEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpAgentSwitchSecondaryAddressEntryGet(USMDB_UNIT_CURRENT,
                                               agentSwitchSecondaryAddressEntryData.agentSwitchIpInterfaceIfIndex,
                                               agentSwitchSecondaryAddressEntryData.agentSwitchSecondaryIpAddress,
                                               &agentSwitchSecondaryAddressEntryData.agentSwitchSecondaryNetMask) != L7_SUCCESS) :
      ((snmpAgentSwitchSecondaryAddressEntryGet(USMDB_UNIT_CURRENT,
                                                agentSwitchSecondaryAddressEntryData.agentSwitchIpInterfaceIfIndex,
                                                agentSwitchSecondaryAddressEntryData.agentSwitchSecondaryIpAddress,
                                                &agentSwitchSecondaryAddressEntryData.agentSwitchSecondaryNetMask) != L7_SUCCESS) &&
       (snmpAgentSwitchSecondaryAddressEntryNextGet(USMDB_UNIT_CURRENT,
                                                    &agentSwitchSecondaryAddressEntryData.agentSwitchIpInterfaceIfIndex,
                                                    &agentSwitchSecondaryAddressEntryData.agentSwitchSecondaryIpAddress,
                                                    &agentSwitchSecondaryAddressEntryData.agentSwitchSecondaryNetMask) != L7_SUCCESS)))
    return(NULL);

  SET_VALID(I_agentSwitchSecondaryNetMask, agentSwitchSecondaryAddressEntryData.valid);

  switch (nominator)
  {
  case -1:
  case I_agentSwitchSecondaryAddressEntryIndex_agentSwitchIpInterfaceIfIndex:
  case I_agentSwitchSecondaryIpAddress:
  case I_agentSwitchSecondaryNetMask:
      if (nominator != -1) break;

  case I_agentSwitchSecondaryStatus:
    agentSwitchSecondaryAddressEntryData.agentSwitchSecondaryStatus = D_agentSwitchSecondaryStatus_active;
    SET_VALID(nominator, agentSwitchSecondaryAddressEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator != 0 && !VALID(nominator, agentSwitchSecondaryAddressEntryData.valid))
    return(NULL);

  return(&agentSwitchSecondaryAddressEntryData);
}

#ifdef SETS
int
k_agentSwitchSecondaryAddressEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchSecondaryAddressEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                         doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchSecondaryAddressEntry_set_defaults(doList_t *dp)
{
    agentSwitchSecondaryAddressEntry_t *data = (agentSwitchSecondaryAddressEntry_t *) (dp->data);


    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentSwitchSecondaryAddressEntry_set(agentSwitchSecondaryAddressEntry_t *data,
                                       ContextInfo *contextInfo, int function)
{
  L7_uint32 agentSwitchSecondaryNetMask=0;
  L7_uint32 intIfIndex=0;

  if (usmDbIntIfNumFromExtIfNum(data->agentSwitchIpInterfaceIfIndex, &intIfIndex) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if(usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_IP_MAP_COMPONENT_ID, L7_IPMAP_FEATURE_MULTINETTING) != L7_TRUE)
  {
    /* If multinetting is not supported */
    return COMMIT_FAILED_ERROR;
  }

  /* check to see if this entry exists allready */
  if (snmpAgentSwitchSecondaryAddressEntryGet(USMDB_UNIT_CURRENT,
                                              data->agentSwitchIpInterfaceIfIndex,
                                              data->agentSwitchSecondaryIpAddress,
                                              &agentSwitchSecondaryNetMask) != L7_SUCCESS)
  {
    /* check to see if creation is being performed */
    if (VALID(I_agentSwitchSecondaryStatus, data->valid) &&
        data->agentSwitchSecondaryStatus == D_agentSwitchSecondaryStatus_createAndGo &&
        VALID(I_agentSwitchSecondaryNetMask, data->valid) &&
        usmDbIpRtrIntfSecondaryIpAddrAdd(USMDB_UNIT_CURRENT, intIfIndex,
                                         data->agentSwitchSecondaryIpAddress,
                                         data->agentSwitchSecondaryNetMask) == L7_SUCCESS)
    {
      CLR_VALID(I_agentSwitchSecondaryNetMask, data->valid);
      CLR_VALID(I_agentSwitchSecondaryStatus, data->valid);
    }
    else
    {
      return COMMIT_FAILED_ERROR;
    }
  }

  if (VALID(I_agentSwitchSecondaryNetMask, data->valid))
  {
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentSwitchSecondaryStatus, data->valid))
  {
    /* if removing the entry */
    if (data->agentSwitchSecondaryStatus == D_agentSwitchSecondaryStatus_destroy)
    {
      if (usmDbIpRtrIntfSecondaryIpAddrRemove(USMDB_UNIT_CURRENT, intIfIndex,
                                              data->agentSwitchSecondaryIpAddress,
                                              agentSwitchSecondaryNetMask) != L7_SUCCESS)
        return COMMIT_FAILED_ERROR;
    }
    /* if not setting it active */
    else if (data->agentSwitchSecondaryStatus != D_agentSwitchSecondaryStatus_active)
    {
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentSwitchSecondaryAddressEntry_UNDO
/* add #define SR_agentSwitchSecondaryAddressEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchSecondaryAddressEntry family.
 */
int
agentSwitchSecondaryAddressEntry_undo(doList_t *doHead, doList_t *doCur,
                                      ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchSecondaryAddressEntry_UNDO */

#endif /* SETS */

/* Helper Address */

agentSwitchHelperAddressEntry_t *
k_agentSwitchHelperAddressEntry_get(int serialNum, ContextInfo *contextInfo,
                                    int nominator,
                                    int searchType,
                                    SR_INT32 agentSwitchIpInterfaceIfIndex,
                                    SR_UINT32 agentSwitchHelperIpAddress)
{
  /* Obsolete */
  return(NULL);
}

#ifdef SETS
int
k_agentSwitchHelperAddressEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchHelperAddressEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}


int
k_agentSwitchHelperAddressEntry_set_defaults(doList_t *dp)
{
    agentSwitchHelperAddressEntry_t *data = (agentSwitchHelperAddressEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentSwitchHelperAddressEntry_set(agentSwitchHelperAddressEntry_t *data,
                                    ContextInfo *contextInfo, int function)
{
  /* Obsolete */
  return COMMIT_FAILED_ERROR;
}


#ifdef SR_agentSwitchHelperAddressEntry_UNDO
/* add #define SR_agentSwitchHelperAddressEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchHelperAddressEntry family.
 */
int
agentSwitchHelperAddressEntry_undo(doList_t *doHead, doList_t *doCur,
                                   ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchHelperAddressEntry_UNDO */

#endif /* SETS */


agentInternalVlanGroup_t *
k_agentInternalVlanGroup_get(int serialNum, ContextInfo *contextInfo,
                             int nominator)
{
   static agentInternalVlanGroup_t agentInternalVlanGroupData;

   agentInternalVlanGroupData.agentInternalVlanBase = usmDbDot1qBaseInternalVlanIdGet();
   agentInternalVlanGroupData.agentInternalVlanPolicy = usmDbDot1qInternalVlanPolicyGet();
   SET_ALL_VALID(agentInternalVlanGroupData.valid);
   return(&agentInternalVlanGroupData);
}

#ifdef SETS
int
k_agentInternalVlanGroup_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentInternalVlanGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                               doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentInternalVlanGroup_set(agentInternalVlanGroup_t *data,
                             ContextInfo *contextInfo, int function)
{
   /* Defining temporary variable for storing the valid bits for the case when the
    * set request is only paritally successful
    */
   L7_char8 tempValid[sizeof(data->valid)];

   bzero(tempValid, sizeof(tempValid));

   if (VALID(I_agentInternalVlanPolicy, data->valid))
   {
     if (usmDbDot1qInternalVlanPolicySet(data->agentInternalVlanPolicy) != L7_SUCCESS)
     {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
     }
     else
     {
       SET_VALID(I_agentInternalVlanPolicy, tempValid);
     }
   }

   return NO_ERROR;
}

#ifdef SR_agentInternalVlanGroup_UNDO
/* add #define SR_agentInternalVlanGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentInternalVlanGroup family.
 */
int
agentInternalVlanGroup_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentInternalVlanGroup_UNDO */

#endif /* SETS */

agentSwitchInternalVlanEntry_t *
k_agentSwitchInternalVlanEntry_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator,
                                   int searchType,
                                   SR_INT32 agentSwitchInternalVlanId)
{
   static agentSwitchInternalVlanEntry_t vlanEntry;

   vlanEntry.agentSwitchInternalVlanId = agentSwitchInternalVlanId;
   if (snmpAgentSwitchInternalVlanEntryGet(USMDB_UNIT_CURRENT,
                                           agentSwitchInternalVlanId,
                                           &vlanEntry.agentSwitchInternalVlanIfIndex) == L7_SUCCESS)
   {
     SET_ALL_VALID(vlanEntry.valid);
     return &vlanEntry;
   }

   if (searchType != EXACT)
   {
     if (snmpAgentSwitchInternalVlanEntryNextGet(USMDB_UNIT_CURRENT,
                                                 &vlanEntry.agentSwitchInternalVlanId,
                                                 &vlanEntry.agentSwitchInternalVlanIfIndex) == L7_SUCCESS)
     {
       SET_ALL_VALID(vlanEntry.valid);
       return(&vlanEntry);
     }
   }
   return NULL;
}


agentBootpDhcpRelayGroup_t *
k_agentBootpDhcpRelayGroup_get(int serialNum, ContextInfo *contextInfo,
                 int nominator)
{
  static agentBootpDhcpRelayGroup_t agentBootpDhcpRelayGroupData;

  ZERO_VALID(agentBootpDhcpRelayGroupData.valid);

  switch(nominator)
  {
  case -1:
  case I_agentBootpDhcpRelayMaxHopCount:
    if (usmDbBootpDhcpRelayMaxHopCountGet(USMDB_UNIT_CURRENT, &agentBootpDhcpRelayGroupData.agentBootpDhcpRelayMaxHopCount) == L7_SUCCESS)
      SET_VALID(I_agentBootpDhcpRelayMaxHopCount, agentBootpDhcpRelayGroupData.valid);
   if(nominator != -1)
    break;

  case I_agentBootpDhcpRelayForwardingIp:
    /* obsolete */
   if(nominator != -1)
    break;

  case I_agentBootpDhcpRelayForwardMode:
    /* obsolete */
   if(nominator != -1)
    break;

  case I_agentBootpDhcpRelayMinWaitTime:
    if (usmDbBootpDhcpRelayMinWaitTimeGet(USMDB_UNIT_CURRENT, &agentBootpDhcpRelayGroupData.agentBootpDhcpRelayMinWaitTime) == L7_SUCCESS)
      SET_VALID(I_agentBootpDhcpRelayMinWaitTime, agentBootpDhcpRelayGroupData.valid);
   if(nominator != -1)
    break;

  case I_agentBootpDhcpRelayCircuitIdOptionMode:
    if (snmpBootpDhcpRelayCircuitIdOptionModeGet(USMDB_UNIT_CURRENT, &agentBootpDhcpRelayGroupData.agentBootpDhcpRelayCircuitIdOptionMode) == L7_SUCCESS)
      SET_VALID(I_agentBootpDhcpRelayCircuitIdOptionMode, agentBootpDhcpRelayGroupData.valid);
   if(nominator != -1)
    break;

  case I_agentBootpDhcpRelayNumOfRequestsReceived:
    /* obsolete */
   if(nominator != -1)
    break;

  case I_agentBootpDhcpRelayNumOfRequestsForwarded:
    /* obsolete */
   if(nominator != -1)
    break;

  case I_agentBootpDhcpRelayNumOfDiscards:
    /* obsolete */
   if(nominator != -1)
    break;

#ifdef I_agentBootpDhcpRelayCheckReply
  case I_agentBootpDhcpRelayCheckReply:
    if (snmpBootpDhcpRelayCheckReplyModeGet(&agentBootpDhcpRelayGroupData.agentBootpDhcpRelayCheckReply) == L7_SUCCESS)
       SET_VALID(I_agentBootpDhcpRelayCheckReply, agentBootpDhcpRelayGroupData.valid);
    break;
#endif

  default:
    return(NULL);
    break;
  }

   if (nominator != -1 && !VALID(nominator, agentBootpDhcpRelayGroupData.valid))
   {
    return(NULL);
   }

  return(&agentBootpDhcpRelayGroupData);
}

#ifdef SETS
int
k_agentBootpDhcpRelayGroup_test(ObjectInfo *object, ObjectSyntax *value,
                  doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentBootpDhcpRelayGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                   doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentBootpDhcpRelayGroup_set(agentBootpDhcpRelayGroup_t *data,
                 ContextInfo *contextInfo, int function)
{

   if (VALID(I_agentBootpDhcpRelayMaxHopCount, data->valid) &&
       usmDbBootpDhcpRelayMaxHopCountSet(USMDB_UNIT_CURRENT, data->agentBootpDhcpRelayMaxHopCount) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_agentBootpDhcpRelayForwardingIp, data->valid)) 
     /* obsolete */
      return COMMIT_FAILED_ERROR ;

   if (VALID(I_agentBootpDhcpRelayForwardMode, data->valid))
     /* obsolete */
      return COMMIT_FAILED_ERROR;

   if (VALID(I_agentBootpDhcpRelayMinWaitTime, data->valid) &&
       usmDbBootpDhcpRelayMinWaitTimeSet(USMDB_UNIT_CURRENT, data->agentBootpDhcpRelayMinWaitTime) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);

   if (VALID(I_agentBootpDhcpRelayCircuitIdOptionMode, data->valid) &&
       snmpBootpDhcpRelayCircuitIdOptionModeSet(USMDB_UNIT_CURRENT, data->agentBootpDhcpRelayCircuitIdOptionMode) != L7_SUCCESS)
      return(COMMIT_FAILED_ERROR);

#ifdef I_agentBootpDhcpRelayCheckReply
   if (VALID(I_agentBootpDhcpRelayCheckReply, data->valid) &&
       snmpBootpDhcpRelayCheckReplyModeSet(data->agentBootpDhcpRelayCheckReply) != L7_SUCCESS)
       return(COMMIT_FAILED_ERROR);
#endif

   return(NO_ERROR);
}


#ifdef SR_agentBootpDhcpRelayGroup_UNDO
/* add #define SR_agentBootpDhcpRelayGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentBootpDhcpRelayGroup family.
 */
int
agentBootpDhcpRelayGroup_undo(doList_t *doHead, doList_t *doCur,
                ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentBootpDhcpRelayGroup_UNDO */

#endif /* SETS */

#if L7_FEAT_DHCP_RELAY_OPTION82_ENHANCEMENTS
agentBootpDhcpRelayIntfEntry_t *
k_agentBootpDhcpRelayIntfEntry_get(int serialNum, ContextInfo *contextInfo,
                  int nominator, int searchType, SR_INT32 agentBootpVlanId)
{
  static agentBootpDhcpRelayIntfEntry_t agentBootpDhcpRelayIntfEntryData;
  L7_uint32 temp_intIfNum;
  L7_RC_t rc;

  ZERO_VALID(agentBootpDhcpRelayIntfEntryData.valid);
  agentBootpDhcpRelayIntfEntryData.agentBootpVlanId = agentBootpVlanId;
  SET_VALID(I_agentBootpVlanId, agentBootpDhcpRelayIntfEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbIpVlanRtrVlanIdGet(USMDB_UNIT_CURRENT, agentBootpDhcpRelayIntfEntryData.agentBootpVlanId) != L7_SUCCESS) :
       ( (usmDbIpVlanRtrVlanIdGet(USMDB_UNIT_CURRENT, agentBootpDhcpRelayIntfEntryData.agentBootpVlanId) != L7_SUCCESS) &&
         (usmDbIpVlanRtrVlanIdGetNext(USMDB_UNIT_CURRENT, &agentBootpDhcpRelayIntfEntryData.agentBootpVlanId) != L7_SUCCESS) ) )
  {
    ZERO_VALID(agentBootpDhcpRelayIntfEntryData.valid);
    return (NULL);
  }

  rc =  usmDbIpVlanRtrVlanIdToIntIfNum(USMDB_UNIT_CURRENT, agentBootpDhcpRelayIntfEntryData.agentBootpVlanId, &temp_intIfNum); 

  switch(nominator)
  {
    case -1:
    case I_agentBootpVlanId:
        if(nominator != -1)
         break;

#ifdef I_agentBootpDhcpRelayIntfCircuitIdOptionMode
    case I_agentBootpDhcpRelayIntfCircuitIdOptionMode:
        if((rc == L7_SUCCESS) && (snmpAgentBootpDhcpRelayIntfEntryCIDOptionGet(temp_intIfNum, 
               &agentBootpDhcpRelayIntfEntryData.agentBootpDhcpRelayIntfCircuitIdOptionMode) == L7_SUCCESS))
         SET_VALID(I_agentBootpDhcpRelayIntfCircuitIdOptionMode, agentBootpDhcpRelayIntfEntryData.valid);
        if(nominator != -1)
         break;
#endif

#ifdef I_agentBootpDhcpRelayIntfCheckReply
    case I_agentBootpDhcpRelayIntfCheckReply:
        if((rc == L7_SUCCESS) && (snmpAgentBootpDhcpRelayIntfEntryCIDCheckReplyGet(temp_intIfNum, 
                    &agentBootpDhcpRelayIntfEntryData.agentBootpDhcpRelayIntfCheckReply) == L7_SUCCESS))
         SET_VALID(I_agentBootpDhcpRelayIntfCheckReply, agentBootpDhcpRelayIntfEntryData.valid);
         break; 
#endif

    default:
        return NULL;
        break;	
  }

  if (nominator != -1 && !VALID(nominator, agentBootpDhcpRelayIntfEntryData.valid))
  {
    return(NULL);
  }
  
  return(&agentBootpDhcpRelayIntfEntryData);
}

#ifdef SETS

int k_agentBootpDhcpRelayIntfEntry_test(ObjectInfo *object, ObjectSyntax *value,
                  doList_t *dp, ContextInfo *contextInfo)

{
   return NO_ERROR;
}

int k_agentBootpDhcpRelayIntfEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                  doList_t *doHead, doList_t *dp)
{
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int k_agentBootpDhcpRelayIntfEntry_set_defaults(doList_t *dp)
{
  agentBootpDhcpRelayIntfEntry_t *data;

   data = (agentBootpDhcpRelayIntfEntry_t *) (dp->data);

#ifdef I_agentBootpDhcpRelayIntfCircuitIdOptionMode
   data->agentBootpDhcpRelayIntfCircuitIdOptionMode = D_agentBootpDhcpRelayIntfCircuitIdOptionMode_disable;
   SET_VALID(I_agentBootpDhcpRelayIntfCircuitIdOptionMode, data->valid);
#endif
#ifdef I_agentBootpDhcpRelayIntfCheckReply
   data->agentBootpDhcpRelayIntfCheckReply = D_agentBootpDhcpRelayIntfCheckReply_enable;
   SET_VALID(I_agentBootpDhcpRelayIntfCheckReply, data->valid);
#endif

   return NO_ERROR;
}

int k_agentBootpDhcpRelayIntfEntry_set(agentBootpDhcpRelayIntfEntry_t *data, 
                                       ContextInfo *contextInfo,int function)
{
  L7_uint32 temp_intIfNum;

  if ((usmDbIpVlanRtrVlanIdGet(USMDB_UNIT_CURRENT, data->agentBootpVlanId) != L7_SUCCESS) ||
     (usmDbIpVlanRtrVlanIdToIntIfNum(USMDB_UNIT_CURRENT, data->agentBootpVlanId, &temp_intIfNum) != L7_SUCCESS))
  {
    return COMMIT_FAILED_ERROR;
  }

#ifdef I_agentBootpDhcpRelayIntfCircuitIdOptionMode
  if(VALID(I_agentBootpDhcpRelayIntfCircuitIdOptionMode, data->valid) &&
     (snmpAgentBootpDhcpRelayIntfEntryCIDOptionSet(temp_intIfNum, data->agentBootpDhcpRelayIntfCircuitIdOptionMode)!= L7_SUCCESS))
     return COMMIT_FAILED_ERROR;
#endif

#ifdef I_agentBootpDhcpRelayIntfCheckReply
  if(VALID(I_agentBootpDhcpRelayIntfCheckReply, data->valid) && 
     (snmpAgentBootpDhcpRelayIntfEntryCIDCheckReplySet(temp_intIfNum, data->agentBootpDhcpRelayIntfCheckReply)!= L7_SUCCESS))
    return COMMIT_FAILED_ERROR;
#endif
 
  return NO_ERROR;
}

#endif
#endif /* L7_FEAT_DHCP_RELAY_OPTION82_ENHANCEMENTS */


 
agentSwitchIpIcmpControlGroup_t *
k_agentSwitchIpIcmpControlGroup_get(int serialNum, ContextInfo *contextInfo,
                                int nominator)
{
  static agentSwitchIpIcmpControlGroup_t agentSwitchIpIcmpControlGroupData;
  L7_uint32 interval, burstsize;
  L7_RC_t rc = L7_FAILURE;
  
  ZERO_VALID(agentSwitchIpIcmpControlGroupData.valid);
   rc =  usmDbIpMapRtrICMPRatelimitGet (&burstsize, &interval);
   switch (nominator)
   {
     case I_agentSwitchIpIcmpEchoReplyMode:
       if (snmpAgentSwitchIpIcmpEchoReplyModeGet(&agentSwitchIpIcmpControlGroupData.agentSwitchIpIcmpEchoReplyMode)
            == L7_SUCCESS)
         SET_VALID(I_agentSwitchIpIcmpEchoReplyMode, agentSwitchIpIcmpControlGroupData.valid);
       break;
     case I_agentSwitchIpIcmpRedirectsMode:
       if (snmpAgentSwitchIpIcmpRedirectsGet(&agentSwitchIpIcmpControlGroupData.agentSwitchIpIcmpRedirectsMode)
           == L7_SUCCESS)
         SET_VALID(I_agentSwitchIpIcmpRedirectsMode, agentSwitchIpIcmpControlGroupData.valid);
       break;
     case I_agentSwitchIpIcmpRateLimitInterval:
       if (rc == L7_SUCCESS)
       {
         agentSwitchIpIcmpControlGroupData.agentSwitchIpIcmpRateLimitInterval = interval;
         SET_VALID(I_agentSwitchIpIcmpRateLimitInterval, agentSwitchIpIcmpControlGroupData.valid);
       }
       break;
     case I_agentSwitchIpIcmpRateLimitBurstSize:      
       if (rc == L7_SUCCESS)
       {
         agentSwitchIpIcmpControlGroupData.agentSwitchIpIcmpRateLimitBurstSize = burstsize;
         SET_VALID(I_agentSwitchIpIcmpRateLimitBurstSize, agentSwitchIpIcmpControlGroupData.valid);
       }

       break;
     default:
     /* unknown nominator */
     return(NULL);
     break;
   }

   return(&agentSwitchIpIcmpControlGroupData);
}

#ifdef SETS
int
k_agentSwitchIpIcmpControlGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchIpIcmpControlGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                  doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}
int
k_agentSwitchIpIcmpControlGroup_set_defaults(doList_t *dp)
{
    agentSwitchIpIcmpControlGroup_t *data = (agentSwitchIpIcmpControlGroup_t *) (dp->data);

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentSwitchIpIcmpControlGroup_set(agentSwitchIpIcmpControlGroup_t *data,
                                ContextInfo *contextInfo, int function)
{

  if (VALID(I_agentSwitchIpIcmpEchoReplyMode, data->valid) &&
      snmpAgentSwitchIpIcmpEchoReplyModeSet (data->agentSwitchIpIcmpEchoReplyMode)!= L7_SUCCESS)
  {
    CLR_VALID(I_agentSwitchIpIcmpEchoReplyMode, data->valid);
    return COMMIT_FAILED_ERROR;
  }
  if (VALID(I_agentSwitchIpIcmpRedirectsMode, data->valid) &&
          snmpAgentSwitchIpIcmpRedirectsSet (data->agentSwitchIpIcmpRedirectsMode) != L7_SUCCESS)
  {
    CLR_VALID(I_agentSwitchIpIcmpRedirectsMode, data->valid);
    return COMMIT_FAILED_ERROR;
  }
  if (VALID(I_agentSwitchIpIcmpRateLimitInterval, data->valid) && 
      snmpAgentSwitchIpIcmpRatelimitIntervalSet(data->agentSwitchIpIcmpRateLimitInterval) != L7_SUCCESS)
  {
    CLR_VALID(I_agentSwitchIpIcmpRateLimitInterval, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentSwitchIpIcmpRateLimitBurstSize,  data->valid) && 
          snmpAgentSwitchIpIcmpRatelimitBurstSizeSet(data->agentSwitchIpIcmpRateLimitBurstSize) != L7_SUCCESS)
  {
    CLR_VALID(I_agentSwitchIpIcmpRateLimitBurstSize, data->valid);
    return COMMIT_FAILED_ERROR;
  }

   return NO_ERROR;
}


#ifdef SR_agentSwitchIpIcmpControlGroup_UNDO
/* add #define SR_agentSwitchIpIcmpControlGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchIpIcmpControlGroup family.
 */
int
agentSwitchIpIcmpControlGroup_undo(doList_t *doHead, doList_t *doCur,
                                   ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchIpIcmpControlGroup_UNDO */

#endif /* SETS */



/* IP helper addresses on interface */
agentSwitchIntfIpHelperAddressEntry_t *
k_agentSwitchIntfIpHelperAddressEntry_get(int serialNum, ContextInfo *contextInfo,
                                          int nominator,
                                          int searchType,
                                          SR_INT32 agentSwitchIpInterfaceIfIndex,
                                          SR_UINT32 agentSwitchIntfIpHelperUdpPort,
                                          SR_UINT32 agentSwitchIntfIpHelperIpAddress)
{
  static agentSwitchIntfIpHelperAddressEntry_t agentSwitchIntfIpHelperAddressEntryData;

  agentSwitchIntfIpHelperAddressEntryData.agentSwitchIpInterfaceIfIndex = agentSwitchIpInterfaceIfIndex;
  SET_VALID(I_agentSwitchIntfIpHelperAddressEntryIndex_agentSwitchIpInterfaceIfIndex,
            agentSwitchIntfIpHelperAddressEntryData.valid);

  agentSwitchIntfIpHelperAddressEntryData.agentSwitchIntfIpHelperIpAddress = agentSwitchIntfIpHelperIpAddress;
  SET_VALID(I_agentSwitchIntfIpHelperIpAddress, agentSwitchIntfIpHelperAddressEntryData.valid);

  agentSwitchIntfIpHelperAddressEntryData.agentSwitchIntfIpHelperUdpPort = agentSwitchIntfIpHelperUdpPort;
  SET_VALID(I_agentSwitchIntfIpHelperUdpPort, agentSwitchIntfIpHelperAddressEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpAgentSwitchIntfIpHelperAddressEntryGet(agentSwitchIntfIpHelperAddressEntryData.agentSwitchIpInterfaceIfIndex,
                                                  agentSwitchIntfIpHelperAddressEntryData.agentSwitchIntfIpHelperIpAddress,
                                                  agentSwitchIntfIpHelperAddressEntryData.agentSwitchIntfIpHelperUdpPort,
                                                  agentSwitchIntfIpHelperAddressEntryData.agentSwitchIntfIpHelperDiscard,
                                                  &agentSwitchIntfIpHelperAddressEntryData.agentSwitchIntfIpHelperHitCount)
       != L7_SUCCESS) :
      ((snmpAgentSwitchIntfIpHelperAddressEntryGet(agentSwitchIntfIpHelperAddressEntryData.agentSwitchIpInterfaceIfIndex,
                                                   agentSwitchIntfIpHelperAddressEntryData.agentSwitchIntfIpHelperIpAddress,
                                                   agentSwitchIntfIpHelperAddressEntryData.agentSwitchIntfIpHelperUdpPort,
                                                   agentSwitchIntfIpHelperAddressEntryData.agentSwitchIntfIpHelperDiscard,
                                                   &agentSwitchIntfIpHelperAddressEntryData.agentSwitchIntfIpHelperHitCount)
        != L7_SUCCESS) &&
       (snmpAgentSwitchIntfIpHelperAddressEntryNextGet(&agentSwitchIntfIpHelperAddressEntryData.agentSwitchIpInterfaceIfIndex,
                                                       &agentSwitchIntfIpHelperAddressEntryData.agentSwitchIntfIpHelperIpAddress,
                                                       &agentSwitchIntfIpHelperAddressEntryData.agentSwitchIntfIpHelperUdpPort,
                                                       &agentSwitchIntfIpHelperAddressEntryData.agentSwitchIntfIpHelperDiscard,
                                                       &agentSwitchIntfIpHelperAddressEntryData.agentSwitchIntfIpHelperHitCount)
        != L7_SUCCESS)))
  {
    return(NULL);
  }

  /* MIB uses a helper address of 0.0.0.0 to identify a discard entry. Since the
   * helper address is a table index, it must have a value for every entry. */
  if (agentSwitchIntfIpHelperAddressEntryData.agentSwitchIntfIpHelperDiscard == L7_TRUE)
  {
    agentSwitchIntfIpHelperAddressEntryData.agentSwitchIntfIpHelperIpAddress = 0;
  }

  switch (nominator)
  {
  case -1:
  case I_agentSwitchIntfIpHelperAddressEntryIndex_agentSwitchIpInterfaceIfIndex:
  case I_agentSwitchIntfIpHelperIpAddress:
  case I_agentSwitchIntfIpHelperUdpPort:
  case I_agentSwitchIntfIpHelperHitCount:
    SET_VALID(nominator, agentSwitchIntfIpHelperAddressEntryData.valid);
    break;

  case I_agentSwitchIntfIpHelperDiscard:
    SET_VALID(I_agentSwitchIntfIpHelperDiscard, agentSwitchIntfIpHelperAddressEntryData.valid);
    break;

  case I_agentSwitchIntfIpHelperStatus:
    agentSwitchIntfIpHelperAddressEntryData.agentSwitchIntfIpHelperStatus = D_agentSwitchIntfIpHelperStatus_active;
    SET_VALID(nominator, agentSwitchIntfIpHelperAddressEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
  }

  if (nominator != 0 && !VALID(nominator, agentSwitchIntfIpHelperAddressEntryData.valid))
  {
    return(NULL);
  }

  return(&agentSwitchIntfIpHelperAddressEntryData);
}

#ifdef SETS
int
k_agentSwitchIntfIpHelperAddressEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                           doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchIntfIpHelperAddressEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                            doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchIntfIpHelperAddressEntry_set_defaults(doList_t *dp)
{
    agentSwitchIntfIpHelperAddressEntry_t *data = (agentSwitchIntfIpHelperAddressEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentSwitchIntfIpHelperAddressEntry_set(agentSwitchIntfIpHelperAddressEntry_t *data,
                                          ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum = 0;
  L7_RC_t rc = L7_FAILURE;

  if (usmDbIntIfNumFromExtIfNum(data->agentSwitchIpInterfaceIfIndex, &intIfNum) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  /* check to see if this entry exists already */
  if (snmpAgentSwitchIntfIpHelperAddressEntryGet(data->agentSwitchIpInterfaceIfIndex,
                                                 data->agentSwitchIntfIpHelperIpAddress,
                                                 data->agentSwitchIntfIpHelperUdpPort,
                                                 data->agentSwitchIntfIpHelperDiscard,
                                                 &data->agentSwitchIntfIpHelperHitCount) != L7_SUCCESS)
  {
    /* No matching entry exists. Create new. */
    if (VALID(I_agentSwitchIntfIpHelperStatus, data->valid) &&
        (data->agentSwitchIntfIpHelperStatus == D_agentSwitchIntfIpHelperStatus_createAndGo) &&
        VALID(I_agentSwitchIntfIpHelperUdpPort, data->valid))
    {
      if (VALID(I_agentSwitchIntfIpHelperIpAddress, data->valid))
      {
        if (data->agentSwitchIntfIpHelperIpAddress == 0)
        {
          /* For SNMP, address value of 0 indicates discard entry. */
          rc = usmDbIpHelperDiscardEntryAdd(intIfNum, data->agentSwitchIntfIpHelperUdpPort);
        }
        else
        {
          /* Non-zero helper address. Create normal entry. */
          rc = usmDbIpHelperAddressAdd(intIfNum, data->agentSwitchIntfIpHelperUdpPort,
                                       data->agentSwitchIntfIpHelperIpAddress);
        }
      }
    }
    if (rc == L7_SUCCESS)
    {
      CLR_VALID(I_agentSwitchIntfIpHelperStatus, data->valid);
    }
    else
    {
      /* Failed to add new helper address entry. */
      return COMMIT_FAILED_ERROR;
    }
  }

  if (VALID(I_agentSwitchIntfIpHelperStatus, data->valid))
  {
    /* if removing the entry */
    if (data->agentSwitchIntfIpHelperStatus == D_agentSwitchSecondaryStatus_destroy)
    {
      if (VALID(I_agentSwitchIntfIpHelperIpAddress, data->valid))
      {
        if (data->agentSwitchIntfIpHelperIpAddress == 0)
        {
          rc = usmDbIpHelperDiscardEntryDelete(intIfNum, data->agentSwitchIntfIpHelperUdpPort);
        }
        else
        {
          rc = usmDbIpHelperAddressDelete(intIfNum, data->agentSwitchIntfIpHelperUdpPort,
                                          data->agentSwitchIntfIpHelperIpAddress);
        }
      }
      if (rc != L7_SUCCESS)
      {
        return COMMIT_FAILED_ERROR;
      }
    }
    else if (data->agentSwitchIntfIpHelperStatus != D_agentSwitchSecondaryStatus_active)
    {
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentSwitchIntfIpHelperAddressEntry_UNDO
/* add #define SR_agentSwitchIntfIpHelperAddressEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchIntfIpHelperAddressEntry family.
 */
int
agentSwitchIntfIpHelperAddressEntry_undo(doList_t *doHead, doList_t *doCur,
                                         ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchIntfIpHelperAddressEntry_UNDO */

#endif /* SETS */


/* Global IP helper configuration */
agentIpHelperGroup_t *
k_agentIpHelperGroup_get(int serialNum, ContextInfo *contextInfo, int nominator)
{
   static agentIpHelperGroup_t agentIpHelperGroupData;
   ipHelperStats_t ihStats;

   ZERO_VALID(agentIpHelperGroupData.valid);

   if (usmDbIpHelperStatisticsGet(&ihStats) != L7_SUCCESS)
   {
     return NULL;
   }

   switch (nominator)
   {
   case -1:
     break;

   case I_agentIpHelperAdminMode :
     if (snmpAgentIpHelperAdminModeGet(&agentIpHelperGroupData.agentIpHelperAdminMode) == L7_SUCCESS)
       SET_VALID(I_agentIpHelperAdminMode, agentIpHelperGroupData.valid);
     break;

     case I_agentDhcpClientMsgsReceived:
       agentIpHelperGroupData.agentDhcpClientMsgsReceived = ihStats.dhcpClientMsgsReceived;
       SET_VALID(I_agentDhcpClientMsgsReceived, agentIpHelperGroupData.valid);
     break;

     case I_agentDhcpClientMsgsRelayed:
       agentIpHelperGroupData.agentDhcpClientMsgsRelayed = ihStats.dhcpClientMsgsRelayed;
       SET_VALID(I_agentDhcpClientMsgsRelayed, agentIpHelperGroupData.valid);
     break;

     case I_agentDhcpServerMsgsReceived:
       agentIpHelperGroupData.agentDhcpServerMsgsReceived = ihStats.dhcpServerMsgsReceived;
       SET_VALID(I_agentDhcpServerMsgsReceived, agentIpHelperGroupData.valid);
     break;

     case I_agentDhcpServerMsgsRelayed:
       agentIpHelperGroupData.agentDhcpServerMsgsRelayed = ihStats.dhcpServerMsgsReceived;
       SET_VALID(I_agentDhcpServerMsgsRelayed, agentIpHelperGroupData.valid);
     break;

     case I_agentUdpClientMsgsReceived:
       agentIpHelperGroupData.agentUdpClientMsgsReceived = ihStats.udpClientMsgsReceived;
       SET_VALID(I_agentUdpClientMsgsReceived, agentIpHelperGroupData.valid);
     break;

     case I_agentUdpClientMsgsRelayed:
       agentIpHelperGroupData.agentUdpClientMsgsRelayed = ihStats.udpClientMsgsReceived;
       SET_VALID(I_agentUdpClientMsgsRelayed, agentIpHelperGroupData.valid);
     break;

     case I_agentUdpClientMsgsTtlExpired:
       agentIpHelperGroupData.agentUdpClientMsgsTtlExpired = ihStats.ttlExpired;
       SET_VALID(I_agentUdpClientMsgsTtlExpired, agentIpHelperGroupData.valid);
     break;

     case I_agentUdpClientMsgsDiscarded:
       agentIpHelperGroupData.agentUdpClientMsgsDiscarded = ihStats.matchDiscardEntry;
       SET_VALID(I_agentUdpClientMsgsDiscarded, agentIpHelperGroupData.valid);
     break;

   default:
     /* unknown nominator */
     return(NULL);
     break;
   }

   if (nominator >= 0 && !VALID(nominator, agentIpHelperGroupData.valid))
     return(NULL);

   return &agentIpHelperGroupData;
}

#ifdef SETS
int
k_agentIpHelperGroup_test(ObjectInfo *object, ObjectSyntax *value,
                          doList_t *dp, ContextInfo *contextInfo)
{
    return NO_ERROR;
}

int
k_agentIpHelperGroup_ready(ObjectInfo *object, ObjectSyntax *value, 
                           doList_t *doHead, doList_t *dp)
{
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentIpHelperGroup_set(agentIpHelperGroup_t *data,
                         ContextInfo *contextInfo, int function)
{
  if (VALID(I_agentIpHelperAdminMode, data->valid) &&
      snmpAgentIpHelperAdminModeSet(data->agentIpHelperAdminMode)!= L7_SUCCESS)
  {
    CLR_VALID(I_agentIpHelperAdminMode, data->valid);
    return COMMIT_FAILED_ERROR;
  }

   return NO_ERROR;
}

#ifdef SR_agentIpHelperGroup_UNDO
/* add #define SR_agentIpHelperGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentIpHelperGroup family.
 */
int
agentIpHelperGroup_undo(doList_t *doHead, doList_t *doCur,
                        ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentIpHelperGroup_UNDO */

#endif /* SETS */

agentSwitchIpHelperAddressEntry_t *
k_agentSwitchIpHelperAddressEntry_get(int serialNum, ContextInfo *contextInfo,
                                      int nominator,
                                      int searchType,
                                      SR_UINT32 agentSwitchIpHelperAddress,
                                      SR_UINT32 agentSwitchIpHelperUdpPort)
{
  static agentSwitchIpHelperAddressEntry_t agentSwitchIpHelperAddressEntryData;

  agentSwitchIpHelperAddressEntryData.agentSwitchIpHelperAddress = agentSwitchIpHelperAddress;
  agentSwitchIpHelperAddressEntryData.agentSwitchIpHelperUdpPort = agentSwitchIpHelperUdpPort;

  if ((searchType == EXACT) ?
      (snmpAgentSwitchIpHelperAddressEntryGet(agentSwitchIpHelperAddressEntryData.agentSwitchIpHelperAddress,
                                              agentSwitchIpHelperAddressEntryData.agentSwitchIpHelperUdpPort,
                                              &agentSwitchIpHelperAddressEntryData.agentSwitchIpHelperHitCount)
       != L7_SUCCESS) :
      ((snmpAgentSwitchIpHelperAddressEntryGet(agentSwitchIpHelperAddressEntryData.agentSwitchIpHelperAddress,
                                               agentSwitchIpHelperAddressEntryData.agentSwitchIpHelperUdpPort,
                                               &agentSwitchIpHelperAddressEntryData.agentSwitchIpHelperHitCount)
        != L7_SUCCESS) &&
       (snmpAgentSwitchIpHelperAddressEntryNextGet(&agentSwitchIpHelperAddressEntryData.agentSwitchIpHelperAddress,
                                                   &agentSwitchIpHelperAddressEntryData.agentSwitchIpHelperUdpPort,
                                                   &agentSwitchIpHelperAddressEntryData.agentSwitchIpHelperHitCount)
        != L7_SUCCESS)))
  {
    return(NULL);
  }

  switch (nominator)
  {
    case -1:
    case I_agentSwitchIpHelperAddress:
      SET_VALID(I_agentSwitchIpHelperAddress, agentSwitchIpHelperAddressEntryData.valid);
      break;
    case I_agentSwitchIpHelperUdpPort:
      SET_VALID(I_agentSwitchIpHelperUdpPort, agentSwitchIpHelperAddressEntryData.valid);
      break;
    case I_agentSwitchIpHelperHitCount:
      SET_VALID(nominator, agentSwitchIpHelperAddressEntryData.valid);
      break;
    case I_agentSwitchIpHelperStatus:
      agentSwitchIpHelperAddressEntryData.agentSwitchIpHelperStatus = D_agentSwitchIpHelperStatus_active;
      SET_VALID(nominator, agentSwitchIpHelperAddressEntryData.valid);
      break;

    default:
      /* unknown nominator */
      return(NULL);
  }

  if (nominator != 0 && !VALID(nominator, agentSwitchIpHelperAddressEntryData.valid))
  {
    return(NULL);
  }

  return &agentSwitchIpHelperAddressEntryData;
}

#ifdef SETS
int
k_agentSwitchIpHelperAddressEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                       doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchIpHelperAddressEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                        doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchIpHelperAddressEntry_set_defaults(doList_t *dp)
{
    agentSwitchIpHelperAddressEntry_t *data = (agentSwitchIpHelperAddressEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentSwitchIpHelperAddressEntry_set(agentSwitchIpHelperAddressEntry_t *data,
                                      ContextInfo *contextInfo, int function)
{
  /* check to see if this entry exists already */
  if (snmpAgentSwitchIpHelperAddressEntryGet(data->agentSwitchIpHelperAddress,
                                             data->agentSwitchIpHelperUdpPort,
                                             &data->agentSwitchIpHelperHitCount) != L7_SUCCESS)
  {
    /* check to see if creating new entry */
    if (VALID(I_agentSwitchIpHelperStatus, data->valid) &&
        data->agentSwitchIpHelperStatus == D_agentSwitchIpHelperStatus_createAndGo &&
        VALID(I_agentSwitchIpHelperUdpPort, data->valid) &&
        VALID(I_agentSwitchIpHelperAddress, data->valid))
    {
      if (usmDbIpHelperAddressAdd(IH_INTF_ANY, data->agentSwitchIpHelperUdpPort,
                                  data->agentSwitchIpHelperAddress) == L7_SUCCESS)
      {
        CLR_VALID(I_agentSwitchIpHelperStatus, data->valid);
      }
      else
      {
        return COMMIT_FAILED_ERROR;
      }
    }
  }
  
  if (VALID(I_agentSwitchIpHelperStatus, data->valid))
  {
    /* if removing the entry */
    if ((data->agentSwitchIpHelperStatus == D_agentSwitchSecondaryStatus_destroy) &&
        VALID(I_agentSwitchIpHelperUdpPort, data->valid) &&
        VALID(I_agentSwitchIpHelperAddress, data->valid))
    {
      if (usmDbIpHelperAddressDelete(IH_INTF_ANY, data->agentSwitchIpHelperUdpPort,
                                     data->agentSwitchIpHelperAddress) != L7_SUCCESS)
      {
        return COMMIT_FAILED_ERROR;
      }
    }
    /* if not setting it active */
    else if (data->agentSwitchIpHelperStatus != D_agentSwitchSecondaryStatus_active)
    {
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentSwitchIpHelperAddressEntry_UNDO
/* add #define SR_agentSwitchIpHelperAddressEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchIpHelperAddressEntry family.
 */
int
agentSwitchIpHelperAddressEntry_undo(doList_t *doHead, doList_t *doCur,
                                     ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchIpHelperAddressEntry_UNDO */

#endif /* SETS */
