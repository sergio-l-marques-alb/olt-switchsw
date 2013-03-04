/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename k_mib_fastpath_keying.c
*
* @purpose  Provide interface to keying Private MIB
*
* @component SNMP
*
* @comments
*
* @create 03/16/2004
*
* @author Suhel Goel
* @end
*
**********************************************************************/
#include <k_private_base.h>
#include "k_mib_fastpath_keying_api.h"
#include "usmdb_cnfgr_api.h"
#include "usmdb_common.h"
#include "usmdb_keying_api.h"
#include "usmdb_util_api.h"
#include "sim_exports.h"

agentFeatureKeyingGroup_t *
k_agentFeatureKeyingGroup_get(int serialNum, ContextInfo *contextInfo,
                              int nominator)
{
   static agentFeatureKeyingGroup_t agentFeatureKeyingGroupData;


   /*Check if keying feature is supported*/
   if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SIM_COMPONENT_ID, L7_SIM_FEATURE_KEYING_FEATURE_ID) != L7_TRUE)
   {
     return(NULL);
   }

   /*
    * put your code to retrieve the information here
    */

   agentFeatureKeyingGroupData.agentFeatureKeyingEnableKey = MakeOctetString(NULL, 0);
   agentFeatureKeyingGroupData.agentFeatureKeyingDisableKey = MakeOctetString(NULL, 0);
   SET_ALL_VALID(agentFeatureKeyingGroupData.valid);
   return(&agentFeatureKeyingGroupData);
}

#ifdef SETS

int
k_agentFeatureKeyingGroup_test(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *dp, ContextInfo *contextInfo)
{
    /*Check if keying feature is supported*/
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SIM_COMPONENT_ID, L7_SIM_FEATURE_KEYING_FEATURE_ID) != L7_TRUE)
    {
      return(NO_ACCESS_ERROR);
    }

    return NO_ERROR;
}

int
k_agentFeatureKeyingGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentFeatureKeyingGroup_set(agentFeatureKeyingGroup_t *data,
                              ContextInfo *contextInfo, int function)
{
  L7_uchar8 snmp_buffer[SNMP_BUFFER_LEN];


  /*Check if keying feature is supported*/
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SIM_COMPONENT_ID, L7_SIM_FEATURE_KEYING_FEATURE_ID) != L7_TRUE)
  {
    return(NO_ACCESS_ERROR);
  }

  if (VALID(I_agentFeatureKeyingEnableKey, data->valid) && (data->agentFeatureKeyingEnableKey != NULL))
    {
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
      memcpy(snmp_buffer, data->agentFeatureKeyingEnableKey->octet_ptr, data->agentFeatureKeyingEnableKey->length);
      if (snmpFeatureKeyingEnableValidate(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS)
        return(COMMIT_FAILED_ERROR);
    }


  if (VALID(I_agentFeatureKeyingDisableKey, data->valid) && (data->agentFeatureKeyingDisableKey != NULL))
    {
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
      memcpy(snmp_buffer, data->agentFeatureKeyingDisableKey->octet_ptr, data->agentFeatureKeyingDisableKey->length);
      if (snmpFeatureKeyingDisableValidate(USMDB_UNIT_CURRENT, snmp_buffer) != L7_SUCCESS)
        return(COMMIT_FAILED_ERROR);
    }
  return NO_ERROR;
}

#ifdef SR_agentFeatureKeyingGroup_UNDO
/* add #define SR_agentFeatureKeyingGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentFeatureKeyingGroup family.
 */
int
agentFeatureKeyingGroup_undo(doList_t *doHead, doList_t *doCur,
                             ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentFeatureKeyingGroup_UNDO */

#endif /* SETS */

agentFeatureKeyingEntry_t *
k_agentFeatureKeyingEntry_get(int serialNum, ContextInfo *contextInfo,
                              int nominator,
                              int searchType,
                              SR_UINT32 agentFeatureKeyingIndex)
{

  static agentFeatureKeyingEntry_t agentFeatureKeyingEntryData;
  L7_uint32 componentId = 0;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 mode = 0, keyable = 0;


  /*Check if keying feature is supported*/
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SIM_COMPONENT_ID, L7_SIM_FEATURE_KEYING_FEATURE_ID) != L7_TRUE)
  {
    return(NULL);
  }
  
  ZERO_VALID(agentFeatureKeyingEntryData.valid);
  agentFeatureKeyingEntryData.agentFeatureKeyingIndex = agentFeatureKeyingIndex;
  SET_VALID(I_agentFeatureKeyingIndex, agentFeatureKeyingEntryData.valid);

   /* Validating the row-entries for GET and GETNEXT operations */
   if (searchType == EXACT)
   {
     if (usmDbComponentIndexGet(agentFeatureKeyingEntryData.agentFeatureKeyingIndex,
                                                          &componentId) != L7_SUCCESS) 
      return(NULL);
   }
   
   else if (searchType == NEXT)
   {
     if ((usmDbComponentIndexGet(agentFeatureKeyingEntryData.agentFeatureKeyingIndex, &componentId) != L7_SUCCESS)
             && (usmDbComponentIndexNextGet(&agentFeatureKeyingEntryData.agentFeatureKeyingIndex,&componentId) != L7_SUCCESS))
       return(NULL);
     else
     {
       usmDbFeatureKeyGet(USMDB_UNIT_CURRENT, componentId, &mode, &keyable);

       while (keyable == L7_FALSE) 
       {
         if (usmDbComponentIndexNextGet(&agentFeatureKeyingEntryData.agentFeatureKeyingIndex, &componentId) == L7_SUCCESS)
           usmDbFeatureKeyGet(USMDB_UNIT_CURRENT, componentId, &mode, &keyable);
         else
           return(NULL);
       }
     }   
     
   }
   
   
   switch (nominator)
     {
     case -1:
     case I_agentFeatureKeyingIndex :
       break;
     case I_agentFeatureKeyingName :
           bzero(snmp_buffer, SNMP_BUFFER_LEN);
           if (usmDbComponentNameGet(componentId, snmp_buffer) == L7_SUCCESS &&
                         SafeMakeOctetStringFromTextExact(&agentFeatureKeyingEntryData.agentFeatureKeyingName,
                                         snmp_buffer) == L7_TRUE)
           SET_VALID(I_agentFeatureKeyingName,agentFeatureKeyingEntryData.valid);
           break;
     
     case I_agentFeatureKeyingStatus :
       if (snmpAgentFeatureKeyingStatusGet(USMDB_UNIT_CURRENT, componentId,
                           &agentFeatureKeyingEntryData.agentFeatureKeyingStatus) == L7_SUCCESS)
       SET_VALID(I_agentFeatureKeyingStatus, agentFeatureKeyingEntryData.valid);
       break;
       
       
     default:
       /* unknown nominator */
         return(NULL);
         break;
   }
   
   if (nominator >= 0 && !VALID(nominator, agentFeatureKeyingEntryData.valid))
     return(NULL);
   
   return(&agentFeatureKeyingEntryData);
   
}

