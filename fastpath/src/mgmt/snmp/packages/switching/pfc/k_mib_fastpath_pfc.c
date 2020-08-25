/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2009
*
**********************************************************************
*
* @filename k_mib_fastpath_pfc.c
*
* @purpose  Provide interface to PFC Private MIB
*
* @component SNMP
*
* @comments
*
* @create 6/02/2009
*
* @author Visakha Erina
* @end
*
**********************************************************************/
#include "k_private_base.h"
#include "k_mib_fastpath_pfc_api.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"
#include "policy_exports.h"
#include "commdefs.h"
#include "pfc_exports.h"
#include "usmdb_pfc.h"


agentPfcEntry_t *
k_agentPfcEntry_get(int serialNum, ContextInfo *contextInfo,
                    int nominator,
                    int searchType,
                    SR_INT32 agentPfcIntfIndex)
{
  static agentPfcEntry_t agentPfcEntryData;
  L7_uint32 intIfNum = 0;

  if(usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT,L7_PFC_COMPONENT_ID,L7_PFC_PORT_PRIORITY_FLOW_CONTROL_FEATURE_ID) != L7_TRUE)
  {
    return(NULL);
  }

  ZERO_VALID(agentPfcEntryData.valid);
  agentPfcEntryData.agentPfcIntfIndex = agentPfcIntfIndex;
  SET_VALID(I_agentPfcIntfIndex, agentPfcEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpAgentPfcEntryGet(agentPfcEntryData.agentPfcIntfIndex,
                                     &intIfNum) != L7_SUCCESS) :
      ((snmpAgentPfcEntryGet(agentPfcEntryData.agentPfcIntfIndex,
                                     &intIfNum) != L7_SUCCESS) &&
       (snmpAgentPfcEntryNextGet(&agentPfcEntryData.agentPfcIntfIndex,
                                     &intIfNum) != L7_SUCCESS)))
  {
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_agentPfcIntfIndex:
    if (nominator != -1) break;

  case I_agentPfcIntfAdminMode:
    if (snmpAgentPfcIntfAdminModeGet(intIfNum, &agentPfcEntryData.agentPfcIntfAdminMode) == L7_SUCCESS)
      SET_VALID(I_agentPfcIntfAdminMode, agentPfcEntryData.valid);
    if (nominator != -1) break;

  case I_agentPfcIntfPfcStatus:
    if (snmpAgentPfcIntfPfcStatusGet(intIfNum, &agentPfcEntryData.agentPfcIntfPfcStatus) == L7_SUCCESS)
      SET_VALID(I_agentPfcIntfPfcStatus, agentPfcEntryData.valid);
    if (nominator != -1) break;

  case I_agentPfcTotalIntfPfcFramesRx:
    if (usmDbIfPfcRxStatGet(intIfNum, &agentPfcEntryData.agentPfcTotalIntfPfcFramesRx) == L7_SUCCESS)
      SET_VALID(I_agentPfcTotalIntfPfcFramesRx, agentPfcEntryData.valid);
    if (nominator != -1) break;

  case I_agentPfcTotalIntfPfcFramesTx:
    if (usmDbIfPfcTxStatGet(intIfNum, &agentPfcEntryData.agentPfcTotalIntfPfcFramesTx) == L7_SUCCESS)
      SET_VALID(I_agentPfcTotalIntfPfcFramesTx, agentPfcEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentPfcEntryData.valid))
    return(NULL);

  return(&agentPfcEntryData);
}

#ifdef SETS
int
k_agentPfcEntry_test(ObjectInfo *object, ObjectSyntax *value,
                     doList_t *dp, ContextInfo *contextInfo)
{
#ifdef NOT_YET
    agentPfcEntry_t *data = (agentPfcEntry_t *) dp->data;

    /*
     * Check the validity of MIB object values here.
     * Perform checks on "object values in isolation".
     */

    if (object->nominator == I_agentPfcIntfAdminMode) {
        /*
         * Examine value->sl_value.  In case of error,
         * return from the function here with an error code.
         */
        SET_VALID(I_agentPfcIntfAdminMode, data->mod);
    }
#endif /* NOT_YET */
    return NO_ERROR;
}

int
k_agentPfcEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                      doList_t *doHead, doList_t *dp)
{
#ifdef NOT_YET
    agentPfcEntry_t *data = (agentPfcEntry_t *) dp->data;

    /*
     * Check the validity of MIB object values here.
     * The proposed new values are in data.
     * Perform checks on "object value relationships".
     */

    if (VALID(I_agentPfcIntfAdminMode, data->valid)) {
        /*
         * Examine data->agentPfcIntfAdminMode in relation to other
         * MIB objects.  If a relationship is not invalid, set
         * dp->state to SR_UNKNOWN and return with NO_ERROR.
         */
    }
#endif /* NOT_YET */
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentPfcEntry_set_defaults(doList_t *dp)
{
    agentPfcEntry_t *data = (agentPfcEntry_t *) (dp->data);

    data->agentPfcIntfAdminMode = 0;
    data->agentPfcIntfPfcStatus = 0;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentPfcEntry_set(agentPfcEntry_t *data,
                    ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */

  L7_uint32 intIfNum = 0;

  L7_char8 tempValid[sizeof(data->valid)];
  memset(tempValid, 0, sizeof(tempValid));


  if(usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT,L7_PFC_COMPONENT_ID,L7_PFC_PORT_PRIORITY_FLOW_CONTROL_FEATURE_ID) != L7_TRUE)
  {
    return(COMMIT_FAILED_ERROR);
  }

  if (snmpAgentPfcEntryGet(data->agentPfcIntfIndex,&intIfNum) != L7_SUCCESS)
  {
    return(COMMIT_FAILED_ERROR);
  }

  if (VALID(I_agentPfcIntfAdminMode, data->valid))
  {
    if (snmpAgentPfcIntfAdminModeSet(intIfNum, data->agentPfcIntfAdminMode) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentPfcIntfAdminMode, tempValid);
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentPfcEntry_UNDO
/* add #define SR_agentPfcEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentPfcEntry family.
 */
int
agentPfcEntry_undo(doList_t *doHead, doList_t *doCur,
                   ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentPfcEntry_UNDO */

#endif /* SETS */

agentPfcActionEntry_t *
k_agentPfcActionEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator,
                          int searchType,
                          SR_INT32 agentPfcIntfIndex,
                          SR_UINT32 agentPfcPriority)
{
  static agentPfcActionEntry_t agentPfcActionEntryData;
  L7_uint32 intfIndex = 0;


  if(usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT,L7_PFC_COMPONENT_ID,L7_PFC_PORT_PRIORITY_FLOW_CONTROL_FEATURE_ID) != L7_TRUE)
  {
    return(NULL);
  }

  ZERO_VALID(agentPfcActionEntryData.valid);
  agentPfcActionEntryData.agentPfcIntfIndex = agentPfcIntfIndex;
  SET_VALID(I_agentPfcActionEntryIndex_agentPfcIntfIndex, agentPfcActionEntryData.valid);
  agentPfcActionEntryData.agentPfcPriority = agentPfcPriority;
  SET_VALID(I_agentPfcPriority, agentPfcActionEntryData.valid);

  if ((searchType == EXACT) ?
      (snmpAgentPfcActionEntryGet(agentPfcActionEntryData.agentPfcIntfIndex,
                                  agentPfcActionEntryData.agentPfcPriority,
                                  &intfIndex) != L7_SUCCESS) :
      ((snmpAgentPfcActionEntryGet(agentPfcActionEntryData.agentPfcIntfIndex,
                                   agentPfcActionEntryData.agentPfcPriority,
                                   &intfIndex) != L7_SUCCESS) &&
       (snmpAgentPfcActionEntryNextGet(&agentPfcActionEntryData.agentPfcIntfIndex,
                                       &agentPfcActionEntryData.agentPfcPriority,
                                       &intfIndex) != L7_SUCCESS)))
  {
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_agentPfcActionEntryIndex_agentPfcIntfIndex:
  case I_agentPfcPriority:
    if (nominator != -1) break;

  case I_agentPfcAction:
    if (snmpAgentPfcActionGet(intfIndex, agentPfcActionEntryData.agentPfcPriority, &agentPfcActionEntryData.agentPfcAction) == L7_SUCCESS)
      SET_VALID(I_agentPfcAction, agentPfcActionEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentPfcActionEntryData.valid))
    return(NULL);

  return(&agentPfcActionEntryData);
}

#ifdef SETS
int
k_agentPfcActionEntry_test(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *dp, ContextInfo *contextInfo)
{
#ifdef NOT_YET
    agentPfcActionEntry_t *data = (agentPfcActionEntry_t *) dp->data;

    /*
     * Check the validity of MIB object values here.
     * Perform checks on "object values in isolation".
     */

    if (object->nominator == I_agentPfcAction) {
        /*
         * Examine value->sl_value.  In case of error,
         * return from the function here with an error code.
         */
        SET_VALID(I_agentPfcAction, data->mod);
    }
#endif /* NOT_YET */
    return NO_ERROR;
}

int
k_agentPfcActionEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                            doList_t *doHead, doList_t *dp)
{
#ifdef NOT_YET
    agentPfcActionEntry_t *data = (agentPfcActionEntry_t *) dp->data;

    /*
     * Check the validity of MIB object values here.
     * The proposed new values are in data.
     * Perform checks on "object value relationships".
     */

    if (VALID(I_agentPfcAction, data->valid)) {
        /*
         * Examine data->agentPfcAction in relation to other
         * MIB objects.  If a relationship is not invalid, set
         * dp->state to SR_UNKNOWN and return with NO_ERROR.
         */
    }
#endif /* NOT_YET */
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentPfcActionEntry_set_defaults(doList_t *dp)
{
    agentPfcActionEntry_t *data = (agentPfcActionEntry_t *) (dp->data);

    data->agentPfcAction = 0;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentPfcActionEntry_set(agentPfcActionEntry_t *data,
                          ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_uint32 intfIndex = 0;

  L7_char8 tempValid[sizeof(data->valid)];
  memset(tempValid, 0, sizeof(tempValid));

  if(usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT,L7_PFC_COMPONENT_ID,L7_PFC_PORT_PRIORITY_FLOW_CONTROL_FEATURE_ID) != L7_TRUE)
  {
    return(COMMIT_FAILED_ERROR);
  }

  if (snmpAgentPfcActionEntryGet(data->agentPfcIntfIndex, data->agentPfcPriority, &intfIndex) != L7_SUCCESS)
  {
    return(COMMIT_FAILED_ERROR);
  }

  if (VALID(I_agentPfcAction, data->valid))
  {
    if (snmpAgentPfcActionSet(intfIndex, data->agentPfcPriority, data->agentPfcAction) != L7_SUCCESS)
    {
      memcpy(data->valid, tempValid, sizeof(data->valid));
      return(COMMIT_FAILED_ERROR);
    }
    else
    {
      SET_VALID(I_agentPfcAction, tempValid);
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentPfcActionEntry_UNDO
/* add #define SR_agentPfcActionEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentPfcActionEntry family.
 */
int
agentPfcActionEntry_undo(doList_t *doHead, doList_t *doCur,
                         ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentPfcActionEntry_UNDO */

#endif /* SETS */

agentPfcIntfStatsPerPriorityEntry_t *
k_agentPfcIntfStatsPerPriorityEntry_get(int serialNum, ContextInfo *contextInfo,
                                        int nominator,
                                        int searchType,
                                        SR_INT32 agentPfcIntfIndex,
                                        SR_UINT32 agentPfcPriority)
{
  static agentPfcIntfStatsPerPriorityEntry_t agentPfcIntfStatsPerPriorityEntryData;
  L7_uint32 intfIndex = 0;   

  if(usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT,L7_PFC_COMPONENT_ID,L7_PFC_PORT_PRIORITY_FLOW_CONTROL_FEATURE_ID) != L7_TRUE)
  {
    return(NULL);
  }

  ZERO_VALID(agentPfcIntfStatsPerPriorityEntryData.valid);
  agentPfcIntfStatsPerPriorityEntryData.agentPfcIntfIndex = agentPfcIntfIndex;
  SET_VALID(I_agentPfcIntfStatsPerPriorityEntryIndex_agentPfcIntfIndex, agentPfcIntfStatsPerPriorityEntryData.valid); 
  agentPfcIntfStatsPerPriorityEntryData.agentPfcPriority = agentPfcPriority;
  SET_VALID(I_agentPfcIntfStatsPerPriorityEntryIndex_agentPfcPriority, agentPfcIntfStatsPerPriorityEntryData.valid);


  if ((searchType == EXACT) ?
      (snmpAgentPfcIntfStatsPerPriorityEntryGet(agentPfcIntfStatsPerPriorityEntryData.agentPfcIntfIndex,
                                                agentPfcIntfStatsPerPriorityEntryData.agentPfcPriority,
                                                &intfIndex) != L7_SUCCESS) :
      ((snmpAgentPfcIntfStatsPerPriorityEntryGet(agentPfcIntfStatsPerPriorityEntryData.agentPfcIntfIndex,
                                                 agentPfcIntfStatsPerPriorityEntryData.agentPfcPriority,  
                                                 &intfIndex) != L7_SUCCESS) &&
       (snmpAgentPfcIntfStatsPerPriorityEntryNextGet(&agentPfcIntfStatsPerPriorityEntryData.agentPfcIntfIndex,
                                                     &agentPfcIntfStatsPerPriorityEntryData.agentPfcPriority,
                                                     &intfIndex) != L7_SUCCESS)))
  {
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_agentPfcIntfStatsPerPriorityEntryIndex_agentPfcIntfIndex:
  case I_agentPfcIntfStatsPerPriorityEntryIndex_agentPfcPriority:
    if (nominator != -1) break;

  case I_agentPfcIntfPfcPriorityFramesRx:
    if (usmDbIfPfcRxPriorityStatGet(intfIndex, agentPfcIntfStatsPerPriorityEntryData.agentPfcPriority, 
                                    &agentPfcIntfStatsPerPriorityEntryData.agentPfcIntfPfcPriorityFramesRx) == L7_SUCCESS)
      SET_VALID(I_agentPfcIntfPfcPriorityFramesRx, agentPfcIntfStatsPerPriorityEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, agentPfcIntfStatsPerPriorityEntryData.valid))
    return(NULL);

  return(&agentPfcIntfStatsPerPriorityEntryData);

}

