/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\mgmt\snmp\snmp_sr\snmpd\unix\k_mib_qos_diffserv.c
*
* @purpose  Provide interface to DiffServ Standard MIB
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
#include "k_mib_qos_diffserv_api.h"
#include "cnfgr.h"
#include "usmdb_common.h"
#include "diffserv_exports.h"
#include "usmdb_mib_diffserv_api.h"
#include "usmdb_util_api.h"

diffServDataPathEntry_t *
k_diffServDataPathEntry_get(int serialNum, ContextInfo *contextInfo,
                            int nominator,
                            int searchType,
                            SR_INT32 ifIndex,
                            SR_INT32 diffServDataPathIfDirection)
{

  static diffServDataPathEntry_t diffServDataPathEntryData;

  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 intIfNum;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    diffServDataPathEntryData.diffServDataPathStart = MakeOID(NULL, 0);
  }

  ZERO_VALID(diffServDataPathEntryData.valid);
  diffServDataPathEntryData.ifIndex = ifIndex;
  diffServDataPathEntryData.diffServDataPathIfDirection = diffServDataPathIfDirection;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  SET_VALID(I_diffServDataPathEntryIndex_ifIndex, diffServDataPathEntryData.valid);
  SET_VALID(I_diffServDataPathIfDirection, diffServDataPathEntryData.valid);

  if ( (searchType == EXACT) ?
       ( (usmDbIntIfNumFromExtIfNum(diffServDataPathEntryData.ifIndex, &intIfNum) != L7_SUCCESS) ||
         (snmpDiffServDataPathGet(USMDB_UNIT_CURRENT, intIfNum,
                                  diffServDataPathEntryData.diffServDataPathIfDirection) != L7_SUCCESS) ) :

       ( ( (usmDbVisibleExtIfNumberCheck( USMDB_UNIT_CURRENT, diffServDataPathEntryData.ifIndex) != L7_SUCCESS) &&
           (usmDbGetNextVisibleExtIfNumber(diffServDataPathEntryData.ifIndex, &diffServDataPathEntryData.ifIndex) != L7_SUCCESS) ) ||
         (usmDbIntIfNumFromExtIfNum(diffServDataPathEntryData.ifIndex, &intIfNum) != L7_SUCCESS) ||
         ( (snmpDiffServDataPathGet(USMDB_UNIT_CURRENT, intIfNum,
                                    diffServDataPathEntryData.diffServDataPathIfDirection) != L7_SUCCESS) &&
           (snmpDiffServDataPathGetNext(USMDB_UNIT_CURRENT, &intIfNum,
                                        &diffServDataPathEntryData.diffServDataPathIfDirection) != L7_SUCCESS)) ||
         (usmDbExtIfNumFromIntIfNum(intIfNum, &diffServDataPathEntryData.ifIndex) != L7_SUCCESS) )
     )
  {
    return(NULL);
  }

  switch (nominator)
  {
  
  case -1:
  case I_diffServDataPathEntryIndex_ifIndex:
    break;

  case I_diffServDataPathIfDirection:
    break;

  case I_diffServDataPathStart :

    if (snmpDiffServDataPathStartGet(USMDB_UNIT_CURRENT,
                                     intIfNum,
                                     diffServDataPathEntryData.diffServDataPathIfDirection,
                                     snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOIDFromDot(&diffServDataPathEntryData.diffServDataPathStart, snmp_buffer) == L7_TRUE))
      SET_VALID(I_diffServDataPathStart, diffServDataPathEntryData.valid);

    break;

  case I_diffServDataPathStorage :

    if (snmpDiffServDataPathStorageTypeGet(USMDB_UNIT_CURRENT, 
                                           intIfNum,
                                           diffServDataPathEntryData.diffServDataPathIfDirection,
                                           &diffServDataPathEntryData.diffServDataPathStorage) == L7_SUCCESS)
      SET_VALID(I_diffServDataPathStorage, diffServDataPathEntryData.valid);
    break;

  case I_diffServDataPathStatus :

    if (snmpDiffServDataPathRowStatusGet(USMDB_UNIT_CURRENT, 
                                         intIfNum,
                                         diffServDataPathEntryData.diffServDataPathIfDirection,
                                         &diffServDataPathEntryData.diffServDataPathStatus) == L7_SUCCESS)
      SET_VALID(I_diffServDataPathStatus, diffServDataPathEntryData.valid);
    break;

  default:
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, diffServDataPathEntryData.valid))
    return(NULL);

  return(&diffServDataPathEntryData); 

}


#ifdef SETS
int
k_diffServDataPathEntry_test(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_diffServDataPathEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                              doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_diffServDataPathEntry_set_defaults(doList_t *dp)
{
  diffServDataPathEntry_t *data = (diffServDataPathEntry_t *) (dp->data);

  if ((data->diffServDataPathStart = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->diffServDataPathStorage = D_diffServDataPathStorage_nonVolatile;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_diffServDataPathEntry_set(diffServDataPathEntry_t *data,
                            ContextInfo *contextInfo, int function)
{

  return COMMIT_FAILED_ERROR;
}

  #ifdef SR_diffServDataPathEntry_UNDO
/* add #define SR_diffServDataPathEntry_UNDO in sitedefs.h to
 * include the undo routine for the diffServDataPathEntry family.
 */
int
diffServDataPathEntry_undo(doList_t *doHead, doList_t *doCur,
                           ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_diffServDataPathEntry_UNDO */

#endif /* SETS */

diffServClassifier_t *
k_diffServClassifier_get(int serialNum, ContextInfo *contextInfo,
                         int nominator)
{
  static diffServClassifier_t diffServClassifierData;

  ZERO_VALID(diffServClassifierData.valid);

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  if (usmDbDiffServClfrIndexNext(USMDB_UNIT_CURRENT, 
		          &diffServClassifierData.diffServClfrNextFree) == L7_SUCCESS)
    SET_VALID(I_diffServClfrNextFree, diffServClassifierData.valid);
  
  if (usmDbDiffServClfrElemIndexNext(USMDB_UNIT_CURRENT, 
		          &diffServClassifierData.diffServClfrElementNextFree) == L7_SUCCESS)
    SET_VALID(I_diffServClfrElementNextFree, diffServClassifierData.valid);
  
  if (usmDbDiffServMFClfrIndexNext(USMDB_UNIT_CURRENT, 
		          &diffServClassifierData.diffServMultiFieldClfrNextFree) == L7_SUCCESS)
    SET_VALID(I_diffServMultiFieldClfrNextFree, diffServClassifierData.valid);
  
  if (nominator >= 0 && !VALID(nominator, diffServClassifierData.valid))
    return(NULL);

  return(&diffServClassifierData);
}


diffServClfrEntry_t *
k_diffServClfrEntry_get(int serialNum, ContextInfo *contextInfo,
                        int nominator,
                        int searchType,
                        SR_UINT32 diffServClfrId)
{

  static diffServClfrEntry_t diffServClfrEntryData;

  ZERO_VALID(diffServClfrEntryData.valid);
  diffServClfrEntryData.diffServClfrId = diffServClfrId;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  SET_VALID(I_diffServClfrId, diffServClfrEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbDiffServClfrGet(USMDB_UNIT_CURRENT, diffServClfrEntryData.diffServClfrId) != L7_SUCCESS) :
       ((usmDbDiffServClfrGet(USMDB_UNIT_CURRENT, diffServClfrEntryData.diffServClfrId) != L7_SUCCESS) &&
        (usmDbDiffServClfrGetNext(USMDB_UNIT_CURRENT, diffServClfrEntryData.diffServClfrId,
                                  &diffServClfrEntryData.diffServClfrId) != L7_SUCCESS)))
  {
    ZERO_VALID(diffServClfrEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  
  case -1:

  case I_diffServClfrId:
    break;

  case I_diffServClfrStorage :

    if (snmpDiffServClfrStorageGet(USMDB_UNIT_CURRENT, 
                                    diffServClfrEntryData.diffServClfrId,
                                    &diffServClfrEntryData.diffServClfrStorage) == L7_SUCCESS)
      SET_VALID(I_diffServClfrStorage, diffServClfrEntryData.valid);
    break;

  case I_diffServClfrStatus :

    if (snmpDiffServClfrStatusGet(USMDB_UNIT_CURRENT, 
                                   diffServClfrEntryData.diffServClfrId,
                                   &diffServClfrEntryData.diffServClfrStatus) == L7_SUCCESS)
      SET_VALID(I_diffServClfrStatus, diffServClfrEntryData.valid);
    break;

  default:
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, diffServClfrEntryData.valid))
    return(NULL);

  return(&diffServClfrEntryData); 
}

#ifdef SETS
int
k_diffServClfrEntry_test(ObjectInfo *object, ObjectSyntax *value,
                         doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_diffServClfrEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                          doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_diffServClfrEntry_set_defaults(doList_t *dp)
{
  diffServClfrEntry_t *data = (diffServClfrEntry_t *) (dp->data);

  data->diffServClfrStorage = D_diffServClfrStorage_nonVolatile;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_diffServClfrEntry_set(diffServClfrEntry_t *data,
                        ContextInfo *contextInfo, int function)
{

  return COMMIT_FAILED_ERROR;
}

  #ifdef SR_diffServClfrEntry_UNDO
/* add #define SR_diffServClfrEntry_UNDO in sitedefs.h to
 * include the undo routine for the diffServClfrEntry family.
 */
int
diffServClfrEntry_undo(doList_t *doHead, doList_t *doCur,
                       ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_diffServClfrEntry_UNDO */

#endif /* SETS */

diffServClfrElementEntry_t *
k_diffServClfrElementEntry_get(int serialNum, ContextInfo *contextInfo,
                               int nominator,
                               int searchType,
                               SR_UINT32 diffServClfrId,
                               SR_UINT32 diffServClfrElementId)
{
  static diffServClfrElementEntry_t diffServClfrElementEntryData;

  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    diffServClfrElementEntryData.diffServClfrElementNext = MakeOID(NULL, 0);
    diffServClfrElementEntryData.diffServClfrElementSpecific = MakeOID(NULL, 0);
  }

  ZERO_VALID(diffServClfrElementEntryData.valid);
  diffServClfrElementEntryData.diffServClfrId = diffServClfrId;
  diffServClfrElementEntryData.diffServClfrElementId = diffServClfrElementId;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  SET_VALID(I_diffServClfrElementEntryIndex_diffServClfrId, diffServClfrElementEntryData.valid);
  SET_VALID(I_diffServClfrElementId, diffServClfrElementEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbDiffServClfrElemGet(USMDB_UNIT_CURRENT, diffServClfrElementEntryData.diffServClfrId,
                                 diffServClfrElementEntryData.diffServClfrElementId) != L7_SUCCESS) :
       ( (usmDbDiffServClfrElemGet(USMDB_UNIT_CURRENT, diffServClfrElementEntryData.diffServClfrId,
                                   diffServClfrElementEntryData.diffServClfrElementId) != L7_SUCCESS) &&
         (usmDbDiffServClfrElemGetNext(USMDB_UNIT_CURRENT, diffServClfrElementEntryData.diffServClfrId,
                                       diffServClfrElementEntryData.diffServClfrElementId,
                                       &diffServClfrElementEntryData.diffServClfrId,
                                       &diffServClfrElementEntryData.diffServClfrElementId) != L7_SUCCESS) ) )
  {
    ZERO_VALID(diffServClfrElementEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  
  case -1:
  case I_diffServClfrElementEntryIndex_diffServClfrId:
    break;

  case I_diffServClfrElementId:
    break;

  case I_diffServClfrElementPrecedence :

    if (usmDbDiffServClfrElemPrecedenceGet(USMDB_UNIT_CURRENT, 
                                           diffServClfrElementEntryData.diffServClfrId,
                                           diffServClfrElementEntryData.diffServClfrElementId,
                                           &diffServClfrElementEntryData.diffServClfrElementPrecedence) == L7_SUCCESS)
      SET_VALID(I_diffServClfrElementPrecedence, diffServClfrElementEntryData.valid);
    break;

  case I_diffServClfrElementNext :

    if (snmpDiffServClfrElemNextGet(USMDB_UNIT_CURRENT,
                                    diffServClfrElementEntryData.diffServClfrId,
                                    diffServClfrElementEntryData.diffServClfrElementId,
                                    snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOIDFromDot(&diffServClfrElementEntryData.diffServClfrElementNext, snmp_buffer) == L7_TRUE))
      SET_VALID(I_diffServClfrElementNext, diffServClfrElementEntryData.valid);

    break;


  case I_diffServClfrElementSpecific :

    if (snmpDiffServClfrElemSpecificGet(USMDB_UNIT_CURRENT,
                                        diffServClfrElementEntryData.diffServClfrId,
                                        diffServClfrElementEntryData.diffServClfrElementId,
                                        snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOIDFromDot(&diffServClfrElementEntryData.diffServClfrElementSpecific, snmp_buffer) == L7_TRUE))
      SET_VALID(I_diffServClfrElementSpecific, diffServClfrElementEntryData.valid);

    break;


  case I_diffServClfrElementStorage :

    if (snmpDiffServClfrElemStorageGet(USMDB_UNIT_CURRENT, 
                                       diffServClfrElementEntryData.diffServClfrId,
                                       diffServClfrElementEntryData.diffServClfrElementId,
                                       &diffServClfrElementEntryData.diffServClfrElementStorage) == L7_SUCCESS)
      SET_VALID(I_diffServClfrElementStorage, diffServClfrElementEntryData.valid);
    break;

  case I_diffServClfrElementStatus :

    if (snmpDiffServClfrElemStatusGet(USMDB_UNIT_CURRENT, 
                                      diffServClfrElementEntryData.diffServClfrId,
                                      diffServClfrElementEntryData.diffServClfrElementId,
                                      &diffServClfrElementEntryData.diffServClfrElementStatus) == L7_SUCCESS)
      SET_VALID(I_diffServClfrElementStatus, diffServClfrElementEntryData.valid);
    break;

  default:
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, diffServClfrElementEntryData.valid))
    return(NULL);

  return(&diffServClfrElementEntryData); 
}

#ifdef SETS
int
k_diffServClfrElementEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_diffServClfrElementEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                 doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_diffServClfrElementEntry_set_defaults(doList_t *dp)
{
  diffServClfrElementEntry_t *data = (diffServClfrElementEntry_t *) (dp->data);

  if ((data->diffServClfrElementNext = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->diffServClfrElementSpecific = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->diffServClfrElementStorage = D_diffServClfrElementStorage_nonVolatile;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_diffServClfrElementEntry_set(diffServClfrElementEntry_t *data,
                               ContextInfo *contextInfo, int function)
{

  return COMMIT_FAILED_ERROR;
}

  #ifdef SR_diffServClfrElementEntry_UNDO
/* add #define SR_diffServClfrElementEntry_UNDO in sitedefs.h to
 * include the undo routine for the diffServClfrElementEntry family.
 */
int
diffServClfrElementEntry_undo(doList_t *doHead, doList_t *doCur,
                              ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_diffServClfrElementEntry_UNDO */

#endif /* SETS */

diffServMultiFieldClfrEntry_t *
k_diffServMultiFieldClfrEntry_get(int serialNum, ContextInfo *contextInfo,
                                  int nominator,
                                  int searchType,
                                  SR_UINT32 diffServMultiFieldClfrId)
{
  static diffServMultiFieldClfrEntry_t diffServMultiFieldClfrEntryData;

  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];
  L7_uint32 temp_uint;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    diffServMultiFieldClfrEntryData.diffServMultiFieldClfrDstAddr = MakeOctetString(NULL, 0);
    diffServMultiFieldClfrEntryData.diffServMultiFieldClfrSrcAddr = MakeOctetString(NULL, 0);
  }

  ZERO_VALID(diffServMultiFieldClfrEntryData.valid);
  diffServMultiFieldClfrEntryData.diffServMultiFieldClfrId = diffServMultiFieldClfrId;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  SET_VALID(I_diffServMultiFieldClfrId, diffServMultiFieldClfrEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbDiffServMFClfrGet(USMDB_UNIT_CURRENT,
                               diffServMultiFieldClfrEntryData.diffServMultiFieldClfrId) != L7_SUCCESS) :
       ( (usmDbDiffServMFClfrGet(USMDB_UNIT_CURRENT,
                                 diffServMultiFieldClfrEntryData.diffServMultiFieldClfrId) != L7_SUCCESS) &&
         (usmDbDiffServMFClfrGetNext(USMDB_UNIT_CURRENT,
                                     diffServMultiFieldClfrEntryData.diffServMultiFieldClfrId,
                                     &diffServMultiFieldClfrEntryData.diffServMultiFieldClfrId) != L7_SUCCESS) ) )
  {
    ZERO_VALID(diffServMultiFieldClfrEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  
  case -1:
  case I_diffServMultiFieldClfrId:
    break;

  case I_diffServMultiFieldClfrAddrType :

    if (snmpDiffServMFClfrAddrTypeGet(USMDB_UNIT_CURRENT, 
                                      diffServMultiFieldClfrEntryData.diffServMultiFieldClfrId,
                                      &diffServMultiFieldClfrEntryData.diffServMultiFieldClfrAddrType) == L7_SUCCESS)
      SET_VALID(I_diffServMultiFieldClfrAddrType, diffServMultiFieldClfrEntryData.valid);
    break;


  case I_diffServMultiFieldClfrDstAddr :

    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((usmDbDiffServMFClfrDstAddrGet(USMDB_UNIT_CURRENT, 
                                       diffServMultiFieldClfrEntryData.diffServMultiFieldClfrId,
                                       &temp_uint) == L7_SUCCESS))
    {
      memcpy(snmp_buffer, (char*)&temp_uint, sizeof(temp_uint));
      if (SafeMakeOctetString(&diffServMultiFieldClfrEntryData.diffServMultiFieldClfrDstAddr, 
                              snmp_buffer, sizeof(temp_uint)) == L7_TRUE)
        SET_VALID(I_diffServMultiFieldClfrDstAddr, diffServMultiFieldClfrEntryData.valid);
    }
    break;


  case I_diffServMultiFieldClfrDstPrefixLength :

    if (usmDbDiffServMFClfrDstPrefixLenGet(USMDB_UNIT_CURRENT, 
                                           diffServMultiFieldClfrEntryData.diffServMultiFieldClfrId,
                                           &diffServMultiFieldClfrEntryData.diffServMultiFieldClfrDstPrefixLength) == L7_SUCCESS)
      SET_VALID(I_diffServMultiFieldClfrDstPrefixLength, diffServMultiFieldClfrEntryData.valid);
    break;

  case I_diffServMultiFieldClfrSrcAddr :

    bzero(snmp_buffer, SNMP_BUFFER_LEN);
    if ((usmDbDiffServMFClfrSrcAddrGet(USMDB_UNIT_CURRENT, 
                                       diffServMultiFieldClfrEntryData.diffServMultiFieldClfrId,
                                       &temp_uint) == L7_SUCCESS))
    {
      memcpy(snmp_buffer, (char*)&temp_uint, sizeof(temp_uint));
      if (SafeMakeOctetString(&diffServMultiFieldClfrEntryData.diffServMultiFieldClfrSrcAddr, 
                              snmp_buffer, sizeof(temp_uint)) == L7_TRUE)
        SET_VALID(I_diffServMultiFieldClfrSrcAddr, diffServMultiFieldClfrEntryData.valid);
    }
    break;


  case I_diffServMultiFieldClfrSrcPrefixLength :

    if (usmDbDiffServMFClfrSrcPrefixLenGet(USMDB_UNIT_CURRENT, 
                                           diffServMultiFieldClfrEntryData.diffServMultiFieldClfrId,
                                           &diffServMultiFieldClfrEntryData.diffServMultiFieldClfrSrcPrefixLength) == L7_SUCCESS)
      SET_VALID(I_diffServMultiFieldClfrSrcPrefixLength, diffServMultiFieldClfrEntryData.valid);
    break;


  case I_diffServMultiFieldClfrDscp :

    if (usmDbDiffServMFClfrDscpGet(USMDB_UNIT_CURRENT, 
                                   diffServMultiFieldClfrEntryData.diffServMultiFieldClfrId,
                                   &diffServMultiFieldClfrEntryData.diffServMultiFieldClfrDscp) == L7_SUCCESS)
      SET_VALID(I_diffServMultiFieldClfrDscp, diffServMultiFieldClfrEntryData.valid);
    break;


  case I_diffServMultiFieldClfrFlowId :

    if (usmDbDiffServMFClfrFlowIdGet(USMDB_UNIT_CURRENT, 
                                     diffServMultiFieldClfrEntryData.diffServMultiFieldClfrId,
                                     &diffServMultiFieldClfrEntryData.diffServMultiFieldClfrFlowId) == L7_SUCCESS)
      SET_VALID(I_diffServMultiFieldClfrFlowId, diffServMultiFieldClfrEntryData.valid);
    break;


  case I_diffServMultiFieldClfrProtocol :

    if (usmDbDiffServMFClfrProtocolGet(USMDB_UNIT_CURRENT, 
                                       diffServMultiFieldClfrEntryData.diffServMultiFieldClfrId,
                                       &diffServMultiFieldClfrEntryData.diffServMultiFieldClfrProtocol) == L7_SUCCESS)
      SET_VALID(I_diffServMultiFieldClfrProtocol, diffServMultiFieldClfrEntryData.valid);
    break;


  case I_diffServMultiFieldClfrDstL4PortMin :

    if (usmDbDiffServMFClfrDstL4PortMinGet(USMDB_UNIT_CURRENT, 
                                           diffServMultiFieldClfrEntryData.diffServMultiFieldClfrId,
                                           &diffServMultiFieldClfrEntryData.diffServMultiFieldClfrDstL4PortMin) == L7_SUCCESS)
      SET_VALID(I_diffServMultiFieldClfrDstL4PortMin, diffServMultiFieldClfrEntryData.valid);
    break;


  case I_diffServMultiFieldClfrDstL4PortMax :

    if (usmDbDiffServMFClfrDstL4PortMaxGet(USMDB_UNIT_CURRENT, 
                                           diffServMultiFieldClfrEntryData.diffServMultiFieldClfrId,
                                           &diffServMultiFieldClfrEntryData.diffServMultiFieldClfrDstL4PortMax) == L7_SUCCESS)
      SET_VALID(I_diffServMultiFieldClfrDstL4PortMax, diffServMultiFieldClfrEntryData.valid);
    break;


  case I_diffServMultiFieldClfrSrcL4PortMin :

    if (usmDbDiffServMFClfrSrcL4PortMinGet(USMDB_UNIT_CURRENT, 
                                           diffServMultiFieldClfrEntryData.diffServMultiFieldClfrId,
                                           &diffServMultiFieldClfrEntryData.diffServMultiFieldClfrSrcL4PortMin) == L7_SUCCESS)
      SET_VALID(I_diffServMultiFieldClfrSrcL4PortMin, diffServMultiFieldClfrEntryData.valid);
    break;


  case I_diffServMultiFieldClfrSrcL4PortMax :

    if (usmDbDiffServMFClfrSrcL4PortMaxGet(USMDB_UNIT_CURRENT, 
                                           diffServMultiFieldClfrEntryData.diffServMultiFieldClfrId,
                                           &diffServMultiFieldClfrEntryData.diffServMultiFieldClfrSrcL4PortMax) == L7_SUCCESS)
      SET_VALID(I_diffServMultiFieldClfrSrcL4PortMax, diffServMultiFieldClfrEntryData.valid);
    break;

  case I_diffServMultiFieldClfrStorage :

    if (snmpDiffServMFClfrStorageGet(USMDB_UNIT_CURRENT, 
                                     diffServMultiFieldClfrEntryData.diffServMultiFieldClfrId,
                                     &diffServMultiFieldClfrEntryData.diffServMultiFieldClfrStorage) == L7_SUCCESS)
      SET_VALID(I_diffServMultiFieldClfrStorage, diffServMultiFieldClfrEntryData.valid);
    break;


  case I_diffServMultiFieldClfrStatus :

    if (snmpDiffServMFClfrStatusGet(USMDB_UNIT_CURRENT, 
                                    diffServMultiFieldClfrEntryData.diffServMultiFieldClfrId,
                                    &diffServMultiFieldClfrEntryData.diffServMultiFieldClfrStorage) == L7_SUCCESS)
      SET_VALID(I_diffServMultiFieldClfrStatus, diffServMultiFieldClfrEntryData.valid);
    break;

  default:
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, diffServMultiFieldClfrEntryData.valid))
    return(NULL);

  return(&diffServMultiFieldClfrEntryData); 
}


#ifdef SETS
int
k_diffServMultiFieldClfrEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                   doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_diffServMultiFieldClfrEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                    doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_diffServMultiFieldClfrEntry_set_defaults(doList_t *dp)
{
  diffServMultiFieldClfrEntry_t *data = (diffServMultiFieldClfrEntry_t *) (dp->data);

  if ((data->diffServMultiFieldClfrDstAddr = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->diffServMultiFieldClfrSrcAddr = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->diffServMultiFieldClfrStorage = D_diffServMultiFieldClfrStorage_nonVolatile;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_diffServMultiFieldClfrEntry_set(diffServMultiFieldClfrEntry_t *data,
                                  ContextInfo *contextInfo, int function)
{

  return COMMIT_FAILED_ERROR;
}

  #ifdef SR_diffServMultiFieldClfrEntry_UNDO
/* add #define SR_diffServMultiFieldClfrEntry_UNDO in sitedefs.h to
 * include the undo routine for the diffServMultiFieldClfrEntry family.
 */
int
diffServMultiFieldClfrEntry_undo(doList_t *doHead, doList_t *doCur,
                                 ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_diffServMultiFieldClfrEntry_UNDO */

#endif /* SETS */

diffServMeter_t *
k_diffServMeter_get(int serialNum, ContextInfo *contextInfo,
                    int nominator)
{
  static diffServMeter_t diffServMeterData;

  ZERO_VALID(diffServMeterData.valid);

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  if (usmDbDiffServMeterIndexNext(USMDB_UNIT_CURRENT, 
		          &diffServMeterData.diffServMeterNextFree) == L7_SUCCESS)
    SET_VALID(I_diffServMeterNextFree, diffServMeterData.valid);
  
  if (nominator >= 0 && !VALID(nominator, diffServMeterData.valid))
    return(NULL);

  return(&diffServMeterData);
}

diffServMeterEntry_t *
k_diffServMeterEntry_get(int serialNum, ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         SR_UINT32 diffServMeterId)
{

  static diffServMeterEntry_t diffServMeterEntryData;

  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    diffServMeterEntryData.diffServMeterSucceedNext = MakeOID(NULL, 0);
    diffServMeterEntryData.diffServMeterFailNext = MakeOID(NULL, 0);
    diffServMeterEntryData.diffServMeterSpecific = MakeOID(NULL, 0);
  }

  ZERO_VALID(diffServMeterEntryData.valid);
  diffServMeterEntryData.diffServMeterId = diffServMeterId;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  SET_VALID(I_diffServMeterId, diffServMeterEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbDiffServMeterGet(USMDB_UNIT_CURRENT, 
                              diffServMeterEntryData.diffServMeterId) != L7_SUCCESS) :
       ( (usmDbDiffServMeterGet(USMDB_UNIT_CURRENT, 
                                diffServMeterEntryData.diffServMeterId) != L7_SUCCESS) &&
         (usmDbDiffServMeterGetNext(USMDB_UNIT_CURRENT,
                                    diffServMeterEntryData.diffServMeterId,
                                    &diffServMeterEntryData.diffServMeterId ) != L7_SUCCESS) ) )
  {
    ZERO_VALID(diffServMeterEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  
  case -1:
  case I_diffServMeterId:
    break;

  case I_diffServMeterSucceedNext :

    if (snmpDiffServMeterSucceedNextGet(USMDB_UNIT_CURRENT,
                                        diffServMeterEntryData.diffServMeterId,
                                        snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOIDFromDot(&diffServMeterEntryData.diffServMeterSucceedNext, snmp_buffer) == L7_TRUE))
      SET_VALID(I_diffServMeterSucceedNext, diffServMeterEntryData.valid);

    break;

  case I_diffServMeterFailNext :

    if (snmpDiffServMeterFailNextGet(USMDB_UNIT_CURRENT,
                                     diffServMeterEntryData.diffServMeterId,
                                     snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOIDFromDot(&diffServMeterEntryData.diffServMeterFailNext, snmp_buffer) == L7_TRUE))
      SET_VALID(I_diffServMeterFailNext, diffServMeterEntryData.valid);

    break;


  case I_diffServMeterSpecific :

    if (snmpDiffServMeterSpecificGet(USMDB_UNIT_CURRENT,
                                     diffServMeterEntryData.diffServMeterId,
                                     snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOIDFromDot(&diffServMeterEntryData.diffServMeterSpecific, snmp_buffer) == L7_TRUE))
      SET_VALID(I_diffServMeterSpecific, diffServMeterEntryData.valid);

    break;

  case I_diffServMeterStorage :

    if (snmpDiffServMeterStorageGet(USMDB_UNIT_CURRENT, 
                                    diffServMeterEntryData.diffServMeterId,
                                    &diffServMeterEntryData.diffServMeterStorage) == L7_SUCCESS)
      SET_VALID(I_diffServMeterStorage, diffServMeterEntryData.valid);
    break;


  case I_diffServMeterStatus :

    if (snmpDiffServMeterStatusGet(USMDB_UNIT_CURRENT, 
                                   diffServMeterEntryData.diffServMeterId,
                                   &diffServMeterEntryData.diffServMeterStatus) == L7_SUCCESS)
      SET_VALID(I_diffServMeterStatus, diffServMeterEntryData.valid);
    break;

#ifdef I_agentDiffServColorAwareLevel
  case I_agentDiffServColorAwareLevel :

    if (snmpAgentDiffServColorAwareLevelGet(USMDB_UNIT_CURRENT, 
                                            diffServMeterEntryData.diffServMeterId,
                                            &diffServMeterEntryData.agentDiffServColorAwareLevel) == L7_SUCCESS)
      SET_VALID(I_agentDiffServColorAwareLevel, diffServMeterEntryData.valid);
    break;
#endif /* I_agentDiffServColorAwareLevel*/
    
#ifdef I_agentDiffServColorAwareMode
  case I_agentDiffServColorAwareMode :

    if (snmpAgentDiffServColorAwareModeGet(USMDB_UNIT_CURRENT, 
                                           diffServMeterEntryData.diffServMeterId,
                                           &diffServMeterEntryData.agentDiffServColorAwareMode) == L7_SUCCESS)
      SET_VALID(I_agentDiffServColorAwareMode, diffServMeterEntryData.valid);
    break;
#endif /* I_agentDiffServColorAwareMode*/
    
#ifdef I_agentDiffServColorAwareValue
  case I_agentDiffServColorAwareValue :

    if (usmDbDiffServColorAwareValueGet(USMDB_UNIT_CURRENT, 
                                        diffServMeterEntryData.diffServMeterId,
                                        &diffServMeterEntryData.agentDiffServColorAwareValue) == L7_SUCCESS)
      SET_VALID(I_agentDiffServColorAwareValue, diffServMeterEntryData.valid);
    break;
#endif /* I_agentDiffServColorAwareValue*/
    
  default:
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, diffServMeterEntryData.valid))
    return(NULL);

  return(&diffServMeterEntryData);
}

#ifdef SETS
int
k_diffServMeterEntry_test(ObjectInfo *object, ObjectSyntax *value,
                          doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_diffServMeterEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                           doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_diffServMeterEntry_set_defaults(doList_t *dp)
{
  diffServMeterEntry_t *data = (diffServMeterEntry_t *) (dp->data);

  if ((data->diffServMeterSucceedNext = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->diffServMeterFailNext = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->diffServMeterSpecific = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->diffServMeterStorage = D_diffServMeterStorage_nonVolatile;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_diffServMeterEntry_set(diffServMeterEntry_t *data,
                         ContextInfo *contextInfo, int function)
{

  return COMMIT_FAILED_ERROR;
}

  #ifdef SR_diffServMeterEntry_UNDO
/* add #define SR_diffServMeterEntry_UNDO in sitedefs.h to
 * include the undo routine for the diffServMeterEntry family.
 */
int
diffServMeterEntry_undo(doList_t *doHead, doList_t *doCur,
                        ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_diffServMeterEntry_UNDO */

#endif /* SETS */

diffServTBParam_t *
k_diffServTBParam_get(int serialNum, ContextInfo *contextInfo,
                      int nominator)
{
  static diffServTBParam_t diffServTBParamData;

  ZERO_VALID(diffServTBParamData.valid);

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  if (usmDbDiffServTBParamIndexNext(USMDB_UNIT_CURRENT, 
		          &diffServTBParamData.diffServTBParamNextFree) == L7_SUCCESS)
    SET_VALID(I_diffServTBParamNextFree, diffServTBParamData.valid);
  
  if (nominator >= 0 && !VALID(nominator, diffServTBParamData.valid))
    return(NULL);

  return(&diffServTBParamData);
}

diffServTBParamEntry_t *
k_diffServTBParamEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_UINT32 diffServTBParamId)
{

  static diffServTBParamEntry_t diffServTBParamEntryData;

  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    diffServTBParamEntryData.diffServTBParamType = MakeOID(NULL, 0);
  }

  ZERO_VALID(diffServTBParamEntryData.valid);
  diffServTBParamEntryData.diffServTBParamId = diffServTBParamId;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  SET_VALID(I_diffServTBParamId, diffServTBParamEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbDiffServTBParamGet(USMDB_UNIT_CURRENT, 
                                diffServTBParamEntryData.diffServTBParamId) != L7_SUCCESS) :
       ( (usmDbDiffServTBParamGet(USMDB_UNIT_CURRENT, 
                                  diffServTBParamEntryData.diffServTBParamId) != L7_SUCCESS) &&
         (usmDbDiffServTBParamGetNext(USMDB_UNIT_CURRENT,
                                      diffServTBParamEntryData.diffServTBParamId,
                                      &diffServTBParamEntryData.diffServTBParamId) != L7_SUCCESS) ) )
  {
    ZERO_VALID(diffServTBParamEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  
  case -1:
  case I_diffServTBParamId:
    break;

  case I_diffServTBParamType :

    if (snmpDiffServTBParamTypeGet(USMDB_UNIT_CURRENT,
                                   diffServTBParamEntryData.diffServTBParamId,
                                   snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOIDFromDot(&diffServTBParamEntryData.diffServTBParamType, snmp_buffer) == L7_TRUE))
      SET_VALID(I_diffServTBParamType, diffServTBParamEntryData.valid);

    break;

  case I_diffServTBParamRate :

    if (usmDbDiffServTBParamRateGet(USMDB_UNIT_CURRENT, 
                                    diffServTBParamEntryData.diffServTBParamId,
                                    &diffServTBParamEntryData.diffServTBParamRate) == L7_SUCCESS)
      SET_VALID(I_diffServTBParamRate, diffServTBParamEntryData.valid);
    break;

  case I_diffServTBParamBurstSize :

    if (usmDbDiffServTBParamBurstSizeGet(USMDB_UNIT_CURRENT, 
                                         diffServTBParamEntryData.diffServTBParamId,
                                         &diffServTBParamEntryData.diffServTBParamBurstSize) == L7_SUCCESS)
      SET_VALID(I_diffServTBParamBurstSize, diffServTBParamEntryData.valid);
    break;

  case I_diffServTBParamInterval :

    if (usmDbDiffServTBParamIntervalGet(USMDB_UNIT_CURRENT, 
                                        diffServTBParamEntryData.diffServTBParamId,
                                        &diffServTBParamEntryData.diffServTBParamInterval) == L7_SUCCESS)
      SET_VALID(I_diffServTBParamInterval, diffServTBParamEntryData.valid);
    break;


  case I_diffServTBParamStorage :

    if (snmpDiffServTBParamStorageGet(USMDB_UNIT_CURRENT, 
                                      diffServTBParamEntryData.diffServTBParamId,
                                      &diffServTBParamEntryData.diffServTBParamStorage) == L7_SUCCESS)
      SET_VALID(I_diffServTBParamStorage, diffServTBParamEntryData.valid);
    break;


  case I_diffServTBParamStatus :

    if (snmpDiffServTBParamStatusGet(USMDB_UNIT_CURRENT, 
                                     diffServTBParamEntryData.diffServTBParamId,
                                     &diffServTBParamEntryData.diffServTBParamStatus) == L7_SUCCESS)
      SET_VALID(I_diffServTBParamStatus, diffServTBParamEntryData.valid);
    break;

  default:
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, diffServTBParamEntryData.valid))
    return(NULL);

  return(&diffServTBParamEntryData);

}

#ifdef SETS
int
k_diffServTBParamEntry_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_diffServTBParamEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                             doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_diffServTBParamEntry_set_defaults(doList_t *dp)
{
  diffServTBParamEntry_t *data = (diffServTBParamEntry_t *) (dp->data);

  if ((data->diffServTBParamType = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->diffServTBParamStorage = D_diffServTBParamStorage_nonVolatile;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_diffServTBParamEntry_set(diffServTBParamEntry_t *data,
                           ContextInfo *contextInfo, int function)
{

  return COMMIT_FAILED_ERROR;
}

  #ifdef SR_diffServTBParamEntry_UNDO
/* add #define SR_diffServTBParamEntry_UNDO in sitedefs.h to
 * include the undo routine for the diffServTBParamEntry family.
 */
int
diffServTBParamEntry_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_diffServTBParamEntry_UNDO */

#endif /* SETS */

diffServAction_t *
k_diffServAction_get(int serialNum, ContextInfo *contextInfo,
                     int nominator)
{
  static diffServAction_t diffServActionData;

  ZERO_VALID(diffServActionData.valid);

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  if (usmDbDiffServActionIndexNext(USMDB_UNIT_CURRENT, 
		          &diffServActionData.diffServActionNextFree) == L7_SUCCESS)
    SET_VALID(I_diffServActionNextFree, diffServActionData.valid);
  
  if (usmDbDiffServCountActIndexNext(USMDB_UNIT_CURRENT, 
		          &diffServActionData.diffServCountActNextFree) == L7_SUCCESS)
    SET_VALID(I_diffServCountActNextFree, diffServActionData.valid);
  
  if (nominator >= 0 && !VALID(nominator, diffServActionData.valid))
    return(NULL);

  return(&diffServActionData);
}

diffServActionEntry_t *
k_diffServActionEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator,
                          int searchType,
                          SR_UINT32 diffServActionId)
{

  static diffServActionEntry_t diffServActionEntryData;

  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    diffServActionEntryData.diffServActionNext = MakeOID(NULL, 0);
    diffServActionEntryData.diffServActionSpecific = MakeOID(NULL, 0);
  }

  ZERO_VALID(diffServActionEntryData.valid);
  diffServActionEntryData.diffServActionId = diffServActionId;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }
  
  SET_VALID(I_diffServActionId, diffServActionEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbDiffServActionGet(USMDB_UNIT_CURRENT, 
                               diffServActionEntryData.diffServActionId) != L7_SUCCESS) :
       ( (usmDbDiffServActionGet(USMDB_UNIT_CURRENT, 
                                 diffServActionEntryData.diffServActionId) != L7_SUCCESS) &&
         (usmDbDiffServActionGetNext(USMDB_UNIT_CURRENT,
                                     diffServActionEntryData.diffServActionId,
                                     &diffServActionEntryData.diffServActionId) != L7_SUCCESS) ) )
  {
    ZERO_VALID(diffServActionEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  
  case -1:
  case I_diffServActionId:
    break;


  case I_diffServActionInterface :

    if (usmDbDiffServActionInterfaceGet(USMDB_UNIT_CURRENT, 
                                        diffServActionEntryData.diffServActionId,
                                        &diffServActionEntryData.diffServActionInterface) == L7_SUCCESS)
      SET_VALID(I_diffServActionInterface, diffServActionEntryData.valid);
    break;

  case I_diffServActionNext :

    if (snmpDiffServActionNextGet(USMDB_UNIT_CURRENT,
                                  diffServActionEntryData.diffServActionId,
                                  snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOIDFromDot(&diffServActionEntryData.diffServActionNext, snmp_buffer) == L7_TRUE))
      SET_VALID(I_diffServActionNext, diffServActionEntryData.valid);

    break;

  case I_diffServActionSpecific :

    if (snmpDiffServActionSpecificGet(USMDB_UNIT_CURRENT,
                                      diffServActionEntryData.diffServActionId,
                                      snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOIDFromDot(&diffServActionEntryData.diffServActionSpecific, snmp_buffer) == L7_TRUE))
      SET_VALID(I_diffServActionSpecific, diffServActionEntryData.valid);

    break;


  case I_diffServActionStorage :

    if (snmpDiffServActionStorageGet(USMDB_UNIT_CURRENT, 
                                     diffServActionEntryData.diffServActionId,
                                     &diffServActionEntryData.diffServActionStorage) == L7_SUCCESS)
      SET_VALID(I_diffServActionStorage, diffServActionEntryData.valid);
    break;


  case I_diffServActionStatus :

    if (snmpDiffServActionStatusGet(USMDB_UNIT_CURRENT, 
                                    diffServActionEntryData.diffServActionId,
                                    &diffServActionEntryData.diffServActionStatus) == L7_SUCCESS)
      SET_VALID(I_diffServActionStatus, diffServActionEntryData.valid);
    break;

  default:
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, diffServActionEntryData.valid))
    return(NULL);

  return(&diffServActionEntryData);
}

#ifdef SETS
int
k_diffServActionEntry_test(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_diffServActionEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                            doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_diffServActionEntry_set_defaults(doList_t *dp)
{
  diffServActionEntry_t *data = (diffServActionEntry_t *) (dp->data);

  if ((data->diffServActionNext = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->diffServActionSpecific = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->diffServActionStorage = D_diffServActionStorage_nonVolatile;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_diffServActionEntry_set(diffServActionEntry_t *data,
                          ContextInfo *contextInfo, int function)
{

  return COMMIT_FAILED_ERROR;
}

  #ifdef SR_diffServActionEntry_UNDO
/* add #define SR_diffServActionEntry_UNDO in sitedefs.h to
 * include the undo routine for the diffServActionEntry family.
 */
int
diffServActionEntry_undo(doList_t *doHead, doList_t *doCur,
                         ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_diffServActionEntry_UNDO */

#endif /* SETS */

diffServDscpMarkActEntry_t *
k_diffServDscpMarkActEntry_get(int serialNum, ContextInfo *contextInfo,
                               int nominator,
                               int searchType,
                               SR_INT32 diffServDscpMarkActDscp)
{
  static diffServDscpMarkActEntry_t diffServDscpMarkActEntryData;

  ZERO_VALID(diffServDscpMarkActEntryData.valid);
  diffServDscpMarkActEntryData.diffServDscpMarkActDscp = diffServDscpMarkActDscp;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  SET_VALID(I_diffServDscpMarkActDscp, diffServDscpMarkActEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbDiffServDscpMarkActGet(USMDB_UNIT_CURRENT, 
                                    diffServDscpMarkActEntryData.diffServDscpMarkActDscp) != L7_SUCCESS) :
       ( (usmDbDiffServDscpMarkActGet(USMDB_UNIT_CURRENT, 
                                      diffServDscpMarkActEntryData.diffServDscpMarkActDscp) != L7_SUCCESS) &&
         (usmDbDiffServDscpMarkActGetNext(USMDB_UNIT_CURRENT,
                                          diffServDscpMarkActEntryData.diffServDscpMarkActDscp,
                                          &diffServDscpMarkActEntryData.diffServDscpMarkActDscp) != L7_SUCCESS) ) )
  {
    ZERO_VALID(diffServDscpMarkActEntryData.valid);
    return(NULL);
  }


  if (nominator >= 0 && !VALID(nominator, diffServDscpMarkActEntryData.valid))
    return(NULL);

  return(&diffServDscpMarkActEntryData);
}

diffServCountActEntry_t *
k_diffServCountActEntry_get(int serialNum, ContextInfo *contextInfo,
                            int nominator,
                            int searchType,
                            SR_UINT32 diffServCountActId)
{

  static diffServCountActEntry_t diffServCountActEntryData;
  static L7_BOOL firstTime = L7_TRUE;

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    diffServCountActEntryData.diffServCountActOctets = MakeCounter64(0);
    diffServCountActEntryData.diffServCountActPkts = MakeCounter64(0);
  }

  ZERO_VALID(diffServCountActEntryData.valid);
  diffServCountActEntryData.diffServCountActId = diffServCountActId;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  SET_VALID(I_diffServCountActId, diffServCountActEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbDiffServCountActGet(USMDB_UNIT_CURRENT, 
                                 diffServCountActEntryData.diffServCountActId) != L7_SUCCESS) :
       ( (usmDbDiffServCountActGet(USMDB_UNIT_CURRENT, 
                                   diffServCountActEntryData.diffServCountActId) != L7_SUCCESS) &&
         (usmDbDiffServCountActGetNext(USMDB_UNIT_CURRENT,
                                       diffServCountActEntryData.diffServCountActId,
                                       &diffServCountActEntryData.diffServCountActId) != L7_SUCCESS) ) )
  {
    ZERO_VALID(diffServCountActEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  
  case -1:
  case I_diffServCountActId:
    break;


  case I_diffServCountActOctets :

    if (usmDbDiffServCountActOctetsGet(USMDB_UNIT_CURRENT, 
                                       diffServCountActEntryData.diffServCountActId,
                                       &diffServCountActEntryData.diffServCountActOctets->big_end,
                                       &diffServCountActEntryData.diffServCountActOctets->little_end) == L7_SUCCESS)
      SET_VALID(I_diffServCountActOctets, diffServCountActEntryData.valid);
    break;


  case I_diffServCountActPkts :

    if (usmDbDiffServCountActPacketsGet(USMDB_UNIT_CURRENT, 
                                        diffServCountActEntryData.diffServCountActId,
                                        &diffServCountActEntryData.diffServCountActPkts->big_end,
                                        &diffServCountActEntryData.diffServCountActPkts->little_end) == L7_SUCCESS)
      SET_VALID(I_diffServCountActPkts, diffServCountActEntryData.valid);
    break;


  case I_diffServCountActStorage :

    if (snmpDiffServCountActStorageGet(USMDB_UNIT_CURRENT, 
                                       diffServCountActEntryData.diffServCountActId,
                                       &diffServCountActEntryData.diffServCountActStorage) == L7_SUCCESS)
      SET_VALID(I_diffServCountActStorage, diffServCountActEntryData.valid);
    break;


  case I_diffServCountActStatus :

    if (snmpDiffServCountActStatusGet(USMDB_UNIT_CURRENT, 
                                      diffServCountActEntryData.diffServCountActId,
                                      &diffServCountActEntryData.diffServCountActStatus) == L7_SUCCESS)
      SET_VALID(I_diffServCountActStatus, diffServCountActEntryData.valid);
    break;


  default:
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, diffServCountActEntryData.valid))
    return(NULL);

  return(&diffServCountActEntryData);
}

#ifdef SETS
int
k_diffServCountActEntry_test(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_diffServCountActEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                              doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_diffServCountActEntry_set_defaults(doList_t *dp)
{
  diffServCountActEntry_t *data = (diffServCountActEntry_t *) (dp->data);

  data->diffServCountActOctets = (SR_UINT32) 0;
  data->diffServCountActPkts = (SR_UINT32) 0;
  data->diffServCountActStorage = D_diffServCountActStorage_nonVolatile;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_diffServCountActEntry_set(diffServCountActEntry_t *data,
                            ContextInfo *contextInfo, int function)
{

  return COMMIT_FAILED_ERROR;
}

  #ifdef SR_diffServCountActEntry_UNDO
/* add #define SR_diffServCountActEntry_UNDO in sitedefs.h to
 * include the undo routine for the diffServCountActEntry family.
 */
int
diffServCountActEntry_undo(doList_t *doHead, doList_t *doCur,
                           ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_diffServCountActEntry_UNDO */

#endif /* SETS */

diffServAlgDrop_t *
k_diffServAlgDrop_get(int serialNum, ContextInfo *contextInfo,
                      int nominator)
{
  static diffServAlgDrop_t diffServAlgDropData;

  ZERO_VALID(diffServAlgDropData.valid);

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  if (usmDbDiffServAlgDropIndexNext(USMDB_UNIT_CURRENT, 
		          &diffServAlgDropData.diffServAlgDropNextFree) == L7_SUCCESS)
    SET_VALID(I_diffServAlgDropNextFree, diffServAlgDropData.valid);
  
  if (nominator >= 0 && !VALID(nominator, diffServAlgDropData.valid))
    return(NULL);

  return(&diffServAlgDropData);
}

diffServAlgDropEntry_t *
k_diffServAlgDropEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_UINT32 diffServAlgDropId)
{
  static diffServAlgDropEntry_t diffServAlgDropEntryData;

  static L7_BOOL firstTime = L7_TRUE;
  L7_char8 snmp_buffer[SNMP_BUFFER_LEN];

  if (firstTime == L7_TRUE)
  {
    firstTime = L7_FALSE;
    diffServAlgDropEntryData.diffServAlgDropNext = MakeOID(NULL, 0);
    diffServAlgDropEntryData.diffServAlgDropQMeasure = MakeOID(NULL, 0);
    diffServAlgDropEntryData.diffServAlgDropSpecific = MakeOID(NULL, 0);
    diffServAlgDropEntryData.diffServAlgDropOctets = MakeCounter64(0);
    diffServAlgDropEntryData.diffServAlgDropPkts = MakeCounter64(0);
    diffServAlgDropEntryData.diffServAlgRandomDropOctets = MakeCounter64(0);
    diffServAlgDropEntryData.diffServAlgRandomDropPkts = MakeCounter64(0);
  }

  ZERO_VALID(diffServAlgDropEntryData.valid);
  diffServAlgDropEntryData.diffServAlgDropId = diffServAlgDropId;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_FEATURE_SUPPORTED) != L7_TRUE)
  {
    return(NULL);
  }

  SET_VALID(I_diffServAlgDropId, diffServAlgDropEntryData.valid);

  if ( (searchType == EXACT) ?
       (usmDbDiffServAlgDropGet(USMDB_UNIT_CURRENT, 
                                diffServAlgDropEntryData.diffServAlgDropId) != L7_SUCCESS) :
       ( (usmDbDiffServAlgDropGet(USMDB_UNIT_CURRENT, 
                                  diffServAlgDropEntryData.diffServAlgDropId) != L7_SUCCESS) &&
         (usmDbDiffServAlgDropGetNext(USMDB_UNIT_CURRENT,
                                      diffServAlgDropEntryData.diffServAlgDropId,
                                      &diffServAlgDropEntryData.diffServAlgDropId) != L7_SUCCESS) ) )
  {
    ZERO_VALID(diffServAlgDropEntryData.valid);
    return(NULL);
  }

  switch (nominator)
  {
  
  case -1:
  case I_diffServAlgDropId:
    break;


  case I_diffServAlgDropType :

    if (snmpDiffServAlgDropTypeGet(USMDB_UNIT_CURRENT, 
                                   diffServAlgDropEntryData.diffServAlgDropId,
                                   &diffServAlgDropEntryData.diffServAlgDropType) == L7_SUCCESS)
      SET_VALID(I_diffServAlgDropType, diffServAlgDropEntryData.valid);
    break;

  case I_diffServAlgDropNext :

    if (snmpDiffServAlgDropNextGet(USMDB_UNIT_CURRENT,
                                   diffServAlgDropEntryData.diffServAlgDropId,
                                   snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOIDFromDot(&diffServAlgDropEntryData.diffServAlgDropNext, snmp_buffer) == L7_TRUE))
      SET_VALID(I_diffServAlgDropNext, diffServAlgDropEntryData.valid);

    break;


  case I_diffServAlgDropQMeasure :

    if (snmpDiffServAlgDropQMeasureGet(USMDB_UNIT_CURRENT,
                                       diffServAlgDropEntryData.diffServAlgDropId,
                                       snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOIDFromDot(&diffServAlgDropEntryData.diffServAlgDropQMeasure, snmp_buffer) == L7_TRUE))
      SET_VALID(I_diffServAlgDropQMeasure, diffServAlgDropEntryData.valid);

    break;

  case I_diffServAlgDropQThreshold :

    if (usmDbDiffServAlgDropQThresholdGet(USMDB_UNIT_CURRENT, 
                                          diffServAlgDropEntryData.diffServAlgDropId,
                                          &diffServAlgDropEntryData.diffServAlgDropQThreshold) == L7_SUCCESS)
      SET_VALID(I_diffServAlgDropQThreshold, diffServAlgDropEntryData.valid);
    break;

  case I_diffServAlgDropSpecific :

    if (snmpDiffServAlgDropSpecificGet(USMDB_UNIT_CURRENT,
                                       diffServAlgDropEntryData.diffServAlgDropId,
                                       snmp_buffer) == L7_SUCCESS &&
        (SafeMakeOIDFromDot(&diffServAlgDropEntryData.diffServAlgDropSpecific, snmp_buffer) == L7_TRUE))
      SET_VALID(I_diffServAlgDropSpecific, diffServAlgDropEntryData.valid);

    break;


  case I_diffServAlgDropOctets :

    if (usmDbDiffServAlgDropOctetsGet(USMDB_UNIT_CURRENT, 
                                      diffServAlgDropEntryData.diffServAlgDropId,
                                      &diffServAlgDropEntryData.diffServAlgDropOctets->big_end,
                                      &diffServAlgDropEntryData.diffServAlgDropOctets->little_end) == L7_SUCCESS)
      SET_VALID(I_diffServAlgDropOctets, diffServAlgDropEntryData.valid);
    break;


  case I_diffServAlgDropPkts :

    if (usmDbDiffServAlgDropPacketsGet(USMDB_UNIT_CURRENT, 
                                       diffServAlgDropEntryData.diffServAlgDropId,
                                       &diffServAlgDropEntryData.diffServAlgDropPkts->big_end,
                                       &diffServAlgDropEntryData.diffServAlgDropPkts->little_end) == L7_SUCCESS)
      SET_VALID(I_diffServAlgDropPkts, diffServAlgDropEntryData.valid);
    break;

  case I_diffServAlgRandomDropOctets :

    if (usmDbDiffServAlgRandomDropOctetsGet(USMDB_UNIT_CURRENT, 
                                            diffServAlgDropEntryData.diffServAlgDropId,
                                            &diffServAlgDropEntryData.diffServAlgRandomDropOctets->big_end,
                                            &diffServAlgDropEntryData.diffServAlgRandomDropOctets->little_end) == L7_SUCCESS)
      SET_VALID(I_diffServAlgRandomDropOctets, diffServAlgDropEntryData.valid);
    break;


  case I_diffServAlgRandomDropPkts :

    if (usmDbDiffServAlgRandomDropPacketsGet(USMDB_UNIT_CURRENT, 
                                             diffServAlgDropEntryData.diffServAlgDropId,
                                             &diffServAlgDropEntryData.diffServAlgRandomDropPkts->big_end,
                                             &diffServAlgDropEntryData.diffServAlgRandomDropPkts->little_end) == L7_SUCCESS)
      SET_VALID(I_diffServAlgRandomDropPkts, diffServAlgDropEntryData.valid);
    break;


  case I_diffServAlgDropStorage :

    if (snmpDiffServAlgDropStorageGet(USMDB_UNIT_CURRENT, 
                                      diffServAlgDropEntryData.diffServAlgDropId,
                                      &diffServAlgDropEntryData.diffServAlgDropStorage) == L7_SUCCESS)
      SET_VALID(I_diffServAlgDropStorage, diffServAlgDropEntryData.valid);
    break;


  case I_diffServAlgDropStatus :

    if (snmpDiffServAlgDropStatusGet(USMDB_UNIT_CURRENT, 
                                     diffServAlgDropEntryData.diffServAlgDropId,
                                     &diffServAlgDropEntryData.diffServAlgDropStatus) == L7_SUCCESS)
      SET_VALID(I_diffServAlgDropStatus, diffServAlgDropEntryData.valid);
    break;

  default:
    return(NULL);
    break;
  }

  if (nominator >= 0 && !VALID(nominator, diffServAlgDropEntryData.valid))
    return(NULL);

  return(&diffServAlgDropEntryData);

}

#ifdef SETS
int
k_diffServAlgDropEntry_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_diffServAlgDropEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                             doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_diffServAlgDropEntry_set_defaults(doList_t *dp)
{
  diffServAlgDropEntry_t *data = (diffServAlgDropEntry_t *) (dp->data);

  if ((data->diffServAlgDropNext = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->diffServAlgDropQMeasure = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->diffServAlgDropSpecific = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->diffServAlgDropOctets = (SR_UINT32) 0;
  data->diffServAlgDropPkts = (SR_UINT32) 0;
  data->diffServAlgRandomDropOctets = (SR_UINT32) 0;
  data->diffServAlgRandomDropPkts = (SR_UINT32) 0;
  data->diffServAlgDropStorage = D_diffServAlgDropStorage_nonVolatile;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_diffServAlgDropEntry_set(diffServAlgDropEntry_t *data,
                           ContextInfo *contextInfo, int function)
{

  return COMMIT_FAILED_ERROR;
}

  #ifdef SR_diffServAlgDropEntry_UNDO
/* add #define SR_diffServAlgDropEntry_UNDO in sitedefs.h to
 * include the undo routine for the diffServAlgDropEntry family.
 */
int
diffServAlgDropEntry_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_diffServAlgDropEntry_UNDO */

#endif /* SETS */

diffServRandomDropEntry_t *
k_diffServRandomDropEntry_get(int serialNum, ContextInfo *contextInfo,
                              int nominator,
                              int searchType,
                              SR_UINT32 diffServRandomDropId)
{
  return(NULL);
}

#ifdef SETS
int
k_diffServRandomDropEntry_test(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_diffServRandomDropEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                                doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_diffServRandomDropEntry_set_defaults(doList_t *dp)
{
  diffServRandomDropEntry_t *data = (diffServRandomDropEntry_t *) (dp->data);

  data->diffServRandomDropStorage = D_diffServRandomDropStorage_nonVolatile;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_diffServRandomDropEntry_set(diffServRandomDropEntry_t *data,
                              ContextInfo *contextInfo, int function)
{

  return COMMIT_FAILED_ERROR;
}

  #ifdef SR_diffServRandomDropEntry_UNDO
/* add #define SR_diffServRandomDropEntry_UNDO in sitedefs.h to
 * include the undo routine for the diffServRandomDropEntry family.
 */
int
diffServRandomDropEntry_undo(doList_t *doHead, doList_t *doCur,
                             ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_diffServRandomDropEntry_UNDO */

#endif /* SETS */

diffServQueue_t *
k_diffServQueue_get(int serialNum, ContextInfo *contextInfo,
                    int nominator)
{
  return(NULL);
}

diffServQEntry_t *
k_diffServQEntry_get(int serialNum, ContextInfo *contextInfo,
                     int nominator,
                     int searchType,
                     SR_UINT32 diffServQId)
{
  return(NULL);
}

#ifdef SETS
int
k_diffServQEntry_test(ObjectInfo *object, ObjectSyntax *value,
                      doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_diffServQEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                       doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_diffServQEntry_set_defaults(doList_t *dp)
{
  diffServQEntry_t *data = (diffServQEntry_t *) (dp->data);

  if ((data->diffServQNext = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->diffServQMinRate = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->diffServQMaxRate = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->diffServQStorage = D_diffServQStorage_nonVolatile;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_diffServQEntry_set(diffServQEntry_t *data,
                     ContextInfo *contextInfo, int function)
{

  return COMMIT_FAILED_ERROR;
}

  #ifdef SR_diffServQEntry_UNDO
/* add #define SR_diffServQEntry_UNDO in sitedefs.h to
 * include the undo routine for the diffServQEntry family.
 */
int
diffServQEntry_undo(doList_t *doHead, doList_t *doCur,
                    ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_diffServQEntry_UNDO */

#endif /* SETS */

diffServScheduler_t *
k_diffServScheduler_get(int serialNum, ContextInfo *contextInfo,
                        int nominator)
{
  return(NULL);
}

diffServSchedulerEntry_t *
k_diffServSchedulerEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_UINT32 diffServSchedulerId)
{
  return(NULL);
}

#ifdef SETS
int
k_diffServSchedulerEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_diffServSchedulerEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                               doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_diffServSchedulerEntry_set_defaults(doList_t *dp)
{
  diffServSchedulerEntry_t *data = (diffServSchedulerEntry_t *) (dp->data);

  if ((data->diffServSchedulerNext = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->diffServSchedulerMethod = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->diffServSchedulerMinRate = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->diffServSchedulerMaxRate = MakeOIDFromDot("0.0")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  data->diffServSchedulerStorage = D_diffServSchedulerStorage_nonVolatile;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_diffServSchedulerEntry_set(diffServSchedulerEntry_t *data,
                             ContextInfo *contextInfo, int function)
{

  return COMMIT_FAILED_ERROR;
}

  #ifdef SR_diffServSchedulerEntry_UNDO
/* add #define SR_diffServSchedulerEntry_UNDO in sitedefs.h to
 * include the undo routine for the diffServSchedulerEntry family.
 */
int
diffServSchedulerEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_diffServSchedulerEntry_UNDO */

#endif /* SETS */

diffServMinRateEntry_t *
k_diffServMinRateEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_UINT32 diffServMinRateId)
{
  return(NULL);
}


#ifdef SETS
int
k_diffServMinRateEntry_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_diffServMinRateEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                             doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_diffServMinRateEntry_set_defaults(doList_t *dp)
{
  diffServMinRateEntry_t *data = (diffServMinRateEntry_t *) (dp->data);

  data->diffServMinRateStorage = D_diffServMinRateStorage_nonVolatile;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_diffServMinRateEntry_set(diffServMinRateEntry_t *data,
                           ContextInfo *contextInfo, int function)
{

  return COMMIT_FAILED_ERROR;
}

  #ifdef SR_diffServMinRateEntry_UNDO
/* add #define SR_diffServMinRateEntry_UNDO in sitedefs.h to
 * include the undo routine for the diffServMinRateEntry family.
 */
int
diffServMinRateEntry_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_diffServMinRateEntry_UNDO */

#endif /* SETS */

diffServMaxRateEntry_t *
k_diffServMaxRateEntry_get(int serialNum, ContextInfo *contextInfo,
                           int nominator,
                           int searchType,
                           SR_UINT32 diffServMaxRateId,
                           SR_UINT32 diffServMaxRateLevel)
{
  return(NULL);
}

#ifdef SETS
int
k_diffServMaxRateEntry_test(ObjectInfo *object, ObjectSyntax *value,
                            doList_t *dp, ContextInfo *contextInfo)
{

  return NO_ERROR;
}

int
k_diffServMaxRateEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                             doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_diffServMaxRateEntry_set_defaults(doList_t *dp)
{
  diffServMaxRateEntry_t *data = (diffServMaxRateEntry_t *) (dp->data);

  data->diffServMaxRateStorage = D_diffServMaxRateStorage_nonVolatile;

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_diffServMaxRateEntry_set(diffServMaxRateEntry_t *data,
                           ContextInfo *contextInfo, int function)
{

  return COMMIT_FAILED_ERROR;
}

  #ifdef SR_diffServMaxRateEntry_UNDO
/* add #define SR_diffServMaxRateEntry_UNDO in sitedefs.h to
 * include the undo routine for the diffServMaxRateEntry family.
 */
int
diffServMaxRateEntry_undo(doList_t *doHead, doList_t *doCur,
                          ContextInfo *contextInfo)
{
  return UNDO_FAILED_ERROR;
}
  #endif /* SR_diffServMaxRateEntry_UNDO */

#endif /* SETS */

