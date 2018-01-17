/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_fastpath_portsecurity.c
*
* @purpose    Port Security agent code 
*
* @component  SNMP
*
* @comments
*
* @create    06/08/2004
*
* @author     kmanish
* @end
*
**********************************************************************/
#include "k_private_base.h"
#include "k_mib_fastpath_portsecurity_api.h"
#include "usmdb_pml_api.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"

agentPortSecurityGroup_t *
k_agentPortSecurityGroup_get(int serialNum, ContextInfo *contextInfo,
                               int nominator)
{
  static agentPortSecurityGroup_t agentPortSecurityGroupData;

  switch (nominator)
  {
    case -1:
      break;

    case I_agentGlobalPortSecurityMode:
        if (snmpAgentGlobalPortSecurityModeGet(&agentPortSecurityGroupData.agentGlobalPortSecurityMode) == L7_SUCCESS)
        SET_VALID(I_agentGlobalPortSecurityMode, agentPortSecurityGroupData.valid);
      break;

    default:
        /* unknown nominator */
        return(NULL);
      break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentPortSecurityGroupData.valid) )
      return(NULL);

  return(&agentPortSecurityGroupData);
}

#ifdef SETS
int
k_agentPortSecurityGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentPortSecurityGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentPortSecurityGroup_set(agentPortSecurityGroup_t *data,
                               ContextInfo *contextInfo, int function)
{

  if (VALID(I_agentGlobalPortSecurityMode, data->valid) &&
      snmpAgentGlobalPortSecurityModeSet(data->agentGlobalPortSecurityMode) != L7_SUCCESS)
  {
    CLR_VALID(I_agentGlobalPortSecurityMode, data->valid);
    return COMMIT_FAILED_ERROR;
  }

  return NO_ERROR;
}

#ifdef SR_agentPortSecurityGroup_UNDO
/* add #define SR_agentPortSecurityGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentPortSecurityGroup family.
 */
int
agentPortSecurityGroup_undo(doList_t *doHead, doList_t *doCur,
                              ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentPortSecurityGroup_UNDO */

#endif /* SETS */

agentPortSecurityEntry_t *
k_agentPortSecurityEntry_get(int serialNum, ContextInfo *contextInfo,
                               int nominator,
                               int searchType,
                               SR_INT32 ifIndex)
{
   static agentPortSecurityEntry_t agentPortSecurityEntryData;
   static L7_BOOL firstTime = L7_TRUE;
   L7_uint32 intIfIndex;

   char snmp_buffer[SNMP_BUFFER_LEN];
   
   ZERO_VALID(agentPortSecurityEntryData.valid);
   bzero(snmp_buffer, sizeof(snmp_buffer));
   agentPortSecurityEntryData.ifIndex = ifIndex;
   SET_VALID(I_agentPortSecurityEntryIndex_ifIndex, agentPortSecurityEntryData.valid);

   if (firstTime == L7_TRUE)
   {
     agentPortSecurityEntryData.agentPortSecurityStaticMACs = MakeOctetString(NULL, 0);
     agentPortSecurityEntryData.agentPortSecurityLastDiscardedMAC = MakeOctetString(NULL, 0);
     agentPortSecurityEntryData.agentPortSecurityMACAddressAdd = MakeOctetString(NULL, 0);
     agentPortSecurityEntryData.agentPortSecurityMACAddressRemove = MakeOctetString(NULL, 0);
     firstTime = L7_FALSE;
   }

     /* Validating the row-entries for GET and GETNEXT operations */

  if ((searchType == EXACT ? ( snmpAgentPortSecurityEntryGet(USMDB_UNIT_CURRENT, agentPortSecurityEntryData.ifIndex) !=
       L7_SUCCESS ) : ( snmpAgentPortSecurityEntryGet(USMDB_UNIT_CURRENT, agentPortSecurityEntryData.ifIndex) != L7_SUCCESS
       && snmpAgentPortSecurityEntryGetNext(USMDB_UNIT_CURRENT, &agentPortSecurityEntryData.ifIndex) != L7_SUCCESS ) ) ||
       usmDbIntIfNumFromExtIfNum(agentPortSecurityEntryData.ifIndex, &intIfIndex) != L7_SUCCESS )
  {
      ZERO_VALID(agentPortSecurityEntryData.valid);
      return(NULL);
  }

  
  switch (nominator)
  {
    case -1:
    case I_agentPortSecurityEntryIndex_ifIndex:
       break;
      
    case I_agentPortSecurityMode:
         if (snmpAgentPortSecurityModeGet(intIfIndex, &agentPortSecurityEntryData.agentPortSecurityMode) == L7_SUCCESS)
         {
           SET_VALID(I_agentPortSecurityMode, agentPortSecurityEntryData.valid);
         } 
       break;

    case I_agentPortSecurityDynamicLimit:
         if (usmDbPmlIntfDynamicLimitGet(USMDB_UNIT_CURRENT, intIfIndex, 
                      &agentPortSecurityEntryData.agentPortSecurityDynamicLimit) == L7_SUCCESS)
         {
           SET_VALID(I_agentPortSecurityDynamicLimit, agentPortSecurityEntryData.valid);
         } 
       break;

    case I_agentPortSecurityStaticLimit:
         if (usmDbPmlIntfStaticLimitGet(USMDB_UNIT_CURRENT, intIfIndex, 
             &agentPortSecurityEntryData.agentPortSecurityStaticLimit) == L7_SUCCESS)
         {
           SET_VALID(I_agentPortSecurityStaticLimit, agentPortSecurityEntryData.valid);
         } 
       break;

    case I_agentPortSecurityViolationTrapMode:
         if (snmpAgentPortSecurityViolationTrapModeGet(intIfIndex, 
             &agentPortSecurityEntryData.agentPortSecurityViolationTrapMode) == L7_SUCCESS)
         {
           SET_VALID(I_agentPortSecurityViolationTrapMode, agentPortSecurityEntryData.valid);
         } 
       break;   

    case I_agentPortSecurityStaticMACs :
         bzero(snmp_buffer, SNMP_BUFFER_LEN);
         if (snmpAgentPortSecurityStaticMACsGet(intIfIndex, snmp_buffer) == L7_SUCCESS &&
           SafeMakeOctetStringFromText(&agentPortSecurityEntryData.agentPortSecurityStaticMACs, snmp_buffer) == L7_TRUE)
         SET_VALID(I_agentPortSecurityStaticMACs, agentPortSecurityEntryData.valid);
         break;
    
    case I_agentPortSecurityLastDiscardedMAC :
         bzero(snmp_buffer, SNMP_BUFFER_LEN);
         if (snmpAgentPortSecurityLastDiscardedMACGet(intIfIndex, snmp_buffer) == L7_SUCCESS &&
           SafeMakeOctetStringFromText(&agentPortSecurityEntryData.agentPortSecurityLastDiscardedMAC,
                                        snmp_buffer) == L7_TRUE)
         SET_VALID(I_agentPortSecurityLastDiscardedMAC, agentPortSecurityEntryData.valid);
         break;
   
    case I_agentPortSecurityMACAddressAdd:

           SET_VALID(I_agentPortSecurityMACAddressAdd, agentPortSecurityEntryData.valid);
         break;

    case I_agentPortSecurityMACAddressRemove:

           SET_VALID(I_agentPortSecurityMACAddressRemove, agentPortSecurityEntryData.valid);
         break;

    case I_agentPortSecurityMACAddressMove:
           agentPortSecurityEntryData.agentPortSecurityMACAddressMove = D_agentPortSecurityMACAddressMove_disable;
           SET_VALID(I_agentPortSecurityMACAddressMove, agentPortSecurityEntryData.valid);
         break;

    default:
           /* unknown nominator */
           return(NULL);
         break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentPortSecurityEntryData.valid) )
    return(NULL);

  return(&agentPortSecurityEntryData);
}





#ifdef SETS
int
k_agentPortSecurityEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentPortSecurityEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentPortSecurityEntry_set_defaults(doList_t *dp)
{
    agentPortSecurityEntry_t *data = (agentPortSecurityEntry_t *) (dp->data);

    data->agentPortSecurityMode = D_agentPortSecurityMode_disable;
    if ((data->agentPortSecurityStaticMACs = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->agentPortSecurityLastDiscardedMAC = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    
    if ((data->agentPortSecurityMACAddressAdd = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    
    if ((data->agentPortSecurityMACAddressRemove = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}


int
k_agentPortSecurityEntry_set(agentPortSecurityEntry_t *data,
                               ContextInfo *contextInfo, int function)
{

   char snmp_buffer[SNMP_BUFFER_LEN];

   L7_uint32 intIfIndex;

   if ( usmDbIntIfNumFromExtIfNum(data->ifIndex, &intIfIndex) != L7_SUCCESS )
    {
      ZERO_VALID(data->valid);
      return(WRONG_VALUE_ERROR);
    }


   if (VALID(I_agentPortSecurityMode, data->valid) &&
       (snmpAgentPortSecurityModeSet(intIfIndex,
                                   data->agentPortSecurityMode) != L7_SUCCESS))
       return(COMMIT_FAILED_ERROR);


   if (VALID(I_agentPortSecurityDynamicLimit, data->valid) &&
       (usmDbPmlIntfDynamicLimitSet(USMDB_UNIT_CURRENT,
                                   intIfIndex,
                                   data->agentPortSecurityDynamicLimit) != L7_SUCCESS))
       return(COMMIT_FAILED_ERROR);


   if (VALID(I_agentPortSecurityStaticLimit, data->valid) &&
       (usmDbPmlIntfStaticLimitSet(USMDB_UNIT_CURRENT,
                                   intIfIndex,
                                   data->agentPortSecurityStaticLimit) != L7_SUCCESS))
       return(COMMIT_FAILED_ERROR);


   if (VALID(I_agentPortSecurityViolationTrapMode, data->valid) &&
       (snmpAgentPortSecurityViolationTrapModeSet(intIfIndex,
                                   data->agentPortSecurityViolationTrapMode) != L7_SUCCESS))
       return(COMMIT_FAILED_ERROR);


   if (VALID(I_agentPortSecurityMACAddressAdd, data->valid))
   {
     bzero(snmp_buffer, SNMP_BUFFER_LEN);
     memcpy(snmp_buffer, data->agentPortSecurityMACAddressAdd->octet_ptr, data->agentPortSecurityMACAddressAdd->length);
     if (snmpAgentPortSecurityMACAddressAdd(intIfIndex,snmp_buffer) != L7_SUCCESS)
       return(COMMIT_FAILED_ERROR);
   }

  
   if (VALID(I_agentPortSecurityMACAddressRemove, data->valid))
   {
     bzero(snmp_buffer, SNMP_BUFFER_LEN);
     memcpy(snmp_buffer, data->agentPortSecurityMACAddressRemove->octet_ptr, data->agentPortSecurityMACAddressRemove->length);
     if (snmpAgentPortSecurityMACAddressRemove(intIfIndex,snmp_buffer) != L7_SUCCESS)
       return(COMMIT_FAILED_ERROR);
   }
   
   if (VALID(I_agentPortSecurityMACAddressMove, data->valid) &&
    (snmpAgentPortSecurityMACAddressMove(intIfIndex,
                                data->agentPortSecurityMACAddressMove) != L7_SUCCESS))
     return(COMMIT_FAILED_ERROR);
  
   return(NO_ERROR);
}

#ifdef SR_agentPortSecurityEntry_UNDO
/* add #define SR_agentPortSecurityEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentPortSecurityEntry family.
 */
int
agentPortSecurityEntry_undo(doList_t *doHead, doList_t *doCur,
                              ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentPortSecurityEntry_UNDO */

#endif /* SETS */

agentPortSecurityDynamicEntry_t *
k_agentPortSecurityDynamicEntry_get(int serialNum, ContextInfo *contextInfo,
                                      int nominator,
                                      int searchType,
                                      SR_INT32 ifIndex,
                                      SR_UINT32 agentPortSecurityDynamicVLANId,
                                      OctetString *agentPortSecurityDynamicMACAddress
                                      )
{

   static agentPortSecurityDynamicEntry_t agentPortSecurityDynamicEntryData;

   static L7_BOOL firstTime = L7_TRUE;

   L7_uint32 intIfIndex;

   L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

   ZERO_VALID(agentPortSecurityDynamicEntryData.valid);
   

   if (firstTime == L7_TRUE)
   {
     firstTime = L7_FALSE;
     agentPortSecurityDynamicEntryData.agentPortSecurityDynamicMACAddress = MakeOctetString(NULL, 0);
   }

   /* Populating the ifIndex field */
   agentPortSecurityDynamicEntryData.ifIndex = ifIndex;
   SET_VALID(I_agentPortSecurityDynamicEntryIndex_ifIndex, agentPortSecurityDynamicEntryData.valid);

   /* Populating the MAC Address field */
   bzero(snmp_buffer, SNMP_BUFFER_LEN);
   memcpy(snmp_buffer, agentPortSecurityDynamicMACAddress->octet_ptr, agentPortSecurityDynamicMACAddress->length);
   SET_VALID(I_agentPortSecurityDynamicMACAddress, agentPortSecurityDynamicEntryData.valid);

   /* Populating the VLAN Id field */
   agentPortSecurityDynamicEntryData.agentPortSecurityDynamicVLANId = agentPortSecurityDynamicVLANId;
   SET_VALID(I_agentPortSecurityDynamicVLANId, agentPortSecurityDynamicEntryData.valid);
   

   /* Validating the row-entries for GET and GETNEXT operations */ 
   if ((searchType == EXACT ? ( snmpAgentPortSecurityDynamicEntryGet(USMDB_UNIT_CURRENT, agentPortSecurityDynamicEntryData.ifIndex,
                                                                    snmp_buffer, 
                                                                    (SR_UINT16)agentPortSecurityDynamicEntryData.agentPortSecurityDynamicVLANId) != L7_SUCCESS ) :
        ((snmpAgentPortSecurityDynamicEntryGet(USMDB_UNIT_CURRENT, agentPortSecurityDynamicEntryData.ifIndex,
                                               snmp_buffer,
                                               (SR_UINT16)agentPortSecurityDynamicEntryData.agentPortSecurityDynamicVLANId) != L7_SUCCESS) && 
         (snmpAgentPortSecurityDynamicEntryGetNext(USMDB_UNIT_CURRENT, 
                                                   &agentPortSecurityDynamicEntryData.ifIndex,
                                                   snmp_buffer,
                                                   &agentPortSecurityDynamicEntryData.agentPortSecurityDynamicVLANId) != L7_SUCCESS) ) ) ||
      usmDbIntIfNumFromExtIfNum(agentPortSecurityDynamicEntryData.ifIndex, &intIfIndex) != L7_SUCCESS )
   {
     ZERO_VALID(agentPortSecurityDynamicEntryData.valid);
     return(NULL);
   }

   SET_VALID(I_agentPortSecurityDynamicEntryIndex_ifIndex, agentPortSecurityDynamicEntryData.valid);

   if(SafeMakeOctetString(&agentPortSecurityDynamicEntryData.agentPortSecurityDynamicMACAddress, 
                       snmp_buffer,L7_MAC_ADDR_LEN) == L7_TRUE)
   {
     SET_VALID(I_agentPortSecurityDynamicMACAddress, agentPortSecurityDynamicEntryData.valid);
   }

   /*agentPortSecurityDynamicEntryData.agentPortSecurityDynamicVLANId = (SR_UINT32)vlanTemp;*/
   SET_VALID(I_agentPortSecurityDynamicVLANId, agentPortSecurityDynamicEntryData.valid);


   switch (nominator)
   {
    case -1:
    case I_agentPortSecurityDynamicEntryIndex_ifIndex:
    case I_agentPortSecurityDynamicMACAddress:
    case I_agentPortSecurityDynamicVLANId: 
      break;
   
    default:
     /* unknown nominator */
     return(NULL);
     break;
   }

   if (nominator >= 0 && !VALID(nominator, agentPortSecurityDynamicEntryData.valid))
     return(NULL);

   return(&agentPortSecurityDynamicEntryData);
}

