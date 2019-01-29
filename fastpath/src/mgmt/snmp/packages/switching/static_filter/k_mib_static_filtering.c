/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_static_filtering.c
*
* @purpose    System-Specific code to support Static Filtering component
*
* @component  SNMP
*
* @comments
*
* @create     4/6/2008
*
* @author     akulkarni
* @end
*
**********************************************************************/
#include "k_private_base.h"
#include "l7_common.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"
#include "usmdb_filter_api.h"
#include "k_mib_fastpath_staticmacfilter_api.h"


agentSwitchStaticMacFilteringEntry_t *
k_agentSwitchStaticMacFilteringEntry_get(int serialNum, ContextInfo *contextInfo,
                                         int nominator,
                                         int searchType,
                                         SR_INT32 agentSwitchStaticMacFilteringVlanId,
                                         OctetString * agentSwitchStaticMacFilteringAddress)
{
  static agentSwitchStaticMacFilteringEntry_t agentSwitchStaticMacFilteringEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_char8 snmp_mask_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_filter_len;
  L7_RC_t rc;

  rc=L7_FAILURE;

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_FILTER_COMPONENT_ID ) == L7_FALSE )
      return(NULL);

  usmDbFilterSizeGet(USMDB_UNIT_CURRENT, &snmp_filter_len);

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;

    agentSwitchStaticMacFilteringEntryData.agentSwitchStaticMacFilteringAddress = MakeOctetString(NULL, 0);
    agentSwitchStaticMacFilteringEntryData.agentSwitchStaticMacFilteringSourcePortMask = MakeOctetString(NULL, 0);
    agentSwitchStaticMacFilteringEntryData.agentSwitchStaticMacFilteringDestPortMask = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(agentSwitchStaticMacFilteringEntryData.valid);
  agentSwitchStaticMacFilteringEntryData.agentSwitchStaticMacFilteringVlanId = agentSwitchStaticMacFilteringVlanId;
  SET_VALID(I_agentSwitchStaticMacFilteringVlanId, agentSwitchStaticMacFilteringEntryData.valid);

  bzero(snmp_buffer, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer, agentSwitchStaticMacFilteringAddress->octet_ptr, agentSwitchStaticMacFilteringAddress->length);

  if ( (searchType == EXACT) ?
       (usmDbFilterIsConfigured(USMDB_UNIT_CURRENT, snmp_buffer, agentSwitchStaticMacFilteringEntryData.agentSwitchStaticMacFilteringVlanId) != L7_SUCCESS) :
       ( (usmDbFilterIsConfigured(USMDB_UNIT_CURRENT, snmp_buffer, agentSwitchStaticMacFilteringEntryData.agentSwitchStaticMacFilteringVlanId) != L7_SUCCESS) &&
        (usmDbFilterNextGet(USMDB_UNIT_CURRENT, snmp_buffer, agentSwitchStaticMacFilteringEntryData.agentSwitchStaticMacFilteringVlanId,
                             snmp_buffer, &agentSwitchStaticMacFilteringEntryData.agentSwitchStaticMacFilteringVlanId) != L7_SUCCESS) ) )
  {
    ZERO_VALID(agentSwitchStaticMacFilteringEntryData.valid);
    return(NULL);
  }
  else
  {
    if (SafeMakeOctetString(&agentSwitchStaticMacFilteringEntryData.agentSwitchStaticMacFilteringAddress, snmp_buffer, L7_MAC_ADDR_LEN) == L7_TRUE)
    {
      SET_VALID(I_agentSwitchStaticMacFilteringAddress, agentSwitchStaticMacFilteringEntryData.valid);
    }
    else
    {
      ZERO_VALID(agentSwitchStaticMacFilteringEntryData.valid);
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
  case I_agentSwitchStaticMacFilteringVlanId:
  case I_agentSwitchStaticMacFilteringAddress:
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchStaticMacFilteringSourcePortMask:
    bzero(snmp_mask_buffer, SNMP_BUFFER_LEN);

    if (usmDbFilterSrcIntfMaskGet(USMDB_UNIT_CURRENT, snmp_buffer, agentSwitchStaticMacFilteringEntryData.agentSwitchStaticMacFilteringVlanId, snmp_mask_buffer) == L7_SUCCESS &&
        SafeMakeOctetString(&agentSwitchStaticMacFilteringEntryData.agentSwitchStaticMacFilteringSourcePortMask, snmp_mask_buffer, snmp_filter_len) == L7_TRUE)
      SET_VALID(I_agentSwitchStaticMacFilteringSourcePortMask, agentSwitchStaticMacFilteringEntryData.valid);
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchStaticMacFilteringDestPortMask:
    bzero(snmp_mask_buffer, SNMP_BUFFER_LEN);

    rc= usmDbFilterDstIntfMaskGet(USMDB_UNIT_CURRENT, snmp_buffer, agentSwitchStaticMacFilteringEntryData.agentSwitchStaticMacFilteringVlanId, snmp_mask_buffer);
    if (rc == L7_SUCCESS &&
        SafeMakeOctetString(&agentSwitchStaticMacFilteringEntryData.agentSwitchStaticMacFilteringDestPortMask, snmp_mask_buffer, snmp_filter_len) == L7_TRUE)
    {
      SET_VALID(I_agentSwitchStaticMacFilteringDestPortMask, agentSwitchStaticMacFilteringEntryData.valid);
    }
    else if (rc == L7_NOT_SUPPORTED)
    {
      if(SafeMakeOctetString(&agentSwitchStaticMacFilteringEntryData.agentSwitchStaticMacFilteringDestPortMask, snmp_mask_buffer, snmp_filter_len) == L7_TRUE)
        SET_VALID(I_agentSwitchStaticMacFilteringDestPortMask, agentSwitchStaticMacFilteringEntryData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchStaticMacFilteringStatus:
    /* if entry shows up, it's active */
    agentSwitchStaticMacFilteringEntryData.agentSwitchStaticMacFilteringStatus = D_agentSwitchStaticMacFilteringStatus_active;
    SET_VALID(I_agentSwitchStaticMacFilteringStatus, agentSwitchStaticMacFilteringEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentSwitchStaticMacFilteringEntryData.valid))
    return(NULL);

  return(&agentSwitchStaticMacFilteringEntryData);
}

#ifdef SETS
int
k_agentSwitchStaticMacFilteringEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                          doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentSwitchStaticMacFilteringEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                           doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentSwitchStaticMacFilteringEntry_set_defaults(doList_t *dp)
{
  agentSwitchStaticMacFilteringEntry_t *data = (agentSwitchStaticMacFilteringEntry_t *) (dp->data);

  if ((data->agentSwitchStaticMacFilteringSourcePortMask = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->agentSwitchStaticMacFilteringDestPortMask = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentSwitchStaticMacFilteringEntry_set(agentSwitchStaticMacFilteringEntry_t *data,
                                         ContextInfo *contextInfo, int function)
{
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_char8 snmp_mask_buffer[SNMP_BUFFER_LEN];
  L7_BOOL isNewRowCreated = L7_FALSE;

  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_FILTER_COMPONENT_ID ) == L7_FALSE )
      return COMMIT_FAILED_ERROR;

  if (data->agentSwitchStaticMacFilteringAddress == L7_NULLPTR)
    return COMMIT_FAILED_ERROR;

  bzero(snmp_buffer, SNMP_BUFFER_LEN);
  memcpy(snmp_buffer, data->agentSwitchStaticMacFilteringAddress->octet_ptr, data->agentSwitchStaticMacFilteringAddress->length);

  if (usmDbFilterIsConfigured(USMDB_UNIT_CURRENT, snmp_buffer, data->agentSwitchStaticMacFilteringVlanId) != L7_SUCCESS)
  {
    /* create a new entry */
    if (VALID(I_agentSwitchStaticMacFilteringStatus, data->valid))
    {
      if((data->agentSwitchStaticMacFilteringStatus == D_agentSwitchStaticMacFilteringStatus_createAndGo) &&
        (snmpAgentSwitchStaticMacFilteringEntryCreate(USMDB_UNIT_CURRENT, snmp_buffer, data->agentSwitchStaticMacFilteringVlanId) != L7_SUCCESS))
      {
        /* don't set the status */
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return COMMIT_FAILED_ERROR;
      }
      else
      {
        SET_VALID(I_agentSwitchStaticMacFilteringStatus, tempValid);
        isNewRowCreated = L7_TRUE;
      }
    }
  }

  if (VALID(I_agentSwitchStaticMacFilteringSourcePortMask, data->valid))
  {
    bzero(snmp_mask_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_mask_buffer, data->agentSwitchStaticMacFilteringSourcePortMask->octet_ptr, data->agentSwitchStaticMacFilteringSourcePortMask->length);

    if (usmDbFilterSrcIntfMaskSet(USMDB_UNIT_CURRENT, snmp_buffer, data->agentSwitchStaticMacFilteringVlanId, snmp_mask_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentSwitchStaticMacFilteringSourcePortMask, tempValid);
    }
  }

  if (VALID(I_agentSwitchStaticMacFilteringDestPortMask, data->valid))
  {
    bzero(snmp_mask_buffer, SNMP_BUFFER_LEN);
    memcpy(snmp_mask_buffer, data->agentSwitchStaticMacFilteringDestPortMask->octet_ptr, data->agentSwitchStaticMacFilteringDestPortMask->length);

    if (usmDbFilterDstIntfMaskSet(USMDB_UNIT_CURRENT, snmp_buffer, data->agentSwitchStaticMacFilteringVlanId, snmp_mask_buffer) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
    else
    {
      SET_VALID(I_agentSwitchStaticMacFilteringDestPortMask, tempValid);
    }
  }

  if (VALID(I_agentSwitchStaticMacFilteringStatus, data->valid) && (isNewRowCreated != L7_TRUE))
  {
    /* ignore if set to active */
    /* if set to destroy and destroy fails, return failure */
    if ((data->agentSwitchStaticMacFilteringStatus != D_agentSwitchStaticMacFilteringStatus_active) &&
        (data->agentSwitchStaticMacFilteringStatus != D_agentSwitchStaticMacFilteringStatus_destroy ||
         (usmDbFilterRemove(USMDB_UNIT_CURRENT, snmp_buffer, data->agentSwitchStaticMacFilteringVlanId) != L7_SUCCESS)))
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}


  #ifdef SR_agentSwitchStaticMacFilteringEntry_UNDO
/* add #define SR_agentSwitchStaticMacFilteringEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchStaticMacFilteringEntry family.
 */
int
agentSwitchStaticMacFilteringEntry_undo(doList_t *doHead, doList_t *doCur,
                                        ContextInfo *contextInfo)
{
  agentSwitchStaticMacFilteringEntry_t *data = (agentSwitchStaticMacFilteringEntry_t *) doCur->data;
  agentSwitchStaticMacFilteringEntry_t *undodata = (agentSwitchStaticMacFilteringEntry_t *) doCur->undodata;
  agentSwitchStaticMacFilteringEntry_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*Copy valid bits from data to undodata */
  if ( undodata != NULL && data != NULL )
    memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* we are either trying to undo an add, a delete, or a modify */
  if ( undodata == NULL )
  {
    /* undoing an add, so delete */
    data->agentSwitchStaticMacFilteringStatus = D_agentSwitchStaticMacFilteringStatus_destroy;
    setdata = data;
    function = SR_DELETE;
  }
  else
  {
    /* undoing a delete or modify, replace the original data */
    if(undodata->agentSwitchStaticMacFilteringStatus == D_agentSwitchStaticMacFilteringStatus_notReady
    || undodata->agentSwitchStaticMacFilteringStatus == D_agentSwitchStaticMacFilteringStatus_notInService)
    {
      undodata->agentSwitchStaticMacFilteringStatus = D_agentSwitchStaticMacFilteringStatus_createAndWait;
    }
    else
    {
      if(undodata->agentSwitchStaticMacFilteringStatus == D_agentSwitchStaticMacFilteringStatus_active)
      {
        undodata->agentSwitchStaticMacFilteringStatus = D_agentSwitchStaticMacFilteringStatus_createAndGo;
      }
    }

    if (undodata->agentSwitchStaticMacFilteringStatus == D_agentSwitchStaticMacFilteringStatus_destroy)
    {
      /* Setting all valid bits for restoring the previous values */
      memset(undodata->valid, 0xff, sizeof(undodata->valid));
    }

    setdata = undodata;
    function = SR_ADD_MODIFY;
  }

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentSwitchStaticMacFilteringEntry_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentSwitchStaticMacFilteringEntry_UNDO */

#endif /* SETS */

