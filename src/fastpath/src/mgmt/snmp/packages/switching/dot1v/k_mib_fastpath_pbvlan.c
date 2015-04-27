/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_fastpath_pbvlan.c
*
* @purpose    System-Specific code to support Protocol based VLAN
*
* @component  SNMP
*
* @comments
*
* @create     3/6/2007
*
* @author     akulkarni
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"
#include "k_private_base.h"
#include "k_mib_fastpath_pbvlan_api.h"
#include "usmdb_pbvlan_api.h"
#include "usmdb_mib_diffserv_private_api.h"

L7_BOOL creationErrorProtocolConfigGroup = L7_FALSE;


agentProtocolConfigGroup_t *
k_agentProtocolConfigGroup_get(int serialNum, ContextInfo *contextInfo,
                               int nominator)
{
   /*static agentProtocolConfigGroup_t agentProtocolConfigGroupData;
   static L7_BOOL firstTime = L7_TRUE;

   if (firstTime == L7_TRUE)
   {
     firstTime = L7_FALSE;

   }

   ZERO_VALID(agentProtocolConfigGroupData.valid);
   SET_VALID(I_agentProtocolGroupCreate, agentProtocolConfigGroupData.valid);

   return(&agentProtocolConfigGroupData);*/
   return (NULL);
}

#ifdef SETS
int
k_agentProtocolConfigGroup_test(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentProtocolConfigGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentProtocolConfigGroup_set(agentProtocolConfigGroup_t *data,
                               ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (VALID(I_agentProtocolGroupCreate, data->valid))
  {
    /* Since this group is obsoluted */
    return COMMIT_FAILED_ERROR;
    /*

    if (snmpAgentProtocolGroupCreateSet(USMDB_UNIT_CURRENT, data->agentProtocolGroupCreate) != L7_SUCCESS)
    {
      creationErrorProtocolConfigGroup = L7_TRUE;
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentProtocolGroupCreate, tempValid);
    }*/
  }

  return NO_ERROR;
}

  #ifdef SR_agentProtocolConfigGroup_UNDO
/* add #define SR_agentProtocolConfigGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentProtocolConfigGroup family.
 */
int
agentProtocolConfigGroup_undo(doList_t *doHead, doList_t *doCur,
                              ContextInfo *contextInfo)
{
    agentProtocolConfigGroup_t *data = (agentProtocolConfigGroup_t *) doCur->data;

  L7_int32 function, temp_index;
  L7_int32 rc = L7_FAILURE;
  L7_int32 agentProtocolGroupTempId = 0;
  L7_char8 temp_protocolGroupName[SNMP_BUFFER_LEN];
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  /* Define a table entry for destroying the created row */
  agentProtocolGroupEntry_t agentProtocolGroupEntryData;
  agentProtocolGroupEntry_t *entrysetdata = &agentProtocolGroupEntryData;


  /* To avoid deletion when the same name is set more than once */
  if ( creationErrorProtocolConfigGroup == L7_TRUE )
  {
    creationErrorProtocolConfigGroup = L7_FALSE;
    return NO_ERROR;
  }

  /* Initialize this table entry */
  agentProtocolGroupEntryData.agentProtocolGroupName = MakeOctetString(NULL, 0);
  ZERO_VALID(agentProtocolGroupEntryData.valid);

  if ( data == NULL )
  {
    free(agentProtocolGroupEntryData.agentProtocolGroupName);
    return UNDO_FAILED_ERROR;
  }

  bzero(snmp_buffer, SNMP_BUFFER_LEN);
  strncpy(snmp_buffer, data->agentProtocolGroupCreate->octet_ptr, data->agentProtocolGroupCreate->length);
  snmp_buffer[data->agentProtocolGroupCreate->length] = '\0';/* Making sure that we get a NULL Terminated string */

  bzero(temp_protocolGroupName, SNMP_BUFFER_LEN);

  temp_index = 0;
  while (usmDbPbVlanGroupGetNext(USMDB_UNIT_CURRENT, temp_index, &temp_index) == L7_SUCCESS)
  {
    if (usmDbPbVlanGroupNameGet(USMDB_UNIT_CURRENT, temp_index, temp_protocolGroupName) == L7_SUCCESS)
    {
      if (strcmp(temp_protocolGroupName, snmp_buffer) == L7_SUCCESS)
      {
        agentProtocolGroupTempId = temp_index;
        rc = L7_SUCCESS;
        break;
      }
    }
  }

  if (rc != L7_SUCCESS)
  {
    free(agentProtocolGroupEntryData.agentProtocolGroupName);
    return UNDO_FAILED_ERROR;
  }

  /* we are trying to undo an add to the Table
   * undoing an add, so delete
  */
  agentProtocolGroupEntryData.agentProtocolGroupStatus = D_agentProtocolGroupStatus_destroy;
  agentProtocolGroupEntryData.agentProtocolGroupId = agentProtocolGroupTempId;
  SET_VALID(I_agentProtocolGroupStatus, agentProtocolGroupEntryData.valid);
  function = SR_DELETE;

  /* use the set method for the undo */
  if ((entrysetdata != NULL) && (k_agentProtocolGroupEntry_set(entrysetdata, contextInfo, function) == NO_ERROR))
  {
    free(agentProtocolGroupEntryData.agentProtocolGroupName);
    return NO_ERROR;
  }

  free(agentProtocolGroupEntryData.agentProtocolGroupName);
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentProtocolConfigGroup_UNDO */

#endif /* SETS */

agentProtocolGroupEntry_t *
k_agentProtocolGroupEntry_get(int serialNum, ContextInfo *contextInfo,
                              int nominator,
                              int searchType,
                              SR_INT32 agentProtocolGroupId)
{
  static agentProtocolGroupEntry_t agentProtocolGroupEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  
  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_PBVLAN_COMPONENT_ID) == L7_FALSE)
  {
      return (NULL);
  }

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentProtocolGroupEntryData.agentProtocolGroupName = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentProtocolGroupEntryData.valid);
  agentProtocolGroupEntryData.agentProtocolGroupId = agentProtocolGroupId;
  SET_VALID(I_agentProtocolGroupId, agentProtocolGroupEntryData.valid);

  if ( (searchType == EXACT) ?
       usmDbPbVlanGroupGetExact(USMDB_UNIT_CURRENT, agentProtocolGroupEntryData.agentProtocolGroupId) != L7_SUCCESS :
       (usmDbPbVlanGroupGetExact(USMDB_UNIT_CURRENT, agentProtocolGroupEntryData.agentProtocolGroupId) != L7_SUCCESS &&
        usmDbPbVlanGroupGetNext(USMDB_UNIT_CURRENT, agentProtocolGroupEntryData.agentProtocolGroupId,
                                &agentProtocolGroupEntryData.agentProtocolGroupId) != L7_SUCCESS) )
  {
    ZERO_VALID(agentProtocolGroupEntryData.valid);
    return(NULL);
  }

  /*
  * if ( nominator != -1 ) condition is added to all the case statements
  * for storing all the values to support the undo functionality.
  */

  switch (nominator)
  {
  case -1:
  case I_agentProtocolGroupId:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentProtocolGroupName:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if (usmDbPbVlanGroupNameGet(USMDB_UNIT_CURRENT, agentProtocolGroupEntryData.agentProtocolGroupId,
                                snmp_buffer) == L7_SUCCESS &&
        SafeMakeOctetStringFromText(&agentProtocolGroupEntryData.agentProtocolGroupName, snmp_buffer) == L7_TRUE)
      SET_VALID(I_agentProtocolGroupName, agentProtocolGroupEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentProtocolGroupVlanId:
    if (usmDbPbVlanGroupVIDGet(USMDB_UNIT_CURRENT, agentProtocolGroupEntryData.agentProtocolGroupId,
                               &agentProtocolGroupEntryData.agentProtocolGroupVlanId) == L7_SUCCESS)
      SET_VALID(I_agentProtocolGroupVlanId, agentProtocolGroupEntryData.valid);
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentProtocolGroupStatus:
    agentProtocolGroupEntryData.agentProtocolGroupStatus = D_agentProtocolGroupStatus_active;
    SET_VALID(I_agentProtocolGroupStatus, agentProtocolGroupEntryData.valid);
    break;

  default:
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentProtocolGroupEntryData.valid))
    return(NULL);

  return(&agentProtocolGroupEntryData);
}

#ifdef SETS
int
k_agentProtocolGroupEntry_test(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentProtocolGroupEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *doHead, doList_t *dp)
{
   if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_PBVLAN_COMPONENT_ID) == L7_FALSE)
        return NO_SUCH_NAME_ERROR;

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentProtocolGroupEntry_set_defaults(doList_t *dp)
{
    agentProtocolGroupEntry_t *data = (agentProtocolGroupEntry_t *) (dp->data);

    if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_PBVLAN_COMPONENT_ID) == L7_FALSE)
        return NO_SUCH_NAME_ERROR;

    if ((data->agentProtocolGroupName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentProtocolGroupEntry_set(agentProtocolGroupEntry_t *data,
                              ContextInfo *contextInfo, int function)
{
   /*
    * Defining temporary variable for storing the valid bits for the case when the
    * set request is only paritally successful
   */
   L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
   L7_char8 tempValid[sizeof(data->valid)];
   bzero(tempValid, sizeof(tempValid));
   
   if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_PBVLAN_COMPONENT_ID) == L7_FALSE)
        return NO_SUCH_NAME_ERROR;

   /*previously we check wheather the group already exist or not before we set any field to that group
   now we dont need to check,Since we are creating the group in the set function by adding create and go*/
  /* if (usmDbPbVlanGroupGetExact(USMDB_UNIT_CURRENT, data->agentProtocolGroupId) != L7_SUCCESS)
     return COMMIT_FAILED_ERROR; */

   if (VALID(I_agentProtocolGroupVlanId, data->valid))
   {
     if( snmpAgentProtocolGroupVlanIdSet(USMDB_UNIT_CURRENT, data->agentProtocolGroupId,
                                       data->agentProtocolGroupVlanId) != L7_SUCCESS)
     {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
     }
     else
     {
       SET_VALID(I_agentProtocolGroupVlanId, tempValid);
     }
   }

   if (VALID(I_agentProtocolGroupName, data->valid))
   {
     bzero(snmp_buffer, SNMP_BUFFER_LEN);
     memcpy(snmp_buffer, data->agentProtocolGroupName->octet_ptr, data->agentProtocolGroupName->length);

     if(usmDbStringAlphaNumericCheck(snmp_buffer)!= L7_SUCCESS)
     {
       return(COMMIT_FAILED_ERROR);
     }
     else if (usmDbPbVlanGroupNameSet(USMDB_UNIT_CURRENT, data->agentProtocolGroupId, snmp_buffer) != L7_SUCCESS)
     {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return(COMMIT_FAILED_ERROR);
     }
     else
     {
       SET_VALID(I_agentProtocolGroupName, tempValid);
     }
   }
   
   if (VALID(I_agentProtocolGroupStatus, data->valid))
   {
     if( snmpAgentProtocolGroupStatusSet(USMDB_UNIT_CURRENT, data->agentProtocolGroupId,
                                       data->agentProtocolGroupStatus) != L7_SUCCESS)
     {
       memcpy(data->valid, tempValid, sizeof(data->valid));
       return COMMIT_FAILED_ERROR;
     }
   }

   return NO_ERROR;
}

#ifdef SR_agentProtocolGroupEntry_UNDO
/* add #define SR_agentProtocolGroupEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentProtocolGroupEntry family.
 */
int
agentProtocolGroupEntry_undo(doList_t *doHead, doList_t *doCur,
                             ContextInfo *contextInfo)
{
  agentProtocolGroupEntry_t *data = (agentProtocolGroupEntry_t *) doCur->data;
  agentProtocolGroupEntry_t *undodata = (agentProtocolGroupEntry_t *) doCur->undodata;
  agentProtocolGroupEntry_t *entrysetdata = NULL;

  L7_int32 function = SR_UNKNOWN;
  L7_int32 temp_index;
  L7_int32 rc = L7_FAILURE;
  L7_int32 agentProtocolGroupTempId = 0; /* Temporary Index */
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_char8 temp_protocolGroupName[SNMP_BUFFER_LEN];

  /* Define a group instance so as to create the deleted row */
  agentProtocolConfigGroup_t agentProtocolConfigGroupTempdata;
  agentProtocolConfigGroup_t *setdata = &agentProtocolConfigGroupTempdata;


  /* Initialize this group instance */
  agentProtocolConfigGroupTempdata.agentProtocolGroupCreate = CloneOctetString(undodata->agentProtocolGroupName);
  ZERO_VALID(agentProtocolConfigGroupTempdata.valid);

  /*Copy valid bits from data to undodata */
  if( undodata != NULL && data != NULL )
    memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /*
   * we are either trying to undo a delete, or a modify
   * undoing a delete or modify, replace the original data
   */
  if(data->agentProtocolGroupStatus ==  D_agentProtocolGroupStatus_destroy)
  {
    /* check that the entry was actually deleted */
    if (usmDbPbVlanGroupGetExact(USMDB_UNIT_CURRENT,
                                 undodata->agentProtocolGroupId) == L7_SUCCESS)
      return NO_ERROR;

    /* Use the set method for the creation of the row with default values */
    SET_VALID(I_agentProtocolGroupCreate, agentProtocolConfigGroupTempdata.valid);
    function = SR_ADD_MODIFY;
    if ((setdata != NULL) && (k_agentProtocolConfigGroup_set(setdata, contextInfo, function) == NO_ERROR))
    {
      bzero(snmp_buffer, SNMP_BUFFER_LEN);
      strncpy(snmp_buffer, undodata->agentProtocolGroupName->octet_ptr, undodata->agentProtocolGroupName->length);

      bzero(temp_protocolGroupName, SNMP_BUFFER_LEN);

      temp_index = 0;
      while (usmDbPbVlanGroupGetNext(USMDB_UNIT_CURRENT, temp_index, &temp_index) == L7_SUCCESS)
      {
        if (usmDbPbVlanGroupNameGet(USMDB_UNIT_CURRENT, temp_index, temp_protocolGroupName) == L7_SUCCESS)
        {
          if (strcmp(temp_protocolGroupName, snmp_buffer) == L7_SUCCESS)
          {
            agentProtocolGroupTempId = temp_index;
            rc = L7_SUCCESS;
            break;
          }
        }
      }

      if (rc == L7_SUCCESS)
      {
        undodata->agentProtocolGroupId = agentProtocolGroupTempId;

        /* set all valid for the setting the previous values to the created row */
        memset(undodata->valid, 0xff, sizeof(undodata->valid));

        /* Status should not be set again */
        CLR_VALID(I_agentProtocolGroupStatus, undodata->valid);

        entrysetdata = undodata;

        /* use the set method for the undo */
        if ((entrysetdata != NULL) && (k_agentProtocolGroupEntry_set(entrysetdata,
                                                                     contextInfo, function) == NO_ERROR))
        {
          free(agentProtocolConfigGroupTempdata.agentProtocolGroupCreate);
          return NO_ERROR;
        }
      }
    }
  }
  else
  {
    /* we are trying to undo a modify to the Table */
    if(undodata->agentProtocolGroupStatus == D_agentProtocolGroupStatus_notReady
    || undodata->agentProtocolGroupStatus == D_agentProtocolGroupStatus_notInService)
    {
      undodata->agentProtocolGroupStatus = D_agentProtocolGroupStatus_createAndWait;
    }
    else
    {
      if(undodata->agentProtocolGroupStatus == D_agentProtocolGroupStatus_active)
      {
        undodata->agentProtocolGroupStatus = D_agentProtocolGroupStatus_createAndGo;
      }
      entrysetdata = undodata;
      function = SR_ADD_MODIFY;
    }

    /* use the set method for the undo */
    if ((entrysetdata != NULL) && (k_agentProtocolGroupEntry_set(entrysetdata, contextInfo, function) == NO_ERROR))
    {
      free(agentProtocolConfigGroupTempdata.agentProtocolGroupCreate);
      return NO_ERROR;
    }
  }

  free(agentProtocolConfigGroupTempdata.agentProtocolGroupCreate);
  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentProtocolGroupEntry_UNDO */

#endif /* SETS */

agentProtocolGroupPortEntry_t *
k_agentProtocolGroupPortEntry_get(int serialNum, ContextInfo *contextInfo,
                                  int nominator,
                                  int searchType,
                                  SR_INT32 agentProtocolGroupId,
                                  SR_INT32 agentProtocolGroupPortIfIndex)
{
  static agentProtocolGroupPortEntry_t agentProtocolGroupPortEntryData;

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_PBVLAN_COMPONENT_ID) == L7_FALSE)
      return (NULL);

  ZERO_VALID(agentProtocolGroupPortEntryData.valid);
  agentProtocolGroupPortEntryData.agentProtocolGroupId = agentProtocolGroupId;
  SET_VALID(I_agentProtocolGroupId, agentProtocolGroupPortEntryData.valid);
  agentProtocolGroupPortEntryData.agentProtocolGroupPortIfIndex = agentProtocolGroupPortIfIndex;
  SET_VALID(I_agentProtocolGroupPortIfIndex, agentProtocolGroupPortEntryData.valid);

  if ( (searchType == EXACT) ?
       (snmpAgentProtocolGroupPortEntryGetExact(USMDB_UNIT_CURRENT,
                                                agentProtocolGroupPortEntryData.agentProtocolGroupId,
                                                agentProtocolGroupPortEntryData.agentProtocolGroupPortIfIndex) != L7_SUCCESS) :
       ((snmpAgentProtocolGroupPortEntryGetExact(USMDB_UNIT_CURRENT,
                                                 agentProtocolGroupPortEntryData.agentProtocolGroupId,
                                                 agentProtocolGroupPortEntryData.agentProtocolGroupPortIfIndex) != L7_SUCCESS) &&
        (snmpAgentProtocolGroupPortEntryGetNext(USMDB_UNIT_CURRENT,
                                                &agentProtocolGroupPortEntryData.agentProtocolGroupId,
                                                &agentProtocolGroupPortEntryData.agentProtocolGroupPortIfIndex) != L7_SUCCESS)) )
  {
    ZERO_VALID(agentProtocolGroupPortEntryData.valid);
    return(NULL);
  }

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_agentProtocolGroupPortEntryIndex_agentProtocolGroupId:
  case I_agentProtocolGroupPortIfIndex:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentProtocolGroupPortStatus:
    agentProtocolGroupPortEntryData.agentProtocolGroupPortStatus = D_agentProtocolGroupPortStatus_active;
    SET_VALID(I_agentProtocolGroupPortStatus, agentProtocolGroupPortEntryData.valid);
    break;

  default:
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, agentProtocolGroupPortEntryData.valid))
      return(NULL);

  return(&agentProtocolGroupPortEntryData);
}

#ifdef SETS
int
k_agentProtocolGroupPortEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                   doList_t *dp, ContextInfo *contextInfo)
{
    if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_PBVLAN_COMPONENT_ID) == L7_FALSE)
        return NO_SUCH_NAME_ERROR;

    return NO_ERROR;
}

int
k_agentProtocolGroupPortEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                    doList_t *doHead, doList_t *dp)
{
    if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_PBVLAN_COMPONENT_ID) == L7_FALSE)
        return NO_SUCH_NAME_ERROR;

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentProtocolGroupPortEntry_set_defaults(doList_t *dp)
{
    agentProtocolGroupPortEntry_t *data = (agentProtocolGroupPortEntry_t *) (dp->data);

    if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_PBVLAN_COMPONENT_ID) == L7_FALSE)
        return NO_SUCH_NAME_ERROR;

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentProtocolGroupPortEntry_set(agentProtocolGroupPortEntry_t *data,
                                  ContextInfo *contextInfo, int function)
{
  L7_uint32 intIfNum;
  L7_BOOL isNewRowCreated = L7_FALSE;

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_PBVLAN_COMPONENT_ID) == L7_FALSE)
      return NO_SUCH_NAME_ERROR;

  if (usmDbIntIfNumFromExtIfNum(data->agentProtocolGroupPortIfIndex, &intIfNum) != L7_SUCCESS)
    return COMMIT_FAILED_ERROR;

  if (snmpAgentProtocolGroupPortEntryGetExact(USMDB_UNIT_CURRENT, data->agentProtocolGroupId,
                                              data->agentProtocolGroupPortIfIndex) != L7_SUCCESS)
  {
    if (VALID(I_agentProtocolGroupPortStatus, data->valid))
    {
      if((data->agentProtocolGroupPortStatus == D_agentProtocolGroupPortStatus_createAndGo ||
         data->agentProtocolGroupPortStatus == D_agentProtocolGroupPortStatus_createAndWait) &&
        usmDbPbVlanGroupPortAdd(USMDB_UNIT_CURRENT, data->agentProtocolGroupId, intIfNum) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_agentProtocolGroupPortStatus, tempValid);
        isNewRowCreated = L7_TRUE;
      }
    }
  }

  if (VALID(I_agentProtocolGroupPortStatus, data->valid) && (isNewRowCreated != L7_TRUE))
  {
     /* If input status is destroy, it should delete one agentProtocolGroupPortEntry. Also, input status can only be 'active' or 'destroy'*/
    if((data->agentProtocolGroupPortStatus == D_agentProtocolGroupPortStatus_destroy &&
        usmDbPbVlanGroupPortDelete(USMDB_UNIT_CURRENT, data->agentProtocolGroupId, intIfNum) != L7_SUCCESS) ||
        (data->agentProtocolGroupPortStatus != D_agentProtocolGroupPortStatus_active
                && data->agentProtocolGroupPortStatus != D_agentProtocolGroupPortStatus_destroy))
     {
          memcpy(data->valid, tempValid, sizeof(data->valid));
          return COMMIT_FAILED_ERROR;
     }
  }

  return NO_ERROR;
}

#ifdef SR_agentProtocolGroupPortEntry_UNDO
/* add #define SR_agentProtocolGroupPortEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentProtocolGroupPortEntry family.
 */
int
agentProtocolGroupPortEntry_undo(doList_t *doHead, doList_t *doCur,
                                 ContextInfo *contextInfo)
{
  agentProtocolGroupPortEntry_t *data = (agentProtocolGroupPortEntry_t *) doCur->data;
  agentProtocolGroupPortEntry_t *undodata = (agentProtocolGroupPortEntry_t *) doCur->undodata;
  agentProtocolGroupPortEntry_t *entrysetdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*Copy valid bits from data to undodata */
  if( undodata != NULL && data != NULL )
    memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
    entrysetdata = undodata;
  }
  else
  {
    /* undoing a delete or modify, replace the original data */
    if(undodata->agentProtocolGroupPortStatus == D_agentProtocolGroupPortStatus_notReady
    || undodata->agentProtocolGroupPortStatus == D_agentProtocolGroupPortStatus_notInService)
    {
      undodata->agentProtocolGroupPortStatus = D_agentProtocolGroupPortStatus_createAndWait;
    }
    else
    {
      if(undodata->agentProtocolGroupPortStatus == D_agentProtocolGroupPortStatus_active)
      {
        undodata->agentProtocolGroupPortStatus = D_agentProtocolGroupPortStatus_createAndGo;
      }
    }

    entrysetdata = undodata;
    function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((entrysetdata != NULL) && (k_agentProtocolGroupPortEntry_set(entrysetdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
#endif /* SR_agentProtocolGroupPortEntry_UNDO */

#endif /* SETS */

#ifdef I_agentProtocolGroupProtocolID
agentProtocolGroupProtocolEntry_t *
k_agentProtocolGroupProtocolEntry_get(int serialNum, ContextInfo *contextInfo,
                                      int nominator,
                                      int searchType,
                                      SR_INT32 agentProtocolGroupId,
                                      SR_INT32 agentProtocolGroupProtocolID)
{
  static agentProtocolGroupProtocolEntry_t agentProtocolGroupProtocolEntryData;

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_PBVLAN_COMPONENT_ID) == L7_FALSE)
     return (NULL);
    
  ZERO_VALID(agentProtocolGroupProtocolEntryData.valid);
  agentProtocolGroupProtocolEntryData.agentProtocolGroupId = agentProtocolGroupId;
  SET_VALID(I_agentProtocolGroupId, agentProtocolGroupProtocolEntryData.valid);
  agentProtocolGroupProtocolEntryData.agentProtocolGroupProtocolID = agentProtocolGroupProtocolID;
  SET_VALID(I_agentProtocolGroupProtocolID, agentProtocolGroupProtocolEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbPbVlanGroupProtocolGetExact(USMDB_UNIT_CURRENT,
                                                agentProtocolGroupProtocolEntryData.agentProtocolGroupId,
                                                agentProtocolGroupProtocolEntryData.agentProtocolGroupProtocolID) != L7_SUCCESS) :
       ((usmDbPbVlanGroupProtocolGetExact(USMDB_UNIT_CURRENT,
                                                 agentProtocolGroupProtocolEntryData.agentProtocolGroupId,
                                                 agentProtocolGroupProtocolEntryData.agentProtocolGroupProtocolID) != L7_SUCCESS) &&
        (snmpAgentProtocolGroupProtocolEntryGetNext(USMDB_UNIT_CURRENT,
                                                &agentProtocolGroupProtocolEntryData.agentProtocolGroupId,
                                                &agentProtocolGroupProtocolEntryData.agentProtocolGroupProtocolID) != L7_SUCCESS)) )
  {
    ZERO_VALID(agentProtocolGroupProtocolEntryData.valid);
    return(NULL);
  }

  
   /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
  case I_agentProtocolGroupProtocolEntryIndex_agentProtocolGroupId:
  case I_agentProtocolGroupProtocolID:
    if ( nominator != -1 ) break;
    /* else pass through */

  case I_agentProtocolGroupProtocolStatus:
    agentProtocolGroupProtocolEntryData.agentProtocolGroupProtocolStatus = D_agentProtocolGroupProtocolStatus_active;
    SET_VALID(I_agentProtocolGroupProtocolStatus, agentProtocolGroupProtocolEntryData.valid);
    break;

  default:
    return(NULL);
  }

  if (nominator >= 0 && !VALID(nominator, agentProtocolGroupProtocolEntryData.valid))
      return(NULL);

  return(&agentProtocolGroupProtocolEntryData);

}


#ifdef SETS
int
k_agentProtocolGroupProtocolEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                       doList_t *dp, ContextInfo *contextInfo)
{ 
    if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_PBVLAN_COMPONENT_ID) == L7_FALSE)
       return NO_SUCH_NAME_ERROR;    
    return NO_ERROR;
}

int
k_agentProtocolGroupProtocolEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                        doList_t *doHead, doList_t *dp)
{
    if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_PBVLAN_COMPONENT_ID) == L7_FALSE)
        return NO_SUCH_NAME_ERROR;

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentProtocolGroupProtocolEntry_set_defaults(doList_t *dp)
{
    agentProtocolGroupProtocolEntry_t *data = (agentProtocolGroupProtocolEntry_t *) (dp->data);

    if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_PBVLAN_COMPONENT_ID) == L7_FALSE)
        return NO_SUCH_NAME_ERROR;

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentProtocolGroupProtocolEntry_set(agentProtocolGroupProtocolEntry_t *data,
                                      ContextInfo *contextInfo, int function)
{
  L7_BOOL isNewRowCreated = L7_FALSE;

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
   */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_PBVLAN_COMPONENT_ID) == L7_FALSE)
    return NO_SUCH_NAME_ERROR;

  if (usmDbPbVlanGroupProtocolGetExact(USMDB_UNIT_CURRENT, data->agentProtocolGroupId,
                                              data->agentProtocolGroupProtocolID) != L7_SUCCESS)
  {
    if (VALID(I_agentProtocolGroupProtocolStatus, data->valid))
    {
      if ((data->agentProtocolGroupProtocolID < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPE_MIN) ||
        (data->agentProtocolGroupProtocolID > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPE_MAX))
      {
        return BAD_VALUE_ERROR;
      }

      if ((data->agentProtocolGroupProtocolStatus == D_agentProtocolGroupProtocolStatus_createAndGo||
          data->agentProtocolGroupProtocolStatus == D_agentProtocolGroupProtocolStatus_createAndWait) &&
         usmDbPbVlanGroupProtocolAdd(USMDB_UNIT_CURRENT, data->agentProtocolGroupId, data->agentProtocolGroupProtocolID, 1) != L7_SUCCESS)
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_agentProtocolGroupProtocolStatus, tempValid);
        isNewRowCreated = L7_TRUE;
      }
    }
  }

  if (VALID(I_agentProtocolGroupProtocolStatus, data->valid) && (isNewRowCreated != L7_TRUE))
  {
    /* If input status is destroy, it should delete one agentProtocolGroupPortEntry. Also, input status can only be 'active' or 'destroy'*/
    if ((data->agentProtocolGroupProtocolStatus == D_agentProtocolGroupProtocolStatus_destroy &&
        usmDbPbVlanGroupProtocolDelete(USMDB_UNIT_CURRENT, data->agentProtocolGroupId, data->agentProtocolGroupProtocolID) != L7_SUCCESS) ||
       (data->agentProtocolGroupProtocolStatus != D_agentProtocolGroupProtocolStatus_active
        && data->agentProtocolGroupProtocolStatus != D_agentProtocolGroupProtocolStatus_destroy))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentProtocolGroupProtocolEntry_UNDO
/* add #define SR_agentProtocolGroupProtocolEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentProtocolGroupProtocolEntry family.
 */
int
agentProtocolGroupProtocolEntry_undo(doList_t *doHead, doList_t *doCur,
                                     ContextInfo *contextInfo)
{
   agentProtocolGroupProtocolEntry_t *data = (agentProtocolGroupPortEntry_t *) doCur->data;
   agentProtocolGroupProtocolEntry_t *undodata = (agentProtocolGroupPortEntry_t *) doCur->undodata;
   agentProtocolGroupProtocolEntry_t *entrysetdata = NULL;

   L7_int32 function = SR_UNKNOWN;

    /*Copy valid bits from data to undodata */
   if ( undodata != NULL && data != NULL )
     memcpy(undodata->valid,data->valid,sizeof(data->valid));

   /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
    /* undoing an add, so delete */
    undodata->agentProtocolGroupProtocolStatus = D_agentProtocolGroupProtocolStatus_destroy;
    entrysetdata = undodata;
    function = SR_DELETE;
  }
  else
  {
    /* undoing a delete or modify, replace the original data */
    if (undodata->agentProtocolGroupProtocolStatus == D_agentProtocolGroupProtocolStatus_notReady 
    || undodata->agentProtocolGroupProtocolStatus == D_agentProtocolGroupProtocolStatus_notInService)
    {
      undodata->agentProtocolGroupProtocolStatus = D_agentProtocolGroupProtocolStatus_createAndWait;
    }
    else
    {
      if (undodata->agentProtocolGroupProtocolStatus == D_agentProtocolGroupProtocolStatus_active)
      {
        undodata->agentProtocolGroupProtocolStatus = D_agentProtocolGroupProtocolStatus_createAndGo;
      }
    }

    entrysetdata = undodata;
    function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((entrysetdata != NULL) && (k_agentProtocolGroupProtocolEntry_set(entrysetdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

   
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentProtocolGroupProtocolEntry_UNDO */

#endif /* SETS */

#endif /* I_agentProtocolGroupProtocolID */
