/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename src\mgmt\snmp\packages\base\k_mib_fastpath_llpf.c
*
* @purpose  Provide interface to LLPF MIB
*
* @component SNMP
*
* @comments
*
* @create 10/28/2009
*
* @author vijayanand k(kvijayan)
* @end
*
**********************************************************************/
#include <k_private_base.h>
#include "k_mib_fastpath_llpf_api.h"
#include "usmdb_util_api.h"
#include "usmdb_common.h"

agentSwitchLlpfPortConfigEntry_t *
k_agentSwitchLlpfPortConfigEntry_get(int serialNum, ContextInfo *contextInfo,
                                     int nominator,
                                     int searchType,
                                     SR_INT32 ifIndex,
                                     SR_UINT32 agentSwitchLlpfProtocolType)
{
  static agentSwitchLlpfPortConfigEntry_t llpfPortConfigData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_uint32      intIfNum;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
  }

  ZERO_VALID(llpfPortConfigData.valid);
  llpfPortConfigData.agentSwitchLlpfProtocolType = agentSwitchLlpfProtocolType;
  llpfPortConfigData.ifIndex = ifIndex;
  SET_VALID(I_agentSwitchLlpfPortConfigEntryIndex_ifIndex, llpfPortConfigData.valid);
  SET_VALID(I_agentSwitchLlpfProtocolType,llpfPortConfigData.valid);
  
   if((searchType == EXACT) ?
      (snmpLlpfIntfGet(llpfPortConfigData.ifIndex,
                      llpfPortConfigData.agentSwitchLlpfProtocolType) != L7_SUCCESS) :
      ((snmpLlpfIntfGet(llpfPortConfigData.ifIndex,
                        llpfPortConfigData.agentSwitchLlpfProtocolType) != L7_SUCCESS) &&
       (snmpLlpfIntfNextGet(llpfPortConfigData.ifIndex, &llpfPortConfigData.ifIndex,
                            llpfPortConfigData.agentSwitchLlpfProtocolType,
                            &llpfPortConfigData.agentSwitchLlpfProtocolType) != L7_SUCCESS))) 
 {
   ZERO_VALID(llpfPortConfigData.valid);
    return(NULL);
 }

  if( usmDbIntIfNumFromExtIfNum(llpfPortConfigData.ifIndex, &intIfNum) != L7_SUCCESS)
  { 
    ZERO_VALID(llpfPortConfigData.valid);
    return(NULL);
  } 

  switch (nominator)
  {
   case -1:
    /* fallthrough all cases except default */
   case I_agentSwitchLlpfProtocolType:
   if(nominator != -1) break;

    case I_agentSwitchLlpfPortBlockMode:
      if(snmpLlpfIntfProtocolModeGet(intIfNum,llpfPortConfigData.agentSwitchLlpfProtocolType,
                                     &llpfPortConfigData.agentSwitchLlpfPortBlockMode) == L7_SUCCESS)
         SET_VALID(I_agentSwitchLlpfPortBlockMode,llpfPortConfigData.valid);   
       break;

    default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, llpfPortConfigData.valid))
	  return(NULL);

  return(&llpfPortConfigData);

}

#ifdef SETS
int
k_agentSwitchLlpfPortConfigEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                      doList_t *dp, ContextInfo *contextInfo)
{
   return NO_ERROR;
}

int
k_agentSwitchLlpfPortConfigEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                       doList_t *doHead, doList_t *dp)
{
    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentSwitchLlpfPortConfigEntry_set_defaults(doList_t *dp)
{
    agentSwitchLlpfPortConfigEntry_t *data = (agentSwitchLlpfPortConfigEntry_t *) (dp->data);

    data->agentSwitchLlpfPortBlockMode = 2;

    ZERO_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentSwitchLlpfPortConfigEntry_set(agentSwitchLlpfPortConfigEntry_t *llpfPortConfigData,
                                     ContextInfo *contextInfo, int function)
{

  /*
   * Defining temporary variable for storing the valid bits for the case
   * when the set request is only paritally successful
   */
  L7_char8  tempValid[sizeof(llpfPortConfigData->valid)];
  L7_uint32 intIfNum;

  memset(tempValid, 0, sizeof(tempValid));

  if (usmDbIntIfNumFromExtIfNum(llpfPortConfigData->ifIndex, &intIfNum) != L7_SUCCESS)
  {
    ZERO_VALID(llpfPortConfigData->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (usmDbPhysicalIntIfNumberCheck(USMDB_UNIT_CURRENT, intIfNum) != L7_SUCCESS)
  {
    ZERO_VALID(llpfPortConfigData->valid);
    return COMMIT_FAILED_ERROR;
  }

  if (VALID(I_agentSwitchLlpfPortBlockMode, llpfPortConfigData->valid))
  {
   if(snmpLlpfIntfProtocolModeSet(intIfNum,llpfPortConfigData->agentSwitchLlpfProtocolType,
                                     llpfPortConfigData->agentSwitchLlpfPortBlockMode) != L7_SUCCESS)
    {
      memcpy(llpfPortConfigData->valid, tempValid, sizeof(llpfPortConfigData->valid));
      return COMMIT_FAILED_ERROR;
    }
    SET_VALID(I_agentSwitchLlpfPortBlockMode, tempValid);
  }
  return(NO_ERROR);
}

#ifdef SR_agentSwitchLlpfPortConfigEntry_UNDO
/* add #define SR_agentSwitchLlpfPortConfigEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchLlpfPortConfigEntry family.
 */
int
agentSwitchLlpfPortConfigEntry_undo(doList_t *doHead, doList_t *doCur,
                                    ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentSwitchLlpfPortConfigEntry_UNDO */

#endif /* SETS */


