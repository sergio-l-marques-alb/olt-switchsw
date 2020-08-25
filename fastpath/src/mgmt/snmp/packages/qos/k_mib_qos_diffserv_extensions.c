/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\mgmt\snmp\snmp_sr\snmpd\unix\k_mib_qos_diffserv_extensions.c
*
* @purpose  Provide interface to DiffServ Private MIB
*                  
* @component SNMP
*
* @comments 
*
* @create 07/12/2002
*
* @author cpverne
* @end
*
**********************************************************************/
#include <k_private_base.h>
#include "k_mib_qos_diffserv_extensions_api.h"
#include "cnfgr.h"
#include "usmdb_common.h"
#include "diffserv_exports.h"
#include "usmdb_mib_diffserv_api.h"
#include "usmdb_util_api.h"

agentDiffServClassifier_t *
k_agentDiffServClassifier_get(int serialNum, ContextInfo *contextInfo,
                              int nominator)
{
  static agentDiffServClassifier_t agentDiffServClassifierData;

  ZERO_VALID(agentDiffServClassifierData.valid);

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  if (usmDbDiffServAuxMFClfrIndexNext(USMDB_UNIT_CURRENT, 
		          &agentDiffServClassifierData.agentDiffServAuxMfClfrNextFree) == L7_SUCCESS)
    SET_VALID(I_agentDiffServAuxMfClfrNextFree, agentDiffServClassifierData.valid);
  
  if (nominator >= 0 && !VALID(nominator, agentDiffServClassifierData.valid))
    return(NULL);

  return(&agentDiffServClassifierData);
}

agentDiffServAuxMfClfrEntry_t *
k_agentDiffServAuxMfClfrEntry_get(int serialNum, ContextInfo *contextInfo,
                                  int nominator,
                                  int searchType,
                                  SR_UINT32 agentDiffServAuxMfClfrId)
{
  static agentDiffServAuxMfClfrEntry_t agentDiffServAuxMfClfrEntryData;
  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 snmp_buffer_len;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrTosMask = MakeOctetStringFromText(NULL);
    agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrDstMac = MakeOctetStringFromText(NULL);
    agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrDstMacMask = MakeOctetStringFromText(NULL);
    agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrSrcMac = MakeOctetStringFromText(NULL);
    agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrSrcMacMask = MakeOctetStringFromText(NULL);
  }

  ZERO_VALID(agentDiffServAuxMfClfrEntryData.valid);
  agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId = agentDiffServAuxMfClfrId;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  SET_VALID(I_agentDiffServAuxMfClfrId, agentDiffServAuxMfClfrEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbDiffServAuxMFClfrGet(USMDB_UNIT_CURRENT, agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId) != L7_SUCCESS) :
       ((usmDbDiffServAuxMFClfrGet(USMDB_UNIT_CURRENT, agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId) != L7_SUCCESS) &&
        (usmDbDiffServAuxMFClfrGetNext(USMDB_UNIT_CURRENT, agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                    &agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId) != L7_SUCCESS)))
  {
    ZERO_VALID(agentDiffServAuxMfClfrEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_agentDiffServAuxMfClfrId:
    break;

  case I_agentDiffServAuxMfClfrDstAddr:
    if (usmDbDiffServAuxMFClfrDstAddrGet(USMDB_UNIT_CURRENT,
                                         agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                         &agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrDstAddr) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAuxMfClfrDstAddr, agentDiffServAuxMfClfrEntryData.valid);
    break;

  case I_agentDiffServAuxMfClfrDstMask:
    if (usmDbDiffServAuxMFClfrDstMaskGet(USMDB_UNIT_CURRENT,
                                         agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                         &agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrDstMask) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAuxMfClfrDstMask, agentDiffServAuxMfClfrEntryData.valid);
    break;

  case I_agentDiffServAuxMfClfrSrcAddr:
    if (usmDbDiffServAuxMFClfrSrcAddrGet(USMDB_UNIT_CURRENT,
                                         agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                         &agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrSrcAddr) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAuxMfClfrSrcAddr, agentDiffServAuxMfClfrEntryData.valid);
    break;

  case I_agentDiffServAuxMfClfrSrcMask:
    if (usmDbDiffServAuxMFClfrSrcMaskGet(USMDB_UNIT_CURRENT,
                                         agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                         &agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrSrcMask) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAuxMfClfrSrcMask, agentDiffServAuxMfClfrEntryData.valid);
    break;

  case I_agentDiffServAuxMfClfrProtocol:
    if (usmDbDiffServAuxMFClfrProtocolGet(USMDB_UNIT_CURRENT,
                                          agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                          &agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrProtocol) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAuxMfClfrProtocol, agentDiffServAuxMfClfrEntryData.valid);
    break;

  case I_agentDiffServAuxMfClfrDstL4PortMin:
    if (usmDbDiffServAuxMFClfrDstL4PortMinGet(USMDB_UNIT_CURRENT,
                                              agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                              &agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrDstL4PortMin) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAuxMfClfrDstL4PortMin, agentDiffServAuxMfClfrEntryData.valid);
    break;

  case I_agentDiffServAuxMfClfrDstL4PortMax:
    if (usmDbDiffServAuxMFClfrDstL4PortMaxGet(USMDB_UNIT_CURRENT,
                                              agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                              &agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrDstL4PortMax) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAuxMfClfrDstL4PortMax, agentDiffServAuxMfClfrEntryData.valid);
    break;

  case I_agentDiffServAuxMfClfrSrcL4PortMin:
    if (usmDbDiffServAuxMFClfrSrcL4PortMinGet(USMDB_UNIT_CURRENT,
                                              agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                              &agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrSrcL4PortMin) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAuxMfClfrSrcL4PortMin, agentDiffServAuxMfClfrEntryData.valid);
    break;

  case I_agentDiffServAuxMfClfrSrcL4PortMax:
    if (usmDbDiffServAuxMFClfrSrcL4PortMaxGet(USMDB_UNIT_CURRENT,
                                              agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                              &agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrSrcL4PortMax) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAuxMfClfrSrcL4PortMax, agentDiffServAuxMfClfrEntryData.valid);
    break;

  case I_agentDiffServAuxMfClfrCos:
    if (usmDbDiffServAuxMFClfrCosGet(USMDB_UNIT_CURRENT,
                                     agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                     &agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrCos) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAuxMfClfrCos, agentDiffServAuxMfClfrEntryData.valid);
    break;
  
  case I_agentDiffServAuxMfClfrTos:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((usmDbDiffServAuxMFClfrTosGet(USMDB_UNIT_CURRENT,
                                      agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                      &snmp_buffer[0]) == L7_SUCCESS) &&
        (SafeMakeOctetString(&agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrTos, snmp_buffer, 1) == L7_TRUE))
      SET_VALID(I_agentDiffServAuxMfClfrTos, agentDiffServAuxMfClfrEntryData.valid);
    break;
  
  case I_agentDiffServAuxMfClfrTosMask:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((usmDbDiffServAuxMFClfrTosMaskGet(USMDB_UNIT_CURRENT,
                                          agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                          &snmp_buffer[0]) == L7_SUCCESS) &&
        (SafeMakeOctetString(&agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrTosMask, snmp_buffer, 1) == L7_TRUE))
      SET_VALID(I_agentDiffServAuxMfClfrTosMask, agentDiffServAuxMfClfrEntryData.valid);
    break;

  case I_agentDiffServAuxMfClfrDstMac:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    snmp_buffer_len = SNMP_BUFFER_LEN;
    if ((usmDbDiffServAuxMFClfrDstMacGet(USMDB_UNIT_CURRENT,
                                         agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                         snmp_buffer, &snmp_buffer_len) == L7_SUCCESS) &&
        SafeMakeOctetString(&agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrDstMac, snmp_buffer, snmp_buffer_len) == L7_TRUE)
      SET_VALID(I_agentDiffServAuxMfClfrDstMac, agentDiffServAuxMfClfrEntryData.valid);
    break;
  
  case I_agentDiffServAuxMfClfrDstMacMask:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    snmp_buffer_len = SNMP_BUFFER_LEN;
    if ((usmDbDiffServAuxMFClfrDstMacMaskGet(USMDB_UNIT_CURRENT,
                                             agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                             snmp_buffer, &snmp_buffer_len) == L7_SUCCESS) &&
        SafeMakeOctetString(&agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrDstMacMask, snmp_buffer, snmp_buffer_len) == L7_TRUE)
      SET_VALID(I_agentDiffServAuxMfClfrDstMacMask, agentDiffServAuxMfClfrEntryData.valid);
    break;
  
  case I_agentDiffServAuxMfClfrSrcMac:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    snmp_buffer_len = SNMP_BUFFER_LEN;
    if ((usmDbDiffServAuxMFClfrSrcMacGet(USMDB_UNIT_CURRENT,
                                         agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                         snmp_buffer, &snmp_buffer_len) == L7_SUCCESS) &&
        SafeMakeOctetString(&agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrSrcMac, snmp_buffer, snmp_buffer_len) == L7_TRUE)
      SET_VALID(I_agentDiffServAuxMfClfrSrcMac, agentDiffServAuxMfClfrEntryData.valid);
    break;
  
  case I_agentDiffServAuxMfClfrSrcMacMask:
    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    snmp_buffer_len = SNMP_BUFFER_LEN;
    if ((usmDbDiffServAuxMFClfrSrcMacMaskGet(USMDB_UNIT_CURRENT,
                                             agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                             snmp_buffer, &snmp_buffer_len) == L7_SUCCESS) &&
        SafeMakeOctetString(&agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrSrcMacMask, snmp_buffer, snmp_buffer_len) == L7_TRUE)
      SET_VALID(I_agentDiffServAuxMfClfrSrcMacMask, agentDiffServAuxMfClfrEntryData.valid);
    break;

#ifdef OBSOLETE
  case I_agentDiffServAuxMfClfrVlanId:
    if (usmDbDiffServAuxMFClfrVlanIdGet(USMDB_UNIT_CURRENT,
                                     agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                     &agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrVlanId) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAuxMfClfrVlanId, agentDiffServAuxMfClfrEntryData.valid);
    break;
#endif

  case I_agentDiffServAuxMfClfrStorage:
    if (snmpAgentDiffServAuxMFClfrStorageGet(USMDB_UNIT_CURRENT,
                                             agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                             &agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrStorage) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAuxMfClfrStorage, agentDiffServAuxMfClfrEntryData.valid);
    break;
  
  case I_agentDiffServAuxMfClfrStatus:
    if (snmpAgentDiffServAuxMFClfrStatusGet(USMDB_UNIT_CURRENT,
                                            agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                            &agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrStatus) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAuxMfClfrStatus, agentDiffServAuxMfClfrEntryData.valid);
    break;

  case I_agentDiffServAuxMfClfrCos2:
    if (usmDbDiffServAuxMFClfrCos2Get(USMDB_UNIT_CURRENT,
                                      agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                      &agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrCos2) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAuxMfClfrCos2, agentDiffServAuxMfClfrEntryData.valid);
    break;

  case I_agentDiffServAuxMfClfrEtypeVal1:
    if (usmDbDiffServAuxMFClfrEtypeVal1Get(USMDB_UNIT_CURRENT,
                                           agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                           &agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrEtypeVal1) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAuxMfClfrEtypeVal1, agentDiffServAuxMfClfrEntryData.valid);
    break;

  case I_agentDiffServAuxMfClfrEtypeVal2:
    if (usmDbDiffServAuxMFClfrEtypeVal2Get(USMDB_UNIT_CURRENT,
                                           agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                           &agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrEtypeVal2) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAuxMfClfrEtypeVal2, agentDiffServAuxMfClfrEntryData.valid);
    break;

  case I_agentDiffServAuxMfClfrVlanIdMin:
    if (usmDbDiffServAuxMFClfrVlanIdMinGet(USMDB_UNIT_CURRENT,
                                           agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                           &agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrVlanIdMin) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAuxMfClfrVlanIdMin, agentDiffServAuxMfClfrEntryData.valid);
    break;

  case I_agentDiffServAuxMfClfrVlanIdMax:
    if (usmDbDiffServAuxMFClfrVlanIdMaxGet(USMDB_UNIT_CURRENT,
                                           agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                           &agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrVlanIdMax) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAuxMfClfrVlanIdMax, agentDiffServAuxMfClfrEntryData.valid);
    break;

  case I_agentDiffServAuxMfClfrVlanId2Min:
    if (usmDbDiffServAuxMFClfrVlanId2MinGet(USMDB_UNIT_CURRENT,
                                            agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                            &agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrVlanId2Min) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAuxMfClfrVlanId2Min, agentDiffServAuxMfClfrEntryData.valid);
    break;

  case I_agentDiffServAuxMfClfrVlanId2Max:
    if (usmDbDiffServAuxMFClfrVlanId2MaxGet(USMDB_UNIT_CURRENT,
                                            agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrId,
                                            &agentDiffServAuxMfClfrEntryData.agentDiffServAuxMfClfrVlanId2Max) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAuxMfClfrVlanId2Max, agentDiffServAuxMfClfrEntryData.valid);
    break;

  default:
    return(NULL);
    break;
  }


  if (nominator >= 0 && !VALID(nominator, agentDiffServAuxMfClfrEntryData.valid))
    return(NULL);
  
  return(&agentDiffServAuxMfClfrEntryData);
}

#ifdef SETS
int
k_agentDiffServAuxMfClfrEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                   doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_agentDiffServAuxMfClfrEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentDiffServAuxMfClfrEntry_set_defaults(doList_t *dp)
{
  agentDiffServAuxMfClfrEntry_t *data = (agentDiffServAuxMfClfrEntry_t *) (dp->data);

  if ((data->agentDiffServAuxMfClfrTos = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->agentDiffServAuxMfClfrTosMask = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->agentDiffServAuxMfClfrDstMac = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->agentDiffServAuxMfClfrDstMacMask = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->agentDiffServAuxMfClfrSrcMac = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->agentDiffServAuxMfClfrSrcMacMask = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->agentDiffServAuxMfClfrStorage = D_agentDiffServAuxMfClfrStorage_nonVolatile;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentDiffServAuxMfClfrEntry_set(agentDiffServAuxMfClfrEntry_t *data,
                                  ContextInfo *contextInfo, int function)
{

  return COMMIT_FAILED_ERROR;
}

  #ifdef SR_agentDiffServAuxMfClfrEntry_UNDO
/* add #define SR_agentDiffServAuxMfClfrEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDiffServAuxMfClfrEntry family.
 */
int
agentDiffServAuxMfClfrEntry_undo(doList_t *doHead, doList_t *doCur,
                                 ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentDiffServAuxMfClfrEntry_UNDO */

#endif /* SETS */

agentDiffServIpPrecMarkActEntry_t *
k_agentDiffServIpPrecMarkActEntry_get(int serialNum, ContextInfo *contextInfo,
                                      int nominator,
                                      int searchType,
                                      SR_INT32 agentDiffServIpPrecMarkActPrecedence)
{
  static agentDiffServIpPrecMarkActEntry_t agentDiffServIpPrecMarkActEntryData;

  ZERO_VALID(agentDiffServIpPrecMarkActEntryData.valid);
  agentDiffServIpPrecMarkActEntryData.agentDiffServIpPrecMarkActPrecedence = agentDiffServIpPrecMarkActPrecedence;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  SET_VALID(I_agentDiffServIpPrecMarkActPrecedence, agentDiffServIpPrecMarkActEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbDiffServIpPrecMarkActGet(USMDB_UNIT_CURRENT, 
                                      agentDiffServIpPrecMarkActEntryData.agentDiffServIpPrecMarkActPrecedence) != L7_SUCCESS) :
       ( (usmDbDiffServIpPrecMarkActGet(USMDB_UNIT_CURRENT, 
                                        agentDiffServIpPrecMarkActEntryData.agentDiffServIpPrecMarkActPrecedence) != L7_SUCCESS) &&
         (usmDbDiffServIpPrecMarkActGetNext(USMDB_UNIT_CURRENT,
                                            agentDiffServIpPrecMarkActEntryData.agentDiffServIpPrecMarkActPrecedence,
                                            &agentDiffServIpPrecMarkActEntryData.agentDiffServIpPrecMarkActPrecedence) != L7_SUCCESS) ) )
  {
    ZERO_VALID(agentDiffServIpPrecMarkActEntryData.valid);
    return(NULL);
  }


  if (nominator >= 0 && !VALID(nominator, agentDiffServIpPrecMarkActEntryData.valid))
    return(NULL);

  return(&agentDiffServIpPrecMarkActEntryData);
}

agentDiffServAction_t *
k_agentDiffServAction_get(int serialNum, ContextInfo *contextInfo,
                          int nominator)
{
  static agentDiffServAction_t agentDiffServActionData;

  ZERO_VALID(agentDiffServActionData.valid);

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  if (usmDbDiffServAssignQueueIndexNext(USMDB_UNIT_CURRENT, 
		          &agentDiffServActionData.agentDiffServAssignQueueNextFree) == L7_SUCCESS)
    SET_VALID(I_agentDiffServAssignQueueNextFree, agentDiffServActionData.valid);
  
  if (usmDbDiffServRedirectIndexNext(USMDB_UNIT_CURRENT, 
		          &agentDiffServActionData.agentDiffServRedirectNextFree) == L7_SUCCESS)
    SET_VALID(I_agentDiffServRedirectNextFree, agentDiffServActionData.valid);
  
  if (usmDbDiffServMirrorIndexNext(USMDB_UNIT_CURRENT, 
		          &agentDiffServActionData.agentDiffServMirrorNextFree) == L7_SUCCESS)
    SET_VALID(I_agentDiffServMirrorNextFree, agentDiffServActionData.valid);
  
  if (nominator >= 0 && !VALID(nominator, agentDiffServActionData.valid))
    return(NULL);

  return(&agentDiffServActionData);
}

agentDiffServCosMarkActEntry_t *
k_agentDiffServCosMarkActEntry_get(int serialNum, ContextInfo *contextInfo,
                                   int nominator,
                                   int searchType,
                                   SR_INT32 agentDiffServCosMarkActCos)
{
  static agentDiffServCosMarkActEntry_t agentDiffServCosMarkActEntryData;

  ZERO_VALID(agentDiffServCosMarkActEntryData.valid);
  agentDiffServCosMarkActEntryData.agentDiffServCosMarkActCos = agentDiffServCosMarkActCos;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  SET_VALID(I_agentDiffServCosMarkActCos, agentDiffServCosMarkActEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbDiffServCosMarkActGet(USMDB_UNIT_CURRENT, 
                                   agentDiffServCosMarkActEntryData.agentDiffServCosMarkActCos) != L7_SUCCESS) :
       ( (usmDbDiffServCosMarkActGet(USMDB_UNIT_CURRENT, 
                                     agentDiffServCosMarkActEntryData.agentDiffServCosMarkActCos) != L7_SUCCESS) &&
         (usmDbDiffServCosMarkActGetNext(USMDB_UNIT_CURRENT,
                                         agentDiffServCosMarkActEntryData.agentDiffServCosMarkActCos,
                                         &agentDiffServCosMarkActEntryData.agentDiffServCosMarkActCos) != L7_SUCCESS) ) )
  {
    ZERO_VALID(agentDiffServCosMarkActEntryData.valid);
    return(NULL);
  }


  if (nominator >= 0 && !VALID(nominator, agentDiffServCosMarkActEntryData.valid))
    return(NULL);

  return(&agentDiffServCosMarkActEntryData);
}

agentDiffServCos2MarkActEntry_t *
k_agentDiffServCos2MarkActEntry_get(int serialNum, ContextInfo *contextInfo,
                                    int nominator,
                                    int searchType,
                                    SR_INT32 agentDiffServCos2MarkActCos)
{
   static agentDiffServCos2MarkActEntry_t agentDiffServCos2MarkActEntryData;

   ZERO_VALID(agentDiffServCos2MarkActEntryData.valid);
   agentDiffServCos2MarkActEntryData.agentDiffServCos2MarkActCos = agentDiffServCos2MarkActCos;
   SET_VALID(I_agentDiffServCos2MarkActCos, agentDiffServCos2MarkActEntryData.valid);

   if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
   {
     return(NULL);
   }

   if ((searchType == EXACT) ?
       (usmDbDiffServCos2MarkActGet(USMDB_UNIT_CURRENT, 
                                    agentDiffServCos2MarkActEntryData.agentDiffServCos2MarkActCos) != L7_SUCCESS) :
       ((usmDbDiffServCos2MarkActGet(USMDB_UNIT_CURRENT, 
                                     agentDiffServCos2MarkActEntryData.agentDiffServCos2MarkActCos) != L7_SUCCESS) &&
        (usmDbDiffServCos2MarkActGetNext(USMDB_UNIT_CURRENT, 
                                         agentDiffServCos2MarkActEntryData.agentDiffServCos2MarkActCos,
                                         &agentDiffServCos2MarkActEntryData.agentDiffServCos2MarkActCos) != L7_SUCCESS)))
     return(NULL);

   if (nominator >= 0 && !VALID(nominator, agentDiffServCos2MarkActEntryData.valid))
     return(NULL);

   return(&agentDiffServCos2MarkActEntryData);
}

agentDiffServAssignQueueEntry_t *
k_agentDiffServAssignQueueEntry_get(int serialNum, ContextInfo *contextInfo,
                                    int nominator,
                                    int searchType,
                                    SR_UINT32 agentDiffServAssignQueueIndex)
{
  static agentDiffServAssignQueueEntry_t agentDiffServAssignQueueEntryData;
  
  ZERO_VALID(agentDiffServAssignQueueEntryData.valid);
  agentDiffServAssignQueueEntryData.agentDiffServAssignQueueIndex = agentDiffServAssignQueueIndex;
  SET_VALID(I_agentDiffServAssignQueueIndex, agentDiffServAssignQueueEntryData.valid);
  
  
  if ((searchType == EXACT) ?
      (usmDbDiffServAssignQueueGet(USMDB_UNIT_CURRENT, agentDiffServAssignQueueEntryData.agentDiffServAssignQueueIndex) != L7_SUCCESS) :
      (usmDbDiffServAssignQueueGet(USMDB_UNIT_CURRENT, agentDiffServAssignQueueEntryData.agentDiffServAssignQueueIndex) != L7_SUCCESS) &&
      (usmDbDiffServAssignQueueGetNext(USMDB_UNIT_CURRENT, agentDiffServAssignQueueEntryData.agentDiffServAssignQueueIndex, 
                                       &agentDiffServAssignQueueEntryData.agentDiffServAssignQueueIndex) != L7_SUCCESS))
  {
    return(NULL);
  }

  switch (nominator)
  {
  case -1:
  case I_agentDiffServAssignQueueIndex:
    break;

  case I_agentDiffServAssignQueueQnum:
    if (usmDbDiffServAssignQueueQnumGet(USMDB_UNIT_CURRENT, 
                                        agentDiffServAssignQueueEntryData.agentDiffServAssignQueueIndex,
                                        &agentDiffServAssignQueueEntryData.agentDiffServAssignQueueQnum) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAssignQueueQnum, agentDiffServAssignQueueEntryData.valid);
    break;

  case I_agentDiffServAssignQueueStorage:
    if (snmpAgentDiffServAssignQueueStorageGet(USMDB_UNIT_CURRENT, 
                                               agentDiffServAssignQueueEntryData.agentDiffServAssignQueueIndex,
                                               &agentDiffServAssignQueueEntryData.agentDiffServAssignQueueStorage) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAssignQueueStorage, agentDiffServAssignQueueEntryData.valid);
    break;

  case I_agentDiffServAssignQueueStatus:
    if (snmpAgentDiffServAssignQueueStatusGet(USMDB_UNIT_CURRENT, 
                                              agentDiffServAssignQueueEntryData.agentDiffServAssignQueueIndex,
                                              &agentDiffServAssignQueueEntryData.agentDiffServAssignQueueStatus) == L7_SUCCESS)
      SET_VALID(I_agentDiffServAssignQueueStatus, agentDiffServAssignQueueEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }
  
  if (nominator >= 0 && !VALID(nominator, agentDiffServAssignQueueEntryData.valid))
    return(NULL);

  return(&agentDiffServAssignQueueEntryData);
}

#ifdef SETS
int
k_agentDiffServAssignQueueEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                     doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentDiffServAssignQueueEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                      doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentDiffServAssignQueueEntry_set_defaults(doList_t *dp)
{
    agentDiffServAssignQueueEntry_t *data = (agentDiffServAssignQueueEntry_t *) (dp->data);

    data->agentDiffServAssignQueueStorage = D_agentDiffServAssignQueueStorage_nonVolatile;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentDiffServAssignQueueEntry_set(agentDiffServAssignQueueEntry_t *data,
                                    ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_agentDiffServAssignQueueEntry_UNDO
/* add #define SR_agentDiffServAssignQueueEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDiffServAssignQueueEntry family.
 */
int
agentDiffServAssignQueueEntry_undo(doList_t *doHead, doList_t *doCur,
                                   ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDiffServAssignQueueEntry_UNDO */

#endif /* SETS */

agentDiffServRedirectEntry_t *
k_agentDiffServRedirectEntry_get(int serialNum, ContextInfo *contextInfo,
                                 int nominator,
                                 int searchType,
                                 SR_UINT32 agentDiffServRedirectId)
{
  static agentDiffServRedirectEntry_t agentDiffServRedirectEntryData;
  
  ZERO_VALID(agentDiffServRedirectEntryData.valid);
  agentDiffServRedirectEntryData.agentDiffServRedirectId = agentDiffServRedirectId;
  SET_VALID(I_agentDiffServRedirectId, agentDiffServRedirectEntryData.valid);
  
  
  if ((searchType == EXACT) ?
     (usmDbDiffServRedirectGet(USMDB_UNIT_CURRENT, 
                                  agentDiffServRedirectEntryData.agentDiffServRedirectId) != L7_SUCCESS) :
     ((usmDbDiffServRedirectGet(USMDB_UNIT_CURRENT, 
                                  agentDiffServRedirectEntryData.agentDiffServRedirectId) != L7_SUCCESS) &&
      (usmDbDiffServRedirectGetNext(USMDB_UNIT_CURRENT, 
                                       agentDiffServRedirectEntryData.agentDiffServRedirectId,
                                       &agentDiffServRedirectEntryData.agentDiffServRedirectId) != L7_SUCCESS)))
   return(NULL);
  
  switch (nominator)
  {
  case -1:
  case I_agentDiffServRedirectId:
    break;

  case I_agentDiffServRedirectIntf:
    if (usmDbDiffServRedirectIntfGet(USMDB_UNIT_CURRENT, 
                                     agentDiffServRedirectEntryData.agentDiffServRedirectId,
                                     &agentDiffServRedirectEntryData.agentDiffServRedirectIntf) == L7_SUCCESS)
      SET_VALID(I_agentDiffServRedirectIntf, agentDiffServRedirectEntryData.valid);
    break;

  case I_agentDiffServRedirectStorage:
    if (snmpAgentDiffServRedirectStorageGet(USMDB_UNIT_CURRENT, 
                                            agentDiffServRedirectEntryData.agentDiffServRedirectId,
                                            &agentDiffServRedirectEntryData.agentDiffServRedirectStorage) == L7_SUCCESS)
      SET_VALID(I_agentDiffServRedirectStorage, agentDiffServRedirectEntryData.valid);
    break;

  case I_agentDiffServRedirectStatus:
    if (snmpAgentDiffServRedirectStatusGet(USMDB_UNIT_CURRENT, 
                                           agentDiffServRedirectEntryData.agentDiffServRedirectId,
                                           &agentDiffServRedirectEntryData.agentDiffServRedirectStatus) == L7_SUCCESS)
      SET_VALID(I_agentDiffServRedirectStatus, agentDiffServRedirectEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
  }
  
  if (nominator != -1 && !VALID(nominator, agentDiffServRedirectEntryData.valid))
    return(NULL);

  return(&agentDiffServRedirectEntryData);
}

#ifdef SETS
int
k_agentDiffServRedirectEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                  doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentDiffServRedirectEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                   doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentDiffServRedirectEntry_set_defaults(doList_t *dp)
{
    agentDiffServRedirectEntry_t *data = (agentDiffServRedirectEntry_t *) (dp->data);

    data->agentDiffServRedirectStorage = D_agentDiffServRedirectStorage_nonVolatile;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentDiffServRedirectEntry_set(agentDiffServRedirectEntry_t *data,
                                 ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_agentDiffServRedirectEntry_UNDO
/* add #define SR_agentDiffServRedirectEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDiffServRedirectEntry family.
 */
int
agentDiffServRedirectEntry_undo(doList_t *doHead, doList_t *doCur,
                                ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDiffServRedirectEntry_UNDO */

#endif /* SETS */

agentDiffServMirrorEntry_t *
k_agentDiffServMirrorEntry_get(int serialNum, ContextInfo *contextInfo,
                               int nominator,
                               int searchType,
                               SR_UINT32 agentDiffServMirrorId)
{
  static agentDiffServMirrorEntry_t agentDiffServMirrorEntryData;
  
  ZERO_VALID(agentDiffServMirrorEntryData.valid);
  agentDiffServMirrorEntryData.agentDiffServMirrorId = agentDiffServMirrorId;
  SET_VALID(I_agentDiffServMirrorId, agentDiffServMirrorEntryData.valid);
  
  
  if ((searchType == EXACT) ?
     (usmDbDiffServMirrorGet(USMDB_UNIT_CURRENT, 
                                agentDiffServMirrorEntryData.agentDiffServMirrorId) != L7_SUCCESS) :
     ((usmDbDiffServMirrorGet(USMDB_UNIT_CURRENT, 
                                  agentDiffServMirrorEntryData.agentDiffServMirrorId) != L7_SUCCESS) &&
      (usmDbDiffServMirrorGetNext(USMDB_UNIT_CURRENT, 
                                     agentDiffServMirrorEntryData.agentDiffServMirrorId,
                                     &agentDiffServMirrorEntryData.agentDiffServMirrorId) != L7_SUCCESS)))
   return(NULL);
  
  switch (nominator)
  {
  case -1:
  case I_agentDiffServMirrorId:
    break;

  case I_agentDiffServMirrorIntf:
    if (usmDbDiffServMirrorIntfGet(USMDB_UNIT_CURRENT, 
                                   agentDiffServMirrorEntryData.agentDiffServMirrorId,
                                   &agentDiffServMirrorEntryData.agentDiffServMirrorIntf) == L7_SUCCESS)
      SET_VALID(I_agentDiffServMirrorIntf, agentDiffServMirrorEntryData.valid);
    break;

  case I_agentDiffServMirrorStorage:
    if (snmpAgentDiffServMirrorStorageGet(USMDB_UNIT_CURRENT, 
                                          agentDiffServMirrorEntryData.agentDiffServMirrorId,
                                          &agentDiffServMirrorEntryData.agentDiffServMirrorStorage) == L7_SUCCESS)
      SET_VALID(I_agentDiffServMirrorStorage, agentDiffServMirrorEntryData.valid);
    break;

  case I_agentDiffServMirrorStatus:
    if (snmpAgentDiffServMirrorStatusGet(USMDB_UNIT_CURRENT, 
                                         agentDiffServMirrorEntryData.agentDiffServMirrorId,
                                         &agentDiffServMirrorEntryData.agentDiffServMirrorStatus) == L7_SUCCESS)
      SET_VALID(I_agentDiffServMirrorStatus, agentDiffServMirrorEntryData.valid);
    break;

  default:
    /* unknown nominator */
    return(NULL);
  }
  
  if (nominator != -1 && !VALID(nominator, agentDiffServMirrorEntryData.valid))
    return(NULL);

  return(&agentDiffServMirrorEntryData);
}

#ifdef SETS
int
k_agentDiffServMirrorEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_agentDiffServMirrorEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                 doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_agentDiffServMirrorEntry_set_defaults(doList_t *dp)
{
    agentDiffServMirrorEntry_t *data = (agentDiffServMirrorEntry_t *) (dp->data);

    data->agentDiffServMirrorStorage = D_agentDiffServMirrorStorage_nonVolatile;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_agentDiffServMirrorEntry_set(agentDiffServMirrorEntry_t *data,
                               ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_agentDiffServMirrorEntry_UNDO
/* add #define SR_agentDiffServMirrorEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentDiffServMirrorEntry family.
 */
int
agentDiffServMirrorEntry_undo(doList_t *doHead, doList_t *doCur,
                              ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentDiffServMirrorEntry_UNDO */

#endif /* SETS */

/*
   agentDiffServColorAwareEntryData.agentDiffServColorAwareLevel = ;
   agentDiffServColorAwareEntryData.agentDiffServColorAwareMode = ;
   agentDiffServColorAwareEntryData.agentDiffServColorAwareValue = ;
   agentDiffServColorAwareEntryData.agentDiffServColorAwareStorage = ;
   agentDiffServColorAwareEntryData.agentDiffServColorAwareStatus = ;
*/
