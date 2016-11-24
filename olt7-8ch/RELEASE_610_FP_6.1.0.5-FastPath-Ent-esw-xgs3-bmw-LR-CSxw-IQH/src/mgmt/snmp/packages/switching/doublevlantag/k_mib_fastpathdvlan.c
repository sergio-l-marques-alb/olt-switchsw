/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_fastpathdvlan.c
*
* @purpose    System-Specific code to support Double vlan tagging component
*
* @component  SNMP
*
* @comments
*
* @create     3/6/2007
*
* @author     nshrivastav
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"
#include "k_private_base.h"
#include "k_mib_fastpathdvlan_api.h"
#include "usmdb_dvlantag_api.h"

#ifdef I_agentSwitchDVlanTagTPid
agentSwitchDVlanTagEntry_t *
k_agentSwitchDVlanTagEntry_get(int serialNum, ContextInfo *contextInfo,
                               int nominator, int searchType, SR_INT32 agentSwitchDVlanTagTPID)
{
   static agentSwitchDVlanTagEntry_t agentSwitchDVlanTagEntryData;
   static L7_BOOL firstTime = L7_TRUE;

   
   if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DVLANTAG_COMPONENT_ID,
                                L7_DVLANTAG_ETHERTYPE_FEATURE_ID) == L7_FALSE)
     return(NULL);

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentSwitchDVlanTagEntryData.agentSwitchDVlanTagTPid = 0;
  }
  ZERO_VALID(agentSwitchDVlanTagEntryData.valid);

  agentSwitchDVlanTagEntryData.agentSwitchDVlanTagTPid = agentSwitchDVlanTagTPID;
  SET_VALID(I_agentSwitchDVlanTagTPid,
             agentSwitchDVlanTagEntryData.valid);

  if(searchType == EXACT)
  {
    if(snmpAgentSwitchDvlanTagEthertypeGet(USMDB_UNIT_CURRENT,
                  (L7_uint32)agentSwitchDVlanTagEntryData.agentSwitchDVlanTagTPid) != L7_SUCCESS)
    {
      ZERO_VALID(agentSwitchDVlanTagEntryData.valid);    
      return (NULL);
    }
  }

  if(searchType != EXACT)
  {
    if(agentSwitchDVlanTagTPID != 0)
    {
      agentSwitchDVlanTagEntryData.agentSwitchDVlanTagTPid = 
                  agentSwitchDVlanTagEntryData.agentSwitchDVlanTagTPid - 1;
    }
    if(snmpAgentSwitchDvlanTagEthertypeNextGet(USMDB_UNIT_CURRENT,
                  (L7_uint32)agentSwitchDVlanTagEntryData.agentSwitchDVlanTagTPid,
                  (L7_uint32 *)&agentSwitchDVlanTagEntryData.agentSwitchDVlanTagTPid) != L7_SUCCESS)
    {
      ZERO_VALID(agentSwitchDVlanTagEntryData.valid);    
      return (NULL);
    }
    
  }

   /*
    * if ( nominator != -1 ) condition is added to all the case statements
    * for storing all the values to support the undo functionality.
    */

   switch (nominator)
   {
   case -1:
#ifdef I_agentSwitchDVlanTagTPid
   case I_agentSwitchDVlanTagTPid:
     if (nominator != -1) break;
#endif
     /* else pass through */


   case I_agentSwitchDVlanTagRowStatus:
     if (snmpAgentSwitchDvlanTagEthertypeGet(USMDB_UNIT_CURRENT,
                  (L7_uint32)agentSwitchDVlanTagEntryData.agentSwitchDVlanTagTPid) == L7_SUCCESS)
     {
       agentSwitchDVlanTagEntryData.agentSwitchDVlanTagRowStatus = 
        D_agentSwitchDVlanTagRowStatus_active;;         
       SET_VALID(I_agentSwitchDVlanTagRowStatus, agentSwitchDVlanTagEntryData.valid);
     }
     if (nominator != -1) break;
     /* else pass through */

#ifdef I_agentSwitchDVlanTagDefaultTPid
   case I_agentSwitchDVlanTagDefaultTPid:
     if (snmpAgentSwitchDvlanTagIsDefaultTPid(USMDB_UNIT_CURRENT,
                 (L7_uint32)agentSwitchDVlanTagEntryData.agentSwitchDVlanTagTPid) == L7_SUCCESS)
     {
       agentSwitchDVlanTagEntryData.agentSwitchDVlanTagDefaultTPid = 
                            D_agentSwitchDVlanTagDefaultTPid_true; /* SNMP TRUE */
     }
     else
     {
       agentSwitchDVlanTagEntryData.agentSwitchDVlanTagDefaultTPid = 
                            D_agentSwitchDVlanTagDefaultTPid_false; /* SNMP FALSE */
     }
     SET_VALID(I_agentSwitchDVlanTagDefaultTPid, agentSwitchDVlanTagEntryData.valid);     
     if (nominator != -1) break;
     /* else pass through */
#endif
   
   default:
     /* unknown nominator */
     return(NULL);
     break;
   }

   if (nominator >= 0 && !VALID(nominator, agentSwitchDVlanTagEntryData.valid))
     return(NULL);

   return(&agentSwitchDVlanTagEntryData);
}

#ifdef SETS
int
k_agentSwitchDVlanTagEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchDVlanTagEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchDVlanTagEntry_set_defaults(doList_t *dp)
{
    agentSwitchDVlanTagEntry_t *data = (agentSwitchDVlanTagEntry_t *) (dp->data);

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentSwitchDVlanTagEntry_set(agentSwitchDVlanTagEntry_t *data,
                               ContextInfo *contextInfo, int function)
{
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DVLANTAG_COMPONENT_ID,
                               L7_DVLANTAG_ETHERTYPE_FEATURE_ID) == L7_FALSE)
  {
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentSwitchDVlanTagTPid, data->valid) &&
    data->agentSwitchDVlanTagRowStatus == D_agentSwitchDVlanTagRowStatus_destroy)
  {
#ifdef I_agentSwitchDVlanTagDefaultTPid
    if (usmDbDvlantagEthertypeSet(USMDB_UNIT_CURRENT,
                               (L7_uint32)data->agentSwitchDVlanTagTPid,
                               data->agentSwitchDVlanTagDefaultTPid,
                               L7_FALSE) != L7_SUCCESS)
#else
    if (usmDbDvlantagEthertypeSet(USMDB_UNIT_CURRENT,
                               (L7_uint32)data->agentSwitchDVlanTagTPid,
                               L7_FALSE,
                               L7_FALSE) != L7_SUCCESS)
#endif
    {
      ZERO_VALID(data->valid);    
      return COMMIT_FAILED_ERROR;
    }
  }
  else if (VALID(I_agentSwitchDVlanTagTPid, data->valid) &&
         data->agentSwitchDVlanTagRowStatus == D_agentSwitchDVlanTagRowStatus_createAndGo)

  {
#ifdef I_agentSwitchDVlanTagDefaultTPid
    if (usmDbDvlantagEthertypeSet(USMDB_UNIT_CURRENT,
                               (L7_uint32)data->agentSwitchDVlanTagTPid,
                               data->agentSwitchDVlanTagDefaultTPid,
                               L7_TRUE) != L7_SUCCESS)
#else
    if (usmDbDvlantagEthertypeSet(USMDB_UNIT_CURRENT,
                               (L7_uint32)data->agentSwitchDVlanTagTPid,
                               L7_FALSE,
                               L7_FALSE) != L7_SUCCESS)
#endif
    {
      ZERO_VALID(data->valid);    
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentSwitchDVlanTagGroup_UNDO
/* add #define SR_agentSwitchDVlanTagGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchDVlanTagGroup family.
 */
int
agentSwitchDVlanTagEntry_undo(doList_t *doHead, doList_t *doCur,
                              ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchDVlanTagGroup_UNDO */

#endif /* SETS */

#endif /* I_agentSwitchDVlanTagTPid */

#ifdef I_agentSwitchPortDVlanTagTPid
agentSwitchPortDVlanTagEntry_t *
k_agentSwitchPortDVlanTagEntry_get(int serialNum, ContextInfo *contextInfo,
                               int nominator, int searchType, SR_INT32 agentSwitchPortDVlanTagInterfaceIfIndex,
                               SR_INT32 agentSwitchPortDVlanTagTPID)
{
  static agentSwitchPortDVlanTagEntry_t agentSwitchPortDVlanTagEntryData;
  static L7_BOOL firstTime = L7_TRUE;

   
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DVLANTAG_COMPONENT_ID,
                            L7_DVLANTAG_ETHERTYPE_FEATURE_ID) == L7_FALSE)
  {                            
    return(NULL);
  }
  
  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentSwitchPortDVlanTagEntryData.agentSwitchPortDVlanTagInterfaceIfIndex = 0;
    agentSwitchPortDVlanTagEntryData.agentSwitchPortDVlanTagTPid = 0;
    agentSwitchPortDVlanTagEntryData.agentSwitchPortDVlanTagMode = 0;
#ifdef I_agentSwitchPortDVlanTagCustomerId
    agentSwitchPortDVlanTagEntryData.agentSwitchPortDVlanTagCustomerId = 0;
#endif
    agentSwitchPortDVlanTagEntryData.agentSwitchPortDVlanTagRowStatus = 0;
  }
  
  ZERO_VALID(agentSwitchPortDVlanTagEntryData.valid);
  
  agentSwitchPortDVlanTagEntryData.agentSwitchPortDVlanTagInterfaceIfIndex = 
          agentSwitchPortDVlanTagInterfaceIfIndex;
  SET_VALID(I_agentSwitchPortDVlanTagInterfaceIfIndex, agentSwitchPortDVlanTagEntryData.valid);
  
  agentSwitchPortDVlanTagEntryData.agentSwitchPortDVlanTagTPid = 
          agentSwitchPortDVlanTagTPID;
  SET_VALID(I_agentSwitchPortDVlanTagTPid, agentSwitchPortDVlanTagEntryData.valid);

  if(searchType == EXACT)
  {
    if(snmpAgentSwitchPortDvlanTagEntryGet(USMDB_UNIT_CURRENT,
                  (L7_uint32)agentSwitchPortDVlanTagEntryData.agentSwitchPortDVlanTagInterfaceIfIndex,
                  (L7_uint32)agentSwitchPortDVlanTagEntryData.agentSwitchPortDVlanTagTPid) != L7_SUCCESS)
    {
      ZERO_VALID(agentSwitchPortDVlanTagEntryData.valid);    
      return (NULL);
    }
  }

  if(searchType != EXACT)
  {
    if(agentSwitchPortDVlanTagTPID != 0)
    {
      agentSwitchPortDVlanTagEntryData.agentSwitchPortDVlanTagTPid = 
                  agentSwitchPortDVlanTagEntryData.agentSwitchPortDVlanTagTPid - 1;
    }

    if(snmpAgentSwitchPortDvlanTagNextEntryGet(USMDB_UNIT_CURRENT,
                            agentSwitchPortDVlanTagEntryData.agentSwitchPortDVlanTagInterfaceIfIndex,
                            &agentSwitchPortDVlanTagEntryData.agentSwitchPortDVlanTagInterfaceIfIndex,
                            agentSwitchPortDVlanTagEntryData.agentSwitchPortDVlanTagTPid,
                            &agentSwitchPortDVlanTagEntryData.agentSwitchPortDVlanTagTPid) != L7_SUCCESS)
    {
      ZERO_VALID(agentSwitchPortDVlanTagEntryData.valid);    
      return (NULL);
    }
  }

   /*
    * if ( nominator != -1 ) condition is added to all the case statements
    * for storing all the values to support the undo functionality.
    */

   switch (nominator)
   {
   case -1:
   case I_agentSwitchPortDVlanTagTPid:
   case I_agentSwitchPortDVlanTagInterfaceIfIndex:
     if (nominator != -1) break;
     /* else pass through */


   case I_agentSwitchPortDVlanTagRowStatus:
    if(snmpAgentSwitchPortDvlanTagEntryGet(USMDB_UNIT_CURRENT,
                  (L7_uint32)agentSwitchPortDVlanTagEntryData.agentSwitchPortDVlanTagInterfaceIfIndex,
                  (L7_uint32)agentSwitchPortDVlanTagEntryData.agentSwitchPortDVlanTagTPid) == L7_SUCCESS)
    {
       agentSwitchPortDVlanTagEntryData.agentSwitchPortDVlanTagRowStatus = 
        D_agentSwitchPortDVlanTagRowStatus_active;;         
       SET_VALID(I_agentSwitchPortDVlanTagRowStatus, agentSwitchPortDVlanTagEntryData.valid);
    }

    if (nominator != -1) break;
    /* else pass through */

   case I_agentSwitchPortDVlanTagMode:
     if (snmpAgentPortDVlanTagModeGet(USMDB_UNIT_CURRENT,
                  (L7_uint32)agentSwitchPortDVlanTagEntryData.agentSwitchPortDVlanTagInterfaceIfIndex,
                  (L7_uint32 *)&agentSwitchPortDVlanTagEntryData.agentSwitchPortDVlanTagMode) == L7_SUCCESS)
     {
       SET_VALID(I_agentSwitchPortDVlanTagMode, agentSwitchPortDVlanTagEntryData.valid);
     }
     if (nominator != -1) break;
     /* else pass through */

#ifdef I_agentSwitchPortDVlanTagCustomerId
  case I_agentSwitchPortDVlanTagCustomerId:
    if (usmDbDvlantagIntfCustIdGet(USMDB_UNIT_CURRENT, 
                  (L7_uint32)agentSwitchPortDVlanTagEntryData.agentSwitchPortDVlanTagInterfaceIfIndex,
                  (L7_uint32 *) &agentSwitchPortDVlanTagEntryData.agentSwitchPortDVlanTagCustomerId) == L7_SUCCESS)
    {                  
      SET_VALID(I_agentSwitchPortDVlanTagCustomerId, agentSwitchPortDVlanTagEntryData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */
#endif

   default:
     /* unknown nominator */
     return(NULL);
     break;
   }

   if (nominator >= 0 && !VALID(nominator, agentSwitchPortDVlanTagEntryData.valid))
     return(NULL);

   return(&agentSwitchPortDVlanTagEntryData);
}

#ifdef SETS
int
k_agentSwitchPortDVlanTagEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchPortDVlanTagEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchPortDVlanTagEntry_set_defaults(doList_t *dp)
{
    agentSwitchPortDVlanTagEntry_t *data = (agentSwitchPortDVlanTagEntry_t *) (dp->data);

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentSwitchPortDVlanTagEntry_set(agentSwitchPortDVlanTagEntry_t *data,
                               ContextInfo *contextInfo, int function)
{

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DVLANTAG_COMPONENT_ID,
                               L7_DVLANTAG_ETHERTYPE_FEATURE_ID) == L7_FALSE)
  {
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentSwitchPortDVlanTagTPid, data->valid) &&
    data->agentSwitchPortDVlanTagRowStatus == D_agentSwitchPortDVlanTagRowStatus_destroy)
  {
    if (usmDbDvlantagIntfEthertypeSet(USMDB_UNIT_CURRENT,
                               (L7_uint32)data->agentSwitchPortDVlanTagInterfaceIfIndex,
                               (L7_uint32)data->agentSwitchPortDVlanTagTPid,
                               L7_FALSE) != L7_SUCCESS)
    {
      ZERO_VALID(data->valid);    
      return COMMIT_FAILED_ERROR;
    }
  }
  else if (VALID(I_agentSwitchPortDVlanTagTPid, data->valid) &&
    data->agentSwitchPortDVlanTagRowStatus == D_agentSwitchPortDVlanTagRowStatus_createAndGo)
  {
    if (usmDbDvlantagIntfEthertypeSet(USMDB_UNIT_CURRENT,
                               (L7_uint32)data->agentSwitchPortDVlanTagInterfaceIfIndex,
                               (L7_uint32)data->agentSwitchPortDVlanTagTPid,
                               L7_TRUE) != L7_SUCCESS)
    {
      ZERO_VALID(data->valid);    
      return COMMIT_FAILED_ERROR;
    }
  }

  if (VALID(I_agentSwitchPortDVlanTagMode, data->valid))
  {
    if(snmpAgentPortDVlanTagModeSet(USMDB_UNIT_CURRENT, data->agentSwitchPortDVlanTagInterfaceIfIndex,
                                    data->agentSwitchPortDVlanTagMode) != L7_SUCCESS)
    {
      ZERO_VALID(data->valid);
      return COMMIT_FAILED_ERROR;
    }
  }

#ifdef I_agentSwitchPortDVlanTagCustomerId
  if (VALID(I_agentSwitchPortDVlanTagCustomerId, data->valid))
  {
    if(usmDbDvlantagIntfCustIdSet(USMDB_UNIT_CURRENT, data->agentSwitchPortDVlanTagInterfaceIfIndex,
                                  data->agentSwitchPortDVlanTagCustomerId) != L7_SUCCESS)
    {
      ZERO_VALID(data->valid);
      return COMMIT_FAILED_ERROR;
    }
  }
#endif
  return NO_ERROR;
}

#ifdef SR_agentSwitchPortDVlanTagGroup_UNDO
/* add #define SR_agentSwitchDVlanTagGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchDVlanTagGroup family.
 */
int
agentSwitchPortDVlanTagEntry_undo(doList_t *doHead, doList_t *doCur,
                              ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchPortDVlanTagGroup_UNDO */

#endif /* SETS */

#endif /* I_agentSwitchPortDVlanTagTPid */


agentSwitchDVlanTagGroup_t *
k_agentSwitchDVlanTagGroup_get(int serialNum, ContextInfo *contextInfo,
                               int nominator)
{
   static agentSwitchDVlanTagGroup_t agentSwitchDVlanTagGroupData;

   if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DVLANTAG_COMPONENT_ID,
                                L7_DVLANTAG_ETHERTYPE_FEATURE_ID) == L7_FALSE)
     return(NULL);

   ZERO_VALID(agentSwitchDVlanTagGroupData.valid);

   /*
    * if ( nominator != -1 ) condition is added to all the case statements
    * for storing all the values to support the undo functionality.
    */

   switch (nominator)
   {
   case -1:
     if (nominator != -1) break;
     /* else pass through */
#ifdef I_agentSwitchDVlanTagEthertype
   case I_agentSwitchDVlanTagEthertype:
     if (snmpAgentSwitchDvlanTagEthertypeGet(USMDB_UNIT_CURRENT,
                  agentSwitchDVlanTagGroupData.agentSwitchDVlanTagEthertype) == L7_SUCCESS)
       SET_VALID(I_agentSwitchDVlanTagEthertype, agentSwitchDVlanTagGroupData.valid);
     break;
#endif

   default:
     /* unknown nominator */
     return(NULL);
     break;
   }

   if (nominator >= 0 && !VALID(nominator, agentSwitchDVlanTagGroupData.valid))
     return(NULL);

   return(&agentSwitchDVlanTagGroupData);
}

#ifdef SETS
int
k_agentSwitchDVlanTagGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentSwitchDVlanTagGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchDVlanTagGroup_set(agentSwitchDVlanTagGroup_t *data,
                               ContextInfo *contextInfo, int function)
{
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DVLANTAG_COMPONENT_ID,
                               L7_DVLANTAG_ETHERTYPE_FEATURE_ID) == L7_FALSE)
  {
    return COMMIT_FAILED_ERROR;
  }

#ifdef I_agentSwitchDVlanTagEthertype
  if (VALID(I_agentSwitchDVlanTagEthertype, data->valid))
  {
    if (usmDbDvlantagEthertypeSet(USMDB_UNIT_CURRENT,
                               data->agentSwitchDVlanTagEthertype, L7_FALSE, L7_FALSE) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
  }
#endif

  return NO_ERROR;
}

#ifdef SR_agentSwitchDVlanTagGroup_UNDO
/* add #define SR_agentSwitchDVlanTagGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchDVlanTagGroup family.
 */
int
agentSwitchDVlanTagGroup_undo(doList_t *doHead, doList_t *doCur,
                              ContextInfo *contextInfo)
{
  agentSwitchDVlanTagGroup_t *data = (agentSwitchDVlanTagGroup_t *) doCur->data;
  agentSwitchDVlanTagGroup_t *undodata = (agentSwitchDVlanTagGroup_t *) doCur->undodata;
  agentSwitchDVlanTagGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid, data->valid, sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentSwitchDVlanTagGroup_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchDVlanTagGroup_UNDO */

#endif /* SETS */

