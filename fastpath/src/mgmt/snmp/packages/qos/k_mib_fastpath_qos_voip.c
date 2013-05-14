/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename k_mib_fastpath_qos_voip.c
*
* @purpose  Provide interface to VOIP Private MIB
*                  
* @component SNMP
*
* @comments 
*
* @create 11/23/2007
*
* @author Amitabha Sen
* @end
*
**********************************************************************/
#include "k_private_base.h"
#include "k_mib_fastpath_qos_voip_api.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"

agentAutoVoIPEntry_t *
k_agentAutoVoIPEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_INT32 agentAutoVoIPIntfIndex)
{
  static agentAutoVoIPEntry_t agentAutoVoIPEntryData;
  L7_uint32 intIfNum = 0;

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_VOIP_COMPONENT_ID) != L7_TRUE)
  {
    return(NULL);
  }

  ZERO_VALID(agentAutoVoIPEntryData.valid);
  agentAutoVoIPEntryData.agentAutoVoIPIntfIndex = agentAutoVoIPIntfIndex;
  SET_VALID(I_agentAutoVoIPIntfIndex, agentAutoVoIPEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpAgentAutoVoIPEntryGet(USMDB_UNIT_CURRENT,
                                     agentAutoVoIPEntryData.agentAutoVoIPIntfIndex,
                                     &intIfNum) != L7_SUCCESS) :
      ((snmpAgentAutoVoIPEntryGet(USMDB_UNIT_CURRENT,
                                      agentAutoVoIPEntryData.agentAutoVoIPIntfIndex,
                                      &intIfNum) != L7_SUCCESS) &&
       (snmpAgentAutoVoIPEntryNextGet(USMDB_UNIT_CURRENT,
                                      &agentAutoVoIPEntryData.agentAutoVoIPIntfIndex,
                                      &intIfNum) != L7_SUCCESS)))
  {
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_agentAutoVoIPIntfIndex:
    if (nominator != -1) break;


  case I_agentAutoVoIPMode:
    if (snmpAgentAutoVoIPModeGet(USMDB_UNIT_CURRENT, intIfNum, 
                                 &agentAutoVoIPEntryData.agentAutoVoIPMode) == L7_SUCCESS)
      SET_VALID(I_agentAutoVoIPMode, agentAutoVoIPEntryData.valid);
    break;

#ifdef I_agentAutoVoIPMinBandwidth
    case I_agentAutoVoIPMinBandwidth:
    if (snmpAgentAutoVoIPMinBandwidthGet(USMDB_UNIT_CURRENT, intIfNum,
                                         &agentAutoVoIPEntryData.agentAutoVoIPMinBandwidth) == L7_SUCCESS)
      SET_VALID(I_agentAutoVoIPMinBandwidth, agentAutoVoIPEntryData.valid);
    break;
#endif

  case I_agentAutoVoIPCosQueue:
    if (snmpAgentAutoVoIPCosQueueGet(USMDB_UNIT_CURRENT, intIfNum,
                                            &agentAutoVoIPEntryData.agentAutoVoIPCosQueue) == L7_SUCCESS)
      SET_VALID(I_agentAutoVoIPCosQueue, agentAutoVoIPEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentAutoVoIPEntryData.valid))
    return(NULL);

  return(&agentAutoVoIPEntryData);
}

#ifdef SETS
int
k_agentAutoVoIPEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentAutoVoIPEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentAutoVoIPEntry_set_defaults(doList_t *dp)
{
  agentAutoVoIPEntry_t *data = (agentAutoVoIPEntry_t *) (dp->data);


  ZERO_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentAutoVoIPEntry_set(agentAutoVoIPEntry_t *data,
                         ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */

  L7_uint32 intIfNum = 0;

  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if ((usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_VOIP_COMPONENT_ID) != L7_TRUE) ||
      (snmpAgentAutoVoIPEntryGet(USMDB_UNIT_CURRENT,data->agentAutoVoIPIntfIndex,
                                 &intIfNum) != L7_SUCCESS))
  {
    return(COMMIT_FAILED_ERROR);
  }

  if (VALID(I_agentAutoVoIPMode, data->valid))
  {
    if (snmpAgentAutoVoIPModeSet(USMDB_UNIT_CURRENT, intIfNum, data->agentAutoVoIPMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentAutoVoIPMode, tempValid);
    }
  }

#ifdef I_agentAutoVoIPMinBandwidth
  if (VALID(I_agentAutoVoIPMinBandwidth, data->valid))
  {
    if (snmpAgentAutoVoIPMinBandwidthSet(USMDB_UNIT_CURRENT, intIfNum, data->agentAutoVoIPMinBandwidth) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentAutoVoIPMinBandwidth, tempValid);
    }
  }
#endif
  return NO_ERROR;
}

#ifdef SR_agentAutoVoIPEntry_UNDO
/* add #define SR_aagentAutoVoIPEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentCosMapIpPrecEntry family.
 */
int
agentAutoVoIPEntry_undo(doList_t *doHead, doList_t *doCur,
                        ContextInfo *contextInfo)
{
  agentAutoVoIPEntry_t *data = (agentAutoVoIPEntry_t *) doCur->data;
  agentAutoVoIPEntry_t *undodata = (agentAutoVoIPEntry_t *) doCur->undodata;
  agentAutoVoIPEntry_t *setdata = NULL;
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
  if (k_agentAutoVoIPEntry_set(setdata, contextInfo, function) == NO_ERROR)
    return(NO_ERROR);

  return(UNDO_FAILED_ERROR);
}
#endif /* SR_agentAutoVoIPEntry_UNDO */

#endif /* SETS */


